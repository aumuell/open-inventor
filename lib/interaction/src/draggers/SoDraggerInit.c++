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
 * Copyright (C) 1990,91,92   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Class:
 |	call initClasses for all dragger classes
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/draggers/SoCenterballDragger.h>
#include <Inventor/draggers/SoDirectionalLightDragger.h>
#include <Inventor/draggers/SoDragPointDragger.h>
#include <Inventor/draggers/SoDragger.h>
#include <Inventor/draggers/SoHandleBoxDragger.h>
#include <Inventor/draggers/SoJackDragger.h>
#include <Inventor/draggers/SoPointLightDragger.h>
#include <Inventor/draggers/SoRotateCylindricalDragger.h>
#include <Inventor/draggers/SoRotateDiscDragger.h>
#include <Inventor/draggers/SoRotateSphericalDragger.h>
#include <Inventor/draggers/SoScale1Dragger.h>
#include <Inventor/draggers/SoScale2Dragger.h>
#include <Inventor/draggers/SoScale2UniformDragger.h>
#include <Inventor/draggers/SoScaleUniformDragger.h>
#include <Inventor/draggers/SoSpotLightDragger.h>
#include <Inventor/draggers/SoTabBoxDragger.h>
#include <Inventor/draggers/SoTabPlaneDragger.h>
#include <Inventor/draggers/SoTrackballDragger.h>
#include <Inventor/draggers/SoTransformBoxDragger.h>
#include <Inventor/draggers/SoTransformerDragger.h>
#include <Inventor/draggers/SoTranslate1Dragger.h>
#include <Inventor/draggers/SoTranslate2Dragger.h>

#if 0
// Include ALL dragger geometry files here to separate them from
// read/write variables in the source:
#include "geom/SoCenterballDraggerGeom.h"
#include "geom/SoDirectionalLightDraggerGeom.h"
#include "geom/SoDragPointDraggerGeom.h"
#include "geom/SoHandleBoxDraggerGeom.h"
#include "geom/SoJackDraggerGeom.h"
#include "geom/SoPointLightDraggerGeom.h"
#include "geom/SoRotateCylindricalDraggerGeom.h"
#include "geom/SoRotateDiscDraggerGeom.h"
#include "geom/SoRotateSphericalDraggerGeom.h"
#include "geom/SoScale1DraggerGeom.h"
#include "geom/SoScale2DraggerGeom.h"
#include "geom/SoScale2UniformDraggerGeom.h"
#include "geom/SoScaleUniformDraggerGeom.h"
#include "geom/SoSpotLightDraggerGeom.h"
#include "geom/SoTabBoxDraggerGeom.h"
#include "geom/SoTabPlaneDraggerGeom.h"
#include "geom/SoTrackballDraggerGeom.h"
#include "geom/SoTransformBoxDraggerGeom.h"
#include "geom/SoTransformerDraggerGeom.h"
#include "geom/SoTranslate1DraggerGeom.h"
#include "geom/SoTranslate2DraggerGeom.h"
#endif

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes ALL Inventor dragger classes.
//
// Use: internal

void
SoDragger::initClasses()
//
////////////////////////////////////////////////////////////////////////
{
    SoDragger::initClass();
    // simple scale draggers
    SoScale1Dragger::initClass();
    SoScale2Dragger::initClass();
    SoScale2UniformDragger::initClass();
    SoScaleUniformDragger::initClass();
    // simple translate draggers
    SoTranslate1Dragger::initClass();
    SoTranslate2Dragger::initClass();
    // simple rotation draggers
    SoRotateSphericalDragger::initClass();
    SoRotateCylindricalDragger::initClass();
    SoRotateDiscDragger::initClass();
    // coord draggers
    SoDragPointDragger::initClass();
    // transform draggers
    SoJackDragger::initClass();
    SoHandleBoxDragger::initClass();
    SoCenterballDragger::initClass();
    SoTabPlaneDragger::initClass();
    SoTabBoxDragger::initClass();
    SoTrackballDragger::initClass();
    // composite transform draggers
    // init these after all the canonical draggers
    SoPointLightDragger::initClass();
    SoTransformBoxDragger::initClass();
    SoTransformerDragger::initClass();
    // lightDraggers
    SoDirectionalLightDragger::initClass();
    SoSpotLightDragger::initClass();
}	

////////////////////////////////////////////////////////////////////////
//
// Description:
//    initialize the class
//
// Use: public, internal

