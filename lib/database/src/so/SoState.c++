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
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |	SoState
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/elements/SoElement.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/misc/SoState.h>

////////////////////////////////////////////////////////////////////////
//
// Description:
//    SoState constructor. It is passed a pointer to the action
//    instance the state is in, and a list of type-ids for all
//    elements enabled in this instance.
//
// Use: public

SoState::SoState(SoAction *_action, const SoTypeList &enabledElements)
//
////////////////////////////////////////////////////////////////////////
{
    int		i;
    SoElement	*elt;

    action = _action;
    depth = 0;

    // Find out number of elements
    numStacks = SoElement::getNumStackIndices();

    // Allocate stack pointers
    stack = new SoElement * [numStacks];

    // Initialize all stacks to NULL
    for (i = 0; i < numStacks; i++)
	stack[i] = NULL;

    // Allocate and initialize one instance of each enabled element.
    // While doing this, set up threaded stack of elements
    topElement = NULL;
    for (i = 0; i < enabledElements.getLength(); i++) {
	// Skip bad elements
	if (enabledElements[i].isBad())
	    continue;

	elt = (SoElement *) enabledElements[i].createInstance();
	elt->setDepth(depth);
	stack[elt->getStackIndex()] = elt;
	elt->init(this);
	elt->setNext(topElement);
	elt->setNextInStack(NULL);
	elt->setNextFree(NULL);
	topElement = elt;
    }

    // Push state to avoid clobbering initial element instances
    push();

    // Assume there are no caches open yet
    setCacheOpen(FALSE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    SoState destructor.
//
// Use: public

SoState::~SoState()
//
////////////////////////////////////////////////////////////////////////
{
    // Pop state; matches push done in constructor
    pop();

#ifdef DEBUG
    if (depth != 0) {
	SoDebugError::post("SoState::~SoState",
			   "State destroyed with non-zero (%d) "
			   "depth", depth);
    }
#endif

    // Get rid of all the elements on all the stacks.
    SoElement *elt, *nextElt;
    int i;
    for (i = 0; i < numStacks; i++) {
	elt = stack[i];
	while (elt != NULL) {
	    nextElt = elt->getNextFree();
	    delete elt;
	    elt = nextElt;
	}
    }

    // Get rid of stack pointer array
    delete[] stack;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns a writable instance of the element on the top of the
//    stack with the given index.
//
// Use: public

SoElement *
SoState::getElement(int stackIndex)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    // Nasty bug:  calling this routine while popping the state
    // can case very bad things to happen (topElement->next chain will
    // end up not being sorted by depth).  We'll check and make sure
    // that doesn't happen:
    if (depth < topElement->getDepth()) {
	SoDebugError::post("SoState::getElement", 
	    "Elements must not be changed while the state is being "
	    "popped (element being changed: %s).",
	    SoElement::getIdFromStackIndex(stackIndex).getName().getString());
    }
#endif

    // Get top of element stack with given index
    SoElement	*elt = stack[stackIndex];

#ifdef DEBUG
    if (elt == NULL) {
	SoDebugError::post("SoState::getElement", "%s is not enabled",
	    SoElement::getIdFromStackIndex(stackIndex).getName().getString());
	return NULL;
    }
#endif /* DEBUG */

    // If element is not at current depth, we have to push a new
    // element on the stack
    if (elt->getDepth() < depth) {
	SoElement *newElt;

	// Each element stack is a doubly-linked list.  The
	// nextInStack pointer points to the next lowest element, and
	// the nextFree pointer points to the next hightest element.
	// The top element's nextFree pointer points to a free element.
	//
	// With this scheme we only have to allocate elements for the
	// stack once; pushing and popping during subsequent
	// traversals just move the topElement pointer up and down the
	// list.
	if (elt->getNextFree()) {
	    newElt = elt->getNextFree();
	} else {
	    newElt = (SoElement *)(elt->getTypeId().createInstance());
	    elt->setNextFree(newElt);
	    newElt->setNextInStack(elt);
	    newElt->setNextFree(NULL);
	}

	newElt->setDepth(depth);

	// Add it to the all-element stack
	newElt->setNext(topElement);

	topElement = stack[stackIndex] = newElt;

	// Call push on new element in case it has side effects
	newElt->push(this);

	// Return new element
	elt = newElt;
    }

    return elt;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pops the state, restoring the state to just before the last push().
//
// Use: public

void
SoState::pop()
//
////////////////////////////////////////////////////////////////////////
{
    SoElement *poppedElt, *nextInStack;

    --depth;

    // Do the popping in two passes. The first calls the pop() method
    // for all popped elements. The second pass actually removes the
    // elements from the stacks and frees them up. We do this in two
    // passes because calling pop() may add elements to the current
    // SoCacheElement, so we want to make sure this is done before
    // that element is popped.

    // Call pop() for all elements that were at previous depth
    for (poppedElt = topElement;
	 poppedElt != NULL && poppedElt->getDepth() > depth;
	 poppedElt = poppedElt->getNext()) {

	// Find next element in same stack as popped element. This
	// element will become the new top of that stack.
	nextInStack = poppedElt->getNextInStack();

	// Give new top element in stack a chance to update things.
	// Pass old element instance just in case.
	poppedElt->getNextInStack()->pop(this, poppedElt);

    }

    // Actually pop all such elements
    while (topElement != NULL && topElement->getDepth() > depth) {
	poppedElt = topElement;

	// Remove from main stack
	topElement = topElement->getNext();

	// Remove from element stack
	stack[poppedElt->getStackIndex()] = poppedElt->getNextInStack();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Prints state to file for debugging.
//
// Use: public

void
SoState::print(FILE *fp)
//
////////////////////////////////////////////////////////////////////////
{
    const SoElement	*elt;
    int			i;

    fprintf(fp, "_________________________________________________________\n");
    fprintf(fp, "SoState\n");
    fprintf(fp, "_________________________________________________________\n");

    for (i = 0; i < numStacks; i++) {

	// Print only enabled element stacks
	if (stack[i] != NULL) {

	    fprintf(fp, "  stack[%02d]:\n", i);

	    for (elt = stack[i]; elt != NULL; elt = elt->getNextInStack()) {
		fprintf(fp, "    ");
		elt->print(fp);
	    }
	}
    }

    fprintf(fp, "_________________________________________________________\n");
}
