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

//  -*- C++ -*-

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Description:
 |	This file defines the SoGLUpdateAreaElement class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_UPDATE_AREA_ELEMENT
#define  _SO_UPDATE_AREA_ELEMENT

#include <Inventor/SbLinear.h>
#include <Inventor/elements/SoSubElement.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoGLUpdateAreaElement
//
//  Element that stores the rectangular area within the current
//  viewport region that needs to be updated when rendering. It can be
//  used for partial rendering updates when applications know that
//  only a portion of the objects need to be rerendered. Cameras can
//  use the info in this element to set up a view volume against which
//  culling can be performed. This element also sets up the GL scissor
//  box to limit drawing. 
//
//  The update area is specified in normalized viewport coordinates,
//  where (0,0) is the lower left corner of the viewport and (1,1) is
//  the upper right corner. The area is given as an origin and a size.
//
//////////////////////////////////////////////////////////////////////////////

SoEXTENDER class SoGLUpdateAreaElement : public SoElement {

    SO_ELEMENT_HEADER(SoGLUpdateAreaElement);

  public:
    // Initializes element
    virtual void	init(SoState *state);

    // Sets the current update area in the state. May have GL side effects.
    static void		set(SoState *state,
			    const SbVec2f &origin, const SbVec2f &size);

    // Returns current update area from the state. Returns TRUE if the
    // update area is the default, namely, the entire viewport.
    static SbBool	get(SoState *state, SbVec2f &origin, SbVec2f &size);

    // Returns the default update area origin and size
    static SbVec2f	getDefaultOrigin()	{ return SbVec2f(0.0, 0.0); }
    static SbVec2f	getDefaultSize()	{ return SbVec2f(1.0, 1.0); }

    // Override push() and pop() methods to keep GL up to date
    virtual void	push(SoState *state);
    virtual void	pop(SoState *state, const SoElement *prevTopElement);

    // Returns TRUE if the update areas match in both elements
    virtual SbBool	matches(const SoElement *elt) const;

    // Create and return a copy of this element
    virtual SoElement	*copyMatchInfo() const;

    // Prints element (for debugging)
    virtual void	print(FILE *fp) const;

  SoINTERNAL public:
    // Initializes the SoGLUpdateAreaElement class
    static void		initClass();

  protected:
    SbVec2f		origin, size;

    virtual ~SoGLUpdateAreaElement();

  private:
    // This flag lets us know if the element is set to its initial
    // default value, which is not sent to GL. This is so we can tell
    // whether the GL value is up to date.
    SbBool		isDefault;

    // Sends region in element to GL
    void		send(SoState *state);
};

#endif /* _SO_UPDATE_AREA_ELEMENT */
