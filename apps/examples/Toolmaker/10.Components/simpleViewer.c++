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
 |   $Revision: 1.3 $
 |
 |   Classes	: simpleViewer
 |
 |   Author(s)	: Alain Dumesny
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <math.h>

#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/Xt/SoXtCursors.h>
#include "simpleViewer.h"


enum ViewerModes {
    IDLE_MODE, 
    TRANS_MODE, 
    SEEK_MODE, 
};


//
// Constructor for the viewer.
//
simpleViewer::simpleViewer(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent, 
    SoXtFullViewer::BuildFlag b, 
    SoXtViewer::Type t)
	: SoXtFullViewer(
	    parent,
	    name, 
	    buildInsideParent, 
	    b, 
	    t, 
	    TRUE) // tell base class to build (since we don't add anything)
{
    // init local vars
    mode = IDLE_MODE;
    createdCursors = FALSE;
    setSize(SbVec2s(520, 360)); //def size
    
    // assign decoration titles
    setPopupMenuString("Simple Viewer");
    setBottomWheelString("transX");
    setLeftWheelString("transY");
    setRightWheelString("Dolly");
    setPrefSheetString("Simple Viewer Preference Sheet");
    setTitle("Simple Viewer");
}

simpleViewer::~simpleViewer()
{
}

//
// Call the base class and set the correct cursor 
// on the window
//
void
simpleViewer::setViewing(SbBool flag)
{
    if (flag == viewingFlag || camera == NULL) {
	viewingFlag = flag;
	return;
    }
    
    // call the base class
    SoXtFullViewer::setViewing(flag);
    
    // set the right cursor
    Widget w = getRenderAreaWidget();
    if (w != (Widget) NULL && XtWindow(w) != (Window) NULL) {
	if ( isViewing() ) {
	    if (!createdCursors)
		defineCursors();
	    XDefineCursor(XtDisplay(w), XtWindow(w), vwrCursor);
	}
	else
	    XUndefineCursor(XtDisplay(w), XtWindow(w));
    }
}

//
// Process the given event to change the camera
//
void
simpleViewer::processEvent(XAnyEvent *xe)
{
    // let the base class handle the common set of events
    if ( processCommonEvents(xe) )
	return;
    
    // check if cursors need to be defined (they can only
    // be defined after the window has been mapped. 
    // Recieving events guarantees that the window has 
    // been mapped.
    if (!createdCursors) {
	defineCursors();
	Widget w = getRenderAreaWidget();
	XDefineCursor(XtDisplay(w), XtWindow(w), vwrCursor);
    }
    
    XButtonEvent    *be;
    XMotionEvent    *me;
    SbVec2s windowSize = getGlxSize();
    
    switch(xe->type) {
	case ButtonPress:
	    be = (XButtonEvent *)xe;
	    locator[0] = be->x;
	    locator[1] = windowSize[1] - be->y;
	    if (be->button == Button1) {
		switch (mode) {
		    case IDLE_MODE: 
			interactiveCountInc();
			switchMode(TRANS_MODE);
			break;
		    case SEEK_MODE:
			seekToPoint(locator);
			break;
		}
	    }
	    break;
	    
	case ButtonRelease:
	    be = (XButtonEvent *)xe;
	    if (be->button == Button1 && mode == TRANS_MODE) {
		switchMode(IDLE_MODE);
		interactiveCountDec();
	    }
	    break;
	    
	case MotionNotify:
	    me = (XMotionEvent *)xe;
	    locator[0] = me->x;
	    locator[1] = windowSize[1] - me->y;
	    if (mode == TRANS_MODE)
		translateCamera();
	    break;
    }
}

