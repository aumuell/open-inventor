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
 |	SoTimerQueueSensor
 |
 |   Author(s)		: Paul Strauss, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */
//////////////////////////////////////////////////////////////////////////////

#include <Inventor/SoDB.h>
#include <Inventor/sensors/SoTimerQueueSensor.h>

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: protected

SoTimerQueueSensor::SoTimerQueueSensor() : SoSensor()
//
////////////////////////////////////////////////////////////////////////
{
    scheduled = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor with function and data.
//
// Use: protected

SoTimerQueueSensor::SoTimerQueueSensor(SoSensorCB *func, void *data) :
	SoSensor(func, data)
//
////////////////////////////////////////////////////////////////////////
{
    scheduled = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: protected

SoTimerQueueSensor::~SoTimerQueueSensor()
//
////////////////////////////////////////////////////////////////////////
{
    // Make sure sensor isn't still scheduled to be triggered
    unschedule();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Schedules the sensor to be triggered by inserting it into timer queue.
//
// Use: public, virtual

void
SoTimerQueueSensor::schedule()
//
////////////////////////////////////////////////////////////////////////
{
    // Don't do anything if there's no callback function or it is
    // already scheduled:
    if (func == NULL || scheduled == TRUE)
	return;

    // Insert into queue
    SoDB::getSensorManager()->insertTimerSensor(this);

    scheduled = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Unschedules the sensor.
//
// Use: public, virtual

void
SoTimerQueueSensor::unschedule()
//
////////////////////////////////////////////////////////////////////////
{
    if (scheduled) {
	// Remove from queue
	SoDB::getSensorManager()->removeTimerSensor(this);

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
SoTimerQueueSensor::isScheduled() const
//
////////////////////////////////////////////////////////////////////////
{
    return scheduled;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Set the trigger time.  If the sensor is scheduled, this
//    unschedules and reschedules it.
//
// Use: protected

void
SoTimerQueueSensor::setTriggerTime(const SbTime &time)
//
////////////////////////////////////////////////////////////////////////
{
    trigTime = time;
    if (isScheduled()) {
	SoDB::getSensorManager()->removeTimerSensor(this);
	SoDB::getSensorManager()->insertTimerSensor(this);
    }
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
SoTimerQueueSensor::trigger()
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
//    Returns TRUE if this sensor should precede sensor s in the timer
//    queue: it must have an earlier trigger time.
//
// Use: private, virtual

SbBool
SoTimerQueueSensor::isBefore(const SoSensor *s) const
//
////////////////////////////////////////////////////////////////////////
{
    // We must assume that s is also an SoTimerQueueSensor
    return (getTriggerTime() <=
	    ((const SoTimerQueueSensor *) s)->getTriggerTime());
}

