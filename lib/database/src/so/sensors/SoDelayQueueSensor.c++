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
 |	SoDelayQueueSensor
 |
 |   Author(s)		: Gavin Bell, Paul Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoDB.h>
#include <Inventor/sensors/SoDelayQueueSensor.h>

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: protected

SoDelayQueueSensor::SoDelayQueueSensor() : SoSensor()
//
////////////////////////////////////////////////////////////////////////
{
    priority  = getDefaultPriority();
    scheduled = FALSE;
    counter   = 0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor with function and data.
//
// Use: protected

SoDelayQueueSensor::SoDelayQueueSensor(SoSensorCB *func, void *data) :
	SoSensor(func, data)
//
////////////////////////////////////////////////////////////////////////
{
    priority  = getDefaultPriority();
    scheduled = FALSE;
    counter   = 0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: protected

SoDelayQueueSensor::~SoDelayQueueSensor()
//
////////////////////////////////////////////////////////////////////////
{
    // Make sure sensor isn't still scheduled to be triggered
    unschedule();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the priority for the sensor. If the sensor is already
//    scheduled, the sensor is unscheduled and rescheduled.
//
// Use: public

void
SoDelayQueueSensor::setPriority(uint32_t pri)
//
////////////////////////////////////////////////////////////////////////
{
    priority = pri;

    if (isScheduled()) {
	unschedule();
	schedule();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Schedules the sensor to be triggered by inserting it into delay queue.
//
// Use: public, virtual

void
SoDelayQueueSensor::schedule()
//
////////////////////////////////////////////////////////////////////////
{
    // Don't do anything if there's no callback function.
    if (func == NULL)
	return;

    // Priority 0 means trigger the sensor immediately. These sensors
    // are added to the queue anyway, and are triggered when
    // notification is done. See comments in SoDB.h.

    if (! scheduled) {
	// Insert into queue
	SoDB::getSensorManager()->insertDelaySensor(this);

	scheduled = TRUE;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Unschedules the sensor.
//
// Use: public, virtual

void
SoDelayQueueSensor::unschedule()
//
////////////////////////////////////////////////////////////////////////
{
    if (scheduled) {
	SoDB::getSensorManager()->removeDelaySensor(this);
    	scheduled = FALSE;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if sensor is scheduled.
//
// Use: public, virtual

SbBool
SoDelayQueueSensor::isScheduled() const
//
////////////////////////////////////////////////////////////////////////
{
    return scheduled;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    By default, DelayQueue sensors are processed when there is idle
//    time OR when the delay queue timeout expires.  Idle sensors,
//    however, override this method and only go off when there is idle
//    time.
//
// Use: public

SbBool
SoDelayQueueSensor::isIdleOnly() const
//
////////////////////////////////////////////////////////////////////////
{
    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Triggers the sensor, calling its callback function. This
//    overrides the method in SoSensor because it has to reset the
//    schedule flag before triggering.
//
// Use: protected

void
SoDelayQueueSensor::trigger()
//
////////////////////////////////////////////////////////////////////////
{
    // Mark the sensor as unscheduled
    scheduled = FALSE;

    // Do the normal triggering stuff
    SoSensor::trigger();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if this sensor should precede sensor s in the delay
//    queue: it must have a lower priority number than s.
//
// Use: private, virtual

SbBool
SoDelayQueueSensor::isBefore(const SoSensor *s) const
//
////////////////////////////////////////////////////////////////////////
{
    // We must assume that s is also an SoDelayQueueSensor
    return (getPriority() <= ((const SoDelayQueueSensor *) s)->getPriority());
}

