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
 |   $Revision: 1.2 $
 |
 |   Classes:
 |	SoAction
 |	SoActionMethodList
 |	SoEnabledElementsList
 |
 |   Author(s)		: Gavin Bell, Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoDB.h>
#include <Inventor/elements/SoElement.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/misc/SoCompactPathList.h>
#include <Inventor/misc/SoBasic.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/actions/SoSubAction.h>
#include <Inventor/SoPath.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoActionMethodList (internal)
//
//////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: protected

SoActionMethodList::SoActionMethodList(SoActionMethodList *parentList)
//
////////////////////////////////////////////////////////////////////////
{
    parent = parentList;
    numValidTypes = 0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds a method to the traversal list
//
// Use: protected

void
SoActionMethodList::addMethod(SoType nodeType, SoActionMethod method)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    // Make sure nodeType is a kind of node!
    if (! nodeType.isDerivedFrom(SoNode::getClassTypeId()))
	SoDebugError::post("SoAction::addMethod", "%s is not a node type",
			   nodeType.getName().getString());
#endif /* DEBUG */

    numValidTypes = 0;
    (*this)[SoNode::getActionMethodIndex(nodeType)] = method;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets up the traversal list, if necessary.
//
// Use: protected

void
SoActionMethodList::setUp()
//
////////////////////////////////////////////////////////////////////////
{
    if (numValidTypes == SoType::getNumTypes())
	return;		// Already set up the table

    // SoNode's slot must be filled in.  If this action doesn't have a
    // parent action, it is filled in with the null action.  If it
    // does have a parent action, a dummy action is used, and the
    // table is overwritten with the parent's method wherever the
    // dummy action appears in a second pass.
    int i = SoNode::getActionMethodIndex(SoNode::getClassTypeId());
    if ((*this)[i] == NULL) {
	if (parent == NULL)
	    (*this)[i] = SoAction::nullAction;
	else
	    (*this)[i] = dummyAction;
    }

    // Next, find all nodes derived from SoNode (note: it is a good
    // thing we don't have to do this often, since getAllDerivedFrom
    // must look through the entire list of types).
    SoTypeList nodes;
    SoType::getAllDerivedFrom(SoNode::getClassTypeId(), nodes);

    // Now, for any empty slots, fill in the slot from a parent with a
    // non-NULL slot:
    for (i = 0; i < nodes.getLength(); i++) {
	SoType n = nodes[i];
	if ((*this)[SoNode::getActionMethodIndex(n)] == NULL) {
	    (*this)[SoNode::getActionMethodIndex(n)] =
		parentMethod(n);
	}
    }

    // Inherit any undefined methods from parent class
    if (parent != NULL) {
	parent->setUp();

	for (i = 0; i < getLength(); i++) {

	    SoActionMethod	&method = (*this)[i];

	    if (method == dummyAction)
		method = (*parent)[i];
	}
    }

    numValidTypes = SoType::getNumTypes();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Dummy action used to mark entries in the action/method table as
//    empty. 
//
// Use: internal

void
SoActionMethodList::dummyAction(SoAction *, SoNode *)
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Recursively looks for a non-NULL action method from a node's
//    parents.
//
// Use: internal

SoActionMethod
SoActionMethodList::parentMethod(SoType t)
//
////////////////////////////////////////////////////////////////////////
{
    SoActionMethod m;
    SoType parent = t;

    // Look through parents until non-NULL method is found
    do {
	parent = parent.getParent();
	m = (*this)[SoNode::getActionMethodIndex(parent)];
    } while (m == NULL);

    return m;
}

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoEnabledElementsList (internal)
//
//////////////////////////////////////////////////////////////////////////////

int	SoEnabledElementsList::counter = 0;

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Constructor.
//
// Use: internal

SoEnabledElementsList::SoEnabledElementsList(
	SoEnabledElementsList *parentList)
//
////////////////////////////////////////////////////////////////////////
{
    setUpCounter = -1;
    parent = parentList;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Adds an element to the list if it's not already in it.
//
// Use: internal

void
SoEnabledElementsList::enable(SoType elementType, int stackIndex)
//
////////////////////////////////////////////////////////////////////////
{
    SoType prev = elements[stackIndex];

    // If not enabled before or if enabled before but we are now
    // enabling a more-specific subclass, add the element.
    if (prev.isBad() || 
	(elementType != prev && elementType.isDerivedFrom(prev))) {
	elements.set(stackIndex, elementType);

	// Increment global counter to indicate that lists have changed
	counter++;
    }

#ifdef DEBUG
    // If we aren't enabling a more general super-class (and therefore
    // don't need to do anything), error:
    else if (! prev.isDerivedFrom(elementType)) {
	const char *eltName = elementType.getName().getString();
	SoDebugError::post("SoAction::enableElement",
			   "Cannot enable element %s because element %s "
			   "is already enabled",
			   eltName, prev.getName().getString());
    }
#endif
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Enables all elements from the given list that are not already
//   enabled in this one.
//
// Use: internal

const SoTypeList &
SoEnabledElementsList::getElements() const
//
////////////////////////////////////////////////////////////////////////
{
    // Cast const away
    SoEnabledElementsList *This = (SoEnabledElementsList *)this;

    // We only need to merge with our parent's list if some element
    // has been enabled since the last time we merged.
    if (setUpCounter != counter) {

	// We may enable new things here which could increment the 
	// enabled elements counter. But all of these elements were already
	// enabled once by the parent class. So we'll store the counter 
	// now and restore it after this loop...
	This->setUpCounter = counter;

	SoEnabledElementsList *parentList = parent;
	while (parentList) {
	    int		i;
	    SoType	t;

	    for (i = 0; i < parentList->elements.getLength(); i++) {
		t = parentList->elements[i];
		if (! t.isBad())
			This->enable(t, i);
	    }

	    parentList = parentList->parent;
	}

	// restore the counter...
	counter = This->setUpCounter;
    }

    return elements;
}

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoAction
//
//////////////////////////////////////////////////////////////////////////////

SoType			SoAction::classTypeId;
SoEnabledElementsList	*SoAction::enabledElements;
SoActionMethodList	*SoAction::methods;

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: protected

SoAction::SoAction() : curPath(32)
//
////////////////////////////////////////////////////////////////////////
{
    SO_ACTION_CONSTRUCTOR(SoAction);

#ifdef DEBUG
    if (! SoDB::isInitialized())
	SoDebugError::post("SoAction::SoAction",
			   "Cannot construct actions before "
			   "calling SoDB::init()");
#endif /* DEBUG */

    isBeingApplied	= FALSE;
    appliedTo.node	= NULL;
    appliedTo.path	= NULL;
    appliedTo.pathList	= NULL;
    state		= NULL;
    terminated		= FALSE;
    tempPath		= NULL;

    // Make sure enabled elements list is set up the first time this
    // is applied.
    enabledElementsCounter = -1;

}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: protected

SoAction::~SoAction()
//
////////////////////////////////////////////////////////////////////////
{
    if (appliedTo.node != NULL)
	appliedTo.node->unref();

    if (appliedTo.path != NULL)
	appliedTo.path->unref();

    if (state != NULL)
	delete state;
    if (tempPath != NULL)
	tempPath->unref();	
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if action is an instance of a action of the given type
//    or an instance of a subclass of it.
//
// Use: public

SbBool
SoAction::isOfType(SoType type) const
//
////////////////////////////////////////////////////////////////////////
{
    return getTypeId().isDerivedFrom(type);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Construct current path from curPath indices
//
//    use: public
//////////////////////////////////////////////////////////////////////
//
const SoPath *
SoAction::getCurPath() 
{
    if(tempPath == NULL){
	tempPath = new SoTempPath(32);
	tempPath->ref();
        }
    curPath.makeTempPath(tempPath);
    return tempPath;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    find current path tail
//
//    use: public, virtual (SoCallbackAction overrides this)
//////////////////////////////////////////////////////////////////////
//
SoNode *
SoAction::getCurPathTail() 
{
#ifdef DEBUG
    if (curPath.getTail() != ((SoFullPath*)getCurPath())->getTail()){
	SoDebugError::post("SoAction::getCurPathTail\n", 
	"Inconsistent path tail.  Did you change the scene graph\n"
	"During traversal?\n");
    }
#endif /*DEBUG*/
    return(curPath.getTail());
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Applies action to the graph rooted by a node.
//
// Use: public

void
SoAction::apply(SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    // Check for the common user error of applying an action to an
    // unreferenced root. This may save some grief.
    if (node->getRefCount() == 0)
	SoDebugError::postWarning("SoAction::apply",
				  "Action applied to a node with a 0 "
				  "reference count. Did you forget to call "
				  "ref() on the node?");
#endif /* DEBUG */

    // If we are already in the middle of being applied, save the
    // current state of what we are applied to, so we can restore it
    // afterwards. This happens, for example, when the
    // SoGLRenderAction applies itself to traverse transparent paths
    // after normal traversal.
    SbBool		needToRestore = isBeingApplied;
    struct AppliedTo	saveAppliedTo;

    if (isBeingApplied)
	saveAppliedTo = appliedTo;

    isBeingApplied = TRUE;

    appliedTo.code = NODE;
    appliedTo.node = node;
    appliedTo.node->ref();
    appliedTo.curPathCode = NO_PATH;

    curPath.setHead(node);
    terminated = FALSE;

    setUpState();

    beginTraversal(node);

    cleanUp();

    // Restore to previous state if necessary
    if (needToRestore)
	appliedTo = saveAppliedTo;

    isBeingApplied = needToRestore;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Applies action to the graph defined by a path.
//
// Use: public

void
SoAction::apply(SoPath *path)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    // Check for the common user error of applying an action to an
    // unreferenced path. This may save some grief.
    if (path->getRefCount() == 0) {
	SoDebugError::postWarning("SoAction::apply",
				  "Action applied to a path with a 0 "
				  "reference count. Did you forget to call "
				  "ref() on the path?");
    }
#endif /* DEBUG */

    // If we are already in the middle of being applied, save the
    // current state of what we are applied to, so we can restore it
    // afterwards. This happens, for example, when the
    // SoGLRenderAction applies itself to traverse transparent paths
    // after normal traversal.
    SbBool		needToRestore = isBeingApplied;
    struct AppliedTo	saveAppliedTo;

    if (isBeingApplied)
	saveAppliedTo = appliedTo;

    isBeingApplied = TRUE;

    appliedTo.code = PATH;
    appliedTo.path = path;
    appliedTo.path->ref();
    appliedTo.curPathCode = (((const SoFullPath *) path)->getLength() == 1 ?
			     BELOW_PATH : IN_PATH);

    curPath.setHead(path->getHead());
    terminated    = FALSE;

    setUpState();

    beginTraversal(path->getHead());

    cleanUp();

    // Restore to previous state if necessary
    if (needToRestore) {
	appliedTo = saveAppliedTo;

	// Restore the head of the path - we assume this is what was
	// in the current path when we got here. NOTE: This rules out
	// the possibility that the action was in the middle of being
	// applied to some graph; it requires that the recursive
	// apply() was called after the graph was traversed, so the
	// current path had only the head node in it (the cleanUp()
	// for the first apply() was not yet called).
	SoNode *head = (appliedTo.code == NODE ? appliedTo.node :
			appliedTo.code == PATH ? appliedTo.path->getHead() :
			(*appliedTo.pathList)[0]->getHead());
	curPath.setHead(head);
    }

    isBeingApplied = needToRestore;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Applies action to the graphs defined by a path list.
//
// Use: public

void
SoAction::apply(const SoPathList &pathList, SbBool obeysRules)
//
////////////////////////////////////////////////////////////////////////
{
    // Check for empty path list
    if (pathList.getLength() == 0)
	return;

    // If path list obeys the rules, just apply the action to it
    if (obeysRules) {
	apply(pathList, pathList, TRUE);
	return;
    }

    //
    // Otherwise, we may have to break it into smaller lists that do
    // obey the rules
    //

    // First, sort the paths
    SoPathList	sortedPathList(pathList);
    sortedPathList.sort();

    // Remove any duplicate paths and any paths that continue through
    // the tail node of another path
    sortedPathList.uniquify();

    int	numPaths = sortedPathList.getLength();

    // If all the remaining paths have the same head, just apply to
    // the sorted path list
    const SoNode *firstHead = sortedPathList[0]->getHead();
    if (sortedPathList[numPaths-1]->getHead() == firstHead)
	apply(sortedPathList, pathList, TRUE);

    // Otherwise, we have to break the path list into smaller ones
    else
	splitPathList(sortedPathList, pathList);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Null action, for use whenever a node has no method for a given
//    action and you don't want an error message.  Put in SoAction for
//    lack of a better place.
//
// Use: extender

#ifdef DEBUG
void
SoAction::nullAction(SoAction *a, SoNode *n)
#else  /* DEBUG */
void
SoAction::nullAction(SoAction *, SoNode *)
#endif /* DEBUG */
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    const char *nodeName = n->getTypeId().getName().getString();
    const char *actName  = a->getTypeId().getName().getString();
    
    SoDebugError::postWarning("SoAction::nullAction",
			      "Called for node %s, action %s",
			      nodeName, actName);
#endif
}

////////////////////////////////////////////////////////////////////////
//
// Description:  usePathCode is invoked by getPathCode, which
//    Returns path code based on where current node (the node at the
//    end of the current path) lies with respect to the path(s) the
//    action is being applied to. If this returns IN_PATH, indices is
//    set to point to an array of indices corresponding to the
//    children that continue the paths and numIndices is set to the
//    number of such children.
//
// Use: extender

void
SoAction::usePathCode(int &numIndices, const int *&indices)
//
////////////////////////////////////////////////////////////////////////
{

	if (appliedTo.code == PATH) {
	    // Use "index" storage in instance to return next index
	    index = appliedTo.path->getIndex(curPath.getFullLength());
	    numIndices	= 1;
	    indices	= &index;
	}

	// Path list case
	else
	    appliedTo.compactPathList->getChildren(numIndices, indices);

}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pushes a node onto the current path being traversed.
//
// Use: internal

void
SoAction::pushCurPath(int childIndex)
//
////////////////////////////////////////////////////////////////////////
{
    // Push the new node
    curPath.push(childIndex);

    // See if new node is on path being applied to. (We must have been
    // on the path already for this to be true.)
    if (appliedTo.curPathCode == IN_PATH) {

	// If traversing a path list, let it know where we are and
	// find out if we are still on a path being traversed.

	if (appliedTo.code == PATH_LIST) {
	    SbBool	onPath = appliedTo.compactPathList->push(childIndex);

	    if (! onPath)
		appliedTo.curPathCode = OFF_PATH;

	    // If still on a path, see if we are at the end by seeing
	    // if there are any children left on the path
	    else {
		int	numChildren;
		const int *indices;

		appliedTo.compactPathList->getChildren(numChildren, indices);
		if (numChildren == 0)
		    appliedTo.curPathCode = BELOW_PATH;
		else
		    appliedTo.curPathCode = IN_PATH;
	    }
	}

	// Otherwise, we're applying to a path:
	else {

	    // Get new length of current path
	    int	l = curPath.getFullLength();

	    // There are three possible cases:
	    // (1) New node is the last node in the path chain    => BELOW_PATH
	    // (2) It's the next node (not the last) in the chain => IN_PATH
	    // (3) It veered off the path chain			  => OFF_PATH

	    // If the new node is NOT the next node in the path, we must
	    // be off the path
	    const SoNode *nextPathNode = appliedTo.path->getNode(l - 1);
	    if (curPath.getNode(l - 1) != nextPathNode)
		appliedTo.curPathCode = OFF_PATH;

	    // Otherwise, if cur path length is now the same as the path
	    // being applied to, we must at the last node in that path
	    else if (l == ((const SoFullPath *) appliedTo.path)->getLength())
		appliedTo.curPathCode = BELOW_PATH;

	    // Otherwise, we're still IN_PATH
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pops the last node from the current path being traversed.
//    Restores the path code to the given value.
//
// Use: internal

void
SoAction::popCurPath(PathCode prevPathCode)
//
////////////////////////////////////////////////////////////////////////
{
    curPath.pop();

    appliedTo.curPathCode = prevPathCode;

    // If we're traversing a path list, let it know where we are
    if (appliedTo.code == PATH_LIST && appliedTo.curPathCode == IN_PATH)
	appliedTo.compactPathList->pop();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the list of enabled elements for a given action
//    subclass.
//
// Use: protected

const SoEnabledElementsList &
SoAction::getEnabledElements() const
//
////////////////////////////////////////////////////////////////////////
{
    return *enabledElements;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Begins traversal of an action at the given node. The default
//    method here just calls traverse(node). This is virtual to allow
//    subclasses to do extra work before or after traversing the node.
//
// Use: protected

void
SoAction::beginTraversal(SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
    traverse(node);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This is used when applying an action to an SoPathList. It
//    returns TRUE to indicate that the action should create a compact
//    version of the path list before applying itself to it.
//
// Use: protected

SbBool
SoAction::shouldCompactPathLists() const
//
////////////////////////////////////////////////////////////////////////
{
    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Creates state if it is NULL or it is no longer valid because new
//    elements have been enabled since it was created. If any element
//    was enabled since then, the global counter will be different
//    from the one stored in this instance.  This will never happen
//    with standard Inventor nodes, but might happen when a
//    user-defined node is dynamically loaded.
//
// Use: private

void
SoAction::setUpState()
//
////////////////////////////////////////////////////////////////////////
{
    // Setup method traversal table
    traversalMethods->setUp();

    // Create state if necessary.  When an new or different element is
    // enabled, the recreateState flag is set.
    if (state == NULL ||
	enabledElementsCounter != SoEnabledElementsList::getCounter()) {

	if (state != NULL)
	    delete state;

	state = new SoState(this, getEnabledElements().getElements());

	// Store current counter in instance
	enabledElementsCounter = SoEnabledElementsList::getCounter();
    }	
}

////////////////////////////////////////////////////////////////////////
//
// Invalidate the state so that it will be created again
// next time the action is applied.
//
// Use: public, virtual

void
SoAction::invalidateState()
//
////////////////////////////////////////////////////////////////////////
{
    if (state != NULL) {
	delete state;
	state = NULL;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Cleans up after an action has been applied.
//
// Use: private

void
SoAction::cleanUp()
//
////////////////////////////////////////////////////////////////////////
{
    switch (appliedTo.code) {

      case NODE:
	if (appliedTo.node != NULL) {
	    appliedTo.node->unref();
	    appliedTo.node = NULL;
	}
	break;

      case PATH:
	if (appliedTo.path != NULL) {
	    appliedTo.path->unref();
	    appliedTo.path = NULL;
	}
	break;

      case PATH_LIST:
	appliedTo.pathList = NULL;
	break;
    }

    curPath.truncate(0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Given a sorted path list that contains paths from different head
//    nodes, this splits the list into one list per head node, then
//    applies the action to each of those lists, in turn.
//
// Use: private

void
SoAction::splitPathList(const SoPathList &sortedList,
			const SoPathList &origPathList)
//
////////////////////////////////////////////////////////////////////////
{
    int		numPaths, curStart, i;
    SoNode	*curHead;

    numPaths = sortedList.getLength();

    // Create a list to hold one of the split lists
    SoPathList	splitList(numPaths);

    // Split list while there are still paths to examine
    curStart = 0;
    while (curStart < numPaths) {

	// Gather all paths with same head
	curHead = sortedList[curStart]->getHead();
	splitList.append(sortedList[curStart]);

	for (i = curStart + 1; i < numPaths; i++) {
	    if (sortedList[i]->getHead() != curHead)
		break;
	    splitList.append(sortedList[i]);
	}

	// Apply action to split list. Indicate that it's the last
	// path list if there are no more paths after these.
	apply(splitList, origPathList, i >= numPaths);

	// Prepare for next set of paths
	splitList.truncate(0);
	curStart = i;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Applies action to the graph defined by a path list, which is
//    guaranteed to obey the rules of sorting, uniqueness, etc. The
//    original path list is passed in so the subclass can use it if
//    necessary. The isLastPathList flag indicates that this is the
//    last path list created from the original, in case a derived
//    class wants this info.
//
// Use: private

void
SoAction::apply(const SoPathList &sortedList,
		const SoPathList &origPathList, SbBool isLastPathList)
//
////////////////////////////////////////////////////////////////////////
{
    // If we are already in the middle of being applied, save the
    // current state of what we are applied to, so we can restore it
    // afterwards. This happens, for example, when the
    // SoGLRenderAction applies itself to traverse transparent paths
    // after normal traversal.
    SbBool		needToRestore = isBeingApplied;
    struct AppliedTo	saveAppliedTo;

    if (isBeingApplied)
	saveAppliedTo = appliedTo;

    isBeingApplied = TRUE;

    appliedTo.code = PATH_LIST;
    appliedTo.pathList       = &sortedList;
    appliedTo.origPathList   = &origPathList;
    appliedTo.isLastPathList = isLastPathList;
    appliedTo.curPathCode    =
	(((const SoFullPath *) sortedList[0])->getLength() == 1 ?
	 BELOW_PATH : IN_PATH);

    curPath.setHead(sortedList[0]->getHead());
    terminated = FALSE;

    setUpState();

    // If requested, create compact path lists for easier traversal
    // and apply to them
    if (shouldCompactPathLists())
	appliedTo.compactPathList = new SoCompactPathList(sortedList);
    else
	appliedTo.compactPathList = NULL;

    beginTraversal(sortedList[0]->getHead());

    cleanUp();

    if (appliedTo.compactPathList != NULL)
	delete appliedTo.compactPathList;

    // Restore to previous state if necessary
    if (needToRestore) {
	appliedTo = saveAppliedTo;

	// Restore the head of the path - we assume this is what was
	// in the current path when we got here. NOTE: This rules out
	// the possibility that the action was in the middle of being
	// applied to some graph; it requires that the recursive
	// apply() was called after the graph was traversed, so the
	// current path had only the head node in it (the cleanUp()
	// for the first apply() was not yet called).
	SoNode *head = (appliedTo.code == NODE ? appliedTo.node :
			appliedTo.code == PATH ? appliedTo.path->getHead() :
			(*appliedTo.pathList)[0]->getHead());
	curPath.setHead(head);
    }

    isBeingApplied = needToRestore;
}

