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
 * softcurvmap.c++ - curve evaluator
 *
 * $Revision: 1.2 $
 */

#include "mystdio.h"
#include "myassert.h"
#include "mystring.h"
#include "defines.h"
#include "softcurvmap.h"

#define SHALLOWCOPY


_SoNurbsCurveMap::_SoNurbsCurveMap(long _type, int _rational, int _ncoords )
{
    type = _type;
    rational = _rational;
    ncoords = _ncoords;
    next = 0; 
    pts = 0; 
    isdefined = 0; 
    isenabled = 0;
}


void	_SoNurbsCurveMap::domain( float * ) {}
void	_SoNurbsCurveMap::point( float * ) {}
void	_SoNurbsCurveMap::uderiv( float * ) {}
void	_SoNurbsCurveMap::uuderiv( float * ) {}

int
_SoNurbsCurveMap::isType( long _type )
{
    return (_type == type) ? 1 : 0;
}

_SoNurbsCurveMap::~_SoNurbsCurveMap()
{ 
    if( pts ) delete[] pts; 
}

void
_SoNurbsCurveMap::disable( void )
{
    isenabled = 0;
}

void
_SoNurbsCurveMap::enable( void )
{
    isenabled = 1;
}

/*-------------------------------------------------------------------------
 * map1f - pass a desription of a curve map
 *-------------------------------------------------------------------------
 */
void
_SoNurbsCurveMap::map1f( 
    REAL _ulo,			/* lower parametric bound */
    REAL _uhi,			/* upper parametric bound */
    long _stride, 		/* distance to next point */
    long _order,		/* parametric order */
    REAL *_pts 			/* control points */
)
{
    assert( _pts );
    assert( _stride >= 0 );
    assert( _order >= 0 );
    assert( _order <= MAXORDER );
    assert( _ulo <= _uhi );

    isdefined = 1;

    lo = _ulo;
    hi = _uhi;
    stride = (int) _stride;
    order = (int) _order;

    if( pts == NULL ) 
	pts = new REAL[MAXCOORDS * MAXORDER];

#ifdef SHALLOWCOPY
    ctlpts = _pts;
#endif

#ifdef DEEPCOPY
    deepCopy( _pts );
#endif

#ifdef HORNER
    deepCopyAndScale( _pts );
#endif
}

/*-------------------------------------------------------------------------
 * evalcoord1f - evaluate a point on a curve
 *-------------------------------------------------------------------------
 */
void 
_SoNurbsCurveMap::evalcoord1f( REAL _u )
{
    REAL pt[MAXCOORDS];
    REAL pu[MAXCOORDS];
    REAL puu[MAXCOORDS];

    if( isdefined && isenabled ) {
	init( order, stride, _u, lo, hi ); // XXX
	for( int i=0; i != ncoords; i++ )
	    evaluate( &ctlpts[i], &pt[i], &pu[i], &puu[i] ); // XXX
	domain( &_u );
	uuderiv( puu );
	uderiv( pu );
	point( pt );
    }
}

void
_SoNurbsCurveMap::deepCopy( REAL *_pts )
{
    assert( _pts );

    if( stride == ncoords ) {
	memcpy( pts, _pts, order * ncoords * sizeof( REAL ) );
    } else { 
	REAL *mp = pts;
	REAL *p = _pts;
	for( int i=0; i != order; i++, p += stride ) 
	    for( int j=0; j != ncoords; j++ ) 
		*(mp++) = p[j];
    }

    ctlpts = pts;
    stride = ncoords;
}

void
_SoNurbsCurveMap::deepCopyAndScale( REAL *_pts )
{
    assert( _pts );

    REAL *mp = pts;
    REAL *p = _pts;
    for( int i=0; i<order; i++, p += stride ) {
	REAL comb = Ch(order-1, i);
	for( int j=0; j<ncoords; j++ ) 
	    *(mp++) = p[j] * comb;
    }

    ctlpts = pts;
    stride = ncoords;
}
