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
 |	SoSwitch
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/actions/SoPickAction.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/elements/SoSwitchElement.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/nodes/SoSwitch.h>

SO_NODE_SOURCE(SoSwitch);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoSwitch::SoSwitch()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoSwitch);
    SO_NODE_ADD_FIELD(whichChild, (SO_SWITCH_NONE));
    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor that takes approximate number of children.
//
// Use: public

SoSwitch::SoSwitch(int nChildren) : SoGroup(nChildren)
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoSwitch);
    SO_NODE_ADD_FIELD(whichChild, (SO_SWITCH_NONE));
    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoSwitch::~SoSwitch()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides method in SoNode to return FALSE if there is no
//    selected child or the selected child does not affect the state.
//
// Use: public

SbBool
SoSwitch::affectsState() const
//
////////////////////////////////////////////////////////////////////////
{
    int which;
    if (whichChild.isIgnored())
	which = SO_SWITCH_NONE;
    else
	which = whichChild.getValue();

    // Special case-- if called during application of an
    // SoSearchAction that is searching all:
    if (SoSearchAction::duringSearchAll)
	which = SO_SWITCH_ALL;

    if (whichChild.isIgnored() || which == SO_SWITCH_NONE)
	return FALSE;

    if (which == SO_SWITCH_ALL ||
	which == SO_SWITCH_INHERIT)
	return TRUE;

    if (getChild(which)->affectsState())
	return TRUE;

    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Typical action traversal.
//
// Use: extender

void
SoSwitch::doAction(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    int		numIndices;
    const int	*indices;

    if (action->getPathCode(numIndices, indices) == SoAction::IN_PATH) {
	int	i;
	for (i = 0; i < numIndices; i++)
	    doChild(action, indices[i]);
    }

    else
	doChild(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Traversal for callback action.
//
// Use: extender

void
SoSwitch::callback(SoCallbackAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoSwitch::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Traversal for GL rendering
//
// Use: extender

void
SoSwitch::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoSwitch::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Traversal for picking
//
// Use: extender

void
SoSwitch::pick(SoPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoSwitch::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Traversal for get bounding box
//
// Use: extender

void
SoSwitch::getBoundingBox(SoGetBoundingBoxAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoSwitch::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Traversal for handle event
//
// Use: extender

void
SoSwitch::handleEvent(SoHandleEventAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoSwitch::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements getMatrix action.
//
// Use: extender

void
SoSwitch::getMatrix(SoGetMatrixAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    int		numIndices;
    const int	*indices;

    // Only need to compute matrix if switch is a node in middle of
    // current path chain or is off path chain (since the only way
    // this could be called if it is off the chain is if the switch is
    // under a group that affects the chain).

    switch (action->getPathCode(numIndices, indices)) {

      case SoAction::NO_PATH:
      case SoAction::BELOW_PATH:
	break;

      case SoAction::OFF_PATH:
      case SoAction::IN_PATH:
	SoSwitch::doAction(action);
	break;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements search action.
//
// Use: extender
//
////////////////////////////////////////////////////////////////////////

void 
SoSwitch::search(SoSearchAction *action)
{
    // if the action is searching everything, then do so...
    if (action->isSearchingAll())
	SoGroup::search(action);

    // Otherwise, traverse according to the regular switch node rules
    else {
	// First, make sure this node is found if we are searching for
	// switches
	SoNode::search(action);

	// Recurse
	if (! action->isFound())
	    SoSwitch::doAction(action);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Traverses correct child based on value of whichChild field. If
//    matchIndex is non-negative, the indicated child must match it
//    for it to be traversed. Sets switch element in state unless we
//    are inheriting the value.
//
// Use: private

void
SoSwitch::doChild(SoAction *action, int matchIndex)
//
////////////////////////////////////////////////////////////////////////
{
    int32_t	which;

    if (whichChild.isIgnored())
	which = SO_SWITCH_NONE;

    else
	which = whichChild.getValue();

    // If index is inherited from state, get value from there
    if (which == SO_SWITCH_INHERIT) {

	which = SoSwitchElement::get(action->getState());

	// Make sure it is in range
	if (which >= getNumChildren())
	    which %= getNumChildren();
    }

    // Store resulting index in state if not already inherited from there
    else
	SoSwitchElement::set(action->getState(), which);

    // Now we have the real value to deal with

    switch (which) {

      case SO_SWITCH_NONE:
	break;

      case SO_SWITCH_ALL:
	// If traversing to compute bounding box, we have to do some
	// work in between children
	if (action->isOfType(SoGetBoundingBoxAction::getClassTypeId())) {
	    SoGetBoundingBoxAction *bba = (SoGetBoundingBoxAction *) action;
	    SbVec3f	totalCenter(0.0, 0.0, 0.0);
	    int		numCenters = 0;
	    int 	lastChild = (matchIndex >= 0 ? matchIndex :
				     getNumChildren()  - 1);

	    for (int i = 0; i <= lastChild; i++) {
		children->traverse(bba, i, i);
		if (bba->isCenterSet()) {
		    totalCenter += bba->getCenter();
		    numCenters++;
		    bba->resetCenter();
		}
	    }
	    // Now, set the center to be the average:
	    if (numCenters != 0)
		bba->setCenter(totalCenter / numCenters, FALSE);
	}
	else {
	    if (matchIndex >= 0)
		children->traverse(action, 0, matchIndex);
	    else
		children->traverse(action);
	}
	break;

      default:

	// Make sure index is reasonable
	if (which < 0 || which >= getNumChildren()) {
#ifdef DEBUG	
	    SoDebugError::post("SoSwitch::doChild",
			       "Child index %d is out of range %d - %d "
			       "(applying %s)",
			       which, 0, getNumChildren() - 1,
			       action->getTypeId().getName().getString());
#endif /* DEBUG */			       
	    break;
	}

	// Traverse indicated child
	if (matchIndex < 0 || matchIndex == which)
	    children->traverse(action, (int) which);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements write action.
//
// Use: extender

void
SoSwitch::write(SoWriteAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoOutput *out = action->getOutput();

    // When writing out a switch that is in a path, we always want to
    // write out ALL children of the switch. If we did the default
    // thing of writing out just those children that affect the nodes
    // in the path, we could screw up. Consider a switch that has two
    // child separators and whichChild set to 1. If a path goes
    // through the switch to the second child, the first child, being
    // a separator, does not affect the path. But if we don't write
    // out the separator, the whichChild will reference a
    // nonexistent child. So we always write out all children.

    // NOTE: SoChildList::traverse() checks the current path code and
    // skips children off the path that do not affect the
    // state. Because of this we have to avoid calling it. Instead, we
    // do its work here.

    // This code is stolen and modified from SoGroup::write() and
    // SoChildList::traverse()

    int lastChild = getNumChildren() - 1;
    SoAction::PathCode pc = action->getCurPathCode();

    // In write-reference counting phase
    if (out->getStage() == SoOutput::COUNT_REFS) {

	// Increment our write reference count
	addWriteReference(out);

	// If this is the first reference (i.e., we don't now have
	// multiple references), also count all appropriate children
	if (! hasMultipleWriteRefs()) {
	    for (int i = 0; i <= lastChild; i++) {
		action->pushCurPath(i);
		action->traverse(getChild(i));
		action->popCurPath(pc);
	    }
	}
    }

    // In writing phase, we have to do some more work
    else if (! writeHeader(out, TRUE, FALSE)) {

	// Write fields
	const SoFieldData *fieldData = getFieldData();
	fieldData->write(out, this);

	// We KNOW that all children should be written, so don't
	// bother calling shouldWrite()

	// If writing binary format, write out number of children
	// that are going to be written
	if (out->isBinary())
	    out->write(getNumChildren());

	for (int i = 0; i <= lastChild; i++) {
	    action->pushCurPath(i);
	    action->traverse(getChild(i));
	    action->popCurPath(pc);
	}

	// Write post-children stuff
	writeFooter(out);
    }
}
