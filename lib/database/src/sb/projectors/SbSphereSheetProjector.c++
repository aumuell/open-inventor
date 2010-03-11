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
 |	SbSphereSheetProjector
 |
 |   Author(s)		: Howard Look, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/projectors/SbSphereSheetProjector.h>
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

SbSphereSheetProjector::SbSphereSheetProjector(SbBool orient)
: SbSphereProjector(orient)
{
}

SbSphereSheetProjector::SbSphereSheetProjector(
    const SbSphere &s,
    SbBool orient)
: SbSphereProjector(s, orient)
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
SbSphereSheetProjector::copy() const
//
////////////////////////////////////////////////////////////////////////
{
    SbSphereSheetProjector *newProjector = new SbSphereSheetProjector;

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
SbSphereSheetProjector::project(const SbVec2f &point)
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f result;
    SbLine workingLine = getWorkingLine(point);

    if (needSetup)
	setupPlane();

    SbVec3f planeIntersection;

    SbVec3f sphereIntersection, dontCare;
    SbBool hitSphere;
    if ( intersectFront == TRUE )
	hitSphere = sphere.intersect(workingLine, sphereIntersection, dontCare);
    else
	hitSphere = sphere.intersect(workingLine, dontCare, sphereIntersection);

    if (hitSphere) {
	// drop the sphere intersection onto the tolerance plane
	
	SbLine projectLine(sphereIntersection, sphereIntersection + planeDir);
	if (! tolPlane.intersect(projectLine, planeIntersection))
#ifdef DEBUG
	    SoDebugError::post("SbSphereSheetProjector::project",
			       "Couldn't intersect working line with plane");
#else
	/* Do nothing */;
#endif
    }
    else if (! tolPlane.intersect(workingLine, planeIntersection))
#ifdef DEBUG
	SoDebugError::post("SbSphereSheetProjector::project",
			   "Couldn't intersect with plane");
#else
	/* Do nothing */;
#endif
    
    // Two possibilities:
    // (1) Intersection is on the sphere inside where the sheet
    //	    hits it
    // (2) Intersection is off sphere, or on sphere but on the sheet
    float dist = (planeIntersection - planePoint).length();
	
    // distance on the plane from the center
    // to the projection of the point where 
    // the sphere meets the hyperbolic sheet
    float sphereSheetDist = sphere.getRadius() * M_SQRT1_2;

    if (dist < sphereSheetDist) {
	// project onto sphere
#ifdef DEBUG
	if (! hitSphere)
	    SoDebugError::post("SbSphereSheetProjector::project",
			       "Couldn't intersect with sphere");
#endif
	result = sphereIntersection;
    }
    else {
	// The equation of a hyperbola in the first quadrant with the
	// axes as its limits is :  y = f * 1/x
	// At the 45 degree point of both the sphere and the
	// hyperbola, x and y will be equal, and sqrt(x^2+y^2) will
	// equal radius.  Therefore, letting x = y:
	// sqrt(2 * x^2) = r --> r^2 / 2 = x^2
	// and:
	// x = f / x --> f = x^2 = r^2 / 2
	//
	// Sb, the equation of the hyperbola is just:
	// y = r^2 / 2 * 1/x
	//
	// In terms of a sphere in working space, y
	// is the offsetDist from the plane,
	// and x is dist from the planePoint.
	
	float f = sphere.getRadius()*sphere.getRadius()/2.0;
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
SbSphereSheetProjector::setupPlane()
//
////////////////////////////////////////////////////////////////////////
{
    // Find the intersection point on the tolerance plane.
    // The plane may either be in working space, or always
    // oriented towards the eye.

    // find orientation of the tolerance plane
    if (orientToEye) {
	if (viewVol.getProjectionType() == SbViewVolume::PERSPECTIVE) {
	    // find the projection point in working space coords
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

    if ( intersectFront == FALSE )
	planeDir *= -1.0;

    // for SphereSheetProjectors, the plane always passed
    // through the origin
    planeDist = 0.0;

    // plane given direction and distance to origin
    planePoint = sphere.getCenter();
    tolPlane = SbPlane(planeDir, planePoint);
    
    needSetup = FALSE;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//   Finds a rotation given two points on this SphereSheet.
//
// Use: protected virtual

SbRotation
SbSphereSheetProjector::getRotation(const SbVec3f &p1, const SbVec3f &p2)
//
////////////////////////////////////////////////////////////////////////
{
    // Bad Method - 
    // Makes the amount of rotation falls off as you travel along
    // the hyperbolic sheet, which makes z-rotations more and more
    // dominant as you get farther away from the center of the window.
    // return SbRotation(
    //    p1 - sphere.getCenter(),
    //    p2 - sphere.getCenter());

    // Good Method -	
    // Use the axis defined by the vectors (p1, p2),
    // but use the 2D distance between the points (p1, p2) to generate
    // the angle to rotate through.  The amount to rotate will be
    // equivalent to the rotation that would occur if both p1 and p2
    // were on the ballSize-sized sphere, so the equation is:
    // sin(1/2 angle) = (1/2 d) / ballSize
    // angle = 2 * asin(d / (2 * ballSize))
    //
    // Caveat: The angle gets so small as you
    // move away from the sphere, the cross product becomes
    // unreliable. Thus this projector is only really good
    // for large trackballs.

    SbVec3f diff = p2 - p1;
    float d = diff.length();
	
    // Check for degenerate cases
    float t = d / (2.0 * sphere.getRadius());
    if (t < 0.000001) 
	return SbRotation::identity(); // Too close; no rotation
    else if (t > 1.0)
	return SbRotation::identity(); // Too much; bag it

    float angle = 2.0 * asin(t);

    SbVec3f v1(p1 - planePoint);
    SbVec3f v2(p2 - planePoint);
    SbVec3f axis = v1.cross(v2);
    axis.normalize();

    return SbRotation(axis, angle);
}
