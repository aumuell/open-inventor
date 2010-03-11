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
 |	SoJackDragger
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

#include <Inventor/nodes/SoSurroundScale.h>
#include <Inventor/nodes/SoAntiSquish.h>
#include <Inventor/nodes/SoTransform.h>

#include <Inventor/draggers/SoDragPointDragger.h>
#include <Inventor/draggers/SoJackDragger.h>
#include <Inventor/draggers/SoRotateSphericalDragger.h>
#include <Inventor/draggers/SoScaleUniformDragger.h>

#include "geom/SoJackDraggerGeom.h"


SO_KIT_SOURCE(SoJackDragger);


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
SoJackDragger::SoJackDragger()
{
    SO_KIT_CONSTRUCTOR(SoJackDragger);

    isBuiltIn = TRUE;

    // Don't create this by default. It's only really put into use
    // if this dragger is put inside a manipulator.
    SO_KIT_ADD_CATALOG_ENTRY(surroundScale, SoSurroundScale, 
				TRUE, topSeparator, geomSeparator,TRUE);
    // Create an anti-squish node by default.
    SO_KIT_ADD_CATALOG_ENTRY(antiSquish, SoAntiSquish, 
				FALSE, topSeparator, geomSeparator,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(scaler, SoScaleUniformDragger, 
				TRUE, topSeparator, geomSeparator,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(rotator, SoRotateSphericalDragger, 
				TRUE, topSeparator, geomSeparator,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(translator, SoDragPointDragger, 
				TRUE, topSeparator, geomSeparator,TRUE);

    // Read the default geometry for this dragger
    if (SO_KIT_IS_FIRST_INSTANCE())
	readDefaultParts( "jackDragger.iv", geomBuffer, sizeof(geomBuffer) );

    SO_KIT_ADD_FIELD(rotation, (0.0, 0.0, 0.0, 1.0));
    SO_KIT_ADD_FIELD(translation, (0.0, 0.0, 0.0));
    SO_KIT_ADD_FIELD(scaleFactor, (1.0, 1.0, 1.0));

    SO_KIT_INIT_INSTANCE();

    // Make the anti-squish node surround the biggest dimension
    SoAntiSquish *as = SO_GET_ANY_PART( this,"antiSquish", SoAntiSquish );
    as->sizing = SoAntiSquish::BIGGEST_DIMENSION;

    // CREATE THE CHILD DRAGGERS:
    SoDragger *sD = SO_GET_ANY_PART( this,"scaler",  SoScaleUniformDragger );
    SoDragger *rD = SO_GET_ANY_PART( this,"rotator", SoRotateSphericalDragger );
    SoDragger *tD = SO_GET_ANY_PART( this,"translator", SoDragPointDragger );

    // Update the rotation and scale fields when the motionMatrix is set.
    addValueChangedCallback( &SoJackDragger::valueChangedCB );

    // Updates the motionMatrix when the scaleFactor field is set.
    rotFieldSensor = new SoFieldSensor(&SoJackDragger::fieldSensorCB, this);
    rotFieldSensor->setPriority( 0 );

    // Updates the motionMatrix when the translationFactor field is set.
    translFieldSensor 
        = new SoFieldSensor( &SoJackDragger::fieldSensorCB, this);
    translFieldSensor->setPriority( 0 );

    // Updates the motionMatrix when the scaleFactor field is set.
    scaleFieldSensor 
	= new SoFieldSensor( &SoJackDragger::fieldSensorCB, this);
    scaleFieldSensor->setPriority( 0 );

    setUpConnections( TRUE, TRUE );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
SoJackDragger::~SoJackDragger()
//
////////////////////////////////////////////////////////////////////////
{
    if (rotFieldSensor)
	delete rotFieldSensor;
    if (translFieldSensor )
	delete translFieldSensor;
    if (scaleFieldSensor )
	delete scaleFieldSensor;
}

//    detach/attach any sensors, callbacks, and/or field connections.
//    Called by:            start/end of SoBaseKit::readInstance
//    and on new copy by:   start/end of SoBaseKit::copy.
//    Classes that redefine must call setUpConnections(TRUE,TRUE) 
//    at end of constructor.
//    Returns the state of the node when this was called.
SbBool
SoJackDragger::setUpConnections( SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( onOff ) {

	// We connect AFTER base class.
	SoDragger::setUpConnections( onOff, FALSE );

	// Set up the parts and callbacks in the child dragger
	// Create a scaling dragger.
	SoDragger *sD = (SoDragger *) getAnyPart( "scaler", FALSE);
	if (sD) {
	    sD->setPartAsDefault("scaler", "jackScalerScaler" );
	    sD->setPartAsDefault("scalerActive", "jackScalerScalerActive");
	    sD->setPartAsDefault("feedback", "jackScalerFeedback" );
	    sD->setPartAsDefault("feedbackActive", "jackScalerFeedbackActive");
	    sD->addStartCallback(
		&SoJackDragger::invalidateSurroundScaleCB, this);
	    sD->addFinishCallback(
		&SoJackDragger::invalidateSurroundScaleCB,this);
	    registerChildDragger( sD );
	}

	// Create a spherical rotator. 
	SoDragger *rD = (SoDragger *) getAnyPart( "rotator", FALSE );
	if (rD) {
	    rD->setPartAsDefault("rotator", "jackRotatorRotator" );
	    rD->setPartAsDefault("rotatorActive", "jackRotatorRotatorActive");
	    rD->setPartAsDefault("feedback", "jackRotatorFeedback" );
	    rD->setPartAsDefault("feedbackActive", "jackRotatorFeedbackActive");
	    rD->addStartCallback(
		&SoJackDragger::invalidateSurroundScaleCB, this);
	    rD->addFinishCallback(
		&SoJackDragger::invalidateSurroundScaleCB,this);
	    registerChildDragger( rD );
	}

	// Create a translator with dragpoint. 
	SoDragger *dD = (SoDragger *) getAnyPart( "translator", FALSE );
	if (dD) {
	    SoNode *n;
	    n = SoNode::getByName("jackTranslatorLineTranslator");
	    dD->setPartAsDefault("xTranslator.translator", n );
	    dD->setPartAsDefault("yTranslator.translator", n );
	    dD->setPartAsDefault("zTranslator.translator", n );

	    n = SoNode::getByName("jackTranslatorLineTranslatorActive");
	    dD->setPartAsDefault("xTranslator.translatorActive", n );
	    dD->setPartAsDefault("yTranslator.translatorActive", n );
	    dD->setPartAsDefault("zTranslator.translatorActive", n );

	    n = SoNode::getByName("jackTranslatorPlaneTranslator");
	    dD->setPartAsDefault("yzTranslator.translator", n );
	    dD->setPartAsDefault("xzTranslator.translator", n );
	    dD->setPartAsDefault("xyTranslator.translator", n );

	    n = SoNode::getByName("jackTranslatorPlaneTranslatorActive");
	    dD->setPartAsDefault("yzTranslator.translatorActive", n );
	    dD->setPartAsDefault("xzTranslator.translatorActive", n );
	    dD->setPartAsDefault("xyTranslator.translatorActive", n );

	    dD->setPartAsDefault("xFeedback", "jackTranslatorXFeedback" );
	    dD->setPartAsDefault("yFeedback", "jackTranslatorYFeedback");
	    dD->setPartAsDefault("zFeedback", "jackTranslatorZFeedback" );
	    dD->setPartAsDefault("yzFeedback", "jackTranslatorYZFeedback" );
	    dD->setPartAsDefault("xzFeedback", "jackTranslatorXZFeedback" );
	    dD->setPartAsDefault("xyFeedback", "jackTranslatorXYFeedback" );

	    dD->addStartCallback(
		&SoJackDragger::invalidateSurroundScaleCB, this);
	    dD->addFinishCallback(
		&SoJackDragger::invalidateSurroundScaleCB,this);

	    registerChildDragger( dD );
	}

	// Call the sensor CBs to make things are up-to-date.
	fieldSensorCB( this, NULL );

	// Connect the field sensors
	if (rotFieldSensor->getAttachedField() != &rotation)
	    rotFieldSensor->attach( &rotation );
	if (translFieldSensor->getAttachedField() != &translation)
	    translFieldSensor->attach( &translation );
	if (scaleFieldSensor->getAttachedField() != &scaleFactor)
	    scaleFieldSensor->attach( &scaleFactor );
    }
    else {

	// We disconnect BEFORE base class.

	// Set up the parts and callbacks in the child dragger
	// Create a scaling dragger.
	SoDragger *sD = (SoDragger *) getAnyPart( "scaler", FALSE);
	if (sD) {
	    sD->removeStartCallback(
		&SoJackDragger::invalidateSurroundScaleCB, this);
	    sD->removeFinishCallback(
		&SoJackDragger::invalidateSurroundScaleCB,this);
	    unregisterChildDragger( sD );
	}

	// Create a spherical rotator. 
	SoDragger *rD = (SoDragger *) getAnyPart( "rotator", FALSE );
	if (rD) {
	    rD->removeStartCallback(
		&SoJackDragger::invalidateSurroundScaleCB, this);
	    rD->removeFinishCallback(
		&SoJackDragger::invalidateSurroundScaleCB,this);
	    unregisterChildDragger( rD );
	}

	// Create a translator with dragpoint. 
	SoDragger *dD = (SoDragger *) getAnyPart( "translator", FALSE );
	if (dD) {
	    dD->setPartAsDefault("xFeedback", "jackTranslatorXFeedback" );
	    dD->setPartAsDefault("yFeedback", "jackTranslatorYFeedback");
	    dD->setPartAsDefault("zFeedback", "jackTranslatorZFeedback" );
	    dD->setPartAsDefault("yzFeedback", "jackTranslatorYZFeedback" );
	    dD->setPartAsDefault("xzFeedback", "jackTranslatorXZFeedback" );
	    dD->setPartAsDefault("xyFeedback", "jackTranslatorXYFeedback" );

	    dD->removeStartCallback(
		&SoJackDragger::invalidateSurroundScaleCB, this);
	    dD->removeFinishCallback(
		&SoJackDragger::invalidateSurroundScaleCB,this);

	    unregisterChildDragger( dD );
	}

	// Disconnect the field sensors.
	if (rotFieldSensor->getAttachedField())
	    rotFieldSensor->detach();
	if (translFieldSensor->getAttachedField())
	    translFieldSensor->detach();
	if (scaleFieldSensor->getAttachedField())
	    scaleFieldSensor->detach();

	SoDragger::setUpConnections( onOff, FALSE );
    }

    return !(connectionsSetUp = onOff);
}


void
SoJackDragger::valueChangedCB( void *, SoDragger *inDragger )
{
    SoJackDragger *m = (SoJackDragger *) inDragger;
    SbMatrix motMat = m->getMotionMatrix();

    SbVec3f    trans, scale;
    SbRotation rot, scaleOrient;
    getTransformFast( motMat, trans, rot, scale, scaleOrient);

    // Disconnect the field sensors
    m->rotFieldSensor->detach();
    m->translFieldSensor->detach();
    m->scaleFieldSensor->detach();

    if ( m->rotation.getValue() != rot )
	m->rotation = rot;
    if ( m->translation.getValue() != trans )
	m->translation = trans;
    if ( m->scaleFactor.getValue() != scale )
	m->scaleFactor = scale;

    // Reconnect the field sensors
    m->rotFieldSensor->attach( &(m->rotation) );
    m->translFieldSensor->attach( &(m->translation) );
    m->scaleFieldSensor->attach( &(m->scaleFactor) );
}

void
SoJackDragger::fieldSensorCB( void *inDragger, SoSensor * )
{
    SoJackDragger *dragger = (SoJackDragger *) inDragger;

    SbMatrix motMat = dragger->getMotionMatrix();
    dragger->workFieldsIntoTransform( motMat );

    dragger->setMotionMatrix( motMat );
}

void 
SoJackDragger::invalidateSurroundScaleCB(  void *parentAsVoid, SoDragger * )
{
    SoJackDragger *jD = (SoJackDragger *) parentAsVoid;

    // Invalidate the surroundScale, if it exists.
    SoSurroundScale *ss = SO_CHECK_PART(jD, "surroundScale", SoSurroundScale );
    if (ss != NULL)
	ss->invalidate();
}

void
SoJackDragger::setDefaultOnNonWritingFields()
{
    // These nodes may change after construction, but we still
    // don't want to write them out.
    surroundScale.setDefault(TRUE);
    antiSquish.setDefault(TRUE);

    // Try not to write out the sub-draggers.
	scaler.setDefault(TRUE);
	rotator.setDefault(TRUE);
	translator.setDefault(TRUE);

    // Call the base class...
    SoDragger::setDefaultOnNonWritingFields();
}
