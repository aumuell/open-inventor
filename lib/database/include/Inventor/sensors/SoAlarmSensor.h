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

//  -*- C++ -*-

/*
 * Copyright (C) 1990,91,92   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Description:
 |	Timer queue sensor that is scheduled to be triggered at a specific
 |	time, which is specified either absolutely (e.g., 2pm) or
 |	relatively (e.g., 5 minutes from now). Once an alarm sensor is
 |	triggered, it is not rescheduled.
 |
 |   Author(s)		: Gavin Bell, Paul Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_ALARM_SENSOR_
#define  _SO_ALARM_SENSOR_

#include <Inventor/sensors/SoTimerQueueSensor.h>

// C-api: prefix=SoAlarmSens
class SoAlarmSensor : public SoTimerQueueSensor {

  public:
    // Constructors. The second form takes standard callback function and data
    SoAlarmSensor();
    // C-api: name=CreateCB
    SoAlarmSensor(SoSensorCB *func, void *data);

    // Destructor
    virtual ~SoAlarmSensor();

    // Set the time to trigger the sensor. The first method specifies
    // an absolute time, the second a time relative to the current time.
    void		setTime(const SbTime &absTime);
    void		setTimeFromNow(const SbTime &relTime);

    // Returns the time the sensor is scheduled to be triggered. This
    // differs from getTriggerTime() in that this method returns the
    // time the sensor was set to be scheduled, even if it has not yet
    // been scheduled.
    const SbTime &	getTime() const			{ return time; }

    // Overrides the regular schedule() method because we have to set
    // up the trigger time first.
    virtual void	schedule();

  private:
    SbTime		time;		// Time sensor is scheduled for
    SbBool		timeSet;	// Whether time value was set
};

#endif  /* _SO_ALARM_SENSOR_ */
