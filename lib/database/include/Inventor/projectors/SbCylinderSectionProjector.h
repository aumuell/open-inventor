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
 * Copyright (C) 1990,91,92   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Description:
 |	This file contains the definition of the SbCylinderSectionProjector
 |	class. This projector projects the mouse position onto the
 |	section of a cylinder that has been sliced by a plane.
 |
 |	The tolerance slice can be specified as a fraction of the radius
 |	of the cylinder. The projection point will not extend beyond the
 |	sliced portion of the cylinder.
 |
 |   Author(s)		: Howard Look, Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef _SB_CYLINDER_SECTION_PROJECTOR_
#define _SB_CYLINDER_SECTION_PROJECTOR_

#include <Inventor/projectors/SbCylinderProjector.h>

// C-api: prefix=SbCylSectProj
class SbCylinderSectionProjector : public SbCylinderProjector
{
  public:
    // Default constructor.
    // The default view volume is undefined.
    // The default working space is identity (world space).
    // The default cylinder is centered about the Y axis and has a radius of 1.0.
    // The default edge tolerance is .9.
    // The default eye orientation is TRUE.
    SbCylinderSectionProjector(float  edgeTol = .9,
			       SbBool orientToEye = TRUE);

    // Constructor taking the cylinder.
    // C-api: name=createCyl
    SbCylinderSectionProjector( const SbCylinder &cyl,
				float edgeTol = .9,
				SbBool orientToEye = TRUE);

    // Destructor
    ~SbCylinderSectionProjector() {}
    
    // Returns an instance that is a copy of this instance. The caller
    // is responsible for deleting the copy when done.
    virtual SbProjector *    copy() const;

    // Apply the projector using the given point, returning the
    // point in three dimensions that it projects to.
    // The point should be normalized from 0-1, with (0,0) at the lower-left.
    virtual SbVec3f	project(const SbVec2f &point);

    // Computes a rotation based on two points on this projector.
    virtual SbRotation	getRotation(const SbVec3f &point1,
				    const SbVec3f &point2);

    // Set/get the edge tolerance as a fraction of the
    // radius of the cylinder. If this is 1.0, the projector is a
    // hemisphere. If this is .1, the projector is a slice of
    // the cylinder with radius .1*radius.  Default is .9.
    // C-api: name=setTol
    void		setTolerance(float edgeTol);

    // C-api: name=getTol
    float		getTolerance() const		  { return tolerance; }

    // Find whether this point on the cylinder or tolerance
    // plane is within tolerance.
    // C-api: name=isWithinTol
    SbBool		isWithinTolerance(const SbVec3f &point);
    
  protected:
    // Sets up the tolerance ring.
    virtual void	setupTolerance();

    // Information about the ring tolerance.
    float		tolerance;  // the edge tolerance
    float		tolDist;    // dist from planeLine to cylinder
    
    // Information about the plane used for intersection testing.
    SbVec3f	planeDir;		// normal direction
    SbLine	planeLine;		// line parallel to axis, but in plane
    float	planeDist;		// distance from cylinder center
    SbPlane	tolPlane;		// the plane itself
};

#endif /* _SB_CYLINDER_SECTION_PROJECTOR_ */