//
// Switches to the specified viewer mode. The correct
// cursor is also set on the window.
//
void
simpleViewer::switchMode(int newMode)
{
    // needed to define new cursors
    Widget w = getRenderAreaWidget();
    Display *display = XtDisplay(w);
    Window window = XtWindow(w);
    if (!createdCursors)
	defineCursors();
    
    // switch to new viewer mode
    mode = newMode;
    switch (mode) {
	case IDLE_MODE:
	    if (window != 0)
		XDefineCursor(display, window, vwrCursor);
	    break;
	    
	case TRANS_MODE:
	    {
		// Figure out the focal plane
		SbMatrix mx;
		mx = camera->orientation.getValue();
		SbVec3f forward(-mx[2][0], -mx[2][1], -mx[2][2]);
		SbVec3f fp = camera->position.getValue() + 
		    forward * camera->focalDistance.getValue();
		focalplane = SbPlane(forward, fp);
		
		// map mouse position onto the viewing plane
		SbVec2s windowSize = getGlxSize();
		SbLine line;
		SbViewVolume cameraVolume = camera->getViewVolume();
		cameraVolume.projectPointToLine(
		    SbVec2f( locator[0] / float(windowSize[0]), 
			locator[1] / float(windowSize[1])), line);
		focalplane.intersect(line, locator3D);
	    }
	    if (window != 0)
		XDefineCursor(display, window, vwrCursor);
	    break;
	
	case SEEK_MODE:
	    if (window != 0)
		XDefineCursor(display, window, seekCursor);
	    break;
    }
}

//
// Call the base class and sets the correct cursor 
// on the window.
//
void
simpleViewer::setSeekMode(SbBool flag)
{
    if ( !isViewing() )
	return;
    
    // call the base class
    SoXtFullViewer::setSeekMode(flag);
    
    // switches to the right mode
    switchMode(isSeekMode() ? SEEK_MODE : IDLE_MODE);
}

//
// Redefine this routine to customize the preference sheet
//
void
simpleViewer::createPrefSheet()
{
    // create the preference sheet shell and form widget
    Widget shell, form;
    createPrefSheetShellAndForm(shell, form);
    
    // create most of the default parts
    Widget widgetList[10];
    int num = 0;
    widgetList[num++] = createSeekPrefSheetGuts(form);
    widgetList[num++] = createZoomPrefSheetGuts(form);
    widgetList[num++] = createClippingPrefSheetGuts(form);
    
    layoutPartsAndMapPrefSheet(widgetList, num, form, shell);
}

// Bring the viewer help card (called by "?" push button)
void
simpleViewer::openViewerHelpCard()
{
    // tell the component to open the file for us
    openHelpCard("simpleViewer.help");
}

//
// Translate the camera right/left (called by thumb wheel).
//
void
simpleViewer::bottomWheelMotion(float newVal)
{
    if (camera == NULL)
	return;
    
    // get camera right vector and translate by wheel 
    // delta rotation
    SbMatrix mx;
    mx = camera->orientation.getValue();
    SbVec3f rightVector(mx[0][0], mx[0][1], mx[0][2]);
    float dist = transXspeed * (bottomWheelVal - newVal);
    camera->position = camera->position.getValue() + 
	dist * rightVector;
    
    bottomWheelVal = newVal;
}

//
// Translate the camera up/down (called by thumb wheel).
//
void
simpleViewer::leftWheelMotion(float newVal)
{
    if (camera == NULL)
	return;
    
    // get camera up vector and translate by wheel 
    // delta rotation
    SbMatrix mx;
    mx = camera->orientation.getValue();
    SbVec3f upVector(mx[1][0], mx[1][1], mx[1][2]);
    float dist = transYspeed * (leftWheelVal - newVal);
    camera->position = camera->position.getValue() + 
	dist * upVector;
    
    leftWheelVal = newVal;
}

