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
 |   $Revision $
 |
 |   Classes:
 |      _SoNurbsCurveRender
 |
 |   Author(s)          : Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <GL/gl.h>
#include <math.h>
#include <stdlib.h>
#include "SoAddPrefix.h"
#include "SoCurveRender.h"
#include "SoCurveRenderEval.h"
#include "SoCurveMaps.h"

_SoNurbsCurveRender::_SoNurbsCurveRender()
    : _SoNurbsNurbsTessellator(curveEvaluator, surfaceEvaluator)
{
    curveEvaluator.initialize ();

    redefineMaps();
    defineMap( N_V3D, 0, 3 );
    defineMap( N_V3DR, 1, 4 );
}

_SoNurbsCurveRender::~_SoNurbsCurveRender()
{
}
void 
_SoNurbsCurveRender::loadMatrices( SbMatrix &mat ) 
{
    loadCullingMatrix( mat );

    //
    // Check the method of sampling the NURBS surface.  If it is an
    // object space tessellation, then a sampling matrix does not 
    // need to be loaded.
    //
    INREAL samplingMethod;

    getnurbsproperty( N_V3D,  N_SAMPLINGMETHOD, &samplingMethod );
    if (samplingMethod != N_FIXEDRATE)
        loadSamplingMatrix( mat );
}

void
_SoNurbsCurveRender::loadSamplingMatrix( SbMatrix &vmat )
{
    float xsize, ysize;
    INREAL smat[4][4];
    GLint vpParams[4];

    ::glGetIntegerv(GL_VIEWPORT, vpParams);

    /* rescale the mapping to correspond to pixels in x/y */
    xsize = 0.5 * (float) (vpParams[2]);
    ysize = 0.5 * (float) (vpParams[3]);

    smat[0][0] = vmat[0][0] * xsize;
    smat[1][0] = vmat[1][0] * xsize;
    smat[2][0] = vmat[2][0] * xsize;
    smat[3][0] = vmat[3][0] * xsize;

    smat[0][1] = vmat[0][1] * ysize;
    smat[1][1] = vmat[1][1] * ysize;
    smat[2][1] = vmat[2][1] * ysize;
    smat[3][1] = vmat[3][1] * ysize;

    smat[0][2] = vmat[0][2];
    smat[1][2] = vmat[1][2];
    smat[2][2] = vmat[2][2];
    smat[3][2] = vmat[3][2];

    smat[0][3] = vmat[0][3];
    smat[1][3] = vmat[1][3];
    smat[2][3] = vmat[2][3];
    smat[3][3] = vmat[3][3];

    setnurbsproperty( N_V3D, N_SAMPLINGMATRIX, &smat[0][0], 4, 1);
    setnurbsproperty( N_V3DR, N_SAMPLINGMATRIX, &smat[0][0], 4, 1);
}

void
_SoNurbsCurveRender::loadCullingMatrix( SbMatrix &vmat )
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


void 
_SoNurbsCurveRender::getVertices( int32_t &nVerts, SbVec2f *&verts ) 
{
    curveEvaluator.getVertices (nVerts, verts);
}




