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
 * curvelist.c++ - $Revision: 1.1.1.1 $
 * 	Derrick Burns - 1991
 */

#include "glimports.h"
#include "myassert.h"
#include "mystdio.h"
#include "quilt.h"
#include "curvelist.h"
#include "curve.h"
#include "nurbsconsts.h"
#include "types.h"

Curvelist::Curvelist( Quilt *quilts, REAL pta, REAL ptb )
{
    curve = 0;
    for( Quilt *q = quilts; q; q = q->next ) 
	curve = new Curve( q, pta, ptb, curve );
    range[0] = pta;
    range[1] = ptb;
    range[2] = ptb - pta;
}

Curvelist::Curvelist( Curvelist &upper, REAL value )
{
    Curvelist &lower = *this;
    curve = 0;
    for( Curve *c = upper.curve; c; c = c->next )
	curve = new Curve( *c, value, curve );

    lower.range[0] = upper.range[0];
    lower.range[1] = value;
    lower.range[2] = value - upper.range[0];
    upper.range[0] = value;
    upper.range[2] = upper.range[1] - value;
}

Curvelist::~Curvelist()
{
    while( curve ) {
	Curve *c = curve;
	curve = curve->next;
	delete c;
    }
}

int
Curvelist::cullCheck( void )
{
    for( Curve *c = curve; c; c = c->next )
	if( c->cullCheck() == CULL_TRIVIAL_REJECT )
	    return CULL_TRIVIAL_REJECT;
    return CULL_ACCEPT;
}

void
Curvelist::getstepsize( void )
{
    Curve *c;
    stepsize = range[2];
    for( c = curve; c; c = c->next ) {
	c->getstepsize();
	c->clamp();
	stepsize =  ((c->stepsize < stepsize) ? c->stepsize : stepsize);
	if( c->needsSamplingSubdivision() ) break;
    }
    needsSubdivision = ( c ) ? 1 : 0;
}

int
Curvelist::needsSamplingSubdivision( void )
{
    return needsSubdivision;
}

