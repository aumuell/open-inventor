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
 |	SoTranslate2Dragger
 |
 |   Author(s): Paul Isaacs
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
#include <Inventor/draggers/SoTranslate2Dragger.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/events/SoEvent.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/actions/SoHandleEventAction.h>

#include "geom/SoTranslate2DraggerGeom.h"


SO_KIT_SOURCE(SoTranslate2Dragger);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
SoTranslate2Dragger::SoTranslate2Dragger()
//
////////////////////////////////////////////////////////////////////////
{
    SO_KIT_CONSTRUCTOR(SoTranslate2Dragger);

    isBuiltIn = TRUE;

    // Put this stuff under the geomSeparator so it will draw more
    // efficiently.
    SO_KIT_ADD_CATALOG_ENTRY(translatorSwitch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(translator, SoSeparator, TRUE,
				translatorSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(translatorActive, SoSeparator, TRUE,
				translatorSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(feedbackSwitch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(feedback, SoSeparator, TRUE,
				feedbackSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(feedbackActive, SoSeparator, TRUE,
				feedbackSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(axisFeedbackSwitch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(xAxisFeedback, SoSeparator, TRUE,
				axisFeedbackSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(yAxisFeedback, SoSeparator, TRUE,
				axisFeedbackSwitch, ,TRUE);

    // read geometry for shared parts
    if (SO_KIT_IS_FIRST_INSTANCE())
	readDefaultParts("translate2Dragger.iv", geomBuffer, sizeof(geomBuffer) );

    SO_KIT_ADD_FIELD(translation, (0.0, 0.0, 0.0));

    SO_KIT_INIT_INSTANCE();

    // create the parts...
   setPartAsDefault("translator",      "translate2Translator");
   setPartAsDefault("translatorActive","translate2TranslatorActive");
   setPartAsDefault("feedback",        "translate2Feedback");
   setPartAsDefault("feedbackActive",  "translate2FeedbackActive");
   setPartAsDefault("xAxisFeedback",  "translate2XAxisFeedback");
   setPartAsDefault("yAxisFeedback",  "translate2YAxisFeedback");

    // Set the switches to 0...
    setSwitchValue( translatorSwitch.getValue(), 0 );
    setSwitchValue( feedbackSwitch.getValue(), 0 );

    // Set the axis feedback switch to SO_SWITCH_NONE
    // They're only displayed while dragging
    setSwitchValue( axisFeedbackSwitch.getValue(), SO_SWITCH_NONE );

    // Create the line projector
    planeProj = new SbPlaneProjector();

    // add the callbacks to perform the dragging
    addStartCallback(  &SoTranslate2Dragger::startCB );
    addMotionCallback( &SoTranslate2Dragger::motionCB );
    addFinishCallback(   &SoTranslate2Dragger::finishCB );

    // add callback to respond to the shift key, for constraining motion
    // to a line within the plane.
    addOtherEventCallback( &SoTranslate2Dragger::metaKeyChangeCB );

    // Updates the translation field when the motionMatrix is set.
    addValueChangedCallback( &SoTranslate2Dragger::valueChangedCB );

    // Updates the motionMatrix when the translation field is set.
    fieldSensor = new SoFieldSensor( &SoTranslate2Dragger::fieldSensorCB, this);
    fieldSensor->setPriority( 0 );

    setUpConnections( TRUE, TRUE );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
SoTranslate2Dragger::~SoTranslate2Dragger()
//
////////////////////////////////////////////////////////////////////////
{
    delete planeProj;
    if (fieldSensor)
	delete fieldSensor;
}

//    detach/attach any sensors, callbacks, and/or field connections.
//    Called by:            start/end of SoBaseKit::readInstance
//    and on new copy by:   start/end of SoBaseKit::copy.
//    Classes that redefine must call setUpConnections(TRUE,TRUE) 
//    at end of constructor.
//    Returns the state of the node when this was called.
SbBool
SoTranslate2Dragger::setUpConnections( SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( onOff ) {

	// We connect AFTER base class.
	SoDragger::setUpConnections( onOff, FALSE );

	// Call the sensor CBs to make things are up-to-date.
	fieldSensorCB( this, NULL );

	// Connect the field sensors
	if (fieldSensor->getAttachedField() != &translation)
	    fieldSensor->attach( &translation );
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
//    Set up the highlighting, projector, and the initial hit on
//    the dragger
//
// Use: private
//
void
SoTranslate2Dragger::dragStart()
//
////////////////////////////////////////////////////////////////////////
{
    // Set the switches to 1...
    setSwitchValue( translatorSwitch.getValue(), 1 );
    setSwitchValue( feedbackSwitch.getValue(), 1 );

    // Set the axis feedback switch to display both.
    // They're displayed while dragging
    setSwitchValue( axisFeedbackSwitch.getValue(), SO_SWITCH_ALL );

    // There is no constrained direction...
    translateDir = -1;

    // Make a note of which modifier keys are down.
    shftDown = getEvent()->wasShiftDown();

    // This is the point we'll use if a metaKey callback makes us re-start.
	worldRestartPt = getWorldStartingPoint();

    // Establish the projector plane in working space.
    // Working space is space at end of motion matrix.
    // Plane normal is defined relative to the translatorPart, so 
    // use z-axis to construct the projector plane.
	SbVec3f startLocalHitPt = getLocalStartingPoint();
	planeProj->setPlane( SbPlane(SbVec3f(0,0,1), startLocalHitPt ) );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Stretch the dragger according to the motion along the plane
//    projector
//
// Use: private
//
void
SoTranslate2Dragger::drag()
//
////////////////////////////////////////////////////////////////////////
{
    // Set up the projector space and view.
    // Working space is space at end of motion matrix.
	planeProj->setViewVolume( getViewVolume() );    
	planeProj->setWorkingSpace( getLocalToWorldMatrix() );

    // Get newHitPt and startHitPt in workspace.
	SbVec3f newHitPt = planeProj->project( getNormalizedLocaterPosition()); 
	SbVec3f startHitPt = getLocalStartingPoint();

    // Convert newHitPt to world space and save this as our new worldRestartPt
	getLocalToWorldMatrix().multVecMatrix( newHitPt, worldRestartPt );

    // Figure out the translation relative to start position.
        SbVec3f motion = newHitPt - startHitPt;

    // Maybe we need to constrain the motion...
    if ( !shftDown )
	translateDir = -1;
    else {
	// The shift key is pressed. This means 1-D translation.
	if ( translateDir == -1 ) {
	    // The 1-D direction is not defined. Calculate it based on which
	    // direction got the maximum locater motion.
            if ( isAdequateConstraintMotion() ) {
		if ( fabs( motion[0]) > fabs( motion[1]))
		    translateDir = 0;
		else 
		    translateDir = 1;

		// Set the axis feedback switch to the given direction.
		setSwitchValue( axisFeedbackSwitch.getValue(), translateDir );
	    }
	    else {
		// Not ready to pick a direction yet. Don't move.
		return;
	    }
	}
	// get the projection of 'motion' onto the preferred axis.
	SbVec3f constrainedMotion(0,0,0);
	constrainedMotion[translateDir] = motion[translateDir];
	motion = constrainedMotion;
    }

    // Append this to the startMotionMatrix, which we saved at the beginning
    // of the drag, to find the current motion matrix.
	setMotionMatrix( appendTranslation( getStartMotionMatrix(), motion ) );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//
// Use: private
//
void
SoTranslate2Dragger::dragFinish()
//
////////////////////////////////////////////////////////////////////////
{
    // Set the switches to 0...
    setSwitchValue( translatorSwitch.getValue(), 0 );
    setSwitchValue( feedbackSwitch.getValue(), 0 );

    // Set the axis feedback switch to SO_SWITCH_NONE
    // They're only displayed while dragging
    setSwitchValue( axisFeedbackSwitch.getValue(), SO_SWITCH_NONE );
}

//
////////////////////////////////////////////////////////////////////////
// keypress/release callback functions
//
// These assure that the proper changes to the highlights, 
// currentState, and projectors are made
//
////////////////////////////////////////////////////////////////////////
//

void
SoTranslate2Dragger::metaKeyChangeCB( void *, SoDragger *inDragger)
{
    SoTranslate2Dragger *d = (SoTranslate2Dragger *) inDragger;
    SoHandleEventAction *ha = d->getHandleEventAction();

    // [1] Only do this if we are grabbing
    if (ha->getGrabber() != d )
	return;

    // [2] We only want key press or release events.
    const SoEvent *event = d->getEvent();
    if ( !SO_KEY_PRESS_EVENT(event, ANY) && !SO_KEY_RELEASE_EVENT(event, ANY))
	return;

    // [3] Is the key the shift key?
    const SoKeyboardEvent *ke = (const SoKeyboardEvent *) event;
    SoKeyboardEvent::Key key = ke->getKey();

    if ( key == SoKeyboardEvent::LEFT_SHIFT ||
	 key == SoKeyboardEvent::RIGHT_SHIFT ) {

	// We want to end the old gesture and start a new one.

	// [A] Release the grabber. This ends the gesture and calls all 
	//     finishCallbacks (on parent dragger, too, if we're registered)
	    ha->releaseGrabber();

	// [B] Set the starting point to be our saved worldRestartPoint
	    d->setStartingPoint( d->worldRestartPt );

	// [C] Become the grabber again. This begins a new gesture and calls all
	//     startCallbacks (parent dragger, too).  Info like viewVolume, 
	//     viewportRegion, handleEventAction, and 
	//     tempPathToThis is still valid.
	    ha->setGrabber(d);

	// [D] set handled
	    ha->setHandled();
    }
}

////////////////////////////////////////////////////////////////////
//  Stubs for callbacks
////////////////////////////////////////////////////////////////////
void
SoTranslate2Dragger::startCB( void *, SoDragger *inDragger )
{
    SoTranslate2Dragger *m = (SoTranslate2Dragger *) inDragger;
    m->dragStart();
}

void
SoTranslate2Dragger::motionCB( void *, SoDragger *inDragger )
{
    SoTranslate2Dragger *m = (SoTranslate2Dragger *) inDragger;
    m->drag();
}

void
SoTranslate2Dragger::finishCB( void *, SoDragger *inDragger )
{
    SoTranslate2Dragger *m = (SoTranslate2Dragger *) inDragger;
    m->dragFinish();
}

void
SoTranslate2Dragger::valueChangedCB( void *, SoDragger *inDragger )
{
    SoTranslate2Dragger *m = (SoTranslate2Dragger *) inDragger;
    SbMatrix motMat = m->getMotionMatrix();

    SbVec3f    trans, scale;
    SbRotation rot, scaleOrient;
    getTransformFast( motMat, trans, rot, scale, scaleOrient );

    // Disconnect the field sensor
    m->fieldSensor->detach();

    if ( m->translation.getValue() != trans )
	m->translation = trans;

    // Reconnect the field sensor
    m->fieldSensor->attach( &(m->translation) );
}

void
SoTranslate2Dragger::fieldSensorCB( void *inDragger, SoSensor *)
{
    SoTranslate2Dragger *dragger = (SoTranslate2Dragger *) inDragger;

    // Incorporate the new field value into the matrix...
    SbMatrix motMat = dragger->getMotionMatrix();
    dragger->workFieldsIntoTransform( motMat );

    dragger->setMotionMatrix( motMat );
}
