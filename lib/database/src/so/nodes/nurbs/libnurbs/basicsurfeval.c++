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
 * basicsurfaceevaluator.c++ - $Revision: 1.1.1.1 $
  *	Derrick Burns - 1992
 */

#include "mystdio.h"
#include "types.h"
#include "basicsurfeval.h"
#include <stdio.h>

void 
BasicSurfaceEvaluator::domain2f( REAL, REAL, REAL, REAL )
{
#ifndef NDEBUG
    dprintf( "domain2f\n" );
#endif
}

void 
BasicSurfaceEvaluator::polymode( long )
{
#ifndef NDEBUG
    dprintf( "polymode\n" );
#endif
}

void
BasicSurfaceEvaluator::range2f( long type, REAL *from, REAL *to )
{
#ifndef NDEBUG
    dprintf( "range2f type %ld, from (%g,%g), to (%g,%g)\n", 
		type, from[0], from[1], to[0], to[1] );
#endif
}

void 
BasicSurfaceEvaluator::enable( long )
{
#ifndef NDEBUG
    dprintf( "enable\n" );
#endif
}

void 
BasicSurfaceEvaluator::disable( long )
{
#ifndef NDEBUG
    dprintf( "disable\n" );
#endif
}

void 
BasicSurfaceEvaluator::bgnmap2f( long )
{
#ifndef NDEBUG
    dprintf( "bgnmap2f\n" );
#endif
}

void 
BasicSurfaceEvaluator::endmap2f( void )
{
#ifndef NDEBUG
    dprintf( "endmap2f\n" );
#endif
}

void 
BasicSurfaceEvaluator::map2f( long, REAL, REAL, long, long, 
				    REAL, REAL, long, long,
			      REAL * )
{
#ifndef NDEBUG
    dprintf( "map2f\n" );
#endif
}

void 
BasicSurfaceEvaluator::mapgrid2f( long, REAL, REAL, long, REAL, REAL )
{
#ifndef NDEBUG
    dprintf( "mapgrid2f\n" );
#endif
}

void 
BasicSurfaceEvaluator::mapmesh2f( long, long, long, long, long )
{
#ifndef NDEBUG
    dprintf( "mapmesh2f\n" );
#endif
}

void 
BasicSurfaceEvaluator::evalcoord2f( long, REAL, REAL )
{
#ifndef NDEBUG
    dprintf( "evalcoord2f\n" );
#endif
}

void 
BasicSurfaceEvaluator::evalpoint2i( long, long )
{
#ifndef NDEBUG
    dprintf( "evalpoint2i\n" );
#endif
}

void 
BasicSurfaceEvaluator::bgnline( void )
{
#ifndef NDEBUG
    dprintf( "bgnline\n" );
#endif
}

void 
BasicSurfaceEvaluator::endline( void )
{
#ifndef NDEBUG
    dprintf( "endline\n" );
#endif
}

void 
BasicSurfaceEvaluator::bgnclosedline( void )
{
#ifndef NDEBUG
    dprintf( "bgnclosedline\n" );
#endif
}

void 
BasicSurfaceEvaluator::endclosedline( void )
{
#ifndef NDEBUG
    dprintf( "endclosedline\n" );
#endif
}

void 
BasicSurfaceEvaluator::bgntmesh( void )
{
#ifndef NDEBUG
    dprintf( "bgntmesh\n" );
#endif
}

void 
BasicSurfaceEvaluator::swaptmesh( void )
{
#ifndef NDEBUG
    dprintf( "swaptmesh\n" );
#endif
}

void 
BasicSurfaceEvaluator::endtmesh( void )
{
#ifndef NDEBUG
    dprintf( "endtmesh\n" );
#endif
}

void 
BasicSurfaceEvaluator::bgnqstrip( void )
{
#ifndef NDEBUG
    dprintf( "bgnqstrip\n" );
#endif
}

void 
BasicSurfaceEvaluator::endqstrip( void )
{
#ifndef NDEBUG
    dprintf( "endqstrip\n" );
#endif
}

