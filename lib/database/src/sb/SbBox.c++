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
 |	SbBox3f
 |	SbXfBox3f
 |	SbBox2f
 |	SbBox2s
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SbBox.h>
#include <Inventor/errors/SoDebugError.h>
#include <limits.h>
#include <float.h>    /* For FLT_MAX */

//
// Return the center of a box
//

SbVec3f
SbBox3f::getCenter() const
{
    return SbVec3f(0.5 * (min[0] + max[0]),
		   0.5 * (min[1] + max[1]), 
		   0.5 * (min[2] + max[2]));
}

//
// Extends Box3f (if necessary) to contain given 3D point
//

void
SbBox3f::extendBy(const SbVec3f &pt)
{
    if (pt[0] < min[0]) min[0] = pt[0];
    if (pt[1] < min[1]) min[1] = pt[1];
    if (pt[2] < min[2]) min[2] = pt[2];
    if (pt[0] > max[0]) max[0] = pt[0];
    if (pt[1] > max[1]) max[1] = pt[1];
    if (pt[2] > max[2]) max[2] = pt[2];
}

//
// Extends Box3f (if necessary) to contain given Box3f
//

void
SbBox3f::extendBy(const SbBox3f &bb)
{
    if (bb.min[0] < min[0]) min[0] = bb.min[0];
    if (bb.min[1] < min[1]) min[1] = bb.min[1];
    if (bb.min[2] < min[2]) min[2] = bb.min[2];
    if (bb.max[0] > max[0]) max[0] = bb.max[0];
    if (bb.max[1] > max[1]) max[1] = bb.max[1];
    if (bb.max[2] > max[2]) max[2] = bb.max[2];
}

//
// Returns TRUE if intersection of given point and Box3f is not empty
//

SbBool
SbBox3f::intersect(const SbVec3f &pt) const
{
    return ((pt[0] >= min[0]) &&
	    (pt[1] >= min[1]) &&
	    (pt[2] >= min[2]) &&
	    (pt[0] <= max[0]) &&
	    (pt[1] <= max[1]) &&
	    (pt[2] <= max[2]));
}

//
// Returns TRUE if intersection of given Box3f and Box3f is not empty
//

SbBool
SbBox3f::intersect(const SbBox3f &bb) const
{
    return ((bb.max[0] >= min[0]) && (bb.min[0] <= max[0]) &&
	    (bb.max[1] >= min[1]) && (bb.min[1] <= max[1]) &&
	    (bb.max[2] >= min[2]) && (bb.min[2] <= max[2]));
}


//
// View-volume culling: axis-aligned bounding box against view volume,
// given as Model/View/Projection matrix.
//
//
// Inputs:
//    MVP:       Matrix from object to NDC space
//                 (model/view/projection)
// Inputs/Outputs:
//    cullBits:  Keeps track of which planes we need to test against.
//               Has three bits, for X, Y and Z.  If cullBits is 0,
//               then the bounding box is completely inside the view
//               and no further cull tests need be done for things
//               inside the bounding box.  Zero bits in cullBits mean
//               the bounding box is completely between the
//               left/right, top/bottom, or near/far clipping planes.
// Outputs:
//    SbBool:    TRUE if bbox is completely outside view volume
//               defined by MVP.
//

