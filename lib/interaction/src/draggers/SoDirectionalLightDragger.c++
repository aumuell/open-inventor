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
 |	SoDirectionalLightDragger
 |
 |
 | Description:
 |  This is the source file for the SoDirectionalLightDragger.
 |  This is a composite dragger which allows independent rotation,
 |  and translation for a directional light.
 |  When working on a directional light, the translation serves only
 |  to position the dragger in space, but has no effect on the lights
 |  direction.
 |
 |  It is composed of an SoRotateSphericalDragger (for rotation),
 |  and an SoDragPointDragger (for translation),
 |
 |   Author(s): Paul Isaacs, David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <stdio.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoPath.h>
#include <Inventor/sensors/SoFieldSensor.h>

#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/draggers/SoDragPointDragger.h>
#include <Inventor/draggers/SoRotateSphericalDragger.h>
#include <Inventor/draggers/SoDirectionalLightDragger.h>
#include <Inventor/projectors/SbSphereSectionProjector.h>

#include "geom/SoDirectionalLightDraggerGeom.h"


SO_KIT_SOURCE(SoDirectionalLightDragger);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
SoDirectionalLightDragger::SoDirectionalLightDragger()
//
////////////////////////////////////////////////////////////////////////
{
    SO_KIT_CONSTRUCTOR(SoDirectionalLightDragger);

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
				TRUE, topSeparator, geomSeparator,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(translatorRotInv, SoRotation, 
				TRUE, translatorSep, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(translator, SoDragPointDragger, 
				TRUE, translatorSep, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(rotator, SoRotateSphericalDragger, 
				TRUE, topSeparator, geomSeparator,TRUE);

    // read geometry for shared parts
    if (SO_KIT_IS_FIRST_INSTANCE())
	readDefaultParts("directionalLightDragger.iv", 
			  geomBuffer, sizeof(geomBuffer) );

    SO_KIT_ADD_FIELD(translation, (0.0, 0.0, 0.0));
    SO_KIT_ADD_FIELD(rotation, (0.0, 0.0, 0.0, 1.0));

    SO_KIT_INIT_INSTANCE();

    // Set the overall material.
    // We need to use a copy of the resource, since the resource is shared
    // by all manips but we are going to edit ours.
    SoNode *resourceMtl 
	= SoNode::getByName("directionalLightOverallMaterial");
    setPartAsDefault("material", resourceMtl->copy() );

    // CREATE THE CHILD DRAGGERS:
    // 1 DragpointDragger
    // 1 RotateSphericalDragger

	SoDragPointDragger *trD;
	trD = SO_GET_ANY_PART( this,"translator", SoDragPointDragger );

	SoRotateSphericalDragger *roD;
	roD = SO_GET_ANY_PART( this, "rotator", SoRotateSphericalDragger );

    // Update the rotation and translation fields when the motionMatrix is set.
    addValueChangedCallback( &SoDirectionalLightDragger::valueChangedCB );

    // Updates the motionMatrix when the translationFactor field is set.
    translFieldSensor 
     = new SoFieldSensor( &SoDirectionalLightDragger::fieldSensorCB, this);
    translFieldSensor->setPriority( 0 );

    // Updates the motionMatrix when the rotation field is set.
    rotFieldSensor 
     = new SoFieldSensor( &SoDirectionalLightDragger::fieldSensorCB, this);
    rotFieldSensor->setPriority( 0 );

    setUpConnections( TRUE, TRUE );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
SoDirectionalLightDragger::~SoDirectionalLightDragger()
//
////////////////////////////////////////////////////////////////////////
{
    if (translFieldSensor )
	delete translFieldSensor;
    if (rotFieldSensor )
	delete rotFieldSensor;
}

//    detach/attach any sensors, callbacks, and/or field connections.
//    Called by:            start/end of SoBaseKit::readInstance
//    and on new copy by:   start/end of SoBaseKit::copy.
//    Classes that redefine must call setUpConnections(TRUE,TRUE) 
//    at end of constructor.
//    Returns the state of the node when this was called.
SbBool
SoDirectionalLightDragger::setUpConnections( SbBool onOff, SbBool doItAlways )
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
	    n = SoNode::getByName("directionalLightTranslatorLineTranslator");
	    trD->setPartAsDefault("xTranslator.translator", n );
	    trD->setPartAsDefault("yTranslator.translator", n );
	    trD->setPartAsDefault("zTranslator.translator", n );

	    n = SoNode::getByName(
			    "directionalLightTranslatorLineTranslatorActive");
	    trD->setPartAsDefault("xTranslator.translatorActive", n );
	    trD->setPartAsDefault("yTranslator.translatorActive", n );
	    trD->setPartAsDefault("zTranslator.translatorActive", n );

	    n = SoNode::getByName("directionalLightTranslatorPlaneTranslator");
	    trD->setPartAsDefault("yzTranslator.translator", n );
	    trD->setPartAsDefault("xzTranslator.translator", n );
	    trD->setPartAsDefault("xyTranslator.translator", n );

	    n = SoNode::getByName(
			    "directionalLightTranslatorPlaneTranslatorActive");
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
			    "directionalLightRotatorRotator");
	    roD->setPartAsDefault("rotatorActive", 
			    "directionalLightRotatorRotatorActive");
	    roD->setPartAsDefault("feedback", 
			    "directionalLightRotatorFeedback" );
	    roD->setPartAsDefault("feedbackActive", 
			  "directionalLightRotatorFeedbackActive");

	    registerChildDragger( roD );

	}

	// Call the sensor CBs to make things are up-to-date.
	fieldSensorCB( this, NULL );

	// Connect the field sensors
	if (rotFieldSensor->getAttachedField() != &rotation)
	    rotFieldSensor->attach( &rotation );
	if (translFieldSensor->getAttachedField() != &translation)
	    translFieldSensor->attach( &translation );
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

	SoDragger::setUpConnections( onOff, FALSE );
    }

    return !(connectionsSetUp = onOff);
}


