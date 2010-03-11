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
 * Copyright (C) 1990-93   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Description:
 |      Definition of the SoInteraction class
 |
 |   Author(s): Thad Beier, David Mott, Alain Dumesny
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <Inventor/SoDB.h>
#include <Inventor/nodekits/SoNodeKit.h>
#include <Inventor/SoInteraction.h>
#include <Inventor/nodes/SoShape.h>
#include <Inventor/nodes/SoLight.h>
#include <Inventor/nodes/SoCamera.h>

#include <Inventor/manips/SoTransformManip.h>
#include <Inventor/manips/SoHandleBoxManip.h>

#include <Inventor/manips/SoCenterballManip.h>
#include <Inventor/manips/SoDirectionalLightManip.h>
#include <Inventor/manips/SoJackManip.h>
#include <Inventor/manips/SoPointLightManip.h>
#include <Inventor/manips/SoSpotLightManip.h>
#include <Inventor/manips/SoTabBoxManip.h>
#include <Inventor/manips/SoTrackballManip.h>
#include <Inventor/manips/SoTransformBoxManip.h>
#include <Inventor/manips/SoTransformerManip.h>

#include <Inventor/nodes/SoAntiSquish.h>
#include <Inventor/nodes/SoSurroundScale.h>

#include <Inventor/nodekits/SoInteractionKit.h>
#include <Inventor/draggers/SoDragger.h>

#include <Inventor/nodes/SoSelection.h>
#include <Inventor/actions/SoBoxHighlightRenderAction.h>
#include <Inventor/actions/SoLineHighlightRenderAction.h>

SbBool SoInteraction::initialized = FALSE;

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initialize all interaction class.
//
// Use: public

void
SoInteraction::init()
//
////////////////////////////////////////////////////////////////////////
{
    // Only initialize once
    if (! initialized) {
	
	SoDB::init(); // no problem if called multiple times
	SoNodeKit::init(); // no problem if called multiple times
	
	// Nodes used in interaction kits to help with transforms.
	SoAntiSquish::initClass();
	SoSurroundScale::initClass();

	// draggers
	SoInteractionKit::initClass();
	SoDragger::initClasses();

	// transform manips
	SoTransformManip::initClass();
	SoCenterballManip::initClass();
	SoHandleBoxManip::initClass();
	SoJackManip::initClass();
	SoTabBoxManip::initClass();
	SoTrackballManip::initClass();
	SoTransformBoxManip::initClass();
	SoTransformerManip::initClass();
	// lightManips
	SoPointLightManip::initClass();
	SoDirectionalLightManip::initClass();
	SoSpotLightManip::initClass();


	// nodes
	SoSelection::initClass();
	
	// actions
	SoBoxHighlightRenderAction::initClass();
	SoLineHighlightRenderAction::initClass();

	initialized = TRUE;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  Initializes the type ID for this manipulator node. This
//  should be called once after SoInteraction::init().
//
// Use: public
//
void
SoCenterballManip::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoCenterballManip, "CenterballManip", SoTransformManip);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  Initializes the type ID for this manipulator node. This
//  should be called once after SoInteraction::init().
//
// Use: public, internal
//
void
SoDirectionalLightManip::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoDirectionalLightManip, "DirectionalLightManip",
		       SoDirectionalLight);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initialize the node
//
// Use: public, internal
//

void
SoHandleBoxManip::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoHandleBoxManip, "HandleBoxManip", SoTransformManip);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  Initializes the type ID for this manipulator node. This
//  should be called once after SoInteraction::init().
//
// Use: public
//
void
SoJackManip::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS( SoJackManip, "JackManip", SoTransformManip );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initialize the node
//
// Use: public, internal
//
void
SoPointLightManip::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoPointLightManip, "PointLightManip", SoPointLight);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  Initializes the type ID for this manipulator node. This
//  should be called once after SoInteraction::init().
//
// Use: public
//
void
SoSpotLightManip::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS( SoSpotLightManip, "SpotLightManip", SoSpotLight);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  Initializes the type ID for this manipulator node. This
//  should be called once after SoInteraction::init().
//
// Use: public
//
void
SoTabBoxManip::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoTabBoxManip, "TabBoxManip", SoTransformManip);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    initialize the class
//
// Use: public, internal

void
SoTrackballManip::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoTrackballManip, "TrackballManip", SoTransformManip);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  Initializes the type ID for this manipulator node. This
//  should be called once after SoInteraction::init().
//
// Use: public
//
void
SoTransformManip::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoTransformManip, "TransformManip", SoTransform);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  Initializes the type ID for this manipulator node. This
//  should be called once after SoInteraction::init().
//
// Use: public
//
void
SoTransformBoxManip::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoTransformBoxManip, "TransformBoxManip", SoTransformManip);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  Initializes the type ID for this manipulator node. This
//  should be called once after SoInteraction::init().
//
// Use: public
//
void
SoTransformerManip::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoTransformerManip, "TransformerManip", SoTransformManip);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initialize the class
//
// Use: static, internal
//

void
SoInteractionKit::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__KIT_INIT_CLASS(SoInteractionKit, "InteractionKit", SoBaseKit);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoAntiSquish class.
//
// Use: internal

void
SoAntiSquish::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoAntiSquish, "AntiSquish", SoTransformation);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoSurroundScale class.
//
// Use: internal

void
SoSurroundScale::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoSurroundScale, "SurroundScale", SoTransformation);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoBoxHighlightRenderAction class.
//
void
SoBoxHighlightRenderAction::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_ACTION_INIT_CLASS(SoBoxHighlightRenderAction, SoGLRenderAction);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoGLRenderAction class.
//
void
SoLineHighlightRenderAction::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_ACTION_INIT_CLASS(SoLineHighlightRenderAction, SoGLRenderAction);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoSelection class.
//
// Use: private

void
SoSelection::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoSelection, "Selection", SoSeparator);
}