//
// How:
//
// An axis-aligned bounding box is the set of all points P,
// Pmin < P < Pmax.  We're interested in finding out whether or not
// any of those points P are clipped after being transformed through
// MVP (transformed into clip space).
//
// A transformed point P'[x,y,z,w] is inside the view if [x,y,z] are
// all between -w and w.  Otherwise the point is outside the view.
//
// Instead of testing individual points, we want to treat the range of
// points P.  We want to know if:  All points P are clipped (in which
// case the cull test succeeds), all points P are NOT clipped (in
// which case they are completely inside the view volume and no more
// cull tests need be done for objects inside P), or some points are
// clipped and some aren't.
//
// P transformed into clip space is a 4-dimensional solid, P'.  To
// speed things up, this algorithm finds the 4-dimensional,
// axis-aligned bounding box of that solid and figures out whether or
// not that bounding box intersects any of the sides of the view
// volume.  In the 4D space with axes x,y,z,w, the view volume planes
// are the planes x=w, x=-w, y=w, y=-w, z=w, z=-w.
//
// This is all easier to think about if we think about each of the X,
// Y, Z axes in clip space independently; worrying only about the X
// axis for a moment:
//
// The idea is to find the minimum and maximum possible X,W
// coordinates of P'.  If all of the points in the
// [(Xmin,Xmax),(Wmin,Wmax)] range satisfy -|W| < X < |W| (|W| is
// absolute value of W), then the original bounding box P is
// completely inside the X-axis (left/right) clipping planes of the
// view volume.  In (x,w) space, a point (x,w) is clipped depending on
// which quadrant it is in:
//
//    x=-w       x=w
//      \   Q0   /
//       \  IN  /
//        \    /
//         \  /
// Q1       \/  Q2
// CLIPPED  /\  CLIPPED    
//         /  \
//        /    \
//       /  Q3  \
//      / CLIPPED\
//
// If the axis-aligned box [(Xmin,Xmax),(Wmin,Wmax)] lies entirely in
// Q0, then it is entirely inside the X-axis clipping planes (IN
// case).  If it is not in Q0 at all, then it is clipped (OUT).  If it
// straddles Q0 and some other quadrant, the bounding box intersects
// the clipping planes (STRADDLE).  The 4 corners of the bounding box
// are tested first using bitwise tests on their quadrant numbers; if
// they determine the case is STRADDLE then a more refined test is
// done on the 8 points of the original bounding box.
// The test isn't perfect-- a bounding box that straddles both Q1 and
// Q2 may be incorrectly classified as STRADDLE; however, those cases
// are rare and the cases that are incorrectly classified will likely
// be culled when testing against the other clipping planes (these
// cases are cases where the bounding box is near the eye).
// 
// Finding [(Xmin,Xmax),(Wmin,Wmax)] is easy.  Consider Xmin.  It is
// the smallest X coordinate when all of the points in the range
// Pmin,Pmax are transformed by MVP; written out:
//     X = P[0]*M[0][0] + P[1]*M[1][0] + P[2]*M[2][0] + M[3][0]
// X will be minimized when each of the terms is minimized.  If the
// matrix entry for the term is positive, then the term is minimized
// by choosing Pmin; if the matrix entry is negative, the term is
// minimized by choosing Pmax.  Three 'if' test will let us calculate
// the transformed Xmin.  Xmax can be calculated similarly.
// 
// Testing for IN/OUT/STRADDLE for the Y and Z coordinates is done
// exactly the same way.
//

//
// Helper functions:
// 
// Given a range in object space, find the minimum or maximum for the
// X,Y,Z or W coordinates in the transformed space.
//    3 multiplies, 3 adds, 3 comparisons/branches.
// Reverse min and max for the opposite test...
static inline float
minExtreme(const SbVec3f &min, const SbVec3f &max, const
	   SbMatrix &MVP, int whichCoord) {
    return
	(MVP[0][whichCoord]>0.0f ? min[0] : max[0])*MVP[0][whichCoord] +
	(MVP[1][whichCoord]>0.0f ? min[1] : max[1])*MVP[1][whichCoord] +
	(MVP[2][whichCoord]>0.0f ? min[2] : max[2])*MVP[2][whichCoord] +
	MVP[3][whichCoord];
}

static inline int
quadrant(float x, float y) {
    return (x < -y) | ((x > y) << 1);
}

static int
findQuadrant(float x, float y, float z,
	     int n, const SbMatrix  &MVP)
{
    float c = MVP[0][n]*x + MVP[1][n]*y + MVP[2][n]*z + MVP[3][n] ;
    float w = MVP[0][3]*x + MVP[1][3]*y + MVP[2][3]*z + MVP[3][3] ;
    return quadrant(c, w);
}

