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
 |	SoShape
 |
 |   Author(s)		: Paul S. Strauss, Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <GL/gl.h>
#include <GL/glu.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/bundles/SoMaterialBundle.h>
#include <Inventor/details/SoDetail.h>
#include <Inventor/details/SoFaceDetail.h>
#include <Inventor/details/SoPointDetail.h>
#include <Inventor/elements/SoComplexityElement.h>
#include <Inventor/elements/SoComplexityTypeElement.h>
#include <Inventor/elements/SoDrawStyleElement.h>
#include <Inventor/elements/SoGLTextureEnabledElement.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoPickStyleElement.h>
#include <Inventor/elements/SoProjectionMatrixElement.h>
#include <Inventor/elements/SoShapeHintsElement.h>
#include <Inventor/elements/SoTextureCoordinateElement.h>
#include <Inventor/elements/SoViewingMatrixElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoShape.h>

SO_NODE_ABSTRACT_SOURCE(SoShape);

// This is used as a surrogate object when rendering or picking a
// shape whose complexity is set to BOUNDING_BOX.
SoCube			*SoShape::bboxCube = NULL;

// Used during generatePrimitives:
SbBool			SoShape::sendTexCoords = 0;
SoMaterialBundle	*SoShape::matlBundle = NULL;

// Used while tesellating non-convex polygons
SoShape::TriangleShape	SoShape::primShapeType;
SoFaceDetail		*SoShape::faceDetail = NULL;
int			SoShape::nestLevel = 0;
SoAction		*SoShape::primAction = NULL;
int			SoShape::primVertNum = 0;
int			SoShape::polyVertNum = 0;
SoShape			*SoShape::primShape = NULL;
SoPrimitiveVertex	*SoShape::primVerts = NULL;
SoPointDetail		*SoShape::vertDetails = NULL;
SoPrimitiveVertex	*SoShape::polyVerts = NULL;
SoPointDetail		*SoShape::polyDetails = NULL;
int			SoShape::numPolyVertsAllocated = 0;
gluTESSELATOR		*SoShape::tobj = NULL;

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides method in SoNode to return FALSE.
//
// Use: public

SbBool
SoShape::affectsState() const
//
////////////////////////////////////////////////////////////////////////
{
    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor.
//
// Use: protected

SoShape::SoShape()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoShape);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoShape::~SoShape()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Computes bounding box for a shape, using the virtual
//    computeBBox() method.
//
// Use: extender

