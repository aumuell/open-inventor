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
 |	SoScale2UniformDragger
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
#include <Inventor/draggers/SoScale2UniformDragger.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoSeparator.h>

#include "geom/SoScale2UniformDraggerGeom.h"


SO_KIT_SOURCE(SoScale2UniformDragger);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
SoScale2UniformDragger::SoScale2UniformDragger()
//
////////////////////////////////////////////////////////////////////////
{
    SO_KIT_CONSTRUCTOR(SoScale2UniformDragger);

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
	readDefaultParts("scale2UniformDragger.iv", geomBuffer, sizeof(geomBuffer) );

    SO_KIT_ADD_FIELD(scaleFactor, (1.0, 1.0, 1.0));

    SO_KIT_INIT_INSTANCE();

    // create the parts...
    setPartAsDefault("scaler",        "scale2UniformScaler");
    setPartAsDefault("scalerActive",  "scale2UniformScalerActive");
    setPartAsDefault("feedback",      "scale2UniformFeedback");
    setPartAsDefault("feedbackActive","scale2UniformFeedbackActive");

    // Set the switches to 0...
    setSwitchValue( scalerSwitch.getValue(), 0 );
    setSwitchValue( feedbackSwitch.getValue(), 0 );

    // Create the line projector
    lineProj = new SbLineProjector();
    closestPtOfZeroScale.setValue(0,0,0);

    // add the callbacks to perform the dragging
    addStartCallback(  &SoScale2UniformDragger::startCB );
    addMotionCallback( &SoScale2UniformDragger::motionCB );
    addFinishCallback(   &SoScale2UniformDragger::finishCB );

    // Updates the scaleFactor field when the motionMatrix is set.
    addValueChangedCallback( &SoScale2UniformDragger::valueChangedCB );

    // Updates the motionMatrix when the scaleFactor field is set.
    fieldSensor = new SoFieldSensor( &SoScale2UniformDragger::fieldSensorCB, this);
    fieldSensor->setPriority( 0 );

    setUpConnections( TRUE, TRUE );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
SoScale2UniformDragger::~SoScale2UniformDragger()
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
SoScale2UniformDragger::setUpConnections( SbBool onOff, SbBool doItAlways )
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
SoScale2UniformDragger::dragStart()
//
////////////////////////////////////////////////////////////////////////
{
    // Set the switches to 1...
    setSwitchValue( scalerSwitch.getValue(), 1 );
    setSwitchValue( feedbackSwitch.getValue(), 1 );

    // First, create a line through origin along the z-axis in working space.
    // This line describes the locus of all points 
    // that are the centers of scaling for planes that have a normal
    // equal to the z-axis.
	SbLine zeroScaleLine( SbVec3f(0,0,0), SbVec3f(0,0,1));

    // The change in scale will be based on the change in distance
    // between the mouse and the zeroScaleLine.
    // We will set up a projection line connecting the startHitPt
    // to the zeroScaleLine. Sliding in and out along that line will
    // change the scale of our transform.
	SbVec3f startLocalHitPt = getLocalStartingPoint();
	closestPtOfZeroScale = zeroScaleLine.getClosestPoint(startLocalHitPt);
	lineProj->setLine( SbLine( closestPtOfZeroScale, startLocalHitPt ));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Stretch the dragger according to the motion along the plane
//    projector
//
// Use: private
//
void
SoScale2UniformDragger::drag()
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

    // Figure out the amount to scale by.
    // [1] Find uniform scale change for the two directions.
    //     This is given by the change in distance from the 
    //     'closestPtOfZeroScale' since last time.
    SbVec3f oldDiff = startHitPt  - closestPtOfZeroScale;
    SbVec3f newDiff = newHitPt - closestPtOfZeroScale;

    float oldDist = oldDiff.length();
    float newDist = newDiff.length();

    // [2] Check if we've flipped to the other side of the origin. 
    if ( oldDiff.dot( newDiff ) < 0.0 )
	newDist *= -1.0;

    // [3] Change in scale is the ratio of newDist to oldDist
#define TINY 0.0001
    float scaleMotion = (fabs(newDist) < TINY || fabs(oldDist) < TINY) 
		       ? 1.0 : newDist / oldDist;
#undef TINY

    // Constrain the scaling to be greater than getMinScale().
	if (scaleMotion < getMinScale() )
	    scaleMotion = getMinScale();

    // Set the scaling (to be done relative to scale in startMotionMatrix).
	SbVec3f scl( scaleMotion, scaleMotion, 1 );

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
SoScale2UniformDragger::dragFinish()
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
SoScale2UniformDragger::startCB( void *, SoDragger *inDragger )
{
    SoScale2UniformDragger *m = (SoScale2UniformDragger *) inDragger;
    m->dragStart();
}

void
SoScale2UniformDragger::motionCB( void *, SoDragger *inDragger )
{
    SoScale2UniformDragger *m = (SoScale2UniformDragger *) inDragger;
    m->drag();
}

void
SoScale2UniformDragger::finishCB( void *, SoDragger *inDragger )
{
    SoScale2UniformDragger *m = (SoScale2UniformDragger *) inDragger;
    m->dragFinish();
}

void
SoScale2UniformDragger::valueChangedCB( void *, SoDragger *inDragger )
{
    SoScale2UniformDragger *m = (SoScale2UniformDragger *) inDragger;
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
SoScale2UniformDragger::fieldSensorCB( void *inDragger, SoSensor * )
{
    SoScale2UniformDragger *dragger = (SoScale2UniformDragger *) inDragger;

    // Incorporate the new field value into the matrix...
    SbMatrix motMat = dragger->getMotionMatrix();
    dragger->workFieldsIntoTransform( motMat );

    dragger->setMotionMatrix( motMat );
}
