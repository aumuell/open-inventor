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
 |	This file defines the SoMaterialBindingElement class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_MATERIAL_BINDING_ELEMENT
#define  _SO_MATERIAL_BINDING_ELEMENT

#include <Inventor/elements/SoInt32Element.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoMaterialBindingElement
//
//  Element that stores the current material binding.
//
//////////////////////////////////////////////////////////////////////////////

SoEXTENDER class SoMaterialBindingElement : public SoInt32Element {

    SO_ELEMENT_HEADER(SoMaterialBindingElement);

  public:
    // The (seemingly random) choice of values is for compatibility
    // with Inventor 2.0 binary format files:
    enum Binding {
	OVERALL = 2,		// Whole object has same material
	PER_PART = 3,		// One material for each part of object
	PER_PART_INDEXED = 4,	// One material for each part, indexed
	PER_FACE = 5,		// One material for each face of object
	PER_FACE_INDEXED = 6,	// One material for each face, indexed
	PER_VERTEX = 7,		// One material for each vertex
	PER_VERTEX_INDEXED = 8	// One material for each vertex, indexed

// C-api: end
#ifndef IV_STRICT
        // Obsolete bindings:
	,			// Comma to continue list
// C-api: begin
	DEFAULT = OVERALL,
	NONE = OVERALL
// C-api: end
#endif
// C-api: begin

    };

    // Initializes element
    virtual void	init(SoState *state);

    // Sets the current material binding in the state
    static void		set(SoState *state, Binding binding)
	{ SoInt32Element::set(classStackIndex, state, (int32_t)binding); }

#ifndef IV_STRICT
    static void         set(SoState *state, SoNode *, Binding binding)
        { set(state, binding); }
#endif

    // Returns current material binding from the state
    static Binding	get(SoState *state)
	{ return (Binding)SoInt32Element::get(classStackIndex, state); }

    // Returns the default material binding
    static Binding	getDefault()		{ return OVERALL; }

    // Prints element (for debugging)
    virtual void	print(FILE *fp) const;

  SoINTERNAL public:
    // Initializes the SoMaterialBindingElement class
    static void		initClass();

  protected:
    virtual ~SoMaterialBindingElement();
};

#endif /* _SO_MATERIAL_BINDING_ELEMENT */
