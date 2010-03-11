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
 |	SbSphere
 |
 |   Author(s)		: Nick Thompson, David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SbLinear.h>
#include <Inventor/SbBox.h>

//////////////////////////////////////////////////////////////////////////////
//
// Sphere class
//
//////////////////////////////////////////////////////////////////////////////

// construct a sphere given a center and radius
SbSphere::SbSphere(const SbVec3f &c, float r)
{
    center = c;
    radius = r;
}

// Change the center and radius
void
SbSphere::setValue(const SbVec3f &c, float r)
{
    center = c;
    radius = r;
}

//////////////////////////////////////////////////////////////////////////////
//
// Change just the center
//

void
SbSphere::setCenter(const SbVec3f &c)
//
//////////////////////////////////////////////////////////////////////////////
{
    center = c;
}

//////////////////////////////////////////////////////////////////////////////
//
// Change just the radius
//

void
SbSphere::setRadius(float r)
//
//////////////////////////////////////////////////////////////////////////////
{
    radius = r;
}

//////////////////////////////////////////////////////////////////////////////
//
// Return a sphere containing a given box
//

void
SbSphere::circumscribe(const SbBox3f &box)
//
//////////////////////////////////////////////////////////////////////////////
{
    center = 0.5 * (box.getMin() + box.getMax());
    radius = (box.getMax() - center).length();
}

//////////////////////////////////////////////////////////////////////////////
//
// Sphere line intersection - this sets the parameter intersection,
// and returns TRUE if the line and sphere really do intersect.
//
// line-sphere intersection algorithm lifted from Eric Haines chapter in 
// Glassner's "Introduction to Ray Tracing", pp. 35-7
//
SbBool
SbSphere::intersect(const SbLine &l, SbVec3f &intersection) const
//
//////////////////////////////////////////////////////////////////////////////
{
    float   B,C;	// At^2 + Bt + C = 0, but A is 1 since we normalize Rd
    float   discr;	// discriminant (B^2 - 4AC)
    SbVec3f v;
    float   t,sqroot;
    SbBool  doesIntersect = TRUE;

    // setup B,C
    v = l.getPosition() - center;
    B = 2.0 * (l.getDirection().dot(v));
    C = v.dot(v) - (radius * radius);

    // compute discriminant
    // if negative, there is no intersection
    discr = B*B - 4.0*C;
    if (discr < 0.0) {
	// line and sphere do not intersect
	doesIntersect = FALSE;
    }
    else {
	// compute t0: (-B - sqrt(B^2 - 4AC)) / 2A  (A = 1)
	sqroot = sqrtf(discr);
	t = (-B - sqroot) * 0.5;
	if (t < 0.0) {
	    // no intersection, try t1: (-B + sqrt(B^2 - 4AC)) / 2A  (A = 1)
	    t = (-B + sqroot) * 0.5;
	}

	if (t < 0.0) {
	    // line and sphere do not intersect
	    doesIntersect = FALSE;
	}
	else {
	    // intersection! point is (point + (dir * t))
	    intersection = l.getPosition() + (l.getDirection() * t);
	}
    }

    return doesIntersect;
}

//////////////////////////////////////////////////////////////////////////////
//
// Sphere line intersection - this sets the parameter intersection,
// and returns TRUE if the line and sphere really do intersect.
//
// line-sphere intersection algorithm lifted from Eric Haines chapter in 
// Glassner's "Introduction to Ray Tracing", pp. 35-7
//
SbBool
SbSphere::intersect(const SbLine &l, SbVec3f &enter, SbVec3f &exit) const
//
//////////////////////////////////////////////////////////////////////////////
{
    float   B,C;	// At^2 + Bt + C = 0, but A is 1 since we normalize Rd
    float   discr;	// discriminant (B^2 - 4AC)
    SbVec3f v;
    float   sqroot;
    SbBool  doesIntersect = TRUE;

    // setup B,C
    v = l.getPosition() - center;
    B = 2.0 * (l.getDirection().dot(v));
    C = v.dot(v) - (radius * radius);

    // compute discriminant
    // if negative, there is no intersection
    discr = B*B - 4.0*C;

    if (discr < 0.0) {
	// line and sphere do not intersect
	doesIntersect = FALSE;
    }
    else {
	sqroot = sqrtf(discr);
	    
	float t0 = (-B - sqroot) * 0.5;
	enter = l.getPosition() + (l.getDirection() * t0);
	
	float t1 = (-B + sqroot) * 0.5;
	exit = l.getPosition() + (l.getDirection() * t1);
    }

    return doesIntersect;
}
