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
 |      SoDirectionalLight
 |
 |   Author(s)          : Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <GL/gl.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/elements/SoGLLightIdElement.h>
#include <Inventor/nodes/SoDirectionalLight.h>

SO_NODE_SOURCE(SoDirectionalLight);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoDirectionalLight::SoDirectionalLight()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoDirectionalLight);
    SO_NODE_ADD_FIELD(direction, (0.0, 0.0, -1.0));
    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoDirectionalLight::~SoDirectionalLight()
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
SoDirectionalLight::GLRender(SoGLRenderAction *action)
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

    // "Position" is the direction vector negated with a 0.0 w
    // component. Yet another GL peccadillo.
    v3 = direction.getValue();
    v4.setValue(-v3[0], -v3[1], -v3[2], 0.0);
    glLightfv((GLenum) id, GL_POSITION, v4.getValue());

    // Make sure no spotlight stuff is on
    glLightf((GLenum) id, GL_SPOT_EXPONENT, 0.0);
    glLightf((GLenum) id, GL_SPOT_CUTOFF, 180.0);

    // Attenuation does not matter for directional sources.
}
