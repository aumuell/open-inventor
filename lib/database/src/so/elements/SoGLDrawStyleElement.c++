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
 |	SoGLDrawStyleElement
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <GL/gl.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/elements/SoGLDrawStyleElement.h>
#include <Inventor/misc/SoState.h>

SO_ELEMENT_SOURCE(SoGLDrawStyleElement);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoGLDrawStyleElement::~SoGLDrawStyleElement()
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
SoGLDrawStyleElement::init(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    // Initialize base class stuff
    SoDrawStyleElement::init(state);

    copiedFromParent = NULL;

}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides push() method to copy draw style
//
// Use: public

void
SoGLDrawStyleElement::push(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    const SoGLDrawStyleElement	*prevElt =
	(const SoGLDrawStyleElement *) getNextInStack();

    data = prevElt->data;

    copiedFromParent = state;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pops element, causing side effects in GL.
//
// Use: public

void
SoGLDrawStyleElement::pop(SoState *state, const SoElement *childElt)
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
    copiedFromParent = NULL;

    // If the previous element didn't have the same value...
    const SoGLDrawStyleElement *child =
	(const SoGLDrawStyleElement *)childElt;
	
    // Restore previous draw style
    if (data != child->data)
	send();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets draw style in element.
//
// Use: protected, virtual

void
SoGLDrawStyleElement::setElt(int32_t style)
//
////////////////////////////////////////////////////////////////////////
{
    // Optimization:  on push, we copy the value from the previous
    // element.  If the element is set to the same value, we don't
    // bother sending it, but do add a cache dependency on the
    // previous element.

    if (data != style) {
	data = style;
	send();
	copiedFromParent = NULL;
    }
    else if (copiedFromParent) {
	SoGLDrawStyleElement *parent = 
	    (SoGLDrawStyleElement *)getNextInStack();
	parent->capture(copiedFromParent);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sends draw style in element to GL.
//
// Use: private

void
SoGLDrawStyleElement::send()
//
////////////////////////////////////////////////////////////////////////
{
    // Tell GL how to draw polygons. NOTE that this works only for
    // filled faces; shapes that contain lines still have to check for
    // POINTS draw styles and act accordingly.

    switch ((Style)data) {

      case FILLED:
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	break;

      case LINES:
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	break;

      case POINTS:
	glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	break;

      case INVISIBLE:
	// Nothing we can do here. This case is checked by the shapes
	// themselves.
	break;
    }
}
