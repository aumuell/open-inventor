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
 |      Defines classes for creating links to be used in linkages.
 |
 |   Author(s)          : Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/nodekits/SoAppearanceKit.h>

#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>

#include <Inventor/SoPickedPoint.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/draggers/SoRotateDiscDragger.h>
#include <Inventor/draggers/SoTranslate1Dragger.h>
#include <Inventor/draggers/SoTranslate2Dragger.h>
#include <Inventor/events/SoEvent.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include <Inventor/errors/SoDebugError.h>

#include "LinkClasses.h"

//////////////////////////////////////////////////////////////////////////////
//    Class: LinkBase
//
//    New nodes in this subclass are:
//      appearance,
//    New fields in this subclass are:
//         none
//
//////////////////////////////////////////////////////////////////////////////

SO_KIT_SOURCE(LinkBase);

//
// Description:
//    This initializes the LinkBase class.
//
// Use: public
//
void
LinkBase::initClass()
{
    SO__KIT_INIT_CLASS(LinkBase, "LinkBase", SoInteractionKit);
}

//
// Description:
//    Constructor
//
// Use: public
//
LinkBase::LinkBase()
{
    SO_KIT_CONSTRUCTOR(LinkBase);

    isBuiltIn = TRUE;

    // Define new entries to catalog for this class.
    SO_KIT_ADD_CATALOG_ENTRY(appearance, SoAppearanceKit, TRUE,
				topSeparator, , TRUE );


    // Add the new fields.
    SO_KIT_ADD_FIELD(draggersOn, (1));
    SO_KIT_ADD_FIELD(isError,    (0));

    SO_KIT_INIT_INSTANCE();

    isShowingErrorColor = FALSE;
    savedMaterial = NULL;

    draggersOnSensor = new SoFieldSensor(&LinkBase::draggersOnSensorCB, this );;
    isErrorSensor = new SoFieldSensor(&LinkBase::isErrorSensorCB, this );;

    setUpConnections( TRUE, TRUE );
}

void
LinkBase::draggersOnSensorCB(  void *mePtr, SoSensor *)
{
    LinkBase *myself = (LinkBase *) mePtr;

    myself->setDraggers( myself->draggersOn.getValue() );
}

void
LinkBase::isErrorSensorCB(  void *mePtr, SoSensor *)
{
    LinkBase *myself = (LinkBase *) mePtr;

    myself->errorColor( myself->isError.getValue() );
}

SbBool
LinkBase::undoConnections() 
{
    draggersOnSensor->detach();
    isErrorSensor->detach();

    setDraggers( FALSE );

    connectionsSetUp = FALSE;
    return !connectionsSetUp;
}

SbBool
LinkBase::setUpConnections(SbBool onOff, SbBool doItAlways ) 
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( !onOff )
	return undoConnections();

    setDraggers( draggersOn.getValue() );
    setDraggers( draggersOn.getValue() );

    draggersOnSensor->attach(&draggersOn);
    isErrorSensor->attach(&isError);

    connectionsSetUp = onOff;
    return !connectionsSetUp;
}

void
LinkBase::setDefaultOnNonWritingFields()
{
    // We won't be writing the draggers to file.  We overloaded the
    // write method to turn off connections before writing, which includes
    // turning off the draggers.
    // (They are turned back on after writing is completed).
    draggersOn.setDefault(TRUE);

    appearance.setDefault(TRUE);

    SoInteractionKit::setDefaultOnNonWritingFields();
}

//
// If middle button goes down over this LinkBase, toggle the draggers
//
void
LinkBase::handleEvent(SoHandleEventAction *ha)
{
    if ( ha->isHandled() )
	return;

    // Only if there's no grabber...
    if ( ha->getGrabber() == NULL  ) {
    
	// get event from the action.
	const SoEvent *event = ha->getEvent();

	// If the middle mouse went down...
	if (SO_MOUSE_PRESS_EVENT(event, BUTTON2)) {

	    // Was there a pick?
	    // It's over us if we're on the pick path.
	    SoPickedPoint *pp = (SoPickedPoint *) ha->getPickedPoint();
	    if (pp && pp->getPath() && pp->getPath()->containsNode(this) ) {

		// We were picked!
		// Toggle the draggerOn value...
		SbBool onNow = draggersOn.getValue();
		draggersOn.setValue( onNow ? FALSE : TRUE );
		ha->setHandled();
		return;
	    }
	}
    }

    // If we didn't handle the event, let the base class traverse the
    // children
    SoInteractionKit::handleEvent( ha );
}

void
LinkBase::setDraggers( SbBool )
{
    // Do nothing in base class
}

void 
LinkBase::errorColor( SbBool useErrorColor )
{
    if (isShowingErrorColor == useErrorColor )
	return;

    if (useErrorColor == FALSE) {
	setPartAsDefault( "material", savedMaterial );
	isShowingErrorColor = FALSE;
    }
    else {
	SoNode *newSavedMtl = getAnyPart( "material", FALSE );
	if (newSavedMtl)
	    newSavedMtl->ref();
	if (savedMaterial)
	    savedMaterial->unref();
	savedMaterial = newSavedMtl;
	SoNode *errorMat = SoNode::getByName("ERROR_MATERIAL");
	setPartAsDefault( "material", errorMat );	
	isShowingErrorColor = TRUE;
    }
}

void
LinkBase::write( SoWriteAction *action )
{
    if (action->getOutput()->getStage()  == SoOutput::COUNT_REFS) {
        // Undo all the connections before counting
	setUpConnections(FALSE);
        SoInteractionKit::write( action );
    }
    else {
        // Put back all the connections afterwards.
        SoInteractionKit::write( action );
        setUpConnections(TRUE);
    }
}

//
// Description:
//    Destructor (necessary since inline destructor is too complex)
//
// Use: public
//
LinkBase::~LinkBase()
{
    if (draggersOnSensor)
        delete draggersOnSensor;
    if (isErrorSensor)
        delete isErrorSensor;
}

//////////////////////////////////////////////////////////////////////////////
//    Class: SimpleLink
//
//    New nodes in this subclass are:
//      originTranslator, originTranslateGeom,
//      angleRotator, angleRotateGeom,
//      endPointTranslateSeparator, endPointTranslator, endPointTranslateGeom,
//      oneDScaleSeparator, oneDScaler, oneDScaleGeom,
//      twoDScaleSeparator, twoDScaler, twoDScaleGeom,
//      threeDScaleSeparator, threeDScaler, threeDScaleGeom,
//
//    New fields in this subclass are:
//         origin         - location of first link point
//         angle          - angle of rotation from (1,0,0) about Z-axis
//         size           - amount of scale of link
//         endPoint         - location of second link point
//////////////////////////////////////////////////////////////////////////////

SO_KIT_SOURCE(SimpleLink);

//
// Description:
//    This initializes the SimpleLink class.
//
// Use: public
//
void
SimpleLink::initClass()
{
    SO__KIT_INIT_CLASS(SimpleLink, "SimpleLink", LinkBase);
}

//
// Description:
//    Constructor
//
// Use: public
//
SimpleLink::SimpleLink()
{
    SO_KIT_CONSTRUCTOR(SimpleLink);

    isBuiltIn = TRUE;

    // Define new entries to catalog for this class.
    SO_KIT_ADD_CATALOG_ENTRY(originTranslator, SoTranslation, TRUE,
				topSeparator, , FALSE );
    SO_KIT_ADD_CATALOG_ENTRY(originTranslateGeom, SoSeparator, TRUE,
				topSeparator, , TRUE );

    SO_KIT_ADD_CATALOG_ENTRY(angleRotator, SoRotationXYZ, TRUE,
				topSeparator, , FALSE );
    SO_KIT_ADD_CATALOG_ENTRY(angleRotateGeom, SoSeparator, TRUE,
				topSeparator, , TRUE );

    SO_KIT_ADD_CATALOG_ENTRY(endPointTranslateSeparator, SoSeparator, TRUE,
				topSeparator, , FALSE );
    SO_KIT_ADD_CATALOG_ENTRY(endPointTranslator, SoTranslation, TRUE,
				endPointTranslateSeparator, , FALSE );
    SO_KIT_ADD_CATALOG_ABSTRACT_ENTRY(endPointTranslateGeom, SoNode, SoCube, TRUE,
				endPointTranslateSeparator, , TRUE );

    SO_KIT_ADD_CATALOG_ENTRY(oneDScaleSeparator, SoSeparator, TRUE,
				topSeparator, , FALSE );
    SO_KIT_ADD_CATALOG_ENTRY(oneDScaler, SoScale, TRUE,
				oneDScaleSeparator, , FALSE );
    SO_KIT_ADD_CATALOG_ABSTRACT_ENTRY(oneDScaleGeom, SoNode, SoCube, TRUE,
				oneDScaleSeparator, , TRUE );

     SO_KIT_ADD_CATALOG_ENTRY(twoDScaleSeparator, SoSeparator, TRUE,
 				topSeparator, , FALSE );
     SO_KIT_ADD_CATALOG_ENTRY(twoDScaler, SoScale, TRUE,
 				twoDScaleSeparator, , FALSE );
     SO_KIT_ADD_CATALOG_ABSTRACT_ENTRY(twoDScaleGeom, SoNode, SoCube, TRUE,
 				twoDScaleSeparator, , TRUE );

     SO_KIT_ADD_CATALOG_ENTRY(threeDScaleSeparator, SoSeparator, TRUE,
 				topSeparator, , FALSE );
     SO_KIT_ADD_CATALOG_ENTRY(threeDScaler, SoScale, TRUE,
 				threeDScaleSeparator, , FALSE );
     SO_KIT_ADD_CATALOG_ABSTRACT_ENTRY(threeDScaleGeom, SoNode, SoCube, TRUE,
 				threeDScaleSeparator, , TRUE );


    // Add the new fields.
    SO_KIT_ADD_FIELD(origin,     (0,0,0));
    SO_KIT_ADD_FIELD(angle,      (0));
    SO_KIT_ADD_FIELD(size,       (1));
    SO_KIT_ADD_FIELD(endPoint,   (1,0,0));

    SO_KIT_INIT_INSTANCE();

    sizeZeroZeroEngine = new SoComposeVec3f;
    sizeZeroZeroEngine->ref();
    sizeZeroZeroEngine->y = 0.0;
    sizeZeroZeroEngine->z = 0.0;

    sizeOneOneEngine = new SoComposeVec3f;
    sizeOneOneEngine->ref();
    sizeOneOneEngine->y = 1.0;
    sizeOneOneEngine->z = 1.0;

    sizeSizeOneEngine = new SoComposeVec3f;
    sizeSizeOneEngine->ref();
    sizeSizeOneEngine->z = 1.0;

    sizeSizeSizeEngine = new SoComposeVec3f;
    sizeSizeSizeEngine->ref();

    endPointEngine = new EndPointFromParamsEngine;
    endPointEngine->ref();

    setUpConnections( TRUE, TRUE );
}

