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
 |	SoGLTextureEnabledElement
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <GL/gl.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/elements/SoGLTextureEnabledElement.h>
#include <Inventor/elements/SoShapeStyleElement.h>
#include <Inventor/misc/SoState.h>

SO_ELEMENT_SOURCE(SoGLTextureEnabledElement);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoGLTextureEnabledElement::~SoGLTextureEnabledElement()
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
SoGLTextureEnabledElement::init(SoState *)
//
////////////////////////////////////////////////////////////////////////
{
    data = getDefault();

    copiedFromParent = NULL;
}

/////////////////////////////////////////////////////////////////////////
//
// Description:
//   Set the current texture enablement in the state
//
void	
SoGLTextureEnabledElement::set(SoState *state, SbBool value)
{
    SoInt32Element::set(classStackIndex, state, (int32_t)value); 	
    SoShapeStyleElement::setTextureEnabled(state,value);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides push() method to copy texture enabled flag
//
// Use: public

void
SoGLTextureEnabledElement::push(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    const SoGLTextureEnabledElement	*prevElt =
	(const SoGLTextureEnabledElement *) getNextInStack();

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
SoGLTextureEnabledElement::pop(SoState *state, const SoElement *childElt)
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
    const SoGLTextureEnabledElement *child =
	(const SoGLTextureEnabledElement *)childElt;
	
    // Restore previous enabled flag
    if (data != child->data)
	send();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets Enabled in element accessed from state.
//
// Use: internal, virtual

void
SoGLTextureEnabledElement::setElt(int32_t flag)
//
////////////////////////////////////////////////////////////////////////
{
    // Optimization:  on push, we copy the value from the previous
    // element.  If the element is set to the same value, we don't
    // bother sending it, but do add a cache dependency on the
    // previous element.

    if (data != flag) {
	data = flag;
	send();
	copiedFromParent = NULL;
    }
    else if (copiedFromParent) {
	SoGLTextureEnabledElement *parent = 
	    (SoGLTextureEnabledElement *) getNextInStack();
	parent->capture(copiedFromParent);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sends flag in element to GL.
//
// Use: private

void
SoGLTextureEnabledElement::send()
//
////////////////////////////////////////////////////////////////////////
{
    if (data)
	glEnable(GL_TEXTURE_2D);
    else
	glDisable(GL_TEXTURE_2D);
}
