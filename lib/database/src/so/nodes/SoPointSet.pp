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
 |   $Revision: 1.1 $
 |
 |   Classes:
 |      SoPointSet
 |
@@ This next line is true for the output file only - ignore it here:
 |   Note: This file was preprocessed from another file. Do not edit it.
 |
 |   Author(s)          : Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <GL/gl.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/bundles/SoMaterialBundle.h>
#include <Inventor/bundles/SoTextureCoordinateBundle.h>
#include <Inventor/caches/SoBoundingBoxCache.h>
#include <Inventor/details/SoPointDetail.h>
#include <Inventor/elements/SoComplexityElement.h>
#include <Inventor/elements/SoGLCoordinateElement.h>
#include <Inventor/elements/SoLightModelElement.h>
#include <Inventor/elements/SoMaterialBindingElement.h>
#include <Inventor/elements/SoNormalBindingElement.h>
#include <Inventor/elements/SoGLNormalElement.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/nodes/SoPointSet.h>

SO_NODE_SOURCE(SoPointSet);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoPointSet::SoPointSet()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoPointSet);
    SO_NODE_ADD_FIELD(numPoints,  (SO_POINT_SET_USE_REST_OF_POINTS));
    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoPointSet::~SoPointSet()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements GL rendering.
//
// Use: extender

void
SoPointSet::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool			materialPerPoint, normalPerPoint;
    int32_t			numPts;
    int				curCoord, i;

    // First see if the object is visible and should be rendered now
    if (! shouldGLRender(action))
	return;

    SoState *state = action->getState();
    // Push state, just in case we decide to turn on BASE_COLOR (when
    // rendering) because we don't have enough normals:
    // ??? temp fix: also, push enables us to put vertex prop in state.
    state->push();

    SoVertexProperty* vp = (SoVertexProperty*)vertexProperty.getValue();
    if(vp) {
	vp->doAction(action);
    }

    // Figure out number of points in set
    const SoGLCoordinateElement	*ce = (const SoGLCoordinateElement *)
	SoCoordinateElement::getInstance(action->getState());
    curCoord = (int) startIndex.getValue();
    numPts = numPoints.getValue();
    if (numPts == SO_POINT_SET_USE_REST_OF_POINTS)
	numPts = ce->getNum() - curCoord;

   
    // This extra level of brackets is to make bundle destructors get
    // called before state->pop() is called:
    {
	materialPerPoint = areMaterialsPerPoint(action);
	normalPerPoint   = areNormalsPerPoint(action);

	// Test for auto-normal case; since this modifies an element this
	// MUST BE DONE BEFORE ANY BUNDLES ARE CREATED!
	const SoGLNormalElement *ne = (const SoGLNormalElement *)
	    SoGLNormalElement::getInstance(state);

	if (ne->getNum() == 0) {
	    SoLightModelElement::set(state,
				     SoLightModelElement::BASE_COLOR);
	    normalPerPoint = FALSE;
	}

	SoMaterialBundle		mb(action);
	SoTextureCoordinateBundle	tcb(action, TRUE);

	// Make sure first material and normal are sent if necessary
	mb.sendFirst();
	if (mb.isColorOnly())
	    normalPerPoint = FALSE;
	if (! mb.isColorOnly() && ! normalPerPoint && ne->getNum() > 0)
	    ne->send(curCoord);

        // Get the complexity element and decide how points will be skipped
        // during rendering.
        float cmplxValue = SoComplexityElement::get(state);
        float delta = 1.8 * (0.5 - ((cmplxValue < 0.5) ? cmplxValue : 0.5));

	glBegin(GL_POINTS);

        float fraction = 0.0;
	for (i = 0; i < numPts; i++, fraction += delta) {

            // Check to see if this point should be skipped due to complexity
            if (fraction >= 1.0) {
                fraction -= 1.0;
                curCoord++;
                continue;
            }

	    // Send next material, normal, and texture coordinate if necessary
	    if (materialPerPoint && i > 0)
		mb.send(curCoord, TRUE);
	    if (normalPerPoint)
		ne->send(curCoord);
	    if (tcb.needCoordinates())
		tcb.send(curCoord);

	    // Send coordinate
	    ce->send(curCoord);

	    curCoord++;
	}
	glEnd();
    }
    // Restore Normal or LightModel element back to what they were.
    state->pop();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Generates points representing a point set.
//
// Use: protected