SbBool
SbBox3f::outside(const SbMatrix &MVP, int &cullBits) const
{
    float Wmax = minExtreme(max, min, MVP, 3);
    if (Wmax < 0) return TRUE;

    float Wmin = minExtreme(min, max, MVP, 3);

    // Do each coordinate:
    for (int i = 0; i < 3; i++) {
        if (cullBits & (1<<i)) {  // STRADDLES:
	    float Cmin = minExtreme(min, max, MVP, i);

	    // The and_bits and or_bits are used to keep track of
	    // which quadrants point lie in.  The cases are:
	    //
	    // All in Q0:  IN
	    //    (or_bits == 0)  --> and_bits MUST also be 0
	    // Some/all in Q1, some/all in Q3: CULLED
	    // Some/all in Q2, some/none in Q3: CULLED
	    //    (and_bits != 0, or_bits != 0)
	    // Some in Q1, some in Q2, some/none in Q3: STRADDLE
	    //    (and_bits == 0, or_bits !=0)
	    //
	    int and_bits;
	    int or_bits;
	    and_bits = or_bits = quadrant(Cmin, Wmin);

	    int q0 = quadrant(Cmin, Wmax);
	    and_bits &= q0;
	    or_bits |= q0;
	    // Hit the STRADDLE case as soon as and_bits == 0 and
	    // or_bits != 0:
	    if (!(and_bits == 0 && or_bits != 0)) {
		float Cmax = minExtreme(max, min, MVP, i);

		q0 = quadrant(Cmax, Wmin);
		and_bits &= q0;
		or_bits |= q0;
		if (!(and_bits == 0 && or_bits != 0)) {
		    q0 = quadrant(Cmax, Wmax);
		    and_bits &= q0;
		    or_bits |= q0;
		    
		    // Either completely IN or completely OUT:
		    if (or_bits == 0) { // IN
			cullBits &= ~(1<<i); // Clear bit
			continue; // Continue for loop
		    }
		    else if (and_bits != 0) {
			return TRUE;  // CULLED
		    }
		}
	    }

	    // Before we give up and just claim it straddles, do a
	    // more refined test-- check the 8 corners of the
	    // bounding box:

	    // Test to see if all 8 corner points of the bounding box
	    // are in the same quadrant.  If so, the object is either
	    // completely in or out of the view.  Otherwise, it straddles
	    // at least one of the view boundaries.
	    and_bits = or_bits = findQuadrant(min[0], min[1], min[2], i, MVP);
	    if (and_bits == 0 && or_bits != 0) continue;

	    q0 = findQuadrant(max[0], max[1], max[2], i, MVP);
	    and_bits &= q0;
	    or_bits |= q0;
	    if (and_bits == 0 && or_bits != 0) continue;

	    q0 = findQuadrant(max[0], min[1], min[2], i, MVP);
	    and_bits &= q0;
	    or_bits |= q0;
	    if (and_bits == 0 && or_bits != 0) continue;

	    q0 = findQuadrant(min[0], max[1], max[2], i, MVP);
	    and_bits &= q0;
	    or_bits |= q0;
	    if (and_bits == 0 && or_bits != 0) continue;

	    q0 = findQuadrant(min[0], max[1], min[2], i, MVP);
	    and_bits &= q0;
	    or_bits |= q0;
	    if (and_bits == 0 && or_bits != 0) continue;

	    q0 = findQuadrant(max[0], min[1], max[2], i, MVP);
	    and_bits &= q0;
	    or_bits |= q0;
	    if (and_bits == 0 && or_bits != 0) continue;

	    q0 = findQuadrant(max[0], max[1], min[2], i, MVP);
	    and_bits &= q0;
	    or_bits |= q0;
	    if (and_bits == 0 && or_bits != 0) continue;

	    q0 = findQuadrant(min[0], min[1], max[2], i, MVP);
	    and_bits &= q0;
	    or_bits |= q0;

	    // Either completely IN or completely OUT:
	    if (or_bits == 0) { // IN
		cullBits &= ~(1<<i); // Clear bit
		continue; // Continue for loop
	    }
	    else if (and_bits != 0) {
		return TRUE;  // CULLED
	    }
        }
    }
    return FALSE;  // Not culled
}

//
// Sets Box3f to contain nothing
//

void
SbBox3f::makeEmpty()
{
    min.setValue(FLT_MAX, FLT_MAX, FLT_MAX);
    max.setValue(- FLT_MAX, - FLT_MAX, - FLT_MAX);
}



