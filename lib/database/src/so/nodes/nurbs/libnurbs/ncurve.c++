/*
 *
 *  Copyright (C) 2000 Silicon Graphics, Inc.  All Rights Reserved. 
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  Further, this software is distributed without any warranty that it is
 *  free of the rightful claim of any third person regarding infringement
 *  or the like.  Any license provided herein, whether implied or
 *  otherwise, applies only to this software file.  Patent licenses, if
 *  any, provided herein do not apply to combinations of this program with
 *  other software, or any other product whatsoever.
 * 
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Contact information: Silicon Graphics, Inc., 1600 Amphitheatre Pkwy,
 *  Mountain View, CA  94043, or:
 * 
 *  http://www.sgi.com 
 * 
 *  For further information regarding this notice, see: 
 * 
 *  http://oss.sgi.com/projects/GenInfo/NoticeExplan/
 *
 */

/*
 * ncurve.c++ - $Revision: 1.1.1.1 $
 * 	Derrick Burns - 1991
 */

#include "myglimports.h"
#include "myassert.h"
#include "mystdio.h"
#include "mymath.h"
#include "curve.h"
#include "mapdesc.h"
#include "types.h"
#include "quilt.h"
#include "nurbsconsts.h"

/*--------------------------------------------------------------------------
 * Curve::Curve - copy curve from quilt and transform control points
 *--------------------------------------------------------------------------
 */

Curve::Curve( Quilt_ptr geo, REAL pta, REAL ptb, Curve *c )
{
    mapdesc = geo->mapdesc;
    next = c;
    needsSampling = mapdesc->isRangeSampling() ? 1 : 0;
    cullval = mapdesc->isCulling() ? CULL_ACCEPT : CULL_TRIVIAL_ACCEPT;
    order = geo->qspec[0].order;
    stride = MAXCOORDS;

    REAL *ps  = geo->cpts; 
    Quiltspec_ptr qs = geo->qspec;
    ps += qs->offset;
    ps += qs->index * qs->order * qs->stride;
    REAL *pend = ps + qs->order * qs->stride;

    if( needsSampling )
	mapdesc->xformSampling( ps, qs->order, qs->stride, spts, stride );
	
    if( cullval == CULL_ACCEPT )
	mapdesc->xformCulling(  ps, qs->order, qs->stride, cpts, stride );

    /* set untrimmed curve range */
    range[0] = qs->breakpoints[qs->index];
    range[1] = qs->breakpoints[qs->index+1];
    range[2] = range[1] - range[0];

    if( range[0] != pta ) {
	Curve lower( *this, pta, 0 );
	lower.next = next;
	*this = lower;
    }
    if( range[1] != ptb ) {
	Curve lower( *this, ptb, 0 );
    }
}

/*--------------------------------------------------------------------------
 * Curve::Curve - subdivide a curve along an isoparametric line
 *--------------------------------------------------------------------------
 */

Curve::Curve( Curve& upper, REAL value, Curve *c )
{
    Curve &lower = *this;

    lower.next = c;
    lower.mapdesc = upper.mapdesc;
    lower.needsSampling = upper.needsSampling;
    lower.order = upper.order;
    lower.stride = upper.stride;
    lower.cullval = upper.cullval;

    REAL d = (value - upper.range[0]) / upper.range[2];

    if( needsSampling )
        mapdesc->subdivide( upper.spts, lower.spts, d, upper.stride, upper.order );

    if( cullval == CULL_ACCEPT ) 
        mapdesc->subdivide( upper.cpts, lower.cpts, d, upper.stride, upper.order );

    lower.range[0] = upper.range[0];
    lower.range[1] = value;
    lower.range[2] = value - upper.range[0];
    upper.range[0] = value;
    upper.range[2] = upper.range[1] - value;
}


/*--------------------------------------------------------------------------
 * Curve::clamp - clamp the sampling rate to a given maximum
 *--------------------------------------------------------------------------
 */

void
Curve::clamp( void )
{
    if( stepsize < minstepsize )
        stepsize = mapdesc->clampfactor * minstepsize;
}

void
Curve::setstepsize( REAL max )
{
    stepsize = ( max >= 1.0 ) ? (range[2] / max) : range[2];
    minstepsize = stepsize;
}

void
Curve::getstepsize( void )
{
    if( mapdesc->isConstantSampling() ) {
	// fixed number of samples per patch in each direction
	// maxrate is number of s samples per patch
        setstepsize( mapdesc->maxrate );
    } else if( mapdesc->isDomainSampling() ) {
	// maxrate is number of s samples per unit s length of domain
        setstepsize( mapdesc->maxrate * range[2] );
    } else {
	// upper bound on path length between sample points

	assert( order <= MAXORDER );
    
	/* points have been transformed, therefore they are homogeneous */
        REAL tmp[MAXORDER][MAXCOORDS];
	const int tstride = sizeof(tmp[0]) / sizeof(REAL);
	int val = mapdesc->project( spts, stride, &tmp[0][0], tstride,  order ); 

        if( val == 0 ) {
	    // control points cross infinity, therefore derivatives are undefined
            setstepsize( mapdesc->maxrate );
        } else {
            REAL t = mapdesc->getProperty( N_PIXEL_TOLERANCE );
	    if( mapdesc->isParametricDistanceSampling() ) {
		REAL d = mapdesc->calcPartialVelocity( &tmp[0][0], tstride, order, 2, range[2] );
		stepsize = (d > 0.0) ? ::sqrtf( 8.0 * t / d ) : range[2];
		minstepsize = ( mapdesc->maxrate > 0.0 ) ? (range[2] / mapdesc->maxrate) : 0.0;
	    } else if( mapdesc->isPathLengthSampling() ) {
		// t is upper bound on path (arc) length
		REAL d = mapdesc->calcPartialVelocity( &tmp[0][0], tstride, order, 1, range[2] );
		stepsize = ( d > 0.0 ) ? (t / d) : range[2];
		minstepsize = ( mapdesc->maxrate > 0.0 ) ? (range[2] / mapdesc->maxrate) : 0.0;
	    } else {
		// control points cross infinity, therefore partials are undefined
		setstepsize( mapdesc->maxrate );
	    }
	}
    }
}

int
Curve::needsSamplingSubdivision( void )
{
    return ( stepsize < minstepsize )  ? 1 : 0;
}

int
Curve::cullCheck( void )
{
    if( cullval == CULL_ACCEPT ) 
	cullval = mapdesc->cullCheck( cpts, order, stride );
    return cullval;
}

