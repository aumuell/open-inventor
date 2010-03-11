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
 |	SbCylinderSheetProjector
 |
 |   Author(s)		: Howard Look
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/projectors/SbCylinderSheetProjector.h>
#include <Inventor/errors/SoDebugError.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructors.
//
// Use: public
//
////////////////////////////////////////////////////////////////////////

SbCylinderSheetProjector::SbCylinderSheetProjector(SbBool orient)
: SbCylinderProjector(orient)
{
}

SbCylinderSheetProjector::SbCylinderSheetProjector(
    const SbCylinder &c,
    SbBool orient)
: SbCylinderProjector(c, orient)
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
SbCylinderSheetProjector::copy() const
//
////////////////////////////////////////////////////////////////////////
{
    SbCylinderSheetProjector *newProjector = new SbCylinderSheetProjector;

    (*newProjector) = (*this);

    return newProjector;
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Projects a mouse point to three space.
//
// Use: public

SbVec3f
SbCylinderSheetProjector::project(const SbVec2f &point)
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f result;
    SbLine workingLine = getWorkingLine(point);

    if (needSetup)
	setupPlane();

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
	    SoDebugError::post("SbCylinderSheetProjector::project",
			       "Couldn't intersect working line with plane");
#else
	/* Do nothing */;
#endif
    }
    else if (! tolPlane.intersect(workingLine, planeIntersection))
#ifdef DEBUG
	SoDebugError::post("SbCylinderSheetProjector::project",
			   "Couldn't intersect working line with plane");
#else
	/* Do nothing */;
#endif

    SbVec3f vecToPoint = planeIntersection -
			 cylinder.getAxis().getClosestPoint(planeIntersection);
    float dist = vecToPoint.length();

    // Distance on the plane from the axis to the 
    // to the projection of the point where 
    // the cylinder meets the hyperbolic sheet
    float cylSheetDist = cylinder.getRadius() * M_SQRT1_2;

    if (dist < cylSheetDist) {
#ifdef DEBUG
	if (! hitCylinder)
	    SoDebugError::post("SbCylinderSheetProjector::project",
			       "Couldn't intersect with cylinder");
#endif
	result = cylIntersection;
    }
    else {
	// Project from plane intersection to hyperbolic sheet.
	// See the SphereSheetProjector for a description
	// of this hyperbolic function.
	
	float f = cylinder.getRadius()*cylinder.getRadius()/2.0;
	float offsetDist = f / dist;
	    
	SbVec3f offset;
	if (orientToEye) {
	    
	    if (viewVol.getProjectionType() == SbViewVolume::PERSPECTIVE)
		offset = workingProjPoint - planeIntersection;
	    else
		worldToWorking.multDirMatrix(viewVol.zVector(), offset);
		
	    offset.normalize();
	}
	else {
	    offset.setValue(0,0,1);
	}

	if ( intersectFront == FALSE )
	    offset *= -1.0;
	    
	offset *= offsetDist;
	
	result = planeIntersection + offset;
    }

    lastPoint = result;
    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  Sets up the plane used for intersections.
//
// Use: protected
void
SbCylinderSheetProjector::setupPlane()
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

    // for CylinderSheetProjectors, the plane always passed
    // through the origin
    tolPlane = SbPlane(planeDir, cylinder.getAxis().getPosition());
    
    needSetup = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Finds a rotation given two points on this CylinderSheet.
//
// Use: public

SbRotation
SbCylinderSheetProjector::getRotation(const SbVec3f &p1, const SbVec3f &p2)
//
////////////////////////////////////////////////////////////////////////
{
    // Only keep portion perpendicular to the cylinder's
    // axis.
    SbVec3f v1 = p1 - cylinder.getAxis().getClosestPoint(p1);
    SbVec3f v2 = p2 - cylinder.getAxis().getClosestPoint(p2);
    SbVec3f diff = v2 - v1;
	
    float d = diff.length();
	
    // prevent numerical instability problems
    float sinHalfAngle = d / (2.0 * cylinder.getRadius());
    if (sinHalfAngle < -1.0 )
        sinHalfAngle = -1.0;
    else if (sinHalfAngle > 1.0)
        sinHalfAngle = 1.0;

    float angle = 2.0 * asin(sinHalfAngle);

    // This cross product gets flaky when the
    // angle between v1 and v2 gets small -
    // when the points are far away from the cylinder.
    SbVec3f rotAxis = v1.cross(v2);

    // Sb we'll snap it to point either with the
    // axis, or in the opposite direction.
    float cosAngle = rotAxis.dot(cylinder.getAxis().getDirection()) / 
			rotAxis.length();
			
    if (cosAngle > 0.0)
	rotAxis = cylinder.getAxis().getDirection();
    else
	rotAxis = -cylinder.getAxis().getDirection();

    return SbRotation(rotAxis, angle);
}
