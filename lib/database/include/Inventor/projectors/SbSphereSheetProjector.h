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
 |	This file defines the SbSphereSheetProjector class. This is a
 |	sphere with a hyperbolic sheet draped over it. As the
 |	mouse moves away from the sphere, rotation gradually
 |	turns into pure roll.
 |
 |	This projector is good for trackballs that want to do
 |	constinuous roll. It allows pure roll as you move away
 |	from the sphere. (This projector uses the same paradigm 
 |	as the "Flip" demo.)
 |
 |	Warning: This projector tends to become unstable when
 |	the mouse position is beyond about 2*radius from the
 |	center of the sphere. Therefore, it is best to use this
 |	as a large, window-sized trackball.
 |
 |   Author(s)		: Howard Look, Gavin Bell, Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef _SB_SPHERE_SHEET_PROJECTOR_
#define _SB_SPHERE_SHEET_PROJECTOR_

#include <Inventor/projectors/SbSphereProjector.h>

// C-api: prefix=SbSphSheetProj
class SbSphereSheetProjector : public SbSphereProjector
{
  public:
    // Default constructor.
    // The default view volume is undefined.
    // The default working space is identity (world space).
    // The default sphere to use has a radius of 1.0 and is centered at (0,0,0).
    // The default eye orientation is TRUE.
    SbSphereSheetProjector(SbBool orientToEye = TRUE);

    // Constructor taking the sphere.
    // C-api: name=createSph
    SbSphereSheetProjector(const SbSphere &sph,
			   SbBool orientToEye = TRUE);

    // Destructor
    ~SbSphereSheetProjector() {}
    
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

  protected:
    // Sets up the plane use to project on to.
    void		setupPlane();

    // The projection point in working space.
    SbVec3f		workingProjPoint;
    
    // Information about the plane used for intersection testing.
    SbVec3f	planePoint;		// point on plane
    SbVec3f	planeDir;		// normal direction
    float	planeDist;		// distance from sphere center
    SbPlane	tolPlane;		// the plane itself
};

#endif /* _SB_SPHERE_SHEET_PROJECTOR_ */
