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
 |	This file defines the SoGLLightIdElement class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_GL_LIGHT_ID_ELEMENT
#define  _SO_GL_LIGHT_ID_ELEMENT

#include <Inventor/elements/SoInt32Element.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoGLLightIdElement
//
//  Element that stores the id of the current light. The first light
//  active in a scene has id 0, the next has 1, and so on. This
//  element can be used to determine how many lights are currently
//  active. The initial value of this element is -1 to indicate that
//  there are no lights active.
//
//  Note that even though the effects of this element accumulate (each
//  light source increments the id), it is derived from
//  SoInt32Element. This is because each call to increment()
//  effectively does a "get" of the current top instance, so caching
//  knows about the dependency of each instance on the previous one.
//
//////////////////////////////////////////////////////////////////////////////

SoEXTENDER class SoGLLightIdElement : public SoInt32Element {

    SO_ELEMENT_HEADER(SoGLLightIdElement);

  public:
    // Initializes element
    virtual void	init(SoState *state);

    // Increments the current light id. This should be called when a
    // light source becomes active. This returns the new id, or -1 if
    // the maximum number of GL light sources has been exceeded.
    // Otherwise, enables light source.
    static int		increment(SoState *state);

#ifndef IV_STRICT
    static int		increment(SoState *state, SoNode *)
	{ return increment(state); }
#endif

    // Returns current light id from the state
    static int		get(SoState *state)
	{ return (int)SoInt32Element::get(classStackIndex, state); }

    // Returns the default light id
    static int		getDefault()			{ return -1; }

    // Returns the maximum number of concurrent light sources
    // supported by GL implementation
    static int		getMaxGLSources();

    // Overrides push() method to copy values from next instance in the stack
    virtual void	push(SoState *state);

    // Override pop() method so side effects can occur in GL
    virtual void	pop(SoState *state, const SoElement *prevTopElement);

    // Prints element (for debugging)
    virtual void	print(FILE *fp) const;

  SoINTERNAL public:
    // Initializes the SoGLLightIdElement class
    static void		initClass();

  protected:
    virtual ~SoGLLightIdElement();

  private:
    static int		maxGLSources;	// Holds max number of GL sources
};

#endif /* _SO_GL_LIGHT_ID_ELEMENT */
