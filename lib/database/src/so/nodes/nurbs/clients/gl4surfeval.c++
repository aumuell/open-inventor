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
 * gl4surfeval.c++ - surface evaluator using IRIS GL 4.0 backend
 *
 * $Revision: 1.1.1.1 $
 */

#include "myglimports.h"
#include "myassert.h"
#include "mystdio.h"
#include "mymath.h"
#include "gl4surfeval.h"
#include "nurbsconsts.h"
#include <stdio.h>

/*extern "C" {
    void	gl_nurbs_bgncps2( long, float, float, long, float, float, long );
    void	gl_nurbs_cp1f( float );
    void	gl_nurbs_cp2f( const float[2] );
    void	gl_nurbs_cp3f( const float[3] );
    void	gl_nurbs_cp4f( const float[4] );
    void	gl_nurbs_endcps2( void );
    void	gl_nurbs_mapmesh2f( long, long, long, long, long );
    void	gl_nurbs_mapgrid2f( long, float, float, long, float, float );
    void	gl_nurbs_evalcoord2f( float, float );
    void	gl_nurbs_evalpoint2i( long, long );
    void	gl_nurbs_bgnmap2f( void );
    void	gl_nurbs_endmap2f( void );
    void	gl_nurbs_bgnline( long );
    void	gl_nurbs_endline( void );
    void	gl_nurbs_bgnclosedline( long );
    void	gl_nurbs_endclosedline( void );
    void	gl_nurbs_bgntmesh( long );
    void	gl_nurbs_swaptmesh( void );
    void	gl_nurbs_endtmesh( void );
    void	gl_nurbs_bgnqstrip( long );
    void	gl_nurbs_endqstrip( void );
};
*/
void
_SoNurbsGL4SurfaceEval::bgnmap2f( long )
{
    dprintf( "gl_nurbs_bgnmap2f\n" );
//    gl_nurbs_bgnmap2f();
}

void
_SoNurbsGL4SurfaceEval::endmap2f( void )
{
    dprintf( "endmap2f\n" );
//    gl_nurbs_endmap2f();
}

void
_SoNurbsGL4SurfaceEval::map2f(
    long,		/* control point type 		*/
    REAL,	/* u lower domain coord		*/
    REAL,	/* u upper domain coord 	*/
    long,	/* interpoint distance		*/
    long,	/* parametric order		*/
    REAL,	/* v lower domain coord		*/
    REAL,	/* v upper domain coord		*/
    long,	/* interpoint distance		*/
    long,	/* parametric order		*/
    REAL * ) 	/* control points		*/
{
    if( sizeof( REAL ) != sizeof( float ) ) {
	assert( 0 );
/* XXX
	float *p = new float[uorder*vorder];
	int nustride = 
	for( int i=0; i!= uorder; i++ )
	    for( int j=0; j!= vorder; j++ )
		//copy points
*/	
    } else {
	dprintf( "gl_nurbs_bgncps2\n" );
//	gl_nurbs_bgncps2( type, ulower, uupper, uorder, vlower, vupper, vorder );
//	for( int j=0; j!= vorder; j++ )
//	    for( int i=0; i!= uorder; i++ )
//		gl_nurbs_cp4f( &ctlpts[i*ustride + j*vstride] ); /* XXX */
//	gl_nurbs_endcps2();
//	dprintf( "gl_nurbs_endcps2\n" );
    }
}

void
_SoNurbsGL4SurfaceEval::mapmesh2f( long, long, long, long, long )
{
//    dprintf( "gl_nurbs_mapmesh2f\n" );
//    gl_nurbs_mapmesh2f( style, umin, umax, vmin, vmax );
}

/*-------------------------------------------------------------------------
 * mapgrid2f - define a lattice of points with origin and offset
 *-------------------------------------------------------------------------
 */
void
_SoNurbsGL4SurfaceEval::mapgrid2f( long, REAL, REAL, long, REAL, REAL )
{
    dprintf( "gl_nurbs_mapgrid2f\n" );
//    gl_nurbs_mapgrid2f( nu, u0, u1, nv, v0, v1 );
}

void
_SoNurbsGL4SurfaceEval::evalcoord2f( long, REAL, REAL )
{
    dprintf( "gl_nurbs_evalcoord2f\n" );
//    gl_nurbs_evalcoord2f( u, v );
}

void
_SoNurbsGL4SurfaceEval::evalpoint2i( long, long )
{
    dprintf( "gl_nurbs_evalpoint2i\n" );
//    gl_nurbs_evalpoint2i( u, v );
}

void
_SoNurbsGL4SurfaceEval::bgnline( void )
{
    dprintf( "gl_nurbs_bgnline\n" );
//    gl_nurbs_bgnline( 0 );
}

void
_SoNurbsGL4SurfaceEval::endline( void )
{
    dprintf( "gl_nurbs_endline\n" );
//    gl_nurbs_endline( );
}

void
_SoNurbsGL4SurfaceEval::bgnclosedline( void )
{
    dprintf( "gl_nurbs_bgnclosedline\n" );
//    gl_nurbs_bgnclosedline( 0 );
}

void
_SoNurbsGL4SurfaceEval::endclosedline( void )
{
    dprintf( "gl_nurbs_endclosedline\n" );
//    gl_nurbs_endclosedline( );
}

void
_SoNurbsGL4SurfaceEval::bgntmesh( void )
{
    dprintf( "gl_nurbs_bgntmesh\n" );
//    gl_nurbs_bgntmesh( 0 );
}

void
_SoNurbsGL4SurfaceEval::swaptmesh( void )
{
    dprintf( "gl_nurbs_swaptmesh\n" );
//    gl_nurbs_swaptmesh();
}

void
_SoNurbsGL4SurfaceEval::endtmesh( void )
{
    dprintf( "gl_nurbs_endtmesh\n" );
//    gl_nurbs_endtmesh();
}

void
_SoNurbsGL4SurfaceEval::bgnqstrip( void )
{
    dprintf( "gl_nurbs_bgnqstrip\n" );
//    gl_nurbs_bgnqstrip( 0 );
}

void
_SoNurbsGL4SurfaceEval::endqstrip( void )
{
    dprintf( "gl_nurbs_endqstrip\n" );
//    gl_nurbs_endqstrip();
}

void
_SoNurbsGL4SurfaceEval::enable( long )
{
    dprintf( "enable\n" );
}

void
_SoNurbsGL4SurfaceEval::disable( long )
{
    dprintf( "disable\n" );
}

void
_SoNurbsGL4SurfaceEval::range2f( long, REAL *, REAL * )
{
    // no-op
}

void
_SoNurbsGL4SurfaceEval::domain2f(REAL, REAL, REAL, REAL )
{
    // no-op
}

int
_SoNurbsGL4SurfaceEval::canRecord( void )
{
    return 0;
}

int
_SoNurbsGL4SurfaceEval::canPlayAndRecord( void )
{
    return 0;
}

int
_SoNurbsGL4SurfaceEval::createHandle( int )
{
    return 0;
}

void
_SoNurbsGL4SurfaceEval::beginOutput( ServiceMode, int )
{
}

void
_SoNurbsGL4SurfaceEval::endOutput( void )
{
} 

void
_SoNurbsGL4SurfaceEval::discardRecording( int )
{
}

void
_SoNurbsGL4SurfaceEval::playRecording( int )
{
}
