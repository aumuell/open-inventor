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
 |	SoRotateCylindricalDragger
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
#include <Inventor/projectors/SbCylinderPlaneProjector.h>
#include <Inventor/draggers/SoRotateCylindricalDragger.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>

#include "geom/SoRotateCylindricalDraggerGeom.h"


SO_KIT_SOURCE(SoRotateCylindricalDragger);


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
SoRotateCylindricalDragger::SoRotateCylindricalDragger()
//
////////////////////////////////////////////////////////////////////////
{
    SO_KIT_CONSTRUCTOR(SoRotateCylindricalDragger);

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
	readDefaultParts("rotateCylindricalDragger.iv", geomBuffer, sizeof(geomBuffer) );

    SO_KIT_ADD_FIELD(rotation, (0.0, 0.0, 0.0, 1.0));

    SO_KIT_INIT_INSTANCE();

    // create the parts...
   setPartAsDefault("rotator",       "rotateCylindricalRotator");
   setPartAsDefault("rotatorActive", 
			 "rotateCylindricalRotatorActive"); 
   setPartAsDefault("feedback",      "rotateCylindricalFeedback");
   setPartAsDefault("feedbackActive",
			 "rotateCylindricalFeedbackActive");

    // Set the switches to 0...
    setSwitchValue(  rotatorSwitch.getValue(), 0 );
    setSwitchValue(  feedbackSwitch.getValue(), 0 );

    // start with our own default projector
    // the user can replace if they want
    cylinderProj = new SbCylinderPlaneProjector();
    userProj = FALSE;

    // add the callbacks to perform the dragging
    addStartCallback(  &SoRotateCylindricalDragger::startCB );
    addMotionCallback( &SoRotateCylindricalDragger::motionCB );
    addFinishCallback( &SoRotateCylindricalDragger::doneCB );

    // Updates the scaleFactor field when the motionMatrix is set.
    addValueChangedCallback( &SoRotateCylindricalDragger::valueChangedCB );

    // Updates the motionMatrix when the scaleFactor field is set.
    fieldSensor 
	= new SoFieldSensor(&SoRotateCylindricalDragger::fieldSensorCB, this);
    fieldSensor->setPriority( 0 );

    setUpConnections( TRUE, TRUE );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
SoRotateCylindricalDragger::~SoRotateCylindricalDragger()
//
////////////////////////////////////////////////////////////////////////
{
    if ( cylinderProj )
	delete cylinderProj;
    if (fieldSensor )
        delete fieldSensor;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Redefines this to also copy the projector.
//
// Use: protected, virtual

void
SoRotateCylindricalDragger::copyContents(const SoFieldContainer *fromFC,
					 SbBool copyConnections)
//
////////////////////////////////////////////////////////////////////////
{
    // Do the usual stuff
    SoDragger::copyContents(fromFC, copyConnections);

    // Now, copy the projector variables...

    const SoRotateCylindricalDragger *origDragger =
	(const SoRotateCylindricalDragger *) fromFC;

    if ( cylinderProj )
	delete cylinderProj;

    if ( origDragger->cylinderProj )
	cylinderProj = (SbCylinderProjector *)
	    origDragger->cylinderProj->copy();
    else
	cylinderProj = NULL;

    userProj = origDragger->userProj;
}

//    detach/attach any sensors, callbacks, and/or field connections.
//    Called by:            start/end of SoBaseKit::readInstance
//    and on new copy by:   start/end of SoBaseKit::copy.
//    Classes that redefine must call setUpConnections(TRUE,TRUE) 
//    at end of constructor.
//    Returns the state of the node when this was called.
SbBool
SoRotateCylindricalDragger::setUpConnections( SbBool onOff, SbBool doItAlways )
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
//    Set the cylinder projector back to the default or to something
//  new.
//
// Use: public
//
void
SoRotateCylindricalDragger::setProjector(SbCylinderProjector *proj)
//
////////////////////////////////////////////////////////////////////////
{
    if ( cylinderProj )
	delete cylinderProj;

    if (proj == NULL) {
	// passing in null resets the projector to the default
	userProj = FALSE;
	cylinderProj = new SbCylinderPlaneProjector();
    }
    else {
	// use the projector passed in
	cylinderProj = proj;
	userProj = TRUE;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Rotate the rotateCylindricalDragger based on mouse motion.
//
// Use: private
//
void
SoRotateCylindricalDragger::dragStart()
//
////////////////////////////////////////////////////////////////////////
{
    // Set the switches to 1...
    setSwitchValue(  rotatorSwitch.getValue(), 1 );
    setSwitchValue(  feedbackSwitch.getValue(), 1 );

    // Establish the projector cylinder in working space.
    // Working space is space at end of motion matrix.
    // translate direction is defined as (1,0,0) in local space.
	// The axis in working space is the Y axis.
	SbLine wsAxis(SbVec3f(0,0,0), SbVec3f(0,1,0));

	SbVec3f startLocalHitPt = getLocalStartingPoint();
	float rad = (startLocalHitPt 
		     - wsAxis.getClosestPoint(startLocalHitPt)).length();
	cylinderProj->setCylinder( SbCylinder(wsAxis, rad) );

    // If the hit point is on the near side of the center from where
    // the eye is, then tell the projector to intersect front.
    // Else, tell it to intersect back.
	SbMatrix lclToWld = getLocalToWorldMatrix();
	cylinderProj->setViewVolume( getViewVolume() );
	cylinderProj->setWorkingSpace( lclToWld );
	if (getFrontOnProjector() ==  USE_PICK )
	  cylinderProj->setFront(cylinderProj->isPointInFront(startLocalHitPt));
	else if (getFrontOnProjector() ==  FRONT )
	    cylinderProj->setFront( TRUE );
	else
	    cylinderProj->setFront( FALSE );

    // The cylindrical (and spherical) projectors are sort of weird in
    // that the initial hit defines the projector, but may not actually lie
    // on the cylinder or sphere selected.
    // This happens when the inital hit is too close to the edge to fit within
    // 'tolerance.'
    // So, to be sure that we get accurate performance, we need to 
    // project the mouse onto the projector once it is defined in order to 
    // get our starting point. 
	SbVec3f localProjectPt, worldProjectPt;
	localProjectPt = cylinderProj->project( getNormalizedLocaterPosition());
	lclToWld.multVecMatrix( localProjectPt, worldProjectPt ); 
	setStartingPoint( worldProjectPt );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Rotate the rotateCylindricalDragger based on mouse motion.
//
// Use: private
//
void
SoRotateCylindricalDragger::drag()
//
////////////////////////////////////////////////////////////////////////
{
    // Set up the projector space and view.
    // Working space is space at end of motion matrix.
	cylinderProj->setViewVolume( getViewVolume() );    
	cylinderProj->setWorkingSpace( getLocalToWorldMatrix() );

    // Get newHitPt and startHitPt in workspace.
	SbVec3f newHitPt 
	    = cylinderProj->project( getNormalizedLocaterPosition()); 
	SbVec3f startHitPt = getLocalStartingPoint();

    // deltaRot is how much we rotated since the mouse button went down.
	SbRotation deltaRot = 
	    cylinderProj->getRotation( startHitPt, newHitPt );

    // Append this to the startMotionMatrix, which we saved at the beginning
    // of the drag, to find the current motion matrix.
	setMotionMatrix( 
	    appendRotation( getStartMotionMatrix(), deltaRot, SbVec3f(0,0,0)));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Rotate the rotateCylindricalDragger based on mouse motion.
//
// Use: private
//
void
SoRotateCylindricalDragger::dragFinish()
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
SoRotateCylindricalDragger::startCB( void *, SoDragger *inDragger )
{
    SoRotateCylindricalDragger *dl = (SoRotateCylindricalDragger *) inDragger;
    dl->dragStart();
}

void
SoRotateCylindricalDragger::motionCB( void *, SoDragger *inDragger )
{
    SoRotateCylindricalDragger *dl = (SoRotateCylindricalDragger *) inDragger;
    dl->drag();
}

void
SoRotateCylindricalDragger::doneCB( void *, SoDragger *inDragger )
{
    SoRotateCylindricalDragger *dl = (SoRotateCylindricalDragger *) inDragger;
    dl->dragFinish();
}

void
SoRotateCylindricalDragger::valueChangedCB( void *, SoDragger *inDragger )
{
    SoRotateCylindricalDragger *m = (SoRotateCylindricalDragger *) inDragger;
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
SoRotateCylindricalDragger::fieldSensorCB( void *inDragger, SoSensor * )
{
    SoRotateCylindricalDragger *dragger 
			    = (SoRotateCylindricalDragger *) inDragger;

    // Incorporate the new field value into the matrix...
    SbMatrix motMat = dragger->getMotionMatrix();
    dragger->workFieldsIntoTransform( motMat );

    dragger->setMotionMatrix( motMat );
}
