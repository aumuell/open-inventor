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
 |   $Revision: 1.3 $
 |
 |   Description:
 |	Defines the (internal) SoSensorManager class
 |
 |   Author(s)		: Nick Thompson, Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/sensors/SoSensorManager.h>
#include <Inventor/sensors/SoSensors.h>
#include <Inventor/errors/SoDebugError.h>
#ifdef __sgi
#include <osfcn.h>
#endif // __sgi
#ifdef __APPLE__
#include <unistd.h>
#endif // __APPLE__

#ifdef DEBUG
#include <SoDebug.h>
static SbTime startTime;
#endif

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoSensorManager::SoSensorManager()
//
////////////////////////////////////////////////////////////////////////
{
    changedFunc     = NULL;
    changedFuncData = NULL;

    delayQueue  = NULL;
    timerQueue = NULL;
    rescheduleQueue = NULL;

    delayQTimeout = SbTime::zero();
    delayQTimeoutSensor = new SoAlarmSensor(delayQTimeoutCB, this);
#ifdef DEBUG
    if (SoDebug::GetEnv("IV_DEBUG_SENSORS")) {
	SoDebug::NamePtr("delayQTimeoutSensor", delayQTimeoutSensor);
	startTime = SbTime::getTimeOfDay();
    }
#endif

    processingQueue = 0;

    delayQCount = 0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: public

SoSensorManager::~SoSensorManager()
//
////////////////////////////////////////////////////////////////////////
{
    delete delayQTimeoutSensor;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Insert a delay queue sensor into the delay queue.
//
// Use: public

void
SoSensorManager::insertDelaySensor(SoDelayQueueSensor *s)
//
////////////////////////////////////////////////////////////////////////
{
    // If the sensor's priority isn't zero, AND the delay queue
    // timeout isn't zero, and the delay queue sensor isn't already
    // scheduled, get the delay queue timeout ready to go
    // off.
    if ((s->getPriority() != 0) && !delayQTimeoutSensor->isScheduled()
	&& (delayQTimeout != SbTime::zero())) {

	delayQTimeoutSensor->unschedule();
	delayQTimeoutSensor->setTimeFromNow(delayQTimeout);
	delayQTimeoutSensor->schedule();

    }
    insert(s, delayQueue);

    if (s->getPriority() != 0) {
#ifdef DEBUG
	if (SoDebug::GetEnv("IV_DEBUG_SENSORS")) {
	    SoDebug::RTPrintf("Inserted delay queue sensor:  "
			      "Name: %s, priority: %d\n",
			      SoDebug::PtrName(s), s->getPriority());
	}
#endif
	// Call changed callbacks if necessary
	notifyChanged();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Insert a timer queue sensor into the timer queue.
//
// Use: public

void
SoSensorManager::insertTimerSensor(SoTimerQueueSensor *s)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (SoDebug::GetEnv("IV_DEBUG_SENSORS")) {
	SoDebug::RTPrintf("Inserting timer queue sensor:  "
			  "Name: %s, trigger time: %g\n",
			  SoDebug::PtrName(s),
			  (s->getTriggerTime()-startTime).getValue());
    }
#endif
    insert(s, timerQueue);

    // Call changed callbacks if necessary
    notifyChanged();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Remove a delay queue sensor from the delay queue.
//
// Use: public

void
SoSensorManager::removeDelaySensor(SoDelayQueueSensor *s)
//
////////////////////////////////////////////////////////////////////////
{
    remove(s, delayQueue);

    if (s->getPriority() != 0) {
	// Call changed callbacks if necessary
	notifyChanged();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Remove a timer queue sensor from the timer queue.
//
// Use: public

void
SoSensorManager::removeTimerSensor(SoTimerQueueSensor *s)
//
////////////////////////////////////////////////////////////////////////
{
    remove(s, timerQueue);

    // Call changed callbacks if necessary
    notifyChanged();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds a timer to the 'to-be-rescheduled' queue.
//
// Use: public

void
SoSensorManager::rescheduleTimer(SoTimerSensor *s)
//
////////////////////////////////////////////////////////////////////////
{
    insert(s, rescheduleQueue);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Remove a timer from the 'to-be-rescheduled' queue.
//
// Use: public

void
SoSensorManager::removeRescheduledTimer(SoTimerQueueSensor *s)
//
////////////////////////////////////////////////////////////////////////
{
    remove(s, rescheduleQueue);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the delay sensor queue timeout.
//
// Use: public

void
SoSensorManager::setDelaySensorTimeout(const SbTime &t)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (SoDebug::GetEnv("IV_DEBUG_SENSORS")) {
	SoDebug::RTPrintf("Setting delay sensor timeout to %g\n",
			  t.getValue());
    }
#endif

    delayQTimeout = t;

    if (t == SbTime::zero()) {
	delayQTimeoutSensor->unschedule();
    } else {
	if (delayQueue != NULL) {
	    delayQTimeoutSensor->setTimeFromNow(t);
	    delayQTimeoutSensor->schedule();
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Gets the delay sensor queue timeout.
//
// Use: public

const SbTime &
SoSensorManager::getDelaySensorTimeout()
//
////////////////////////////////////////////////////////////////////////
{
    return delayQTimeout;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback function from alarm sensor that processed the delay
//    queue every once in a while.
//
// Use: private, static

void
SoSensorManager::delayQTimeoutCB(void *data, SoSensor *)

//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (SoDebug::GetEnv("IV_DEBUG_SENSORS")) {
	SoDebug::RTPrintf("Processing delay queue; timeout expired\n");
    }
#endif

    SoSensorManager *s = (SoSensorManager *)data;
    s->processDelayQueue(FALSE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Processes all sensors in the delay queue.
//
// Use: public

void
SoSensorManager::processDelayQueue(SbBool isIdle)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (SoDebug::GetEnv("IV_DEBUG_SENSORS")) {
	SoDebug::RTPrintf("Processing delay queue (isIdle:%d)\n", isIdle);
    }
#endif

    SoSensor		*prev;
    SoDelayQueueSensor	*first;

    if (delayQueue == NULL)
	return;

    ++processingQueue;

    // We want to process all of the sensors that are currently in the
    // queue. But we have to be careful to NOT process sensors that
    // are added to the queue while we are processing it. Therefore,
    // we first mark all of the sensors in the queue as being current.
    // Then we process only the current sensors. When there are no
    // current sensors left, we're done. NOTE: removing a sensor marks
    // it as being not-current; inserting a new sensor does not mark
    // it as current.
#ifdef DEBUG
    if (((SoDelayQueueSensor *) delayQueue)->getPriority() == 0)
	SoDebugError::post("SoSensorManager::processDelayQueue",
			   "Immediate sensor in delay queue!!!!");
#endif

    // We want to process all of the sensors currently in the queue.
    // To avoid looping infinitely here, we do not trigger any sensor
    // more than once. To detect this, we keep a running counter
    // every time this method is called. When a sensor is triggered
    // here, it is marked with the current counter. If a sensor in the
    // queue is so marked, we know it has already been triggered once,
    // and we can skip it the next time.

    // Increment the count
    delayQCount++;

    while (delayQueue != NULL) {

	// Find first sensor that has not yet been triggered (its
	// counter is not the same as our current counter), saving
	// previous sensor in queue (to keep links accurate)
	prev = NULL;

	first = (SoDelayQueueSensor *) delayQueue;
	while (first != NULL) {
	    // Process this sensor IF:
	    // -- it wasn't processed before, and
	    // -- it is a OneShot, or an Idle and we have idle time
	    if (first->getCounter() != delayQCount &&
		(isIdle || !first->isIdleOnly())) break;
	    prev = first;
	    first = (SoDelayQueueSensor *) first->getNextInQueue();
	}

	// If we didn't find one, we're done!
	if (first == NULL)
	    break;

	// Remove sensor from queue
	if (prev == NULL)
	    delayQueue = first->getNextInQueue();
	else
	    prev->setNextInQueue(first->getNextInQueue());
	first->setNextInQueue(NULL);

	// Mark the sensor with the current counter so it won't
	// trigger again immediately if it gets added to the queue again
	first->setCounter(delayQCount);

#ifdef DEBUG
	if (SoDebug::GetEnv("IV_DEBUG_SENSORS")) {
	    SoDebug::RTPrintf("Triggering delay queue sensor  "
			      "Name: %s, priority: %d\n",
			      SoDebug::PtrName(first), first->getPriority());
	    
	}
#endif
	// Trigger the sensor. NOTE: this can add new entries to the
	// queue or remove entries.
	((SoSensor *) first)->trigger();
    }
    --processingQueue;

    // If there are still sensors left in the queue after processing,
    // setup the delay queue timeout:
    if ((delayQueue != NULL) && (delayQTimeout != SbTime::zero())) {
	delayQTimeoutSensor->setTimeFromNow(delayQTimeout);
	delayQTimeoutSensor->schedule();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Processes all immediate (priority 0) sensors in the delay queue.
//
// Use: public

void
SoSensorManager::processImmediateQueue()
{
    SoDelayQueueSensor	*first;
    static int		processingImmediate = FALSE;

    if (delayQueue == NULL)
	return;

    // Triggering an immediate sensor may initiate notification,
    // calling this method recursively. To avoid doing extra work,
    // just return if we are already processing immediate sensors.
    if (processingImmediate)
	return;

    processingImmediate = TRUE;
    processingQueue++;

    // NOTE: all immediate sensors are processed as added, even if
    // this causes an infinite loop. (That's what they're supposed to
    // do.) No counter check is made here.

    while (delayQueue != NULL &&
	   ((SoDelayQueueSensor *) delayQueue)->getPriority() == 0) {

	first = (SoDelayQueueSensor *) delayQueue;
	delayQueue = delayQueue->getNextInQueue();

	first->setNextInQueue(NULL);

#if 0
	if (SoDebug::GetEnv("IV_DEBUG_SENSORS")) {
	    SoDebug::RTPrintf("Triggering immediate sensor  "
			      "Name: %s, priority: %d\n",
			      SoDebug::PtrName(first), first->getPriority());
	
	}
#endif
	((SoSensor *) first)->trigger();
    }

    processingImmediate = FALSE;
    processingQueue--;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Processes all sensors in the timer queue that are before the
//    current time.
//
// Use: public

void
SoSensorManager::processTimerQueue()
//
////////////////////////////////////////////////////////////////////////
{
    ++processingQueue;
    SoSensor	*first;

    // NOTE: Infinite loops are possible if timerQueue sensors
    // reschedule themselves inside their trigger() routine.  This
    // isn't possible with Timer sensors, because they are added to
    // the rescheduleQueue and can only be triggered once per call to
    // processTimerQueue.
    // The assumption here is that if the user reschedules an alarm,
    // we assume they know what they are doing.

    // Stop if no sensors left or next sensor really is after now
    while (timerQueue != NULL &&
	   ((SoTimerQueueSensor *) timerQueue)->getTriggerTime() <= 
	       SbTime::getTimeOfDay()) {

	// Get pointer to first sensor
	first = timerQueue;

	// Remove sensor from queue.
	timerQueue = timerQueue->getNextInQueue();
	first->setNextInQueue(NULL);

#ifdef DEBUG
	if (SoDebug::GetEnv("IV_DEBUG_SENSORS")) {
	    SoDebug::RTPrintf("Triggering timer sensor  "
		      "Name: %s, trig time: %g\n",
		      SoDebug::PtrName(first),
		      (((SoTimerQueueSensor *)first)->
			      getTriggerTime()-startTime).getValue());
	}
#endif
	// Trigger the sensor.
	first->trigger();
    }

    //
    // Timers used to reschedule themselves at the end of their
    // trigger() methods.  However, this can lead to timer queue
    // saturation if there are two sensors that take a long time in
    // their trigger() methods.  The first sensor will schedule itself
    // for sometime in the future, but by the time the second sensor
    // is done triggering (and scheduling itself in the future), the
    // first sensor is ready to fire.
    // By scheduling all timers relative to one time, AFTER they have
    // been triggered, this problem is avoided.
    //
    if (rescheduleQueue != NULL) {
	SbTime	now = SbTime::getTimeOfDay();

	// Now reschedule timers, relative to now

	while (rescheduleQueue != NULL) {
	    SoTimerSensor *timer = (SoTimerSensor *)rescheduleQueue;
	    // Remove sensor from queue
	    rescheduleQueue = rescheduleQueue->getNextInQueue();
	    timer->setNextInQueue(NULL);

	    timer->reschedule(now);
	}
    }	

    --processingQueue;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if there is at least one sensor in the timer queue,
//    and returns the time of the first sensor in tm.
//
// Use: public

SbBool
SoSensorManager::isTimerSensorPending(SbTime &tm)
//
////////////////////////////////////////////////////////////////////////
{
    if (timerQueue != NULL)
	tm = ((SoTimerQueueSensor *) timerQueue)->getTriggerTime();
    return (timerQueue != NULL);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Just like U*X select() call, but does our tasks while waiting.
//
// Use: public

int
SoSensorManager::doSelect(int nfds, fd_set *readfds, fd_set *writefds,
			  fd_set *exceptfds, struct timeval *userTimeOut)
//
////////////////////////////////////////////////////////////////////////
{

// Possible sources of events
#define EVENT_SOURCE_FD			0x01
#define EVENT_SOURCE_TIMER		0x02
#define EVENT_SOURCE_IDLE		0x04
#define EVENT_SOURCE_USER_TIMEOUT	0x08
#define EVENT_SOURCE_TIMEOUT		0x10

    SbTime		now, userDeadline, timeOut, t;
    fd_set		readfdsTmp, writefdsTmp, exceptfdsTmp;
    int32_t		eventSources;
    struct timeval	tvStruct, *tv;
    int			selectReturn;

    now = SbTime::getTimeOfDay();

    if (userTimeOut != NULL)
	userDeadline = now + SbTime(userTimeOut);

    while (1) {

	eventSources = 0;

	// The priority of stuff is:
	//
	// 1. Timers.  The set of pending timers will be processed before
	// an event (or user timeout) is returned.  Note that if
	// timers saturate the queue (e.g. there is a timer with an
	// interval of zero), only one event will be processed between
	// processing the timer queue.
	//
	// 2. User timeout.  See the comment above.
	//
	// 3. Events.  See the comment above.
	//
	// 4. Idle/Delay processes.  OneShot sensors are processed
	// whenver there is idle time (there are no timers or events
	// pending) OR when a user-specifiable timeout occurs.  Idle
	// sensors are processed only when there is idle time.
	//

	// If there are any delay sensors around, set the appropriate bit
	// and set the timeout to 0
	if (isDelaySensorPending()) {
	    timeOut = SbTime::zero();
	    eventSources |= EVENT_SOURCE_IDLE | EVENT_SOURCE_TIMEOUT;
	}

	// If there are timers around, see when they are supposed to occur.
	if (isTimerSensorPending(t)) {
	    if (t < now) {
		// If there are timers pending, set the appropriate
		// bit and set the timeout to 0.
		timeOut = SbTime::zero();
		eventSources = EVENT_SOURCE_TIMER | EVENT_SOURCE_TIMEOUT;
	    }
	    else {
		// If there are timer events in the future:
		if (eventSources & EVENT_SOURCE_IDLE) {
		    // If there are delay sensors, they will time out
		    // before us, so do nothing.
		}
		else {
		    // Figure out the appropriate timeout and set the
		    // right bits.
		    timeOut = t - now;
		    eventSources = EVENT_SOURCE_TIMER |
			EVENT_SOURCE_TIMEOUT;
		}
	    }
	}

	// if there is a user timeout, set the appropriate bit and
	// timeout value.
	if (userTimeOut != NULL) {
	    if (userDeadline <= now) {
		// If the user deadline has passed, set the appropriate
		// bit and set the timeout to 0.
		timeOut = SbTime::zero();
		eventSources = EVENT_SOURCE_USER_TIMEOUT |
		    EVENT_SOURCE_TIMEOUT;
	    }
	    else {
		// If there is a deadline in the future, find the
		// appropriate timeout interval and set the timeout bit.
		if (eventSources & EVENT_SOURCE_TIMEOUT) {
		    // something is already scheduled to timeout.
		    // see if the user timeout is earlier
		    if (userDeadline - now < timeOut) {
			timeOut = userDeadline - now;
			eventSources = EVENT_SOURCE_USER_TIMEOUT |
			    EVENT_SOURCE_TIMEOUT;
		    }
		}
		else {
		    // Nothing is scheduled to time out yet.
		    // Use the user's timeout value.
		    timeOut = userDeadline - now;
		    eventSources = EVENT_SOURCE_USER_TIMEOUT |
			EVENT_SOURCE_TIMEOUT;
		}
	    }
	}

	if (eventSources & EVENT_SOURCE_TIMEOUT) {
	    tv = &tvStruct;
	    timeOut.getValue(tv);
	}
	else
	    tv = NULL;

	// Save the fdset data
	if (readfds != NULL)
	    readfdsTmp = *readfds;
	else
	    FD_ZERO(&readfdsTmp);

	if (writefds != NULL)
	    writefdsTmp = *writefds;
	else
	    FD_ZERO(&writefdsTmp);

	if (exceptfds != NULL)
	    exceptfdsTmp = *exceptfds;
	else
	    FD_ZERO(&exceptfdsTmp);

	selectReturn = select(nfds, &readfdsTmp, &writefdsTmp,
			      &exceptfdsTmp, tv);
	if (selectReturn < 0) {
	    perror("select");
	    return selectReturn;
	}
	// if there is anything ready, set the appropriate bits
	else if (selectReturn > 0)
	    eventSources |= EVENT_SOURCE_FD;

	// The order of these if statements determines priority of
	// event delivery
	if (eventSources & EVENT_SOURCE_USER_TIMEOUT ||
	    eventSources & EVENT_SOURCE_FD) {

	    // set the fd masks
	    if (readfds != NULL)
		*readfds = readfdsTmp;
	    if (writefds != NULL)
		*writefds = writefdsTmp;
	    if (exceptfds != NULL)
		*exceptfds = exceptfdsTmp;

	    // Make sure timers get fired even if the event queue is swamped
	    if (eventSources & EVENT_SOURCE_TIMER)
		processTimerQueue();

	    return selectReturn;
	}

	else if (eventSources & EVENT_SOURCE_TIMER)
	    processTimerQueue();

	else if (eventSources & EVENT_SOURCE_IDLE)
	    processDelayQueue(TRUE);

	// Going around again... update our notion of current time.
	now		= SbTime::getTimeOfDay();
    }
}

#undef EVENT_SOURCE_FD
#undef EVENT_SOURCE_TIMER
#undef EVENT_SOURCE_IDLE
#undef EVENT_SOURCE_USER_TIMEOUT
#undef EVENT_SOURCE_TIMEOUT

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Inserts a sensor into one of the queues. The head of the queue
//    is passed in by reference.
//
// Use: public

void
SoSensorManager::insert(SoSensor *s, SoSensor *&head)
//
////////////////////////////////////////////////////////////////////////
{
    SoSensor *prev = NULL, *next;

    // Loop through all sensors in queue
    for (next = head; next != NULL; next = next->getNextInQueue()) {

	// Stop if the new sensor should be inserted before the
	// current one in the queue. Use the virtual isBefore() method
	// which works for all types of sensors.
	if (s->isBefore(next))
	    break;

	prev = next;
    }

    // Insert sensor into queue
    s->setNextInQueue(next);
    if (prev == NULL)
	head = s;
    else
	prev->setNextInQueue(s);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Removes a sensor from one of the queues. The head of the queue
//    is passed in by reference.
//
// Use: public

void
SoSensorManager::remove(SoSensor *s, SoSensor *&head)
//
////////////////////////////////////////////////////////////////////////
{
    SoSensor *prev = NULL, *cur;

    // Loop through all sensors in queue, looking for s
    for (cur = head; cur != NULL; cur = cur->getNextInQueue()) {

	if (cur == s)
	    break;

	prev = cur;
    }

    // Remove sensor from queue
    if (prev == NULL)
	head = s->getNextInQueue();
    else
	prev->setNextInQueue(s->getNextInQueue());
    s->setNextInQueue(NULL);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Calls callback functions to tell them about changes to the
//    sensor queues.  The callbacks are for higher-level toolkits
//    (like Xt) so timeouts can be set.
//
// Use: public

void
SoSensorManager::notifyChanged()
//
////////////////////////////////////////////////////////////////////////
{
    if (changedFunc != NULL && !processingQueue) {
	changedFunc(changedFuncData);

#ifdef DEBUG
	if (SoDebug::GetEnv("IV_DEBUG_SENSORS")) {
	    SoDebug::RTPrintf("SoSensorManager::notifyChanged()\n");
	}
#endif
    }
}


