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

/*--------------------------------------------------------------
 *  This is an example from the Inventor Toolmaker,
 *  chapter 2, example 2.
 *
 *  Source file for "Glow" property node.
 *------------------------------------------------------------*/

#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/elements/SoLazyElement.h>
#include <Inventor/elements/SoOverrideElement.h>

#include "Glow.h"

SO_NODE_SOURCE(Glow);

//
// Initializes the Glow class. This is a one-time thing that is
// done after database initialization and before any instance of
// this class is constructed.
//

void
Glow::initClass()
{
   // Initialize type id variables. The arguments to the macro
   // are: the name of the node class, the class this is derived
   // from, and the name registered with the type of the parent
   // class.
   SO_NODE_INIT_CLASS(Glow, SoNode, "Node");
}

//
// Constructor
//

Glow::Glow()
{
   // Do standard constructor stuff
   SO_NODE_CONSTRUCTOR(Glow);

   // Add "color" field to the field data. The default value for
   // this field is R=G=B=1, which is white
   SO_NODE_ADD_FIELD(color, (1.0, 1.0, 1.0));

   // Add "brightness" field to the field data. The default
   // value for this field is 0.
   SO_NODE_ADD_FIELD(brightness, (0.0));
   
   // Add "transparency" field to the field data.  Default value is 
   // 0 (opaque)
   SO_NODE_ADD_FIELD(transparency,  (0.0));
   
   // Initialize the color Packer (required of any property node that
   // uses an SoColorPacker to set diffuse color or transparency:
   colorPacker = new SoColorPacker;
   
}

//
// Destructor
//

Glow::~Glow()
{
    //  Delete the color Packer:
    delete colorPacker;
}

//
// Implements GL render action.
//

void
Glow::GLRender(SoGLRenderAction *action)
{
   // Set the elements in the state correctly. Note that we
   // prefix the call to doAction() with the class name. This
   // avoids problems if someone derives a new class from the
   // Glow node and inherits the GLRender() method; Glow's
   // doAction() will still be called in that case.
   Glow::doAction(action);

   //  Note that in Inventor 2.1 the GLRender method only sets the color in
   //  the lazy element, does not send it to GL.  This is for efficiency, since
   //  there is no need to send it until it is really used.

}

//
// Implements callback action.
//

void
Glow::callback(SoCallbackAction *action)
{
   // Set the elements in the state correctly
   Glow::doAction(action);
}

//
// Typical action implementation - it sets the correct element
// in the action's traversal state. We assume that the lazy element
// has been enabled.
//

void
Glow::doAction(SoAction *action)
{
   // Make sure the "brightness" field is not ignored. If it is,
   // then we don't need to change anything in the state.
   
   // Also check that the emissive color is not being overridden; if it is
   // this node should not set it.
   
   if (! brightness.isIgnored() && 
        ! SoOverrideElement::getEmissiveColorOverride(action->getState())) {

       SbColor emissiveColor = color.getValue() * brightness.getValue();

       //Use the Lazy element to set emissive color. 
       //Note that this will not actually send the color to GL.       
       SoLazyElement::setEmissive(action->getState(), &emissiveColor);
       
   }
   
   // To send transparency we again check ignore flag and override element.
 
   if (! transparency.isIgnored() &&
	! SoOverrideElement::getTransparencyOverride(action->getState())) {
   
	// keep a copy of the transparency that we are putting in the state:
	transpValue = transparency.getValue();
	
	// The color packer must be provided when the transparency is set,
	// so that the transparency will be merged with current diffuse color
	// in the state:
	SoLazyElement::setTransparency(action->getState(), this, 1, 
	    &transpValue,  colorPacker);
	    
    } 
}
