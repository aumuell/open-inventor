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
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |	SoPointLightDragger
 |
 |   Author(s): Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <stdio.h>
#include <Inventor/SoDB.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include <Inventor/draggers/SoDragPointDragger.h>
#include <Inventor/draggers/SoPointLightDragger.h>

#include "geom/SoPointLightDraggerGeom.h"


SO_KIT_SOURCE(SoPointLightDragger);


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
SoPointLightDragger::SoPointLightDragger()
//
////////////////////////////////////////////////////////////////////////
{
    SO_KIT_CONSTRUCTOR(SoPointLightDragger);

    isBuiltIn = TRUE;

    // This gives the dragger an overall material.  It is edited by lightManips
    // to make its dragger match the color of the light.  Any materials within 
    // other parts will override this one. 
    SO_KIT_ADD_CATALOG_ENTRY(material, SoMaterial, 
				TRUE, topSeparator, geomSeparator,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(translator,SoDragPointDragger,
				TRUE, topSeparator, geomSeparator  ,TRUE);

    // Read the default geometry 
    if (SO_KIT_IS_FIRST_INSTANCE())
	readDefaultParts( "pointLightDragger.iv",
			   geomBuffer, sizeof(geomBuffer) );

    SO_KIT_ADD_FIELD(translation, (0.0, 0.0, 0.0));

    SO_KIT_INIT_INSTANCE();

    // Set the overall material.
    // We need to use a copy of the resource, since the resource is shared
    // by all manips but we are going to edit ours.
    SoNode *resourceMtl = SoNode::getByName("pointLightOverallMaterial");
    setPartAsDefault("material", resourceMtl->copy() );


    // Create the simple draggers that comprise this dragger.
    // This dragger uses these:
    // 1 DragPointDragger
	SoDragPointDragger *trD;
	trD = SO_GET_ANY_PART( this,"translator", SoDragPointDragger );

    // Update the rotation and scale fields when the motionMatrix is set.
    addValueChangedCallback( &SoPointLightDragger::valueChangedCB );

    // Updates the motionMatrix when the translationFactor field is set.
    fieldSensor = new SoFieldSensor( &SoPointLightDragger::fieldSensorCB, this);
    fieldSensor->setPriority( 0 );

    setUpConnections( TRUE, TRUE );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
SoPointLightDragger::~SoPointLightDragger()
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
SoPointLightDragger::setUpConnections( SbBool onOff, SbBool doItAlways )
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
	    n = SoNode::getByName("pointLightTranslatorLineTranslator");
	    trD->setPartAsDefault("xTranslator.translator", n );
	    trD->setPartAsDefault("yTranslator.translator", n );
	    trD->setPartAsDefault("zTranslator.translator", n );

	    n = SoNode::getByName( "pointLightTranslatorLineTranslatorActive");
	    trD->setPartAsDefault("xTranslator.translatorActive", n );
	    trD->setPartAsDefault("yTranslator.translatorActive", n );
	    trD->setPartAsDefault("zTranslator.translatorActive", n );

	    n = SoNode::getByName("pointLightTranslatorPlaneTranslator");
	    trD->setPartAsDefault("yzTranslator.translator", n );
	    trD->setPartAsDefault("xzTranslator.translator", n );
	    trD->setPartAsDefault("xyTranslator.translator", n );

	    n = SoNode::getByName( "pointLightTranslatorPlaneTranslatorActive");
	    trD->setPartAsDefault("yzTranslator.translatorActive", n );
	    trD->setPartAsDefault("xzTranslator.translatorActive", n );
	    trD->setPartAsDefault("xyTranslator.translatorActive", n );

	    registerChildDragger( trD );
	}

	// Call the sensor CBs to make things are up-to-date.
	fieldSensorCB( this, NULL );

	// Connect the field sensors
	if (fieldSensor->getAttachedField() != &translation)
	    fieldSensor->attach( &translation );
    }
    else {

	// We disconnect BEFORE base class.

	// remove callbacks from the child draggers
        SoDragger *trD = (SoDragger *) getAnyPart( "translator", FALSE );
        unregisterChildDragger( trD );

	// Disconnect the field sensors.
	if (fieldSensor->getAttachedField())
	    fieldSensor->detach();

	SoDragger::setUpConnections( onOff, FALSE );
    }

    return !(connectionsSetUp = onOff);
}

void
SoPointLightDragger::valueChangedCB( void *, SoDragger *inDragger )
{
    SoPointLightDragger *d = (SoPointLightDragger *) inDragger;
    SbMatrix motMat = d->getMotionMatrix();

    SbVec3f    trans, scale;
    SbRotation rot, scaleOrient;
    motMat.getTransform( trans, rot, scale, scaleOrient );

    // Disconnect the field sensors
    d->fieldSensor->detach();

    if ( d->translation.getValue() != trans )
	d->translation = trans;

    // Reconnect the field sensors
    d->fieldSensor->attach( &(d->translation) );
}

void
SoPointLightDragger::fieldSensorCB(void *inDragger, SoSensor *)
{
   SoPointLightDragger *dragger = (SoPointLightDragger *) inDragger;

    SbMatrix motMat = dragger->getMotionMatrix();
    dragger->workFieldsIntoTransform(motMat);

    dragger->setMotionMatrix(motMat);
}

void
SoPointLightDragger::setDefaultOnNonWritingFields()
{
    // Try not to write out the sub-draggers.
	translator.setDefault(TRUE);

    // Call the base class...
    SoDragger::setDefaultOnNonWritingFields();
}
