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
 |	SoGLTextureCoordinateElement class.
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson, Thad Beier,
 |                        Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <GL/gl.h>
#include <Inventor/elements/SoGLTextureCoordinateElement.h>
#include <Inventor/errors/SoDebugError.h>

SO_ELEMENT_SOURCE(SoGLTextureCoordinateElement);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoGLTextureCoordinateElement::~SoGLTextureCoordinateElement()
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
SoGLTextureCoordinateElement::init(SoState *s)
//
////////////////////////////////////////////////////////////////////////
{
    SoTextureCoordinateElement::init(s);

    texgenCB = NULL;
    texgenCBData = NULL;
    copiedFromParent = NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    TexGen is being used
//
// Use: public

void
SoGLTextureCoordinateElement::setTexGen(SoState *state, SoNode *node,
	SoTexCoordTexgenCB *texGenFunc, void *texGenData,
	SoTextureCoordinateFunctionCB *func, void *funcData)
//
////////////////////////////////////////////////////////////////////////
{
    // Do base-class stuff
    SoTextureCoordinateElement::setFunction(state, node, func,
					    funcData);
    
    // Get an element we can modify:
    SoGLTextureCoordinateElement	*elt;
    elt = (SoGLTextureCoordinateElement *)
	getElement(state, classStackIndex, node);

    if (elt != NULL) {

	elt->setElt(texGenFunc, texGenData);

    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    TexGen is being used
//
// Use: protected

void
SoGLTextureCoordinateElement::setElt(SoTexCoordTexgenCB *func,
				     void *userData)
//
////////////////////////////////////////////////////////////////////////
{
    // Enable or disable texgen as appropriate
    if (func != NULL) {
	// Only call Enable if not already enabled:
	if (texgenCB == NULL) {
	    glEnable(GL_TEXTURE_GEN_S);
	    glEnable(GL_TEXTURE_GEN_T);
	    copiedFromParent = NULL;
	}
	else if (copiedFromParent) {
	    SoGLTextureCoordinateElement *parent = 
		(SoGLTextureCoordinateElement *) getNextInStack();
	    parent->capture(copiedFromParent);
	}

	// Call function to set up texgen parameters
	(*func)(userData);

	whatKind     = FUNCTION;
    }
    else {
	// Only call Disable if parent element was enabled:
	if (texgenCB != NULL) {
	    glDisable(GL_TEXTURE_GEN_S);
	    glDisable(GL_TEXTURE_GEN_T);
	    copiedFromParent = NULL;
	}
	else if (copiedFromParent) {
	    SoGLTextureCoordinateElement *parent = 
		(SoGLTextureCoordinateElement *) getNextInStack();
	    parent->capture(copiedFromParent);
	}
    }

    texgenCB     = func;
    texgenCBData = userData;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns code depending on which set routine was called.
//
// Use: public, virtual

SoGLTextureCoordinateElement::CoordType
SoGLTextureCoordinateElement::getType() const
//
////////////////////////////////////////////////////////////////////////
{
    return whatKind;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the top (current) instance of the element in the state.
//
// Use: public

const SoGLTextureCoordinateElement *
SoGLTextureCoordinateElement::getInstance(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    return (const SoGLTextureCoordinateElement *)
	getConstElement(state, classStackIndex);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Given an index, send the appropriate stuff to GL.
//
// Use: public

void
SoGLTextureCoordinateElement::send(int index) const
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (whatKind != EXPLICIT)
	SoDebugError::post("SoGLTextureCoordinateElement::send",
			   "explicit texture coordinates were not set!");

    if (index < 0 || index >= numCoords)
	SoDebugError::post("SoGLTextureCoordinateElement::send",
			   "Index (%d) out of range 0 - %d",
			   index, numCoords - 1);
#endif /* DEBUG */

    if (coordsAre2D)
	glTexCoord2fv(coords2[index].getValue());
    else
	glTexCoord4fv(coords4[index].getValue());
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pushes element, initializing new instance.
//
// Use: public

void
SoGLTextureCoordinateElement::push(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    // Copy texgen function and data from previous instance
    const SoGLTextureCoordinateElement *prevElt =
	(const SoGLTextureCoordinateElement *) getNextInStack();
    texgenCB     = prevElt->texgenCB;
    texgenCBData = prevElt->texgenCBData;
    
    copiedFromParent = state;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pops element, causing side effects in GL if necessary.  This
//    code is a little convoluted-- we're trying pretty hard not to
//    send GL commands if we don't have to.
//
// Use: public

void
SoGLTextureCoordinateElement::pop(SoState *state, const SoElement *childElt)
//
////////////////////////////////////////////////////////////////////////
{
    // If the previous element didn't have the same value...
    const SoGLTextureCoordinateElement *child =
	(const SoGLTextureCoordinateElement *)childElt;

    // Since popping this element may have GL side effects, we must
    // capture the state.  We may not send any GL commands, but
    // the cache dependency must exist even if we don't send any GL
    // commands, because if the element changes, the _lack_ of GL
    // commands here is a bug (remember, GL commands issued here are
    // put inside the cache).
    capture(state);
    copiedFromParent = NULL;
    
    // Different callbacks, must either disable texgen or resend:
    if (texgenCB != NULL) {

	// Enable if it wasn't enabled before:
	if (child->texgenCB == NULL) {  
	    glEnable(GL_TEXTURE_GEN_S);
	    glEnable(GL_TEXTURE_GEN_T);
	}

	(*texgenCB)(texgenCBData);
    }
    else if (child->texgenCB != NULL) {
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
    }	
}
