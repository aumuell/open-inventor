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
 |	This file defines the abstract SoEXTENDER SoInt32Element class.
 |      This is a base class used to make the library smaller by
 |      sharing code.
 |
 |   Classes:
 |	SoInt32Element
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_INT32_ELEMENT
#define  _SO_INT32_ELEMENT

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoInt32Element
//
//  Subclasses store a single int32_t, int, or enum value.  This class
//  defines generic matches() and copy() methods.
//
//////////////////////////////////////////////////////////////////////////////

#include <Inventor/elements/SoSubElement.h>

SoEXTENDER class SoInt32Element : public SoElement {

    SO_ELEMENT_ABSTRACT_HEADER(SoInt32Element);

  public:
    // Comparison based on value of int32_ts
    virtual SbBool	matches(const SoElement *elt) const;

  SoINTERNAL public:
    // Initializes the SoInt32Element class
    static void		initClass();

    // Copy method, takes care copying the data.
    virtual SoElement 	*copyMatchInfo() const;
    
    // Set value.  Derived classes have static inline methods
    // that pass in the stack index and cast their type into a int32_t.
    // This just gets an appropriate instance and calls the virtual
    // set.
    static void		set(int stackIndex, SoState *state, int32_t value);

#ifndef IV_STRICT
    static void         set(int stackIndex, 
				SoState *state, SoNode *, int32_t value)
        { set(stackIndex, state, value); }
    static void         set(int stackIndex, 
				SoState *state, SoNode *, 
			  	long value)	// System long
        { set(stackIndex, state, (int32_t) value); }
#endif

    // Virtual set that subclasses can override to implement
    // side-effect behaviors.
    virtual void	setElt(int32_t value);

    // Get value.  Derived classes have static inline methods to pass
    // in the stackIndex and cast the returned value to the right type.
    static int32_t		get(int stackIndex, SoState *state)
    	{return((const SoInt32Element *)
	    getConstElement(state, stackIndex))->data;}

    // Prints element (for debugging)
    virtual void	print(FILE *fp) const;

  protected:
    // Storage for data.
    int32_t data;

    virtual ~SoInt32Element();
};

#endif /* _SO_INT32_ELEMENT */
