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
 |	SbCylinder
 |
 |   Author(s)		: Howard Look
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SbLinear.h>

//////////////////////////////////////////////////////////////////////////////
//
// Cylinder class
//
//////////////////////////////////////////////////////////////////////////////

// construct a default cylinder
SbCylinder::SbCylinder()
{
    axis.setValue(SbVec3f(0.0, 0.0, 0.0), SbVec3f(0.0, 1.0, 0.0));
    radius = 1.0;
}

// construct a cylinder given an axis and radius
SbCylinder::SbCylinder(const SbLine &a, float r)
{
    axis = a;
    radius = r;
}

// Change the axis and radius
void
SbCylinder::setValue(const SbLine &a, float r)
{
    axis = a;
    radius = r;
}

//////////////////////////////////////////////////////////////////////////////
//
// Change just the axis
//

void
SbCylinder::setAxis(const SbLine &a)
//
//////////////////////////////////////////////////////////////////////////////
{
    axis = a;
}

//////////////////////////////////////////////////////////////////////////////
//
// Change just the radius
//

void
SbCylinder::setRadius(float r)
//
//////////////////////////////////////////////////////////////////////////////
{
    radius = r;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  Intersect given line with this cylinder, returning the
//  first intersection in result. Returns TRUE if there was an
//  intersection (and result is valid).
//
// Use: public

SbBool
SbCylinder::intersect(const SbLine &line, SbVec3f &result) const
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f whoCares;
    return intersect(line, result, whoCares);
}    

////////////////////////////////////////////////////////////////////////
//
// Description:
//  Intersect given line with this cylinder, returning the
//  results in enter and exit. Returns TRUE if there was an
//  intersection (and results are valid).
//
// Use: public

SbBool
SbCylinder::intersect(const SbLine &line, SbVec3f &enter, SbVec3f &exit) const
//
////////////////////////////////////////////////////////////////////////
{
    // The intersection will actually be done on a radius 1 cylinder
    // aligned with the y axis, so we transform the line into that
    // space, then intersect, then transform the results back.

    // rotation to y axis
    SbRotation	rotToYAxis(axis.getDirection(), SbVec3f(0,1,0));
    SbMatrix	mtxToYAxis;
    mtxToYAxis.setRotate(rotToYAxis);

    // scale to unit space
    float	scaleFactor = 1.0/radius;
    SbMatrix	toUnitCylSpace;
    toUnitCylSpace.setScale(SbVec3f(scaleFactor, scaleFactor, scaleFactor));
    toUnitCylSpace.multLeft(mtxToYAxis);

    // find the given line un-translated
    SbVec3f origin = line.getPosition();
    origin -= axis.getPosition();
    SbLine noTranslationLine(origin, origin + line.getDirection());

    // find the un-translated line in unit cylinder's space
    SbLine cylLine;
    toUnitCylSpace.multLineMatrix(noTranslationLine, cylLine);

    // find the intersection on the unit cylinder
    SbVec3f cylEnter, cylExit;
    SbBool intersected = unitCylinderIntersect(cylLine, cylEnter, cylExit);

    if (intersected) {
	// transform back to original space
	SbMatrix fromUnitCylSpace = toUnitCylSpace.inverse();

	fromUnitCylSpace.multVecMatrix(cylEnter, enter);
	enter += axis.getPosition();

	fromUnitCylSpace.multVecMatrix(cylExit, exit);
	exit += axis.getPosition();
    }    

    return intersected;
}    

////////////////////////////////////////////////////////////////////////
//
// Description:
//  Intersect the line with a unit cylinder. Returns TRUE if
//  there was an intersection, and returns the intersection points
//  in enter and exit.
//
//  The cylinder has radius 1 and is aligned with the
//  y axis, such that x^2 + z^2 - 1 = 0
//
//  Taken from Pat Hanrahan's chapter in Glassner's
//  _Intro to Ray Tracing_, page 91, and some code
//  stolen from Paul Strauss.
//
// Use: private, static

SbBool
SbCylinder::unitCylinderIntersect(const SbLine &l,
				  SbVec3f &enter, SbVec3f &exit)
//
////////////////////////////////////////////////////////////////////////
{
    float		A, B, C, discr, sqroot, t0, t1;
    const SbVec3f	&pos = l.getPosition(), &dir = l.getDirection();
    SbBool		doesIntersect = TRUE;

    A = dir[0] * dir[0] + dir[2] * dir[2];

    B = 2.0 * (pos[0] * dir[0] + pos[2] * dir[2]);

    C = pos[0] * pos[0] + pos[2] * pos[2] - 1;

    // discriminant = B^2 - 4AC
    discr = B*B - 4.0*A*C;

    // if discriminant is negative, no intersection
    if (discr < 0.0) {
	doesIntersect = FALSE;
    }
    else {
	sqroot = sqrtf(discr);

	// magic to stabilize the answer
	if (B > 0.0) {
	    t0 = -(2.0 * C) / (sqroot + B);
	    t1 = -(sqroot + B) / (2.0 * A);
	}
	else {
	    t0 = (2.0 * C) / (sqroot - B);
	    t1 = (sqroot - B) / (2.0 * A);
	}	    

	enter = pos + (dir * t0);
	exit = pos + (dir * t1);
    }

    return doesIntersect;
}
