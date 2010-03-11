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
 |	SoCompactPathList
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/misc/SoCompactPathList.h>
#include <Inventor/SoPath.h>
#include <Inventor/errors/SoDebugError.h>

////////////////////////////////////////////////////////////////////////
//
//  Some notes about the compact path storage:
//
//  The list of paths from a common head are stored in "array", an
//  array of integers. The children of the head node are stored first.
//  Any set of children is stored as:
//
//	+ The number of children (1 integer)
//
//	+ The child indices of the children (1 integer per child)
//
//	+ The indices in "array" of the location of the first integer
//	  (the number of children) of child information for each child
//	  (1 integer per child)
//
//  The order of storage is depth first.
//
////////////////////////////////////////////////////////////////////////

// Returns path i from SoPathList list as an SoFullPath
#define GET_PATH(list, i)	((SoFullPath *) list[i])

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoCompactPathList::SoCompactPathList(const SoPathList &list)
//
////////////////////////////////////////////////////////////////////////
{
    // Determine number of indices we will need and check for path
    // list validity
    int		arraySize = computeArraySize(list);

    // Allocate stuff
    array = new int [arraySize];
    stack = new int [128];		// Should be deep enough

    // Store paths in compact form in array
    compactPaths(0, 1, list, 0, list.getLength());

    // Initialize variables
    reset();

#if DEBUGGING
    int i;
    printf("SoCompactPathList::array (%d):\n\t", arraySize);
    for (i = 0; i < arraySize; i++) printf(" %2d", i);
    printf("\n\t");
    for (i = 0; i < arraySize; i++) printf(" %2d", array[i]);
    printf("\n");
#endif
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: public

SoCompactPathList::~SoCompactPathList()
//
////////////////////////////////////////////////////////////////////////
{
    delete [] array;
    delete [] stack;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Resets traversal to the beginning. This allows an instance to be
//    traversed more than once.
//
// Use: public

void
SoCompactPathList::reset()
//
////////////////////////////////////////////////////////////////////////
{
    stackDepth = 0;			// Empty stack
    curNode    = 0;			// Start at head node

    // Put a valid node on top of the stack so the last pop() will
    // reset to something reasonable
    pushCurNode();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the indices of the current node that are in paths in the
//    list. The number of indices is returned in "numIndices", and the
//    indices are returned in "indices". numIndices will be 0 if the
//    current node has no children in any path.
//
// Use: public

void
SoCompactPathList::getChildren(int &numIndices, const int *&indices)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (curNode < 0) {
	SoDebugError::post("SoCompactPathList::getChildren",
			   "currently not on a path in list");
	numIndices = 0;
	return;
    }
#endif /* DEBUG */

    numIndices = getNumIndices();
    indices    = &array[getStartIndex()];
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Traverses the child with given index of the current node. The
//    child becomes the new current node. If the child is on a path in
//    the list, then getChildren() can be called to get the next set
//    of children. Otherwise, it will always return no children. This
//    method returns TRUE if the given childIndex is in one of the
//    paths in the list, and FALSE otherwise.
//
// Use: public

SbBool
SoCompactPathList::push(int childIndex)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (curNode < 0) {
	SoDebugError::post("SoCompactPathList::push",
			   "currently not on a path in list");
	return FALSE;
    }
#endif /* DEBUG */

    int	i;

    // See if the node to push is in a path by looking through the
    // children of the current node
    for (i = 0; i < getNumIndices(); i++)
	if (array[getStartIndex() + i] == childIndex)
	    break;

    // If child is in a path
    if (i < getNumIndices())
	curNode = getChild(i);

    // Child is not in a path
    else
	curNode = -1;

    pushCurNode();

    // Return TRUE if child is in a path
    return (curNode != -1);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Restores current node to what it was before the most recent
//    push().
//
// Use: public

void
SoCompactPathList::pop()
//
////////////////////////////////////////////////////////////////////////
{
    popCurNode();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Computes number of array indices needed to store stuff.
//
// Use: private

int
SoCompactPathList::computeArraySize(const SoPathList &list)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (list.getLength() == 0) {
	SoDebugError::post("SoCompactPathList::SoCompactPathList",
			   "Empty path list");
	return 1;
    }
#endif /* DEBUG */

    SoNode	*head = GET_PATH(list, 0)->getHead();
    int		p, totalLength = 0;

    for (p = 0; p < list.getLength(); p++) {

	// Make sure all paths have same head node
	if (GET_PATH(list, p)->getHead() != head) {
	    SoDebugError::postWarning("SoCompactPathList::SoCompactPathList",
				      "Not all paths have same head node");
	    continue;
	}

	// Add up lengths of all paths without the head node. This is
	// probably a lot more than the number of distinct nodes in
	// the path, but what's a little memory?
	totalLength += GET_PATH(list, p)->getLength() - 1;
    }

    // Each node will have 3 entries in the array: the child index,
    // the number of children, and the index of the first child.
    // There's also 1 extra word for the number of children of the
    // head node.
    return 3 * totalLength + 1;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Stores the paths in a compact form. This is a recursive method
//    that eventually compacts all paths. "curSlot" is the slot in the
//    array in which to store the first index computed. "depth" is the
//    depth in the paths in which we are looking at indices (> 0,
//    since depth 0 is the head of the path). "firstPath" and
//    "numPaths" give the range of paths to examine. This method
//    returns the next open slot after processing.
//
// Use: private

int
SoCompactPathList::compactPaths(int curSlot, int depth,
				const SoPathList &list,
				int firstPath, int numPaths)
//
////////////////////////////////////////////////////////////////////////
{
    int		nextSlot;
    int		curIndex, prevIndex, nextIndex;
    int		firstPathWithChild, numPathsWithChild, curPath, lastPath;
    int		numChildren, curChild;

    // If we are below the depth of the paths, just store a 0 (for no
    // children) and return
    if (depth >= GET_PATH(list, firstPath)->getLength()) {
	array[curSlot] = 0;
	return curSlot + 1;
    }

    // First, count the number of distinct children at the current depth
    prevIndex   = -1;
    numChildren = 0;
    for (curPath = 0; curPath < numPaths; curPath++) {
	curIndex = GET_PATH(list, firstPath + curPath)->getIndex(depth);
	if (curIndex != prevIndex) {
	    numChildren++;
	    prevIndex = curIndex;
	}
    }

    // Store number of children in first slot in array
    array[curSlot] = numChildren;

    // Move to next slot that will be open after we store the children info
    nextSlot = curSlot + 1 + 2 * numChildren;

    //
    // Find each distinct child to recurse. Keep track of which paths
    // contain the child.
    //

    // Start with first child (from first path)
    curChild  = 0;
    curIndex = GET_PATH(list, firstPath)->getIndex(depth);
    firstPathWithChild = firstPath;
    lastPath = firstPath + numPaths - 1;

    // Keep going while there are still paths left to check
    while (firstPathWithChild <= lastPath) {

	// Count the number of paths with the current child (i.e.,
	// have the same child index as curIndex). There has to be at
	// least one or we wouldn't be here
	numPathsWithChild = 1;
	for (curPath = firstPathWithChild + numPathsWithChild;
	     curPath <= lastPath; curPath++) {

	    nextIndex = GET_PATH(list, curPath)->getIndex(depth);

	    if (nextIndex == curIndex)
		numPathsWithChild++;
	    else
		break;		// Found a different child
	}

	// Store current child and index of child array in array
	array[curSlot + 1 + curChild]			= curIndex;
	array[curSlot + 1 + curChild + numChildren]	= nextSlot;

	// Recurse
	nextSlot = compactPaths(nextSlot, depth + 1, list,
				firstPathWithChild, numPathsWithChild);

	// Prepare for next child
	curChild++;
	curIndex = nextIndex;
	firstPathWithChild += numPathsWithChild;
    }

    return nextSlot;
}

#undef GET_PATH
