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

//  -*- C++ -*-

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.3 $
 |
 |   Description:
 |	This file contains definitions of various linear algebra classes,
 |	such as vectors, coordinates, etc..
 |
 |   Classes:
 |	SbVec3f
 |	SbVec2f
 |	SbVec2s
 |	SbVec4f
 |	SbRotation
 |	SbMatrix
 |	SbViewVolume
 |
 |	SbLine
 |	SbPlane
 |	SbSphere
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson, 
 |			  David Mott, Alain Dumesny
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef _SB_LINEAR_
#define _SB_LINEAR_

#include <math.h>
#include <Inventor/SbBasic.h>

// -----------------------------------
//
// Types/classes defined in this file:
// 
// -----------------------------------

// C-api: end
typedef float SbMat[4][4];

// C-api: begin

class SbVec3f;
class SbVec2f;
class SbVec2s;
class SbVec4f;
class SbRotation;
class SbMatrix;

class SbLine;
class SbPlane;
class SbCylinder;
class SbSphere;

// definition so we can use this in SbViewVolume
class SbBox3f;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SbVec3f
//
//  3D vector used to represent points or directions. Each component of
//  the vector is a floating-point number.
//
//  WARNING!!!!!  Transcription of arrays of this class assume that the
//                only data stored in this class are three consecutive values.
//                Do not add any extra data members!!!
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SbV3f
// C-api.h: struct SbVec3f {
// C-api.h: 	float vec[3];
// C-api.h: };

// C-api: end

class SbVec3f {
  public:
    // Default constructor
    SbVec3f()						{ }

    // Constructor given an array of 3 components
    SbVec3f(const float v[3])
	 { vec[0] = v[0]; vec[1] = v[1]; vec[2] = v[2]; }

    // Constructor given 3 individual components
    SbVec3f(float x, float y, float z)
	 { vec[0] = x; vec[1] = y; vec[2] = z; }

    // Constructor given 3 planes
    SbVec3f(SbPlane &p0, SbPlane &p1, SbPlane &p2);

// C-api: begin
    // Returns right-handed cross product of vector and another vector
    SbVec3f	cross(const SbVec3f &v) const;
// C-api: end

    // Returns dot (inner) product of vector and another vector
    float	dot(const SbVec3f &v) const;
// C-api.h: #define SbV3fDot(_v0, _v1)
// C-api.h:     ((_v0).vec[0] * (_v1).vec[0] + (_v0).vec[1] * (_v1).vec[1] +
// C-api.h:      (_v0).vec[2] * (_v1).vec[2])

    // Returns pointer to array of 3 components
    const float	*getValue() const			{ return vec; }
// C-api.h: #define SbV3fGetXYZ(_xyz, _src)
// C-api.h:     (((_xyz)[0] = (_src).vec[0]), ((_xyz)[1] = (_src).vec[1]),
// C-api.h:	 ((_xyz)[2] = (_src).vec[2]))

    // Returns 3 individual components
    void	getValue(float &x, float &y, float &z) const;
// C-api.h: #define SbV3fGetX_Y_Z(_x, _y, _z, _src)
// C-api.h:     (((_x) = (_src).vec[0]), ((_y) = (_src).vec[1]), ((_z) = (_src).vec[2]))

    // Returns geometric length of vector
    float	length() const;
// C-api.h: #define SbV3fLen(_v)
// C-api.h:     (sqrtf(SbV3fDot((_v), (_v))))

// C-api: begin
    // Changes vector to be unit length
    // C-api: name=norm
    float	normalize();
// C-api: end

    // Negates each component of vector in place
    void	negate();
// C-api.h: #define SbV3fNegate(_v)
// C-api.h: 	SbV3fMultBy(_v, -1.0)

    // Sets value of vector from array of 3 components
    SbVec3f &	setValue(const float v[3])
	 { vec[0] = v[0]; vec[1] = v[1]; vec[2] = v[2]; return *this; }
// C-api.h: #define SbV3fSetXYZ(_dest, _src)
// C-api.h:     (((_dest).vec[0] = (_src)[0]), ((_dest).vec[1] = (_src)[1]),
// C-api.h:      ((_dest).vec[2] = (_src)[2]))

    // Sets value of vector from 3 individual components
    SbVec3f &	setValue(float x, float y, float z)
	 { vec[0] = x; vec[1] = y; vec[2] = z; return *this; }
// C-api.h: #define SbV3fSetX_Y_Z(_dest, _x, _y, _z)
// C-api.h:     (((_dest).vec[0] = (_x)), ((_dest).vec[1] = (_y)),
// C-api.h:      ((_dest).vec[2] = (_z)))

    // Sets value of vector to be convex combination of 3 other
    // vectors, using barycentic coordinates
    SbVec3f &	setValue(const SbVec3f &barycentic,
		const SbVec3f &v0, const SbVec3f &v1, const SbVec3f &v2);

    // Accesses indexed component of vector
    float &	  operator [](int i) 		{ return (vec[i]); }
    const float & operator [](int i) const	{ return (vec[i]); }

    // Component-wise scalar multiplication and division operators
    SbVec3f &	operator *=(float d);
// C-api.h: #define SbV3fMultBy(_v, _s)
// C-api.h:     (((_v).vec[0] *= (_s)), ((_v).vec[1] *= (_s)), ((_v).vec[2] *= (_s)))

    SbVec3f &	operator /=(float d)
	{ return *this *= (1.0 / d); }
// C-api.h: #define SbV3fDivBy(_v, _s)
// C-api.h:     (((_v).vec[0] /= (_s)), ((_v).vec[1] /= (_s)), ((_v).vec[2] /= (_s)))

    // Component-wise vector addition and subtraction operators
    SbVec3f &	operator +=(SbVec3f v);
    SbVec3f &	operator -=(SbVec3f v);

    // Nondestructive unary negation - returns a new vector
    SbVec3f	operator -() const;

    // Component-wise binary scalar multiplication and division operators
    friend SbVec3f	operator *(const SbVec3f &v, float d);
    friend SbVec3f	operator *(float d, const SbVec3f &v)
	{ return v * d; }
    friend SbVec3f	operator /(const SbVec3f &v, float d)
	{ return v * (1.0 / d); }

    // Component-wise binary vector addition and subtraction operators
    friend SbVec3f	operator +(const SbVec3f &v1, const SbVec3f &v2);
// C-api.h: #define SbV3fAdd(_dest, _src1, _src2)
// C-api.h:     (((_dest).vec[0] = (_src1).vec[0] + (_src2).vec[0]),
// C-api.h:      ((_dest).vec[1] = (_src1).vec[1] + (_src2).vec[1]),
// C-api.h:      ((_dest).vec[2] = (_src1).vec[2] + (_src2).vec[2]))

