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
 * Copyright (C) 1990,91,92   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.2 $
 |
 |   Classes:
 |	SoPath, SoLightPath
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson, Alan Norton
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoOutput.h>
#include <Inventor/SoPath.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/errors/SoReadError.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/misc/SoNotification.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoSeparator.h>

SoType SoPath::classTypeId;

////////////////////////////////////////////////////////////////////////
//
// SoPath class
//
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoPath class.
//
// Use: internal

void
SoPath::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    // Create SoType type
    classTypeId = SoType::createType(SoBase::getClassTypeId(), "Path",
				     SoPath::createInstance);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor for SoPath.
//
// Use: public

SoPath::SoPath()
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (! SoDB::isInitialized())
	SoDebugError::post("SoPath::SoPath",
			   "Cannot construct paths before "
			   "calling SoDB::init()");
#endif /* DEBUG */

    doAuditors = TRUE;
    minNumPublic = 0;
    numPublic  = 0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor given approximate number of nodes in chain.
//
// Use: public

SoPath::SoPath(int approxLength) : nodes(approxLength), indices(approxLength)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (! SoDB::isInitialized())
	SoDebugError::post("SoPath::SoPath",
			   "Cannot construct paths before "
			   "calling SoDB::init()");
#endif /* DEBUG */

    doAuditors = TRUE;
    numPublic  = 0;
    minNumPublic  = 0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor given head node to insert in path.
//
// Use: public