SbBool
SimpleLink::undoConnections()
{
    setDraggers( FALSE );

    SoTranslation *trans;
    SoRotationXYZ *rot;
    SoScale       *scale;

    // Translation of Origin
	trans = (SoTranslation *) getAnyPart("originTranslator",TRUE);
	trans->translation.disconnect();

    // Rotation about Angle
        rot = (SoRotationXYZ *) getAnyPart("angleRotator",TRUE);
	rot->angle.disconnect();

    // Translation to EndPoint
	sizeZeroZeroEngine->x.disconnect();

	trans = (SoTranslation *) getAnyPart("endPointTranslator",TRUE);
	trans->translation.disconnect();

    // oneD scale to EndPoint
        sizeOneOneEngine->x.disconnect();

	scale = (SoScale *) getAnyPart("oneDScaler", TRUE );
	scale->scaleFactor.disconnect();

    // twoD scale to EndPoint
	sizeSizeOneEngine->x.disconnect();
	sizeSizeOneEngine->y.disconnect();

	scale = (SoScale *) getAnyPart("twoDScaler", TRUE );
	scale->scaleFactor.disconnect();

    // threeD scale to EndPoint
	sizeSizeSizeEngine->x.disconnect();
	sizeSizeSizeEngine->y.disconnect();
	sizeSizeSizeEngine->z.disconnect();

	scale = (SoScale *) getAnyPart("threeDScaler", TRUE );
	scale->scaleFactor.disconnect();

    // Calculates endPoint field based on other params.
        endPointEngine->inOrigin.disconnect();
        endPointEngine->inSize.disconnect();
        endPointEngine->inAngle.disconnect();

	// This is a field that might be connected from somewhere other than
	// our own engine. Only disconnect if appropriate.
	SoEngineOutput *eo;
	if ( endPoint.getConnectedEngine(eo)) {
	    if ( eo == &endPointEngine->outEndPoint )
		endPoint.disconnect();
	}

    LinkBase::undoConnections();

    connectionsSetUp = FALSE;
    return !connectionsSetUp;
}


SbBool
SimpleLink::setUpConnections(SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( !onOff )
	return undoConnections();

    LinkBase::setUpConnections(onOff, doItAlways);

    SoTranslation *trans;
    SoRotationXYZ *rot;
    SoScale       *scale;

    // Translation of Origin
	trans = (SoTranslation *) getAnyPart("originTranslator",TRUE);
	trans->translation.connectFrom( &origin );

    // Rotation about Angle
        rot = (SoRotationXYZ *) getAnyPart("angleRotator",TRUE);
	if (rot->axis.getValue() != SoRotationXYZ::Z )
	    rot->axis.setValue(SoRotationXYZ::Z);
	rot->angle.connectFrom( &angle );

    // Translation to EndPoint
	sizeZeroZeroEngine->x.connectFrom( &size );

	trans = (SoTranslation *) getAnyPart("endPointTranslator",TRUE);
	trans->translation.connectFrom( &sizeZeroZeroEngine->vector );

    // oneD scale to EndPoint
        sizeOneOneEngine->x.connectFrom( &size );

	scale = (SoScale *) getAnyPart("oneDScaler", TRUE );
	scale->scaleFactor.connectFrom( &sizeOneOneEngine->vector );

    // twoD scale to EndPoint
	sizeSizeOneEngine->x.connectFrom( &size );
	sizeSizeOneEngine->y.connectFrom( &size );

	scale = (SoScale *) getAnyPart("twoDScaler", TRUE );
	scale->scaleFactor.connectFrom( &sizeSizeOneEngine->vector );

    // threeD scale to EndPoint
	sizeSizeSizeEngine->x.connectFrom( &size );
	sizeSizeSizeEngine->y.connectFrom( &size );
	sizeSizeSizeEngine->z.connectFrom( &size );

	scale = (SoScale *) getAnyPart("threeDScaler", TRUE );
	scale->scaleFactor.connectFrom( &sizeSizeSizeEngine->vector );

    // Calculates endPoint field based on other params.
        endPointEngine->inOrigin.connectFrom( &origin );
        endPointEngine->inSize.connectFrom( &size );
        endPointEngine->inAngle.connectFrom( &angle );
	// This may be connected from somewhere else. We don't want
	// to override...
	if ( ! endPoint.isConnected() )
	    endPoint.connectFrom( &endPointEngine->outEndPoint );

    setDraggers( draggersOn.getValue() );

    connectionsSetUp = onOff;
    return !connectionsSetUp;
}
void
SimpleLink::setDefaultOnNonWritingFields()
{
    // This information shouldn't write to file. It will be calculated
    // when read in from file by the engines.
    originTranslator.setDefault(TRUE);
    endPointTranslator.setDefault(TRUE);
    angleRotator.setDefault(TRUE);
    oneDScaler.setDefault(TRUE);
    twoDScaler.setDefault(TRUE);
    threeDScaler.setDefault(TRUE);

    LinkBase::setDefaultOnNonWritingFields();
}

void
SimpleLink::setDraggers( SbBool on )
{
    LinkBase::setDraggers( on );
}

//
// Description:
//    Destructor (necessary since inline destructor is too complex)
//
// Use: public
//
SimpleLink::~SimpleLink()
{
    if (sizeZeroZeroEngine)
	sizeZeroZeroEngine->unref();
    if (sizeOneOneEngine)
	sizeOneOneEngine->unref();
    if (sizeSizeOneEngine)
	sizeSizeOneEngine->unref();
    if (sizeSizeSizeEngine)
	sizeSizeSizeEngine->unref();
    if (endPointEngine)
        endPointEngine->unref();
}

//////////////////////////////////////////////////////////////////////////////
//    Class: GroundedSimpleLink - subclass of SimpleLink with a
//                              a dragger to control the origin field
//
//    New nodes in this subclass are:
//       originDragger
//
//    New fields in this subclass are:
//       none
//////////////////////////////////////////////////////////////////////////////

SO_KIT_SOURCE(GroundedSimpleLink);

//
// Description:
//    This initializes the Link class.
//
// Use: internal
//
void
GroundedSimpleLink::initClass()
{
    SO__KIT_INIT_CLASS(GroundedSimpleLink, "GroundedSimpleLink", SimpleLink);
}

//
// Description:
//    Constructor
//
// Use: public
//
GroundedSimpleLink::GroundedSimpleLink()
{
    SO_KIT_CONSTRUCTOR(GroundedSimpleLink);

    isBuiltIn = TRUE;

    // Define new entries to catalog for this class.
    SO_KIT_ADD_CATALOG_ENTRY(originDragger, SoTranslate2Dragger, TRUE,
				topSeparator, appearance, TRUE );

    SO_KIT_INIT_INSTANCE();

    // Add the new fields. - none for this class

    setUpConnections( TRUE, TRUE );
}

SbBool
GroundedSimpleLink::undoConnections()
{
    setDraggers( FALSE );

    // BASE CLASS
    SimpleLink::undoConnections();

    connectionsSetUp = FALSE;
    return !connectionsSetUp;
}

