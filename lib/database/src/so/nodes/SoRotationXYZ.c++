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
 |      SoRotationXYZ
 |
 |   Author(s)          : Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoPickAction.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/nodes/SoRotationXYZ.h>

SO_NODE_SOURCE(SoRotationXYZ);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoRotationXYZ::SoRotationXYZ()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoRotationXYZ);
    SO_NODE_ADD_FIELD(axis,  (X));
    SO_NODE_ADD_FIELD(angle, (0.0));

    // Set up static info for enumerated type field
    SO_NODE_DEFINE_ENUM_VALUE(Axis, X);
    SO_NODE_DEFINE_ENUM_VALUE(Axis, Y);
    SO_NODE_DEFINE_ENUM_VALUE(Axis, Z);

    // Set up info in enumerated type field
    SO_NODE_SET_SF_ENUM_TYPE(axis, Axis);

    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoRotationXYZ::~SoRotationXYZ()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns SbRotation equivalent to rotation.
//
// Use: public

SbRotation
SoRotationXYZ::getRotation() const
//
////////////////////////////////////////////////////////////////////////
{
    if (angle.isIgnored() || angle.isDefault())
	return SbRotation::identity();

    else {
	SbVec3f		ax(0.0, 0.0, 0.0);
	SbRotation	rot;

	switch (axis.getValue()) {

	  case X:
	    ax[0] = 1.0;
	    break;

	  case Y:
	    ax[1] = 1.0;
	    break;

	  case Z:
	    ax[2] = 1.0;
	    break;
	}

	rot.setValue(ax, angle.getValue());

	return rot;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements most actions.
//
// Use: extender

void
SoRotationXYZ::doAction(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    if (! angle.isIgnored() && ! angle.isDefault())
	SoModelMatrixElement::rotateBy(action->getState(), this,
				       getRotation());
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Handles callback action
//
// Use: extender

void
SoRotationXYZ::callback(SoCallbackAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoRotationXYZ::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Handles GL render action
//
// Use: extender

void
SoRotationXYZ::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoRotationXYZ::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Handles get bounding box action
//
// Use: extender

void
SoRotationXYZ::getBoundingBox(SoGetBoundingBoxAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoRotationXYZ::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns transformation matrix.
//
// Use: extender

void
SoRotationXYZ::getMatrix(SoGetMatrixAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SbMatrix	&ctm = action->getMatrix();
    SbMatrix	&inv = action->getInverse();
    SbMatrix	m;
    SbRotation	rot = getRotation();

    rot.getValue(m);
    ctm.multLeft(m);
    rot.invert();
    rot.getValue(m);
    inv.multRight(m);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Handles pick action.
//
// Use: extender

void
SoRotationXYZ::pick(SoPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoRotationXYZ::doAction(action);
}
