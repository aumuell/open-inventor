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
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Description:
 |	Manager for scheduling and triggering sensors. The manager
 |	maintains a queue of delay sensors (derived from SoDelayQueueSensor)
 |	and timer sensors (derived from SoTimerQueueSensor).
 |
 |   Classes
 |	SoSensorManager
 |
 |   Author(s)		: Nick Thompson, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_SENSOR_MANAGER_
#define  _SO_SENSOR_MANAGER_

#include <Inventor/misc/SoBasic.h>
#include <Inventor/SbTime.h>

class SoSensor;
class SoDelayQueueSensor;
class SoTimerQueueSensor;
class SoTimerSensor;
class SoAlarmSensor;

SoINTERNAL class SoSensorManager {
  public:

    // Constructor, destructor
    SoSensorManager();
    ~SoSensorManager();

    // Set up a function to call when either queue has a sensor added
    // or removed
    void		setChangedCallback(void (*func)(void *), void *data)
	{ changedFunc = func; changedFuncData = data; }

    // Insert/remove an delay or timer event into/from the appropriate queue
    void		insertDelaySensor(SoDelayQueueSensor *s);
    void		insertTimerSensor(SoTimerQueueSensor *s);
    void		removeDelaySensor(SoDelayQueueSensor *s);
    void		removeTimerSensor(SoTimerQueueSensor *s);

    // Timers are all rescheduled at the same time, after they have
    // been triggered.  This avoids timer queue saturation.
    void		rescheduleTimer(SoTimerSensor *s);
    void		removeRescheduledTimer(SoTimerQueueSensor *s);

    // Set/get delay queue sensors (OneShot, Node, Path, etc)
    // timeout value.  Delay sensor go off whenever there is idle time
    // or when the timeout expires.
    void		setDelaySensorTimeout(const SbTime &t);
    const SbTime	&getDelaySensorTimeout();

    // Process all sensors in the delay queue.  The isIdle flag should
    // be TRUE if the delay queue is being processed because there is
    // idle time; it should be false if it is being processed because
    // the delay sensor timeout has expired.
    void		processDelayQueue(SbBool isIdle);

    // Process all immediate (priority 0) sensors in the delay queue
    void		processImmediateQueue();

    // Process appropriate sensors in the timer queue based on current time
    void		processTimerQueue();

    // Returns TRUE if there is at least one sensor in the delay queue
    SbBool		isDelaySensorPending()
	    			{ return (delayQueue != NULL); }

    // Returns TRUE if there is at least one sensor in the timer
    // queue. If there is one, the time of the first sensor is placed
    // in tm.
    SbBool		isTimerSensorPending(SbTime &tm);

    // Just like UNIX select() call, but checks sensor queues while
    // waiting for events.
    int			doSelect(int nfds, fd_set *readfds, fd_set *writefds,
				 fd_set *exceptfds,
				 struct timeval *userTimeOut);
  private:
    SoSensor		*delayQueue;	// Head of delay sensor queue
    SoSensor		*timerQueue;	// Head of timer sensor queue
    SoSensor		*rescheduleQueue;// Timers ready to be rescheduled

    // These are used to process the delay queue every once in a
    // while:
    SbTime		delayQTimeout;
    SoAlarmSensor	*delayQTimeoutSensor;
    static void		delayQTimeoutCB(void *, SoSensor *);

    // Non-zero when processing a queue.  The changedCallback will not
    // be called when we are in the middle of processing a queue, and
    // the immediate queue uses this to avoid recursion.  Because a
    // timer sensor might decide to process the delay queue in its
    // trigger method, this is an integer instead of a simple Boolean.
    int			processingQueue;

    uint32_t		delayQCount;	// Avoids loops processing delay queue

    // Insert/remove an entry into/from the queue with the given head
    void		insert(SoSensor *s, SoSensor *&head);
    void		remove(SoSensor *s, SoSensor *&head);

    // Callback which is called whenever a sensor is inserted or removed
    void		(*changedFunc)(void *);
    void		*changedFuncData;

    // Calls the changed callback, if there is one
    void		notifyChanged();
};

#endif /* _SO_SENSOR_MANAGER_ */
