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
 |   $Revision: 1.1 $
 |
 |   Description:
 |	This file defines the SoPerspectiveCamera node class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_PERSPECTIVE_CAMERA_
#define  _SO_PERSPECTIVE_CAMERA_

#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/nodes/SoCamera.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoPerspectiveCamera
//
//  perspectiveCamera node.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoPerspCam
// C-api: public= viewportMapping, position, orientation, aspectRatio
// C-api: public= nearDistance, farDistance, focalDistance
// C-api: public= heightAngle
class SoPerspectiveCamera : public SoCamera {

    SO_NODE_HEADER(SoPerspectiveCamera);

  public:
    // Fields (some are inherited from SoCamera)
    SoSFFloat		heightAngle;	// Angle (in radians) of field
					// of view, in height direction

    // Constructor
    SoPerspectiveCamera();

    // Scales the height of the camera, in this case, the 'heightAngle' field.
    // C-api: begin
    virtual void	scaleHeight( float scaleFactor );

    // Fills in a view volume structure, based on the camera. If the
    // useAspectRatio field is not 0.0 (the default), the camera uses
    // that ratio instead of the one it has.
    virtual SbViewVolume getViewVolume(float useAspectRatio = 0.0) const;

  SoINTERNAL public:
    static void		initClass();

  protected:
    // Positions camera to view passed bounding box with given aspect
    // ratio and current height angle
    virtual void	viewBoundingBox(const SbBox3f &box,
					float aspect, float slack);

    virtual ~SoPerspectiveCamera();
};

#endif /* _SO_PERSPECTIVE_CAMERA_ */