    friend SbVec3f	operator -(const SbVec3f &v1, const SbVec3f &v2);
// C-api.h: #define SbV3fSub(_dest, _src1, _src2)
// C-api.h:     (((_dest).vec[0] = (_src1).vec[0] - (_src2).vec[0]),
// C-api.h:      ((_dest).vec[1] = (_src1).vec[1] - (_src2).vec[1]),
// C-api.h:      ((_dest).vec[2] = (_src1).vec[2] - (_src2).vec[2]))

// C-api: begin
    // Equality comparison operator
    friend int		operator ==(const SbVec3f &v1, const SbVec3f &v2);
    friend int		operator !=(const SbVec3f &v1, const SbVec3f &v2)
	{ return !(v1 == v2); }

    // Equality comparison within given tolerance - the square of the
    // length of the maximum distance between the two vectors
    SbBool		equals(const SbVec3f v, float tolerance) const;

    // Returns principal axis that is closest (based on maximum dot
    // product) to this vector
    SbVec3f		getClosestAxis() const;

  protected:
    float	vec[3];		// Storage for vector components
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SbVec2f
//
//  2D vector used to represet points or directions. Each component of
//  the vector is a float.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SbV2f
// C-api.h: struct SbVec2f {
// C-api.h: 	float vec[2];
// C-api.h: };

// C-api: end

class SbVec2f {
  public:

    // Default constructor
    SbVec2f()						{ }

    // Constructor given an array of 2 components
    SbVec2f(const float v[2])				{ setValue(v); }

    // Constructor given 2 individual components
    SbVec2f(float x, float y)				{ setValue(x, y); }

    // Returns dot (inner) product of vector and another vector
    float	dot(const SbVec2f &v) const;
// C-api.h: #define SbV2fDot(_v0, _v1)
// C-api.h:     ((_v0).vec[0] * (_v1).vec[0] + (_v0).vec[1] * (_v1).vec[1])

    // Returns pointer to array of 2 components
    const float	*getValue() const			{ return vec; }
// C-api.h: #define SbV2fGetXY(_xy, _src)
// C-api.h:     (((_xy)[0] = (_src).vec[0]), ((_xy)[1] = (_src).vec[1]))

    // Returns 2 individual components
    void	getValue(float &x, float &y) const;
// C-api.h: #define SbV2fGetX_Y(_x, _y, _src)
// C-api.h:     (((_x) = (_src).vec[0]), ((_y) = (_src).vec[1]))

    // Returns geometric length of vector
    float	length() const;
// C-api.h: #define SbV2fLen(_v)
// C-api.h:     (sqrtf(SbV2fDot((_v), (_v))))

    // Negates each component of vector in place
    void	negate();
// C-api.h: #define SbV2fNegate(_v)
// C-api.h: 	SbV2fMultBy(_v, -1.0)

// C-api: begin
    // Changes vector to be unit length
    // C-api: name=norm
    float	normalize();
// C-api: end

    // Sets value of vector from array of 2 components
    SbVec2f &	setValue(const float v[2]);
// C-api.h: #define SbV2fSetXY(_dest, _src)
// C-api.h:     (((_dest).vec[0] = (_src)[0]), ((_dest).vec[1] = (_src)[1]))

    // Sets value of vector from 2 individual components
    SbVec2f &	setValue(float x, float y);
// C-api.h: #define SbV2fSetX_Y(_dest, _x, _y)
// C-api.h:     (((_dest).vec[0] = (_x)), ((_dest).vec[1] = (_y)))

    // Accesses indexed component of vector
    float &	  operator [](int i) 		{ return (vec[i]); }
    const float & operator [](int i) const 	{ return (vec[i]); }

    // Component-wise scalar multiplication and division operators
    SbVec2f &	operator *=(float d);
// C-api.h: #define SbV2fMultBy(_v, _s)
// C-api.h:     (((_v).vec[0] *= (_s)), ((_v).vec[1] *= (_s)))

    SbVec2f &	operator /=(float d)
	{ return *this *= (1.0 / d); }
// C-api.h: #define SbV2fDivBy(_v, _s)
// C-api.h:     (((_v).vec[0] /= (_s)), ((_v).vec[1] /= (_s)))

    // Component-wise vector addition and subtraction operators
    SbVec2f &	operator +=(const SbVec2f &u);
    SbVec2f &	operator -=(const SbVec2f &u);

    // Nondestructive unary negation - returns a new vector
    SbVec2f	operator -() const;

    // Component-wise binary scalar multiplication and division operators
    friend SbVec2f	operator *(const SbVec2f &v, float d);
    friend SbVec2f	operator *(float d, const SbVec2f &v)
	{ return v * d; }
    friend SbVec2f	operator /(const SbVec2f &v, float d)
	{ return v * (1.0 / d); }

    // Component-wise binary vector addition and subtraction operators
    friend SbVec2f	operator +(const SbVec2f &v1, const SbVec2f &v2);
// C-api.h: #define SbV2fAdd(_dest, _src1, _src2)
// C-api.h:     (((_dest).vec[0] = (_src1).vec[0] + (_src2).vec[0]),
// C-api.h:      ((_dest).vec[1] = (_src1).vec[1] + (_src2).vec[1]))

    friend SbVec2f	operator -(const SbVec2f &v1, const SbVec2f &v2);
// C-api.h: #define SbV2fSub(_dest, _src1, _src2)
// C-api.h:     (((_dest).vec[0] = (_src1).vec[0] - (_src2).vec[0]),
// C-api.h:      ((_dest).vec[1] = (_src1).vec[1] - (_src2).vec[1]))

// C-api: begin
    // Equality comparison operator
    friend int		operator ==(const SbVec2f &v1, const SbVec2f &v2);
    friend int		operator !=(const SbVec2f &v1, const SbVec2f &v2)
	{ return !(v1 == v2); }

    // Equality comparison within given tolerance - the square of the
    // length of the maximum distance between the two vectors
    SbBool		equals(const SbVec2f v, float tolerance) const;

  protected:
    float	vec[2];		// Storage for vector components
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SbVec2s
//
//  2D vector used to represet points or directions. Each component of
//  the vector is a (short) integer.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SbV2s
// C-api.h: struct SbVec2s {
// C-api.h: 	short vec[2];
// C-api.h: };

// C-api: end

class SbVec2s {
  public:

    // Default constructor
    SbVec2s()						{ }

