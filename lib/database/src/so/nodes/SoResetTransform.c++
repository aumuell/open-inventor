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
 |      SoResetTransform
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
#include <Inventor/elements/SoGLModelMatrixElement.h>
#include <Inventor/nodes/SoResetTransform.h>

SO_NODE_SOURCE(SoResetTransform);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoResetTransform::SoResetTransform()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoResetTransform);
    SO_NODE_ADD_FIELD(whatToReset, (TRANSFORM));

    // Set up static info for enumerated type field
    SO_NODE_DEFINE_ENUM_VALUE(ResetType, TRANSFORM);
    SO_NODE_DEFINE_ENUM_VALUE(ResetType, BBOX);

    // Set up info in enumerated type field
    SO_NODE_SET_SF_ENUM_TYPE(whatToReset, ResetType);

    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoResetTransform::~SoResetTransform()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements most actions.
//
// Use: extender

void
SoResetTransform::doAction(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // If has effect on transform
    if (! whatToReset.isIgnored() && (whatToReset.getValue() & TRANSFORM))
	SoModelMatrixElement::makeIdentity(action->getState(), this);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Handles callback action
//
// Use: extender

void
SoResetTransform::callback(SoCallbackAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoResetTransform::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Handles GL render action
//
// Use: extender

void
SoResetTransform::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // If has effect on transform
    if (! whatToReset.isIgnored() && (whatToReset.getValue() & TRANSFORM))
	SoGLModelMatrixElement::makeIdentity(action->getState(), this);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Changes transformation used when computing bounding box. Also
//    sets bounding box to empty if so specified.
//
// Use: extender

void
SoResetTransform::getBoundingBox(SoGetBoundingBoxAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // Do regular transform stuff
    SoResetTransform::doAction(action);

    // If supposed to reset bounding box to empty
    if (whatToReset.getValue() & BBOX) {
	action->getXfBoundingBox().makeEmpty();
	action->resetCenter();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns transformation matrix.
//
// Use: extender

void
SoResetTransform::getMatrix(SoGetMatrixAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // If has effect on transform
    if (! whatToReset.isIgnored() && (whatToReset.getValue() & TRANSFORM)) {

	// Overwrite the current matrices with identity
	action->getMatrix().makeIdentity();
	action->getInverse().makeIdentity();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Handles pick action.
//
// Use: extender

void
SoResetTransform::pick(SoPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoResetTransform::doAction(action);
}
