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
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Classes:
 |	SoRotateDiscDragger
 |
 |   Author(s): Paul Isaacs, Howard Look
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <stdio.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SoDB.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoPath.h>
#include <Inventor/projectors/SbPlaneProjector.h>
#include <Inventor/draggers/SoRotateDiscDragger.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoSeparator.h>

#include "geom/SoRotateDiscDraggerGeom.h"


SO_KIT_SOURCE(SoRotateDiscDragger);


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
SoRotateDiscDragger::SoRotateDiscDragger()
//
////////////////////////////////////////////////////////////////////////
{
    SO_KIT_CONSTRUCTOR(SoRotateDiscDragger);

    isBuiltIn = TRUE;

    // Put this stuff under the geomSeparator so it will draw more
    // efficiently.
    SO_KIT_ADD_CATALOG_ENTRY(rotatorSwitch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(rotator, SoSeparator, TRUE,
				rotatorSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(rotatorActive, SoSeparator, TRUE,
				rotatorSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(feedbackSwitch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(feedback, SoSeparator, TRUE,
				feedbackSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(feedbackActive, SoSeparator, TRUE,
				feedbackSwitch, ,TRUE);

    // read geometry for shared parts
    if (SO_KIT_IS_FIRST_INSTANCE())
	readDefaultParts("rotateDiscDragger.iv", geomBuffer, sizeof(geomBuffer) );

    SO_KIT_ADD_FIELD(rotation, (0.0, 0.0, 0.0, 1.0));

    SO_KIT_INIT_INSTANCE();

    // create the parts...
   setPartAsDefault("rotator",       "rotateDiscRotator");
   setPartAsDefault("rotatorActive", 
			 "rotateDiscRotatorActive"); 
   setPartAsDefault("feedback",      "rotateDiscFeedback");
   setPartAsDefault("feedbackActive",
			 "rotateDiscFeedbackActive");

    // Set the switches to 0...
    setSwitchValue( rotatorSwitch.getValue(), 0 );
    setSwitchValue( feedbackSwitch.getValue(), 0 );

    // our humble projector
    planeProj = new SbPlaneProjector();

    // add the callbacks to perform the dragging
    addStartCallback(  &SoRotateDiscDragger::startCB );
    addMotionCallback( &SoRotateDiscDragger::motionCB );
    addFinishCallback( &SoRotateDiscDragger::doneCB );

    // Updates the scaleFactor field when the motionMatrix is set.
    addValueChangedCallback( &SoRotateDiscDragger::valueChangedCB );

    // Updates the motionMatrix when the scaleFactor field is set.
    fieldSensor 
	= new SoFieldSensor(&SoRotateDiscDragger::fieldSensorCB, this);
    fieldSensor->setPriority( 0 );

    setUpConnections( TRUE, TRUE );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
SoRotateDiscDragger::~SoRotateDiscDragger()
//
////////////////////////////////////////////////////////////////////////
{
    delete planeProj;
    if (fieldSensor )
        delete fieldSensor;
}

//    detach/attach any sensors, callbacks, and/or field connections.
//    Called by:            start/end of SoBaseKit::readInstance
//    and on new copy by:   start/end of SoBaseKit::copy.
//    Classes that redefine must call setUpConnections(TRUE,TRUE) 
//    at end of constructor.
//    Returns the state of the node when this was called.
SbBool
SoRotateDiscDragger::setUpConnections( SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( onOff ) {

	// We connect AFTER base class.
	SoDragger::setUpConnections( onOff, FALSE );

	// Call the sensor CBs to make things are up-to-date.
	fieldSensorCB( this, NULL );

	// Connect the field sensors
	if (fieldSensor->getAttachedField() != &rotation)
	    fieldSensor->attach( &rotation );
    }
    else {

	// We disconnect BEFORE base class.

	// Disconnect the field sensors.
	if (fieldSensor->getAttachedField())
	    fieldSensor->detach();

	SoDragger::setUpConnections( onOff, FALSE );
    }

    return !(connectionsSetUp = onOff);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Rotate the rotateDiscDragger based on mouse motion.
//
// Use: private
//
void
SoRotateDiscDragger::dragStart()
//
////////////////////////////////////////////////////////////////////////
{
    // Set the switches to 1...
    setSwitchValue( rotatorSwitch.getValue(), 1 );
    setSwitchValue( feedbackSwitch.getValue(), 1 );

    // Establish the projector plane in working space.
    // Working space is space at end of motion matrix.
    // The plane used should be the x-y plane.
	SbVec3f startLocalHitPt = getLocalStartingPoint();
	SbLine  workSpaceAxis( SbVec3f(0,0,0), SbVec3f(0,0,1) );
	SbVec3f planeOrigin = workSpaceAxis.getClosestPoint(startLocalHitPt);
	planeProj->setPlane(SbPlane(SbVec3f(0,0,1), planeOrigin));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Rotate the rotateDiscDragger based on mouse motion.
//
// Use: private
//
void
SoRotateDiscDragger::drag()
//
////////////////////////////////////////////////////////////////////////
{
    // Set up the projector space and view.
    // Working space is space at end of motion matrix.
	planeProj->setViewVolume( getViewVolume() );    
	planeProj->setWorkingSpace( getLocalToWorldMatrix() );

    // Get newHitPt and startHitPt in workspace.
	SbVec3f newHitPt 
	    = planeProj->project( getNormalizedLocaterPosition()); 
	SbVec3f startHitPt = getLocalStartingPoint();

    // Find the amount of rotation
    SbVec3f oldVec = startHitPt;
    SbVec3f newVec = newHitPt;
    // Remove the part of these vectors that is parallel to the normal
    oldVec -= SbVec3f( 0, 0, oldVec[2] );
    newVec -= SbVec3f( 0, 0, newVec[2] );

    // deltaRot is how much we rotated since the mouse button went down.
    SbRotation deltaRot = SbRotation( oldVec, newVec );
	
    // Append this to the startMotionMatrix, which we saved at the beginning
    // of the drag, to find the current motion matrix.
	setMotionMatrix( 
	    appendRotation( getStartMotionMatrix(), deltaRot, SbVec3f(0,0,0)));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Rotate the rotateDiscDragger based on mouse motion.
//
// Use: private
//
void
SoRotateDiscDragger::dragFinish()
//
////////////////////////////////////////////////////////////////////////
{
    // Set the switches to 0...
    setSwitchValue( rotatorSwitch.getValue(), 0 );
    setSwitchValue( feedbackSwitch.getValue(), 0 );
}    

////////////////////////////////////////////////////////////////////
//  Stubs for callbacks
////////////////////////////////////////////////////////////////////
void
SoRotateDiscDragger::startCB( void *, SoDragger *inDragger )
{
    SoRotateDiscDragger *dl = (SoRotateDiscDragger *) inDragger;
    dl->dragStart();
}

void
SoRotateDiscDragger::motionCB( void *, SoDragger *inDragger )
{
    SoRotateDiscDragger *dl = (SoRotateDiscDragger *) inDragger;
    dl->drag();
}

void
SoRotateDiscDragger::doneCB( void *, SoDragger *inDragger )
{
    SoRotateDiscDragger *dl = (SoRotateDiscDragger *) inDragger;
    dl->dragFinish();
}

void
SoRotateDiscDragger::valueChangedCB( void *, SoDragger *inDragger )
{
    SoRotateDiscDragger *m = (SoRotateDiscDragger *) inDragger;
    SbMatrix motMat = m->getMotionMatrix();

    SbVec3f    trans, scale;
    SbRotation rot, scaleOrient;
    getTransformFast( motMat, trans, rot, scale, scaleOrient );

    // Disconnect the field sensor
    m->fieldSensor->detach();

    if ( m->rotation.getValue() != rot )
	m->rotation = rot;

    // Reconnect the field sensor
    m->fieldSensor->attach( &(m->rotation) );
}

void
SoRotateDiscDragger::fieldSensorCB( void *inDragger, SoSensor * )
{
    SoRotateDiscDragger *dragger = (SoRotateDiscDragger *) inDragger;

    // Incorporate the new field value into the matrix...
    SbMatrix motMat = dragger->getMotionMatrix();
    dragger->workFieldsIntoTransform( motMat );

    dragger->setMotionMatrix( motMat );
}