    // Constructor given an array of 2 components
    SbVec2s(const short v[2])				{ setValue(v); }

    // Constructor given 2 individual components
    SbVec2s(short x, short y)				{ setValue(x, y); }

    // Returns dot (inner) product of vector and another vector
    int32_t	dot(const SbVec2s &v) const;
// C-api.h: #define SbV2sDot(_v0, _v1)
// C-api.h:     ((_v0).vec[0] * (_v1).vec[0] + (_v0).vec[1] * (_v1).vec[1])

    // Returns pointer to array of 2 components
    const short	*getValue() const			{ return vec; }
// C-api.h: #define SbV2sGetXY(_dest, _src)
// C-api.h:     (((_dest)[0] = (_src).vec[0]), ((_dest)[1] = (_src).vec[1]))

    // Returns 2 individual components
    void	getValue(short &x, short &y) const;
// C-api.h: #define SbV2sGetX_Y(_x, _y, _src)
// C-api.h:     (((_x) = (_src).vec[0]), ((_y) = (_src).vec[1]))

    // Negates each component of vector in place
    void	negate();
// C-api.h: #define SbV2sNegate(_v)
// C-api.h: 	SbV2sMultBy(_v, -1.0)

    // Sets value of vector from array of 2 components
    SbVec2s &	setValue(const short v[2]);
// C-api.h: #define SbV2sSetXY(_dest, _src)
// C-api.h:     (((_dest).vec[0] = (_src)[0]), ((_dest)[1] = (_src)[1]))

    // Sets value of vector from 2 individual components
    SbVec2s &	setValue(short x, short y);
// C-api.h: #define SbV2sSetX_Y(_dest, _x, _y)
// C-api.h:     (((_dest).vec[0] = (_x)), ((_dest).vec[1] = (_y)))

    // Accesses indexed component of vector
    short &	  operator [](int i) 		{ return (vec[i]); }
    const short & operator [](int i) const 	{ return (vec[i]); }

    // Component-wise scalar multiplication and division operators
    SbVec2s &	operator *=(int d);
    SbVec2s &	operator *=(double d);
// C-api.h: #define SbV2sMultBy(_v, _s)
// C-api.h:     (((_v).vec[0] *= (_s)), ((_v).vec[1] *= (_s)))

    SbVec2s &	operator /=(int d);
    SbVec2s &	operator /=(double d)
	{ return *this *= (1.0 / d); }
// C-api.h: #define SbV2sDivBy(_v, _s)
// C-api.h:     (((_v).vec[0] /= (_s)), ((_v).vec[1] /= (_s)))

    // Component-wise vector addition and subtraction operators
    SbVec2s &	operator +=(const SbVec2s &u);
    SbVec2s &	operator -=(const SbVec2s &u);

    // Nondestructive unary negation - returns a new vector
    SbVec2s	operator -() const;

    // Component-wise binary scalar multiplication and division operators
    friend SbVec2s	operator *(const SbVec2s &v, int d);
    friend SbVec2s	operator *(const SbVec2s &v, double d);
    friend SbVec2s	operator *(int d, const SbVec2s &v)
	{ return v * d; }
    friend SbVec2s	operator *(double d, const SbVec2s &v)
	{ return v * d; }
    friend SbVec2s	operator /(const SbVec2s &v, int d);
    friend SbVec2s	operator /(const SbVec2s &v, double d)
	{ return v * (1.0 / d); }

    // Component-wise binary vector addition and subtraction operators
    friend SbVec2s	operator +(const SbVec2s &v1, const SbVec2s &v2);
// C-api.h: #define SbV2sAdd(_dest, _src1, _src2)
// C-api.h:     (((_dest).vec[0] = (_src1).vec[0] + (_src2).vec[0]),
// C-api.h:      ((_dest).vec[1] = (_src1).vec[1] + (_src2).vec[1]))

    friend SbVec2s	operator -(const SbVec2s &v1, const SbVec2s &v2);
// C-api.h: #define SbV2sSub(_dest, _src1, _src2)
// C-api.h:     (((_dest).vec[0] = (_src1).vec[0] - (_src2).vec[0]),
// C-api.h:      ((_dest).vec[1] = (_src1).vec[1] - (_src2).vec[1]))

// C-api: begin
    // Equality comparison operator
    friend int		operator ==(const SbVec2s &v1, const SbVec2s &v2);
    friend int		operator !=(const SbVec2s &v1, const SbVec2s &v2)
	{ return !(v1 == v2); }

  protected:
    short	vec[2];		// Storage for vector components
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SbVec4f
//
//  4D vector used to represet rational points or directions. Each component of
//  the vector is a float.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SbV4f
// C-api.h: struct SbVec4f {
// C-api.h: 	float vec[4];
// C-api.h: };

// C-api: end

class SbVec4f {
  public:

    // Default constructor
    SbVec4f()						{ }

    // Constructor given an array of 4 components
    SbVec4f(const float v[4])				{ setValue(v); }

    // Constructor given 4 individual components
    SbVec4f(float x, float y, float z, float w)	      { setValue(x, y, z, w); }

    // Returns dot (inner) product of vector and another vector
    float	dot(const SbVec4f &v) const;
// C-api.h: #define SbV4fDot(_v0, _v1)
// C-api.h:     ((_v0).vec[0] * (_v1).vec[0] + (_v0).vec[1] * (_v1).vec[1] +
// C-api.h:      (_v0).vec[2] * (_v1).vec[2] + (_v0).vec[3] * (_v1).vec[3])

// C-api: begin
    // Returns the real portion of the vector by dividing by the fourth value
    void	getReal(SbVec3f &v) const;
// C-api: end

    // Returns pointer to array of 4 components
    const float	*getValue() const			{ return vec; }
// C-api.h: #define SbV4fGetXYZW(_dest, _src)
// C-api.h:     (((_dest)[0] = (_src).vec[0]), ((_dest)[1] = (_src).vec[1]),
// C-api.h:      ((_dest)[2] = (_src).vec[2]), ((_dest)[3] = (_src).vec[3]))

    // Returns 4 individual components
    void	getValue(float &x, float &y, float &z, float &w) const;
// C-api.h: #define SbV4fGetX_Y_Z_W(_x, _y, _z, _w, _src)
// C-api.h:     (((_x) = (_src).vec[0]), ((_y) = (_src).vec[1]),
// C-api.h:      ((_z) = (_src).vec[2]), ((_w) = (_src).vec[3]))

    // Returns geometric length of vector
    float	length() const;
// C-api.h: #define SbV4fLen(_v)
// C-api.h:     (sqrtf(SbV4fDot((_v), (_v))))

