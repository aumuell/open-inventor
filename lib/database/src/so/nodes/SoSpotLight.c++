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
 |      SoSpotLight
 |
 |   Author(s)          : Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <GL/gl.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/elements/SoGLLightIdElement.h>
#include <Inventor/elements/SoLightAttenuationElement.h>
#include <Inventor/nodes/SoSpotLight.h>

SO_NODE_SOURCE(SoSpotLight);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoSpotLight::SoSpotLight()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoSpotLight);

    SO_NODE_ADD_FIELD(location,	   (0.0, 0.0, 1.0));
    SO_NODE_ADD_FIELD(direction,   (0.0, 0.0, -1.0));
    SO_NODE_ADD_FIELD(dropOffRate, (0.0));
    SO_NODE_ADD_FIELD(cutOffAngle, (M_PI / 4.0));
    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoSpotLight::~SoSpotLight()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Creates a light source during rendering.
//
// Use: extender

void
SoSpotLight::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    int32_t	id;

    // Don't turn light on if it's off
    if (! on.getValue())
	return;

    // Get a new light id to use for this light
    id = SoGLLightIdElement::increment(action->getState());

    // Element is being overridden or we have too many sources for GL
    // to handle? Skip the whole deal.
    if (id < 0)
	return;

    //
    // Create a new source and send it to GL. The SoGLLightIdElement
    // has already enabled the light.
    //

    SbVec3f	v3;
    SbVec4f	v4;

    id = GL_LIGHT0 + id;

    // RGBA intensities of source are the product of the color and
    // intensity, with 1.0 alpha
    v3 = intensity.getValue() * color.getValue();
    v4.setValue(v3[0], v3[1], v3[2], 1.0);

    glLightfv((GLenum) id, GL_AMBIENT, SbVec4f(0.0, 0.0, 0.0, 1.0).getValue());
    glLightfv((GLenum) id, GL_DIFFUSE,  v4.getValue());
    glLightfv((GLenum) id, GL_SPECULAR, v4.getValue());

    // Set position
    v3 = location.getValue();
    v4.setValue(v3[0], v3[1], v3[2], 1.0);
    glLightfv((GLenum) id, GL_POSITION, v4.getValue());

    // Set up spotlight stuff. Note that the GL angle must be specified
    // in degrees, though the field is in radians
    glLightfv((GLenum) id, GL_SPOT_DIRECTION, direction.getValue().getValue());
//???
//???  This is a temporary fix, inserted because of a bug in openGL:
//???  You should be able to set GL_SPOT_EXPONENT to 0 and have it work. (It
//???  was fine in regular gl).  But in openGL, setting it to 0 results in 
//???  the light behaving like a point light, regardless of the cutoff angle.
//???  So, if dropOffRate is 0, well send down a value of .01 instead.
//???  
//???
    float dropRate = dropOffRate.getValue();
    if (dropRate <= 0.0)
        glLightf((GLenum) id, GL_SPOT_EXPONENT, .01);
    else 
        glLightf((GLenum) id, GL_SPOT_EXPONENT,  dropRate * 128.0);
    glLightf((GLenum) id, GL_SPOT_CUTOFF, cutOffAngle.getValue()*(180.0/M_PI));

    // Attenuation is accessed from the state
    const SbVec3f &atten = SoLightAttenuationElement::get(action->getState());
    glLightf((GLenum) id, GL_CONSTANT_ATTENUATION,  atten[2]);
    glLightf((GLenum) id, GL_LINEAR_ATTENUATION,    atten[1]);
    glLightf((GLenum) id, GL_QUADRATIC_ATTENUATION, atten[0]);
}
