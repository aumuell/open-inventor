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
 |	SoDataSensor
 |
 |   Author(s)		: Paul Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/nodes/SoNode.h>
#include <Inventor/SoPath.h>
#include <Inventor/sensors/SoDataSensor.h>
#include <Inventor/errors/SoDebugError.h>

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: protected

SoDataSensor::SoDataSensor() : SoDelayQueueSensor()
//
////////////////////////////////////////////////////////////////////////
{
    deleteFunc = NULL;
    deleteData = NULL;

    trigNode   = NULL;
    trigField  = NULL;
    trigPath   = NULL;
    doTrigPath = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor with function and data.
//
// Use: protected

SoDataSensor::SoDataSensor(SoSensorCB *func, void *data) :
	SoDelayQueueSensor(func, data)
//
////////////////////////////////////////////////////////////////////////
{
    deleteFunc = NULL;
    deleteData = NULL;

    trigNode   = NULL;
    trigField  = NULL;
    trigPath   = NULL;
    doTrigPath = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: public

SoDataSensor::~SoDataSensor()
//
////////////////////////////////////////////////////////////////////////
{
    unschedule();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Get the trigger node.  This is available only for immediate
//  sensors (see header for an explanation of why).
//
// Use: public

SoNode *
SoDataSensor::getTriggerNode() const
//
////////////////////////////////////////////////////////////////////////
{
    if (getPriority() != 0) {
#ifdef DEBUG
	SoDebugError::postWarning("SoDataSensor::getTriggerNode",
				  "Sensor priority is not zero"
				  " (priority is %d)", getPriority());
#endif
	return NULL;
    }
    return trigNode;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Get the trigger path.  This is available only for immediate
//  sensors (see header for an explanation of why).
//
// Use: public

SoPath *
SoDataSensor::getTriggerPath() const
//
////////////////////////////////////////////////////////////////////////
{
    if (getPriority() != 0) {
#ifdef DEBUG
	SoDebugError::postWarning("SoDataSensor::getTriggerPath",
				  "Sensor priority is not zero"
				  " (priority is %d)", getPriority());
#endif
	return NULL;
    }
    return trigPath;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Get the trigger field.  This is available only for immediate
//  sensors (see header for an explanation of why).
//
// Use: public

SoField *
SoDataSensor::getTriggerField() const
//
////////////////////////////////////////////////////////////////////////
{
    if (getPriority() != 0) {
#ifdef DEBUG
	SoDebugError::postWarning("SoDataSensor::getTriggerField",
				  "Sensor priority is not zero"
				  " (priority is %d)", getPriority());
#endif
	return NULL;
    }
    return trigField;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Override unschedule to reset trigNode/path.
//
// Use: public

void
SoDataSensor::unschedule()
//
////////////////////////////////////////////////////////////////////////
{
    SoDelayQueueSensor::unschedule();
    if (trigNode) {
	trigNode = NULL;
    }
    if (trigPath) {
	trigPath->unref();
	trigPath = NULL;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Override trigger() to trigNode/path after the callback
//    function has had a chance to look at them.
//
// Use: protected

void
SoDataSensor::trigger()
//
////////////////////////////////////////////////////////////////////////
{
    SoDelayQueueSensor::trigger();
    if (trigNode) {
	trigNode = NULL;
    }
    if (trigPath) {
	trigPath->unref();
	trigPath = NULL;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Invokes delete callback (if it exists).
//
// Use: protected

void
SoDataSensor::invokeDeleteCallback()
//
////////////////////////////////////////////////////////////////////////
{
    if (deleteFunc != NULL)
	deleteFunc(deleteData, this);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Propagates modification notification through an instance. By
//    default, this schedules the sensor to be triggered and saves
//    some information from the notification list. Subclasses may
//    override this method to do more detailed checks of the
//    notification record list before scheduling.
//
// Use: protected, virtual

void
SoDataSensor::notify(SoNotList *list)
//
////////////////////////////////////////////////////////////////////////
{
    if (trigPath != NULL)
	trigPath->unref();

    // Save node that triggered notification
    SoNotRec *nodeNotRec = list->getFirstRecAtNode();
    // nodeNotRec may be NULL if a path change causes notification.
    if (nodeNotRec != NULL) {
	trigNode = (SoNode *) nodeNotRec->getBase();
    }
    else trigNode = NULL;

    // Save field that triggered notification
    trigField = list->getLastField();

    // If requested, save path from last node down to trigNode
    if (doTrigPath && trigNode != NULL) {
	const SoNotRec	*rec;

	trigPath = new SoPath;
	trigPath->ref();

	// Find last notification record that has a node
	for (rec = list->getLastRec();
	     ! rec->getBase()->isOfType(SoNode::getClassTypeId());
	     rec = rec->getPrevious())
	    ;

	// That node is the head of the path
	trigPath->setHead((SoNode *) rec->getBase());

	// Add successive nodes (if any) until we get to the one that
	// triggered the notification
	if (rec->getBase() != trigNode) {
	    do {
		rec = rec->getPrevious();
		trigPath->append((SoNode *) rec->getBase());
	    } while (rec->getBase() != trigNode);
	}
    }
    else
	trigPath = NULL;

    schedule();
}


