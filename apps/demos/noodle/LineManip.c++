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

//
// A very specialized node; this manipulator completely takes care of
// editing a set of line segments.  This is somewhat different from
// other manipulators, because you insert it in the scene graph and it
// edits itself (instead of pointing it at the thing you want it to
// edit).
//

#include <stdio.h>
#include <assert.h>

#include <Inventor/SbColor.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/details/SoDetail.h>
#include <Inventor/details/SoLineDetail.h>
#include <Inventor/details/SoPointDetail.h>
#include <Inventor/elements/SoViewVolumeElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/events/SoEvent.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/events/SoLocation2Event.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/projectors/SbLineProjector.h>
#include <Inventor/projectors/SbPlaneProjector.h>
#include <Inventor/sensors/SoFieldSensor.h>

#include "LineManip.h"

SO_NODE_SOURCE(LineManip2);

//
// Constructor; sets up scene graph
//
LineManip2::LineManip2()
{
    SO_NODE_CONSTRUCTOR(LineManip2);

    coord = new SoCoordinate3();
    addChild(coord);
    coord->point.deleteValues(0);

    pset = new SoPointSet;
    pset->numPoints.setValue(0);
    addChild(pset);

    drawStyle = new SoDrawStyle;
    drawStyle->style = SoDrawStyle::LINES;
    addChild(drawStyle);

    lset = new SoLineSet;
    lset->numVertices.setValue(0);
    addChild(lset);

    current_coord = -1;
    planeProj = new SbPlaneProjector(FALSE);
    planeProj->setPlane( SbPlane( SbVec3f(0,0,1), 0 ) );
    lineProj = new SbLineProjector();
    lineProj->setLine( SbLine(  SbVec3f(0,0,0),SbVec3f(0,0,1) ) );

    hilightSize = 0.05;

    initHilightStuff();

    myHandleEventAction = NULL;

    // This sensor will insure that the pset and lset nodes will have
    // the correct number of points.
    coordFieldSensor = new SoFieldSensor(&LineManip2::coordFieldSensorCB, this);
    coordFieldSensor->attach( &coord->point );
    coordFieldSensor->setPriority(0);
}

//
// Destructor.  This will be called when this is unref'ed.
LineManip2::~LineManip2()
{
    //
    // Don't need to delete children; they will have already been
    // unref'ed and deleted (since this is derived from SoGroup).
    //

    delete planeProj;
    delete lineProj;
    delete coordFieldSensor;
}

//
// Magic stuff to register this node with the database
//
void
LineManip2::initClass()
{
    SO_NODE_INIT_CLASS(LineManip2, SoSeparator, "Separator");
}

//
// This tiny routine is how this Node gets used.  After creating a
// LineManip2, call this routine and put a data sensor on the Coordinate3
// returned.
//
SoCoordinate3 *
LineManip2::getCoordinate3()
{
    return coord;
}

//
// This tiny routine is how this Node gets used.  After creating a
// LineManip2, call this routine and put a data sensor on the Coordinate3
// returned.
//
void
LineManip2::setCoordinate3( SoCoordinate3 *newNode )
{
    if ( newNode == coord)
	return;

    if ( coord != NULL ) {
	removeChild( coord );
        coordFieldSensor->detach();
    }
    if ( newNode != NULL ) {
	insertChild(newNode,0);
        coordFieldSensor->attach( &newNode->point );
    }
    coord = newNode;

    coordFieldSensorCB( this, NULL);
}

//
// For setting the plane that the manip works in. You pass it a normal.
//
void
LineManip2::setPlaneNormal( const SbVec3f &newNormal )
{
    SbVec3f zeroVec(0,0,0);
    planeNormal = newNormal;
    planeProj->setPlane( SbPlane( planeNormal, zeroVec ) );
    lineProj->setLine( SbLine( zeroVec, zeroVec + planeNormal ) );
    hilightTransform->rotation.setValue( SbRotation(SbVec3f(0,0,1), SbVec3f(planeNormal)));
}

