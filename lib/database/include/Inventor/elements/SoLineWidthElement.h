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
 |	This file defines the SoLineWidthElement class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_LINE_WIDTH_ELEMENT
#define  _SO_LINE_WIDTH_ELEMENT

#include <Inventor/elements/SoFloatElement.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoLineWidthElement
//
//  Element that stores the current line width.
//
//////////////////////////////////////////////////////////////////////////////

SoEXTENDER class SoLineWidthElement : public SoFloatElement {

    SO_ELEMENT_HEADER(SoLineWidthElement);

  public:
    // Initializes element
    virtual void	init(SoState *state);

    // Sets the current line width in the state
    static void		set(SoState *state, float width)
	{ SoFloatElement::set(classStackIndex, state, width); }

#ifndef IV_STRICT
    static void         set(SoState *state, SoNode *, float width)
        { set(state, width); }
#endif

    // Returns current line width from the state
    static float	get(SoState *state)
	{ return SoFloatElement::get(classStackIndex, state); }

    // Returns the default line width
    static float	getDefault()			{ return 0; }

  SoINTERNAL public:
    // Initializes the SoLineWidthElement class
    static void		initClass();

  protected:
    virtual ~SoLineWidthElement();
};

#endif /* _SO_LINE_WIDTH_ELEMENT */
