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
 |	Abstract base class for sensors that attach to some part of a
 |	scene graph and detect changes to it. A data sensor is scheduled
 |	when a change is made to the thing to which it is attached. This
 |	change is detected via the notification mechanism.
 |
 |   Author(s)		: Nick Thompson, Gavin Bell, Paul Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_DATA_SENSOR_
#define  _SO_DATA_SENSOR_

class SoBase;
class SoNode;
class SoPath;
class SoNotList;

#include <Inventor/sensors/SoDelayQueueSensor.h>

// C-api: abstract
// C-api: prefix=SoDataSens
class SoDataSensor : public SoDelayQueueSensor {

  public:
    // Constructors. The second form takes standard callback function and data
    // C-api: end
    SoDataSensor();
    SoDataSensor(SoSensorCB *func, void *data);
    // C-api: begin

    // Destructor
    virtual ~SoDataSensor();

    // Sets a callback to call when the item (SoBase) to which the
    // sensor is attached is about to be deleted. Set this to NULL to
    // remove the callback.
    // C-api: name=setDelCB
    void		setDeleteCallback(SoSensorCB *func, void *data = NULL)
	{ deleteFunc = func; deleteData = data; }

    // This may be called from the callback function of immediate
    // (priority 0) sensors. It returns the node in the scene graph
    // that caused the sensor to be scheduled and triggered, if
    // there was such a node.  If there wasn't such a node, or if the
    // sensor is not an immediate sensor, NULL is returned.
    // This is only valid for immediate sensors because if the sensor
    // isn't immediate then the sensor may be scheduled several times,
    // and there isn't one particular node that can be said to have
    // caused the trigger.
    // C-api: name=getTrigNode
    SoNode *		getTriggerNode() const;

    // Like getTriggerNode(), but returns the field that started
    // notification (NULL if the sensor isn't priority 0 or if
    // notification didn't start at a field).
    // C-api: name=getTrigField
    SoField *		getTriggerField() const;
    
    // Returns the path from the node to which this sensor is attached
    // down to the child node that changed.  setTriggerPathFlag(TRUE)
    // must be called before the sensor is scheduled, or this will
    // return NULL.  It will also return NULL if the sensor is not
    // immediate or if the notification doesn't go through a node
    // C-api: name=getTrigPath
    SoPath *		getTriggerPath() const;

    // Sets/returns the flag that indicates whether the trigger path
    // (see getTriggerPath()) is available to callback methods. This
    // is FALSE by default. Note that setting this to TRUE will add a
    // little overhead when the sensor is notified.
    // C-api: name=setTrigPathFlag
    void		setTriggerPathFlag(SbBool flag) { doTrigPath = flag; }
    // C-api: name=getTrigPathFlag
    SbBool		getTriggerPathFlag() const	{ return doTrigPath; }

    // Override unschedule() to reset trigNode and trigPath.
    virtual void unschedule();

  SoINTERNAL public:
    // Override trigger to reset trigNode and trigPath, if
    // necessary.
    virtual void trigger();

    // Propagates modification notification through an instance. By
    // default, this schedules the sensor to be triggered and saves
    // some information from the notification list for use by the
    // callback function.  Called by SoBase.
    virtual void	notify(SoNotList *list);

    // This is called when the base (path, field, node, whatever) is
    // deleted. All subclasses must implement this to do the right
    // thing.
    virtual void	dyingReference() = 0;

  protected:
    // Invokes the delete callback
    void		invokeDeleteCallback();

  private:
    SoSensorCB		*deleteFunc;    // CB and data for when attached SoBase
    void		*deleteData;	//     is about to be deleted
    SoNode		*trigNode;	// Node that triggered sensor
    SoField		*trigField;	// Field that triggered sensor
    SoPath		*trigPath;	// Path to trigNode
    SbBool		doTrigPath;	// Whether to compute trigger path
};

#endif  /* _SO_DATA_SENSOR_ */
