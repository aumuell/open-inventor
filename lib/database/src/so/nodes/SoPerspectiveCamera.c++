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
 |      SoPerspectiveCamera
 |
 |   Author(s)          : Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <math.h>

#define MINIMUM_NEAR_PLANE 0.01	    /* minimum near clipping distance */
				    /* (from center) */

SO_NODE_SOURCE(SoPerspectiveCamera);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoPerspectiveCamera::SoPerspectiveCamera()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoPerspectiveCamera);
    SO_NODE_ADD_FIELD(heightAngle,    (M_PI_4));	// 45 degrees
    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor (necessary since inline destructor is too complex)
//
// Use: private

SoPerspectiveCamera::~SoPerspectiveCamera()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Scales the height of the camera, in this case the 'heightAngle' field
//
// Use: public

void
SoPerspectiveCamera::scaleHeight(float scaleFactor)
//
////////////////////////////////////////////////////////////////////////
{
    if (scaleFactor == 0.0)
	return;

    heightAngle.setValue(scaleFactor * heightAngle.getValue());
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Fills in a view volume structure, based on the camera. If the
//    useAspectRatio field is not 0.0 (the default), the camera uses
//    that ratio instead of the one it has.
//
// Use: public

SbViewVolume
SoPerspectiveCamera::getViewVolume( float useAspectRatio) const
//
////////////////////////////////////////////////////////////////////////
{
    SbViewVolume view;

    float	camAspect = (useAspectRatio != 0.0 ? useAspectRatio :
			     aspectRatio.getValue());

    // Set up the perspective camera.
    view.perspective(heightAngle.getValue(), camAspect,
		     nearDistance.getValue(), farDistance.getValue());

    // Note that these move the camera rather than moving objects
    // relative to the camera.
    view.rotateCamera(orientation.getValue());
    view.translateCamera(position.getValue());
    return view;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Finds a bounding sphere around the passed bounding box, then
//    positions the camera without changing its height angle so that
//    the entire sphere is visible. The aspect ratio to use for the
//    camera is passed in.
// 
// Use: protected

void
SoPerspectiveCamera::viewBoundingBox(const SbBox3f &box,
				     float aspect, float slack)
//
////////////////////////////////////////////////////////////////////////
{
    SbSphere	bSphere;
    float	distToCenter;
    SbMatrix	rotation;
    SbVec3f	pos;

    // if the bounding box is not empty, create the bounding sphere
    if (! box.isEmpty())
	bSphere.circumscribe(box);
    else {
	pos.setValue(0., 0., 0.);
	bSphere.setValue(pos, 1.0);
    }

    // Find the angle necessary to fit the object completely in the
    // window.  We don't need any slack, because the bounding sphere
    // is already bigger than the bounding box.
    //	    h = height (radius of sphere)
    //	    d = distance to eye
    //	    tan(alpha) gives us h/d
    float hOverD = tan(heightAngle.getValue()  / 2.0);
    if (aspect < 1.0)
	hOverD *= aspect;

    distToCenter = bSphere.getRadius() / hOverD;  //   h / (h/d) gives us d

    rotation = orientation.getValue();
    rotation.multVecMatrix(SbVec3f(0.0, 0.0, distToCenter), pos);

    position      = pos + bSphere.getCenter();
    nearDistance  = distToCenter - slack * bSphere.getRadius();
    if (nearDistance.getValue() < 0.0 || 
    	nearDistance.getValue() < (MINIMUM_NEAR_PLANE * distToCenter))
    	nearDistance = MINIMUM_NEAR_PLANE * distToCenter;
    farDistance   = distToCenter + slack * bSphere.getRadius();
    focalDistance = distToCenter;
}
