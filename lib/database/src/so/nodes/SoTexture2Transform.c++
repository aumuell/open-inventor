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
 |	SoTexTransform
 |
 |   Author(s)          : Thad Beier, from SoTransform by PSS
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoPickAction.h>
#include <Inventor/elements/SoGLTextureMatrixElement.h>
#include <Inventor/elements/SoOverrideElement.h>
#include <Inventor/elements/SoTextureMatrixElement.h>
#include <Inventor/nodes/SoTexture2Transform.h>

SO_NODE_SOURCE(SoTexture2Transform);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoTexture2Transform::SoTexture2Transform()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoTexture2Transform);
    SO_NODE_ADD_FIELD(translation, (SbVec2f(0.0, 0.0)));
    SO_NODE_ADD_FIELD(rotation,    (0.0));
    SO_NODE_ADD_FIELD(scaleFactor, (SbVec2f(1.0, 1.0)));
    SO_NODE_ADD_FIELD(center,      (SbVec2f(0.0, 0.0)));
    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoTexture2Transform::~SoTexture2Transform()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does all the work for most actions
//
// Use: extender

void
SoTexture2Transform::doAction(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool	doCenter;
    SoState	*state = action->getState();
    SbVec2f	t2;
    SbVec3f	t3;

    doCenter = (! center.isIgnored() && ! center.isDefault());

    // Do these in GL (right-to-left) order
    if (! translation.isIgnored() && ! translation.isDefault()) {
	t2 = translation.getValue();
	t3.setValue(t2[0], t2[1], 0.0);
	SoTextureMatrixElement::translateBy(state, this, t3);
    }
    
    if (doCenter) {
	t2 = center.getValue();
	t3.setValue(t2[0], t2[1], 0.0);
	SoTextureMatrixElement::translateBy(state, this, t3);
    }

    if (! rotation.isIgnored() && ! rotation.isDefault()) {
	SbRotation tRot(SbVec3f(0, 0, 1), rotation.getValue());
	SoTextureMatrixElement::rotateBy(state, this, tRot);
    }

    if (! scaleFactor.isIgnored() && ! scaleFactor.isDefault()) {
	t2 = scaleFactor.getValue();
	t3.setValue(t2[0], t2[1], 1.0);
	SoTextureMatrixElement::scaleBy(state, this, t3);
    }
    if (doCenter) {
	t2 = -center.getValue();
	t3.setValue(t2[0], t2[1], 0.0);
	SoTextureMatrixElement::translateBy(state, this, t3);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Handles callback action
//
// Use: extender

void
SoTexture2Transform::callback(SoCallbackAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoTexture2Transform::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Handles GL render action
//
// Use: extender

void
SoTexture2Transform::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoTexture2Transform::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Handles get matrix action
//
// Use: extender

void
SoTexture2Transform::getMatrix(SoGetMatrixAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SbMatrix	&ctm = action->getTextureMatrix();
    SbMatrix	&inv = action->getTextureInverse();
    SbMatrix	m;
    SbBool	doCenter;
    SbVec2f	t2;
    SbVec3f	t3;

// Shorthand: accumulates translation stored in vector t2
#define TRANSLATE()							      \
    t3.setValue(t2[0], t2[1], 0.0);					      \
    m.setTranslate(t3);							      \
    ctm.multLeft(m);							      \
    m.setTranslate(-t3);						      \
    inv.multRight(m)

    doCenter = (! center.isIgnored() && ! center.isDefault());

    // Do these in GL (right-to-left) order
    if (! translation.isIgnored() && ! translation.isDefault()) {
	t2 = translation.getValue();
	TRANSLATE();
    }
    
    if (doCenter) {
	t2 = center.getValue();
	TRANSLATE();
    }

    if (! rotation.isIgnored() && ! rotation.isDefault()) {
	SbRotation tRot(SbVec3f(0, 0, 1), rotation.getValue());
	tRot.getValue(m);
	ctm.multLeft(m);
	tRot.invert();
	tRot.getValue(m);
	inv.multRight(m);
    }

    if (! scaleFactor.isIgnored() && ! scaleFactor.isDefault()) {
	t2 = scaleFactor.getValue();
	t3.setValue(t2[0], t2[1], 1.0);
	m.setScale(t3);
	ctm.multLeft(m);
	t3.setValue(1.0 / t2[0], 1.0 / t2[1], 1.0);
	m.setScale(t3);
	inv.multRight(m);
    }

    if (doCenter) {
	t2 = -center.getValue();
	TRANSLATE();
    }

#undef TRANSLATE
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Handles pick action.
//
// Use: extender

void
SoTexture2Transform::pick(SoPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoTexture2Transform::doAction(action);
}

