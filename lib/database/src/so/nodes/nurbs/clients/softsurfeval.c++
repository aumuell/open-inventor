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
 * softsurfaceeval.c++ - surface evaluator
 *
 * $Revision: 1.1.1.1 $
 */

#include "../SoAddPrefix.h"
#include "myassert.h"
#include <stdio.h>
#include <GL/gl.h>
#include "mystdio.h"
#include "mymath.h"
#include "softsurfmap.h"
#include "softsurfeval.h"
#include "nurbsconsts.h"

inline REAL abs( REAL x ) { return (x < 0.0) ? -x : x; }
inline int equal( REAL x, REAL y ) { return (abs(x-y) < 1.0E-6) ? 1 : 0; }

_SoNurbsSurfaceEvaluator::~_SoNurbsSurfaceEvaluator() 
{ 
    for( _SoNurbsSurfaceMap *next; maps; maps = next ) {
	next = maps->next;
	delete maps;
    }
}

inline _SoNurbsSurfaceMap *
_SoNurbsSurfaceEvaluator::firstMap( void )
{
    return maps;
}

inline _SoNurbsSurfaceMap *
_SoNurbsSurfaceEvaluator::nextMap( _SoNurbsSurfaceMap *m )
{
    return m->next;
}

inline int
_SoNurbsSurfaceEvaluator::isMap( _SoNurbsSurfaceMap *m )
{
    return (m ? 1 : 0);
}

/*---------------------------------------------------------------------------
 * disable - turn off a map
 *---------------------------------------------------------------------------
 */
void
_SoNurbsSurfaceEvaluator::disable( long type )
{
    _SoNurbsSurfaceMap *map = typeToMap(type);
    map->disable();
}

/*---------------------------------------------------------------------------
 * enable - turn on a map
 *---------------------------------------------------------------------------
 */
void
_SoNurbsSurfaceEvaluator::enable( long type )
{
    _SoNurbsSurfaceMap *map = typeToMap(type);
    map->enable();
}

void
_SoNurbsSurfaceEvaluator::validreset()
{
    valid[0][0] = valid[0][1] = 0;
    valid[1][0] = valid[1][1] = 0;
    valid[2][0] = valid[2][1] = 0;
}

/*-------------------------------------------------------------------------
 * mapgrid2f - define a lattice of points with origin and offset
 *-------------------------------------------------------------------------
 */
void
_SoNurbsSurfaceEvaluator::mapgrid2f( long nu, REAL u0, REAL u1, long nv, REAL v0, REAL v1 )
{
    validreset();
    us = u0; dus = (u1-u0)/(REAL)nu;
    vs = v0; dvs = (v1-v0)/(REAL)nv;
}

int 
_SoNurbsSurfaceEvaluator::nextlru( void )
{
    if( lru == 3 ) lru = 0;
    return lru++;
}

void
_SoNurbsSurfaceEvaluator::bgnline( void )
{
    glBegin(GL_LINE_STRIP);
    for( _SoNurbsSurfaceMap *map = firstMap(); isMap( map ); map = nextMap(map) )
        map->startNewTriStrip();
}

void
_SoNurbsSurfaceEvaluator::endline( void )
{
    glEnd();
}

void
_SoNurbsSurfaceEvaluator::range2f( long, REAL *, REAL * )
{
}

void
_SoNurbsSurfaceEvaluator::domain2f( REAL, REAL, REAL, REAL )
{
    //printf( "domain2f [%g,%g] [%g,%g]\n", ulo, uhi, vlo, vhi );
}

void
_SoNurbsSurfaceEvaluator::bgnclosedline( void )
{
    glBegin(GL_LINE_LOOP);
    for( _SoNurbsSurfaceMap *map = firstMap(); isMap( map ); map = nextMap(map) )
        map->startNewTriStrip();
}

void
_SoNurbsSurfaceEvaluator::endclosedline( void )
{
    glEnd();
}

void
_SoNurbsSurfaceEvaluator::bgntmesh( void )
{
    glBegin(GL_TRIANGLES);
    for( _SoNurbsSurfaceMap *map = firstMap(); isMap( map ); map = nextMap(map) )
        map->startNewTriStrip();
}

void
_SoNurbsSurfaceEvaluator::swaptmesh( void )
{
    for( _SoNurbsSurfaceMap *map = firstMap(); isMap( map ); map = nextMap(map) )
        map->swapTriStripVertices();
}

void
_SoNurbsSurfaceEvaluator::endtmesh( void )
{
    glEnd();
}

