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
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |	SoNode
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoDB.h>
#include <Inventor/SoOutput.h>
#include <Inventor/actions/SoActions.h>
#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/fields/SoFieldData.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/misc/SoNotification.h>
#include <Inventor/nodes/SoSubNode.h>		/* Has some handy stuff */

SoType		SoNode::classTypeId;		// Type identifier
uint32_t	SoNode::nextUniqueId;		// Unique ID counter
int		SoNode::nextActionMethodIndex;	// Index for action tables

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: protected

SoNode::SoNode()
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (! SoDB::isInitialized()) {
	SoDebugError::post("SoNode::SoNode",
			   "Cannot construct nodes before "
			   "calling SoDB::init()");
    }
#endif /* DEBUG */

    override = FALSE;
    uniqueId = nextUniqueId++;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: protected

SoNode::~SoNode()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Turns override flag on or off. Causes notification.
//
// Use: public

void
SoNode::setOverride(SbBool state)
//
////////////////////////////////////////////////////////////////////////
{
    override = state;
    startNotify();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Creates and returns an exact copy of the node.
//
// Use: public

SoNode *
SoNode::copy(SbBool copyConnections) const
//
////////////////////////////////////////////////////////////////////////
{
    ////////////////////////////////////////////
    //
    // The copy operation is done in two passes:
    //
    // (1) Determine which nodes are "inside" the copy. That is, which
    //     nodes are under the original node being copied.
    //
    // (2) Copy the graph. Inside nodes and engines re-use the same
    //     instance each time encountered. References to "outside"
    //     nodes and engines are copied as just pointers.
    //     (An engine is inside only if there is at least one
    //     inside node connected somewhere at both ends.)
    //
    // To make these steps more efficient, a dictionary of inside
    // nodes is created in step (1) and used in step (2). Engines
    // determined to be inside in step (2) are also added to the
    // dictionary. This dictionary is maintained in the
    // SoFieldContainer class so both nodes and engines can access it.
    //
    ////////////////////////////////////////////

    // Ref ourselves, just in case our ref count is 0
    ref();

    // Step (1):

    // Set up a new dictionary. Recursive copy operations use new
    // dictionaries to avoid confusion.
    initCopyDict();

    // Recursively figure out which nodes are inside and add them to
    // the copy dictionary, each with NO ref().
    SoNode *newNode = addToCopyDict();
    newNode->ref();

    // Step (2):

    // Copy the contents of this node into the new copy. This will
    // recurse (for groups) and will also handle connections and
    // fields that point to nodes, paths, or engines.
    newNode->copyContents(this, copyConnections);

    // Get rid of the dictionary
    copyDone();

    // Return the copy
    newNode->unrefNoDelete();
    unref();
    return newNode;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Recursively adds this node and all nodes under it to the copy
//    dictionary. Returns the copy of this node.
//
// Use: internal, virtual

SoNode *
SoNode::addToCopyDict() const
//
////////////////////////////////////////////////////////////////////////
{
    // If this node is already in the dictionary, nothing else to do
    SoNode *copy = (SoNode *) checkCopy(this);
    if (copy == NULL) {

	// Create and add a new instance to the dictionary
	copy = (SoNode *) getTypeId().createInstance();
	copy->ref();
	addCopy(this, copy);		// Adds a ref()
	copy->unrefNoDelete();

	// Recurse on children, if any
	SoChildList *kids = getChildren();
	if (kids != NULL)
	    for (int i = 0; i < kids->getLength(); i++)
		(*kids)[i]->addToCopyDict();
    }

    return copy;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Copies the contents of the given node into this instance. This
//    calls the default implementation and also copies the override
//    flag.
//
// Use: internal, virtual

void
SoNode::copyContents(const SoFieldContainer *fromFC, SbBool copyConnections)
//
////////////////////////////////////////////////////////////////////////
{
    // Copy the regular stuff
    SoFieldContainer::copyContents(fromFC, copyConnections);

    // Copy the override flag
    override = ((SoNode *) fromFC)->override;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    During a copy operation, this copies an instance that is
//    encountered through a field connection.
//
// Use: internal, virtual

SoFieldContainer *
SoNode::copyThroughConnection() const
//
////////////////////////////////////////////////////////////////////////
{
    // If there is already a copy of this node (created during the
    // first pass of a copy() operation), use it. Otherwise, just use
    // this node.
    SoFieldContainer *copy = findCopy(this, TRUE);
    if (copy != NULL)
	return copy;
    return (SoFieldContainer *) this;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the last node given the name 'name'.  Returns NULL if
//    there is no node with the given name.
//
// Use: public

SoNode *
SoNode::getByName(const SbName &name)
//
////////////////////////////////////////////////////////////////////////
{
    return (SoNode *)getNamedBase(name, SoNode::getClassTypeId());
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds all nodes named 'name' to the list.  Returns the number of
//    nodes found.
//
// Use: public

int
SoNode::getByName(const SbName &name, SoNodeList &list)
//
////////////////////////////////////////////////////////////////////////
{
    return getNamedBases(name, list, SoNode::getClassTypeId());
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if a node has an affect on the state during
//    traversal. The default method returns TRUE. Node classes (such
//    as SoSeparator) that isolate their effects from the rest of the
//    graph override this method to return FALSE.
//
// Use: public

SbBool
SoNode::affectsState() const
//
////////////////////////////////////////////////////////////////////////
{
    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Virtual methods implementing actions handled in subclasses. Most
//    of these do nothing at all.
//
// Use: extender (all)
//
////////////////////////////////////////////////////////////////////////

void
SoNode::doAction(SoAction *)
{
#ifdef DEBUG_DEFAULT_METHODS
    SoDebugInfo::post("SoNode::doAction",
		      "Called for %s", getTypeId().getName().getString());
#endif /* DEBUG_DEFAULT_METHODS */
}    

void
SoNode::callback(SoCallbackAction *)
{
#ifdef DEBUG_DEFAULT_METHODS
    SoDebugInfo::post("SoNode::callback",
		      "Called for %s", getTypeId().getName().getString());
#endif /* DEBUG_DEFAULT_METHODS */
}    

void
SoNode::GLRender(SoGLRenderAction *)
{
#ifdef DEBUG_DEFAULT_METHODS
    SoDebugInfo::post("SoNode::GLRender",
		      "Called for %s", getTypeId().getName().getString());
#endif /* DEBUG_DEFAULT_METHODS */
}

void
SoNode::GLRenderBelowPath(SoGLRenderAction *action)
{
    GLRender(action);
}

void
SoNode::GLRenderInPath(SoGLRenderAction *action)
{
    GLRender(action);
}

void
SoNode::GLRenderOffPath(SoGLRenderAction *action)
{
    GLRender(action);
}

void
SoNode::getBoundingBox(SoGetBoundingBoxAction *)
{
#ifdef DEBUG_DEFAULT_METHODS
    SoDebugInfo::post("SoNode::getBoundingBox",
		      "Called for %s", getTypeId().getName().getString());
#endif /* DEBUG_DEFAULT_METHODS */
}    

void
SoNode::getMatrix(SoGetMatrixAction *)
{
#ifdef DEBUG_DEFAULT_METHODS
    SoDebugInfo::post("SoNode::getMatrix",
		      "Called for %s", getTypeId().getName().getString());
#endif /* DEBUG_DEFAULT_METHODS */
}    

void
SoNode::handleEvent(SoHandleEventAction *)
{
#ifdef DEBUG_DEFAULT_METHODS
    SoDebugInfo::post("SoNode::handleEvent",
		      "Called for %s", getTypeId().getName().getString());
#endif /* DEBUG_DEFAULT_METHODS */
}    

void
SoNode::pick(SoPickAction *)
{
#ifdef DEBUG_DEFAULT_METHODS
    SoDebugInfo::post("SoNode::pick",
		      "Called for %s", getTypeId().getName().getString());
#endif /* DEBUG_DEFAULT_METHODS */
}    

void
SoNode::rayPick(SoRayPickAction *action)
{
    // If the node doesn't have a specific rayPick() method, it may
    // have a more general pick() method for any pick action.
    pick(action);
}    

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements search action for most nodes.
//
// Use: extender

void
SoNode::search(SoSearchAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    int		lookingFor = action->getFind();
    SbBool	foundMe = TRUE;

    // We have to match everything set by the search action.
    // First, see if node doesn't match:
    if ((lookingFor & SoSearchAction::NODE) && action->getNode() != this)
	foundMe = FALSE;

    // Next, see if the name doesn't match:
    if ((lookingFor & SoSearchAction::NAME) &&
	action->getName() != this->getName())
	foundMe = FALSE;

    // Finally, figure out if types match:
    if (lookingFor & SoSearchAction::TYPE) {
	int	derivedOk;
	SoType	t = action->getType(derivedOk);
	if (! (derivedOk ? isOfType(t) : getTypeId() == t))
	    foundMe = FALSE;
    }

    if (foundMe) {
	// We have a match! Add it to the action.

	if (action->getInterest() == SoSearchAction::ALL)
	    action->getPaths().append(action->getCurPath()->copy());

	else {
	    action->addPath(action->getCurPath()->copy());
	    if (action->getInterest() == SoSearchAction::FIRST)
		action->setFound();
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements write action for most nodes.
//
// Use: extender

void
SoNode::write(SoWriteAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    if (action->getOutput()->getStage() == SoOutput::COUNT_REFS)
	addWriteReference(action->getOutput());

    // We assume this node would not be traversed if is has not been counted
    else if (! writeHeader(action->getOutput(), FALSE, FALSE)) {

	const SoFieldData *fieldData = getFieldData();

	fieldData->write(action->getOutput(), this);

	writeFooter(action->getOutput());
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements default setup of event grabbing
//
// Use: extender

void
SoNode::grabEventsSetup()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements default cleanup of event grabbing
//
// Use: extender

void
SoNode::grabEventsCleanup()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initiates notification from the node.
//
// Use: internal

void
SoNode::startNotify()
//
////////////////////////////////////////////////////////////////////////
{
    // Update our unique id to indicate that we are a different node.
    uniqueId = nextUniqueId++;

    // Let FieldContainer pass notification on to auditors...
    SoFieldContainer::startNotify();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Propagates notification through the node. First updates uniqueId
//    and then does base class action.
//
// Use: internal

void
SoNode::notify(SoNotList *list)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (list == NULL) {
	SoDebugError::post("SoNode::notify",
			   "notification list pointer is NULL");
	return;
    }
#endif /* DEBUG */

    // Do nothing if this node has already been notified by this
    // current notification. We can tell this by comparing the
    // notification list's time stamp with the node id.
    if (list->getTimeStamp() <= uniqueId)
	return;

    // Update our unique id to indicate that we are a different node
    uniqueId = nextUniqueId++;

    // Let FieldContainer do most of the work.
    SoFieldContainer::notify(list);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns pointer to children. Default returns NULL since most
//    nodes don't have children.
//
// Use: internal

SoChildList *
SoNode::getChildren() const
//
////////////////////////////////////////////////////////////////////////
{
    return NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes instance to SoOutput. This is called only through
//    non-standard traversal mechanisms. (write() is used for the
//    standard one.) For example, this is called when a node connected
//    to a field or pointed to by a field is written out. This needs
//    to create a new SoWriteAction to continue the writing process.
//
// Use: internal

void
SoNode::writeInstance(SoOutput *out)
//
////////////////////////////////////////////////////////////////////////
{
    SoWriteAction	wa(out);
    wa.continueToApply(this);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Static callbacks for most actions just call corresponding
//    virtual method.
//
// Use: private (all)
//
////////////////////////////////////////////////////////////////////////

void
SoNode::getMatrixS(SoAction *action, SoNode *node)
{
    node->getMatrix((SoGetMatrixAction *) action);
}

void
SoNode::handleEventS(SoAction *action, SoNode *node)
{
    node->handleEvent((SoHandleEventAction *) action);
}

void
SoNode::pickS(SoAction *action, SoNode *node)
{
    node->pick((SoPickAction *) action);
}

void
SoNode::rayPickS(SoAction *action, SoNode *node)
{
    node->rayPick((SoRayPickAction *) action);
}

void
SoNode::searchS(SoAction *action, SoNode *node)
{
    node->search((SoSearchAction *) action);
}

void
SoNode::writeS(SoAction *action, SoNode *node)
{
    node->write((SoWriteAction *) action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    These three methods are treated differently because they do some
//    extra work.
//
// Use: private (all)
//
////////////////////////////////////////////////////////////////////////

void
SoNode::callbackS(SoAction *action, SoNode *node) 
{
    SoCallbackAction *a = (SoCallbackAction *)action;

    // Pre/post callbacks are automatically handled.  If the callbacks
    // set the 'response' flag to stop traversal, handle that also. 

    if (a->hasTerminated())
	return;

    a->setCurrentNode(node);
    
    a->invokePreCallbacks(node);

    if (! a->hasTerminated() &&
	a->getCurrentResponse() != SoCallbackAction::PRUNE)
	node->callback(a);

    a->invokePostCallbacks(node);
}

void
SoNode::GLRenderS(SoAction *action, SoNode *node)
{
    SoGLRenderAction *a = (SoGLRenderAction *) action;

    if (! a->abortNow())
	node->GLRender(a);
    else {
	SoCacheElement::invalidate(action->getState());
    }
}

void
SoNode::getBoundingBoxS(SoAction *action, SoNode *node) 
{
    SoGetBoundingBoxAction *a = (SoGetBoundingBoxAction *)action;

    a->checkResetBefore();
    node->getBoundingBox(a);
    a->checkResetAfter();
}
