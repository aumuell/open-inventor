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
 |	SoTimerSensor
 |
 |   Author(s)		: Nick Thompson, Paul Strauss, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoDB.h>
#include <Inventor/sensors/SoTimerSensor.h>

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoTimerSensor::SoTimerSensor()
//
////////////////////////////////////////////////////////////////////////
{
    baseTimeSet = FALSE;
    interval    = SbTime(1.0 / 30.0);
    triggering = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor with function and data
//
// Use: public

SoTimerSensor::SoTimerSensor(SoSensorCB *func, void *data) :
	SoTimerQueueSensor(func, data)
//
////////////////////////////////////////////////////////////////////////
{
    baseTimeSet = FALSE;
    interval    = SbTime(1.0 / 30.0);
    triggering = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoTimerSensor::~SoTimerSensor()
//
////////////////////////////////////////////////////////////////////////
{
    // Make sure sensor isn't still scheduled to be triggered
    unschedule();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Triggers the sensor, and adds it to the to-be-rescheduled queue
//
// Use: public

void
SoTimerSensor::trigger()
//
////////////////////////////////////////////////////////////////////////
{
    triggering = TRUE;

    // Add to the to-be-rescheduled queue
    SoDB::getSensorManager()->rescheduleTimer(this);

    // Do the normal triggering stuff.  If the callback calls
    // schedule(), it will be ignored (because triggering = TRUE).  If
    // the callback calls unschedul(), the timer will be removed from
    // the rescheduleQueue.
    // Note that SoTimerQueueSensor::trigger is NOT called here,
    // because timers are not automatically unscheduled when they
    // trigger (see the corresponding logic in the schedule() and
    // unschedule() methods).
    SoSensor::trigger();

    // Note: the triggering process isn't considered over until the
    // sensor has been rescheduled or unscheduled.
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides the regular schedule() method because we have to set
//    up the trigger time first.
//
// Use: public

void
SoTimerSensor::schedule()
//
////////////////////////////////////////////////////////////////////////
{
    SbTime	now = SbTime::getTimeOfDay();

    // Set base time to current time if it was not set explicitly
    if (! baseTimeSet)
	baseTime = now;

    // If we are in the middle of the triggering this sensor, don't
    // bother setting the trigger time or adding it to the timer
    // queue; that will be done after trigger() returns and the
    // 'to-be-scheduled' queue is done.  However, if we are triggering
    // and we have been unscheduled, that means unschedule() was
    // called inside the trigger() callback; in this case, we should
    // add this sensor back onto the to-be-rescheduled queue.
    if (triggering) {
	if (!scheduled) {
	    SoDB::getSensorManager()->rescheduleTimer(this);
	}
	// In any case, wait for the sensor to be scheduled after it
	// has been triggered.
	return;
    }

    // Set the timer to trigger at the base time. If the base time is
    // before the current time, add sufficient whole intervals to get
    // it past the current time.
    setTriggerTime(baseTime +
		   interval * ceil((now - baseTime) / interval + 0.0000001));

    // Do standard scheduling stuff
    SoTimerQueueSensor::schedule();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Unschedules the sensor.  If this is called from inside a
//    trigger(), the sensor is removed from the to-be-rescheduled
//    queue (it has already been removed from the timer queue).
//
// Use: public

void
SoTimerSensor::unschedule()
//
////////////////////////////////////////////////////////////////////////
{
    if (triggering) {
	SoDB::getSensorManager()->removeRescheduledTimer(this);
	scheduled = FALSE;
	triggering = FALSE;
    }
    else SoTimerQueueSensor::unschedule();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reschedules the sensor
//
// Use: internal public

void
SoTimerSensor::reschedule(const SbTime &now)
//
////////////////////////////////////////////////////////////////////////
{
    // This will be set to TRUE by
    // SoTimerQueueSensor::schedule()
    scheduled = FALSE;

    triggering = FALSE;

    // Set base time to current time if it was not set explicitly
    if (! baseTimeSet) {
	baseTime = now;

	setTriggerTime(now+interval+0.0000001);
    }
    else {
	// Reschedule to trigger again after the next interval. If we
	// missed some triggers because it took too long to trigger the
	// sensor, they are lost forever. The next trigger will take place
	// at the end of the next interval from now. Otherwise, this
	// sensor would just saturate the queue.
	SbTime triggerTime = baseTime 
	    + interval * ceil((now - baseTime) / interval
			  + 0.0000001);

	setTriggerTime(triggerTime);
    }

    SoTimerQueueSensor::schedule();
}
