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
 |   $Revision $
 |
 |   Classes:
 |	SoNormalBundle class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/errors/SoDebugError.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/bundles/SoNormalBundle.h>
#include <Inventor/elements/SoCreaseAngleElement.h>
#include <Inventor/elements/SoNormalBindingElement.h>
#include <Inventor/elements/SoShapeHintsElement.h>

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoNormalBundle::SoNormalBundle(SoAction *action, SbBool forRendering) :
	SoBundle(action)
//
////////////////////////////////////////////////////////////////////////
{
    normElt = SoNormalElement::getInstance(state);

    isRendering = forRendering;

    // If we are using the bundle for GL rendering, access a GL
    // version of the element for sending normals to GL
    GLNormElt = isRendering ? (const SoGLNormalElement *) normElt : NULL;

    generator   = NULL;
    pushedState = FALSE;

    // Save a pointer to the node that created the bundle, which is
    // assumed to be the tail of the current path in the action
    currentNode = action->getCurPathTail();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: public

SoNormalBundle::~SoNormalBundle()
//
////////////////////////////////////////////////////////////////////////
{
    // Restore state if we did a push() in generate() or set()
    if (pushedState)
	state->pop();

    if (generator != NULL)
	delete generator;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//
// Return TRUE if there are no normals in the state.
//
// Use: public

SbBool
SoNormalBundle::shouldGenerate(int numNormalsNeeded)
//
////////////////////////////////////////////////////////////////////////
{
    if (normElt->getNum() > 0) return FALSE;

    initGenerator(numNormalsNeeded);

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes generation in the case where shouldGenerate() is not
//    called. (shouldGenerate() does this automatically).
//
// Use: public

void
SoNormalBundle::initGenerator(int initialNum)
//
////////////////////////////////////////////////////////////////////////
{
    // Figure out whether or not polys are clockwise or
    // counter-clockwise
    SbBool ccw = TRUE;
    SoShapeHintsElement::VertexOrdering vertexOrdering;
    SoShapeHintsElement::ShapeType shapeType;
    SoShapeHintsElement::FaceType faceType;
    SoShapeHintsElement::get(state, vertexOrdering, shapeType, faceType);
    if (vertexOrdering == SoShapeHintsElement::CLOCKWISE) ccw = FALSE;

    if (generator != NULL) delete generator;
    generator = new SoNormalGenerator(ccw, initialNum);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Calculates the normals once all vertices have been sent. The
//    normals are stored by the bundle so the caller does not have to
//    deal with them directly. The startIndex argument specifies the
//    index at which the generated normals will begin - it can be used
//    by shapes that allow the coordinates and normals to be offset.
//
// Use: public

void
SoNormalBundle::generate(int startIndex, SbBool addToState)
//
////////////////////////////////////////////////////////////////////////
{
    // Get crease angle from state and generate normals
    generator->generate(SoCreaseAngleElement::get(state));

    // Offset the normals, if necessary:
    if (startIndex > 0) {
	int numNorms = generator->getNumNormals();
        for (int i = numNorms-1; i >= 0; i--) {
	    SbVec3f n = generator->getNormal(i);
	    generator->setNormal(i+startIndex, n);
	}
    }

    // Set the normals in the state
    if (addToState) {
	set(generator->getNumNormals(), generator->getNormals());
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This method may be called by shapes that generate their own
//    normals and want them added to the state.
//
// Use: public

void
SoNormalBundle::set(int32_t numNormals, const SbVec3f *normals)
//
////////////////////////////////////////////////////////////////////////
{
    // Push the state before we change it
    state->push();
    pushedState = TRUE;

    // Set the normals in the normal element
    SoNormalElement::set(state, currentNode,
			 numNormals, normals);

    // Get the new instance of the normal element for inquiring or
    // sending normals
    normElt = SoNormalElement::getInstance(state);
    if (isRendering)
	GLNormElt = (const SoGLNormalElement *) normElt;
}