//
// Note:
// This node eats events.  It uses the entire surface of the render
// area as its playground, sucking up mouse motion, left mouse and
// backspace/delete events, even if the mouse isn't on top of it.
//
void
LineManip2::handleEvent(SoHandleEventAction *ha)
{
    myHandleEventAction = ha;

    const SoEvent *e = ha->getEvent();

    if (SO_MOUSE_PRESS_EVENT(e, BUTTON1)) {
	if (dragStart())
	    ha->setHandled();
    }
    else if (e->isOfType(SoLocation2Event::getClassTypeId())) {
	// If not dragging, locate hilight
	if (ha->getGrabber() != this) {
	    if (locateHilight())
	        ha->setHandled();
	}
	else {	// Are dragging, move coordinate:
	    moveCoord();
	    ha->setHandled();
	}
    }
    else if (SO_MOUSE_RELEASE_EVENT(e, BUTTON1) &&
	     ha->getGrabber() == this) {
	ha->releaseGrabber();
	ha->setHandled();
    }
    else if (SO_KEY_PRESS_EVENT(e, BACKSPACE) ||
	     SO_KEY_PRESS_EVENT(e, DELETE)) {
	if (remove())
	    ha->setHandled();
	// If we are in mid-motion and we remove the point, 
	// we better stop grabber, cause we got nothin to grab anymore.
	if (ha->getGrabber() == this)
	    ha->releaseGrabber();
        current_coord = -1;
    }
}

static SbColor locateColor(0.5, 0.5, 0.5);
static SbColor pickColor(0.9, 0.9, 0.9);

//
// Figures out which part of the line manipulator is picked by the
// given detail.  This will also set current_position and
// current_coord. 
//
#ifdef __C_PLUS_PLUS_2
LineManipPart
#else
LineManip2::Part
#endif
LineManip2::whichPart()
{
    // Can't pick if we don't have an action, now can we?
    if (myHandleEventAction == NULL)
	return NOTHING;

    // We're also out of luck if there's no coord node.
    if (coord == NULL)
	return NOTHING;

    const SoPickedPoint *picked_point = myHandleEventAction->getPickedPoint();

    projectMouse(current_position);

    int numCoords = coord->point.getNum();

    if (numCoords == 0 || picked_point == NULL) {
	//
	// Endpoints
	// Pick the endPoint that's closer to where the mouse is now.
	if (numCoords == 0) {
	    current_coord = -1;
	}
	else {
	    SbVec3f v1, v2;
	    v1 = current_position - coord->point[0];
	    v2 = current_position - coord->point[numCoords-1];
	    //
	    // Choose the endPoint closer to the current position.
	    //
	    if (v1.dot(v1) < v2.dot(v2)) {
		current_coord = 0;
	    }
	    else {
		current_coord = numCoords;
	    }
	}
	return ENDPOINTS;
    }

    const SoPath *dpath = picked_point->getPath();
    const SoDetail *detail = picked_point->getDetail();

    if (dpath->containsNode(pset)) {
	current_coord =
	    ((SoPointDetail *)detail)->getCoordinateIndex();
	return POINTS;
    }
    if (dpath->containsNode(lset)) {
	current_coord =
	    ((SoLineDetail *)detail)->getPartIndex();
	return LINES;
    }
    return NOTHING;
}

//
// Called when the mouse goes down to figure out what is being picked.
//
SbBool
LineManip2::dragStart()
{
    if ( coord == NULL)
	return FALSE;

    SbBool gotSomething = TRUE;

    switch(whichPart()) {
      case POINTS:
	myHandleEventAction->setGrabber(this);
	hilightVertex(coord->point[current_coord], pickColor);
	break;

      case LINES:
	// Create a new vertex
	coord->point.insertSpace(current_coord+1, 1);
	coord->point.set1Value(current_coord+1, current_position);
	myHandleEventAction->setGrabber(this);
	++current_coord;
	hilightVertex(coord->point[current_coord], pickColor);
	break;

      case ENDPOINTS:
	// Create a new vertex
	if (current_coord != -1) {
	    coord->point.insertSpace(current_coord, 1);
	}
	else current_coord = 0;
	coord->point.set1Value(current_coord, current_position);
	myHandleEventAction->setGrabber(this);
	hilightVertex(coord->point[current_coord], pickColor);
	break;

      case NOTHING:
	current_coord = -1;
	removeHilights();
	gotSomething = FALSE;
	break;
    }

    return gotSomething;
}

//
// Called by SoHandleEventAction when we grab events 
// using myHandleEventAction->setGrabber( this );
//
// We will record the viewport and viewvolume.
// This is because once we've grabbed, we can't rely on the new handleEvent
// action to have correct view information.
// Why? Because grabbing bypasses traversal and sends the event directly
// to this node. The camera node has no chance to modify the state.
//

void
LineManip2::grabEventsSetup()
{
    extractViewingParams(myHandleEventAction);
    savedRenderCachingVal 
	= (SoSeparator::CacheEnabled) renderCaching.getValue();
    renderCaching = OFF;
}


void
LineManip2::grabEventsCleanup()
{
    removeHilights();
    current_coord = -1;

    renderCaching = savedRenderCachingVal;
}

