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
 |	SoHandleBoxDragger
 |
 |   Author(s): Paul Isaacs, David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <stdio.h>
#include <Inventor/SoDB.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include <Inventor/events/SoEvent.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/projectors/SbPlaneProjector.h>
#include <Inventor/projectors/SbLineProjector.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/draggers/SoHandleBoxDragger.h>
#include <Inventor/nodes/SoSurroundScale.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoTransform.h>
#include <GL/gl.h>

#include "geom/SoHandleBoxDraggerGeom.h"


SO_KIT_SOURCE(SoHandleBoxDragger);


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
SoHandleBoxDragger::SoHandleBoxDragger()
//
////////////////////////////////////////////////////////////////////////
{
    SO_KIT_CONSTRUCTOR(SoHandleBoxDragger);

    isBuiltIn = TRUE;

    // Don't create this by default. It's only really put into use
    // if this dragger is put inside a manipulator.
    SO_KIT_ADD_CATALOG_ENTRY(surroundScale, SoSurroundScale, 
				TRUE, topSeparator, geomSeparator,TRUE);

    // These parts will all go under the geomSeparator, for efficient 
    // rendering
    SO_KIT_ADD_CATALOG_ENTRY(drawStyle, SoDrawStyle, TRUE,
				geomSeparator, ,FALSE);

    SO_KIT_ADD_CATALOG_ENTRY(translator1Switch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(translator1, SoSeparator, TRUE,
				translator1Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(translator1Active, SoSeparator, TRUE,
				translator1Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(translator2Switch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(translator2, SoSeparator, TRUE,
				translator2Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(translator2Active, SoSeparator, TRUE,
				translator2Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(translator3Switch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(translator3, SoSeparator, TRUE,
				translator3Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(translator3Active, SoSeparator, TRUE,
				translator3Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(translator4Switch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(translator4, SoSeparator, TRUE,
				translator4Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(translator4Active, SoSeparator, TRUE,
				translator4Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(translator5Switch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(translator5, SoSeparator, TRUE,
				translator5Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(translator5Active, SoSeparator, TRUE,
				translator5Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(translator6Switch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(translator6, SoSeparator, TRUE,
				translator6Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(translator6Active, SoSeparator, TRUE,
				translator6Switch, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(extruder1Switch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(extruder1, SoSeparator, TRUE,
				extruder1Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(extruder1Active, SoSeparator, TRUE,
				extruder1Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(extruder2Switch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(extruder2, SoSeparator, TRUE,
				extruder2Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(extruder2Active, SoSeparator, TRUE,
				extruder2Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(extruder3Switch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(extruder3, SoSeparator, TRUE,
				extruder3Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(extruder3Active, SoSeparator, TRUE,
				extruder3Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(extruder4Switch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(extruder4, SoSeparator, TRUE,
				extruder4Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(extruder4Active, SoSeparator, TRUE,
				extruder4Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(extruder5Switch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(extruder5, SoSeparator, TRUE,
				extruder5Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(extruder5Active, SoSeparator, TRUE,
				extruder5Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(extruder6Switch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(extruder6, SoSeparator, TRUE,
				extruder6Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(extruder6Active, SoSeparator, TRUE,
				extruder6Switch, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(uniform1Switch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(uniform1, SoSeparator, TRUE,
				uniform1Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(uniform1Active, SoSeparator, TRUE,
				uniform1Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(uniform2Switch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(uniform2, SoSeparator, TRUE,
				uniform2Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(uniform2Active, SoSeparator, TRUE,
				uniform2Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(uniform3Switch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(uniform3, SoSeparator, TRUE,
				uniform3Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(uniform3Active, SoSeparator, TRUE,
				uniform3Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(uniform4Switch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(uniform4, SoSeparator, TRUE,
				uniform4Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(uniform4Active, SoSeparator, TRUE,
				uniform4Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(uniform5Switch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(uniform5, SoSeparator, TRUE,
				uniform5Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(uniform5Active, SoSeparator, TRUE,
				uniform5Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(uniform6Switch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(uniform6, SoSeparator, TRUE,
				uniform6Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(uniform6Active, SoSeparator, TRUE,
				uniform6Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(uniform7Switch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(uniform7, SoSeparator, TRUE,
				uniform7Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(uniform7Active, SoSeparator, TRUE,
				uniform7Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(uniform8Switch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(uniform8, SoSeparator, TRUE,
				uniform8Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(uniform8Active, SoSeparator, TRUE,
				uniform8Switch, ,TRUE);

    SO_KIT_ADD_CATALOG_ENTRY(arrowTranslation, SoTranslation, 
				TRUE, geomSeparator, ,FALSE);

    SO_KIT_ADD_CATALOG_ENTRY(arrow1Switch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(arrow1, SoSeparator, TRUE,
				arrow1Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(arrow2Switch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(arrow2, SoSeparator, TRUE,
				arrow2Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(arrow3Switch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(arrow3, SoSeparator, TRUE,
				arrow3Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(arrow4Switch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(arrow4, SoSeparator, TRUE,
				arrow4Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(arrow5Switch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(arrow5, SoSeparator, TRUE,
				arrow5Switch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(arrow6Switch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(arrow6, SoSeparator, TRUE,
				arrow6Switch, ,TRUE);

    // read geometry for shared parts
    if (SO_KIT_IS_FIRST_INSTANCE())
	readDefaultParts("handleBoxDragger.iv", geomBuffer, sizeof(geomBuffer));

    SO_KIT_ADD_FIELD(translation, (0.0, 0.0, 0.0));
    SO_KIT_ADD_FIELD(scaleFactor, (1.0, 1.0, 1.0));

    SO_KIT_INIT_INSTANCE();

    // Set up the drawStyle node
    SoDrawStyle *ds = (SoDrawStyle *) getAnyPart( "drawStyle", TRUE );
    ds->style.setValue(SoDrawStyle::LINES);

    // translation box pieces.
  setPartAsDefault("translator1",     "handleBoxTranslator1");
  setPartAsDefault("translator1Active","handleBoxTranslator1Active");
  setPartAsDefault("translator2",     "handleBoxTranslator2");
  setPartAsDefault("translator2Active","handleBoxTranslator2Active");
  setPartAsDefault("translator3",     "handleBoxTranslator3");
  setPartAsDefault("translator3Active","handleBoxTranslator3Active");
  setPartAsDefault("translator4",     "handleBoxTranslator4");
  setPartAsDefault("translator4Active","handleBoxTranslator4Active");
  setPartAsDefault("translator5",     "handleBoxTranslator5");
  setPartAsDefault("translator5Active","handleBoxTranslator5Active");
  setPartAsDefault("translator6",     "handleBoxTranslator6");
  setPartAsDefault("translator6Active","handleBoxTranslator6Active");

    // face scale pieces.
    setPartAsDefault("extruder1",      "handleBoxExtruder1");
    setPartAsDefault("extruder1Active","handleBoxExtruder1Active");
    setPartAsDefault("extruder2",      "handleBoxExtruder2");
    setPartAsDefault("extruder2Active","handleBoxExtruder2Active");
    setPartAsDefault("extruder3",      "handleBoxExtruder3");
    setPartAsDefault("extruder3Active","handleBoxExtruder3Active");
    setPartAsDefault("extruder4",      "handleBoxExtruder4");
    setPartAsDefault("extruder4Active","handleBoxExtruder4Active");
    setPartAsDefault("extruder5",      "handleBoxExtruder5");
    setPartAsDefault("extruder5Active","handleBoxExtruder5Active");
    setPartAsDefault("extruder6",      "handleBoxExtruder6");
    setPartAsDefault("extruder6Active","handleBoxExtruder6Active");

    // uniform scale pieces.
    setPartAsDefault("uniform1",      "handleBoxUniform1");
    setPartAsDefault("uniform1Active","handleBoxUniform1Active");
    setPartAsDefault("uniform2",      "handleBoxUniform2");
    setPartAsDefault("uniform2Active","handleBoxUniform2Active");
    setPartAsDefault("uniform3",      "handleBoxUniform3");
    setPartAsDefault("uniform3Active","handleBoxUniform3Active");
    setPartAsDefault("uniform4",      "handleBoxUniform4");
    setPartAsDefault("uniform4Active","handleBoxUniform4Active");
    setPartAsDefault("uniform5",      "handleBoxUniform5");
    setPartAsDefault("uniform5Active","handleBoxUniform5Active");
    setPartAsDefault("uniform6",      "handleBoxUniform6");
    setPartAsDefault("uniform6Active","handleBoxUniform6Active");
    setPartAsDefault("uniform7",      "handleBoxUniform7");
    setPartAsDefault("uniform7Active","handleBoxUniform7Active");
    setPartAsDefault("uniform8",      "handleBoxUniform8");
    setPartAsDefault("uniform8Active","handleBoxUniform8Active");

    setAllPartsActive(FALSE);

    // Arrow Feedback for translation.
    setAnyPart("arrowTranslation", new SoTranslation );

    setPartAsDefault("arrow1",      "handleBoxArrow1");
    setPartAsDefault("arrow2",      "handleBoxArrow2");
    setPartAsDefault("arrow3",      "handleBoxArrow3");
    setPartAsDefault("arrow4",      "handleBoxArrow4");
    setPartAsDefault("arrow5",      "handleBoxArrow5");
    setPartAsDefault("arrow6",      "handleBoxArrow6");

    setSwitchValue( arrow1Switch.getValue(), SO_SWITCH_NONE );
    setSwitchValue( arrow2Switch.getValue(), SO_SWITCH_NONE );
    setSwitchValue( arrow3Switch.getValue(), SO_SWITCH_NONE );
    setSwitchValue( arrow4Switch.getValue(), SO_SWITCH_NONE );
    setSwitchValue( arrow5Switch.getValue(), SO_SWITCH_NONE );
    setSwitchValue( arrow6Switch.getValue(), SO_SWITCH_NONE );

    motionPlaneProj = new SbPlaneProjector;
    motionLineProj = new SbLineProjector;

    // by default, handlebox is a bit bigger than the bounding box
//OBSOLETE    setDraggerScale( SbVec3f( 1.2, 1.2, 1.2 ) );

    currentState = INACTIVE;
    restartState = INACTIVE;


    // add the callbacks to perform the dragging.
    addStartCallback(  &SoHandleBoxDragger::startCB );
    addMotionCallback( &SoHandleBoxDragger::motionCB );
    addFinishCallback(   &SoHandleBoxDragger::finishCB );

    // add the callback to update things each time a meta key changes.
    addOtherEventCallback(   &SoHandleBoxDragger::metaKeyChangeCB );

    // Update the rotation and scale fields when the motionMatrix is set.
    addValueChangedCallback( &SoHandleBoxDragger::valueChangedCB );

    // Updates the motionMatrix when the translationFactor field is set.
    translFieldSensor 
        = new SoFieldSensor( &SoHandleBoxDragger::fieldSensorCB, this);
    translFieldSensor->setPriority( 0 );

    // Updates the motionMatrix when the scaleFactor field is set.
    scaleFieldSensor 
	= new SoFieldSensor( &SoHandleBoxDragger::fieldSensorCB, this);
    scaleFieldSensor->setPriority( 0 );

    setUpConnections( TRUE, TRUE );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
SoHandleBoxDragger::~SoHandleBoxDragger()
//
////////////////////////////////////////////////////////////////////////
{
    delete motionPlaneProj;
    delete motionLineProj;
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
SoHandleBoxDragger::setUpConnections( SbBool onOff, SbBool doItAlways )
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
    }
    else {

	// We disconnect BEFORE base class.

	// Disconnect the field sensors.
	if (translFieldSensor->getAttachedField())
	    translFieldSensor->detach();
	if (scaleFieldSensor->getAttachedField())
	    scaleFieldSensor->detach();

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
SoHandleBoxDragger::dragStart()
//
////////////////////////////////////////////////////////////////////////
{

    const SoPath *pickPath = getPickPath();

    // Invalidate surroundScale if it exists.
    SoSurroundScale *ss = SO_CHECK_PART(this, "surroundScale", SoSurroundScale);
    if (ss != NULL)
	ss->invalidate();

    // This gives us an opportunity for a part to start a new gesture
    // after releasing and re-grabbing in the metaKey callback.
    currentState = restartState;

    translateDir = -1;

    // Make a note of which modifier keys are down.
    altDown  = getEvent()->wasAltDown();
    ctlDown  = getEvent()->wasCtrlDown();
    shftDown = getEvent()->wasShiftDown();

    // See which subGraph of the handle box was hit to determine operation

    if ( FALSE ) {
	// Are we scaling geometry only?
	currentState = SCALE_GEOM_ONLY;
    }

    // Note: we won't even bother if the restartState was not INACTIVE.
    if ( currentState == INACTIVE && pickPath != NULL ) {

	// Try finding a part that was picked.

	// TRANSLATIONS
	if (      pickPath->containsNode( translator1Switch.getValue() ))
	    currentState = TOP_TRANSLATE;
	else if ( pickPath->containsNode( translator2Switch.getValue() ))
	    currentState = BOT_TRANSLATE;
	else if ( pickPath->containsNode( translator3Switch.getValue() ))
	    currentState = LFT_TRANSLATE;
	else if ( pickPath->containsNode( translator4Switch.getValue() ))
	    currentState = RIT_TRANSLATE;
	else if ( pickPath->containsNode( translator5Switch.getValue() ))
	    currentState = FNT_TRANSLATE;
	else if ( pickPath->containsNode( translator6Switch.getValue() ))
	    currentState = BAK_TRANSLATE;

	// 1-d SCALING
	else if ( pickPath->containsNode( extruder1Switch.getValue() ))
	    currentState = TOP_Y_SCALE;
	else if ( pickPath->containsNode( extruder2Switch.getValue() ))
	    currentState = BOT_Y_SCALE;
	else if ( pickPath->containsNode( extruder3Switch.getValue() ))
	    currentState = LFT_X_SCALE;
	else if ( pickPath->containsNode( extruder4Switch.getValue() ))
	    currentState = RIT_X_SCALE;
	else if ( pickPath->containsNode( extruder5Switch.getValue() ))
	    currentState = FNT_Z_SCALE;
	else if ( pickPath->containsNode( extruder6Switch.getValue() ))
	    currentState = BAK_Z_SCALE;

	// 3-d SCALING
	else if ( pickPath->containsNode( uniform1Switch.getValue() ))
	    currentState = PX_PY_PZ_3D_SCALE;
	else if ( pickPath->containsNode( uniform2Switch.getValue() ))
	    currentState = PX_PY_NZ_3D_SCALE;
	else if ( pickPath->containsNode( uniform3Switch.getValue() ))
	    currentState = PX_NY_PZ_3D_SCALE;
	else if ( pickPath->containsNode( uniform4Switch.getValue() ))
	    currentState = PX_NY_NZ_3D_SCALE;
	else if ( pickPath->containsNode( uniform5Switch.getValue() ))
	    currentState = NX_PY_PZ_3D_SCALE;
	else if ( pickPath->containsNode( uniform6Switch.getValue() ))
	    currentState = NX_PY_NZ_3D_SCALE;
	else if ( pickPath->containsNode( uniform7Switch.getValue() ))
	    currentState = NX_NY_PZ_3D_SCALE;
	else if ( pickPath->containsNode( uniform8Switch.getValue() ))
	    currentState = NX_NY_NZ_3D_SCALE;
    }

    if (currentState == INACTIVE ) {

	// Try looking at the surrogate part paths...

	// Surrogate part paths...
	if ( getSurrogatePartPickedName() ==  "translator1" )
	    currentState = TOP_TRANSLATE;
	else if ( getSurrogatePartPickedName() ==  "translator2" )
	    currentState = BOT_TRANSLATE;
	else if ( getSurrogatePartPickedName() ==  "translator3" )
	    currentState = LFT_TRANSLATE;
	else if ( getSurrogatePartPickedName() ==  "translator4" )
	    currentState = RIT_TRANSLATE;
	else if ( getSurrogatePartPickedName() ==  "translator5" )
	    currentState = FNT_TRANSLATE;
	else if ( getSurrogatePartPickedName() ==  "translator6" )
	    currentState = BAK_TRANSLATE;
	else if ( getSurrogatePartPickedName() ==  "extruder1" )
	    currentState = TOP_Y_SCALE;
	else if ( getSurrogatePartPickedName() ==  "extruder2" )
	    currentState = BOT_Y_SCALE;
	else if ( getSurrogatePartPickedName() ==  "extruder3" )
	    currentState = LFT_X_SCALE;
	else if ( getSurrogatePartPickedName() ==  "extruder4" )
	    currentState = RIT_X_SCALE;
	else if ( getSurrogatePartPickedName() ==  "extruder5" )
	    currentState = FNT_Z_SCALE;
	else if ( getSurrogatePartPickedName() ==  "extruder6" )
	    currentState = BAK_Z_SCALE;
	else if ( getSurrogatePartPickedName() ==  "uniform1" )
	    currentState = PX_PY_PZ_3D_SCALE;
	else if ( getSurrogatePartPickedName() ==  "uniform2" )
	    currentState = PX_PY_NZ_3D_SCALE;
	else if ( getSurrogatePartPickedName() ==  "uniform3" )
	    currentState = PX_NY_PZ_3D_SCALE;
	else if ( getSurrogatePartPickedName() ==  "uniform4" )
	    currentState = PX_NY_NZ_3D_SCALE;
	else if ( getSurrogatePartPickedName() ==  "uniform5" )
	    currentState = NX_PY_PZ_3D_SCALE;
	else if ( getSurrogatePartPickedName() ==  "uniform6" )
	    currentState = NX_PY_NZ_3D_SCALE;
	else if ( getSurrogatePartPickedName() ==  "uniform7" )
	    currentState = NX_NY_PZ_3D_SCALE;
	else if ( getSurrogatePartPickedName() ==  "uniform8" )
	    currentState = NX_NY_NZ_3D_SCALE;
    }

    setHighlights();

    switch( currentState ) {
	case RIT_TRANSLATE:
	case LFT_TRANSLATE:
	case TOP_TRANSLATE:
	case BOT_TRANSLATE:
	case FNT_TRANSLATE:
	case BAK_TRANSLATE:
	    translateInit();
	    break;
	case PX_PY_PZ_3D_SCALE:
	case PX_PY_NZ_3D_SCALE:
	case PX_NY_PZ_3D_SCALE:
	case PX_NY_NZ_3D_SCALE:
	case NX_PY_PZ_3D_SCALE:
	case NX_PY_NZ_3D_SCALE:
	case NX_NY_PZ_3D_SCALE:
	case NX_NY_NZ_3D_SCALE:
	    uniformScaleInit();
	    break;
	case RIT_X_SCALE:
	case LFT_X_SCALE:
	case TOP_Y_SCALE:
	case BOT_Y_SCALE:
	case FNT_Z_SCALE:
	case BAK_Z_SCALE:
	    faceScaleInit();
	    break;
	case SCALE_GEOM_ONLY:
	    scaleGeomSizeOnlyInit(); 
	    break;
	case INACTIVE:
	default:
	    break;
    }
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Get set to begin translating.
//
// Use: private
//
SbBool
SoHandleBoxDragger::translateInit()
//
////////////////////////////////////////////////////////////////////////
{
    // This is the point we'll use if a metaKey callback makes us re-start.
	worldRestartPt = getWorldStartingPoint();

    // Establish the projector line in working space.
    // Working space is space after "surroundScale"

	SbMatrix workSpaceToLocal, localToWorkSpace;
	getPartToLocalMatrix("surroundScale",workSpaceToLocal,localToWorkSpace);
	SbMatrix worldSpaceToWork = getWorldToLocalMatrix();
	worldSpaceToWork.multRight( localToWorkSpace );
	SbVec3f startWorkHit;
	worldSpaceToWork.multVecMatrix(getWorldStartingPoint(), startWorkHit);

	// do different things depending on what you hit...
	SbVec3f workSpaceNormal;

	switch( currentState ) {
	    case RIT_TRANSLATE:
	    case LFT_TRANSLATE:
		workSpaceNormal.setValue(  1,  0,  0);
		break;
	    case TOP_TRANSLATE:
	    case BOT_TRANSLATE:
		workSpaceNormal.setValue(  0,  1,  0);
		break;
	    case FNT_TRANSLATE:
	    case BAK_TRANSLATE:
		workSpaceNormal.setValue(  0,  0,  1);
		break;
	    default:
		return FALSE;
	}

    // At this point, the translation direction (used for 1-D translating)
    // is undefined.
	translateDir = -1;

    // For calculating motion within the plane.
    motionPlaneProj->setPlane(SbPlane(workSpaceNormal,startWorkHit));

    // If alt key is down, we also need a projector perpendicular to plane.
    if ( ctlDown )
	motionLineProj->setLine(
	    SbLine(startWorkHit, startWorkHit + workSpaceNormal));

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Get set to begin scaling, based on input from a knob placed on a 
//    face of the handleBox.
//    This implies a one dimensional scaling.
//
// Use: private
//
SbBool
SoHandleBoxDragger::faceScaleInit()
//
////////////////////////////////////////////////////////////////////////
{
    // This is the point we'll use if a metaKey callback makes us re-start.
	worldRestartPt = getWorldStartingPoint();

    // Establish the projector line in working space.
    // Working space is space after "surroundScale"

	SbMatrix workSpaceToLocal, localToWorkSpace;
	getPartToLocalMatrix("surroundScale",workSpaceToLocal,localToWorkSpace);
	SbMatrix worldSpaceToWork = getWorldToLocalMatrix();
	worldSpaceToWork.multRight( localToWorkSpace );
	SbVec3f startWorkHit;
	worldSpaceToWork.multVecMatrix(getWorldStartingPoint(), startWorkHit);

	// do different things depending on what you hit...
	SbVec3f axisPt2;

	switch( currentState ) {
	    case RIT_X_SCALE:
	    case LFT_X_SCALE:
		axisPt2 = startWorkHit + SbVec3f(  1,  0,  0);
		break;
	    case TOP_Y_SCALE:
	    case BOT_Y_SCALE:
		axisPt2 = startWorkHit + SbVec3f(  0,  1,  0);
		break;
	    case FNT_Z_SCALE:
	    case BAK_Z_SCALE:
		axisPt2 = startWorkHit + SbVec3f(  0,  0,  1);
		break;
	    default:
		return FALSE;
	}

	motionLineProj->setLine( SbLine( startWorkHit, axisPt2 ) );

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Get set to begin scaling, based on input from a knob placed on a 
//    vertex of the handleBox.
//    This implies a one dimensional scaling.
//
// Use: private
//
SbBool
SoHandleBoxDragger::uniformScaleInit()
//
////////////////////////////////////////////////////////////////////////
{
    // This is the point we'll use if a metaKey callback makes us re-start.
	worldRestartPt = getWorldStartingPoint();

    // Establish the projector line in working space.
    // Working space is space after "surroundScale"

	SbMatrix workSpaceToLocal, localToWorkSpace;
	getPartToLocalMatrix("surroundScale",workSpaceToLocal,localToWorkSpace);
	SbMatrix worldSpaceToWork = getWorldToLocalMatrix();
	worldSpaceToWork.multRight( localToWorkSpace );
	SbVec3f startWorkHit;
	worldSpaceToWork.multVecMatrix(getWorldStartingPoint(), startWorkHit);

    // create a line between the box center and the point that was hit
	motionLineProj->setLine( SbLine( SbVec3f(0,0,0),startWorkHit ) );

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Get set to begin scaling only the Geometry, based on
//    scaling along a line connecting (0,0,0) to the point originally hit.
//
//
// Use: private
//
SbBool
SoHandleBoxDragger::scaleGeomSizeOnlyInit()
//
////////////////////////////////////////////////////////////////////////
{
    // This is the point we'll use if a metaKey callback makes us re-start.
	worldRestartPt = getWorldStartingPoint();

    // Establish the projector line in working space.
    // Working space is space after "surroundScale"

	SbMatrix workSpaceToLocal, localToWorkSpace;
	getPartToLocalMatrix("surroundScale",workSpaceToLocal,localToWorkSpace);
	SbMatrix worldSpaceToWork = getWorldToLocalMatrix();
	worldSpaceToWork.multRight( localToWorkSpace );
	SbVec3f startWorkHit;
	worldSpaceToWork.multVecMatrix(getWorldStartingPoint(), startWorkHit);

    // the motion line connects the origin with the point that was hit
	motionLineProj->setLine( SbLine( SbVec3f(0,0,0),startWorkHit));

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Drag the handlebox based on locater motion.
//    This routine just ships the work to translateDrag or 
//    scaleDrag.
//
// Use: private
//
void
SoHandleBoxDragger::drag()
//
////////////////////////////////////////////////////////////////////////
{
    switch( currentState ) {
	case RIT_TRANSLATE:
	case LFT_TRANSLATE:
	case TOP_TRANSLATE:
	case BOT_TRANSLATE:
	case FNT_TRANSLATE:
	case BAK_TRANSLATE:
	    translateDrag();
	    break;
	case PX_PY_PZ_3D_SCALE:
	case PX_PY_NZ_3D_SCALE:
	case PX_NY_PZ_3D_SCALE:
	case PX_NY_NZ_3D_SCALE:
	case NX_PY_PZ_3D_SCALE:
	case NX_PY_NZ_3D_SCALE:
	case NX_NY_PZ_3D_SCALE:
	case NX_NY_NZ_3D_SCALE:

	case RIT_X_SCALE:
	case LFT_X_SCALE:
	case TOP_Y_SCALE:
	case BOT_Y_SCALE:
	case FNT_Z_SCALE:
	case BAK_Z_SCALE:
	    scaleDrag();
	    break;
	case SCALE_GEOM_ONLY:
	    scaleGeomSizeOnlyDrag(); 
	    break;
	case INACTIVE:
	default:
	    break;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Translate the handlebox and object based on locater motion.
//
// Use: private
//
SbBool
SoHandleBoxDragger::translateDrag()
//
////////////////////////////////////////////////////////////////////////
{
    SbMatrix workSpaceToLocal, localToWorkSpace;
    getPartToLocalMatrix("surroundScale", workSpaceToLocal, localToWorkSpace);
    SbMatrix workSpaceToWorld = getLocalToWorldMatrix();
    workSpaceToWorld.multLeft( workSpaceToLocal );
    SbMatrix worldSpaceToWork = getWorldToLocalMatrix();
    worldSpaceToWork.multRight( localToWorkSpace );

    // Get startHitPt in workspace.
	SbVec3f startHitPt;
	worldSpaceToWork.multVecMatrix(getWorldStartingPoint(), startHitPt);

    // Calculate intersection with plane projector.
    // Find mouse hit on plane currently in use...
	motionPlaneProj->setViewVolume( getViewVolume() );
	motionPlaneProj->setWorkingSpace( workSpaceToWorld );
	SbVec3f workSpacePlanePt
	    = motionPlaneProj->project(getNormalizedLocaterPosition());

    // If we need to start a new gesture, we'll carry on from this pt.
	workSpaceToWorld.multVecMatrix( workSpacePlanePt, worldRestartPt);

    // Get newHitPt in workSpace.
	SbVec3f  newHitPt;

    // If CONTROL key down, our 'newHitPt' must be taken from the
    // line projector instead.
    if ( !ctlDown ) {
        newHitPt = workSpacePlanePt;
    }
    else { 
	// If modify key is down, then we are moving PERPENDICULAR to the 
	// motion plane.  
	motionLineProj->setViewVolume( getViewVolume() );
	motionLineProj->setWorkingSpace( workSpaceToWorld );
	newHitPt 
	    = motionLineProj->project( getNormalizedLocaterPosition());
    }

    // find the difference between current and beginning intersections.
    SbVec3f  workSpaceMotion;
    workSpaceMotion = newHitPt - startHitPt;

    if ( !shftDown || ctlDown )
					// constrainKey not down, or we are 
				 	// doing perpendicular (CONTROL) motion.
				 	// Clear the 1-D translation dir.
	translateDir = -1;
    else {
	// the constrain key is pressed. This means 1-D translation.
	if ( translateDir == -1 ) {
	    // The 1-D direction is not defined.  Calculate it
	    // based on which direction got the maximum locater motion.
	    if ( isAdequateConstraintMotion() ) {
		if (    fabs( workSpaceMotion[0]) > fabs( workSpaceMotion[1]) 
		     && fabs( workSpaceMotion[0]) > fabs( workSpaceMotion[2]) )
		    translateDir = 0;
		else if (fabs( workSpaceMotion[1]) > fabs( workSpaceMotion[2]) )
		    translateDir = 1;
		else 
		    translateDir = 2;
		// We'll need to display different translation arrows.
    		setFeedbackArrows();
	    }
	    else {
		// Not ready to pick a direction yet. Don't move.
		return FALSE;
	    }
	}
	// get the projection of 'workSpaceMotion' onto the preferred axis.
	SbVec3f constrainedMotion(0,0,0);
	constrainedMotion[translateDir] = workSpaceMotion[translateDir];
	workSpaceMotion = constrainedMotion;
    }

    // Append this to the startMotionMatrix, which we saved at the beginning
    // of the drag, to find the current motion matrix.
    // We need to send our matrix that converts from workSpace to localSpace.
	setMotionMatrix( appendTranslation( getStartMotionMatrix(), workSpaceMotion,
			     &workSpaceToLocal ) );

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Scale the handlebox and object based on locater motion.
//
// Use: private
//
SbBool
SoHandleBoxDragger::scaleDrag()
//
////////////////////////////////////////////////////////////////////////
{
    // Get your spaces straight...
	SbMatrix workSpaceToLocal, localToWorkSpace;
	getPartToLocalMatrix("surroundScale",workSpaceToLocal,localToWorkSpace);
	SbMatrix workSpaceToWorld = getLocalToWorldMatrix();
	workSpaceToWorld.multLeft( workSpaceToLocal );
	SbMatrix worldSpaceToWork = getWorldToLocalMatrix();
	worldSpaceToWork.multRight( localToWorkSpace );

    // Get startHitPt in workspace.
	SbVec3f startHitPt;
	worldSpaceToWork.multVecMatrix(getWorldStartingPoint(), startHitPt);

    // determine the center for scaling...
	SbVec3f  scaleCenter;
	if ( !ctlDown ) {
	    // In this case, always want to scale towards center.
	    switch( currentState ) {
		case PX_PY_PZ_3D_SCALE:
		case PX_PY_NZ_3D_SCALE:
		case PX_NY_PZ_3D_SCALE:
		case PX_NY_NZ_3D_SCALE:
		case NX_PY_PZ_3D_SCALE:
		case NX_PY_NZ_3D_SCALE:
		case NX_NY_PZ_3D_SCALE:
		case NX_NY_NZ_3D_SCALE:
		case SCALE_GEOM_ONLY:
	    	    scaleCenter.setValue(0, 0, 0);
		    break;

		case RIT_X_SCALE:
		case LFT_X_SCALE:
		    scaleCenter.setValue(  0,  startHitPt[1], startHitPt[2]);
		    break;
		case TOP_Y_SCALE:
		case BOT_Y_SCALE:
		    scaleCenter.setValue(  startHitPt[0],  0, startHitPt[2]);
		    break;
		case FNT_Z_SCALE:
		case BAK_Z_SCALE:
		    scaleCenter.setValue(  startHitPt[0], startHitPt[1],  0);
		    break;
	    }
	}
	else {  // With alt key down, edge being pulled should move and 
		// leave the opposite edge standing still.

	    switch( currentState ) {
		case PX_PY_PZ_3D_SCALE:
		    scaleCenter.setValue( -1, -1, -1 );
		    break;
		case PX_PY_NZ_3D_SCALE:
		    scaleCenter.setValue( -1, -1,  1 );
		    break;
		case PX_NY_PZ_3D_SCALE:
		    scaleCenter.setValue( -1,  1, -1 );
		    break;
		case PX_NY_NZ_3D_SCALE:
		    scaleCenter.setValue( -1,  1,  1 );
		    break;
		case NX_PY_PZ_3D_SCALE:
		    scaleCenter.setValue(  1, -1, -1 );
		    break;
		case NX_PY_NZ_3D_SCALE:
		    scaleCenter.setValue(  1, -1,  1 );
		    break;
		case NX_NY_PZ_3D_SCALE:
		    scaleCenter.setValue(  1,  1, -1 );
		    break;
		case NX_NY_NZ_3D_SCALE:
		    scaleCenter.setValue(  1,  1,  1 );
		    break;

		case RIT_X_SCALE:
		    scaleCenter.setValue( -1,  startHitPt[1], startHitPt[2]);
		    break;
		case LFT_X_SCALE:
		    scaleCenter.setValue(  1,  startHitPt[1], startHitPt[2]);
		    break;
		case TOP_Y_SCALE:
		    scaleCenter.setValue(  startHitPt[0], -1, startHitPt[2]);
		    break;
		case BOT_Y_SCALE:
		    scaleCenter.setValue(  startHitPt[0],  1, startHitPt[2]);
		    break;
		case FNT_Z_SCALE:
		    scaleCenter.setValue(  startHitPt[0], startHitPt[1], -1);
		    break;
		case BAK_Z_SCALE:
		    scaleCenter.setValue(  startHitPt[0], startHitPt[1],  1);
		    break;
		case SCALE_GEOM_ONLY:
		    // This mode always scales about origin.
		    scaleCenter.setValue( 0, 0, 0);
		    break;
	    }
	}

    // Set up projector. 
	motionLineProj->setViewVolume( getViewVolume() );
	motionLineProj->setWorkingSpace( workSpaceToWorld  );
    // We need to re-establish the motion line, because the line should always
    // pass through the scale center.
	motionLineProj->setLine( SbLine( scaleCenter, startHitPt ) );

    // Get newHitPt in workSpace.
	SbVec3f  newHitPt;
	newHitPt 
	    = motionLineProj->project(getNormalizedLocaterPosition());

    // Save the final point in world space.
    // We'll need it if we hit a meta-key to start off the next gesture.
	workSpaceToWorld.multVecMatrix( newHitPt, worldRestartPt);


#define TINY 0.00001

    // set delta to be the proportionate change in distance from 
    // the scaleCenter in each direction.
    SbVec3f oldDiff = startHitPt  - scaleCenter;
    SbVec3f newDiff = newHitPt - scaleCenter;
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
    // We need to send our matrix that converts from workSpace to localSpace.
	setMotionMatrix( appendScale( getStartMotionMatrix(), 
			    delta, scaleCenter, &workSpaceToLocal ) );
    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Scale up only the geometry of the handlebox, by intersecting
//    an imaginary sphere around the center of the handlebox
//
// Use: private
//
SbBool
SoHandleBoxDragger::scaleGeomSizeOnlyDrag()
//
////////////////////////////////////////////////////////////////////////
{
    SbMatrix workSpaceToLocal, localToWorkSpace;
    getPartToLocalMatrix( "surroundScale", workSpaceToLocal, localToWorkSpace);
    SbMatrix workSpaceToWorld = getLocalToWorldMatrix();
    workSpaceToWorld.multLeft( workSpaceToLocal );
    SbMatrix worldSpaceToWork = getWorldToLocalMatrix();
    worldSpaceToWork.multRight( localToWorkSpace );

    // Get startHitPt in workspace.
	SbVec3f startHitPt;
	worldSpaceToWork.multVecMatrix(getWorldStartingPoint(), startHitPt);

    // Get newHitPt in workSpace.
	SbVec3f  newHitPt;
	motionLineProj->setViewVolume( getViewVolume() );
	motionLineProj->setWorkingSpace( workSpaceToWorld  );
	newHitPt 
	    = motionLineProj->project(getNormalizedLocaterPosition());

    // Save the final point in world space.
    // We'll need it if we hit a meta-key to start off the next gesture.
	workSpaceToWorld.multVecMatrix( newHitPt, worldRestartPt);

    // Find the oldRadius and the new radius
	SbVec3f  origin(0,0,0);
	float oldRadius, newRadius;
	oldRadius = (startHitPt - origin).length();
	newRadius = (newHitPt - origin).length();

    // Find the change in scale.
	float delta = 1.0;
	if ( newRadius != 0.0 && oldRadius != 0.0 ) {
	    delta = newRadius / oldRadius;
	}

//OBSOLETE        SbVec3f relScale = getDraggerScale();
//OBSOLETE        relScale *= delta;
//OBSOLETE        setDraggerScale( relScale );

    return TRUE;
}

//
////////////////////////////////////////////////////////////////////////
// keypress/release callback functions
//
// These assure that the proper changes to the highlights, 
// currentState, and projectors are made
//
////////////////////////////////////////////////////////////////////////
//

void
SoHandleBoxDragger::metaKeyChangeCB( void *, SoDragger *inDragger)
{
    SoHandleBoxDragger  *hb = (SoHandleBoxDragger *) inDragger;
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
	      ( ( key == SoKeyboardEvent::LEFT_SHIFT ||
	          key == SoKeyboardEvent::RIGHT_SHIFT ) &&
	        ( hb->currentState == RIT_TRANSLATE ||
	          hb->currentState == LFT_TRANSLATE ||
	          hb->currentState == TOP_TRANSLATE ||
	          hb->currentState == BOT_TRANSLATE ||
	          hb->currentState == FNT_TRANSLATE ||
	          hb->currentState == BAK_TRANSLATE ))) {

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
SoHandleBoxDragger::dragFinish()
//
////////////////////////////////////////////////////////////////////////
{
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
// Use: private
//
void
SoHandleBoxDragger::highlightAxisForScale( SoHandleBoxDragger::WhichAxis axis )
//
////////////////////////////////////////////////////////////////////////
{
    switch ( axis ) {
	case POSY:
	    setSwitchValue( extruder1Switch.getValue(), 1 );
	    if ( !ctlDown )
		setSwitchValue( extruder2Switch.getValue(), 1 );
	    break;
	case NEGY:
	    setSwitchValue( extruder2Switch.getValue(), 1 );
	    if ( !ctlDown )
		setSwitchValue( extruder1Switch.getValue(), 1 );
	    break;
	case NEGX:
	    setSwitchValue( extruder3Switch.getValue(), 1 );
	    if ( !ctlDown )
		setSwitchValue( extruder4Switch.getValue(), 1 );
	    break;
	case POSX:
	    setSwitchValue( extruder4Switch.getValue(), 1 );
	    if ( !ctlDown )
		setSwitchValue( extruder3Switch.getValue(), 1 );
	    break;
	case POSZ:
	    setSwitchValue( extruder5Switch.getValue(), 1 );
	    if ( !ctlDown )
		setSwitchValue( extruder6Switch.getValue(), 1 );
	    break;
	case NEGZ:
	    setSwitchValue( extruder6Switch.getValue(), 1 );
	    if ( !ctlDown )
		setSwitchValue( extruder5Switch.getValue(), 1 );
	    break;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Use: protected
//
void
SoHandleBoxDragger::setAllPartsActive( SbBool onOrOff )
//
////////////////////////////////////////////////////////////////////////
{
    int sVal = (onOrOff == TRUE) ? 1 : 0;

    setSwitchValue(translator1Switch.getValue(), sVal );
    setSwitchValue(translator2Switch.getValue(), sVal );
    setSwitchValue(translator3Switch.getValue(), sVal );
    setSwitchValue(translator4Switch.getValue(), sVal );
    setSwitchValue(translator5Switch.getValue(), sVal );
    setSwitchValue(translator6Switch.getValue(), sVal );

    setSwitchValue(extruder1Switch.getValue(), sVal );
    setSwitchValue(extruder2Switch.getValue(), sVal );
    setSwitchValue(extruder3Switch.getValue(), sVal );
    setSwitchValue(extruder4Switch.getValue(), sVal );
    setSwitchValue(extruder5Switch.getValue(), sVal );
    setSwitchValue(extruder6Switch.getValue(), sVal );

    setSwitchValue(uniform1Switch.getValue(), sVal );
    setSwitchValue(uniform2Switch.getValue(), sVal );
    setSwitchValue(uniform3Switch.getValue(), sVal );
    setSwitchValue(uniform4Switch.getValue(), sVal );
    setSwitchValue(uniform5Switch.getValue(), sVal );
    setSwitchValue(uniform6Switch.getValue(), sVal );
    setSwitchValue(uniform7Switch.getValue(), sVal );
    setSwitchValue(uniform8Switch.getValue(), sVal );

}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the various switch nodes to highlight the correct parts
//    of the handleBox, depending on the state.
//
// Use: private
//
void
SoHandleBoxDragger::setHighlights()
//
////////////////////////////////////////////////////////////////////////
{
    // Turn notification off temporarily so that
    // all these switches switching dont cause a zillion
    // notifies.
    enableNotify(FALSE);
    
    setFeedbackArrows();

    // Set up the drawStyle node
    SoDrawStyle *ds = (SoDrawStyle *) drawStyle.getValue();
    if ( ds == NULL ) {
        ds = (SoDrawStyle *) getAnyPart( "drawStyle", TRUE );
        ds->style.setValue(SoDrawStyle::LINES);
    }

    ds->lineWidth.setValue(1);
    ds->linePattern.setValue(0xffff);
    ds->setOverride(FALSE);

    // if switching everything on, just do it and return.
    switch ( currentState ) {
	case SCALE_GEOM_ONLY:
	    setAllPartsActive( TRUE );
	    ds->lineWidth.setValue(2);
	    ds->linePattern.setValue(0xf0f0);
	    ds->setOverride(TRUE);
   	    enableNotify(TRUE);
	    touch();
	    return;

	case RIT_TRANSLATE:
	case LFT_TRANSLATE:
	case TOP_TRANSLATE:
	case BOT_TRANSLATE:
	case FNT_TRANSLATE:
	case BAK_TRANSLATE:
	    setAllPartsActive( TRUE );
   	    enableNotify(TRUE);
	    touch();
	    return;
    }

    // The other states require some things on, some things off.
    // Start by turning everything off, then turn on what's needed.
    setAllPartsActive( FALSE );

    switch ( currentState ) {

	case PX_PY_PZ_3D_SCALE:
	    setSwitchValue( uniform1Switch.getValue(), 1 );
	    highlightAxisForScale( POSX );
	    highlightAxisForScale( POSY );
	    highlightAxisForScale( POSZ );
	    break;
	case PX_PY_NZ_3D_SCALE:
	    setSwitchValue( uniform2Switch.getValue(), 1 );
	    highlightAxisForScale( POSX );
	    highlightAxisForScale( POSY );
	    highlightAxisForScale( NEGZ );
	    break;
	case PX_NY_PZ_3D_SCALE:
	    setSwitchValue( uniform3Switch.getValue(), 1 );
	    highlightAxisForScale( POSX );
	    highlightAxisForScale( NEGY );
	    highlightAxisForScale( POSZ );
	    break;
	case PX_NY_NZ_3D_SCALE:
	    setSwitchValue( uniform4Switch.getValue(), 1 );
	    highlightAxisForScale( POSX );
	    highlightAxisForScale( NEGY );
	    highlightAxisForScale( NEGZ );
	    break;
	case NX_PY_PZ_3D_SCALE:
	    setSwitchValue( uniform5Switch.getValue(), 1 );
	    highlightAxisForScale( NEGX );
	    highlightAxisForScale( POSY );
	    highlightAxisForScale( POSZ );
	    break;
	case NX_PY_NZ_3D_SCALE:
	    setSwitchValue( uniform6Switch.getValue(), 1 );
	    highlightAxisForScale( NEGX );
	    highlightAxisForScale( POSY );
	    highlightAxisForScale( NEGZ );
	    break;
	case NX_NY_PZ_3D_SCALE:
	    setSwitchValue( uniform7Switch.getValue(), 1 );
	    highlightAxisForScale( NEGX );
	    highlightAxisForScale( NEGY );
	    highlightAxisForScale( POSZ );
	    break;
	case NX_NY_NZ_3D_SCALE:
	    setSwitchValue( uniform8Switch.getValue(), 1 );
	    highlightAxisForScale( NEGX );
	    highlightAxisForScale( NEGY );
	    highlightAxisForScale( NEGZ );
	    break;

	case RIT_X_SCALE:
	    highlightAxisForScale( POSX );
	    break;
	case LFT_X_SCALE:
	    highlightAxisForScale( NEGX );
	    break;
	case TOP_Y_SCALE:
	    highlightAxisForScale( POSY );
	    break;
	case BOT_Y_SCALE:
	    highlightAxisForScale( NEGY );
	    break;
	case FNT_Z_SCALE:
	    highlightAxisForScale( POSZ );
	    break;
	case BAK_Z_SCALE:
	    highlightAxisForScale( NEGZ );
	    break;

	case INACTIVE:
	default:
	    break;
    }

    // Turn notification back on and cause one notify
    // to eminate from the dragger
    enableNotify(TRUE);
    touch();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the various switch nodes to highlight the correct parts
//    of the handleBox, depending on the state.
//
// Use: private
//
void
SoHandleBoxDragger::setFeedbackArrows()
//
////////////////////////////////////////////////////////////////////////
{
    setSwitchValue( arrow1Switch.getValue(), SO_SWITCH_NONE  );
    setSwitchValue( arrow2Switch.getValue(), SO_SWITCH_NONE  );
    setSwitchValue( arrow3Switch.getValue(), SO_SWITCH_NONE  );
    setSwitchValue( arrow4Switch.getValue(), SO_SWITCH_NONE  );
    setSwitchValue( arrow5Switch.getValue(), SO_SWITCH_NONE  );
    setSwitchValue( arrow6Switch.getValue(), SO_SWITCH_NONE  );

    SoTranslation *arrowTrans = (SoTranslation *) arrowTranslation.getValue();
    if (!arrowTrans) {
	setAnyPart("arrowTranslation", new SoTranslation );
        arrowTrans = (SoTranslation *) arrowTranslation.getValue();
    }

    // if switching everything on, just do it and return.
    if ( !ctlDown ) {
	switch ( currentState ) {
	    case RIT_TRANSLATE:
		if ( translateDir == -1 || translateDir == 1 ) {
		    setSwitchValue( arrow1Switch.getValue(), 0  );
		    setSwitchValue( arrow2Switch.getValue(), 0  );
		}
		if ( translateDir == -1 || translateDir == 2 ) {
		    setSwitchValue( arrow5Switch.getValue(), 0  );
		    setSwitchValue( arrow6Switch.getValue(), 0  );
		}
		arrowTrans->translation.setValue(SbVec3f(1,0,0));
		break;
	    case LFT_TRANSLATE:
		if ( translateDir == -1 || translateDir == 1 ) {
		    setSwitchValue( arrow1Switch.getValue(), 0  );
		    setSwitchValue( arrow2Switch.getValue(), 0  );
		}
		if ( translateDir == -1 || translateDir == 2 ) {
		    setSwitchValue( arrow5Switch.getValue(), 0  );
		    setSwitchValue( arrow6Switch.getValue(), 0  );
		}
		arrowTrans->translation.setValue(SbVec3f(-1,0,0));
		break;
	    case TOP_TRANSLATE:
		if ( translateDir == -1 || translateDir == 0 ) {
		    setSwitchValue( arrow4Switch.getValue(), 0  );
		    setSwitchValue( arrow3Switch.getValue(), 0  );
		}
		if ( translateDir == -1 || translateDir == 2 ) {
		    setSwitchValue( arrow5Switch.getValue(), 0  );
		    setSwitchValue( arrow6Switch.getValue(), 0  );
		}
		arrowTrans->translation.setValue(SbVec3f(0,1,0));
		break;
	    case BOT_TRANSLATE:
		if ( translateDir == -1 || translateDir == 0 ) {
		    setSwitchValue( arrow4Switch.getValue(), 0  );
		    setSwitchValue( arrow3Switch.getValue(), 0  );
		}
		if ( translateDir == -1 || translateDir == 2 ) {
		    setSwitchValue( arrow5Switch.getValue(), 0  );
		    setSwitchValue( arrow6Switch.getValue(), 0  );
		}
		arrowTrans->translation.setValue(SbVec3f(0,-1,0));
		break;
	    case FNT_TRANSLATE:
		if ( translateDir == -1 || translateDir == 0 ) {
		    setSwitchValue( arrow4Switch.getValue(), 0  );
		    setSwitchValue( arrow3Switch.getValue(), 0  );
		}
		if ( translateDir == -1 || translateDir == 1 ) {
		    setSwitchValue( arrow1Switch.getValue(), 0  );
		    setSwitchValue( arrow2Switch.getValue(), 0  );
		}
		arrowTrans->translation.setValue(SbVec3f(0,0,1));
		break;
	    case BAK_TRANSLATE:
		if ( translateDir == -1 || translateDir == 0 ) {
		    setSwitchValue( arrow4Switch.getValue(), 0  );
		    setSwitchValue( arrow3Switch.getValue(), 0  );
		}
		if ( translateDir == -1 || translateDir == 1 ) {
		    setSwitchValue( arrow1Switch.getValue(), 0  );
		    setSwitchValue( arrow2Switch.getValue(), 0  );
		}
		arrowTrans->translation.setValue(SbVec3f(0,0,-1));
		break;
	}
    }
    else {
	switch ( currentState ) {
	    case RIT_TRANSLATE:
		setSwitchValue( arrow4Switch.getValue(), 0  );
		setSwitchValue( arrow3Switch.getValue(), 0  );
		arrowTrans->translation.setValue(SbVec3f(1,0,0));
		break;
	    case LFT_TRANSLATE:
		setSwitchValue( arrow4Switch.getValue(), 0  );
		setSwitchValue( arrow3Switch.getValue(), 0  );
		arrowTrans->translation.setValue(SbVec3f(-1,0,0));
		break;
	    case TOP_TRANSLATE:
		setSwitchValue( arrow1Switch.getValue(), 0  );
		setSwitchValue( arrow2Switch.getValue(), 0  );
		arrowTrans->translation.setValue(SbVec3f(0,1,0));
		break;
	    case BOT_TRANSLATE:
		setSwitchValue( arrow1Switch.getValue(), 0  );
		setSwitchValue( arrow2Switch.getValue(), 0  );
		arrowTrans->translation.setValue(SbVec3f(0,-1,0));
		break;
	    case FNT_TRANSLATE:
		setSwitchValue( arrow5Switch.getValue(), 0  );
		setSwitchValue( arrow6Switch.getValue(), 0  );
		arrowTrans->translation.setValue(SbVec3f(0,0,1));
		break;
	    case BAK_TRANSLATE:
		setSwitchValue( arrow5Switch.getValue(), 0  );
		setSwitchValue( arrow6Switch.getValue(), 0  );
		arrowTrans->translation.setValue(SbVec3f(0,0,-1));
		break;
	}
    }
}

////////////////////////////////////////////////////////////////////
//  Stubs for callbacks
////////////////////////////////////////////////////////////////////
void 
SoHandleBoxDragger::startCB( void *, SoDragger *inDragger )
{
    SoHandleBoxDragger *hb = (SoHandleBoxDragger *) inDragger;
    hb->dragStart();
}

void 
SoHandleBoxDragger::motionCB( void *, SoDragger *inDragger )
{
    SoHandleBoxDragger *hb = (SoHandleBoxDragger *) inDragger;
    hb->drag();
}

void 
SoHandleBoxDragger::finishCB( void *, SoDragger *inDragger )
{
    SoHandleBoxDragger *hb = (SoHandleBoxDragger *) inDragger;
    hb->dragFinish();
}

void
SoHandleBoxDragger::valueChangedCB( void *, SoDragger *inDragger )
{
    SoHandleBoxDragger *m = (SoHandleBoxDragger *) inDragger;
    SbMatrix motMat = m->getMotionMatrix();

    SbVec3f trans, scale;
    SbRotation rot, scaleOrient;
    getTransformFast( motMat, trans, rot, scale, scaleOrient);

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
SoHandleBoxDragger::fieldSensorCB( void *inDragger, SoSensor * )
{
    SoHandleBoxDragger *dragger = (SoHandleBoxDragger *) inDragger;

    // Incorporate the new field values into the matrix 
    SbMatrix motMat = dragger->getMotionMatrix();
    dragger->workFieldsIntoTransform(motMat);

    dragger->setMotionMatrix( motMat );
}

void
SoHandleBoxDragger::setDefaultOnNonWritingFields()
{
    // These nodes may change after construction, but we still
    // don't want to write them out.
    surroundScale.setDefault(TRUE);
    drawStyle.setDefault(TRUE);
    arrowTranslation.setDefault(TRUE);

    // Call the base class...
    SoDragger::setDefaultOnNonWritingFields();
}
