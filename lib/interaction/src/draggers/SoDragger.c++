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
 |	SoDragger
 |
 |   Author(s): Paul Isaacs, David Mott, Howard Look
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <stdio.h>

#include <Inventor/SbLinear.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/SoPath.h>

#include <Inventor/fields/SoSFRotation.h>
#include <Inventor/fields/SoSFVec3f.h>

#include <Inventor/errors/SoDebugError.h>
#include <Inventor/events/SoEvent.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/events/SoLocation2Event.h>
#include <Inventor/events/SoMouseButtonEvent.h>

#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoSearchAction.h>

#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoMatrixTransform.h>

#include <Inventor/misc/SoState.h>
#include <Inventor/misc/SoTempPath.h>
#include <Inventor/misc/SoAuditorList.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/elements/SoViewVolumeElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>

#include <Inventor/draggers/SoDragger.h>

// The smallest scale that any dragger will write. If the user attempts
// to go below this amount, the dragger will set it to this minimum.
// Default is .001
float SoDragger::minScale = .001;

SO_KIT_SOURCE(SoDragger);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
SoDragger::SoDragger()
//
////////////////////////////////////////////////////////////////////////
{
    SO_KIT_CONSTRUCTOR(SoDragger);

    isBuiltIn = TRUE;

    SO_KIT_ADD_CATALOG_ENTRY(motionMatrix, SoMatrixTransform, FALSE, 
				topSeparator, geomSeparator,FALSE);

    // initialize field.
    SO_KIT_ADD_FIELD(isActive,(0));

    SO_KIT_INIT_INSTANCE();

    // init local variables
    startingWorldPoint = SbVec3f(0,0,0);

    // callback lists
    startCallbacks = new SoCallbackList;
    motionCallbacks = new SoCallbackList;
    finishCallbacks = new SoCallbackList;

    valueChangedCallbacks = new SoCallbackList;
    valueChangedCallbacksEnabled = TRUE;
    activeChildDragger = NULL;

    otherEventCallbacks = new SoCallbackList;

    // initialize tempPathToThis, pickPath
    tempPathToThis = NULL;
    tempPathNumKidsHack = NULL;
    pickPath = NULL;

    // initialize surrogatePick info.
    surrogateNameInPickOwner = "";
    pathToSurrogatePickOwner = NULL;
    surrogatePathInPickOwner = NULL;

    // The matrix cache starts as invalid
    cachedPathToThisValid = FALSE;
    cachedMotionMatrixValid = FALSE;

    // By default, do include this dragger in bounding box calculations.
    // This is temporarily overridden when the dragger itself applies
    // a bbox action so that it doesn't include itself.
    ignoreInBbox = FALSE;

    // Minimum amount to move before choosing a constraint based
    // on the user's gesture.
    minGesture = 8; // pixels

    setHandleEventAction( NULL );
    setCameraInfo( NULL );

    // By default, projectors for rotation will call setFront() based
    // on where the mouse went down: on the front or rear of the virtual sphere
    // or cylinder.
    setFrontOnProjector( USE_PICK );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
SoDragger::~SoDragger()
//
////////////////////////////////////////////////////////////////////////
{
    if ( pickPath != NULL )
	 pickPath->unref();

    // tempPaths actually get deleted, not unref'ed
    if ( tempPathToThis != NULL ) {
	delete tempPathToThis;
	tempPathToThis = NULL;
    }
    if ( tempPathNumKidsHack != NULL ) {
	delete tempPathNumKidsHack;
	tempPathNumKidsHack = NULL;
    }

    if ( activeChildDragger )
	activeChildDragger->unref();

    setNoPickedSurrogate();

    delete startCallbacks;
    delete motionCallbacks;
    delete finishCallbacks;

    delete valueChangedCallbacks;

    delete otherEventCallbacks;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This attempts to handle the passed event. It will call methods on
// the SoHandleEventAction if it wants to grab and if it handles the event.
//
// Use: public
//
void
SoDragger::handleEvent(SoHandleEventAction *ha)
//
////////////////////////////////////////////////////////////////////////
{
    // let base class traverse the children
    if ( ha->getGrabber() != this ) 
	SoBaseKit::handleEvent( ha );

    // if no children handled the event, we will!
    if ( ha->isHandled() )
	return;

    setHandleEventAction( ha );

    // get event and window size from the action
    const SoEvent *event = ha->getEvent();

    if (SO_MOUSE_PRESS_EVENT(event, BUTTON1)) {

	SbBool happyPath = FALSE;

	const SoPickedPoint *pp = ha->getPickedPoint();
	SoPath *pPath = ( pp != NULL ) ? pp->getPath() : NULL; 

	if ( pPath ) {

	    // Is our current traversal path part of the pickPath?
	    if ( pPath->containsPath(ha->getCurPath())) {
	        happyPath = TRUE;
	    }
	    // Is the path a surrogate path for us or any draggers 
	    // contained within us?
	    else {
		SoPath *pathToOwner, *surrogatePath;
		SbName  surrogateName;

		if (isPathSurrogateInMySubgraph( pPath, pathToOwner, 
					surrogateName, surrogatePath) ) {
		    pathToOwner->ref();
		    surrogatePath->ref();
		    if (shouldGrabBasedOnSurrogate( pPath, surrogatePath )) {
		        setPickedSurrogate( pathToOwner, surrogateName, 
					    surrogatePath );
		        happyPath = TRUE;
		    }
		    surrogatePath->unref();
		    pathToOwner->unref();
	        }
	    }
	}

	if ( happyPath ) {

	    setStartingPoint( pp );
		
	    // Since the pick path may be on a surrogate object, use
	    // the current action path to get a path to this node.
	    setTempPathToThis( ha->getCurPath() );
	    setCameraInfo( ha );

	    setPickPath( pPath );

	    ha->setGrabber(this);
	    ha->setHandled();
	}		    
	else 
	    otherEventCallbacks->invokeCallbacks(this);
    }
    else if ( event->isOfType(SoLocation2Event::getClassTypeId() ) && 
    	      ha->getGrabber() == this ) {

	    mouseMovedYet = TRUE;
	    motionCallbacks->invokeCallbacks(this);

	    ha->setHandled();
    }
    else if ( SO_MOUSE_RELEASE_EVENT(event, BUTTON1) &&
              ha->getGrabber() == this ) {

	    // Releasing the grabber will result in a call to 
	    // grabEventsCleanup(), which will do some important things...
	    ha->releaseGrabber();

	    if ( mouseMovedYet == TRUE ) {
		// If the mouse didn't move, then don't handle the event.
		// Let some other node make use of it (for example, the 
		// selection node might want to de-select what's inside
		// the dragger.
		ha->setHandled();
	    }
    }
    else
	otherEventCallbacks->invokeCallbacks(this);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This virtual function is called when a dragger gains
//    status as "grabber" of events.
//
// Use: public
//
void
SoDragger::grabEventsSetup()
//
////////////////////////////////////////////////////////////////////////
{
    renderCaching = OFF;

    setStartLocaterPosition( getLocaterPosition() );
    saveStartParameters();

    mouseMovedYet = FALSE;

    isActive.setValue( TRUE );
    startCallbacks->invokeCallbacks(this);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This virtual function is called when a dragger loses
//    status as "grabber" of events.
//
// Use: public
//
void
SoDragger::grabEventsCleanup()
//
////////////////////////////////////////////////////////////////////////
{
    isActive.setValue( FALSE );
    finishCallbacks->invokeCallbacks(this);

    setPickPath( NULL );
    setNoPickedSurrogate();

    renderCaching = AUTO;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  Returns the event carried by the current handleEventAction,
//  if any.
//
// Use: protected
//
const SoPath *
SoDragger::getPickPath() const
//
////////////////////////////////////////////////////////////////////////
{
    const SoDragger *subDragger = getActiveChildDragger();
    if (subDragger != NULL)
	return subDragger->getPickPath();
    else
	return pickPath;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  Returns the event carried by the current handleEventAction,
//  if any.
//
// Use: protected
//
const SoEvent *
SoDragger::getEvent() const
//
////////////////////////////////////////////////////////////////////////
{
    if ( getHandleEventAction()  == NULL ) {
#ifdef DEBUG
	SoDebugError::post("SoDragger::getEvent", "HandleEvent action is NULL");
#endif
	return NULL;
    }

    return ( getHandleEventAction()->getEvent() );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  Returns the point in space that begins the dragging gesture.
//  Usually this is just the picked point, but in special
//  cases, the point is set without picking. 
//  For example, when a modifier key goes down, a dragger will often
//  start a new drag gesture without doing a pick. Instead, the previously
//  used point will be chosen to start a new gesture.
//
// Use: protected
//
SbVec3f
SoDragger::getWorldStartingPoint()
//
////////////////////////////////////////////////////////////////////////
{
    return startingWorldPoint;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  Transforms the starting point into local space and returns it.
//
// Use: protected
//
SbVec3f
SoDragger::getLocalStartingPoint()
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f thePoint = getWorldStartingPoint();

    getWorldToLocalMatrix().multVecMatrix(thePoint, thePoint);
    return thePoint;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This is called to get the position of the locater in 
//    normalized (0.0 to 1.0) screen space.  (0,0) is lower left
//    the event. 
//
// Use: protected
//
SbVec2f
SoDragger::getNormalizedLocaterPosition()
//
////////////////////////////////////////////////////////////////////////
{
    return( getEvent()->getNormalizedPosition( getViewportRegion() ));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This is called to get the position of the locater in 
//    non-normalized screen space.  (0,0) is lower left
//    the event. 
//
// Use: protected
//
SbVec2s
SoDragger::getLocaterPosition()
//
////////////////////////////////////////////////////////////////////////
{
    return( getEvent()->getPosition( getViewportRegion()) );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  Returns whether the locater has moved enough to assume a
//  constraint gesture.
//
// Use: private
//
SbBool
SoDragger::isAdequateConstraintMotion()
//
////////////////////////////////////////////////////////////////////////
{
    SbVec2s moved = (getStartLocaterPosition() - getLocaterPosition());
    short lengthSquared = moved[0]*moved[0] + moved[1]*moved[1];
    return (SbBool) (lengthSquared >= minGesture*minGesture);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This sets the viewport and view volume information 
//    explicitly to given values.
//
// Use: protected
//
void
SoDragger::setCameraInfo(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // If NULL, use default values
    if (action == NULL ) {

	viewVolume.ortho(-1, 1, -1, 1, 1, 10);
	vpRegion = SbViewportRegion(1,1);
    }
    // If we've got an action...
    else {
	// Get the viewport and viewVolume...
	SoState *state = action->getState();
	viewVolume = SoViewVolumeElement::get( state );
	vpRegion   = SoViewportRegionElement::get(state);
    }

    // Here's a chance to set up a good tempPathToThis if for some reason
    // we don't already have one.
    SoPath *pathToMe = createPathToThis();
    if (pathToMe)  {
	// We've can create a path. Just get rid of the one it gave us.
	pathToMe->ref();
	pathToMe->unref();
    }
    else if (action != NULL )
	setTempPathToThis( action->getCurPath() );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This sets the handleEventAction
//
// Use: protected
//
void
SoDragger::setHandleEventAction( SoHandleEventAction *newAction )
//
////////////////////////////////////////////////////////////////////////
{
    handleEventAction = newAction;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//
// Use: private
//
void
SoDragger::setTempPathToThis( const SoPath *somethingClose )
//
////////////////////////////////////////////////////////////////////////
{
    // Check that we're not doing something unnecessary...
    // If paths don't fork until 'this', we can keep the path we've got.
    // But even if we've already got an okay path, we should invalidate
    // here, since some matrix has probably changed.
    if ( tempPathToThis != NULL && somethingClose != NULL ) {
	int forkInd = tempPathToThis->findFork( somethingClose );
	if ( forkInd == (tempPathToThis->getLength() - 1)) {
	    cachedPathToThisValid	    = FALSE;
	    return;
        }
    }

    // We can not keep our old tempPathToThis. Delete it now. 
    // ( you delete tempPaths, not ref/unref)
    if (tempPathToThis != NULL) {
	delete tempPathToThis;
	tempPathToThis = NULL;
    }
    if (tempPathNumKidsHack != NULL) {
	delete tempPathNumKidsHack;
	tempPathNumKidsHack = NULL;
    }

    // Figure out tempPathToThis from somethingClose, which may 
    // not be exactly what we need...
    if ( somethingClose != NULL && somethingClose->containsNode(this) ) {
	// Create tempPathToThis. Copy from something close.
	tempPathToThis 
	    = createTempPathFromFullPath( (const SoFullPath *)somethingClose );

	// Then pop off  'til the you get to this.
	for (SoNode *n = tempPathToThis->getTail(); n != this; ) {
	    tempPathToThis->pop();
	    n = tempPathToThis->getTail();
	}
    }
    else if ( somethingClose != NULL && somethingClose->getLength() > 0 ) {
	// Start at the root and search for 'this' underneath.
	// Turn on nodekit searching for the duration...
	static SoSearchAction *sa = NULL;
	if (sa == NULL)
	    sa = new SoSearchAction;
	else
	    sa->reset();

	SbBool oldBool = SoBaseKit::isSearchingChildren();
	SoBaseKit::setSearchingChildren( TRUE );
	    sa->setNode( this );
	    sa->apply( somethingClose->getHead() );
	SoBaseKit::setSearchingChildren( oldBool );
	if ( sa->getPath() != NULL )
	    tempPathToThis 
		= createTempPathFromFullPath((const SoFullPath *)sa->getPath());
    }

    // Create the tempPathNumKidsHack.  This is an sbPlist that records
    // how many children are under each node along the path.
    // We use it later to reconstruct the path if the parent/child pairs
    // are still okay, but the indices in the path have changed.
    if ( tempPathToThis != NULL ) {
	SoNode *pathNode;

	tempPathNumKidsHack = new SbPList( tempPathToThis->getLength() );

	for ( int i = 0; i < tempPathToThis->getLength(); i++ ) {
	    pathNode = tempPathToThis->getNode(i);
	    if ( !pathNode  || !pathNode->getChildren() )
		tempPathNumKidsHack->append( (void *) 0 );
	    else
		tempPathNumKidsHack->append( 
			    (void *) (unsigned long) pathNode->getChildren()->getLength() );
	}
    }

    // In both cases, the cachedPath has changed...
    cachedPathToThisValid	    = FALSE;
}

SoPath *
SoDragger::createPathToThis() 
{
    if ( isTempPathToThisOk() )
	return tempPathToThis->copy();
    else
	return NULL;
}

SbBool
SoDragger::isTempPathToThisOk()
{
    // ??? Hack alert! ???
    //     This whole method is a hack!
    // 

    if ( tempPathToThis == NULL || tempPathNumKidsHack == NULL)
	return FALSE;

    SbBool isOkay = TRUE;

    SoNode *actualNode = NULL;
    SoNode *nodeInPath = NULL;
    
    for(int numFmTail = 0; numFmTail < tempPathToThis->getLength();numFmTail++){

	int numFmHead = tempPathToThis->getLength() - numFmTail -1;
	nodeInPath    = tempPathToThis->getNode( numFmHead );

	if (numFmTail == 0) {
	    if (this != nodeInPath) {
		isOkay = FALSE;
		break;
	    }
	}
	else {
	    // actual node is still node from last time.
	    // See if you can find 'nodeInPath' as a parent in the
	    // auditor list of actualNode.
	    const SoAuditorList aList = actualNode->getAuditors();
	    if ( aList.find( nodeInPath, SoNotRec::PARENT ) == -1 ) {
		isOkay = FALSE;
		break;
	    }
	    else {
		// Since this is a temp path, it does not get notified 
		// when children are added or deleted. So we should
		// check and adjust the indices in the path if they have
		// gotten screwed up...

		// Make sure that the
		// index is correct for this parent/child pair.
		SoNode *parent = nodeInPath;
		SoNode *child  = actualNode;

		SoChildList *children = parent->getChildren();
		if (children == NULL) {
		    isOkay = FALSE;
		    break;
		}

		int indexInPath = tempPathToThis->getIndex(numFmHead+1);

		int numKidsNow    = children->getLength();
#if (_MIPS_SZPTR == 64 || __ia64)
		int numKidsBefore = (int) ((long) (*tempPathNumKidsHack)[numFmHead]);
#else
		int numKidsBefore = (int) (*tempPathNumKidsHack)[numFmHead];
#endif

		// To be correct, the childNode has to be the correct numbered
		// child under the parent, and the parent should still
		// have the same number of children it did before. 
		if (   (numKidsNow != numKidsBefore)
		    || (numKidsNow <= indexInPath)
		    || ((*children)[indexInPath] != child) ) {

		    // We have a problem. Try to fix it.

		    // First, figure out where it is most likely to belong.
		    // Also, update the tempPathNumKidsHack
		    int bestSpot = indexInPath;
		    if ( numKidsNow != numKidsBefore ) {
			// update the number of kids.
			tempPathNumKidsHack->remove(numFmHead);
			tempPathNumKidsHack->insert( (void *) (unsigned long) numKidsNow,
						    numFmHead);

			bestSpot = indexInPath + (numKidsNow - numKidsBefore);
		    }

		    // Look forward and back from bestSpot and try to find the
		    // node as a child. See which way we come to the node first.
		    // (we have to do this because of instancing.)
		    int early;
		    int late;
		    SbBool newSpot = -1;
		    for ( early = bestSpot, late = bestSpot; 
			  early >= 0 || late < numKidsNow;
			  early--, late++ ) {
			if (early >= 0 && early < numKidsNow &&
			    (child == (*children)[early])) {
				newSpot = early;
				break;
			}
			if (late >= 0 && late < numKidsNow &&
			    (child == (*children)[late])) {
				newSpot = late;
				break;
			}
		    }

		    if ( newSpot == -1 ) {
			isOkay = FALSE;
			break;
		    }

		    if ( newSpot != indexInPath ) {

			// We need to alter the path so we increment or
			// decrement the index.

			// Increments or decrements index of the child, since
			// some children may have been added or deleted.
			if (newSpot < indexInPath) {
			    for (int j = newSpot; j < indexInPath; j++ )
				tempPathToThis->removeIndex(nodeInPath, 0 );
			}
			else if (newSpot > indexInPath) {
			    for (int j = newSpot; j > indexInPath; j-- )
				tempPathToThis->insertIndex(nodeInPath, 0 );
			}
		    }
		}
	    }
	}
	actualNode = nodeInPath;
    }

    if (isOkay == FALSE) {
	if (tempPathToThis != NULL) {
	    delete tempPathToThis;
	    tempPathToThis = NULL;
	}
	if (tempPathNumKidsHack != NULL) {
	    delete tempPathNumKidsHack;
	    tempPathNumKidsHack = NULL;
	}
    }

    return isOkay;
}

SoTempPath *
SoDragger::createTempPathFromFullPath( const SoFullPath *fp ) const
{
    SoTempPath *answer = new SoTempPath( fp->getLength() );

    for ( int i = 0; i < fp->getLength(); i++ )
	answer->append( fp->getNode(i) );

    return answer;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the pickPath to be the given path.
//
void
SoDragger::setPickPath( SoPath *newPickPath )
//
////////////////////////////////////////////////////////////////////////
{
    // Set the pickPath. This should just equal the input path.
    // ref the input before unref'ing the old path, in case they're the same
	if ( newPickPath != NULL )
	    newPickPath->ref();

	// get rid of old path if it exists
	if ( pickPath != NULL ) {
	    pickPath->unref();
	    pickPath = NULL;
	}
	// add new path if it exists
	if ( newPickPath != NULL )
	    pickPath = (SoPath *) newPickPath;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the surrogatePick information to initial values.
//
void
SoDragger::setNoPickedSurrogate()
//
////////////////////////////////////////////////////////////////////////
{
    // Set the name to be empty
    surrogateNameInPickOwner = "";

    // get rid of old paths if they exist.
    if ( pathToSurrogatePickOwner != NULL ) {
	 pathToSurrogatePickOwner->unref();
	 pathToSurrogatePickOwner = NULL;
    }
    if ( surrogatePathInPickOwner != NULL ) {
	 surrogatePathInPickOwner->unref();
	 surrogatePathInPickOwner = NULL;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//
SbBool
SoDragger::shouldGrabBasedOnSurrogate( const SoPath *pickPath, 
				const SoPath *surrogatePath )
//
////////////////////////////////////////////////////////////////////////
{
    // The pickPath must contain the surrogatePath
    if ( pickPath->containsPath( surrogatePath ) == FALSE )
	return FALSE;

    const SoFullPath *fullPick = (const SoFullPath *) pickPath;
    const SoFullPath *fullSurr = (const SoFullPath *) surrogatePath;

    // Find the tail of surrogatePath.
    SoNode *surrTail = fullSurr->getTail();

    // Go from the tail of pickPath backwards.
    // If you find a dragger before you find surrTail, return FALSE.
    // Otherwise, return TRUE.
    SoNode *pickNode;
    for (int i = fullPick->getLength() - 1; i >= 0; i-- ) {
	pickNode = fullPick->getNode(i);
	if (pickNode == surrTail)
	    return TRUE;
	if (pickNode->isOfType( SoDragger::getClassTypeId() ))
	    return FALSE;
    }
    // Should never get here...
    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the surrogatePick information.
//
void
SoDragger::setPickedSurrogate( SoPath *pathToOwner, SbName &nameUsedByOwner,
			       SoPath *pathUsedByOwner )
//
////////////////////////////////////////////////////////////////////////
{
    // Set the pathToSurrogatePickOwner. 
	if ( pathToOwner != pathToSurrogatePickOwner ) {
	    if ( pathToOwner != NULL )
		pathToOwner->ref();

	    if ( pathToSurrogatePickOwner != NULL )
		 pathToSurrogatePickOwner->unref();

	    pathToSurrogatePickOwner = pathToOwner;
	}

    // Set the surrogatePathInPickOwner. 
	if ( pathUsedByOwner != surrogatePathInPickOwner ) {
	    if ( pathUsedByOwner != NULL )
		pathUsedByOwner->ref();

	    if ( surrogatePathInPickOwner != NULL )
		 surrogatePathInPickOwner->unref();

	    surrogatePathInPickOwner = pathUsedByOwner;
	}

    // Set the name...
	surrogateNameInPickOwner = nameUsedByOwner;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the point in space where dragger is to begin.
//    The pickedPoint version would usually be the result of a pick.
//    Assumes that 'newPoint' is in world space.
//
void
SoDragger::setStartingPoint( const SoPickedPoint *newPoint )
//
////////////////////////////////////////////////////////////////////////
{
    startingWorldPoint = newPoint->getPoint();
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the point in space where dragger is to begin.
//    The SbVec3f version would usually be a piont saved from the end of 
//    another gesture. For example, when a modifier key goes down, we might
//    save the current position and use it to begin another connected gesture.
//    Assumes that 'newPoint' is in world space.
//
void
SoDragger::setStartingPoint( const SbVec3f &newPoint )
//
////////////////////////////////////////////////////////////////////////
{
    startingWorldPoint = newPoint;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This implements the SoGetBoundingBoxAction.
//
// Use: protected
//
void
SoDragger::getBoundingBox(SoGetBoundingBoxAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    if ( ! ignoreInBbox )
	SoBaseKit::getBoundingBox( action );
}

const SbMatrix &
SoDragger::getMotionMatrix()
//
////////////////////////////////////////////////////////////////////////
{
    // Try the fast way to access this node first...
    SoMatrixTransform *mm = (SoMatrixTransform *) motionMatrix.getValue();

    // If that fails, then make a new part...
    if (mm == NULL)
	mm = (SoMatrixTransform *) getAnyPart("motionMatrix", TRUE);

    return ( mm->matrix.getValue() );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the motion matrix to be an explicit matrix. Triggers value
//    changed callbacks, but only if (newMatrix != the current motionMatrix)
//
void
SoDragger::setMotionMatrix( const SbMatrix &newMatrix )
//
////////////////////////////////////////////////////////////////////////
{
    // Return if no change...
	if ( getMotionMatrix() == newMatrix )
	    return;

    // Set motion matrix (the field will be non-null, 
    // since 'getMotionMatrix()' was just called
	((SoMatrixTransform *)motionMatrix.getValue())->matrix = newMatrix;

    // We'll need to recalculate the conversion matrices.
	cachedMotionMatrixValid = FALSE;

    // Invokes the value changed callbacks
	valueChanged();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//     Invokes the valueChangedCallbacks, if they are enabled.
//     These are invoked whenever setMotionMatrix() changes the motion matrix.
//     If a subclass wishes to invoke the valueChanged callbacks for some
//     other reason, they may call valueChanged(). Example: SoSpotLightDragger
//     changes its angle field without altering the motionMatrix. So it
//     calls valueChanged() to invoke callbacks.
//
void
SoDragger::valueChanged()
//
////////////////////////////////////////////////////////////////////////
{
    if (valueChangedCallbacksEnabled == TRUE)
        valueChangedCallbacks->invokeCallbacks(this);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Saves the motionMatrix as the startMotionMatrix.
//    The startMotionMatrix is used to remember where the dragger was
//    at the beginning of dragging.
//
void
SoDragger::saveStartParameters()
//
////////////////////////////////////////////////////////////////////////
{
    startMotionMatrix = getMotionMatrix();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//     This sets the 'activeChildDragger.'
//     Called by childStartCB(), childFinishCB(),
//     childValueChangedCB(), etc.
//     Establishes which dragger is the child, information
//     needed by the parent in order to do what it needs to do.
//
void
SoDragger::setActiveChildDragger( SoDragger *newChildDragger )
//
////////////////////////////////////////////////////////////////////////
{
    if (newChildDragger)
	newChildDragger->ref();

    if (activeChildDragger)
	activeChildDragger->unref();

    activeChildDragger = newChildDragger;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//     This function is invoked by child draggers 
//     when they change their value.
//
void 
SoDragger::transferMotion( SoDragger *childDragger)
//
////////////////////////////////////////////////////////////////////////
{
    // Get the motion matrix from the child
	SbMatrix childMotion = childDragger->getMotionMatrix();

    // There's a lot we don't need to bother with if the childMotion is
    // identity...
	SbBool childIdent = ( childMotion == SbMatrix::identity() );
    
    // Return if childMotion is identity and our motionMatrix already 
    // matches our saved startMatrix.
	if ( childIdent && (getMotionMatrix() == getStartMotionMatrix()))
	    return;

	if ( !childIdent ) {
	    // First, set the childDragger matrix to identity.
	    childDragger->setMotionMatrix( SbMatrix::identity() );

	    // Convert the childMotion from child LOCAL space to  world space.
	    childDragger->transformMatrixLocalToWorld(childMotion,childMotion);

	    // Convert the childMotion from world space to our LOCAL space.
	    transformMatrixWorldToLocal(childMotion,childMotion);
	}

    // Append this transformed child motion to our saved start matrix.
	SbMatrix newMotion = getStartMotionMatrix();
	if ( !childIdent )
	    newMotion.multLeft( childMotion );

	setMotionMatrix( newMotion );

    // Changing the parent matrix invalidates the matrix cache of the
    // childDragger
	childDragger->cachedPathToThisValid = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Use: private
//
void
SoDragger::validateMatrices()
//
////////////////////////////////////////////////////////////////////////
{
    // If both aspects of the matrices are still okay, then we can
    // continue to use the cached values.
    if ( cachedPathToThisValid && cachedMotionMatrixValid )
	return;

    // If the tempPathToThis is no longer valid, then we've got to run a 
    // getMatrix action to find the preMotionToWorld and 
    // worldToPreMotion matrices.
    if ( cachedPathToThisValid == FALSE ) {

	// Do a get matrix action from world space to this node.
	SoPath *pathToMe = createPathToThis();
	if ( pathToMe != NULL ) {
	    pathToMe->ref();
	    static SoGetMatrixAction *ma = NULL;
	    if (ma == NULL)
		ma = new SoGetMatrixAction( getViewportRegion() );
	    else
		ma->setViewportRegion( getViewportRegion() );

	    ma->apply( pathToMe );
	    preMotionToWorldMatrix = ma->getMatrix();
	    worldToPreMotionMatrix = ma->getInverse();
	    pathToMe->unref();
	}
	else {
	    preMotionToWorldMatrix.makeIdentity();
	    worldToPreMotionMatrix.makeIdentity();
	}
    }

    // If the cachedMotionMatrix is no longer valid, then figure 'em out...
    if ( cachedMotionMatrixValid == FALSE ) {
	cachedMotionMatrix = getMotionMatrix();
    }

    // Last, we append the motion matrix to the preMotion matrix to get
    // the postMotion matrix.
    postMotionToWorldMatrix = preMotionToWorldMatrix;
    postMotionToWorldMatrix.multLeft( cachedMotionMatrix );

    worldToPostMotionMatrix = worldToPreMotionMatrix;
    worldToPostMotionMatrix.multRight( cachedMotionMatrix.inverse() );

    cachedMotionMatrixValid = TRUE;
    cachedPathToThisValid = TRUE;
}
    
    
////////////////////////////////////////////////////////////////////////
//
// Use: protected
//
SbMatrix
SoDragger::getLocalToWorldMatrix()
//
////////////////////////////////////////////////////////////////////////
{
    // Make sure everything is kosher
    validateMatrices();

    return postMotionToWorldMatrix;
}

////////////////////////////////////////////////////////////////////////
//
// Use: protected
//
SbMatrix
SoDragger::getWorldToLocalMatrix()
//
////////////////////////////////////////////////////////////////////////
{
    // Make sure everything is kosher
    validateMatrices();

    return worldToPostMotionMatrix;
}

////////////////////////////////////////////////////////////////////////
//
// Use: protected
//
void
SoDragger::transformMatrixLocalToWorld( const SbMatrix   &fromMatrix, 
			                  SbMatrix         &toMatrix)
//
////////////////////////////////////////////////////////////////////////
{
    toMatrix = fromMatrix;

    SbMatrix forward = getLocalToWorldMatrix();
    SbMatrix backward = getWorldToLocalMatrix();

    toMatrix.multRight( forward );
    toMatrix.multLeft( backward );
}

////////////////////////////////////////////////////////////////////////
//
// Use: protected
//
void
SoDragger::transformMatrixWorldToLocal( const SbMatrix   &fromMatrix, 
			                  SbMatrix         &toMatrix)
//
////////////////////////////////////////////////////////////////////////
{
    toMatrix = fromMatrix;

    SbMatrix forward = getWorldToLocalMatrix();
    SbMatrix backward = getLocalToWorldMatrix();

    toMatrix.multRight( forward );
    toMatrix.multLeft( backward );
}


////////////////////////////////////////////////////////////////////////
//
// Use: EXTENDER public
//
// Description:
// Get the matrix which converts from the space of one part into
// local space. Good to use if transforms occur between 'motionMatrix'
// and the space you want to work in.
// Note: This routine will try not to create parts that don't exist.
//       Instead it finds the existing part that precedes it in traversal.
//       But this only works if the partName is in this nodekit's catalog.
//       If the part is nested within another kit below this one or 
//       sitting inside a list part, the part will be created when it 
//       doesn't exist.
//
void 
SoDragger::getPartToLocalMatrix( const SbName &partName, 
	    SbMatrix &partToLocalMatrix, SbMatrix &localToPartMatrix)
//
////////////////////////////////////////////////////////////////////////
{
    // We need to temporarily ref ourself, since we build paths
    // and stuff...
    ref();

    SoPath *pathToMe = createPathToThis();
    if (pathToMe)
	pathToMe->ref();

    SoPath *pathToPart;

    // We want to figure this out without creating any parts unnecessarily.
    // So, instead of forcing creation, do a check.
        pathToPart = createPathToAnyPart(partName,FALSE,FALSE,FALSE,pathToMe);

    // If we didn't find a path that already exists:
	if (pathToPart == NULL) {
    	    const SoNodekitCatalog *cat = getNodekitCatalog();
	    int pNum = cat->getPartNumber(partName);
	    if ( pNum != SO_CATALOG_NAME_NOT_FOUND ) {

		// If it fails and the part is in this catalog, then traverse 
		// backwards until we can find the last part before partName 
		// and after "motionMatrix" If we hit "motionMatrix", or 
		// "this", then just set path to NULL
		int thisPnum   = cat->getPartNumber("this");
		int motMatPnum = cat->getPartNumber("motionMatrix");

		while ((pathToPart == NULL) &&
		       (pNum != thisPnum) && (pNum != motMatPnum)) 
		{
		    // Find left sibling or parent.  Can check 'em together
		    // since left sibling never precedes parent.
		    for (int i = pNum-1; i >= 0; i-- ) {
			if ((cat->getRightSiblingPartNumber(i) == pNum) ||
			    (cat->getParentPartNumber(pNum) == i)) {
			    pNum = i;
			    break;
			}
		    }
		    if ( pNum != thisPnum && pNum != motMatPnum ) {
			pathToPart = createPathToAnyPart(cat->getName(pNum),
				FALSE, FALSE, FALSE, pathToMe);
		    }
		}

	    }
	    else {
		// If the part doesn't exist yet and the partName
		// is not in this catalog, we're sort of stuck. Force 
		// it and create the part anyway. (2nd arg == TRUE)
		pathToPart = createPathToAnyPart(partName,TRUE,
						 FALSE,FALSE,pathToMe);
	    }
	}

    // We don't need this path anymore.
    if (pathToMe)
	pathToMe->unref();

    if ( pathToPart == NULL ) {
	partToLocalMatrix = localToPartMatrix = SbMatrix::identity();
        // Undo temporary ref on ourself.
	unrefNoDelete();
	return;
    }

    pathToPart->ref();

    static SoGetMatrixAction *ma = NULL;
    if (ma == NULL)
	ma = new SoGetMatrixAction( getViewportRegion() );
    else
	ma->setViewportRegion( getViewportRegion() );

    ma->apply( pathToPart );
    SbMatrix partToWorld = ma->getMatrix();
    SbMatrix worldToPart = ma->getInverse();

    pathToPart->unref();

    partToLocalMatrix = partToWorld;
    partToLocalMatrix.multRight( getWorldToLocalMatrix() );

    localToPartMatrix = getLocalToWorldMatrix();
    localToPartMatrix.multRight( worldToPart);

    // Undo temporary ref on ourself.
    unrefNoDelete();
}

void 
SoDragger::transformMatrixToLocalSpace( const SbMatrix &fromMatrix,
	    SbMatrix &toMatrix, const SbName &fromSpacePartName)
{
    SbMatrix fromToLocalM, localToFromM;

    getPartToLocalMatrix( fromSpacePartName, fromToLocalM, localToFromM);

    toMatrix = fromMatrix;
    toMatrix.multRight( fromToLocalM );
    toMatrix.multLeft( localToFromM );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Simple functions for adding to and removing from
//    the different callback lists.
//
////////////////////////////////////////////////////////////////////////
/////////
// START
/////////
void SoDragger::addStartCallback( SoDraggerCB *f, void *d )
{  startCallbacks->addCallback( (SoCallbackListCB *)f, d ); }

void SoDragger::removeStartCallback( SoDraggerCB *f, void *d )
{  startCallbacks->removeCallback( (SoCallbackListCB *)f, d ); }

/////////
// MOTION
/////////
void SoDragger::addMotionCallback( SoDraggerCB *f, void *d )
{  motionCallbacks->addCallback( (SoCallbackListCB *)f, d ); }

void SoDragger::removeMotionCallback( SoDraggerCB *f, void *d )
{  motionCallbacks->removeCallback( (SoCallbackListCB *)f, d ); }

/////////
// DONE
/////////
void SoDragger::addFinishCallback( SoDraggerCB *f, void *d )
{  finishCallbacks->addCallback( (SoCallbackListCB *)f, d ); }

void SoDragger::removeFinishCallback( SoDraggerCB *f, void *d )
{  finishCallbacks->removeCallback( (SoCallbackListCB *)f, d ); }

/////////
// VALUECHANGED
/////////
void SoDragger::addValueChangedCallback( SoDraggerCB *f, void *d )
{  valueChangedCallbacks->addCallback( (SoCallbackListCB *)f, d ); }

void SoDragger::removeValueChangedCallback( SoDraggerCB *f, void *d )
{  valueChangedCallbacks->removeCallback( (SoCallbackListCB *)f, d ); }

SbBool SoDragger::enableValueChangedCallbacks( SbBool newVal )
{
    SbBool oldVal = valueChangedCallbacksEnabled;
    valueChangedCallbacksEnabled = newVal;
    return oldVal;
}

/////////
// OTHEREVENT
/////////
void SoDragger::addOtherEventCallback( SoDraggerCB *f, void *d )
{  otherEventCallbacks->addCallback( (SoCallbackListCB *)f, d ); }

void SoDragger::removeOtherEventCallback( SoDraggerCB *f, void *d )
{  otherEventCallbacks->removeCallback( (SoCallbackListCB *)f, d ); }


void SoDragger::registerChildDragger(SoDragger *child)
{
    // This calls transferMotion, followed by the callbacks for this node.
    child->addValueChangedCallback( 
			SoDragger::childTransferMotionAndValueChangedCB, this );

    child->addStartCallback(SoDragger::childStartCB, this);
    child->addMotionCallback(SoDragger::childMotionCB, this);
    child->addFinishCallback(SoDragger::childFinishCB, this);
    child->addOtherEventCallback(SoDragger::childOtherEventCB, this);
}

void SoDragger::unregisterChildDragger(SoDragger *child)
{
    child->removeValueChangedCallback( 
			SoDragger::childTransferMotionAndValueChangedCB, this );

    child->removeStartCallback(SoDragger::childStartCB, this);
    child->removeMotionCallback(SoDragger::childMotionCB, this);
    child->removeFinishCallback(SoDragger::childFinishCB, this);
    child->removeOtherEventCallback(SoDragger::childOtherEventCB, this);
}

void SoDragger::registerChildDraggerMovingIndependently(SoDragger *child)
{
    child->addValueChangedCallback( SoDragger::childValueChangedCB, this );

    child->addStartCallback(SoDragger::childStartCB, this);
    child->addMotionCallback(SoDragger::childMotionCB, this);
    child->addFinishCallback(SoDragger::childFinishCB, this);
    child->addOtherEventCallback(SoDragger::childOtherEventCB, this);
}

void SoDragger::unregisterChildDraggerMovingIndependently(SoDragger *child)
{
    child->removeValueChangedCallback( SoDragger::childValueChangedCB, this );

    child->removeStartCallback(SoDragger::childStartCB, this);
    child->removeMotionCallback(SoDragger::childMotionCB, this);
    child->removeFinishCallback(SoDragger::childFinishCB, this);
    child->removeOtherEventCallback(SoDragger::childOtherEventCB, this);
}

void SoDragger::childTransferMotionAndValueChangedCB(void      *parentAsVoid, 
						     SoDragger *childDragger)
{
    SoDragger *parent = (SoDragger *) parentAsVoid;

    SoDragger *savedChild = parent->getActiveChildDragger();
    if (savedChild) savedChild->ref();
    parent->setActiveChildDragger( childDragger );
	// Save these variables to we can put 'em back when we're done.
	SoHandleEventAction *oldHa  = parent->getHandleEventAction();
	SbViewVolume         oldVV  = parent->getViewVolume();
	SbViewportRegion     oldVPR = parent->getViewportRegion();

	parent->setHandleEventAction(childDragger->getHandleEventAction());
	parent->setViewVolume(childDragger->getViewVolume());
	parent->setViewportRegion(childDragger->getViewportRegion());

	SoPath *pathToKid = childDragger->createPathToThis();
	if (pathToKid) pathToKid->ref();
	parent->setTempPathToThis( pathToKid );
	if (pathToKid) pathToKid->unref();

	// Before calling the other valueChanged callbacks, transfer the
	// motion of the childDragger into our own motion matrix.
	    // We do not want to trigger any of our other valueChanged callbacks
	    // while this is being done...
	    SbBool saveEnabled = parent->enableValueChangedCallbacks( FALSE );
	    parent->transferMotion( childDragger );
	    parent->enableValueChangedCallbacks( saveEnabled );

	parent->valueChanged();
    parent->setActiveChildDragger( savedChild );

	// Restore saved values of our variables
	parent->setHandleEventAction(oldHa);
	parent->setViewVolume(oldVV);
	parent->setViewportRegion(oldVPR);

    if (savedChild) savedChild->unref();
}

void SoDragger::childValueChangedCB(void *parentAsVoid, SoDragger *childDragger)
{
    SoDragger *parent = (SoDragger *) parentAsVoid;

    SoDragger *savedChild = parent->getActiveChildDragger();
    if (savedChild) savedChild->ref();
    parent->setActiveChildDragger( childDragger );
	// Save these variables to we can put 'em back when we're done.
	SoHandleEventAction *oldHa  = parent->getHandleEventAction();
	SbViewVolume         oldVV  = parent->getViewVolume();
	SbViewportRegion     oldVPR = parent->getViewportRegion();

	parent->setHandleEventAction(childDragger->getHandleEventAction());
	parent->setViewVolume(childDragger->getViewVolume());
	parent->setViewportRegion(childDragger->getViewportRegion());

	SoPath *pathToKid = childDragger->createPathToThis();
	if (pathToKid) pathToKid->ref();
	parent->setTempPathToThis( pathToKid );
	if (pathToKid) pathToKid->unref();

	parent->valueChanged();
    parent->setActiveChildDragger( savedChild );
	// Restore saved values of our variables
	parent->setHandleEventAction(oldHa);
	parent->setViewVolume(oldVV);
	parent->setViewportRegion(oldVPR);

    if (savedChild) savedChild->unref();
}

void SoDragger::childStartCB(void *parentAsVoid, SoDragger *childDragger )
{
    SoDragger *parent = (SoDragger *) parentAsVoid;

    SoDragger *savedChild = parent->getActiveChildDragger();
    if (savedChild) savedChild->ref();
    parent->setActiveChildDragger( childDragger );
	parent->saveStartParameters();

	// Save these variables to we can put 'em back when we're done.
	SoHandleEventAction *oldHa  = parent->getHandleEventAction();
	SbViewVolume         oldVV  = parent->getViewVolume();
	SbViewportRegion     oldVPR = parent->getViewportRegion();

	parent->setHandleEventAction(childDragger->getHandleEventAction());
	parent->setViewVolume(childDragger->getViewVolume());
	parent->setViewportRegion(childDragger->getViewportRegion());

	SoPath *pathToKid = childDragger->createPathToThis();
	if (pathToKid) pathToKid->ref();
	parent->setTempPathToThis( pathToKid );
	if (pathToKid) pathToKid->unref();

        parent->setStartingPoint( childDragger->getWorldStartingPoint() );

	// While the child is manipulating, we should not bother caching here.
	parent->renderCaching = OFF;
	parent->startCallbacks->invokeCallbacks(parent);
    parent->setActiveChildDragger( savedChild );
	// Restore saved values of our variables
	parent->setHandleEventAction(oldHa);
	parent->setViewVolume(oldVV);
	parent->setViewportRegion(oldVPR);

    if (savedChild) savedChild->unref();
}

void SoDragger::childMotionCB(void *parentAsVoid, SoDragger *childDragger )
{
    SoDragger *parent = (SoDragger *) parentAsVoid;

    SoDragger *savedChild = parent->getActiveChildDragger();
    if (savedChild) savedChild->ref();
    parent->setActiveChildDragger( childDragger );
	// Save these variables to we can put 'em back when we're done.
	SoHandleEventAction *oldHa  = parent->getHandleEventAction();
	SbViewVolume         oldVV  = parent->getViewVolume();
	SbViewportRegion     oldVPR = parent->getViewportRegion();

	parent->setHandleEventAction(childDragger->getHandleEventAction());
	parent->setViewVolume(childDragger->getViewVolume());
	parent->setViewportRegion(childDragger->getViewportRegion());

	SoPath *pathToKid = childDragger->createPathToThis();
	if (pathToKid) pathToKid->ref();
	parent->setTempPathToThis( pathToKid );
	if (pathToKid) pathToKid->unref();

	parent->motionCallbacks->invokeCallbacks(parent);
    parent->setActiveChildDragger( savedChild );
	// Restore saved values of our variables
	parent->setHandleEventAction(oldHa);
	parent->setViewVolume(oldVV);
	parent->setViewportRegion(oldVPR);

    if (savedChild) savedChild->unref();
}

void SoDragger::childFinishCB(void *parentAsVoid, SoDragger *childDragger )
{
    SoDragger *parent = (SoDragger *) parentAsVoid;

    SoDragger *savedChild = parent->getActiveChildDragger();
    if (savedChild) savedChild->ref();
    parent->setActiveChildDragger( childDragger );
	// Save these variables to we can put 'em back when we're done.
	SoHandleEventAction *oldHa  = parent->getHandleEventAction();
	SbViewVolume         oldVV  = parent->getViewVolume();
	SbViewportRegion     oldVPR = parent->getViewportRegion();

	parent->setHandleEventAction(childDragger->getHandleEventAction());
	parent->setViewVolume(childDragger->getViewVolume());
	parent->setViewportRegion(childDragger->getViewportRegion());

	SoPath *pathToKid = childDragger->createPathToThis();
	if (pathToKid) pathToKid->ref();
	parent->setTempPathToThis( pathToKid );
	if (pathToKid) pathToKid->unref();

	// When child is finished manipulating, we resume caching.
	parent->renderCaching = AUTO;
	parent->finishCallbacks->invokeCallbacks(parent);
    parent->setActiveChildDragger( savedChild );
	// Restore saved values of our variables
	parent->setHandleEventAction(oldHa);
	parent->setViewVolume(oldVV);
	parent->setViewportRegion(oldVPR);

    if (savedChild) savedChild->unref();
}

void SoDragger::childOtherEventCB(void *parentAsVoid, SoDragger *childDragger )
{
    SoDragger *parent = (SoDragger *) parentAsVoid;

    SoDragger *savedChild = parent->getActiveChildDragger();
    if (savedChild) savedChild->ref();
    parent->setActiveChildDragger( childDragger );
	// Save these variables to we can put 'em back when we're done.
	SoHandleEventAction *oldHa  = parent->getHandleEventAction();
	SbViewVolume         oldVV  = parent->getViewVolume();
	SbViewportRegion     oldVPR = parent->getViewportRegion();

	parent->setHandleEventAction(childDragger->getHandleEventAction());
	parent->setViewVolume(childDragger->getViewVolume());
	parent->setViewportRegion(childDragger->getViewportRegion());

	SoPath *pathToKid = childDragger->createPathToThis();
	if (pathToKid) pathToKid->ref();
	parent->setTempPathToThis( pathToKid );
	if (pathToKid) pathToKid->unref();

	parent->otherEventCallbacks->invokeCallbacks(parent);
    parent->setActiveChildDragger( savedChild );
	// Restore saved values of our variables
	parent->setHandleEventAction(oldHa);
	parent->setViewVolume(oldVV);
	parent->setViewportRegion(oldVPR);

    if (savedChild) savedChild->unref();
}

void
SoDragger::workFieldsIntoTransform( SbMatrix &mtx )
{
    SbVec3f    trans,  *translationPtr = NULL;
    SbRotation rot,    *rotationPtr = NULL;
    SbVec3f    scale,  *scaleFactorPtr = NULL;
    SbRotation orient, *scaleOrientationPtr = NULL;
    SbVec3f    center, *centerPtr = NULL;
    SoField    *f;

    // Assign values from any fields you might find...
	if ( (f = getField( "translation" )) != NULL ) {
	    trans =  ((SoSFVec3f *)f)->getValue();
	    translationPtr = &trans;
	}
	if ( (f = getField( "rotation" )) != NULL ) {
	    rot =  ((SoSFRotation *)f)->getValue();
	    rotationPtr = &rot;
	}
	if ( (f = getField( "scaleFactor" )) != NULL ) {
	    scale =  ((SoSFVec3f *)f)->getValue();
	    scaleFactorPtr = &scale;
	}
	if ( (f = getField( "scaleOrientation" )) != NULL ) {
	    orient =  ((SoSFRotation *)f)->getValue();
	    scaleOrientationPtr = &orient;
	}
	if ( (f = getField( "center" )) != NULL ) {
	    center =  ((SoSFVec3f *)f)->getValue();
	    centerPtr = &center;
	}

    workValuesIntoTransform( mtx, translationPtr,
	rotationPtr, scaleFactorPtr, scaleOrientationPtr, centerPtr );
}

void
SoDragger::workValuesIntoTransform( SbMatrix &mtx, 
			    const SbVec3f *translationPtr,
			    const SbRotation *rotationPtr, 
			    const SbVec3f *scaleFactorPtr,
			    const SbRotation *scaleOrientationPtr, 
			    const SbVec3f *centerPtr )
{
    SbVec3f    trans, scale;
    SbRotation rot, scaleOrient;
    SbVec3f    center(0,0,0);

    // To begin with, get the values currently in the matrix. If we were 
    // given a center, use it for the calculations.
	if ( centerPtr != NULL )
	    center =  *centerPtr;
	SoDragger::getTransformFast( mtx, trans,rot,scale,scaleOrient,center);

    // Now, replace any values which should be dictated by our input.
    // Don't need to do center again, since it should remain unchanged.
	if ( translationPtr != NULL )
	    trans = *translationPtr; 
	if ( rotationPtr != NULL )
	    rot = *rotationPtr; 
	if ( scaleFactorPtr != NULL )
	    scale = *scaleFactorPtr; 
	if ( scaleOrientationPtr != NULL )
	    scaleOrient = *scaleOrientationPtr; 

    // Finally, construct a new transform with these values.
	mtx.setTransform( trans, rot, scale, scaleOrient, center );
}

void
SoDragger::getTransformFast( SbMatrix &mtx, SbVec3f &translation,
			     SbRotation &rotation, SbVec3f &scaleFactor,
			     SbRotation &scaleOrientation, 
			     const SbVec3f &center )
{
    if (center != SbVec3f(0,0,0)) {
	// to get fields for a non-0 center, we
	// need to decompose a new matrix "m" such
	// that [-center][m][center] = [this]
	// i.e., [m] = [center][this][-center]
	// (this trick stolen from Showcase code)
        SbMatrix m,c;
	m.setTranslate(-center);
	m.multLeft(mtx);
	c.setTranslate(center);
	m.multLeft(c);
	SoDragger::getTransformFast( m, translation, rotation, scaleFactor,
			       scaleOrientation);
    }
    else
	SoDragger::getTransformFast( mtx, translation, rotation, scaleFactor,
			       scaleOrientation);
}

void
SoDragger::getTransformFast(  SbMatrix &mtx, SbVec3f &translation,
			SbRotation &rotation, SbVec3f &scaleFactor,
			SbRotation &scaleOrientation)
{
    SbBool canDoFast = TRUE;

    // If the last column is (0,0,0,1), then we don't have to worry 
    // about projection matrix.  If not, we need to call SbMatrix::factor
    if ( mtx[0][3] != 0 || mtx[1][3] != 0 || mtx[2][3] != 0 || mtx[3][3] != 1)
	canDoFast = FALSE;

    // You get maxXVec, matYVec, and matZVec if you send the xVec,yVec,zVec,
    // through the matrix.  
    SbVec3f xVec(1,0,0), yVec(0,1,0), zVec(0,0,1);
    SbVec3f matXVec( mtx[0][0], mtx[0][1], mtx[0][2] );
    SbVec3f matYVec( mtx[1][0], mtx[1][1], mtx[1][2] );
    SbVec3f matZVec( mtx[2][0], mtx[2][1], mtx[2][2] );
    // If they are orthogonal, that means the 
    // scaleOrientation is identity() and we are free to factor the matrix.
    // Only need to test two sets, since 3rd answer is implicit.
#define TINY 0.00001
    if (       fabs( matXVec.dot( matYVec )) > TINY )
	canDoFast = FALSE;
    else if (  fabs( matYVec.dot( matZVec )) > TINY )
	canDoFast = FALSE;
#undef TINY

    if ( canDoFast == TRUE ) {

	scaleOrientation.setValue(0,0,0,1);

	// Translation is just the first three entries in bottom row.
	translation.setValue( mtx[3][0], mtx[3][1], mtx[3][2] );

	// scaleFactor is scale of the three transformed axes.
	// Kill two birds with one stone and normalize to get the sizes...
	scaleFactor.setValue( matXVec.normalize(),
			      matYVec.normalize(),
			      matZVec.normalize());

	rotation = SbMatrix( matXVec[0], matXVec[1], matXVec[2], 0,
			     matYVec[0], matYVec[1], matYVec[2], 0,
			     matZVec[0], matZVec[1], matZVec[2], 0,
			     0,          0,          0,          1 );
    }
    else {
        // If 'canDoFast' == FALSE, send the info to SbMatrix::factor
#ifdef DEBUG
#if 0
	SoDebugError::post("SoDragger::getTransformFast", 
		      "This is a tricky matrix. Giving it to SbMatrix::factor");
#endif
#endif

	SbMatrix proj, rotMatrix, scaleOrientMatrix;
	mtx.factor(scaleOrientMatrix, scaleFactor, rotMatrix, translation,proj);
	rotation = rotMatrix;
	// have to transpose because factor gives transpose of correct answer
        scaleOrientation = scaleOrientMatrix.transpose();  
    }
}

SbMatrix
SoDragger::appendTranslation( const SbMatrix &mtx, 
			const SbVec3f &translation, 
			const SbMatrix *conversion )
{
    SbBool  isCnvIdent = (conversion == NULL 
			  || (*conversion) == SbMatrix::identity());
    SbBool  isMtxIdent = (mtx        == SbMatrix::identity());

    // Get motion into local Space.
    // Local space for translation is at the beginning of the matrix.
    // Convert using multDirMatrix, not multVecMatrix, since this is motion.
    SbVec3f lclMotion = translation;
    if ( !isCnvIdent )
	conversion->multDirMatrix( lclMotion, lclMotion );
    if ( !isMtxIdent )
	mtx.multDirMatrix( lclMotion, lclMotion );

    SbVec3f startTranslate = mtx[3];
    SbVec3f newTranslate = startTranslate + lclMotion;

    SbMatrix answer = mtx;
    answer[3][0] = newTranslate[0];
    answer[3][1] = newTranslate[1];
    answer[3][2] = newTranslate[2];

    return answer;
}

SbMatrix 
SoDragger::appendScale( const SbMatrix &mtx, 
		  const SbVec3f &scale, const SbVec3f &scaleCenter,
		  const SbMatrix *conversion )
{
    SbBool  isCnvIdent = (conversion == NULL 
			  || (*conversion) == SbMatrix::identity());
    SbBool  isMtxIdent = (mtx == SbMatrix::identity());
    SbMatrix  convInverse, mtxInverse;


    if ( !isCnvIdent )
	convInverse = conversion->inverse();
    if ( !isMtxIdent )
	mtxInverse = mtx.inverse();

    // Calculate 'matrixWithScale.' 
    // [matrixWithScale] = [convInverse][scaleMtx][conversion][mtx]
	// Create a scaling matrix;
	    SbMatrix scaleMtx;
	    scaleMtx.setScale( scale );
	// convert it to space at end of matrix, 
	    // [scaleMtx] = [convInverse][scaleMtx][conversion]
	    if ( !isCnvIdent ) {
		scaleMtx.multRight( (*conversion) );
		scaleMtx.multLeft( convInverse );
	    }
	// Append this scaling to mtx.
	// [mtxWithScale] = [scaleMtx][mtx]
	    SbMatrix mtxWithScale;
	    if ( !isMtxIdent ) {
	        mtxWithScale = mtx;
		mtxWithScale.multLeft( scaleMtx );
	    }
	    else
		mtxWithScale = scaleMtx;

    // Extract the new values from the merged matrix.
	SbVec3f    mrgTrans, mrgScale;
	SbRotation mrgRot, mrgScaleOrient;
	getTransformFast(mtxWithScale,mrgTrans,mrgRot, mrgScale,mrgScaleOrient);

    // Constrain the scaling to be greater than getMinScale().
	SbVec3f okayMrgScale = mrgScale;
	for (int i = 0; i < 3; i++ ) {
	    if (okayMrgScale[i] <= getMinScale() )
		okayMrgScale[i] = getMinScale();
	}

    SbVec3f okayScale;
    if ( okayMrgScale == mrgScale )
	okayScale = scale;
    else {
	// If we needed to constrain, figure out 'okayScale.' 

	// First, construct 'okayMtxWithScale.'
	// This is a version of 'matrixWithScale' where we replace
	// 'mrgScale' with 'okayMrgScale.'
	    SbMatrix okayMtxWithScale;
	    okayMtxWithScale.setTransform( mrgTrans, mrgRot, 
					okayMrgScale, mrgScaleOrient );

	// Using the same relationship as earlier:
        // [okayMtxWithScale] = [convInverse][okayScaleMtx][conversion][mtx]
	// Solve for 'okayScaleMtx' to find the scale matrix that gives
	// us the desired result.
	    SbMatrix okayScaleMtx = okayMtxWithScale;
	    if ( !isMtxIdent )
		okayScaleMtx.multRight( mtxInverse );
	    if ( !isCnvIdent ) {
		okayScaleMtx.multRight( convInverse );
		okayScaleMtx.multLeft( (*conversion) );
	    }

	// Get the okayScale from its matrix.
	    okayScale[0] = okayScaleMtx[0][0];
	    okayScale[1] = okayScaleMtx[1][1];
	    okayScale[2] = okayScaleMtx[2][2];
    }

    // Now we've got a scale (okayScale) and scaleCenter we know we can use.
    // Create the right matrix and append it to get our answer.
    // [answer] = [convInvserse][scaleAboutCenter][conversion][mtx]
    // where: [scaleAboutCenter] = [centerInverse][okayScale][center]
	SbMatrix scaleAboutCenter;
	scaleAboutCenter.setScale( okayScale );
	if ( scaleCenter != SbVec3f(0,0,0) ) {
	    SbMatrix tm;
	    tm.setTranslate( scaleCenter );
	    scaleAboutCenter.multRight( tm );
	    tm.setTranslate( -scaleCenter );
	    scaleAboutCenter.multLeft( tm );
	}

        SbMatrix answer = scaleAboutCenter;
	if ( !isCnvIdent ) {
	    answer.multLeft( convInverse );
	    answer.multRight( (*conversion) );
	}
	if ( !isMtxIdent )
	    answer.multRight( mtx );

    return answer;
}

SbMatrix
SoDragger::appendRotation( const SbMatrix &mtx,
		    const SbRotation &rot, const SbVec3f &rotCenter,
		    const SbMatrix *conversion )
{
    SbBool  isCnvIdent = (conversion == NULL 
			  || (*conversion) == SbMatrix::identity());
    SbBool  isMtxIdent = (mtx == SbMatrix::identity());
    SbMatrix  convInverse, mtxInverse;


    if ( !isCnvIdent )
	convInverse = conversion->inverse();
    if ( !isMtxIdent )
	mtxInverse = mtx.inverse();

    // Create a matrix for rotating about the rotCenter and append it to our
    // mtx.
    // [answer] = [convInvserse][rotateAboutCenter][conversion][mtx]
    // where: [rotateAboutCenter] = [centerInverse][rotMat][center]
	SbMatrix rotateAboutCenter;
	rotateAboutCenter.setRotate( rot );
	if ( rotCenter != SbVec3f(0,0,0) ) {
	    SbMatrix tm;
	    tm.setTranslate( rotCenter );
	    rotateAboutCenter.multRight( tm );
	    tm.setTranslate( -rotCenter );
	    rotateAboutCenter.multLeft( tm );
	}

        SbMatrix answer = rotateAboutCenter;
	if ( !isCnvIdent ) {
	    answer.multLeft( convInverse );
	    answer.multRight( (*conversion) );
	}
	if ( !isMtxIdent )
	    answer.multRight( mtx );

    return answer;
}

// Called by the SoBaseKit::write() method. 
//
// Draggers don't want to write out fields if they have default vals.
//
// sets isActive to default if default and not connected.
//
// Looks for fields named: 
//    'rotation'    where value is within TINY of SbRotation::identity()
//    'translation' where value is within TINY of (0,0,0)
//    'center'      where value is within TINY of (0,0,0)
//    'scaleFactor' where value is within TINY of (1,1,1)
// and sets them to default if they are not connected from a field.
// (note that most draggers are missing at least some of these, but thats okay)
//
// Then calls the method for SoInteractionKit.
//
// NOTE: Parts which are set to default may still wind up writing to file 
//       if, for example, they lie on a path.
void
SoDragger::setDefaultOnNonWritingFields()
{
    // We don't write out isActive if it has default value.
    if ( ! (isActive.isConnected() && isActive.isConnectionEnabled())
         && isActive.getValue() == FALSE )
		      isActive.setDefault(TRUE);

    // Since so many draggers have fields named 'rotation',
    // 'translation', 'scaleFactor', and 'center', we'll check for them
    // in the base class and set them to default if we can...
#define TINY 0.00001
    SoField *f;
    if ( (f = getField( "rotation" )) != NULL ) {
	if ( !(f->isConnected() && f->isConnectionEnabled())
	     && (((SoSFRotation *)f)->getValue()).equals(
			SbRotation::identity(),TINY))
	f->setDefault(TRUE);
    }
    if ( (f = getField( "translation" )) != NULL ) {
	if ( !(f->isConnected() && f->isConnectionEnabled())
	     && (((SoSFVec3f *)f)->getValue()).equals(SbVec3f(0,0,0),TINY))
	f->setDefault(TRUE);
    }
    if ( (f = getField( "scaleFactor" )) != NULL ) {
	if ( !(f->isConnected() && f->isConnectionEnabled())
	     && (((SoSFVec3f *)f)->getValue()).equals(SbVec3f(1,1,1),TINY))
	f->setDefault(TRUE);
    }
    if ( (f = getField( "center" )) != NULL ) {
	if ( !(f->isConnected() && f->isConnectionEnabled())
	     && (((SoSFVec3f *)f)->getValue()).equals(SbVec3f(0,0,0),TINY))
	f->setDefault(TRUE);
    }
#undef TINY

    // This node may change after construction, but we still
    // don't want to write it out.
    motionMatrix.setDefault(TRUE);

    // Call the base class...
    SoInteractionKit::setDefaultOnNonWritingFields();
}
