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
 |	SoAlarmSensor
 |
 |   Author(s)		: Gavin Bell, Paul Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/errors/SoDebugError.h>
#include <Inventor/sensors/SoAlarmSensor.h>

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoAlarmSensor::SoAlarmSensor()
//
////////////////////////////////////////////////////////////////////////
{
    timeSet = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor with function and data
//
// Use: public

SoAlarmSensor::SoAlarmSensor(SoSensorCB *func, void *data) :
	SoTimerQueueSensor(func, data)
//
////////////////////////////////////////////////////////////////////////
{
    timeSet = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoAlarmSensor::~SoAlarmSensor()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the trigger time to the given absolute time.
//
// Use: public

void
SoAlarmSensor::setTime(const SbTime &absTime)
//
////////////////////////////////////////////////////////////////////////
{
    time = absTime;
    timeSet = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the trigger time to the given time, relative to the current
//    time.
//
// Use: public

void
SoAlarmSensor::setTimeFromNow(const SbTime &relTime)
//
////////////////////////////////////////////////////////////////////////
{
    setTime(SbTime::getTimeOfDay() + relTime);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides the regular schedule() method because we have to set
//    up the trigger time first.
//
// Use: public

void
SoAlarmSensor::schedule()
//
////////////////////////////////////////////////////////////////////////
{
    // It is an error to schedule an alarm sensor that has not had its
    // time set
    if (! timeSet) {
	SoDebugError::post("SoAlarmSensor::schedule",
			   "Alarm time was never set");
	return;
    }

    // Set the timer to trigger at the base time
    setTriggerTime(time);

    // Do standard scheduling stuff
    SoTimerQueueSensor::schedule();
}

