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

////////////////////////////////////////////////////////////////////////
//
// Description:
//  This is the source file for the SoSpotLightDragger.
//  This is a composite dragger which allows independent rotation,
//  translation, and beam spread editting of a spot light.
//
//  It is composed of an SoRotateSphericalDragger (for rotation),
//  an SoDragPointDragger (for translation), and it creates its own projector
//  handles mouse events for doing it's own dragging of the beam angle.
//
//  The beam is editted by behaving like an SoRotateDiscDragger, but the
//  plane of the disc is re-defined every time a drag is initiated.
//  The plane always passes through the z axis and the selected point.
//  When the rotation angle is determined, however, the beam is not rotated,
//  but scaled so it looks like an opening or closing umbrella. This is done
//  by scaling evenly in x and y, and a different amount in z, so the distance
//  between the selected point and the origin remains constant.
//
////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <math.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoPath.h>
#include <Inventor/projectors/SbPlaneProjector.h>
#include <Inventor/projectors/SbSphereSectionProjector.h>
#include <Inventor/sensors/SoFieldSensor.h>

#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoSpotLight.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/draggers/SoDragPointDragger.h>
#include <Inventor/draggers/SoRotateSphericalDragger.h>
#include <Inventor/draggers/SoSpotLightDragger.h>

#include "geom/SoSpotLightDraggerGeom.h"


SO_KIT_SOURCE(SoSpotLightDragger);

////////////////////////////////////////////////////////////////////////
//
// Description:
//  Constructors. Each constructor calls constructorSub(), which
//  does work that is common to each.
//
// Use: public
//
////////////////////////////////////////////////////////////////////////

