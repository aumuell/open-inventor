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
 * Copyright (C) 1990,91,92   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |	SbSphereSectionProjector
 |
 |   Author(s)		: Howard Look
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/projectors/SbSphereSectionProjector.h>
#include <Inventor/errors/SoDebugError.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructors
//
// Use: public
//
////////////////////////////////////////////////////////////////////////

SbSphereSectionProjector::SbSphereSectionProjector(
    float tol,
    SbBool orient)
: SbSphereProjector(orient)
{
    setTolerance(tol);
    setRadialFactor(0.0);
}

SbSphereSectionProjector::SbSphereSectionProjector(
    const SbSphere &s,
    float tol,
    SbBool orient)
: SbSphereProjector(s, orient)
{
    setTolerance(tol);
    setRadialFactor(0.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns an instance that is a copy of this instance. The caller
//    is responsible for deleting the copy when done.
//
// Use: public, virtual
//

SbProjector *
SbSphereSectionProjector::copy() const
//
////////////////////////////////////////////////////////////////////////
{
    SbSphereSectionProjector *newProjector = new SbSphereSectionProjector;

    (*newProjector) = (*this);

    return newProjector;
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Project the mouse position onto a point on this sphere.
//
// Use: public

SbVec3f
SbSphereSectionProjector::project(const SbVec2f &point)
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f result;
    SbLine workingLine = getWorkingLine(point);

    if (needSetup)
	setupTolerance();

    SbVec3f planeIntersection;
    SbVec3f sphereIntersection, dontCare;

    SbBool hitSphere;

    // Depending on whether we are intersecting front or rear, we care 
    // about different arguments returned from intersect
    if ( intersectFront )
	hitSphere = sphere.intersect(workingLine, sphereIntersection, dontCare);
    else 
	hitSphere = sphere.intersect(workingLine, dontCare, sphereIntersection);

    if (hitSphere) {
	// drop the sphere intersection onto the tolerance plane

	SbLine projectLine(sphereIntersection, sphereIntersection + planeDir);
	if (! tolPlane.intersect(projectLine, planeIntersection))
#ifdef DEBUG
	    SoDebugError::post("SbSphereSectionProjector::project",
			       "Couldn't intersect working line with plane");
#else
	/* Do nothing */;
#endif
    }
    else if (! tolPlane.intersect(workingLine, planeIntersection))
#ifdef DEBUG
	SoDebugError::post("SbSphereSectionProjector::project",
			   "Couldn't intersect working line with plane");
#else
	/* Do nothing */;
#endif

    float dist = (planeIntersection - planePoint).length();

    if (dist < tolDist) {
#ifdef DEBUG
	if (! hitSphere)
	    SoDebugError::post("SbSphereSectionProjector::project",
			       "Couldn't intersect with sphere");
#endif
	result = sphereIntersection;
    }
    else {
	result = planeIntersection;
    }

    lastPoint = result;
    return result;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
// Set the edge tolerance, 0.0 - 1.0.
//
// Use: public

void
SbSphereSectionProjector::setTolerance(float t)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (t < 0.0) {
	SoDebugError::post("SbSphereSectionProjector::setEdgeTolerance",
			   "Tolerance cannot be less than 0.0");
	t = 0.0;
    }
    else if (t > 1.0) {
	SoDebugError::post("SbSphereSectionProjector::setEdgeTolerance",
			   "Tolerance cannot be greater than 1.0");
	t = 1.0;
    }
#endif

    tolerance = t;
    needSetup = TRUE;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
// Find if this point on the sphere is within the tolerance
// ring.
//
// Use: virtual public

SbBool
SbSphereSectionProjector::isWithinTolerance(const SbVec3f &point)
//
////////////////////////////////////////////////////////////////////////
{
    if (needSetup)
	setupTolerance();

    // Drop a perpendicular from the point to the tolerance
    // plane
    SbVec3f planeIntersection;
    SbLine line(point, point + planeDir);

    if (! tolPlane.intersect(line, planeIntersection)) {
#ifdef DEBUG
	SoDebugError::post("SbSphereSectionProjector::isWithinTolerance",
			   "Couldn't intersect with plane");
#else
	/* Do nothing */;
#endif
	return FALSE;
    }

    float dist = (planeIntersection - planePoint).length();

    // Need to give a little slack to allow for fp precision
    // Err on the side of returning this point out of tolerance
    // so that it is easier to get pure roll.
    // ??? Seems like a lot of fudge, doesn't it? Perhaps this
    // should relate to the radius of the sphere, or be
    // a percentage of the tolerance.
    return (dist < (tolDist - .001));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  Sets up the tolerance plane.
//
// Use: protected
void
SbSphereSectionProjector::setupTolerance()
//
////////////////////////////////////////////////////////////////////////
{
    // Find the intersection point on the tolerance plane.
    // The plane may either be in working space, or always
    // oriented towards the eye.

    // find orientation of the tolerance plane, in working space
    if (orientToEye) {
	if (viewVol.getProjectionType() == SbViewVolume::PERSPECTIVE) {
	    // find the projection point in working space coords
	    SbVec3f workingProjPoint;
	    worldToWorking.multVecMatrix(
		viewVol.getProjectionPoint(), workingProjPoint);

	    planeDir = workingProjPoint - sphere.getCenter();
	}
	else {
	    // Use the projection direction in an orthographic
	    // view vol
	    worldToWorking.multDirMatrix(viewVol.zVector(), planeDir);
	}	 
	planeDir.normalize();
    }
    else {
	planeDir.setValue(0,0,1);
    }

    // distance from planePoint to edge of tolerance ring
    tolDist = sphere.getRadius() * tolerance;

    // find disntance from the center of the sphere to the tolerance
    // plane
    planeDist =
	sqrtf((sphere.getRadius()*sphere.getRadius()) -
	      (tolDist * tolDist));

    // If we are intersecting with the back half of the sphere, then
    // face the plane the other way.
    if ( intersectFront == FALSE )
	planeDir *= -1.0;

    // plane given direction and point to pass through
    planePoint = sphere.getCenter() + planeDist*planeDir;

    tolPlane = SbPlane(planeDir, planePoint);

    needSetup = FALSE;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//  Get a rotation based on two points on this projector.
//
// Use: public

SbRotation
SbSphereSectionProjector::getRotation(const SbVec3f &p1, const SbVec3f &p2)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool tol1 = isWithinTolerance(p1);
    SbBool tol2 = isWithinTolerance(p2);

    if (tol1 && tol2) {
	// both points in tolerance, rotate about
	// sphere center

	return SbRotation(
	    p1 - sphere.getCenter(),
	    p2 - sphere.getCenter());
    }
    else if (!tol1 && !tol2) {
	// both points out of tolerance, rotate about
	// plane point

	// Would like to just use this:
	SbRotation badRot = SbRotation(p1 - planePoint, p2 - planePoint);
	// but fp instablity gives back a goofy axis, so we don't get
	// pure roll.

	// So we need to snap the axis to be parallel to plane dir
	SbVec3f badAxis; float goodAngle;
	badRot.getValue(badAxis, goodAngle);

	SbVec3f goodAxis;
	if (badAxis.dot(planeDir) > 0.0)
	    goodAxis = planeDir;
	else 	    
	    goodAxis = -planeDir;

	SbRotation rollRot(goodAxis, goodAngle);

	//Now find rotation in the direction perpendicular to this:
	SbVec3f diff1 = p1 - planePoint;
	SbVec3f diff2 = p2 - planePoint;
	float d = diff2.length() - diff1.length();

	// Check for degenerate cases
	float theta = d / sphere.getRadius();
	if ( fabs(theta) < 0.000001 || fabs(theta) > 1.0 )
	    return rollRot;

	diff1.normalize();
	SbVec3f pullAxis = planeDir.cross( diff1 );
	pullAxis.normalize();
	SbRotation pullRot(pullAxis, getRadialFactor() * theta );

	SbRotation totalRot = rollRot * pullRot;
	return totalRot;

    }
    else {
	// one point in, one point out, so rotate about
	// the center of the sphere from the point on the
	// sphere to the intersection of the plane and the
	// sphere closest to the point off the sphere

	SbLine planeLine;
	SbVec3f intersection;

	if (tol1) {
	    planeLine.setValue(planePoint, p2);
	}
	else {
	    planeLine.setValue(planePoint, p1);
	}

	if (! sphere.intersect(planeLine, intersection))
#ifdef DEBUG
	    SoDebugError::post("SbSphereSectionProjector::getRotation",
			       "Couldn't intersect plane line with sphere");
#else
	/* Do nothing */;
#endif

	if (tol1) {
	    // went off sphere
	    return SbRotation(
		p1 - sphere.getCenter(),
		intersection - sphere.getCenter());
	}
	else {
	    // came on to sphere
	    // "Hey cutie. You've got quite a radius..."
	    return SbRotation(
		intersection - sphere.getCenter(),
		p2 - sphere.getCenter());
	}
    }

}
