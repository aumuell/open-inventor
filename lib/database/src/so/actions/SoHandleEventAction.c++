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
 |	SoHandleEventAction
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/events/SoEvent.h>

SO_ACTION_SOURCE(SoHandleEventAction);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor.
//
// Use: public

SoHandleEventAction::SoHandleEventAction(const SbViewportRegion &region)
//
////////////////////////////////////////////////////////////////////////
{
    SO_ACTION_CONSTRUCTOR(SoHandleEventAction);

    event	    = NULL;
    pickRoot	    = NULL;
    pickedPoint	    = NULL;
    pickValid	    = FALSE;
    usedPickAll	    = FALSE;
    pickAct	    = new SoRayPickAction(region);
    eventGrabber    = NULL;
    vpRegion	    = region;

    // Assume that we need to find only the closest object along the
    // ray. If the user requests all objects (by calling
    // getPickedPointList()), we will re-pick after calling
    // setPickAll(TRUE).
    pickAct->setPickAll(FALSE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: public

SoHandleEventAction::~SoHandleEventAction()
//
////////////////////////////////////////////////////////////////////////
{
    if (pickRoot != NULL)
	pickRoot->unref();

    delete pickAct;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Set a node to be the current event grabber, or turn off event
// grabbing.
//
// Use: public

void
SoHandleEventAction::setGrabber(SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
    // inform the previous grabber that it's no longer grabbing
    if (eventGrabber)
	eventGrabber->grabEventsCleanup();

    eventGrabber = node;

    // inform the new node that it's now grabbing
    if (eventGrabber)
	eventGrabber->grabEventsSetup();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the root node used for initiating a pick action for those
//    nodes that want to know who is under the cursor.
//
// Use: public

void
SoHandleEventAction::setPickRoot(SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
    if (node != NULL)
	node->ref();

    if (pickRoot != NULL)
	pickRoot->unref();

    pickRoot = node;

    // Previous pick info is no longer valid
    pickValid = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the object hit (as an SoPickedPoint) by performing a
//    pick based on the current mouse location as specified in the
//    event the action is being applied for. This initiates a pick
//    action if necessary to find this object.
//
// Use: public

const SoPickedPoint *
SoHandleEventAction::getPickedPoint()
//
////////////////////////////////////////////////////////////////////////
{
    const SoPath	*pathAppliedTo;

    // Re-use previous pickedPoint if it's still valid
    if (pickValid)
	return pickedPoint;

    // Otherwise, we have to do the pick...

#ifdef DEBUG
    if (event == NULL) {
	SoDebugError::post("SoHandleEventAction::getPickedPoint",
			   "Event is NULL");
	return NULL;
    }

    if (pickRoot == NULL) {
	SoDebugError::post("SoHandleEventAction::getPickedPoint",
			   "Picking root node is NULL");
	return NULL;
    }
#endif /* DEBUG */

    // Set the pick region, using the raw X position and the windowSize 
    // The radius of the region, in pixels, will be 5.0
    pickAct->setPoint(event->getPosition());
    pickAct->setRadius(5.0);

    // If the action is being applied to a node or pathList, then
    // apply to the pickRoot
    if (getWhatAppliedTo() != PATH)
	pickAct->apply(pickRoot);

    // If the action is applied to a path, use the part of the path
    // from the pickRoot on down. If the pickRoot does not appear in
    // the path being applied to, just apply to the pickRoot node.
    else {

	pathAppliedTo = getPathAppliedTo();

	// If pickRoot is head of path, just apply action to path
	if (pathAppliedTo->getHead() == pickRoot)
	    pickAct->apply( (SoPath *) pathAppliedTo);

	else {
	    int i;

	    // Search for pickRoot in path
	    for (i = 1; i < pathAppliedTo->getLength(); i++)
		if (pathAppliedTo->getNode(i) == pickRoot)
		    break;

	    // If found, construct a path from the pickRoot on down
	    if (i < pathAppliedTo->getLength()) {

		// Copy path starting at node i (the pickRoot)
		SoPath	*newPath = pathAppliedTo->copy(i);
		newPath->ref();

		pickAct->apply(newPath);

		newPath->unref();
	    }

	    // If not found, just apply to root
	    else
		pickAct->apply(pickRoot);
	}
    }

    // The returned hit is the first one in the list
    pickedPoint = pickAct->getPickedPoint();

    pickValid = TRUE;
    usedPickAll = pickAct->isPickAll();

    return pickedPoint;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Return a list of objects hit, sorted from front to back.
//
// Use: public
//
const SoPickedPointList &
SoHandleEventAction::getPickedPointList()
//
////////////////////////////////////////////////////////////////////////
{
    // The user obviously wants all objects along the pick ray, so we
    // have to set the pickAll flag in the pick action.

    // If the last pick we did is valid and already set pickAll to
    // TRUE, then we don't need to repick. Otherwise, we do.
    if (! pickValid || ! usedPickAll) {

	pickAct->setPickAll(TRUE);

	// Make sure the pick always gets done
	pickValid = FALSE;

	// Pick as usual, building the list of picked points
	getPickedPoint();

	// Reset this to FALSE, which is the default value
	pickAct->setPickAll(FALSE);
    }

    return pickAct->getPickedPointList();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initiates action on a graph.
//
// Use: protected

void
SoHandleEventAction::beginTraversal(SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
    setPickRoot(node);

    SoViewportRegionElement::set(state, vpRegion);

    // if a node is grabbing, pass the event directly to it.
    if (eventGrabber != NULL)
	traverse(eventGrabber);

    // if no grabber or if the grabber declined to handle, pass the event
    // to the scene graph.
    if (! isHandled())
	traverse(node);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the viewport region.  This should be called when the window
//    changes size.
//
// Use: public
void
SoHandleEventAction::setViewportRegion(const SbViewportRegion &newRegion)
//
////////////////////////////////////////////////////////////////////////
{
    vpRegion = newRegion;
    pickAct->setViewportRegion(newRegion);
}
