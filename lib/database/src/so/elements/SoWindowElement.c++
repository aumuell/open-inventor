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
 * Copyright (C) 1995-96   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |	SoWindowElement
 |
 |   Author(s)		: Alain Dumesny
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/elements/SoWindowElement.h>
#include <Inventor/errors/SoDebugError.h>

SO_ELEMENT_SOURCE(SoWindowElement);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoWindowElement::~SoWindowElement()
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
SoWindowElement::init(SoState *)
//
////////////////////////////////////////////////////////////////////////
{
    window = 0;
    context = NULL;
    display = NULL;
    glRenderAction = NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the window attributes
//
// Use: public, static

void
SoWindowElement::set(SoState *state, Window w, GLXContext ctx, 
    Display *d, SoGLRenderAction *glAc)
//
////////////////////////////////////////////////////////////////////////
{
    SoWindowElement	*elt;

    // Get an instance we can change (pushing if necessary)
    elt = (SoWindowElement *) getElement(state, classStackIndex);

    elt->window = w;
    elt->context = ctx;
    elt->display = d;
    elt->glRenderAction = glAc;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the window information from the state
//
// Use: public, static

void
SoWindowElement::get(SoState *state, Window &w, GLXContext &ctx, 
    Display *&d, SoGLRenderAction *&glAc)
//
////////////////////////////////////////////////////////////////////////
{
    const SoWindowElement *elt;

    elt = (const SoWindowElement *) getConstElement(state, classStackIndex);

    w	= elt->window;
    ctx = elt->context;
    d	= elt->display;
    glAc = elt->glRenderAction;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    this should NOT be called - for anything but the very first depth
//  (since depth 0 elements are supposed to be read only).
//
// Use: public virtual

void
SoWindowElement::push(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (state->getDepth() > 1)
	SoDebugError::post("SoWindowElement::push",
	    "must not set this element");
#endif
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides this method to always return TRUE. This element
//    should never have anything to do with cache invalidation.
//
// Use: public

SbBool
SoWindowElement::matches(const SoElement *) const
//
////////////////////////////////////////////////////////////////////////
{
    SoDebugError::post("SoWindowElement::matches",
		       "This method should never be called!");

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//     Override method to return NULL; it should never be called.
//
// Use: protected

SoElement *
SoWindowElement::copyMatchInfo() const
//
////////////////////////////////////////////////////////////////////////
{
    SoDebugError::post("SoWindowElement::copyMatchInfo",
		       "This method should never be called!");
    
    SoWindowElement *elt = new SoWindowElement;
    elt->window = window;
    elt->context = context;
    elt->display = display;
    elt->glRenderAction = glRenderAction;
    
    return elt;
}

