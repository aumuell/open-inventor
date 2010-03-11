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
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |	SbPlane
 |
 |   Author(s)		: Nick Thompson
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SbLinear.h>

//////////////////////////////////////////////////////////////////////////////
//
// Plane class
//
//////////////////////////////////////////////////////////////////////////////

// construct a plane given 3 points (and orientation?)
SbPlane::SbPlane(const SbVec3f &p0, const SbVec3f &p1, const SbVec3f &p2)
{
    normalVec = (p1 - p0).cross(p2 - p0);
    normalVec.normalize();
    distance = normalVec.dot(p0);
}

// construct a plane given normal and distance from origin along normal
SbPlane::SbPlane(const SbVec3f &n, float d)
{
    normalVec = n;
    normalVec.normalize();
    distance = d;
}

// construct a plane given normal and a point to pass through
SbPlane::SbPlane(const SbVec3f &n, const SbVec3f &p)
{
    normalVec = n;
    normalVec.normalize();
    distance = normalVec.dot(p);
}

// offset a plane by a given distance
void
SbPlane::offset(float d)
{
    distance += d;
}

// intersect line and plane
SbBool
SbPlane::intersect(const SbLine &l, SbVec3f &intersection) const
{
    float t, denom;

    // solve for t:
    //  n . (l.p + t * l.d) - d == 0

    denom = normalVec.dot(l.getDirection());
    if ( denom == 0.0 )
	return FALSE;

    //  t = - (n . l.p - d) / (n . l.d)
    t = - (normalVec.dot(l.getPosition()) - distance) /  denom;

    intersection = l.getPosition() + t * l.getDirection();
    return TRUE;
}

// transform a plane by the given matrix
void
SbPlane::transform(const SbMatrix &matrix)
{
    // Find the point on the plane along the normal from the origin
    SbVec3f	point = distance * normalVec;

    // Transform the plane normal by the matrix
    // to get the new normal. Use the inverse transpose
    // of the matrix so that normals are not scaled incorrectly.
    SbMatrix invTran = matrix.inverse().transpose();
    invTran.multDirMatrix(normalVec, normalVec);
    normalVec.normalize();

    // Transform the point by the matrix
    matrix.multVecMatrix(point, point);

    // The new distance is the projected distance of the vector to the
    // transformed point onto the (unit) transformed normal. This is
    // just a dot product.
    distance = point.dot(normalVec);
}

// Returns TRUE if the given point is within the half-space defined by
// the plane
SbBool
SbPlane::isInHalfSpace(const SbVec3f &point) const
{
    // Multiply point by plane equation coefficients, compare distances
    return point.dot(normalVec) >= distance;
}

// Equality comparison operator
int
operator ==(const SbPlane &p1, const SbPlane &p2)
{
    return (p1.distance == p2.distance && p1.normalVec == p2.normalVec);
}
