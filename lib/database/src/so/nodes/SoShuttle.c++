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
 |      SoShuttle
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
#include <Inventor/nodes/SoShuttle.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include <Inventor/sensors/SoOneShotSensor.h>

SO_NODE_SOURCE(SoShuttle);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoShuttle::SoShuttle()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoShuttle);
    SO_NODE_ADD_FIELD(translation0, (SbVec3f(0,0,0)));
    SO_NODE_ADD_FIELD(translation1, (SbVec3f(0,0,0)));
    SO_NODE_ADD_FIELD(speed, (1));
    SO_NODE_ADD_FIELD(on, (TRUE));

    time = new SoElapsedTime;
    calc = new SoCalculator;
    interp = new SoInterpolateVec3f;
    time->ref();
    calc->ref();
    interp->ref();

    // next make the sensors (when wiring the engines
    // together the notify method will try to schedule
    // the sensor, so make sure it exists).
    translationSensor = new SoOneShotSensor(translationSensorCB, this);
    translationSensor->setPriority(0);
    translation0Sensor = new SoFieldSensor(translationSensorCB, this);
    translation0Sensor->setPriority(0);
    translation0Sensor->attach(&translation0);
    translation1Sensor = new SoFieldSensor(translationSensorCB, this);
    translation1Sensor->setPriority(0);
    translation1Sensor->attach(&translation1);
    onSensor = new SoFieldSensor(onSensorCB, this);
    onSensor->setPriority(0);
    onSensor->attach(&on);

    time->on.connectFrom(&on);
    calc->a.connectFrom(&speed);
    calc->b.connectFrom(&time->timeOut);
    // input "c" to the calculation is a phase offset that is
    // set by the rotation sensor callback.
    calc->expression.setValue("oa = 0.5 + 0.5 * sin(a*b*2*M_PI + c)");
    translationSensorCB(this, NULL); // init offset to the default translation
    interp->input0.connectFrom(&translation0);
    interp->input1.connectFrom(&translation1);
    interp->alpha.connectFrom(&calc->oa);
    translation.connectFrom(&interp->output);

    // keep the internal connection, so we can tell
    // later if somebody overrode it.
    translation.getConnectedEngine(internalConnection);

    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor (necessary since inline destructor is too complex)
//
// Use: private

SoShuttle::~SoShuttle()
//
////////////////////////////////////////////////////////////////////////
{
    time->unref();
    calc->unref();
    interp->unref();

    delete translationSensor;
    delete translation0Sensor;
    delete translation1Sensor;
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
SoShuttle::write(SoWriteAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool 		save = translation.isConnectionEnabled();
    SoEngineOutput	*translationSource;

    // if the connection is to the internal engine, disable it
    if (translation.isConnected() &&
	translation.getConnectedEngine(translationSource) &&
	translationSource == internalConnection) {

	translation.enableConnection(FALSE);
    }

    SoTranslation::write(action);

    translation.enableConnection(save);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Override notify to notice when the translation field is touched.
// Can't use field sensor because we need to distinguish
// our own touching of the field from a setValue by the client.
//
// Use: protected

void
SoShuttle::notify(SoNotList *list)
//
////////////////////////////////////////////////////////////////////////
{
    SoNotRec *rec = list->getFirstRec();

    // only interested in a field-to-container notification
    if (rec->getType() == SoNotRec::CONTAINER &&
	rec->getBase() == this &&
	list->getLastField() == &translation) {

	translationSensor->schedule();
    }

    SoTranslation::notify(list);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback is called when somebody sets the translation field.
//
// Use: protected

void
SoShuttle::translationSensorCB(void *data, SoSensor *)
//
////////////////////////////////////////////////////////////////////////
{
    SoShuttle *shuttle = (SoShuttle *) data;
    SbVec3f	v = shuttle->translation.getValue();
    SbVec3f	v0 = shuttle->translation0.getValue();
    SbVec3f	v1 = shuttle->translation1.getValue();

    // find the value of the interpolant that would
    // line up with v.  See where v projects onto the line
    // from v0 to v1
    SbVec3f	diff = v1 - v0;
    float	len = diff.length(), alpha;
    if (len == 0)
	alpha = 0;
    else {
	alpha = (v - v0).dot(diff)/(len*len);
	if (alpha > 1)
	    alpha = 1;
	if (alpha < 0)
	    alpha = 0;
    }

    float a = shuttle->calc->a[0];
    float b = shuttle->calc->b[0];
    // invert the expression: alpha = .5 + .5 sin(a*b*2*PI + c)
    shuttle->calc->c.setValue(asin(2*alpha-1) - a*b*2*M_PI);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback is called when somebody sets the on field
//
// Use: protected

void
SoShuttle::onSensorCB(void *data, SoSensor *)
//
////////////////////////////////////////////////////////////////////////
{
    SoShuttle *shuttle = (SoShuttle *) data;

    if (shuttle->on.getValue()) {
	// if turned on, force the internal connection to be made and enabled,
	// no matter what sort of mucking has happened to it.
	shuttle->translation.enableConnection(TRUE);
	shuttle->translation.connectFrom(&shuttle->interp->output);
	shuttle->translation.getConnectedEngine(shuttle->internalConnection);
    }
}
