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
 |	SbLine
 |
 |   Author(s)		: Nick Thompson, Howard Look
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SbBox.h>
#include <Inventor/SbLinear.h>

//////////////////////////////////////////////////////////////////////////////
//
// Line class
//
//////////////////////////////////////////////////////////////////////////////

// construct a line given 2 points on the line
SbLine::SbLine(const SbVec3f &p0, const SbVec3f &p1)
{
    setValue(p0, p1);
}

// setValue constructs a line give two points on the line
void
SbLine::setValue(const SbVec3f &p0, const SbVec3f &p1)
{
    pos = p0;
    dir = p1 - p0;
    dir.normalize();
}

// find points on this line and on line2 that are closest to each other.
// If the lines intersect, then ptOnThis and ptOnLine2 will be equal.
// If the lines are parallel, then FALSE is returned, and the contents of
// ptOnThis and ptOnLine2 are undefined.
SbBool
SbLine::getClosestPoints( const SbLine &line2,
				SbVec3f &ptOnThis,
				SbVec3f &ptOnLine2 ) const
{
    float    s, t, A, B, C, D, E, F, denom;
    SbVec3f  pos2 = line2.getPosition();
    SbVec3f  dir2 = line2.getDirection();


//  DERIVATION:
//      [1] parametric descriptions of desired pts
//          pos  + s * dir  = ptOnThis
//          pos2 + t * dir2 = ptOnLine2
//      [2] (By some theorem or other--)
//          If these are closest points between lines, then line connecting 
//          these points is perpendicular to both this line and line2.
//          dir  . ( ptOnLine2 - ptOnThis ) = 0
//          dir2 . ( ptOnLine2 - ptOnThis ) = 0
//          OR...
//          dir  . ( pos2 + t * dir2 - pos - s * dir ) = 0
//          dir2 . ( pos2 + t * dir2 - pos - s * dir ) = 0
//      [3] Rearrange the terms:
//          t * [ dir  . dir2 ] - s * [dir  . dir ] = dir  . pos - dir  . pos2 
//          t * [ dir2 . dir2 ] - s * [dir2 . dir ] = dir2 . pos - dir2 . pos2 
//      [4] Let:
//          A= dir  . dir2
//          B= dir  . dir
//          C= dir  . pos - dir . pos2
//          D= dir2 . dir2
//          E= dir2 . dir
//          F= dir2 . pos - dir2 . pos2
//          So [3] above turns into:
//          t * A    - s * B =   C
//          t * D    - s * E =   F
//      [5] Solving for s gives:
//          s = (CD - AF) / (AE - BD)
//      [6] Solving for t gives:
//          t = (CE - BF) / (AE - BD)

    A = dir.dot( dir2 );
    B = dir.dot( dir );
    C = dir.dot( pos ) - dir.dot( pos2 );
    D = dir2.dot( dir2 );
    E = dir2.dot( dir );
    F = dir2.dot( pos ) - dir2.dot( pos2 );


    denom = A * E - B * D;
    if ( denom == 0.0)    // the two lines are parallel
	return FALSE;

    s = ( C * D - A * F ) / denom;
    t = ( C * E - B * F ) / denom;
    ptOnThis  = pos  + dir  * s;
    ptOnLine2 = pos2 + dir2 * t;

    return TRUE;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//  Returns the closes point on this line to the given point.
//
// Use: public

SbVec3f
SbLine::getClosestPoint(const SbVec3f &point) const
//
////////////////////////////////////////////////////////////////////////
{
    // vector from origin of this line to given point
    SbVec3f p = point - pos;

    // find the length of p when projected onto this line
    // (which has direction d)
    // length = |p| * cos(angle b/w p and d) = (p.d)/|d|
    // but |d| = 1, so
    float length = p.dot(dir);

    // vector coincident with this line
    SbVec3f proj = dir;
    proj *= length;

    SbVec3f result = pos + proj;
    return result;
}    


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Intersect the line with a 3D box.  The line is intersected with the
//    twelve triangles that make up the box.
//
// Use: internal

SbBool
SbLine::intersect(const SbBox3f &box, SbVec3f &enter, SbVec3f &exit) const
//
////////////////////////////////////////////////////////////////////////
{
    if (box.isEmpty())
	return FALSE;

    const SbVec3f	&pos = getPosition(), &dir = getDirection();
    const SbVec3f	&max = box.getMax(), &min = box.getMin();
    SbVec3f		points[8], inter, bary;
    SbPlane		plane;
    int			i, v0, v1, v2;
    SbBool		front = FALSE, valid, validIntersection = FALSE;

    //
    // First, check the distance from the ray to the center
    // of the box.  If that distance is greater than 1/2
    // the diagonal distance, there is no intersection
    // diff is the vector from the closest point on the ray to the center
    // dist2 is the square of the distance from the ray to the center
    // rad2 is the square of 1/2 the diagonal length of the bounding box
    //
    float    t = (box.getCenter() - pos).dot(dir);
    SbVec3f  diff(pos + t * dir - box.getCenter());
    float    dist2 = diff.dot(diff);
    float    rad2 = (max - min).dot(max - min) * .25;

    if (dist2 > rad2)
	return FALSE;

    // set up the eight coords of the corners of the box
    for(i = 0; i < 8; i++)
	points[i].setValue(i & 01 ? min[0] : max[0],
			   i & 02 ? min[1] : max[1],
			   i & 04 ? min[2] : max[2]);

    // intersect the 12 triangles.
    for(i = 0; i < 12; i++) {
	switch(i) {
	case  0: v0 = 2; v1 = 1; v2 = 0; break;		// +z
	case  1: v0 = 2; v1 = 3; v2 = 1; break;

	case  2: v0 = 4; v1 = 5; v2 = 6; break;		// -z
	case  3: v0 = 6; v1 = 5; v2 = 7; break;

	case  4: v0 = 0; v1 = 6; v2 = 2; break;		// -x
	case  5: v0 = 0; v1 = 4; v2 = 6; break;

	case  6: v0 = 1; v1 = 3; v2 = 7; break;		// +x
	case  7: v0 = 1; v1 = 7; v2 = 5; break;

	case  8: v0 = 1; v1 = 4; v2 = 0; break;		// -y
	case  9: v0 = 1; v1 = 5; v2 = 4; break;

	case 10: v0 = 2; v1 = 7; v2 = 3; break;		// +y
	case 11: v0 = 2; v1 = 6; v2 = 7; break;
	}
	if ((valid = intersect(points[v0], points[v1], points[v2],
			       inter, bary, front)) == TRUE) {
	    if	(front) {
		enter = inter;
		validIntersection = valid;
	    }
	    else {
		exit = inter;
		validIntersection = valid;
	    }
	}
    }
    return validIntersection;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Intersect the line with a 3D box.  The line is augmented with an
//    angle to form a cone.  The box must lie within pickAngle of the
//    line.  If the angle is < 0.0, abs(angle) is the radius of a
//    cylinder for an orthographic intersection. 
//
// Use: internal

SbBool
SbLine::intersect(float angle, const SbBox3f &box) const
//
////////////////////////////////////////////////////////////////////////
{
    if (box.isEmpty())
	return FALSE;

    const SbVec3f	&max = box.getMax(), &min = box.getMin();
    float		fuzz = 0.0;
    int			i;

    if (angle < 0.0)
	fuzz = - angle;

    else {
	// Find the farthest point on the bounding box (where the pick
	// cone will be largest).  The amount of fuzz at this point will
	// be the minimum we can use.  Expand the box by that amount and
	// do an intersection.
	double tanA = tan(angle);
	for(i = 0; i < 8; i++) {
	    SbVec3f point(i & 01 ? min[0] : max[0],
			  i & 02 ? min[1] : max[1],
			  i & 04 ? min[2] : max[2]);
	    // how far is point from line origin??
	    SbVec3f	diff(point - getPosition());
	    double	thisFuzz = sqrt(diff.dot(diff)) * tanA;

	    if (thisFuzz > fuzz)
		fuzz = thisFuzz;
	}
    }

    SbBox3f fuzzBox = box;

    fuzzBox.extendBy(SbVec3f(min[0] - fuzz, min[1] - fuzz, min[2] - fuzz));
    fuzzBox.extendBy(SbVec3f(max[0] + fuzz, max[1] + fuzz, max[2] + fuzz));

    SbVec3f scratch1, scratch2;
    return intersect(fuzzBox, scratch1, scratch2);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Intersect the line with a point.  The line is augmented with an
//    angle to form a cone.  The point must lie within pickAngle of the
//    line.  If the angle is < 0.0, abs(angle) is
//    the radius of a cylinder for an orthographic intersection.
//
// Use: internal

SbBool
SbLine::intersect(
    float pickAngle,			// The angle which makes the cone
    const SbVec3f &point) const		// The point to interesect.
//
////////////////////////////////////////////////////////////////////////
{
    float	t, d;

    // how far is point from line origin??
    SbVec3f	diff(point - getPosition());

    t = diff.dot(getDirection());
    if(t > 0) {
	d = sqrt(diff.dot(diff) - t*t);
        if (pickAngle < 0.0)
  	    return (d < -pickAngle);
	return ((d/t) < pickAngle);
    }
    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Intersect the line with a line segment.  The line is augmented with
//    an angle to form a cone.  The line segment must lie within pickAngle
//    of the line.  If an intersection occurs, the intersection point is
//    placed in intersection.
//
// Use: internal

SbBool
SbLine::intersect(
   float pickAngle,		// The angle which makes the cone
   const SbVec3f &v0,		// One endpoint of the line segment
   const SbVec3f &v1,		// The other endpoint of the line segment
   SbVec3f &intersection) const	// The intersection point
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f	ptOnLine;
    SbLine	inputLine(v0, v1);
    float	distance;
    SbBool      validIntersection = FALSE;

    if(getClosestPoints(inputLine, ptOnLine, intersection)) {
        // check to make sure the intersection is within the line segment
        if((intersection - v0).dot(v1 - v0) < 0)
            intersection = v0;
        else if((intersection - v1).dot(v0 - v1) < 0)
            intersection = v1;

	distance = (ptOnLine - intersection).length();
        if (pickAngle < 0.0)
            return (distance < -pickAngle);
	validIntersection = ((distance / (ptOnLine - getPosition()).length())
	                  < pickAngle);
    }
    return validIntersection;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Intersects the line with the triangle formed bu v0, v1, v2.
//    Returns TRUE if there is an intersection. If there is an
//    intersection, barycentric will contain the barycentric
//    coordinates of the intersection (for v0, v1, v2, respectively)
//    and front will be set to TRUE if the ray intersected the front
//    of the triangle (as defined by the right hand rule).
//
// Use: internal

SbBool
SbLine::intersect(const SbVec3f &v0, const SbVec3f &v1, const SbVec3f &v2,
		  SbVec3f &intersection,
		  SbVec3f &barycentric, SbBool &front) const
//
////////////////////////////////////////////////////////////////////////
{
    //////////////////////////////////////////////////////////////////
    //
    // The method used here is described by Didier Badouel in Graphics
    // Gems (I), pages 390 - 393.
    //
    //////////////////////////////////////////////////////////////////

#define EPSILON 1e-10

    //
    // (1) Compute the plane containing the triangle
    //
    SbVec3f	v01 = v1 - v0;
    SbVec3f	v12 = v2 - v1;
    SbVec3f	norm = v12.cross(v01);	// Un-normalized normal
    // Normalize normal to unit length, and make sure the length is
    // not 0 (indicating a zero-area triangle)
    if (norm.normalize() < EPSILON)
	return FALSE;

    //
    // (2) Compute the distance t to the plane along the line
    //
    float d = getDirection().dot(norm);
    if (d < EPSILON && d > -EPSILON)
	return FALSE;			// Line is parallel to plane
    float t = norm.dot(v0 - getPosition()) / d;

    // Note: we DO NOT ignore intersections behind the eye (t < 0.0)

    //
    // (3) Find the largest component of the plane normal. The other
    //     two dimensions are the axes of the aligned plane we will
    //     use to project the triangle.
    //
    float	xAbs = norm[0] < 0.0 ? -norm[0] : norm[0];
    float	yAbs = norm[1] < 0.0 ? -norm[1] : norm[1];
    float	zAbs = norm[2] < 0.0 ? -norm[2] : norm[2];
    int		axis0, axis1;

    if (xAbs > yAbs && xAbs > zAbs) {
	axis0 = 1;
	axis1 = 2;
    }
    else if (yAbs > zAbs) {
	axis0 = 2;
	axis1 = 0;
    }
    else {
	axis0 = 0;
	axis1 = 1;
    }

    //
    // (4) Determine if the projected intersection (of the line and
    //     the triangle's plane) lies within the projected triangle.
    //     Since we deal with only 2 components, we can avoid the
    //     third computation.
    //
    float intersection0 = getPosition()[axis0] + t * getDirection()[axis0];
    float intersection1 = getPosition()[axis1] + t * getDirection()[axis1];

    SbVec2f	diff0, diff1, diff2;
    SbBool	isInter = FALSE;
    float	alpha, beta;

    diff0[0] = intersection0 - v0[axis0];
    diff0[1] = intersection1 - v0[axis1];
    diff1[0] = v1[axis0]     - v0[axis0];
    diff1[1] = v1[axis1]     - v0[axis1];
    diff2[0] = v2[axis0]     - v0[axis0];
    diff2[1] = v2[axis1]     - v0[axis1];

    // Note: This code was rearranged somewhat from the code in
    // Graphics Gems to provide a little more numeric
    // stability. However, it can still miss some valid intersections
    // on very tiny triangles.
    isInter = FALSE;
    beta = ((diff0[1] * diff1[0] - diff0[0] * diff1[1]) /
	    (diff2[1] * diff1[0] - diff2[0] * diff1[1]));
    if (beta >= 0.0 && beta <= 1.0) {
	alpha = -1.0;
	if (diff1[1] < -EPSILON || diff1[1] > EPSILON) 
	    alpha = (diff0[1] - beta * diff2[1]) / diff1[1];
	else
	    alpha = (diff0[0] - beta * diff2[0]) / diff1[0];
	isInter = (alpha >= 0.0 && alpha + beta <= 1.0);
    }

    //
    // (5) If there is an intersection, set up the barycentric
    //     coordinates and figure out if the front was hit.
    //
    if (isInter) {
	barycentric.setValue(1.0 - (alpha + beta), alpha, beta);
	front = (getDirection().dot(norm) < 0.0);
	intersection = getPosition() + t * getDirection();
    }

    return isInter;

#undef EPSILON
}
