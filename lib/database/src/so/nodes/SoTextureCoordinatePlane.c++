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
 |      SoTextureCoordinatePlane
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
#include <Inventor/elements/SoTextureQualityElement.h>
#include <Inventor/nodes/SoTextureCoordinatePlane.h>

#include <GL/gl.h>

SO_NODE_SOURCE(SoTextureCoordinatePlane);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoTextureCoordinatePlane::SoTextureCoordinatePlane()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoTextureCoordinatePlane);
    SO_NODE_ADD_FIELD(directionS,   (SbVec3f(1, 0, 0)));
    SO_NODE_ADD_FIELD(directionT,   (SbVec3f(0, 1, 0)));
    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: public

SoTextureCoordinatePlane::~SoTextureCoordinatePlane()
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
SoTextureCoordinatePlane::valueCallback(void *instance,
    const SbVec3f &position,
    const SbVec3f & /* normal - not used */)
//
////////////////////////////////////////////////////////////////////////
{
    SoTextureCoordinatePlane *tc = 
	(SoTextureCoordinatePlane *)instance;

    static SbVec4f result;

    const SbVec3f &ds = tc->directionS.getValue();
    result[0] = ds.dot(position);
    const SbVec3f &dt = tc->directionT.getValue();
    result[1] = dt.dot(position);
    result[2] = 0.0;
    result[3] = 1.0;

    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does GL render action.
//
// Use: protected

void
SoTextureCoordinatePlane::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoState *state = action->getState();

    // Special case to workaround OpenGL on Indigo/IndigoII bug:
    if (SoTextureOverrideElement::getQualityOverride(state) &&
	SoTextureQualityElement::get(state) == 0.0) return;

    SoGLTextureCoordinateElement::setTexGen(state, this, 
					    doTexgen, this,
					    valueCallback, this);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback registered with the TextureCoordinateElement that keep
//    the GL up-to-date as the state is pushed/popped.
//
// Use: private, static

void
SoTextureCoordinatePlane::doTexgen(void *me)
//
////////////////////////////////////////////////////////////////////////
{
    const SoTextureCoordinatePlane *p =
	(const SoTextureCoordinatePlane *)me;

    SbVec4f t;

    const SbVec3f &ds = p->directionS.getValue();
    t.setValue(ds[0], ds[1], ds[2], 0.0);
    glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenfv(GL_S, GL_OBJECT_PLANE, t.getValue());
    
    const SbVec3f &dt = p->directionT.getValue();
    t.setValue(dt[0], dt[1], dt[2], 0.0);
    glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenfv(GL_T, GL_OBJECT_PLANE, t.getValue());
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does callback action thing.
//
// Use: protected

void
SoTextureCoordinatePlane::callback(SoCallbackAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoTextureCoordinatePlane::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does pick action...
//
// Use: protected

void
SoTextureCoordinatePlane::pick(SoPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoTextureCoordinatePlane::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    doAction.  Add this node to the state
//
// Use: Extender public

void
SoTextureCoordinatePlane::doAction(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoState *state = action->getState();

    SoTextureCoordinateElement::setFunction(state, this,
					    valueCallback, this);
}