//
// Moves the camera closer/further away from the plane 
// of interest, which is defined by the viewing normal 
// and the camera focalDistance field value.
//
void
simpleViewer::rightWheelMotion(float newVal)
{
    if (camera == NULL)
	return;
    
    // shorter/grow the focal distance given the wheel rotation
    float focalDistance = camera->focalDistance.getValue();
    float newFocalDist = focalDistance / 
			    pow(2.0, newVal - rightWheelVal);
    
    // finally reposition the camera
    SbMatrix mx;
    mx = camera->orientation.getValue();
    SbVec3f forward(-mx[2][0], -mx[2][1], -mx[2][2]);
    camera->position = camera->position.getValue() + 
    	    	       (focalDistance - newFocalDist) * forward;
    camera->focalDistance = newFocalDist;
    
    rightWheelVal = newVal;
}

//
// This routine is used to define cursors, which can 
// only be called after the window has been realized.
//
void
simpleViewer::defineCursors()
{
    XColor foreground;
    Pixmap source;
    Display *display = getDisplay();
    Drawable d = DefaultRootWindow(display);
    
    // set a red color
    foreground.red = 65535;
    foreground.green = foreground.blue = 0;
    
    // view plane translate cursor
    source = XCreateBitmapFromData(display, d, 
	so_xt_flat_hand_bits, so_xt_flat_hand_width, so_xt_flat_hand_height);
    vwrCursor = XCreatePixmapCursor(display, source, source, 
	&foreground, &foreground, so_xt_flat_hand_x_hot, 
	so_xt_flat_hand_y_hot);
    XFreePixmap(display, source);
    
    // seek cursor
    source = XCreateBitmapFromData(display, d, 
	so_xt_target_bits, so_xt_target_width, so_xt_target_height);
    seekCursor = XCreatePixmapCursor(display, source, source, 
	&foreground, &foreground, so_xt_target_x_hot, so_xt_target_y_hot);
    XFreePixmap(display, source);
    
    createdCursors = TRUE;
}

//
// Moves the camera into the plane defined by the camera 
// forward vector and the focal point (using the camera
// focalDistance field) to follow the new mouse location.
//
void
simpleViewer::translateCamera()
{
    if (camera == NULL)
	return;
    
    SbVec2s windowSize = getGlxSize();
    SbVec2f newLocator( locator[0] / float(windowSize[0]), 
	locator[1] / float(windowSize[1]) );
    
    // map new mouse location into the camera focal plane
    SbLine	    line;
    SbVec3f	    newLocator3D;
    SbViewVolume cameraVolume = camera->getViewVolume();
    cameraVolume.projectPointToLine(newLocator, line);
    focalplane.intersect(line, newLocator3D);
    
    // move the camera by the delta 3D position amount
    camera->position = camera->position.getValue() + 
	(locator3D - newLocator3D);
    
    // You would think we would have to set locator3D to
    // newLocator3D here.  But we don't, because moving 
    // the camera essentially makes locator3D equal to 
    // newLocator3D in the transformed space, and we will 
    // project the next newLocator3D in this transformed space.
}

// Called by the bottom and left thumbwheels to compute 
// the translation factors (how fast should we translate 
// given a wheel rotation).
void
simpleViewer::computeTranslateValues()
{
    if (camera == NULL)
	return;
    
    float height;
    
    if ( camera->isOfType( 
	SoPerspectiveCamera::getClassTypeId()) ) {
	float angle = ((SoPerspectiveCamera *) 
	    camera)->heightAngle.getValue() / 2;
	float dist = camera->focalDistance.getValue();
	height = dist * tanf(angle);
    }
    else if ( camera->isOfType( 
	SoOrthographicCamera::getClassTypeId()) )
	height = ((SoOrthographicCamera *) 
	    camera)->height.getValue() / 2;
    
    // given the size of the viewing plane, figure out 
    // the up/down and right/left speeds for the thumb wheels.
    transYspeed = height / 2;
    transXspeed = transYspeed * camera->aspectRatio.getValue();
}

// Thumb wheels start callbacks
void
simpleViewer::bottomWheelStart()
{
    computeTranslateValues();
    
    // call parent class
    SoXtFullViewer::bottomWheelStart();
}

void
simpleViewer::leftWheelStart()
{
    computeTranslateValues();
    
    // call parent class
    SoXtFullViewer::leftWheelStart();
}
