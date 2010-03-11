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
 |      SoCallback
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
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/elements/SoGLCacheContextElement.h>
#include <Inventor/nodes/SoCallback.h>

SO_NODE_SOURCE(SoCallback);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoCallback::SoCallback()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoCallback);

    isBuiltIn    = TRUE;
    callbackFunc = NULL;
    callbackData = NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Copies the contents of the given node into this instance.
//
// Use: protected, virtual

void
SoCallback::copyContents(const SoFieldContainer *fromFC,
			 SbBool copyConnections)
//
////////////////////////////////////////////////////////////////////////
{
    // Copy the usual stuff
    SoNode::copyContents(fromFC, copyConnections);

    // Copy the callback function and data
    const SoCallback *fromCB = (const SoCallback *) fromFC;
    setCallback(fromCB->callbackFunc, fromCB->callbackData);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoCallback::~SoCallback()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Typical action method.
//
// Use: extender

void
SoCallback::doAction(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoType	actionType = action->getTypeId();
    SoState *state = action->getState();

    if (this->callbackFunc != NULL)
	(*this->callbackFunc)(this->callbackData, action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does the callback action
//
// Use: extender

void
SoCallback::callback(SoCallbackAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoCallback::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does the GL render action
//
// Use: extender

void
SoCallback::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // Ask to be cached, to match Inventor 2.0 default:
    SoGLCacheContextElement::shouldAutoCache(action->getState(), TRUE);
    
    SoCallback::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does the get bounding box action
//
// Use: extender

void
SoCallback::getBoundingBox(SoGetBoundingBoxAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoCallback::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does the get matrix action
//
// Use: extender

void
SoCallback::getMatrix(SoGetMatrixAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoCallback::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does the handle event thing
//
// Use: extender

void
SoCallback::handleEvent(SoHandleEventAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoCallback::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pick.
//
// Use: extender

void
SoCallback::pick(SoPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoCallback::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does search...
//
// Use: extender

void
SoCallback::search(SoSearchAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoCallback::doAction(action);
    SoNode::search(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does the write action
//
// Use: extender

void
SoCallback::write(SoWriteAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoCallback::doAction(action);
    SoNode::write(action);
}