    // Negates each component of vector in place
    void	negate();
// C-api.h: #define SbV4fNegate(_v)
// C-api.h: 	SbV4fMultBy(_v, -1.0)

// C-api: begin
    // Changes vector to be unit length
    // C-api: name=norm
    float	normalize();
// C-api: end

    // Sets value of vector from array of 4 components
    SbVec4f &	setValue(const float v[4]);
// C-api.h: #define SbV4fSetXYZW(_dest, _src)
// C-api.h:     (((_dest).vec[0] = (_src)[0]), ((_dest).vec[1] = (_src)[1]),
// C-api.h:      ((_dest).vec[2] = (_src)[2]), ((_dest).vec[3] = (_src)[3]))

    // Sets value of vector from 4 individual components
    SbVec4f &	setValue(float x, float y, float z, float w);
// C-api.h: #define SbV4fSetX_Y_Z_W(_dest, _x, _y, _z, _w)
// C-api.h:     (((_dest).vec[0] = (_x)), ((_dest).vec[1] = (_y)),
// C-api.h:      ((_dest).vec[2] = (_z)), ((_dest).vec[3] = (_w)))

    // Accesses indexed component of vector
    float &	  operator [](int i) 		{ return (vec[i]); }
    const float & operator [](int i) const	{ return (vec[i]); }

    // Component-wise scalar multiplication and division operators
    SbVec4f &	operator *=(float d);
// C-api.h: #define SbV4fMultBy(_v, _s)
// C-api.h:     (((_v).vec[0] *= (_s)), ((_v).vec[1] *= (_s)),
// C-api.h:      ((_v).vec[2] *= (_s)), ((_v).vec[3] *= (_s)))

    SbVec4f &	operator /=(float d)
	{ return *this *= (1.0 / d); }
// C-api.h: #define SbV4fDivBy(_v, _s)
// C-api.h:     (((_v).vec[0] /= (_s)), ((_v).vec[1] /= (_s)),
// C-api.h:      ((_v).vec[2] /= (_s)), ((_v).vec[3] /= (_s)))

    // Component-wise vector addition and subtraction operators
    SbVec4f &	operator +=(const SbVec4f &u);
    SbVec4f &	operator -=(const SbVec4f &u);

    // Nondestructive unary negation - returns a new vector
    SbVec4f	operator -() const;

    // Component-wise binary scalar multiplication and division operators
    friend SbVec4f	operator *(const SbVec4f &v, float d);
    friend SbVec4f	operator *(float d, const SbVec4f &v)
	{ return v * d; }
    friend SbVec4f	operator /(const SbVec4f &v, float d)
	{ return v * (1.0 / d); }

    // Component-wise binary vector addition and subtraction operators
    friend SbVec4f	operator +(const SbVec4f &v1, const SbVec4f &v2);
// C-api.h: #define SbV4fAdd(_dest, _src1, _src2)
// C-api.h:     (((_dest).vec[0] = (_src1).vec[0] + (_src2).vec[0]),
// C-api.h:      ((_dest).vec[1] = (_src1).vec[1] + (_src2).vec[1]),
// C-api.h:      ((_dest).vec[2] = (_src1).vec[2] + (_src2).vec[2]))
// C-api.h:      ((_dest).vec[3] = (_src1).vec[3] + (_src2).vec[3]))

    friend SbVec4f	operator -(const SbVec4f &v1, const SbVec4f &v2);
// C-api.h: #define SbV4fSub(_dest, _src1, _src2)
// C-api.h:     (((_dest).vec[0] = (_src1).vec[0] - (_src2).vec[0]),
// C-api.h:      ((_dest).vec[1] = (_src1).vec[1] - (_src2).vec[1]),
// C-api.h:      ((_dest).vec[2] = (_src1).vec[2] - (_src2).vec[2]))
// C-api.h:      ((_dest).vec[3] = (_src1).vec[3] - (_src2).vec[3]))

// C-api: begin
    // Equality comparison operator
    friend int		operator ==(const SbVec4f &v1, const SbVec4f &v2);
    friend int		operator !=(const SbVec4f &v1, const SbVec4f &v2)
	{ return !(v1 == v2); }

    // Equality comparison within given tolerance - the square of the
    // length of the maximum distance between the two vectors
    SbBool		equals(const SbVec4f v, float tolerance) const;

  protected:
    float	vec[4];		// Storage for vector components
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SbRotation
//
//  Rotation specfication. It is stored internally as a quaternion,
//  which has 4 floating-point components.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SbRot
// C-api.h: struct SbRotation {
// C-api.h: 	float quat[4];
// C-api.h: };

// C-api: end

class SbRotation {
  public:

    // Default constructor
    SbRotation()
	{}

    // Constructor given a quaternion as an array of 4 components
    SbRotation(const float v[4])
	{ setValue(v); }

    // Constructor given 4 individual components of a quaternion
    SbRotation(float q0, float q1, float q2, float q3)
	{ setValue(q0, q1, q2, q3); }

    // Constructor given a rotation matrix
    SbRotation(const SbMatrix &m)
	{ setValue(m); }

    // Constructor given 3D rotation axis vector and angle in radians
    SbRotation(const SbVec3f &axis, float radians)
	{ setValue(axis, radians); }

    // Constructor for rotation that rotates one direction vector to another
    SbRotation(const SbVec3f &rotateFrom, const SbVec3f &rotateTo)
	{ setValue(rotateFrom, rotateTo); }

    // Returns pointer to array of 4 components defining quaternion
    const float	*	getValue() const
	{ return (quat); }
// C-api.h: #define SbRotGetQuat(_dest, _src)
// C-api.h:     (((_dest)[0] = (_src).quat[0]), ((_dest)[1] = (_src).quat[1]),
// C-api.h:      ((_dest)[2] = (_src).quat[2]), ((_dest)[3] = (_src).quat[3]))

    // Returns 4 individual components of rotation quaternion 
    void		getValue(float &q0, float &q1,
				 float &q2, float &q3) const;
// C-api.h: #define SbRotGetQ_U_A_T(_x, _y, _z, _w, _src)
// C-api.h:     (((_x) = (_src).quat[0]), ((_y) = (_src).quat[1]),
// C-api.h:      ((_z) = (_src).quat[2]), ((_w) = (_src).quat[3]))

// C-api: begin
    // Returns corresponding 3D rotation axis vector and angle in radians
    // C-api: name=getAxisAngle
    void		getValue(SbVec3f &axis, float &radians) const;

