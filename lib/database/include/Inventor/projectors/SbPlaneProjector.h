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
 |	This file contains the definition of the SbPlaneProjector
 |	class. This projects the mouse onto a plane. If orientToEye
 |	is set to TRUE, the plane will be oriented to the eye.
 |	  
 |   Author(s)		: Howard Look, Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef _SB_PLANE_PROJECTOR_
#define _SB_PLANE_PROJECTOR_

#include <Inventor/projectors/SbProjector.h>

// C-api: prefix=SbPlnProj
class SbPlaneProjector : public SbProjector
{
  public:
  
    // Constructor
    // The default plane passes through the origin and is
    // oriented toward the Z axis.
    SbPlaneProjector(SbBool orient = FALSE);
    
    // Constructor that takes a plane.
    // C-api: name=createPln
    SbPlaneProjector(const SbPlane &plane,
		     SbBool orient = FALSE);

    // Destructor
    ~SbPlaneProjector(){};
    
    // Returns an instance that is a copy of this instance. The caller
    // is responsible for deleting the copy when done.
    virtual SbProjector *    copy() const;

    // Apply the projector using the given point, returning the
    // point in three dimensions that it projects to.
    // The point should be normalized from 0-1, with (0,0) at
    // the lower-left.
    virtual SbVec3f	project(const SbVec2f &point);

    // Set/get the plane to use.
    // C-api: name=setPln
    void		setPlane(const SbPlane &plane);

    // C-api: name=getPln
    const SbPlane &	getPlane() const		  { return plane; }

    // Set/get whether the projector should be oriented towards
    // the eye.
    // If orient is set to TRUE, the given plane's direction
    // is ignored, and the plane will be oriented to the eye. 
    // It will pass through the same point from the origin defined
    // by the original dir * dist;
    // Set to FALSE if the plane's direction should remain in working space.
    void		setOrientToEye(SbBool orientToEye);
    SbBool		isOrientToEye()	const		{ return orientToEye; }

    // Get a vector on this plane given two normalized mouse points.
    // C-api: expose
    // C-api: name=getVecMouse2
    virtual SbVec3f	getVector(const SbVec2f &mousePosition1,
				  const SbVec2f &mousePosition2);
					    
    // Get a vector given the current mouse point.
    // Uses the last point on this projector from the previous
    // call to getVector or setStartPostion. Do not use this
    // if the working space transform is changing since
    // the new point will be in a different space than
    // the old one.
    // C-api: expose
    // C-api: name=getVecMouse1
    virtual SbVec3f	getVector(const SbVec2f &mousePosition);
					    
    // Set the initialial from a mouse position.
    // C-api: name=setStartPosMouse
    void		setStartPosition(const SbVec2f &mousePosition)
					{ lastPoint = project(mousePosition); }

    // Set the initialial position from a point on the projector.
    // C-api: name=setStartPosPt
    void		setStartPosition(const SbVec3f &point)
					    { lastPoint = point; }
  protected:

    SbPlane	plane;
    SbBool	orientToEye;	// TRUE is plane should be oriented to eye.
    SbBool	needSetup;	// Set TRUE when plane or orientToEye changes.
    SbPlane	nonOrientPlane;	// Non-EyeOriented plane.

    SbVec3f	lastPoint;	// Cached last point.

    void	setupPlane();	// Sets up plane orientation, if necessary.
};

#endif /* _SB_PLANE_PROJECTOR_ */
