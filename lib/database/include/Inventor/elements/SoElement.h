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
 |	This file defines the abstract base SoEXTENDER SoElement class.
 |
 |   Classes:
 |	SoElement
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_ELEMENT
#define  _SO_ELEMENT

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoElement
//
//  This is the abstract base class for all state elements. This class
//  defines the following features for all of its derived classes:
//
//      Type identifiers. Each class of element has a unique (static)
//      SoType identifier. The correct type id is also stored in each
//      instance for easy access.
//
//	Copying.  Elements are copied into the list of elements used
//      in an SoCache.  Performing any operation other than matches()
//      on a copied element is not guaranteed to work (and will not
//      work for things like Coordinates, if the coordinate they were
//      pointing to has been deleted).
//
//      Stack indices. Since a subclass of an element class needs to
//      be in the same state stack as the class from which it is
//      derived, stack indices are inherited.
//
//      Capturing. Each time an element's value is accessed, that
//      element is added to the elements-used list of all currently
//      open caches.
//
//////////////////////////////////////////////////////////////////////////////

#include <Inventor/SoType.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/nodes/SoNode.h>

class SoTypeList;

SoEXTENDER class SoElement {

  public:
    // Initializes element. Called for first element of its kind in
    // stack. Default method does nothing.
    virtual void	init(SoState *state);

    // Pushes/pops element. Allows for side effects to occur. Default
    // methods do nothing.
    virtual void	push(SoState *state);
    virtual void	pop(SoState *state, const SoElement *prevTopElement);

    // Prints element (for debugging)
    virtual void	print(FILE *fp) const;

    // Returns TRUE if the element matches another element (of the
    // same class, presumably) with respect to cache validity.  If you
    // write a matches() method, you must also write a copy() method.
    virtual SbBool	matches(const SoElement *elt) const = 0;

    // Create a copy that we can put in a cache used list and call
    // matches() on later.
    virtual SoElement	*copyMatchInfo() const = 0;

    // Returns type identifier for SoElement class
    static SoType	getClassTypeId()	{ return classTypeId; }

    // Returns type identifier for element instance
    SoType		getTypeId() const	{ return typeId; }

  SoINTERNAL public:
    // Initialize ALL Inventor element classes
    static void		initElements();

    // Initializes the SoElement class
    static void		initClass();

    // Returns the number of stack indices allocated
    static int		getNumStackIndices()	{ return nextStackIndex; }

    // Returns the id for the element with the given stack index
    static SoType	getIdFromStackIndex(int stackIndex);

    // Returns the stack index for an element instance 
    int			getStackIndex() const	{ return stackIndex; }

    // Sets stuff in an element instance
    void		setDepth(int dpth)		{ depth = dpth; }
    void		setNext(SoElement *nxt)		{ next = nxt; }
    void		setNextInStack(SoElement *nxt)	{ nextInStack = nxt; }
    void		setNextFree(SoElement *nxt)	{ nextFree = nxt; }

    // Returns stuff from element instance
    int			getDepth() const	{ return depth; }
    SoElement *		getNext()  const	{ return next; }

    // Destructor
    virtual ~SoElement();

  protected:
    // Constructor; use typeId.createInstance to create elements.
    SoElement();

    // Returns an instance of an element from the stack with the given
    // index in the given state. This instance is writeable. This
    // returns NULL if no writable instance can be returned.
    static SoElement *	getElement(SoState *state, int stackIndex)
	{ return state->getElement(stackIndex); }

    // Returns a read-only pointer to the top instance in the given
    // element stack
    static inline const SoElement * getConstElement(SoState *state,
						    int stackIndex);

    // Does whatever is necessary in state to capture this element for
    // caching purposes. Should be called by subclasses whenever
    // any value in the element is accessed.
    void		capture(SoState *state) const
	{ if (state->isCacheOpen()) captureThis(state); }

    // Really captures this element, once it has been determined that
    // a cache is open to capture it
    virtual void	captureThis(SoState *state) const;

    // Sets typeId in instance
    void		setTypeId(SoType id)		{ typeId = id; }

    // Sets stackIndex in instance
    void		setStackIndex(int index)	{ stackIndex = index; }

    // Returns next instance in specific element stack
    SoElement *		getNextInStack() const		{ return nextInStack; }

    // Returns next free element in a specific element stack
    SoElement *		getNextFree() const		{ return nextFree; }

  SoINTERNAL protected:
    static int		classStackIndex;  // Stack index for SoElement class

    // Creates and returns a new stack index
    static int		createStackIndex(SoType id);

  private:
    // Type identifier stuff
    static SoType	classTypeId;	// Type identifier for SoElement class
    SoType		typeId;		// Type identifier for instance

    // Stack index stuff
    static int		nextStackIndex;	// Next stack index to allocate
    static SoTypeList	*stackToType;	// Correlates stack index to type id
    int			stackIndex;	// Stack index for instance

    // Other per-instance stuff

    // nextInStack and nextFree are the pointers in a doubly-linked
    // list for each stack in the state.  nextInStack points toward
    // the bottom of the stack and nextFree points toward the top of
    // the stack.  Once a stack has been fully allocated, pushing
    // becomes simply following the nextFree pointer and popping
    // becomes following the nextInStack pointer.

    SoElement 		*nextInStack;	// Next element of same type on stack
    SoElement		*nextFree;	// Next free element

    SoElement 		*next;		// Next element in all-element
					// stack
    int			depth;		// Stack depth of element
friend class SoState;
};

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Returns a read-only pointer to the top instance in the given
//	element stack.  inline to speed up traversal.
//
// Use: protected

inline const SoElement *SoElement::getConstElement(SoState *state,
						   int stackIndex)
//
////////////////////////////////////////////////////////////////////////
{
    const SoElement * elt = (state->getConstElement(stackIndex));
    elt->capture(state);
    return elt;
}

#endif /* _SO_ELEMENT */
