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
 * Copyright (C) 1992   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Classes:
 |	SoCenterballDragger
 |
 |   Author(s): Paul Isaacs, Howard Look
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <stdio.h>

#include <Inventor/SoDB.h> 
#include <Inventor/sensors/SoFieldSensor.h>

#include <Inventor/events/SoEvent.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/draggers/SoCenterballDragger.h>

#include <Inventor/elements/SoModelMatrixElement.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoSurroundScale.h>
#include <Inventor/nodes/SoAntiSquish.h>

#include <Inventor/draggers/SoTranslate2Dragger.h>
#include <Inventor/draggers/SoRotateCylindricalDragger.h>
#include <Inventor/draggers/SoRotateSphericalDragger.h>

#include "geom/SoCenterballDraggerGeom.h"

SO_KIT_SOURCE(SoCenterballDragger);


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
SoCenterballDragger::SoCenterballDragger()
//
////////////////////////////////////////////////////////////////////////
{
    SO_KIT_CONSTRUCTOR(SoDragger);

    isBuiltIn = TRUE;

    // Create the center translator, a matrix that will move 
    // the dragger over the center. It's separate from the motion matrix
    // because 'center' does not affect an aggregate tranform matrix.
    SO_KIT_ADD_CATALOG_ENTRY(translateToCenter, SoMatrixTransform, TRUE,
				topSeparator, geomSeparator, TRUE);

    // Don't create this by default. It's only really put into use
    // if this dragger is put inside a manipulator.
    SO_KIT_ADD_CATALOG_ENTRY(surroundScale, SoSurroundScale,
				    TRUE, topSeparator, geomSeparator, TRUE);
    // Create an anti-squish node by default.
    SO_KIT_ADD_CATALOG_ENTRY(antiSquish, SoAntiSquish,
				FALSE, topSeparator, geomSeparator, TRUE);

    // This whole manip gets modelled in BASE_COLOR
    SO_KIT_ADD_CATALOG_ENTRY(lightModel, SoLightModel,
				TRUE, topSeparator, geomSeparator,TRUE);

    // This little grouping goes under the geomSeparator, since it's 
    // geometry only, not draggers.
    SO_KIT_ADD_CATALOG_ENTRY(XAxisSwitch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(XAxis, SoSeparator, TRUE,
				XAxisSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(YAxisSwitch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(YAxis, SoSeparator, TRUE,
				YAxisSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(ZAxisSwitch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(ZAxis, SoSeparator, TRUE,
				ZAxisSwitch, ,TRUE);

    // The rest of the stuff will go after the geomSeparator
    SO_KIT_ADD_CATALOG_ENTRY(rotator, SoRotateSphericalDragger, TRUE,
				topSeparator, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(YRotator, SoRotateCylindricalDragger, TRUE,
				topSeparator, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(ZCenterChanger, SoTranslate2Dragger, TRUE,
				topSeparator, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(rotX90, SoRotation, TRUE,
				topSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(ZRotator, SoRotateCylindricalDragger, TRUE,
				topSeparator, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(YCenterChanger, SoTranslate2Dragger, TRUE,
				topSeparator, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(rotY90, SoRotation, TRUE,
				topSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(XCenterChanger, SoTranslate2Dragger, TRUE,
				topSeparator, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(rot2X90, SoRotation, TRUE,
				topSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(XRotator, SoRotateCylindricalDragger, TRUE,
				topSeparator, ,TRUE);


    if ( SO_KIT_IS_FIRST_INSTANCE())
	readDefaultParts("centerballDragger.iv",geomBuffer, sizeof(geomBuffer));

    SO_KIT_ADD_FIELD(rotation, (0.0, 0.0, 0.0, 1.0));
    SO_KIT_ADD_FIELD(center,   (0.0, 0.0, 0.0));

    SO_KIT_INIT_INSTANCE();

    // Make the anti-squish node surround the biggest dimension
    SoAntiSquish *as = SO_GET_ANY_PART( this, "antiSquish", SoAntiSquish);
    as->sizing = SoAntiSquish::LONGEST_DIAGONAL;

    // Make the dragger phong shaded by default.
    SoLightModel *lm = new SoLightModel;
    lm->model = SoLightModel::PHONG;
    setAnyPartAsDefault( "lightModel", lm );

    // Create the child draggers that live under this dragger.

	// ROTATOR BALL
	    SoDragger *ball = (SoDragger *)  getAnyPart("rotator", TRUE );

	// STRIPES
	    SoDragger *stps[3];
	    stps[0] = SO_GET_ANY_PART(this,"XRotator", SoDragger);
	    stps[1] = SO_GET_ANY_PART(this,"YRotator", SoDragger);
	    stps[2] = SO_GET_ANY_PART(this,"ZRotator", SoDragger);

	// CENTER CHANGERS
	    SoDragger *ccs[3];
	    ccs[0] = SO_GET_ANY_PART(this,"XCenterChanger", SoDragger);
	    ccs[1] = SO_GET_ANY_PART(this,"YCenterChanger", SoDragger);
	    ccs[2] = SO_GET_ANY_PART(this,"ZCenterChanger", SoDragger);

    // AXES
	setPartAsDefault("XAxis",         "centerballXAxis");
	setPartAsDefault("YAxis",         "centerballYAxis");
	setPartAsDefault("ZAxis",         "centerballZAxis");

    // Turn off the axes completely. They're just used as feedback
	setSwitchValue( XAxisSwitch.getValue(), SO_SWITCH_NONE );
	setSwitchValue( YAxisSwitch.getValue(), SO_SWITCH_NONE );
	setSwitchValue( ZAxisSwitch.getValue(), SO_SWITCH_NONE );

    // Rotation nodes that orient the parts...
        SoRotation *rNodeX90 = new SoRotation;
        SoRotation *rNodeY90 = new SoRotation;
        rNodeX90->rotation.setValue( SbVec3f(1,0,0), 1.57079 );
        rNodeY90->rotation.setValue( SbVec3f(0,1,0), 1.57079 );

	setAnyPartAsDefault( "rotX90",  rNodeX90 );
	setAnyPartAsDefault( "rotY90",  rNodeY90 );
	setAnyPartAsDefault( "rot2X90", rNodeX90 );

    // Update the rotation field when the motionMatrix is set.
    addValueChangedCallback(   &SoCenterballDragger::valueChangedCB );

    // Updates the motionMatrix when the rotation field is set.
    rotFieldSensor= new SoFieldSensor(&SoCenterballDragger::fieldSensorCB,this);
    rotFieldSensor->setPriority(0);

    // Updates the motionMatrix when the center field is set.
    centerFieldSensor= new SoFieldSensor(
		&SoCenterballDragger::fieldSensorCB, this);
    centerFieldSensor->setPriority(0);

    setUpConnections( TRUE, TRUE );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: public

SoCenterballDragger::~SoCenterballDragger()
//
////////////////////////////////////////////////////////////////////////
{
    if (rotFieldSensor)
	delete rotFieldSensor;
    if (centerFieldSensor)
	delete centerFieldSensor;
}

//    detach/attach any sensors, callbacks, and/or field connections.
//    Called by:            start/end of SoBaseKit::readInstance
//    and on new copy by:   start/end of SoBaseKit::copy.
//    Classes that redefine must call setUpConnections(TRUE,TRUE) 
//    at end of constructor.
//    Returns the state of the node when this was called.
SbBool
SoCenterballDragger::setUpConnections( SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( onOff ) {

	// We connect AFTER base class.
	SoDragger::setUpConnections( onOff, FALSE );

	// FIRST, DEAL WITH CHILD DRAGGERS:
	// Set the default parts and add callback to our child draggers.
	// The parts will only be changed if they are still set as default.
	// So we won't stomp over a non-default, intentionally set part.
        SoSeparator *dummySep = new SoSeparator;
        dummySep->ref();

	// BALL
	SoDragger *ball;
	ball = (SoDragger *) getAnyPart( "rotator", FALSE );
	if (ball != NULL) {
	    ball->setPartAsDefault("rotator",      "centerballRotator");
	    ball->setPartAsDefault("rotatorActive","centerballRotatorActive");
	    ball->setPartAsDefault("feedback",      dummySep);
	    ball->setPartAsDefault("feedbackActive",dummySep);
	    registerChildDragger( ball );
	    ball->addStartCallback(&SoCenterballDragger::kidStartCB, this );
	    ball->addFinishCallback(&SoCenterballDragger::kidFinishCB, this );
	}

	// STRIPES
	SoDragger *stps[3];
	stps[0] = (SoDragger *) getAnyPart("XRotator", FALSE);
	stps[1] = (SoDragger *) getAnyPart("YRotator", FALSE);
	stps[2] = (SoDragger *) getAnyPart("ZRotator", FALSE);
        int i;
	for (i = 0; i < 3; i++) {
	    if ( stps[i] == NULL ) continue;
	    stps[i]->setPartAsDefault("rotator",      "centerballStripe" );
	    stps[i]->setPartAsDefault("rotatorActive","centerballStripeActive");
	    stps[i]->setPartAsDefault("feedback",      dummySep );
	    stps[i]->setPartAsDefault("feedbackActive",dummySep );
	    registerChildDragger( stps[i] );
	    stps[i]->addStartCallback(&SoCenterballDragger::kidStartCB,this);
	    stps[i]->addFinishCallback(&SoCenterballDragger::kidFinishCB,this);
	}
	// CENTER CHANGERS
	SoDragger *ccs[3];
	ccs[0] = (SoDragger *) getAnyPart("XCenterChanger", FALSE );
	ccs[1] = (SoDragger *) getAnyPart("YCenterChanger", FALSE );
	ccs[2] = (SoDragger *) getAnyPart("ZCenterChanger", FALSE );
	for (i = 0; i < 3; i++) {
	    if ( ccs[i] == NULL ) continue;
	    ccs[i]->setPartAsDefault("translator", 
			"centerballCenterChanger");
	    ccs[i]->setPartAsDefault("translatorActive",
			"centerballCenterChangerActive");
	    ccs[i]->setPartAsDefault("feedback",       dummySep );
	    ccs[i]->setPartAsDefault("feedbackActive", dummySep );
	    ccs[i]->setPartAsDefault("xAxisFeedback",
			"centerballCenterXAxisFeedback");
	    ccs[i]->setPartAsDefault("yAxisFeedback",
			"centerballCenterYAxisFeedback");

	    // When the centers translate, they should not affect our
	    // motion matrix. Instead, they affect our translateToCenter
	    // node.
	    registerChildDraggerMovingIndependently( ccs[i] );
	    ccs[i]->addStartCallback(&SoCenterballDragger::kidStartCB,this);
	    ccs[i]->addFinishCallback(&SoCenterballDragger::kidFinishCB,this);
	}
	dummySep->unref();

	// Call the sensor CBs to make things are up-to-date.
	fieldSensorCB( this, NULL );

	// Connect the field sensors
	if (rotFieldSensor->getAttachedField() != &rotation)
	    rotFieldSensor->attach( &rotation );
	if (centerFieldSensor->getAttachedField() != &center)
	    centerFieldSensor->attach( &center );
    }
    else {

	// We disconnect BEFORE base class.

	// Disconnect the field sensors.
	if (rotFieldSensor->getAttachedField())
	    rotFieldSensor->detach();
	if (centerFieldSensor->getAttachedField())
	    centerFieldSensor->detach();

	// REMOVE callbacks from our child draggers:
	// BALL
	SoDragger *ball;
	ball = (SoDragger *) getAnyPart( "rotator", FALSE );
	if (ball != NULL) {
	    unregisterChildDragger( ball );
	    ball->removeStartCallback(&SoCenterballDragger::kidStartCB, this );
	    ball->removeFinishCallback(&SoCenterballDragger::kidFinishCB, this );
	}

	// STRIPES
	SoDragger *stps[3];
	stps[0] = (SoDragger *) getAnyPart("XRotator", FALSE);
	stps[1] = (SoDragger *) getAnyPart("YRotator", FALSE);
	stps[2] = (SoDragger *) getAnyPart("ZRotator", FALSE);
        int i;
	for (i = 0; i < 3; i++) {
	    if ( stps[i] == NULL ) continue;
	    unregisterChildDragger( stps[i] );
	    stps[i]->removeStartCallback(&SoCenterballDragger::kidStartCB,this);
	    stps[i]->removeFinishCallback(&SoCenterballDragger::kidFinishCB,this);
	}
	// CENTER CHANGERS
	SoDragger *ccs[3];
	ccs[0] = (SoDragger *) getAnyPart("XCenterChanger", FALSE );
	ccs[1] = (SoDragger *) getAnyPart("YCenterChanger", FALSE );
	ccs[2] = (SoDragger *) getAnyPart("ZCenterChanger", FALSE );
	for (i = 0; i < 3; i++) {
	    if ( ccs[i] == NULL ) continue;
	    unregisterChildDraggerMovingIndependently( ccs[i] );
	    ccs[i]->removeStartCallback(&SoCenterballDragger::kidStartCB,this);
	    ccs[i]->removeFinishCallback(&SoCenterballDragger::kidFinishCB,this);
	}


	SoDragger::setUpConnections( onOff, FALSE );
    }

    return !(connectionsSetUp = onOff);
}

void 
SoCenterballDragger::valueChangedCB(void *, SoDragger *inDragger )
{
    SoCenterballDragger *dBall = (SoCenterballDragger *) inDragger;

    // If the value was changed due to a centerDragger, transfer the 
    // centerDragger motion into our 'translateToCenter' matrix.
	SoDragger *cd = dBall->getActiveChildDragger();
	if ( cd != NULL &&
	     ( cd == dBall->XCenterChanger.getValue() ||
	       cd == dBall->YCenterChanger.getValue() ||
	       cd == dBall->ZCenterChanger.getValue() ) ) {

	    // Temporarily disable callbacks, since within this method
	    // we'll be resetting the centerDragger's motion matrix
	    // to identity, which would re-trigger this callback.
	    SbBool saveEnabled = dBall->enableValueChangedCallbacks( FALSE );
	    dBall->transferCenterDraggerMotion( cd );
	    dBall->enableValueChangedCallbacks( saveEnabled );

	    // No reason to call valueChanged() here. This should always
	    // be the first callback in our valueChangedCallback list.
	    return;
	}

    // Otherwise, the value was changed by something other than a
    // centerDragger.  We treat it as an oridinary change affecting the
    // motionMatrix.
	SbMatrix motMat = dBall->getMotionMatrix();

	SbVec3f    trans, scale;
	SbRotation rot,   scaleOrient;
	const SbVec3f &center = dBall->center.getValue();
	motMat.getTransform(trans,rot,scale,scaleOrient,center);

	// Disconnect the field sensor
	dBall->rotFieldSensor->detach();

	if ( dBall->rotation.getValue() != rot )
	    dBall->rotation = rot;

	// Reconnect the field sensor
	dBall->rotFieldSensor->attach( &(dBall->rotation) );
}

void
SoCenterballDragger::fieldSensorCB(void *inDragger, SoSensor *inSensor )
{
    SoCenterballDragger *dragger = (SoCenterballDragger *) inDragger;

    SoField *trigF = NULL;
    if ( inSensor )
	trigF = ((SoDataSensor *)inSensor)->getTriggerField();

    // If inSensor is NULL or the trigger field is NULL, we will
    // do both the matrix and center stuff, since we don't know what changed.
    SbBool doMatrix = TRUE;
    SbBool doCenter   = TRUE;

    // But if this is invoked by a sensor with a trigger field, 
    // we can do different things if it was the center 
    // or the rotation fields that triggered.
	if ( trigF ) {

	    if ( trigF == &dragger->rotation )
		doCenter = FALSE;

	    if ( trigF == &dragger->center )
		doMatrix = FALSE;
	}

    if ( doCenter ) {

	// Center field is given in local space of the motionmatrix.
	// Copy this value as a translation into the translateToCenter matrix
	SbVec3f center = dragger->center.getValue();

	// Build a matrix to translate the origin of the ball
	// to the transformed center...
	SbMatrix newMat;
	newMat.setTranslate( center );

	// Set the translate to center matrix...
	SoMatrixTransform *mx = SO_GET_ANY_PART(dragger,"translateToCenter",
				    SoMatrixTransform );
	mx->matrix = newMat;

	// We need to specifically tell the dragger to invoke the
	// value changed callbacks, since only calling setMotionMatrix
	// automatically does this.
	if ( doMatrix == FALSE )
	    dragger->valueChanged();
    }

    if ( doMatrix ) {

	SbMatrix motMat = dragger->getMotionMatrix();
	dragger->workFieldsIntoTransform(motMat);

	dragger->setMotionMatrix(motMat);
    }
}

void 
SoCenterballDragger::kidStartCB(  void *parentAsVoid, SoDragger *childM )
{
    // ESTABLISHES HIGHLIGHTS and TELLS THE SURROUNDSCALE PART TO UPDATE

    SoCenterballDragger *cbD = (SoCenterballDragger *) parentAsVoid;

    // DEAL WITH SURROUNDSCALE
    SoSurroundScale *ss = SO_CHECK_PART(cbD, "surroundScale", SoSurroundScale );
    if (ss != NULL)
	ss->invalidate();

    cbD->setSwitches( childM );

}

void 
SoCenterballDragger::kidFinishCB( void *parentAsVoid, SoDragger * )
{
    SoCenterballDragger *cbD = (SoCenterballDragger *) parentAsVoid;

    // ESTABLISHES HIGHLIGHTS and TELLS THE SURROUNDSCALE PART TO UPDATE

    // DEAL WITH SURROUNDSCALE
    SoSurroundScale *ss = SO_CHECK_PART(cbD, "surroundScale", SoSurroundScale );
    if (ss != NULL)
	ss->invalidate();

    cbD->setSwitches( NULL );
}

void 
SoCenterballDragger::setSwitches( SoDragger *activeChild )
{
    // Figure out what we are doing...
    SbBool freeRotate = ( activeChild && activeChild == rotator.getValue());
    SbBool changeCenter = (     activeChild && 
			    (   activeChild == XCenterChanger.getValue() 
			     || activeChild == YCenterChanger.getValue() 
			     || activeChild == ZCenterChanger.getValue()));
    SbBool xRotate = (activeChild && activeChild == XRotator.getValue());
    SbBool yRotate = (activeChild && activeChild == YRotator.getValue());
    SbBool zRotate = (activeChild && activeChild == ZRotator.getValue());

    // ROTATOR - only highlight during free rotation
	if ( freeRotate )
	    setSwitchValue( getAnyPart("rotator.rotatorSwitch",FALSE ), 1);
	else
	    setSwitchValue( getAnyPart("rotator.rotatorSwitch",FALSE ), 0);

    // TRANSLATORS - ALL hilight during free rotation or centerChanging
	if ( freeRotate || changeCenter ) {
	  setSwitchValue(getAnyPart("XCenterChanger.translatorSwitch",FALSE),1);
	  setSwitchValue(getAnyPart("YCenterChanger.translatorSwitch",FALSE),1);
	  setSwitchValue(getAnyPart("ZCenterChanger.translatorSwitch",FALSE),1);
	}
	else {
	  setSwitchValue(getAnyPart("XCenterChanger.translatorSwitch",FALSE),0);
	  setSwitchValue(getAnyPart("YCenterChanger.translatorSwitch",FALSE),0);
	  setSwitchValue(getAnyPart("ZCenterChanger.translatorSwitch",FALSE),0);
	}

    // STRIPES - ALL hilight during free rotation 
    //           Each hilights during its own rotation
	if ( freeRotate || xRotate )
	    setSwitchValue( getAnyPart("XRotator.rotatorSwitch",FALSE), 1);
	else 
	    setSwitchValue( getAnyPart("XRotator.rotatorSwitch",FALSE), 0);

	if ( freeRotate || yRotate )
	    setSwitchValue( getAnyPart("YRotator.rotatorSwitch",FALSE), 1);
	else 
	    setSwitchValue( getAnyPart("YRotator.rotatorSwitch",FALSE), 0);

	if ( freeRotate || zRotate )
	    setSwitchValue( getAnyPart("ZRotator.rotatorSwitch",FALSE), 1);
	else 
	    setSwitchValue( getAnyPart("ZRotator.rotatorSwitch",FALSE), 0);

    // AXES - ALL displayed during rotation or center changing.
    //        Each displayed during its rotation.
	if ( freeRotate || changeCenter || xRotate )
	    setSwitchValue( XAxisSwitch.getValue(), 0 );
	else
	    setSwitchValue( XAxisSwitch.getValue(), SO_SWITCH_NONE );

	if ( freeRotate || changeCenter || yRotate )
	    setSwitchValue( YAxisSwitch.getValue(), 0 );
	else
	    setSwitchValue( YAxisSwitch.getValue(), SO_SWITCH_NONE );

	if ( freeRotate || changeCenter || zRotate )
	    setSwitchValue( ZAxisSwitch.getValue(), 0 );
	else
	    setSwitchValue( ZAxisSwitch.getValue(), SO_SWITCH_NONE );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Saves the matrix that moves the center relative to the rest of the
//    parts after it calls the regular SoDragger method.
//
void
SoCenterballDragger::saveStartParameters()
//
////////////////////////////////////////////////////////////////////////
{
    SoDragger::saveStartParameters();

    SoMatrixTransform *thePart 
        = SO_GET_ANY_PART(this,"translateToCenter",SoMatrixTransform);

    this->startCenterMatrix = thePart->matrix.getValue();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//     This function is invoked by the valueChangedCB if the
//     reason was because a centerDragger was moved.
//
void 
SoCenterballDragger::transferCenterDraggerMotion( SoDragger *childDragger )
//
////////////////////////////////////////////////////////////////////////
{
    // Get the motion matrix from the child
	SbMatrix childMotion = childDragger->getMotionMatrix();

    // There's a lot we don't need to bother with if the childMotion is
    // identity...
	SbBool childIdent = ( childMotion == SbMatrix::identity() );

    // Return if childMotion is identity and our center matrix has not changed.
	SoMatrixTransform *centerTranslatePart 
	 = SO_GET_ANY_PART(this,"translateToCenter",SoMatrixTransform);
	SbMatrix curCenterMatrix = centerTranslatePart->matrix.getValue();
	if ( childIdent && (curCenterMatrix == startCenterMatrix) )
	    return;

	if (!childIdent) {
	    // First, set the childDragger matrix to identity.
	    childDragger->setMotionMatrix( SbMatrix::identity() );

	    // Convert the childMotion from child LOCAL space to world space
	    childDragger->transformMatrixLocalToWorld(childMotion, childMotion);

	    // Convert the childMotion from world space to our LOCAL space.
	    transformMatrixWorldToLocal(childMotion,childMotion);
	}

    // Multiply the local space motion by the parent's 
    // 'startCenterMatrix'. We start with childMotion in space before parent,
    // so we use a multRight, not multLeft.
	SbMatrix newCenterMatrix = startCenterMatrix;
	if ( !childIdent )
	    newCenterMatrix.multRight( childMotion );
	centerTranslatePart->matrix = newCenterMatrix;

    // Now determine and set the field value...
	// Disconnect the field sensors
	    centerFieldSensor->detach();

	// Figure out the center
	    SbVec3f newCenter(curCenterMatrix[3][0],
			      curCenterMatrix[3][1],
			      curCenterMatrix[3][2]);

	// Set the field if we need to...
	    if ( center.getValue() != newCenter )
		center = newCenter;

	// Reconnect the field sensors
	    centerFieldSensor->attach( &(center) );

    // Force child to update its local matrix. (it changes when parent moves.)
	SoPath *myPath = childDragger->createPathToThis();
	// Important to ref and unref, or the path will not get deleted.
	if (myPath) myPath->ref();
	childDragger->setTempPathToThis(myPath);
	if (myPath) myPath->unref();
}

void
SoCenterballDragger::setDefaultOnNonWritingFields()
{
    // These nodes may change after construction, but we still
    // don't want to write them out.
    surroundScale.setDefault(TRUE);
    antiSquish.setDefault(TRUE);
    translateToCenter.setDefault(TRUE);

    // Try not to write out the sub-draggers.
	rotator.setDefault(TRUE);

	XRotator.setDefault(TRUE);
	YRotator.setDefault(TRUE);
	ZRotator.setDefault(TRUE);

	XCenterChanger.setDefault(TRUE);
	YCenterChanger.setDefault(TRUE);
	ZCenterChanger.setDefault(TRUE);

    // Call the base class...
    SoDragger::setDefaultOnNonWritingFields();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Differs from SoDragger as follows: If it has a surroundScale node with 
//   'ignoreInBbox' set to TRUE, this means surroundScale is calculating its 
//   bbox for sizing. We need to tell the surroundScale not to 
//   do translations.
//
// Use: protected
//
void
SoCenterballDragger::getBoundingBox(SoGetBoundingBoxAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool oldFlag = TRUE;
    SoSurroundScale *ss = SO_CHECK_PART(this, "surroundScale", SoSurroundScale);
    if (ss) {
	oldFlag = ss->isDoingTranslations();
	ss->setDoingTranslations(FALSE);
    }

    // Call base class method
    SoDragger::getBoundingBox( action );

    if (ss)
	ss->setDoingTranslations(oldFlag);
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//   Differs from SoDragger as follows: If it has a surroundScale node with 
//   'ignoreInBbox' set to TRUE, this means surroundScale is calculating its 
//   bbox for sizing. We need to tell the surroundScale not to 
//   do translations.
//
// Use: protected
//
void
SoCenterballDragger::getMatrix(SoGetMatrixAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool oldFlag = TRUE;
    SoSurroundScale *ss = SO_CHECK_PART(this, "surroundScale", SoSurroundScale);
    if (ss) {
	oldFlag = ss->isDoingTranslations();
	ss->setDoingTranslations(FALSE);
    }

    // Call base class method
    SoDragger::getMatrix( action );

    if (ss)
	ss->setDoingTranslations(oldFlag);
}
