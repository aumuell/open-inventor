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
 |	SoRotateSphericalDragger
 |
 |   Author(s): Paul Isaacs, Howard Look
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
#include <Inventor/projectors/SbSphereSectionProjector.h>

#include <Inventor/draggers/SoRotateSphericalDragger.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoSeparator.h>

#include "geom/SoRotateSphericalDraggerGeom.h"


SO_KIT_SOURCE(SoRotateSphericalDragger);


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
SoRotateSphericalDragger::SoRotateSphericalDragger()
//
////////////////////////////////////////////////////////////////////////
{
    SO_KIT_CONSTRUCTOR(SoRotateSphericalDragger);

    isBuiltIn = TRUE;

    // Put this stuff under the geomSeparator so it will draw more
    // efficiently.
    SO_KIT_ADD_CATALOG_ENTRY(rotatorSwitch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(rotator, SoSeparator, TRUE,
				rotatorSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(rotatorActive, SoSeparator, TRUE,
				rotatorSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(feedbackSwitch, SoSwitch, TRUE,
				geomSeparator, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(feedback, SoSeparator, TRUE,
				feedbackSwitch, ,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(feedbackActive, SoSeparator, TRUE,
				feedbackSwitch, ,TRUE);

    // read geometry for shared parts
    if (SO_KIT_IS_FIRST_INSTANCE())
	readDefaultParts("rotateSphericalDragger.iv", geomBuffer, sizeof(geomBuffer) );

    SO_KIT_ADD_FIELD(rotation, (0.0, 0.0, 0.0, 1.0));

    SO_KIT_INIT_INSTANCE();

    // create the parts...
    setPartAsDefault("rotator",       "rotateSphericalRotator");
    setPartAsDefault("rotatorActive",
		    "rotateSphericalRotatorActive");
    setPartAsDefault("feedback",      "rotateSphericalFeedback");
    setPartAsDefault("feedbackActive",
		    "rotateSphericalFeedbackActive");

    // Set the switches to 0...
    setSwitchValue( rotatorSwitch.getValue(), 0 );
    setSwitchValue( feedbackSwitch.getValue(), 0 );

    // start with our own default projector
    // the user can replace if they want
    sphereProj = new SbSphereSectionProjector();
    ((SbSphereSectionProjector *)sphereProj)->setRadialFactor( 0.85 );
    userProj = FALSE;

    // add the callbacks to perform the dragging
    addStartCallback(  &SoRotateSphericalDragger::startCB );
    addMotionCallback( &SoRotateSphericalDragger::motionCB );
    addFinishCallback( &SoRotateSphericalDragger::doneCB );

    // Updates the scaleFactor field when the motionMatrix is set.
    addValueChangedCallback( &SoRotateSphericalDragger::valueChangedCB );

    // Updates the motionMatrix when the scaleFactor field is set.
    fieldSensor 
	= new SoFieldSensor(&SoRotateSphericalDragger::fieldSensorCB, this);
    fieldSensor->setPriority( 0 );

    setUpConnections( TRUE, TRUE );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
SoRotateSphericalDragger::~SoRotateSphericalDragger()
//
////////////////////////////////////////////////////////////////////////
{
    if ( sphereProj )
	delete sphereProj;
    if (fieldSensor )
        delete fieldSensor;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Redefines this to also copy the projector.
//
// Use: protected, virtual

void
SoRotateSphericalDragger::copyContents(const SoFieldContainer *fromFC,
				       SbBool copyConnections)
//
////////////////////////////////////////////////////////////////////////
{
    // Do the usual stuff
    SoDragger::copyContents(fromFC, copyConnections);

    // Now, copy the projector variables...

    const SoRotateSphericalDragger *origDragger =
	(const SoRotateSphericalDragger *) fromFC;

    if ( sphereProj )
	delete sphereProj;

    if ( origDragger->sphereProj )
	sphereProj = (SbSphereProjector *)
	    origDragger->sphereProj->copy();
    else
	sphereProj = NULL;

    userProj = origDragger->userProj;
}

//    detach/attach any sensors, callbacks, and/or field connections.
//    Called by:            start/end of SoBaseKit::readInstance
//    and on new copy by:   start/end of SoBaseKit::copy.
//    Classes that redefine must call setUpConnections(TRUE,TRUE) 
//    at end of constructor.
//    Returns the state of the node when this was called.
SbBool
SoRotateSphericalDragger::setUpConnections( SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( onOff ) {

	// We connect AFTER base class.
	SoDragger::setUpConnections( onOff, FALSE );

	// Call the sensor CBs to make things are up-to-date.
	fieldSensorCB( this, NULL );

	// Connect the field sensors
	if (fieldSensor->getAttachedField() != &rotation)
	    fieldSensor->attach( &rotation );
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
//    Set the sphere projector back to the default or to something
//  new.
//;
// Use: public
//
void
SoRotateSphericalDragger::setProjector(SbSphereProjector *proj)
//
////////////////////////////////////////////////////////////////////////
{
    if ( sphereProj )
	delete sphereProj;

    if (proj == NULL) {
	// passing in null resets the projector to the default
	userProj = FALSE;
	sphereProj = new SbSphereSectionProjector();
	((SbSphereSectionProjector *)sphereProj)->setRadialFactor( 0.85 );
    }
    else {
	// use the projector passed in
	sphereProj = proj;
	userProj = TRUE;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Rotate the rotateSphericalDragger based on mouse motion.
//
// Use: private
//
void
SoRotateSphericalDragger::dragStart()
//
////////////////////////////////////////////////////////////////////////
{
    // Set the switches to 1...
    setSwitchValue( rotatorSwitch.getValue(), 1 );
    setSwitchValue( feedbackSwitch.getValue(), 1 );

    // Establish the projector sphere in working space.
    // Working space is space at end of motion matrix.
	SbVec3f startLocalHitPt = getLocalStartingPoint();
        SbVec3f rad = startLocalHitPt - SbVec3f(0,0,0);
        sphereProj->setSphere( SbSphere( SbVec3f(0,0,0), rad.length()) );

    // If the hit point is on the near side of the center from where
    // the eye is, then tell the projector to intersect front.
    // Else, tell it to intersect back.
	SbMatrix lclToWld = getLocalToWorldMatrix();
	sphereProj->setViewVolume( getViewVolume() );
	sphereProj->setWorkingSpace( lclToWld );
	if (getFrontOnProjector() ==  USE_PICK )
	    sphereProj->setFront( sphereProj->isPointInFront( startLocalHitPt));
	else if (getFrontOnProjector() ==  FRONT )
	    sphereProj->setFront( TRUE );
	else
	    sphereProj->setFront( FALSE );

    // The spherical (and cylindrical) projectors are sort of weird in
    // that the initial hit defines the projector, but may not actually lie
    // on the cylinder or sphere selected.
    // This happens when the inital hit is too close to the edge to fit within
    // 'tolerance.'
    // So, to be sure that we get accurate performance, we need to 
    // project the mouse onto the projector once it is defined in order to 
    // get our prevWorldHitPt 
	SbVec3f localProjPt, worldProjPt;
	localProjPt = sphereProj->project( getNormalizedLocaterPosition()); 
	lclToWld.multVecMatrix( localProjPt, worldProjPt ); 

    // Unlike other draggers, we MUST use incremental changes,
    // since the sphere projector does not give consistent results across
    // long motions. Each motion must be fairly short. So we'll be saving
    // each previous mouse point and motion matrix.
    // Save the hit point, defined in world space.
	prevWorldHitPt = worldProjPt;
    // Save the matrix.
	prevMotionMatrix = getMotionMatrix();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Rotate the rotateSphericalDragger based on mouse motion.
//
// Use: private
//
void
SoRotateSphericalDragger::drag()
//
////////////////////////////////////////////////////////////////////////
{
    // Unlike other draggers, we MUST use incremental changes,
    // since the sphere projector does not give consistent results across
    // long motions. Each motion must be fairly short. So we'll be saving
    // each previous mouse point and motion matrix.

    // Set up the projector space and view.
    // Working space is space at end of motion matrix.
	sphereProj->setViewVolume( getViewVolume() );    
	sphereProj->setWorkingSpace( getLocalToWorldMatrix() );

    // Get newHitPt and prevHitPt in workspace.
	SbVec3f prevHitPt;
	getWorldToLocalMatrix().multVecMatrix(prevWorldHitPt, prevHitPt);

	SbVec3f newHitPt = sphereProj->project( getNormalizedLocaterPosition());

    // deltaRot is how much we rotated since last time.
        SbRotation deltaRot = sphereProj->getRotation( prevHitPt, newHitPt );

    // Append this to the prevMotionMatrix, which we saved last time,
    // to find the new matrix.
	SbMatrix newMotionMatrix = 
	    appendRotation( prevMotionMatrix, deltaRot, SbVec3f(0,0,0));

    // Save the parameters we need to save:
	// Convert hit to world space to get prevWorldHitPt for next time..
	getLocalToWorldMatrix().multVecMatrix(newHitPt,prevWorldHitPt);
        // Save the incremental results of our matrix.
	prevMotionMatrix = newMotionMatrix;

    // Set the new motion matrix
        setMotionMatrix( newMotionMatrix );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Rotate the rotateSphericalDragger based on mouse motion.
//
// Use: private
//
void
SoRotateSphericalDragger::dragFinish()
//
////////////////////////////////////////////////////////////////////////
{
    // Set the switches to 0...
    setSwitchValue( rotatorSwitch.getValue(), 0 );
    setSwitchValue( feedbackSwitch.getValue(), 0 );
}    

////////////////////////////////////////////////////////////////////
//  Stubs for callbacks
////////////////////////////////////////////////////////////////////
void
SoRotateSphericalDragger::startCB( void *, SoDragger *inDragger )
{
    SoRotateSphericalDragger *dl = (SoRotateSphericalDragger *) inDragger;
    dl->dragStart();
}

void
SoRotateSphericalDragger::motionCB( void *, SoDragger *inDragger )
{
    SoRotateSphericalDragger *dl = (SoRotateSphericalDragger *) inDragger;
    dl->drag();
}

void
SoRotateSphericalDragger::doneCB( void *, SoDragger *inDragger )
{
    SoRotateSphericalDragger *dl = (SoRotateSphericalDragger *) inDragger;
    dl->dragFinish();
}

void
SoRotateSphericalDragger::valueChangedCB( void *, SoDragger *inDragger )
{
    SoRotateSphericalDragger *m = (SoRotateSphericalDragger *) inDragger;
    SbMatrix motMat = m->getMotionMatrix();

    SbVec3f    trans, scale;
    SbRotation rot, scaleOrient;
    getTransformFast( motMat, trans, rot, scale, scaleOrient );

    // Disconnect the field sensor
    m->fieldSensor->detach();

    if ( m->rotation.getValue() != rot )
	m->rotation = rot;

    // Reconnect the field sensor
    m->fieldSensor->attach( &(m->rotation) );
}

void
SoRotateSphericalDragger::fieldSensorCB( void *inDragger, SoSensor * )
{
    SoRotateSphericalDragger *dragger 
			    = (SoRotateSphericalDragger *) inDragger;

    // Incorporate the new field value into the matrix...
    SbMatrix motMat = dragger->getMotionMatrix();
    dragger->workFieldsIntoTransform( motMat );

    dragger->setMotionMatrix( motMat );
}
