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
 |   $Revision: 1.1 $
 |
 |   Classes:
 |	SoTransformBoxDragger
 |
 |   Author(s): Paul Isaacs, Howard Look
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <stdio.h>
#include <stdio.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SoDB.h>

#include <Inventor/sensors/SoFieldSensor.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoSurroundScale.h>
#include <Inventor/nodes/SoAntiSquish.h>

#include <Inventor/draggers/SoRotateCylindricalDragger.h>
#include <Inventor/draggers/SoTransformBoxDragger.h>
#include <Inventor/draggers/SoTranslate2Dragger.h>
#include <Inventor/draggers/SoScaleUniformDragger.h>

#include "geom/SoTransformBoxDraggerGeom.h"


SO_KIT_SOURCE(SoTransformBoxDragger);

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
SoTransformBoxDragger::SoTransformBoxDragger()
{
    SO_KIT_CONSTRUCTOR(SoTransformBoxDragger);

    isBuiltIn = TRUE;

    // Don't create this by default. It's only really put into use
    // if this dragger is put inside a manipulator.
    SO_KIT_ADD_CATALOG_ENTRY(surroundScale, SoSurroundScale, 
				TRUE, topSeparator, geomSeparator,TRUE);
    // Create an anti-squish node by default.
    SO_KIT_ADD_CATALOG_ENTRY(antiSquish, SoAntiSquish, 
				FALSE, topSeparator, geomSeparator,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(scaler, SoScaleUniformDragger, TRUE,
				topSeparator, geomSeparator,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(rotator1Sep, SoSeparator, FALSE,
				topSeparator, geomSeparator,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(rotator1Rot, SoRotation, TRUE,
				rotator1Sep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(rotator1, SoRotateCylindricalDragger, TRUE,
				rotator1Sep, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(rotator2Sep, SoSeparator, FALSE,
				topSeparator, geomSeparator,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(rotator2Rot, SoRotation, TRUE,
				rotator2Sep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(rotator2, SoRotateCylindricalDragger, TRUE,
				rotator2Sep, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(rotator3Sep, SoSeparator, FALSE,
				topSeparator, geomSeparator,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(rotator3Rot, SoRotation, TRUE,
				rotator3Sep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(rotator3, SoRotateCylindricalDragger, TRUE,
				rotator3Sep, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(translator1Sep, SoSeparator, FALSE,
				topSeparator, geomSeparator,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(translator1Rot, SoRotation, TRUE,
				translator1Sep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(translator1, SoTranslate2Dragger, TRUE,
				translator1Sep, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(translator2Sep, SoSeparator, FALSE,
				topSeparator, geomSeparator,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(translator2Rot, SoRotation, TRUE,
				translator2Sep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(translator2, SoTranslate2Dragger, TRUE,
				translator2Sep, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(translator3Sep, SoSeparator, FALSE,
				topSeparator, geomSeparator,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(translator3Rot, SoRotation, TRUE,
				translator3Sep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(translator3, SoTranslate2Dragger, TRUE,
				translator3Sep, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(translator4Sep, SoSeparator, FALSE,
				topSeparator, geomSeparator,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(translator4Rot, SoRotation, TRUE,
				translator4Sep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(translator4, SoTranslate2Dragger, TRUE,
				translator4Sep, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(translator5Sep, SoSeparator, FALSE,
				topSeparator, geomSeparator,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(translator5Rot, SoRotation, TRUE,
				translator5Sep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(translator5, SoTranslate2Dragger, TRUE,
				translator5Sep, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(translator6Sep, SoSeparator, FALSE,
				topSeparator, geomSeparator,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(translator6Rot, SoRotation, TRUE,
				translator6Sep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(translator6, SoTranslate2Dragger, TRUE,
				translator6Sep, ,TRUE);

    // Read the default geometry for this dragger
    if (SO_KIT_IS_FIRST_INSTANCE())
	readDefaultParts( "transformBoxDragger.iv", geomBuffer, sizeof(geomBuffer) );

    SO_KIT_ADD_FIELD(rotation, (0.0, 0.0, 0.0, 1.0));
    SO_KIT_ADD_FIELD(translation, (0.0, 0.0, 0.0));
    SO_KIT_ADD_FIELD(scaleFactor, (1.0, 1.0, 1.0));

    SO_KIT_INIT_INSTANCE();

    // Make the anti-squish node surround the biggest dimension
    SoAntiSquish *as = SO_GET_ANY_PART( this,"antiSquish", SoAntiSquish );
    as->sizing = SoAntiSquish::BIGGEST_DIMENSION;

    // Create the CHILD DRAGGERS.
    // 1 ScaleUniformDragger
    // 3 Cylinder Draggers
    // 6 Translate2Draggers

    SoDragger *sDrag = SO_GET_ANY_PART( this,"scaler", SoScaleUniformDragger );

    SoDragger *rDs[3];
    rDs[0] = SO_GET_ANY_PART( this, "rotator1", SoRotateCylindricalDragger);
    rDs[1] = SO_GET_ANY_PART( this, "rotator2", SoRotateCylindricalDragger);
    rDs[2] = SO_GET_ANY_PART( this, "rotator3", SoRotateCylindricalDragger);

    SoDragger *tDs[6];
    tDs[0] = SO_GET_ANY_PART( this, "translator1", SoTranslate2Dragger);
    tDs[1] = SO_GET_ANY_PART( this, "translator2", SoTranslate2Dragger);
    tDs[2] = SO_GET_ANY_PART( this, "translator3", SoTranslate2Dragger);
    tDs[3] = SO_GET_ANY_PART( this, "translator4", SoTranslate2Dragger);
    tDs[4] = SO_GET_ANY_PART( this, "translator5", SoTranslate2Dragger);
    tDs[5] = SO_GET_ANY_PART( this, "translator6", SoTranslate2Dragger);

    // Create rotations to place the rotate draggers.
    SoRotation              *rRots[3];
    int i;
    for (i = 0; i < 3; i++) {
	// The cylinder dragger works about the y axis by default
	rRots[i] = new SoRotation;
	SbVec3f fromAxis(0,1,0), toAxis(0,0,0);
	toAxis[i] = 1; // make x, y or z axis
	rRots[i]->rotation.setValue(SbRotation(fromAxis, toAxis));
    }
    setAnyPartAsDefault( "rotator1Rot", rRots[0] );
    setAnyPartAsDefault( "rotator2Rot", rRots[1] );
    setAnyPartAsDefault( "rotator3Rot", rRots[2] );

    // Create rotations to place the translate draggers.
    SoRotation              *tRots[6];
    for (i = 0; i < 6; i++) {

	tRots[i] = new SoRotation;
	
	// The tranlater dragger is perpindicular to z by default
	SbVec3f fromAxis(0,0,1), toAxis(0,0,0);
	if ( i < 3 )
	    toAxis[i] = 1;    // make x, y or z axis
	else 
	    toAxis[ i - 3 ] = -1; // make negative x, y or z axis
	tRots[i]->rotation.setValue(SbRotation(fromAxis, toAxis));
    }
    setAnyPartAsDefault( "translator1Rot", tRots[0] );
    setAnyPartAsDefault( "translator2Rot", tRots[1] );
    setAnyPartAsDefault( "translator3Rot", tRots[2] );
    setAnyPartAsDefault( "translator4Rot", tRots[3] );
    setAnyPartAsDefault( "translator5Rot", tRots[4] );
    setAnyPartAsDefault( "translator6Rot", tRots[5] );

    // Update the rotation and scale fields when the motionMatrix is set.
    addValueChangedCallback( &SoTransformBoxDragger::valueChangedCB );

    // Updates the motionMatrix when the scaleFactor field is set.
    rotFieldSensor 
	= new SoFieldSensor(&SoTransformBoxDragger::fieldSensorCB, this);
    rotFieldSensor->setPriority( 0 );

    // Updates the motionMatrix when the translationFactor field is set.
    translFieldSensor 
        = new SoFieldSensor( &SoTransformBoxDragger::fieldSensorCB, this);
    translFieldSensor->setPriority( 0 );

    // Updates the motionMatrix when the scaleFactor field is set.
    scaleFieldSensor 
	= new SoFieldSensor( &SoTransformBoxDragger::fieldSensorCB, this);
    scaleFieldSensor->setPriority( 0 );

    setUpConnections( TRUE, TRUE );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
SoTransformBoxDragger::~SoTransformBoxDragger()
//
////////////////////////////////////////////////////////////////////////
{
    if (rotFieldSensor )
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
SoTransformBoxDragger::setUpConnections( SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( onOff ) {

	// We connect AFTER base class.
	SoDragger::setUpConnections( onOff, FALSE );

	SoDragger *sD = (SoDragger *) getAnyPart("scaler", FALSE );
	if (sD) {
	    // Set up the parts in the child dragger
	    sD->setPartAsDefault("scaler", 
			    "transformBoxScalerScaler");
	    sD->setPartAsDefault("scalerActive", 
			    "transformBoxScalerScalerActive");
	    sD->setPartAsDefault("feedback", 
			    "transformBoxScalerFeedback");
	    sD->setPartAsDefault("feedbackActive", 
			    "transformBoxScalerFeedbackActive");
	    sD->addStartCallback(
		&SoTransformBoxDragger::invalidateSurroundScaleCB, this );
	    sD->addFinishCallback(
		&SoTransformBoxDragger::invalidateSurroundScaleCB, this );

	    registerChildDragger( sD );
	} 

	SoDragger *rDs[3];
	rDs[0] = (SoDragger *) getAnyPart( "rotator1", FALSE);
	rDs[1] = (SoDragger *) getAnyPart( "rotator2", FALSE);
	rDs[2] = (SoDragger *) getAnyPart( "rotator3", FALSE);
	int i;
	for (i = 0; i < 3; i++) {
	    if ( rDs[i] == NULL)
		continue;
	    rDs[i]->setPartAsDefault("rotator", 
			    "transformBoxRotatorRotator");
	    rDs[i]->setPartAsDefault("rotatorActive", 
			    "transformBoxRotatorRotatorActive");
	    rDs[i]->setPartAsDefault("feedback", 
			    "transformBoxRotatorFeedback");
	    rDs[i]->setPartAsDefault("feedbackActive", 
			   "transformBoxRotatorFeedbackActive");

	    rDs[i]->addStartCallback(
		    &SoTransformBoxDragger::invalidateSurroundScaleCB, this );
	    rDs[i]->addFinishCallback(
		    &SoTransformBoxDragger::invalidateSurroundScaleCB, this );

	    registerChildDragger( rDs[i] );
	}

	SoDragger *tDs[6];
	tDs[0] = (SoDragger *) getAnyPart( "translator1", FALSE);
	tDs[1] = (SoDragger *) getAnyPart( "translator2", FALSE);
	tDs[2] = (SoDragger *) getAnyPart( "translator3", FALSE);
	tDs[3] = (SoDragger *) getAnyPart( "translator4", FALSE);
	tDs[4] = (SoDragger *) getAnyPart( "translator5", FALSE);
	tDs[5] = (SoDragger *) getAnyPart( "translator6", FALSE);
	for (i = 0; i < 6; i++) {
	    if ( tDs[i] == NULL) 
		continue;

	    // Set up the parts in the child dragger
	    tDs[i]->setPartAsDefault("translator", 
		    "transformBoxTranslatorTranslator");
	    tDs[i]->setPartAsDefault("translatorActive", 
		    "transformBoxTranslatorTranslatorActive");
	    tDs[i]->setPartAsDefault("xAxisFeedback", 
		    "transformBoxTranslatorXAxisFeedback");
	    tDs[i]->setPartAsDefault("yAxisFeedback", 
		    "transformBoxTranslatorYAxisFeedback");

	    tDs[i]->addStartCallback(
		    &SoTransformBoxDragger::invalidateSurroundScaleCB, this );
	    tDs[i]->addFinishCallback(
		    &SoTransformBoxDragger::invalidateSurroundScaleCB, this );

	    registerChildDragger( tDs[i] );
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

	SoDragger *sD = (SoDragger *) getAnyPart("scaler", FALSE );
	if (sD) {
	    sD->removeStartCallback(
		&SoTransformBoxDragger::invalidateSurroundScaleCB, this );
	    sD->removeFinishCallback(
		&SoTransformBoxDragger::invalidateSurroundScaleCB, this );
	    unregisterChildDragger( sD );
	} 

	SoDragger *rDs[3];
	rDs[0] = (SoDragger *) getAnyPart( "rotator1", FALSE);
	rDs[1] = (SoDragger *) getAnyPart( "rotator2", FALSE);
	rDs[2] = (SoDragger *) getAnyPart( "rotator3", FALSE);
        int i;
	for (i = 0; i < 3; i++) {
	    if ( rDs[i] == NULL)
		continue;
	    rDs[i]->removeStartCallback(
		    &SoTransformBoxDragger::invalidateSurroundScaleCB, this );
	    rDs[i]->removeFinishCallback(
		    &SoTransformBoxDragger::invalidateSurroundScaleCB, this );
	    unregisterChildDragger( rDs[i] );
	}

	SoDragger *tDs[6];
	tDs[0] = (SoDragger *) getAnyPart( "translator1", FALSE);
	tDs[1] = (SoDragger *) getAnyPart( "translator2", FALSE);
	tDs[2] = (SoDragger *) getAnyPart( "translator3", FALSE);
	tDs[3] = (SoDragger *) getAnyPart( "translator4", FALSE);
	tDs[4] = (SoDragger *) getAnyPart( "translator5", FALSE);
	tDs[5] = (SoDragger *) getAnyPart( "translator6", FALSE);
	for (i = 0; i < 6; i++) {
	    if ( tDs[i] == NULL) 
		continue;
	    tDs[i]->removeStartCallback(
		    &SoTransformBoxDragger::invalidateSurroundScaleCB, this );
	    tDs[i]->removeFinishCallback(
		    &SoTransformBoxDragger::invalidateSurroundScaleCB, this );
	    unregisterChildDragger( tDs[i] );
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
SoTransformBoxDragger::valueChangedCB( void *, SoDragger *inDragger )
{
    SoTransformBoxDragger *m = (SoTransformBoxDragger *) inDragger;
    SbMatrix motMat = m->getMotionMatrix();

    SbVec3f    trans, scale;
    SbRotation rot, scaleOrient;
    motMat.getTransform( trans, rot, scale, scaleOrient);

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
SoTransformBoxDragger::fieldSensorCB( void *inDragger, SoSensor * )
{
    SoTransformBoxDragger *dragger = (SoTransformBoxDragger *) inDragger;

    SbMatrix motMat = dragger->getMotionMatrix();
    dragger->workFieldsIntoTransform( motMat );

    dragger->setMotionMatrix( motMat );
}

void 
SoTransformBoxDragger::invalidateSurroundScaleCB(void *parentAsVoid, SoDragger * )
{
    SoTransformBoxDragger *tbD = (SoTransformBoxDragger *) parentAsVoid;

    // Invalidate the surroundScale, if it exists.
    SoSurroundScale *ss = SO_CHECK_PART(tbD, "surroundScale", SoSurroundScale );
    if (ss != NULL)
	ss->invalidate();
}

void
SoTransformBoxDragger::setDefaultOnNonWritingFields()
{
    // These nodes may change after construction, but we still
    // don't want to write them out.
    surroundScale.setDefault(TRUE);
    antiSquish.setDefault(TRUE);

    // Try not to write out the sub-draggers.
	scaler.setDefault(TRUE);

	rotator1.setDefault(TRUE);
	rotator2.setDefault(TRUE);
	rotator3.setDefault(TRUE);

	translator1.setDefault(TRUE);
	translator2.setDefault(TRUE);
	translator3.setDefault(TRUE);
	translator4.setDefault(TRUE);
	translator5.setDefault(TRUE);
	translator6.setDefault(TRUE);

    // Call the base class...
    SoDragger::setDefaultOnNonWritingFields();
}

