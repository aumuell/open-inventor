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
 |	SoScaleUniformDragger
 |
 |   Author(s): Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <stdio.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoPath.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include <Inventor/projectors/SbLineProjector.h>
#include <Inventor/draggers/SoScaleUniformDragger.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>

#include "geom/SoScaleUniformDraggerGeom.h"


SO_KIT_SOURCE(SoScaleUniformDragger);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
SoScaleUniformDragger::SoScaleUniformDragger()
//
////////////////////////////////////////////////////////////////////////
{
    SO_KIT_CONSTRUCTOR(SoScaleUniformDragger);

    isBuiltIn = TRUE;

    // Put this stuff under the geomSeparator so it will draw more
    // efficiently.
    SO_KIT_ADD_CATALOG_ENTRY(scalerSwitch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(scaler, SoSeparator, TRUE,
				scalerSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(scalerActive, SoSeparator, TRUE,
				scalerSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(feedbackSwitch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(feedback, SoSeparator, TRUE,
				feedbackSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(feedbackActive, SoSeparator, TRUE,
				feedbackSwitch, ,TRUE);

    // read geometry for shared parts
    if (SO_KIT_IS_FIRST_INSTANCE())
	readDefaultParts("scaleUniformDragger.iv", geomBuffer, sizeof(geomBuffer) );

    SO_KIT_ADD_FIELD(scaleFactor, (1.0, 1.0, 1.0));

    SO_KIT_INIT_INSTANCE();

    // create the parts...
    setPartAsDefault("scaler",         "scaleUniformScaler");
    setPartAsDefault("scalerActive",   "scaleUniformScalerActive");
    setPartAsDefault("feedback",       "scaleUniformFeedback");
    setPartAsDefault("feedbackActive", "scaleUniformFeedbackActive");

    // Set the switches to 0...
    setSwitchValue( scalerSwitch.getValue(), 0 );
    setSwitchValue( feedbackSwitch.getValue(), 0 );

    // Create the line projector
    lineProj = new SbLineProjector();

    // add the callbacks to perform the dragging
    addStartCallback(  &SoScaleUniformDragger::startCB );
    addMotionCallback( &SoScaleUniformDragger::motionCB );
    addFinishCallback(   &SoScaleUniformDragger::finishCB );

    // Updates the scaleFactor field when the motionMatrix is set.
    addValueChangedCallback( &SoScaleUniformDragger::valueChangedCB );

    // Updates the motionMatrix when the scaleFactor field is set.
    fieldSensor = new SoFieldSensor( &SoScaleUniformDragger::fieldSensorCB, this);
    fieldSensor->setPriority( 0 );

    setUpConnections( TRUE, TRUE );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
SoScaleUniformDragger::~SoScaleUniformDragger()
//
////////////////////////////////////////////////////////////////////////
{
    delete lineProj;
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
SoScaleUniformDragger::setUpConnections( SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( onOff ) {

	// We connect AFTER base class.
	SoDragger::setUpConnections( onOff, FALSE );

	// Call the sensor CBs to make things are up-to-date.
	fieldSensorCB( this, NULL );

	// Connect the field sensors
	if (fieldSensor->getAttachedField() != &scaleFactor)
	    fieldSensor->attach( &scaleFactor );
    }
    else {

	// We disconnect BEFORE base class.

	// Disconnect the field sensors.
	if (fieldSensor->getAttachedField())
	    fieldSensor->detach();

	SoDragger::setUpConnections( onOff, FALSE );
    }

    return !(connectionsSetUp = onOff);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Set up the highlighting, projector, and the initial hit on
//    the dragger
//
// Use: private
//
void
SoScaleUniformDragger::dragStart()
//
////////////////////////////////////////////////////////////////////////
{
    // Set the switches to 1...
    setSwitchValue( scalerSwitch.getValue(), 1 );
    setSwitchValue( feedbackSwitch.getValue(), 1 );

    // Establish the projector line in working space.
    // Working space is space at end of motion matrix.
    // Scaling line connects the origin to the startHitPt
	SbVec3f startLocalHitPt = getLocalStartingPoint();
        lineProj->setLine( SbLine( SbVec3f(0,0,0), startLocalHitPt ) );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Stretch the dragger according to the motion along the line
//    projector
//
// Use: private
//
void
SoScaleUniformDragger::drag()
//
////////////////////////////////////////////////////////////////////////
{
    // Set up the projector space and view.
    // Working space is space at end of motion matrix.
	lineProj->setViewVolume( getViewVolume() );    
	lineProj->setWorkingSpace( getLocalToWorldMatrix() );

    // Get newHitPt and startHitPt in workspace.
	SbVec3f newHitPt = lineProj->project( getNormalizedLocaterPosition()); 
	SbVec3f startHitPt = getLocalStartingPoint();

    // [1] Find the distance from the old and new projected points to origin.
    float oldDist = startHitPt.length();
    float newDist = newHitPt.length();

    // [2] Check if we've flipped to the other side of the origin. 
    if ( startHitPt.dot( newHitPt ) < 0.0 )
	newDist *= -1.0;

    // [3] Change in scale is the ratio of newDist to oldDist
#define TINY 0.0001
    float delta = ( fabs(oldDist) < TINY || fabs(newDist) < TINY) 
		    ? 1.0 : newDist / oldDist;
#undef TINY

    // Constrain the scaling to be greater than getMinScale().
	if (delta < getMinScale() )
	    delta = getMinScale();

    // Set the scaling (to be done relative to scale in startMotionMatrix).
	SbVec3f scl( delta, delta, delta );

    // Append this to the startMotionMatrix, which we saved at the beginning
    // of the drag, to find the current motion matrix.
	setMotionMatrix( appendScale( getStartMotionMatrix(), scl,SbVec3f(0,0,0)));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//
// Use: private
//
void
SoScaleUniformDragger::dragFinish()
//
////////////////////////////////////////////////////////////////////////
{
    // Set the switches to 0...
    setSwitchValue( scalerSwitch.getValue(), 0 );
    setSwitchValue( feedbackSwitch.getValue(), 0 );
}    

////////////////////////////////////////////////////////////////////
//  Stubs for callbacks
////////////////////////////////////////////////////////////////////
void
SoScaleUniformDragger::startCB( void *, SoDragger *inDragger )
{
    SoScaleUniformDragger *m = (SoScaleUniformDragger *) inDragger;
    m->dragStart();
}

void
SoScaleUniformDragger::motionCB( void *, SoDragger *inDragger )
{
    SoScaleUniformDragger *m = (SoScaleUniformDragger *) inDragger;
    m->drag();
}

void
SoScaleUniformDragger::finishCB( void *, SoDragger *inDragger )
{
    SoScaleUniformDragger *m = (SoScaleUniformDragger *) inDragger;
    m->dragFinish();
}

void
SoScaleUniformDragger::valueChangedCB( void *, SoDragger *inDragger )
{
    SoScaleUniformDragger *m = (SoScaleUniformDragger *) inDragger;
    SbMatrix motMat = m->getMotionMatrix();

    SbVec3f    trans, scale;
    SbRotation rot, scaleOrient;
    getTransformFast( motMat, trans, rot, scale, scaleOrient );

    // Disconnect the field sensor
    m->fieldSensor->detach();

    if ( m->scaleFactor.getValue() != scale )
	m->scaleFactor = scale;

    // Reconnect the field sensor
    m->fieldSensor->attach( &(m->scaleFactor) );
}

void
SoScaleUniformDragger::fieldSensorCB( void *inDragger, SoSensor * )
{
    SoScaleUniformDragger *dragger = (SoScaleUniformDragger *) inDragger;

    // Incorporate the new field value into the matrix...
    SbMatrix motMat = dragger->getMotionMatrix();
    dragger->workFieldsIntoTransform( motMat );

    dragger->setMotionMatrix( motMat );
}
