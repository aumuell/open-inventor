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
 * basiccrveval.c++ - $Revision: 1.1.1.1 $
 *	Derrick Burns - 1992
 */

#include "mystdio.h"
#include "types.h"
#include "basiccrveval.h"
#include <stdio.h>

void 
BasicCurveEvaluator::domain1f( REAL, REAL )
{
#ifndef NDEBUG
    dprintf( "domain1f\n" );
#endif
}

void 
BasicCurveEvaluator::range1f( long type, REAL *, REAL * )
{
#ifndef NDEBUG
    dprintf( "range1f\n" );
#endif
}

void 
BasicCurveEvaluator::enable( long )
{
#ifndef NDEBUG
    dprintf( "enable\n" );
#endif
}

void 
BasicCurveEvaluator::disable( long )
{
#ifndef NDEBUG
    dprintf( "disable\n" );
#endif
}

void 
BasicCurveEvaluator::bgnmap1f( long )
{
#ifndef NDEBUG
    dprintf( "bgnmap1f\n" );
#endif
}

void 
BasicCurveEvaluator::map1f( long, REAL, REAL, long, long, REAL * )
{
#ifndef NDEBUG
    dprintf( "map1f\n" );
#endif
}

void 
BasicCurveEvaluator::mapgrid1f( long, REAL, REAL )
{
#ifndef NDEBUG
    dprintf( "mapgrid1f\n" );
#endif
}

void 
BasicCurveEvaluator::mapmesh1f( long, long, long )
{
#ifndef NDEBUG
    dprintf( "mapmesh1f\n" );
#endif
}

void 
BasicCurveEvaluator::evalcoord1f( long, REAL )
{
#ifndef NDEBUG
    dprintf( "evalcoord1f\n" );
#endif
}

void 
BasicCurveEvaluator::endmap1f( void )
{
#ifndef NDEBUG
    dprintf( "endmap1f\n" );
#endif
}

void 
BasicCurveEvaluator::bgnline( void )
{
#ifndef NDEBUG
    dprintf( "bgnline\n" );
#endif
}

void 
BasicCurveEvaluator::endline( void )
{
#ifndef NDEBUG
    dprintf( "endline\n" );
#endif
}