void
SoShape::getBoundingBox(SoGetBoundingBoxAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SbBox3f	bbox;
    SbVec3f	center;

    computeBBox(action, bbox, center);

    action->extendBy(bbox);

    // Make sure the center is first transformed by the current local
    // transformation matrix
    action->setCenter(center, TRUE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements rendering by rendering each primitive generated by
//    subclass.
//
// Use: extender

void
SoShape::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // First see if the object is visible and should be rendered now
    if (shouldGLRender(action)) {

	SoState *state = action->getState();

	//
	// Set up some info in instance that will be used during
	// rendering of generated primitives
	//

	// Send the first material and remember it was sent
	SoMaterialBundle	mb(action);
	matlBundle = &mb;
	matlBundle->sendFirst();

	// See if textures are enabled and we need to send texture coordinates
	sendTexCoords = (SoGLTextureEnabledElement::get(state));

	// Generate primitives to approximate the shape. Each
	// primitive will be rendered separately (through callbacks).
	generatePrimitives(action);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements picking along a ray by intersecting the ray with each
//    primitive generated by subclass.
//
// Use: extender

void
SoShape::rayPick(SoRayPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // First see if the object is pickable
    if (shouldRayPick(action)) {

	// Compute the picking ray in the space of the shape
	computeObjectSpaceRay(action);

	// Generate primitives to approximate the shape. Each
	// primitive will be intersected (through callbacks) with the
	// ray.
	generatePrimitives(action);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements the generation of primitives for the shape.
//
// Use: extender

void
SoShape::callback(SoCallbackAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // First see if the object should have primitives generated for it.
    if (action->shouldGeneratePrimitives(this)) {

	// Generate primitives to approximate the shape. Each primitive
        // will be sent back to the application through callbacks.
	generatePrimitives(action);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This can be used by subclasses when the complexity type is
//    SCREEN_SPACE to determine how many window pixels are covered by
//    the shape. It returns in rectSize the number of pixels in the
//    window rectangle that covers the given 3D bounding box.
//
// Use: extender, static

void
SoShape::getScreenSize(SoState *state, const SbBox3f &boundingBox,
		       SbVec2s &rectSize)
//
////////////////////////////////////////////////////////////////////////
{
    SbMatrix	objToScreen;
    SbVec2s	winSize;
    SbVec3f	min, max, screenPoint[8];
    SbBox2f	screenBox;
    int		i;

    // Get the matrices from the state to convert from object to screen space
    objToScreen = (SoModelMatrixElement::get(state) *
		   SoViewingMatrixElement::get(state) *
		   SoProjectionMatrixElement::get(state));

    // Get the size of the window from the state
    winSize = SoViewportRegionElement::get(state).getWindowSize();

    // Transform the 8 vertices of the bounding box into screen space

    boundingBox.getBounds(min, max);

    objToScreen.multVecMatrix(SbVec3f(min[0], min[1], min[2]), screenPoint[0]);
    objToScreen.multVecMatrix(SbVec3f(min[0], min[1], max[2]), screenPoint[1]);
    objToScreen.multVecMatrix(SbVec3f(min[0], max[1], min[2]), screenPoint[2]);
    objToScreen.multVecMatrix(SbVec3f(min[0], max[1], max[2]), screenPoint[3]);
    objToScreen.multVecMatrix(SbVec3f(max[0], min[1], min[2]), screenPoint[4]);
    objToScreen.multVecMatrix(SbVec3f(max[0], min[1], max[2]), screenPoint[5]);
    objToScreen.multVecMatrix(SbVec3f(max[0], max[1], min[2]), screenPoint[6]);
    objToScreen.multVecMatrix(SbVec3f(max[0], max[1], max[2]), screenPoint[7]);

    for (i = 0; i < 8; i++)
	screenBox.extendBy(SbVec2f((screenPoint[i][0] * winSize[0]),
				   (screenPoint[i][1] * winSize[1])));

    // Get the size of the resulting box
    SbVec2f boxSize;
    screenBox.getSize(boxSize[0], boxSize[1]);

    // Screen space size is actually half of this size. Test for
    // overflow and use the maximum size if necessary.
    boxSize /= 2.0;

    if (boxSize[0] > SHRT_MAX)
	rectSize[0] = SHRT_MAX;
    else if (boxSize[0] < SHRT_MIN)
	rectSize[0] = SHRT_MIN;
    else
	rectSize[0] = (short) boxSize[0];

    if (boxSize[1] > SHRT_MAX)
	rectSize[1] = SHRT_MAX;
    else if (boxSize[1] < SHRT_MIN)
	rectSize[1] = SHRT_MIN;
    else
	rectSize[1] = (short) boxSize[1];
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if the shape should be rendered now.
//
// Use: protected 

SbBool
SoShape::shouldGLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // SoNode has already checked for render abort, so don't need to
    // do it now

    // Check if the shape is invisible
    if (SoDrawStyleElement::get(action->getState()) ==
	SoDrawStyleElement::INVISIBLE)
	return FALSE;

    // If the shape is transparent and transparent objects are being
    // delayed, don't render now
    if (action->handleTransparency())
	return FALSE;

    // If the current complexity is BOUNDING_BOX, just render the
    // cuboid surrounding the shape and tell the shape to stop
    if (SoComplexityTypeElement::get(action->getState()) ==
	SoComplexityTypeElement::BOUNDING_BOX) {
	GLRenderBoundingBox(action);
	return FALSE;
    }

    // Otherwise, go ahead and render the object
    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if the shape may be picked.
//
// Use: protected, static

SbBool
SoShape::shouldRayPick(SoRayPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool	shapeShouldPick;

    switch (SoPickStyleElement::get(action->getState())) {

      case SoPickStyleElement::SHAPE:
	shapeShouldPick = TRUE;
	break;

      case SoPickStyleElement::BOUNDING_BOX:
	// Just pick the cuboid surrounding the shape
	rayPickBoundingBox(action);
	shapeShouldPick = FALSE;
	break;

      case SoPickStyleElement::UNPICKABLE:
	shapeShouldPick = FALSE;
	break;
    }

    return shapeShouldPick;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This is called by a subclass before rendering to set up shape
//    hints for a solid object.
//
// Use: protected

void
SoShape::beginSolidShape(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoState *state = action->getState();
    state->push();

    // If the current draw style is not FILLED, we don't want to turn
    // on backface culling. (It may already be on, but that's up to
    // the application to decide.)
    if (SoDrawStyleElement::get(action->getState()) !=
	SoDrawStyleElement::FILLED) {
	return;
    }

    // Turn on backface culling, using shape hints element, unless it
    // is already set up ok. Save state first if changing things
    SoShapeHintsElement::VertexOrdering	oldOrder;
    SoShapeHintsElement::ShapeType		oldShape;
    SoShapeHintsElement::FaceType		oldFace;

    SoShapeHintsElement::get(state, oldOrder, oldShape, oldFace);

    if (oldOrder != SoShapeHintsElement::COUNTERCLOCKWISE ||
	oldShape != SoShapeHintsElement::SOLID) {
	SoShapeHintsElement::set(state,
				 SoShapeHintsElement::COUNTERCLOCKWISE,
				 SoShapeHintsElement::SOLID,
				 SoShapeHintsElement::FACE_TYPE_AS_IS);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This is called by a subclass after rendering, if
//    beginSolidShape() was called beforehand.
//
// Use: protected

void
SoShape::endSolidShape(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    action->getState()->pop();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Computes a picking ray in the object space of the shape
//    instance. The picking ray is stored in the SoRayPickAction for
//    later access by the subclass.
//
// Use: protected

void
SoShape::computeObjectSpaceRay(SoRayPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    action->setObjectSpace();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Same as above, but allows subclass to specify a matrix to
//    concatenate with the current transformation matrix.
//
// Use: protected

void
SoShape::computeObjectSpaceRay(SoRayPickAction *action, const SbMatrix &matrix)
//
////////////////////////////////////////////////////////////////////////
{
    action->setObjectSpace(matrix);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This is called during picking to create a detail containing
//    extra info about a pick intersection on a triangle. The default
//    method returns NULL.
//
// Use: protected, virtual

SoDetail *
SoShape::createTriangleDetail(SoRayPickAction *,
			      const SoPrimitiveVertex *,
			      const SoPrimitiveVertex *,
			      const SoPrimitiveVertex *,
			      SoPickedPoint *)
//
////////////////////////////////////////////////////////////////////////
{
    return NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This is called during picking to create a detail containing
//    extra info about a pick intersection on a line segment. The
//    default method returns NULL.
//
// Use: protected, virtual

SoDetail *
SoShape::createLineSegmentDetail(SoRayPickAction *,
				 const SoPrimitiveVertex *,
				 const SoPrimitiveVertex *,
				 SoPickedPoint *)
//
////////////////////////////////////////////////////////////////////////
{
    return NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This is called during picking to create a detail containing
//    extra info about a pick intersection on a point. The default
//    method returns NULL.
//
// Use: protected, virtual

SoDetail *
SoShape::createPointDetail(SoRayPickAction *, const SoPrimitiveVertex *,
			   SoPickedPoint *)
//
////////////////////////////////////////////////////////////////////////
{
    return NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Invokes correct callbacks for triangle primitive generation.
//
// Use: protected

void
SoShape::invokeTriangleCallbacks(SoAction *action,
				 const SoPrimitiveVertex *v1,
				 const SoPrimitiveVertex *v2,
				 const SoPrimitiveVertex *v3)
//
////////////////////////////////////////////////////////////////////////
{
    const SoType &actionType = action->getTypeId();

    // Treat rendering and picking cases specially
    if (actionType.isDerivedFrom(SoRayPickAction::getClassTypeId()))
	rayPickTriangle((SoRayPickAction *) action, v1, v2, v3);

    else if (actionType.isDerivedFrom(SoGLRenderAction::getClassTypeId()))
	GLRenderTriangle((SoGLRenderAction *) action, v1, v2, v3);

    // Otherwise, this is invoked through the callback action, so
    // invoke the triangle callbacks.
    else {
        SoCallbackAction *cbAct = (SoCallbackAction *) action;
        cbAct->invokeTriangleCallbacks(this, v1, v2, v3);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Invokes correct callbacks for line segment primitive generation.
//
// Use: protected

void
SoShape::invokeLineSegmentCallbacks(SoAction *action,
				    const SoPrimitiveVertex *v1,
				    const SoPrimitiveVertex *v2)
//
////////////////////////////////////////////////////////////////////////
{
    const SoType &actionType = action->getTypeId();

    // Treat rendering and picking cases specially
    if (actionType.isDerivedFrom(SoRayPickAction::getClassTypeId()))
	rayPickLineSegment((SoRayPickAction *) action, v1, v2);

    else if (actionType.isDerivedFrom(SoGLRenderAction::getClassTypeId()))
	GLRenderLineSegment((SoGLRenderAction *) action, v1, v2);

    // Otherwise, this is invoked through the callback action, so
    // invoke the triangle callbacks.
    else {
        SoCallbackAction *cbAct = (SoCallbackAction *) action;
        cbAct->invokeLineSegmentCallbacks(this, v1, v2);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Invokes correct callbacks for point primitive generation.
//
// Use: protected

void
SoShape::invokePointCallbacks(SoAction *action, const SoPrimitiveVertex *v)
//
////////////////////////////////////////////////////////////////////////
{
    const SoType &actionType = action->getTypeId();

    // Treat rendering and picking cases specially
    if (actionType.isDerivedFrom(SoRayPickAction::getClassTypeId()))
	rayPickPoint((SoRayPickAction *) action, v);

    else if (actionType.isDerivedFrom(SoGLRenderAction::getClassTypeId()))
	GLRenderPoint((SoGLRenderAction *) action, v);

    // Otherwise, this is invoked through the callback action, so
    // invoke the triangle callbacks.
    else {
        SoCallbackAction *cbAct = (SoCallbackAction *) action;
        cbAct->invokePointCallbacks(this, v);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Begins a shape composed of triangles during primitive generation.
//
// Use: protected

void
SoShape::beginShape(SoAction *action, TriangleShape shapeType,
		    SoFaceDetail *_faceDetail)
//
////////////////////////////////////////////////////////////////////////
{
    if (primVerts == NULL) {
	primVerts   = new SoPrimitiveVertex[2];
	vertDetails = new SoPointDetail[2];
    }

    primShapeType = shapeType;
    primVertNum   = 0;
    primShape	  = this;
    primAction    = action;

    // Save face detail unless we are called recursively
    if (nestLevel++ == 0)
	faceDetail = _faceDetail;

    switch (shapeType) {

      case TRIANGLE_STRIP:
      case TRIANGLE_FAN:
      case TRIANGLES:
	// If the face detail is not NULL, get it ready to store the 3
	// point details for each triangle
	if (faceDetail != NULL)
	    faceDetail->setNumPoints(3);
	break;

      case POLYGON:
	{
	    SoShapeHintsElement::VertexOrdering vo;
	    SoShapeHintsElement::ShapeType st;
	    SoShapeHintsElement::FaceType ft;
	    SoShapeHintsElement::get(action->getState(), vo, st, ft);

	    if (ft == SoShapeHintsElement::CONVEX) {
		// Convex polygons can be drawn as triangle fans
		primShapeType = TRIANGLE_FAN;
		// Do the same stuff needed for TRIANGLE_FAN:
		if (faceDetail != NULL)
		    faceDetail->setNumPoints(3);
	    }
	    else polyVertNum = 0;
	}
	break;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds a vertex to a shape composed of triangles during primitive
//    generation.
//
// Use: protected

void
SoShape::shapeVertex(const SoPrimitiveVertex *v)
//
////////////////////////////////////////////////////////////////////////
{
    switch (primShapeType) {

      case TRIANGLE_STRIP:
	triangleVertex(v, primVertNum & 1);
	break;

      case TRIANGLE_FAN:
	triangleVertex(v, primVertNum == 0 ? 0 : 1);
	break;

      case TRIANGLES:
	triangleVertex(v, primVertNum == 2 ? -1 : primVertNum);
	// Reset for next triangle if processed 3 vertices
	if (primVertNum == 3)
	    primVertNum = 0;
	break;

      case POLYGON:
	// Make sure there is enough room in polyVerts array
	allocateVerts();
	polyVerts[polyVertNum] = *v;

	if (faceDetail != NULL) {

	    // Save point detail for given vertex in array
	    polyDetails[polyVertNum] =
		* (const SoPointDetail *) v->getDetail();

	    // Store pointer to point detail in saved polygon vertex
	    polyVerts[polyVertNum].setDetail(&polyDetails[polyVertNum]);
	}

	++polyVertNum;
	break;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements shapeVertex() for one of the three triangle-based types.
//
// Use: private

void
SoShape::triangleVertex(const SoPrimitiveVertex *v, int vertToReplace)
//
////////////////////////////////////////////////////////////////////////
{
    if (faceDetail == NULL) {
	// Generate a triangle if we have 3 vertices
	if (primVertNum >= 2)
	    invokeTriangleCallbacks(primAction,
				    &primVerts[0], &primVerts[1], v);
	
	// Save vertex in one of the two slots
	if (vertToReplace >= 0)
	    primVerts[vertToReplace] = *v;
    }
    
    // If face detail was supplied, set it to contain the 3 point
    // details. Make sure the primitive vertices all point to the
    // face detail.
    else {
	const SoPointDetail	*pd = (const SoPointDetail *) v->getDetail();
	SoPrimitiveVertex	pv = *v;
	
	pv.setDetail(faceDetail);
	
	// Generate a triangle if we have 3 vertices
	if (primVertNum >= 2) {
	    faceDetail->setPoint(0, &vertDetails[0]);
	    faceDetail->setPoint(1, &vertDetails[1]);
	    faceDetail->setPoint(2, pd);
	    invokeTriangleCallbacks(primAction,
				    &primVerts[0], &primVerts[1], &pv);
	}
	
	// Save vertex and details in one of the two slots
	if (vertToReplace >= 0) {
	    primVerts[vertToReplace] = pv;
	    vertDetails[vertToReplace] = *pd;
	}
    }

    primVertNum++;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    For polygons, re-allocates the polyVerts array if more space is
//    needed to hold all vertices.  We have to hold all of the
//    vertices in memory because the tesellator can't decompose
//    concave polygons until it has seen all of the vertices.
//
// Use: private

void
SoShape::allocateVerts()
//
////////////////////////////////////////////////////////////////////////
{
    // 8 vertices are allocated to begin with
    if (polyVerts == NULL) {
	polyVerts   = new SoPrimitiveVertex[8];
	polyDetails = new SoPointDetail[8];
	numPolyVertsAllocated = 8;
    }

    else {
	if (polyVertNum >= numPolyVertsAllocated) {
	    SoPrimitiveVertex *oldVerts   = polyVerts;
	    SoPointDetail     *oldDetails = polyDetails;

	    // Double storage
	    numPolyVertsAllocated = polyVertNum*2;
	    polyVerts   = new SoPrimitiveVertex[numPolyVertsAllocated];
	    polyDetails = new SoPointDetail[numPolyVertsAllocated];

	    // Copy over old vertices and details
	    for (int i = 0; i < polyVertNum; i++) {
		polyVerts[i]   = oldVerts[i];
		polyDetails[i] = oldDetails[i];
		polyVerts[i].setDetail(&polyDetails[i]);
	    }

	    // Delete old storage
	    delete [] oldVerts;
	    delete [] oldDetails;
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Ends a shape composed of triangles during primitive generation.
//
// Use: protected

void
SoShape::endShape()
//
////////////////////////////////////////////////////////////////////////
{
    int i;

    switch (primShapeType) {
      case TRIANGLE_STRIP:
      case TRIANGLE_FAN:
      case TRIANGLES:
	primVertNum = 0;
	break;

      case POLYGON:
	// Don't bother with degenerate polygons
	if (polyVertNum < 3) {
	    polyVertNum = 0;
	    break;
	}

	// Concave polygons need to be tesselated; we'll use the
	// GLU routines to do this:
	if (tobj == NULL) {
	    tobj = gluNewTess();
	    gluTessCallback(tobj, (GLenum)GLU_BEGIN,
			    (void (*)())SoShape::beginCB);
	    gluTessCallback(tobj, (GLenum)GLU_END, 
			    (void (*)())SoShape::endCB);
	    gluTessCallback(tobj, (GLenum)GLU_VERTEX, 
			    (void (*)())SoShape::vtxCB);
	    gluTessCallback(tobj, (GLenum)GLU_ERROR,
			    (void (*)())SoShape::errorCB);
	}
	gluBeginPolygon(tobj);

	for (i = 0; i < polyVertNum; i++) {
	    const SbVec3f &t = polyVerts[i].getPoint();

	    GLdouble dv[3];  // glu requires double...
	    dv[0] = t[0]; dv[1] = t[1]; dv[2] = t[2];
	    gluTessVertex(tobj, dv, (void *)&polyVerts[i]);
	}
	gluEndPolygon(tobj);

	polyVertNum = 0;
	break;
    }

    nestLevel--;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Called by the GLU tesselator when we are beginning a triangle
//    strip, fan, or set of independent triangles.
//
// Use: static, private

void
SoShape::beginCB(GLenum primType)
//
////////////////////////////////////////////////////////////////////////
{
    switch(primType) {
      case GL_TRIANGLE_STRIP:
	primShape->beginShape(primShape->primAction, TRIANGLE_STRIP);
	break;
      case GL_TRIANGLE_FAN:
	primShape->beginShape(primShape->primAction, TRIANGLE_FAN);
	break;
      case GL_TRIANGLES:
	primShape->beginShape(primShape->primAction, TRIANGLES);
	break;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Called by the GLU tesselator when we are generating primitives.
//
// Use: static, private

void
SoShape::vtxCB(void *data)
//
////////////////////////////////////////////////////////////////////////
{
    const SoPrimitiveVertex *v = (const SoPrimitiveVertex *)data;

    primShape->shapeVertex(v);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Called by the GLU tesselator when we are done with the
//    strip/fan/etc.
//
// Use: static, private

void
SoShape::endCB()
//
////////////////////////////////////////////////////////////////////////
{
    primShape->endShape();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Called by the GLU tesselator if there is an error (typically
//    because the polygons self-intersects).
//
// Use: static, private

void
SoShape::errorCB(GLenum err)
//
////////////////////////////////////////////////////////////////////////
{
    SoDebugError::post("SoShape::errorCB",
		       "GLU error: %s", gluErrorString(err));
}

//
// This macro is used by the rendering methods to follow:
//

#define RENDER_VERTEX(pv)						      \
    if (sendTexCoords)							      \
	glTexCoord4fv(pv->getTextureCoords().getValue());		      \
    matlBundle->send(pv->getMaterialIndex(), TRUE);			      \
    if (! matlBundle->isColorOnly())					      \
	glNormal3fv(pv->getNormal().getValue());			      \
    glVertex3fv(pv->getPoint().getValue())

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Renders a triangle primitive generated by a subclass.
//
// Use: private

void
SoShape::GLRenderTriangle(SoGLRenderAction *,
			  const SoPrimitiveVertex *v1,
			  const SoPrimitiveVertex *v2,
			  const SoPrimitiveVertex *v3)
//
////////////////////////////////////////////////////////////////////////
{
    glBegin(GL_TRIANGLES);

    RENDER_VERTEX(v1);
    RENDER_VERTEX(v2);
    RENDER_VERTEX(v3);

    glEnd();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Renders a line segment primitive generated by a subclass.
//
// Use: private

void
SoShape::GLRenderLineSegment(SoGLRenderAction *,
			     const SoPrimitiveVertex *v1,
			     const SoPrimitiveVertex *v2)
//
////////////////////////////////////////////////////////////////////////
{
    glBegin(GL_LINES);

    RENDER_VERTEX(v1);
    RENDER_VERTEX(v2);

    glEnd();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Renders a point primitive generated by a subclass.
//
// Use: private

void
SoShape::GLRenderPoint(SoGLRenderAction *, const SoPrimitiveVertex *v)
//
////////////////////////////////////////////////////////////////////////
{
    glBegin(GL_POINTS);

    RENDER_VERTEX(v);

    glEnd();
}

#undef RENDER_VERTEX

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Picks a triangle primitive generated by a subclass.
//
// Use: private

void
SoShape::rayPickTriangle(SoRayPickAction *action,
			 const SoPrimitiveVertex *v1,
			 const SoPrimitiveVertex *v2,
			 const SoPrimitiveVertex *v3)
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f		point;
    SbVec3f		barycentric;
    SbBool		onFrontSide;
    SoPickedPoint	*pp;

    if (action->intersect(v1->getPoint(), v2->getPoint(), v3->getPoint(),
			  point, barycentric, onFrontSide) &&
	(pp = action->addIntersection(point)) != NULL) {

	SbVec3f		norm;
	SbVec4f		texCoord;
	SoDetail	*detail;

	// Compute normal by interpolating vertex normals using
	// barycentric coordinates
	norm = (v1->getNormal() * barycentric[0] +
		v2->getNormal() * barycentric[1] +
		v3->getNormal() * barycentric[2]);
	norm.normalize();
	pp->setObjectNormal(norm);

	// Compute texture coordinates the same way
	texCoord = (v1->getTextureCoords() * barycentric[0] +
		    v2->getTextureCoords() * barycentric[1] +
		    v3->getTextureCoords() * barycentric[2]);
	pp->setObjectTextureCoords(texCoord);

	// Copy material index from closest detail, since it can't
	// be interpolated
	if (barycentric[0] < barycentric[1] && barycentric[0] < barycentric[2])
	    pp->setMaterialIndex(v1->getMaterialIndex());
	else if (barycentric[1] < barycentric[2])
	    pp->setMaterialIndex(v2->getMaterialIndex());
	else
	    pp->setMaterialIndex(v3->getMaterialIndex());

	// Create a detail for the specific shape
	detail = createTriangleDetail(action, v1, v2, v3, pp);
	if (detail != NULL)
	    pp->setDetail(detail, this);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Picks a line segment primitive generated by a subclass.
//
// Use: private

void
SoShape::rayPickLineSegment(SoRayPickAction *action,
			    const SoPrimitiveVertex *v1,
			    const SoPrimitiveVertex *v2)
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f		point;
    SoPickedPoint	*pp;

    if (action->intersect(v1->getPoint(), v2->getPoint(), point) &&
	(pp = action->addIntersection(point)) != NULL) {

	float		ratioFromV1;
	SbVec3f		norm;
	SbVec4f		texCoord;
	SoDetail	*detail;

	// Compute normal by interpolating vertex normals
	ratioFromV1 = ((point - v1->getPoint()).length() /
		       (v2->getPoint() - v1->getPoint()).length());
	norm = (v1->getNormal() * (1.0 - ratioFromV1) +
		v2->getNormal() * ratioFromV1);
	norm.normalize();
	pp->setObjectNormal(norm);

	// Compute texture coordinates the same way
	texCoord = (v1->getTextureCoords() * (1.0 - ratioFromV1) +
		    v2->getTextureCoords() * ratioFromV1);
	pp->setObjectTextureCoords(texCoord);

	// Copy material index from closer detail, since it can't be
	// interpolated
	if (ratioFromV1 < 0.5)
	    pp->setMaterialIndex(v1->getMaterialIndex());
	else
	    pp->setMaterialIndex(v2->getMaterialIndex());

	// Create a detail for the specific shape
	detail = createLineSegmentDetail(action, v1, v2, pp);
	if (detail != NULL)
	    pp->setDetail(detail, this);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Picks a point primitive generated by a subclass.
//
// Use: private

void
SoShape::rayPickPoint(SoRayPickAction *action, const SoPrimitiveVertex *v)
//
////////////////////////////////////////////////////////////////////////
{
    SoPickedPoint	*pp;

    if (action->intersect(v->getPoint()) &&
	(pp = action->addIntersection(v->getPoint())) != NULL) {

	SoDetail	*detail;

	pp->setObjectNormal(v->getNormal());
	pp->setObjectTextureCoords(v->getTextureCoords());
	pp->setMaterialIndex(v->getMaterialIndex());

	// Create a detail for the specific shape
	detail = createPointDetail(action, v, pp);
	if (detail != NULL)
	    pp->setDetail(detail, this);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Applies render action to the bounding box surrounding the shape.
//    This is used to render shapes when BOUNDING_BOX complexity is on.
//
// Use: protected

void
SoShape::GLRenderBoundingBox(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // Create a surrogate cube to render, if not already done
    if (bboxCube == NULL) {
	bboxCube = new SoCube;
	bboxCube->ref();
    }

    // Compute the bounding box of the shape, using the virtual
    // computeBBox() method. By using this method (rather than by
    // applying an SoGetBoundingBoxAction), we can make sure that any
    // elements used to compute the bounding box are known to any
    // currently open caches in the render action. Otherwise, objects
    // (such as 2D text) that use extra elements to compute bounding
    // boxes would not be rendered correctly when cached.
    SbBox3f	box;
    SbVec3f	center;
    computeBBox(action, box, center);

    // Render the cube using a special method that is designed for
    // this task
    bboxCube->GLRenderBoundingBox(action, box);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Applies rayPick action to the bounding box surrounding the
//    shape. This is used to pick shapes when BOUNDING_BOX complexity
//    is on.
//
// Use: private

void
SoShape::rayPickBoundingBox(SoRayPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // Create a surrogate cube to pick if not already done
    if (bboxCube == NULL) {
	bboxCube = new SoCube;
	bboxCube->ref();
    }

    // Compute the bounding box of the shape, using the virtual
    // computeBBox() method
    SbBox3f	box;
    SbVec3f	center;
    computeBBox(action, box, center);

    // Pick the cube using a special method that is designed for
    // this task
    bboxCube->rayPickBoundingBox(action, box);
}