// Default constructor.
SoSpotLightDragger::SoSpotLightDragger()
{
    SO_KIT_CONSTRUCTOR(SoSpotLightDragger);

    isBuiltIn = TRUE;

    // This gives the dragger an overall material.  It is edited by lightManips
    // to make its dragger match the color of the light.  Any materials within 
    // other parts will override this one. 
    SO_KIT_ADD_CATALOG_ENTRY(material, SoMaterial, 
				TRUE, topSeparator, geomSeparator,TRUE);

    // The translator is kept under a separator along with a
    // rotation that is maintained as the inverse to the rotation of the
    // light. This means that using the rotator does not rotate the
    // coordinate system that we translate the base of the dragger in.
    SO_KIT_ADD_CATALOG_ENTRY(translatorSep, SoSeparator, 
				TRUE, topSeparator, geomSeparator,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(translatorRotInv, SoRotation, 
				TRUE, translatorSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(translator, SoDragPointDragger, 
				TRUE, translatorSep, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(rotator, SoRotateSphericalDragger, 
				TRUE, topSeparator, geomSeparator,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(beamSep, SoSeparator, 
				TRUE, topSeparator, geomSeparator,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(beamPlacement, SoTranslation, 
				TRUE, beamSep, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(beamScale, SoScale, 
				TRUE, beamSep, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(beamSwitch, SoSwitch, 
				TRUE, beamSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(beam, SoSeparator, 
				TRUE, beamSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(beamActive, SoSeparator, 
				TRUE, beamSwitch, ,TRUE);

    // Read the default geometry for this dragger
    if (SO_KIT_IS_FIRST_INSTANCE())
	readDefaultParts( "spotLightDragger.iv", geomBuffer, sizeof(geomBuffer) );
    SO_KIT_ADD_FIELD(rotation, (0.0, 0.0, 0.0, 1.0));
    SO_KIT_ADD_FIELD(translation, (0.0, 0.0, 0.0));
    SO_KIT_ADD_FIELD(angle, (1.0));

    SO_KIT_INIT_INSTANCE();

    // Set the overall material.
    // We need to use a copy of the resource, since the resource is shared
    // by all manips but we are going to edit ours.
    SoNode *resourceMtl 
	= SoNode::getByName("spotLightOverallMaterial");
    setPartAsDefault("material", resourceMtl->copy() );


    // Create the simple draggers that comprise this dragger.
    // This dragger employs 2 simple draggers:
    // 1 DragpointDragger
    // 1 RotateSphericalDragger

	SoDragPointDragger *trD;
	trD = SO_GET_ANY_PART( this,"translator", SoDragPointDragger );

	SoRotateSphericalDragger *roD;
	roD = SO_GET_ANY_PART( this, "rotator", SoRotateSphericalDragger );

    // Create everything needed for the beam spreader.
	// Create the parts.
	setPartAsDefault("beam",          "spotLightBeam");
	setPartAsDefault("beamActive",    "spotLightBeamActive");
	setPartAsDefault("beamPlacement", "spotLightBeamPlacement");

	// Set the switch to 0...
	SoNode *bs = getAnyPart("beamSwitch", TRUE );
	setSwitchValue(bs, 0); 

	// Create a plane projector for use in dragging.
	planeProj = new SbPlaneProjector();

	addStartCallback(  &SoSpotLightDragger::startCB );
	addMotionCallback( &SoSpotLightDragger::motionCB );
	addFinishCallback( &SoSpotLightDragger::doneCB );

    // Update the rotation and scale fields when the motionMatrix is set.
    addValueChangedCallback( &SoSpotLightDragger::valueChangedCB );

    // Updates the motionMatrix when the scaleFactor field is set.
    rotFieldSensor 
	= new SoFieldSensor(&SoSpotLightDragger::fieldSensorCB, this);
    rotFieldSensor->setPriority( 0 );

    // Updates the motionMatrix when the translationFactor field is set.
    translFieldSensor 
        = new SoFieldSensor( &SoSpotLightDragger::fieldSensorCB, this);
    translFieldSensor->setPriority( 0 );

    // Updates the motionMatrix when the scaleFactor field is set.
    angleFieldSensor 
	= new SoFieldSensor( &SoSpotLightDragger::fieldSensorCB, this);
    angleFieldSensor->setPriority( 0 );

    setUpConnections( TRUE, TRUE );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
SoSpotLightDragger::~SoSpotLightDragger()
//
////////////////////////////////////////////////////////////////////////
{
    delete planeProj;
    if (rotFieldSensor )
	delete rotFieldSensor; 
    if (translFieldSensor )
	delete translFieldSensor; 
    if (angleFieldSensor )
	delete angleFieldSensor; 
}

//    detach/attach any sensors, callbacks, and/or field connections.
//    Called by:            start/end of SoBaseKit::readInstance
//    and on new copy by:   start/end of SoBaseKit::copy.
//    Classes that redefine must call setUpConnections(TRUE,TRUE) 
//    at end of constructor.
//    Returns the state of the node when this was called.
SbBool
SoSpotLightDragger::setUpConnections( SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( onOff ) {

	// We connect AFTER base class.
	SoDragger::setUpConnections( onOff, FALSE );

	// Set default parts and add callbacks to child draggers.
	SoDragger *trD = (SoDragger *) getAnyPart( "translator", FALSE );
	if (trD != NULL) {

	    // Set up the parts in the child dragger
	    SoNode *n;
	    n = SoNode::getByName("spotLightTranslatorLineTranslator");
	    trD->setPartAsDefault("xTranslator.translator", n );
	    trD->setPartAsDefault("yTranslator.translator", n );
	    trD->setPartAsDefault("zTranslator.translator", n );

	    n = SoNode::getByName(
			    "spotLightTranslatorLineTranslatorActive");
	    trD->setPartAsDefault("xTranslator.translatorActive", n );
	    trD->setPartAsDefault("yTranslator.translatorActive", n );
	    trD->setPartAsDefault("zTranslator.translatorActive", n );

	    n = SoNode::getByName("spotLightTranslatorPlaneTranslator");
	    trD->setPartAsDefault("yzTranslator.translator", n );
	    trD->setPartAsDefault("xzTranslator.translator", n );
	    trD->setPartAsDefault("xyTranslator.translator", n );

	    n = SoNode::getByName(
			    "spotLightTranslatorPlaneTranslatorActive");
	    trD->setPartAsDefault("yzTranslator.translatorActive", n );
	    trD->setPartAsDefault("xzTranslator.translatorActive", n );
	    trD->setPartAsDefault("xyTranslator.translatorActive", n );

	    registerChildDragger( trD );
	}

	SoRotateSphericalDragger *roD 
	    = (SoRotateSphericalDragger *) getAnyPart( "rotator", FALSE );
	if (roD != NULL) {

	    // Give it a projector that moves freely in the radial direction.
	    // We need this because our geometry is just a little stick, not a 
	    // big ol' ball
	    SbSphereSectionProjector *ssp = new SbSphereSectionProjector();
	    ssp->setRadialFactor( 1.0 );
	    roD->setProjector(ssp);

	    // Set up the parts in the child dragger
	    roD->setPartAsDefault("rotator", 
			    "spotLightRotatorRotator");
	    roD->setPartAsDefault("rotatorActive", 
			    "spotLightRotatorRotatorActive");
	    roD->setPartAsDefault("feedback", 
			    "spotLightRotatorFeedback" );
	    roD->setPartAsDefault("feedbackActive", 
			  "spotLightRotatorFeedbackActive");

	    registerChildDragger( roD );

	}

	// Call the sensor CBs to make things are up-to-date.
	fieldSensorCB( this, NULL );

	// Connect the field sensors
	if (rotFieldSensor->getAttachedField() != &rotation)
	    rotFieldSensor->attach( &rotation );
	if (translFieldSensor->getAttachedField() != &translation)
	    translFieldSensor->attach( &translation );
	if (angleFieldSensor->getAttachedField() != &angle)
	    angleFieldSensor->attach( &angle );
    }
    else {

	// We disconnect BEFORE base class.

	// remove callbacks from the child draggers
        SoDragger *trD = (SoDragger *) getAnyPart( "translator", FALSE );
	if (trD)
        unregisterChildDragger( trD );

        SoDragger *roD = (SoDragger *) getAnyPart( "rotator", FALSE );
	if (roD)
        unregisterChildDragger( roD );

	// Disconnect the field sensors.
	if (rotFieldSensor->getAttachedField())
	    rotFieldSensor->detach();
	if (translFieldSensor->getAttachedField())
	    translFieldSensor->detach();
	if (angleFieldSensor->getAttachedField())
	    angleFieldSensor->detach();

	SoDragger::setUpConnections( onOff, FALSE );
    }

    return !(connectionsSetUp = onOff);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Calculates the fields of the beamScale, based on
//    a beam spread angle.
//
//    At the angle of 45 degrees, the scale is (1,1,1)
//    A greater beam angle increases the scale in x and y equally,
//      while decreasing the angle in z.
//    A smaller beam angle decreases the scale in x and y equally,
//      while increasing the angle in z.
//
//    The equation for the relationship is:
//      xScale = yScale = sin( beamAngle );
//               zScale = cos( beamAngle );
//
//    Angles are clamped to lie between 0 and 90 degrees.
//
// Use: protected
//
void
SoSpotLightDragger::setBeamScaleFromAngle( float beamAngle )
//
////////////////////////////////////////////////////////////////////////
{
    float myNewAngle = beamAngle;
    // This dragger clamps angle values to lie between 0 and PI/2 radians.
    // But we will be more restrictive when we display, so that users
    // can always (hopefully) hit the beam geometry.
#define TWO_AND_A_HALF_DEGREES 0.043633
    if ( beamAngle <  TWO_AND_A_HALF_DEGREES )
	myNewAngle =  TWO_AND_A_HALF_DEGREES;
#undef TWO_AND_A_HALF_DEGREES
    if ( beamAngle > (M_PI / 2.0) )
	myNewAngle = (M_PI / 2.0);

    float myCos = cosf(myNewAngle);
    float mySin = sinf(myNewAngle);

    if (myCos < getMinScale())
	myCos = getMinScale();

    if (mySin < getMinScale())
	mySin = getMinScale();

    SoScale *bs = (SoScale *) getAnyPart("beamScale",TRUE);
    if (bs) {
	SbVec3f newSF(mySin, mySin, myCos);
	if ( bs->scaleFactor.getValue() != newSF )
	    bs->scaleFactor.setValue( newSF );
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Prepare for dragging the beam spreading piece.
//
// Use: private
//
void
SoSpotLightDragger::dragStart()
//
////////////////////////////////////////////////////////////////////////
{
    // We only perform dragging if we received the mouse-down.
    // If there is an active child dragger, then we should just return.
    if ( getActiveChildDragger() != NULL)
	return;

    // Set the switch to 1...
    setSwitchValue( beamSwitch.getValue(), 1 );

    // Establish the working space.
    // Working space is space at end of "beamPlacement", (but before
    // "beamScale").
	SbMatrix workSpaceToLocal, localToWorkSpace;
	getPartToLocalMatrix("beamPlacement",workSpaceToLocal,localToWorkSpace);
	SbMatrix worldSpaceToWork = getWorldToLocalMatrix();
	worldSpaceToWork.multRight( localToWorkSpace );

    // Establish the projector plane in working space.
    // The plane is defined by two points on the z axis and the selected point.
    // If they are colinear or nearly so, then make the plane face the viewer.
	SbVec3f p0(0,0,0), p1(0,0,1);

	SbVec3f startWorkSpaceHitPt;
	worldSpaceToWork.multVecMatrix(getWorldStartingPoint(), 
				       startWorkSpaceHitPt);
	SbVec3f p2 = startWorkSpaceHitPt;
	p2.normalize();
#define MAX_WORKABLE_ANGLE_COS .98
	if (((p2 - p0).dot(p1 - p0)) > MAX_WORKABLE_ANGLE_COS)  {
	    // Find a new p2.
	    SbVec3f worldProjDir = getViewVolume().getProjectionDirection();
	    SbVec3f lclProjDir;
	    worldSpaceToWork.multDirMatrix(worldProjDir,lclProjDir);
	    lclProjDir.normalize();
	    lclProjDir *= -1.0;
	    SbVec3f otherVecInPlane = lclProjDir.cross( p1 - p0 );
	    p2 = (otherVecInPlane - p0);
	}
#undef MAX_WORKABLE_ANGLE_COS
	    
	SbPlane wsPlane(p0, p1, p2);

	planeProj->setPlane( wsPlane );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Set the beam angle based on the new hit to the planeProj
//
// Use: private
//
void
SoSpotLightDragger::drag()
//
////////////////////////////////////////////////////////////////////////
{
    // We only perform dragging if we received the mouse-down.
    // If there is an active child dragger, then we should just return.
    if ( getActiveChildDragger() != NULL)
	return;

    // Establish the working space.
    // Working space is space at end of "beamPlacement", (but before
    // "beamScale").
	SbMatrix workSpaceToLocal, localToWorkSpace;
	getPartToLocalMatrix("beamPlacement",workSpaceToLocal,localToWorkSpace);
	SbMatrix workSpaceToWorld = getLocalToWorldMatrix();
	workSpaceToWorld.multLeft( workSpaceToLocal );
	SbMatrix worldSpaceToWork = getWorldToLocalMatrix();
	worldSpaceToWork.multRight( localToWorkSpace );

    // Set the projector space and viewVolume.
	planeProj->setViewVolume( getViewVolume() );    
	planeProj->setWorkingSpace( workSpaceToWorld );

    // Get newHitPt and startHitPt in workspace. 
	SbVec3f newHitPt 
	    = planeProj->project( getNormalizedLocaterPosition()); 
	SbVec3f startHitPt;
	worldSpaceToWork.multVecMatrix( getWorldStartingPoint(), startHitPt );

    // Based on the position, determine the new beamAngle.
    // Theta will be the angle between the minus z axis and the 
    // vector from (0,0,0) to newHitPt.

	// Normalize these and they become directions from (0,0,0) to hit points
	    newHitPt.normalize();
	    startHitPt.normalize();
	// We also need this direction:
	    SbVec3f minusZ(0,0,-1);

	// If we've gone 'out of bounds' let's figure out what to do...
	    // If newHitPt was in positive z, the angle will be > (PI/2)
	    SbBool isPosZ = ( newHitPt[2] > 0.0 );

	    // If newHitPt and startHitPt are on opposite sides of the
	    // z axis within the projector plane, we've completely closed
	    // the 'umbrella' and gone to the other side, so we'll want to
	    // clamp the value at 0.0
	    SbVec3f newCross   =   newHitPt.cross(minusZ);
	    SbVec3f startCross = startHitPt.cross(minusZ);
	    
	    SbBool isNewOppositeStart = ( newCross.dot(startCross) < 0.0 );

	// Find theta
	// Do different things depending on if we've gone out of bounds.
	    float curAngle = angle.getValue();
	    float theta;

	    // If both conditions are violated, use the value closer to 
	    // the current angle.
	    if ( isPosZ && isNewOppositeStart ) {
		if ( fabs(curAngle - 0.0) < fabs((M_PI/2.0) - curAngle) )
		    theta = 0.0;
		else 
		    theta = M_PI / 2.0;
	    }
	    else if ( isPosZ ) {
		theta = M_PI / 2.0;
	    }
	    else if ( isNewOppositeStart ) {
		theta = 0.0;
	    }
	    else {
		// we should be able to go unconstrained.
		// Find angle between minusZ vector and newHitPt

		// Dot product of unit vectors is cosine of angle between them.
		float thetaCos = minusZ.dot( newHitPt ); 
		theta = acosf( thetaCos );

		// clamp theta to lie between 0 and PI/2
		theta = (theta < 0.0)          ? 0.0          : theta;
		theta = (theta > (M_PI / 2.0)) ? (M_PI / 2.0) : theta;
	    }

    // Now we've got a new angle.  Set the beamScale appropriately,
    // set the angle field, and call the special angle callbacks.

        setBeamScaleFromAngle(theta);

	angleFieldSensor->detach();         // Disconnect sensor
	if ( curAngle != theta )        // Set field
	    angle = theta;
	angleFieldSensor->attach( &angle ); // Reconnect  sensor

	// Invoke the value changed callbacks.
	// We must specifically do this since only calling setMotionMatrix()
	// does it automatically.
	valueChanged();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Rotate the spoLightDragger based on mouse motion.
//
// Use: private
//
void
SoSpotLightDragger::dragFinish()
//
////////////////////////////////////////////////////////////////////////
{
    // We only perform dragging if we received the mouse-down.
    // If there is an active child dragger, then we should just return.
    if ( getActiveChildDragger() != NULL)
	return;

    // Set the switches to 0...
    setSwitchValue( beamSwitch.getValue(), 0 );
}

////////////////////////////////////////////////////////////////////
//  Stubs for callbacks
////////////////////////////////////////////////////////////////////
void
SoSpotLightDragger::startCB( void *, SoDragger *inDragger )
{
    SoSpotLightDragger *dl = (SoSpotLightDragger *) inDragger;
    dl->dragStart();
}

void
SoSpotLightDragger::motionCB( void *, SoDragger *inDragger )
{
    SoSpotLightDragger *dl = (SoSpotLightDragger *) inDragger;
    dl->drag();
}

void
SoSpotLightDragger::doneCB( void *, SoDragger *inDragger )
{
    SoSpotLightDragger *dl = (SoSpotLightDragger *) inDragger;
    dl->dragFinish();
}


void
SoSpotLightDragger::valueChangedCB( void *, SoDragger *inDragger )
{
    SoSpotLightDragger *m = (SoSpotLightDragger *) inDragger;
    SbMatrix motMat = m->getMotionMatrix();

    SbVec3f    trans, scale;
    SbRotation rot, scaleOrient;
    motMat.getTransform( trans, rot, scale, scaleOrient );

    // Disconnect the field sensors
    m->rotFieldSensor->detach();
    m->translFieldSensor->detach();
    m->angleFieldSensor->detach();

    if ( m->rotation.getValue() != rot )
	m->rotation = rot;
    if ( m->translation.getValue() != trans )
	m->translation = trans;

    // Make the rotation inside the "translaterRotInv" be the inverse 
    // of the new rotation.
    SbRotation newInv = rot.inverse();
    SoRotation *rotInv = (SoRotation *) m->getAnyPart("translatorRotInv",TRUE);
    if (rotInv->rotation.getValue() != newInv)
	rotInv->rotation = newInv;

    // Note: we do not need to do anything special for 'angle.'
    //       When the dragging routine changes the angle, it sets both
    //       the angle field and the internal tranform for us.

    // Reconnect the field sensors
    m->rotFieldSensor->attach( &(m->rotation) );
    m->translFieldSensor->attach( &(m->translation) );
    m->angleFieldSensor->attach( &(m->angle) );
}

void
SoSpotLightDragger::fieldSensorCB(void *inDragger, SoSensor *inSensor )
{
   SoSpotLightDragger *dragger = (SoSpotLightDragger *) inDragger;

    SoField *trigF = NULL;
    if ( inSensor )
	trigF = ((SoDataSensor *)inSensor)->getTriggerField();

    // If inSensor is NULL or the trigger field is NULL, we
    // will do both the matrix and beam stuff, since we don't know what changed.
    SbBool doMatrix = TRUE;
    SbBool doBeam   = TRUE;

    // But if this is invoked by a sensor with a trigger field, 
    // we can do different things if it was the angle 
    // or the rotation/translation fields that triggered.
	if ( trigF ) {

	    if ( trigF == &dragger->rotation || trigF == &dragger->translation )
		doBeam = FALSE;

	    if ( trigF == &dragger->angle )
		doMatrix = FALSE;
	}

    // Save the cut off angle, since it can get screwed up when the
    // matrix on the dragger is set:
	float myCutOff = dragger->angle.getValue();

    if ( doMatrix ) {

	SbMatrix motMat = dragger->getMotionMatrix();
	dragger->workFieldsIntoTransform(motMat);

	dragger->setMotionMatrix(motMat);
    }

    // Restore the cut off angle:
    if (dragger->angle.getValue() != myCutOff)
	dragger->angle = myCutOff;

    if ( doBeam ) {

	// This line is special work done for spotlights that is 
	// different from most other draggers.
	dragger->setBeamScaleFromAngle( dragger->angle.getValue() );

	// We need to specifically tell the dragger to invoke the
	// value changed callbacks, since only calling setMotionMatrix
	// automatically does this.
	if ( doMatrix == FALSE )
	    dragger->valueChanged();
    }
}

void
SoSpotLightDragger::setDefaultOnNonWritingFields()
{
    // We don't write out angle if it has default value.
    if ( ! (angle.isConnected() && angle.isConnectionEnabled())
         && angle.getValue() == 1.0 )
		      angle.setDefault(TRUE);

    // These nodes may change after construction, but we still
    // don't want to write them out.
    translatorRotInv.setDefault(TRUE);
    beamScale.setDefault(TRUE);

    // Try not to write out the sub-draggers.
	translator.setDefault(TRUE);
	rotator.setDefault(TRUE);

    // Call the base class...
    SoDragger::setDefaultOnNonWritingFields();
}
