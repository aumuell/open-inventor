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
 |      SoRotor
 |
 |   Author(s)          : Ronen Barzel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoDB.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/engines/SoCalculator.h>
#include <Inventor/engines/SoCompose.h>
#include <Inventor/engines/SoElapsedTime.h>
#include <Inventor/nodes/SoRotor.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include <Inventor/sensors/SoOneShotSensor.h>

SO_NODE_SOURCE(SoRotor);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoRotor::SoRotor()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoRotor);
    SO_NODE_ADD_FIELD(speed, (1));
    SO_NODE_ADD_FIELD(on, (TRUE));

    // first create the engines
    time = new SoElapsedTime;
    calc = new SoCalculator;
    compose = new SoComposeRotation;
    time->ref();
    calc->ref();
    compose->ref();

    rotationSensor = NULL;

    // finally, wire the engines together
    time->on.connectFrom(&on);
    calc->a.connectFrom(&speed);
    calc->b.connectFrom(&time->timeOut);
    // input "c" to the calculation is a phase offset that is
    // set by the rotation sensor callback.
    calc->expression.setValue("oa = fmod(a*b*2*M_PI + c, 2*M_PI)");

    // initial value 
    compose->angle.connectFrom(&calc->oa);

    SbVec3f axis;
    float angle;
    rotation.getValue(axis, angle);
    compose->axis.setValue(axis);
    rotation.connectFrom(&compose->rotation);

    // keep the internal connection, so we can tell
    // later if somebody overrode it.
    rotation.getConnectedEngine(internalConnection);

    // next make the sensors
    rotationSensor = new SoOneShotSensor(rotationSensorCB, this);
    rotationSensor->setPriority(0);

    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor (necessary since inline destructor is too complex)
//
// Use: private

SoRotor::~SoRotor()
//
////////////////////////////////////////////////////////////////////////
{
    time->unref();
    calc->unref();
    compose->unref();

    delete rotationSensor;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Write action.  Disables connections to internal engines
//    during the write.
//
// Use: protected

void
SoRotor::write(SoWriteAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool 		save = rotation.isConnectionEnabled();
    SoEngineOutput	*rotationSource;

    // if the connection is to the internal engine, disable it
    if (rotation.isConnected() &&
	rotation.getConnectedEngine(rotationSource) &&
	rotationSource == internalConnection) {

	rotation.enableConnection(FALSE);
    }

    SoRotation::write(action);

    rotation.enableConnection(save);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Override notify to notice when the rotation field is touched.
// Can't use field sensor because we need to distinguish
// our own touching of the field from a setValue by the client.
//
// Use: protected

void
SoRotor::notify(SoNotList *list)
//
////////////////////////////////////////////////////////////////////////
{
    SoNotRec *rec = list->getFirstRec();

    // only interested in a field-to-container notification
    if (rec->getType() == SoNotRec::CONTAINER &&
	rec->getBase() == this &&
	list->getLastField() == &rotation) {

	if (rotationSensor)
	    rotationSensor->schedule();
    }

    SoRotation::notify(list);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback is called when somebody sets the rotation field.
//
// Use: protected

void
SoRotor::rotationSensorCB(void *data, SoSensor *)
//
////////////////////////////////////////////////////////////////////////
{
    SoRotor *rotor = (SoRotor *) data;
    SbVec3f	axis;
    float	angle;
    rotor->rotation.getValue(axis, angle);

    if (angle != 0.0) {

	float a, b;
	a = rotor->calc->a[0];
	b = rotor->calc->b[0];
	// invert the expression: angle = a*b*2*pi + c    (mod 2 pi)
	rotor->calc->c.setValue(angle - (a*b*2*M_PI));

	rotor->compose->axis.setValue(axis);
    }
#ifdef DEBUG
    else {
	SoDebugError::post("SoRotor::rotation.setValue", 
			   "You should specify a non-zero angle"
			   " for initial rotation.");
    }
#endif
}
