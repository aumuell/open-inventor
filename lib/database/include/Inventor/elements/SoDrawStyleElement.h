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
 |	This file defines the SoDrawStyleElement class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_DRAW_STYLE_ELEMENT
#define  _SO_DRAW_STYLE_ELEMENT

#include <Inventor/elements/SoInt32Element.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoDrawStyleElement
//
//  Element that stores the current draw style.
//
//////////////////////////////////////////////////////////////////////////////

SoEXTENDER class SoDrawStyleElement : public SoInt32Element {

    SO_ELEMENT_HEADER(SoDrawStyleElement);

  public:
    // These are the available draw styles:
    enum Style {
	FILLED,			// Filled regions
	LINES,			// Outlined regions
	POINTS,			// Points
	INVISIBLE		// Nothing!
    };

    // Initializes element
    virtual void	init(SoState *state);

    // Sets the current draw style in the state
    static void		set(SoState *state, Style style);

#ifndef IV_STRICT
    static void         set(SoState *state, SoNode *, Style style)
        { set(state, style); }
#endif

    // Returns current draw style from the state
    static Style	get(SoState *state)
	{ return (Style)SoInt32Element::get(classStackIndex, state); }

    // Returns the default draw style
    static Style	getDefault()		{ return FILLED; }

    // Prints element (for debugging)
    virtual void	print(FILE *fp) const;

  SoINTERNAL public:
    // Initializes the SoDrawStyleElement class
    static void		initClass();

  protected:
    virtual ~SoDrawStyleElement();
};

#endif /* _SO_DRAW_STYLE_ELEMENT */
