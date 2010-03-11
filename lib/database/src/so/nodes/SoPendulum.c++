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
 |      SoPendulum
 |
 |   Author(s)          : Ronen Barzel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoDB.h>
#include <Inventor/engines/SoElapsedTime.h>
#include <Inventor/engines/SoCalculator.h>
#include <Inventor/engines/SoInterpolate.h>
#include <Inventor/nodes/SoPendulum.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include <Inventor/sensors/SoOneShotSensor.h>

SO_NODE_SOURCE(SoPendulum);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoPendulum::SoPendulum()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoPendulum);
    SO_NODE_ADD_FIELD(rotation0, (SbRotation(0,0,0,1)));
    SO_NODE_ADD_FIELD(rotation1, (SbRotation(0,0,0,1)));
    SO_NODE_ADD_FIELD(speed, (1));
    SO_NODE_ADD_FIELD(on, (TRUE));

    time = new SoElapsedTime;
    calc = new SoCalculator;
    interp = new SoInterpolateRotation;
    time->ref();
    calc->ref();
    interp->ref();

    // next make the sensors (when wiring the engines
    // together the notify method will try to schedule
    // the sensor, so make sure it exists).
    rotationSensor = new SoOneShotSensor(rotationSensorCB, this);
    rotationSensor->setPriority(0);
    rotation0Sensor = new SoFieldSensor(rotationSensorCB, this);
    rotation0Sensor->setPriority(0);
    rotation0Sensor->attach(&rotation0);
    rotation1Sensor = new SoFieldSensor(rotationSensorCB, this);
    rotation1Sensor->setPriority(0);
    rotation1Sensor->attach(&rotation1);
    onSensor = new SoFieldSensor(onSensorCB, this);
    onSensor->setPriority(0);
    onSensor->attach(&on);

    time->on.connectFrom(&on);
    calc->a.connectFrom(&speed);
    calc->b.connectFrom(&time->timeOut);
    // input "c" to the calculation is a phase offset that is
    // set by the rotation sensor callback.
    calc->expression.setValue("oa = 0.5 + 0.5 * sin(a*b*2*M_PI + c)");
    rotationSensorCB(this, NULL); // init the offset to the default rotation
    interp->input0.connectFrom(&rotation0);
    interp->input1.connectFrom(&rotation1);
    interp->alpha.connectFrom(&calc->oa);
    rotation.connectFrom(&interp->output);

    // keep the internal connection, so we can tell
    // later if somebody overrode it.
    rotation.getConnectedEngine(internalConnection);

    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor (necessary since inline destructor is too complex)
//
// Use: private

SoPendulum::~SoPendulum()
//
////////////////////////////////////////////////////////////////////////
{
    time->unref();
    calc->unref();
    interp->unref();

    delete rotationSensor;
    delete rotation0Sensor;
    delete rotation1Sensor;
    delete onSensor;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Write action.  Disables connections to internal engines
//    during the write.
//
// Use: protected

void
SoPendulum::write(SoWriteAction *action)
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
SoPendulum::notify(SoNotList *list)
//
////////////////////////////////////////////////////////////////////////
{
    SoNotRec *rec = list->getFirstRec();

    // only interested in a field-to-container notification
    if (rec->getType() == SoNotRec::CONTAINER &&
	rec->getBase() == this &&
	list->getLastField() == &rotation) {

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
SoPendulum::rotationSensorCB(void *data, SoSensor *)
//
////////////////////////////////////////////////////////////////////////
{
    SoPendulum *pendulum = (SoPendulum *) data;
    SbVec4f	q, q1, q0;
    pendulum->rotation.getValue().getValue(q[0],q[1],q[2],q[3]);
    pendulum->rotation0.getValue().getValue(q0[0],q0[1],q0[2],q0[3]);
    pendulum->rotation1.getValue().getValue(q1[0],q1[1],q1[2],q1[3]);

    // find the value of the interpolant that would
    // line up with q.  See where q projects onto the line
    // from q0 to q1
    SbVec4f	diff = q1 - q0;
    float	len = diff.length(), alpha;
    if (len == 0)
	alpha = 0;
    else {
	alpha = (q - q0).dot(diff)/(len*len);
	if (alpha > 1)
	    alpha = 1;
	if (alpha < 0)
	    alpha = 0;
    }

    float a = pendulum->calc->a[0];
    float b = pendulum->calc->b[0];
    // invert the expression: alpha = .5 + .5 sin(a*b*2*PI + c)
    pendulum->calc->c.setValue(asin(2*alpha-1) - a*b*2*M_PI);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback is called when somebody sets the on field
//
// Use: protected

void
SoPendulum::onSensorCB(void *data, SoSensor *)
//
////////////////////////////////////////////////////////////////////////
{
    SoPendulum *pendulum = (SoPendulum *) data;

    if (pendulum->on.getValue()) {
	// if turned on, force the internal connection to be made and enabled,
	// no matter what sort of mucking has happened to it.
	pendulum->rotation.enableConnection(TRUE);
	pendulum->rotation.connectFrom(&pendulum->interp->output);
	pendulum->rotation.getConnectedEngine(pendulum->internalConnection);
    }
}
