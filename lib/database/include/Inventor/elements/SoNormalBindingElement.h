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
 |	This file defines the SoNormalBindingElement class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_NORMAL_BINDING_ELEMENT
#define  _SO_NORMAL_BINDING_ELEMENT

#include <Inventor/elements/SoInt32Element.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoNormalBindingElement
//
//  Element that stores the current normal binding.
//
//////////////////////////////////////////////////////////////////////////////

SoEXTENDER class SoNormalBindingElement : public SoInt32Element {

    SO_ELEMENT_HEADER(SoNormalBindingElement);

  public:
    // The (seemingly random) choice of values is for compatibility
    // with Inventor 2.0 binary format files:
    enum Binding {
	OVERALL = 2,		// Whole object has same normal
	PER_PART = 3,		// One normal for each part of object
	PER_PART_INDEXED = 4,	// One normal for each part, indexed
	PER_FACE = 5,		// One normal for each face of object
	PER_FACE_INDEXED = 6,	// One normal for each face, indexed
	PER_VERTEX = 7,		// One normal for each vertex
	PER_VERTEX_INDEXED = 8	// One normal for each vertex, indexed
        
// C-api: end
#ifndef IV_STRICT
        // Obsolete bindings:
        ,                       // Comma to continue list
// C-api: begin
        DEFAULT = PER_VERTEX_INDEXED,
        NONE = OVERALL
// C-api: end
#endif
// C-api: begin

    };

    // Initializes element
    virtual void	init(SoState *state);

    // Sets the current normal binding in the state
    static void		set(SoState *state, Binding binding)
	{ SoInt32Element::set(classStackIndex, state, (int32_t)binding); }

#ifndef IV_STRICT
    static void         set(SoState *state, SoNode *, Binding binding)
        { set(state, binding); }
#endif

    // Returns current normal binding from the state
    static Binding	get(SoState *state)
	{ return (Binding)SoInt32Element::get(classStackIndex, state); }

    // Returns the default normal binding
    static Binding	getDefault()		{ return PER_VERTEX_INDEXED; }

    // Prints element (for debugging)
    virtual void	print(FILE *fp) const;

  SoINTERNAL public:
    // Initializes the SoNormalBindingElement class
    static void		initClass();

  protected:
    virtual ~SoNormalBindingElement();
};

#endif /* _SO_NORMAL_BINDING_ELEMENT */