SbBool
GroundedSimpleLink::setUpConnections(SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( !onOff )
	return undoConnections();

    // BASE CLASS
	SimpleLink::setUpConnections(onOff, doItAlways);

    setDraggers( draggersOn.getValue() );

    connectionsSetUp = onOff;
    return !connectionsSetUp;
}
void
GroundedSimpleLink::setDefaultOnNonWritingFields()
{
    SimpleLink::setDefaultOnNonWritingFields();
}

void
GroundedSimpleLink::setDraggers( SbBool on )
{
    // First call base class
    SimpleLink::setDraggers(on);

    setOriginDragger( on );
}

void
GroundedSimpleLink::setOriginDragger( SbBool on )
{
    if ( on == TRUE ) {

	// Create and connect dragger for origin 
	// But only do it if the field is not connected to anything else.
	// We'll also be setting parts from resources, if we can find them.

	if ( ! origin.isConnected() ) {
	    // Set to NULL to make sure we get a new one!
	    setAnyPart("originDragger", NULL );
	    SoTranslate2Dragger *orDragger 
		= (SoTranslate2Dragger *) getAnyPart("originDragger", TRUE );

	    // Set the parts based on resources
	    SoNode *part = SoNode::getByName("LINK_POINT_DRAGGER");
	    SoNode *activePart = SoNode::getByName("LINK_POINT_DRAGGER_ACTIVE");
	    if (part != NULL)
		orDragger->setPartAsDefault("translator", part, FALSE );
	    if (activePart != NULL)
		orDragger->setPartAsDefault("translatorActive", 
					    activePart,FALSE);

	    // Load initial value.
	    if (orDragger->translation.getValue() != origin.getValue())
		orDragger->translation.setValue(origin.getValue());

	    // Connect our origin field from the output of this dragger.
	    origin.connectFrom( &orDragger->translation );
	}
    }
    else {
	SoTranslate2Dragger *orDragger 
	    = SO_CHECK_ANY_PART(this, "originDragger", SoTranslate2Dragger );
	if ( orDragger != NULL ) {
	    SoField *f;
	    if ( origin.getConnectedField(f) ) {
		if ( f == &orDragger->translation )
		    origin.disconnect();
	    }
	    // Remove the dragger.
	    setAnyPart("originDragger", NULL );
	}
    }
}

//
// Description:
//    Destructor (necessary since inline destructor is too complex)
//
// Use: public
//
GroundedSimpleLink::~GroundedSimpleLink()
{
}

//////////////////////////////////////////////////////////////////////////////
//    Class: Link - subclass of GroundedSimpleLink with 
//                  dragger to control the endPoint
//
//    New nodes in this subclass are:
//       endPointDragger
//
//    New fields in this subclass are:
//       none
//////////////////////////////////////////////////////////////////////////////

SO_KIT_SOURCE(Link);

//
// Description:
//    This initializes the Link class.
//
// Use: internal
//
void
Link::initClass()
{
    SO__KIT_INIT_CLASS(Link, "Link", GroundedSimpleLink);
}

//
// Description:
//    Constructor
//
// Use: public
//
Link::Link()
{
    SO_KIT_CONSTRUCTOR(Link);

    isBuiltIn = TRUE;

    // Define new entries to catalog for this class.
    SO_KIT_ADD_CATALOG_ENTRY(endPointDragger, SoTranslate2Dragger, TRUE,
				topSeparator, appearance, TRUE );

    SO_KIT_INIT_INSTANCE();

    // Add the new fields. - no new ones in this class

    // Set the parts that we can find as resources...
    setPartAsDefault("originTranslateGeom",   "linkOriginTranslateGeom"); 
    setPartAsDefault("angleRotateGeom",       "linkAngleRotateGeom"); 
    setPartAsDefault("endPointTranslateGeom", "linkEndPointTranslateGeom"); 
    setPartAsDefault("oneDScaleGeom",         "linkOneDScaleGeom");
    setPartAsDefault("twoDScaleGeom",         "linkTwoDScaleGeom");
    setPartAsDefault("threeDScaleGeom",       "linkThreeDScaleGeom");
    setPartAsDefault("material",              "linkMaterial");

    getAngleAndSizeEngine = new LinkEngine;
    getAngleAndSizeEngine->ref();

    setUpConnections( TRUE, TRUE );
}

SbBool
Link::undoConnections()
{
    setDraggers( FALSE );

    getAngleAndSizeEngine->inOrigin.disconnect();
    getAngleAndSizeEngine->inEndPoint.disconnect();

    // This is a field that might be connected from somewhere other than
    // our own engine. Only disconnect if appropriate.
    SoEngineOutput *eo;
    if ( angle.getConnectedEngine(eo)) {
	if ( eo == &getAngleAndSizeEngine->outAngle )
    	    angle.disconnect();
    }
    if ( size.getConnectedEngine(eo)) {
	if ( eo == &getAngleAndSizeEngine->outSize )
    	    size.disconnect();
    }

    // BASE CLASS
    GroundedSimpleLink::undoConnections();

    connectionsSetUp = FALSE;
    return !connectionsSetUp;
}

SbBool
Link::setUpConnections(SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( !onOff )
	return undoConnections();

    // BASE CLASS
	GroundedSimpleLink::setUpConnections(onOff, doItAlways);

    // In the base class (SimpleLink) the endPoint is
    // connected from the endPointEngine, so that it depends on
    // size and angle.  In this class, the dependency switches.
    // So we disconnect the endPoint if necessary.
    SoEngineOutput *eo;
    if ( endPoint.getConnectedEngine(eo)) {
	if ( eo == &endPointEngine->outEndPoint )
	    endPoint.disconnect();
    }

    // Now, connect the angle and size to depend on origin and endPoint.
    getAngleAndSizeEngine->inOrigin.connectFrom( &origin );
    getAngleAndSizeEngine->inEndPoint.connectFrom( &endPoint );
    // This may be connected from somewhere else. We don't want
    // to override...
    if ( ! angle.isConnected() )
	angle.connectFrom( &getAngleAndSizeEngine->outAngle );
    if ( ! size.isConnected() )
	size.connectFrom( &getAngleAndSizeEngine->outSize );

    setDraggers( draggersOn.getValue() );

    connectionsSetUp = onOff;
    return !connectionsSetUp;
}
void
Link::setDefaultOnNonWritingFields()
{
    GroundedSimpleLink::setDefaultOnNonWritingFields();
}

void
Link::setDraggers( SbBool on )
{
    // Allow the base class to turn on its draggers...
    GroundedSimpleLink::setDraggers( on );

    setEndPointDragger( on );
}

void
Link::setEndPointDragger( SbBool on )
{
    if ( on == TRUE ) {

	// Create and connect dragger for endPoint
	// But only do it if the field is not connected to anything else.
	// We'll also be setting parts from resources, if we can find them.

	if ( ! endPoint.isConnected() ) {

	    // Set to NULL to make sure we get a new one!
	    setAnyPart("endPointDragger", NULL );
	    SoTranslate2Dragger *xPDrag 
		= (SoTranslate2Dragger *) getAnyPart("endPointDragger", TRUE );

	    // Set the parts based on resources
	    SoNode *part = SoNode::getByName("LINK_POINT_DRAGGER");
	    SoNode *activePart = SoNode::getByName("LINK_POINT_DRAGGER_ACTIVE");
	    if (part != NULL)
		xPDrag->setPartAsDefault("translator", part, FALSE );
	    if (activePart != NULL)
		xPDrag->setPartAsDefault("translatorActive", activePart, FALSE);

	    // Give an initial value
	    if (xPDrag->translation.getValue() != endPoint.getValue())
		xPDrag->translation.setValue(endPoint.getValue());

	    // Connect our endPoint from the dragger.
	    endPoint.connectFrom( &xPDrag->translation );
	}
    }
    else {
	SoTranslate2Dragger *xPDrag 
	    = SO_CHECK_ANY_PART(this, "endPointDragger", SoTranslate2Dragger );
	if ( xPDrag != NULL ) {
	    SoField *f;
	    if ( endPoint.getConnectedField(f) ) {
		if ( f == &xPDrag->translation )
		    endPoint.disconnect();
	    }
	    // Remove the dragger.
	    setAnyPart("endPointDragger", NULL );
	}
    }
}

//
// Description:
//    Destructor (necessary since inline destructor is too complex)
//
// Use: public
//
Link::~Link()
{
    if (getAngleAndSizeEngine)
        getAngleAndSizeEngine->unref();
}

////////////////////////////////////////////////////////////////////
//    Class: SizedLink - subclass of GroundedSimpleLink
//                        Adds a dragger to control the size.
//
//    New nodes in this subclass are:
//      sizeDragger
//
//    New fields in this subclass are:
//         none
//
////////////////////////////////////////////////////////////////////

SO_KIT_SOURCE(SizedLink);

//
// Description:
//    This initializes the SizedLink class.
//
// Use: internal
//
void
SizedLink::initClass()
{
    SO__KIT_INIT_CLASS(SizedLink, "SizedLink", GroundedSimpleLink);
}

