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

#include "../SoAddPrefix.h"
#include "mymath.h"
#include "mystdlib.h"
#include "gl4base.h"

_SoNurbsGL4Base::_SoNurbsGL4Base(
                  _SoNurbsBasicCurveEvaluator& curveEvaluator, 
		  _SoNurbsBasicSurfaceEvaluator& surfaceEvaluator )
	: _SoNurbsNurbsTessellator( curveEvaluator, surfaceEvaluator )
{
    redefineMaps();
    defineMap( N_C4D, 0, 4 );
    defineMap( N_C4DR, 1, 5 );
    defineMap( N_T2D, 0, 2 );
    defineMap( N_T2DR, 1, 3 );
    defineMap( N_V3D, 0, 3 );
    defineMap( N_V3DR, 1, 4 );

    makingGLobj = 0;
    needMatrices = 0;
}

void		
_SoNurbsGL4Base::makeGLobj( int n ) 
{
    makingGLobj = 1;
    objnum = n;
}

void		
_SoNurbsGL4Base::gl4samplingmode( void ) 
{
    needMatrices = 1;
}

void
_SoNurbsGL4Base::bgnrender( void )
{
    if( needMatrices ) loadGLMatrices();
//    if( makingGLobj ) {
//	if( ::isobj( objnum ) ) ::delobj( objnum );
//	::makeobj( objnum );
//    }
}

void
_SoNurbsGL4Base::endrender( void )
{
//    if( makingGLobj )
//	::closeobj( );
}

void 
_SoNurbsGL4Base::loadGLMatrices( void ) 
{
    float vmat[4][4];
    grabGLMatrix( vmat );
    loadCullingMatrix( vmat );
    loadSamplingMatrix( vmat );
}

void
_SoNurbsGL4Base::errorHandler( int )
{
//    ::fprintf( stderr, "Error: %s\n", nurbserror[msg] );
}

/*--------------------------------------------------------------------------
 * grabGLMatrix  
 *--------------------------------------------------------------------------
 */

void
_SoNurbsGL4Base::grabGLMatrix( float [4][4] )
{
//    long matmmode = getmmode();
//    if( matmmode != MSINGLE ) {
//	::mmode( MVIEWING );	
//        float nmat[4][4];
//	::getmatrix( nmat );		/* get viewing matrix */
//	::mmode( MPROJECTION );
//	float pmat[4][4];			
//	::getmatrix( pmat );		/* get projection matrix */
//	multmatrix4d( vmat, nmat, pmat );
//    } else {
//        ::getmatrix( vmat );		/* get combined matrix */
//    }
//    ::mmode( (short) matmmode );
}

void
_SoNurbsGL4Base::loadSamplingMatrix( float [4][4] )
{
//    short llx, urx, lly, ury;
//    ::getviewport( &llx, &urx, &lly, &ury );

    /* rescale the mapping to correspond to pixels in x/y */
/*
    REAL xsize = 0.5 * (REAL) (urx - llx); 
    REAL ysize = 0.5 * (REAL) (ury - lly); 
 
    INREAL smat[4][4];
    smat[0][0] = vmat[0][0] * xsize;
    smat[1][0] = vmat[1][0] * xsize;
    smat[2][0] = vmat[2][0] * xsize;
    smat[3][0] = vmat[3][0] * xsize;

    smat[0][1] = vmat[0][1] * ysize;
    smat[1][1] = vmat[1][1] * ysize;
    smat[2][1] = vmat[2][1] * ysize;
    smat[3][1] = vmat[3][1] * ysize;

    smat[0][2] = 0.0;
    smat[1][2] = 0.0;
    smat[2][2] = 0.0;
    smat[3][2] = 0.0;

    smat[0][3] = vmat[0][3];
    smat[1][3] = vmat[1][3];
    smat[2][3] = vmat[2][3];
    smat[3][3] = vmat[3][3];

    const long rstride = sizeof( smat[0]) / sizeof(smat[0][0]);
    const long cstride = 1;

    setnurbsproperty( N_V3D, N_SAMPLINGMATRIX, &smat[0][0], rstride, cstride );
    setnurbsproperty( N_V3DR, N_SAMPLINGMATRIX, &smat[0][0], rstride, cstride );
*/
}

void
_SoNurbsGL4Base::loadCullingMatrix( float vmat[4][4] )
{
    INREAL cmat[4][4];

    cmat[0][0] = vmat[0][0];
    cmat[0][1] = vmat[0][1];
    cmat[0][2] = vmat[0][2];
    cmat[0][3] = vmat[0][3];

    cmat[1][0] = vmat[1][0];
    cmat[1][1] = vmat[1][1];
    cmat[1][2] = vmat[1][2];
    cmat[1][3] = vmat[1][3];

    cmat[2][0] = vmat[2][0];
    cmat[2][1] = vmat[2][1];
    cmat[2][2] = vmat[2][2];
    cmat[2][3] = vmat[2][3];

    cmat[3][0] = vmat[3][0];
    cmat[3][1] = vmat[3][1];
    cmat[3][2] = vmat[3][2];
    cmat[3][3] = vmat[3][3];

//    const long rstride = sizeof( cmat[0]) / sizeof(cmat[0][0]);
//    const long cstride = 1;

//    setnurbsproperty( N_V3D, N_CULLINGMATRIX, &cmat[0][0], rstride, cstride );
//    setnurbsproperty( N_V3DR, N_CULLINGMATRIX, &cmat[0][0], rstride, cstride );
}

/*---------------------------------------------------------------------
 * A = B * MAT ; transform a 4d vector through a 4x4 matrix
 *---------------------------------------------------------------------
 */
void
_SoNurbsGL4Base::transform4d( float A[4], float B[4], float  mat[4][4] )
{

    A[0] = B[0]*mat[0][0] + B[1]*mat[1][0] + B[2]*mat[2][0] + B[3]*mat[3][0];
    A[1] = B[0]*mat[0][1] + B[1]*mat[1][1] + B[2]*mat[2][1] + B[3]*mat[3][1];
    A[2] = B[0]*mat[0][2] + B[1]*mat[1][2] + B[2]*mat[2][2] + B[3]*mat[3][2];
    A[3] = B[0]*mat[0][3] + B[1]*mat[1][3] + B[2]*mat[2][3] + B[3]*mat[3][3];
}

/*---------------------------------------------------------------------
 * new = [left][right] ; multiply two matrices together
 *---------------------------------------------------------------------
 */
void
_SoNurbsGL4Base::multmatrix4d ( float n[4][4], float left[4][4], float right[4][4] )
{
    transform4d (n[0],left[0],right);
    transform4d (n[1],left[1],right);
    transform4d (n[2],left[2],right);
    transform4d (n[3],left[3],right);
}


