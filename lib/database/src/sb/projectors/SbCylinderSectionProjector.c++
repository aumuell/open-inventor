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
 |	SbCylinderSectionProjector
 |
 |   Author(s)		: Howard Look
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/projectors/SbCylinderSectionProjector.h>
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

SbCylinderSectionProjector::SbCylinderSectionProjector(
    float tol,
    SbBool orient)
: SbCylinderProjector(orient)
{
    setTolerance(tol);
}

SbCylinderSectionProjector::SbCylinderSectionProjector(
    const SbCylinder &c,
    float tol,
    SbBool orient)
: SbCylinderProjector(c, orient)
{
    setTolerance(tol);
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
SbCylinderSectionProjector::copy() const
//
////////////////////////////////////////////////////////////////////////
{
    SbCylinderSectionProjector *newProjector = new SbCylinderSectionProjector;

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
SbCylinderSectionProjector::project(const SbVec2f &point)
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f result;
    SbLine workingLine = getWorkingLine(point);

    if (needSetup)
	setupTolerance();

    SbVec3f planeIntersection;
    SbVec3f cylIntersection, dontCare;
    SbBool hitCylinder;
    if ( intersectFront == TRUE )
	hitCylinder = cylinder.intersect(workingLine, cylIntersection,dontCare);
    else
	hitCylinder = cylinder.intersect(workingLine,dontCare, cylIntersection);

    if (hitCylinder) {
	// drop the cylinder intersection onto the tolerance plane

	SbLine projectLine(cylIntersection, cylIntersection + planeDir);
	if (! tolPlane.intersect(projectLine, planeIntersection))
#ifdef DEBUG
	    SoDebugError::post("SbCylinderSectionProjector::project",
			       "Couldn't intersect working line with plane");
#else
	/* Do nothing */;
#endif
    }
    else if (! tolPlane.intersect(workingLine, planeIntersection))
#ifdef DEBUG
	SoDebugError::post("SbCylinderSectionProjector::project",
			   "Couldn't intersect working line with plane");
#else
	/* Do nothing */;
#endif

    
    SbVec3f vecToPoint = planeIntersection - 
			 planeLine.getClosestPoint(planeIntersection);
    float dist = vecToPoint.length();
	
    if (dist < tolDist) {
#ifdef DEBUG
	if (! hitCylinder)
	    SoDebugError::post("SbCylinderSectionProjector::project",
			       "Couldn't intersect with cylinder");
#endif
	result = cylIntersection;
    }
    else {
	// get the point that is on the tolerance line
	SbVec3f tolVec = vecToPoint;
	SbVec3f axisPoint = planeIntersection - tolVec;
	tolVec.normalize();
	tolVec *= tolDist;
	result = axisPoint + tolVec;
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
SbCylinderSectionProjector::setTolerance(float t)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (t < 0.0) {
	SoDebugError::post("SbCylinderSectionProjector::setEdgeTolerance",
			   "Tolerance cannot be less than 0.0");
	t = 0.0;
    }
    else if (t > 1.0) {
	SoDebugError::post("SbCylinderSectionProjector::setEdgeTolerance",
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
//  Find if this point on the projector is within tolerance.
//
// Use: virtual public

SbBool
SbCylinderSectionProjector::isWithinTolerance(const SbVec3f &point)
//
////////////////////////////////////////////////////////////////////////
{
    if (needSetup)
	setupTolerance();

    // Drop the point onto the tolerance plane.	
    SbVec3f planeIntersection;
    SbLine projectLine(point, point + planeDir);
    if (! tolPlane.intersect(projectLine, planeIntersection))
#ifdef DEBUG
	SoDebugError::post("SbCylinderSectionProjector::isWithinTolerance",
			   "Couldn't intersect working line with plane");
#else
	/* Do nothing */;
#endif
    
    SbVec3f vecToPoint = planeIntersection - 
			 planeLine.getClosestPoint(planeIntersection);
    float dist = vecToPoint.length();
    
    if (dist < tolDist)
	return TRUE;
    else
	return FALSE;
}    

////////////////////////////////////////////////////////////////////////
//
// Description:
//  Sets up the tolerance plane.
//
// Use: protected
void
SbCylinderSectionProjector::setupTolerance()
//
////////////////////////////////////////////////////////////////////////
{
    // The plane may either be in working space, or always
    // oriented towards the eye (as best as possible). In
    // either case, the plane is always parallel to the axis
    // of the cylinder.

    // Find orientation of the tolerance plane, in working space.
    // The plane is defined by the axis vector and the
    // perpendicular to the axis and eyeDir (or z)
    SbVec3f perpDir; // perpendicular to axis and plane
    if (orientToEye) {
	SbVec3f eyeDir;
	
	if (viewVol.getProjectionType() == SbViewVolume::PERSPECTIVE) {
	    // find the projection point in working space coords
	    SbVec3f workingProjPoint;
	    worldToWorking.multVecMatrix(
		viewVol.getProjectionPoint(), workingProjPoint);
	
	    eyeDir = workingProjPoint - cylinder.getAxis().getPosition();
	}
	else {
	    // Use the projection direction in an orthographic
	    // view vol
	    worldToWorking.multDirMatrix(viewVol.zVector(), eyeDir);
	}	 

	perpDir = (cylinder.getAxis().getDirection()).cross(eyeDir);
    }
    else {
	perpDir = (cylinder.getAxis().getDirection()).cross(SbVec3f(0,0,1));
    }
    planeDir = perpDir.cross(cylinder.getAxis().getDirection());
    planeDir.normalize();

    if ( intersectFront == FALSE )
	planeDir *= -1.0;

    // distance from planePoint to edge of tolerance
    tolDist = cylinder.getRadius() * tolerance;
    
    // find disntance from the center of the cylinder to the tolerance
    // plane
    planeDist = sqrtf((cylinder.getRadius()*cylinder.getRadius()) - 
		(tolDist * tolDist));

    // plane given direction and distance to origin
    SbVec3f planePoint = planeDist*planeDir + cylinder.getAxis().getPosition();
    tolPlane = SbPlane(planeDir, planePoint);
    planeLine.setValue(planePoint,
		       planePoint + cylinder.getAxis().getDirection());
    
    needSetup = FALSE;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//  Get a rotation based on two points on this projector.
//  Rotations are _always_ about the axis of the cylinder.
//
// Use: public
//
////////////////////////////////////////////////////////////////////////

SbRotation
SbCylinderSectionProjector::getRotation(const SbVec3f &p1, const SbVec3f &p2)
{
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
