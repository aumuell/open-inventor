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
 |	SoTrackballDragger
 |
 |   Author(s): Paul Isaacs, David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <stdio.h>

#include <Inventor/errors/SoDebugError.h>
#include <Inventor/events/SoEvent.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/SoDB.h>
#include <Inventor/sensors/SoTimerSensor.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/draggers/SoTrackballDragger.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSurroundScale.h>
#include <Inventor/nodes/SoAntiSquish.h>
#include <Inventor/projectors/SbCylinderPlaneProjector.h>
#include <Inventor/projectors/SbSphereSectionProjector.h>
#include <Inventor/projectors/SbLineProjector.h>

#include "geom/SoTrackballDraggerGeom.h"


// Defines

#define FAST_ANIM_RATE (1/60.0)    // animation frame rate
#define ANIM_ADJUSTMENT 0.05       // used to schedule timerSensor to take 
			           // a little more or a little less time.

// size of the rotation buffer, which is used to animate the spinning ball.
#define ROT_BUFFER_SIZE 5

SO_KIT_SOURCE(SoTrackballDragger);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
SoTrackballDragger::SoTrackballDragger()
//
////////////////////////////////////////////////////////////////////////
{
    SO_KIT_CONSTRUCTOR(SoTrackballDragger);

    isBuiltIn = TRUE;

    // Don't create this by default. It's only really put into use
    // if this dragger is put inside a manipulator.
    SO_KIT_ADD_CATALOG_ENTRY(surroundScale, SoSurroundScale, 
				TRUE, topSeparator, geomSeparator,TRUE);
    // Create an anti-squish node by default.
    SO_KIT_ADD_CATALOG_ENTRY(antiSquish, SoAntiSquish, 
				FALSE, topSeparator, geomSeparator,TRUE);

    // Put this stuff under the geomSeparator so it will draw more
    // efficiently.
    SO_KIT_ADD_CATALOG_ENTRY(rotatorSwitch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(rotator, SoSeparator, TRUE,
				rotatorSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(rotatorActive, SoSeparator, TRUE,
				rotatorSwitch, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(XRotatorSwitch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(XRotator, SoSeparator, TRUE,
				XRotatorSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(XRotatorActive, SoSeparator, TRUE,
				XRotatorSwitch, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(YRotatorSwitch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(YRotator, SoSeparator, TRUE,
				YRotatorSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(YRotatorActive, SoSeparator, TRUE,
				YRotatorSwitch, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(ZRotatorSwitch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(ZRotator, SoSeparator, TRUE,
				ZRotatorSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(ZRotatorActive, SoSeparator, TRUE,
				ZRotatorSwitch, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(userAxisRotation, SoRotation, TRUE,
				geomSeparator, ,FALSE);

    SO_KIT_ADD_CATALOG_ENTRY(userAxisSwitch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(userAxis, SoSeparator, TRUE,
				userAxisSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(userAxisActive, SoSeparator, TRUE,
				userAxisSwitch, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(userRotatorSwitch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(userRotator, SoSeparator, TRUE,
				userRotatorSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(userRotatorActive, SoSeparator, TRUE,
				userRotatorSwitch, ,TRUE);

    // read geometry for shared parts
    if (SO_KIT_IS_FIRST_INSTANCE())
	readDefaultParts("trackballDragger.iv", geomBuffer, sizeof(geomBuffer) );

    SO_KIT_ADD_FIELD(rotation, (0.0, 0.0, 0.0, 1.0));
    SO_KIT_ADD_FIELD(scaleFactor, (1.0, 1.0, 1.0));

    SO_KIT_INIT_INSTANCE();

    // Make the anti-squish node surround the biggest dimension
    SoAntiSquish *as = SO_GET_ANY_PART( this,"antiSquish", SoAntiSquish );
    as->sizing = SoAntiSquish::LONGEST_DIAGONAL;

    // create the parts...
  setPartAsDefault("rotator",          "trackballRotator");
  setPartAsDefault("rotatorActive",    "trackballRotatorActive");
  setPartAsDefault("XRotator",         "trackballXRotator");
  setPartAsDefault("XRotatorActive",   "trackballXRotatorActive");
  setPartAsDefault("YRotator",         "trackballYRotator");
  setPartAsDefault("YRotatorActive",   "trackballYRotatorActive");
  setPartAsDefault("ZRotator",         "trackballZRotator");
  setPartAsDefault("ZRotatorActive",   "trackballZRotatorActive");
  setPartAsDefault("userAxis",         "trackballUserAxis");
  setPartAsDefault("userAxisActive",   "trackballUserAxisActive");
  setPartAsDefault("userRotator",      "trackballUserRotator");
  setPartAsDefault("userRotatorActive","trackballUserRotatorActive");

  // Set the switches of the ball and stripes to 0...
    setSwitchValue(  rotatorSwitch.getValue(), 0 );
    setSwitchValue( XRotatorSwitch.getValue(), 0 );
    setSwitchValue( YRotatorSwitch.getValue(), 0 );
    setSwitchValue( ZRotatorSwitch.getValue(), 0 );

  //Turn off userAxis Completely (SO_SWITCH_NONE)
    setSwitchValue(    userAxisSwitch.getValue(), SO_SWITCH_NONE );
    setSwitchValue( userRotatorSwitch.getValue(), SO_SWITCH_NONE );

  // Set the rotation of the userStuffRotation to an initial value.
    SoRotation *rN = SO_GET_ANY_PART(this,"userAxisRotation", SoRotation);
    rN->rotation.setValue( SbRotation( SbVec3f(0,1,0), 0.0 ) );

    currentState = INACTIVE;

    setHighlights();

    // init the user axis to align with 'y'
    userAxisVec.setValue( 0, 1, 0);

    // queues used for animation
    rotBuffer =  new SbRotation[ROT_BUFFER_SIZE];
    timeBuffer = new SbTime[ROT_BUFFER_SIZE];

    // allocate animation sensor
    spinSensor = new SoTimerSensor(&SoTrackballDragger::spinSensorCB, this);

    animationEnabled = TRUE;
    wasSpinningAtDragStart = FALSE;

    sphereProj = new SbSphereSectionProjector( 0.85 );
    stripeProj = new SbCylinderPlaneProjector( 0.85 );

    // add the callbacks to perform the dragging
    addStartCallback(  &SoTrackballDragger::startCB );
    addMotionCallback( &SoTrackballDragger::motionCB );
    addFinishCallback(   &SoTrackballDragger::finishCB );

    // add the callback to update things each time a meta key changes.
    addOtherEventCallback( &SoTrackballDragger::metaKeyChangeCB );

    // Update the rotation and scale fields when the motionMatrix is set.
    addValueChangedCallback( &SoTrackballDragger::valueChangedCB );

    // Updates the motionMatrix when the scaleFactor field is set.
    rotFieldSensor = new SoFieldSensor(&SoTrackballDragger::fieldSensorCB,this);
    rotFieldSensor->setPriority( 0 );

    // Updates the motionMatrix when the scaleFactor field is set.
    scaleFieldSensor 
	= new SoFieldSensor( &SoTrackballDragger::fieldSensorCB, this);
    scaleFieldSensor->setPriority( 0 );

    setUpConnections( TRUE, TRUE );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: public

SoTrackballDragger::~SoTrackballDragger()
//
////////////////////////////////////////////////////////////////////////
{
    // deallocate queues
    delete [ /*ROT_BUFFER_SIZE*/ ] rotBuffer;
    delete [ /*ROT_BUFFER_SIZE*/ ] timeBuffer;

    delete sphereProj;
    delete stripeProj;

    // delete sensors
    if (spinSensor != NULL)
	delete spinSensor;
    if (rotFieldSensor != NULL)
	delete rotFieldSensor;
    if (scaleFieldSensor != NULL)
	delete scaleFieldSensor;
}

//    detach/attach any sensors, callbacks, and/or field connections.
//    Called by:            start/end of SoBaseKit::readInstance
//    and on new copy by:   start/end of SoBaseKit::copy.
//    Classes that redefine must call setUpConnections(TRUE,TRUE) 
//    at end of constructor.
//    Returns the state of the node when this was called.
SbBool
SoTrackballDragger::setUpConnections( SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( onOff ) {

	// We connect AFTER base class.
	SoDragger::setUpConnections( onOff, FALSE );

	// Call the sensor CBs to make things are up-to-date.
	fieldSensorCB( this, NULL );

	// Figure out the userAxisVec (a protected variable) from the 
	// 'userAxisRotation' part.
	SoRotation *rN = (SoRotation *) userAxisRotation.getValue();
	if (rN != NULL) {
	    SbRotation uRot = rN->rotation.getValue();
	    uRot.multVec( SbVec3f(0,1,0), userAxisVec );

	    // decide whether or not to display the spinning wheel.
	    // only show it if does not line up with a major axis.
	    if (   ( fabs(userAxisVec[0]) > 0.99 )
		|| ( fabs(userAxisVec[1]) > 0.99 )
		|| ( fabs(userAxisVec[2]) > 0.99 ) ) {
		setSwitchValue( userAxisSwitch.getValue(), SO_SWITCH_NONE );
		setSwitchValue( userRotatorSwitch.getValue(), SO_SWITCH_NONE );
	    }
	    else {
		setSwitchValue( userAxisSwitch.getValue(), 1 );
		setSwitchValue( userRotatorSwitch.getValue(), 1 );
	    }
	}

	// Connect the field sensors
	if (rotFieldSensor->getAttachedField() != &rotation)
	    rotFieldSensor->attach( &rotation );
	if (scaleFieldSensor->getAttachedField() != &scaleFactor)
	    scaleFieldSensor->attach( &scaleFactor );
    }
    else {

	// We disconnect BEFORE base class.

	// Disconnect the field sensors.
	if (rotFieldSensor->getAttachedField())
	    rotFieldSensor->detach();
	if (scaleFieldSensor->getAttachedField())
	    scaleFieldSensor->detach();

	SoDragger::setUpConnections( onOff, FALSE );
    }

    return !(connectionsSetUp = onOff);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: public
void
SoTrackballDragger::setAnimationEnabled( SbBool newVal )
//
////////////////////////////////////////////////////////////////////////
{
    animationEnabled = newVal;
    if (animationEnabled == FALSE )
	resetSpinStuff();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the various switch nodes to highlight the correct parts
//    of the trackball, depending on the state.
//
// Use: private
//
void
SoTrackballDragger::setHighlights()
//
////////////////////////////////////////////////////////////////////////
{
    switch ( currentState ) {
	case INACTIVE:
	case SPINNING:
	    setAllPartsActive( FALSE );
	    break;
	case FREE_ROTATE:
	case SCALE:
	    setAllPartsActive( TRUE );
	    break;
	case X_ROTATE:
	    setAllPartsActive( FALSE );
	    setSwitchValue( XRotatorSwitch.getValue(), 1 );
	    break;
	case Y_ROTATE:
	    setAllPartsActive( FALSE );
	    setSwitchValue( YRotatorSwitch.getValue(), 1 );
	    break;
	case Z_ROTATE:
	    setAllPartsActive( FALSE );
	    setSwitchValue( ZRotatorSwitch.getValue(), 1 );
	    break;
	case USER_AXIS_ROTATE:
	case USER_AXIS_ADJUST:
	    {
	    setAllPartsActive( FALSE );

	    SoSwitch *sw = (SoSwitch *) userAxisSwitch.getValue();
	    if ( sw && sw->whichChild.getValue() != SO_SWITCH_NONE )
		setSwitchValue( userAxisSwitch.getValue(), 1 );

	    sw = (SoSwitch *) userRotatorSwitch.getValue();
	    if ( sw && sw->whichChild.getValue() != SO_SWITCH_NONE )
		setSwitchValue( userRotatorSwitch.getValue(), 1 );
	    }
	    break;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Rotate the trackball and object based on mouse motion.
//
// Use: private
//
void
SoTrackballDragger::dragStart()
//
////////////////////////////////////////////////////////////////////////
{
    // register time for animation of continuous spinning
    prevTime = getEvent()->getTime(); 

    // We want to remember if we were spinning at drag start.
    // If so, we will always call ha->setHandled() in dragFinish().
    // This way, it won't be considered an item selection, which is usually
    // the case when you click on an object without moving the mouse.
    wasSpinningAtDragStart = (currentState == SPINNING);
    resetSpinStuff();     // check if spinning should be stopped

    const SoPath *pickPath = getPickPath();

    // Invalidate surroundScale if it exists.
    SoSurroundScale *ss = SO_CHECK_PART(this, "surroundScale", SoSurroundScale);
    if (ss != NULL)
	ss->invalidate();

    // Make a note of which modifier keys are down
    ctlDown = getEvent()->wasCtrlDown();
    shftDown = getEvent()->wasShiftDown();

    // Determine the current state 
    if (ctlDown)
	currentState = SCALE;
    else if (shftDown)
	currentState = USER_AXIS_ADJUST;
    else if (pickPath && pickPath->containsNode( XRotatorSwitch.getValue()  ) ||
	      getSurrogatePartPickedName() == "XRotator" )
	currentState = X_ROTATE;
    else if (pickPath && pickPath->containsNode( YRotatorSwitch.getValue()  ) ||
	      getSurrogatePartPickedName() == "YRotator" )
	currentState = Y_ROTATE;
    else if (pickPath && pickPath->containsNode( ZRotatorSwitch.getValue()  ) ||
	      getSurrogatePartPickedName() == "ZRotator" )
	currentState = Z_ROTATE;
    else if (pickPath && pickPath->containsNode(userRotatorSwitch.getValue()) ||
	      getSurrogatePartPickedName() == "userRotator" )
	currentState = USER_AXIS_ROTATE;
    else 
	currentState = FREE_ROTATE;

    // set the constrained axis, if appropriate
    switch( currentState ) {
	case X_ROTATE:
	    constrainedAxis.setValue( 1, 0, 0);
	    break;
	case Y_ROTATE:
	    constrainedAxis.setValue( 0, 1, 0);
	    break;
	case Z_ROTATE:
	    constrainedAxis.setValue( 0, 0, 1);
	    break;
	case USER_AXIS_ROTATE:
	    constrainedAxis = userAxisVec;
	    break;
    }

    // Working space is space after "antiSquish"
    SbMatrix workSpaceToLocal, localToWorkSpace;
    getPartToLocalMatrix( "antiSquish", workSpaceToLocal, localToWorkSpace);
    SbMatrix worldSpaceToWork = getWorldToLocalMatrix();
    worldSpaceToWork.multRight( localToWorkSpace );
    SbMatrix workSpaceToWorld = getLocalToWorldMatrix();
    workSpaceToWorld.multLeft( workSpaceToLocal );

    // Find the starting point in workspace.
    SbVec3f startWorkSpaceHitPt;
    worldSpaceToWork.multVecMatrix(getWorldStartingPoint(),startWorkSpaceHitPt);


    float   rad;
    SbVec3f zero( 0,0,0 );
    SbLine  theAxis;
    rad = startWorkSpaceHitPt.length();


    // Setup projector and get a starting value
    switch( currentState ) {
	case SCALE:
	case USER_AXIS_ADJUST:
	case FREE_ROTATE:
	    {
	    SbVec3f       sphCenter(0,0,0);
	    sphereProj->setSphere( SbSphere( sphCenter, rad ));

	    SbViewVolume viewVol = getViewVolume();

	    sphereProj->setViewVolume(viewVol );
	    sphereProj->setWorkingSpace( workSpaceToWorld );

	    // If the hit point is on the near side of the center from where
	    // the eye is, then tell the projector to intersect front.
	    // Else, tell it to intersect back.
	    if (getFrontOnProjector() ==  USE_PICK )
	        sphereProj->setFront( sphereProj->isPointInFront( startWorkSpaceHitPt ));
	    else if (getFrontOnProjector() ==  FRONT )
		sphereProj->setFront( TRUE );
	    else
		sphereProj->setFront( FALSE );
	    // We need to project and save the starting world point, 
	    // Since our rotate projectors might intersect to find a 
	    // different point than we hit on the geometry itself.
	    SbVec3f ptOnProjector, worldSpacePt;
	    ptOnProjector = sphereProj->project(getNormalizedLocaterPosition());
	    workSpaceToWorld.multVecMatrix(ptOnProjector,worldSpacePt);
	    setStartingPoint(worldSpacePt);
	    }
	    break;
	case X_ROTATE:
	case Y_ROTATE:
	case Z_ROTATE:
	case USER_AXIS_ROTATE:
	    {
	    theAxis.setValue( zero, zero + constrainedAxis );
	    stripeProj->setCylinder( SbCylinder(theAxis, rad) );
	    stripeProj->setViewVolume(getViewVolume());
	    stripeProj->setWorkingSpace( workSpaceToWorld );

	    // If the hit point is on the near side of the center from where
	    // the eye is, then tell the projector to intersect front.
	    // Else, tell it to intersect back.
	    if (getFrontOnProjector() ==  USE_PICK )
		stripeProj->setFront( stripeProj->isPointInFront( startWorkSpaceHitPt));
	    else if (getFrontOnProjector() ==  FRONT )
		stripeProj->setFront( TRUE );
	    else
		stripeProj->setFront( FALSE );

	    // We need to project and save the starting world point, 
	    // Since our rotate projectors might intersect to find a 
	    // different point than we hit on the geometry itself.
	    SbVec3f ptOnProjector, worldSpacePt;
	    ptOnProjector = stripeProj->project(getNormalizedLocaterPosition());
	    workSpaceToWorld.multVecMatrix(ptOnProjector,worldSpacePt);
	    setStartingPoint(worldSpacePt);
	    }
	    break;
	case INACTIVE:
	case SPINNING:
	    break;
    }

    // Save for incremental rotations, needed by spherical projector.
    prevWorldHitPt   = getWorldStartingPoint();
    prevMotionMatrix = getStartMotionMatrix();

    if ( currentState == USER_AXIS_ADJUST )
	userStripeDrag();

    setHighlights();
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Drag the trackball based on mouse motion.
//    This routine just ships the work to rotateDrag or
//    scaleDrag.
//
// Use: private
//
void
SoTrackballDragger::drag()
//
////////////////////////////////////////////////////////////////////////
{
    switch( currentState ) {
	case SCALE:
	    scaleDrag();
	    break;
	case USER_AXIS_ADJUST:
	    userStripeDrag();
	    break;
	case FREE_ROTATE:
	case X_ROTATE:
	case Y_ROTATE:
	case Z_ROTATE:
	case USER_AXIS_ROTATE:
	    rotateDrag();
	    break;
	case INACTIVE:
	case SPINNING:
	    break;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Rotate the trackball and object based on mouse motion.
//
// Use: private
//
SbBool
SoTrackballDragger::rotateDrag()
//
////////////////////////////////////////////////////////////////////////
{
    SbMatrix workSpaceToLocal, localToWorkSpace;
    getPartToLocalMatrix( "antiSquish", workSpaceToLocal, localToWorkSpace);
    SbMatrix workSpaceToWorld = getLocalToWorldMatrix();
    workSpaceToWorld.multLeft( workSpaceToLocal );
    SbMatrix worldSpaceToWork = getWorldToLocalMatrix();
    worldSpaceToWork.multRight( localToWorkSpace );

    // update the viewvolume and workspace on the projectors
    sphereProj->setViewVolume(getViewVolume());
    stripeProj->setViewVolume(getViewVolume());
    sphereProj->setWorkingSpace( workSpaceToWorld );
    stripeProj->setWorkingSpace( workSpaceToWorld );

    // Get newHitPt and prevHitPt in workspace.
    SbVec3f     prevHitPt;
    worldSpaceToWork.multVecMatrix(prevWorldHitPt, prevHitPt );
    SbVec3f	newHitPt;
    SbRotation  deltaRot;
    if ( currentState == FREE_ROTATE ) {
	newHitPt = sphereProj->project(getNormalizedLocaterPosition());
	deltaRot = sphereProj->getRotation(prevHitPt,newHitPt);
    }
    else {
	newHitPt = stripeProj->project(getNormalizedLocaterPosition());
	deltaRot = stripeProj->getRotation(prevHitPt,newHitPt);
    }

    // Append deltaRot to prevMotionMatrix, which we saved last time.
    SbMatrix newMotionMatrix = 
	appendRotation( prevMotionMatrix, deltaRot, SbVec3f(0,0,0),
			&workSpaceToLocal );

    // Save our incremental results.
	workSpaceToWorld.multVecMatrix(newHitPt, prevWorldHitPt);
	prevMotionMatrix = newMotionMatrix;

    setMotionMatrix( newMotionMatrix );

    // save information for animation of continuous spining
    prevTime = getEvent()->getTime();

    // save increment of rotation for animation
    lastIndex = ((lastIndex+1) % ROT_BUFFER_SIZE);
    rotBuffer[lastIndex] = deltaRot;
    timeBuffer[lastIndex] = prevTime;

    // check if queue is full
	// if so, through away oldest events from queue
    if (((lastIndex+1) % ROT_BUFFER_SIZE) == firstIndex)
		firstIndex = ((firstIndex+1) % ROT_BUFFER_SIZE);

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Apply a uniform scale to the trackball, by finding the closest point
//    on the line and seeing how much further (orcloser) from the
//    center we are than last time.
//
// Use: private
//
SbBool
SoTrackballDragger::scaleDrag()
//
////////////////////////////////////////////////////////////////////////
{
    SbMatrix workSpaceToLocal, localToWorkSpace;
    getPartToLocalMatrix( "antiSquish", workSpaceToLocal, localToWorkSpace);
    SbMatrix workSpaceToWorld = getLocalToWorldMatrix();
    workSpaceToWorld.multLeft( workSpaceToLocal );
    SbMatrix worldSpaceToWork = getWorldToLocalMatrix();
    worldSpaceToWork.multRight( localToWorkSpace );

    SbVec3f         startHitPt, cntr( 0,0,0 );
    SbLineProjector lineProj;
    worldSpaceToWork.multVecMatrix( getWorldStartingPoint(), startHitPt );

    // make a projection line between the center and the initial hit point.
    lineProj.setLine( SbLine( cntr, startHitPt ));
    lineProj.setViewVolume(getViewVolume());
    lineProj.setWorkingSpace( workSpaceToWorld );

    SbVec3f         newHitPt;
    // lineProj has the line we are locked to in expanding and contracting
    // the trackball.  Find the point from lineProj closest to the mouseLine
    newHitPt = lineProj.project(getNormalizedLocaterPosition());

    SbVec3f oldRadVec = startHitPt - cntr;
    SbVec3f newRadVec = newHitPt - cntr;

    // [1] Find the distance from the old and new projected points to origin
    float oldRad = oldRadVec.length();
    float newRad = newRadVec.length();

    // [2] Check if we've flipped to the other side of the origin.
    if (startHitPt.dot(newHitPt) < 0.0)
	newRad *= -1.0;

    // [3] Change in scale is ratio of newRad to oldRad
#define TINY .0001
    float delta = (fabs(oldRad) < TINY || fabs(newRad) < TINY)
		    ? 1.0 : newRad / oldRad;
#undef TINY

    // constrain the scaling to be greater than getMinScale()
    if (delta < getMinScale())
	delta = getMinScale();
    
    // Set the scaling (to be done relative to scale in startMotionMatrix).
    SbVec3f scl( delta, delta, delta );

    // Save our incremental results.
	workSpaceToWorld.multVecMatrix(newHitPt, prevWorldHitPt);

    // Append this to the startMotionMatrix, which we saved at the beginning
    // of the drag, to find the current motion matrix.
    setMotionMatrix( appendScale( getStartMotionMatrix(), scl,SbVec3f(0,0,0),
				    &workSpaceToLocal));

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Select a new user-selected axis for constrained rotation.
//    When the user selects this special axis, an axis and a wheel
//    are displayed.  Once selected and displayed, the user can 
//    pick the wheel and use it to spin the trackball about this 
//    axis.
//
// Use: private
//
SbBool
SoTrackballDragger::userStripeDrag()
//
////////////////////////////////////////////////////////////////////////
{
    SbMatrix workSpaceToLocal, localToWorkSpace;
    getPartToLocalMatrix( "antiSquish", workSpaceToLocal, localToWorkSpace);
    SbMatrix workSpaceToWorld = getLocalToWorldMatrix();
    workSpaceToWorld.multLeft( workSpaceToLocal );
    SbMatrix worldSpaceToWork = getWorldToLocalMatrix();
    worldSpaceToWork.multRight( localToWorkSpace );

    SbVec3f	newHitPt;

    // get the newHitPt
    // temporarily make the tolerance = 1.0, since we want all possible sphere
    // intersections. This makes for a nicer interface of axis picking
    ((SbSphereSectionProjector *)sphereProj)->setTolerance( 1.0 );
    newHitPt = sphereProj->project(getNormalizedLocaterPosition());
    ((SbSphereSectionProjector *)sphereProj)->setTolerance( 0.85 );

    // find the new axis
    userAxisVec = newHitPt;
    userAxisVec.normalize();

    SbRotation plcmntRot(SbVec3f(0,1,0), userAxisVec);
    ((SoRotation *)userAxisRotation.getValue())->rotation.setValue( plcmntRot );

    // decide whether or not to display the spinning wheel.
    // only show it if does not line up with a major axis.
    if (   ( fabs(userAxisVec[0]) > 0.99 )
        || ( fabs(userAxisVec[1]) > 0.99 )
	|| ( fabs(userAxisVec[2]) > 0.99 ) ) {
	setSwitchValue( userAxisSwitch.getValue(), SO_SWITCH_NONE );
	setSwitchValue( userRotatorSwitch.getValue(), SO_SWITCH_NONE );
    }
    else {
	setSwitchValue( userAxisSwitch.getValue(), 1 );
	setSwitchValue( userRotatorSwitch.getValue(), 1 );
    }

    // save intersection for later
    workSpaceToWorld.multVecMatrix( newHitPt, prevWorldHitPt );

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Called when the mouse-button is released.
//    Prepares the spinSensor to continue spinning...
//
// Use: private
//
void
SoTrackballDragger::dragFinish()
//
////////////////////////////////////////////////////////////////////////
{
    // Invalidate surroundScale if it exists.
    SoSurroundScale *ss = SO_CHECK_PART(this, "surroundScale", SoSurroundScale);
    if (ss != NULL)
	ss->invalidate();

    switch ( currentState ) {
	case FREE_ROTATE:
	case X_ROTATE:
	case Y_ROTATE:
	case Z_ROTATE:
	case USER_AXIS_ROTATE:
	    // check if we need to continue spinning
	    if ( isAnimationEnabled()  
		 && (getEvent()->getTime() - prevTime) == SbTime::zero()) {   
		    // button came up during motion

		    spinTime.setToTimeOfDay();
		    scheduleRate = FAST_ANIM_RATE;
		    currentState = SPINNING;
		    computeAverage = TRUE;
		    spinSensor->setInterval(scheduleRate);
		    spinSensor->schedule();
	    }
	    else
		currentState = INACTIVE;
	    break;
	default:
	    currentState = INACTIVE;
	    break;
    }

    setHighlights();

    if (wasSpinningAtDragStart)
	getHandleEventAction()->setHandled();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Resets all variables for turning off the spinSensor
//
// Use: private

void
SoTrackballDragger::resetSpinStuff()
//
////////////////////////////////////////////////////////////////////////
{
    if ( currentState == SPINNING ) {
	currentState = INACTIVE;
	spinSensor->unschedule();
	renderCaching = AUTO;
    }

    // reset animation queue for spinning
    firstIndex = 0;
    lastIndex = -1;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Routine which animates the ball spinning (called by sensor).
//
// Use: private

void
SoTrackballDragger::spinAnimate()
//
////////////////////////////////////////////////////////////////////////
{
    // get the time difference
    SbTime time = SbTime::getTimeOfDay();
    SbTime timeDiff = time - spinTime;
    spinTime = time;	// save current time
    double sec = timeDiff.getValue();
    float angle;
    
    // check if average rotation needs to be computed
    if (computeAverage) {
	
	// get number of samples
	int num = (((lastIndex - firstIndex) + 1 + 
	    ROT_BUFFER_SIZE) % ROT_BUFFER_SIZE);
	
	// check for not enough samples
	if (num < 2) {
	    resetSpinStuff();
	    return;
	}
	
	// get average axis of rotation
	// ??? right now only take one sample for axis
	rotBuffer[firstIndex].getValue(averageAxis, angle);

	// get angular velocity
	SbVec3f axis;
	angleVelocity = 0;
	for (int i=0; i<num; i++) {
	    int n = (firstIndex + i) % ROT_BUFFER_SIZE;
	    rotBuffer[n].getValue(axis, angle);
	    angleVelocity += angle;
	}
	SbTime deltaTime = timeBuffer[lastIndex] - timeBuffer[firstIndex];
	
#ifdef DEBUG
	// check for negative number 
	if (deltaTime.getValue() <= 0.0)
	    SoDebugError::post("SoTrackballDragger::spinAnimate",
				"Zero or negative Dt %f", deltaTime.getValue());
#endif

	angleVelocity /= deltaTime.getValue();
	
	computeAverage = FALSE;
    }
    
    // calculate the change in rotation
    angle = angleVelocity * sec;
    
    // append the incremental rotation after the current rotation
    SbRotation rotIncrement(averageAxis, angle);

    // recalculate trackball placement
    // 1 -- convert local motion into matrix
	    SbMatrix tm;
	    tm.setRotate( rotIncrement );
    // 2 -- transform matrix across antisquish node.
	    transformMatrixToLocalSpace( tm, tm, "antiSquish");
    // 3 -- append matrix to motion
	    SbMatrix newMotion = getMotionMatrix();
	    setMotionMatrix( newMotion.multLeft( tm ) );

    if ( timeDiff  > ( scheduleRate * ( 1 + ANIM_ADJUSTMENT) ))
	// If things are taking significantly longer than expected, 
	// increase the amount of time between calls.
	scheduleRate *= ( 1 + ANIM_ADJUSTMENT );
    else if ( timeDiff  < ( scheduleRate * ( 1 + (ANIM_ADJUSTMENT/2.0)) )) {
	// otherwise, if things are going quickly, reduce the time. 
	// See which is smaller, scaling down the rate (newRate1) 
	// or a binary-search hop down to the FAST_ANIM_RATE
	SbTime newRate1 = scheduleRate * ( 1 - ANIM_ADJUSTMENT );
	SbTime newRate2 = scheduleRate - 0.5 * (scheduleRate - FAST_ANIM_RATE);
	scheduleRate = ( newRate1 < newRate2 ) ? newRate1 : newRate2;
    }

    // Make sure we don't go below the fastest rate
    if ( scheduleRate < FAST_ANIM_RATE )
	scheduleRate = FAST_ANIM_RATE;

    // reschedule at the new rate, starting at THIS time + one increment
    // We don't need to unschedule first --  it's done automatically!
    spinSensor->setInterval(scheduleRate);
    spinSensor->setBaseTime(spinTime + scheduleRate);
    spinSensor->schedule();
}

//
////////////////////////////////////////////////////////////////////////
// static callbacks stubs
////////////////////////////////////////////////////////////////////////
//

void
SoTrackballDragger::spinSensorCB(void *v, SoSensor *)
{
    SoTrackballDragger *m = (SoTrackballDragger *) v;

    m->renderCaching = OFF;
    m->spinAnimate();
}

////////////////////////////////////////////////////////////////////////
//
// Use: protected
//
void
SoTrackballDragger::setAllPartsActive( SbBool onOrOff )
//
////////////////////////////////////////////////////////////////////////
{
    int sVal = (onOrOff == TRUE) ? 1 : 0;

    setSwitchValue( rotatorSwitch.getValue(), sVal );
    setSwitchValue( XRotatorSwitch.getValue(), sVal );
    setSwitchValue( YRotatorSwitch.getValue(), sVal );
    setSwitchValue( ZRotatorSwitch.getValue(), sVal );

    SoSwitch *sw = (SoSwitch *) userAxisSwitch.getValue();
    if ( sw && sw->whichChild.getValue() != SO_SWITCH_NONE )
	setSwitchValue( userAxisSwitch.getValue(), sVal );

    sw = (SoSwitch *) userRotatorSwitch.getValue();
    if ( sw && sw->whichChild.getValue() != SO_SWITCH_NONE )
	setSwitchValue( userRotatorSwitch.getValue(), sVal );
}

//
////////////////////////////////////////////////////////////////////////
// keypress/release callback function
//
// This assures that the proper changes to the highlights, 
// currentState, and projectors are made
//
// It is called every time a meta key is pressed or released
//
////////////////////////////////////////////////////////////////////////
//

void
SoTrackballDragger::metaKeyChangeCB( void *, SoDragger *inDragger)
{
    SoTrackballDragger *tb = (SoTrackballDragger *) inDragger;

    SoHandleEventAction *ha = tb->getHandleEventAction();

    //[1] Only do this if we are grabbing
    if ( ha->getGrabber() != tb )
	return;

    //[2] We only want key press or release events.
    const SoEvent *event = tb->getEvent();
    if ( !SO_KEY_PRESS_EVENT(event, ANY) && !SO_KEY_RELEASE_EVENT(event, ANY))
	return;

    //[3] Is the key constrain or modify?
    const SoKeyboardEvent *ke = (const SoKeyboardEvent *) event;
    SoKeyboardEvent::Key key = ke->getKey();
    if ( key == SoKeyboardEvent::LEFT_SHIFT ||
	 key == SoKeyboardEvent::RIGHT_SHIFT || 
	 key == SoKeyboardEvent::LEFT_CONTROL || 
	 key == SoKeyboardEvent::RIGHT_CONTROL ) {

	// We want to end the old gesture and start a new one.

	// [A] Release the grabber. This ends the gesture and calls all 
	//     finishCallbacks (on parent dragger, too, if we're registered)
	    ha->releaseGrabber();

	// [B] Set the starting point to be our saved prevWorldHitPt
	    tb->setStartingPoint( tb->prevWorldHitPt );

	// [C] Become the grabber again. This begins a new gesture and calls all
	//     startCallbacks (parent dragger, too).  Info like viewVolume, 
	//     viewportRegion, handleEventAction, and tempPathToThis 
	//     is still valid.
	    ha->setGrabber(tb);

	// [D] set handled
	    ha->setHandled();
    }
}

////////////////////////////////////////////////////////////////////
//  Stubs for callbacks
////////////////////////////////////////////////////////////////////
void
SoTrackballDragger::startCB( void *, SoDragger *inDragger )
{
    SoTrackballDragger *tb = (SoTrackballDragger *) inDragger;
    tb->dragStart();
}

void
SoTrackballDragger::motionCB( void *, SoDragger *inDragger )
{
    SoTrackballDragger *tb = (SoTrackballDragger *) inDragger;
    tb->drag();
}

void
SoTrackballDragger::finishCB( void *, SoDragger *inDragger )
{
    SoTrackballDragger *tb = (SoTrackballDragger *) inDragger;
    tb->dragFinish();
}

void
SoTrackballDragger::valueChangedCB( void *, SoDragger *inDragger )
{
    SoTrackballDragger *m = (SoTrackballDragger *) inDragger;
    SbMatrix motMat = m->getMotionMatrix();

    SbVec3f    trans, scale;
    SbRotation rot, scaleOrient;
    getTransformFast( motMat, trans, rot, scale, scaleOrient);

    // Disconnect the field sensors
    m->rotFieldSensor->detach();
    m->scaleFieldSensor->detach();

    if ( m->rotation.getValue() != rot )
	m->rotation = rot;
    if ( m->scaleFactor.getValue() != scale )
	m->scaleFactor = scale;

    // Reconnect the field sensors
    m->rotFieldSensor->attach( &(m->rotation) );
    m->scaleFieldSensor->attach( &(m->scaleFactor) );
}

void
SoTrackballDragger::fieldSensorCB( void *inDragger, SoSensor * )
{
    SoTrackballDragger *dragger = (SoTrackballDragger *) inDragger;

    SbMatrix motMat = dragger->getMotionMatrix();
    dragger->workFieldsIntoTransform( motMat );

    dragger->setMotionMatrix( motMat );
}

void
SoTrackballDragger::setDefaultOnNonWritingFields()
{
    // These nodes may change after construction, but we still
    // don't want to write them out.
    surroundScale.setDefault(TRUE);
    antiSquish.setDefault(TRUE);

    // If the userAxis is not being displayed, then do not write
    // out the userAxisRotation node.
    SoSwitch *sw = (SoSwitch *) userAxisSwitch.getValue();
    if ( sw == NULL || sw->whichChild.getValue() == SO_SWITCH_NONE ) {
	userAxisRotation.setDefault(TRUE);
    }

    // Call the base class...
    SoDragger::setDefaultOnNonWritingFields();
}
