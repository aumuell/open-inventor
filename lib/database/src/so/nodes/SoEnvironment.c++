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
 |   $Revision: 1.1 $
 |
 |   Classes:
 |      SoEnvironment
 |
 |   Author(s)          : Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <GL/gl.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/elements/SoLightAttenuationElement.h>
#include <Inventor/elements/SoViewVolumeElement.h>
#include <Inventor/nodes/SoEnvironment.h>

SO_NODE_SOURCE(SoEnvironment);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoEnvironment::SoEnvironment()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoEnvironment);
    SO_NODE_ADD_FIELD(ambientIntensity,	(0.2));
    SO_NODE_ADD_FIELD(ambientColor,	(1.0, 1.0, 1.0));
    SO_NODE_ADD_FIELD(attenuation,	(0.0, 0.0, 1.0));
    SO_NODE_ADD_FIELD(fogType,		(NONE));
    SO_NODE_ADD_FIELD(fogColor,		(1.0, 1.0, 1.0));
    SO_NODE_ADD_FIELD(fogVisibility,	(0.0));

    // Set up static info for enumerated type field
    SO_NODE_DEFINE_ENUM_VALUE(FogType, NONE);
    SO_NODE_DEFINE_ENUM_VALUE(FogType, HAZE);
    SO_NODE_DEFINE_ENUM_VALUE(FogType, FOG);
    SO_NODE_DEFINE_ENUM_VALUE(FogType, SMOKE);

    // Set up info in enumerated type fields
    SO_NODE_SET_SF_ENUM_TYPE(fogType, FogType);

    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoEnvironment::~SoEnvironment()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets up environment parameters during rendering.
//
// Use: extender

void
SoEnvironment::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f	v3;
    SbVec4f	v4;

    //////////////////////
    //
    // Set up ambient lighting.
    //

    // RGBA ambient intensity is the product of the color and
    // intensity, with 1.0 alpha
    v3 = ambientIntensity.getValue() * ambientColor.getValue();
    v4.setValue(v3[0], v3[1], v3[2], 1.0);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, v4.getValue());

    //////////////////////
    //
    // Set up light attenuation. This is stored in the
    // SoLightAttenuationElement, which is then accessed by subsequent
    // light sources.
    //

    SoLightAttenuationElement::set(action->getState(), this,
				   attenuation.getValue());

    //////////////////////
    //
    // Set up fog.
    //
    FogType	type = (FogType) fogType.getValue();

    if (type == NONE)
	glDisable(GL_FOG);

    else {
	float visibility = fogVisibility.getValue();

	// Check for visibility of 0, which is the default value - this
	// means that we should use the far plane of the current view
	// volume as the visibility
	if (visibility == 0.0) {
	    const SbViewVolume &vol =
		SoViewVolumeElement::get(action->getState());
	    visibility = vol.getNearDist() + vol.getDepth();
	}

	glEnable(GL_FOG);
	glFogfv(GL_FOG_COLOR, fogColor.getValue().getValue());

	switch (type) {

	  case NONE:
	    // Can't get here!
	    break;

	  case HAZE:
	    // Set up linear ramp based on visibility
	    glFogf(GL_FOG_MODE,		GL_LINEAR);
	    glFogf(GL_FOG_START,	0.0);
	    glFogf(GL_FOG_END,		visibility);
	    break;
	    
	  case FOG:
	    glEnable(GL_FOG);
	    glFogf(GL_FOG_MODE,		GL_EXP);
	    glFogf(GL_FOG_DENSITY,	computeDensity(visibility, FALSE));
	    break;
	    
	  case SMOKE:
	    glFogf(GL_FOG_MODE,		GL_EXP2);
	    glFogf(GL_FOG_DENSITY,	computeDensity(visibility, TRUE));
	    break;
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does callback action thing.
//
// Use: extender
void
SoEnvironment::callback(SoCallbackAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoLightAttenuationElement::set(action->getState(), this,
                                   attenuation.getValue());
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Computes fog density based on visibility.
//
// Use: private

float
SoEnvironment::computeDensity(float visibility, SbBool squared)
//
////////////////////////////////////////////////////////////////////////
{
    //
    // We want nearly total opacity at a distance of "visibility" from
    // the eye. The exponential GL fog function is:
    //
    //		f = e ** (-density * distance)
    //
    // (the exponent is squared in the SMOKE case)
    //
    // Since this function approaches 0 asymptotically, we have to
    // choose a reasonable cutoff point that approximates total
    // opacity. e ** -4 is about 0.018, so all we have to do is make
    // the exponent equal to -4 at a distance of "visibility".
    //

    if (squared)
	return 2.0 / visibility;

    else
	return 4.0 / visibility;
}
