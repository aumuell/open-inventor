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
 |      SoMaterialBinding
 |
 |   Author(s)          : Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoPickAction.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/elements/SoOverrideElement.h>

SO_NODE_SOURCE(SoMaterialBinding);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoMaterialBinding::SoMaterialBinding()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoMaterialBinding);

    SO_NODE_ADD_FIELD(value,
			 (SoMaterialBindingElement::getDefault()));

    // Set up static info for enumerated type field
    SO_NODE_DEFINE_ENUM_VALUE(Binding, OVERALL);
    SO_NODE_DEFINE_ENUM_VALUE(Binding, PER_PART);
    SO_NODE_DEFINE_ENUM_VALUE(Binding, PER_PART_INDEXED);
    SO_NODE_DEFINE_ENUM_VALUE(Binding, PER_FACE);
    SO_NODE_DEFINE_ENUM_VALUE(Binding, PER_FACE_INDEXED);
    SO_NODE_DEFINE_ENUM_VALUE(Binding, PER_VERTEX);
    SO_NODE_DEFINE_ENUM_VALUE(Binding, PER_VERTEX_INDEXED);

    // And obsolete bindings:
    if (firstInstance) {
	fieldData->addEnumValue("Binding", "DEFAULT", 0);
	fieldData->addEnumValue("Binding", "NONE", 1);
    }


    // Set up info in enumerated type field
    SO_NODE_SET_SF_ENUM_TYPE(value, Binding);

    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoMaterialBinding::~SoMaterialBinding()
//
////////////////////////////////////////////////////////////////////////
{
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Override read to translate obsolete bindings
//
// Use: extender

SbBool
SoMaterialBinding::readInstance(SoInput *in, unsigned short flags)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool result = SoNode::readInstance(in, flags);

    // Deal with obsolete bindings:
    int b = value.getValue();
    if (b == 0 || b == 1) value = OVERALL;

    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Handles most actions.
//
// Use: extender

void
SoMaterialBinding::doAction(SoAction *action)

//
////////////////////////////////////////////////////////////////////////
{
    if (! value.isIgnored() && 
	!SoOverrideElement::getMaterialBindingOverride(action->getState())) {
	if (isOverride()) {
	    SoOverrideElement::
		setMaterialBindingOverride(action->getState(), this, TRUE);
	}
	SoMaterialBindingElement::set(action->getState(),
		(SoMaterialBindingElement::Binding)value.getValue());
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does callback action thing.
//
// Use: extender

void
SoMaterialBinding::callback(SoCallbackAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoMaterialBinding::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does GL render action.
//
// Use: extender

void
SoMaterialBinding::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoMaterialBinding::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does pick action...
//
// Use: extender

void
SoMaterialBinding::pick(SoPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoMaterialBinding::doAction(action);
}
