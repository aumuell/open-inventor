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
 |	This file contains the definition of the SoCompactPathList class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

// C-api: end

#ifndef  _SO_COMPACT_PATH_LIST_
#define  _SO_COMPACT_PATH_LIST_

#include <Inventor/SoLists.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoCompactPathList (internal)
//
//  This class represents a list of paths (like an SoPathList) in a
//  more compact form. Since many paths in a path list may share
//  common sub-paths from common head nodes, this class stores them
//  with less redundancy.
//
//  The SoCompactPathList constructor is passed an SoPathList. It is
//  assumed that the SoPathList is not empty and that all paths in the
//  SoPathList have a common head node; an error will occur otherwise.
//  It is also assumed that no two paths are identical, and that if
//  one path goes through node X, then no other path will end at node
//  X.
//
//  While an SoPathList provides random access to any node in any
//  path, an SoCompactPathList provides only sequential access. That
//  is, you can only access the paths in the list in order. Each
//  instance maintains the notion of the "current node" - the node
//  that traversal has last reached - to support this access.
//
//////////////////////////////////////////////////////////////////////////////

SoINTERNAL class SoCompactPathList {

  public:
    // Constructor given path list
    SoCompactPathList(const SoPathList &list);
    ~SoCompactPathList();

    // Resets traversal to the beginning. This allows an instance to
    // be traversed more than once.
    void		reset();

    // Returns the indices of the current node that are in paths in
    // the list. The number of indices is returned in "numIndices",
    // and the indices are returned in "indices". numIndices will be 0
    // if the current node has no children in any path.
    void		getChildren(int &numIndices, const int *&indices);

    // Traverses the child with given index of the current node. The
    // child becomes the new current node. If the child is on a path
    // in the list, then getChildren() can be called to get the next
    // set of children. Otherwise, it will always return no children.
    // This method returns TRUE if the given childIndex is in one of
    // the paths in the list, and FALSE otherwise.
    SbBool		push(int childIndex);

    // Restores current node to what it was before the most recent push()
    void		pop();

  private:
    int			*array;		// Path list as a series of integers
    int			curNode;	// Index of current node in array
    int			*stack;		// Current traversal stack of indices
    int			stackDepth;	// Depth of stack

    // Returns number of path child indices of current node
    int			getNumIndices()	{ return array[curNode]; }

    // Returns index in array of first child index of current node
    int			getStartIndex()	{ return curNode + 1; }

    // Returns index in array of i'th on-path child of current node
    int			getChild(int i)
	{ return array[getStartIndex() + getNumIndices() + i]; }

    // Pushes curNode on stack
    void		pushCurNode()	{ stack[stackDepth++] = curNode; }

    // Pops stack, setting curNode to resulting top node
    void		popCurNode()	{ curNode = stack[--stackDepth - 1]; }

    // Computes number of array indices needed to store stuff
    int			computeArraySize(const SoPathList &list);

    // Stores the paths in a compact form, recursively
    int			compactPaths(int curSlot, int depth,
				     const SoPathList &list,
				     int firstPath, int numPaths);
};

#endif /* _SO_COMPACT_PATH_LIST_ */
