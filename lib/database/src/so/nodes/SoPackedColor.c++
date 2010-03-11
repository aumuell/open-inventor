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
 |      SoPackedColor
 |
 |   Author(s)          : Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/elements/SoGLLazyElement.h>
#include <Inventor/elements/SoOverrideElement.h>
#include <Inventor/nodes/SoPackedColor.h>

SO_NODE_SOURCE(SoPackedColor);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoPackedColor::SoPackedColor()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoPackedColor);
    SO_NODE_ADD_FIELD(orderedRGBA, (SoLazyElement::getDefaultPacked()));
    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoPackedColor::~SoPackedColor()
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
SoPackedColor::doAction(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoState *state = action->getState();

    if (! orderedRGBA.isIgnored() && orderedRGBA.getNum() > 0
	&& ! SoOverrideElement::getDiffuseColorOverride(state)) {
	if (isOverride()) {
	    SoOverrideElement::setDiffuseColorOverride(state, this, TRUE);
	}
	SoLazyElement::setPacked(state, this,
			orderedRGBA.getNum(), orderedRGBA.getValues(0));
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Performs GL rendering on a packedColor node.
//
// Use: extender

void
SoPackedColor::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoPackedColor::doAction(action);

    // If there's only one color, we might as well send it now.  This
    // prevents cache dependencies in some cases that were
    // specifically optimized for Inventor 2.0.
    if (orderedRGBA.getNum() == 1 && !orderedRGBA.isIgnored())
	SoGLLazyElement::sendAllMaterial(action->getState());
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Performs callback action
//
// Use: extender

void
SoPackedColor::callback(SoCallbackAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoPackedColor::doAction(action);
}

///////////////////////////////////////////////////////////////////////////
//
// Description:
// When notified of change in orderedRGBA field, reevaluate transparent flag
//
// use: SoExtender public
//
void
SoPackedColor::notify(SoNotList *list)
//
////////////////////////////////////////////////////////////////////////
{
    if ((list->getLastRec()->getType() == SoNotRec::CONTAINER) &&
	(list->getLastField() == &orderedRGBA) ) {
	transparent = FALSE;
	for(int i = 0; i < orderedRGBA.getNum(); i++){
	    if((orderedRGBA[i] & 0xff) != 0xff){
		transparent = TRUE;
		break;
	    }	
	}
    }

    SoNode::notify(list);
}
