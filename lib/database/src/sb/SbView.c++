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
 |	SbViewVolume
 |
 |   Author(s)		: Nick Thompson, Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SbLinear.h>
#include <Inventor/SbBox.h>
#include <math.h> // for M_PI_2

////////////////////////////////////////////
//
// And now, the member functions...
//
////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Yes, this should be inlineable
//
// Use: public

SbViewVolume::SbViewVolume()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the affine*projection matrix corresponding to the view volume
//
// Use: public

SbMatrix
SbViewVolume::getMatrix() const
//
////////////////////////////////////////////////////////////////////////
{
    SbMatrix affine, proj;
    getMatrices(affine, proj);

    return affine.multRight( proj );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns two matrices corresponding to the view volume.  The
//    first is a viewing matrix, which is guaranteed to be an affine
//    transformation.  The second is suitable for use as a projection
//    matrix in GL. This part about finding the projection matrix 
//    is taken from the old GT Graphics Library User's Guide, page B-3.
//
// Use: public

void
SbViewVolume::getMatrices(SbMatrix &affine, SbMatrix &proj) const
//
////////////////////////////////////////////////////////////////////////
{
    SbMatrix skewMat;

    SbVec3f right = lrfO - llfO;
    SbVec3f up = ulfO - llfO;

    float width = right.length();
    float height = up.length();

    //
    // skewMat is the matrix that would take a nice orthogonal view volume
    // that is aligned with x,y and neg-z and skew it to this view volume, 
    // based on llfO being at the origin.
    //
    skewMat[0][0] = right[0]/width;
    skewMat[0][1] = right[1]/width;
    skewMat[0][2] = right[2]/width;
    skewMat[0][3] = 0;

    skewMat[1][0] = up[0]/height;
    skewMat[1][1] = up[1]/height;
    skewMat[1][2] = up[2]/height;
    skewMat[1][3] = 0;

    skewMat[2][0] = -projDir[0];
    skewMat[2][1] = -projDir[1];
    skewMat[2][2] = -projDir[2];
    skewMat[2][3] = 0;

    skewMat[3][0] = 0;    
    skewMat[3][1] = 0;    
    skewMat[3][2] = 0;    
    skewMat[3][3] = 1;

    // Therefore, its inverse takes our probably rotated and potentially
    // skewed view volume and makes it orthogonal (unskewed) and aligned
    // with neg-z axis
    SbMatrix skewMatInv = skewMat.inverse();

    affine.setTranslate(-(llfO+projPoint));
    affine.multRight( skewMatInv );

    SbVec3f eye;
    affine.multVecMatrix(projPoint, eye);

    SbMatrix moveToEye;
    moveToEye.setTranslate(- eye);
    affine.multRight( moveToEye );

    SbVec3f llfEye, lrfEye, ulfEye;
    skewMatInv.multVecMatrix(llfO, llfEye);
    skewMatInv.multVecMatrix(lrfO, lrfEye);
    skewMatInv.multVecMatrix(ulfO, ulfEye);

    proj = SbMatrix::identity();

    // Convenient stuff for building the projection matrices
    float rightMinusLeft = lrfEye[0] - llfEye[0];
    float rightPlusLeft = lrfEye[0] + llfEye[0];

    float topMinusBottom = ulfEye[1] - llfEye[1];
    float topPlusBottom = ulfEye[1] + llfEye[1];

    const float & farMinusNear = nearToFar;
    float far = nearDist + nearToFar;
    float farPlusNear = far + nearDist;

    if (type == ORTHOGRAPHIC) {
	proj[0][0] =   2.0 / rightMinusLeft;
	proj[1][1] =   2.0 / topMinusBottom;
	proj[2][2] =  -2.0 / farMinusNear;

	proj[3][0] = - rightPlusLeft / rightMinusLeft;
	proj[3][1] = - topPlusBottom / topMinusBottom;
	proj[3][2] = - farPlusNear / farMinusNear;
    }
    else {			// type == PERSPECTIVE

	proj[0][0] = 2.0 * nearDist / rightMinusLeft;

	proj[1][1] = 2.0 * nearDist / topMinusBottom;

	proj[2][0] = rightPlusLeft / rightMinusLeft;
	proj[2][1] = topPlusBottom / topMinusBottom;
	proj[2][2] = - farPlusNear / farMinusNear;
	proj[2][3] = - 1.0;

	proj[3][2] = - 2.0 * nearDist * far / farMinusNear;
	proj[3][3] = 0.0;
    }
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns a matrix that transforms the view volume into camera
//    space: it translates the view volume so the view point is at the
//    origin, and rotates it so the view direction is along the
//    negative z axis.
//
// Use: public

SbMatrix
SbViewVolume::getCameraSpaceMatrix() const
//
////////////////////////////////////////////////////////////////////////
{
    SbMatrix	m, m2;

    // Translate projPoint to (0,0,0)
    m.setTranslate(- projPoint);

    // Rotate projDir into negative z axis
    m2.setRotate(SbRotation(projDir, SbVec3f(0.0, 0.0, -1.0)));
    m *= m2;

    return m;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//
// Use: public

void
SbViewVolume::projectPointToLine(const SbVec2f &pt,
				 SbVec3f &line0, SbVec3f &line1) const
//
////////////////////////////////////////////////////////////////////////
{
    float ptx = 2.0 * pt[0] - 1.0;
    float pty = 2.0 * pt[1] - 1.0;
    SbMatrix mat = getMatrix().inverse();
    float x, y, z, w;

    /* ptz = -1 */
    x = ptx*mat[0][0] + pty*mat[1][0] - mat[2][0] + mat[3][0];
    y = ptx*mat[0][1] + pty*mat[1][1] - mat[2][1] + mat[3][1];
    z = ptx*mat[0][2] + pty*mat[1][2] - mat[2][2] + mat[3][2];
    w = ptx*mat[0][3] + pty*mat[1][3] - mat[2][3] + mat[3][3];
    line0[0] = x/w;
    line0[1] = y/w;
    line0[2] = z/w;

    /* ptz = +1 */
    x += 2.0 * mat[2][0];
    y += 2.0 * mat[2][1];
    z += 2.0 * mat[2][2];
    w += 2.0 * mat[2][3];
    line1[0] = x/w;
    line1[1] = y/w;
    line1[2] = z/w;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//
// Use: public

void
SbViewVolume::projectPointToLine(const SbVec2f &pt, SbLine &line) const
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f p0, p1;
    projectPointToLine(pt, p0, p1);
    line.setValue(p0, p1);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  Projects a point in world coordinates to normalized screen
//  coordinates. 
//
// Use: public

void
SbViewVolume::projectToScreen(const SbVec3f &src, SbVec3f &dst) const
//
////////////////////////////////////////////////////////////////////////
{
    SbMatrix mat = getMatrix();

    mat.multVecMatrix(src, dst);

    // dst will now range from -1 to +1 in x, y, and z. Normalize this
    // to range from 0 to 1.
    dst[0] = (1.0 + dst[0]) / 2.0;
    dst[1] = (1.0 + dst[1]) / 2.0;
    dst[2] = (1.0 + dst[2]) / 2.0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns a plane parallel to the near (or far) plane of the view
//    volume at a given distance from the projection point (eye).
//
// Use: public

SbPlane
SbViewVolume::getPlane(float distFromEye) const
//
////////////////////////////////////////////////////////////////////////
{
    return SbPlane(-projDir, projPoint + distFromEye * projDir);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the point along the line of sight at the given distance
//    from the projection point (eye).
//
// Use: public

SbVec3f
SbViewVolume::getSightPoint(float distFromEye) const
//
////////////////////////////////////////////////////////////////////////
{
    return projPoint + distFromEye * projDir;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the projection of a given point in normalized screen
//    coords (see projectToScreen()) onto the plane parallel to the
//    near plane that is at distFromEye units from the eye.
//
// Use: public

SbVec3f
SbViewVolume::getPlanePoint(float distFromEye, const SbVec2f &normPoint) const
//
////////////////////////////////////////////////////////////////////////
{
    SbLine	line;
    SbVec3f	point;
    SbPlane	plane = getPlane(distFromEye);

    // Project screen point to line through view volume
    projectPointToLine(normPoint, line);

    // This intersection should always be valid, since the plane is
    // parallel to the near plane
    plane.intersect(line, point);

    return point;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns a rotation that would align a viewed object so that its
//    positive x-axis (of its object space) is to the right in the
//    view and it's positive y-axis is up. If rightAngleOnly is TRUE,
//    it will come as close as it can to this goal by using only 90
//    degree rotations.
//
// Use: public

SbRotation
SbViewVolume::getAlignRotation(SbBool rightAngleOnly) const
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f	yAxis(0.0, 1.0, 0.0);
    SbVec3f	up    = ulfO - llfO;
    SbVec3f	right = lrfO - llfO;
    SbVec3f	newRight;
    SbMatrix	rotMat;
    SbRotation	result;

    up.normalize();
    right.normalize();

    if (! rightAngleOnly) {

	// First rotate so that y-axis becomes "up".
	result.setValue(yAxis, up);

	// Then rotate about "up" so the x-axis becomes "right".
	rotMat.setRotate(result);
	rotMat.multDirMatrix(SbVec3f(1.0, 0.0, 0.0), newRight);

	// Need to make certain that the rotation is phrased as a rot
	// about the y axis.  If we just use
	// SbRotation(newRight,right), in the case where 'newRight' is
	// opposite direction of 'right' the algorithm gives us a 180
	// degree rot about z, not y, which screws things up.
	float thetaCos = newRight.dot(right);
	if (thetaCos < -0.99999) {
	    result *= SbRotation( SbVec3f(0,1,0), 3.14159 );
	}
	else {
	    result *= SbRotation(newRight, right);
	}
    }

    else {
	SbRotation	rotToUp, rot1, rot2;
	SbVec3f		vec;
	float		d, max;
	int		i;

	// Rotate to get the best possible rotation to put Y close to "up"
	rotToUp.setValue(yAxis, up.getClosestAxis());

	// Rotate to get the best possible rotation to put X close to "right".
	rotMat.setRotate(rotToUp);
	rotMat.multDirMatrix(SbVec3f(1.0, 0.0, 0.0), newRight);

	// Find which of the 4 rotations that are multiples of 90
	// degrees about the y-axis brings X closest to right

	max = -237.4;

	for (i = 0; i < 4; i++) {

	    // Rotate by -90, 0, 90, 180 degrees
	    rot1.setValue(yAxis, (i-1) * M_PI_2);

	    rot2 = rot1 * rotToUp;
	    rotMat.setRotate(rot2);
	    rotMat.multDirMatrix(newRight, vec);
	    d = vec.dot(right);
	    if (d > max) {
		result = rot2;
		max = d;
	    }
	}
    }

    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns a scale factor that would scale a unit sphere centered
//    at worldCenter so that it would appear to have the given radius
//    in normalized screen coordinates when projected onto the near
//    plane.
//
// Use: public

float
SbViewVolume::getWorldToScreenScale(const SbVec3f &worldCenter,
				    float normRadius) const
//
////////////////////////////////////////////////////////////////////////
{
    // Project worldCenter into normalized coordinates
    SbVec3f normCenter3;
    projectToScreen(worldCenter, normCenter3);
    SbVec2f normCenter( normCenter3[0], normCenter3[1] );


    // This method really behaves best if you keep the normalized
    // points within the (0,0) (1,1) range.  So we shift the
    // normCenter if necessary
    SbBool centerShifted = FALSE;
    if ( normCenter[0] < 0.0 ) {
	 normCenter[0] = 0.0;
         centerShifted = TRUE;
    }
    if ( normCenter[0] > 1.0 ) {
	 normCenter[0] = 1.0;
         centerShifted = TRUE;
    }
    if ( normCenter[1] < 0.0 ) {
	 normCenter[1] = 0.0;
         centerShifted = TRUE;
    }
    if ( normCenter[1] > 1.0 ) {
	 normCenter[1] = 1.0;
         centerShifted = TRUE;
    }

    // We'll either take the distance between a point that's offset
    // vertically or horizontally from the original point.  This
    // depends on the aspect ration of the view.
    //
    // If it's wider-than-tall, we'll use the vertical offset since
    // the area subtended in the world will not change vertically as
    // we stretch horizontally until we reach a square aspec.
    //
    // If it's taller-than-wide, we'll use the hozizontal offset for
    // similar reasons.  Also, we'll offset up or down depending on
    // which brings us more toward the center of the viewport, where
    // results are better.

    // Should we use a vertical or horizontal offset?
    SbBool goVertical = (getWidth() > getHeight());

    // Pick a point that is normRadius away from normCenter in the 
    // desired direction.,
    SbVec2f     offsetPoint = normCenter;
    if ( goVertical ) {
	if (offsetPoint[1] < 0.5)
	    offsetPoint[1] += normRadius;
	else
	    offsetPoint[1] -= normRadius;
    }
    else {
	if (offsetPoint[0] < 0.5)
	    offsetPoint[0] += normRadius;
	else
	    offsetPoint[0] -= normRadius;
    }

    // The original method only works for perspective projections. The
    // problem is in construction of 'plane.' For an ortho view, this
    // should be a plane at the location worldCenter, but with a
    // normal in the direction parallel to our line.

    // Find centerLine, the line you get when you project normCenter into
    // the scene.
    SbLine	centerLine;
    projectPointToLine(normCenter, centerLine);

    // Find the plane that passes through worldCenter and is
    // perpendicular to the centerLine
    SbVec3f	norm = centerLine.getDirection();
    norm.normalize();
    SbPlane plane(norm, worldCenter);

    // Project offsetPoint onto that plane and return distance from
    // worldCenter.
    SbLine offsetLine;
    projectPointToLine(offsetPoint, offsetLine);

    // Intersect centerLine with the plane to get the location of normCenter
    // projected onto that plane.  If we didn't need to shift the normCenter 
    // then this is just the same as worldCenter
    SbVec3f worldSeedPoint = worldCenter;

    if ( centerShifted == TRUE ) {
	SbBool isOk = plane.intersect(centerLine, worldSeedPoint);

	if ( !isOk ) {
	    // intersection did not succeeded, just return a value of 1
	    return 1.0;
	}
    }

    // Fix uninitialized memory read.  We need to check if the plane
    // intersection is successful, and if not we must not use results
    // to calculate an answer.  Instead, if the plane intersection
    // fails, we return 1.0
    SbVec3f	worldOffsetPoint;
    SbBool isOk = plane.intersect(offsetLine, worldOffsetPoint);

    if ( !isOk ) {
	// intersection did not succeeded, just return a value of 1
	return 1.0;
    }

    // intersection succeeded. return dist from worldCenter
    float answerDist = (worldSeedPoint - worldOffsetPoint).length();

    return answerDist;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Projects the given 3D bounding box onto the near plane and
//    returns the size (in normalized screen coords) of the
//    rectangular region that encloses it.
//
// Use: public

SbVec2f
SbViewVolume::projectBox(const SbBox3f &box) const
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f	min, max, screenPoint[8];

    box.getBounds(min, max);

    // Project points to (0 <= x,y,z <= 1) screen coordinates
    projectToScreen(SbVec3f(min[0], min[1], min[2]), screenPoint[0]);
    projectToScreen(SbVec3f(min[0], min[1], max[2]), screenPoint[1]);
    projectToScreen(SbVec3f(min[0], max[1], min[2]), screenPoint[2]);
    projectToScreen(SbVec3f(min[0], max[1], max[2]), screenPoint[3]);
    projectToScreen(SbVec3f(max[0], min[1], min[2]), screenPoint[4]);
    projectToScreen(SbVec3f(max[0], min[1], max[2]), screenPoint[5]);
    projectToScreen(SbVec3f(max[0], max[1], min[2]), screenPoint[6]);
    projectToScreen(SbVec3f(max[0], max[1], max[2]), screenPoint[7]);

    // Find the encompassing 2d box (-1 <= x,y <= 1)
    SbBox2f	fBox;
    for (int i = 0; i < 8; i++)
	fBox.extendBy(SbVec2f(screenPoint[i][0], screenPoint[i][1]));

    // Return size of box
    SbVec2f	size;
    fBox.getSize(size[0], size[1]);
    return size;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Given a view volume, this narrows the view to the given sub-rectangle
//    of the near plane. The coordinates of the rectangle are between
//    0 and 1, where (0,0) is the lower-left corner of the near plane
//    and (1,1) is the upper-right corner.
//
// Use: public

SbViewVolume
SbViewVolume::narrow(float left, float bottom, float right, float top) const
//
////////////////////////////////////////////////////////////////////////
{
    SbViewVolume vv;

    vv.type = type;
    vv.projPoint = projPoint;
    vv.projDir = projDir;
    vv.llfO = (lrfO - llfO) * left  + (ulfO - llfO) * bottom + llfO;
    vv.lrfO = (lrfO - llfO) * right + (ulfO - llfO) * bottom + llfO;
    vv.ulfO = (lrfO - llfO) * left  + (ulfO - llfO) * top    + llfO;
    vv.llf = vv.llfO + vv.projPoint;  // For compatibility
    vv.lrf = vv.lrfO + vv.projPoint;
    vv.ulf = vv.ulfO + vv.projPoint;
    vv.nearDist  = nearDist;
    vv.nearToFar = nearToFar;

    return vv;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets up an orthographic view volume with the given sides.
//    The parameters are the same as for the GL ortho() routine.
//
// Use: public

void
SbViewVolume::ortho(float left, float right,
		    float bottom, float top,
		    float near, float far)
//
////////////////////////////////////////////////////////////////////////
{
    type = ORTHOGRAPHIC;
    projPoint	= SbVec3f(0.0, 0.0, 0.0);
    projDir	= SbVec3f(0.0, 0.0, -1.0);
    llfO	= SbVec3f(left,  bottom, -near);
    lrfO	= SbVec3f(right, bottom, -near);
    ulfO	= SbVec3f(left,  top,    -near);
    llf		= llfO + projPoint;  // For compatibility
    lrf		= lrfO + projPoint;
    ulf		= ulfO + projPoint;
    nearDist	= near;
    nearToFar	= far - near;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets up a perspective view volume with the given field of view
//    and aspect ratio. The parameters are the same as for the GL
//    perspective() routine, except that the field of view angle is
//    specified in radians.
//
// Use: public

void
SbViewVolume::perspective(float fovy, float aspect,
			  float near, float far)
//
////////////////////////////////////////////////////////////////////////
{
    float tanfov = tan(fovy / 2.0);

    type = PERSPECTIVE;

    projPoint.setValue(0.0, 0.0, 0.0);
    projDir.setValue(0.0, 0.0, -1.0);
    llfO[2] = lrfO[2] = ulfO[2] = - near;

    ulfO[1] = near * tanfov;
    llfO[1] = lrfO[1] = - ulfO[1];
    ulfO[0] = llfO[0] = aspect * llfO[1];
    lrfO[0] = - llfO[0];

    llf		= llfO + projPoint; // For compatibility
    lrf		= lrfO + projPoint;
    ulf		= ulfO + projPoint;

    nearDist  = near;
    nearToFar = far - near;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Translate the camera viewpoint.  Note that this accomplishes
//    the reverse of doing a GL translate() command after defining a
//    camera, which translates the scene viewed by the camera.
//
// Use: public

void
SbViewVolume::translateCamera(const SbVec3f &v)
//
////////////////////////////////////////////////////////////////////////
{
    projPoint	+= v;
    llf		+= v;
    lrf		+= v;
    ulf		+= v;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Rotate the camera view direction.  Note that this accomplishes
//    the reverse of doing a GL rotate() command after defining a
//    camera, which rotates the scene viewed by the camera.
//
// Use: public

void
SbViewVolume::rotateCamera(const SbRotation &r)
//
////////////////////////////////////////////////////////////////////////
{
    SbMatrix	m;
    m.setRotate(r);

    m.multDirMatrix(projDir, projDir);

    m.multDirMatrix(llfO, llfO);
    m.multDirMatrix(lrfO, lrfO);
    m.multDirMatrix(ulfO, ulfO);

    llf		= llfO + projPoint; // For compatibility
    lrf		= lrfO + projPoint;
    ulf		= ulfO + projPoint;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the vector that will map to the positive z axis in eye
//    space.  This vector will be perpendicular to the near and far
//    clipping planes.  In this coordinate system, the z value of the
//    near plane should be GREATER than the z value of the far plane.
//
// Use: public

SbVec3f
SbViewVolume::zVector() const
//
////////////////////////////////////////////////////////////////////////
{
    SbPlane	plane;

    // note dependency on how the plane is calculated: we want the
    // returned vector to point away from the projDir
    plane = SbPlane(llfO, ulfO, lrfO);

    // If we wanted a world-space plane, would also have to translate
    // it to projPoint.  But all we're interested in here is the normal:
    return - plane.getNormal();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns a narrowed view volume which contains as tightly as
//    possible the given interval on the z axis (in eye space).  The
//    returned view volume will never be larger than the current volume,
//    however.
//
//    Near and far are relative to the current clipping planes, where
//    1.0 is the current near plane and 0.0 is the current far plane.
//
// Use: public

SbViewVolume
SbViewVolume::zNarrow(float near, float far) const
//
////////////////////////////////////////////////////////////////////////
{
    SbPlane		plane;
    SbViewVolume	narrowed;
    SbVec3f		zVec = zVector();

    // make sure we aren't expanding the volume
    if (near > 1.0)
	near = 1.0;
    if (far < 0.0)
	far = 0.0;

    narrowed.type	= type;
    narrowed.projPoint	= projPoint;
    narrowed.projDir	= projDir;

    // the near-to-far distance can be calculated from the new near
    // and far values
    narrowed.nearDist	= near;
    narrowed.nearToFar	= (near - far) * nearToFar;

    // the new near plane
    // find the old near plane
    plane = SbPlane(zVec, llfO);
    // offset it
    plane.offset((near - 1.0) * nearToFar);

    // intersect various lines with the new near plane to find the new
    // info for the view volume
    if (type == ORTHOGRAPHIC) {
	plane.intersect(SbLine(llfO, llfO + projDir), narrowed.llfO);
	plane.intersect(SbLine(lrfO, lrfO + projDir), narrowed.lrfO );
	plane.intersect(SbLine(ulfO, ulfO + projDir), narrowed.ulfO );
    }
    else {				// type == PERSPECTIVE
	SbVec3f origin(0,0,0);
	plane.intersect(SbLine(origin, llfO), narrowed.llfO );
	plane.intersect(SbLine(origin, lrfO), narrowed.lrfO );
	plane.intersect(SbLine(origin, ulfO), narrowed.ulfO );
    }

    narrowed.llf = narrowed.llfO+narrowed.projPoint;  // For compatibility
    narrowed.lrf = narrowed.lrfO+narrowed.projPoint;
    narrowed.ulf = narrowed.ulfO+narrowed.projPoint;

    return narrowed;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Scales width and height of view volume by given factor.
//
// Use: public

void
SbViewVolume::scale(float factor)
//
////////////////////////////////////////////////////////////////////////
{
    // Don't like negative factors:
    if (factor < 0) factor = -factor;

    // Compute amount to move corners
    float	diff = (1.0 - factor) / 2.0;

    // Find vectors from lower left corner to lower right corner and
    // to upper left corner and scale them
    SbVec3f	widthVec  = diff * (lrfO - llfO);
    SbVec3f	heightVec = diff * (ulfO - llfO);

    // Move all corners in correct direction
    llfO += ( heightVec + widthVec);
    lrfO += ( heightVec - widthVec);
    ulfO += (-heightVec + widthVec);

    llf		= llfO + projPoint;  // For compatibility
    lrf		= lrfO + projPoint;
    ulf		= ulfO + projPoint;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Scales view volume to be the given ratio of its current width,
//    leaving the resulting view volume centered about the same point
//    (in the near plane) as the current one.
//
// Use: public

void
SbViewVolume::scaleWidth(float ratio)
//
////////////////////////////////////////////////////////////////////////
{
    // Don't like negative ratios:
    if (ratio < 0) ratio = -ratio;

    // Find vector from lower left corner to lower right corner
    SbVec3f	widthVec = lrfO - llfO;

    // Compute amount to move corners left or right and scale vector
    widthVec *= (1.0 - ratio) / 2.0;

    // Move all corners in correct direction
    llfO += widthVec;
    ulfO += widthVec;
    lrfO -= widthVec;

    llf		= llfO + projPoint;  // For compatibility
    lrf		= lrfO + projPoint;
    ulf		= ulfO + projPoint;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Scales view volume to be the given ratio of its current height,
//    leaving the resulting view volume centered about the same point
//    (in the near plane) as the current one.
//
// Use: public

void
SbViewVolume::scaleHeight(float ratio)
//
////////////////////////////////////////////////////////////////////////
{
    // Don't like negative ratios:
    if (ratio < 0) ratio = -ratio;

    // Find vector from lower left corner to upper left corner
    SbVec3f	heightVec = ulfO - llfO;

    // Compute amount to move corners up or down and scale vector
    heightVec *= (1.0 - ratio) / 2.0;

    // Move all corners in correct direction
    llfO += heightVec;
    lrfO += heightVec;
    ulfO -= heightVec;

    llf		= llfO + projPoint;  // For compatibility
    lrf		= lrfO + projPoint;
    ulf		= ulfO + projPoint;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//
// Use: public

SbViewVolume
SbViewVolume::narrow(const SbBox3f &box) const
//
////////////////////////////////////////////////////////////////////////
{
    SbViewVolume	view;
    const SbVec3f	&max = box.getMax(), &min = box.getMin();

    view = narrow(min[0], min[1], max[0], max[1]);

    return view.zNarrow(max[2], min[2]);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Transforms the view volume by the given matrix.
//    NOTE: If there is a scale and a rotation in the matrix, the angles
//	between the transformed projection direction and everything else
//	are not preserved!
//
// Use: internal

void
SbViewVolume::transform(const SbMatrix &matrix)
//
////////////////////////////////////////////////////////////////////////
{
    SbViewVolume xfVol;
    SbVec3f nearPt, farPt;

    xfVol.type = type;
    matrix.multVecMatrix(projPoint, xfVol.projPoint);
    matrix.multDirMatrix(projDir, xfVol.projDir);
    xfVol.projDir.normalize();

    // Matrices that translate to/from origin-centered space.
    // We want to find llf', and we know that:
    // llf'+projPoint' = matrix*(llf+projPoint)

    matrix.multVecMatrix((llfO+projPoint), xfVol.llfO);
    xfVol.llfO -= xfVol.projPoint;
    matrix.multVecMatrix((lrfO+projPoint), xfVol.lrfO);
    xfVol.lrfO -= xfVol.projPoint;
    matrix.multVecMatrix((ulfO+projPoint), xfVol.ulfO);
    xfVol.ulfO -= xfVol.projPoint;

    matrix.multVecMatrix(projPoint + nearDist * projDir, nearPt);
    matrix.multVecMatrix(projPoint + (nearDist + nearToFar) * projDir, farPt);
    xfVol.nearDist = (nearPt - xfVol.projPoint).length();
    if (nearDist < 0)
	xfVol.nearDist = -xfVol.nearDist;
    xfVol.nearToFar = (farPt - xfVol.projPoint).length() - xfVol.nearDist;

    *this = xfVol;

    // Check for inside-out view volume:
    SbVec3f wVec = lrfO-llfO;
    SbVec3f hVec = ulfO-llfO;
    if ((hVec.cross(wVec)).dot(projDir) <= 0.0) {
	// Swap left and right:
	SbVec3f temp = llfO;
	llfO = lrfO;
	lrfO = temp;
	ulfO = llfO+hVec;
    }

    llf		= llfO + projPoint;  // For compatibility
    lrf		= lrfO + projPoint;
    ulf		= ulfO + projPoint;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Intersects view volume with point. Returns TRUE if intersection.
//
// Use: internal

SbBool
SbViewVolume::intersect(const SbVec3f &point) const
//
////////////////////////////////////////////////////////////////////////
{
    // Transform point to origin:
    SbVec3f pt = point - projPoint;

    // Compare the point with all 6 planes of the view volume
    SbVec3f origin(0,0,0);

    if (type == PERSPECTIVE) {
	// Left plane is formed from origin, llfO, ulfO
	SbPlane leftPlane(origin, llfO, ulfO);
	if (! leftPlane.isInHalfSpace(pt))
	    return FALSE;

	// Figure out urf point:
	SbVec3f urfO = lrfO + (ulfO - llfO);
	// Right is origin, urfO, lrfO
	SbPlane rightPlane(origin, urfO, lrfO);
	if (! rightPlane.isInHalfSpace(pt))
	    return FALSE;

	// Near is lrfO, llfO, ulfO:
	SbPlane nearPlane(lrfO, llfO, ulfO);
	if (! nearPlane.isInHalfSpace(pt))
	    return FALSE;

	// Far is near points in opposite order, translated to far plane:
	SbVec3f farOffset = nearToFar * projDir;
	SbPlane farPlane(ulfO+farOffset, llfO+farOffset, lrfO+farOffset);
	if (! farPlane.isInHalfSpace(pt))
	    return FALSE;

	// Bottom is origin, lrfO, llfO
	SbPlane bottomPlane(origin, lrfO, llfO);
	if (! bottomPlane.isInHalfSpace(pt))
	    return FALSE;

	// Finally, top is origin, ulfO, urfO
	SbPlane topPlane(origin, ulfO, urfO);
	if (! topPlane.isInHalfSpace(pt))
	    return FALSE;
    }

    else {			// type == ORTHOGRAPHIC
	// Left plane is formed from llfO, lff+projDir, ulfO
	SbPlane leftPlane(llfO, llfO+projDir, ulfO);
	if (! leftPlane.isInHalfSpace(pt))
	    return FALSE;

	// Figure out urfO point:
	SbVec3f urfO = lrfO + (ulfO - llfO);
	// Right is urfO+projDir, lrfO, urfO
	SbPlane rightPlane(urfO+projDir, lrfO, urfO);
	if (! rightPlane.isInHalfSpace(pt))
	    return FALSE;

	// Near is lrfO, llfO, ulfO:
	SbPlane nearPlane(lrfO, llfO, ulfO);
	if (! nearPlane.isInHalfSpace(pt))
	    return FALSE;

	// Far is near points in opposite order, translated to far plane:
	SbVec3f farOffset = nearToFar * projDir;
	SbPlane farPlane(ulfO+farOffset, llfO+farOffset, lrfO+farOffset);
	if (! farPlane.isInHalfSpace(pt))
	    return FALSE;

	// Bottom is lrfO, lrfO+projDir, lrfO, llfO
	SbPlane bottomPlane(lrfO, lrfO+projDir, llfO);
	if (! bottomPlane.isInHalfSpace(pt))
	    return FALSE;

	// Finally, top is urfO, ulfO, ulfO+projDir
	SbPlane topPlane(urfO, ulfO, ulfO+projDir);
	if (! topPlane.isInHalfSpace(pt))
	    return FALSE;
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Intersects view volume with line segment. Returns TRUE if intersection.
//
// Use: internal

SbBool
SbViewVolume::intersect(const SbVec3f &p0, const SbVec3f &p1,
			SbVec3f &closestPoint) const
//
////////////////////////////////////////////////////////////////////////
{
    SbLine	line(p0, p1);
    SbLine	projLine;
    SbVec3f	centerPt, ptOnProjLine;

    // Use line from projection point through center of near rectangle
    centerPt = llfO+projPoint + 0.5 * ((ulfO - llfO) + (lrfO - llfO));
    if (type == ORTHOGRAPHIC)
	// projDir is normalized, so it might not be in the same
	// ballpark as centerPt. To fix this, use the vector from the
	// near plane to the far plane.
	projLine.setValue(centerPt - nearToFar * projDir, centerPt);
    else
	projLine.setValue(projPoint, centerPt);

    SbBool validIntersection =

	// Find point on segment that's closest to projection line.
	// (If they are parallel, no intersection.)
	(line.getClosestPoints(projLine, closestPoint, ptOnProjLine) &&

	 // Make sure this point is within the ends of the segment
	 ((closestPoint - p0).dot(p1 - p0) >= 0.0 &&
	  (closestPoint - p1).dot(p0 - p1) >= 0.0) &&

	 // Also make sure that the intersection is within the view volume
	 intersect(closestPoint));

    return validIntersection;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Intersects view volume with box. Returns TRUE if intersection.
//
// Use: internal

SbBool
SbViewVolume::intersect(const SbBox3f &box) const
//
////////////////////////////////////////////////////////////////////////
{
    // Empty bboxes can cause problems:
    if (box.isEmpty()) return FALSE;

    //
    // The bounding box is the set of all points between its bounds;
    // that is, all points (x,y,z) such that:
    //		x_min <= x <= x_max
    //		y_min <= y <= y_max
    //		z_min <= z <= z_max
    // (Inventor bounding boxes are axis-aligned)
    //
    // We will consider there to be no intersection if all of those
    // points are on the 'outside' side of any of the view-volume
    // planes (this may pass some things that could be culled, but
    // will never cull things that are visible). So we want to see if
    // any point is on the inside, in which case the intersection
    // returns TRUE.
    //
    // The equation of the view-volume planes is Ax+By+Cz=D, where
    // (A,B,C) is the plane normal and D is the distance from the
    // origin. The condition for a point (x,y,z) being on the
    // 'outside' side is Ax+By+Cz-D < 0. We need to test the largest
    // value of Ax+By+Cz-D to see if it is negative. If it is, then we
    // know all points are outside.
    //
    // We can minimize the number of point/plane checks we do by
    // noticing that Ax+By+Cx-D will be greatest when each of its
    // terms is greatest; for example, if A is positive, Ax will be
    // greatest when x is x_max. If A is negative, Ax will be greatest
    // when x is x_min.
    // So, we can reduce the test to a check of one point against each
    // of the 6 plane equations. The outsideTest() method does this.
    //

    SbVec3f min, max;
    box.getBounds(min, max);

    // Transform bbox to origin:
    min -= projPoint;
    max -= projPoint;

    // OPPORTUNITY FOR OPTIMIZATION HERE:  We could precompute planes
    // and save some work.

    SbVec3f origin(0,0,0);

    if (type == PERSPECTIVE) {
	// Left plane is formed from origin, llfO, ulfO
	SbPlane leftPlane(origin, llfO, ulfO);
	if (outsideTest(leftPlane, min, max))
	    return FALSE;

	// Figure out urf point:
	SbVec3f urfO = lrfO + (ulfO - llfO);
	// Right is origin, urfO, lrfO
	SbPlane rightPlane(origin, urfO, lrfO);
	if (outsideTest(rightPlane, min, max))
	    return FALSE;

	// Near is lrfO, llfO, ulfO:
	SbPlane nearPlane(lrfO, llfO, ulfO);
	if (outsideTest(nearPlane, min, max))
	    return FALSE;

	// Far is near points in opposite order, translated to far plane:
	SbVec3f farOffset = nearToFar * projDir;
	SbPlane farPlane(ulfO+farOffset, llfO+farOffset, lrfO+farOffset);
	if (outsideTest(farPlane, min, max))
	    return FALSE;

	// Bottom is origin, lrfO, llfO
	SbPlane bottomPlane(origin, lrfO, llfO);
	if (outsideTest(bottomPlane, min, max))
	    return FALSE;

	// Finally, top is origin, ulfO, urfO
	SbPlane topPlane(origin, ulfO, urfO);
	if (outsideTest(topPlane, min, max))
	    return FALSE;
    }

    else {			// type == ORTHOGRAPHIC
	// Left plane is formed from llfO, lff+projDir, ulfO
	SbPlane leftPlane(llfO, llfO+projDir, ulfO);
	if (outsideTest(leftPlane, min, max))
	    return FALSE;

	// Figure out urfO point:
	SbVec3f urfO = lrfO + (ulfO - llfO);
	// Right is urfO+projDir, lrfO, urfO
	SbPlane rightPlane(urfO+projDir, lrfO, urfO);
	if (outsideTest(rightPlane, min, max))
	    return FALSE;

	// Near is lrfO, llfO, ulfO:
	SbPlane nearPlane(lrfO, llfO, ulfO);
	if (outsideTest(nearPlane, min, max))
	    return FALSE;

	// Far is near points in opposite order, translated to far plane:
	SbVec3f farOffset = nearToFar * projDir;
	SbPlane farPlane(ulfO+farOffset, llfO+farOffset, lrfO+farOffset);
	if (outsideTest(farPlane, min, max))
	    return FALSE;

	// Bottom is lrfO, lrfO+projDir, lrfO, llfO
	SbPlane bottomPlane(lrfO, lrfO+projDir, llfO);
	if (outsideTest(bottomPlane, min, max))
	    return FALSE;

	// Finally, top is urfO, ulfO, ulfO+projDir
	SbPlane topPlane(urfO, ulfO, ulfO+projDir);
	if (outsideTest(topPlane, min, max))
	    return FALSE;
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if the bounding box defined by the two given points
//    is totally outside the given plane. See the comments in
//    intersect(bbox) for details.
//
// Use: internal

SbBool
SbViewVolume::outsideTest(const SbPlane &p,
			  const SbVec3f &min, const SbVec3f &max) const
//
////////////////////////////////////////////////////////////////////////
{
    const SbVec3f	&abc = p.getNormal();
    float		sum;

    // Compute the greatest value of Ax+By+Cz-D
    sum = -p.getDistanceFromOrigin();				// -D
    sum += abc[0] > 0.0 ? max[0] * abc[0] : min[0] * abc[0];	// Ax
    sum += abc[1] > 0.0 ? max[1] * abc[1] : min[1] * abc[1];	// By
    sum += abc[2] > 0.0 ? max[2] * abc[2] : min[2] * abc[2];	// Cz

    // Box is outside only if largest value is negative
    return (sum < 0.0 ? TRUE : FALSE);
}