////////////////////////////////////////////////////////////////////////
//
// Description:
//  Gets the closest point to the box to the given point. If the
//  given point is dead center, returns the point centered on the
//  positive X side.
//
// Use: public

SbVec3f
SbBox3f::getClosestPoint(const SbVec3f &point)
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f result;

    // trivial cases first
    if (isEmpty())
	return point;
    else if (point == getCenter()) {
	// middle of z side
	result[0] = (max[0] + min[0])/2.0;
	result[1] = (max[1] + min[1])/2.0;
	result[2] = max[2];
    }
    else {
	// Find the closest point on a unit box (from -1 to 1),
	// then scale up.

	// Find the vector from center to the point, then scale
	// to a unit box.
	SbVec3f vec = point - getCenter();
	float sizeX, sizeY, sizeZ;
	getSize(sizeX, sizeY, sizeZ);
	float halfX = sizeX/2.0;
	float halfY = sizeY/2.0;
	float halfZ = sizeZ/2.0;
	if (halfX > 0.0)
	    vec[0] /= halfX;
	if (halfY > 0.0)
	    vec[1] /= halfY;
	if (halfZ > 0.0)
	    vec[2] /= halfZ;

	// Side to snap side that has greatest magnitude in the vector.
	SbVec3f mag;
	mag[0] = fabs(vec[0]);
	mag[1] = fabs(vec[1]);
	mag[2] = fabs(vec[2]);

	result = mag;

	// Check if beyond corners
	if (result[0] > 1.0)
	    result[0] = 1.0;
	if (result[1] > 1.0)
	    result[1] = 1.0;
	if (result[2] > 1.0)
	    result[2] = 1.0;

	// snap to appropriate side	    
	if ((mag[0] > mag[1]) && (mag[0] >  mag[2])) {
	    result[0] = 1.0;
	}
	else if ((mag[1] > mag[0]) && (mag[1] >  mag[2])) {
	    result[1] = 1.0;
	}
	else if ((mag[2] > mag[0]) && (mag[2] >  mag[1])) {
	    result[2] = 1.0;
	}
	else if ((mag[0] == mag[1]) && (mag[0] == mag[2])) {
	    // corner
	    result = SbVec3f(1,1,1);
	}
	else if (mag[0] == mag[1]) {
	    // edge parallel with z
	    result[0] = 1.0;
	    result[1] = 1.0;
	}
	else if (mag[0] == mag[2]) {
	    // edge parallel with y
	    result[0] = 1.0;
	    result[2] = 1.0;
	}
	else if (mag[1] == mag[2]) {
	    // edge parallel with x
	    result[1] = 1.0;
	    result[2] = 1.0;
	}
#ifdef DEBUG
	else
	    SoDebugError::post("SbBox3f::getClosestPoint",
			       "Can't determine vector to point");
#endif
	// Now make everything point the right way
	for (int i=0; i < 3; i++)
	    if (vec[i] < 0.0)
		result[i] = -result[i];

	// scale back up and move to center
	result[0] *= halfX;
	result[1] *= halfY;
	result[2] *= halfZ;

	result += getCenter();
    }

    return result;
}

//
// Finds the span of a bounding box along a particular direction
//

void
SbBox3f::getSpan(const SbVec3f &direction, float &dMin, float &dMax) const
{
    int		i;
    SbVec3f	points[8];
    SbVec3f	dir = direction;

    dir.normalize();

    /* Set up the eight points at the corners of the extent */
    points[0][2] = points[2][2] = points[4][2] = points[6][2] = min[2];
    points[1][2] = points[3][2] = points[5][2] = points[7][2] = max[2];

    points[0][0] = points[1][0] = points[2][0] = points[3][0] = min[0];
    points[4][0] = points[5][0] = points[6][0] = points[7][0] = max[0];

    points[0][1] = points[1][1] = points[4][1] = points[5][1] = min[1];
    points[2][1] = points[3][1] = points[6][1] = points[7][1] = max[1];

    points[0][2] = points[2][2] = points[4][2] = points[6][2] = min[2];
    points[1][2] = points[3][2] = points[5][2] = points[7][2] = max[2];

    dMin = FLT_MAX;
    dMax = - FLT_MAX;

    for (i = 0; i < 8; i++) {
	float proj = points[i].dot(dir);
	if (proj < dMin)
	    dMin = proj;
	if (proj > dMax)
	    dMax = proj;
    }
}