//
// Description:
//    Constructor
//
// Use: public
//
SizedLink::SizedLink()
{
    SO_KIT_CONSTRUCTOR(SizedLink);

    isBuiltIn = TRUE;

    // Define new entries to catalog for this class.
    SO_KIT_ADD_CATALOG_ENTRY(sizeDragger, SoTranslate1Dragger, TRUE,
		endPointTranslateSeparator, endPointTranslator, TRUE );


    SO_KIT_INIT_INSTANCE();

    // Add the new fields. - none for this class.

    sizeFromTranslateEngine = new SoDecomposeVec3f;
    sizeFromTranslateEngine->ref();

    setUpConnections( TRUE, TRUE );
}

SbBool
SizedLink::undoConnections()
{
    setDraggers( FALSE );

    // BASE CLASS
    GroundedSimpleLink::undoConnections();

    connectionsSetUp = FALSE;
    return !connectionsSetUp;
}

SbBool
SizedLink::setUpConnections(SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( !onOff )
	return undoConnections();

    // BASE CLASS
	GroundedSimpleLink::setUpConnections(onOff, doItAlways);

    setDraggers( draggersOn.getValue() );

    connectionsSetUp = onOff;
    return !connectionsSetUp;
}
void
SizedLink::setDefaultOnNonWritingFields()
{
    GroundedSimpleLink::setDefaultOnNonWritingFields();
}

void
SizedLink::setDraggers( SbBool on )
{
    // Allow the base class to turn on its draggers...
    GroundedSimpleLink::setDraggers( on );

    setSizeDragger( on );
}

void
SizedLink::setSizeDragger( SbBool on )
{
    if ( on == TRUE ) {

	// Create and connect dragger for size
	// But only do it if the field is not connected to anything else.
	// We'll also be setting parts from resources, if we can find them.

	if ( ! size.isConnected() ) {

	    // Set to NULL to make sure we get a new one!
	    setAnyPart("sizeDragger", NULL );
	    SoTranslate1Dragger *szDrag 
		= (SoTranslate1Dragger *) getAnyPart("sizeDragger", TRUE);

	    // Set the parts based on resources
	    SoNode *part = SoNode::getByName("SIZE_DRAGGER");
	    SoNode *activePart =SoNode::getByName("SIZE_DRAGGER_ACTIVE");
	    if (part != NULL)
		szDrag->setPartAsDefault("translator", part, FALSE );
	    if (activePart != NULL)
		szDrag->setPartAsDefault("translatorActive", activePart,FALSE );

	    // Initialize the dragger with value from the size field
	    if (szDrag->translation.getValue() != SbVec3f(size.getValue(),0,0))
		szDrag->translation.setValue(SbVec3f( size.getValue(), 0, 0 ));

	    // Connect our size field from the dragger.
	    // We need to employ a decompose engine for this.
	    sizeFromTranslateEngine->vector.connectFrom( &szDrag->translation );
	    size.connectFrom( &sizeFromTranslateEngine->x );
	}
    }
    else {
	SoTranslate1Dragger *szDrag 
	    = SO_CHECK_ANY_PART(this, "sizeDragger", SoTranslate1Dragger );
	if ( szDrag != NULL ) {
	    SoEngineOutput *eo;
	    if (size.getConnectedEngine(eo) ) {
		if ( eo == &sizeFromTranslateEngine->x )
		    size.disconnect();
	    }
	    // Remove the dragger.
	    setAnyPart("sizeDragger", NULL );
	}
    }
}

//
// Description:
//    Destructor (necessary since inline destructor is too complex)
//
// Use: public
//
SizedLink::~SizedLink()
{
    if (sizeFromTranslateEngine)
        sizeFromTranslateEngine->unref();
}

////////////////////////////////////////////////////////////////////
//    Class: RivetHinge - subclass of SizedLink
//                        Determines angle from 'origin' and 'hingePoint' 
//                        fields.  Contains dragger to control 'hingePoint'.
//
//    New nodes in this subclass are:
//      hingePointDragger,
//
//    New fields in this subclass are:
//         hingePoint   - location of the rivet hinge
//
////////////////////////////////////////////////////////////////////

SO_KIT_SOURCE(RivetHinge);

//
// Description:
//    This initializes the RivetHinge class.
//
// Use: internal
//
void
RivetHinge::initClass()
{
    SO__KIT_INIT_CLASS(RivetHinge, "RivetHinge", SizedLink);
}

//
// Description:
//    Constructor
//
// Use: public
//
RivetHinge::RivetHinge()
{
    SO_KIT_CONSTRUCTOR(RivetHinge);

    isBuiltIn = TRUE;

    // Define new entries to catalog for this class.
    SO_KIT_ADD_CATALOG_ENTRY(hingePointDragger, SoTranslate2Dragger, TRUE,
				topSeparator, appearance , TRUE );

    // Add the new fields.
    SO_KIT_ADD_FIELD(hingePoint,    (1,0,0));

    SO_KIT_INIT_INSTANCE();

    // Set the parts that we can find as resources...
    setPartAsDefault("originTranslateGeom", "rivetHingeOriginTranslateGeom");
    setPartAsDefault("angleRotateGeom", "rivetHingeAngleRotateGeom");
    setPartAsDefault("endPointTranslateGeom","rivetHingeEndPointTranslateGeom");
    setPartAsDefault("oneDScaleGeom", "rivetHingeOneDScaleGeom");
    setPartAsDefault("twoDScaleGeom", "rivetHingeTwoDScaleGeom");
    setPartAsDefault("threeDScaleGeom", "rivetHingeThreeDScaleGeom");
    setPartAsDefault("material",              "rivetHingeMaterial");

    myRivetEngine = new RivetHingeEngine;
    myRivetEngine->ref();

    setUpConnections( TRUE, TRUE );
}

SbBool
RivetHinge::undoConnections()
{
    setDraggers( FALSE );

    myRivetEngine->inOrigin.disconnect();
    myRivetEngine->inSize.disconnect();
    myRivetEngine->inHingePoint.disconnect();

    // This is a field that might be connected from somewhere other than
    // our own engine. Only disconnect if appropriate.
    SoEngineOutput *eo;
    if ( angle.getConnectedEngine(eo)) {
	if ( eo == &myRivetEngine->outAngle )
    	    angle.disconnect();
    }

    isError.disconnect();

    // BASE CLASS
    SizedLink::undoConnections();

    connectionsSetUp = FALSE;
    return !connectionsSetUp;
}

SbBool
RivetHinge::setUpConnections(SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( !onOff )
	return undoConnections();

    // BASE CLASS
	SizedLink::setUpConnections(onOff, doItAlways);

    myRivetEngine->inOrigin.connectFrom( &origin );
    myRivetEngine->inSize.connectFrom( &size );
    myRivetEngine->inHingePoint.connectFrom( &hingePoint );

    // This may be connected from somewhere else. We don't want
    // to override...
    if ( ! angle.isConnected() )
        angle.connectFrom( &myRivetEngine->outAngle );
    isError.connectFrom( &myRivetEngine->outError );

    setDraggers( draggersOn.getValue() );

    connectionsSetUp = onOff;
    return !connectionsSetUp;
}
void
RivetHinge::setDefaultOnNonWritingFields()
{
    SizedLink::setDefaultOnNonWritingFields();
}

void
RivetHinge::setDraggers( SbBool on )
{
    // Allow the base class to turn on its draggers...
    SizedLink::setDraggers( on );

    setHingePointDragger( on );
}

void
RivetHinge::setHingePointDragger( SbBool on )
{
    if ( on == TRUE ) {

	// Create and connect dragger for hingePoint 
	// But only do it if the field is not connected to anything else.
	// We'll also be setting parts from resources, if we can find them.

	if ( ! hingePoint.isConnected() ) {

		// Set to NULL to make sure we get a new one!
		setAnyPart("hingePointDragger", NULL );
	    SoTranslate2Dragger *hpDrag 
		= (SoTranslate2Dragger *) getAnyPart("hingePointDragger", TRUE);

	    // Set the parts based on resources
	    SoNode *part = SoNode::getByName("HINGE_POINT_DRAGGER");
	    SoNode *activePart =SoNode::getByName("HINGE_POINT_DRAGGER_ACTIVE");
	    if (part != NULL)
		hpDrag->setPartAsDefault("translator", part,FALSE );
	    if (activePart != NULL)
		hpDrag->setPartAsDefault("translatorActive", activePart,FALSE );

	    // Give an initial value
	    if (hpDrag->translation.getValue() != hingePoint.getValue())
		hpDrag->translation.setValue(hingePoint.getValue());

	    // Connect the hinge point from the dragger translation.
	    hingePoint.connectFrom( &hpDrag->translation );
	}
    }
    else {
	SoTranslate2Dragger *hpDrag 
	    = SO_CHECK_ANY_PART(this, "hingePointDragger", SoTranslate2Dragger);
	if ( hpDrag != NULL ) {
	    SoField *f;
	    if ( hingePoint.getConnectedField(f) ) {
		if ( f == &hpDrag->translation )
		    hingePoint.disconnect();
	    }
	    // Remove the dragger.
	    setAnyPart("hingePointDragger", NULL );
	}
    }
}

//
// Description:
//    Destructor (necessary since inline destructor is too complex)
//
// Use: public
//
RivetHinge::~RivetHinge()
{
    if (myRivetEngine)
        myRivetEngine->unref();
}

