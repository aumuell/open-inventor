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
 |	SbCylinderPlaneProjector
 |
 |   Author(s)		: Howard Look
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/projectors/SbCylinderPlaneProjector.h>
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

SbCylinderPlaneProjector::SbCylinderPlaneProjector(
    float tol,
    SbBool orient)
: SbCylinderSectionProjector(tol,orient)
{
}

SbCylinderPlaneProjector::SbCylinderPlaneProjector(
    const SbCylinder &c,
    float,
    SbBool orient)
: SbCylinderSectionProjector(c, orient)
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
SbCylinderPlaneProjector::copy() const
//
////////////////////////////////////////////////////////////////////////
{
    SbCylinderPlaneProjector *newProjector = new SbCylinderPlaneProjector;

    (*newProjector) = (*this);

    return newProjector;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Project the mouse position onto a point on this cylinder.
//
// Use: public

SbVec3f
SbCylinderPlaneProjector::project(const SbVec2f &point)
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
	SoDebugError::post("SbCylinderPlaneProjector::project",
			   "Couldn't intersect working line with plane");
#else
	/* Do nothing */;
#endif
    
    SbVec3f cylIntersection, dontCare;
    SbBool hitCyl;
    if ( intersectFront == TRUE )
	hitCyl = cylinder.intersect(workingLine, cylIntersection, dontCare);
    else
	hitCyl = cylinder.intersect(workingLine, dontCare, cylIntersection);

    if (hitCyl == FALSE ) {
	// missed the cylinder, so hit the plane
	result = planeIntersection;
    }
    else {
	// See if the hit on the cylinder is within tolerance.
	// Project it onto the plane, and find the distance to
	// the planeLine.

	SbLine projectLine(cylIntersection, cylIntersection + planeDir);
	SbVec3f projectIntersection;
	if (! tolPlane.intersect(projectLine, projectIntersection))
#ifdef DEBUG
	    SoDebugError::post("SbCylinderPlaneProjector::project",
			       "Couldn't intersect working line with plane");
#else
	/* Do nothing */;
#endif

	SbVec3f vecToPoint = projectIntersection - 
			     planeLine.getClosestPoint(projectIntersection);
	float dist = vecToPoint.length();
	
	if (dist < tolDist)
	    result = cylIntersection;
	else
	    result = planeIntersection;
    }

    lastPoint = result;
    return result;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//  Get a rotation based on two points on this projector.
//  Rotations are _always_ about the axis of the cylinder.
//
// Use: virtual protected
//
////////////////////////////////////////////////////////////////////////

SbRotation
SbCylinderPlaneProjector::getRotation(const SbVec3f &p1, const SbVec3f &p2)
{
    SbBool tol1 = isWithinTolerance(p1);
    SbBool tol2 = isWithinTolerance(p2);
    return getRotation(p1, tol1, p2, tol2);
}

SbRotation
SbCylinderPlaneProjector::getRotation(
    const SbVec3f &p1, SbBool tol1,
    const SbVec3f &p2, SbBool tol2)
    
{
    if (tol1 && tol2) {
	// Both points in tolerance, rotate about
	// cylinder's axis.

	// Find perpendiculars to from cylinder's axis to each
	// point.
	SbVec3f v1 = p1 - cylinder.getAxis().getClosestPoint(p1);
	SbVec3f v2 = p2 - cylinder.getAxis().getClosestPoint(p2);
	
	float cosAngle = v1.dot(v2)/(v1.length()*v2.length());
	
	// prevent numerical instability problems
	if ((cosAngle > 1.0) || (cosAngle < -1.0))
	    return SbRotation::identity();
	    
	float angle = acosf(cosAngle);

	// This will either be the same as the cylinder's
	// axis, or the same but with direction reversed
	SbVec3f rotAxis = v1.cross(v2);
	
	return SbRotation(rotAxis, angle);
    }
    else if (!tol1 && !tol2) {
	SbVec3f v1 = p1 - planeLine.getClosestPoint(p1);
	SbVec3f v2 = p2 - planeLine.getClosestPoint(p2);
	if ( v1.dot( v2 ) < 0.0 ) {
	    // points are on opposite sides of the cylinder.
	    // Break this rotation up into 3 parts.
	    // [1] p1 to ptOnCylP1Side,
	    // [2] ptOnCylP1Side to ptOnCylP2Side,
	    // [3] ptOnCylP2Side to p2.

	    // Find the points on planeLine that are closest to p1 and p2
	    SbVec3f linePtNearestP1 = planeLine.getClosestPoint(p1);
	    SbVec3f linePtNearestP2 = planeLine.getClosestPoint(p2);

	    // Find the directions that go from the points in the line towards
	    // p1 and p2
	    SbVec3f dirToP1 = p1 - linePtNearestP1;
	    SbVec3f dirToP2 = p2 - linePtNearestP2;
	    dirToP1.normalize();
	    dirToP2.normalize();

	    // Find the points on the cylinder nearest p1 and p2.
	    SbVec3f ptOnCylP1Side = linePtNearestP1 + dirToP1 * tolDist;
	    SbVec3f ptOnCylP2Side = linePtNearestP2 + dirToP2 * tolDist;

	    return
		getRotation(p1, FALSE, ptOnCylP1Side, FALSE) *
		getRotation(ptOnCylP1Side, TRUE, ptOnCylP2Side, TRUE) *
		getRotation(ptOnCylP2Side, FALSE, p2, FALSE);
	}
	else {
	    // Points are on same side of the cylinder.
	    // Rotate from one to the other, and only
	    // keep portion perpendicular to the cylinder's axis.

	    SbVec3f diff = v2 - v1;
	    
	    float d = diff.length();
	    
	    // moving a distance of 2*PI*radius is equivalent to 
	    // rotating through an angle of 2*PI.
	    // So,  (d / 2*PI*radius) = (angle / 2*PI);
	    // angle = d / radius

	    float angle = (cylinder.getRadius()==0.0) 
			  ? 0 : (d / cylinder.getRadius());

	    SbVec3f rotAxis = planeDir.cross(v1);
	    
	    // Moving towards or moving away from cylinder.
	    if (v2.length() > v1.length())
		return SbRotation(rotAxis, angle);
	    else
		return SbRotation(rotAxis, -angle);
	}
    }
    else {
	// One point in tolerance, one point out of tolerance.
	// (Pretend that the two points lie on a line
	// that is perpendicular to the axis of the cyl.)
	// Find the point on both the plane and cylinder that
	// is on that line.
	// Rotate twice:
	// (1) from the point on the cylinder to intersection
	// (2) from intersection to point off cylinder

	// offCylinderPt is the one that isn't within tolerance
	SbVec3f offCylinderPt = (tol1) ? p2 : p1;

	// Find point on planeLine closest to offCylinderPt
	SbVec3f linePtNearest = planeLine.getClosestPoint(offCylinderPt);

	// Find direction that goes from linePtNearest towards offCylinderPt
	SbVec3f dirToOffCylinderPt = offCylinderPt - linePtNearest;
	dirToOffCylinderPt.normalize();

	// Find point on the cylinder nearest offCylinderPt
	SbVec3f ptOnCylinder = linePtNearest + dirToOffCylinderPt * tolDist;

	if (tol1) {

	    // p1 is on cyl, p2 off - went off cylinder
	    return
		getRotation(p1, TRUE, ptOnCylinder, TRUE) *
		getRotation(ptOnCylinder, FALSE, p2, FALSE);
	}
	else {

	    // p1 is off cyl, p2 on - came on to cylinder
	    return
		getRotation(p1, FALSE, ptOnCylinder, FALSE) *
		getRotation(ptOnCylinder, TRUE, p2, TRUE);
	}
    }

}
