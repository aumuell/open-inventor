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
 |      SoShapeHints
 |
 |   Author(s)          : Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoPickAction.h>
#include <Inventor/elements/SoGLShapeHintsElement.h>
#include <Inventor/elements/SoCreaseAngleElement.h>
#include <Inventor/elements/SoOverrideElement.h>
#include <Inventor/nodes/SoShapeHints.h>

SO_NODE_SOURCE(SoShapeHints);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoShapeHints::SoShapeHints()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoShapeHints);

    SO_NODE_ADD_FIELD(vertexOrdering,
		      (SoShapeHintsElement::getDefaultVertexOrdering()));
    SO_NODE_ADD_FIELD(shapeType,
		      (SoShapeHintsElement::getDefaultShapeType()));
    SO_NODE_ADD_FIELD(faceType,
		      (SoShapeHintsElement::getDefaultFaceType()));
    SO_NODE_ADD_FIELD(creaseAngle,
		      (SoCreaseAngleElement::getDefault()));

    //
    // Set up static info for enumerated type fields
    //

    SO_NODE_DEFINE_ENUM_VALUE(VertexOrdering,	UNKNOWN_ORDERING);
    SO_NODE_DEFINE_ENUM_VALUE(VertexOrdering,	CLOCKWISE);
    SO_NODE_DEFINE_ENUM_VALUE(VertexOrdering,	COUNTERCLOCKWISE);

    SO_NODE_DEFINE_ENUM_VALUE(ShapeType,	UNKNOWN_SHAPE_TYPE);
    SO_NODE_DEFINE_ENUM_VALUE(ShapeType,	SOLID);

    SO_NODE_DEFINE_ENUM_VALUE(FaceType,		UNKNOWN_FACE_TYPE);
    SO_NODE_DEFINE_ENUM_VALUE(FaceType,		CONVEX);

    SO_NODE_SET_SF_ENUM_TYPE(vertexOrdering,	VertexOrdering);
    SO_NODE_SET_SF_ENUM_TYPE(shapeType,		ShapeType);
    SO_NODE_SET_SF_ENUM_TYPE(faceType,		FaceType);

    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoShapeHints::~SoShapeHints()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Updates state for actions
//
// Use: extender

void
SoShapeHints::doAction(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoState *state = action->getState();

    if (! creaseAngle.isIgnored()
	&& ! SoOverrideElement::getCreaseAngleOverride(state)) {
	if (isOverride()) {
	    SoOverrideElement::setCreaseAngleOverride(state, this, TRUE);
	}
	SoCreaseAngleElement::set(state, creaseAngle.getValue());
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Updates state for actions that also set shape hints element.
//
// Use: private

void
SoShapeHints::doAction2(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoShapeHints::doAction(action);
    SoState *state = action->getState();

    if (SoOverrideElement::getShapeHintsOverride(state)) {
	return;
    }

    if (isOverride()) {
	SoOverrideElement::setShapeHintsOverride(state, this, TRUE);
    }

    //
    // Determine which hints to set. If any are ignored, leave them AS_IS
    //

    SoShapeHintsElement::VertexOrdering	order;
    SoShapeHintsElement::ShapeType		shape;
    SoShapeHintsElement::FaceType		face;

    order = (vertexOrdering.isIgnored() ?
	     SoShapeHintsElement::ORDERING_AS_IS :
	     (SoShapeHintsElement::VertexOrdering)vertexOrdering.getValue());

    shape  = (shapeType.isIgnored() ?
	     SoShapeHintsElement::SHAPE_TYPE_AS_IS :
	     (SoShapeHintsElement::ShapeType) shapeType.getValue());

    face  = (faceType.isIgnored() ?
	     SoShapeHintsElement::FACE_TYPE_AS_IS :
	     (SoShapeHintsElement::FaceType) faceType.getValue());

    SoShapeHintsElement::set(state, order, shape, face);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Handles callback action
//
// Use: extender

void
SoShapeHints::callback(SoCallbackAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoShapeHints::doAction2(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Handles GL render action
//
// Use: extender

void
SoShapeHints::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoShapeHints::doAction2(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Handles pick action
//
// Use: extender

void
SoShapeHints::pick(SoPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoShapeHints::doAction2(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Handles get bounding box action
//
// Use: extender

void
SoShapeHints::getBoundingBox(SoGetBoundingBoxAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoShapeHints::doAction(action);
}
