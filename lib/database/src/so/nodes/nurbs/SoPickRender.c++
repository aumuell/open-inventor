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
 |      SoPickTess
 |
 |   Author(s)          : Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "SoAddPrefix.h"
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoViewVolumeElement.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoNormal.h>
#include "SoPickRender.h"
#include "SoPickEvals.h"
#include "clients/gl4types.h"

static SbVec2s viewportSize;

_SoNurbsPickRender::_SoNurbsPickRender(SoRayPickAction *pa)
    : _SoNurbsNurbsTessellator(curveEvaluator, surfaceEvaluator)
{
    pa->setObjectSpace();

    // Find a point along the pick ray, and project it onto the
    // screen.  This point will be used by the loadCullingMatrix
    // routine to cull away the NURBS surface that is not contained
    // within the pixel located at that point on the screen.
    const SbLine & pickerLine  = pa->getLine();
    SbVec3f        pickerDir   = pickerLine.getDirection();
    SbVec3f rayPt, worldPt;

    pickerDir.normalize();
    rayPt = pickerLine.getPosition() + pickerDir;

    SbViewVolume viewVol;
    SbMatrix mMat;
    mMat = SoModelMatrixElement::get(pa->getState());
    viewVol = SoViewVolumeElement::get(pa->getState());
    mMat.multVecMatrix(rayPt, worldPt);
    viewVol.projectToScreen(worldPt, objPt);

    redefineMaps();
    defineMap( N_V3D, 0, 3 );
    defineMap( N_V3DR, 1, 4 );
    defineMap( N_T2D, 0, 2 );
    defineMap( N_T2DR, 1, 3 );

    surfaceEvaluator.initialize(pa);
    curveEvaluator.initialize(pa);
}

_SoNurbsPickRender::~_SoNurbsPickRender()
{
}

void 
_SoNurbsPickRender::loadMatrices( SbMatrix &mat, const SbVec2s &vpSize ) 
{
    //
    // Check the method of sampling the NURBS surface.  If it is an
    // object space tessellation, then a sampling matrix does not 
    // need to be loaded.
    //
    INREAL samplingMethod;

    getnurbsproperty( N_V3D,  N_SAMPLINGMETHOD, &samplingMethod );
    if (samplingMethod != N_FIXEDRATE)
        loadSamplingMatrix( mat, vpSize );
   
    // Save the viewport size for the culling matrix
    viewportSize = vpSize;
    loadCullingMatrix( mat );
}

void
_SoNurbsPickRender::loadSamplingMatrix( SbMatrix &vmat, const SbVec2s &vpSize )
{
    float xsize, ysize;
    INREAL smat[4][4];

    /* rescale the mapping to correspond to pixels in x/y */
    xsize = 0.5 * (float) (vpSize[0]);
    ysize = 0.5 * (float) (vpSize[1]);

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

void
_SoNurbsPickRender::loadCullingMatrix( SbMatrix &vmat )
{
    SbMatrix shrinkMat;
    SbVec3f pixelLoc;
    float xsize, ysize;
    INREAL cmat[4][4];

    //
    // Form the culling matrix by adding a matrix which scales the vmat
    // down to the size of one pixel and translates it to the picked pixel
    // position.
    //
    xsize = ((float)(viewportSize[0]));
    ysize = ((float)(viewportSize[1]));
    SbVec2f pixSize(2.0, 2.0);

    shrinkMat.makeIdentity();
    shrinkMat[0][0] = xsize;
    shrinkMat[1][1] = ysize;
    shrinkMat[3][0] = -(objPt[0]*2.0-1.0) * xsize;
    shrinkMat[3][1] = -(objPt[1]*2.0-1.0) * ysize;

    vmat *= shrinkMat;

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
    setnurbsproperty( N_V3D, N_S_STEPS, 2 );
    setnurbsproperty( N_V3DR, N_S_STEPS, 2 );
    setnurbsproperty( N_V3D, N_T_STEPS, 2 );
    setnurbsproperty( N_V3DR, N_T_STEPS, 2 );
    setnurbsproperty( N_SUBDIVISIONS, 10 );
}





