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
 |      SoBlinker
 |
 |   Author(s)          : Ronen Barzel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoDB.h>
#include <Inventor/engines/SoTimeCounter.h>
#include <Inventor/nodes/SoBlinker.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include <Inventor/sensors/SoOneShotSensor.h>

SO_NODE_SOURCE(SoBlinker);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoBlinker::SoBlinker()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoBlinker);
    SO_NODE_ADD_FIELD(speed, (1));
    SO_NODE_ADD_FIELD(on, (TRUE));

    nchildren = -1;

    count = new SoTimeCounter;
    count->ref();

    // next make the sensors (when wiring the engines
    // together the notify method will try to schedule
    // the sensor, so make sure it exists).
    childrenSensor = new SoOneShotSensor(childrenSensorCB, this);
    childrenSensor->setPriority(0);
    whichChildSensor = new SoOneShotSensor(whichChildSensorCB, this);
    whichChildSensor->setPriority(0);
    onSensor = new SoFieldSensor(onSensorCB, this);
    onSensor->setPriority(0);
    onSensor->attach(&on);

    count->on.connectFrom(&on);
    count->frequency.connectFrom(&speed);
    whichChild.connectFrom(&count->output);

    // keep the internal connection, so we can tell
    // later if somebody overrode it.
    whichChild.getConnectedEngine(internalConnection);

    // Make sure counter is set up correctly
    childrenSensorCB(this, NULL);

    // Make sure the sensor on the "on" field acts correctly
    wasOn = TRUE;

    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor (necessary since inline destructor is too complex)
//
// Use: private

SoBlinker::~SoBlinker()
//
////////////////////////////////////////////////////////////////////////
{
    count->unref();
    delete childrenSensor;
    delete whichChildSensor;
    delete onSensor;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    The bounding box takes all children into account.
//
// Use: extender

void
SoBlinker::getBoundingBox(SoGetBoundingBoxAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoGroup::getBoundingBox(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Write action.  Disables connections to internal engines
//    during the write.
//
// Use: extender

void
SoBlinker::write(SoWriteAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool 		save = whichChild.isConnectionEnabled();
    SoEngineOutput	*whichChildSource;

    // if the connection is to the internal engine, disable it
    if (whichChild.isConnected() &&
	whichChild.getConnectedEngine(whichChildSource) &&
	whichChildSource == internalConnection) {

	whichChild.enableConnection(FALSE);
    }

    SoSwitch::write(action);

    whichChild.enableConnection(save);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Override notify to notice when the number of children changes.
// Also, detect when a setValue is done on whichChild, to allow
// resetting the phase (can't use a sensor, because it would detect
// function writes as well as setValue's).
//
// Use: protected

void
SoBlinker::notify(SoNotList *list)
//
////////////////////////////////////////////////////////////////////////
{
    if (getNumChildren() != nchildren) {
	nchildren = getNumChildren();
	childrenSensor->schedule();
    }

    SoNotRec *rec = list->getFirstRec();

    // only interested in a field-to-container notification
    if (rec->getType() == SoNotRec::CONTAINER &&
	rec->getBase() == this &&
	list->getLastField() == &whichChild) {

	whichChildSensor->schedule();
    }

    SoSwitch::notify(list);
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback is called when the number of children changes
// Resets the counter to do the appropriate thing.
//
// Use: protected

void
SoBlinker::childrenSensorCB(void *data, SoSensor *)
//
////////////////////////////////////////////////////////////////////////
{
    SoBlinker *blinker = (SoBlinker *) data;
    switch (blinker->getNumChildren()) {
	case 0:
	    blinker->count->min = SO_SWITCH_NONE;
	    blinker->count->max = SO_SWITCH_NONE;
	    blinker->count->step = 0;
	    break;

	case 1:
	    blinker->count->min = SO_SWITCH_NONE;
	    blinker->count->max = 0;
	    blinker->count->step = -(SO_SWITCH_NONE);
	    break;

	default:
	    blinker->count->min = 0;
	    blinker->count->max = blinker->getNumChildren()-1;
	    blinker->count->step = 1;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback is called when somebody sets the whichChild field.
// Resets the counter to count from the specified child.
//
// Use: protected

void
SoBlinker::whichChildSensorCB(void *data, SoSensor *)
//
////////////////////////////////////////////////////////////////////////
{
    SoBlinker *blinker = (SoBlinker *) data;
    blinker->count->reset.setValue((short) blinker->whichChild.getValue());
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback is called when somebody sets the on field
//
// Use: protected

void
SoBlinker::onSensorCB(void *data, SoSensor *)
//
////////////////////////////////////////////////////////////////////////
{
    SoBlinker *blinker = (SoBlinker *) data;

    if (blinker->on.getValue()) {

	// Do this only if the blinker was off and is now on
	if (! blinker->wasOn) {
	    // if turned on, force the internal connection to be made
	    // and enabled, no matter what sort of mucking has
	    // happened to it.
	    blinker->whichChild.enableConnection(TRUE);
	    blinker->whichChild.connectFrom(&blinker->count->output);
	    blinker->whichChild.getConnectedEngine(blinker->internalConnection);
	    blinker->wasOn = TRUE;
	}
    }

    else
	blinker->wasOn = FALSE;
}
