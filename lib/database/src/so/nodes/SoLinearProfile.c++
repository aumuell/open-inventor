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
 |	SoLinearProfile
 |
 |   Author(s)          : Thad Beier, Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/elements/SoProfileCoordinateElement.h>
#include <Inventor/nodes/SoLinearProfile.h>

SO_NODE_SOURCE(SoLinearProfile);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoLinearProfile::SoLinearProfile()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoLinearProfile);
    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor (necessary since inline destructor is too complex)
//
// Use: private

SoLinearProfile::~SoLinearProfile()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Static helper routine that figures out how many points will
//    really be used, taking into account USE_REST_OF_VERTICES
//
// Use: internal

static int
getNumPoints(const SoMFInt32 &index, int numCoords)
//
////////////////////////////////////////////////////////////////////////
{
    int numIndices = index.getNum();
    if (index[numIndices-1] != SO_LINEAR_PROFILE_USE_REST_OF_VERTICES)
	return numIndices;

    int lastIndex = (numIndices == 1 ? -1 : index[numIndices-2]+1);

    // Return regular indices plus number of coordinates left:
    return (numIndices-1) + (numCoords-1 - lastIndex);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Static helper routine that returns the correct index,
//    taking into account USE_REST_OF_VERTICES
//
// Use: internal

static int
getIndex(int i, const SoMFInt32 &index, int numCoords)
//
////////////////////////////////////////////////////////////////////////
{
    int numIndices = index.getNum();
    int result;

    if (index[numIndices-1] != SO_LINEAR_PROFILE_USE_REST_OF_VERTICES) {
	result = index[i];
    } else {
	if (i < numIndices-1) result = i;
	else {
	    int lastIndex = (numIndices == 1 ? -1 : index[numIndices-2]+1);
	    
	    result = lastIndex+1 + i-(numIndices-1);
	}
    }

#ifdef DEBUG
    if (result < 0 || result > numCoords) {
	SoDebugError::post("SoLinearProfile", "index[%d]=%d out of "
			   "range (0,%d)", i, result, 0, numCoords);
    }
#endif
    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns a list of points used in making a piecewise linear trim
//    curve. numKnots and knotVector will be returned as 0 and NULL.
//
// Use: extender

void
SoLinearProfile::getTrimCurve(SoState *state, int32_t &numPoints, float *&points,
			      int &floatsPerVec,
			      int32_t &numKnots, float *&knotVector)
//
////////////////////////////////////////////////////////////////////////
{
    const SoProfileCoordinateElement	*pce;
    int					i;

    pce = SoProfileCoordinateElement::getInstance(state);

    numPoints = getNumPoints(index, pce->getNum());

    if (pce->is2D()) {
	floatsPerVec = 2;
	points = new float[numPoints * 2];

	for (i = 0; i < numPoints; i++) {
	    const SbVec2f &t = pce->get2(getIndex(i, index, pce->getNum()));
	    points[i*2]   = t[0];
	    points[i*2+1] = t[1];
	}
    }
    else {
	floatsPerVec = 3;
	points = new float[numPoints * 3];

	for (i = 0; i < numPoints; i++) {
	    const SbVec3f &t = pce->get3(getIndex(i, index, pce->getNum()));
	    points[i*3]   = t[0];
	    points[i*3+1] = t[1];
	    points[i*3+2] = t[2];
	}
    }	
    numKnots = 0;
    knotVector = NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the vertices of the profile.  These will just be the
//    profile coordinates.  If the coordinates are rational, divide
//    out the third component.  Space will be allocated to store the
//    vertices.  The calling routine is responsible for freeing this
//    space.
//
// Use: extender

void
SoLinearProfile::getVertices(SoState *state,
			     int32_t &nVertices, SbVec2f *&vertices)
//
////////////////////////////////////////////////////////////////////////
{
    const SoProfileCoordinateElement	*pce;
    int					i;

    pce = SoProfileCoordinateElement::getInstance(state);

    nVertices = getNumPoints(index, pce->getNum());
    if (nVertices > 0) {
	vertices  = new SbVec2f[nVertices];
    
	for (i = 0; i < nVertices; i++)
	    vertices[i] = pce->get2(getIndex(i, index, pce->getNum()));
    } else {
	vertices = NULL;
    }
}
