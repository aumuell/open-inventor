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
 |	SoFieldSensor
 |
 |   Author(s)		: Paul Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/fields/SoField.h>
#include <Inventor/fields/SoFieldContainer.h>
#include <Inventor/sensors/SoFieldSensor.h>

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoFieldSensor::SoFieldSensor() : SoDataSensor()
//
////////////////////////////////////////////////////////////////////////
{
    field = NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor with function and data.
//
// Use: public

SoFieldSensor::SoFieldSensor(SoSensorCB *func, void *data) :
	SoDataSensor(func, data)
//
////////////////////////////////////////////////////////////////////////
{
    field = NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: protected

SoFieldSensor::~SoFieldSensor()
//
////////////////////////////////////////////////////////////////////////
{
    detach();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Attaches the sensor to the given field. Will not attach if the
//    field is not contained in a node or function.
//
// Use: public

void
SoFieldSensor::attach(SoField *fieldToAttachTo)
//
////////////////////////////////////////////////////////////////////////
{
    if (field != NULL)
	detach();

    field = fieldToAttachTo;

    // Add auditor to field so that we get notified when field changes
    field->addAuditor(this, SoNotRec::SENSOR);
    field->evaluate();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Detaches the sensor if it is attached to a field.
//
// Use: public

void
SoFieldSensor::detach()
//
////////////////////////////////////////////////////////////////////////
{
    if (field != NULL) {
	field->removeAuditor(this, SoNotRec::SENSOR);
	field = NULL;

	// If we are scheduled, there's no point leaving it scheduled,
	// since it's not attached any more to whatever caused it to
	// become scheduled.
	unschedule();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This is called by the field's container when it (the container)
//    is about to be deleted.
//
// Use: private

void
SoFieldSensor::dyingReference()
//
////////////////////////////////////////////////////////////////////////
{
    // We want to detach the sensor if it's still attached to the
    // dying field's container after we invoke the callback. If the
    // callback attaches to something else, we don't want to detach
    // it. So we'll compare the field containers before and after the
    // callback is invoked and detach only if it's the same one.

    SoFieldContainer *dyingFieldCont = getAttachedField()->getContainer();

    invokeDeleteCallback();

    if (getAttachedField() != NULL &&
	getAttachedField()->getContainer() == dyingFieldCont)
	detach();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Propagates modification notification through an instance. This
//    checks to see if the field that triggered notification matches
//    the field we are attached to.
//
// Use: private, virtual

void
SoFieldSensor::notify(SoNotList *list)
//
////////////////////////////////////////////////////////////////////////
{
    // See if the field that caused the notification (may be NULL) is
    // the same as the field we are attached to.
    if (list->getLastField() == field) {
	SoDataSensor::notify(list);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Override trigger to evaluate the field we're connected to, just
//   in case the trigger method doesn't get the value.
//
// Use: internal

void
SoFieldSensor::trigger()
//
////////////////////////////////////////////////////////////////////////
{
    field->evaluate();
    SoDataSensor::trigger();
}