    // Returns corresponding 4x4 rotation matrix
    // C-api: name=getMx
    void		getValue(SbMatrix &matrix) const;

    // Changes a rotation to be its inverse
    SbRotation &	invert();

    // Returns the inverse of a rotation
    SbRotation		inverse() const
	{ SbRotation q = *this; return q.invert(); }
// C-api: end

    // Sets value of rotation from array of 4 components of a quaternion
    SbRotation &	setValue(const float q[4]);
// C-api.h: #define SbRotSetQuat(_dest, _src)
// C-api.h:     (((_dest).quat[0] = (_src)[0]), ((_dest).quat[1] = (_src)[1]),
// C-api.h:      ((_dest).quat[2] = (_src)[2]), ((_dest).quat[3] = (_src)[3]))

    // Sets value of rotation from 4 individual components of a quaternion 
    SbRotation &	setValue(float q0, float q1, float q2, float q3);
// C-api.h: #define SbRotSetQ_U_A_T(_dest, _x, _y, _z, _w)
// C-api.h:     (((_dest).quat[0] = (_x)), ((_dest).quat[1] = (_y)),
// C-api.h:      ((_dest).quat[2] = (_z)), ((_dest).quat[3] = (_w)))

// C-api: begin
    // Sets value of rotation from a rotation matrix
    // I don't know what will happen if you call this with something
    // that isn't a rotation.
    // C-api: name=setMx
    SbRotation &	setValue(const SbMatrix &m);

    // Sets value of vector from 3D rotation axis vector and angle in radians
    // C-api: name=setAxisAngle
    SbRotation &	setValue(const SbVec3f &axis, float radians);

    // Sets rotation to rotate one direction vector to another
    // C-api: name=setFromTo
    SbRotation &	setValue(const SbVec3f &rotateFrom,
				 const SbVec3f &rotateTo);
// C-api: end

    // Multiplies by another rotation; results in product of rotations
    SbRotation &	 operator *=(const SbRotation &q);

// C-api: begin
    // Equality comparison operator
    friend int	operator ==(const SbRotation &q1, const SbRotation &q2);
    friend int	operator !=(const SbRotation &q1, const SbRotation &q2)
	{ return !(q1 == q2); }

    // Equality comparison within given tolerance - the square of the
    // length of the maximum distance between the two quaternion vectors
    SbBool		equals(const SbRotation &r, float tolerance) const;

    // Multiplication of two rotations; results in product of rotations
    friend SbRotation	operator *(const SbRotation &q1, const SbRotation &q2);

    // Puts the given vector through this rotation
    // (Multiplies the given vector by the matrix of this rotation),.
    void	multVec(const SbVec3f &src, SbVec3f &dst) const;

    // Keep the axis the same. Multiply the angle of rotation by 
    // the amount 'scaleFactor'
    void scaleAngle( float scaleFactor );

    // Spherical linear interpolation: as t goes from 0 to 1, returned
    // value goes from rot0 to rot1
    static SbRotation	slerp(const SbRotation &rot0,
			      const SbRotation &rot1, float t);

    // Null rotation
    // C-api: name=ident
    static SbRotation	identity()
	{ return SbRotation(0.0, 0.0, 0.0, 1.0); }

  private:
    float	quat[4];	// Storage for quaternion components

    // Returns the norm (square of the 4D length) of a rotation's quaterion
    float	norm() const;

    // Normalizes a rotation quaternion to unit 4D length
    void	normalize();
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SbMatrix
//
//  4x4 matrix of floating-point elements.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SbMx
// C-api.h: struct SbMatrix {
// C-api.h: 	float mx[4][4];
// C-api.h: };

// C-api: end

class SbMatrix {
  public:

    // Default constructor
    SbMatrix()						{ }

    // Constructor given all 16 elements in row-major order
    SbMatrix(float a11, float a12, float a13, float a14,
	     float a21, float a22, float a23, float a24, 
	     float a31, float a32, float a33, float a34, 
	     float a41, float a42, float a43, float a44);

    // Constructor from a 4x4 array of elements
    SbMatrix(const SbMat &m);

    // Sets value from 4x4 array of elements
    void	setValue(const SbMat &m);

    public:

// C-api: begin
    // Sets matrix to be identity
    // C-api: name=makeIdent
    void	makeIdentity();

    // Returns an identity matrix 
    // C-api: name=ident
    static SbMatrix	identity();

    // Sets matrix to rotate by given rotation
    // C-api: name=setRot
    void	setRotate(const SbRotation &q);

    // Sets matrix to scale by given uniform factor
    void	setScale(float s);

    // Sets matrix to scale by given vector
    // C-api: name=scaleVec
    void	setScale(const SbVec3f &s);

    // Sets matrix to translate by given vector
    // C-api: name=setXlate
    void	setTranslate(const SbVec3f &t);

    // Composes the matrix based on a translation, rotation, scale,
    // orientation for scale, and center.  The "center" is the
    // center point for scaling and rotation.  The "scaleOrientation"
    // chooses the primary axes for the scale.
    // C-api: name=setXform
    void	setTransform(
			const SbVec3f &translation,
			const SbRotation &rotation,
			const SbVec3f &scaleFactor,
			const SbRotation &scaleOrientation,
			const SbVec3f &center);
// C-api: end
    // Overloaded methods as a kludge because the compiler won't let
    // us have SbVec3f(0,0,0) as a default value:
    void	setTransform(const SbVec3f &t, const SbRotation &r,
			     const SbVec3f &s)
    		{ setTransform(t, r, s,
			       SbRotation(0,0,0,1), SbVec3f(0,0,0)); }
    void	setTransform(const SbVec3f &t, const SbRotation &r,
			     const SbVec3f &s, const SbRotation &so)
    		{ setTransform(t, r, s, so, SbVec3f(0,0,0)); }

// C-api: begin
    // Decomposes the matrix into a translation, rotation, scale,
    // and scale orientation.  Any projection information is discarded.
    // The decomposition depends upon choice of center point for
    // rotation and scaling, which is optional as the last parameter.
    // Note that if the center is 0, decompose() is the same as
    // factor() where "t" is translation, "u" is rotation, "s" is scaleFactor,
    // and "r" is ScaleOrientattion.
    // C-api: name=getXform
    void	getTransform(SbVec3f &translation,
			  SbRotation &rotation,
			  SbVec3f &scaleFactor,
			  SbRotation &scaleOrientation,
			  const SbVec3f &center) const;
// C-api: end
    void	getTransform(SbVec3f &t, SbRotation &r,
			  SbVec3f &s, SbRotation &so) const
		{ getTransform(t, r, s, so, SbVec3f(0,0,0)); }