////////////////////////////////////////////////////////////////////
//
//    Class: Crank - subclass of SizedLink
//                   Adds a dragger to control 'angle'
//
//    New nodes in this subclass are:
//      angleDragger
//
//    New fields in this subclass are:
//      none.
//
////////////////////////////////////////////////////////////////////

SO_KIT_SOURCE(Crank);

//
// Description:
//    This initializes the Crank class.
//
// Use: internal
//
void
Crank::initClass()
{
    SO__KIT_INIT_CLASS(Crank, "Crank", SizedLink);
}

//
// Description:
//    Constructor
//
// Use: public
//
Crank::Crank()
{
    SO_KIT_CONSTRUCTOR(Crank);

    isBuiltIn = TRUE;

    // Define new entries to catalog for this class.
    SO_KIT_ADD_CATALOG_ENTRY(angleDragger, SoRotateDiscDragger, TRUE,
				topSeparator, angleRotator, TRUE );

    SO_KIT_INIT_INSTANCE();

    // Add the new fields. - none for this class

    // Set the parts that we can find as resources...
    setPartAsDefault("originTranslateGeom", "crankOriginTranslateGeom");
    setPartAsDefault("angleRotateGeom", "crankAngleRotateGeom");
    setPartAsDefault("endPointTranslateGeom", "crankEndPointTranslateGeom");
    setPartAsDefault("oneDScaleGeom", "crankOneDScaleGeom");
    setPartAsDefault("twoDScaleGeom", "crankTwoDScaleGeom");
    setPartAsDefault("threeDScaleGeom", "crankThreeDScaleGeom");
    setPartAsDefault("material",              "crankMaterial");

    angleFromRotationEngine = new ZAngleFromRotationEngine;
    angleFromRotationEngine->ref();

    setUpConnections( TRUE, TRUE );
}

SbBool
Crank::undoConnections()
{
    setDraggers( FALSE );

    // BASE CLASS
    SizedLink::undoConnections();

    connectionsSetUp = FALSE;
    return !connectionsSetUp;
}

SbBool
Crank::setUpConnections(SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( !onOff )
	return undoConnections();

    // BASE CLASS
	SizedLink::setUpConnections(onOff, doItAlways);

    setDraggers( draggersOn.getValue() );

    connectionsSetUp = onOff;
    return !connectionsSetUp;
}
void
Crank::setDefaultOnNonWritingFields()
{
    SizedLink::setDefaultOnNonWritingFields();
}

void
Crank::setDraggers( SbBool on )
{
    // Allow the base class to turn on its draggers...
    SizedLink::setDraggers( on );

    setAngleDragger( on );
}

void
Crank::setAngleDragger( SbBool on )
{
    if ( on == TRUE ) {

	// Create and connect dragger for angle
	// But only do it if the field is not connected to anything else.
	// We'll also be setting parts from resources, if we can find them.

	if ( ! angle.isConnected() ) {

	    // Set to NULL to make sure we get a new one!
	    setAnyPart("angleDragger", NULL );
	    SoRotateDiscDragger *angDrag 
		= (SoRotateDiscDragger *) getAnyPart("angleDragger", TRUE);

	    // Set the parts based on resources
	    SoNode *part = SoNode::getByName("ANGLE_DRAGGER");
	    SoNode *activePart =SoNode::getByName("ANGLE_DRAGGER_ACTIVE");
	    if (part != NULL)
		angDrag->setPartAsDefault("rotator", part,FALSE );
	    if (activePart != NULL)
		angDrag->setPartAsDefault("rotatorActive", activePart,FALSE );
	    // Get rid of that nasty axis geometry!
	    angDrag->setPartAsDefault("feedback", new SoSeparator,FALSE  );
	    angDrag->setPartAsDefault("feedbackActive", new SoSeparator,FALSE);

	    // Initialize the dragger with value from the inAngle field
	    SbRotation initRot(SbVec3f(0,0,1), angle.getValue());
	    if (angDrag->rotation.getValue() != initRot)
	        angDrag->rotation.setValue(initRot);

	    // Connect our angle field from the rotation in the crank.
	    // We need to use our engine to convert.
	    angleFromRotationEngine->inRotation.connectFrom(&angDrag->rotation);

	    angle.connectFrom( &angleFromRotationEngine->outAngle );
	}
    }
    else {
	SoRotateDiscDragger *angDrag 
	    = SO_CHECK_ANY_PART(this, "angleDragger", SoRotateDiscDragger );
	if ( angDrag != NULL ) {
	    SoEngineOutput *eo;
	    if (angle.getConnectedEngine(eo) ) {
		if ( eo == &angleFromRotationEngine->outAngle )
		    angle.disconnect();
	    }
	    // Remove the dragger.
	    setAnyPart("angleDragger", NULL );
	}
    }
}

//
// Description:
//    Destructor (necessary since inline destructor is too complex)
//
// Use: public
//
Crank::~Crank()
{
    if (angleFromRotationEngine)
	angleFromRotationEngine->unref();
}

//////////////////////////////////////////////////////////////////////////////
//    Class: DoubleLink
//		a class that contains a two-bar linkage
//
//
//    New nodes in this subclass are:
//      origin1Dragger, origin2Dragger,
//      sharedPointDragger
//      Link1, Link2
//
//    New fields in this subclass are:
//         origin1       - origin of first link
//         origin2       - origin of second link
//         size1         - size of first link
//         size2         - size of second link
//         sharedPoint   - endPoint of both links
//
//////////////////////////////////////////////////////////////////////////////

SO_KIT_SOURCE(DoubleLink);

//
// Description:
//    This initializes the DoubleLink class.
//
// Use: public
//
void
DoubleLink::initClass()
{
    SO__KIT_INIT_CLASS(DoubleLink, "DoubleLink", LinkBase);
}

//
// Description:
//    Constructor
//
// Use: public
//
DoubleLink::DoubleLink()
{
    SO_KIT_CONSTRUCTOR(DoubleLink);

    isBuiltIn = TRUE;

    // Define new entries to catalog for this class.
    SO_KIT_ADD_CATALOG_ENTRY(origin1Dragger, SoTranslate2Dragger, TRUE,
				topSeparator, , TRUE );
    SO_KIT_ADD_CATALOG_ENTRY(origin2Dragger, SoTranslate2Dragger, TRUE,
				topSeparator, , TRUE );
    SO_KIT_ADD_CATALOG_ENTRY(sharedPointDragger, SoTranslate2Dragger, TRUE,
				topSeparator, , TRUE );

    SO_KIT_ADD_CATALOG_ENTRY(link1, Link, TRUE, topSeparator, , TRUE );
    SO_KIT_ADD_CATALOG_ENTRY(link2, Link, TRUE, topSeparator, , TRUE );

    // Add the new fields.
    SO_KIT_ADD_FIELD(origin1,     (0,0,0));
    SO_KIT_ADD_FIELD(origin2,     (2,0,0));
    SO_KIT_ADD_FIELD(size1,       (1));
    SO_KIT_ADD_FIELD(size2,       (1));
    SO_KIT_ADD_FIELD(sharedPoint, (1,0,0));

    SO_KIT_INIT_INSTANCE();

    // Make sure to create the two link parts.
    // (default geometry will be set up in setUpConnections)
    SoNode *l1 = getPart( "link1", TRUE );
    SoNode *l2 = getPart( "link2", TRUE );

    myOriginEngine = new DoubleLinkMoveOriginEngine;
    myOriginEngine->ref();
    mySharedPtEngine = new DoubleLinkMoveSharedPtEngine;
    mySharedPtEngine->ref();

    setPartAsDefault("material",   "doubleLinkMaterial");

    setUpConnections( TRUE, TRUE );
}

SbBool
DoubleLink::undoConnections()
{
    setDraggers( FALSE );

    isError.disconnect();
    sharedPoint.disconnect();

    Link *theLink1 = (Link *) getAnyPart("link1", TRUE );
    Link *theLink2 = (Link *) getAnyPart("link2", TRUE );

    theLink1->origin.disconnect();
    theLink2->origin.disconnect();
    theLink1->endPoint.disconnect();
    theLink2->endPoint.disconnect();

    theLink1->isError.disconnect();
    theLink2->isError.disconnect();

    theLink1->undoConnections();
    theLink2->undoConnections();

    LinkBase::undoConnections();

    connectionsSetUp = FALSE;
    return !connectionsSetUp;
}

