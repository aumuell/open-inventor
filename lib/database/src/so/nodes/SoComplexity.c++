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
 |      SoComplexity
 |
 |   Author(s)          : Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoPickAction.h>
#include <Inventor/elements/SoComplexityElement.h>
#include <Inventor/elements/SoTextureQualityElement.h>
#include <Inventor/elements/SoOverrideElement.h>
#include <Inventor/nodes/SoComplexity.h>

SO_NODE_SOURCE(SoComplexity);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoComplexity::SoComplexity()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoComplexity);

    SO_NODE_ADD_FIELD(type,  (SoComplexityTypeElement::getDefault()));
    SO_NODE_ADD_FIELD(value, (SoComplexityElement::getDefault()));
    SO_NODE_ADD_FIELD(textureQuality,
		      (SoTextureQualityElement::getDefault()));

    // Set up static info for enumerated type field
    SO_NODE_DEFINE_ENUM_VALUE(Type, OBJECT_SPACE);
    SO_NODE_DEFINE_ENUM_VALUE(Type, SCREEN_SPACE);
    SO_NODE_DEFINE_ENUM_VALUE(Type, BOUNDING_BOX);

    // Set up info in enumerated type field
    SO_NODE_SET_SF_ENUM_TYPE(type, Type);

    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: public

SoComplexity::~SoComplexity()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Handles any action.
//
// Use: extender

void
SoComplexity::doAction(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoState *state = action->getState();

    if (! type.isIgnored()
	&& ! SoOverrideElement::getComplexityTypeOverride(state)) {
	if (isOverride()) {
	    SoOverrideElement::setComplexityTypeOverride(state, this, TRUE);
	}
	SoComplexityTypeElement::set(state, (SoComplexityTypeElement::Type)
				     type.getValue());
    }

    if (! value.isIgnored()
	&& ! SoOverrideElement::getComplexityOverride(state)) {
	if (isOverride()) {
	    SoOverrideElement::setComplexityOverride(state, this, TRUE);
	}
	SoComplexityElement::set(state, value.getValue());
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does callback action thing.
//
// Use: extender

void
SoComplexity::callback(SoCallbackAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoComplexity::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does the get bounding box thing
//
// Use: extender

void
SoComplexity::getBoundingBox(SoGetBoundingBoxAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoComplexity::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does GL render action.
//
// Use: extender

void
SoComplexity::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoState *state = action->getState();

    if (! textureQuality.isIgnored()
	&& ! SoTextureOverrideElement::getQualityOverride(state)) {
	if (isOverride()) {
	    SoTextureOverrideElement::setQualityOverride(state, TRUE);
	}
	SoTextureQualityElement::set(state, textureQuality.getValue());
    }

    SoComplexity::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does pick action thing.
//
// Use: extender

void
SoComplexity::pick(SoPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoComplexity::doAction(action);
}