//
// Transforms Box3f by matrix, enlarging Box3f to contain result.
// Clever method courtesy of Graphics Gems, pp. 548-550
//
// This works for projection matrices as well as simple affine
// transformations.  Coordinates of the box are rehomogenized if there
// is a projection matrix
//

void
SbBox3f::transform(const SbMatrix &m)
{
    // a transformed empty box is still empty
    if (isEmpty())
	return;

    SbVec3f	newMin, newMax;
    int		i;

    for (i = 0; i < 3; i++) {
	newMin[i] = minExtreme(min, max, m, i);
	newMax[i] = minExtreme(max, min, m, i);
    }
    float Wmin = minExtreme(min, max, m, 3);
    float Wmax = minExtreme(max, min, m, 3);

    // Division by small W's make things bigger; wacky things happen
    // if W's are negative, but negative W's are wacky so I think
    // that's OK:

    newMin /= Wmax;
    newMax /= Wmin;

    min = newMin;
    max = newMax;
}


//
// Finds the volume of the box (0 for an empty box)
//

float
SbBox3f::getVolume() const
{
    if (isEmpty())
	return 0.0;

    return (max[0] - min[0]) * (max[1] - min[1]) * (max[2] - min[2]);
}

//////////////////////////////////////////////////////////////////////////////
//
// Equality comparison operator. 
//
int
operator ==(const SbBox3f &b1, const SbBox3f &b2)
//////////////////////////////////////////////////////////////////////////////
{
    return ( (b1.min == b2.min) && (b1.max == b2.max ) );
}

//
// Default constructor - leaves box totally empty
//
SbXfBox3f::SbXfBox3f()
{
    xform.makeIdentity();
    xformInv.makeIdentity();
    makeEmpty();
}

//
// Constructor given minimum and maximum points 
//
SbXfBox3f::SbXfBox3f(const SbVec3f &_min, const SbVec3f &_max)
{
    xform.makeIdentity();
    xformInv.makeIdentity();
    setBounds(_min, _max);
}

//
// Constructor given Box3f
//
SbXfBox3f::SbXfBox3f(const SbBox3f &box)
{
    xform.makeIdentity();
    xformInv.makeIdentity();
    *(SbBox3f *)this = box;
}

#define PRECISION_LIMIT (1.0e-13)

//
// Set the transformation on the box.  This is careful about
// non-invertable transformations.
//
void
SbXfBox3f::setTransform(const SbMatrix &m)
{
    xform = m; 
    
    // Check for degenerate matrix:
    float det = m.det4();
    if (det < PRECISION_LIMIT && det > -PRECISION_LIMIT) {
	// We'll mark inverse[0][0] with FLT_MAX (max floating point
	// value) as special value to indicate degenerate transform.
	xformInv = SbMatrix(FLT_MAX,0,0,0,
			    0,0,0,0,   0,0,0,0,  0,0,0,0);
    } else {
	xformInv = m.inverse();
    }
}

#undef PRECISION_LIMIT

//
// Return the center of a box
//
SbVec3f
SbXfBox3f::getCenter() const
{
    SbVec3f	p;

    // transform the center before returning it
    xform.multVecMatrix(.5 * (getMin() + getMax()), p);

    return p;
}

//
// Extend (if necessary) to contain given 3D point
//
void
SbXfBox3f::extendBy(const SbVec3f &pt)
{
    // If our transform is degenerate, project this box, which will
    // transform min/max and get a box with identity xforms:
    if (xformInv[0][0] == FLT_MAX) {
	*this = SbXfBox3f(this->project());
    }
    
    SbVec3f p;
    xformInv.multVecMatrix(pt, p);
    SbBox3f::extendBy(p);
}

//
// Finds the volume of the box (0 for an empty box)
//

