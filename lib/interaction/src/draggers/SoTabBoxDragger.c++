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
 |	SoTabBoxDragger
 |
 |   Author(s): Paul Isaacs, Howard Look
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <stdio.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SoDB.h>

#include <Inventor/nodes/SoTransform.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSurroundScale.h>

#include <Inventor/draggers/SoTabPlaneDragger.h>
#include <Inventor/draggers/SoTabBoxDragger.h>

#include "geom/SoTabBoxDraggerGeom.h"


SO_KIT_SOURCE(SoTabBoxDragger);

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
SoTabBoxDragger::SoTabBoxDragger()
{
    SO_KIT_CONSTRUCTOR(SoTabBoxDragger);

    isBuiltIn = TRUE;

    // Don't create this by default. It's only really put into use
    // if this dragger is put inside a manipulator.
    SO_KIT_ADD_CATALOG_ENTRY(surroundScale, SoSurroundScale, 
				TRUE, topSeparator, geomSeparator,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(tabPlane1Sep, SoSeparator, FALSE,
				topSeparator, geomSeparator,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(tabPlane1Xf, SoTransform, TRUE,
				tabPlane1Sep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(tabPlane1, SoTabPlaneDragger, TRUE,
				tabPlane1Sep, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(tabPlane2Sep, SoSeparator, FALSE,
				topSeparator, geomSeparator,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(tabPlane2Xf, SoTransform, TRUE,
				tabPlane2Sep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(tabPlane2, SoTabPlaneDragger, TRUE,
				tabPlane2Sep, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(tabPlane3Sep, SoSeparator, FALSE,
				topSeparator, geomSeparator,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(tabPlane3Xf, SoTransform, TRUE,
				tabPlane3Sep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(tabPlane3, SoTabPlaneDragger, TRUE,
				tabPlane3Sep, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(tabPlane4Sep, SoSeparator, FALSE,
				topSeparator, geomSeparator,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(tabPlane4Xf, SoTransform, TRUE,
				tabPlane4Sep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(tabPlane4, SoTabPlaneDragger, TRUE,
				tabPlane4Sep, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(tabPlane5Sep, SoSeparator, FALSE,
				topSeparator, geomSeparator,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(tabPlane5Xf, SoTransform, TRUE,
				tabPlane5Sep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(tabPlane5, SoTabPlaneDragger, TRUE,
				tabPlane5Sep, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(tabPlane6Sep, SoSeparator, FALSE,
				topSeparator, geomSeparator,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(tabPlane6Xf, SoTransform, TRUE,
				tabPlane6Sep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(tabPlane6, SoTabPlaneDragger, TRUE,
				tabPlane6Sep, ,TRUE);

    // Geometry to draw a nice box instead of drawing the planes separately...
    SO_KIT_ADD_CATALOG_ENTRY(boxGeom, SoSeparator, TRUE,
				geomSeparator, ,TRUE);

    // Read the default geometry for this dragger
    if (SO_KIT_IS_FIRST_INSTANCE())
	readDefaultParts( "tabBoxDragger.iv", geomBuffer, sizeof(geomBuffer) );

    SO_KIT_ADD_FIELD(translation, (0.0, 0.0, 0.0));
    SO_KIT_ADD_FIELD(scaleFactor, (1.0, 1.0, 1.0));

    SO_KIT_INIT_INSTANCE();

    // Create the simple draggers that comprise this dragger.
    // This dragger uses these:
    // 6 Tab Plane Draggers

    SoTabPlaneDragger *tDs[6];
    tDs[0] = SO_GET_ANY_PART( this, "tabPlane1", SoTabPlaneDragger);
    tDs[1] = SO_GET_ANY_PART( this, "tabPlane2", SoTabPlaneDragger);
    tDs[2] = SO_GET_ANY_PART( this, "tabPlane3", SoTabPlaneDragger);
    tDs[3] = SO_GET_ANY_PART( this, "tabPlane4", SoTabPlaneDragger);
    tDs[4] = SO_GET_ANY_PART( this, "tabPlane5", SoTabPlaneDragger);
    tDs[5] = SO_GET_ANY_PART( this, "tabPlane6", SoTabPlaneDragger);

    // Set up the boxGeom...
    setPartAsDefault("boxGeom", "tabBoxBoxGeom");

    SoTransform *tXfs[6];

    // Create a transform to place each cube face.
    for (int i = 0; i < 6; i++) {
	
	// The tranlater dragger is perpindicular to z by default
	SbVec3f fromAxis(0,0,1), toAxis(0,0,0);
	if ( i < 3 )
	    toAxis[i] = 1;    // make x, y or z axis
	else 
	    toAxis[ i - 3 ] = -1; // make negative x, y or z axis
	tXfs[i] = new SoTransform;
	tXfs[i]->rotation.setValue(SbRotation(fromAxis, toAxis));
	tXfs[i]->translation = toAxis;
    }
    setAnyPartAsDefault("tabPlane1Xf", tXfs[0] );
    setAnyPartAsDefault("tabPlane2Xf", tXfs[1] );
    setAnyPartAsDefault("tabPlane3Xf", tXfs[2] );
    setAnyPartAsDefault("tabPlane4Xf", tXfs[3] );
    setAnyPartAsDefault("tabPlane5Xf", tXfs[4] );
    setAnyPartAsDefault("tabPlane6Xf", tXfs[5] );

    // Update the rotation and scale fields when the motionMatrix is set.
    addValueChangedCallback( &SoTabBoxDragger::valueChangedCB );
    addFinishCallback( &SoTabBoxDragger::adjustScaleTabSizeCB, this );

    // Updates the motionMatrix when the translationFactor field is set.
    translFieldSensor 
        = new SoFieldSensor( &SoTabBoxDragger::fieldSensorCB, this);
    translFieldSensor->setPriority( 0 );

    // Updates the motionMatrix when the scaleFactor field is set.
    scaleFieldSensor 
	= new SoFieldSensor( &SoTabBoxDragger::fieldSensorCB, this);
    scaleFieldSensor->setPriority( 0 );

    setUpConnections( TRUE, TRUE );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
SoTabBoxDragger::~SoTabBoxDragger()
//
////////////////////////////////////////////////////////////////////////
{
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
SoTabBoxDragger::setUpConnections( SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( onOff ) {

	// We connect AFTER base class.
	SoDragger::setUpConnections( onOff, FALSE );

	// Create parts and callbacks for the Child draggers.
	SoDragger *tDs[6];
	tDs[0] = (SoDragger *) getAnyPart( "tabPlane1", FALSE);
	tDs[1] = (SoDragger *) getAnyPart( "tabPlane2", FALSE);
	tDs[2] = (SoDragger *) getAnyPart( "tabPlane3", FALSE);
	tDs[3] = (SoDragger *) getAnyPart( "tabPlane4", FALSE);
	tDs[4] = (SoDragger *) getAnyPart( "tabPlane5", FALSE);
	tDs[5] = (SoDragger *) getAnyPart( "tabPlane6", FALSE);
	// Create a tabPlane for each cube face.
	for (int i = 0; i < 6; i++) {
	    if (tDs[i] == NULL) 
		continue;
	    // Set up the parts in the child dragger
	    tDs[i]->setPartAsDefault("translator", 
		    "tabBoxTranslator");
	    tDs[i]->setPartAsDefault("scaleTabMaterial", 
		    "tabBoxScaleTabMaterial");
	    tDs[i]->setPartAsDefault("scaleTabHints", 
		    "tabBoxScaleTabHints");

	    tDs[i]->addStartCallback(
		    &SoTabBoxDragger::invalidateSurroundScaleCB, this );
	    tDs[i]->addFinishCallback(
		    &SoTabBoxDragger::invalidateSurroundScaleCB, this );

	    registerChildDragger( tDs[i] );
	}

	// Call the sensor CBs to make things are up-to-date.
	fieldSensorCB( this, NULL );

	// Connect the field sensors
	if (translFieldSensor->getAttachedField() != &translation)
	    translFieldSensor->attach( &translation );
	if (scaleFieldSensor->getAttachedField() != &scaleFactor)
	    scaleFieldSensor->attach( &scaleFactor );
    }
    else {

	// We disconnect BEFORE base class.


	// remove the callbacks from the Child draggers.
	SoDragger *tDs[6];
	tDs[0] = (SoDragger *) getAnyPart( "tabPlane1", FALSE);
	tDs[1] = (SoDragger *) getAnyPart( "tabPlane2", FALSE);
	tDs[2] = (SoDragger *) getAnyPart( "tabPlane3", FALSE);
	tDs[3] = (SoDragger *) getAnyPart( "tabPlane4", FALSE);
	tDs[4] = (SoDragger *) getAnyPart( "tabPlane5", FALSE);
	tDs[5] = (SoDragger *) getAnyPart( "tabPlane6", FALSE);
	// Create a tabPlane for each cube face.
	for (int i = 0; i < 6; i++) {
	    if (tDs[i] == NULL) 
		continue;

	    tDs[i]->removeStartCallback(
		    &SoTabBoxDragger::invalidateSurroundScaleCB, this );
	    tDs[i]->removeFinishCallback(
		    &SoTabBoxDragger::invalidateSurroundScaleCB, this );

	    unregisterChildDragger( tDs[i] );
	}

	// Disconnect the field sensors.
	if (translFieldSensor->getAttachedField())
	    translFieldSensor->detach();
	if (scaleFieldSensor->getAttachedField())
	    scaleFieldSensor->detach();

	SoDragger::setUpConnections( onOff, FALSE );
    }

    return !(connectionsSetUp = onOff);
}


void 
SoTabBoxDragger::adjustScaleTabSize()
{
    SoTabPlaneDragger *myPlane;

    myPlane = SO_GET_ANY_PART( this, "tabPlane1", SoTabPlaneDragger);
    myPlane->adjustScaleTabSize();
    myPlane = SO_GET_ANY_PART( this, "tabPlane2", SoTabPlaneDragger);
    myPlane->adjustScaleTabSize();
    myPlane = SO_GET_ANY_PART( this, "tabPlane3", SoTabPlaneDragger);
    myPlane->adjustScaleTabSize();
    myPlane = SO_GET_ANY_PART( this, "tabPlane4", SoTabPlaneDragger);
    myPlane->adjustScaleTabSize();
    myPlane = SO_GET_ANY_PART( this, "tabPlane5", SoTabPlaneDragger);
    myPlane->adjustScaleTabSize();
    myPlane = SO_GET_ANY_PART( this, "tabPlane6", SoTabPlaneDragger);
    myPlane->adjustScaleTabSize();
}

void
SoTabBoxDragger::valueChangedCB( void *, SoDragger *inDragger )
{
    SoTabBoxDragger *m = (SoTabBoxDragger *) inDragger;
    SbMatrix motMat = m->getMotionMatrix();

    SbVec3f    trans, scale;
    SbRotation rot, scaleOrient;
    motMat.getTransform( trans, rot, scale, scaleOrient);

    // Disconnect the field sensors
    m->translFieldSensor->detach();
    m->scaleFieldSensor->detach();

    if ( m->translation.getValue() != trans )
	m->translation = trans;
    if ( m->scaleFactor.getValue() != scale )
	m->scaleFactor = scale;

    // Reconnect the field sensors
    m->translFieldSensor->attach( &(m->translation) );
    m->scaleFieldSensor->attach( &(m->scaleFactor) );
}

void
SoTabBoxDragger::fieldSensorCB( void *inDragger, SoSensor * )
{
    SoTabBoxDragger *dragger = (SoTabBoxDragger *) inDragger;

    SbMatrix motMat = dragger->getMotionMatrix();
    dragger->workFieldsIntoTransform( motMat );

    dragger->setMotionMatrix( motMat );
}

void 
SoTabBoxDragger::invalidateSurroundScaleCB(  void *parentAsVoid, SoDragger * )
{
    SoTabBoxDragger *tbD = (SoTabBoxDragger *) parentAsVoid;

    // Invalidate the surroundScale, if it exists.
    SoSurroundScale *ss = SO_CHECK_PART(tbD, "surroundScale", SoSurroundScale );
    if (ss != NULL)
	ss->invalidate();
}

void 
SoTabBoxDragger::adjustScaleTabSizeCB(  void *tabBox, SoDragger * )
{
    SoTabBoxDragger *mySelf = (SoTabBoxDragger *) tabBox;

    mySelf->adjustScaleTabSize();
}

void
SoTabBoxDragger::setDefaultOnNonWritingFields()
{
    // This node may change after construction, but we still
    // don't want to write it out.
    surroundScale.setDefault(TRUE);

    // Try not to write out the sub-draggers.
	tabPlane1.setDefault(TRUE);
	tabPlane2.setDefault(TRUE);
	tabPlane3.setDefault(TRUE);
	tabPlane4.setDefault(TRUE);
	tabPlane5.setDefault(TRUE);
	tabPlane6.setDefault(TRUE);

    // Call the base class...
    SoDragger::setDefaultOnNonWritingFields();
}
