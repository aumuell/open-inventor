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
 |	SoGLShapeHintsElement
 |
 |   Author(s)		: Paul S. Strauss, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <GL/gl.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/elements/SoGLShapeHintsElement.h>
#include <Inventor/misc/SoState.h>
#include <SoDebug.h>

SO_ELEMENT_SOURCE(SoGLShapeHintsElement);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoGLShapeHintsElement::~SoGLShapeHintsElement()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes element.
//
// Use: public

void
SoGLShapeHintsElement::init(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    // Let our base class do its stuff..
    SoShapeHintsElement::init(state);

    copiedFromParent = NULL;

}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides push() method to copy from previous element
//
// Use: public

void
SoGLShapeHintsElement::push(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    // Let the parent copy values for us...
    SoShapeHintsElement::push(state);

    // But remember that we're getting our values from above:
    copiedFromParent = state;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pops element, causing side effects in GL.
//
// Use: public

void
SoGLShapeHintsElement::pop(SoState *state, const SoElement *childElt)
//
////////////////////////////////////////////////////////////////////////
{
    // Since popping this element has GL side effects, make sure any
    // open caches capture it.   We may not send any GL commands, but
    // the cache dependency must exist even if we don't send any GL
    // commands, because if the element changes, the _lack_ of GL
    // commands here is a bug (remember, GL commands issued here are
    // put inside the cache).
    capture(state);
    copiedFromParent = NULL;

    // If the previous element didn't have the same value...
    const SoGLShapeHintsElement *child =
	(const SoGLShapeHintsElement *) childElt;

    // Restore previous shape hints if anything differs
    if (child->vertexOrdering != vertexOrdering	||
	child->shapeType      != shapeType)
	send();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Virtual set method, overridden to send GL commands
//
// Use: protected

void
SoGLShapeHintsElement::setElt(VertexOrdering _vertexOrdering,
			      ShapeType _shapeType, FaceType _faceType)
//
////////////////////////////////////////////////////////////////////////
{
    // Optimization:  on push, we copy the value from the previous
    // element.  If the element is set to the same value, we don't
    // bother sending it, but do add a cache dependency on the
    // previous element.

    if (_vertexOrdering == ORDERING_AS_IS) 
	_vertexOrdering = vertexOrdering;
    if (_faceType == FACE_TYPE_AS_IS) 
	_faceType = faceType;
    if (_shapeType == SHAPE_TYPE_AS_IS) 
	_shapeType = shapeType;

    // If anything is different, resend everything:
    // Note: don't bother with faceType, we don't send any GL commands
    // based on it (in OpenGL, everything is convex):
    SbBool needToSend = (vertexOrdering != _vertexOrdering ||
			 shapeType      != _shapeType);

    // Let base class keep state up to date:
    SoShapeHintsElement::setElt(_vertexOrdering, _shapeType, _faceType);

    if (needToSend) {
	send();
	copiedFromParent = NULL;
    }
    else if (copiedFromParent) {
	// Cache dependency-- we rely on previous element to set up GL
	// correctly.
	SoGLShapeHintsElement *parent =
	    (SoGLShapeHintsElement *) getNextInStack();
	parent->capture(copiedFromParent);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sends shape hints in element to GL.
//
// Use: private

void
SoGLShapeHintsElement::send()
//
////////////////////////////////////////////////////////////////////////
{
    if (vertexOrdering != UNKNOWN_ORDERING) {

	// Tell GL which ordering to use
	glFrontFace(vertexOrdering == CLOCKWISE ? GL_CW : GL_CCW);

	// If the shapes are solid and the vertices are ordered,
	// we know we can do successful backface culling
	if (shapeType == SOLID) {
	    glEnable(GL_CULL_FACE);
	    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
	}

	// If the shapes are not solid, but the vertices are ordered,
	// we know that we can do successful two-sided lighting
	else {
	    glDisable(GL_CULL_FACE);
	    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
#ifdef DEBUG
	    if (SoDebug::GetEnv("SO_DEBUG_PERF")) {
		SoDebug::RTPrintf("Two-sided lighting turned on\n");
	    }
#endif
	}
    }

    // We know nothing
    else {
	glDisable(GL_CULL_FACE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
    }
}
