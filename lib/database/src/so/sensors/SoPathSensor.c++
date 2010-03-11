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
 |	SoPathSensor
 |
 |   Author(s)		: Paul Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/misc/SoChildList.h>
#include <Inventor/SoPath.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/sensors/SoPathSensor.h>

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoPathSensor::SoPathSensor() : SoDataSensor()
//
////////////////////////////////////////////////////////////////////////
{
    path = NULL;
    head = NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor with function and data.
//
// Use: public

SoPathSensor::SoPathSensor(SoSensorCB *func, void *data) :
	SoDataSensor(func, data)
//
////////////////////////////////////////////////////////////////////////
{
    path = NULL;
    head = NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: protected

SoPathSensor::~SoPathSensor()
//
////////////////////////////////////////////////////////////////////////
{
    detach();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Attaches the sensor to the given path.
//
// Use: public

void
SoPathSensor::attach(SoPath *pathToAttachTo)
//
////////////////////////////////////////////////////////////////////////
{
    if (path != NULL)
	detach();

    path = (SoFullPath *)pathToAttachTo;
    path->addAuditor(this, SoNotRec::SENSOR);

    head = path->getHead();
    if (head != NULL)
	head->addAuditor(this, SoNotRec::SENSOR);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Detaches the sensor if it is attached to a path.
//
// Use: public

void
SoPathSensor::detach()
//
////////////////////////////////////////////////////////////////////////
{
    if (path != NULL) {
	path->removeAuditor(this, SoNotRec::SENSOR);
	path = NULL;

	if (head != NULL) {
	    head->removeAuditor(this, SoNotRec::SENSOR);
	    head = NULL;
	}

	// If we are scheduled, there's no point leaving it scheduled,
	// since it's not attached any more to whatever caused it to
	// become scheduled.
	unschedule();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This is called by the attached path when it (the path) is about
//    to be deleted.
//
// Use: private

void
SoPathSensor::dyingReference()
//
////////////////////////////////////////////////////////////////////////
{
    // We want to detach the sensor if it's still attached to the
    // dying path after we invoke the callback. If the callback
    // attaches to something else, we don't want to detach it. So
    // we'll compare the paths before and after the callback is
    // invoked and detach only if it's the same one.

    SoPath *dyingPath = getAttachedPath();

    invokeDeleteCallback();

    if (getAttachedPath() == dyingPath)
	detach();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Notification code; this is pretty hairy!
//
//    If the path started notification, it means the path changed
//    length, was truncated, etc.  In that case, we just call the
//    user's callback (using our base classes notify method).
//
//    If we are being notified of a change through the path, we just
//    ignore that notification-- we will also be notified through the
//    head node of the path.  This will happen for topology changes
//    that affect the path (adding removing children, etc).
//
//    If we are being notified from the head of the path, we need to
//    determine if the change made is on the path.  If it is, we call
//    the callbacks; otherwise, we ignore it.
//
// Use: internal

void
SoPathSensor::notify(SoNotList *list)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool doNotify;

    // If the path started notification:
    if (list->getFirstRec()->getBase() == path)
	doNotify = TRUE;

#ifdef DEBUG
    // If we are being notified of a change through the path:
    else if (list->getLastRec()->getBase() == path) {
	SoDebugError::post("(internal) SoPathSensor::notify",
			   "Path sensor notified by path!");
	// ignore this notification, we'll get another one through the
	// head node...
	doNotify = FALSE;
    }
#endif /* DEBUG */
    
    // The complicated case.  We are being notified through the head
    // node, we need to decide whether or not the change is on the
    // path:
    else
	doNotify = path->isRelevantNotification(list);

    // If the head node of the path changes, detach from old head and
    // attach to new:
    if (path->getHead() != head) {
	// Detach from old:
	if (head != NULL)
	    head->removeAuditor(this, SoNotRec::SENSOR);
	head = path->getHead();
	// Attach to new:
	if (head != NULL)
	    head->addAuditor(this, SoNotRec::SENSOR);
    }

    // Notify if we're supposed to...
    if (doNotify)
	SoDataSensor::notify(list);
}