void
_SoNurbsSurfaceEvaluator::bgnqstrip( void )
{
    glBegin(GL_TRIANGLES);
    for( _SoNurbsSurfaceMap *map = firstMap(); isMap( map ); map = nextMap(map) )
        map->startNewTriStrip();
}

void
_SoNurbsSurfaceEvaluator::endqstrip( void )
{
    glEnd();
}

/*-------------------------------------------------------------------------
 * bgnmap2f - preamble to surface definition and evaluations
 *-------------------------------------------------------------------------
 */
void
_SoNurbsSurfaceEvaluator::bgnmap2f( long )
{
    for( _SoNurbsSurfaceMap *map = firstMap(); isMap( map ); map = nextMap(map) )
	map->disable();
    lru = 0;
    vvals[0] = vvals[1] = vvals[2] =*(REAL *)(& NaN);
    validreset();
}

/*-------------------------------------------------------------------------
 * endmap2f - postamble to a map
 *-------------------------------------------------------------------------
 */
void
_SoNurbsSurfaceEvaluator::endmap2f( void )
{
    for( _SoNurbsSurfaceMap *map = firstMap(); isMap( map ); map = nextMap(map) )
	map->disable();
}

void
_SoNurbsSurfaceEvaluator::addMap( _SoNurbsSurfaceMap *m )
{
    m->next = maps;
    maps = m;
}

/*-------------------------------------------------------------------------
 * map2f - pass a desription of a surface map
 *-------------------------------------------------------------------------
 */
void
_SoNurbsSurfaceEvaluator::map2f(
    long _type,
    REAL _ulower,	/* u lower domain coord		*/
    REAL _uupper,	/* u upper domain coord 	*/
    long _ustride,	/* interpoint distance		*/
    long _uorder,	/* parametric order		*/
    REAL _vlower,	/* v lower domain coord		*/
    REAL _vupper, 	/* v upper domain coord		*/
    long _vstride,	/* interpoint distance		*/
    long _vorder,	/* parametric order		*/
    REAL *_ctlpts) 	/* control points		*/
{
    _SoNurbsSurfaceMap *map = typeToMap(_type);
    map->map2f( _ulower, _uupper, _ustride, _uorder,
	        _vlower, _vupper, _vstride, _vorder, _ctlpts );

    vvals[0] = vvals[1] = vvals[2] =*(REAL *)(& NaN);
    validreset();
}

/*---------------------------------------------------------------------------
 * typeToMap - translate a type symbol to a map pointer
 *---------------------------------------------------------------------------
 */

_SoNurbsSurfaceMap *
_SoNurbsSurfaceEvaluator::typeToMap( long type )
{
    _SoNurbsSurfaceMap *map;
    for( map = firstMap(); map; map = nextMap(map) )
        if( map->isType( type ) ) break;

    return map;
}

/*-------------------------------------------------------------------------
 * mapmesh2f - evaluate a mesh of points on lattice
 *-------------------------------------------------------------------------
 */
void
_SoNurbsSurfaceEvaluator::mapmesh2f( long style, long umin, long umax, long vmin, long vmax )
{
    makeMesh( umin, umax, vmin, vmax );
}

/*-------------------------------------------------------------------------
 * makeMesh - make the mesh of points on lattice
 *-------------------------------------------------------------------------
 */
