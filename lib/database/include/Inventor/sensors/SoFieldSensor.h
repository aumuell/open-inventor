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
 |	Data sensor that is attached to a field in a node or elsewhere.
 |	The sensor is scheduled when a change is made to that field. Note:
 |	the field must be contained within a node or function, or
 |	attachment will not work.
 |
 |   Author(s)		: Paul Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_FIELD_SENSOR_
#define  _SO_FIELD_SENSOR_

#include <Inventor/sensors/SoDataSensor.h>

class SoFieldContainer;

// C-api: prefix=SoFieldSens
class SoFieldSensor : public SoDataSensor {

  public:
    // Constructors. The second form takes standard callback function and data
    SoFieldSensor();
    // C-api: name=CreateCB
    SoFieldSensor(SoSensorCB *func, void *data);

    // Destructor
    virtual ~SoFieldSensor();

    // Attaches the sensor to the given field. Will not attach if the
    // field is not contained in a node or function.
    void		attach(SoField *field);

    // Detaches the sensor if it is attached to a field
    void		detach();

    // Returns the field to which the sensor is attached, or NULL if it
    // is not attached.
    SoField *		getAttachedField() const	{ return field; }

  SoINTERNAL public:
    // Override trigger to evaluate the field we're connected to, just
    // in case the trigger method doesn't get the value.
    virtual void trigger();

  private:
    SoField *		field;		// Field sensor is attached to

    // Propagates modification notification through an instance. This
    // checks to see if the field that triggered notification matches
    // the field we are attached to. (It also checks indices if necessary.)
    virtual void	notify(SoNotList *list);

    // Called by the attached path when it (the path) is about to be deleted
    virtual void	dyingReference();
};

#endif  /* _SO_FIELD_SENSOR_ */