SbBool
DoubleLink::setUpConnections(SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( !onOff )
	return undoConnections();

    LinkBase::setUpConnections(onOff, doItAlways);

    Link *theLink1 = (Link *) getAnyPart("link1", TRUE );
    Link *theLink2 = (Link *) getAnyPart("link2", TRUE );

    // Set parts within the two links if they haven't already been
    // set to a non-default value...
    SoNode *part;
    part = SoNode::getByName("doubleLinkOriginTranslateGeom");
    theLink1->setPartAsDefault("originTranslateGeom", part); 
    theLink2->setPartAsDefault("originTranslateGeom", part); 
    part = SoNode::getByName("doubleLinkAngleRotateGeom");
    theLink1->setPartAsDefault("angleRotateGeom", part); 
    theLink2->setPartAsDefault("angleRotateGeom", part); 
    part = SoNode::getByName("doubleLinkEndPointTranslateGeom");
    theLink1->setPartAsDefault("endPointTranslateGeom", part); 
    theLink2->setPartAsDefault("endPointTranslateGeom", part); 
    part = SoNode::getByName("doubleLinkOneDScaleGeom");
    theLink1->setPartAsDefault("oneDScaleGeom", part); 
    theLink2->setPartAsDefault("oneDScaleGeom", part); 
    part = SoNode::getByName("doubleLinkTwoDScaleGeom");
    theLink1->setPartAsDefault("twoDScaleGeom", part); 
    theLink2->setPartAsDefault("twoDScaleGeom", part); 
    part = SoNode::getByName("doubleLinkThreeDScaleGeom");
    theLink1->setPartAsDefault("threeDScaleGeom", part); 
    theLink2->setPartAsDefault("threeDScaleGeom", part); 
    part = SoNode::getByName("doubleLinkMaterial");
    theLink1->setPartAsDefault("material", part); 
    theLink2->setPartAsDefault("material", part); 

    theLink1->setUpConnections(onOff, doItAlways );
    theLink2->setUpConnections(onOff, doItAlways );

    theLink1->origin.connectFrom( &origin1 );
    theLink2->origin.connectFrom( &origin2 );
    theLink1->endPoint.connectFrom( &sharedPoint );
    theLink2->endPoint.connectFrom( &sharedPoint );
    theLink1->isError.disconnect();
    theLink2->isError.disconnect();

    // Attach connections for the  DoubleLinkMoveOriginEngine
    // This engine moves the shared point based on the origins and sized of 
    // the two links.
    // When the sharedPointDragger is being dragger, this engine is disconnected
    // and a different engine is used.  This second engine leaves the origins
    // where they are, but changes the size of the links so they are big enough
    // to reach the shared point.
    // So, the default engine keeps the links the same size and seems to work
    // as if we are using the mechanism of the double-link.
    // While the other engine seems to alter the physical make-up (i.e.,lengths)
    // of the pieces.
    myOriginEngine->inOrigin1.connectFrom( &origin1 ); 
    myOriginEngine->inOrigin2.connectFrom( &origin2 ); 
    myOriginEngine->inSize1.connectFrom( &size1 ); 
    myOriginEngine->inSize2.connectFrom( &size2 ); 
    myOriginEngine->inSharedPoint.connectFrom( &sharedPoint ); 
    sharedPoint.connectFrom( &myOriginEngine->outSharedPoint );
    isError.connectFrom( &myOriginEngine->outError );


    setDraggers( draggersOn.getValue() );

    connectionsSetUp = onOff;
    return !connectionsSetUp;
}
void
DoubleLink::setDefaultOnNonWritingFields()
{
    // We'd rather not write out these nodes, since their parameters
    // should be reflected by the DoubleLink parameters.
    link1.setDefault(TRUE);
    link2.setDefault(TRUE);

    // We'll even go so far as to set the fields of the sub-links to 
    // default.  So they will only write out if the geometry has been altered.
    Link *theLink1 = (Link *) getAnyPart("link1", TRUE );
    if (theLink1 ) {
	theLink1->isError.setDefault(TRUE);
	theLink1->origin.setDefault(TRUE);
	theLink1->angle.setDefault(TRUE);
	theLink1->size.setDefault(TRUE);
	theLink1->endPoint.setDefault(TRUE);
    }
    Link *theLink2 = (Link *) getAnyPart("link2", TRUE );
    if (theLink2 ) {
	theLink2->isError.setDefault(TRUE);
	theLink2->origin.setDefault(TRUE);
	theLink2->angle.setDefault(TRUE);
	theLink2->size.setDefault(TRUE);
	theLink2->endPoint.setDefault(TRUE);
    }

    LinkBase::setDefaultOnNonWritingFields();
}

void
DoubleLink::setDraggers( SbBool on )
{
    // Turn OFF the draggers in the two links owned by the double link...
    // We have our own draggers to use instead.
	Link *theLink1 = (Link *) getAnyPart("link1", TRUE );
	Link *theLink2 = (Link *) getAnyPart("link2", TRUE );

	theLink1->draggersOn.setValue( FALSE );
	theLink2->draggersOn.setValue( FALSE );

    // Allow the base class to turn on its draggers...
	LinkBase::setDraggers( on );

    // Turn on our special draggers.
	setOrigin1Dragger(on);
	setOrigin2Dragger(on);
	setSharedPointDragger(on);
}

void
DoubleLink::setOrigin1Dragger( SbBool on )
{
    if ( on == TRUE ) {

	// Create and connect draggers if necessary...

	if ( ! origin1.isConnected() ) {

	    // Set to NULL to make sure we get a new one!
	    setAnyPart("origin1Dragger", NULL );
	    SoTranslate2Dragger *or1Drag 
		= (SoTranslate2Dragger *) getAnyPart("origin1Dragger", TRUE);

	    // Set the parts based on resources
	    SoNode *part = SoNode::getByName("LINK_POINT_DRAGGER");
	    SoNode *activePart =SoNode::getByName("LINK_POINT_DRAGGER_ACTIVE");
	    if (part != NULL)
		or1Drag->setPartAsDefault("translator", part, FALSE );
	    if (activePart != NULL)
		or1Drag->setPartAsDefault("translatorActive", activePart,FALSE);

	    // Initialize the dragger with value from the origin1 field
	    if (or1Drag->translation.getValue() != origin1.getValue())
	        or1Drag->translation.setValue(origin1.getValue());

	    // Connect our origin field from the output of this dragger.
	    origin1.connectFrom( &or1Drag->translation );
	}
    }
    else {
	SoTranslate2Dragger *or1Drag 
	    = SO_CHECK_ANY_PART(this, "origin1Dragger", SoTranslate2Dragger );
	if ( or1Drag != NULL ) {
	    SoField *f;
	    if ( origin1.getConnectedField(f) ) {
		if ( f == &or1Drag->translation )
		    origin1.disconnect();
	    }
	    // Remove the dragger.
	    setAnyPart("origin1Dragger", NULL );
	}
    }
}

void
DoubleLink::setOrigin2Dragger( SbBool on )
{
    if ( on == TRUE ) {

	// Create and connect draggers if necessary...

	if ( ! origin2.isConnected() ) {

	    // Set to NULL to make sure we get a new one!
	    setAnyPart("origin2Dragger", NULL );
	    SoTranslate2Dragger *or2Drag 
		= (SoTranslate2Dragger *) getAnyPart("origin2Dragger", TRUE);

	    // Set the parts based on resources
	    SoNode *part = SoNode::getByName("LINK_POINT_DRAGGER");
	    SoNode *activePart =SoNode::getByName("LINK_POINT_DRAGGER_ACTIVE");
	    if (part != NULL)
		or2Drag->setPartAsDefault("translator", part,FALSE );
	    if (activePart != NULL)
		or2Drag->setPartAsDefault("translatorActive", activePart,FALSE);

	    // Initialize the dragger with value from the origin2 field
	    if (or2Drag->translation.getValue() != origin2.getValue())
	        or2Drag->translation.setValue(origin2.getValue());

	    // Connect our origin field from the output of this dragger.
	    origin2.connectFrom( &or2Drag->translation );
	}
    }
    else {
	SoTranslate2Dragger *or2Drag 
	    = SO_CHECK_ANY_PART(this, "origin2Dragger", SoTranslate2Dragger );
	if ( or2Drag != NULL ) {
	    SoField *f;
	    if ( origin2.getConnectedField(f) ) {
		if ( f == &or2Drag->translation )
		    origin2.disconnect();
	    }
	    // Remove the dragger.
	    setAnyPart("origin2Dragger", NULL );
	}
    }
}
void
DoubleLink::setSharedPointDragger( SbBool on )
{
    if ( on == TRUE ) {

	// Create and connect draggers if necessary...

	SoEngineOutput *eo;
	// If the shared point is either not connected or connected to our
	// internal engine...
	if ( !sharedPoint.isConnected() 
	    || (sharedPoint.getConnectedEngine(eo) && 
		eo == &myOriginEngine->outSharedPoint )) {

	    // Set to NULL to make sure we get a new one!
	    setAnyPart("sharedPointDragger", NULL );
	    SoTranslate2Dragger *spDrag 
		= (SoTranslate2Dragger *) getAnyPart("sharedPointDragger",TRUE);

	    // Set the parts based on resources
	    SoNode *part = SoNode::getByName("LINK_POINT_DRAGGER");
	    SoNode *activePart =SoNode::getByName("LINK_POINT_DRAGGER_ACTIVE");
	    if (part != NULL)
		spDrag->setPartAsDefault("translator", part, FALSE );
	    if (activePart != NULL)
		spDrag->setPartAsDefault("translatorActive", activePart, FALSE);

	    // Make the sharedPt dragger follow the shared point.
	    // When the dragger is actually in use, the dragger will lead
	    // rather than follow.  The sharedPtDragStartCB sees to this.
	    spDrag->translation.connectFrom( &sharedPoint );

	    // Do NOT connect our sharedPoint field from output of this dragger.
	    // By default, sharedPoint is always calculated based on the
	    // two origins and sizes.
	    // We we ONLY connect the field while the 
	    // dragger is in use. The start and end callbacks will connect
	    // and disconnect the field for us.
	    spDrag->addStartCallback(  &DoubleLink::sharedPtDragStartCB, this );
	    spDrag->addFinishCallback( &DoubleLink::sharedPtDragFinishCB, this);
	}
    }
    else {
	SoTranslate2Dragger *spDrag 
	    = SO_CHECK_ANY_PART(this,"sharedPointDragger",SoTranslate2Dragger );
	if ( spDrag != NULL ) {
	    SoField *f;
	    if ( sharedPoint.getConnectedField(f) ) {
		if ( f == &spDrag->translation )
		    sharedPoint.disconnect();
	    }
	    // Remove the dragger.
	    setAnyPart("sharedPointDragger", NULL );
	}
    }
}