    // The following methods return matrix values and other info:

    // Returns 4x4 array of elements
    void	getValue(SbMat &m) const;
    const SbMat &	getValue() const { return matrix; }

// C-api: begin
    // Returns determinant of 3x3 submatrix composed of given row and
    // column indices (0-3 for each).
    // C-api: name=det3Mx
    float	det3(int r1, int r2, int r3, int c1, int c2, int c3) const;

    // Returns determinant of upper-left 3x3 submatrix
    float	det3() const { return det3(0, 1, 2, 0, 1, 2); }

    // Returns determinant of entire matrix
    float	det4() const;

    // Factors a matrix m into 5 pieces: m = r s r^ u t, where r^
    // means transpose of r, and r and u are rotations, s is a scale,
    // and t is a translation. Any projection information is returned
    // in proj.
    SbBool	factor(SbMatrix &r, SbVec3f &s, SbMatrix &u,
		       SbVec3f &t, SbMatrix &proj) const;

    // Returns inverse of matrix. Results are undefined for
    // singular matrices.  Uses LU decompostion
    SbMatrix	inverse() const;

    // Perform in-place LU decomposition of matrix.  indx is index of rows
    // in matrix. d is the parity of row swaps.  Returns FALSE if singular
    // C-api: name=LUDecomp
    SbBool	LUDecomposition(int index[4], float &d);

    // Perform back-substitution on LU-decomposed matrix. Index is
    // permutation of rows from original matrix
    // C-api: name=LUBackSub
    void	LUBackSubstitution(int index[4], float b[4]) const;

    // Returns transpose of matrix
    SbMatrix	transpose() const;


    // The following methods provide Mx/mx and mx/vec arithmetic:

    // Multiplies matrix by given matrix on right or left
    SbMatrix &	multRight(const SbMatrix &m);	// this = this * m
    SbMatrix &	multLeft(const SbMatrix &m);	// this = m * this

    // Multiplies matrix by given column vector, giving vector result
    // C-api: name=multMxVec
    void	multMatrixVec(const SbVec3f &src, SbVec3f &dst) const;

    // Multiplies given row vector by matrix, giving vector result
    // C-api: name=multVecMx
    void	multVecMatrix(const SbVec3f &src, SbVec3f &dst) const;

    // Multiplies given row vector by matrix, giving vector result
    // src is assumed to be a direction vector, so translation part of
    // matrix is ignored.
    // C-api: name=multDirMx
    void	multDirMatrix(const SbVec3f &src, SbVec3f &dst) const;

    // Multiplies the given line's origin by the matrix, and the
    // line's direction by the rotation portion of the matrix
    // C-api: name=multLineMx
    void	multLineMatrix(const SbLine &src, SbLine &dst) const;


    // The following methods are miscellaneous Mx functions:

    // Prints a formatted version of the matrix to the given file pointer
    void	print(FILE *fp) const;
// C-api: end


    // Cast: returns pointer to storage of first element
    operator float *() 		{ return &matrix[0][0]; }

    // Cast: returns reference to 4x4 array
    operator SbMat &() 		{ return matrix; }

    // Make it look like a usual matrix (so you can do m[3][2])
    float *	  operator [](int i) 	    { return &matrix[i][0]; }
    const float * operator [](int i) const  { return &matrix[i][0]; }

    // Sets value from 4x4 array of elements
    SbMatrix &	operator =(const SbMat &m);

// C-api: begin
    SbMatrix &	operator =(const SbMatrix &m);
// C-api: end
    // This silly little inline method is needed because some
    // compilers won't use the above method...
#ifdef __sgi
    SbMatrix &  operator =(      SbMat &m) { return *this = (const SbMat &)m; }
#endif // __sgi

    // Sets value from a rotation
    SbMatrix &	operator =(const SbRotation &q)	{ setRotate(q); return *this; }

// C-api: begin
    // Performs right multiplication with another matrix
    SbMatrix &	operator *=(const SbMatrix &m)	{ return multRight(m); }

    // Binary multiplication of matrices
    friend SbMatrix	operator *(const SbMatrix &m1, const SbMatrix &m2);

    // Equality comparison operator
    friend int		operator ==(const SbMatrix &m1, const SbMatrix &m2);
    friend int		operator !=(const SbMatrix &m1, const SbMatrix &m2)
	{ return !(m1 == m2); }

    // Equality comparison within given tolerance, for each component
    SbBool		equals(const SbMatrix &m, float tolerance) const;

  private:
    SbMat	matrix;		// Storage for 4x4 matrix

    // Diagonalizes 3x3 matrix
    void	jacobi3(float evalues[3], SbVec3f evectors[3], int &rots) const;

    SbBool affine_inverse(const SbMatrix &in, SbMatrix &out) const;
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SbViewVolume
//
//  Defines a 3D view volume. For perspective projection, the view
//  volume is a frustum. For orthographic (parallel) projection, the
//  view volume is a rectangular prism.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SbViewVol

class SbViewVolume {
  public:

    // Default constructor
    SbViewVolume();
    ~SbViewVolume()			       {}

    // Returns two matrices corresponding to the view volume.  The
    // first is a viewing matrix, which is guaranteed to be an affine
    // transformation.  The second is suitable for use as a projection
    // matrix in GL.
    // C-api: name=getMxs
    void		getMatrices(SbMatrix &affine, SbMatrix &proj) const;

    // Like the method above, but returns the affine and projection parts
    // together in one matrix (i.e. affine.multRight( proj ) ).
    // C-api: name=getMx
    SbMatrix		getMatrix() const;

    // Returns a matrix that transforms the view volume into camera
    // space: it translates the view volume so the view point is at
    // the origin, and rotates it so the view direction is along the
    // negative z axis.
    // C-api: name=getBoxMx
    SbMatrix		getCameraSpaceMatrix() const;

    // Maps a 2d point (in 0 <= x,y <= 1) to a 3d line.
    // C-api: name=projPtToLine
    void		projectPointToLine(const SbVec2f &pt,
					SbLine &line) const;
    // C-api: name=projPtToLinePts				
    void		projectPointToLine(const SbVec2f &pt,
				       SbVec3f &line0, SbVec3f &line1) const;

    // Maps the 3d point in world coordinates to a 2d point in
    // normalized screen coordinates (0 <= x,y,z <= 1, 0 <= z <= 1).
    // The z-screen coordinate represents the homogonized z coordinate
    // which goes (non-linearly) from 0 at the near clipping plane to
    // 1 at the far clipping plane.
    // C-api: name=projToScreen
    void		projectToScreen(const SbVec3f &src,
					SbVec3f &dst) const;

