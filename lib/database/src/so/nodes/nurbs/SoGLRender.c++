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
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |      _SoNurbsGLRender
 |
 |   Author(s)          : Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <math.h>
#include <stdlib.h>
#include <Inventor/errors/SoDebugError.h>
#include "SoAddPrefix.h"
#include "SoGLRender.h"
#include "clients/softcurvemaps.h"
#include "clients/softsurfacemaps.h"

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor for the SoGLRender class.
//
// Use: public

_SoNurbsGLRender::_SoNurbsGLRender( void )
	: _SoNurbsNurbsTessellator( curveEvaluator, surfaceEvaluator )

//
////////////////////////////////////////////////////////////////////////
{
    redefineMaps();
    defineMap( N_C4D, 0, 4 );
    defineMap( N_C4DR, 1, 5 );
    defineMap( N_T2D, 0, 2 );
    defineMap( N_T2DR, 1, 3 );
    defineMap( N_V3D, 0, 3 );
    defineMap( N_V3DR, 1, 4 );

    v3SurfaceMap = new _SoNurbsVertex3SurfaceMap;
    v4SurfaceMap = new _SoNurbsVertex4SurfaceMap;
    c4SurfaceMap = new _SoNurbsColor4SurfaceMap;
    t2SurfaceMap = new _SoNurbsTexture2SurfaceMap;

    v3CurveMap   = new _SoNurbsVertex3CurveMap;
    v4CurveMap   = new _SoNurbsVertex4CurveMap;
    c4CurveMap   = new _SoNurbsColor4CurveMap;
//    t2CurveMap   = new _SoNurbsTexture2CurveMap;

    surfaceEvaluator.addMap( v3SurfaceMap );
    surfaceEvaluator.addMap( v4SurfaceMap );
    surfaceEvaluator.addMap( c4SurfaceMap );
    surfaceEvaluator.addMap( t2SurfaceMap );

    curveEvaluator.addMap( v3CurveMap );
    curveEvaluator.addMap( v4CurveMap );
    curveEvaluator.addMap( c4CurveMap );
//    curveEvaluator.addMap( t2CurveMap );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor for the SoGLRender class.
//
// Use: public

_SoNurbsGLRender::~_SoNurbsGLRender()

//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Load the sampling and culling matrices used in the tessellation.
//
// Use: public

void 
_SoNurbsGLRender::loadMatrices( SbMatrix &vmat, const SbVec2s &vpSize ) 

//
////////////////////////////////////////////////////////////////////////
{
    loadCullingMatrix( vmat );

    //
    // Check the method of sampling the NURBS surface.  If it is an
    // object space tessellation, then a sampling matrix does not 
    // need to be loaded.
    //
    INREAL samplingMethod;

    getnurbsproperty( N_V3D,  N_SAMPLINGMETHOD, &samplingMethod );
    if (samplingMethod != N_FIXEDRATE)
        loadSamplingMatrix( vmat, vpSize );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Load the sampling matrix.
//
// Use: protected

void
_SoNurbsGLRender::loadSamplingMatrix( SbMatrix &vmat, const SbVec2s &vpSize )

//
////////////////////////////////////////////////////////////////////////
{
    /* rescale the mapping to correspond to pixels in x/y */
    REAL xsize = 0.5 * (REAL) (vpSize[0]); 
    REAL ysize = 0.5 * (REAL) (vpSize[1]); 

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

    setnurbsproperty( N_V3D, N_SAMPLINGMATRIX, &smat[0][0], 4, 1);
    setnurbsproperty( N_V3DR, N_SAMPLINGMATRIX, &smat[0][0], 4, 1);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Load the culling matrix.
//
// Use: protected

void
_SoNurbsGLRender::loadCullingMatrix( SbMatrix &vmat )

//
////////////////////////////////////////////////////////////////////////
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

    setnurbsproperty( N_V3D, N_CULLINGMATRIX, &cmat[0][0], 4, 1);
    setnurbsproperty( N_V3DR, N_CULLINGMATRIX, &cmat[0][0], 4, 1);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Handle an error generated by bad input data.
//
// Use: public

void
_SoNurbsGLRender::errorHandler( int msg )

//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
#if	0	/* libnurbs doesn't support error messages any longer */
    SoDebugError::post("_SoNurbsGLRender::errorHandler",
            "NURBS - %s", _SoNurbsNurbsTessellator::nurbserror[msg]);
#endif
#endif      
}