// Re-configures the fields to calculate sizes based on fixed origins
// and a moving shared point.
// This only is used while the sharedPointDragger is being dragged.
void 
DoubleLink::sharedPtDragStartCB( void *mePtr, SoDragger *dragPtr )
{
    SoTranslate2Dragger *spDragger = (SoTranslate2Dragger *) dragPtr;

    DoubleLink *myself = (DoubleLink *) mePtr;

    myself->size1.disconnect();
    myself->size2.disconnect();

    // Make the sharedPt follow the shared point dragger.
    myself->sharedPoint.connectFrom( &spDragger->translation);

    // Attach connections for the  DoubleLinkMoveSharedPtEngine
    myself->mySharedPtEngine->inOrigin1.connectFrom( &myself->origin1 ); 
    myself->mySharedPtEngine->inOrigin2.connectFrom( &myself->origin2 ); 
    myself->mySharedPtEngine->inSharedPoint.connectFrom( &myself->sharedPoint ); 
    myself->size1.connectFrom( &myself->mySharedPtEngine->outSize1 );
    myself->size2.connectFrom( &myself->mySharedPtEngine->outSize2 );
    myself->isError.connectFrom( &myself->mySharedPtEngine->outError );
}

// Returns fields to default configuration where sharedPoint is calculated
// based on two origins and two sizes.
// We do this because we are finished dragging the sharedPoint.
void 
DoubleLink::sharedPtDragFinishCB( void *mePtr, SoDragger * )
{
    DoubleLink *myself = (DoubleLink *) mePtr;

    myself->sharedPoint.disconnect();
    myself->size1.disconnect();
    myself->size2.disconnect();

    // Attach connections for the  DoubleLinkMoveOriginEngine
    myself->myOriginEngine->inOrigin1.connectFrom( &myself->origin1 ); 
    myself->myOriginEngine->inOrigin2.connectFrom( &myself->origin2 ); 
    myself->myOriginEngine->inSize1.connectFrom( &myself->size1 ); 
    myself->myOriginEngine->inSize2.connectFrom( &myself->size2 ); 
    myself->myOriginEngine->inSharedPoint.connectFrom( &myself->sharedPoint ); 

    myself->sharedPoint.connectFrom( &myself->myOriginEngine->outSharedPoint );
    myself->isError.connectFrom( &myself->myOriginEngine->outError );

    // Make the sharedPt dragger follow the shared point.
    SoTranslate2Dragger *spDrag 
	= (SoTranslate2Dragger *) myself->getAnyPart("sharedPointDragger",TRUE);
    spDrag->translation.connectFrom( &myself->sharedPoint );
}

void 
DoubleLink::errorColor( SbBool useErrorColor )
{
    LinkBase::errorColor( useErrorColor );

    Link *theLink1 = (Link *) getAnyPart("link1", TRUE );
    Link *theLink2 = (Link *) getAnyPart("link2", TRUE );

    theLink1->isError.setValue( isError.getValue() );
    theLink2->isError.setValue( isError.getValue() );
}

//
// Description:
//    Destructor (necessary since inline destructor is too complex)
//
// Use: public
//
DoubleLink::~DoubleLink()
{
    if (myOriginEngine)
	myOriginEngine->unref();
    if (mySharedPtEngine)
	mySharedPtEngine->unref();
}

////////////////////////////////////////////////////////////////////
//    Class: Piston
//
//    contains two RivetHinge nodes, and hooks up its own fields to the
//    fields of the RivetHinges
//
//    New nodes in this subclass are:
//      origin1Dragger, origin2Dragger
//      link1, link2
//
//    New fields in this subclass are:
//         origin1         - location of first link's fixed point
//         origin2         - location of second link's fixed point
//         size1      - size of the first link
//         size2      - size of the second link
//
////////////////////////////////////////////////////////////////////

SO_KIT_SOURCE(Piston);

//
// Description:
//    This initializes the Piston class.
//
// Use: internal
//
void
Piston::initClass()
{
    SO__KIT_INIT_CLASS(Piston, "Piston", LinkBase);
}

//
// Description:
//    Constructor
//
// Use: public
//
Piston::Piston()
{
    SO_KIT_CONSTRUCTOR(Piston);

    isBuiltIn = TRUE;

    // Define new entries to catalog for this class.
    SO_KIT_ADD_CATALOG_ENTRY(origin1Dragger, SoTranslate2Dragger, TRUE,
				topSeparator, , TRUE );
    SO_KIT_ADD_CATALOG_ENTRY(origin2Dragger, SoTranslate2Dragger, TRUE,
				topSeparator, , TRUE );
    SO_KIT_ADD_CATALOG_ENTRY(link1, RivetHinge, TRUE, topSeparator, , TRUE );
    SO_KIT_ADD_CATALOG_ENTRY(link2, RivetHinge, TRUE, topSeparator, , TRUE );

    // Add the new fields.
    SO_KIT_ADD_FIELD(origin1,      (0,0,0));
    SO_KIT_ADD_FIELD(origin2,    (1,0,0));
    SO_KIT_ADD_FIELD(size1,       (1.0));
    SO_KIT_ADD_FIELD(size2,       (0.5));

    SO_KIT_INIT_INSTANCE();

    // Make sure to create the two link parts.
    // (default geometry will be set up in setUpConnections)
    SoNode *l1 = getPart( "link1", TRUE );
    SoNode *l2 = getPart( "link2", TRUE );

    setPartAsDefault("material", "pistonMaterial"); 

    myPistonErrorEngine = new PistonErrorEngine;
    myPistonErrorEngine->ref();

    setUpConnections( TRUE, TRUE );
}

SbBool
Piston::undoConnections()
{
    setDraggers( FALSE );

    myPistonErrorEngine->inOrigin1.disconnect();
    myPistonErrorEngine->inOrigin2.disconnect();
    myPistonErrorEngine->inSize1.disconnect();
    myPistonErrorEngine->inSize2.disconnect();

    isError.disconnect();

    RivetHinge *rivet1 = (RivetHinge *) getAnyPart("link1", TRUE );
    RivetHinge *rivet2 = (RivetHinge *) getAnyPart("link2", TRUE );

    rivet1->origin.disconnect();
    rivet2->origin.disconnect();

    rivet1->hingePoint.disconnect();
    rivet2->hingePoint.disconnect();

    rivet1->size.disconnect();
    rivet2->size.disconnect();

    rivet1->isError.disconnect();
    rivet2->isError.disconnect();

    rivet1->undoConnections();
    rivet2->undoConnections();

    LinkBase::undoConnections();

    connectionsSetUp = FALSE;
    return !connectionsSetUp;
}

