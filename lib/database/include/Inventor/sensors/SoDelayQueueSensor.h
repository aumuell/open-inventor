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
 |	Abstract base class for all sensors that are added to the delay
 |	queue, which means that they will be executed at some time in the
 |	future (when there is idle time, or when a user-defined timeout
 |	occurs-- see SoDB::setDelaySensorTimeout()).
 |
 |	Sensors in the delay queue are sorted by priorities. A priority of
 |	0 indicates that the sensor is immediate and should be triggered
 |	immediately when it is scheduled. Sensors with other priorities
 |	are sorted by increasing priority.
 |
 |   Author(s)		: Nick Thompson, Paul Strauss, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_DELAY_QUEUE_SENSOR_
#define  _SO_DELAY_QUEUE_SENSOR_

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoDelayQueueSensor
//
//
//////////////////////////////////////////////////////////////////////////////

#include <Inventor/sensors/SoSensor.h>

// C-api: prefix=SoDelQSens
class SoDelayQueueSensor : public SoSensor {

  public:
    // Constructors. The second form takes standard callback function and data
    SoDelayQueueSensor();
    // C-api: name=createCB
    SoDelayQueueSensor(SoSensorCB *func, void *data);

    // Destructor
    virtual ~SoDelayQueueSensor();

    // Sets/returns the priority for the sensor. If the priority is
    // changed to 0 (immediate) and it is already scheduled, the
    // sensor is unscheduled and triggered.
    // C-api: name=setPri
    void		setPriority(uint32_t pri);
    // C-api: name=getPri
    uint32_t		getPriority() const		{ return priority; }

    // Returns default sensor priority
    static uint32_t getDefaultPriority()		{ return 100; }

    // Scheduling methods
    virtual void	schedule();
    virtual void	unschedule();
    virtual SbBool	isScheduled() const;

    // This method is overriden by IdleSensors to tell sensor manager
    // that they should only be processed when there really is idle
    // time (and not when the delay queue timeout expires).
    virtual SbBool	isIdleOnly() const;
  SoINTERNAL public:
    // Set/inquire counter when sensor was last triggered. This allows
    // the sensor manager to avoid triggering a sensor more than once
    // during the same call to processDelayQueue().
    void		setCounter(uint32_t c)		{ counter = c;  }
    uint32_t		getCounter() const		{ return counter; }

  protected:
    // Triggers the sensor, calling its callback function. This
    // overrides the method in SoSensor because it has to reset the
    // schedule flag before triggering.
    virtual void	trigger();

    SbBool		scheduled;	// Whether sensor is scheduled

  private:
    uint32_t		priority;	// Sensor priority
    uint32_t		counter;	// Used by sensor manager

    // Returns TRUE if this sensor should precede sensor s in the delay
    // queue: it must have a lower priority number than s.
    virtual SbBool	isBefore(const SoSensor *s) const;
};

#endif  /* _SO_DELAY_QUEUE_SENSOR_ */
