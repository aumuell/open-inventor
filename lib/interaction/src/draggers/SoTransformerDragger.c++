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
 |   $Revision: 1.2 $
 |
 |   Classes:
 |	SoTransformerDragger
 |
 |   Author(s): Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <stdio.h>
#include <Inventor/SoDB.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include <Inventor/events/SoEvent.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/projectors/SbPlaneProjector.h>
#include <Inventor/projectors/SbLineProjector.h>
#include <Inventor/projectors/SbCylinderPlaneProjector.h>
#include <Inventor/projectors/SbSphereSectionProjector.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSurroundScale.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>

#include <Inventor/nodes/SoAntiSquish.h>
#include <Inventor/nodes/SoLocateHighlight.h>
#include <Inventor/draggers/SoTransformerDragger.h>
#include "geom/SoTransformerDraggerGeom.h"

#ifndef __sgi
#define _ABS(x) ((x) < 0 ? -(x) : (x))
#endif // !__sgi

int    SoTransformerDragger::colinearThreshold = 4;

SO_KIT_SOURCE(SoTransformerDragger);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
SoTransformerDragger::SoTransformerDragger()
//
////////////////////////////////////////////////////////////////////////
{
    SO_KIT_CONSTRUCTOR(SoTransformerDragger);

    isBuiltIn = TRUE;

    // Broke this down so contructor isn't big and compiler doesn't gripe
    makeCatalog();


    // read geometry for shared parts
    if (SO_KIT_IS_FIRST_INSTANCE())
	readDefaultParts("transformerDragger.iv", geomBuffer, sizeof(geomBuffer));

    SO_KIT_ADD_FIELD(translation, (0.0, 0.0, 0.0));
    SO_KIT_ADD_FIELD(scaleFactor, (1.0, 1.0, 1.0));
    SO_KIT_ADD_FIELD(rotation,    (0.0, 0.0, 0.0, 1.0));
    SO_KIT_ADD_FIELD(minDiscRotDot, (0.025));

    SO_KIT_INIT_INSTANCE();

    // Finds and sets default geometry for all parts defined in this
    // class.
    setAllDefaultParts();

    setSwitchValue(  xAxisFeedbackSwitch.getValue(),          SO_SWITCH_NONE );
    setSwitchValue(  yAxisFeedbackSwitch.getValue(),          SO_SWITCH_NONE );
    setSwitchValue(  zAxisFeedbackSwitch.getValue(),          SO_SWITCH_NONE );
    setSwitchValue(translateBoxFeedbackSwitch.getValue(),          SO_SWITCH_NONE );
    setSwitchValue(scaleBoxFeedbackSwitch.getValue(),          SO_SWITCH_NONE );
    setSwitchValue(posXWallFeedbackSwitch.getValue(),          SO_SWITCH_NONE );
    setSwitchValue(posYWallFeedbackSwitch.getValue(),          SO_SWITCH_NONE );
    setSwitchValue(posZWallFeedbackSwitch.getValue(),          SO_SWITCH_NONE );
    setSwitchValue(negXWallFeedbackSwitch.getValue(),          SO_SWITCH_NONE );
    setSwitchValue(negYWallFeedbackSwitch.getValue(),          SO_SWITCH_NONE );
    setSwitchValue(negZWallFeedbackSwitch.getValue(),          SO_SWITCH_NONE );
    setSwitchValue( radialFeedbackSwitch.getValue(),          SO_SWITCH_NONE );
    setSwitchValue(circleFeedbackTransformSwitch.getValue(), SO_SWITCH_NONE );
    setSwitchValue(xCircleFeedbackSwitch.getValue(),          SO_SWITCH_NONE );
    setSwitchValue(yCircleFeedbackSwitch.getValue(),          SO_SWITCH_NONE );
    setSwitchValue(zCircleFeedbackSwitch.getValue(),          SO_SWITCH_NONE );

    planeProj = new SbPlaneProjector;
    lineProj = new SbLineProjector;
    sphereProj = new SbSphereSectionProjector( 0.85 );
    cylProj = new SbCylinderPlaneProjector();

    currentState = INACTIVE;
    restartState = INACTIVE;

    // add the callbacks to perform the dragging.
    addStartCallback(  &SoTransformerDragger::startCB );
    addMotionCallback( &SoTransformerDragger::motionCB );
    addFinishCallback(   &SoTransformerDragger::finishCB );

    // add the callback to update things each time a meta key changes.
    addOtherEventCallback(   &SoTransformerDragger::metaKeyChangeCB );

    // Update the rotation and scale fields when the motionMatrix is set.
    addValueChangedCallback( &SoTransformerDragger::valueChangedCB );

    // Updates the motionMatrix when the translationFactor field is set.
    translFieldSensor 
        = new SoFieldSensor( &SoTransformerDragger::fieldSensorCB, this);
    translFieldSensor->setPriority( 0 );

    // Updates the motionMatrix when the scaleFactor field is set.
    scaleFieldSensor 
	= new SoFieldSensor( &SoTransformerDragger::fieldSensorCB, this);
    scaleFieldSensor->setPriority( 0 );

    // Updates the motionMatrix when the rotation field is set.
    rotateFieldSensor 
	= new SoFieldSensor( &SoTransformerDragger::fieldSensorCB, this);
    rotateFieldSensor->setPriority( 0 );

    setUpConnections( TRUE, TRUE );

    // Empirical testing shows better results with this many pixels
    // before selecting gesture
	setMinGesture( 15 );

    locateHighlightOn = TRUE;
}

void
SoTransformerDragger::makeCatalog()
{
    // Don't create this by default. It's only really put into use
    // if this dragger is put inside a manipulator.
    SO_KIT_ADD_CATALOG_ENTRY(surroundScale, SoSurroundScale, 
				TRUE, topSeparator, geomSeparator,TRUE);

    // These parts will all go under the geomSeparator, for efficient 
    // rendering
    SO_KIT_ADD_CATALOG_ENTRY(overallStyle, SoGroup, TRUE,
				topSeparator, geomSeparator, FALSE );

    SO_KIT_ADD_CATALOG_ENTRY(translatorSep, SoSeparator, TRUE,
				topSeparator, ,FALSE);

    // ORDER OF THESE IS IMPORTANT!
    // This is because all commands in this routine and subroutines
    // add parts to catalog in an order that depends on which parts are
    // already added.

    makeTranslaterCatalogParts();
    makeRotaterCatalogParts();
    makeScalerCatalogParts();

    makeAxisFeedbackCatalogParts();
    makeBoxFeedbackCatalogParts();
    makeWallFeedbackCatalogParts();
    makeRadialFeedbackCatalogParts();
    makeCircleFeedbackCatalogParts();
}

