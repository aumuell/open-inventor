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
 |   $Revision: 1.4 $
 |
 |   Classes:
 |	SoEventCallback
 |
 |   Author(s): David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoDB.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/events/SoEvent.h>
#include <Inventor/nodes/SoEventCallback.h>

// internal class for storing event types, callback funcs, user data
typedef struct {
    SoType		eventType;
    SoEventCallbackCB	*func;
    void		*userData;
} SoEventCallbackData;

SO_NODE_SOURCE(SoEventCallback);

////////////////////////////////////////////////////////////////////////
//
// Constructor
//
SoEventCallback::SoEventCallback()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoEventCallback);

    isBuiltIn        = TRUE;
    cblist	     = new SbPList;
    pathOfInterest   = NULL;
    eventAction	     = NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Destructor
//
SoEventCallback::~SoEventCallback()
//
////////////////////////////////////////////////////////////////////////
{
    if (pathOfInterest != NULL)
	pathOfInterest->unref();

    // delete the data elements, and delete the callback list
    for (int i = 0; i < cblist->getLength(); i++)
	delete (SoEventCallbackData *) (*cblist)[i];
    delete cblist;
}


////////////////////////////////////////////////////////////////////////
//
// Pay attention to the passed type of event, calling f whenever
// and event of the passed type occurs. userData will be passed to f.
//
void
SoEventCallback::addEventCallback(
    SoType eventType,
    SoEventCallbackCB *f,
    void *userData)
//
////////////////////////////////////////////////////////////////////////
{
    SoEventCallbackData *data = new SoEventCallbackData;
    data->eventType = eventType;
    data->func = f;
    data->userData = userData;
    cblist->append(data);
}

////////////////////////////////////////////////////////////////////////
//
// No longer pay attention to the passed type of event for the function f.
//
void
SoEventCallback::removeEventCallback(
    SoType eventType,
    SoEventCallbackCB *f,
    void *userData)
//
////////////////////////////////////////////////////////////////////////
{
    // loop from the end of the list, in case we remove items
    for (int i = cblist->getLength() - 1; i >= 0; i--) {
	SoEventCallbackData *data = (SoEventCallbackData *) (*cblist)[i];
	if ((data->eventType == eventType) &&
	    (data->func == f) &&
	    (data->userData == userData)) {
	    
	    // found the func/event type/data triplet - remove it!
	    cblist->remove(i);
	    delete data;
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// This copies the passed path into pathOfInterest.
//
// Use: public
//
void
SoEventCallback::setPath(SoPath *path)
{
    // ref the input path
    if (path != NULL)
	path->ref();

    // nuke the old path
    if (pathOfInterest != NULL) {
    	pathOfInterest->unref();
	pathOfInterest = NULL;
    }
    // and copy the new path
    if (path != NULL) {
    	pathOfInterest = path->copy();
    	pathOfInterest->ref();
    }

    // unref the input path
    if (path != NULL)
	path->unref();

}

////////////////////////////////////////////////////////////////////////
//
// Invoke all callback funcs interested in the passed event.
//
// Use: private
//
void
SoEventCallback::invokeCallbacks(const SoEvent *e)
//
////////////////////////////////////////////////////////////////////////
{
    // Call all callback funcs interested in this event type
    for (int i = 0; i < cblist->getLength(); i++) {
	SoEventCallbackData *data = (SoEventCallbackData *) (*cblist)[i];
	if (e->isOfType(data->eventType)) {
	    (*data->func) (data->userData, this);
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// This is called for every SoHandleEventAction traversal.
//
// Use: protected
//
void
SoEventCallback::handleEvent(SoHandleEventAction *ha)
//
////////////////////////////////////////////////////////////////////////
{
    // set eventAction so that the app may call setHandled(), grab(), etc
    eventAction = ha;
    
    // Are we monitoring a path?
    if (pathOfInterest == NULL)
	invokeCallbacks(ha->getEvent());
    else {
	// make sure the path of interest was picked
	const SoPickedPoint *pp = ha->getPickedPoint();

	if ((pp != NULL) && pp->getPath()->containsPath(pathOfInterest))
	    invokeCallbacks(ha->getEvent());
    }
    
    eventAction = NULL;
}