void
SoPointSet::generatePrimitives(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // When generating primitives for picking, delay computing default
    // texture coordinates
    SbBool forPicking = action->isOfType(SoRayPickAction::getClassTypeId());

    SbBool			materialPerPoint, normalPerPoint;
    int32_t			numPts;
    int				curCoord, i;
    SoPrimitiveVertex		pv;
    SoPointDetail		detail;

    // Push state, just in case we decide to set the NormalElement
    // because we're doing auto-normal generation.
    SoState *state = action->getState();
    state->push();
  
    // Put vertexProperty stuff into state:
    SoVertexProperty *vp = (SoVertexProperty*)vertexProperty.getValue();
    if (vp) {
	vp->doAction(action);
    }

    // This extra level of brackets is to make bundle constructors get
    // called before state->pop() is called:
    {
	const SoGLCoordinateElement	*ce = (const SoGLCoordinateElement *)
	    SoCoordinateElement::getInstance(action->getState());

	// Figure out number of points in set
	curCoord = (int) startIndex.getValue();
	numPts = numPoints.getValue();
	if (numPts == SO_POINT_SET_USE_REST_OF_POINTS)
	    numPts = ce->getNum() - curCoord;

	materialPerPoint = areMaterialsPerPoint(action);
	normalPerPoint   = areNormalsPerPoint(action);

	// Test for auto-normal case; since this modifies an element this
	// MUST BE DONE BEFORE ANY BUNDLES ARE CREATED!
	const SoNormalElement *ne = SoNormalElement::getInstance(state);
	if (ne->getNum() == 0) {
	    normalPerPoint = FALSE;
	}

	if (forPicking)
	    pv.setTextureCoords(SbVec4f(0.0, 0.0, 0.0, 0.0));

	pv.setDetail(&detail);

	SoTextureCoordinateBundle	tcb(action, FALSE, ! forPicking);

	pv.setMaterialIndex(curCoord);
	detail.setMaterialIndex(curCoord);

	if (! normalPerPoint) {
	    if (ne->getNum() == 0) pv.setNormal(SbVec3f(0,0,0));
	    else pv.setNormal(ne->get(0));
	    detail.setNormalIndex(0);
	}

        // Get the complexity element and decide how points will be skipped
	// during processing; note that we don't want to skip anything
	// when picking.
        float cmplxValue = SoComplexityElement::get(action->getState());
        float delta = 1.8 * (0.5 - ((cmplxValue < 0.5) ? cmplxValue : 0.5));
        float fraction = 0.0;
	if (forPicking)
	    delta = 0.0;

	for (i = 0; i < numPts; i++, fraction += delta) {

            // Check to see if this point should be skipped due to complexity
            if (fraction >= 1.0) {
                fraction -= 1.0;
                curCoord++;
                continue;
            }

	    // Set coordinates, normal, and texture coordinates in
	    // detail

	    pv.setPoint(ce->get3(curCoord));
	    detail.setCoordinateIndex(curCoord);
	    if (normalPerPoint) {
		pv.setNormal(ne->get(curCoord));
		detail.setNormalIndex(curCoord);
	    }
	    if (materialPerPoint) {
		pv.setMaterialIndex(curCoord);
		detail.setMaterialIndex(curCoord);
	    }
	    if (tcb.isFunction()) {
		if (! forPicking)
		    pv.setTextureCoords(tcb.get(pv.getPoint(),
						pv.getNormal()));
		detail.setTextureCoordIndex(0);
	    }
	    else {
		pv.setTextureCoords(tcb.get(curCoord));
		detail.setTextureCoordIndex(curCoord);
	    }

	    // Generate a point primitive
	    invokePointCallbacks(action, &pv);

	    curCoord++;
	}

    }
    state->pop();     // Restore NormalElement
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Redefines this to tell open caches that they contain points.
//
// Use: extender

void
SoPointSet::getBoundingBox(SoGetBoundingBoxAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // Let our parent class do the real work
    SoNonIndexedShape::getBoundingBox(action);

    // If there are any open bounding box caches, tell them that they
    // contain points
    SoBoundingBoxCache::setHasLinesOrPoints(action->getState());
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Computes bounding box of vertices of point set.
//
// Use: protected

void
SoPointSet::computeBBox(SoAction *action, SbBox3f &box, SbVec3f &center)
//
////////////////////////////////////////////////////////////////////////
{
    // Call the method on SoNonIndexedShape that computes the bounding
    // box and center of the given number of coordinates
    computeCoordBBox(action, (int) numPoints.getValue(), box, center);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides standard method to create an SoPointDetail instance
//    representing a picked intersection with a vertex of a point set.
//
// Use: protected, virtual

SoDetail *
SoPointSet::createPointDetail(SoRayPickAction *action,
			      const SoPrimitiveVertex *v,
			      SoPickedPoint *pp)
//
////////////////////////////////////////////////////////////////////////
{
    SoPointDetail	*detail = new SoPointDetail;

    *detail = *((const SoPointDetail *) v->getDetail());

    // Compute texture coordinates at intersection point and store it
    // in the picked point
    SoTextureCoordinateBundle	tcb(action, FALSE);
    if (tcb.isFunction())
	pp->setObjectTextureCoords(tcb.get(pp->getObjectPoint(),
					   pp->getObjectNormal()));

    return detail;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if materials are bound to individual points.
//
// Use: private

SbBool
SoPointSet::areMaterialsPerPoint(SoAction *action) const
//
////////////////////////////////////////////////////////////////////////
{
    if (SoMaterialBindingElement::get(action->getState()) ==
	SoMaterialBindingElement::OVERALL)
	return FALSE;

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if normals are bound to individual points.
//
// Use: private

SbBool
SoPointSet::areNormalsPerPoint(SoAction *action) const
//
////////////////////////////////////////////////////////////////////////
{
    if (SoNormalBindingElement::get(action->getState())
      == SoNormalBindingElement::OVERALL)
	return FALSE;

    return TRUE;
}
