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
 |	SoGLViewportRegionElement
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <GL/gl.h>
#include <Inventor/elements/SoGLViewportRegionElement.h>

SO_ELEMENT_SOURCE(SoGLViewportRegionElement);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoGLViewportRegionElement::~SoGLViewportRegionElement()
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
SoGLViewportRegionElement::init(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    // Do regular stuff
    SoViewportRegionElement::init(state);

    // Set flag to indicate we are using the default value
    isDefault = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pushes element, copying from previous element.
//
// Use: public

void
SoGLViewportRegionElement::push(SoState *)
//
////////////////////////////////////////////////////////////////////////
{
    const SoGLViewportRegionElement *nextElt =
	(const SoGLViewportRegionElement *) getNextInStack();

    viewportRegion = nextElt->viewportRegion;
    isDefault      = nextElt->isDefault;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pops element, causing side effects in GL.
//
// Use: public

void
SoGLViewportRegionElement::pop(SoState *state, const SoElement *prevTopElement)
//
////////////////////////////////////////////////////////////////////////
{
    // If the new top element had a default value, set it to the
    // current value, which will remain in effect in GL
    if (isDefault) {
	const SoGLViewportRegionElement *prevElt =
	    (const SoGLViewportRegionElement *) prevTopElement;
	viewportRegion = prevElt->viewportRegion;
	isDefault = FALSE;
    }

    // Otherwise, restore the previous viewport
    else {
	// Since popping this element has GL side effects, make sure any
	// open caches capture it
	capture(state);

	// Restore previous viewport region
	send();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets GLviewport region in element.
//
// Use: protected, virtual

void
SoGLViewportRegionElement::setElt(const SbViewportRegion &vpReg)
//
////////////////////////////////////////////////////////////////////////
{
    // If we already have a non-default value and it's the same as the
    // new one, do nothing
    // NOTE: since we know that no nodes set this element, we don't	???
    // have to worry about making open caches depend on it. This may	???
    // change later.							???
    if (! isDefault && viewportRegion == vpReg)
	return;

    // Set region in element and send to GL
    viewportRegion = vpReg;
    isDefault      = FALSE;
    send();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sends current viewport region to GL.
//
// Use: private

void
SoGLViewportRegionElement::send()
//
////////////////////////////////////////////////////////////////////////
{
    const SbVec2s	&vpOrig  = viewportRegion.getViewportOriginPixels();
    const SbVec2s	&vpSize  = viewportRegion.getViewportSizePixels();

    glViewport(vpOrig[0], vpOrig[1], vpSize[0], vpSize[1]);
}