SbBool
Piston::setUpConnections(SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( !onOff )
	return undoConnections();

    LinkBase::setUpConnections(onOff, doItAlways);

    RivetHinge *rivet1 = (RivetHinge *) getAnyPart("link1", TRUE );
    RivetHinge *rivet2 = (RivetHinge *) getAnyPart("link2", TRUE );

    // Set the parts that we can find as resources...
    SoNode *part;
    part = SoNode::getByName("pistonOriginTranslateGeom");
    rivet1->setPartAsDefault("originTranslateGeom", part); 
    rivet2->setPartAsDefault("originTranslateGeom", part); 
    part = SoNode::getByName("pistonAngleRotateGeom");
    rivet1->setPartAsDefault("angleRotateGeom", part); 
    rivet2->setPartAsDefault("angleRotateGeom", part); 
    part = SoNode::getByName("pistonEndPointTranslateGeom");
    rivet1->setPartAsDefault("endPointTranslateGeom", part); 
    rivet2->setPartAsDefault("endPointTranslateGeom", part); 
    part = SoNode::getByName("pistonOneDScaleGeom");
    rivet1->setPartAsDefault("oneDScaleGeom", part); 
    rivet2->setPartAsDefault("oneDScaleGeom", part); 
    part = SoNode::getByName("pistonTwoDScaleGeom");
    rivet1->setPartAsDefault("twoDScaleGeom", part); 
    rivet2->setPartAsDefault("twoDScaleGeom", part); 
    part = SoNode::getByName("pistonThreeDScaleGeom");
    rivet1->setPartAsDefault("threeDScaleGeom", part); 
    rivet2->setPartAsDefault("threeDScaleGeom", part); 
    part = SoNode::getByName("pistonMaterial");
    rivet1->setPartAsDefault("material", part); 
    rivet1->setPartAsDefault("material", part); 

    rivet1->setUpConnections(onOff, doItAlways );
    rivet2->setUpConnections(onOff, doItAlways );

    rivet1->origin.connectFrom( &origin1 );
    rivet2->origin.connectFrom( &origin2 );

    rivet1->hingePoint.connectFrom( &origin2 );
    rivet2->hingePoint.connectFrom( &origin1 );

    rivet1->size.connectFrom( &size1 );
    rivet2->size.connectFrom( &size2 );

    rivet1->isError.disconnect();
    rivet2->isError.disconnect();

    myPistonErrorEngine->inOrigin1.connectFrom( &origin1 );
    myPistonErrorEngine->inOrigin2.connectFrom( &origin2 );
    myPistonErrorEngine->inSize1.connectFrom( &size1 );
    myPistonErrorEngine->inSize2.connectFrom( &size2 );

    isError.connectFrom( &myPistonErrorEngine->outError );

    setDraggers( draggersOn.getValue() );

    connectionsSetUp = onOff;
    return !connectionsSetUp;
}
void
Piston::setDefaultOnNonWritingFields()
{
    // We'd rather not write out these nodes, since their parameters
    // should be reflected by the DoubleLink parameters.
    link1.setDefault(TRUE);
    link2.setDefault(TRUE);

    // We'll even go so far as to set the fields of the sub-links to 
    // default.  So they will only write out if the geometry has been altered.
    RivetHinge *rivet1 = (RivetHinge *) getAnyPart("link1", TRUE );
    if (rivet1 ) {
	rivet1->isError.setDefault(TRUE);
	rivet1->origin.setDefault(TRUE);
	rivet1->angle.setDefault(TRUE);
	rivet1->size.setDefault(TRUE);
	rivet1->endPoint.setDefault(TRUE);
	rivet1->hingePoint.setDefault(TRUE);
    }
    RivetHinge *rivet2 = (RivetHinge *) getAnyPart("link2", TRUE );
    if (rivet2 ) {
	rivet2->isError.setDefault(TRUE);
	rivet2->origin.setDefault(TRUE);
	rivet2->angle.setDefault(TRUE);
	rivet2->size.setDefault(TRUE);
	rivet2->endPoint.setDefault(TRUE);
	rivet2->hingePoint.setDefault(TRUE);
    }

    LinkBase::setDefaultOnNonWritingFields();
}

void
Piston::setDraggers( SbBool on )
{
    // Turn OFF the draggers in the two links owned by the double link...
    // We have our own draggers to use instead.
	RivetHinge *rivet1 = (RivetHinge *) getAnyPart("link1", TRUE );
	RivetHinge *rivet2 = (RivetHinge *) getAnyPart("link2", TRUE );

	rivet1->draggersOn.setValue( FALSE );
	rivet2->draggersOn.setValue( FALSE );


    // Allow the base class to turn on its draggers...
	LinkBase::setDraggers( on );

    // Turn on our special draggers.
	setOrigin1Dragger(on);
	setOrigin2Dragger(on);
}
void
Piston::setOrigin1Dragger( SbBool on )
{
    if ( on == TRUE ) {

	// Create and connect draggers if necessary...

	if ( ! origin1.isConnected() ) {

	    // Set to NULL to make sure we get a new one!
	    setAnyPart("origin1Dragger", NULL );
	    SoTranslate2Dragger *or1Drag 
		= (SoTranslate2Dragger *) getAnyPart("origin1Dragger", TRUE);

	    // Set the parts based on resources
	    SoNode *part = SoNode::getByName("LINK_POINT_DRAGGER");
	    SoNode *activePart =SoNode::getByName("LINK_POINT_DRAGGER_ACTIVE");
	    if (part != NULL)
		or1Drag->setPartAsDefault("translator", part, FALSE );
	    if (activePart != NULL)
		or1Drag->setPartAsDefault("translatorActive", activePart,FALSE);

	    // Initialize the dragger with value from the origin1 field
	    if (or1Drag->translation.getValue() != origin1.getValue())
		or1Drag->translation.setValue(origin1.getValue());

	    // connect from dragger
	    origin1.connectFrom( &or1Drag->translation );
	}
    }
    else {
	SoTranslate2Dragger *or1Drag 
	    = SO_CHECK_ANY_PART(this, "origin1Dragger", SoTranslate2Dragger );
	if ( or1Drag != NULL ) {
	    SoField *f;
	    if ( origin1.getConnectedField(f) ) {
		if ( f == &or1Drag->translation )
		    origin1.disconnect();
	    }
	    // Remove the dragger.
	    setAnyPart("origin1Dragger", NULL );
	}
    }
}
void
Piston::setOrigin2Dragger( SbBool on )
{
    if ( on == TRUE ) {

	// Create and connect draggers if necessary...

	if ( ! origin2.isConnected() ) {

	    // Set to NULL to make sure we get a new one!
	    setAnyPart("origin2Dragger", NULL );
	    SoTranslate2Dragger *or2Drag 
		= (SoTranslate2Dragger *) getAnyPart("origin2Dragger", TRUE);

	    // Set the parts based on resources
	    SoNode *part = SoNode::getByName("LINK_POINT_DRAGGER");
	    SoNode *activePart =SoNode::getByName("LINK_POINT_DRAGGER_ACTIVE");
	    if (part != NULL)
		or2Drag->setPartAsDefault("translator", part, FALSE );
	    if (activePart != NULL)
		or2Drag->setPartAsDefault("translatorActive", activePart,FALSE);

	    // Initialize the dragger with value from the origin2 field
	    if (or2Drag->translation.getValue() !=origin2.getValue())
		or2Drag->translation.setValue(origin2.getValue());

	    // connect from dragger
	    origin2.connectFrom( &or2Drag->translation );
	}
    }
    else {
	SoTranslate2Dragger *or2Drag 
	    = SO_CHECK_ANY_PART(this, "origin2Dragger", SoTranslate2Dragger );
	if ( or2Drag != NULL ) {
	    SoField *f;
	    if ( origin2.getConnectedField(f) ) {
		if ( f == &or2Drag->translation )
		    origin2.disconnect();
	    }
	    // Remove the dragger.
	    setAnyPart("origin2Dragger", NULL );
	}
    }
}

void 
Piston::errorColor( SbBool useErrorColor )
{
    LinkBase::errorColor( useErrorColor );

    RivetHinge *rivet1 = (RivetHinge *) getAnyPart("link1", TRUE );
    RivetHinge *rivet2 = (RivetHinge *) getAnyPart("link2", TRUE );

    rivet1->isError.setValue( isError.getValue() );
    rivet2->isError.setValue( isError.getValue() );
}

//
// Description:
//    Destructor (necessary since inline destructor is too complex)
//
// Use: public
//
Piston::~Piston()
{
    if (myPistonErrorEngine)
        myPistonErrorEngine->unref();
}





SO_KIT_SOURCE(Button);

void
Button::initClass()
{
    SO__KIT_INIT_CLASS(Button, "Button", SoDragger );
}

Button::Button()
{
    SO_KIT_CONSTRUCTOR(Button);

    isBuiltIn = TRUE;

    SO_KIT_ADD_CATALOG_ENTRY(buttonSwitch, SoSwitch, TRUE,
			    geomSeparator, ,FALSE);

    SO_KIT_ADD_CATALOG_ENTRY(buttonSep, SoSeparator, TRUE,
			    buttonSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(buttonGeom, SoSeparator, TRUE,
			    buttonSep, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(buttonText, SoSeparator, TRUE,
			    buttonSep, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(buttonActiveSep, SoSeparator, TRUE,
			    buttonSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(buttonActiveGeom, SoSeparator, TRUE,
			    buttonActiveSep, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(buttonActiveText, SoSeparator, TRUE,
			    buttonActiveSep, ,TRUE);

    SO_KIT_INIT_INSTANCE();

    setPartAsDefault("buttonGeom",       "buttonButtonGeom");
    setPartAsDefault("buttonText",       "buttonButtonText");
    setPartAsDefault("buttonActiveGeom", "buttonButtonActiveGeom");
    setPartAsDefault("buttonActiveText", "buttonButtonActiveText");

    setSwitchValue(buttonSwitch.getValue(), 0 );

    addStartCallback( &Button::startCB );
    addFinishCallback( &Button::finishCB );
}

Button::~Button()
{
}

void 
Button::dragStart()
{
    setSwitchValue( buttonSwitch.getValue(), 1 );
}

void 
Button::dragFinish()
{
    setSwitchValue( buttonSwitch.getValue(), 0 );
}

void
Button::startCB(void *, SoDragger *inDragger )
{
    Button *b = (Button *) inDragger;
    b->dragStart();
}
void
Button::finishCB(void *, SoDragger *inDragger )
{
    Button *b = (Button *) inDragger;
    b->dragFinish();
}
