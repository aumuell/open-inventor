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
 |	SoNurbsProfile
 |
 |   Author(s)          : Thad Beier, Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/elements/SoComplexityElement.h>
#include <Inventor/elements/SoComplexityTypeElement.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoProfileCoordinateElement.h>
#include <Inventor/elements/SoProjectionMatrixElement.h>
#include <Inventor/elements/SoViewingMatrixElement.h>
#include <Inventor/nodes/SoNurbsProfile.h>
#include "nurbs/SoAddPrefix.h"
#include "nurbs/SoCurveMaps.h"
#include "nurbs/SoCurveRender.h"

SO_NODE_SOURCE(SoNurbsProfile);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoNurbsProfile::SoNurbsProfile()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoNurbsProfile);
    SO_NODE_ADD_FIELD(knotVector, (0));
    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor (necessary since inline destructor is too complex)
//
// Use: private

SoNurbsProfile::~SoNurbsProfile()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the list of control points and the knot vector. Space
//    will be allocated to store this data.  The calling routine is
//    responsible for freeing this space.
//
// Use: elements

void
SoNurbsProfile::getTrimCurve(SoState *state, int32_t &numPoints, float *&points,
			     int &floatsPerVec,
			     int32_t &numKnots, float *&knots)
//
////////////////////////////////////////////////////////////////////////
{
    const SoProfileCoordinateElement	*pce;
    int					i;
    const float				*tknots;

    pce = SoProfileCoordinateElement::getInstance(state);

    numPoints = index.getNum();

    if (pce->is2D()) {
	floatsPerVec = 2;
	points = new float[numPoints * 2];

	for (i = 0; i < numPoints; i++) {
	    const SbVec2f &t = pce->get2((int) index[i]);
	    points[i*2]   = t[0];
	    points[i*2+1] = t[1];
	}
    }
    else {
	floatsPerVec = 3;
	points = new float[numPoints * 3];

	for (i = 0; i < numPoints; i++) {
	    const SbVec3f &t = pce->get3((int) index[i]);
	    points[i*3]   = t[0];
	    points[i*3+1] = t[1];
	    points[i*3+2] = t[2];
	}
    }

    numKnots = (int32_t) (knotVector.getNum());
    knots    = new float[numKnots];
    tknots   = knotVector.getValues(0);
    memcpy((void *) knots, (const void *) tknots,
	  (int) numKnots * sizeof(float));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns vertices approximating the profile.  Space will be
//    allocated to store the vertices.  The calling routine is
//    responsible for freeing this space.
//
// Use: extender

void
SoNurbsProfile::getVertices(SoState *state,
			    int32_t &nVertices, SbVec2f *&vertices)
//
////////////////////////////////////////////////////////////////////////
{
    _SoNurbsCurveRender			*render = new _SoNurbsCurveRender;
    float				complexity, pixTolerance;
    int					steps, i;
    const SoProfileCoordinateElement	*pce;
    SbVec2f				*verts;
    float				*points;
    int32_t				numPoints;
    int32_t				type, offset;

    pce = SoProfileCoordinateElement::getInstance(state);

    numPoints = index.getNum();

    // Check for no points
    if (numPoints == 0) {
        nVertices = 0;
        vertices = NULL;
        delete render;
        return;
    }

    complexity = SoComplexityElement::get(state);

    if (complexity < 0.0)
	complexity = 0.0;
    if (complexity > 1.0)
	complexity = 1.0;

    if (complexity < 0.10)
	steps = 2;
    else if (complexity < 0.25)
	steps = 3;
    else if (complexity < 0.40)
	steps = 4;
    else if (complexity < 0.55)
	steps = 5;
    else
	steps = (int) (powf(complexity, 3.32) * 28) + 2;

    pixTolerance = 104.0 * complexity * complexity - 252.0 * complexity + 150;

    if (SoComplexityTypeElement::get(state) ==
	SoComplexityTypeElement::OBJECT_SPACE) {
        render->setnurbsproperty(N_V3D,  N_SAMPLINGMETHOD, N_FIXEDRATE);
        render->setnurbsproperty(N_V3DR, N_SAMPLINGMETHOD, N_FIXEDRATE);
        render->setnurbsproperty(N_V3D,  N_S_STEPS, steps);
        render->setnurbsproperty(N_V3D,  N_T_STEPS, steps);
        render->setnurbsproperty(N_V3DR, N_S_STEPS, steps);
        render->setnurbsproperty(N_V3DR, N_T_STEPS, steps);
    }
    else {
        render->setnurbsproperty(N_V3D,  N_SAMPLINGMETHOD, N_NOSAMPLING);
        render->setnurbsproperty(N_V3DR, N_SAMPLINGMETHOD, N_NOSAMPLING);
        render->setnurbsproperty(N_PIXEL_TOLERANCE, pixTolerance);

        //
        // Calculate the total transformation matrix and pass it to the renderer
        //
        SbMatrix mat = (SoModelMatrixElement::get(state)   *
    		        SoViewingMatrixElement::get(state) *
		        SoProjectionMatrixElement::get(state));
        render->loadMatrices(mat);
    }

    //
    // Draw the NURBS curve
    //
    if (pce->is2D()) {
	points = new float[numPoints * 3];

	for (i = 0; i < numPoints; i++) {
	    const SbVec2f &t = pce->get2((int)(index[i]));
	    points[i*3]   = t[0];
	    points[i*3+1] = t[1];
	    points[i*3+2] = 0.0;
	}

        offset = 3 * sizeof(float);
        type = N_V3D;
    }
    else {
	points = new float[numPoints * 4];

	for (i = 0; i < numPoints; i++) {
	    const SbVec3f &t = pce->get3((int)(index[i]));
	    points[i*4]   = t[0];
	    points[i*4+1] = t[1];
	    points[i*4+2] = t[2];
	    points[i*4+3] = 0.0;
	}

        offset = 4 * sizeof(float);
        type = N_V3DR;
    }

    render->bgncurve(0);
    render->nurbscurve(knotVector.getNum(),
		       (INREAL *) knotVector.getValues(0),
		       offset, (INREAL *) points,
		       knotVector.getNum() - numPoints, type);
    render->endcurve();

    //
    // The render now contains the list of vertices.  Return them to
    // the caller.
    //
    render->getVertices(nVertices, verts);
    vertices = new SbVec2f[nVertices];
    memcpy((void *) vertices, (void *) verts,
	  (int) nVertices * sizeof(SbVec2f));

    delete points;
    delete render;

    return;
}