void 
SoTransformerDragger::makeTranslaterCatalogParts()
{
    SO_KIT_ADD_CATALOG_ENTRY(translator1Switch, SoSwitch, TRUE,
				translatorSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(translator1LocateGroup,SoLocateHighlight, TRUE,
				translator1Switch, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(translator1, SoSeparator, TRUE,
				translator1LocateGroup, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(translator1Active, SoSeparator, TRUE,
				translator1Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(translator2Switch, SoSwitch, TRUE,
				translatorSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(translator2LocateGroup,SoLocateHighlight, TRUE,
				translator2Switch, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(translator2, SoSeparator, TRUE,
				translator2LocateGroup, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(translator2Active, SoSeparator, TRUE,
				translator2Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(translator3Switch, SoSwitch, TRUE,
				translatorSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(translator3LocateGroup,SoLocateHighlight, TRUE,
				translator3Switch, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(translator3, SoSeparator, TRUE,
				translator3LocateGroup, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(translator3Active, SoSeparator, TRUE,
				translator3Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(translator4Switch, SoSwitch, TRUE,
				translatorSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(translator4LocateGroup,SoLocateHighlight, TRUE,
				translator4Switch, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(translator4, SoSeparator, TRUE,
				translator4LocateGroup, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(translator4Active, SoSeparator, TRUE,
				translator4Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(translator5Switch, SoSwitch, TRUE,
				translatorSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(translator5LocateGroup,SoLocateHighlight, TRUE,
				translator5Switch, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(translator5, SoSeparator, TRUE,
				translator5LocateGroup, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(translator5Active, SoSeparator, TRUE,
				translator5Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(translator6Switch, SoSwitch, TRUE,
				translatorSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(translator6LocateGroup,SoLocateHighlight, TRUE,
				translator6Switch, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(translator6, SoSeparator, TRUE,
				translator6LocateGroup, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(translator6Active, SoSeparator, TRUE,
				translator6Switch, ,TRUE);
}

void
SoTransformerDragger::makeRotaterCatalogParts()
{
    SO_KIT_ADD_CATALOG_ENTRY(rotatorSep, SoSeparator, TRUE,
				topSeparator, ,FALSE);

    SO_KIT_ADD_CATALOG_ENTRY(rotator1Switch, SoSwitch, TRUE,
				rotatorSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(rotator1LocateGroup, SoLocateHighlight, TRUE,
				rotator1Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(rotator1, SoSeparator, TRUE,
				rotator1LocateGroup, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(rotator1Active, SoSeparator, TRUE,
				rotator1Switch, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(rotator2Switch, SoSwitch, TRUE,
				rotatorSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(rotator2LocateGroup, SoLocateHighlight, TRUE,
				rotator2Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(rotator2, SoSeparator, TRUE,
				rotator2LocateGroup, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(rotator2Active, SoSeparator, TRUE,
				rotator2Switch, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(rotator3Switch, SoSwitch, TRUE,
				rotatorSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(rotator3LocateGroup, SoLocateHighlight, TRUE,
				rotator3Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(rotator3, SoSeparator, TRUE,
				rotator3LocateGroup, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(rotator3Active, SoSeparator, TRUE,
				rotator3Switch, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(rotator4Switch, SoSwitch, TRUE,
				rotatorSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(rotator4LocateGroup, SoLocateHighlight, TRUE,
				rotator4Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(rotator4, SoSeparator, TRUE,
				rotator4LocateGroup, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(rotator4Active, SoSeparator, TRUE,
				rotator4Switch, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(rotator5Switch, SoSwitch, TRUE,
				rotatorSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(rotator5LocateGroup, SoLocateHighlight, TRUE,
				rotator5Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(rotator5, SoSeparator, TRUE,
				rotator5LocateGroup, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(rotator5Active, SoSeparator, TRUE,
				rotator5Switch, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(rotator6Switch, SoSwitch, TRUE,
				rotatorSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(rotator6LocateGroup, SoLocateHighlight, TRUE,
				rotator6Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(rotator6, SoSeparator, TRUE,
				rotator6LocateGroup, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(rotator6Active, SoSeparator, TRUE,
				rotator6Switch, ,TRUE);
}

void
SoTransformerDragger::makeScalerCatalogParts()
{
    SO_KIT_ADD_CATALOG_ENTRY(scaleSep, SoSeparator, TRUE,
				topSeparator, ,FALSE);

    SO_KIT_ADD_CATALOG_ENTRY(scale1Switch, SoSwitch, TRUE,
				scaleSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(scale1LocateGroup, SoLocateHighlight, TRUE,
				scale1Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(scale1, SoSeparator, TRUE,
				scale1LocateGroup, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(scale1Active, SoSeparator, TRUE,
				scale1Switch, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(scale2Switch, SoSwitch, TRUE,
				scaleSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(scale2LocateGroup, SoLocateHighlight, TRUE,
				scale2Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(scale2, SoSeparator, TRUE,
				scale2LocateGroup, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(scale2Active, SoSeparator, TRUE,
				scale2Switch, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(scale3Switch, SoSwitch, TRUE,
				scaleSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(scale3LocateGroup, SoLocateHighlight, TRUE,
				scale3Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(scale3, SoSeparator, TRUE,
				scale3LocateGroup, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(scale3Active, SoSeparator, TRUE,
				scale3Switch, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(scale4Switch, SoSwitch, TRUE,
				scaleSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(scale4LocateGroup, SoLocateHighlight, TRUE,
				scale4Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(scale4, SoSeparator, TRUE,
				scale4LocateGroup, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(scale4Active, SoSeparator, TRUE,
				scale4Switch, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(scale5Switch, SoSwitch, TRUE,
				scaleSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(scale5LocateGroup, SoLocateHighlight, TRUE,
				scale5Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(scale5, SoSeparator, TRUE,
				scale5LocateGroup, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(scale5Active, SoSeparator, TRUE,
				scale5Switch, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(scale6Switch, SoSwitch, TRUE,
				scaleSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(scale6LocateGroup, SoLocateHighlight, TRUE,
				scale6Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(scale6, SoSeparator, TRUE,
				scale6LocateGroup, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(scale6Active, SoSeparator, TRUE,
				scale6Switch, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(scale7Switch, SoSwitch, TRUE,
				scaleSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(scale7LocateGroup, SoLocateHighlight, TRUE,
				scale7Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(scale7, SoSeparator, TRUE,
				scale7LocateGroup, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(scale7Active, SoSeparator, TRUE,
				scale7Switch, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(scale8Switch, SoSwitch, TRUE,
				scaleSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(scale8LocateGroup, SoLocateHighlight, TRUE,
				scale8Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(scale8, SoSeparator, TRUE,
				scale8LocateGroup, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(scale8Active, SoSeparator, TRUE,
				scale8Switch, ,TRUE);
}

void 
SoTransformerDragger::makeAxisFeedbackCatalogParts()
{
    SO_KIT_ADD_CATALOG_ENTRY(axisFeedbackSep, SoSeparator, 
				TRUE, geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(axisFeedbackLocation, SoTranslation, 
				TRUE, axisFeedbackSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(xAxisFeedbackSwitch, SoSwitch, TRUE,
				axisFeedbackSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(xAxisFeedbackActive, SoSeparator, TRUE,
				xAxisFeedbackSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(xAxisFeedbackSelect, SoSeparator, TRUE,
				xAxisFeedbackSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(xCrosshairFeedback, SoSeparator, TRUE,
				xAxisFeedbackSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(yAxisFeedbackSwitch, SoSwitch, TRUE,
				axisFeedbackSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(yAxisFeedbackActive, SoSeparator, TRUE,
				yAxisFeedbackSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(yAxisFeedbackSelect, SoSeparator, TRUE,
				yAxisFeedbackSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(yCrosshairFeedback, SoSeparator, TRUE,
				yAxisFeedbackSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(zAxisFeedbackSwitch, SoSwitch, TRUE,
				axisFeedbackSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(zAxisFeedbackActive, SoSeparator, TRUE,
				zAxisFeedbackSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(zAxisFeedbackSelect, SoSeparator, TRUE,
				zAxisFeedbackSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(zCrosshairFeedback, SoSeparator, TRUE,
				zAxisFeedbackSwitch, ,TRUE);
}

void
SoTransformerDragger::makeBoxFeedbackCatalogParts()
{
    SO_KIT_ADD_CATALOG_ENTRY(translateBoxFeedbackSep, SoSeparator, 
				TRUE, geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(translateBoxFeedbackSwitch, SoSwitch, TRUE,
				translateBoxFeedbackSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(translateBoxFeedbackRotation, SoRotation, 
				TRUE, translateBoxFeedbackSwitch, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(translateBoxFeedback, SoSeparator, TRUE,
				translateBoxFeedbackSwitch, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(scaleBoxFeedbackSwitch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(scaleBoxFeedback, SoSeparator, TRUE,
				scaleBoxFeedbackSwitch, ,TRUE);
}

void
SoTransformerDragger::makeWallFeedbackCatalogParts()
{
    SO_KIT_ADD_CATALOG_ENTRY(posXWallFeedbackSwitch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(posXWallFeedback, SoSeparator, TRUE,
				posXWallFeedbackSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(posXRoundWallFeedback, SoSeparator, TRUE,
				posXWallFeedbackSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(posYWallFeedbackSwitch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(posYWallFeedback, SoSeparator, TRUE,
				posYWallFeedbackSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(posYRoundWallFeedback, SoSeparator, TRUE,
				posYWallFeedbackSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(posZWallFeedbackSwitch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(posZWallFeedback, SoSeparator, TRUE,
				posZWallFeedbackSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(posZRoundWallFeedback, SoSeparator, TRUE,
				posZWallFeedbackSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(negXWallFeedbackSwitch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(negXWallFeedback, SoSeparator, TRUE,
				negXWallFeedbackSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(negXRoundWallFeedback, SoSeparator, TRUE,
				negXWallFeedbackSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(negYWallFeedbackSwitch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(negYWallFeedback, SoSeparator, TRUE,
				negYWallFeedbackSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(negYRoundWallFeedback, SoSeparator, TRUE,
				negYWallFeedbackSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(negZWallFeedbackSwitch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(negZWallFeedback, SoSeparator, TRUE,
				negZWallFeedbackSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(negZRoundWallFeedback, SoSeparator, TRUE,
				negZWallFeedbackSwitch, ,TRUE);
}

void
SoTransformerDragger::makeRadialFeedbackCatalogParts()
{
    SO_KIT_ADD_CATALOG_ENTRY(radialFeedbackSwitch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(radialFeedback, SoSeparator, TRUE,
				radialFeedbackSwitch, ,TRUE);
}

void
SoTransformerDragger::makeCircleFeedbackCatalogParts()
{
    // This feedback goes under the topSeparator, not the geomSeparator.
    // This is because the antiSquish node can cause matrices that change
    // every frame and might break caches for the whole box.
    SO_KIT_ADD_CATALOG_ENTRY(circleFeedbackSep, SoSeparator, TRUE,
				topSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(circleFeedbackTransformSwitch, SoSwitch, TRUE,
				circleFeedbackSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(circleFeedbackAntiSquish, SoAntiSquish, TRUE,
				circleFeedbackTransformSwitch, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(circleFeedbackTransform, SoTransform, TRUE,
				circleFeedbackTransformSwitch, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(xCircleFeedbackSwitch, SoSwitch, TRUE,
				circleFeedbackSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(xCircleFeedback, SoSeparator, TRUE,
				xCircleFeedbackSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(yCircleFeedbackSwitch, SoSwitch, TRUE,
				circleFeedbackSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(yCircleFeedback, SoSeparator, TRUE,
				yCircleFeedbackSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(zCircleFeedbackSwitch, SoSwitch, TRUE,
				circleFeedbackSep, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(zCircleFeedback, SoSeparator, TRUE,
				zCircleFeedbackSwitch, ,TRUE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    called by Constructor to set up all default geom.
//
void
SoTransformerDragger::setAllDefaultParts()
//
////////////////////////////////////////////////////////////////////////
{
   // Set the Locate Highlight Colors if the resource is there:
   SoNode *hln = SoNode::getByName("transformerLocateMaterial");
   if (hln && hln->isOfType( SoMaterial::getClassTypeId() )) {
       SoLocateHighlight *hlg;
       SoMaterial *hlm = (SoMaterial *) hln;
       SbColor emc = hlm->emissiveColor[0];
       SoLocateHighlight::Styles myStyle = SoLocateHighlight::EMISSIVE_DIFFUSE;
       hlg 
        = SO_GET_ANY_PART(this,"translator1LocateGroup",SoLocateHighlight);
       hlg->color = emc; hlg->style = myStyle;
       hlg 
        = SO_GET_ANY_PART(this,"translator2LocateGroup",SoLocateHighlight);
       hlg->color = emc; hlg->style = myStyle;
       hlg 
        = SO_GET_ANY_PART(this,"translator3LocateGroup",SoLocateHighlight);
       hlg->color = emc; hlg->style = myStyle;
       hlg 
        = SO_GET_ANY_PART(this,"translator4LocateGroup",SoLocateHighlight);
       hlg->color = emc; hlg->style = myStyle;
       hlg 
        = SO_GET_ANY_PART(this,"translator5LocateGroup",SoLocateHighlight);
       hlg->color = emc; hlg->style = myStyle;
       hlg 
        = SO_GET_ANY_PART(this,"translator6LocateGroup",SoLocateHighlight);
       hlg->color = emc; hlg->style = myStyle;

       hlg = SO_GET_ANY_PART(this,"rotator1LocateGroup",SoLocateHighlight);
       hlg->color = emc; hlg->style = myStyle;
       hlg = SO_GET_ANY_PART(this,"rotator2LocateGroup",SoLocateHighlight);
       hlg->color = emc; hlg->style = myStyle;
       hlg = SO_GET_ANY_PART(this,"rotator3LocateGroup",SoLocateHighlight);
       hlg->color = emc; hlg->style = myStyle;
       hlg = SO_GET_ANY_PART(this,"rotator4LocateGroup",SoLocateHighlight);
       hlg->color = emc; hlg->style = myStyle;
       hlg = SO_GET_ANY_PART(this,"rotator5LocateGroup",SoLocateHighlight);
       hlg->color = emc; hlg->style = myStyle;
       hlg = SO_GET_ANY_PART(this,"rotator6LocateGroup",SoLocateHighlight);
       hlg->color = emc; hlg->style = myStyle;

       hlg = SO_GET_ANY_PART(this,"scale1LocateGroup",SoLocateHighlight);
       hlg->color = emc; hlg->style = myStyle;
       hlg = SO_GET_ANY_PART(this,"scale2LocateGroup",SoLocateHighlight);
       hlg->color = emc; hlg->style = myStyle;
       hlg = SO_GET_ANY_PART(this,"scale3LocateGroup",SoLocateHighlight);
       hlg->color = emc; hlg->style = myStyle;
       hlg = SO_GET_ANY_PART(this,"scale4LocateGroup",SoLocateHighlight);
       hlg->color = emc; hlg->style = myStyle;
       hlg = SO_GET_ANY_PART(this,"scale5LocateGroup",SoLocateHighlight);
       hlg->color = emc; hlg->style = myStyle;
       hlg = SO_GET_ANY_PART(this,"scale6LocateGroup",SoLocateHighlight);
       hlg->color = emc; hlg->style = myStyle;
       hlg = SO_GET_ANY_PART(this,"scale7LocateGroup",SoLocateHighlight);
       hlg->color = emc; hlg->style = myStyle;
       hlg = SO_GET_ANY_PART(this,"scale8LocateGroup",SoLocateHighlight);
       hlg->color = emc; hlg->style = myStyle;
    }

    // Set up the overallStyle node
  setAnyPartAsDefault("overallStyle",     "transformerOverallStyle");

    // translation transformer pieces.
  setPartAsDefault("translator1",     "transformerTranslator1");
  setPartAsDefault("translator1Active","transformerTranslator1Active");
  setPartAsDefault("translator2",     "transformerTranslator2");
  setPartAsDefault("translator2Active","transformerTranslator2Active");
  setPartAsDefault("translator3",     "transformerTranslator3");
  setPartAsDefault("translator3Active","transformerTranslator3Active");
  setPartAsDefault("translator4",     "transformerTranslator4");
  setPartAsDefault("translator4Active","transformerTranslator4Active");
  setPartAsDefault("translator5",     "transformerTranslator5");
  setPartAsDefault("translator5Active","transformerTranslator5Active");
  setPartAsDefault("translator6",     "transformerTranslator6");
  setPartAsDefault("translator6Active","transformerTranslator6Active");

    // When we create the rotation and scale pieces, we make copies instead of
    // using instances of the original. This is because they will probably 
    // contain SoAntiSquish nodes, and we don't want to instance these since
    // they would draw differently in different places in the scene.
    SoSeparator *sep;

    sep = (SoSeparator *) SoNode::getByName("transformerRotator1")->copy();
    setPartAsDefault("rotator1",      sep);
    sep = (SoSeparator *) SoNode::getByName("transformerRotator1Active")->copy();
    setPartAsDefault("rotator1Active",sep);
    sep = (SoSeparator *) SoNode::getByName("transformerRotator2")->copy();
    setPartAsDefault("rotator2",      sep);
    sep = (SoSeparator *) SoNode::getByName("transformerRotator2Active")->copy();
    setPartAsDefault("rotator2Active",sep);
    sep = (SoSeparator *) SoNode::getByName("transformerRotator3")->copy();
    setPartAsDefault("rotator3",      sep);
    sep = (SoSeparator *) SoNode::getByName("transformerRotator3Active")->copy();
    setPartAsDefault("rotator3Active",sep);
    sep = (SoSeparator *) SoNode::getByName("transformerRotator4")->copy();
    setPartAsDefault("rotator4",      sep);
    sep = (SoSeparator *) SoNode::getByName("transformerRotator4Active")->copy();
    setPartAsDefault("rotator4Active",sep);
    sep = (SoSeparator *) SoNode::getByName("transformerRotator5")->copy();
    setPartAsDefault("rotator5",      sep);
    sep = (SoSeparator *) SoNode::getByName("transformerRotator5Active")->copy();
    setPartAsDefault("rotator5Active",sep);
    sep = (SoSeparator *) SoNode::getByName("transformerRotator6")->copy();
    setPartAsDefault("rotator6",      sep);
    sep = (SoSeparator *) SoNode::getByName("transformerRotator6Active")->copy();
    setPartAsDefault("rotator6Active",sep);

    // scale scale pieces.
    sep = (SoSeparator *) SoNode::getByName("transformerScale1")->copy();
    setPartAsDefault("scale1",      sep);
    sep = (SoSeparator *) SoNode::getByName("transformerScale1Active")->copy();
    setPartAsDefault("scale1Active",sep);
    sep = (SoSeparator *) SoNode::getByName("transformerScale2")->copy();
    setPartAsDefault("scale2",      sep);
    sep = (SoSeparator *) SoNode::getByName("transformerScale2Active")->copy();
    setPartAsDefault("scale2Active",sep);
    sep = (SoSeparator *) SoNode::getByName("transformerScale3")->copy();
    setPartAsDefault("scale3",      sep);
    sep = (SoSeparator *) SoNode::getByName("transformerScale3Active")->copy();
    setPartAsDefault("scale3Active",sep);
    sep = (SoSeparator *) SoNode::getByName("transformerScale4")->copy();
    setPartAsDefault("scale4",      sep);
    sep = (SoSeparator *) SoNode::getByName("transformerScale4Active")->copy();
    setPartAsDefault("scale4Active",sep);
    sep = (SoSeparator *) SoNode::getByName("transformerScale5")->copy();
    setPartAsDefault("scale5",      sep);
    sep = (SoSeparator *) SoNode::getByName("transformerScale5Active")->copy();
    setPartAsDefault("scale5Active",sep);
    sep = (SoSeparator *) SoNode::getByName("transformerScale6")->copy();
    setPartAsDefault("scale6",      sep);
    sep = (SoSeparator *) SoNode::getByName("transformerScale6Active")->copy();
    setPartAsDefault("scale6Active",sep);
    sep = (SoSeparator *) SoNode::getByName("transformerScale7")->copy();
    setPartAsDefault("scale7",      sep);
    sep = (SoSeparator *) SoNode::getByName("transformerScale7Active")->copy();
    setPartAsDefault("scale7Active",sep);
    sep = (SoSeparator *) SoNode::getByName("transformerScale8")->copy();
    setPartAsDefault("scale8",      sep);
    sep = (SoSeparator *) SoNode::getByName("transformerScale8Active")->copy();
    setPartAsDefault("scale8Active",sep);

    // Put scale,rot,and translate parts in default settings
    setAllPartSwitches(0, 0, 0);

    // Axis Feedback for translation.
    setAnyPart("axisFeedbackLocation",   new SoTranslation );

    // Anti Squish for rotation feedback
    SoAntiSquish *myAS = new SoAntiSquish;
    myAS->recalcAlways = FALSE;
    setAnyPart("circleFeedbackAntiSquish", myAS );
    setAnyPart("circleFeedbackTransform", new SoTransform );

    setPartAsDefault("translateBoxFeedback",   "transformerTranslateBoxFeedback");
    setPartAsDefault("scaleBoxFeedback",   "transformerScaleBoxFeedback");
    setPartAsDefault("posXWallFeedback",   "transformerPosXWallFeedback");
    setPartAsDefault("posYWallFeedback",   "transformerPosYWallFeedback");
    setPartAsDefault("posZWallFeedback",   "transformerPosZWallFeedback");
    setPartAsDefault("negXWallFeedback",   "transformerNegXWallFeedback");
    setPartAsDefault("negYWallFeedback",   "transformerNegYWallFeedback");
    setPartAsDefault("negZWallFeedback",   "transformerNegZWallFeedback");
    setPartAsDefault("posXRoundWallFeedback",   "transformerPosXRoundWallFeedback");
    setPartAsDefault("posYRoundWallFeedback",   "transformerPosYRoundWallFeedback");
    setPartAsDefault("posZRoundWallFeedback",   "transformerPosZRoundWallFeedback");
    setPartAsDefault("negXRoundWallFeedback",   "transformerNegXRoundWallFeedback");
    setPartAsDefault("negYRoundWallFeedback",   "transformerNegYRoundWallFeedback");
    setPartAsDefault("negZRoundWallFeedback",   "transformerNegZRoundWallFeedback");
    setPartAsDefault("xAxisFeedbackActive",      "transformerXAxisFeedbackActive");
    setPartAsDefault("yAxisFeedbackActive",      "transformerYAxisFeedbackActive");
    setPartAsDefault("zAxisFeedbackActive",      "transformerZAxisFeedbackActive");
    setPartAsDefault("xAxisFeedbackSelect",      "transformerXAxisFeedbackSelect");
    setPartAsDefault("yAxisFeedbackSelect",      "transformerYAxisFeedbackSelect");
    setPartAsDefault("zAxisFeedbackSelect",      "transformerZAxisFeedbackSelect");
    setPartAsDefault("xCrosshairFeedback",      "transformerXCrosshairFeedback");
    setPartAsDefault("yCrosshairFeedback",      "transformerYCrosshairFeedback");
    setPartAsDefault("zCrosshairFeedback",      "transformerZCrosshairFeedback");
    setPartAsDefault("radialFeedback",     "transformerRadialFeedback");
    setPartAsDefault("xCircleFeedback",    "transformerXCircleFeedback");
    setPartAsDefault("yCircleFeedback",    "transformerYCircleFeedback");
    setPartAsDefault("zCircleFeedback",    "transformerZCircleFeedback");
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
SoTransformerDragger::~SoTransformerDragger()
//
////////////////////////////////////////////////////////////////////////
{
    delete planeProj;
    delete lineProj;
    delete sphereProj;
    delete cylProj;

    if (translFieldSensor )
	delete translFieldSensor; 
    if (scaleFieldSensor )
	delete scaleFieldSensor; 
    if (rotateFieldSensor )
	delete rotateFieldSensor; 
}

//    detach/attach any sensors, callbacks, and/or field connections.
//    Called by:            start/end of SoBaseKit::readInstance
//    and on new copy by:   start/end of SoBaseKit::copy.
//    Classes that redefine must call setUpConnections(TRUE,TRUE) 
//    at end of constructor.
//    Returns the state of the node when this was called.
SbBool
SoTransformerDragger::setUpConnections( SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( onOff ) {

	// We connect AFTER base class.
	SoDragger::setUpConnections( onOff, FALSE );

	// Call the sensor CBs to make things are up-to-date.
	fieldSensorCB( this, NULL );

	// Connect the field sensors
	if (translFieldSensor->getAttachedField() != &translation)
	    translFieldSensor->attach( &translation );
	if (scaleFieldSensor->getAttachedField() != &scaleFactor)
	    scaleFieldSensor->attach( &scaleFactor );
	if (rotateFieldSensor->getAttachedField() != &rotation)
	    rotateFieldSensor->attach( &rotation );

	// Fill up our list of searched SoAntiSquish nodes.
        updateAntiSquishList();
	unsquishKnobs();
    }
    else {

	// empty out our list of SoAntiSquish nodes.
        antiSquishList.truncate(0);

	// We disconnect BEFORE base class.

	// Disconnect the field sensors.
	if (translFieldSensor->getAttachedField())
	    translFieldSensor->detach();
	if (scaleFieldSensor->getAttachedField())
	    scaleFieldSensor->detach();
	if (rotateFieldSensor->getAttachedField())
	    rotateFieldSensor->detach();

	SoDragger::setUpConnections( onOff, FALSE );
    }

    return !(connectionsSetUp = onOff);
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Get set to begin dragging.
//
// Use: private
//
void
SoTransformerDragger::dragStart()
//
////////////////////////////////////////////////////////////////////////
{
    // Invalidate surroundScale if it exists.
	SoSurroundScale *ss 
	    = SO_CHECK_PART(this,"surroundScale",SoSurroundScale);
	if (ss != NULL)
	    ss->invalidate();

    // Make a note of which modifier keys are down.
	altDown  = getEvent()->wasAltDown();
	ctlDown  = getEvent()->wasCtrlDown();
	shftDown = getEvent()->wasShiftDown();


    // Determine what state we are in:
	// This gives us an opportunity for a part to start a new gesture
	// after releasing and re-grabbing in the metaKey callback.
	currentState = restartState;

	// Note: we won't even bother if the restartState was not INACTIVE.
	if ( currentState == INACTIVE )
	    currentState = getStateFromPick();

    // Constraining depends on both shftDown and on which part was hit.
    // Rotation constrains by default, without pressing shift key.
    // Other parts constrain when the shift key is down.
	constraining = FALSE;
	if ( currentState ==  RIT_X_ROTATE || currentState ==  LFT_X_ROTATE || 
	     currentState ==  TOP_Y_ROTATE || currentState ==  BOT_Y_ROTATE || 
	     currentState ==  FNT_Z_ROTATE || currentState ==  BAK_Z_ROTATE ) {
	    if ( ! shftDown )
		constraining = TRUE;
	}
	else if ( shftDown )
	    constraining = TRUE;

    // Reset direction for gesture-selected constrained motion
    // But if we are restarting and also constraining to a direction,
    // leave it alone:
	if ( ! (restartState != INACTIVE && constraining) )
	    currentDir = -1;

    // Switch the correct parts on/off depending on state
	setHighlights();

    // This is the point we'll use if a metaKey callback makes us re-start.
	worldRestartPt = getWorldStartingPoint();

    // Go to the appropriate Init() routine:
	switch( currentState ) {
	    case RIT_TRANSLATE: case LFT_TRANSLATE: case TOP_TRANSLATE:
	    case BOT_TRANSLATE: case FNT_TRANSLATE: case BAK_TRANSLATE:
		translateInit();
		break;
	    case PX_PY_PZ_3D_SCALE: case PX_PY_NZ_3D_SCALE:
	    case PX_NY_PZ_3D_SCALE: case PX_NY_NZ_3D_SCALE:
	    case NX_PY_PZ_3D_SCALE: case NX_PY_NZ_3D_SCALE:
	    case NX_NY_PZ_3D_SCALE: case NX_NY_NZ_3D_SCALE:
		scaleInit();
		break;
	    case RIT_X_ROTATE: case LFT_X_ROTATE: case TOP_Y_ROTATE:
	    case BOT_Y_ROTATE: case FNT_Z_ROTATE: case BAK_Z_ROTATE:
		rotateInit();
		break;
	    case INACTIVE:
	    default:
		break;
	}
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Called by dragStart to see which part was picked.
//
// Use: private
//
SoTransformerDragger::State
SoTransformerDragger::getStateFromPick()
//
////////////////////////////////////////////////////////////////////////
{
    const SoPath *pickPath = getPickPath();
    SoSwitch *hitSwitch = NULL;

    // First, find the first SoSwitch node underneath this node
    // that lies on the pickPath. We'll compare this value to 
    // our various part switches to determine the hit.

	if ( pickPath != NULL ) {

		SoFullPath *fp = (SoFullPath *) pickPath;
		SbBool gotThis = FALSE; 
		for (int i = 0; i < fp->getLength() && hitSwitch == NULL;i++){
		    SoNode *n = fp->getNode(i);
		    if ( ! gotThis ) {
			if ( n == this )
			    gotThis = TRUE;
		    }
		    else {
			if ( n->isOfType( SoSwitch::getClassTypeId() ) )
			    hitSwitch = (SoSwitch *) n;
		    }
		}
	}

    // See which subGraph of the SoTransformer was hit to determine operation
	if (hitSwitch != NULL) {

	    // TRANSLATIONS
	    if ( hitSwitch == translator1Switch.getValue() )
		return TOP_TRANSLATE;
	    if ( hitSwitch == translator2Switch.getValue() )
		return BOT_TRANSLATE;
	    if ( hitSwitch == translator3Switch.getValue() )
		return LFT_TRANSLATE;
	    if ( hitSwitch == translator4Switch.getValue() )
		return RIT_TRANSLATE;
	    if ( hitSwitch == translator5Switch.getValue() )
		return FNT_TRANSLATE;
	    if ( hitSwitch == translator6Switch.getValue() )
		return BAK_TRANSLATE;

	    // 1-d SCALING
	    if ( hitSwitch == rotator1Switch.getValue() )
		return TOP_Y_ROTATE;
	    if ( hitSwitch == rotator2Switch.getValue() )
		return BOT_Y_ROTATE;
	    if ( hitSwitch == rotator3Switch.getValue() )
		return LFT_X_ROTATE;
	    if ( hitSwitch == rotator4Switch.getValue() )
		return RIT_X_ROTATE;
	    if ( hitSwitch == rotator5Switch.getValue() )
		return FNT_Z_ROTATE;
	    if ( hitSwitch == rotator6Switch.getValue() )
		return BAK_Z_ROTATE;

	    // 3-d SCALING
	    if ( hitSwitch == scale1Switch.getValue() )
		return PX_PY_PZ_3D_SCALE;
	    if ( hitSwitch == scale2Switch.getValue() )
		return PX_PY_NZ_3D_SCALE;
	    if ( hitSwitch == scale3Switch.getValue() )
		return PX_NY_PZ_3D_SCALE;
	    if ( hitSwitch == scale4Switch.getValue() )
		return PX_NY_NZ_3D_SCALE;
	    if ( hitSwitch == scale5Switch.getValue() )
		return NX_PY_PZ_3D_SCALE;
	    if ( hitSwitch == scale6Switch.getValue() )
		return NX_PY_NZ_3D_SCALE;
	    if ( hitSwitch == scale7Switch.getValue() )
		return NX_NY_PZ_3D_SCALE;
	    if ( hitSwitch == scale8Switch.getValue() )
		return NX_NY_NZ_3D_SCALE;
        }

    // Try looking at the surrogate part paths...

    // Surrogate part paths...
	const SbName &theName = getSurrogatePartPickedName();

	if ( theName ==  "translator1" ) return TOP_TRANSLATE;
	if ( theName ==  "translator2" ) return BOT_TRANSLATE;
	if ( theName ==  "translator3" ) return LFT_TRANSLATE;
	if ( theName ==  "translator4" ) return RIT_TRANSLATE;
	if ( theName ==  "translator5" ) return FNT_TRANSLATE;
	if ( theName ==  "translator6" ) return BAK_TRANSLATE;

	if ( theName ==  "rotator1" ) return TOP_Y_ROTATE;
	if ( theName ==  "rotator2" ) return BOT_Y_ROTATE;
	if ( theName ==  "rotator3" ) return LFT_X_ROTATE;
	if ( theName ==  "rotator4" ) return RIT_X_ROTATE;
	if ( theName ==  "rotator5" ) return FNT_Z_ROTATE;
	if ( theName ==  "rotator6" ) return BAK_Z_ROTATE;

	if ( theName ==  "scale1" ) return PX_PY_PZ_3D_SCALE;
	if ( theName ==  "scale2" ) return PX_PY_NZ_3D_SCALE;
	if ( theName ==  "scale3" ) return PX_NY_PZ_3D_SCALE;
	if ( theName ==  "scale4" ) return PX_NY_NZ_3D_SCALE;
	if ( theName ==  "scale5" ) return NX_PY_PZ_3D_SCALE;
	if ( theName ==  "scale6" ) return NX_PY_NZ_3D_SCALE;
	if ( theName ==  "scale7" ) return NX_NY_PZ_3D_SCALE;
	if ( theName ==  "scale8" ) return NX_NY_NZ_3D_SCALE;

    return INACTIVE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Get set to begin translating.
//
// Use: private
//
SbBool
SoTransformerDragger::translateInit()
//
////////////////////////////////////////////////////////////////////////
{
    // Calculations will be in world space, so that 
    // gesture based constraints will be evenly weighted in all 3 directions
    // instead of skewed by scale of object.
	SbVec3f planePt = getWorldStartingPoint();

    // do different things depending on what you hit...
	SbVec3f planeNormal;

	switch( currentState ) {
	    case RIT_TRANSLATE:
	    case LFT_TRANSLATE:
		planeNormal = getBoxDirInWorldSpace( SbVec3f(1,0,0));
		break;
	    case TOP_TRANSLATE:
	    case BOT_TRANSLATE:
		planeNormal = getBoxDirInWorldSpace( SbVec3f(0,1,0));
		break;
	    case FNT_TRANSLATE:
	    case BAK_TRANSLATE:
		planeNormal = getBoxDirInWorldSpace( SbVec3f(0,0,1));
		break;
	    default:
		return FALSE;
	}
	planeNormal.normalize();

    // For calculating motion within the plane.
	planeProj->setPlane( SbPlane(planeNormal, planePt) );
	planeProj->setWorkingSpace( SbMatrix::identity() );

    // If control key is down, we also need a projector perpendicular to plane.
	if ( ctlDown ) {
	    lineProj->setLine( SbLine(planePt, planePt + planeNormal) );
	    lineProj->setWorkingSpace( SbMatrix::identity() );
	}

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Get set to begin scaling, based on input from a knob placed on a 
//    vertex of the box.
//    This implies a one dimensional scaling.
//
// Use: private
//
SbBool
SoTransformerDragger::scaleInit()
//
////////////////////////////////////////////////////////////////////////
{
    // Calculations will be in world space, so that 
    // gesture based constraints will be evenly weighted in all 3 directions
    // instead of skewed by scale of object.

    // create a line between the box center and the point that was hit
	SbVec3f hitPt = getWorldStartingPoint();
	SbVec3f boxCtr = getBoxPointInWorldSpace(  SbVec3f(0,0,0) );
	lineProj->setLine( SbLine( boxCtr, hitPt ) );
	lineProj->setWorkingSpace( SbMatrix::identity() );

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Get set to begin rotating, based on input from a knob placed on a 
//    face of the box.
//    Default is free 3-D rotation, holding shift key constrains to one
//    of 2 directions.
//
// Use: private
//
SbBool
SoTransformerDragger::rotateInit()
//
////////////////////////////////////////////////////////////////////////
{
    // Calculations will be in world space, so that gesture based rotations 
    // will be round in world instead of resulting in egg-shaped scaling.

    // Set the box-space center of rotation based on the <CONTROL> key 
    // and which know was picked.
	if ( ! ctlDown ) {
	    // With no ctl key down, we rotate about center.
	    interactiveCenterInBoxSpace.setValue(0,0,0);
	}
	else {
	    switch ( currentState ) {
		case RIT_X_ROTATE: 
	    	    interactiveCenterInBoxSpace.setValue(-1, 0, 0 );
		    break;
		case LFT_X_ROTATE: 
	    	    interactiveCenterInBoxSpace.setValue( 1, 0, 0 );
		    break;
		case TOP_Y_ROTATE:
	    	    interactiveCenterInBoxSpace.setValue( 0,-1, 0 );
		    break;
		case BOT_Y_ROTATE: 
	    	    interactiveCenterInBoxSpace.setValue( 0, 1, 0 );
		    break;
		case FNT_Z_ROTATE: 
	    	    interactiveCenterInBoxSpace.setValue( 0, 0,-1 );
		    break;
		case BAK_Z_ROTATE:
	    	    interactiveCenterInBoxSpace.setValue( 0, 0, 1 );
		    break;
	    }
	}

    // If we're doing free rotation, initialize the sphere projector.
    // Otherwise we'll have to wait until later because we don't know 
    // whether we'll use a disc or cylinder projector style -- it depends
    // on the orientation of the rotation plane with respect to the eye.
    if ( ! constraining )
	initSphereProjector();

    // Unlike other draggers, we MUST use incremental changes,
    // since rotations do not give consistent results across
    // long motions. Each motion must be fairly short. So we'll be saving
    // each previous mouse point and motion matrix.
	// The spherical projectors are sort of weird -- the initial hit 
	// defines the projector, but since the projector is shaped like a 
	// sphere with a plane through it, the initial hit may not actually lie         // on the sphere part of the surface. This happens when the inital hit 
	// is too close to the edge to fit within 'tolerance.' To insure 
	// accurate performance, we must project the mouse onto the projector 
	// after it is defined in order to get our prevWorldHitPt 
	if ( ! constraining )
	    prevWorldHitPt 
		= sphereProj->project(getNormalizedLocaterPosition()); 
	else {
	    prevWorldHitPt = getWorldStartingPoint();
	    // Store the normalizedLocater position because if we later 
	    // create a cylinder projector we'll need it to perform this
	    // "trick" again:
    	    startNormalizedLocaterPosition = getNormalizedLocaterPosition(); 
	}

    // Save the matrix.
	prevMotionMatrix = getMotionMatrix();

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the projector used for free rotation.
//    This is the only case where the sphere projector is used.
//
// Use: private
//
void
SoTransformerDragger::initSphereProjector()
//
////////////////////////////////////////////////////////////////////////
{
    // Establish the projector sphere in world space.
	SbVec3f hitPt = getWorldStartingPoint();

	SbVec3f sphCenter 
		    = getBoxPointInWorldSpace( interactiveCenterInBoxSpace );
        SbVec3f rad = hitPt - sphCenter;

        sphereProj->setSphere( SbSphere( sphCenter, rad.length()) );
	sphereProj->setViewVolume( getViewVolume() );
	sphereProj->setWorkingSpace( SbMatrix::identity() );

    // If the hit point is on the near side of the center from where
    // the eye is, then tell the projector to intersect front.
    // Else, tell it to intersect back.
    if (getFrontOnProjector() ==  USE_PICK )
	sphereProj->setFront( sphereProj->isPointInFront( hitPt ));
    else if (getFrontOnProjector() ==  FRONT )
	sphereProj->setFront( TRUE );
    else
	sphereProj->setFront( FALSE );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Creates a plane projector for doing disc-style rotations
//    based on the current rotation axis given by currentDir.
// 
// Use: private
//
void
SoTransformerDragger::initDiscProjector()
//
////////////////////////////////////////////////////////////////////////
{
    // Get startHit in world space.
	SbVec3f startHit    = getWorldStartingPoint();;
    
    // Figure out center of disc in world space.
	SbVec3f wldCenter 
		    = getBoxPointInWorldSpace( interactiveCenterInBoxSpace );

    // Build the projection plane we need, in world space:
	// Construct normalLine, between wldCenter and point along normal:
	// The normal depends on currentDir:
	    SbVec3f normalDir;
	    if (currentDir == 0)
		normalDir = getBoxDirInWorldSpace(SbVec3f(1,0,0));
	    else if (currentDir == 1)
		normalDir = getBoxDirInWorldSpace(SbVec3f(0,1,0));
	    else
		normalDir = getBoxDirInWorldSpace(SbVec3f(0,0,1));
	    normalDir.normalize();
	    SbLine  normalLine( wldCenter, wldCenter + normalDir );
	// Origin of plane is point on line at same level as startHit
	    SbVec3f planeOrigin = normalLine.getClosestPoint(startHit);

    // Set up the projector
	planeProj->setViewVolume( getViewVolume() );
	planeProj->setWorkingSpace( SbMatrix::identity() );
	planeProj->setPlane(SbPlane(normalDir, planeOrigin));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Creates a cylinder projector for doing rolling-pin style rotations
//    based on the current rotation axis given by currentDir.
// 
// Use: private
//
void
SoTransformerDragger::initCylinderProjector()
//
////////////////////////////////////////////////////////////////////////
{
    // Get startHit in world space.
	SbVec3f startHit    = getWorldStartingPoint();;
    
    // Figure out center of cylinder in world space.
	SbVec3f cylCenter 
		    = getBoxPointInWorldSpace( interactiveCenterInBoxSpace );

    // Build the projection cylinder we need, in world space:
	// Construct the cylinder axis.
	// The direction depends on currentDir:
	    SbVec3f boxSpaceCylTop;
	    if (currentDir == 0)
		boxSpaceCylTop = interactiveCenterInBoxSpace + SbVec3f(1,0,0);
	    else if (currentDir == 1)
		boxSpaceCylTop = interactiveCenterInBoxSpace + SbVec3f(0,1,0);
	    else
		boxSpaceCylTop = interactiveCenterInBoxSpace + SbVec3f(0,0,1);

	    SbVec3f cylTop;
	    cylTop = getBoxPointInWorldSpace(boxSpaceCylTop);

	    SbLine cylAxis(cylCenter, cylTop);

	// Figure out cylinder radius.
	    float rad = (startHit - cylAxis.getClosestPoint(startHit)).length();

	// Set up the projector
	    cylProj->setViewVolume( getViewVolume() );
	    cylProj->setWorkingSpace( SbMatrix::identity() );
	    cylProj->setCylinder( SbCylinder(cylAxis, rad) );

	// If the hit point is on the near side of the center from where
	// the eye is, then tell the projector to intersect front.
	if (getFrontOnProjector() ==  USE_PICK )
	    cylProj->setFront( cylProj->isPointInFront(startHit));
	else if (getFrontOnProjector() ==  FRONT )
	    cylProj->setFront( TRUE );
	else
	    cylProj->setFront( FALSE );

	// The cylinder projectors are sort of weird -- the initial hit 
	// defines the projector, but since the projector is shaped like a 
	// cylinder with a plane through it, the initial hit may not actually 
	// lie on the cylinder part of the surface. This happens when the 
	// inital hit is too close to the edge to fit within 'tolerance.' 
	// To insure accurate performance, we must project the mouse onto the 
	// projector after it is defined in order to get our prevWorldHitPt 
	// Since we haven't actually rotated yet, it's okay to change this 
	// now.
	    prevWorldHitPt = cylProj->project( startNormalizedLocaterPosition );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Drag the box based on locater motion.
//    This routine just ships the work to translateDrag or 
//    scaleDrag.
//
// Use: private
//
void
SoTransformerDragger::drag()
//
////////////////////////////////////////////////////////////////////////
{
    switch( currentState ) {

	case RIT_TRANSLATE: case LFT_TRANSLATE: case TOP_TRANSLATE:
	case BOT_TRANSLATE: case FNT_TRANSLATE: case BAK_TRANSLATE:
	    translateDrag();
	    break;

	case PX_PY_PZ_3D_SCALE: case PX_PY_NZ_3D_SCALE:
	case PX_NY_PZ_3D_SCALE: case PX_NY_NZ_3D_SCALE:
	case NX_PY_PZ_3D_SCALE: case NX_PY_NZ_3D_SCALE:
	case NX_NY_PZ_3D_SCALE: case NX_NY_NZ_3D_SCALE:
	    scaleDrag();
	    break;

	case RIT_X_ROTATE: case LFT_X_ROTATE: case TOP_Y_ROTATE:
	case BOT_Y_ROTATE: case FNT_Z_ROTATE: case BAK_Z_ROTATE:
	    if ( ! constraining )
		rotateDrag();
	    else
		rotateConstrainedDrag();
	    break;

	case INACTIVE:
	default:
	    break;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Translate the box and object based on locater motion.
//
// Use: private
//
SbBool
SoTransformerDragger::translateDrag()
//
////////////////////////////////////////////////////////////////////////
{
    // All calculations are in world space:
	SbVec3f newHit;

    if ( !ctlDown ) {
    	// Regular translation within the plane:

	// Calculate intersection with plane projector.
	// Don't bother with setWorkingSpace. For world space, always identity()
	    planeProj->setViewVolume( getViewVolume() );
	    newHit = planeProj->project(getNormalizedLocaterPosition());

	// If we need to start a new gesture, we'll carry on from this pt.
	    worldRestartPt = newHit;
    }
    else {
	// We are moving PERPENDICULAR to the motion plane.  

	// Calculate intersection with line projector.
	    lineProj->setViewVolume( getViewVolume() );
	    newHit = lineProj->project( getNormalizedLocaterPosition());

	// Move the projection plane up to the height of the new hit.
	    const SbPlane &oldPlane = planeProj->getPlane();
	    const SbVec3f &oldNorm  = oldPlane.getNormal();
	    planeProj->setPlane( SbPlane( oldNorm, newHit ) );

	// Intersect with the new plane to find the new worldRestartPt
	// If we need to start a new gesture, we'll carry on from this pt.
	    planeProj->setViewVolume( getViewVolume() );
	    worldRestartPt = planeProj->project(getNormalizedLocaterPosition());
    }


    // find the difference between current and beginning intersections.
	SbVec3f  worldMotion = newHit - getWorldStartingPoint();

    // Constrain motion if necessary
	if ( !constraining || ctlDown )
					// constrainKey not down, or we are 
				 	// doing perpendicular (CONTROL) motion.
				 	// Clear the 1-D translation dir.
	    currentDir = -1;
	else {
	    // the constrain key is pressed. This means 1-D translation.

	    if ( currentDir == -1 ) {
		// The 1-D direction is not defined.  Calculate it
		// based on which direction got the maximum locater motion.

		if ( isAdequateConstraintMotion() ) {
		    switch( currentState ) {
			case RIT_TRANSLATE: case LFT_TRANSLATE: 
		    	    currentDir = getMouseGestureDirection(0,1,1);
			    break;
			case TOP_TRANSLATE: case BOT_TRANSLATE: 
		    	    currentDir = getMouseGestureDirection(1,0,1);
			    break;
			case FNT_TRANSLATE: case BAK_TRANSLATE:
		    	    currentDir = getMouseGestureDirection(1,1,0);
			    break;
		    }
		    setFeedback();
		}
		else {
		    // Not ready to pick a direction yet. Don't move.
		    return FALSE;
		}
	    }

	    // get the projection of 'workSpaceMotion' onto the preferred axis.
	    SbVec3f dirVec;
	    if (currentDir == 0) 
		dirVec = getBoxDirInWorldSpace(  SbVec3f(1,0,0) );
	    else if (currentDir == 1) 
		dirVec = getBoxDirInWorldSpace(  SbVec3f(0,1,0) );
	    else 
		dirVec = getBoxDirInWorldSpace(  SbVec3f(0,0,1) );
	    dirVec.normalize();

	    worldMotion = dirVec * dirVec.dot( worldMotion );
	}

    // Append this to the startMotionMatrix, which we saved at the beginning
    // of the drag, to find the current motion matrix.
    // We need to send our matrix that converts from world space to localSpace.
	SbMatrix wldToLcl = getWorldToLocalMatrix();
	setMotionMatrix( 
	    appendTranslation( getStartMotionMatrix(), worldMotion, &wldToLcl));

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Scale the box and object based on locater motion.
//
// Use: private
//
SbBool
SoTransformerDragger::scaleDrag()
//
////////////////////////////////////////////////////////////////////////
{
    // If SHIFT key down, we may have to pick constraint direction:
	if ( !constraining ) {
	    currentDir = -1;
	}
	else if ( currentDir == -1 ) {
	    // SHIFT key is down and the 1-D direction is not defined.  
	    // Calculate it based on direction that got max locater motion.

	    if ( isAdequateConstraintMotion() ) {
		currentDir = getMouseGestureDirection(TRUE,TRUE,TRUE);
		setHighlights();
	    }
	    else {
		// Not ready to pick a direction yet. Don't move.
		return FALSE;
	    }
	}

    // Get startHit in world space and in boxSpace:.
	SbVec3f startHit    = getWorldStartingPoint();;
	SbVec3f	startHitBox = getWorldPointInBoxSpace( startHit );

    // determine the center for scaling in both world and box space...
	SbVec3f  sclCntr;
	if ( !ctlDown ) {
	    switch( currentDir ) {
		case -1:
		    interactiveCenterInBoxSpace.setValue(0,0,0);
		    break;
		case 0:
		    interactiveCenterInBoxSpace.setValue(
					    0, startHitBox[1], startHitBox[2]);
		    break;
		case 1:
		    interactiveCenterInBoxSpace.setValue(
					    startHitBox[0], 0, startHitBox[2]);
		    break;
		case 2:
		    interactiveCenterInBoxSpace.setValue(
					    startHitBox[0], startHitBox[1], 0);
		    break;
	    }
	}
	else {  // With ctl key down, edge being pulled should move and 
		// leave the opposite edge standing still.

	    if (currentDir == -1) {
		switch( currentState ) {
		    case PX_PY_PZ_3D_SCALE:
			interactiveCenterInBoxSpace.setValue( -1, -1, -1 );
			break;
		    case PX_PY_NZ_3D_SCALE:
			interactiveCenterInBoxSpace.setValue( -1, -1,  1 );
			break;
		    case PX_NY_PZ_3D_SCALE:
			interactiveCenterInBoxSpace.setValue( -1,  1, -1 );
			break;
		    case PX_NY_NZ_3D_SCALE:
			interactiveCenterInBoxSpace.setValue( -1,  1,  1 );
			break;
		    case NX_PY_PZ_3D_SCALE:
			interactiveCenterInBoxSpace.setValue(  1, -1, -1 );
			break;
		    case NX_PY_NZ_3D_SCALE:
			interactiveCenterInBoxSpace.setValue(  1, -1,  1 );
			break;
		    case NX_NY_PZ_3D_SCALE:
			interactiveCenterInBoxSpace.setValue(  1,  1, -1 );
			break;
		    case NX_NY_NZ_3D_SCALE:
			interactiveCenterInBoxSpace.setValue(  1,  1,  1 );
			break;
		}
	    }
	    if (currentDir == 0) {
		switch( currentState ) {
		    case PX_PY_PZ_3D_SCALE:
		    case PX_PY_NZ_3D_SCALE:
		    case PX_NY_PZ_3D_SCALE:
		    case PX_NY_NZ_3D_SCALE:
			interactiveCenterInBoxSpace.setValue(
					    -1, startHitBox[1],startHitBox[2]);
			break;
		    case NX_PY_PZ_3D_SCALE:
		    case NX_PY_NZ_3D_SCALE:
		    case NX_NY_PZ_3D_SCALE:
		    case NX_NY_NZ_3D_SCALE:
			interactiveCenterInBoxSpace.setValue( 
					    1, startHitBox[1],startHitBox[2]);
			break;
		}
	    }
	    if (currentDir == 1) {
		switch( currentState ) {
		    case PX_PY_PZ_3D_SCALE:
		    case PX_PY_NZ_3D_SCALE:
		    case NX_PY_PZ_3D_SCALE:
		    case NX_PY_NZ_3D_SCALE:
			interactiveCenterInBoxSpace.setValue(
					    startHitBox[0], -1,startHitBox[2]);
			break;
		    case PX_NY_PZ_3D_SCALE:
		    case PX_NY_NZ_3D_SCALE:
		    case NX_NY_PZ_3D_SCALE:
		    case NX_NY_NZ_3D_SCALE:
			interactiveCenterInBoxSpace.setValue(
					    startHitBox[0],  1,startHitBox[2]);
			break;
		}
	    }
	    if (currentDir == 2) {
		switch( currentState ) {
		    case PX_PY_PZ_3D_SCALE:
		    case PX_NY_PZ_3D_SCALE:
		    case NX_PY_PZ_3D_SCALE:
		    case NX_NY_PZ_3D_SCALE:
			interactiveCenterInBoxSpace.setValue(
					    startHitBox[0],startHitBox[1], -1);
			break;
		    case PX_PY_NZ_3D_SCALE:
		    case PX_NY_NZ_3D_SCALE:
		    case NX_PY_NZ_3D_SCALE:
		    case NX_NY_NZ_3D_SCALE:
			interactiveCenterInBoxSpace.setValue(
					    startHitBox[0],startHitBox[1],  1);
			break;
		}
	    }
	}
	sclCntr = getBoxPointInWorldSpace( interactiveCenterInBoxSpace );

    // Set up projector. 
    // We need to re-establish the motion line, because the line should always
    // pass through the scale center.
	lineProj->setViewVolume( getViewVolume() );
	lineProj->setLine( SbLine( sclCntr, startHit ) );

    // Get newHit in world space and box space, and also save the world 
    // version.If we need to start a new gesture, we'll carry on from this pt.
	SbVec3f newHit = lineProj->project(getNormalizedLocaterPosition());
	SbVec3f newHitBox = getWorldPointInBoxSpace( newHit );
	worldRestartPt = newHit;

#define TINY 0.00001

    // set delta to be the proportionate change in distance from 
    // the sclCntr in each direction.
    SbVec3f oldDiff = startHitBox  - interactiveCenterInBoxSpace;
    SbVec3f newDiff = newHitBox - interactiveCenterInBoxSpace;
    // If either vector is close to zero, then leave delta at 1.0
    SbVec3f delta( 1.0, 1.0, 1.0 );
    int ind;
    for ( ind = 0; ind < 3; ind++ ) {
	if ((fabs(newDiff[ind]) > TINY) && (fabs(oldDiff[ind]) > TINY))
	    delta[ind] = newDiff[ind] / oldDiff[ind];
    }
#undef TINY

    // Make sure the scale doesn't go below getMinScale()
	for (ind = 0; ind < 3; ind++ )
	    if ( delta[ind] < getMinScale() )
		delta[ind] = getMinScale();

    // Append this to the startMotionMatrix, which we saved at the beginning
    // of the drag, to find the current motion matrix.
    // We need to send our matrix that converts from boxSpace to localSpace.
	SbMatrix boxToLcl, lclToBox;
	getPartToLocalMatrix("surroundScale",boxToLcl,lclToBox);
	setMotionMatrix( 
	    appendScale(getStartMotionMatrix(), delta, 
			interactiveCenterInBoxSpace,&boxToLcl));

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Rotate box and object based on locater motion.
//
// Use: private
//
SbBool
SoTransformerDragger::rotateDrag()
//
////////////////////////////////////////////////////////////////////////
{
    // Unlike other draggers, we MUST use incremental changes, since the sphere 
    // projector don't give consistent results over long motions. Each step
    // must be fairly short. So save each previous mouse point and motion matrix

    // Set up the projector space and view.
    // Working space is space at end of motion matrix.
	sphereProj->setViewVolume( getViewVolume() );    

    // Get newHitPt in world space.
	SbVec3f newHitPt = sphereProj->project(getNormalizedLocaterPosition());
	worldRestartPt = newHitPt;

    // deltaRot is how much we rotated since last time.
        SbRotation deltaRot = sphereProj->getRotation(prevWorldHitPt, newHitPt);

    // Append this to the prevMotionMatrix, which we saved last time,
    // to find the new matrix.
    // We need to send our matrix that converts from world space to localSpace.
	SbMatrix wldToLcl = getWorldToLocalMatrix();
	SbMatrix newMotionMatrix = 
	    appendRotation( prevMotionMatrix, deltaRot, 
			getBoxPointInWorldSpace( interactiveCenterInBoxSpace),
			&wldToLcl );

    // Save the parameters we need to save:
	// Save the new hit as prevWorldHitPt for next time..
	prevWorldHitPt = newHitPt;
        // Save the incremental results of our matrix.
	prevMotionMatrix = newMotionMatrix;

    // Set the new motion matrix
        setMotionMatrix( newMotionMatrix );

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    For constrained rotation about a single axis.
//    If necessary, pick the direction of rotation based on gesture.
//    Then compare orientation of the axis of rotation to the eyepoints
//    view direction.
//    This determines whether we use disc-style rotation (like spinning a
//    turntable with your finger) or cylinder-style rotation (like rolling a 
//    rolling pin on a table)
//    Based on this decision, set up the appropriate kind of projector.
//
//    Once these decisions are made and set-up is done, subsequent 
//    calls will be funnelled directly to the proper drag routine.
//
// Use: private
//
SbBool
SoTransformerDragger::rotateConstrainedDrag()
//
////////////////////////////////////////////////////////////////////////
{
    // If we don't know which direction we've chosen for rotation:
	if (currentDir == -1) {

	    // Figure out which way we are rotating based on gesture:
	    if ( isAdequateConstraintMotion() ) {
		currentDir = getConstrainedRotationAxis();
		setHighlights();
	    }
	    else {
		// Not ready to pick a direction yet. Don't move.
		return FALSE;
	    }

	    // Compare axis of rotation to eyepoint dir to decide if
	    // we rotate as disc or cylinder:
	    rotatingAsDisc = getShouldRotateAsDisc();

	    if (rotatingAsDisc)
		initDiscProjector();
	    else
		initCylinderProjector();
	}

    // Everything should be initialized. Just call the correct method:
	if (rotatingAsDisc)
	    return rotateConstrainedDiscDrag();
	else
	    return rotateConstrainedCylindricalDrag();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Rotate SoTransformer and object based on locater motion.
SbBool
SoTransformerDragger::rotateConstrainedDiscDrag()
//
////////////////////////////////////////////////////////////////////////
{
    // Unlike other draggers, we MUST use incremental changes, since the sphere 
    // projector don't give consistent results over long motions. Each step
    // must be fairly short. So save each previous mouse point and motion matrix

    // Get startHit in world space.
	SbVec3f startHit    = getWorldStartingPoint();;
    
    // Figure out center of disc in world space.
	SbVec3f wldCenter 
		    = getBoxPointInWorldSpace( interactiveCenterInBoxSpace );

    // At this point we've got a plane projector all set up:
    // Get newHitPt in world space.
	SbVec3f newHitPt = planeProj->project( getNormalizedLocaterPosition()); 
	worldRestartPt = newHitPt;

    // To calculate rotation, we'll find angle between 
    // Last time's line from center to cursor and 
    // this time's line from center to cursor
    // (newHitPt - planeOrigin) and (prevWorldHitPt - planeOrigin)
	const SbPlane &pln = planeProj->getPlane();
	const SbVec3f &normalDir = pln.getNormal();
	SbLine  normalLine( wldCenter, wldCenter + normalDir );
	SbVec3f planeOrigin = normalLine.getClosestPoint(startHit);
	SbVec3f prevVec = prevWorldHitPt - planeOrigin;
	SbVec3f newVec  = newHitPt - planeOrigin;

    // Before finding the rotation, remove the part of these vectors that 
    // is parallel to the normal
	prevVec -= normalDir * prevVec.dot( normalDir );
	newVec  -= normalDir * newVec.dot( normalDir );
	prevVec.normalize();
	newVec.normalize();

    // deltaRot is how much we rotated since last time.
	SbRotation deltaRot = SbRotation( prevVec, newVec );

    // Append this to the prevMotionMatrix, which we saved last time,
    // to find the new matrix.
    // We need to send our matrix that converts from world space to localSpace.
	SbMatrix wldToLcl = getWorldToLocalMatrix();
	SbMatrix newMotionMatrix = 
	    appendRotation( prevMotionMatrix, deltaRot, wldCenter, &wldToLcl );

    // Save the parameters we need to save:
	// Save the new hit as prevWorldHitPt for next time..
	prevWorldHitPt = newHitPt;
        // Save the incremental results of our matrix.
	prevMotionMatrix = newMotionMatrix;

    // Set the new motion matrix
        setMotionMatrix( newMotionMatrix );

    return TRUE;
}
////////////////////////////////////////////////////////////////////////
// Description:
//    Rotate SoTransformer and object based on locater motion.
//
SbBool
SoTransformerDragger::rotateConstrainedCylindricalDrag()
//
////////////////////////////////////////////////////////////////////////
{
    // Unlike other draggers, we MUST use incremental changes, since the sphere 
    // projector don't give consistent results over long motions. Each step
    // must be fairly short. So save each previous mouse point and motion matrix

    // Figure out center of cylinder in world space.
	SbVec3f wldCenter 
		    = getBoxPointInWorldSpace( interactiveCenterInBoxSpace );

    // At this point we've got a cylindrical projector all set up:
    // Get newHitPt in world space.
	SbVec3f newHitPt = cylProj->project( getNormalizedLocaterPosition() ); 
	worldRestartPt = newHitPt;

    // deltaRot is how much we rotated since last time:
	SbRotation deltaRot = cylProj->getRotation( prevWorldHitPt, newHitPt );

    // Append this to the prevMotionMatrix, which we saved last time,
    // to find the new matrix.
    // We need to send our matrix that converts from world space to localSpace.
	SbMatrix wldToLcl = getWorldToLocalMatrix();
	SbMatrix newMotionMatrix =
	    appendRotation( prevMotionMatrix, deltaRot, wldCenter, &wldToLcl );

    // Save the parameters we need to save:
	// Save the new hit as prevWorldHitPt for next time..
	prevWorldHitPt = newHitPt;
	// Save the incremental results of our matrix.
	prevMotionMatrix = newMotionMatrix;

    // Set the new motion matrix
	setMotionMatrix( newMotionMatrix ); 

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Decides whether to do 1-D rotation as disc model (like spinning a 
//    turntable) or cylinder model (like rolling a rolling pin).
//
//    Compares the vector between the eye and the rotation center with
//    the axis of rotation. If the dot product of the two is less than
//    minDiscRotDot, then the cylindrical model is used.
//
SbBool
SoTransformerDragger::getShouldRotateAsDisc()
//
////////////////////////////////////////////////////////////////////////
{
    // Find the axis of rotation in world space
	SbVec3f worldAxis;
	if (currentDir == 0)
	    worldAxis = getBoxDirInWorldSpace(SbVec3f(1,0,0));
	else if (currentDir == 1)
	    worldAxis = getBoxDirInWorldSpace(SbVec3f(0,1,0));
	else
	    worldAxis = getBoxDirInWorldSpace(SbVec3f(0,0,1));
	worldAxis.normalize();

    // Find the direction from eye to the center of rotation:
	SbVec3f worldEye = getViewVolume().getProjectionPoint();
	SbVec3f worldCenter 
		    = getBoxPointInWorldSpace( interactiveCenterInBoxSpace );
	SbVec3f sightDir = worldCenter - worldEye;
	sightDir.normalize();

    // Compare the dot product to the minDiscRotDot field:
	if (  fabs( sightDir.dot( worldAxis )) > minDiscRotDot.getValue() )
	    return TRUE;
	else
	    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Selects a rotation axis based on mouse gesture and part picked.
//
// Use: private
//
int
SoTransformerDragger::getConstrainedRotationAxis()
//
////////////////////////////////////////////////////////////////////////
{
    // Each rotate knob has 2 possible mouse motion directions.
    // The mouse motion will cause rotation about a different
    // axis, which we set as currentDir.
    // For example, mouseDir of z on the x knob rotates in Y.

    int mouseDir;
    int rotAxis = -1;
    switch( currentState ) {
	case RIT_X_ROTATE: case LFT_X_ROTATE: 
	    mouseDir = getMouseGestureDirection(0,1,1);
	    rotAxis = (mouseDir == 1) ? 2 : 1;
	    break;
	case TOP_Y_ROTATE: case BOT_Y_ROTATE: 
	    mouseDir = getMouseGestureDirection(1,0,1);
	    rotAxis = (mouseDir == 0) ? 2 : 0;
	    break;
	case FNT_Z_ROTATE: case BAK_Z_ROTATE:
	    mouseDir = getMouseGestureDirection(1,1,0);
	    rotAxis = (mouseDir == 0) ? 1 : 0;
	    break;
    }
    return rotAxis;
}

////////////////////////////////////////////////////////////////////////
// keypress/release callback functions
//
// These assure that the proper changes to the highlights, 
// currentState, and projectors are made
//
////////////////////////////////////////////////////////////////////////
//

void
SoTransformerDragger::metaKeyChangeCB( void *, SoDragger *inDragger)
{
    SoTransformerDragger  *hb = (SoTransformerDragger *) inDragger;
    SoHandleEventAction *ha = hb->getHandleEventAction();

    //[1] Only do this if we are grabbing events
    if ( ha->getGrabber() != hb )
	return;

    //[2] We only want key press or release events.
    const SoEvent *event = hb->getEvent();
    if ( !SO_KEY_PRESS_EVENT(event, ANY) && !SO_KEY_RELEASE_EVENT(event, ANY))
	return;

    //[3] Is the key constrain, modify, or control?
    const SoKeyboardEvent *ke = (const SoKeyboardEvent *) event;
    SoKeyboardEvent::Key key = ke->getKey();

    if ( key == SoKeyboardEvent::LEFT_CONTROL  ||
	 key == SoKeyboardEvent::RIGHT_CONTROL || 
	 key == SoKeyboardEvent::LEFT_SHIFT ||
	 key == SoKeyboardEvent::RIGHT_SHIFT ) {

	// We want to end the old gesture and start a new one.

	// [A] Release the grabber. This ends the gesture and calls all 
	//     finishCallbacks (on parent dragger, too, if we're registered)
	//     Remember the state so you can restart with it after releasing.
	    State savedState = hb->currentState;
	    ha->releaseGrabber();

	// [B] Set the starting point to be our saved worldRestartPoint
	    hb->restartState = savedState;
	    hb->setStartingPoint( hb->worldRestartPt );

	// [C] Become the grabber again. This begins a new gesture and calls all
	//     startCallbacks (parent dragger, too).  Info like viewVolume, 
	//     viewportRegion, handleEventAction, and tempPathToThis 
	//     is still valid.
	    ha->setGrabber(hb);

	// [D] set handled
	    ha->setHandled();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Finish dragging.
//
// Use: private
//
void
SoTransformerDragger::dragFinish()
//
////////////////////////////////////////////////////////////////////////
{
    // We will unsquish the knobs at the completion of any 
    // scale or rotational dragging.
    // We'll only do it on mouse-up events. Note that dragFinish can be
    // called in response to a meta-key press/release event. In this case,
    // we're starting a new gesture that's part of one long click-drag-release
    // cycle. For efficient interaction speeds, we will NOT unsquish 
    // in the meta-key case.
    const SoEvent *ev = getEvent();
    if ( SO_MOUSE_RELEASE_EVENT( ev, BUTTON1 ) ) {
	switch( currentState ) {
	    case RIT_X_ROTATE: case  TOP_Y_ROTATE: case  FNT_Z_ROTATE: 
	    case LFT_X_ROTATE: case  BOT_Y_ROTATE: case  BAK_Z_ROTATE: 
		// Rotating can re-orient the dragger beneath a non-uniform 
		// scale, changing the squish of the knobs.
		unsquishKnobs();
		break;
	    case PX_PY_PZ_3D_SCALE: case PX_PY_NZ_3D_SCALE: 
	    case PX_NY_PZ_3D_SCALE: case PX_NY_NZ_3D_SCALE: 
	    case NX_PY_PZ_3D_SCALE: case NX_PY_NZ_3D_SCALE: 
	    case NX_NY_PZ_3D_SCALE: case NX_NY_NZ_3D_SCALE: 
		    unsquishKnobs();
		break;
	    case RIT_TRANSLATE: case LFT_TRANSLATE: case TOP_TRANSLATE:
	    case BOT_TRANSLATE: case FNT_TRANSLATE: case BAK_TRANSLATE:
	    case INACTIVE: 
		// Translation will not change the relative dimensions of knobs
	    default:
		break;
	}
    }

    currentState = INACTIVE;
    restartState = INACTIVE;
    setHighlights();

    // Invalidate surroundScale if it exists.
    SoSurroundScale *ss = SO_CHECK_PART(this, "surroundScale", SoSurroundScale);
    if (ss != NULL)
	ss->invalidate();
}; 


////////////////////////////////////////////////////////////////////////
//
// Use: protected
//
void
SoTransformerDragger::setAllPartSwitches( int     scaleAssemblyWhich,
				  int    rotateAssemblyWhich,
				  int translateAssemblyWhich )
								
//
////////////////////////////////////////////////////////////////////////
{
    setSwitchValue(translator1Switch.getValue(), translateAssemblyWhich );
    setSwitchValue(translator2Switch.getValue(), translateAssemblyWhich );
    setSwitchValue(translator3Switch.getValue(), translateAssemblyWhich );
    setSwitchValue(translator4Switch.getValue(), translateAssemblyWhich );
    setSwitchValue(translator5Switch.getValue(), translateAssemblyWhich );
    setSwitchValue(translator6Switch.getValue(), translateAssemblyWhich );

    setSwitchValue(rotator1Switch.getValue(), rotateAssemblyWhich );
    setSwitchValue(rotator2Switch.getValue(), rotateAssemblyWhich );
    setSwitchValue(rotator3Switch.getValue(), rotateAssemblyWhich );
    setSwitchValue(rotator4Switch.getValue(), rotateAssemblyWhich );
    setSwitchValue(rotator5Switch.getValue(), rotateAssemblyWhich );
    setSwitchValue(rotator6Switch.getValue(), rotateAssemblyWhich );

    setSwitchValue(scale1Switch.getValue(), scaleAssemblyWhich );
    setSwitchValue(scale2Switch.getValue(), scaleAssemblyWhich );
    setSwitchValue(scale3Switch.getValue(), scaleAssemblyWhich );
    setSwitchValue(scale4Switch.getValue(), scaleAssemblyWhich );
    setSwitchValue(scale5Switch.getValue(), scaleAssemblyWhich );
    setSwitchValue(scale6Switch.getValue(), scaleAssemblyWhich );
    setSwitchValue(scale7Switch.getValue(), scaleAssemblyWhich );
    setSwitchValue(scale8Switch.getValue(), scaleAssemblyWhich );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the various switch nodes to highlight the correct parts
//    of the box, depending on the state.
//
// Use: private
//
void
SoTransformerDragger::setHighlights()
//
////////////////////////////////////////////////////////////////////////
{
    // Turn notification off temporarily so that
    // all these switches switching dont cause a zillion
    // notifies.
    SbBool wasEnabled = enableNotify(FALSE);
    
    setFeedback();

    // if switching everything off, just do it and return.
    switch ( currentState ) {
	case RIT_TRANSLATE:
	case LFT_TRANSLATE:
	case TOP_TRANSLATE:
	case BOT_TRANSLATE:
	case FNT_TRANSLATE:
	case BAK_TRANSLATE:
	    // Turn off all translate,scale, and rot parts.
	    // We'll show only the feedback we created in setFeedback()
    	    setAllPartSwitches(SO_SWITCH_NONE, SO_SWITCH_NONE, SO_SWITCH_NONE); 
   	    enableNotify(wasEnabled);
	    touch();
	    return;
    }

    // The other states require some things on, some things off.

    switch ( currentState ) {

	case PX_PY_PZ_3D_SCALE:
    	    setAllPartSwitches( 0, SO_SWITCH_NONE, SO_SWITCH_NONE );
	    setSwitchValue( scale1Switch.getValue(), 1 );
	    break;
	case PX_PY_NZ_3D_SCALE:
    	    setAllPartSwitches( 0, SO_SWITCH_NONE, SO_SWITCH_NONE );
	    setSwitchValue( scale2Switch.getValue(), 1 );
	    break;
	case PX_NY_PZ_3D_SCALE:
    	    setAllPartSwitches( 0, SO_SWITCH_NONE, SO_SWITCH_NONE );
	    setSwitchValue( scale3Switch.getValue(), 1 );
	    break;
	case PX_NY_NZ_3D_SCALE:
    	    setAllPartSwitches( 0, SO_SWITCH_NONE, SO_SWITCH_NONE );
	    setSwitchValue( scale4Switch.getValue(), 1 );
	    break;
	case NX_PY_PZ_3D_SCALE:
    	    setAllPartSwitches( 0, SO_SWITCH_NONE, SO_SWITCH_NONE );
	    setSwitchValue( scale5Switch.getValue(), 1 );
	    break;
	case NX_PY_NZ_3D_SCALE:
    	    setAllPartSwitches( 0, SO_SWITCH_NONE, SO_SWITCH_NONE );
	    setSwitchValue( scale6Switch.getValue(), 1 );
	    break;
	case NX_NY_PZ_3D_SCALE:
    	    setAllPartSwitches( 0, SO_SWITCH_NONE, SO_SWITCH_NONE );
	    setSwitchValue( scale7Switch.getValue(), 1 );
	    break;
	case NX_NY_NZ_3D_SCALE:
    	    setAllPartSwitches( 0, SO_SWITCH_NONE, SO_SWITCH_NONE );
	    setSwitchValue( scale8Switch.getValue(), 1 );
	    break;

	case RIT_X_ROTATE:
	case LFT_X_ROTATE:
    	    setAllPartSwitches( SO_SWITCH_NONE, 0, SO_SWITCH_NONE );
	    setSwitchValue( rotator4Switch.getValue(), 1 );
	    setSwitchValue( rotator3Switch.getValue(), 1 );
	    break;
	case TOP_Y_ROTATE:
	case BOT_Y_ROTATE:
    	    setAllPartSwitches( SO_SWITCH_NONE, 0, SO_SWITCH_NONE );
	    setSwitchValue( rotator1Switch.getValue(), 1 );
	    setSwitchValue( rotator2Switch.getValue(), 1 );
	    break;
	case FNT_Z_ROTATE:
	case BAK_Z_ROTATE:
    	    setAllPartSwitches( SO_SWITCH_NONE, 0, SO_SWITCH_NONE );
	    setSwitchValue( rotator5Switch.getValue(), 1 );
	    setSwitchValue( rotator6Switch.getValue(), 1 );
	    break;

	case INACTIVE:
	default:
    	    setAllPartSwitches( 0, 0, 0 );
	    break;
    }

    // Turn notification back on and cause one notify
    // to eminate from the dragger
    enableNotify(wasEnabled);
    touch();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Turns off all feedback, then 
//    calls appropriate routine to turn on appropriate feedback.
//
// Use: private
//
void
SoTransformerDragger::setFeedback()
//
////////////////////////////////////////////////////////////////////////
{
    SbBool wasEnabled = enableNotify(FALSE);

    // Start by flipping off all 3 switches.
	setSwitchValue( xAxisFeedbackSwitch.getValue(), SO_SWITCH_NONE  );
	setSwitchValue( yAxisFeedbackSwitch.getValue(), SO_SWITCH_NONE  );
	setSwitchValue( zAxisFeedbackSwitch.getValue(), SO_SWITCH_NONE  );
	setSwitchValue(translateBoxFeedbackSwitch.getValue(), SO_SWITCH_NONE  );
	setSwitchValue(scaleBoxFeedbackSwitch.getValue(), SO_SWITCH_NONE  );
	setSwitchValue(posXWallFeedbackSwitch.getValue(), SO_SWITCH_NONE  );
	setSwitchValue(posYWallFeedbackSwitch.getValue(), SO_SWITCH_NONE  );
	setSwitchValue(posZWallFeedbackSwitch.getValue(), SO_SWITCH_NONE  );
	setSwitchValue(negXWallFeedbackSwitch.getValue(), SO_SWITCH_NONE  );
	setSwitchValue(negYWallFeedbackSwitch.getValue(), SO_SWITCH_NONE  );
	setSwitchValue(negZWallFeedbackSwitch.getValue(), SO_SWITCH_NONE  );
	setSwitchValue(radialFeedbackSwitch.getValue(), SO_SWITCH_NONE  );
        setSwitchValue(circleFeedbackTransformSwitch.getValue(),SO_SWITCH_NONE);
	setSwitchValue(xCircleFeedbackSwitch.getValue(), SO_SWITCH_NONE  );
	setSwitchValue(yCircleFeedbackSwitch.getValue(), SO_SWITCH_NONE  );
	setSwitchValue(zCircleFeedbackSwitch.getValue(), SO_SWITCH_NONE  );

    // Go to appropriate subroutine.
	switch ( currentState ) {
	    case RIT_TRANSLATE: case LFT_TRANSLATE: case TOP_TRANSLATE: 
	    case BOT_TRANSLATE: case FNT_TRANSLATE: case BAK_TRANSLATE:
		setFeedbackForTranslate();
		break;
	    case PX_PY_PZ_3D_SCALE: case PX_PY_NZ_3D_SCALE:
	    case PX_NY_PZ_3D_SCALE: case PX_NY_NZ_3D_SCALE:
	    case NX_PY_PZ_3D_SCALE: case NX_PY_NZ_3D_SCALE:
	    case NX_NY_PZ_3D_SCALE: case NX_NY_NZ_3D_SCALE:
		setFeedbackForScale();
		break;
	    case RIT_X_ROTATE: case LFT_X_ROTATE: case TOP_Y_ROTATE:
	    case BOT_Y_ROTATE: case FNT_Z_ROTATE: case BAK_Z_ROTATE:
		setFeedbackForRotate();
		break;
	    default:
		break;
	}

    enableNotify(wasEnabled);
    touch();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Normally displays two axes.
//    If constrained, displays only one.
//
// Use: private
//
void
SoTransformerDragger::setFeedbackForTranslate()
//
////////////////////////////////////////////////////////////////////////
{
    // Turn on the feedback box and orient it correctly.
    // It is a purple box with a yellow top.  The top will be aligned by 
    // the dragger to match the plane of motion
	setSwitchValue( translateBoxFeedbackSwitch.getValue(), SO_SWITCH_ALL  );
	SoRotation *boxRot = (SoRotation *)translateBoxFeedbackRotation.getValue();
	if (!boxRot) {
	    boxRot = new SoRotation;
	    setAnyPart("translateBoxFeedbackRotation", boxRot );
	}
	SbVec3f topDir;
	switch ( currentState ) {
	    case RIT_TRANSLATE: topDir.setValue( 1, 0, 0); break;
	    case LFT_TRANSLATE: topDir.setValue(-1, 0, 0); break;
	    case TOP_TRANSLATE: topDir.setValue( 0, 1, 0); break;
	    case BOT_TRANSLATE: topDir.setValue( 0,-1, 0); break;
	    case FNT_TRANSLATE: topDir.setValue( 0, 0, 1); break;
	    case BAK_TRANSLATE: topDir.setValue( 0, 0,-1); break;
	}
	boxRot->rotation = SbRotation( SbVec3f(0,1,0), topDir );


    // If translating constrained, turn on constrained axis in active state.
    if ( currentDir == 0 )
	setSwitchValue( xAxisFeedbackSwitch.getValue(), 0  );
    else if ( currentDir == 1 )
	setSwitchValue( yAxisFeedbackSwitch.getValue(), 0  );
    else if ( currentDir == 2 )
	setSwitchValue( zAxisFeedbackSwitch.getValue(), 0  );
    else {
	// If unconstrained, turn on either two axes in plane or perp axis:
	// If shiftkey down, we show child '2' for selecting an axis,
	// otherwise show child '1'
	// If <Control> down, also show small crosshairs in other 2 dirs to 
	// ground the feedback in the plane. Index 3 means crosshairs
	int kid = (constraining) ? 1 : 0;
	switch ( currentState ) {
	    case RIT_TRANSLATE: case LFT_TRANSLATE: 
		if (ctlDown) {
		    setSwitchValue( xAxisFeedbackSwitch.getValue(), 0  );
		    setSwitchValue( yAxisFeedbackSwitch.getValue(), 2  );
		    setSwitchValue( zAxisFeedbackSwitch.getValue(), 2  );
		}
		else {
		    setSwitchValue( yAxisFeedbackSwitch.getValue(), kid  );
		    setSwitchValue( zAxisFeedbackSwitch.getValue(), kid  );
		}
		break;
	    case TOP_TRANSLATE: case BOT_TRANSLATE: 
		if (ctlDown) {
		    setSwitchValue( yAxisFeedbackSwitch.getValue(), 0  );
		    setSwitchValue( xAxisFeedbackSwitch.getValue(), 2  );
		    setSwitchValue( zAxisFeedbackSwitch.getValue(), 2  );
		}
		else {
		    setSwitchValue( xAxisFeedbackSwitch.getValue(), kid  );
		    setSwitchValue( zAxisFeedbackSwitch.getValue(), kid  );
		}
		break;
	    case FNT_TRANSLATE: case BAK_TRANSLATE:
		if (ctlDown) {
		    setSwitchValue( zAxisFeedbackSwitch.getValue(), 0  );
		    setSwitchValue( xAxisFeedbackSwitch.getValue(), 2  );
		    setSwitchValue( yAxisFeedbackSwitch.getValue(), 2  );
		}
		else {
		    setSwitchValue( xAxisFeedbackSwitch.getValue(), kid  );
		    setSwitchValue( yAxisFeedbackSwitch.getValue(), kid  );
		}
		break;
	}
    }

// Finally, move the axes to lay beneath the cursor:
    SoTranslation *axLoc = (SoTranslation *)axisFeedbackLocation.getValue();
    if (!axLoc) {
	axLoc = new SoTranslation;
	setAnyPart("axisFeedbackLocation", axLoc );
    }
    axLoc->translation = getWorldPointInBoxSpace( getWorldStartingPoint() );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Normally displays radial feedback, connecting center to corner.
// 
//    If constraining and decided, displays one axis.
//
//    If constraining but undecided, displays all three axes centered at corner.
//
// Use: private
//
void
SoTransformerDragger::setFeedbackForScale()
//
////////////////////////////////////////////////////////////////////////
{
    if ( constraining ) {
	// Turn on the purple-lined box
	setSwitchValue( scaleBoxFeedbackSwitch.getValue(), SO_SWITCH_ALL  );
    }

    if ( ctlDown ) {
	// Show walls that will stay still during scaling

	// The wall opposite a corner will be shown if that corner is picked.
	// If in addition (currentDir != -1), then the currentDir must be in
	// the dirction perpendicular to that wall if it is to be shown.
	SoTransformerDragger::State cS = currentState;
	if ( cS == NX_PY_PZ_3D_SCALE || cS == NX_PY_NZ_3D_SCALE ||
	     cS == NX_NY_PZ_3D_SCALE || cS == NX_NY_NZ_3D_SCALE ) {
	    if (currentDir == -1 || currentDir == 0)
		setSwitchValue( posXWallFeedbackSwitch.getValue(), 0  );
	}
	if ( cS == PX_NY_PZ_3D_SCALE || cS == PX_NY_NZ_3D_SCALE ||
	     cS == NX_NY_PZ_3D_SCALE || cS == NX_NY_NZ_3D_SCALE ) {
	    if (currentDir == -1 || currentDir == 1)
		setSwitchValue( posYWallFeedbackSwitch.getValue(), 0  );
	}
	if ( cS == PX_PY_NZ_3D_SCALE || cS == PX_NY_NZ_3D_SCALE ||
	     cS == NX_PY_NZ_3D_SCALE || cS == NX_NY_NZ_3D_SCALE ) {
	    if (currentDir == -1 || currentDir == 2)
		setSwitchValue( posZWallFeedbackSwitch.getValue(), 0  );
	}
	if ( cS == PX_PY_PZ_3D_SCALE || cS == PX_PY_NZ_3D_SCALE ||
	     cS == PX_NY_PZ_3D_SCALE || cS == PX_NY_NZ_3D_SCALE ) {
	    if (currentDir == -1 || currentDir == 0)
		setSwitchValue( negXWallFeedbackSwitch.getValue(), 0  );
	}
	if ( cS == PX_PY_PZ_3D_SCALE || cS == PX_PY_NZ_3D_SCALE ||
	     cS == NX_PY_PZ_3D_SCALE || cS == NX_PY_NZ_3D_SCALE ) {
	    if (currentDir == -1 || currentDir == 1)
		setSwitchValue( negYWallFeedbackSwitch.getValue(), 0  );
	}
	if ( cS == PX_PY_PZ_3D_SCALE || cS == PX_NY_PZ_3D_SCALE ||
	     cS == NX_PY_PZ_3D_SCALE || cS == NX_NY_PZ_3D_SCALE ) {
	    if (currentDir == -1 || currentDir == 2)
		setSwitchValue( negZWallFeedbackSwitch.getValue(), 0  );
	}
    }

    if ( ! constraining ) {
        //  If not constrained, displays radial feedback, 
	//  connecting center to corner.
	setSwitchValue( radialFeedbackSwitch.getValue(), 0  );
    }
    else if ( currentDir == 0 ) {
        // constrained to x-scale. Index 1 is for highlighted version
	setSwitchValue( xAxisFeedbackSwitch.getValue(), 0  );
    }
    else if ( currentDir == 1 ) {
        // constrained to y-scale. Index 1 is for highlighted version
	setSwitchValue( yAxisFeedbackSwitch.getValue(), 0  );
    }
    else if ( currentDir == 2 ) {
        // constrained to z-scale. Index 1 is for highlighted version
	setSwitchValue( zAxisFeedbackSwitch.getValue(), 0  );
    }
    else {
	// constraining but undecided, displays all 3 axes centered at corner.
	// Index 2 is for selction of axis version of feedback.
	setSwitchValue( xAxisFeedbackSwitch.getValue(), 1  );
	setSwitchValue( yAxisFeedbackSwitch.getValue(), 1  );
	setSwitchValue( zAxisFeedbackSwitch.getValue(), 1  );
    }

    // Figure out which corner we're at:
    SbVec3f cornerLoc;
    switch ( currentState ) {
	case PX_PY_PZ_3D_SCALE: 
	    cornerLoc.setValue( 1, 1, 1 );
	    break;
	case PX_PY_NZ_3D_SCALE:
	    cornerLoc.setValue( 1, 1,-1 );
	    break;
	case PX_NY_PZ_3D_SCALE: 
	    cornerLoc.setValue( 1,-1, 1 );
	    break;
	case PX_NY_NZ_3D_SCALE:
	    cornerLoc.setValue( 1,-1,-1 );
	    break;
	case NX_PY_PZ_3D_SCALE: 
	    cornerLoc.setValue(-1, 1, 1 );
	    break;
	case NX_PY_NZ_3D_SCALE:
	    cornerLoc.setValue(-1, 1,-1 );
	    break;
	case NX_NY_PZ_3D_SCALE: 
	    cornerLoc.setValue(-1,-1, 1 );
	    break;
	case NX_NY_NZ_3D_SCALE:
	    cornerLoc.setValue(-1,-1,-1 );
	    break;
    }

    // Finally, position feedback if needed:
    if ( constraining ) {
	// Place axis feedback at corner.
	SoTranslation *axLoc = (SoTranslation *)axisFeedbackLocation.getValue();
	if (!axLoc) {
	    axLoc = new SoTranslation;
	    setAnyPart("axisFeedbackLocation", axLoc );
	}
	axLoc->translation = cornerLoc;
    }

}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Normally displays all three rings.
// 
//    If constraining and decided, displays one ring.
//
//    If constraining but undecided, displays two rings to choose from.
//
// Use: private
//
void
SoTransformerDragger::setFeedbackForRotate()
//
////////////////////////////////////////////////////////////////////////
{
    // Show the antiSquish node whenever we display rotate feedback
    setSwitchValue(circleFeedbackTransformSwitch.getValue(), SO_SWITCH_ALL );

    // Set the sizing to be based on the knob that was selected.
    SoAntiSquish *myAS = (SoAntiSquish *) circleFeedbackAntiSquish.getValue();
    if (myAS) {
	myAS->recalc();
	switch ( currentState ) {
	    case RIT_X_ROTATE: case LFT_X_ROTATE: 
		myAS->sizing = SoAntiSquish::X;
		break;
	    case TOP_Y_ROTATE: case BOT_Y_ROTATE: 
		myAS->sizing = SoAntiSquish::Y;
		break;
	    case FNT_Z_ROTATE: case BAK_Z_ROTATE:
		myAS->sizing = SoAntiSquish::Z;
		break;
	}
    }

    // If the ctl key is down, we rotate about opposite side, so 
    // scale up the circles and center about the opposite end.
    SoTransform *myXf = (SoTransform *) circleFeedbackTransform.getValue();
    if (myXf) {
	if (ctlDown) {
	    switch ( currentState ) {
		case RIT_X_ROTATE: 
		    myXf->translation.setValue(-1,0,0);
		    break;
		case LFT_X_ROTATE: 
		    myXf->translation.setValue(1,0,0);
		    break;
		case TOP_Y_ROTATE: 
		    myXf->translation.setValue(0,-1,0);
		    break;
		case BOT_Y_ROTATE: 
		    myXf->translation.setValue(0,1,0);
		    break;
		case FNT_Z_ROTATE: 
		    myXf->translation.setValue(0,0,-1);
		    break;
		case BAK_Z_ROTATE:
		    myXf->translation.setValue(0,0,1);
		    break;
	    }
	    myXf->scaleFactor.setValue(1.8,1.8,1.8);
	}
	else {
	    myXf->translation.setValue(0,0,0);
	    myXf->scaleFactor.setValue(1,1,1);
	}
    }

    // Show ring feedback
    if ( ! constraining ) {
        //  If not constrained, displays all three rings.
	setSwitchValue( xCircleFeedbackSwitch.getValue(), 0  );
	setSwitchValue( yCircleFeedbackSwitch.getValue(), 0  );
	setSwitchValue( zCircleFeedbackSwitch.getValue(), 0  );
    }
    if ( currentDir == 0 ) {
        // constrained to x-rotation
	setSwitchValue( xCircleFeedbackSwitch.getValue(), 0  );
    }
    else if ( currentDir == 1 ) {
        // constrained to y-rotation
	setSwitchValue( yCircleFeedbackSwitch.getValue(), 0  );
    }
    else if ( currentDir == 2 ) {
        // constrained to z-rotation
	setSwitchValue( zCircleFeedbackSwitch.getValue(), 0  );
    }
    else {
	// Show only the two circles that can be selected with this knob.
	switch ( currentState ) {
	    case RIT_X_ROTATE: case LFT_X_ROTATE: 
		setSwitchValue( yCircleFeedbackSwitch.getValue(), 0  );
		setSwitchValue( zCircleFeedbackSwitch.getValue(), 0  );
		break;
	    case TOP_Y_ROTATE: case BOT_Y_ROTATE: 
		setSwitchValue( xCircleFeedbackSwitch.getValue(), 0  );
		setSwitchValue( zCircleFeedbackSwitch.getValue(), 0  );
		break;
	    case FNT_Z_ROTATE: case BAK_Z_ROTATE:
		setSwitchValue( xCircleFeedbackSwitch.getValue(), 0  );
		setSwitchValue( yCircleFeedbackSwitch.getValue(), 0  );
		break;
	}
    }

    if ( ctlDown ) {
	// Show walls that will stay still during rotating

	// The wall opposite a knob will be shown if that knob is picked.
	switch ( currentState ) {
	    case RIT_X_ROTATE: 
		setSwitchValue( negXWallFeedbackSwitch.getValue(), 1  );
		break;
	    case LFT_X_ROTATE: 
		setSwitchValue( posXWallFeedbackSwitch.getValue(), 1  );
		break;
	    case TOP_Y_ROTATE:
		setSwitchValue( negYWallFeedbackSwitch.getValue(), 1  );
		break;
	    case BOT_Y_ROTATE: 
		setSwitchValue( posYWallFeedbackSwitch.getValue(), 1  );
		break;
	    case FNT_Z_ROTATE: 
		setSwitchValue( negZWallFeedbackSwitch.getValue(), 1  );
		break;
	    case BAK_Z_ROTATE:
		setSwitchValue( posZWallFeedbackSwitch.getValue(), 1  );
		break;
	}
    }

    // Show axis feedback
    if ( constraining ) {
	// constraining but undecided, displays the two appropriate axes.
	if ( currentDir == -1 ) {
	    // Index 2 means selection of axis color
	    switch ( currentState ) {
		case RIT_X_ROTATE: case LFT_X_ROTATE: 
		    setSwitchValue( yAxisFeedbackSwitch.getValue(), 1  );
		    setSwitchValue( zAxisFeedbackSwitch.getValue(), 1  );
		    break;
		case TOP_Y_ROTATE: case BOT_Y_ROTATE: 
		    setSwitchValue( xAxisFeedbackSwitch.getValue(), 1  );
		    setSwitchValue( zAxisFeedbackSwitch.getValue(), 1  );
		    break;
		case FNT_Z_ROTATE: case BAK_Z_ROTATE:
		    setSwitchValue( xAxisFeedbackSwitch.getValue(), 1  );
		    setSwitchValue( yAxisFeedbackSwitch.getValue(), 1  );
		    break;
	    }
	}
	else {
	    // Index 1 means highlight axis color
	    switch ( currentState ) {
		case RIT_X_ROTATE: case LFT_X_ROTATE: 
		    if ( currentDir == 1 )
			setSwitchValue( zAxisFeedbackSwitch.getValue(), 0  );
		    else
			setSwitchValue( yAxisFeedbackSwitch.getValue(), 0  );
		    break;
		case TOP_Y_ROTATE: case BOT_Y_ROTATE: 
		    if ( currentDir == 0 )
			setSwitchValue( zAxisFeedbackSwitch.getValue(), 0  );
		    else
			setSwitchValue( xAxisFeedbackSwitch.getValue(), 0  );
		    break;
		case FNT_Z_ROTATE: case BAK_Z_ROTATE:
		    if ( currentDir == 0 )
			setSwitchValue( yAxisFeedbackSwitch.getValue(), 0  );
		    else
			setSwitchValue( xAxisFeedbackSwitch.getValue(), 0  );
		    break;
	    }
	}


	// Finally, position the feedback at the right knob:
	    SoTranslation *loc 
		= (SoTranslation *)axisFeedbackLocation.getValue();
	    if (!loc) {
		loc = new SoTranslation;
		setAnyPart("axisFeedbackLocation", loc );
	    }

	// Figure out which corner we're at:
	switch ( currentState ) {
	    case RIT_X_ROTATE: 
		loc->translation.setValue( 1.25, 0, 0 );
		break;
	    case LFT_X_ROTATE: 
		loc->translation.setValue(-1.25, 0, 0 );
		break;
	    case TOP_Y_ROTATE: 
		loc->translation.setValue( 0, 1.25, 0 );
		break;
	    case BOT_Y_ROTATE: 
		loc->translation.setValue( 0,-1.25, 0 );
		break;
	    case FNT_Z_ROTATE: 
		loc->translation.setValue( 0, 0, 1.25 );
		break;
	    case BAK_Z_ROTATE:
		loc->translation.setValue( 0, 0,-1.25 );
		break;
	}
    }
}

////////////////////////////////////////////////////////////////////
//  Stubs for callbacks
////////////////////////////////////////////////////////////////////
void 
SoTransformerDragger::startCB( void *, SoDragger *inDragger )
{
    SoTransformerDragger *hb = (SoTransformerDragger *) inDragger;
    hb->dragStart();
}

void 
SoTransformerDragger::motionCB( void *, SoDragger *inDragger )
{
    SoTransformerDragger *hb = (SoTransformerDragger *) inDragger;
    hb->drag();
}

void 
SoTransformerDragger::finishCB( void *, SoDragger *inDragger )
{
    SoTransformerDragger *hb = (SoTransformerDragger *) inDragger;
    hb->dragFinish();
}

void
SoTransformerDragger::valueChangedCB( void *, SoDragger *inDragger )
{
    SoTransformerDragger *m = (SoTransformerDragger *) inDragger;
    SbMatrix motMat = m->getMotionMatrix();

    SbVec3f trans, scale;
    SbRotation rot, scaleOrient;
    getTransformFast( motMat, trans, rot, scale, scaleOrient);

    // Disconnect the field sensors
    m->translFieldSensor->detach();
    m->scaleFieldSensor->detach();
    m->rotateFieldSensor->detach();

    if ( m->translation.getValue() != trans )
	m->translation = trans;
    if ( m->scaleFactor.getValue() != scale )
	m->scaleFactor = scale;
    if ( m->rotation.getValue() != rot )
	m->rotation = rot;

    // Reconnect the field sensors
    m->translFieldSensor->attach( &(m->translation) );
    m->scaleFieldSensor->attach( &(m->scaleFactor) );
    m->rotateFieldSensor->attach( &(m->rotation) );
}

void
SoTransformerDragger::fieldSensorCB( void *inDragger, SoSensor * )
{
    SoTransformerDragger *dragger = (SoTransformerDragger *) inDragger;

    // Incorporate the new field values into the matrix 
    SbMatrix motMat = dragger->getMotionMatrix();
    dragger->workFieldsIntoTransform(motMat);

    dragger->setMotionMatrix( motMat );
}

void
SoTransformerDragger::setDefaultOnNonWritingFields()
{
    // These nodes may change after construction, but we still
    // don't want to write them out.

    // The locate highlight groups:
	translator1LocateGroup.setDefault(TRUE);
	translator2LocateGroup.setDefault(TRUE);
	translator3LocateGroup.setDefault(TRUE);
	translator4LocateGroup.setDefault(TRUE);
	translator5LocateGroup.setDefault(TRUE);
	translator6LocateGroup.setDefault(TRUE);

	rotator1LocateGroup.setDefault(TRUE);
	rotator2LocateGroup.setDefault(TRUE);
	rotator3LocateGroup.setDefault(TRUE);
	rotator4LocateGroup.setDefault(TRUE);
	rotator5LocateGroup.setDefault(TRUE);
	rotator6LocateGroup.setDefault(TRUE);

	scale1LocateGroup.setDefault(TRUE);
	scale2LocateGroup.setDefault(TRUE);
	scale3LocateGroup.setDefault(TRUE);
	scale4LocateGroup.setDefault(TRUE);
	scale5LocateGroup.setDefault(TRUE);
	scale6LocateGroup.setDefault(TRUE);
	scale7LocateGroup.setDefault(TRUE);
	scale8LocateGroup.setDefault(TRUE);

    surroundScale.setDefault(TRUE);
    axisFeedbackLocation.setDefault(TRUE);
    circleFeedbackAntiSquish.setDefault(TRUE);
    circleFeedbackTransform.setDefault(TRUE);
    translateBoxFeedbackRotation.setDefault(TRUE);

    // Call the base class...
    SoDragger::setDefaultOnNonWritingFields();
}

// Finds all SoAntiSquish nodes contained within rotate or scale knobs
// in subgraph and puts them in list.
void 
SoTransformerDragger::updateAntiSquishList()
{
    ref();

    SbBool wasSearching  = SoBaseKit::isSearchingChildren();
    SoBaseKit::setSearchingChildren(TRUE);

    SoSearchAction *sa = new SoSearchAction;
    sa->setType(SoAntiSquish::getClassTypeId());
    sa->setSearchingAll(TRUE);
    sa->setInterest(SoSearchAction::ALL);
    sa->apply(this);

    const SoPathList &paths = sa->getPaths();
    SoAntiSquish *sq;
    for (int i = 0; i < paths.getLength(); i++) {
	// Only use this antisquish if it lies under a rotator or scale
	// switch.
	SoFullPath *fp = (SoFullPath *) paths[i];
	if ( fp->containsNode( rotator1Switch.getValue() ) ||
	     fp->containsNode( rotator2Switch.getValue() ) ||
	     fp->containsNode( rotator3Switch.getValue() ) ||
	     fp->containsNode( rotator4Switch.getValue() ) ||
	     fp->containsNode( rotator5Switch.getValue() ) ||
	     fp->containsNode( rotator6Switch.getValue() ) ||

	     fp->containsNode( scale1Switch.getValue() ) ||
	     fp->containsNode( scale2Switch.getValue() ) ||
	     fp->containsNode( scale3Switch.getValue() ) ||
	     fp->containsNode( scale4Switch.getValue() ) ||
	     fp->containsNode( scale5Switch.getValue() ) ||
	     fp->containsNode( scale6Switch.getValue() ) ||
	     fp->containsNode( scale7Switch.getValue() ) ||
	     fp->containsNode( scale8Switch.getValue() )) {

	    sq = (SoAntiSquish * ) fp->getTail();

	    antiSquishList.append( sq );
	}
    }

    SoBaseKit::setSearchingChildren(wasSearching);

    delete sa;

    unrefNoDelete();
}

// Tells all nodes in the antiSquishList to recalculate next time through.
void 
SoTransformerDragger::unsquishKnobs()
{
    int num = antiSquishList.getLength();
    if ( num == 0 )
	return;

    SoNode *topSep = topSeparator.getValue();
    if (topSep == NULL)
	return;

    SbBool wasEnabled = topSep->enableNotify(FALSE);

    for (int i = 0; i < num; i++ ) {
	SoAntiSquish *as = (SoAntiSquish * ) antiSquishList[i];
	as->recalc();
    }

    topSep->enableNotify(wasEnabled);
    topSep->touch();
}


// Turns on/off locate highlighting on all locate highlight parts.
void 
SoTransformerDragger::setLocateHighlighting( SbBool onOff )
{
    if (locateHighlightOn == onOff)
	return;

    SoLocateHighlight::Modes myMode;
    if (onOff == TRUE)
	myMode = SoLocateHighlight::AUTO;
    else
	myMode = SoLocateHighlight::OFF;

    SbBool wasEnabled = isNotifyEnabled();
    enableNotify(FALSE);

    SoLocateHighlight *g;

    g = SO_CHECK_ANY_PART(this,"translator1LocateGroup",SoLocateHighlight);
    g->mode = myMode;
    g = SO_CHECK_ANY_PART(this,"translator2LocateGroup",SoLocateHighlight);
    g->mode = myMode;
    g = SO_CHECK_ANY_PART(this,"translator3LocateGroup",SoLocateHighlight);
    g->mode = myMode;
    g = SO_CHECK_ANY_PART(this,"translator4LocateGroup",SoLocateHighlight);
    g->mode = myMode;
    g = SO_CHECK_ANY_PART(this,"translator5LocateGroup",SoLocateHighlight);
    g->mode = myMode;
    g = SO_CHECK_ANY_PART(this,"translator6LocateGroup",SoLocateHighlight);
    g->mode = myMode;

    g = SO_CHECK_ANY_PART(this,"rotator1LocateGroup",SoLocateHighlight);
    g->mode = myMode;
    g = SO_CHECK_ANY_PART(this,"rotator2LocateGroup",SoLocateHighlight);
    g->mode = myMode;
    g = SO_CHECK_ANY_PART(this,"rotator3LocateGroup",SoLocateHighlight);
    g->mode = myMode;
    g = SO_CHECK_ANY_PART(this,"rotator4LocateGroup",SoLocateHighlight);
    g->mode = myMode;
    g = SO_CHECK_ANY_PART(this,"rotator5LocateGroup",SoLocateHighlight);
    g->mode = myMode;
    g = SO_CHECK_ANY_PART(this,"rotator6LocateGroup",SoLocateHighlight);
    g->mode = myMode;

    g = SO_CHECK_ANY_PART(this,"scale1LocateGroup",SoLocateHighlight);
    g->mode = myMode;
    g = SO_CHECK_ANY_PART(this,"scale2LocateGroup",SoLocateHighlight);
    g->mode = myMode;
    g = SO_CHECK_ANY_PART(this,"scale3LocateGroup",SoLocateHighlight);
    g->mode = myMode;
    g = SO_CHECK_ANY_PART(this,"scale4LocateGroup",SoLocateHighlight);
    g->mode = myMode;
    g = SO_CHECK_ANY_PART(this,"scale5LocateGroup",SoLocateHighlight);
    g->mode = myMode;
    g = SO_CHECK_ANY_PART(this,"scale6LocateGroup",SoLocateHighlight);
    g->mode = myMode;
    g = SO_CHECK_ANY_PART(this,"scale7LocateGroup",SoLocateHighlight);
    g->mode = myMode;
    g = SO_CHECK_ANY_PART(this,"scale8LocateGroup",SoLocateHighlight);
    g->mode = myMode;

    locateHighlightOn = onOff;

    enableNotify(wasEnabled);
    touch();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
// Returns the index of the axis the locater is closest to.
//
// By setting the input arguments, you can pick with axes you 
// are interested in choosing between

int
SoTransformerDragger::getMouseGestureDirection(SbBool xAllowed, 
				       SbBool yAllowed, SbBool zAllowed)
//
////////////////////////////////////////////////////////////////////////
{
    int answer = -1;

    // Step 1: Start with the point where the mouse began, and build an 
    // imaginary set of axes around it.
    // Begin with the point we move relative to as expressed in world space.
	SbVec3f worldOrigin = getWorldStartingPoint();
	SbVec2f screenOrigin = getWorldPointInPixelSpace( worldOrigin );

    // Find the ends of the axes in screen coords.
    // three axes, two endpts each
	SbVec3f worldDir[3][2];
	worldDir[0][0] = getBoxDirInWorldSpace(SbVec3f(-1,0,0));
	worldDir[0][1] = getBoxDirInWorldSpace(SbVec3f( 1,0,0));
	worldDir[1][0] = getBoxDirInWorldSpace(SbVec3f(0,-1,0));
	worldDir[1][1] = getBoxDirInWorldSpace(SbVec3f(0, 1,0));
	worldDir[2][0] = getBoxDirInWorldSpace(SbVec3f(0,0,-1));
	worldDir[2][1] = getBoxDirInWorldSpace(SbVec3f(0,0, 1));

	// If any scales are tiny, beef them up:
#define TINY 0.001
	SbBool areAnyTiny = FALSE;
	int i;
	for (i = 0; i < 3; i++)
	    for (int j = 0; j < 2; j++)
		if ( fabs(worldDir[i][j].dot(worldDir[i][j])) < TINY )
		    areAnyTiny = TRUE;
#undef TINY
#define BEEF_UP_FACTOR 1000
	if (areAnyTiny) {
	    for (i = 0; i < 3; i++)
		for (int j = 0; j < 2; j++)
		    worldDir[i][j] *= BEEF_UP_FACTOR;
	}
#undef BEEF_UP_FACTOR

	SbVec3f worldEnd[3][2];
	worldEnd[0][0] = worldOrigin + worldDir[0][0];
	worldEnd[0][1] = worldOrigin + worldDir[0][1];
	worldEnd[1][0] = worldOrigin + worldDir[1][0];
	worldEnd[1][1] = worldOrigin + worldDir[1][1];
	worldEnd[2][0] = worldOrigin + worldDir[2][0];
	worldEnd[2][1] = worldOrigin + worldDir[2][1];

	SbVec2f screenEnd[3][2];
	screenEnd[0][0] = getWorldPointInPixelSpace( worldEnd[0][0] );
	screenEnd[0][1] = getWorldPointInPixelSpace( worldEnd[0][1] );
	screenEnd[1][0] = getWorldPointInPixelSpace( worldEnd[1][0] );
	screenEnd[1][1] = getWorldPointInPixelSpace( worldEnd[1][1] );
	screenEnd[2][0] = getWorldPointInPixelSpace( worldEnd[2][0] );
	screenEnd[2][1] = getWorldPointInPixelSpace( worldEnd[2][1] );

    // Step 2: If two of the axes are too close
    // to being colinear, ignore the shorter of
    // the two.
    // returns the index of the axis to ignore (x,y or z)
    // or -1 if they're ok
    int ignoreAxis = getIgnoreAxis(screenEnd, xAllowed, yAllowed, zAllowed );

    // Step 3: If one axis is ignored and the other two are colinear,
    //         make gesture picking easier by changing them to be perpendicular.
    //         The longer axis gets to keep its orientation, and the shorter
    //         gets moved.
    if ( ! xAllowed )
	makeMinorAxisPerpendicularIfColinear(screenOrigin, screenEnd, 1, 2 );
    else if ( ! yAllowed )
	makeMinorAxisPerpendicularIfColinear(screenOrigin, screenEnd, 0, 2 );
    else if ( ! zAllowed )
	makeMinorAxisPerpendicularIfColinear(screenOrigin, screenEnd, 0, 1 );

    // Step 4: look at the "distance" of the mouse
    // to each of the axes, and find the closest
    // The distance is actually the dot product, normalized
    // from -1.0 to 1.0, where -1 is as far apart as you
    // can get and 1.0 is as close as you can get

    float closestDist = -2.0; // the closest distance

    SbVec2f locaterVec;
    locaterVec[0] = (float)getLocaterPosition()[0] - screenOrigin[0];
    locaterVec[1] = (float)getLocaterPosition()[1] - screenOrigin[1];
    locaterVec.normalize();

    for(int axis = 0; axis < 3; axis++) {
	if (axis == ignoreAxis)
	    continue;
	if (axis == 0 && !xAllowed )
	    continue;
	if (axis == 1 && !yAllowed )
	    continue;
	if (axis == 2 && !zAllowed )
	    continue;
	for(int end = 0; end < 2; end++) {

	    // work in floating point
	    SbVec2f axisVec;
	    axisVec[0] = (screenEnd[axis][end] - screenOrigin)[0];
	    axisVec[1] = (screenEnd[axis][end] - screenOrigin)[1];
	    axisVec.normalize();

	    float distance = fabs( locaterVec.dot(axisVec) );

	    if (distance >= closestDist) {
		answer = axis;

		closestDist = distance;
	    }
	}
    }
    return answer;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
// Returns the index of an axis to ignore due to colinearity,
// or -1 if none
//
// Use: EXTENDER public, static 

int
SoTransformerDragger::getIgnoreAxis(SbVec2f axis[3][2],
			    SbBool xAllowed, SbBool yAllowed, SbBool zAllowed )
//
////////////////////////////////////////////////////////////////////////
{
    // If an axis is not allowed, it is ignored.
    // In this case, if the
    if ( ! xAllowed )
	return 0;
    if ( ! yAllowed )
	return 1;
    if ( ! zAllowed )
	return 2;

    float length[3]; // length of each axis

    // Get screen lengths of axes. 
    for (int i = 0; i < 3; i++)
	length[i] = (axis[i][1] - axis[i][0]).length();

    // Since none of the axes are short, check if any pair
    // of them are colinear. If so. Ignore the shorter of
    // the two

    // check X and Y
    if (isColinear(axis[0],axis[1],colinearThreshold))
	return (length[0] < length[1]) ? 0 : 1;

    // check X and Z
    if (isColinear(axis[0],axis[2],colinearThreshold))
	return (length[0] < length[2]) ? 0 : 2;

    // check Y and Z
    if (isColinear(axis[1],axis[2],colinearThreshold))
	return (length[1] < length[2]) ? 1 : 2;

    // all the axes are okeedokee
    // don't ignore any of them
    return -1;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//     Used to make gesture picking easier if one axis is ignored and the 
//     other two are colinear. Changes them to be perpendicular.
//     The longer axis gets to keep its orientation, and the shorter
//     gets moved.
//
//   Looks at the two axes described by indexA and indexB.  If colinear,
//   makes the shorter one be perpendicular to the longer, passing through the
//   origin.
//
// Use: EXTENDER public, static
//
void 
SoTransformerDragger::makeMinorAxisPerpendicularIfColinear( SbVec2f origin, 
		SbVec2f axisEnds[3][2], int indexA, int indexB )
//
////////////////////////////////////////////////////////////////////////
{
    if ( ! isColinear(axisEnds[indexA],axisEnds[indexB],1))
	return;

    // Which is shorter?
    float lengthA = (axisEnds[indexA][1] - axisEnds[indexA][0]).length();
    float lengthB = (axisEnds[indexB][1] - axisEnds[indexB][0]).length();

    int shortInd, longInd;
    if (lengthA > lengthB) {
	longInd = indexA;
	shortInd = indexB;
    }
    else {
	longInd = indexB;
	shortInd = indexA;
    }

    // Create perpendicular directions:
    SbVec2f longDir1 = axisEnds[longInd][1] - origin;
    SbVec2f longDir0 = axisEnds[longInd][0] - origin;
    SbVec2f perpDir1, perpDir0;
    perpDir1[0] = -longDir1[1];
    perpDir1[1] = longDir1[0];
    perpDir0[0] = -longDir0[1];
    perpDir0[1] = longDir0[0];

    // Assign the new values to the endpoints
    axisEnds[shortInd][0] = origin + perpDir0;
    axisEnds[shortInd][1] = origin + perpDir1;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
// Returns if two lines are colinear within
// colinearThreshold pixels.
// 
// For one endpoint on each line,
// finds the distance from the endpoint to
// the other line. 
// Taken from handbook of mathematical tables p.159
//
// Use: EXTENDER public, static

SbBool
SoTransformerDragger::isColinear(SbVec2f a1[2], SbVec2f a2[2], int pixels)
//
////////////////////////////////////////////////////////////////////////
{
    // Find slope of line along a1
	float dx,dy,slope;
	dx = a1[0][0] - a1[1][0];
	dy = a1[0][1] - a1[1][1];
	if (dx == 0.0)
	    dx = 0.0001;
	slope = dy/dx;  

    float yIntercept,A,B,C,dist1,dist2;
    yIntercept = a1[0][1] - slope*a1[0][0]; // y intercept

    A = -slope;
    B = 1.0;
    C = -yIntercept;

    dist1 = _ABS((A*a2[0][0] + B*a2[0][1] + C)/sqrtf(A*A + B*B));

    if (dist1 > (float)pixels)
	return FALSE;
    else {
        dist2 = 
	    _ABS((A*a2[1][0] + B*a2[1][1] + C)/sqrtf(A*A + B*B));

        if (dist2 > (float)pixels)
	    return FALSE;
	else
	    return TRUE;
    }
}

////////////////////////////////////////////////////////////////////////
//
// These convert points and directions between frequently needed spaces.
//
// -- world space is world coordinates of scene
// -- pixel space is pixel location within viewport
// -- boxSpace is unit cube in space where dragger geom is defined.
//    This is the space following the "surroundScale" part.
//
////////////////////////////////////////////////////////////////////////
SbVec3f 
SoTransformerDragger::getBoxPointInWorldSpace( const SbVec3f &pointOnUnitBox )
{
    // Box space is the space right after surroundScale:

    // Get Matrix from boxSpace to local space:
	SbMatrix boxSpaceToLocal, localToBoxSpace;
	getPartToLocalMatrix("surroundScale",boxSpaceToLocal,localToBoxSpace);

    // Get Matrix from boxSpace to world space:
	SbMatrix boxSpaceToWorld = boxSpaceToLocal;
	boxSpaceToWorld.multRight( getLocalToWorldMatrix() );

    // Multiply through
	SbVec3f answer;
	boxSpaceToWorld.multVecMatrix(pointOnUnitBox, answer);

    return answer;
}

SbVec3f 
SoTransformerDragger::getBoxDirInWorldSpace( const SbVec3f &dirOnUnitBox )
{
    // Box space is the space right after surroundScale:

    // Get Matrix from boxSpace to local space:
	SbMatrix boxSpaceToLocal, localToBoxSpace;
	getPartToLocalMatrix("surroundScale",boxSpaceToLocal,localToBoxSpace);

    // Get Matrix from boxSpace to world space:
	SbMatrix boxSpaceToWorld = boxSpaceToLocal;
	boxSpaceToWorld.multRight( getLocalToWorldMatrix() );

    // Multiply through
	SbVec3f answer;
	boxSpaceToWorld.multDirMatrix(dirOnUnitBox, answer);

    return answer;
}
SbVec3f 
SoTransformerDragger::getWorldPointInBoxSpace( const SbVec3f &pointInWorldSpace )
{
    // Box space is the space right after surroundScale:

    // Get Matrix from local space to boxSpace:
	SbMatrix boxSpaceToLocal, localToBoxSpace;
	getPartToLocalMatrix("surroundScale",boxSpaceToLocal,localToBoxSpace);

    // Get Matrix from world space to boxSpace:
	SbMatrix worldSpaceToBox = getWorldToLocalMatrix();
	worldSpaceToBox.multRight( localToBoxSpace );

    // Multiply through
	SbVec3f answer;
	worldSpaceToBox.multVecMatrix(pointInWorldSpace, answer);

    return answer;
}

SbVec2f 
SoTransformerDragger::getWorldPointInPixelSpace( const SbVec3f &thePoint )
{
    // nrmlzdScreen is in range [0..1] in x and y
    SbVec3f nrmlzdScreen;
    getViewVolume().projectToScreen( thePoint, nrmlzdScreen );

    SbVec2f screenPoint;
    SbVec2s vpPixels = getViewportRegion().getViewportSizePixels();

    screenPoint[0] = nrmlzdScreen[0]*(float)vpPixels[0];
    screenPoint[1] = nrmlzdScreen[1]*(float)vpPixels[1];

    return screenPoint;
}
