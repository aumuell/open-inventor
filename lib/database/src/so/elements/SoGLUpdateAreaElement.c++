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
 |	SoGLUpdateAreaElement
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <GL/gl.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/elements/SoGLUpdateAreaElement.h>

SO_ELEMENT_SOURCE(SoGLUpdateAreaElement);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoGLUpdateAreaElement::~SoGLUpdateAreaElement()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes element
//
// Use: public

void
SoGLUpdateAreaElement::init(SoState *)
//
////////////////////////////////////////////////////////////////////////
{
    origin = getDefaultOrigin();
    size   = getDefaultSize();

    // Set flag to indicate we are using the default value
    isDefault = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets update area in element accessed from state.
//
// Use: public, static

void
SoGLUpdateAreaElement::set(SoState *state,
			   const SbVec2f &origin, const SbVec2f &size)
//
////////////////////////////////////////////////////////////////////////
{
    SoGLUpdateAreaElement	*elt;

    // Get an instance we can change (pushing if necessary)
    elt = (SoGLUpdateAreaElement *) getElement(state, classStackIndex);

    if (elt != NULL) {
	// If we already have a non-default value and it's the same as the
	// new one, do nothing
	// NOTE: since we know that no nodes set this element, we don't	???
	// have to worry about making open caches depend on it. This may???
	// change later.						???
	if (! elt->isDefault && elt->origin == origin && elt->size == size)
	    return;

	// Set area in element and send to GL
	elt->origin = origin;
	elt->size   = size;
	elt->isDefault = FALSE;
	elt->send(state);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns update area from state. Returns TRUE if the update area
//    is the default, namely, the entire viewport.
//
// Use: public, static

SbBool
SoGLUpdateAreaElement::get(SoState *state, SbVec2f &origin, SbVec2f &size)
//
////////////////////////////////////////////////////////////////////////
{
    const SoGLUpdateAreaElement *elt;

    elt = (const SoGLUpdateAreaElement *)
	getConstElement(state, classStackIndex);

    origin = elt->origin;
    size   = elt->size;

    return (origin == getDefaultOrigin() && size == getDefaultSize());
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pushes element, copying from previous element.
//
// Use: public

void
SoGLUpdateAreaElement::push(SoState *)
//
////////////////////////////////////////////////////////////////////////
{
    const SoGLUpdateAreaElement *nextElt =
	(const SoGLUpdateAreaElement *) getNextInStack();

    origin	= nextElt->origin;
    size	= nextElt->size;
    isDefault	= nextElt->isDefault;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pops element, causing side effects in GL.
//
// Use: public

void
SoGLUpdateAreaElement::pop(SoState *state, const SoElement *prevTopElement)
//
////////////////////////////////////////////////////////////////////////
{
    // If the new top element had a default value, set it to the
    // current value, which will remain in effect in GL
    if (isDefault) {
	const SoGLUpdateAreaElement *prevElt =
	    (const SoGLUpdateAreaElement *) prevTopElement;
	origin	  = prevElt->origin;
	size	  = prevElt->size;
	isDefault = FALSE;
    }

    // Otherwise, restore the previous update area
    else {
	// Since popping this element has GL side effects, make sure any
	// open caches capture it
	capture(state);

	// Restore previous update area
	send(state);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides this method to compare update areas.
//
// Use: public

SbBool
SoGLUpdateAreaElement::matches(const SoElement *elt) const
//
////////////////////////////////////////////////////////////////////////
{
    const SoGLUpdateAreaElement *uaElt = (const SoGLUpdateAreaElement *) elt;

    return (origin == uaElt->origin && size == uaElt->size);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//     Create a copy of this instance suitable for calling matches()
//     on.
//
// Use: protected

SoElement *
SoGLUpdateAreaElement::copyMatchInfo() const
//
////////////////////////////////////////////////////////////////////////
{
    SoGLUpdateAreaElement *result =
	(SoGLUpdateAreaElement *)getTypeId().createInstance();

    result->origin = origin;
    result->size   = size;
    
    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Prints element for debugging.
//
// Use: public
//
////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
void
SoGLUpdateAreaElement::print(FILE *fp) const
{
    SoElement::print(fp);

    fprintf(fp, "\tUpdate Area: origin = (%g, %g), size = (%g, %g)\n",
	    origin[0], origin[1], size[0], size[1]);
}
#else  /* DEBUG */
void
SoGLUpdateAreaElement::print(FILE *) const
{
}
#endif /* DEBUG */

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sends current update area to GL.
//
// Use: private

void
SoGLUpdateAreaElement::send(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    // We need to know the current window size. This could
    // potentially be wrong if the window size changes during
    // rendering, but that's just not possible.
    const SbViewportRegion	&vpReg = SoViewportRegionElement::get(state);
    const SbVec2s		&winSize = vpReg.getWindowSize();

    glScissor((GLint) (origin[0] * winSize[0]),
	      (GLint) (origin[1] * winSize[1]),
	      (GLsizei) (size[0] * winSize[0]),
	      (GLsizei) (size[1] * winSize[1]));
}