void 
LineManip2::extractViewingParams( SoHandleEventAction *ha )
{
    if (ha == NULL) {
        // If the action is NULL, use default values for view info.
	myViewVolume.ortho(-1,1,-1,1,1,10);
	myVpRegion = SbViewportRegion(1,1);
    }
    else {
	// Get the view info from the action.
	SoState *state = ha->getState();
	myViewVolume = SoViewVolumeElement::get( state );
	myVpRegion = SoViewportRegionElement::get( state );
    }
}

//
// Called when the backspace or delete keys is hit.  Note that if the
// mouse is close to the middle of a line segment (so a pick would
// insert a point) nothing is done.  This used to delete the entire
// line segment, but that was not intuitive.  That might be a good
// interface for creating disconnected line segments, though...
//
SbBool
LineManip2::remove()
{
    if (coord == NULL)
	return FALSE;

    SbBool removedSomething = TRUE;

    if ( myHandleEventAction->getGrabber() == this ) {
	// Remove whatever we're working on.
	if (current_coord != -1)
	    coord->point.deleteValues(current_coord, 1);
    }
    else {
	// Remove whatever the cursor indicates should be gone.
	switch (whichPart()) {
	  case POINTS:
	    coord->point.deleteValues(current_coord, 1);
	    break;

	  case LINES:
	    // Do nothing.
	    break;

	  case ENDPOINTS:
	    if (current_coord != -1) {
		if (current_coord != coord->point.getNum())
		    coord->point.deleteValues(current_coord, 1);
		else 
		    coord->point.deleteValues(current_coord-1, 1);
	    }
	    break;

	  case NOTHING:
	    removedSomething = FALSE;
	    break;
	}
    }

    removeHilights();

    return removedSomething;
}

//
// Called when the mouse is moving around, but the left mouse button
// is up.
//
SbBool
LineManip2::locateHilight()
{
    if (coord == NULL )
	return FALSE;

    SbBool hilitSomething = TRUE;

    switch (whichPart()) {
      case POINTS:
	hilightVertex(coord->point[current_coord], locateColor);
	break;

      case LINES:
	hilightLine(current_coord, current_position, locateColor);
	break;

      case ENDPOINTS:
	if (current_coord == -1) {
	    // There are no coordinates at all. We can't perform a
	    // hilight.
	    hilitSomething = FALSE;
	}
	else if (current_coord == 0)
	    hilightLine(-1, current_position, locateColor);
	else
	    hilightLine(current_coord-1, current_position, locateColor);
	break;

      case NOTHING:
        hilitSomething = FALSE;
	break;
    }

    return hilitSomething;
}

//
// Called to move the selected point when the mouse is moved and the
// left mouse button is pressed.
//
void
LineManip2::moveCoord()
{
    if (coord == NULL)
	return;

    assert(current_coord > -1 && current_coord < coord->point.getNum());
    updateProjectors( coord->point[current_coord] );
    SbVec3f t; 
    projectMouse(t);
    coord->point.set1Value(current_coord, t);

    hilightVertex(coord->point[current_coord], pickColor);
}

void
LineManip2::updateProjectors( const SbVec3f &curPt )
{
    SbVec3f  dir = lineProj->getLine().getDirection();
    lineProj->setLine( SbLine( curPt, curPt + dir ) );

    dir = planeProj->getPlane().getNormal();
    planeProj->setPlane( SbPlane( dir, curPt ) );
}

//
// Does the grunt work of projecting the mouse onto the plane
// containing the line manipulator.
//
void
LineManip2::projectMouse(SbVec3f &result)
{
    const SoEvent *e = myHandleEventAction->getEvent();

    // If we're grabbing, we've already got our view parameters.
    // If not, then we'd better get them now.
    // NOTE: If you're grabbing and you try to get view parameters, you'll
    //       get a bad answer. Grabbing causes you to skip over camera
    //       nodes directly to this one. So the state has no valid view info.
    if ( myHandleEventAction->getGrabber() != this )
	extractViewingParams(myHandleEventAction);

    // If alt key is down, then we move in line perpendicular to plane
    if ( e->wasAltDown() == FALSE ) {
	planeProj->setViewVolume(myViewVolume);
	result = planeProj->project(e->getNormalizedPosition(myVpRegion));
    }
    else {
	lineProj->setViewVolume(myViewVolume);
	result = lineProj->project(e->getNormalizedPosition(myVpRegion));
    }
}

// This sensor will insure that the pset and lset nodes will have
// the correct number of points.
void    
LineManip2::coordFieldSensorCB( void *myPtr, SoSensor *)
{
    LineManip2 *myself = (LineManip2 *)myPtr;

    int numPoints = 0;
    if (myself->coord)
	numPoints = myself->coord->point.getNum();

    if (myself->pset)
	myself->pset->numPoints.setValue(numPoints);
    if (myself->lset)
	myself->lset->numVertices.setValue(numPoints);
}
