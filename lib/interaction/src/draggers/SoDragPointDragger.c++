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
 |	SoDragPointDragger
 |
 |   Author(s): Paul Isaacs, Howard Look
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <stdio.h>
#include <limits.h>

#include <Inventor/SoDB.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/sensors/SoFieldSensor.h>

#include <Inventor/SbLinear.h>
#include <Inventor/SoPath.h>
#include <Inventor/SoPickedPoint.h>

#include <Inventor/events/SoEvent.h>
#include <Inventor/events/SoKeyboardEvent.h>

#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>

#include <Inventor/draggers/SoDragPointDragger.h>
#include <Inventor/draggers/SoTranslate1Dragger.h>
#include <Inventor/draggers/SoTranslate2Dragger.h>

#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoTranslation.h>

#include <Inventor/projectors/SbLineProjector.h>
#include <Inventor/projectors/SbPlaneProjector.h>

#include "geom/SoDragPointDraggerGeom.h"


SO_KIT_SOURCE(SoDragPointDragger);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
SoDragPointDragger::SoDragPointDragger()
//
////////////////////////////////////////////////////////////////////////
{
    SO_KIT_CONSTRUCTOR(SoDragPointDragger);

    isBuiltIn = TRUE;

    SO_KIT_ADD_CATALOG_ENTRY(noRotSep, SoSeparator, FALSE,
				topSeparator, geomSeparator,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(xTranslatorSwitch, SoSwitch, FALSE, 
				noRotSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(xTranslator, SoTranslate1Dragger, TRUE, 
				xTranslatorSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(xyTranslatorSwitch, SoSwitch, FALSE, 
				noRotSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(xyTranslator, SoTranslate2Dragger, TRUE, 
				xyTranslatorSwitch, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(rotXSep, SoSeparator, FALSE,
				topSeparator, geomSeparator,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(rotX, SoRotation, TRUE, 
				rotXSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(xzTranslatorSwitch, SoSwitch, FALSE, 
				rotXSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(xzTranslator, SoTranslate2Dragger, TRUE, 
				xzTranslatorSwitch, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(rotYSep, SoSeparator, FALSE,
				topSeparator, geomSeparator,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(rotY, SoRotation, TRUE, 
				rotYSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(zTranslatorSwitch, SoSwitch, FALSE, 
				rotYSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(zTranslator, SoTranslate1Dragger, TRUE, 
				zTranslatorSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(yzTranslatorSwitch, SoSwitch, FALSE, 
				rotYSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(yzTranslator, SoTranslate2Dragger, TRUE, 
				yzTranslatorSwitch, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(rotZSep, SoSeparator, FALSE,
				topSeparator, geomSeparator,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(rotZ, SoRotation, TRUE, 
				rotZSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(yTranslatorSwitch, SoSwitch, FALSE, 
				rotZSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(yTranslator, SoTranslate1Dragger, TRUE, 
				yTranslatorSwitch, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(xFeedbackSwitch, SoSwitch, FALSE, 
				topSeparator, geomSeparator,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(xFeedbackSep, SoSeparator, FALSE,
				xFeedbackSwitch, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(xFeedbackTranslation, SoTranslation, FALSE,
				xFeedbackSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(xFeedback, SoSeparator, TRUE,
				xFeedbackSep, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(yFeedbackSwitch, SoSwitch, FALSE, 
				topSeparator, geomSeparator,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(yFeedbackSep, SoSeparator, FALSE,
				yFeedbackSwitch, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(yFeedbackTranslation, SoTranslation, FALSE,
				yFeedbackSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(yFeedback, SoSeparator, TRUE,
				yFeedbackSep, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(zFeedbackSwitch, SoSwitch, FALSE, 
				topSeparator, geomSeparator,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(zFeedbackSep, SoSeparator, FALSE,
				zFeedbackSwitch, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(zFeedbackTranslation, SoTranslation, FALSE,
				zFeedbackSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(zFeedback, SoSeparator, TRUE,
				zFeedbackSep, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(planeFeedbackSep, SoSeparator, FALSE,
				topSeparator, geomSeparator,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(planeFeedbackTranslation, SoTranslation, FALSE,
				planeFeedbackSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(planeFeedbackSwitch, SoSwitch, FALSE, 
				planeFeedbackSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(yzFeedback, SoSeparator, TRUE,
				planeFeedbackSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(xzFeedback, SoSeparator, TRUE,
				planeFeedbackSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(xyFeedback, SoSeparator, TRUE,
				planeFeedbackSwitch, ,TRUE);

    // read geometry for shared parts
    if (SO_KIT_IS_FIRST_INSTANCE())
	readDefaultParts("dragPointDragger.iv",geomBuffer,sizeof(geomBuffer) );

    // The field that reflects where the dragger has been translated to
    SO_KIT_ADD_FIELD(translation, (0.0, 0.0, 0.0));

    SO_KIT_INIT_INSTANCE();

    // Cached values to make updating the feedback geometry more efficient
    oldXAxisNode = NULL;
    oldYAxisNode = NULL;
    oldZAxisNode = NULL;

    //******************
    // Set up the parts.
    //******************

    // Set up the rotations to orient the draggers correctly.
    SoRotation *myRotX = new SoRotation;
    SoRotation *myRotY = new SoRotation;
    SoRotation *myRotZ = new SoRotation;
    myRotX->rotation = SbRotation( SbVec3f(1,0,0), 1.57079 );
    myRotY->rotation = SbRotation( SbVec3f(0,1,0), 1.57079 );
    myRotZ->rotation = SbRotation( SbVec3f(0,0,1), 1.57079 );
    setAnyPartAsDefault("rotX", myRotX );
    setAnyPartAsDefault("rotY", myRotY );
    setAnyPartAsDefault("rotZ", myRotZ );

    // CREATE THE CHILD DRAGGERS.
    // Create the translate1Draggers...
    SoTranslate1Dragger *myXTrans, *myYTrans, *myZTrans;
    myXTrans = SO_GET_ANY_PART(this,"xTranslator",SoTranslate1Dragger);
    myYTrans = SO_GET_ANY_PART(this,"yTranslator",SoTranslate1Dragger);
    myZTrans = SO_GET_ANY_PART(this,"zTranslator",SoTranslate1Dragger);

    // Create the translate2Draggers...
    SoTranslate2Dragger *myYZTrans, *myXZTrans, *myXYTrans;
    myYZTrans = SO_GET_ANY_PART(this,"yzTranslator",SoTranslate2Dragger);
    myXZTrans = SO_GET_ANY_PART(this,"xzTranslator",SoTranslate2Dragger);
    myXYTrans = SO_GET_ANY_PART(this,"xyTranslator",SoTranslate2Dragger);

    //******************
    // The feedback parts jump around as the limit box changes. That is, they
    // stay fixed in space while the dragger moves around.
    // However, they jump to a new location when the dragger nears the edge.
    // These parts a separate translation node, since they move differently
    // than the dragger itself.
    //******************

    //******************
    // The feedback parts jump around as the limit box changes. That is, they
    // stay fixed in space while the dragger moves around.
    // However, they jump to a new location when the dragger nears the edge.
    // These parts a separate translation node, since they move differently
    // than the dragger itself.
    //
    // Only one plane or one axis is shown at a time, depending on which
    // translator has initiated the dragging.
    //******************

   setPartAsDefault("xFeedback",      "dragPointXFeedback");
   setPartAsDefault("yFeedback",      "dragPointYFeedback");
   setPartAsDefault("zFeedback",      "dragPointZFeedback");

   setPartAsDefault("yzFeedback", "dragPointYZFeedback");
   setPartAsDefault("xzFeedback", "dragPointXZFeedback");
   setPartAsDefault("xyFeedback", "dragPointXYFeedback");

    //********************
    // initialize state, limitbox, gesture variables
    //********************

    // To begin with, only the yTranslator and xzTranslators are turned on.
    // You can switch between pairs of line/plane draggers by hitting the 
    // CONTROL key
    setSwitchValue(xTranslatorSwitch.getValue(),   SO_SWITCH_NONE );
    setSwitchValue(yTranslatorSwitch.getValue(),   0 );
    setSwitchValue(zTranslatorSwitch.getValue(),   SO_SWITCH_NONE );
    setSwitchValue(yzTranslatorSwitch.getValue(),   SO_SWITCH_NONE );
    setSwitchValue(xzTranslatorSwitch.getValue(),   0 );
    setSwitchValue(xyTranslatorSwitch.getValue(),  SO_SWITCH_NONE );

    // ??? Would be cool to be able to choose a free
    // axis, rotate it around to whatever direction, maybe even
    // have it snap to things in the scene, then constrain
    // dragging to that line.

    // Start off inactive
    currentDragger = NULL;

    // The state of the modifier keys
    shftDown = FALSE;

    // Need to initialize since checkBoxLimits will look at it...
    startLocalHitPt.setValue(0,0,0);

    // The jump axes will jump when the edit point gets within
    // 10% of their ends
    jumpLimit = .1;

    // makes the offsetWorkLimit box
    limitBox.makeEmpty();

    updateLimitBoxAndFeedback();

    // These will be called by the child draggers after they call
    // their own callbacks...
    addStartCallback( &SoDragPointDragger::startCB );
    addMotionCallback( &SoDragPointDragger::motionCB );
    addFinishCallback( &SoDragPointDragger::finishCB );

    // When modify keys are released, we need to turn off any constraints.
    addOtherEventCallback( &SoDragPointDragger::metaKeyChangeCB );

    // Updates the translation field when the motionMatrix is set.
    addValueChangedCallback( &SoDragPointDragger::valueChangedCB );

    // Updates the motionMatrix when the translation field is set.
    fieldSensor = new SoFieldSensor( &SoDragPointDragger::fieldSensorCB, this);
    fieldSensor->setPriority( 0 );

    setUpConnections( TRUE, TRUE );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: public
//
SoDragPointDragger::~SoDragPointDragger()
//
////////////////////////////////////////////////////////////////////////
{
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
SoDragPointDragger::setUpConnections( SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( onOff ) {

	// We connect AFTER base class.
	SoDragger::setUpConnections( onOff, FALSE );

	// SET Parts and callbacks on CHLD DRAGGERS
	// Create the translate1Draggers...
	SoSeparator *dummySep = new SoSeparator;
	dummySep->ref();

	SoDragger *xD, *yD, *zD;
	xD = (SoDragger *) getAnyPart("xTranslator",FALSE);
	yD = (SoDragger *) getAnyPart("yTranslator",FALSE);
	zD = (SoDragger *) getAnyPart("zTranslator",FALSE);
	if (xD) {
	    xD->setPartAsDefault("translator","dragPointXTranslatorTranslator");
	    xD->setPartAsDefault("translatorActive",
			    "dragPointXTranslatorTranslatorActive");
	    xD->setPartAsDefault("feedback", dummySep );
	    xD->setPartAsDefault("feedbackActive", dummySep );
	    registerChildDragger( xD );
	}
	if (yD) {
	    yD->setPartAsDefault("translator","dragPointYTranslatorTranslator");
	    yD->setPartAsDefault("translatorActive",
			    "dragPointYTranslatorTranslatorActive");
	    yD->setPartAsDefault("feedback", dummySep );
	    yD->setPartAsDefault("feedbackActive", dummySep );
	    registerChildDragger( yD );
	}
	if (zD) {
	    zD->setPartAsDefault("translator","dragPointZTranslatorTranslator");
	    zD->setPartAsDefault("translatorActive",
			    "dragPointZTranslatorTranslatorActive");
	    zD->setPartAsDefault("feedback", dummySep );
	    zD->setPartAsDefault("feedbackActive", dummySep );
	    registerChildDragger( zD );
	}

	// Create the translate2Draggers...
	SoDragger *yzD, *xzD, *xyD;
	yzD = (SoDragger *) getAnyPart("yzTranslator",FALSE);
	xzD = (SoDragger *) getAnyPart("xzTranslator",FALSE);
	xyD = (SoDragger *) getAnyPart("xyTranslator",FALSE);
	if (yzD) {
	    yzD->setPartAsDefault("translator",
			    "dragPointYZTranslatorTranslator");
	    yzD->setPartAsDefault("translatorActive",
			    "dragPointYZTranslatorTranslatorActive");
	    yzD->setPartAsDefault("feedback", dummySep );
	    yzD->setPartAsDefault("feedbackActive", dummySep );
	    yzD->setPartAsDefault("xAxisFeedback", dummySep );
	    yzD->setPartAsDefault("yAxisFeedback", dummySep );
	    registerChildDragger( yzD );
	}

	if (xzD) {
	    xzD->setPartAsDefault("translator",
			    "dragPointXZTranslatorTranslator");
	    xzD->setPartAsDefault("translatorActive",
			    "dragPointXZTranslatorTranslatorActive");
	    xzD->setPartAsDefault("feedback", dummySep );
	    xzD->setPartAsDefault("feedbackActive", dummySep );
	    xzD->setPartAsDefault("xAxisFeedback", dummySep );
	    xzD->setPartAsDefault("yAxisFeedback", dummySep );
	    registerChildDragger( xzD );
	}

	if (xyD) {
	    xyD->setPartAsDefault("translator",
			    "dragPointXYTranslatorTranslator");
	    xyD->setPartAsDefault("translatorActive",
			    "dragPointXYTranslatorTranslatorActive");
	    xyD->setPartAsDefault("feedback", dummySep );
	    xyD->setPartAsDefault("feedbackActive", dummySep );
	    xyD->setPartAsDefault("xAxisFeedback", dummySep );
	    xyD->setPartAsDefault("yAxisFeedback", dummySep );
	    registerChildDragger( xyD );
	}

	dummySep->unref();

	// Call the sensor CBs to make things are up-to-date.
	fieldSensorCB( this, NULL );

	// Connect the field sensors
	if (fieldSensor->getAttachedField() != &translation)
	    fieldSensor->attach( &translation );
    }
    else {

	// We disconnect BEFORE base class.

	// Remove callbacks from CHLD DRAGGERS
	SoDragger *xD, *yD, *zD;
	xD = (SoDragger *) getAnyPart("xTranslator",FALSE);
	yD = (SoDragger *) getAnyPart("yTranslator",FALSE);
	zD = (SoDragger *) getAnyPart("zTranslator",FALSE);
	if (xD)
	    unregisterChildDragger( xD );
	if (yD)
	    unregisterChildDragger( yD );
	if (zD)
	    unregisterChildDragger( zD );

	SoDragger *yzD, *xzD, *xyD;
	yzD = (SoDragger *) getAnyPart("yzTranslator",FALSE);
	xzD = (SoDragger *) getAnyPart("xzTranslator",FALSE);
	xyD = (SoDragger *) getAnyPart("xyTranslator",FALSE);
	if (yzD)
	    unregisterChildDragger( yzD );
	if (xzD)
	    unregisterChildDragger( xzD );
	if (xyD)
	    unregisterChildDragger( xyD );

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
//    Called by a child dragger after dragStart.
//
//   Figures out which dragger is active, then updates the feedback
//   accordingly
//
// Use: protected
//
void
SoDragPointDragger::dragStart()
//
////////////////////////////////////////////////////////////////////////
{
    currentDragger = getActiveChildDragger();

    // If there is no active child dragger, then we should just return.
    if ( currentDragger == NULL)
	return;

    shftDown = getEvent()->wasShiftDown();

    // Save the starting point as expressed in local space.
    // We need to do this so that our feedback will work nicely.
    startLocalHitPt = getLocalStartingPoint();

    updateLimitBoxAndFeedback();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Called by a child dragger after dragging.
//  Update the feedback to sit in the right place.
//
// Use: protected
//
void
SoDragPointDragger::drag()
//
////////////////////////////////////////////////////////////////////////
{
    updateLimitBoxAndFeedback();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Called by a child dragger after dragFinish.
//
// Use: protected
//
void
SoDragPointDragger::dragFinish()
//
////////////////////////////////////////////////////////////////////////
{
    currentDragger = NULL;

    updateLimitBoxAndFeedback();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the feedback geometry depending on which dragger is active.
//    A 1-D translator results in that line's feedback turning on.
//    A 2-D translator results in that plane's feedback turning on.
//
// Use: private
//
void
SoDragPointDragger::setFeedbackGeometry()
//
////////////////////////////////////////////////////////////////////////
{
    // First, figure out if we need to know a constrained translation
    // direction. This happens if a plane dragger is in use, but is
    // being constrained.
    int translateDir = -1;
#define TINY .0001
    if ( shftDown ) {
	SbVec3f current = getMotionMatrix()[3];
	SbVec3f start   = getStartMotionMatrix()[3];
	SbVec3f motion  = current - start;
	if (    fabs( motion[0]) > fabs( motion[1])
	     && fabs( motion[0]) > fabs( motion[2]))
	     translateDir = 0;
	else if ( fabs( motion[1]) > fabs( motion[2]))
	    translateDir = 1;
	else if ( fabs( motion[2] ) > TINY )
	    translateDir = 2;
    }
#undef TINY

    // Turn on appropriate parts, based on state.
    if (currentDragger == NULL) {
	setSwitchValue(xFeedbackSwitch.getValue(),      SO_SWITCH_NONE );
	setSwitchValue(yFeedbackSwitch.getValue(),      SO_SWITCH_NONE );
	setSwitchValue(zFeedbackSwitch.getValue(),      SO_SWITCH_NONE );
	setSwitchValue(planeFeedbackSwitch.getValue(),  SO_SWITCH_NONE );
    }
    else if (currentDragger == xTranslator.getValue() ) {
	setSwitchValue(xFeedbackSwitch.getValue(),      0 );
	setSwitchValue(yFeedbackSwitch.getValue(),      SO_SWITCH_NONE );
	setSwitchValue(zFeedbackSwitch.getValue(),      SO_SWITCH_NONE );
	setSwitchValue(planeFeedbackSwitch.getValue(),  SO_SWITCH_NONE );
    }
    else if (currentDragger == yTranslator.getValue() ) {
	setSwitchValue(xFeedbackSwitch.getValue(),      SO_SWITCH_NONE );
	setSwitchValue(yFeedbackSwitch.getValue(),      0 );
	setSwitchValue(zFeedbackSwitch.getValue(),      SO_SWITCH_NONE );
	setSwitchValue(planeFeedbackSwitch.getValue(),  SO_SWITCH_NONE );
    }
    else if (currentDragger == zTranslator.getValue() ) {
	setSwitchValue(xFeedbackSwitch.getValue(),      SO_SWITCH_NONE );
	setSwitchValue(yFeedbackSwitch.getValue(),      SO_SWITCH_NONE );
	setSwitchValue(zFeedbackSwitch.getValue(),      0 );
	setSwitchValue(planeFeedbackSwitch.getValue(),  SO_SWITCH_NONE );
    }
    else if (currentDragger == yzTranslator.getValue() ) {
	setSwitchValue(xFeedbackSwitch.getValue(),      SO_SWITCH_NONE );
	if ( translateDir == -1 || translateDir == 1 )
	    setSwitchValue(yFeedbackSwitch.getValue(),  0 );
	else
	    setSwitchValue(yFeedbackSwitch.getValue(),  SO_SWITCH_NONE );
	if ( translateDir == -1 || translateDir == 2 )
	    setSwitchValue(zFeedbackSwitch.getValue(),  0 );
	else
	    setSwitchValue(zFeedbackSwitch.getValue(),  SO_SWITCH_NONE );
	setSwitchValue(planeFeedbackSwitch.getValue(),  0 );
    }
    else if (currentDragger == xzTranslator.getValue() ) {
	if ( translateDir == -1 || translateDir == 0 )
	    setSwitchValue(xFeedbackSwitch.getValue(),  0 );
	else
	    setSwitchValue(xFeedbackSwitch.getValue(),  SO_SWITCH_NONE );
	setSwitchValue(yFeedbackSwitch.getValue(),      SO_SWITCH_NONE );
	if ( translateDir == -1 || translateDir == 2 )
	    setSwitchValue(zFeedbackSwitch.getValue(),  0 );
	else
	    setSwitchValue(zFeedbackSwitch.getValue(),  SO_SWITCH_NONE );
	setSwitchValue(planeFeedbackSwitch.getValue(),  1 );
    }
    else if (currentDragger == xyTranslator.getValue() ) {
	if ( translateDir == -1 || translateDir == 0 )
	    setSwitchValue(xFeedbackSwitch.getValue(),  0 );
	else
	    setSwitchValue(xFeedbackSwitch.getValue(),  SO_SWITCH_NONE );
	if ( translateDir == -1 || translateDir == 1 )
	    setSwitchValue(yFeedbackSwitch.getValue(),  0 );
	else
	    setSwitchValue(yFeedbackSwitch.getValue(),  SO_SWITCH_NONE );
	setSwitchValue(zFeedbackSwitch.getValue(),      SO_SWITCH_NONE );
	setSwitchValue(planeFeedbackSwitch.getValue(),  2 );
    }

    checkBoxLimits();

    //**************************************
    // Set the transforms for the feedback axes and/or planes.
    //**************************************

    // First, find the center and scale for each part, in LOCAL space.

	// The limit box is defined in a space aligned and scaled as 
	// LOCAL space, but with it's center remaining fixed in WORLD 
	// space.

	// We need to transform the center of the limitBox
	// from WORLD space to LOCAL space to 
	// find the location of the center of the limit box feedback.
	SbMatrix worldToLocal = getWorldToLocalMatrix();

	SbVec3f limitBoxCenterInLocal = limitBox.getCenter();
	worldToLocal.multVecMatrix( limitBoxCenterInLocal, 
				     limitBoxCenterInLocal );

	SbVec3f xAxisSpot = limitBoxCenterInLocal;
	SbVec3f yAxisSpot = limitBoxCenterInLocal;
	SbVec3f zAxisSpot = limitBoxCenterInLocal;
	SbVec3f planeSpot = limitBoxCenterInLocal;

        //With center matching mouse location...
	// The point under the mouse is stored as 'startLocalHitPt', which is
	// expressed in LOCAL space. 

	xAxisSpot[1] = startLocalHitPt[1];
	xAxisSpot[2] = startLocalHitPt[2];

	yAxisSpot[0] = startLocalHitPt[0];
	yAxisSpot[2] = startLocalHitPt[2];

	zAxisSpot[0] = startLocalHitPt[0];
	zAxisSpot[1] = startLocalHitPt[1];

	if (   currentDragger == yzTranslator.getValue()) {
	    planeSpot[0] = startLocalHitPt[0];
	}
	else if (   currentDragger == xzTranslator.getValue()) {
	    planeSpot[1] = startLocalHitPt[1];
	}
	else if (   currentDragger == xyTranslator.getValue()) {
	    planeSpot[2] = startLocalHitPt[2];
	}

    SoTranslation *transNode;
    // Set position of each axis feedback
	if ( xFeedbackTranslation.getValue() == NULL )
	    setAnyPart( "xFeedbackTranslation", new SoTranslation );
	transNode = (SoTranslation *) xFeedbackTranslation.getValue();
	transNode->translation = xAxisSpot;

	if ( yFeedbackTranslation.getValue() == NULL )
	    setAnyPart( "yFeedbackTranslation", new SoTranslation );
	transNode = (SoTranslation *) yFeedbackTranslation.getValue();
	transNode->translation = yAxisSpot;

	if ( zFeedbackTranslation.getValue() == NULL )
	    setAnyPart( "zFeedbackTranslation", new SoTranslation );
	transNode = (SoTranslation *) zFeedbackTranslation.getValue();
	transNode->translation = zAxisSpot;

    // Set position of plane feedback
	if ( planeFeedbackTranslation.getValue() == NULL )
	    setAnyPart( "planeFeedbackTranslation", new SoTranslation );
	transNode = (SoTranslation *) planeFeedbackTranslation.getValue();
	transNode->translation = planeSpot;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//
// Sets switches to show one line dragger and one plane dragger.
// It cycles through 3 different configurations:
// xline/yzPlane, yline/xzplane, zline/xyplane
//
////////////////////////////////////////////////////////////////////////
void
SoDragPointDragger::showNextDraggerSet()
{
    SoSwitch *sw;

    // Case 1. Currently, xline is displayed. Switch to y.
    sw = (SoSwitch *) xTranslatorSwitch.getValue();
    if ( sw && sw->whichChild.getValue() != SO_SWITCH_NONE ) {
	setSwitchValue(xTranslatorSwitch.getValue(),   SO_SWITCH_NONE );
	setSwitchValue(yTranslatorSwitch.getValue(),   0 );
	setSwitchValue(zTranslatorSwitch.getValue(),   SO_SWITCH_NONE );
	setSwitchValue(yzTranslatorSwitch.getValue(),   SO_SWITCH_NONE );
	setSwitchValue(xzTranslatorSwitch.getValue(),   0 );
	setSwitchValue(xyTranslatorSwitch.getValue(),  SO_SWITCH_NONE );
	return;
    }

    // Case 2. Currently, yline is displayed. Switch to z.
    sw = (SoSwitch *) yTranslatorSwitch.getValue();
    if ( sw && sw->whichChild.getValue() != SO_SWITCH_NONE ) {
	setSwitchValue(xTranslatorSwitch.getValue(),   SO_SWITCH_NONE );
	setSwitchValue(yTranslatorSwitch.getValue(),   SO_SWITCH_NONE );
	setSwitchValue(zTranslatorSwitch.getValue(),   0 );
	setSwitchValue(yzTranslatorSwitch.getValue(),   SO_SWITCH_NONE );
	setSwitchValue(xzTranslatorSwitch.getValue(),   SO_SWITCH_NONE );
	setSwitchValue(xyTranslatorSwitch.getValue(),  0 );
	return;
    }

    // Case 3. Currently, zline is displayed. Switch to x.
    sw = (SoSwitch *) zTranslatorSwitch.getValue();
    if ( sw && sw->whichChild.getValue() != SO_SWITCH_NONE ) {
	setSwitchValue(xTranslatorSwitch.getValue(),   0 );
	setSwitchValue(yTranslatorSwitch.getValue(),   SO_SWITCH_NONE );
	setSwitchValue(zTranslatorSwitch.getValue(),   SO_SWITCH_NONE );
	setSwitchValue(yzTranslatorSwitch.getValue(),   0 );
	setSwitchValue(xzTranslatorSwitch.getValue(),   SO_SWITCH_NONE );
	setSwitchValue(xyTranslatorSwitch.getValue(),  SO_SWITCH_NONE );
	return;
    }
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//   This routine sets the limitBox. 
// The limit box is defined in a space aligned and scaled as 
// LOCAL space, but with it's center remaining fixed in WORLD 
// space.
//
// Use: static private
//
void
SoDragPointDragger::updateLimitBoxAndFeedback()
//
////////////////////////////////////////////////////////////////////////
{
    // This gets called in the constructor, while the ref count is still 0.
    // Since there will be some ref'ing and unref'ing done inside here,
    // add a temporary ref and undo it at the end.
    ref();

    if ( xFeedback.getValue() != oldXAxisNode ||
	 yFeedback.getValue() != oldYAxisNode ||
	 zFeedback.getValue() != oldZAxisNode ) {

	oldXAxisNode = SO_GET_ANY_PART(this,"xFeedback",SoSeparator);
	oldYAxisNode = SO_GET_ANY_PART(this,"yFeedback",SoSeparator);
	oldZAxisNode = SO_GET_ANY_PART(this,"zFeedback",SoSeparator);

	// Get the bounds of the axis parts. 
	static SoGetBoundingBoxAction *bba = NULL;
	if (bba == NULL)
	    bba = new SoGetBoundingBoxAction(getViewportRegion());
	else
	    bba->setViewportRegion(getViewportRegion());

	float   xMin, yMin, zMin, xMax, yMax, zMax;
	SbVec3f min, max;

	bba->apply(xFeedback.getValue());
	bba->getBoundingBox().getBounds( xMin, yMin, zMin, xMax, yMax, zMax );
	min[0] = xMin;
	max[0] = xMax;

	bba->apply(yFeedback.getValue());
	bba->getBoundingBox().getBounds( xMin, yMin, zMin, xMax, yMax, zMax );
	min[1] = yMin; 
	max[1] = yMax; 

	bba->apply(zFeedback.getValue());
	bba->getBoundingBox().getBounds( xMin, yMin, zMin, xMax, yMax, zMax );
	min[2] = zMin;
	max[2] = zMax;

	// The limit box is defined in a space aligned and scaled as 
	// LOCAL space, but with it's center remaining fixed in WORLD 
	// space.

	SbVec3f newDiag = (max - min) / 2.0;

	// Give a default size of 1, in case no axis parts exist.
	for (int i = 0; i < 3; i++) {
	    if (newDiag[i] <= getMinScale())
		newDiag[i] = 1.0;
	}

	SbVec3f oldDiag = limitBox.getMax() - 
			  limitBox.getCenter();

	// If the size of the boundingBox has changed...
	if ( newDiag != oldDiag ) {

	    // curEdit point needs to be the current origin expressed in world
	    // space (i.e., this bizarro space).
	    SbMatrix localToWorld = getLocalToWorldMatrix();

	    SbVec3f zeroPt(0,0,0);
	    localToWorld.multVecMatrix( zeroPt, zeroPt );

	    limitBox.setBounds(zeroPt - newDiag, zeroPt + newDiag );
	}
    }

    setFeedbackGeometry();

    // undo the temporary ref.
    unrefNoDelete();
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//  Checks the limit box and extends it if necessary
//
// Use: private

void
SoDragPointDragger::checkBoxLimits()
//
////////////////////////////////////////////////////////////////////////
{
    // The limit box is defined in a space aligned and scaled as LOCAL 
    // space, but with it's center remaining fixed in WORLD space.

    // We need to do this work in LOCAL space, so to begin with,
    // get the location of the center of this box in LOCAL space.
    SbMatrix worldToLocal = getWorldToLocalMatrix();
    SbVec3f limitBoxCenterInLocal = limitBox.getCenter();
    worldToLocal.multVecMatrix( limitBoxCenterInLocal, 
				     limitBoxCenterInLocal );

    // Now, if our current position is out of range, we need to move
    // the limit box center accordingly. We continue to do our work
    // in LOCAL space.

    SbBool changed = FALSE;
    SbVec3f boxSize = limitBox.getMax() - limitBox.getMin();

    for (int i = 0; i < 3; i++) {

	float length = boxSize[i];
	float halfLength = length * 0.5;

	// Check the location of startLocalHitPt against boundaries of the limit
	// box (keeping in mind the jump limit as a % of the total length).

	// Have we gone too far in the positive direction?
	float high = limitBoxCenterInLocal[i] + halfLength;
	while ( (high - startLocalHitPt[i]) / length < jumpLimit ) {
	    limitBoxCenterInLocal[i] += halfLength;
	    high += halfLength;
	    changed = TRUE;

	}

	// Have we gone too far in the negative direction?
	float low  = limitBoxCenterInLocal[i] - halfLength;
	while (( startLocalHitPt[i] - low ) / length < jumpLimit ) {
	    limitBoxCenterInLocal[i] -= halfLength;
	    low  -= halfLength;
	    changed = TRUE;
	}
    }

    if (changed == TRUE ) {
	// First, convert the changed limitBoxCenterInLocal
	// into WORLD space...
	SbMatrix localToWorld = getLocalToWorldMatrix();
	SbVec3f newCenter;
	localToWorld.multVecMatrix(limitBoxCenterInLocal,newCenter);

	// Next, set the bounds of the limit box to have the same size
	// as before, but centered about this new point.
	SbVec3f diag = limitBox.getMax() - 
		       limitBox.getCenter();

	limitBox.setBounds( newCenter - diag, newCenter + diag );
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Check to see if the Shift or Control key changed. 

void
SoDragPointDragger::metaKeyChangeCB( void *, SoDragger *inDragger )
//
////////////////////////////////////////////////////////////////////////
{
    SoDragPointDragger *dp = (SoDragPointDragger *) inDragger;

    SoHandleEventAction *ha = dp->getHandleEventAction();

    // We care if the shift key is down for drawing feedback.
	const SoEvent *event = dp->getEvent();
        dp->shftDown = event->wasShiftDown();

    // Don't check for grabber yet.
    // CONTROL key press overrides if the grabber is a childDragger.

    // [1] We only respond to CONTROL key press events.
	if ( !SO_KEY_PRESS_EVENT(event,  LEFT_CONTROL) &&
	     !SO_KEY_PRESS_EVENT(event, RIGHT_CONTROL))
	     return;

    //[2] Should we return?
    //    Stay here if there's no grabber and the mouse is over us,
    //         or over a surrogate part.
    //    Stay here if we are the grabber.
    //    Stay here if our currentDragger is grabbing events.
    //    Return if anyone else is grabbing.
	SbBool takeIt = FALSE;
	if ( ha->getGrabber() == NULL  ) {

	    // grabber is NULL...
	    const SoPickedPoint *p = ha->getPickedPoint();

	    if ( p && p->getPath()) {

		// Are we on the pickPath?
		// Or is the path a surrogate path for us or any draggers 
		// contained within us?
	        if (  p->getPath()->containsNode(dp) ||
		     dp->isPathSurrogateInMySubgraph(p->getPath()) )
		        takeIt = TRUE;
	    }
	}
	else if ( ha->getGrabber() == dp )
	    takeIt = TRUE;
	else if ( ha->getGrabber() == dp->currentDragger )
	    takeIt = TRUE;
	else 
	    takeIt = FALSE;

	if ( !takeIt )
	    return;

    //[3] Now, switch the set of visible draggers...
	dp->showNextDraggerSet();

    //[4] If a child is grabbing, release events and hand it all over to 
    //    the next one...
	SoDragger *oldDragger = dp->currentDragger;
	if (oldDragger) {

	    // Ref the oldDragger.
		oldDragger->ref();

	    // Release the grabber. This will call grabEventsCleanUp() if 
	    // the grabber is a dragger.
		ha->releaseGrabber();

	    // If there was an oldDragger, 
	    // [a] select a new dragger to grab events.
	    // [b] Set up a plane or line projector oriented like newDragger.
	    // [c] Find out where current mouse position intersects that new 
	    //     plane or line. The new gesture will continue from there.
		SoDragger *newDragger;
		SbVec3f    projPt;
		SbLineProjector  lp;
		SbPlaneProjector pp;
		lp.setViewVolume( dp->getViewVolume() );
		pp.setViewVolume( dp->getViewVolume() );
		lp.setWorkingSpace( dp->getLocalToWorldMatrix() );
		pp.setWorkingSpace( dp->getLocalToWorldMatrix() );

		if (      oldDragger == dp->xTranslator.getValue() ) {
		    newDragger = (SoDragger *) dp->yTranslator.getValue();
		    lp.setLine( SbLine( SbVec3f(0,0,0), SbVec3f(0,1,0)));
		    projPt = lp.project(dp->getNormalizedLocaterPosition());
		}
		else if ( oldDragger == dp->yTranslator.getValue() ) {
		    newDragger = (SoDragger *) dp->zTranslator.getValue();
		    lp.setLine( SbLine( SbVec3f(0,0,0), SbVec3f(0,0,1)));
		    projPt = lp.project(dp->getNormalizedLocaterPosition());
		}
		else if ( oldDragger == dp->zTranslator.getValue() ) {
		    newDragger = (SoDragger *) dp->xTranslator.getValue();
		    lp.setLine( SbLine( SbVec3f(0,0,0), SbVec3f(1,0,0)));
		    projPt = lp.project(dp->getNormalizedLocaterPosition());
		}
		else if ( oldDragger == dp->yzTranslator.getValue() ) {
		    newDragger = (SoDragger *) dp->xzTranslator.getValue();
		    pp.setPlane( SbPlane( SbVec3f(0,1,0), SbVec3f(0,0,0)));
		    projPt = pp.project(dp->getNormalizedLocaterPosition());
		}
		else if ( oldDragger == dp->xzTranslator.getValue() ) {
		    newDragger = (SoDragger *) dp->xyTranslator.getValue();
		    pp.setPlane( SbPlane( SbVec3f(0,0,1), SbVec3f(0,0,0)));
		    projPt = pp.project(dp->getNormalizedLocaterPosition());
		}
		else if ( oldDragger == dp->xyTranslator.getValue() ) {
		    newDragger = (SoDragger *) dp->yzTranslator.getValue();
		    pp.setPlane( SbPlane( SbVec3f(1,0,0), SbVec3f(0,0,0)));
		    projPt = pp.project(dp->getNormalizedLocaterPosition());
		}

	    // unref oldDragger. We don't need it any more.
		oldDragger->unref();

	    // Give newDragger our handleEvent action. 
		newDragger->setHandleEventAction(ha);

	    // Cast the projPt into world space for the new starting point.
		dp->getLocalToWorldMatrix().multVecMatrix(projPt,projPt);
		newDragger->setStartingPoint( projPt );

	    // Give the newDragger a path to itself
		SoPath *pathToDragger;

		    // We must ref() & unref() dpThisPath to dispose of it.
		    SoPath *dpThisPath = dp->createPathToThis();
		    if (dpThisPath) dpThisPath->ref();
		    pathToDragger = dp->createPathToPart(
			dp->getPartString(newDragger), TRUE, dpThisPath );
		    if (dpThisPath) dpThisPath->unref();

		if (pathToDragger)
		    pathToDragger->ref();
		newDragger->setTempPathToThis( pathToDragger );
		if (pathToDragger)
		    pathToDragger->unref();

	    // Give the newDragger our viewing information.
		newDragger->setViewVolume(dp->getViewVolume());
		newDragger->setViewportRegion(dp->getViewportRegion());

	    // Set the grabber. This will call starting callbacks on the new
	    // grabber, as well as it's registered parent, moi!
		ha->setGrabber( newDragger );
	}

    //[5] set handled
	ha->setHandled();
}

////////////////////////////////////////////////////////////////////
//  Stubs for callbacks
////////////////////////////////////////////////////////////////////
void
SoDragPointDragger::startCB( void *, SoDragger *inDragger )
{
    SoDragPointDragger *dp = (SoDragPointDragger *) inDragger;
    dp->dragStart();
}

void
SoDragPointDragger::motionCB( void *, SoDragger *inDragger )
{
    SoDragPointDragger *dp = (SoDragPointDragger *) inDragger;
    dp->drag();
}

void
SoDragPointDragger::finishCB( void *, SoDragger *inDragger )
{
    SoDragPointDragger *dp = (SoDragPointDragger *) inDragger;
    dp->dragFinish();
}

void
SoDragPointDragger::valueChangedCB( void *, SoDragger *inDragger )
{
    SoDragPointDragger *m = (SoDragPointDragger *) inDragger;
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
SoDragPointDragger::fieldSensorCB( void *inDragger, SoSensor *)
{
    SoDragPointDragger *dragger = (SoDragPointDragger *) inDragger;

    // Incorporate the new field value into the matrix...
    SbMatrix motMat = dragger->getMotionMatrix();
    dragger->workFieldsIntoTransform( motMat );

    dragger->setMotionMatrix( motMat );
}

void
SoDragPointDragger::setDefaultOnNonWritingFields()
{
    // These nodes may change after construction, but we still
    // don't want to write them out.
    xFeedbackTranslation.setDefault(TRUE);
    yFeedbackTranslation.setDefault(TRUE);
    zFeedbackTranslation.setDefault(TRUE);
    planeFeedbackTranslation.setDefault(TRUE);

    // Try not to write out the sub-draggers.
	xTranslator.setDefault(TRUE);
	yTranslator.setDefault(TRUE);
	zTranslator.setDefault(TRUE);

	xyTranslator.setDefault(TRUE);
	xzTranslator.setDefault(TRUE);
	yzTranslator.setDefault(TRUE);

    // Call the base class...
    SoDragger::setDefaultOnNonWritingFields();
}
