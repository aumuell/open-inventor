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
 |	Defines the SoSensor base class. 
 |	The sensor hierarchy is:
 |
 |		*SoSensor
 |			*SoDelayQueueSensor
 |				*SoDataSensor
 |					SoNodeSensor
 |					SoPathSensor
 |					SoFieldSensor
 |				SoIdleSensor
 |				SoOneShotSensor
 |			*SoTimerQueueSensor
 |				SoAlarmSensor
 |				SoTimerSensor
 |
 |	* means the class is abstract.
 |
 | 	Sensors provide a callback mechanism based on some event: a
 |	particular time being reached, a change to a scene graph, or
 |	lack of other events to process.
 |
 |   Author(s)		: Nick Thompson, Paul Strauss, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_SENSOR_
#define  _SO_SENSOR_

#include <Inventor/SbBasic.h>

class SoField;
class SoMField;
class SoSensor;

//////////////////////////////////////////////////////////////////////////////
//
// This typedef defines the calling sequence for all callbacks from sensors
//
//////////////////////////////////////////////////////////////////////////////

typedef void SoSensorCB(void *data, SoSensor *sensor);

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoSensor
//
//  Abstract base class for all sensors. Defines basic callback
//  definition (explicit or in constructor) and scheduling and
//  triggering methods.
//
//////////////////////////////////////////////////////////////////////////////
// C-api: prefix=SoSens
// C-api: abstract
class SoSensor {

  public:

    // Constructors. The second form takes callback function and data
    // C-api: end
    SoSensor()				{ func = NULL; funcData = NULL; }
    SoSensor(SoSensorCB *f, void *d)	{ func = f;    funcData = d; }
    // C-api: begin

    // Virtual destructor so that subclasses are deleted properly
    // C-api: expose
    virtual ~SoSensor();

    // Sets/returns callback function and user data pointers.
    // C-api: name=setFunc
    void		setFunction(SoSensorCB *f)	{ func = f;	}
    void		setData(void *d)		{ funcData = d;	}
    // C-api: name=getFunc
    SoSensorCB *	getFunction() const		{ return func; }
    void *		getData() const			{ return funcData; }

    // Schedules the sensor for triggering at the appropriate time
    // C-api: expose
    // C-api: name=sched
    virtual void	schedule() = 0;

    // Unschedules sensor to keep it from being triggered
    // C-api: expose
    // C-api: name=unsched
    virtual void	unschedule() = 0;

    // Returns whether the sensor is scheduled
    // C-api: expose
    // C-api: name=isSched
    virtual SbBool	isScheduled() const = 0;

  SoINTERNAL public:
    // Initialize static members, etc.
    static void		initClass();

    // Triggers the sensor, calling its callback function
    virtual void	trigger();

    // This returns TRUE if this sensor should precede sensor s in
    // whichever queue this sensor would be in.
    virtual SbBool	isBefore(const SoSensor *s) const = 0;

    // Sets/returns the next sensor in whichever queue the sensor is in
    void		setNextInQueue(SoSensor *next) { nextInQueue = next; }
    SoSensor *		getNextInQueue() const	       { return nextInQueue; }

  protected:
    SoSensorCB *	func;		// Callback function
    void *		funcData;	// Data to pass to callback

  private:
    SoSensor		*nextInQueue;	// Next sensor in queue
};

#endif /* _SO_SENSOR_ */
