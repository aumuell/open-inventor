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
 * Copyright (C) 1990-93   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Classes:
 |	SoSelection
 |
 |   Authors: David Mott, Paul Isaacs, Thaddeus Beier
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <X11/Xlib.h>

#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoInteraction.h>
#include <Inventor/SoLists.h>
#include <Inventor/SoOutput.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/events/SoEvent.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/fields/SoFieldData.h>
#include <Inventor/misc/SoCallbackList.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoSubNode.h>


SO_NODE_SOURCE(SoSelection);

SoSearchAction *SoSelection::searchAction = NULL;


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor initialization
//
// Use: private
//
void
SoSelection::constructorCommon()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoSelection);
    SO_NODE_ADD_FIELD(policy, (SoSelection::SHIFT));

    // Set up static info for enumerated type field
    SO_NODE_DEFINE_ENUM_VALUE(Policy, SINGLE);
    SO_NODE_DEFINE_ENUM_VALUE(Policy, TOGGLE);
    SO_NODE_DEFINE_ENUM_VALUE(Policy, SHIFT);

    // Set up info in enumerated type field
    SO_NODE_SET_SF_ENUM_TYPE(policy, Policy);

    isBuiltIn = TRUE;

    // selection callback functions and user data
    selCBList = NULL;
    deselCBList = NULL;
    startCBList = NULL;
    finishCBList = NULL;  
    changeCBList = NULL;
    
    pickCBFunc = NULL;
    pickCBData = NULL;
    
    mouseDownPickPath = NULL;
    pickMatching = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructors
//
// Use: public
//
SoSelection::SoSelection()
{
    constructorCommon();
}