SoPath::SoPath(SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (! SoDB::isInitialized())
	SoDebugError::post("SoPath::SoPath",
			   "Cannot construct paths before "
			   "calling SoDB::init()");
#endif /* DEBUG */

    doAuditors = TRUE;
    numPublic  = 0;
    minNumPublic  = 0;
    setHead(node);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoPath::~SoPath()
//
////////////////////////////////////////////////////////////////////////
{
    truncate(0, FALSE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the head (first) node of the path chain. The path is
//    cleared first, if necessary.
//
// Use: public

void
SoPath::setHead(SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
    // Clear out existing nodes
    truncate(0, FALSE);

    // The index value doesn't matter, since it should never be used
    append(node, -1);

    if (doAuditors)
	startNotify();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds a node to the end of the current chain of nodes in a path.
//    The node is given by its child index in the currently last node
//    in the path.
//
// Use: public

void
SoPath::append(int childIndex)
//
////////////////////////////////////////////////////////////////////////
{
    SoNode	*tail;

#ifdef DEBUG
    // Make sure there is already a node in the path
    if (getFullLength() == 0) {
	SoDebugError::post("SoPath::append", "No head node to append to");
	return;
    }
#endif /* DEBUG */

    // Get real tail node of path
    tail = nodes[getFullLength() - 1];

#ifdef DEBUG
    // If there is one, make sure it can have children
    SoChildList	*children = tail->getChildren();
    if (children == NULL) {
	SoDebugError::post("SoPath::append",
			   "Tail node does not allow children to be added");
	return;
    }

    // And make sure index is valid
    if (childIndex < 0 || childIndex >= children->getLength()) {
	SoDebugError::post("SoPath::append",
			   "Invalid child index %d", childIndex);
	return;
    }
#endif

    append((*tail->getChildren())[childIndex], childIndex);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds a node to the end of the current chain of nodes in a path.
//    The node is passed by pointer, so it must be a child of the
//    current last node in the chain. If it is used more than once as
//    a child, the first occurrance is appended. If the path is empty,
//    this is equivalent to setHead(node).
//
// Use: public

void
SoPath::append(SoNode *childNode)
//
////////////////////////////////////////////////////////////////////////
{
    SoNode	*tail;
    int		childIndex;

    // If the path is empty, this is a setHead() operation
    if (getFullLength() == 0) {
	setHead(childNode);
	return;
    }

    // Get real tail node of path
    tail = nodes[getFullLength() - 1];

#ifdef DEBUG
    // Make sure tail node can have children
    if (tail->getChildren() == NULL) {
	SoDebugError::post("SoPath::append",
			   "Tail node does not allow children to be added");
	return;
    }
#endif

    // Find index of node as child of tail node
    childIndex = tail->getChildren()->find(childNode);

#ifdef DEBUG
    // Make sure index is valid
    if (childIndex < 0) {
	SoDebugError::post("SoPath::append",
			   "Node to append is not a child of path tail");
	return;
    }
#endif

    append(childNode, childIndex);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds all nodes in path to end of chain; head node of fromPath
//    must be the same as or a child of current last node.
//
// Use: public

void
SoPath::append(const SoPath *fromPath)
//
////////////////////////////////////////////////////////////////////////
{
    SoNode	*tailNode;
    SoChildList	*children;
    SbBool	headIsTail;
    int		firstIndex, i;

    if (fromPath->getFullLength() == 0)
	return;

#ifdef DEBUG
    // Make sure there is already a node in the path
    if (getFullLength() == 0) {
	SoDebugError::post("SoPath::append", "No head node to append to");
	return;
    }
#endif

    // Get real tail node of current path
    tailNode = nodes[getFullLength() - 1];

    // Get children of tail node
    children = tailNode->getChildren();

#ifdef DEBUG
    // Make sure tail node can have children
    if (children == NULL) {
	SoDebugError::post("SoPath::append",
			   "Tail node does not allow children to be added");
	return;
    }
#endif

    // See if the head node of fromPath is the same as the tail node
    // or if it is a child of it
    headIsTail = (tailNode == fromPath->getHead());
    if (! headIsTail)
	firstIndex = children->find(fromPath->getHead());

#ifdef DEBUG
    // Make sure head node of fromPath is same as tail node or a child
    // of the tail node
    if (! headIsTail && firstIndex < 0) {
	SoDebugError::post("SoPath::append",
			   "Head of path to append is not the same as or a "
			   "child of the path tail");
	return;
    }
#endif

    // Append each node in fromPath to end of path. Skip the first
    // node if it's already in the path.
    if (! headIsTail)
	append(fromPath->getHead(), firstIndex);
    for (i = 1; i < fromPath->getFullLength(); i++)
	append(fromPath->getNode(i), fromPath->getIndex(i));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the public length of the path.
//
// Use: public

int
SoPath::getLength() const
//
////////////////////////////////////////////////////////////////////////
{
    // Cast const away...
    SoPath *This = (SoPath *)this;

    // If we aren't sure how many are public, figure it out:
    if (numPublic == -1) {

	int lastPublicIndex = 0;
	if (minNumPublic > 1)
	    lastPublicIndex = minNumPublic - 1;

	// Last test is for the second to last node.
	// If it passes, then lastPublicIndex will be incremented to be the
	// final node, which we don't need to test.

	for (  ; lastPublicIndex < (getFullLength() - 1) ; lastPublicIndex++) {
	    // Children of this node will be private, so stop.
	    if ( ! nodes[lastPublicIndex]->isOfType(SoGroup::getClassTypeId()))
		break;
	}
	This->numPublic = This->minNumPublic = lastPublicIndex + 1;
    }
    return numPublic;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Removes all nodes in the path starting at the given node index
//    (0 = head of path chain, etc.).   Just calls the "real" one.
//
// Use: public

void
SoPath::truncate(int start)
//
////////////////////////////////////////////////////////////////////////
{
    truncate(start, TRUE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if the passed node is in the path chain.
//
// Use: public

SbBool
SoPath::containsNode(const SoNode *node) const
//
////////////////////////////////////////////////////////////////////////
{
    for (int i = 0; i < getFullLength(); i++)
    	if (nodes[i] == node)
	    return TRUE;

    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if the nodes in the chain in the passed path are
//    contained (in consecutive order) in this path chain.
//
// Use: public

SbBool
SoPath::containsPath(const SoPath *path) const
//
////////////////////////////////////////////////////////////////////////
{
    int i, j;

    // First find the head of the target path in this path
    for (i = 0; i < getFullLength(); i++)
	if (getNode(i) == path->getHead())
	    break;

    // Head node is not there
    if (i == getFullLength())
	return FALSE;

    // If there aren't enough nodes left in this path, then no match
    if (getFullLength() - i < path->getFullLength())
	return FALSE;

    // Otherwise, start comparing indices in the two paths to see if the
    // paths match. Skip the head node, which we already know is a match.
    for (j = 1; j < path->getFullLength(); j++)
	if (path->getIndex(j) != getIndex(i + j))
	    return FALSE;

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    If the paths have different head nodes, this returns -1.
//    Otherwise, it returns the index into the chain of the last node
//    (starting at the head) that is the same for both paths.
//
// Use: public

int
SoPath::findFork(const SoPath *path) const
//
////////////////////////////////////////////////////////////////////////
{
    int shorterLength, i;

    // Return -1 if heads are different nodes
    if (path->getHead() != getHead())
	return -1;

    // Find length of shorter path
    shorterLength = path->getFullLength();
    if (getFullLength() < shorterLength)
	shorterLength = getFullLength();

    // Return the index of the last pair of nodes that match
    for (i = 1; i < shorterLength; i++)
	if (getIndex(i) != path->getIndex(i))
	    return i - 1;

    // All the nodes matched - return the index of the tail
    return shorterLength - 1;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Creates and returns a new path that is a copy of some or all of
//    this path. Copying starts at the given index (default is 0,
//    which is the head node). numNodes = 0 (the default) means copy
//    all nodes from the starting index to the end. Returns NULL on error.
//
// Use: public

SoPath *
SoPath::copy(int startFromNodeIndex, int numNodes) const
//
////////////////////////////////////////////////////////////////////////
{
    SoPath	*newPath;
    int		lastNodeIndex, i;

#ifdef DEBUG
    if (startFromNodeIndex < 0 ||
	startFromNodeIndex >= getFullLength()) {
	SoDebugError::post("SoPath::copy", "Starting index is invalid");
	return NULL;
    }
#endif /* DEBUG */

    if (numNodes == 0)
	lastNodeIndex = getFullLength() - 1;

    else {
	lastNodeIndex = startFromNodeIndex + numNodes - 1;

#ifdef DEBUG
	if (lastNodeIndex >= getFullLength()) {
	    lastNodeIndex  = getFullLength() - 1;
	    SoDebugError::postWarning("SoPath::copy",
				      "Copying only %d nodes (asked for %d)",
				      lastNodeIndex - startFromNodeIndex + 1,
				      numNodes);
	}
#endif /* DEBUG */
    }

    // Allocate path of correct size
    newPath = new SoPath(lastNodeIndex - startFromNodeIndex + 1);

    newPath->setHead(getNode(startFromNodeIndex));
    for (i = startFromNodeIndex + 1; i <= lastNodeIndex; i++)
	newPath->append(indices[i]);

    return newPath;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Comparison operator tests path equality. Two paths are equal if
//    their chains are the same length and all node pointers in them
//    are the same.
//
// Use: public

int
operator ==(const SoPath &p1, const SoPath &p2)
//
////////////////////////////////////////////////////////////////////////
{
    int	i;

    if (p1.getFullLength() != p2.getFullLength())
	return FALSE;

    // Compare path nodes from bottom up since there is more
    // likelihood that they differ at the bottom. That means that
    // unequal paths will exit this loop sooner.

    for (i = p1.getFullLength() - 1; i >= 0; --i)
	if (p1.nodes[i] != p2.nodes[i] || p1.indices[i] != p2.indices[i])
	    return FALSE;

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Gets the path's typeId
//
// Use: public

SoType
SoPath::getTypeId() const
//
////////////////////////////////////////////////////////////////////////
{
    return classTypeId;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the last path given the name 'name'.  Returns NULL if
//    there is no path with the given name.
//
// Use: public

SoPath *
SoPath::getByName(const SbName &name)
//
////////////////////////////////////////////////////////////////////////
{
    return (SoPath *)getNamedBase(name, SoPath::getClassTypeId());
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds all paths named 'name' to the list.  Returns the number of
//    paths found.
//
// Use: public

int
SoPath::getByName(const SbName &name, SoPathList &list)
//
////////////////////////////////////////////////////////////////////////
{
    return getNamedBases(name, list, SoPath::getClassTypeId());
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This is called when a node in the path chain has a child added.
//    The passed index is the index of the new child. This makes sure
//    the integrity of the path is not corrupted.
//
// Use: internal

void
SoPath::insertIndex(SoNode *parent, int newIndex)
//
////////////////////////////////////////////////////////////////////////
{
    int	i;

    // Find index of parent node in path
    for (i = 0; i < getFullLength(); i++)
	if (nodes[i] == parent)
	    break;

#ifdef DEBUG
    if (i == getFullLength()) {
	SoDebugError::post("(internal) SoPath::insertIndex",
			   "Can't find parent node");
	return;
    }
#endif /* DEBUG */

    // Increment index of next node if there is one and if necessary
    if (++i < getFullLength() && indices[i] >= newIndex)
	++indices[i];
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This is called when a node in the path chain has a child removed.
//    The passed index is the index of the child to be removed. This
//    makes sure the integrity of the path is not corrupted.
//
// Use: internal

void
SoPath::removeIndex(SoNode *parent, int oldIndex)
//
////////////////////////////////////////////////////////////////////////
{
    int	i;

    // Find index of parent node in path
    for (i = 0; i < getFullLength(); i++)
	if (nodes[i] == parent)
	    break;

#ifdef DEBUG
    if (i == getFullLength()) {
	SoDebugError::post("(internal) SoPath::removeIndex",
			   "Can't find parent node");
	return;
    }
#endif /* DEBUG */

    // If there is a next node
    if (++i < getFullLength()) {

	// If the next node is the one being removed, truncate the path
	if (indices[i] == oldIndex)
	    truncate(i, FALSE);

	// Decrement index of next node if it is after the removed one
	else if (indices[i] > oldIndex)
	    --indices[i];
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This is called when a node in the path chain has a child replaced.
//    The passed index is the index of the child to be replaced. This
//    makes sure the integrity of the path is not corrupted.
//
// Use: internal

void
SoPath::replaceIndex(SoNode *parent, int index, SoNode *newChild)
//
////////////////////////////////////////////////////////////////////////
{
    int	i;

    // Find index of parent node in path
    for (i = 0; i < getFullLength(); i++)
	if (nodes[i] == parent)
	    break;

#ifdef DEBUG
    if (i == getFullLength()) {
	SoDebugError::post("(internal) SoPath::replaceIndex",
			   "Can't find parent node");
	return;
    }
#endif /* DEBUG */

    // If there is a next node and it is the one being replaced, change it
    if (++i < getFullLength() && indices[i] == index) {

	// The children of the new node are most likely different from
	// those in the old one. So if the path continued below the
	// old node, truncate it so the path won't believe that the
	// old children are under the new node.
	truncate(i, FALSE);

	// Add the new node and same old index
	append(newChild, index);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes path using the given write action.
//
// Use: internal

void
SoPath::write(SoWriteAction *writeAction) const
//
////////////////////////////////////////////////////////////////////////
{
    int		i, j, index;
    SoOutput	*out = writeAction->getOutput();
    SoChildList	*children;

    if (writeHeader(out, FALSE, FALSE))
	return;

    if (getHead() != NULL) {

	// Write out the head node
	writeAction->traverse(getHead());

	// Write number of indices (after head) first
	if (! out->isBinary())
	    out->indent();
	out->write(getFullLength() - 1);
	if (! out->isBinary())
	    out->write('\n');

	for (i = 1; i < getFullLength(); i++) {

	    // Adjust index if necessary by checking if nodes will be
	    // written or not
	    index = indices[i];
	    children = nodes[i-1]->getChildren();
	    for (j = 0; j < indices[i]; j++)
		if (! (*children)[j]->shouldWrite())
		    --index;

	    if (! out->isBinary())
		out->indent();
	    out->write(index);
	    if (! out->isBinary())
		out->write('\n');
	}
    }

    writeFooter(out);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if the given notification list involves a change to
//    a node that affects the path. It is assumed that the last (most
//    recent) node in the list is the head node of the path.
//
// Use: internal

SbBool
SoPath::isRelevantNotification(SoNotList *list) const
//
////////////////////////////////////////////////////////////////////////
{
    // Trace down the notification list to find the first node (if
    // any) that is not on the path. Stop when we reach the first
    // (in time) record that was at a node in the graph.

    const SoNotRec	*rec = list->getLastRec();
    const SoNotRec	*prevRec = NULL;
    int			curIndex = 0;
    SbBool		offPath  = FALSE;

    while (rec != NULL && curIndex < getLength()) {

	// Stop if the node in the current record is not the next node
	// in the path
	if (rec->getBase() != getNode(curIndex)) {
	    offPath = TRUE;
	    break;
	}

	// Go to the next record and path node, IF we're following a
	// PARENT notification
	if (rec->getPrevious() != NULL &&
	    rec->getPrevious()->getType() != SoNotRec::PARENT) {
	    break;
	}
	prevRec = rec;
	rec = rec->getPrevious();

	curIndex++;
    }

    // If not all notified nodes are on the path, we have to do some
    // extra testing
    if (offPath) {

	const SoNode	*node;
	int		index;

	// The "rec" record points to a node that is off the path.
	// Find out the index of this node in the parent node, which
	// is pointed to by the previous record (which is guaranteed
	// to exist and be on the path).
	node = (const SoNode *) rec->getBase();
	index= ((const SoNode *)prevRec->getBase())->getChildren()->find(node);

	// If the node is to the right of the path, the change does
	// not affect the path
	if (index > getIndex(curIndex))
	    return FALSE;

	// If it is to the left, it doesn't affect the path if any of
	// the notification records go through a separator-type object:
	else {
	    while (TRUE) {
		if (! node->affectsState())
		    return FALSE;

		rec = rec->getPrevious();
		if (rec == NULL || rec->getType() != SoNotRec::PARENT)
		    break;

		node = (const SoNode *) rec->getBase();
	    }
	}
    }

    // If we made it this far, it's a relevant notification
    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Appends the given node and index to the lists.
//
// Use: private

void
SoPath::append(SoNode *node, int index)
//
////////////////////////////////////////////////////////////////////////
{
    // Optimization here: we just set numPublic to -1 to indicate that
    // we aren't sure how many are public, since keeping it up to date
    // can be fairly expensive if the path is changing a lot.
    numPublic = -1;

    // Append to lists
    nodes.append(node);
    indices.append(index);

    // Tell the childlist of the node that the path cares about it
    if (doAuditors) {
	SoChildList *childList = node->getChildren();
	if (childList != NULL) {
	    childList->addPathAuditor(this);
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This is called by SoType::createInstance to create a new
//    instance of a path, given its name.
//
// Use: private

void *
SoPath::createInstance()
//
////////////////////////////////////////////////////////////////////////
{
    return (void *)(new SoPath);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads stuff into empty instance of SoPath. Returns FALSE on
//    error.
//
// Use: private

SbBool
SoPath::readInstance(SoInput *in, unsigned short /* flags not used */)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool	ok;
    SoBase	*rootBase;

    // Read head node
    ok = SoBase::read(in, rootBase, SoNode::getClassTypeId());

    if (ok && rootBase != NULL) {
	int	numIndices, index, i;

	setHead((SoNode *) rootBase);

	// Read indices of rest of nodes in path
	if (! in->read(numIndices)) {
	    SoReadError::post(in, "Couldn't read number of indices in path");
	    ok = FALSE;
	}

	else
	    for (i = 0; i < numIndices; i++) {

		if (! in->read(index)) {
		    SoReadError::post(in, "Couldn't read indices of path");
		    ok = FALSE;
		    break;
		}

		append(index);
	    }
    }

    return ok;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Removes all nodes in the path starting at the given node index
//    (0 = head of path chain, etc.). Flag indicates whether to notify.
//
// Use: private

void
SoPath::truncate(int start, SbBool doNotify)
//
////////////////////////////////////////////////////////////////////////
{
    int	i;

#ifdef DEBUG
    if (start < 0 || (start > 0 && start >= getFullLength())) {
	SoDebugError::post("SoPath::truncate", "Starting index is invalid");
	return;
    }
#endif /* DEBUG */

    // Remove path from all affected nodes' auditors lists
    if (doAuditors)
	for (i = start; i < getFullLength(); i++) {
	    SoChildList *childList = nodes[i]->getChildren();
	    if (childList != NULL) {
		childList->removePathAuditor(this);
	    }
	}

    // Truncate both lists
    nodes.truncate(start);
    indices.truncate(start);

    if (start < minNumPublic) {
	minNumPublic = numPublic = start;
    }

    if (doAuditors && doNotify)
	startNotify();
}

////////////////////////////////////////////////////////////////////////
//
// SoFullPath class
//
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor. Virtual, so has to be defined non-inline.
//
// Use: private

SoFullPath::~SoFullPath()
//
////////////////////////////////////////////////////////////////////////
{
}
//////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////
//
// SoLightPath class
// This is a lightweight version of SoTempPath.  It maintains only a
// list of child indices, for fast pushing and popping during traversal.
//
////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor given approximate number of nodes in chain.
//
// Use: public

SoLightPath::SoLightPath(int approxLength) : indices(approxLength)
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoLightPath::~SoLightPath()
//
////////////////////////////////////////////////////////////////////////
{
    truncate(0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the head (first) node of the path chain. The path is
//    cleared first, if necessary.
//
// Use: public

void
SoLightPath::setHead(SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
    // Clear out existing nodes
    truncate(0);

    // The head index value is set to -1, it should never be referenced. 
    indices.append(-1);
    headNode = node;

}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    fills in values for a full (Temp) path that is a copy of
//    all of this LightPath. The temp path should already be constructed. 
//
// Use: public

void
SoLightPath::makeTempPath(SoTempPath *tmpPath) const
//
////////////////////////////////////////////////////////////////////////
{
    int		i;

    if (tmpPath == NULL){
        SoDebugError::post("SoLightPath::makeTempPath",
			 "Error, NULL SoTempPath");
	return;
	}

    tmpPath->setHead(headNode);
    for (i = 1; i < getFullLength(); i++)
	{
#ifdef DEBUG
	if (indices[i] < 0 ) {
            SoDebugError::post("SoLightPath::makeTempPath",
			 "Error, invalid indices of child");
	    return;
	    }
#endif
	tmpPath->append(indices[i]);
	}
    return;
}

//////////////////////////////////////////////////////////////
//
// Description:
//    Returns SoNode that is ith node in path
//    (where 0th node is head node)
//
//    use: public
//
SoNode *
SoLightPath::getNode(int i) const
//
/////////////////////////////////////////////////////////////
{
    SoNode * curNode = headNode;
    SoChildList *children;
    for(int j=0; j<i; j++){
	//traverse all (even hidden) children
        children = curNode->getChildren();
#ifdef DEBUG
        // Make sure it's valid:
	if(children == NULL || j>= getFullLength()) {
            SoDebugError::post("SoLightPath::getNode",
			 "Error, no child at level");
	}
#endif
	curNode = (*children)[indices[j+1]]; 
    }
    return curNode;
}
