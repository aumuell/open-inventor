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
 |	SbRotation
 |
 |   Author(s)		: Nick Thompson
 |
 |   Heavily based on code by Ken Shoemake and code by Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SbLinear.h>
#include <Inventor/errors/SoDebugError.h>

// amount squared to figure if two floats are equal
#define DELTA 1e-6

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns 4 individual components of rotation quaternion.
//
// Use: public

void
SbRotation::getValue(float &q0, float &q1, float &q2, float &q3) const
//
////////////////////////////////////////////////////////////////////////
{
    q0 = quat[0];
    q1 = quat[1];
    q2 = quat[2];
    q3 = quat[3];
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns corresponding 3D rotation axis vector and angle in radians.
//
// Use: public

void
SbRotation::getValue(SbVec3f &axis, float &radians) const
//
////////////////////////////////////////////////////////////////////////
{
    float	len;
    SbVec3f	q;

    q[0] = quat[0];
    q[1] = quat[1];
    q[2] = quat[2];

    if ((len = q.length()) > 0.00001) {
	axis	= q * (1.0 / len);
	radians	= 2.0 * acosf(quat[3]);
    }

    else {
	axis.setValue(0.0, 0.0, 1.0);
	radians = 0.0;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns corresponding 4x4 rotation matrix.
//
// Use: public

void
SbRotation::getValue(SbMatrix &matrix) const
//
////////////////////////////////////////////////////////////////////////
{
    SbMat m;

    m[0][0] = 1 - 2.0 * (quat[1] * quat[1] + quat[2] * quat[2]);
    m[0][1] =     2.0 * (quat[0] * quat[1] + quat[2] * quat[3]);
    m[0][2] =     2.0 * (quat[2] * quat[0] - quat[1] * quat[3]);
    m[0][3] = 0.0;

    m[1][0] =     2.0 * (quat[0] * quat[1] - quat[2] * quat[3]);
    m[1][1] = 1 - 2.0 * (quat[2] * quat[2] + quat[0] * quat[0]);
    m[1][2] =     2.0 * (quat[1] * quat[2] + quat[0] * quat[3]);
    m[1][3] = 0.0;

    m[2][0] =     2.0 * (quat[2] * quat[0] + quat[1] * quat[3]);
    m[2][1] =     2.0 * (quat[1] * quat[2] - quat[0] * quat[3]);
    m[2][2] = 1 - 2.0 * (quat[1] * quat[1] + quat[0] * quat[0]);
    m[2][3] = 0.0;

    m[3][0] = 0.0;
    m[3][1] = 0.0;
    m[3][2] = 0.0;
    m[3][3] = 1.0;

    matrix = m;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Changes a rotation to be its inverse.
//
// Use: public

SbRotation &
SbRotation::invert()
//
////////////////////////////////////////////////////////////////////////
{
    float invNorm = 1.0 / norm();

    quat[0] = -quat[0] * invNorm;
    quat[1] = -quat[1] * invNorm;
    quat[2] = -quat[2] * invNorm;
    quat[3] =  quat[3] * invNorm;

    return *this;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets value of rotation from array of 4 components of a
//    quaternion.
//
// Use: public

SbRotation &
SbRotation::setValue(const float q[4])
//
////////////////////////////////////////////////////////////////////////
{
    quat[0] = q[0];
    quat[1] = q[1];
    quat[2] = q[2];
    quat[3] = q[3];
    normalize();

    return (*this);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets value of rotation from 4 individual components of a
//    quaternion.
//
// Use: public

SbRotation &
SbRotation::setValue(float q0, float q1, float q2, float q3)
//
////////////////////////////////////////////////////////////////////////
{
    quat[0] = q0;
    quat[1] = q1;
    quat[2] = q2;
    quat[3] = q3;
    normalize();

    return (*this);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets value of rotation from a rotation matrix.
//    This algorithm is from "Quaternions and 4x4 Matrices", Ken
//    Shoemake, Graphics Gems II.
//
//  Here's the logic:
//    We're trying to find a quaterion 'q' that represents the same
//    rotation as the given matrix 'm'.
//    We know how to convert a quaterion to a rotation matrix; the
//    matrix is given by (where x,y,z,w are the quaterion elements):
//
//    x^2-y^2-z^2+w^2  2xy+2zw          2wx - 2yw        0
//    2xy-2zw          y^2-z^2-x^2+w^2  2yz + 2xw        0
//    2zx+2zw          2yz-2xw          z^2-x^2-y^2+w^2  0
//    0                0                0                x^2+y^2+z^2+w^2
//             (note that x^2+y^2+z^2+w^2==1 for a normalized quaterion)
//
//    We know m, we want to find x,y,z,w.  If you don't mind doing
//    square roots, then it is easy; for example:
//    m[0][0]+m[1][1]+m[2][2]+m[3][3] = 4w^2
//       (write it all out, and see all the terms cancel)
//    Or, w = sqrt(m[0][0]+m[1][1]+m[2][2]+m[3][3])/2
//    Similarly,
//        x = sqrt(m[0][0]-m[1][1]-m[2][2]+m[3][3])/2
//        y = sqrt(m[1][1]-m[2][2]-m[0][0]+m[3][3])/2
//        z = sqrt(m[2][2]-m[0][0]-m[1][1]+m[3][3])/2
//    However, you only really need to do one sqrt and find one of
//    x,y,z,w, because using the other elements of the matrix you
//    find, for example, that:
//    m[0][1]+m[1][0] = 2xy+2zw+2xy-2zw = 4xy
//    So if you know either x or y, you can find the other.
//
//    That is assuming that the first thing you find isn't zero, of
//    course.  In fact, you want the first element you find to be as
//    large as possible, to get more accuracy in the division.  You
//    can rewrite the diagonal elements as:
//    (w^2 - x^2 - y^2 - z^2) + 2x^2    = m[0][0]
//    (w^2 - x^2 - y^2 - z^2) + 2y^2    = m[1][1]
//    (w^2 - x^2 - y^2 - z^2) + 2z^2    = m[2][2]
//    ... and write the sum of the diagonals as:
//    (w^2 - x^2 - y^2 - z^2) + 2w^2    = m[0][0]+m[1][1]+m[2][2]+m[3][3]
//
//    Why do this?  Because now it is easy to see that if x is greater
//    than y, z, or w, then m[0][0] will be greater than the other
//    diagonals or the sum of the diagonals.
//
//    So, the overall strategy is:  Figure out which if x,y,z, or w
//    will be greatest by looking at the diagonals.  Compute that
//    value using the sqrt() formula.  Then compute the other values
//    using the other set of formulas.
//
// Use: public

SbRotation &
SbRotation::setValue(const SbMatrix &m)
//
////////////////////////////////////////////////////////////////////////
{
    int i, j, k;

    // First, find largest diagonal in matrix:
    if (m[0][0] > m[1][1]) {
	if (m[0][0] > m[2][2]) {
	    i = 0;
	}
	else i = 2;
    }
    else {
	if (m[1][1] > m[2][2]) {
	    i = 1;
	}
	else i = 2;
    }
    if (m[0][0]+m[1][1]+m[2][2] > m[i][i]) {
	// Compute w first:
	quat[3] = sqrt(m[0][0]+m[1][1]+m[2][2]+m[3][3])/2.0;

	// And compute other values:
	quat[0] = (m[1][2]-m[2][1])/(4*quat[3]);
	quat[1] = (m[2][0]-m[0][2])/(4*quat[3]);
	quat[2] = (m[0][1]-m[1][0])/(4*quat[3]);
    }
    else {
	// Compute x, y, or z first:
	j = (i+1)%3; k = (i+2)%3;
    
	// Compute first value:
	quat[i] = sqrt(m[i][i]-m[j][j]-m[k][k]+m[3][3])/2.0;
       
	// And the others:
	quat[j] = (m[i][j]+m[j][i])/(4*quat[i]);
	quat[k] = (m[i][k]+m[k][i])/(4*quat[i]);

	quat[3] = (m[j][k]-m[k][j])/(4*quat[i]);
    }

#ifdef DEBUG
    // Check to be sure output matches input:
    SbMatrix check;
    getValue(check);
    SbBool ok = TRUE;
    for (i = 0; i < 4 && ok; i++) {
	for (j = 0; j < 4 && ok; j++) {
	    if (fabsf(m[i][j]-check[i][j]) > 1.0e-5)
		ok = FALSE;
	}
    }
    if (!ok) {
	SoDebugError::post("SbRotation::setValue(const SbMatrix &)",
			   "Rotation does not agree with matrix; "
			   "this routine only works with rotation "
			   "matrices!");
    }
#endif

    return (*this);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets value of rotation from 3D rotation axis vector and angle in
//    radians.
//
// Use: public

SbRotation &
SbRotation::setValue(const SbVec3f &axis, float radians)
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f	q;

    q = axis;
    q.normalize();

    q *= sinf(radians / 2.0);

    quat[0] = q[0];
    quat[1] = q[1];
    quat[2] = q[2];

    quat[3] = cosf(radians / 2.0);

    return(*this);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets rotation to rotate one direction vector to another.
//
// Use: public

SbRotation &
SbRotation::setValue(const SbVec3f &rotateFrom, const SbVec3f &rotateTo)
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f	from = rotateFrom;
    SbVec3f	to = rotateTo;
    SbVec3f	axis;
    float	cost;

    from.normalize();
    to.normalize();
    cost = from.dot(to);

    // check for degeneracies
    if (cost > 0.99999) {		// vectors are parallel
	quat[0] = quat[1] = quat[2] = 0.0;
	quat[3] = 1.0;
	return *this;
    }
    else if (cost < -0.99999) {		// vectors are opposite
	// find an axis to rotate around, which should be
	// perpendicular to the original axis
	// Try cross product with (1,0,0) first, if that's one of our
	// original vectors then try  (0,1,0).
	SbVec3f tmp = from.cross(SbVec3f(1.0, 0.0, 0.0));
	if (tmp.length() < 0.00001)
	    tmp = from.cross(SbVec3f(0.0, 1.0, 0.0));

	tmp.normalize();
	setValue(tmp[0], tmp[1], tmp[2], 0.0);
	return *this;
    }

    axis = rotateFrom.cross(rotateTo);
    axis.normalize();

    // use half-angle formulae
    // sin^2 t = ( 1 - cos (2t) ) / 2
    axis *= sqrt(0.5 * (1.0 - cost));

    // scale the axis by the sine of half the rotation angle to get
    // the normalized quaternion
    quat[0] = axis[0];
    quat[1] = axis[1];
    quat[2] = axis[2];

    // cos^2 t = ( 1 + cos (2t) ) / 2
    // w part is cosine of half the rotation angle
    quat[3] = sqrt(0.5 * (1.0 + cost));

    return (*this);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Multiplies by another rotation.
//
// Use: public

SbRotation &
SbRotation::operator *=(const SbRotation &q)
//
////////////////////////////////////////////////////////////////////////
{
    float p0, p1, p2, p3;

    p0 = (q.quat[3] * quat[0] + q.quat[0] * quat[3] +
	  q.quat[1] * quat[2] - q.quat[2] * quat[1]);
    p1 = (q.quat[3] * quat[1] + q.quat[1] * quat[3] +
	  q.quat[2] * quat[0] - q.quat[0] * quat[2]);
    p2 = (q.quat[3] * quat[2] + q.quat[2] * quat[3] +
	  q.quat[0] * quat[1] - q.quat[1] * quat[0]);
    p3 = (q.quat[3] * quat[3] - q.quat[0] * quat[0] -
	  q.quat[1] * quat[1] - q.quat[2] * quat[2]);
    quat[0] = p0;
    quat[1] = p1;
    quat[2] = p2;
    quat[3] = p3;

    normalize();

    return(*this);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Equality comparison operator.
//
// Use: public

int
operator ==(const SbRotation &q1, const SbRotation &q2)
//
////////////////////////////////////////////////////////////////////////
{
    return (q1.quat[0] == q2.quat[0] &&
	    q1.quat[1] == q2.quat[1] &&
	    q1.quat[2] == q2.quat[2] &&
	    q1.quat[3] == q2.quat[3]);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Equality comparison operator within given tolerance - the square
//    of the length of the maximum distance between the two vectors.
//
// Use: public

SbBool
SbRotation::equals(const SbRotation &r, float tolerance) const
//
////////////////////////////////////////////////////////////////////////
{
    return SbVec4f(quat).equals(SbVec4f(r.quat), tolerance);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Binary multiplication operator.
//
// Use: public

SbRotation
operator *(const SbRotation &q1, const SbRotation &q2)
//
////////////////////////////////////////////////////////////////////////
{
    SbRotation q(q2.quat[3] * q1.quat[0] + q2.quat[0] * q1.quat[3] +
		  q2.quat[1] * q1.quat[2] - q2.quat[2] * q1.quat[1],

		  q2.quat[3] * q1.quat[1] + q2.quat[1] * q1.quat[3] +
		  q2.quat[2] * q1.quat[0] - q2.quat[0] * q1.quat[2],

		  q2.quat[3] * q1.quat[2] + q2.quat[2] * q1.quat[3] +
		  q2.quat[0] * q1.quat[1] - q2.quat[1] * q1.quat[0],

		  q2.quat[3] * q1.quat[3] - q2.quat[0] * q1.quat[0] -
		  q2.quat[1] * q1.quat[1] - q2.quat[2] * q1.quat[2]);
    q.normalize();

    return (q);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Puts the given vector through this rotation
//    (Multiplies the given vector by the matrix of this rotation).
//
// Use: public

void
SbRotation::multVec(const SbVec3f &src, SbVec3f &dst) const
//
////////////////////////////////////////////////////////////////////////
{
    SbMatrix myMat;
    getValue( myMat );

    myMat.multVecMatrix( src, dst );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Keep the axis the same. Multiply the angle of rotation by
//    the amount 'scaleFactor'
//
// Use: public

void
SbRotation::scaleAngle(float scaleFactor )
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f myAxis;
    float   myAngle;

    // Get the Axis and angle.
    getValue( myAxis, myAngle );

    setValue( myAxis, (myAngle * scaleFactor) );
}

//
// 
//
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Spherical linear interpolation: as t goes from 0 to 1, returned
//    value goes from rot0 to rot1.
//
// Use: public

SbRotation
SbRotation::slerp(const SbRotation &rot0, const SbRotation &rot1, float t)
//
////////////////////////////////////////////////////////////////////////
{
        const float*    r1q = rot1.getValue();

        SbRotation      rot;
        float           rot1q[4];
        double          omega, cosom, sinom;
        double          scalerot0, scalerot1;
        int             i;

        // Calculate the cosine
        cosom = rot0.quat[0]*rot1.quat[0] + rot0.quat[1]*rot1.quat[1]
                + rot0.quat[2]*rot1.quat[2] + rot0.quat[3]*rot1.quat[3];

        // adjust signs if necessary
        if ( cosom < 0.0 ) {
                cosom = -cosom;
                for ( int j = 0; j < 4; j++ )
                        rot1q[j] = -r1q[j];
        } else  {
                for ( int j = 0; j < 4; j++ )
                        rot1q[j] = r1q[j];
        }

        // calculate interpolating coeffs
        if ( (1.0 - cosom) > 0.00001 ) {
                // standard case
                omega = acos(cosom);
                sinom = sin(omega);
                scalerot0 = sin((1.0 - t) * omega) / sinom;
                scalerot1 = sin(t * omega) / sinom;
        } else {        
                // rot0 and rot1 very close - just do linear interp.
                scalerot0 = 1.0 - t;
                scalerot1 = t;
        }

        // build the new quarternion
        for (i = 0; i <4; i++)
                rot.quat[i] = scalerot0 * rot0.quat[i] + scalerot1 * rot1q[i];

        return rot;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the norm (square of the 4D length) of the quaternion
//    defining the rotation.
//
// Use: private

float
SbRotation::norm() const
//
////////////////////////////////////////////////////////////////////////
{
    return (quat[0] * quat[0] +
	    quat[1] * quat[1] +
	    quat[2] * quat[2] +
	    quat[3] * quat[3]);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Normalizes a rotation quaternion to unit 4D length.
//
// Use: private

void
SbRotation::normalize()
//
////////////////////////////////////////////////////////////////////////
{
    float	dist = 1.0 / sqrt(norm());

    quat[0] *= dist;
    quat[1] *= dist;
    quat[2] *= dist;
    quat[3] *= dist;
}