SoSelection::SoSelection(int numChildren)
 : SoSeparator(numChildren)
{
    constructorCommon();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: protected

SoSelection::~SoSelection()
//
////////////////////////////////////////////////////////////////////////
{
    delete selCBList;
    delete deselCBList;
    delete startCBList;
    delete finishCBList;  
    delete changeCBList;
    
    if (mouseDownPickPath != NULL)
	mouseDownPickPath->unref();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Traversal for event handling. The parent class SoSeparator is called
// to traverse the children. If the event is not handled by any children,
// then handle it here by invoking the current selection policy.
// Calls action->setHandled() if the event was handled.
//
// Use: protected

void
SoSelection::handleEvent(SoHandleEventAction *action)
//
////////////////////////////////////////////////////////////////////////
{    
    // let SoSeparator traverse the children
    SoSeparator::handleEvent(action);

    // On Button1 press, see who got picked.
    // On Button1 release, see who got picked.
    // If pick-matching is on, and they match,
    // then select the object that got picked.
    //??? this event should be programmer configurable! translation tables?
    const SoEvent *event = action->getEvent();
    if (! event->isOfType(SoMouseButtonEvent::getClassTypeId()))
	return;

    const SoMouseButtonEvent *be = (const SoMouseButtonEvent *) event;
    if (be->getButton()	!= SoMouseButtonEvent::BUTTON1)
	return;

    if ((pickMatching && (be->getState() == SoButtonEvent::DOWN)) ||
	(be->getState() == SoButtonEvent::UP)) {	
	//??? change the action so we only pick the FIRST thing, not a list?
	const SoPickedPoint *pickedPoint = action->getPickedPoint();
	SoPath *pickPath = NULL;
	
	// Get the pick path
	if (pickedPoint != NULL) {
	    // If the pick callback exists, let it tell us what path to pick
	    if (pickCBFunc != NULL) {
		if (callPickCBOnlyIfSelectable) {
		    // Make sure pick passes through this
		    if (pickedPoint->getPath()->containsNode(this))
			pickPath = (*pickCBFunc)(pickCBData, pickedPoint);
		}
		else pickPath = (*pickCBFunc)(pickCBData, pickedPoint);
	    }
	    // Else no pick CB - use the picked path as is.
	    else pickPath = pickedPoint->getPath();
	}
	
	// For button press, save the pick path whether a child
	// handled the event or not - we may get a crack at the
	// button release event later.
	if (be->getState() == SoButtonEvent::DOWN) {
	    if (mouseDownPickPath != NULL)
		mouseDownPickPath->unref(); // out with the old...
		
	    mouseDownPickPath = copyFromThis(pickPath);
	    
	    if (mouseDownPickPath != NULL) {
		mouseDownPickPath->ref();	// ...in with the new
		action->setHandled();
	    }
	}
	// Else button release.
	// If no children handled the event during SoSeparator::handleEvent()
	// traversal, handle it here.
	else if (! action->isHandled()) {
	    // For button release, get the pick path and see if it matches
	    // the button press pick path.
	    //
	    // If they match, invoke the selection policy.
	    // If they do NOT match, do nothing.
	    
	    if (pickPath == NULL) {
		// If nothing was picked, pass NULL to invokeSelectionPolicy.
		if ((! pickMatching) || (mouseDownPickPath == NULL)) {
		    invokeSelectionPolicy(NULL, be->wasShiftDown());
		    action->setHandled();
		}
		// else paths do not match - ignore event
	    }
	    else {
		pickPath->ref();
		
		if ((! pickMatching) || (mouseDownPickPath != NULL)) {
		    // Mouse down pick hit something.
		    // Was it same as mouse up pick?
		    // See if the pick path passes through this node
		    SoPath *mouseUpPickPath = copyFromThis(pickPath);
		    if (mouseUpPickPath != NULL) {
			mouseUpPickPath->ref();
		    
			// If paths match, invoke the selection policy. 
			if ((! pickMatching) ||
			    (*mouseDownPickPath == *mouseUpPickPath)) {
			    if (mouseUpPickPath->getLength() == 1) {
				// Path is to ONLY the selection node.
				// Act as if nothing were picked, but
				// allow traversal to continue.
				invokeSelectionPolicy(NULL, be->wasShiftDown());
			    }
			    else {
				// Alter selection and halt traversal.
				invokeSelectionPolicy(mouseUpPickPath,
				    be->wasShiftDown());
				action->setHandled();
			    }
			}
			// else paths do not match - ignore event
			
			mouseUpPickPath->unref();
		    }
		    // else path does not pass through this node - ignore event
		}
		// else paths do not match - ignore event
		
		pickPath->unref();
	    }
	    
	    // All done with mouse down pick path
	    if (mouseDownPickPath != NULL) {
		mouseDownPickPath->unref();
		mouseDownPickPath = NULL;
	    }
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   invoke the appropriate routine which implements the current
// selection policy. 
//
// Use: protected
//
void
SoSelection::invokeSelectionPolicy(SoPath *path, SbBool shiftDown)
//
////////////////////////////////////////////////////////////////////////
{
    // employ the current selection policy
    switch (policy.getValue()) {
	case SINGLE:
	    performSingleSelection(path);
	    break;
	case TOGGLE:
	    performToggleSelection(path);
	    break;
	case SHIFT:
	    if (shiftDown)
	    	 performToggleSelection(path); // SHIFT DOWN
	    else performSingleSelection(path); // NO SHIFT
	    break;
	default:
#ifdef DEBUG
	    SoDebugError::post("SoSelection::invokeSelectionPolicy",
	    "Unknown selection policy %d", policy.getValue());
#endif
	    break;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   clear the selection, then select the picked object.
//   if no object is picked, then nothing is selected.
//   only one object may be selected at a time. The passed path should have
//   already been copied (i.e. we just store it!)
//
// Use: protected
//
void
SoSelection::performSingleSelection(SoPath *path)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool needFinishCB = FALSE;
    
    // let app know (if and only if) user is changing the selection
    if ((getNumSelected() > 0) || (path != NULL)) {
	if (startCBList != NULL)
	    startCBList->invokeCallbacks(this);
	needFinishCB = TRUE;
    }

    if (path == NULL) {
	deselectAll();
    }
    else {
	if (isSelected(path)) {
	    // Deselect everything except the given path.
	    // This avoids deselecting and then selecting the same thing.
	    int which = selectionList.findPath(*path);
	    if (which != -1) {
		for (int i = selectionList.getLength() - 1; i >= 0; i--) {
		    if (i != which)
			removePath(i);
		}
	    }
	}
	else {
	    deselectAll();
	    addPath(path);
	}
    }
    
    // let app know user is done changing the selection
    if (needFinishCB) {
	if (finishCBList != NULL)
	    finishCBList->invokeCallbacks(this);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   picking on an object toggles that objects selection status.
//   multiple objects may be selected. The passed path should have
//   already been copied (i.e. we just store it!)
//
// Use: protected
//
void
SoSelection::performToggleSelection(SoPath *path)
//
////////////////////////////////////////////////////////////////////////
{
    if (path != NULL) {
	// let app know user is changing the selection
	if (startCBList != NULL)
	    startCBList->invokeCallbacks(this);
	
	// toggle the picked object
	int which = findPath(path);
	if (which == -1)
	     addPath(path);	// path not in list, add it
	else removePath(which);	// path in list, remove it

	// let app know user is done changing the selection
	if (finishCBList != NULL)
	    finishCBList->invokeCallbacks(this);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Find 'this' in the path, then copy the path from 'this' down to
// the path tail. If 'this' is not in the path, return NULL.
//
// Use: private
//
SoPath *
SoSelection::copyFromThis(const SoPath *path) const
{
    if (path == NULL)
	return NULL;
	
    SoNode *node;
    int i, indexToThis = -1;
    SoPath *p = NULL;

    SoFullPath *fullInPath = (SoFullPath *) path;

    for (i = 0; i < fullInPath->getLength(); i++) {
	node = fullInPath->getNode(i);
	if (node == this) {
	    indexToThis = i;
	    break;
	}
    }
    
    if (indexToThis != -1) {
        p = fullInPath->copy(indexToThis);
    }
    
    return p;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Add path to the selection list. It is assumed that path->head
// is this - DO NOT CALL THIS FUNCTION unless this is true. Also, the
// path must include more than just the selection node by itself.
//
// Use: private
//
void
SoSelection::addPath(SoPath *path)
//
////////////////////////////////////////////////////////////////////////
{
    // Add the path to the selection list,
    // and notify the app that this path has been selected
    if (selectionList.findPath(*path) == -1) {
    	path->ref();
	selectionList.append(path);
	if (selCBList != NULL)
	    selCBList->invokeCallbacks(path);
	path->unref();
	
	if (changeCBList != NULL)
	    changeCBList->invokeCallbacks(this);
    }
}

//////////////////////////////////////////////////////////////////////////////
//
//  This method takes the path indexed by `which' out of the selection list.
//  The path will be unreferenced by the SoPathList (SoBaseList) class.
//
//  Use: protected
//
//
void
SoSelection::removePath(int which)
//
//////////////////////////////////////////////////////////////////////////////
{
    // Remove the path from the selection list,
    // and notify the app that this path has been deselected.
    if (which >= 0) {
	SoPath *p = (SoPath *) selectionList[which];
	p->ref();
	selectionList.remove(which);
	if (deselCBList != NULL)
	    deselCBList->invokeCallbacks(p);
	p->unref();	
	
	if (changeCBList != NULL)
	    changeCBList->invokeCallbacks(this);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Add this path to the selection list.  Path should include this
// node. The path will be referenced by the SoPathList (SoBaseList) class.
//
// Use: public
//
void
SoSelection::select(const SoPath *path)
//
////////////////////////////////////////////////////////////////////////
{
    // get a path whose head is this selection node
    SoPath *selPath = copyFromThis(path);
    if ((selPath != NULL) && (selPath->getLength() > 1)) {
	selPath->ref();
    	addPath(selPath);
	selPath->unref();
    }
#ifdef DEBUG
    else {
	SoDebugError::post("SoSelection::select",
	"Path does not pass through this selection node.");
	return;
    }
#endif
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Search for the passed node under this, then select that path.
//
// Use: public
//
void
SoSelection::select(SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
    if (node != NULL) {
	node->ref();
	
	if (searchAction == NULL)
	    searchAction = new SoSearchAction;
	else
	    searchAction->reset();
	searchAction->setInterest(SoSearchAction::FIRST);
	searchAction->setFind(SoSearchAction::NODE);
	searchAction->setNode(node);
	searchAction->apply(this);
	
	SoPath *p = searchAction->getPath();
	if ((p != NULL) && (p->getLength() > 1))
	    addPath(p);
#ifdef DEBUG
	else {
	    SoDebugError::post("SoSelection::select",
	    "Node does not pass through this selection node.");
	    return;
	}
#endif
	
	node->unref();
    }
}

//////////////////////////////////////////////////////////////////////////////
//
//  This method takes the path indexed by `which' out of the selection list.
//  The path will be unreferenced by the SoPathList (SoBaseList) class.
//
//  Use: public
//
//
void
SoSelection::deselect(int which)
//
//////////////////////////////////////////////////////////////////////////////
{
    removePath(which);
}

//////////////////////////////////////////////////////////////////////////////
//
//  remove a path, finds path and removes it by index
//
//  Use: public
//
void
SoSelection::deselect(const SoPath *path)
//
//////////////////////////////////////////////////////////////////////////////
{
//??? should we copyFromThis() as a convenience to the caller?
    if (path != NULL) {
	int which;
	if ((which = findPath(path)) != -1)
	    removePath(which);
	
#ifdef DEBUG
	else
	    SoDebugError::post("SoSelection::deselect", "Path not already selected");
#endif

    }

#ifdef DEBUG
    else {
	SoDebugError::post("SoSelection::deselect", "Passed path is NULL");
    }
#endif
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Search for the passed node under this, then deselect that path.
//
// Use: public
//
void
SoSelection::deselect(SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
    if (node != NULL) {
	node->ref();
	
	if (searchAction == NULL)
	    searchAction = new SoSearchAction;
	else
	    searchAction->reset();
	searchAction->setInterest(SoSearchAction::FIRST);
	searchAction->setFind(SoSearchAction::NODE);
	searchAction->setNode(node);
	searchAction->apply(this);
	if (searchAction->getPath() != NULL)
	    deselect(searchAction->getPath());
	
	node->unref();
    }
}


//////////////////////////////////////////////////////////////////////////////
//
//  returns the index of the path in the selection list
//
//  Use: protected
//
int
SoSelection::findPath(const SoPath *path) const
//
//////////////////////////////////////////////////////////////////////////////
{
    int index = -1;
    if (path != NULL) {
	SoPath *selPath = NULL;
	if (path->getHead() != this)
	     selPath = copyFromThis(path);
	else selPath = (SoPath *) path; // const cast away
	
	// selPath still not NULL? (copyFromThis() might have returned NULL)
	if (selPath != NULL) {
	    selPath->ref();
	    index = ((SoPathList)selectionList).findPath(*selPath);
	    selPath->unref();
	}
    }
    
    return index;
}

//////////////////////////////////////////////////////////////////////////////
//
//  returns TRUE if the path is already selected
//
//  Use: public
//
SbBool
SoSelection::isSelected(const SoPath *path) const
//
//////////////////////////////////////////////////////////////////////////////
{
//??? should we copyFromThis() as a convenience?
    return (findPath(path) != -1);
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Search for the passed node under this, then return whether it
// is selected.
//
// Use: public
//
SbBool
SoSelection::isSelected(SoNode *node) const
//
////////////////////////////////////////////////////////////////////////
{
    SbBool itIs = FALSE;
    
    if (node != NULL) {
	node->ref();
	
	if (searchAction == NULL)
	    searchAction = new SoSearchAction;
	else
	    searchAction->reset();
	searchAction->setInterest(SoSearchAction::FIRST);
	searchAction->setFind(SoSearchAction::NODE);
	searchAction->setNode(node);
	searchAction->apply((SoSelection *) this); // const cast away
	if (searchAction->getPath() != NULL)
	    itIs = isSelected(searchAction->getPath());
	
	node->unref();
    }
    
    return itIs;
}

//////////////////////////////////////////////////////////////////////////////
//
//  toggle adds path if it's not in the selectionList, removes path if it is.
//
//  Use: public
//
void
SoSelection::toggle(const SoPath *path)
//
//////////////////////////////////////////////////////////////////////////////
{
    // get a path whose head is this selection node
    SoPath *selPath = copyFromThis(path);
    if ((selPath != NULL) && (selPath->getLength() > 1)) {
	selPath->ref();
	int which = findPath(selPath);
	if (which == -1)
	     addPath(selPath);	// path not in list, add it
	else removePath(which);	// path in list, remove it
	selPath->unref();
    }
#ifdef DEBUG
    else {
	SoDebugError::post("SoSelection::toggle",
	"Path does not pass through this selection node.");
    }
#endif
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Search for the passed node under this, then toggle that path.
//
// Use: public
//
void
SoSelection::toggle(SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
    if (node != NULL) {
	node->ref();
	
	if (searchAction == NULL)
	    searchAction = new SoSearchAction;
	else
	    searchAction->reset();
	searchAction->setInterest(SoSearchAction::FIRST);
	searchAction->setFind(SoSearchAction::NODE);
	searchAction->setNode(node);
	searchAction->apply(this);
	if (searchAction->getPath() != NULL)
	    toggle(searchAction->getPath());
	
	node->unref();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    removes every path in the selection list
//
// Use: public
void
SoSelection::deselectAll()
//
////////////////////////////////////////////////////////////////////////
{
    for(int i = selectionList.getLength() - 1; i >= 0; i--)
	deselect(i);
}


//////////////////////////////////////////////////////////////////////////////
//
//  Description:
//    get the ith path.
//
//  Use: public
//
SoPath *
SoSelection::getPath(int index) const
//
//////////////////////////////////////////////////////////////////////////////
{
    SoPath *p;
    
#ifdef DEBUG
    if ((index >= selectionList.getLength()) || (index < 0)) {
	SoDebugError::post("SoSelection::getPath", "Index out of range.  Index = %d, numSelected = %d",
	    index, selectionList.getLength());
	p = NULL;
    }
    else
#endif
	p = selectionList[index];
	
    return p;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//	Methods for setting callback functions.
//
// Use: public
//
//
////////////////////////////////////////////////////////////////////////

void
SoSelection::addSelectionCallback(SoSelectionPathCB *f, void *userData)
{
    if (selCBList == NULL)
	selCBList = new SoCallbackList;
    selCBList->addCallback((SoCallbackListCB *) f, userData);
}
void
SoSelection::removeSelectionCallback(SoSelectionPathCB *f, void *userData)
{
    if (selCBList != NULL)
	selCBList->removeCallback((SoCallbackListCB *) f, userData);
}

void
SoSelection::addDeselectionCallback(SoSelectionPathCB *f, void *userData)
{
    if (deselCBList == NULL)
	deselCBList = new SoCallbackList;
    deselCBList->addCallback((SoCallbackListCB *) f, userData);
}
void
SoSelection::removeDeselectionCallback(SoSelectionPathCB *f, void *userData)
{
    if (deselCBList != NULL)
	deselCBList->removeCallback((SoCallbackListCB *) f, userData);
}

void
SoSelection::addStartCallback(SoSelectionClassCB *f, void *userData)
{
    if (startCBList == NULL)
	startCBList = new SoCallbackList;
    startCBList->addCallback((SoCallbackListCB *) f, userData);
}
void
SoSelection::removeStartCallback(SoSelectionClassCB *f, void *userData)
{
    if (startCBList != NULL)
	startCBList->removeCallback((SoCallbackListCB *) f, userData);
}

void
SoSelection::addFinishCallback(SoSelectionClassCB *f, void *userData)
{
    if (finishCBList == NULL)
	finishCBList = new SoCallbackList;
    finishCBList->addCallback((SoCallbackListCB *) f, userData);
}
void
SoSelection::removeFinishCallback(SoSelectionClassCB *f, void *userData)
{
    if (finishCBList != NULL)
	finishCBList->removeCallback((SoCallbackListCB *) f, userData);
}

void
SoSelection::addChangeCallback(SoSelectionClassCB *f, void *userData)
{
    if (changeCBList == NULL)
	changeCBList = new SoCallbackList;
    changeCBList->addCallback((SoCallbackListCB *) f, userData);
}
void
SoSelection::removeChangeCallback(SoSelectionClassCB *f, void *userData)
{
    if (changeCBList != NULL)
	changeCBList->removeCallback((SoCallbackListCB *) f, userData);
}

void
SoSelection::setPickFilterCallback(
    SoSelectionPickCB *f,
    void *userData, 
    SbBool callOnlyIfSelectable)
{
    pickCBFunc = f;
    pickCBData = userData;
    callPickCBOnlyIfSelectable = callOnlyIfSelectable;
}
