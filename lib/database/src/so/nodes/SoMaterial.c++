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
 |   $Revision: 1.2 $
 |
 |   Classes:
 |	SoMaterial
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
#include <Inventor/nodes/SoMaterial.h>

SO_NODE_SOURCE(SoMaterial);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoMaterial::SoMaterial()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoMaterial);

    SO_NODE_ADD_FIELD(ambientColor,  (SoLazyElement::getDefaultAmbient()));
    SO_NODE_ADD_FIELD(diffuseColor,  (SoLazyElement::getDefaultDiffuse()));
    SO_NODE_ADD_FIELD(specularColor,(SoLazyElement::getDefaultSpecular()));
    SO_NODE_ADD_FIELD(emissiveColor,(SoLazyElement::getDefaultEmissive()));
    SO_NODE_ADD_FIELD(shininess,        (SoLazyElement::getDefaultShininess()));
    SO_NODE_ADD_FIELD(transparency,  (SoLazyElement::getDefaultTransparency()));
    isBuiltIn = TRUE;
    colorPacker = new SoColorPacker;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoMaterial class.
//
// Use: internal

void
SoMaterial::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoMaterial, "Material", SoNode);

    // Enable elements:
    SO_ENABLE(SoCallbackAction, SoLazyElement);       
    SO_ENABLE(SoGLRenderAction, SoGLLazyElement);
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor (necessary since inline destructor is too complex)
//
// Use: private

SoMaterial::~SoMaterial()
//
////////////////////////////////////////////////////////////////////////
{
    delete colorPacker;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Performs accumulation of state for actions.
//
// Use: extender

void
SoMaterial::doAction(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoState		*state = action->getState();
    register uint32_t bitmask = 0;    

    // Set all non-ignored components

    if (! ambientColor.isIgnored() && ambientColor.getNum() > 0
	&& ! SoOverrideElement::getAmbientColorOverride(state)) {
	if (isOverride()) {
	    SoOverrideElement::setAmbientColorOverride(state, this, TRUE);
	}
	bitmask |= SoLazyElement::AMBIENT_MASK;	
    }

    if (! diffuseColor.isIgnored() && diffuseColor.getNum() > 0
	&& ! SoOverrideElement::getDiffuseColorOverride(state)) {
	if (isOverride()) {
	    SoOverrideElement::setDiffuseColorOverride(state, this, TRUE);
	    // Diffuse color and transparency share override state
            if (! transparency.isIgnored() && transparency.getNum() > 0)
                bitmask |= SoLazyElement::TRANSPARENCY_MASK;
	}
	bitmask |= SoLazyElement::DIFFUSE_MASK;
    }

    if (! transparency.isIgnored() && transparency.getNum() > 0
	&& ! SoOverrideElement::getTransparencyOverride(state)) {
	if (isOverride()) {
	    SoOverrideElement::setTransparencyOverride(state, this, TRUE);
	    // Diffuse color and transparency share override state
            if (! diffuseColor.isIgnored() && diffuseColor.getNum() > 0)
                bitmask |= SoLazyElement::DIFFUSE_MASK;
	}
        bitmask |= SoLazyElement::TRANSPARENCY_MASK;
    }
    if (! specularColor.isIgnored() && specularColor.getNum() > 0
	&& ! SoOverrideElement::getSpecularColorOverride(state)) {
	if (isOverride()) {
	    SoOverrideElement::setSpecularColorOverride(state, this, TRUE);
	}
	bitmask |= SoLazyElement::SPECULAR_MASK;
    }

    if (! emissiveColor.isIgnored() && emissiveColor.getNum() > 0
	&& ! SoOverrideElement::getEmissiveColorOverride(state)) {
	if (isOverride()) {
	    SoOverrideElement::setEmissiveColorOverride(state, this, TRUE);
	}
	bitmask |= SoLazyElement::EMISSIVE_MASK;
    }

    if (! shininess.isIgnored() && shininess.getNum() > 0
	&& ! SoOverrideElement::getShininessOverride(state)) {
	if (isOverride()) {
	    SoOverrideElement::setShininessOverride(state, this, TRUE);
	}
	bitmask |= SoLazyElement::SHININESS_MASK;
    }
    SoLazyElement::setMaterials(state, this, bitmask, colorPacker,   
	diffuseColor, transparency, ambientColor, 
	emissiveColor, specularColor, shininess);

}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Method for callback action
//
// Use: extender

void
SoMaterial::callback(SoCallbackAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoMaterial::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Method for GL rendering
//
// Use: extender

void
SoMaterial::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoMaterial::doAction(action);

    // If there's only one color, we might as well send it now.  This
    // prevents cache dependencies in some cases that were
    // specifically optimized for Inventor 2.0.
    if (diffuseColor.getNum() == 1 && !diffuseColor.isIgnored())
	SoGLLazyElement::sendAllMaterial(action->getState());
}