void
SoCenterballDragger::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__KIT_INIT_CLASS(SoCenterballDragger, "CenterballDragger", SoDragger);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  Initializes the type ID for this dragger node. This
//  should be called once after SoInteraction::init().
//
// Use: public, internal
//
void
SoDirectionalLightDragger::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__KIT_INIT_CLASS(SoDirectionalLightDragger, "DirectionalLightDragger",
		       SoDragger);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initialize the dragger
//
// Use: public, internal
//

void
SoDragPointDragger::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__KIT_INIT_CLASS(SoDragPointDragger, "DragPointDragger", SoDragger);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initialize the dragger
//
// Use: public, internal
//
void
SoDragger::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__KIT_INIT_CLASS(SoDragger, "Dragger", SoInteractionKit);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initialize the dragger
//
// Use: public, internal
//

void
SoHandleBoxDragger::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__KIT_INIT_CLASS(SoHandleBoxDragger, "HandleBoxDragger", SoDragger);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  Initializes the type ID for this dragger node. This
//  should be called once after SoInteraction::init().
//
// Use: public
//
void
SoJackDragger::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__KIT_INIT_CLASS(
		       SoJackDragger,	// class
		       "JackDragger",	// name to be written to files
		       SoDragger	// parent class
    );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initialize the dragger
//
// Use: public, internal
//
void
SoPointLightDragger::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__KIT_INIT_CLASS(SoPointLightDragger, "PointLightDragger", SoDragger);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initialize the dragger
//
// Use: public, internal
//
void
SoRotateCylindricalDragger::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__KIT_INIT_CLASS(SoRotateCylindricalDragger, "RotateCylindricalDragger",
		       SoDragger);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initialize the dragger
//
// Use: public, internal
//
void
SoRotateDiscDragger::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__KIT_INIT_CLASS(SoRotateDiscDragger, "RotateDiscDragger", SoDragger);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initialize the dragger
//
// Use: public, internal
//
void
SoRotateSphericalDragger::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__KIT_INIT_CLASS(SoRotateSphericalDragger, "RotateSphericalDragger",
		       SoDragger);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initialize the dragger
//
// Use: public, internal
//
void
SoScale1Dragger::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__KIT_INIT_CLASS(SoScale1Dragger, "Scale1Dragger", SoDragger);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initialize the dragger
//
// Use: public, internal
//
void
SoScale2Dragger::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__KIT_INIT_CLASS(SoScale2Dragger, "Scale2Dragger", SoDragger);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initialize the dragger
//
// Use: public, internal
//
void
SoScale2UniformDragger::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__KIT_INIT_CLASS(SoScale2UniformDragger, "Scale2UniformDragger",
		       SoDragger);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initialize the dragger
//
// Use: public, internal
//
void
SoScaleUniformDragger::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__KIT_INIT_CLASS(SoScaleUniformDragger, "ScaleUniformDragger", SoDragger);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  Initializes the type ID for this dragger node. This
//  should be called once after SoInteraction::init().
//
// Use: public
//
void
SoSpotLightDragger::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__KIT_INIT_CLASS(
		       SoSpotLightDragger,	// class
		       "SpotLightDragger",	// name to be written to files
		       SoDragger		// parent class
    );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  Initializes the type ID for this dragger node. This
//  should be called once after SoInteraction::init().
//
// Use: public
//
void
SoTabBoxDragger::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__KIT_INIT_CLASS(SoTabBoxDragger, "TabBoxDragger", SoDragger);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  Initializes the type ID for this dragger node. This
//  should be called once after SoInteraction::init().
//
// Use: public
//
void
SoTabPlaneDragger::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__KIT_INIT_CLASS(SoTabPlaneDragger, "TabPlaneDragger", SoDragger);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    initialize the class
//
// Use: public, internal

void
SoTrackballDragger::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__KIT_INIT_CLASS(SoTrackballDragger, "TrackballDragger", SoDragger);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  Initializes the type ID for this dragger node. This
//  should be called once after SoInteraction::init().
//
// Use: public
//
void
SoTransformBoxDragger::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__KIT_INIT_CLASS(SoTransformBoxDragger, "TransformBoxDragger", SoDragger);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  Initializes the type ID for this dragger node. This
//  should be called once after SoInteraction::init().
//
// Use: public
//
void
SoTransformerDragger::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__KIT_INIT_CLASS(SoTransformerDragger, "TransformerDragger", SoDragger);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initialize the dragger
//
// Use: public, internal
//
void
SoTranslate1Dragger::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__KIT_INIT_CLASS(SoTranslate1Dragger, "Translate1Dragger", SoDragger);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initialize the dragger
//
// Use: public, internal
//
void
SoTranslate2Dragger::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__KIT_INIT_CLASS(SoTranslate2Dragger, "Translate2Dragger", SoDragger);
}

