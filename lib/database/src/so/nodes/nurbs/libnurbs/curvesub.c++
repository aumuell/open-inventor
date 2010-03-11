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
 * curvesub.c++ - $Revision: 1.1.1.1 $
 * 	Derrick Burns - 1991
 */

#include "glimports.h"
#include "myassert.h"
#include "mystdio.h"
#include "subdivider.h"
#include "renderhints.h"
#include "backend.h"
#include "quilt.h"
#include "curvelist.h"
#include "curve.h"
#include "nurbsconsts.h"

/*--------------------------------------------------------------------------
 * drawCurves - main curve rendering entry point
 *--------------------------------------------------------------------------
 */

void
Subdivider::drawCurves( void )
{
    REAL 	from[1], to[1];
    Flist	bpts;
    qlist->getRange( from, to, bpts );

    renderhints.init( );

    backend.bgncurv();
    for( int i=bpts.start; i<bpts.end-1; i++ ) {
        REAL pta, ptb;
	pta = bpts.pts[i];
	ptb = bpts.pts[i+1];

	qlist->downloadAll( &pta, &ptb, backend );

	Curvelist curvelist( qlist, pta, ptb );
	samplingSplit( curvelist, renderhints.maxsubdivisions );
    }
    backend.endcurv();
}


/*--------------------------------------------------------------------------
 * samplingSplit - recursively subdivide patch, cull check each subpatch  
 *--------------------------------------------------------------------------
 */

void
Subdivider::samplingSplit( Curvelist& curvelist, int subdivisions )
{
    if( curvelist.cullCheck() == CULL_TRIVIAL_REJECT )  return;

    curvelist.getstepsize();

    if( curvelist.needsSamplingSubdivision() && (subdivisions > 0) ) {
	REAL mid = ( curvelist.range[0] + curvelist.range[1] ) * 0.5;
	Curvelist lowerlist( curvelist, mid );
	samplingSplit( lowerlist, subdivisions-1 ); // lower
	samplingSplit( curvelist, subdivisions-1 ); // upper
    } else {
	long nu = 1 + ((long) (curvelist.range[2] / curvelist.stepsize));
	backend.curvgrid( curvelist.range[0], curvelist.range[1], nu );
	backend.curvmesh( 0, nu );
    }
}

