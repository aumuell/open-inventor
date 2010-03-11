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
 |	SbSpherePlaneProjector
 |
 |   Author(s)		: Howard Look
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/projectors/SbSpherePlaneProjector.h>
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

SbSpherePlaneProjector::SbSpherePlaneProjector(
    float tol,
    SbBool orient)
: SbSphereSectionProjector(tol, orient)
{
}

SbSpherePlaneProjector::SbSpherePlaneProjector(
    const SbSphere &s,
    float tol,
    SbBool orient)
: SbSphereSectionProjector(s, tol, orient)
{
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
SbSpherePlaneProjector::copy() const
//
////////////////////////////////////////////////////////////////////////
{
    SbSpherePlaneProjector *newProjector = new SbSpherePlaneProjector;

    (*newProjector) = (*this);

    return newProjector;
}
SbVec3f
SbSpherePlaneProjector::project(const SbVec2f &point)
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f result;
    SbLine workingLine = getWorkingLine(point);

    if (needSetup)
	setupTolerance();

    SbVec3f planeIntersection;
    if (! tolPlane.intersect(workingLine, planeIntersection))
#ifdef DEBUG
	SoDebugError::post("SbSpherePlaneProjector::project",
			   "Couldn't intersect working line with plane");
#else
	/* Do nothing */;
#endif

    SbVec3f frontIntersect, rearIntersect, sphereIntersection;

    if (! sphere.intersect(workingLine, frontIntersect, rearIntersect)) {
	// missed the sphere, so hit the plane
	result = planeIntersection;
    }
    else {
	if ( intersectFront == TRUE )
	    sphereIntersection = frontIntersect;
	else 
	    sphereIntersection = rearIntersect;

	// See if the hit on the sphere is within tolerance.
	// Project it onto the plane, and find the distance to
	// the planeLine.

	SbLine projectLine(sphereIntersection, sphereIntersection + planeDir);
	SbVec3f projectIntersection;
	if (! tolPlane.intersect(projectLine, projectIntersection))
#ifdef DEBUG
	    SoDebugError::post("SbSpherePlaneProjector::project",
			       "Couldn't intersect working line with plane");
#else
	/* Do nothing */;
#endif

	float dist = (planeIntersection - planePoint).length();
	
	if (dist < tolDist)
	    result = sphereIntersection;
	else
	    result = planeIntersection;
    }
    
    lastPoint = result;
    return result;
}

SbRotation
SbSpherePlaneProjector::getRotation(const SbVec3f &p1, const SbVec3f &p2)
{
    SbBool tol1 = isWithinTolerance(p1);
    SbBool tol2 = isWithinTolerance(p2);
    return getRotation(p1, tol1, p2, tol2);
}

SbRotation
SbSpherePlaneProjector::getRotation(
    const SbVec3f &p1, SbBool tol1,
    const SbVec3f &p2, SbBool tol2)
    
{
    if (tol1 && tol2) {
	// both points in tolerance, rotate about
	// sphere center
	
	return SbRotation(
	    p1 - sphere.getCenter(),
	    p2 - sphere.getCenter());
    }
    else if (!tol1 && !tol2) {
	// ??? Code copied from SphereSheet::getRotation
	
	// both points out of tolerance, rotate
	// as if string drawn from p1 to p2

	SbVec3f diff = p2 - p1;
	float d = diff.length();
	
	float angle = (sphere.getRadius()==0.0) 
		      ? 0 : (d / sphere.getRadius());

	SbVec3f axis = planeDir.cross(diff);
	axis.normalize();

	return SbRotation(axis, angle);
    }
    else {
	// one point in, one point out, so rotate about
	// the center of the sphere from the point on the
	// sphere to the intersection of the plane and the
	// sphere closest to the point off the sphere

	SbLine planeLine;

	// find the line that lies in the tolerance
	// plane and goes to the point off the sphere
	if (tol1)
	    planeLine.setValue(planePoint, p2);
	else
	    planeLine.setValue(planePoint, p1);

	// find the point on the sphere along that line
	SbVec3f intersection;
	if (! sphere.intersect(planeLine, intersection))
#ifdef DEBUG
	    SoDebugError::post("SbSpherePlaneProjector::getRotation",
			       "Couldn't intersect plane line with sphere");
#else
	/* Do nothing */;
#endif

	if (tol1) {
	    // went off sphere
	    return
		getRotation(p1, TRUE, intersection, TRUE) *
		getRotation(intersection, FALSE, p2, FALSE);
	}
	else {
	    // came on to sphere
	    // "Hey cutie. You've got quite a radius..."
	    return
		getRotation(p1, FALSE, intersection, FALSE) *
		getRotation(intersection, TRUE, p2, TRUE);
	}
    }

}