    // Returns a plane parallel to the near (or far) plane of the view
    // volume at a given distance from the projection point (eye)
    // C-api: name=getPln
    SbPlane		getPlane(float distFromEye) const;

    // Returns the point along the line of sight at the given distance
    // from the projection point (eye)
    // C-api: name=getSightPt
    SbVec3f		getSightPoint(float distFromEye) const;

    // Returns the projection of a given point in normalized screen
    // coords (see projectToScreen()) onto the plane parallel to the
    // near plane that is at distFromEye units from the eye
    // C-api: name=getPlnPt
    SbVec3f		getPlanePoint(float distFromEye,
				      const SbVec2f &normPoint) const;

    // Returns a rotation that would align a viewed object so that
    // its positive x-axis (of its object space) is to the right in
    // the view and it's positive y-axis is up. If rightAngleOnly is
    // TRUE, it will come as close as it can to this goal by using
    // only 90 degree rotations.
    // C-api: name=getAlignRot
    SbRotation		getAlignRotation(SbBool rightAngleOnly = FALSE) const;

    // Returns a scale factor that would scale a unit sphere centered
    // at worldCenter so that it would appear to have the given radius
    // in normalized screen coordinates when projected onto the near plane
    float		getWorldToScreenScale(const SbVec3f &worldCenter,
					      float normRadius) const;

    // Projects the given 3D bounding box onto the near plane and
    // returns the size (in normalized screen coords) of the
    // rectangular region that encloses it
    // C-api: name=projBox
    SbVec2f		projectBox(const SbBox3f &box) const;

    // Given a view volume, this narrows the view to the given sub-rectangle
    // of the near plane. The coordinates of the rectangle are between
    // 0 and 1, where (0,0) is the lower-left corner of the near plane
    // and (1,1) is the upper-right corner.
    // C-api: name=narrow2d
    SbViewVolume	narrow(float left,  float bottom,
			       float right, float top) const;

    // Narrow a view volume by the given box.  The box must lie inside
    // the unit cube, and the view will be shrunk according to the
    // size of the box.
    // C-api: name=narrow3d
    SbViewVolume	narrow(const SbBox3f &box) const;

    // Sets up an orthographic view volume with the given sides.
    // The parameters are the same as for the GL ortho() routine.
    void		ortho(float left,   float right,
			      float bottom, float top,
			      float near,   float far);

    // Sets up a perspective view volume with the given field of view
    // and aspect ratio. The parameters are the same as for the GL
    // perspective() routine, except that the field of view angle is
    // specified in radians.
    // C-api: name=persp
    void		perspective(float fovy, float aspect,
				    float near, float far);

    // Rotate the camera view direction.  Note that this accomplishes
    // the reverse of doing a GL rotate() command after defining a
    // camera, which rotates the scene viewed by the camera.
    // C-api: name=rotCam
    void		rotateCamera(const SbRotation &q);

    // Translate the camera viewpoint.  Note that this accomplishes
    // the reverse of doing a GL translate() command after defining a
    // camera, which translates the scene viewed by the camera.
    // C-api: name=xlateCam
    void		translateCamera(const SbVec3f &v);

    // Returns the positive z axis in eye space.  In this coordinate
    // system, the z value of the near plane should be GREATER than the
    // z value of the far plane.
    // C-api: name=zVec
    SbVec3f		zVector() const;

    // Returns a narrowed view volume which contains as tightly as
    // possible the given interval on the z axis (in eye space).  The
    // returned view volume will never be larger than the current volume,
    // however.  Near and far are given in terms of zVector(): this
    // means that near > far must hold.
    SbViewVolume	zNarrow(float near, float far) const;

    // Scales width and height of view volume by given factor
    void		scale(float factor);

    // Scales view volume to be the given ratio of its current width
    // or height, leaving the resulting view volume centered about the
    // same point (in the near plane) as the current one.
    void		scaleWidth(float ratio);
    void		scaleHeight(float ratio);

    // Return projection information
    enum ProjectionType	{ 
	ORTHOGRAPHIC, 
	PERSPECTIVE 
    };

    // C-api: name=getProjType
    ProjectionType	getProjectionType() const  { return type; }

    // C-api: name=getProjPt
    const SbVec3f &	getProjectionPoint() const { return projPoint; }

    // C-api: name=getProjDir
    const SbVec3f &	getProjectionDirection() const   { return projDir; }

    // Returns distance from projection point to near plane
    float		getNearDist() const	   { return nearDist; }

    // return bounds of viewing frustum
    float		getWidth() const      { return (lrfO-llfO).length(); }
    float		getHeight() const     { return (ulfO-llfO).length(); }
    float		getDepth() const      { return nearToFar; }

  SoINTERNAL public:
    ProjectionType type;

    // Note that there is redundant info in this data structure and its
    // elements should not be changed by hand.  
    SbVec3f	projPoint;		// must be (0,0,0) for ortho
    SbVec3f	projDir;
    float	nearDist;		// distance to near plane
    float	nearToFar;		// distance between z clips
    SbVec3f	llf;
    SbVec3f	lrf;
    SbVec3f	ulf;

    // Transforms the view volume by the given matrix. NOTE: if the
    // matrix performs a scale and a rotation, angles between the
    // transformed projection direction and the sides of the view
    // volume may not be preserved.
    void	transform(const SbMatrix &matrix);

    // Returns TRUE if view volume contains point
    SbBool	intersect(const SbVec3f &point) const;

    // Returns TRUE if line segment between 2 points may intersect
    // volume. Returns closest point on line to center ray of volume
    // if intersection is found.
    SbBool	intersect(const SbVec3f &p0, const SbVec3f &p1,
			  SbVec3f &closestPoint) const;

    // Returns TRUE if bounding box may intersect volume
    SbBool	intersect(const SbBox3f &box) const;

    // Returns TRUE if the bounding box defined by min,max is totally
    // outside plane p.
    SbBool	outsideTest(const SbPlane &p,
			const SbVec3f &min, const SbVec3f &max) const;

  private:
    // Points on the near clipping plane.  Add in the projPoint to
    // figure out where they are in world space:
    SbVec3f	llfO;			// x = -w, y = -w, z = -w
    SbVec3f	lrfO;			// x =  w, y = -w, z = -w
    SbVec3f	ulfO;			// x = -w, y =  w, z = -w
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SbLine
//
//  Represents a directed line in 3D space.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: public=pos, dir

class SbLine {
  public:
// C-api: end
    SbLine()	{}

