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
 |	This file defines the SoGLTextureEnabledElement class.
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_GL_TEXTURE_ENABLED_ELEMENT
#define  _SO_GL_TEXTURE_ENABLED_ELEMENT

#include <Inventor/elements/SoInt32Element.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoGLTextureEnabledElement
//
//  Element that enables/disables textures.  This is implemented as a
//  separate element from the TextureImageElement so the texture image
//  can be changed without blowing caches containing shapes-- shapes
//  need to know if texturing is going on or not so they can decide to
//  send down texture coordinates or not.
//
//  This is set by the GLTextureImageElement and the
//  GLTextureImageElement; the interaction between the three is a
//  little complicated.  Each of the other two elements always sets up
//  the GL texture state appropriately; however, texturing is only
//  enabled if both elements agree that it should be enabled (they
//  check each other's value before calling set).
//
//  This element cannot be overridden, but, since the elements that
//  set it check each other's value, overriding those elements has
//  the same effect.
//
//////////////////////////////////////////////////////////////////////////////

SoEXTENDER class SoGLTextureEnabledElement :
	public SoInt32Element {

    SO_ELEMENT_HEADER(SoGLTextureEnabledElement);

  public:
    // Initializes element
    virtual void	init(SoState *state);

    // Used by GLTextureImageElement, set to TRUE when a non-null
    // image is set, set to FALSE when there is no texture image (the
    // default).
    static void		set(SoState *state, SbBool value);

#ifndef IV_STRICT
    static void         set(SoState *state, SoNode *, SbBool value)
        { set(state, value); }
#endif

    // Used by shapes to figure out if they need to send texture
    // coordinates:
    static SbBool	get(SoState *state)
	{ return (SbBool)SoInt32Element::get(classStackIndex, state); }

    // By default there is not texture
    static SbBool	getDefault()	{ return FALSE; }

    // Override push() so we can remember previous elements value and
    // avoid making GL calls if this element has the same value.
    virtual void	push(SoState *state);

    // Override pop() method so side effects can occur in GL
    virtual void	pop(SoState *state, const SoElement *prevTopElement);

  SoINTERNAL public:
    // Initializes the SoGLTextureEnabledElement class
    static void		initClass();

    // Override base class set to make GL calls.
    virtual void	setElt(int32_t value);

    virtual ~SoGLTextureEnabledElement();

  private:
    // We save the state to figure out if the lastPattern variable was
    // copied from a parent element; if it was, then caches will have
    // to depend on that element because if it changes we have to have
    // a chance to change our decision about what GL calls to make.
    // If this is NULL, then there are no cache dependencies.
    SoState *		copiedFromParent;

    // Sends enabled in element to GL
    void		send();
};

#endif /* _SO_GL_TEXTURE_ENABLED_ELEMENT */
