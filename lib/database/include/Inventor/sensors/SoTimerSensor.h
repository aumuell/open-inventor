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
 |	Timer sensors trigger repeatedly at regular
 |	intervals, starting at a given base time. By default, the base
 |	time is the current time (at the time the sensor is scheduled or
 |	rescheduled) and the interval is 1/30 of a second; both of these
 |	values may be changed.  If more than one interval passes while
 |	sensors are being triggered, then the extra intervals will be lost
 |	(timers always reschedule themselves to occur in the future).
 |	If more than one timer has expired (and therefore more than one
 |	needs to be rescheduled), all of them are first triggered, and are
 |	then rescheduled relative to the current time.  The reschedule()
 |	method is called by the sensor manager to accomplish this (see the
 |	comments there about saturating the timer queue).
 |
 |   Author(s)		: Nick Thompson, Paul Strauss, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_TIMER_SENSOR_
#define  _SO_TIMER_SENSOR_

#include <Inventor/sensors/SoTimerQueueSensor.h>

// C-api: prefix=SoTimerSens
class SoTimerSensor : public SoTimerQueueSensor {

  public:
    // Constructors. The second form takes standard callback function and data
    SoTimerSensor();
    // C-api: name=CreateCB
    SoTimerSensor(SoSensorCB *func, void *data);

    // Destructor
    virtual ~SoTimerSensor();

    // Sets/returns the base time and interval length. The base time
    // is undefined unless it has been specified by the user or the
    // sensor is currently scheduled.
    void		setBaseTime(const SbTime &base)  { baseTime = base;
						           baseTimeSet = TRUE;}
    void		setInterval(const SbTime &intvl) { interval = intvl; }
    const SbTime &	getBaseTime() const		 { return baseTime;  }
    const SbTime &	getInterval() const		 { return interval;  }

    // Overrides the regular schedule() method because we have to set
    // up the trigger time first.
    virtual void	schedule();

    // Overrides the regular unschedule() because the timer could be
    // in either the timer queue or the waiting-to-be-rescheduled
    // queue, depending on the state of the 'triggering' flag.
    virtual void	unschedule();

  SoINTERNAL public:

    // Reschedule this timer relative to the given time
    void		reschedule(const SbTime &);

  private:
    SbTime		baseTime;	// Base time for scheduling triggering
    SbTime		interval;	// Interval between triggering
    SbBool		baseTimeSet;	// Whether base time was set by user

    SbBool		triggering;	// True if in trigger() code

    // Triggers the sensor, calling its callback function. This
    // overrides the method in SoSensor because it has to reschedule
    // itself for the next interval after triggering.
    virtual void	trigger();
};

#endif  /* _SO_TIMER_SENSOR_ */
