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
 |	Defines the SoHandleEventAction class
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_HANDLE_EVENT_ACTION_
#define  _SO_HANDLE_EVENT_ACTION_

#include <Inventor/SbViewportRegion.h>
#include <Inventor/actions/SoSubAction.h>
#include <Inventor/actions/SoRayPickAction.h>

class SoEvent;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoHandleEventAction
//
//  Event handling action. This traverses a graph looking for a node
//  or nodes interested in a particular event.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoEvAct
class SoHandleEventAction : public SoAction {

    SO_ACTION_HEADER(SoHandleEventAction);

  public:
    // Constructor takes viewport region to use for handling events.
    SoHandleEventAction(const SbViewportRegion &viewportRegion);

    // Destructor
    virtual ~SoHandleEventAction();

    // Sets current viewport region to use for the event processing
    // C-api: name=setVPReg
    void		setViewportRegion(const SbViewportRegion &newRegion);


    // Returns current viewport region
    // C-api: name=getVPReg
    const SbViewportRegion &getViewportRegion() const	{ return vpRegion; }


    // Sets/returns the event to handle
    // C-api: name=setEv
    void		setEvent(const SoEvent *ev)	{ event = ev; }
    // C-api: name=getEv
    const SoEvent *	getEvent() const		{ return event; }

    // Sets/returns whether any node has yet handled the event
    void		setHandled()		{ setTerminated(TRUE); }
    SbBool		isHandled() const	{ return hasTerminated(); }

    // grab() - node will receive all events until release() is called.
    // grabber() returns which node is currently grabbing events.
    void    	    	setGrabber(SoNode *node);
    void    	    	releaseGrabber()	    { setGrabber(NULL); }
    SoNode *	    	getGrabber() const	    { return eventGrabber; }

    // Sets/returns the root node used for initiating a pick action
    // for those nodes that want to know who is under the cursor
    void		setPickRoot(SoNode *node);
    SoNode *		getPickRoot() const		{ return pickRoot; }

    // Set the radius (in pixels) around the point. This is used when
    // testing the ray against lines and points.
    void		setPickRadius(float radiusInPixels) 
			    { pickAct->setRadius(radiusInPixels); }

    // Returns the object hit (as an SoPickedPoint) by performing a
    // pick based on the current mouse location as specified in the
    // event the action is being applied for. This initiates a pick
    // action if necessary to find this object. The storage for the
    // pickedPoint remains valid as long as the SoHandleEventAction is
    // not re-used or deleted.
    // C-api: name=getPickPt
    const SoPickedPoint *getPickedPoint();

    // Return a list of objects hit, sorted from front to back
    // C-api: name=getPickPtList
    const SoPickedPointList &getPickedPointList();

  SoINTERNAL public:
    static void		initClass();

  protected:
    // Initiates action on graph
    virtual void	beginTraversal(SoNode *node);

  private:
    const SoEvent	*event;		// Event being handled
    SoNode		*pickRoot;	// Root node for initiating picking
    SoPickedPoint	*pickedPoint;	// PickedPoint from last pick
    SbBool		pickValid;	// Whether last pick is still valid
    SbBool		usedPickAll;	// TRUE if last pick used pickAll=TRUE
    SoRayPickAction	*pickAct;	// Pick action
    SoNode  	    	*eventGrabber;  // Event grabber - gets all events
    SbViewportRegion	vpRegion;	// Current viewport region
};

#endif /* _SO_HANDLE_EVENT_ACTION_ */
