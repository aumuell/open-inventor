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
 |	SoEventCallback node class - invokes callbacks when the handle
 |   event action is applied to this node.
 |
 |   Author(s): David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_EVENT_CALLBACK_
#define  _SO_EVENT_CALLBACK_

#include <Inventor/misc/SoCallbackList.h>
#include <Inventor/SoPath.h>
#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/actions/SoHandleEventAction.h>

class SoEvent;
class SoEventCallback;

// callback function prototypes
typedef void SoEventCallbackCB(void *userData, SoEventCallback *node);

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoEventCallback
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoEvCB
class SoEventCallback : public SoNode {

    SO_NODE_HEADER(SoEventCallback);

  public:
    // Constructor
    SoEventCallback();
    
    // Set the path to monitor - the callbacks are only invoked
    // if this path is picked. If path is NULL, then the callbacks
    // are invoked automatically on every event of the type specified
    // in setEventInterest, without a pick occurring.
    // This makes its own copy of path.
    void		setPath(SoPath *path);
    const SoPath *	getPath()		{ return pathOfInterest; }

    // Specify callback functions to be invoked. When invoked, the
    // callback will be passed userData, and a pointer to this. To
    // specify interest in all event types, pass
    // SoEvent::getClassTypeId() as the eventType. Else, pass the type
    // of event you are interested in (e.g.
    // SoLocation2Event::getClassTypeId() for mouse motion)
    // C-api: name=addEvCB
    void		addEventCallback(SoType eventType,
					 SoEventCallbackCB *f,
					 void *userData = NULL);
    // C-api: name=removeEvCB
    void		removeEventCallback(SoType eventType,
					    SoEventCallbackCB *f,
					    void *userData = NULL);

    //////////////////////
    //
    // These all provide information to callback functions. They
    // return NULL when called from anywhere but a callback function.
    //
    
    // Returns the SoHandleEventAction being applied
    // C-api: name=getAct
    SoHandleEventAction *	getAction() const { return eventAction; }

    // Returns the event being handled by the action
    // C-api: name=getEv
    const SoEvent *		getEvent() const
	{ return (eventAction != NULL ? eventAction->getEvent() : NULL); }

    // Returns pick information from the action
    // C-api: name=getPckPt
    const SoPickedPoint *	getPickedPoint() const
	{ return (eventAction != NULL ? eventAction->getPickedPoint() : NULL);}
    
    //
    //////////////////////

    // Tells the node the event was handled. The callback function is
    // responsible for setting whether the event was handled or not.
    // If there is more than one callback function, all of them will be
    // invoked, regardless of whether one has handled the event or not.
    void		setHandled()
	{ if (eventAction != NULL) eventAction->setHandled(); }

    // Returns whether the event was handled
    SbBool		isHandled() const
	{ return (eventAction != NULL) ? eventAction->isHandled() : FALSE; }
    
    // Tells the event callback node to grab events or release the
    // grab. While grabbing, the node will consume all events;
    // however, the callback functions are still only invoked for
    // events of interest.
    // C-api: name=grabEv
    void		grabEvents()
	{ if (eventAction != NULL) eventAction->setGrabber(this); }

    // C-api: name=releaseEv
    void		releaseEvents()
	{ if (eventAction != NULL) eventAction->releaseGrabber(); }

  SoINTERNAL public:
    static void		initClass();	// initialize the class

  protected:
    // Destructor - protected since ref/unref is what should destroy this
    virtual ~SoEventCallback();

    // This will be called during handleEventAction traversal.
    virtual void    	handleEvent(SoHandleEventAction *ha);
    
  private:
    // Only invoke callbacks if this path was picked.
    // If path is NULL, always invoke callbacks.
    SoPath		*pathOfInterest;
    
    // List of callback functions, event types, and user data.
    SbPList *		cblist;
    
    // This is set for each SoHandleEventAction traversal of this node
    // so that the apps callback routine can invoke methods on the action.
    SoHandleEventAction	*eventAction;
    
    // This will look through cblist and invoke each callback function
    // that is interested in the passed event.
    void		invokeCallbacks(const SoEvent *e);
};

#endif /* _SO_EVENT_CALLBACK_ */
