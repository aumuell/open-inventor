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
 |	This file defines the SoState class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_STATE_
#define  _SO_STATE_

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoState
//
//  An SoState collects and holds state while traversing a scene
//  graph. A state is composed of a variety of elements, each of which
//  holds some specific information, such as coordinates or diffuse
//  color of the surface material.
//
//  Each element is stored in its own stack so that save and restore
//  can be implemented as push and pop. These stack operations are
//  performed lazily, so that pushing of a value occurs only when the
//  value would be overwritten, for efficiency.
//
//////////////////////////////////////////////////////////////////////////////

#define SO_STATE_GET_CONST_ELEMENT(stackIndex)\
	return stack[stackIndex];

#include <Inventor/SoLists.h>

class SoAction;
class SoElement;

SoEXTENDER class SoState {

  public:
    // Constructor. Takes pointer to action instance this state is
    // part of and a list of type-ids of elements that are enabled.
    SoState(SoAction *action, const SoTypeList &enabledElements);

    // Destructor
    ~SoState();

    // Returns the action instance the state is part of
    SoAction *		getAction() const	{ return action; }

    // Returns a writable instance of the element on the top of the
    // stack with the given index.
    SoElement *		getElement(int stackIndex);

    // Returns the top (read-only) instance of the given element stack
    const SoElement *	getConstElement(int stackIndex) const
	{ return stack[stackIndex]; }

    // Pushes (saves) the current state until a pop() restores it. The
    // push is done lazily: this just increments the depth in the
    // state. When an element is accessed with getElement() and its
    // depth is less than the current depth, it is then pushed
    // individually.
    void		push() 
			{ depth++; }

    // Pops the state, restoring the state to just before the last push().
    void		pop();

    // Prints state to file (for debugging)
    void		print(FILE *fp);

  SoINTERNAL public:
    // Returns TRUE if element with given stack index is enabled in state
    SbBool		isElementEnabled(int stackIndex) const
	{ return (stack[stackIndex] != NULL); }

    // Returns current depth of state
    int			getDepth() const { return depth; }

    // Sets/returns flag that indicates whether a cache is open. This
    // flag lets us optimize element capturing; we don't need to try
    // to capture elements if the flag is FALSE.
    void		setCacheOpen(SbBool flag)	{ cacheOpen = flag; }
    SbBool		isCacheOpen() const		{ return cacheOpen; }

    // Internal-only, dangerous method that returns a writeable
    // element without checking for state depth and doing a push.
    // Be very careful and consider the caching implications before
    // using this method!
    SoElement *		getElementNoPush(int stackIndex) const
	{ return stack[stackIndex]; }

  private:
    SoAction		*action;	// Action instance state is in
    int			depth;		// Current depth
    SoElement		**stack;	// Array of element stacks
    int			numStacks;	// Number of stacks in array
    SoElement *		topElement;	// First element in threaded stack

    SbBool		cacheOpen;	// TRUE if a cache is open
};

#endif	/* _SO_STATE_ */