float
SbXfBox3f::getVolume() const
{
    if (isEmpty())
	return 0.0;

    // The volume of a transformed box is just its untransformed
    // volume times the determinant of the upper-left 3x3 of
    // the xform matrix. Quoth Paul Strauss: "Pretty cool, indeed."
    float objVol = SbBox3f::getVolume();
    float factor = xform.det3();
    return factor * objVol;
}

//
// Extends XfBox3f (if necessary) to contain given XfBox3f
//

void
SbXfBox3f::extendBy(const SbXfBox3f &bb)
{
    if (bb.isEmpty())			// bb is empty, no change
	return;
    else if (isEmpty())			// we're empty, use bb
	*this = bb;

    else if (xformInv[0][0] != FLT_MAX && bb.xformInv[0][0] != FLT_MAX) {
	// Neither box is empty and they are in different spaces. To
	// get the best results, we'll perform the merge of the two
	// boxes in each of the two spaces. Whichever merge ends up
	// being smaller is the one we'll use.
	// Note that we don't perform a project() as part of the test.
	// This is because projecting almost always adds a little extra
	// space. It also gives an unfair advantage to the
	// box more closely aligned with world space.  In the simplest
	// case this might be preferable. However, over many objects,
	// we are better off going with the minimum in local space,
	// and not worrying about projecting until the very end.

	SbXfBox3f	xfbox1, xfbox2;
	SbBox3f		box1, box2;

	// Convert bb into this's space to get box1
	xfbox1 = bb;
	// Rather than calling transform(), which calls inverse(),
	// we'll do it ourselves, since we already know the inverse matrix.
	// I.e., we could call: xfbox1.transform(xformInv);
	xfbox1.xform *= xformInv;
	xfbox1.xformInv.multRight(xform);
	box1 = xfbox1.project();

	// Convert this into bb's space to get box2
	xfbox2 = *this;
	// Same here for: xfbox2.transform(bb.xformInv);
	xfbox2.xform *= bb.xformInv;
	xfbox2.xformInv.multRight(bb.xform);
	box2 = xfbox2.project();

	// Extend this by box1 to get xfbox1
	xfbox1 = *this;
	xfbox1.SbBox3f::extendBy(box1);
	// Use SbBox3f method; box1 is already in xfbox1's space
	// (otherwise, we'll get an infinite loop!)

	// Extend bb by box2 to get xfbox2
	xfbox2 = bb;
	xfbox2.SbBox3f::extendBy(box2);
	// Use SbBox3f method; box2 is already in xfbox2's space
	// (otherwise, we'll get an infinite loop!)

	float vol1 = xfbox1.getVolume();
	float vol2 = xfbox2.getVolume();

	// Take the smaller result and extend appropriately
	if (vol1 <= vol2) {
	    SbBox3f::extendBy(box1);
	}
	else {
	    *this = bb;
	    SbBox3f::extendBy(box2);
	}
    }
    else if (xformInv[0][0] == FLT_MAX) {
	if (bb.xformInv[0][0] == FLT_MAX) {
	    // Both boxes are degenerate; project them both and
	    // combine them:
	    SbBox3f box = this->project();
	    box.extendBy(bb.project());
	    *this = SbXfBox3f(box);
	} else {
	    // this is degenerate; transform our min/max into bb's
	    // space, and combine there:
	    SbBox3f box(getMin(), getMax());
	    box.transform(xform*bb.xformInv);
	    *this = bb;
	    SbBox3f::extendBy(box);
	}
    } else {
	// bb is degenerate; transform it into our space and combine:
	SbBox3f box(bb.getMin(), bb.getMax());
	box.transform(bb.xform*xformInv);
	SbBox3f::extendBy(box);
    }
}

//
// Returns TRUE if intersection of given point and Box3f is not empty
// (being careful about degenerate transformations...).
//
SbBool
SbXfBox3f::intersect(const SbVec3f &pt) const
{
    if (xformInv[0][0] != FLT_MAX) {
	SbVec3f p;
	xformInv.multVecMatrix(pt, p);
	return SbBox3f::intersect(p);
    }
    SbBox3f box = this->project();  // Degenerate; project and test:
    return box.intersect(pt);
}