    // Construct a line from two points lying on the line.  If you
    // want to construct a line from a position and a direction, use
    // SbLine(p, p + d).
    // Line is directed from p0 to p1.
    SbLine(const SbVec3f &p0, const SbVec3f &p1);

    // Set that value!
    void		setValue(const SbVec3f &p0, const SbVec3f &p1);

// C-api: begin
    // Find closest points between the two lines. Return FALSE if they are 
    // parallel, otherwise return TRUE.
    // C-api: name=getClosestPts
    SbBool		getClosestPoints(const SbLine  &line2,
					 SbVec3f &ptOnThis,
					 SbVec3f &ptOnLine2 ) const;

    // Returns the closest point on the line to the given point.
    // C-api: name=getClosestPt
    SbVec3f		getClosestPoint(const SbVec3f &point) const;

// C-api: end

    // Accessors
    const SbVec3f &	getPosition() const	{ return pos; }
    const SbVec3f &	getDirection() const	{ return dir; }

// C-api: begin

  SoINTERNAL public:
    // Intersect the line with a box, point, line, and triangle.
    SbBool              intersect( const SbBox3f &box,
                                   SbVec3f &enter, SbVec3f &exit ) const;
    SbBool              intersect( float angle, const SbBox3f &box ) const;
    SbBool              intersect( float angle, const SbVec3f &point ) const;
    SbBool              intersect( float angle, const SbVec3f &v0,
                                   const SbVec3f &v1, SbVec3f &pt ) const;
    SbBool              intersect( const SbVec3f &v0,
				  const SbVec3f &v1,
				  const SbVec3f &v2,
				  SbVec3f &pt, SbVec3f &barycentric,
				  SbBool &front ) const;

  private:
    // Parametric description:
    //  l(t) = pos + t * dir
    SbVec3f	pos;
    SbVec3f	dir;
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SbPlane
//
//  Represents an oriented plane in 3D space. The plane is defined by
//  a plane normal and a distance from the origin along that normal.
//  SbPlanes may be used to represent either planes or half-spaces. In
//  the latter case (as for the isInHalfSpace() method), the
//  half-space is defined to be all points on the plane or on the side
//  of the plane in the direction of the plane normal.
//
//  The 4 coefficients of the plane equation of an SbPlane can be
//  obtained easily as the 3 coordinates of the plane normal and the
//  distance, in that order.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SbPln
// C-api: public=normalVec, distance

class SbPlane {
  public:
// C-api: end
    SbPlane()	{}

    // Construct a plane given 3 points.
    // Orientation is computed by taking (p1 - p0) x (p2 - p0) and
    // pointing the normal in that direction.
    SbPlane(const SbVec3f &p0, const SbVec3f &p1, const SbVec3f &p2);

    // Construct a plane given normal and distance from origin along normal.
    // Orientation is given by the normal vector n.
    SbPlane(const SbVec3f &n, float d);

    // Construct a plane given normal and a point to pass through
    // Orientation is given by the normal vector n.
    SbPlane(const SbVec3f &n, const SbVec3f &p);

// C-api: begin
    // Offset a plane by a given distance.
    void		offset(float d);

    // Intersect line and plane, returning TRUE if there is an intersection
    // FALSE if line is parallel to plane
    SbBool		intersect(const SbLine &l,
				  SbVec3f &intersection) const;

    // Transforms the plane by the given matrix
    void		transform(const SbMatrix &matrix);

    // Returns TRUE if the given point is within the half-space
    // defined by the plane
    SbBool		isInHalfSpace(const SbVec3f &point) const;

// C-api: end

    // Accessors
    const SbVec3f &	getNormal() const		{ return normalVec; }
    float		getDistanceFromOrigin() const	{ return distance; }

// C-api: begin
    // Equality/inequality comparison operators
    friend int		operator ==(const SbPlane &p1, const SbPlane &p2);
    friend int		operator !=(const SbPlane &p1, const SbPlane &p2)
	{ return !(p1 == p2); }

  private:
    // Plane is all p such that normalVec . p - distance = 0

    // Normal to the plane
    SbVec3f	normalVec;

    // Distance from origin to plane: distance * normalVec is on the plane
    float	distance;
};


//////////////////////////////////////////////////////////////////////////////
//
//  Class: SbCylinder
//
//  Represents a cylinder in 3D space.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SbCyl
// C-api: public=axis, radius

class SbCylinder {
  public:
// C-api: end
    // Constructor
    SbCylinder();

    // Construct a cylinder given its axis and radius
    SbCylinder(const SbLine &a, float r);

    // Change the axis and radius
    void 	setValue(const SbLine &a, float r);

    // Set just the axis or radius
    void	setAxis(const SbLine &a);
    void	setRadius(float r);

    // Return the axis and radius
    const SbLine & 	getAxis() const			{ return axis; }
    float		getRadius() const		{ return radius; }

// C-api: begin
    // Intersect line and cylinder, returning TRUE if there is an intersection
    SbBool	intersect(const SbLine &l, SbVec3f &intersection) const;
    // C-api: name=intersect2
    SbBool	intersect(const SbLine &l,
			  SbVec3f &enter, SbVec3f &exit) const;

  private:
    SbLine	axis;
    float	radius;

    static SbBool	unitCylinderIntersect(const SbLine &l,
					      SbVec3f &in, SbVec3f &out);
};


//////////////////////////////////////////////////////////////////////////////
//
//  Class: SbSphere
//
//  Represents a sphere in 3D space.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SbSph
// C-api: public=center, radius

class SbSphere {
  public:

// C-api: end
    // Boring default constructor
    SbSphere()	{}

    // Construct a sphere given center and radius
    SbSphere(const SbVec3f &c, float r);

    // Change the center and radius
    void 	setValue(const SbVec3f &c, float r);

    // Set just the center or radius
    void	setCenter(const SbVec3f &c);
    void	setRadius(float r);

    // Return the center and radius
    const SbVec3f & 	getCenter() const		{ return center; }
    float		getRadius() const		{ return radius; }

// C-api: begin
    // Return a sphere containing a given box
    void	circumscribe(const SbBox3f &box);

    // Intersect line and sphere, returning TRUE if there is an intersection
    SbBool	intersect(const SbLine &l, SbVec3f &intersection) const;
    // C-api: name=intersect2
    SbBool	intersect(const SbLine &l, SbVec3f &enter, SbVec3f &exit) const;

  private:
    SbVec3f	center;
    float	radius;
};

#endif /* _SB_LINEAR_ */
