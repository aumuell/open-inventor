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
 * softcurveval.c++ - curve evaluator
 *
 * $Revision: 1.1 $
 */

#include "../SoAddPrefix.h"
#include <GL/gl.h>
#include "myassert.h"
#include "softcurveval.h"
#include "softcurvmap.h"
#include "cachingeval.h"

 
_SoNurbsCurveEvaluator::_SoNurbsCurveEvaluator( void ) 
{ 
    maps = 0;
}

_SoNurbsCurveEvaluator::~_SoNurbsCurveEvaluator( void ) 
{ 
    for( _SoNurbsCurveMap *next; maps; maps = next ) {
	next = maps->next;
	delete maps;
    }
}

void
_SoNurbsCurveEvaluator::addMap( _SoNurbsCurveMap *m )
{
    m->next = maps;
    maps = m;
}

_SoNurbsCurveMap *
_SoNurbsCurveEvaluator::firstMap( void )
{
    return maps;
}

_SoNurbsCurveMap *
_SoNurbsCurveEvaluator::nextMap( _SoNurbsCurveMap *m )
{
    return  m->next;
}

int
_SoNurbsCurveEvaluator::isMap( _SoNurbsCurveMap *m )
{
    return (m ? 1 : 0);
}

void
_SoNurbsCurveEvaluator::range1f( long, REAL *, REAL * )
{
}

void
_SoNurbsCurveEvaluator::domain1f( REAL, REAL )
{
}

void
_SoNurbsCurveEvaluator::bgnline( void )
{
    glBegin( GL_LINE_STRIP );
}

void
_SoNurbsCurveEvaluator::endline( void )
{
    glEnd();
}

/*---------------------------------------------------------------------------
 * disable - turn off a curve map
 *---------------------------------------------------------------------------
 */
void
_SoNurbsCurveEvaluator::disable( long type )
{
    _SoNurbsCurveMap *map = typeToMap(type);
    assert( map );
    map->disable();
}

/*---------------------------------------------------------------------------
 * enable - turn on a curve map
 *---------------------------------------------------------------------------
 */
void
_SoNurbsCurveEvaluator::enable( long type )
{
    _SoNurbsCurveMap *map = typeToMap(type);
    assert( map );
    map->enable();
}

/*-------------------------------------------------------------------------
 * mapgrid1f - define a lattice of points with origin and offset
 *-------------------------------------------------------------------------
 */
void 
_SoNurbsCurveEvaluator::mapgrid1f( long nu, REAL u0, REAL u1 )
{
    cus = u0;
    cdus = (u1 - u0)/(REAL)nu;
}

/*-------------------------------------------------------------------------
 * bgnmap1 - preamble to curve definition and evaluations
 *-------------------------------------------------------------------------
 */
void
_SoNurbsCurveEvaluator::bgnmap1f( long )
{
    for( _SoNurbsCurveMap *map = firstMap(); isMap(map); map = nextMap(map) )
        map->disable();
}

/*-------------------------------------------------------------------------
 * endmap1 - postamble to a curve map
 *-------------------------------------------------------------------------
 */
void
_SoNurbsCurveEvaluator::endmap1f( void )
{
    for( _SoNurbsCurveMap *map = firstMap(); isMap(map); map = nextMap(map) )
	map->disable();
}

/*-------------------------------------------------------------------------
 * map1f - pass a desription of a curve map
 *-------------------------------------------------------------------------
 */
void
_SoNurbsCurveEvaluator::map1f( 
    long type,		 	/* map type */
    REAL ulo,			/* lower parametric bound */
    REAL uhi,			/* upper parametric bound */
    long stride, 		/* distance to next point in REALS */
    long order,			/* parametric order */
    REAL *pts 			/* control points */
)
{
    _SoNurbsCurveMap *map = typeToMap(type);
    assert( map );
    assert( map->isType( type ) ); 
    map->map1f( ulo, uhi, stride, order, pts );
}

/*-------------------------------------------------------------------------
 * mapmesh1f - evaluate a mesh of points on lattice
 *-------------------------------------------------------------------------
 */
void _SoNurbsCurveEvaluator::mapmesh1f( long, long from, long to )
{
    assert( to - from >= 0 );
    bgnline();
    for( long i = from; i <= to; i++ )
        evalcoord1f( 0, cus + i * cdus );
    endline();
}

/*-------------------------------------------------------------------------
 * evalpoint1i - evaluate a point on a curve
 *-------------------------------------------------------------------------
 */
void _SoNurbsCurveEvaluator::evalpoint1i( long i )
{
    for( _SoNurbsCurveMap *map = firstMap(); isMap(map); map = nextMap(map) ) 
	map->evalcoord1f( cus + i * cdus );
}

/*-------------------------------------------------------------------------
 * evalcoord1f - evaluate a point on a curve
 *-------------------------------------------------------------------------
 */
void _SoNurbsCurveEvaluator::evalcoord1f( long, REAL u )
{
    for( _SoNurbsCurveMap *map = firstMap(); isMap(map); map = nextMap(map) ) 
	map->evalcoord1f( u );
}

/*---------------------------------------------------------------------------
 * typeToMap - translate a type symbol to a map pointer
 *---------------------------------------------------------------------------
 */

_SoNurbsCurveMap *
_SoNurbsCurveEvaluator::typeToMap( long type )
{
    _SoNurbsCurveMap *map;
    for( map = firstMap(); map; map = nextMap(map) )
        if( map->isType( type ) ) break;
    return map;
}

int
_SoNurbsCurveEvaluator::canRecord( void )
{
    return 0;
}

int
_SoNurbsCurveEvaluator::canPlayAndRecord( void )
{
    return 0;
}

int
_SoNurbsCurveEvaluator::createHandle( int )
{
    return 0;
}

void
_SoNurbsCurveEvaluator::beginOutput( ServiceMode, int )
{
}

void
_SoNurbsCurveEvaluator::endOutput( void )
{
} 

void
_SoNurbsCurveEvaluator::discardRecording( int )
{
}

void
_SoNurbsCurveEvaluator::playRecording( int )
{
}