//
// Transform this box by a matrix
//
void
SbXfBox3f::transform(const SbMatrix &m) 
{
    SbMatrix new_xf = xform*m;
    setTransform(new_xf);
}
    

//////////////////////////////////////////////////////////////////////////////
//
// Equality comparison operator. 
//
int
operator ==(const SbXfBox3f &b1, const SbXfBox3f &b2)
//////////////////////////////////////////////////////////////////////////////
{
    SbBox3f b1Proj = b1.project();
    SbBox3f b2Proj = b2.project();
    return (b1Proj == b2Proj);
}

//
// Projects an SbXfBox to an SbBox
//

SbBox3f
SbXfBox3f::project() const
{
    SbBox3f	box(getMin(), getMax());
    box.transform(xform);
    return box;
}

//
// Return the center of a box
//

SbVec2f
SbBox2f::getCenter() const
{
    return SbVec2f(0.5 * (min[0] + max[0]),
		   0.5 * (min[1] + max[1]));
}

//////////////////////////////////////////////////////////////////////////////
//
//  Extends Box2f (if necessary) to contain given 2D point
//
void
SbBox2f::extendBy(const SbVec2f &pt)
//
//////////////////////////////////////////////////////////////////////////////
{
    if (pt[0] < min[0]) min[0] = pt[0];
    if (pt[0] > max[0]) max[0] = pt[0];

    if (pt[1] < min[1]) min[1] = pt[1];
    if (pt[1] > max[1]) max[1] = pt[1];
}

//////////////////////////////////////////////////////////////////////////////
//
// Extends Box2f (if necessary) to contain given Box2f
//
void
SbBox2f::extendBy(const SbBox2f &r)
//
//////////////////////////////////////////////////////////////////////////////
{
    if (r.min[0] < min[0]) min[0] = r.min[0];
    if (r.max[0] > max[0]) max[0] = r.max[0];
    if (r.min[1] < min[1]) min[1] = r.min[1];
    if (r.max[1] > max[1]) max[1] = r.max[1];
}

//////////////////////////////////////////////////////////////////////////////
//
// Returns TRUE if intersection of given point and Box2f is not empty
//
SbBool
SbBox2f::intersect(const SbVec2f &pt) const
//
//////////////////////////////////////////////////////////////////////////////
{
    return ((pt[0] >= min[0]) &&
	    (pt[1] >= min[1]) &&
	    (pt[0] <= max[0]) &&
	    (pt[1] <= max[1]));
}

//////////////////////////////////////////////////////////////////////////////
//
// Returns TRUE if intersection of given Box2f and Box2f is not empty
//
SbBool
SbBox2f::intersect(const SbBox2f &r) const
//
//////////////////////////////////////////////////////////////////////////////
{
    return ((r.max[0] >= min[0]) && (r.min[0] <= max[0]) &&
	    (r.max[1] >= min[1]) && (r.min[1] <= max[1]));
}


//////////////////////////////////////////////////////////////////////////////
//
// Sets Box2f to contain nothing
//
void
SbBox2f::makeEmpty()
//
//////////////////////////////////////////////////////////////////////////////
{
    min.setValue( FLT_MAX,  FLT_MAX);
    max.setValue(-FLT_MAX, -FLT_MAX);
}

//////////////////////////////////////////////////////////////////////////////
//
// Equality comparison operator. 
//
int
operator ==(const SbBox2f &b1, const SbBox2f &b2)
//////////////////////////////////////////////////////////////////////////////
{
    return ( (b1.min == b2.min) && (b1.max == b2.max ) );
}


//////////////////////////////////////////////////////////////////////////////
//
//  Extends Box2s (if necessary) to contain given 2D point
//
void
SbBox2s::extendBy(const SbVec2s &pt)
//
//////////////////////////////////////////////////////////////////////////////
{
    if (pt[0] < min[0]) min[0] = pt[0];
    if (pt[0] > max[0]) max[0] = pt[0];

    if (pt[1] < min[1]) min[1] = pt[1];
    if (pt[1] > max[1]) max[1] = pt[1];
}