void
SoDirectionalLightDragger::valueChangedCB( void *, SoDragger *inDragger )
{
    SoDirectionalLightDragger *d = (SoDirectionalLightDragger *) inDragger;
    SbMatrix motMat = d->getMotionMatrix();

    SbVec3f    trans, scale;
    SbRotation rot, scaleOrient;
    motMat.getTransform( trans, rot, scale, scaleOrient );

    // Disconnect the field sensors
    d->translFieldSensor->detach();
    d->rotFieldSensor->detach();

    if ( d->translation.getValue() != trans )
	d->translation = trans;
    if ( d->rotation.getValue() != rot )
	d->rotation = rot;

    // Make the rotation inside the "translaterRotInv" be the inverse 
    // of the new rotation.
    SbRotation newInv = rot.inverse();
    SoRotation *rotInv = (SoRotation *) d->getAnyPart("translatorRotInv",TRUE);
    if (rotInv->rotation.getValue() != newInv)
	rotInv->rotation = newInv;

    // Reconnect the field sensors
    d->translFieldSensor->attach( &(d->translation) );
    d->rotFieldSensor->attach( &(d->rotation) );
}

void
SoDirectionalLightDragger::fieldSensorCB(void *inDragger, SoSensor *)
{
   SoDirectionalLightDragger *dragger = (SoDirectionalLightDragger *) inDragger;

    SbMatrix motMat = dragger->getMotionMatrix();
    dragger->workFieldsIntoTransform(motMat);

    dragger->setMotionMatrix(motMat);
}

void
SoDirectionalLightDragger::setDefaultOnNonWritingFields()
{
    // This node may change after construction, but we still
    // don't want to write it out.
    translatorRotInv.setDefault(TRUE);

    // Try not to write out the sub-draggers.
	translator.setDefault(TRUE);
	rotator.setDefault(TRUE);

    // Call the base class...
    SoDragger::setDefaultOnNonWritingFields();
}
