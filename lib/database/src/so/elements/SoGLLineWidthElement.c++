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
 |	SoGLLineWidthElement
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <GL/gl.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/elements/SoGLLineWidthElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/misc/SoState.h>

SO_ELEMENT_SOURCE(SoGLLineWidthElement);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoGLLineWidthElement::~SoGLLineWidthElement()
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
SoGLLineWidthElement::init(SoState *_state)
//
////////////////////////////////////////////////////////////////////////
{
    // Initialize base class stuff
    SoLineWidthElement::init(_state);

    copiedFromParent = FALSE;

    // Save the state so we can get the SoViewportRegionElement when
    // we send the line width to GL
    state = _state;

}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pushes element, copying data from previous element
//
// Use: public

void
SoGLLineWidthElement::push(SoState *_state)
//
////////////////////////////////////////////////////////////////////////
{
    const SoGLLineWidthElement	*prevElt =
	(const SoGLLineWidthElement *) getNextInStack();

    data = prevElt->data;

    copiedFromParent = TRUE;
    state = _state;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pops element, causing side effects in GL.
//
// Use: public

void
SoGLLineWidthElement::pop(SoState *state, const SoElement *childElt)
//
////////////////////////////////////////////////////////////////////////
{
    // Since popping this element has GL side effects, make sure any
    // open caches capture it.  We may not send any GL commands, but
    // the cache dependency must exist even if we don't send any GL
    // commands, because if the element changes, the _lack_ of GL
    // commands here is a bug (remember, GL commands issued here are
    // put inside the cache).
    capture(state);
    copiedFromParent = FALSE;

    // If the previous element didn't have the same value...
    const SoGLLineWidthElement *child =
	(const SoGLLineWidthElement *)childElt;
	
    // Restore previous line width
    if (data != child->data)
	send();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets line width in element.
//
// Use: protected, virtual

void
SoGLLineWidthElement::setElt(float width)
//
////////////////////////////////////////////////////////////////////////
{
    // Optimization:  on push, we copy the value from the previous
    // element.  If the element is set to the same value, we don't
    // bother sending it, but do add a cache dependency on the
    // previous element.

    if (data != width) {
	data = width;
	send();
	copiedFromParent = FALSE;
    }
    else if (copiedFromParent) {
	SoGLLineWidthElement *parent =
	    (SoGLLineWidthElement *) getNextInStack();
	parent->capture(state);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sends line width in element to GL.
//
// Use: private

void
SoGLLineWidthElement::send()
//
////////////////////////////////////////////////////////////////////////
{
    float	width;

    // If line width is 0 (the default), use the default GL value of 1.0
    if (data == 0.0)
	width = 1.0;

    // Otherwise, compute the line width to be the appropriate number
    // of pixels, based on the current viewport region data
    else
	width = data * SoViewportRegionElement::get(state).getPixelsPerPoint();

    glLineWidth(width);
}
