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
 |	Abstract base class for all sensors that are added to the timer
 |	queue. Each sensor in the timer queue contains an SbTime
 |	indicating when the sensor should be triggered. Sensors in the
 |	timer queue are sorted by their trigger times.
 |	Each subclass defines public methods for specifying when and how
 |	to trigger the sensor.
 |
 |   Author(s)		: Nick Thompson, Paul Strauss, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_TIMER_QUEUE_SENSOR_
#define  _SO_TIMER_QUEUE_SENSOR_

#include <Inventor/sensors/SoSensor.h>
#include <Inventor/SbTime.h>

// C-api: prefix=SoTimerQSens
class SoTimerQueueSensor : public SoSensor {

  public:
    // Constructors. The second form takes standard callback function and data
    SoTimerQueueSensor();
    // C-api: name=createCB
    SoTimerQueueSensor(SoSensorCB *func, void *data);

    // Destructor
    virtual ~SoTimerQueueSensor();

    // Returns the time at which the sensor is scheduled to be
    // triggered. Results are undefined if the sensor is not scheduled.
    // C-api: name=getTrigTime
    const SbTime &	getTriggerTime() const		{ return trigTime; }

    // Scheduling methods
    virtual void	schedule();
    virtual void	unschedule();
    virtual SbBool	isScheduled() const;

  protected:
    // Used by subclasses to set the time at which the sensor is to be
    // triggered.
    void		setTriggerTime(const SbTime &time);

    // Triggers the sensor, calling its callback function. This
    // overrides the method in SoSensor because it has to reset the
    // schedule flag before triggering.
    virtual void	trigger();

    SbBool		scheduled;	// Whether sensor is scheduled

  private:
    SbTime		trigTime;	// Time when sensor is to be triggered

    // Returns TRUE if this sensor should precede sensor s in the delay
    // queue: it must have a lower priority number than s.
    virtual SbBool	isBefore(const SoSensor *s) const;
};

#endif  /* _SO_TIMER_SENSOR_ */