void
_SoNurbsSurfaceEvaluator::makeMesh( long umin, long umax, long vmin, long vmax )
{
    assert( umax - umin >= 0 );
    assert( vmax - vmin >= 0 );

    REAL domain[2];
    REAL v0;
    int vstart;

    REAL v1 = vs + dvs * vmin;

    int lastv = (int) (vmax + 1);
    for( int gridv1 = (int) (vmin)+1; gridv1 != lastv; gridv1++ ) {

	v0 = v1;
	/* strength reduction == roundoff error */
	v1 = vs + dvs * gridv1;  

	if( equal(v0, vvals[1]) ) {
	    if( ! equal( v1, vvals[0]) ) {
		setv( v1, 0 );
	    }
	    vstart = 1;
	} else if( equal(v0, vvals[0]) ) {
	    if( ! equal(v1, vvals[1]) ) {
	        setv( v1, 1 );
	    }
	    vstart = 0;
	} else if( equal(v1, vvals[1]) ) {
	    setv( v0, 0 );
	    vstart = 0;
	} else {
	    setv( v0, 1 );
	    setv( v1, 0 );
	    vstart = 1;
	}

	assert( equal(vvals[vstart],v0) );
	assert( equal(vvals[1-vstart],v1) );

	bgnqstrip();
 	if( vstart == 1 ) {
	    int lastu = (int) (umax + 1);
	    int gridi = (int) umin;
	    for( int gridu = gridi++; gridu != lastu; gridu = gridi++ ) {

		/* strength reduction == roundoff error */
		domain[0] = us + dus * gridu;

		domain[1] = vvals[1];
		compute( domain, 1, gridi );

		domain[1] = vvals[0];
		compute( domain, 0, gridi );
	    }
	} else {
	    int lastu = (int) (umax + 1);
	    int gridi = (int) umin;
	    for( int gridu = gridi++; gridu != lastu; gridu = gridi++ ) {
		/* strength reduction == roundoff error */
		domain[0] = us + dus * gridu;

		domain[1] = vvals[0];
		compute( domain, 0, gridi );

		domain[1] = vvals[1];
		compute( domain, 1, gridi );
	    }
	}
	endqstrip();
    }
}
/*-------------------------------------------------------------------------
 * evalcoord2f - evaluate a point on a surface
 *-------------------------------------------------------------------------
 */
void
_SoNurbsSurfaceEvaluator::evalcoord2f( long, REAL u, REAL v )
{
    REAL domain[2];
    domain[0] = u;
    domain[1] = v;
    if( equal(v,vvals[0]) ) {
	mapeval( domain, 0, 0 );
    } else if( equal(v,vvals[1]) ) {
	mapeval( domain, 1, 0 );
    } else if( equal(v,vvals[2]) ) {
	mapeval( domain, 2, 0 );
    } else {
	// approximately 20-30% of the time this branch is taken
	int index = nextlru();
	setv( v, index );
	mapeval( domain, index, 0 );
    }
}

/*-------------------------------------------------------------------------
 * evalpoint2i - evaluate a grid point
 *-------------------------------------------------------------------------
 */
void
_SoNurbsSurfaceEvaluator::evalpoint2i( long u, long v )
{
    int gridi = 1 + (int) u;
    REAL domain[2];
    domain[0] = u * dus + us;
    domain[1] = v * dvs + vs;

    if( equal(domain[1], vvals[1]) ) {
	compute( domain, 1, gridi );
    } else if( equal(domain[1], vvals[0]) ) {
	compute( domain, 0, gridi );
    } else if( equal(domain[1], vvals[2]) ) {
	mapeval( domain, 2, 0 );
    } else {
	// this branch is executed less than 20% of the time
	int index = nextlru();
	setv( domain[1], index );
	mapeval( domain, index, 0 );
    }
}

/*-------------------------------------------------------------------------
 * compute - output cached point or evaluate point and output
 *-------------------------------------------------------------------------
 */
void
_SoNurbsSurfaceEvaluator::compute( REAL *domain, int index, int place )
{
    assert( vvals[index] == domain[1] );
    if( ((place >= valid[index][0]) && (place < valid[index][1])) ) {
	assert( place != 0 );
	for( _SoNurbsSurfaceMap *map = firstMap(); isMap( map ); map = nextMap(map) )
	    map->output( domain, index, place );
    } else {
	if( place >= MAXCACHED ) {
	    place = 0;
	} else if( place == valid[index][1] ) {
	    valid[index][1]++;
	} else if( valid[index][0] == valid[index][1] ) {
	    valid[index][0] = place;
	    valid[index][1] = place+1;
	} else if( place == valid[index][0]-1 ) {
	    valid[index][0]--;
	}
	mapeval( domain, index, place );
    }
}

/*---------------------------------------------------------------------------
 * setv - set v coordinate for current isoparametric line
 *---------------------------------------------------------------------------
 */

void
_SoNurbsSurfaceEvaluator::setv( REAL v, int index )
{
    valid[index][0] = valid[index][1] = 0;
    vvals[index] = v;

    for( _SoNurbsSurfaceMap *map = firstMap(); isMap( map ); map = nextMap(map) ) 
	map->setv( v, index );
}

/*---------------------------------------------------------------------------
 * mapeval - evaluate all maps at given point of given isoline
 *---------------------------------------------------------------------------
 */

void
_SoNurbsSurfaceEvaluator::mapeval( REAL *domain, int index, int place )
{
    for( _SoNurbsSurfaceMap *map = firstMap(); isMap( map ); map = nextMap(map) )
	map->mapeval( domain, index, place );
}
