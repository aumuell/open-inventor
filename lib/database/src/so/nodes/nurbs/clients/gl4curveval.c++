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
 * gl4curveval.c++ - curve evaluator
 *
 * $Revision: 1.1.1.1 $
 */

#include "myglimports.h"
#include "myassert.h"
#include "mystdio.h"
#include "mymath.h"
#include "gl4curveval.h"

/*
extern "C" {
    void	gl_nurbs_bgncps1( long, float, float, long  );
    void	gl_nurbs_cp1f( float );
    void	gl_nurbs_cp2f( const float[2] );
    void	gl_nurbs_cp3f( const float[3] );
    void	gl_nurbs_cp4f( const float[4] );
    void	gl_nurbs_endcps1( void );
    void	gl_nurbs_mapmesh1f( long, long, long );
    void	gl_nurbs_mapgrid1f( long, float, float );
    void	gl_nurbs_evalcoord1f( float );
    void	gl_nurbs_evalpoint1i( long );
    void	gl_nurbs_bgnmap1f( void );
    void	gl_nurbs_endmap1f( void );
    void	gl_nurbs_bgnline( long );
    void	gl_nurbs_endline( void );
};
*/
void
_SoNurbsGL4CurveEval::bgnmap1f( long )
{
//    gl_nurbs_bgnmap1f();
}

void
_SoNurbsGL4CurveEval::endmap1f( void )
{
//    gl_nurbs_endmap1f();
}

void
_SoNurbsGL4CurveEval::map1f(
    long,		/* control point type 		*/
    REAL,	/* u lower domain coord		*/
    REAL, 	/* u upper domain coord 	*/
    long,	/* interpoint distance		*/
    long,	/* parametric order		*/
    REAL * ) 	/* control points		*/
{
/*
    assert( sizeof( REAL) == sizeof( float ) );
    // XXX - if REAL != float need to copy ctlpts
    gl_nurbs_bgncps1( type, ulower, uupper, uorder );
    for( int i=0; i != uorder; i++ )
	gl_nurbs_cp4f( &(ctlpts[i*ustride]) );
    gl_nurbs_endcps1();
*/
}

void
_SoNurbsGL4CurveEval::mapmesh1f( long, long, long )
{
//    gl_nurbs_mapmesh1f( style, umin, umax );
}

void
_SoNurbsGL4CurveEval::evalcoord1f( long, REAL )
{
//    gl_nurbs_evalcoord1f( u );
}

/*-------------------------------------------------------------------------
 * mapgrid1f - define a lattice of points with origin and offset
 *-------------------------------------------------------------------------
 */
void
_SoNurbsGL4CurveEval::mapgrid1f( long, REAL, REAL  )
{
//    gl_nurbs_mapgrid1f( nu, u0, u1 );
}

void
_SoNurbsGL4CurveEval::bgnline( void )
{
//    gl_nurbs_bgnline(0);
}

void
_SoNurbsGL4CurveEval::endline( void )
{
//    gl_nurbs_endline();
}

void
_SoNurbsGL4CurveEval::enable( long )
{
    // no-op
}

void
_SoNurbsGL4CurveEval::disable( long )
{
    // no-op
}

void
_SoNurbsGL4CurveEval::range1f( long, REAL *, REAL * )
{
    // no-op
}

void
_SoNurbsGL4CurveEval::domain1f( REAL, REAL )
{
    // no-op
}

int
_SoNurbsGL4CurveEval::canRecord( void )
{
    return 0;
}

int
_SoNurbsGL4CurveEval::canPlayAndRecord( void )
{
    return 0;
}

int
_SoNurbsGL4CurveEval::createHandle( int )
{
    return 0;
}

void
_SoNurbsGL4CurveEval::beginOutput( ServiceMode, int )
{
}

void
_SoNurbsGL4CurveEval::endOutput( void )
{
} 

void
_SoNurbsGL4CurveEval::discardRecording( int )
{
}

void
_SoNurbsGL4CurveEval::playRecording( int )
{
}
