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
 |	SoGroup
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <Inventor/misc/SoChildList.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoOutput.h>
#include <Inventor/actions/SoActions.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/errors/SoReadError.h>
#include <Inventor/fields/SoFieldData.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/elements/SoCacheElement.h>

SO_NODE_SOURCE(SoGroup);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Default constructor
//
// Use: public

SoGroup::SoGroup()
//
////////////////////////////////////////////////////////////////////////
{
    children = new SoChildList(this);
    SO_NODE_CONSTRUCTOR(SoGroup);
    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor that takes approximate number of children.
//
// Use: public

SoGroup::SoGroup(int nChildren)
//
////////////////////////////////////////////////////////////////////////
{
    children = new SoChildList(this, nChildren);
    SO_NODE_CONSTRUCTOR(SoGroup);
    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoGroup::~SoGroup()
//
////////////////////////////////////////////////////////////////////////
{
    delete children;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This adds a child as the last one in the group.
//
// Use: public

void
SoGroup::addChild(SoNode *child)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (child == NULL) {
	SoDebugError::post("SoGroup::addChild", "NULL child node");
	return;
    }
#endif /* DEBUG */

    children->append(child);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This inserts a child into a group so that it will have the given
//    index.
//
// Use: public

void
SoGroup::insertChild(SoNode *child, int newChildIndex)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (child == NULL) {
	SoDebugError::post("SoGroup::insertChild", "NULL child node");
	return;
    }

    // Make sure index is reasonable
    if (newChildIndex < 0 || newChildIndex > getNumChildren()) {
	SoDebugError::post("SoGroup::insertChild",
			   "Index %d is out of range %d - %d",
			   newChildIndex, 0, getNumChildren());
	return;
    }
#endif /* DEBUG */

    // See if adding at end
    if (newChildIndex >= getNumChildren())
	children->append(child);

    else
	children->insert(child, newChildIndex);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns child with the given index
//
// Use: public

SoNode *
SoGroup::getChild(int index) const
//
////////////////////////////////////////////////////////////////////////
{
    return(*children)[index];
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns number of children
//
// Use: public

int
SoGroup::getNumChildren() const
//
////////////////////////////////////////////////////////////////////////
{
    return children->getLength();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the first index (starting with 0) of a child that
//    matches the given node pointer, or -1 if no such child is found.
//
// Use: public

int
SoGroup::findChild(const SoNode *child) const
//
////////////////////////////////////////////////////////////////////////
{
    int i, num;

    num = getNumChildren();

    for (i = 0; i < num; i++)
	if (getChild(i) == child) return(i);

    return(-1);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Removes the child with the given index from the group.
//
// Use: public

void
SoGroup::removeChild(int index)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (index < 0 || index >= getNumChildren()) {
	SoDebugError::post("SoGroup::removeChild",
			   "Index %d is out of range %d - %d",
			   index, 0, getNumChildren() - 1);
	return;
    }
#endif /* DEBUG */

    // Play it safe anyway...
    if (index >= 0) {
	children->remove(index);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Removes all children from the group.
//
// Use: public

void
SoGroup::removeAllChildren()
//
////////////////////////////////////////////////////////////////////////
{
    children->truncate(0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Replaces child with given index with new child.
//
// Use: public

void
SoGroup::replaceChild(int index, SoNode *newChild)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (index < 0 || index >= getNumChildren()) {
	SoDebugError::post("SoGroup::replaceChild",
			   "Index %d is out of range %d - %d",
			   index, 0, getNumChildren() - 1);
	return;
    }
#endif /* DEBUG */

    // Play it safe anyway...
    if (index >= 0)
	children->set(index, newChild);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns pointer to children.
//
// Use: internal

SoChildList *
SoGroup::getChildren() const
//
////////////////////////////////////////////////////////////////////////
{
    return children;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads stuff into instance of SoGroup. Returns FALSE on error.
//    This also deals with field data (if any), so this method should
//    be useful for most subclasses of SoGroup as well.
//
// Use: protected

SbBool
SoGroup::readInstance(SoInput *in, unsigned short flags)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool readOK = TRUE;

    // First, turn off notification for this node
    SbBool saveNotify = enableNotify(FALSE);

    // Read field info. We can't just call
    // SoFieldContainer::readInstance() to read the fields here
    // because we need to tell the SoFieldData that it's ok if a name
    // is found that is not a valid field name - it could be the name
    // of a child node.
    SbBool notBuiltIn; // Not used
    readOK = getFieldData()->read(in, this, FALSE, notBuiltIn);
    if (!readOK) return readOK;

    // If binary BUT was written without children (which can happen
    // if it was read as an unknown node and then written out in
    // binary), don't try to read children:
    if (!in->isBinary() || (flags & IS_GROUP)) 
	readOK = readChildren(in);
    
    // Re-enable notification
    enableNotify(saveNotify);

    return readOK;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads children into instance of SoGroup. Returns FALSE on error.
//
// Use: protected

SbBool
SoGroup::readChildren(SoInput *in)
//
////////////////////////////////////////////////////////////////////////
{
    SoBase	*base;
    SbBool	ret = TRUE;

    // If reading binary, read number of children first
    if (in->isBinary()) {

	int	numToRead, i;

	if (!in->read(numToRead))
	    ret = FALSE;

	else {
	    for (i = 0; i < numToRead; i++) {

		if (SoBase::read(in, base, SoNode::getClassTypeId()) &&
		    base != NULL)
		    addChild((SoNode *) base);

		// Running out of children is now an error, since the
		// number of children in the file must be exact
		else {
		    ret = FALSE;
		    break;
		}
	    }
	    // If we are reading a 1.0 file, read the GROUP_END_MARKER
	    if (ret && in->getIVVersion() == 1.0f) {
		const int GROUP_END_MARKER = -1;
		int marker;

		// Read end marker if it is there. If not, some sort of
		// error occurred.
		if (! in->read(marker) || marker != GROUP_END_MARKER)
		    ret = FALSE;
	    }
	}
    }

    // ASCII: Read children until none left. Deal with children
    // causing errors by adding them as is.
    else {
	while (TRUE) {
	    ret = SoBase::read(in, base, SoNode::getClassTypeId()) && ret;

	    // Add child, even if error occurred, unless there is no
	    // child to add.
	    if (base != NULL)
		addChild((SoNode *) base);

	    // Stop when we run out of valid children
	    else
		break;
	}
    }

    return ret;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements typical traversal.
//
// Use: extender

void
SoGroup::doAction(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    int		numIndices;
    const int	*indices;

    if (action->getPathCode(numIndices, indices) == SoAction::IN_PATH)
	children->traverse(action, 0, indices[numIndices - 1]);

    else
	children->traverse(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does the callback action
//
// Use: extender

void
SoGroup::callback(SoCallbackAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoGroup::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does the GL render action
//
// Use: extender

void
SoGroup::GLRender(SoGLRenderAction *action)

////////////////////////////////////////////////////////////////////////
{
    int numIndices;
    const int *indices;
    SoAction::PathCode pc = action->getPathCode(numIndices, indices);

    // Perform fast-path GLRender traversal:
    if (pc != SoAction::IN_PATH) {
	action->pushCurPath();
	for (int i = 0; i < children->getLength(); i++) {

	    action->popPushCurPath(i);
            if (! action->abortNow())
	        ((SoNode*)(children->get(i)))->GLRender(action);
    	    else
             	SoCacheElement::invalidate(action->getState());

            // Stop if action has reached termination condition. (For
            // example, search has found what it was looking for, or event
            // was handled.)
            if (action->hasTerminated())
                break;
        }
	action->popCurPath();
    }

    else {

	// This is the same as SoChildList::traverse(), except that it
	// checks render abort for each child

	int lastChild = indices[numIndices - 1];
	for (int i = 0; i <= lastChild; i++) {

	    SoNode *child = (SoNode *) children->get(i);

	    if (pc == SoAction::OFF_PATH && ! child->affectsState())
		continue;

	    action->pushCurPath(i);
	    if (action->getCurPathCode() != SoAction::OFF_PATH ||
		child->affectsState()) {

		if (! action->abortNow())
		    child->GLRender(action);
		else
		    SoCacheElement::invalidate(action->getState());
	    }

	    action->popCurPath(pc);

	    if (action->hasTerminated())
		break;
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does the get bounding box action.  This takes care of averaging
//    the centers of all children to get a combined center.
//
// Use: extender

void
SoGroup::getBoundingBox(SoGetBoundingBoxAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f	totalCenter(0,0,0);
    int		numCenters = 0;
    int		numIndices;
    const int	*indices;
    int		lastChild;

    if (action->getPathCode(numIndices, indices) == SoAction::IN_PATH)
	lastChild = indices[numIndices - 1];
    else
	lastChild = getNumChildren() - 1;

    for (int i = 0; i <= lastChild; i++) {
	children->traverse(action, i, i);
	if (action->isCenterSet()) {
	    totalCenter += action->getCenter();
	    numCenters++;
	    action->resetCenter();
	}
    }
    // Now, set the center to be the average. Don't re-transform the
    // average, which should already be transformed.
    if (numCenters != 0)
	action->setCenter(totalCenter / numCenters, FALSE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does the handle event thing
//
// Use: extender

void
SoGroup::handleEvent(SoHandleEventAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoGroup::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pick.
//
// Use: extender

void
SoGroup::pick(SoPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoGroup::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does search...
//
// Use: extender

void
SoGroup::search(SoSearchAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // First see if the caller is searching for this node
    SoNode::search(action);

    // Then recurse on children if not already found
    if (! action->isFound())
	SoGroup::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements get matrix action.
//
// Use: extender

void
SoGroup::getMatrix(SoGetMatrixAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    int		numIndices;
    const int	*indices;

    // Only need to compute matrix if group is a node in middle of
    // current path chain or is off path chain (since the only way
    // this could be called if it is off the chain is if the group is
    // under a group that affects the chain).

    switch (action->getPathCode(numIndices, indices)) {

      case SoAction::NO_PATH:
	break;

      case SoAction::IN_PATH:
	children->traverse(action, 0, indices[numIndices - 1]);
	break;

      case SoAction::BELOW_PATH:
	break;

      case SoAction::OFF_PATH:
	children->traverse(action);
	break;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements write action.
//
// Use: extender

void
SoGroup::write(SoWriteAction *action)

//
////////////////////////////////////////////////////////////////////////
{

    SoOutput	*out = action->getOutput();

    // In write-reference counting phase
    if (out->getStage() == SoOutput::COUNT_REFS) {

	// Increment our write reference count
	addWriteReference(out);

	// If this is the first reference (i.e., we don't now have
	// multiple references), also count the appropriate children
	if (! hasMultipleWriteRefs())
	    SoGroup::doAction(action);
    }

    // In writing phase, we have to do some more work
    else if (! writeHeader(out, TRUE, FALSE)) {

	// Write fields
	const SoFieldData	*fieldData = getFieldData();
	fieldData->write(out, this);

	// See which children to traverse
	int			i, lastChild, numIndices;
	const int		*indices;
	if (action->getPathCode(numIndices, indices) == SoAction::IN_PATH)
	    lastChild = indices[numIndices - 1];
	else
	    lastChild = getNumChildren() - 1;

	int numChildren = 0;
	for (i = 0; i <= lastChild; i++)
	    if (getChild(i)->shouldWrite() > 0)
		numChildren++;

	// If writing binary format, write out number of children
	// that are going to be written
	if (out->isBinary())
	    out->write(numChildren);

	// In the writing stage, we can't use standard traversal,
	// because if we are writing out one path of a path list, we
	// might need to write out children that are not relevant to
	// the current path (but are relevant to another path in the
	// list). Therefore, we implement a different traversal that
	// writes out each child that is referenced.
	for (i = 0; i <= lastChild; i++)
	    if (getChild(i)->shouldWrite())
		children->traverse(action, i);

	// Write post-children stuff
	writeFooter(out);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Copies the contents of the given node into this instance.
//
// Use: protected, virtual

void
SoGroup::copyContents(const SoFieldContainer *fromFC, SbBool copyConnections)
//
////////////////////////////////////////////////////////////////////////
{
    // Copy the usual stuff
    SoNode::copyContents(fromFC, copyConnections);

    // Copy the kids
    const SoGroup *fromGroup = (const SoGroup *) fromFC;
    for (int i = 0; i < fromGroup->getNumChildren(); i++) {

	// If this node is being copied, it must be "inside" (see
	// SoNode::copy() for details.) Therefore, all of its children
	// must be inside, as well.
	SoNode *fromKid = fromGroup->getChild(i);
	SoNode *kidCopy = (SoNode *) findCopy(fromKid, copyConnections);

#ifdef DEBUG
	if (kidCopy == NULL)
	    SoDebugError::post("(internal) SoGroup::copyContents",
			       "Child %d has not been copied yet", i);
#endif /* DEBUG */

	addChild(kidCopy);
    }
}
