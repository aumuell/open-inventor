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
 |	This file defines the SoPendulum node class.
 |
 |   Author(s)		: Ronen Barzel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_PENDULUM_
#define  _SO_PENDULUM_

#include <Inventor/nodes/SoRotation.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFBool.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoPendulum
//
//  SoRotation node oscillates between two rotations
//
//////////////////////////////////////////////////////////////////////////////

// C-api: public=rotation,rotation0,rotation1,speed,on
class SoPendulum : public SoRotation {

    SO_NODE_HEADER(SoPendulum);

  public:
    // Fields
    SoSFRotation	rotation0;
    SoSFRotation	rotation1;
    SoSFFloat		speed;	// cycles per second
    SoSFBool		on;	// FALSE to stop rotation

    // Constructor
    SoPendulum();

  SoINTERNAL public:
    static void		initClass();

  protected:
    virtual ~SoPendulum();

  private:
    // internal engines to compute the rotation
    class SoElapsedTime		*time;
    class SoCalculator		*calc;
    class SoInterpolateRotation	*interp;

    // overload write method not to write internal engines
    virtual void	write(class SoWriteAction *action);

    // keep track of internal connection
    class SoEngineOutput	*internalConnection;

    // overload notify method to check for field updates
    virtual void notify(SoNotList *list);

    class SoOneShotSensor	*rotationSensor;
    class SoFieldSensor		*rotation0Sensor;
    class SoFieldSensor		*rotation1Sensor;
    class SoFieldSensor		*onSensor;

    static void rotationSensorCB(void *data, class SoSensor *sensor);
    static void onSensorCB(void *data, class SoSensor *sensor);
};

#endif /* _SO_PENDULUM_ */
