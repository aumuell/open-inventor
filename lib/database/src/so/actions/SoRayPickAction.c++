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
 |   $Revision: 1.2 $
 |
 |   Classes:
 |	SoRayPickAction
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoPickedPoint.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/elements/SoClipPlaneElement.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoPickRayElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/elements/SoViewVolumeElement.h>
#include <Inventor/errors/SoDebugError.h>

SO_ACTION_SOURCE(SoRayPickAction);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoRayPickAction::SoRayPickAction(const SbViewportRegion &viewportRegion) :
	SoPickAction(viewportRegion)
//
////////////////////////////////////////////////////////////////////////
{
    SO_ACTION_CONSTRUCTOR(SoRayPickAction);

    VPPoint.setValue(0,0); // Random point
    VPRadius = 5.0;		 // Big enough for easy line/point picking

    lineWasSet     = FALSE;
    rayWasComputed = FALSE;
    pickAll        = FALSE;
    normPointSet   = FALSE;

    clipToNear = clipToFar = TRUE;

    objToWorld.makeIdentity();
    worldToObj.makeIdentity();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: public

SoRayPickAction::~SoRayPickAction()
//
////////////////////////////////////////////////////////////////////////
{
    // The destructor for SoPickedPointList will delete all of the
    // intersection info
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the viewport-space point through which the ray passes.
//
// Use: public

void
SoRayPickAction::setPoint(const SbVec2s &viewportPoint)
//
////////////////////////////////////////////////////////////////////////
{
    VPPoint	    = viewportPoint;
    normPointSet    = FALSE;
    lineWasSet      = FALSE;
    rayWasComputed  = FALSE;

    clipToNear = clipToFar = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the viewport point in normalized coordinates.
//
// Use: public

void
SoRayPickAction::setNormalizedPoint(const SbVec2f &normPoint)
//
////////////////////////////////////////////////////////////////////////
{
    normVPPoint	    = normPoint;
    normPointSet    = TRUE;
    lineWasSet      = FALSE;
    rayWasComputed  = FALSE;

    clipToNear = clipToFar = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the radius around the point. This is used when testing the
//    ray against lines and points.
//    
// Use: public

void
SoRayPickAction::setRadius(float radiusInPixels)
//
////////////////////////////////////////////////////////////////////////
{
    VPRadius = radiusInPixels;
    lineWasSet      = FALSE;
    rayWasComputed  = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the world-space line along which to pick.
//
// Use: public

void
SoRayPickAction::setRay(const SbVec3f &start, const SbVec3f &direction,
			float nearDistance, float farDistance)
//
////////////////////////////////////////////////////////////////////////
{
    lineWasSet = TRUE;

    // If nearDistance or farDistance is negative, set up bogus
    // non-negative values to use for setting up the perspective view
    // volume. These values will be otherwise ignored.
    if (nearDistance < 0.0) {
	clipToNear = FALSE;
	nearDistance = 1.1;
    }
    else
	clipToNear = TRUE;
    if (farDistance < 0.0) {
	clipToFar = FALSE;
	farDistance = 10.0;
    }
    else
	clipToFar = TRUE;

    // Save values for later. Make the volume perspective for ease.
    worldVol.perspective(0.0, 1.0, nearDistance, farDistance);
    worldVol.translateCamera(start);
    worldVol.rotateCamera(SbRotation(SbVec3f(0.0, 0.0, -1.0), direction));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the indexed picked point from the list. index defaults to 0.
//
// Use: public

SoPickedPoint *
SoRayPickAction::getPickedPoint(int index) const
//
////////////////////////////////////////////////////////////////////////
{
    if (index >= ptList.getLength())
        return NULL;

    return ptList[index];
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    If a ray was not defined with setRay(), this causes the world
//    space pick ray to be computed from the screen space point and
//    radius, using the current view specification from the state.
//    This is typically done when a camera is encountered during
//    traversal.
//
// Use: extender

void
SoRayPickAction::computeWorldSpaceRay()
//
////////////////////////////////////////////////////////////////////////
{
    // See if the world-space line was set by the user
    if (lineWasSet)
	return;

    // Get the current viewport region and view volume from the state
    const SbViewportRegion	&vpReg   = SoViewportRegionElement::get(state);
    const SbViewVolume		&viewVol = SoViewVolumeElement::get(state);

    float			invWidth, invHeight;
    float			normRadius;

    // Figure out the radius of the pick circle in the near plane as a
    // fraction of the viewport size
    invWidth  = 1.0 / vpReg.getViewportSizePixels()[0];
    invHeight = 1.0 / vpReg.getViewportSizePixels()[1];
    normRadius = (VPRadius *
		  (invWidth >= invHeight ? invWidth : invHeight));

    // If necessary, convert the current pick point from viewport
    // coords into normalized viewport coords
    if (! normPointSet) {
	normVPPoint[0] = invWidth  * (VPPoint[0] -
				      vpReg.getViewportOriginPixels()[0]); 
	normVPPoint[1] = invHeight * (VPPoint[1] -
				      vpReg.getViewportOriginPixels()[1]);
    }

    // Narrow the camera's view volume to a small rectangle around
    // the selected point. The width and height of the rectangle
    // are twice the radius.
    worldVol = viewVol.narrow(normVPPoint[0] - normRadius,
			      normVPPoint[1] - normRadius,
			      normVPPoint[0] + normRadius,
			      normVPPoint[1] + normRadius);

    // Store the resulting volume in the state so it can be pushed/popped
    SoPickRayElement::set(state, worldVol);

    rayWasComputed = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This returns TRUE if the action has had a world space ray set or
//    computed.
//
// Use: extender

SbBool
SoRayPickAction::hasWorldSpaceRay() const
//
////////////////////////////////////////////////////////////////////////
{
    // This would be a simple matter of checking the lineWasSet and
    // rayWasComputed flags, except for one thing. Suppose a scene has
    // 2 cameras in it, under 2 different separators, S1 and S2. The
    // first camera calls computeWorldSpaceRay(), which sets
    // rayWasComputed to TRUE. Then S1 pops the state, so the first
    // camera is no longer in effect - but the flag remains. When S2
    // comes along, it calls this to see if it can do pick culling. We
    // want to make sure we don't think a camera was set in this
    // case. So we check the SoViewVolumeElement's depth to see if
    // it's > 0, indicating that a camera is active.

    SbBool ret;

    if (lineWasSet)
	ret = TRUE;

    else if (! rayWasComputed)
	ret = FALSE;

    else {
	// Get the top element from the state
	const SoElement *VVElt =
	    state->getConstElement(SoViewVolumeElement::getClassStackIndex());
	ret = (VVElt->getDepth() > 0);
    }

    return ret;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets up object space for later intersection tests.
//
// Use: extender

void
SoRayPickAction::setObjectSpace()
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    // Make sure world space ray was set up correctly
    if (! hasWorldSpaceRay()) {
	SoDebugError::post("SoRayPickAction::setObjectSpace",
			   "Camera never set up world space ray");
	return;
    }
#endif /* DEBUG */

    // Make sure matrices are up to date
    computeMatrices();

    // No matrix was passed in
    extraMatrixSet = FALSE;

    // Compute object-space picking ray
    computeObjVolAndLine();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets up object space for later intersection tests, given extra
//    object-to-world-space matrix.
//
// Use: extender

void
SoRayPickAction::setObjectSpace(const SbMatrix &matrix)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    // Make sure world space ray was set up correctly
    if (! hasWorldSpaceRay()) {
	SoDebugError::post("SoRayPickAction::setObjectSpace",
			   "Camera never set up world space ray");
	return;
    }
#endif /* DEBUG */

    // Make sure matrices are up to date
    computeMatrices();

    // Save matrix so it can be used to compute object-space angle later
    extraMatrix    = matrix.inverse();
    extraMatrixSet = TRUE;

    // Compute object-space picking ray
    computeObjVolAndLine();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Intersects object-space triangle specified by three vertices
//    with current ray. Returns success or failure. If it succeeds, it
//    returns intersection point, barycentric coordinates, and whether
//    the front side (defined by right-hand-rule) was hit.
//
// Use: extender

SbBool
SoRayPickAction::intersect(const SbVec3f &v0,
			   const SbVec3f &v1,
			   const SbVec3f &v2,
			   SbVec3f &intersection, SbVec3f &barycentric,
			   SbBool &front) const
//
////////////////////////////////////////////////////////////////////////
{
    if (! objLine.intersect(v0, v1, v2, intersection, barycentric, front))
	return FALSE;

    // Make sure intersection is between near/far bounds
    if (! isBetweenPlanes(intersection))
	return FALSE;

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Intersects object-space line specified by two vertices
//    with current ray. Returns success or failure. If it succeeds, it
//    returns intersection point.
//
// Use: extender

SbBool
SoRayPickAction::intersect(const SbVec3f &v0, const SbVec3f &v1,
			   SbVec3f &intersection) const
//
////////////////////////////////////////////////////////////////////////
{
    return (objVol.intersect(v0, v1, intersection) &&
	    isBetweenPlanes(intersection));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Intersects object-space point with current ray. Returns success
//    or failure.
//
// Use: extender

SbBool
SoRayPickAction::intersect(const SbVec3f &point) const
//
////////////////////////////////////////////////////////////////////////
{
    return (objVol.intersect(point) &&
	    isBetweenPlanes(point));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Intersects object-space bounding box with current ray. Returns
//    success or failure. If useFullViewVolume is TRUE, it intersects
//    the picking view volume with the box. Otherwise, it uses just
//    the picking ray, which is faster.
//
// Use: extender

SbBool
SoRayPickAction::intersect(const SbBox3f &box, SbBool useFullViewVolume)
//
////////////////////////////////////////////////////////////////////////
{
    // If the ray was set as a world-space line, we don't really have
    // a valid object-space view volume to test against the box. (The
    // view volume is a degenerate case - a line.) So just use the
    // object-space line to do the intersection. Also, if
    // useFullViewVolume is FALSE, use this test as well.
    if (! useFullViewVolume || lineWasSet) {
	SbVec3f enter, exit;
	if (! objLine.intersect(box, enter, exit))
	    return FALSE;

	SbVec3f worldEnter, worldExit;
	objToWorld.multVecMatrix(enter, worldEnter);
	objToWorld.multVecMatrix(exit,  worldExit);

	// If the bounding box does not lie at least partially between
	// the near and far clipping planes, there is no valid
	// intersection. Most of this is stolen from isBetweenPlanes().
	if (clipToNear || clipToFar) {

	    // See if the entry point is past the far plane
	    float t = worldVol.getProjectionDirection().dot(    
			worldEnter - worldVol.getProjectionPoint());
	    if (clipToFar && t > worldVol.nearDist + worldVol.nearToFar)
		return FALSE;

	    // See if the exit point is in front of the near plane
	    t = worldVol.getProjectionDirection().dot(    
			worldExit - worldVol.getProjectionPoint());
	    if (clipToNear && t < worldVol.nearDist)
		return FALSE;
	}

	// If we are looking for only the frontmost pick (pickAll is
	// FALSE) and we have found a previous intersection with an
	// object, we can reject the box intersection if the entry
	// point is farther than that intersection point.
	if (! pickAll && ptList.getLength() > 0 &&
	    worldVol.getProjectionDirection().dot(ptList[0]->getPoint()) <
	    worldVol.getProjectionDirection().dot(worldEnter))
	    return FALSE;

	// If we get here, the intersection is valid
	return TRUE;
    }

    else
	return objVol.intersect(box);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if the given object-space intersection point is
//    between the near and far planes of the object-space view volume,
//    as well as any clipping planes that have been defined. This test
//    can be used to determine whether the point of intersection of
//    the ray with an object is valid with respect to the clipping
//    planes.
//
// Use: extender

SbBool
SoRayPickAction::isBetweenPlanes(const SbVec3f &intersection) const
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f	centerVec;
    float	t;

    // Ok. So we actually do the test in world space. This is because
    // in object space, angles between the projection direction and
    // anything else are not preserved if there is a rotate and a scale,
    // making this test bogus. In world space, there is no such problem.
    // In world space, noone can hear you scream.

    SbVec3f worldIntersection;
    objToWorld.multVecMatrix(intersection, worldIntersection);

    // The dot product of this vector with the vector from the
    // projection point to the intersection point is the parametric
    // distance to the intersection along the direction of projection.
    t = worldVol.getProjectionDirection().dot(    
	    worldIntersection - worldVol.getProjectionPoint());

    // Test this distance against the near and far planes. If either
    // plane is disabled, don't test against it.
    if ((clipToNear && t < worldVol.nearDist) ||
	(clipToFar  && t > worldVol.nearDist + worldVol.nearToFar))
	return FALSE;

    // Test the point against each active user-defined clipping plane
    const SoClipPlaneElement *elt =
	SoClipPlaneElement::getInstance(getState());

    // Get each clipping plane in world space and test point
    for (int i = 0; i < elt->getNum(); i++)
	if (! elt->get(i, TRUE).isInHalfSpace(worldIntersection))
	    return FALSE;

    // If we've made it this far, accept the intersection point
    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds an SoPickedPoint instance representing the given object
//    space point to the current list and returns a pointer to it. If
//    pickAll is TRUE, this inserts the instance in correct sorted
//    order. If it is FALSE, it replaces the one instance in the list
//    only if the new one is closer; if the new one is farther away,
//    no instance is created and NULL is returned, meaning that no
//    more work has to be done to set up the SoPickedPoint.
//
// Use: extender

SoPickedPoint *
SoRayPickAction::addIntersection(const SbVec3f &objectSpacePoint)
//
////////////////////////////////////////////////////////////////////////
{
    SoPickedPoint *pp = new SoPickedPoint(getCurPath(), getState(),
					  objectSpacePoint);

    // If collecting all objects, find correct place in sorted order
    // and insert point
    if (pickAll) {
	int	i;

	for (i = 0; i < ptList.getLength(); i++)
	    if (isCloser(pp, ptList[i]))
		break;

	ptList.insert(pp, i);
    }

    // If just looking for closest object, replace first object (if
    // any) if new one is closer
    else {

	// Nothing in list yet? Add the new one.
	if (ptList.getLength() == 0)
	    ptList.append(pp);

	// New intersection is closer than the one in the list, so
	// delete the old one and insert the new one.
	else if (isCloser(pp, ptList[0]))
	    ptList.set(0, pp);

	// Point is not going to be stored in list, so get rid of it
	// now and return NULL
	else {
	    delete pp;
	    pp = NULL;
	}
    }

    return pp;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initiates action on a graph.
//
// Use: protected

void
SoRayPickAction::beginTraversal(SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
    // If someone set the ray in world space, store the world-space
    // picking ray in the state
    if (lineWasSet)
	SoPickRayElement::set(state, worldVol);

    // Clear the intersection list
    ptList.truncate(0);

    SoPickAction::beginTraversal(node);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Computes matrices to go between object and world space. Does
//    this only if they have changed from last time.
//
// Use: protected

void
SoRayPickAction::computeMatrices()
//
////////////////////////////////////////////////////////////////////////
{
    SbMatrix m = SoModelMatrixElement::get(getState());

    if (m != objToWorld) {
	objToWorld = m;
	worldToObj = m.inverse();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Computes object-space volume and line from world-space info and
//    current matrices. Stores result in objVol and objLine in instance.
//
// Use: private

void
SoRayPickAction::computeObjVolAndLine()
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f	centerPt;

    // Transform world-space view volume into object space
    objVol = SoPickRayElement::get(state);
    if (extraMatrixSet) {
	SbMatrix m = worldToObj * extraMatrix;
	objVol.transform(m);
    }
    else
	objVol.transform(worldToObj);

    // Set up object-space line for easy intersection tests. The line
    // passes through the center of the view volume's viewport.

    if (lineWasSet)
	centerPt = objVol.getProjectionPoint() +objVol.getProjectionDirection();
    else
	centerPt = objVol.llf + 0.5 * ((objVol.ulf - objVol.llf) +
				       (objVol.lrf - objVol.llf));

    if (objVol.getProjectionType() == SbViewVolume::ORTHOGRAPHIC) {
	float d = objVol.getNearDist();
	if (d == 0.0)
	    objLine.setValue(centerPt, 
			     centerPt + objVol.getProjectionDirection());
	else {
	    // Make sure the line points in the right direction
	    if (d < 0.0)
		d = -d;
	    objLine.setValue(centerPt - (d * objVol.getProjectionDirection()),
			     centerPt);
	}
    }
    else
	objLine.setValue(objVol.getProjectionPoint(), centerPt);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if the first intersection point is closer to the
//    starting point of the ray than the second.
//
// Use: private

SbBool
SoRayPickAction::isCloser(const SoPickedPoint *pp0, const SoPickedPoint *pp1)
//
////////////////////////////////////////////////////////////////////////
{
    // See which world space point is closer along the viewing
    // direction of the world-space view volume
    return (worldVol.getProjectionDirection().dot(pp0->getPoint()) <
	    worldVol.getProjectionDirection().dot(pp1->getPoint()));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Computes distance of point along ray: start * t * direction. The
//    point has to be on the ray for this to work.
//
// Use: private, static

float
SoRayPickAction::rayDistance(const SbVec3f &start, const SbVec3f &direction,
			     const SbVec3f &point)
//
////////////////////////////////////////////////////////////////////////
{
#define ABS(x) ((x) < 0.0 ? -(x) : (x))

    // Find the vector component with the maximum absolute value
    float	max, c;
    int		which;

    max = ABS(direction[0]);
    which = 0;

    c = ABS(direction[1]);
    if (c > max) {
	max = c;
	which = 1;
    }

    c = ABS(direction[2]);
    if (c > max) {
	max = c;
	which = 2;
    }

    // If all components are 0, can't do this
    if (max == 0.0)
	return 0.0;

    return (point[which] - start[which]) / direction[which];

#undef ABS
}
