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
 |      SoTextureCoordinateEnvironment
 |
 |   Author(s)          : Thad Beier, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoPickAction.h>
#include <Inventor/elements/SoGLTextureCoordinateElement.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoTextureQualityElement.h>
#include <Inventor/elements/SoViewingMatrixElement.h>
#include <Inventor/nodes/SoTextureCoordinateEnvironment.h>

#include <GL/gl.h>

SO_NODE_SOURCE(SoTextureCoordinateEnvironment);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoTextureCoordinateEnvironment::SoTextureCoordinateEnvironment()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoTextureCoordinateEnvironment);
    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: public

SoTextureCoordinateEnvironment::~SoTextureCoordinateEnvironment()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Calculates texture coordinate
//
// Use: public

const SbVec4f &
SoTextureCoordinateEnvironment::valueCallback(void *action,
    const SbVec3f &point,
    const SbVec3f &normal)
//
////////////////////////////////////////////////////////////////////////
{
    SoAction *a = (SoAction *)action;

    //
    // See the glTexGen() man page for the math here.
    //

    // First, map normal and point into eye space:
    const SbMatrix &mm = SoModelMatrixElement::get(a->getState());
    const SbMatrix &vm = SoViewingMatrixElement::get(a->getState());

    // Compute the matrix that transforms normals from object-space to
    // eye-space; use the inverse transpose to scale correctly
    SbVec3f normalE;
    SbMatrix nm = (vm * mm).inverse().transpose();
    nm.multDirMatrix(normal, normalE);

    SbVec3f pointE;
    mm.multVecMatrix(point, pointE);  // Gives world-space point
    vm.multVecMatrix(pointE, pointE); // ... to eye-space.

    // Get the normalized vector from the eye (which is conveniently
    // at 0,0,0 in eye space) to the point.
    pointE.normalize();

    // Now, figure out reflection vector, from formula:
    // R = P - 2 (N . N) pointE
    SbVec3f reflection = pointE - 2.0 * normalE.dot(normalE) * pointE;

    // Finally, compute s/t coordinates...
    reflection[2] += 1.0;
    float magnitude = reflection.length();

    // This is static so we can return a reference to it
    static SbVec4f result;
    result.setValue(reflection[0] / magnitude + 0.5,
		    reflection[1] / magnitude + 0.5,
		    0.0, 1.0);

    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does GL render action.
//
// Use: protected

void
SoTextureCoordinateEnvironment::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoState *state = action->getState();

    // Special case to workaround OpenGL on Indigo/IndigoII bug:
    if (SoTextureOverrideElement::getQualityOverride(state) &&
	SoTextureQualityElement::get(state) == 0.0) return;

    // Let the state know that the GL is generating texture
    // coordinates.
    SoGLTextureCoordinateElement::setTexGen(state, this,
					    doTexgen, this,
					    valueCallback, action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback registered with the TextureCoordinateElement that keep
//    the GL up-to-date as the state is pushed/popped.
//
// Use: private, static

void
SoTextureCoordinateEnvironment::doTexgen(void *)
//
////////////////////////////////////////////////////////////////////////
{
    glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does callback action thing.
//
// Use: protected

void
SoTextureCoordinateEnvironment::callback(SoCallbackAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoTextureCoordinateEnvironment::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does pick action...
//
// Use: protected

void
SoTextureCoordinateEnvironment::pick(SoPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoTextureCoordinateEnvironment::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    doAction.  Add this node to the state
//
// Use: Extender public

void
SoTextureCoordinateEnvironment::doAction(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoState *state = action->getState();

    SoTextureCoordinateElement::setFunction(state, this,
					    valueCallback, action);
}