//////////////////////////////////////////////////////////////////////////////
//
// Extends Box2s (if necessary) to contain given Box2s
//
void
SbBox2s::extendBy(const SbBox2s &r)
//
//////////////////////////////////////////////////////////////////////////////
{
    if (r.min[0] < min[0]) min[0] = r.min[0];
    if (r.max[0] > max[0]) max[0] = r.max[0];
    if (r.min[1] < min[1]) min[1] = r.min[1];
    if (r.max[1] > max[1]) max[1] = r.max[1];
}

//////////////////////////////////////////////////////////////////////////////
//
// Returns TRUE if intersection of given point and Box2s is not empty
//
SbBool
SbBox2s::intersect(const SbVec2s &pt) const
//
//////////////////////////////////////////////////////////////////////////////
{
    return ((pt[0] >= min[0]) &&
	    (pt[1] >= min[1]) &&
	    (pt[0] <= max[0]) &&
	    (pt[1] <= max[1]));
}

//////////////////////////////////////////////////////////////////////////////
//
// Returns TRUE if intersection of given Box2s and Box2s is not empty
//
SbBool
SbBox2s::intersect(const SbBox2s &r) const
//
//////////////////////////////////////////////////////////////////////////////
{
    return ((r.max[0] >= min[0]) && (r.min[0] <= max[0]) &&
	    (r.max[1] >= min[1]) && (r.min[1] <= max[1]));
}


//////////////////////////////////////////////////////////////////////////////
//
// Sets Box2s to contain nothing
//
void
SbBox2s::makeEmpty()
//
//////////////////////////////////////////////////////////////////////////////
{
    min.setValue(SHRT_MAX, SHRT_MAX);
    max.setValue(SHRT_MIN, SHRT_MIN);
}

//////////////////////////////////////////////////////////////////////////////
//
// Equality comparison operator. 
//
int
operator ==(const SbBox2s &b1, const SbBox2s &b2)
//////////////////////////////////////////////////////////////////////////////
{
    return ( (b1.min == b2.min) && (b1.max == b2.max ) );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  Gets the closest point to the box to the given point. If the
//  given point is dead center, returns the point centered on the
//  positive X side.
//
// Use: public

SbVec2f
SbBox2f::getClosestPoint(const SbVec2f &point)
//
////////////////////////////////////////////////////////////////////////
{
    SbVec2f result;

    // trivial cases first
    if (isEmpty())
	return point;
    else if (point == getCenter()) {
	// middle of x side
	result[0] = max[0];
	result[1] = (max[1] + min[1])/2.0;
    }
    else if (min[0] == max[0]) {
	result[0] = min[0];
	result[1] = point[1];
    }
    else if (min[1] == max[1]) {
	result[0] = point[0];
	result[1] = min[1];
    }
    else {
	// Find the closest point on a unit box (from -1 to 1),
	// then scale up.

	// Find the vector from center to the point, then scale
	// to a unit box.
	SbVec2f vec = point - getCenter();
	float sizeX, sizeY;
	getSize(sizeX, sizeY);
	float halfX = sizeX/2.0;
	float halfY = sizeY/2.0;
	if (halfX > 0.0)
	    vec[0] /= halfX;
	if (halfY > 0.0)
	    vec[1] /= halfY;

	// Side to snap to has greatest magnitude in the vector.
	float magX = fabs(vec[0]);
	float magY = fabs(vec[1]);

	if (magX > magY) {
	    result[0] = (vec[0] > 0) ? 1.0 : -1.0;
	    if (magY > 1.0)
		magY = 1.0;
	    result[1] = (vec[1] > 0) ? magY : -magY;
	}
	else if (magY > magX) {
	    if (magX > 1.0)
		magX = 1.0;
	    result[0] = (vec[0] > 0) ? magX : -magX;
	    result[1] = (vec[1] > 0) ? 1.0 : -1.0;
	}
	else {
	    // must be one of the corners
	    result[0] = (vec[0] > 0) ? 1.0 : -1.0;
	    result[1] = (vec[1] > 0) ? 1.0 : -1.0;
	}

	// scale back the result and move it to the center of the box
	result[0] *= halfX;
	result[1] *= halfY;
	result += getCenter();
    }

    return result;
}
