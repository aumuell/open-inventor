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
 * Copyright (C) 1992   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Classes:
 |	SoTabPlaneDragger
 |
 |   Author(s): Paul Isaacs, Howard Look
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SbLinear.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoPath.h>
#include <Inventor/SoPickedPoint.h>

#include <Inventor/events/SoEvent.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/sensors/SoFieldSensor.h>

#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoHandleEventAction.h>

#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/elements/SoProjectionMatrixElement.h>
#include <Inventor/elements/SoViewingMatrixElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>

#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoTransform.h>

#include <Inventor/draggers/SoTabPlaneDragger.h>

#include "geom/SoTabPlaneDraggerGeom.h"

#ifndef __sgi
#define _ABS(x)     ((x) < 0 ? -(x) : (x))
#endif


SO_KIT_SOURCE(SoTabPlaneDragger);

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
SoTabPlaneDragger::SoTabPlaneDragger()
{
    SO_KIT_CONSTRUCTOR(SoTabPlaneDragger);

    isBuiltIn = TRUE;

    SO_KIT_ADD_CATALOG_ENTRY( planeSwitch, SoSwitch, TRUE,
				 geomSeparator, , FALSE );
    SO_KIT_ADD_CATALOG_ENTRY( translator, SoSeparator, TRUE,
				 planeSwitch, , TRUE );
    SO_KIT_ADD_CATALOG_ENTRY( scaleTabs, SoSeparator, TRUE,
				 planeSwitch, , FALSE );
    SO_KIT_ADD_CATALOG_ENTRY( scaleTabMaterial, SoMaterial, TRUE,
				 scaleTabs, , TRUE );
    SO_KIT_ADD_CATALOG_ENTRY( scaleTabHints, SoShapeHints, TRUE,
				 scaleTabs, , TRUE );
    SO_KIT_ADD_CATALOG_ENTRY( scaleTabMaterialBinding, SoMaterialBinding, TRUE,
				 scaleTabs, , FALSE );
    SO_KIT_ADD_CATALOG_ENTRY( scaleTabNormalBinding, SoNormalBinding, TRUE,
				 scaleTabs, , FALSE );
    SO_KIT_ADD_CATALOG_ENTRY( scaleTabNormal, SoNormal, TRUE,
				 scaleTabs, , FALSE );
    SO_KIT_ADD_CATALOG_ENTRY( edgeScaleCoords, SoCoordinate3, TRUE,
				 scaleTabs, , FALSE );
    SO_KIT_ADD_CATALOG_ENTRY( edgeScaleTab0, SoIndexedFaceSet, TRUE,
				 scaleTabs, , FALSE );
    SO_KIT_ADD_CATALOG_ENTRY( edgeScaleTab1, SoIndexedFaceSet, TRUE,
				 scaleTabs, , FALSE );
    SO_KIT_ADD_CATALOG_ENTRY( edgeScaleTab2, SoIndexedFaceSet, TRUE,
				 scaleTabs, , FALSE );
    SO_KIT_ADD_CATALOG_ENTRY( edgeScaleTab3, SoIndexedFaceSet, TRUE,
				 scaleTabs, , FALSE );
    SO_KIT_ADD_CATALOG_ENTRY( cornerScaleCoords, SoCoordinate3, TRUE,
				 scaleTabs, , FALSE );
    SO_KIT_ADD_CATALOG_ENTRY( cornerScaleTab0, SoIndexedFaceSet, TRUE,
				 scaleTabs, , FALSE );
    SO_KIT_ADD_CATALOG_ENTRY( cornerScaleTab1, SoIndexedFaceSet, TRUE,
				 scaleTabs, , FALSE );
    SO_KIT_ADD_CATALOG_ENTRY( cornerScaleTab2, SoIndexedFaceSet, TRUE,
				 scaleTabs, , FALSE );
    SO_KIT_ADD_CATALOG_ENTRY( cornerScaleTab3, SoIndexedFaceSet, TRUE,
				 scaleTabs, , FALSE );

    if (SO_KIT_IS_FIRST_INSTANCE())
	readDefaultParts("tabPlaneDragger.iv", geomBuffer, sizeof(geomBuffer));

    SO_KIT_ADD_FIELD(translation, (0.0, 0.0, 0.0));
    SO_KIT_ADD_FIELD(scaleFactor, (1.0, 1.0, 1.0));

    SO_KIT_INIT_INSTANCE();

    // Create All the parts...

	setPartAsDefault("translator",       "tabPlaneTranslator");
	setPartAsDefault("scaleTabMaterial", 
		"tabPlaneScaleTabMaterial");
	setPartAsDefault("scaleTabHints",   "tabPlaneScaleTabHints");

	// Initialize the materialBinding part  
	SoMaterialBinding *mb = new SoMaterialBinding;
	mb->value = SoMaterialBinding::OVERALL;
	setAnyPartAsDefault("scaleTabMaterialBinding",  mb );

	// Initialize the normalBinding part  
	SoNormalBinding *nb = new SoNormalBinding;
	nb->value = SoNormalBinding::OVERALL;
	setAnyPartAsDefault("scaleTabNormalBinding",  nb );

	// Initialize the normal part  
	SoNormal *nrm = new SoNormal;
	nrm->vector = SbVec3f( 0, 0, 1);
	setAnyPartAsDefault("scaleTabNormal",  nrm );

	// Initialize the coordinate nodes.
        SbVec3f *pts;
	SoCoordinate3    *crdn;

	crdn = SO_GET_ANY_PART(this,"edgeScaleCoords", SoCoordinate3);
	crdn->point.setNum(16);
        pts = crdn->point.startEditing();
	pts[0].setValue(  -.1,   .9, .002);
	pts[1].setValue(   .1,   .9, .002);
	pts[2].setValue(   .1,  1,  .002);
	pts[3].setValue(  -.1,  1,  .002);
	pts[4].setValue(  -.1, -1, .002);
	pts[5].setValue(   .1, -1, .002);
	pts[6].setValue(   .1,  -.9, .002);
	pts[7].setValue(  -.1,  -.9, .002);
	pts[8].setValue(   .9,  -.1, .002);
	pts[9].setValue(  1,    -.1, .002);
	pts[10].setValue( 1,     .1, .002);
	pts[11].setValue(  .9,   .1, .002);
	pts[12].setValue(-1,    -.1, .002);
	pts[13].setValue( -.9,  -.1, .002);
	pts[14].setValue( -.9,   .1, .002);
	pts[15].setValue(-1,     .1, .002);
        crdn->point.finishEditing();

	crdn = SO_GET_ANY_PART(this,"cornerScaleCoords", SoCoordinate3);
	crdn->point.setNum(16);
        pts = crdn->point.startEditing();
	pts[0].setValue( .9,  .9, .002);
	pts[1].setValue( 1,  .9, .002);
	pts[2].setValue( 1,  1,  .002);
	pts[3].setValue( .9,  1,  .002);
	pts[4].setValue( .9,  -1, .002);
	pts[5].setValue( 1,  -1, .002);
	pts[6].setValue( 1, -.9, .002);
	pts[7].setValue( .9, -.9, .002);
	pts[8].setValue( -1, -1, .002);
	pts[9].setValue( -.9,  -1, .002);
	pts[10].setValue( -.9,  -.9, .002);
	pts[11].setValue( -1, -.9, .002);
	pts[12].setValue( -1,  .9, .002);
	pts[13].setValue( -.9, .9, .002);
	pts[14].setValue( -.9, 1, .002);
	pts[15].setValue( -1,   1, .002);
        crdn->point.finishEditing();

	// Initialize the indexed face set nodes
	int32_t             *inds;
	SoIndexedFaceSet *fsn;

	fsn = new SoIndexedFaceSet;
	fsn->coordIndex.setNum(5);
        inds = fsn->coordIndex.startEditing();
	inds[0] = 0;
	inds[1] = 1;
	inds[2] = 2;
	inds[3] = 3;
	inds[4] = -1;
        fsn->coordIndex.finishEditing();
	setAnyPartAsDefault("edgeScaleTab0", fsn );
	setAnyPartAsDefault("cornerScaleTab0", fsn->copy() );

	fsn = new SoIndexedFaceSet;
	fsn->coordIndex.setNum(5);
        inds = fsn->coordIndex.startEditing();
	inds[0] = 4;
	inds[1] = 5;
	inds[2] = 6;
	inds[3] = 7;
	inds[4] = -1;
        fsn->coordIndex.finishEditing();
	setAnyPartAsDefault("edgeScaleTab1", fsn );
	setAnyPartAsDefault("cornerScaleTab1", fsn->copy() );

	fsn = new SoIndexedFaceSet;
	fsn->coordIndex.setNum(5);
        inds = fsn->coordIndex.startEditing();
	inds[0] = 8;
	inds[1] = 9;
	inds[2] = 10;
	inds[3] = 11;
	inds[4] = -1;
        fsn->coordIndex.finishEditing();
	setAnyPartAsDefault("edgeScaleTab2", fsn );
	setAnyPartAsDefault("cornerScaleTab2", fsn->copy() );

	fsn = new SoIndexedFaceSet;
	fsn->coordIndex.setNum(5);
        inds = fsn->coordIndex.startEditing();
	inds[0] = 12;
	inds[1] = 13;
	inds[2] = 14;
	inds[3] = 15;
	inds[4] = -1;
        fsn->coordIndex.finishEditing();
	setAnyPartAsDefault("edgeScaleTab3", fsn );
	setAnyPartAsDefault("cornerScaleTab3", fsn->copy() );


    // Set the switch to display everything under it...
    SoSwitch *sw = SO_GET_ANY_PART(this,"planeSwitch",SoSwitch);
    setSwitchValue( sw, SO_SWITCH_ALL);

    // Create the projectors
    lineProj = new SbLineProjector();
    planeProj = new SbPlaneProjector();

    // add the callbacks to perform the dragging
    addStartCallback(  &SoTabPlaneDragger::startCB );
    addMotionCallback( &SoTabPlaneDragger::motionCB );
    addFinishCallback( &SoTabPlaneDragger::finishCB );

    // add the callback to update things each time a meta key changes.
    addOtherEventCallback( &SoTabPlaneDragger::metaKeyChangeCB );

    // Updates the translation and scale fields when the motionMatrix is set.
    addValueChangedCallback( &SoTabPlaneDragger::valueChangedCB );

    // Updates the motionMatrix when the translationFactor field is set.
    translFieldSensor
		= new SoFieldSensor( &SoTabPlaneDragger::fieldSensorCB, this);
    translFieldSensor->setPriority( 0 );

    // Updates the motionMatrix when the scaleFactor field is set.
    scaleFieldSensor
		= new SoFieldSensor( &SoTabPlaneDragger::fieldSensorCB, this);
    scaleFieldSensor->setPriority( 0 );

    currentState = INACTIVE;
    restartState = INACTIVE;
    adjustScaleTabSize();

    setUpConnections( TRUE, TRUE );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
SoTabPlaneDragger::~SoTabPlaneDragger()
//
////////////////////////////////////////////////////////////////////////
{
    delete lineProj;
    delete planeProj;

    if (translFieldSensor)
	delete translFieldSensor;
    if (scaleFieldSensor)
	delete scaleFieldSensor;
}

//    detach/attach any sensors, callbacks, and/or field connections.
//    Called by:            start/end of SoBaseKit::readInstance
//    and on new copy by:   start/end of SoBaseKit::copy.
//    Classes that redefine must call setUpConnections(TRUE,TRUE) 
//    at end of constructor.
//    Returns the state of the node when this was called.
SbBool
SoTabPlaneDragger::setUpConnections( SbBool onOff, SbBool doItAlways )
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
//    Move the tabPlaneDragger based on mouse motion.
//
// Use: private
//
void
SoTabPlaneDragger::dragStart()
//
////////////////////////////////////////////////////////////////////////
{
    const SoPath *pickPath = getPickPath();

    currentState = INACTIVE;
    shftDown = getEvent()->wasShiftDown();

    if (restartState != INACTIVE ) {
	currentState = restartState;
    }
    else if (pickPath && pickPath->containsNode(translator.getValue()) ||
        getSurrogatePartPickedName() == "translator" ) {
	currentState = TRANSLATING;
    }
    else if ( pickPath && pickPath->containsNode( scaleTabs.getValue() )) {

	if (pickPath->containsNode( edgeScaleTab0.getValue()) ) {
	    currentScalePatch = 0;
	    currentState = EDGE_SCALING;
	}
	else if (pickPath->containsNode( edgeScaleTab1.getValue()) ) {
	    currentScalePatch = 1;
	    currentState = EDGE_SCALING;
	}
	else if (pickPath->containsNode( edgeScaleTab2.getValue()) ) {
	    currentScalePatch = 2;
	    currentState = EDGE_SCALING;
	}
	else if (pickPath->containsNode( edgeScaleTab3.getValue()) ) {
	    currentScalePatch = 3;
	    currentState = EDGE_SCALING;
	}
	else if (pickPath->containsNode( cornerScaleTab0.getValue()) ) {
	    currentScalePatch = 0;
	    currentState = CORNER_SCALING;
	}
	else if (pickPath->containsNode( cornerScaleTab1.getValue()) ) {
	    currentScalePatch = 1;
	    currentState = CORNER_SCALING;
	}
	else if (pickPath->containsNode( cornerScaleTab2.getValue()) ) {
	    currentScalePatch = 2;
	    currentState = CORNER_SCALING;
	}
	else if (pickPath->containsNode( cornerScaleTab3.getValue()) ) {
	    currentScalePatch = 3;
	    currentState = CORNER_SCALING;
	}

	if ( currentState == CORNER_SCALING  && shftDown )
	    currentState = UNIFORM_SCALING;
    }

    if (currentState == TRANSLATING )
        translateStart();
    else if (currentState == EDGE_SCALING )
        edgeScaleStart();
    else if (currentState == CORNER_SCALING )
        cornerScaleStart();
    else if (currentState == UNIFORM_SCALING )
        scaleUniformStart();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//
// Use: private
//
void
SoTabPlaneDragger::drag()
//
////////////////////////////////////////////////////////////////////////
{
    if ( currentState == TRANSLATING )
	translateDrag();
    else if ( currentState == EDGE_SCALING )
	edgeScaleDrag();
    else if ( currentState == CORNER_SCALING )
	cornerScaleDrag();
    else if ( currentState == UNIFORM_SCALING )
	scaleUniformDrag();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//
// Use: private
//
void
SoTabPlaneDragger::dragFinish()
//
////////////////////////////////////////////////////////////////////////
{
    // turn off highlighting and adjust scale tabs
    currentState = INACTIVE;
    restartState = INACTIVE;
    adjustScaleTabSize();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes for motion
//
void
SoTabPlaneDragger::translateStart()
//
////////////////////////////////////////////////////////////////////////
{
    // This is the point we'll use if a metaKey callback makes us re-start.
	worldRestartPt = getWorldStartingPoint();

    // Establish the projector line in local space.
	SbVec3f startLocalHitPt = getLocalStartingPoint();

	SbVec3f planeNormal(0,0,1);
        planeProj->setPlane( SbPlane( planeNormal, startLocalHitPt ) );

    // as of yet, translation direction (for 1-D translating) is undefined.
        translateDir = -1;
}

////////////////////////////////////////////////////////////////////////
//
void
SoTabPlaneDragger::translateDrag()
//
////////////////////////////////////////////////////////////////////////
{
    if (shftDown && ! isAdequateConstraintMotion())
	return;

    // Get startHitPt in local space.
	SbVec3f startHitPt = getLocalStartingPoint();;

    // Calculate intersection with plane projector.
    // Find mouse hit on plane currently in use...
	planeProj->setViewVolume( getViewVolume() );
	planeProj->setWorkingSpace( getLocalToWorldMatrix() );
	SbVec3f newHitPt
	    = planeProj->project(getNormalizedLocaterPosition());

    // If we need to start a new gesture, we'll carry on from this pt.
	getLocalToWorldMatrix().multVecMatrix( newHitPt, worldRestartPt);

    // find the difference between current and previous intersections.
        SbVec3f localMotion = newHitPt - startHitPt;

    if ( !shftDown )
	// constrainKey not down. Clear the 1-D translation dir.
	translateDir = -1;
    else {
	// the constrain key is pressed. This means 1-D translation.
	if ( translateDir == -1 ) {
	    // The 1-D direction is not defined.  Calculate it
	    // based on which direction got the maximum locater motion.
	    if (    _ABS( localMotion[0]) > _ABS( localMotion[1]) 
		&& _ABS( localMotion[0]) > _ABS( localMotion[2]) )
		translateDir = 0;
	    else if (    _ABS( localMotion[1]) > _ABS( localMotion[2]) )
		translateDir = 1;
	    else 
		translateDir = 2;
	}

	// get the projection of 'localMotion' onto the preferred axis.
	SbVec3f constrainedMotion(0,0,0);
	constrainedMotion[translateDir] = localMotion[translateDir];
	localMotion = constrainedMotion;
    }

    // Append this to the startMotionMatrix, which we saved at the beginning
    // of the drag, to find the current motion matrix.
	setMotionMatrix(appendTranslation(getStartMotionMatrix(),localMotion));

    return;
}

////////////////////////////////////////////////////////////////////////
//
void
SoTabPlaneDragger::edgeScaleStart()
//
////////////////////////////////////////////////////////////////////////
{
    // This is the point we'll use if a metaKey callback makes us re-start.
	worldRestartPt = getWorldStartingPoint();

    // Setup scale center and direction for projector line.
    SbVec3f lineDir;
    switch (currentScalePatch) {
	case 0:
	    scaleCenter.setValue( 0, -1, 0 );
	    lineDir.setValue( 0, 1, 0);
	    break;
	case 1:
	    scaleCenter.setValue( 0, 1, 0 );
	    lineDir.setValue( 0, 1, 0);
	    break;
	case 2:
	    scaleCenter.setValue( -1, 0, 0 );
	    lineDir.setValue( 1, 0, 0);
	    break;
	case 3:
	    scaleCenter.setValue( 1, 0, 0 );
	    lineDir.setValue( 1, 0, 0);
	    break;
    }
    
    // Establish the projector line
    SbVec3f startLocalHitPt = getLocalStartingPoint();

    lineProj->setLine( SbLine(startLocalHitPt, startLocalHitPt + lineDir));
}

////////////////////////////////////////////////////////////////////////
void
SoTabPlaneDragger::edgeScaleDrag()
//
////////////////////////////////////////////////////////////////////////
{
    lineProj->setViewVolume( getViewVolume() );
    lineProj->setWorkingSpace( getLocalToWorldMatrix() );

    SbVec3f startHitPt = getLocalStartingPoint();

    SbVec3f newHitPt = lineProj->project( getNormalizedLocaterPosition());
    // If we need to start a new gesture, we'll carry on from this pt.
	getLocalToWorldMatrix().multVecMatrix( newHitPt, worldRestartPt);

    // Figure out 'delta', the amount to scale by.
    // To do this:
    // [1] the line we are really supposed to scale along passes through
    //     scaleCenter in the direction of our projection line.
    //     Drop a perpendicular from 'scaleCenter' to our projection line
    //     to find the origin to use in calculating change of scale.
    SbVec3f projectedScaleCenter 
		   = lineProj->getLine().getClosestPoint(scaleCenter);

    // [2] Find the distance from the old and new projected points to this
    //     center.
    SbVec3f oldDiff = startHitPt  - projectedScaleCenter;
    SbVec3f newDiff = newHitPt - projectedScaleCenter;

    float oldDist = oldDiff.length();
    float newDist = newDiff.length();

    if ( oldDiff.dot( newDiff ) < 0.0 ) 
	newDist *= -1.0;

    // [3] Change in scale is the ratio of newDist to oldDist
#define TINY 0.0001
    float delta = ( fabs(oldDist) < TINY || fabs(newDist) < TINY) 
		    ? 1.0 : newDist / oldDist;
#undef TINY

    // Constrain the scaling to be greater than getMinScale();
	if (delta < getMinScale() )
	    delta = getMinScale();

    // Get the scale we want to apply. It's 'delta' in the direction of our
    // projector line and 1 in the other directions
	SbVec3f newScaling(1,1,1);
	SbVec3f lineDir = lineProj->getLine().getDirection();
	for (int i = 0; i < 3; i++ )
	    if (lineDir[i] == 1.0)
		newScaling[i] = delta;

    // append this to the startMotionMatrix.
    setMotionMatrix(appendScale(getStartMotionMatrix(),newScaling,scaleCenter));
}


////////////////////////////////////////////////////////////////////////
//
void
SoTabPlaneDragger::cornerScaleStart()
//
////////////////////////////////////////////////////////////////////////
{
    // This is the point we'll use if a metaKey callback makes us re-start.
	worldRestartPt = getWorldStartingPoint();

    // Setup scale center
    switch (currentScalePatch) {
	case 0: scaleCenter.setValue( -1, -1, 0 ); break;
	case 1: scaleCenter.setValue( -1,  1, 0 ); break;
	case 2: scaleCenter.setValue( 1, 1, 0 ); break;
	case 3: scaleCenter.setValue( 1, -1, 0 ); break;
    }
    
    SbVec3f startLocalHitPt = getLocalStartingPoint();

    // 2-D scaling motion within plane.  Establish the projector plane
    SbVec3f planeNormal(0,0,1);
    planeProj->setPlane( SbPlane( planeNormal, startLocalHitPt ) );
}

////////////////////////////////////////////////////////////////////////
void
SoTabPlaneDragger::cornerScaleDrag()
//
////////////////////////////////////////////////////////////////////////
{
    planeProj->setViewVolume( getViewVolume() );
    planeProj->setWorkingSpace( getLocalToWorldMatrix() );

    SbVec3f startHitPt = getLocalStartingPoint();

    SbVec3f newHitPt = planeProj->project( getNormalizedLocaterPosition());
    // If we need to start a new gesture, we'll carry on from this pt.
	getLocalToWorldMatrix().multVecMatrix( newHitPt, worldRestartPt);

    // Figure out 'delta', the amount to scale by.
    // To do this:
    // [1] the plane we are really supposed to scale in passes through
    //     scaleCenter with normal of zAxis
    //     Drop perpendicular from 'scaleCenter' to projection plane
    //     to find the origin to use in calculating change of scale.
    SbVec3f projectedScaleCenter;
    SbLine  lineThroughCenter( scaleCenter, scaleCenter + SbVec3f(0,0,1) );
    planeProj->getPlane().intersect(lineThroughCenter,projectedScaleCenter);

    // [2] Now find the scale change in each of the x and y directions.
    //     Start by projecting each newHitPt's offset onto the two axes.
    SbVec3f oldDiff = startHitPt  - projectedScaleCenter;
    SbVec3f newDiff = newHitPt - projectedScaleCenter;

    // [3] Change in scale is the ratio of new to old
#define TINY 0.0001
    float xAxisDelta = (_ABS(newDiff[0]) < TINY || _ABS(oldDiff[0]) 
			< TINY) ? 1.0 : newDiff[0] / oldDiff[0];
    float yAxisDelta = (_ABS(newDiff[1]) < TINY || _ABS(oldDiff[1]) 
			< TINY) ? 1.0 : newDiff[1] / oldDiff[1];
#undef TINY

    // Make sure we didn't fall below minscale
    if ( xAxisDelta < getMinScale() )
         xAxisDelta = getMinScale();
    if ( yAxisDelta < getMinScale() )
         yAxisDelta = getMinScale();

    SbVec3f scaleDelta( xAxisDelta, yAxisDelta, 1 );


    setMotionMatrix( appendScale( getStartMotionMatrix(), 
				  scaleDelta, scaleCenter ) );
}

////////////////////////////////////////////////////////////////////////
//
void
SoTabPlaneDragger::scaleUniformStart()
//
////////////////////////////////////////////////////////////////////////
{
    // This is the point we'll use if a metaKey callback makes us re-start.
	worldRestartPt = getWorldStartingPoint();

    // We wish to scale in 2 dimensions about the corner opposite
    // the patch that we have hit.
    switch (currentScalePatch) {
	case 0: 
	    scaleCenter.setValue( -1, -1, 0);
	    break;
	case 1: 
	    scaleCenter.setValue( -1,  1, 0);
	    break;
	case 2: 
	    scaleCenter.setValue( 1, 1, 0); 
	    break;
	case 3: 
	    scaleCenter.setValue( 1, -1, 0); 
	    break;
    }

    // Find the projection line.
    // This line should lie within the same local z-plane 
    // as the startLocalHitPt.
    // It should pass through the startLocalHitPt.
    // It should also pass through the point where the scaleCenter
    // projects onto the same z-plane as the startLocalHitPt.
    SbVec3f startLocalHitPt = getLocalStartingPoint();
    SbVec3f zPlaneCenter( scaleCenter[0], scaleCenter[1], startLocalHitPt[2]); 
    lineProj->setLine( SbLine( startLocalHitPt, zPlaneCenter ) );
}

////////////////////////////////////////////////////////////////////////
void
SoTabPlaneDragger::scaleUniformDrag()
//
////////////////////////////////////////////////////////////////////////
{
    lineProj->setViewVolume( getViewVolume() );
    lineProj->setWorkingSpace( getLocalToWorldMatrix() );

    SbVec3f startHitPt = getLocalStartingPoint();

    SbVec3f newHitPt = lineProj->project( getNormalizedLocaterPosition());
    // If we need to start a new gesture, we'll carry on from this pt.
	getLocalToWorldMatrix().multVecMatrix( newHitPt, worldRestartPt);

    // [1] Find distance from old and new projected points to projectedCenter
    //     projectedCenter is zero-scale point on the lineProj.
    SbVec3f projectedCenter = lineProj->getLine().getClosestPoint(scaleCenter);
    SbVec3f oldDiff = startHitPt  - projectedCenter;
    SbVec3f newDiff = newHitPt - projectedCenter;

    float oldDist = oldDiff.length();
    float newDist = newDiff.length();

    // [2] Change in scale is ratio of newDist to oldDist
#define TINY 0.0001
    float delta =(oldDist < TINY || newDist < TINY) ? 1.0 : newDist/oldDist;
#undef TINY

    // [3] Check if sign of scale changed.
    if (oldDiff.dot(newDiff) < 0 )
	delta *= -1.0;

    if ( delta < getMinScale() )
         delta = getMinScale();

    SbVec3f scaleDelta( delta, delta, delta );

    setMotionMatrix( appendScale( getStartMotionMatrix(), 
				  scaleDelta, scaleCenter ));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//     In addition to usual rendering, this dragger
//     turns off the faces of the dragger that face away from the camera.
void 
SoTabPlaneDragger::GLRender( SoGLRenderAction *action )
//
////////////////////////////////////////////////////////////////////////
{
    if ( needScaleTabAdjustment == TRUE)
	reallyAdjustScaleTabSize(action);

    SoDragger::GLRender( action );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//     Adjusts the size of the scale tabs to be something reasonable in
//  screen space.
//
////////////////////////////////////////////////////////////////////////

#define SCREEN_TAB_SIZE 8

void 
SoTabPlaneDragger::adjustScaleTabSize()
{
    needScaleTabAdjustment = TRUE;
    // This forces a redraw, so any caches containing this will be broken
    touch();
}

void 
SoTabPlaneDragger::reallyAdjustScaleTabSize(SoGLRenderAction *action)
{
    // If the viewport size is still (1,1) or (0,0) then the 
    // Window we are in has not yet been mapped and realized.
    // So we need to just return and wait until next time.
    if ( action == NULL )
	return;
    SoState *state = action->getState();
    const SbVec2s &vpSize 
	= SoViewportRegionElement::get(state).getViewportSizePixels();
    if (vpSize[0] < 2 || vpSize[1] < 2)
	return;

    needScaleTabAdjustment = FALSE;

    setCameraInfo( action );
    setTempPathToThis( action->getCurPath() );

    SbMatrix localToWorld = getLocalToWorldMatrix();
    SbMatrix viewMat = SoViewingMatrixElement::get(action->getState());
    SbMatrix viewMatInv = viewMat.inverse();
    SbMatrix projMat = SoProjectionMatrixElement::get(action->getState());
    SbMatrix lclToScr = localToWorld * viewMat * projMat;

    // This will give the length of the x and y axes,
    // each axis going from -1 to +1.
    SbVec2f screenLengths;
    getXYScreenLengths(screenLengths, 
		       lclToScr, getViewportRegion().getViewportSizePixels());

    //
    // Here's how big the tabs will be, in local space. 
    // The 2.0 is because the box
    // is of width 2. 
    // Each dimension is considered separately, because they will almost
    // always be different sizes on screen.
    //
    SbVec2f localTabSize(2.0 * SCREEN_TAB_SIZE/screenLengths[0], 
    	                 2.0 * SCREEN_TAB_SIZE/screenLengths[1]);

    SbVec2f innerEdgeDist;
    innerEdgeDist[0] = 1.0 - localTabSize[0];
    innerEdgeDist[1] = 1.0 - localTabSize[1];

    SbVec2f outerEdgeDist(1.0, 1.0);

    SbVec2f halfTabWidth;
    halfTabWidth[0] = (localTabSize[0]/2.0);
    halfTabWidth[1] = (localTabSize[1]/2.0);

    // Be sure to keep vertices in counter-clockwise order. For each
    // group of four, they go in order:  3  2
    // when front-facing                 0  1

    // use a small z value to move points off of cube faces...
    const float z = .002;
	
    SoCoordinate3 *esc = (SoCoordinate3 *) edgeScaleCoords.getValue();
    if ( esc != NULL ) {

        // Make sure there is enough room in the coordinate node. 
	esc->point.setNum( 16 );
        SbVec3f *pts = esc->point.startEditing();
	    
	pts[0].setValue(   -halfTabWidth[0],  innerEdgeDist[1], z);
	pts[1].setValue(    halfTabWidth[0],  innerEdgeDist[1], z);
	pts[2].setValue(    halfTabWidth[0],  outerEdgeDist[1], z);
	pts[3].setValue(   -halfTabWidth[0],  outerEdgeDist[1], z);
	    
	pts[4].setValue(   -halfTabWidth[0], -outerEdgeDist[1], z);
	pts[5].setValue(    halfTabWidth[0], -outerEdgeDist[1], z);
	pts[6].setValue(    halfTabWidth[0], -innerEdgeDist[1], z);
	pts[7].setValue(   -halfTabWidth[0], -innerEdgeDist[1], z);
	    
	pts[8].setValue(   innerEdgeDist[0],  -halfTabWidth[1], z);
	pts[9].setValue(   outerEdgeDist[0],  -halfTabWidth[1], z);
	pts[10].setValue(  outerEdgeDist[0],   halfTabWidth[1], z);
	pts[11].setValue(  innerEdgeDist[0],   halfTabWidth[1], z);
	    
	pts[12].setValue( -outerEdgeDist[0],  -halfTabWidth[1], z);
	pts[13].setValue( -innerEdgeDist[0],  -halfTabWidth[1], z);
	pts[14].setValue( -innerEdgeDist[0],   halfTabWidth[1], z);
	pts[15].setValue( -outerEdgeDist[0],   halfTabWidth[1], z);

        esc->point.finishEditing();
    }
	
    SoCoordinate3 *csc = (SoCoordinate3 *) cornerScaleCoords.getValue();
    if ( csc != NULL ) {

        // Make sure there is enough room in the coordinate node. 
	csc->point.setNum( 16 );
        SbVec3f *pts = csc->point.startEditing();
	    
	pts[0].setValue(  innerEdgeDist[0],  innerEdgeDist[1], z);
	pts[1].setValue(  outerEdgeDist[0],  innerEdgeDist[1], z);
	pts[2].setValue(  outerEdgeDist[0],  outerEdgeDist[1], z);
	pts[3].setValue(  innerEdgeDist[0],  outerEdgeDist[1], z);
	    
	pts[4].setValue(  innerEdgeDist[0], -outerEdgeDist[1], z);
	pts[5].setValue(  outerEdgeDist[0], -outerEdgeDist[1], z);
	pts[6].setValue(  outerEdgeDist[0], -innerEdgeDist[1], z);
	pts[7].setValue(  innerEdgeDist[0], -innerEdgeDist[1], z);
	    
	pts[8].setValue( -outerEdgeDist[0], -outerEdgeDist[1], z);
	pts[9].setValue( -innerEdgeDist[0], -outerEdgeDist[1], z);
	pts[10].setValue(-innerEdgeDist[0], -innerEdgeDist[1], z);
	pts[11].setValue(-outerEdgeDist[0], -innerEdgeDist[1], z);
	    
	pts[12].setValue(-outerEdgeDist[0],  innerEdgeDist[1], z);
	pts[13].setValue(-innerEdgeDist[0],  innerEdgeDist[1], z);
	pts[14].setValue(-innerEdgeDist[0],  outerEdgeDist[1], z);
	pts[15].setValue(-outerEdgeDist[0],  outerEdgeDist[1], z);

        csc->point.finishEditing();
    }
}

////////////////////////////////////////////////////////////////////
//  Stubs for callbacks
////////////////////////////////////////////////////////////////////
void
SoTabPlaneDragger::startCB( void *, SoDragger *inDragger )
{
    SoTabPlaneDragger *dl = (SoTabPlaneDragger *) inDragger;
    dl->dragStart();
}

void
SoTabPlaneDragger::motionCB( void *, SoDragger *inDragger )
{
    SoTabPlaneDragger *dl = (SoTabPlaneDragger *) inDragger;
    dl->drag();
}

void
SoTabPlaneDragger::finishCB( void *, SoDragger *inDragger )
{
    SoTabPlaneDragger *dl = (SoTabPlaneDragger *) inDragger;
    dl->dragFinish();
}

//
// For each of the two primary axes, x and y, see how int32_t the segment is
// that goes from -1 to 1.
//
void
SoTabPlaneDragger::getXYScreenLengths(SbVec2f &lengths, 
		    const SbMatrix &localToScreen, const SbVec2s &winSize)
{
    // Set the values of the endpoints in local space.
    SbVec3f plusX(1,0,0), minusX(-1, 0,0);
    SbVec3f plusY(0,1,0), minusY( 0,-1,0);

    // Transform them into screen space.
    localToScreen.multVecMatrix(  plusX,  plusX );
    localToScreen.multVecMatrix( minusX, minusX );
    localToScreen.multVecMatrix(  plusY,  plusY );
    localToScreen.multVecMatrix( minusY, minusY );

    // These results are in -1 to 1 space.
    // Get them in 0 to 1 space.
     plusX[0] = 0.5 *  (plusX[0] + 1.0);
     plusX[1] = 0.5 *  (plusX[1] + 1.0);
    minusX[0] = 0.5 * (minusX[0] + 1.0);
    minusX[1] = 0.5 * (minusX[1] + 1.0);

     plusY[0] = 0.5 *  (plusY[0] + 1.0);
     plusY[1] = 0.5 *  (plusY[1] + 1.0);
    minusY[0] = 0.5 * (minusY[0] + 1.0);
    minusY[1] = 0.5 * (minusY[1] + 1.0);

    // Find square of the length of each transformed segment,
    // in pixels.
    SbVec2f xAxisPix;
    xAxisPix[0] = (plusX[0] - minusX[0]) * winSize[0];
    xAxisPix[1] = (plusX[1] - minusX[1]) * winSize[1];

    SbVec2f yAxisPix;
    yAxisPix[0] = (plusY[0] - minusY[0]) * winSize[0];
    yAxisPix[1] = (plusY[1] - minusY[1]) * winSize[1];

    lengths[0] = xAxisPix.length();
    lengths[1] = yAxisPix.length();
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
SoTabPlaneDragger::metaKeyChangeCB( void *, SoDragger *inDragger)
{
    SoTabPlaneDragger *tpd = (SoTabPlaneDragger *) inDragger;

    SoHandleEventAction *ha = tpd->getHandleEventAction();

    //[1] Only do this if we are grabbing events
    if ( ha->getGrabber() != tpd )
	return;

    //[2] We only want key press or release events.
    const SoEvent *event = tpd->getEvent();
    if ( !SO_KEY_PRESS_EVENT(event, ANY) && !SO_KEY_RELEASE_EVENT(event, ANY))
	return;

    //[3] Is the key 'shift'
    const SoKeyboardEvent *ke = (const SoKeyboardEvent *) event;
    SoKeyboardEvent::Key key = ke->getKey();

    if ( ( key == SoKeyboardEvent::LEFT_SHIFT ||
	   key == SoKeyboardEvent::RIGHT_SHIFT ) &&
	        ( tpd->currentState == TRANSLATING ||
	          tpd->currentState == UNIFORM_SCALING ||
	          tpd->currentState == CORNER_SCALING )) {

	// We want to end the old gesture and start a new one.

	// [A] Release the grabber. This ends the gesture and calls all 
	//     finishCallbacks (on parent dragger, too, if we're registered)
	    State savedState = tpd->currentState;
	    ha->releaseGrabber();

	// [B] Set the starting point to be our saved worldRestartPoint
	    tpd->setStartingPoint( tpd->worldRestartPt );
	    tpd->restartState = savedState;

	// [C] Become the grabber again. This begins a new gesture and calls all
	//     startCallbacks (parent dragger, too).  Info like viewVolume, 
	//     viewportRegion, handleEventAction, and tempPathToThis 
	//     is still valid.
	    ha->setGrabber(tpd);

	// [D] set handled
	    ha->setHandled();
    }
}

void
SoTabPlaneDragger::valueChangedCB( void *, SoDragger *inDragger )
{
    SoTabPlaneDragger *tpd = (SoTabPlaneDragger *) inDragger;
    SbMatrix motMat = tpd->getMotionMatrix();

    SbVec3f trans, scale;
    SbRotation rot, scaleOrient;
    getTransformFast( motMat, trans, rot, scale, scaleOrient);

    // Disconnect the field sensors
    tpd->translFieldSensor->detach();
    tpd->scaleFieldSensor->detach();

    if ( tpd->translation.getValue() != trans )
	tpd->translation = trans;
    if ( tpd->scaleFactor.getValue() != scale )
	tpd->scaleFactor = scale;

    // Reconnect the field sensors
    tpd->translFieldSensor->attach( &(tpd->translation) );
    tpd->scaleFieldSensor->attach( &(tpd->scaleFactor) );
}

void
SoTabPlaneDragger::fieldSensorCB( void *inDragger, SoSensor * )
{
    SoTabPlaneDragger *tpd = (SoTabPlaneDragger *) inDragger;

    // Incorporate the new field values into the matrix 
    SbMatrix motMat = tpd->getMotionMatrix();
    tpd->workFieldsIntoTransform(motMat);

    tpd->setMotionMatrix( motMat );
}


void
SoTabPlaneDragger::setDefaultOnNonWritingFields()
{
    // This node may change after construction, but we still
    // don't want to write it out.
    edgeScaleCoords.setDefault(TRUE);
    cornerScaleCoords.setDefault(TRUE);

    // Call the base class...
    SoDragger::setDefaultOnNonWritingFields();
}
