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
 |   Classes	: SoXtWalkViewer
 |
 |   Author(s)	: Alain Dumesny
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */
#include <inttypes.h>
#include <math.h>

#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

#include <Xm/LabelG.h>
#include <Xm/Form.h>
#include <Xm/PushBG.h>
#include <Sgm/ThumbWheel.h>

#include <Inventor/SoDB.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include <Inventor/fields/SoSFTime.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/Xt/SoXtCursors.h>
#include <Inventor/Xt/viewers/SoXtWalkViewer.h>
#include <Inventor/Xt/SoXtResource.h>
#include <GL/gl.h>


/*
 *  Defines
 */
enum ViewerModes {
    PICK_MODE, 
    VIEW_MODE, 
    WALK_MODE_ACTIVE, 
    PAN_MODE, 
    PAN_MODE_ACTIVE, 
    TILT_MODE_ACTIVE, 
    SEEK_MODE, 
    SET_UP_MODE
};

#define REC_SIZE	12	// size of the square region on the screen
#define CROSS		12	// cross feedback size
#define TURN_SPEED	5.0
#define WALK_SPEED_FACTOR 6.0
#define WHEEL_DOLLY_FACTOR  0.5
#define WHEEL_HEIGHT_FACTOR 0.01


// Resources for labels.
typedef struct {
	char *walkViewer;
	char *wvPrefSheet;
} RES_LABELS;
static RES_LABELS rl;
static char *defaultLabel[]={ 
	"Walk Viewer",  
	"Walk Viewer Preference Sheet", 
};


////////////////////////////////////////////////////////////////////////
//
// Public constructor - build the widget right now
//
SoXtWalkViewer::SoXtWalkViewer(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent, 
    SoXtFullViewer::BuildFlag b, 
    SoXtViewer::Type t)
	: SoXtConstrainedViewer(
	    parent,
	    name, 
	    buildInsideParent, 
	    b, 
	    t, 
	    FALSE) // tell base class not to build just yet  
//
////////////////////////////////////////////////////////////////////////
{
    // In this case, render area is what the app wants, so buildNow = TRUE
    constructorCommon(TRUE);
}

////////////////////////////////////////////////////////////////////////
//
// SoEXTENDER constructor - the subclass tells us whether to build or not
//
SoXtWalkViewer::SoXtWalkViewer(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent, 
    SoXtFullViewer::BuildFlag b, 
    SoXtViewer::Type t, 
    SbBool buildNow)
	: SoXtConstrainedViewer(
	    parent,
	    name, 
	    buildInsideParent, 
	    b, 
	    t, 
	    FALSE) // tell base class not to build just yet  
//
////////////////////////////////////////////////////////////////////////
{
    // In this case, render area may be what the app wants, 
    // or it may want a subclass of render area. Pass along buildNow
    // as it was passed to us.
    constructorCommon(buildNow);
}

////////////////////////////////////////////////////////////////////////
//
// Called by the constructors
//
// private
//
void
SoXtWalkViewer::constructorCommon(SbBool buildNow)
//
////////////////////////////////////////////////////////////////////////
{
    // init local vars
    mode = isViewing() ? VIEW_MODE : PICK_MODE;
    createdCursors = FALSE;
    walkCursor = panCursor = tiltCursor = seekCursor = upCursor = 0;
    setClassName("SoXtWalkViewer");
      
    // init animation variables
    animatingFlag = FALSE;
    animationSensor = new SoFieldSensor(SoXtWalkViewer::animationSensorCB, this);
    
    
    addFinishCallback(SoXtWalkViewer::setFocalPointFinishCallback, this);
    
    // Build the widget tree, and let SoXtComponent know about our base widget.
    if (buildNow) {

	// get resources...

	SoXtResource xr(getParentWidget());
	if (!xr.getResource( "walkViewer",               "WalkViewer",               rl.walkViewer ))
	    rl.walkViewer    = defaultLabel[0];
	if (!xr.getResource( "walkViewerPreferenceSheet","WalkViewerPreferenceSheet",rl.wvPrefSheet ))
	    rl.wvPrefSheet   = defaultLabel[1];

        // assign decoration titles
        setPopupMenuString( rl.walkViewer );
        setPrefSheetString( rl.wvPrefSheet );

	Widget w = buildWidget(getParentWidget());
	setBaseWidget(w);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: public

SoXtWalkViewer::~SoXtWalkViewer()
//
////////////////////////////////////////////////////////////////////////
{
    delete animationSensor;
    
    // free the viewer cursors
    if (getDisplay()) {
	Display *display = getDisplay();
	if (walkCursor) XFreeCursor(display, walkCursor);
	if (panCursor) XFreeCursor(display, panCursor);
	if (tiltCursor) XFreeCursor(display, tiltCursor);
	if (seekCursor) XFreeCursor(display, seekCursor);
	if (upCursor) XFreeCursor(display, upCursor);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Call the base class and sets the correct cursors on the window
//
// Use: virtual public
void
SoXtWalkViewer::setViewing(SbBool flag)
//
////////////////////////////////////////////////////////////////////////
{
    if (flag == viewingFlag)
	return;
    
    // call the base class
    SoXtConstrainedViewer::setViewing(flag);
    
    switchMode(isViewing() ? VIEW_MODE : PICK_MODE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Enables/Disable the viewer cursor on the window.
//
// Use: virtual public
void
SoXtWalkViewer::setCursorEnabled(SbBool flag)
//
////////////////////////////////////////////////////////////////////////
{
    if (flag == cursorEnabledFlag)
	return;
    
    cursorEnabledFlag = flag;
    
    if (! isViewing())
	return;
    
    Display *display = getDisplay();
    Widget w = getRenderAreaWidget();
    Window window = (w != NULL) ? XtWindow(w) : 0;
    if (! window)
	return;
    
    updateCursor();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	redefines this to force the camera to be perspective (since
//  orthographic camera don't any sense in this viewer).
//
// Use: virtual public
void
SoXtWalkViewer::setCamera(SoCamera *newCamera)
//
////////////////////////////////////////////////////////////////////////
{
    if (camera == newCamera)
	return;
    
    // call parent class
    SoXtConstrainedViewer::setCamera(newCamera);
    
    // now make sure the camera is not orthographic, else switch to
    // perspective.
    if (camera != NULL && 
	camera->isOfType(SoOrthographicCamera::getClassTypeId()))
	toggleCameraType();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	This is redefined to prevent the camera type from being changed.
//
// Use: virtual public
void
#ifdef DEBUG
SoXtWalkViewer::setCameraType(SoType type)
#else
SoXtWalkViewer::setCameraType(SoType)
#endif
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (! type.isDerivedFrom(SoPerspectiveCamera::getClassTypeId()))
	SoDebugError::post("SoXtWalkViewer::setCameraType()",
			"ignored - must be perspective camera");
#endif
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Call the base class and switches to the correct viewer state.
//
// Use: virtual public
void
SoXtWalkViewer::setSeekMode(SbBool flag)
//
////////////////////////////////////////////////////////////////////////
{
    if (!isViewing())
	return;
    
    // call the base class
    SoXtConstrainedViewer::setSeekMode(flag);
    
    switchMode(isSeekMode() ? SEEK_MODE : VIEW_MODE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Brings the viewer help card (called by "?" push button)
//
// Use: virtual protected
void
SoXtWalkViewer::openViewerHelpCard()
//
////////////////////////////////////////////////////////////////////////
{
    // tell the base class to open the file for us
    openHelpCard("SoXtWalkViewer.help");
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Process the given event to do viewing stuff
//
// Use: virtual protected
void
SoXtWalkViewer::processEvent(XAnyEvent *xe)
//
////////////////////////////////////////////////////////////////////////
{
    if ( processCommonEvents(xe) )
	return;
    
    if (!createdCursors)
	updateCursor();
    
    XButtonEvent    *be;
    XMotionEvent    *me;
    XKeyEvent	    *ke;
    XCrossingEvent  *ce;
    KeySym	    keysym;
    
    SbVec2s raSize = getGlxSize();
    
    switch(xe->type) {
    case ButtonPress:
    case ButtonRelease:
	be = (XButtonEvent *)xe;
	if (be->button != Button1 && be->button != Button2)
	    break;
	
	locator[0] = be->x;
	locator[1] = raSize[1] - be->y;
	
	if (mode == SEEK_MODE) {
	    if (xe->type == ButtonPress)
		seekToPoint(locator);
	}
	else if (mode == SET_UP_MODE) {
	    if (xe->type == ButtonPress) {
		findUpDirection(locator);
		switchMode(VIEW_MODE);
	    }
	}
	else
	    updateViewerMode(be->state);
	break;
	
    case KeyPress:
    case KeyRelease:
	ke = (XKeyEvent *)xe;
	keysym = XLookupKeysym(ke, 0);
	
	locator[0] = ke->x;
	locator[1] = raSize[1] - ke->y;
	if (keysym == XK_Control_L || keysym == XK_Control_R)
	    updateViewerMode(ke->state);
	else if (keysym == XK_u && xe->type == KeyPress) {
	    if (isSeekMode())
		setSeekMode(FALSE);
	    switchMode( (mode == SET_UP_MODE) ? VIEW_MODE : SET_UP_MODE );
	}
	break;
	
    case MotionNotify:
	me = (XMotionEvent *)xe;
	locator[0] = me->x;
	locator[1] = raSize[1] - me->y;
	if (mode == TILT_MODE_ACTIVE)
	    rotateCamera();
	break;
	
    case LeaveNotify:
    case EnterNotify:
	ce = (XCrossingEvent *)xe;
	//
	// because the application might use Ctrl-key for motif menu
	// accelerators we might not receive a key-up event, so make sure
	// to reset any Ctrl mode if we loose focus, but don't do anything
	// if Ctrl-key is not down (nothing to do) or if a mouse button 
	// is down (we will get another leaveNotify).
	//
	if (! (ce->state & ControlMask))
	    break;
	if (ce->state & Button1Mask || ce->state & Button2Mask)
	    break;
	if (xe->type == LeaveNotify)
	    switchMode(VIEW_MODE);
	else
	    updateViewerMode(ce->state);
	break;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    sets the viewer mode based on what keys and buttons are being pressed
//
// Use: private
void
SoXtWalkViewer::updateViewerMode(unsigned int state)
//
////////////////////////////////////////////////////////////////////////
{
    // ??? WARNING - this routine ONLY works because of 
    // ??? SoXtViewer::updateEventState() which is called for us
    // ??? by SoXtViewer::processCommonEvents(). 
    // ??? (XEvent state for button and modifier keys is not updated
    // ??? until after the event is received. WEIRD)
    
    // LEFT+MIDDLE down
    if (state & Button1Mask && state & Button2Mask) {
	switchMode(TILT_MODE_ACTIVE);
    }
    
    // LEFT down
    else if (state & Button1Mask) {
	if (state & ControlMask)
	    switchMode(PAN_MODE_ACTIVE);
	else
	    switchMode(WALK_MODE_ACTIVE);
    }
    
    // MIDDLE DOWN
    else if (state & Button2Mask) {
	if (state & ControlMask)
	    switchMode(TILT_MODE_ACTIVE);
	else
	    switchMode(PAN_MODE_ACTIVE);
    }
    
    // no buttons down...
    else {
	if (state & ControlMask)
	    switchMode(PAN_MODE);
	else
	    switchMode(VIEW_MODE);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    switches to the specified viewer mode
//
// Use: private
void
SoXtWalkViewer::switchMode(int newMode)
//
////////////////////////////////////////////////////////////////////////
{
    // assign new mode
    SbBool redrawNeeded = FALSE;
    int prevMode = mode;
    mode = newMode;
    
    // update the cursor
    updateCursor();
    
    // check the old viewer mode
    switch (prevMode) {
	case WALK_MODE_ACTIVE:
	case PAN_MODE_ACTIVE:
	    stopAnimating();
	    redrawNeeded = TRUE;
	    break;
	case TILT_MODE_ACTIVE:
	    redrawNeeded = TRUE;
	    interactiveCountDec();
	    break;
    }
    
    // switch to new viewer mode
    switch (newMode) {
	case PICK_MODE:
	    stopAnimating();
	    break;
	    
	case WALK_MODE_ACTIVE:
	case PAN_MODE_ACTIVE:
	    startPos = locator;
	    startAnimating();
	    redrawNeeded = TRUE;
	    break;
	    
	case TILT_MODE_ACTIVE:
	    startPos = prevPos = locator;
	    redrawNeeded = TRUE;
	    interactiveCountInc();
	    break;
    }
    
    if (redrawNeeded)
	scheduleRedraw();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    updates the viewer cursor
//
// Use: private
void
SoXtWalkViewer::updateCursor()
//
////////////////////////////////////////////////////////////////////////
{
    Widget w = getRenderAreaWidget();
    Display *display = w ? XtDisplay(w) : NULL;
    Window window = w ? XtWindow(w) : 0;
    
    if (! window)
	return;
    
    if (! createdCursors)
	defineCursors();
    
    // the viewer cursor are not enabled, then we don't set a new cursor.
    // Instead erase the old viewer cursor.
    if (! cursorEnabledFlag) {
	XUndefineCursor(display, window);
	return;
    }
    
    // ...else set the right cursor for the viewer mode....
    switch(mode) {
	case PICK_MODE:
	    XUndefineCursor(display, window);
	    break;
	    
	case VIEW_MODE:
	case WALK_MODE_ACTIVE:
	    XDefineCursor(display, window, walkCursor);
	    break;
	    
	case PAN_MODE:
	case PAN_MODE_ACTIVE:
	    XDefineCursor(display, window, panCursor);
	    break;
	    
	case TILT_MODE_ACTIVE:
	    XDefineCursor(display, window, tiltCursor);
	    break;
	    
	case SEEK_MODE:
	    XDefineCursor(display, window, seekCursor);
	    break;
	    
	case SET_UP_MODE:
	    XDefineCursor(display, window, upCursor);
	    break;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Starts ongoing animation
//
// Use: private
void
SoXtWalkViewer::startAnimating()
//
////////////////////////////////////////////////////////////////////////
{
    if (! animatingFlag) {
	animationSensor->attach(viewerRealTime);
	animationSensor->schedule();
	prevAnimTime = viewerRealTime->getValue();
	interactiveCountInc();
	animatingFlag = TRUE;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Stops ongoing animation (if any)
//
// Use: private
void
SoXtWalkViewer::stopAnimating()
//
////////////////////////////////////////////////////////////////////////
{
    if (animatingFlag) {
	animatingFlag = FALSE;
	animationSensor->detach();
	animationSensor->unschedule();
	interactiveCountDec();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    draws viewer feedback during a render area redraw of the scene.
//
// Use: virtual protected
void
SoXtWalkViewer::actualRedraw()
//
////////////////////////////////////////////////////////////////////////
{
    // have the base class draw the scene
    SoXtConstrainedViewer::actualRedraw();
    
    // now draw the viewer feedback
    if (isViewing() && (mode == WALK_MODE_ACTIVE || mode == PAN_MODE_ACTIVE 
			|| mode == TILT_MODE_ACTIVE)) {
	
	setFeedbackOrthoProjection(getGlxSize());
	
	// draw simple starting point feedback
	glLineWidth(1);
	glColor3ub(215,215,215);
	if (mode == WALK_MODE_ACTIVE || mode == PAN_MODE_ACTIVE) {
	    glBegin(GL_LINE_LOOP);
	    glVertex2s(startPos[0] - REC_SIZE, startPos[1] - REC_SIZE);
	    glVertex2s(startPos[0] - REC_SIZE, startPos[1] + REC_SIZE);
	    glVertex2s(startPos[0] + REC_SIZE, startPos[1] + REC_SIZE);
	    glVertex2s(startPos[0] + REC_SIZE, startPos[1] - REC_SIZE);
	    glEnd();
	}
	else {	// TILT_MODE_ACTIVE
	    glBegin(GL_LINES);
	    glVertex2s(startPos[0] - CROSS, startPos[1]);
	    glVertex2s(startPos[0] + CROSS, startPos[1]);
	    glVertex2s(startPos[0], startPos[1] - CROSS);
	    glVertex2s(startPos[0], startPos[1] + CROSS);
	    glEnd();
	}
	
	// now restore state
	restoreGLStateAfterFeedback();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Redefine this to add an extra thumbwheel on the left side to change
//  the eye level.
//
// Use: virtual protected
Widget
SoXtWalkViewer::buildLeftTrim(Widget parent)
//
////////////////////////////////////////////////////////////////////////
{
    int		n;
    Arg		args[12];
    
    // create a form to hold all the parts
    Widget form = XtCreateWidget("LeftTrimForm", xmFormWidgetClass, parent, NULL, 0);
    
    // create all the parts
    buildLeftWheel(form);
    
    // allocate extra thumbwheel
    n = 0;
    XtSetArg(args[n], XmNvalue, 0); n++;
    XtSetArg(args[n], SgNangleRange, 0); n++;
    XtSetArg(args[n], SgNunitsPerRotation, 360); n++;
    XtSetArg(args[n], SgNshowHomeButton, FALSE); n++;
    XtSetArg(args[n], XmNhighlightThickness, 0); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    Widget wheel = SgCreateThumbWheel(form, NULL, args, n);
    
    XtAddCallback(wheel, XmNvalueChangedCallback, 
	(XtCallbackProc) SoXtWalkViewer::heightWheelCB, (XtPointer) this);
    XtAddCallback(wheel, XmNdragCallback, 
	(XtCallbackProc) SoXtWalkViewer::heightWheelCB, (XtPointer) this);
    heightWheelVal = 0;
    
    Widget label = XtCreateWidget("H", xmLabelGadgetClass, form, NULL, 0);
    Widget butForm = buildAppButtons(form);
    
    //
    // layout
    //
    
    n = 0;
    XtSetArg(args[n], XmNrightAttachment,     	XmNONE); n++;
    XtSetArg(args[n], XmNleftAttachment,  	XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset,  	    	3); n++;
    XtSetArg(args[n], XmNbottomAttachment,   	XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, 	XmNONE); n++;
    XtSetValues(leftWheel, args, n);
    
    n = 0;
    XtSetArg(args[n], XmNrightAttachment,     	XmNONE); n++;
    XtSetArg(args[n], XmNleftAttachment,  	XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset,  		7); n++;
    XtSetArg(args[n], XmNbottomAttachment,   	XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNbottomWidget,   	leftWheel); n++;
    XtSetArg(args[n], XmNbottomOffset,   	10); n++;
    XtSetArg(args[n], XmNtopAttachment, 	XmNONE); n++;
    XtSetValues(label, args, n);
    
    n = 0;
    XtSetArg(args[n], XmNrightAttachment,     	XmNONE); n++;
    XtSetArg(args[n], XmNleftAttachment,  	XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset,  	    	3); n++;
    XtSetArg(args[n], XmNbottomAttachment,   	XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNbottomWidget,   	label); n++;
    XtSetArg(args[n], XmNtopAttachment, 	XmNONE); n++;
    XtSetValues(wheel, args, n);
    
    n = 0;
    XtSetArg(args[n], XmNrightAttachment,     	XmNONE); n++;
    XtSetArg(args[n], XmNleftAttachment,  	XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, 	XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment,   	XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNbottomWidget,   	wheel); n++;
    XtSetValues(butForm, args, n);
    
    // manage children
    XtManageChild(leftWheel);
    XtManageChild(label);
    XtManageChild(wheel);
    XtManageChild(butForm);
    
    return form;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Redefine this routine to add some viewer specific stuff.
//
// Use: virtual protected
void
SoXtWalkViewer::createPrefSheet()
//
////////////////////////////////////////////////////////////////////////
{
    // create the preference sheet shell and form widget
    Widget shell, form;
    createPrefSheetShellAndForm(shell, form);
    
    // create all of the parts
    Widget widgetList[10];
    int num = 0;
    createDefaultPrefSheetParts(widgetList, num, form);
    widgetList[num++] = createSpeedPrefSheetGuts(form);
    
    layoutPartsAndMapPrefSheet(widgetList, num, form, shell);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This routine is used to define cursors (can only be called after
//  window has been realized).
//
// Use: private

void
SoXtWalkViewer::defineCursors()
//
////////////////////////////////////////////////////////////////////////
{
    XColor foreground, background;
    Pixmap source, mask;
    Display *display = getDisplay();
    Drawable d = DefaultRootWindow(display);
    
    // set color
    foreground.red = 65535;
    foreground.green = foreground.blue = 0;
    background.red = background.green = background.blue = 65535;
    
    // walk cursor
    source = XCreateBitmapFromData(display, d, 
	so_xt_walk_bits, so_xt_walk_width, so_xt_walk_height);
    mask = XCreateBitmapFromData(display, d, 
	so_xt_walk_mask_bits, so_xt_walk_width, so_xt_walk_height);
    walkCursor = XCreatePixmapCursor(display, source, mask, 
	&foreground, &background, so_xt_walk_x_hot, so_xt_walk_y_hot);
    XFreePixmap(display, source);
    XFreePixmap(display, mask);
    
    // panning cursor
    source = XCreateBitmapFromData(display, d, 
	so_xt_pan_bits, so_xt_pan_width, so_xt_pan_height);
    panCursor = XCreatePixmapCursor(display, source, source, 
	&foreground, &foreground, so_xt_pan_x_hot, so_xt_pan_y_hot);
    XFreePixmap(display, source);
    
    // tilt cursor
    source = XCreateBitmapFromData(display, d, 
	so_xt_tilt_bits, so_xt_tilt_width, so_xt_tilt_height);
    mask = XCreateBitmapFromData(display, d, 
	so_xt_tilt_mask_bits, so_xt_tilt_width, so_xt_tilt_height);
    tiltCursor = XCreatePixmapCursor(display, source, mask, 
	&foreground, &background, so_xt_tilt_x_hot, so_xt_tilt_y_hot);
    XFreePixmap(display, source);
    XFreePixmap(display, mask);
    
    // seek cursor
    source = XCreateBitmapFromData(display, d, 
	so_xt_target_bits, so_xt_target_width, so_xt_target_height);
    seekCursor = XCreatePixmapCursor(display, source, source, 
	&foreground, &foreground, so_xt_target_x_hot, so_xt_target_y_hot);
    XFreePixmap(display, source);
    
    // up direction cursor
    source = XCreateBitmapFromData(display, d, 
	so_xt_normal_vec_bits, so_xt_normal_vec_width, so_xt_normal_vec_height);
    upCursor = XCreatePixmapCursor(display, source, source, 
	&foreground, &foreground, so_xt_normal_vec_x_hot, so_xt_normal_vec_y_hot);
    XFreePixmap(display, source);
    
    createdCursors = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Called when we rotate the camera (tilt up/down and rotate sideway)
//
// Use: private

void
SoXtWalkViewer::rotateCamera()
//
////////////////////////////////////////////////////////////////////////
{
    if (camera == NULL)
	return;
    
    SbVec2s raSize = getGlxSize();
    
    //
    // tilt the head up/down (constrained to 180 degree)
    //
    
    float mouseY = (locator[1] - prevPos[1]) / float(raSize[1]);
    if (mouseY)
	SoXtConstrainedViewer::tiltCamera(mouseY * M_PI);
    
    //
    // rotate the camera left/right around the Up direction (no roll alowed)
    //
    
    float mouseX = (locator[0] - prevPos[0]) / float(raSize[0]);
    if (mouseX) {
	float angle = - mouseX * M_PI;
	SbRotation rot(upDirection, angle);
	camera->orientation = camera->orientation.getValue() * rot;
    }
    
    prevPos = locator;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Called by the animation sensor to change the camera position
//
// Use: private

void
SoXtWalkViewer::doCameraAnimation()
//
////////////////////////////////////////////////////////////////////////
{
    if (camera == NULL)
	return;
    
    SbVec2s raSize = getGlxSize();
    
    //
    // get time interval since last call
    //
    SbTime time = viewerRealTime->getValue();
    float sec = float((time - prevAnimTime).getValue());
    prevAnimTime = time;
    
    // make sure to have at least a delta time for the first call.
    if (sec == 0.0)
	sec = 1.0/72.0;
    
    //
    // move forward/backward and sideway
    // 
    if (mode == WALK_MODE_ACTIVE) {
	
	// turn the camera left/right using the distance^2 which gives a nice
	// gradual speedup.
	float mouseX = (locator[0] - startPos[0]) / float(raSize[0]);
	float angle = TURN_SPEED * (mouseX * mouseX) * sec;
	if (angle != 0.0) {
	    if (mouseX < 0)
		angle = -angle;
	    SbRotation rot(upDirection, -angle);
	    camera->orientation = camera->orientation.getValue() * rot;
	}
	
	// move forward/backward
	float mouseY = (locator[1] - startPos[1]) / float(raSize[1]);
	float dist = viewerSpeed * sceneSize * (mouseY * mouseY) * sec 
			* WALK_SPEED_FACTOR;
	if (mouseY < 0)
	    dist = -dist;
	if (dist != 0.0)
	    dollyCamera(dist);
    }
    //
    // else pan in the viewer focal plane.
    //
    else if (mode == PAN_MODE_ACTIVE) {
	
	// move the camera up/down/left/right using the distance^2 which 
	// gives a nice gradual speedup.
	float mouseX = (locator[0] - startPos[0]) / float(raSize[0]);
	float mouseY = (locator[1] - startPos[1]) / float(raSize[1]);
	float distX = viewerSpeed * sceneHeight * (mouseX * mouseX) * sec 
			* WALK_SPEED_FACTOR;
	float distY = viewerSpeed * sceneHeight * (mouseY * mouseY) * sec 
			* WALK_SPEED_FACTOR;
	if (mouseX < 0)
	    distX = -distX;
	if (mouseY < 0)
	    distY = -distY;
	
	// figure out the up and right vector
	SbMatrix mx;
	mx = camera->orientation.getValue();
	SbVec3f upVector(mx[1][0], mx[1][1], mx[1][2]);
	SbVec3f rightVector(mx[0][0], mx[0][1], mx[0][2]);
	
	// finally move the camera
	if (distX)
	    camera->position = camera->position.getValue() + distX * rightVector;
	if (distY)
	    camera->position = camera->position.getValue() + distY * upVector;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   	Redefines this to move at the eye level (default) rather than
//  directly forward.
//
// Use: virtual protected

void
SoXtWalkViewer::rightWheelMotion(float newVal)
//
////////////////////////////////////////////////////////////////////////
{
    float dist = (newVal - rightWheelVal) * sceneSize * viewerSpeed 
	    * WHEEL_DOLLY_FACTOR;
    dollyCamera(dist);
    
    rightWheelVal = newVal;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Move the camera in the eye level plane (default 
// way of walking) or in the lookat direction which is an alternate way
// of walking (base class)
// 
// Use: private

void
SoXtWalkViewer::dollyCamera(float dist)
//
////////////////////////////////////////////////////////////////////////
{
    if (camera == NULL)
	return;
    
    // compute the eye level direction vector. This is simply
    // the cross produc between the up direction and the 
    // camera right vector
    SbMatrix mx;
    mx = camera->orientation.getValue();
    SbVec3f eyeLevel = upDirection.cross( SbVec3f(mx[0][0], mx[0][1], mx[0][2]) );
    eyeLevel.normalize();
    
    // move camera by distance
    camera->position = camera->position.getValue() + eyeLevel * dist;
    camera->focalDistance = camera->focalDistance.getValue() - dist;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine will pick a few times around the center of the
//  screen to get an estimate of what the camera focal distance should
//  be. Some viewers (like the examiner viewer) depend a resonable
//  focal distance, so we need to have it point at objects in front of us.
//
// Use: private

void
SoXtWalkViewer::updateCameraFocalPoint()
//
////////////////////////////////////////////////////////////////////////
{
    int numPicked = 0;
    float distance = 0;
    float locations[4][2] = {{.5, .3}, {.5, .7}, {.3, .5}, {.7, .5}};
    SbVec3f camPos = camera->position.getValue();
    SoPickedPoint *pp;
    
    SoRayPickAction pick = SoRayPickAction(SbViewportRegion(getGlxSize()));
    pick.setRadius(1.0);
    pick.setPickAll(FALSE); // pick only the closest object
    
    //
    // pick around the center point, and average
    // all the picks to get a more resonable value.
    //
    for (int i=0; i<4; i++) {
	pick.setNormalizedPoint(SbVec2f(locations[i]));
	pick.apply(sceneRoot);
	pp = pick.getPickedPoint();
	if (pp) {
	    distance += (pp->getPoint() - camPos).length();
	    numPicked++;
	    
	    // only need two good picks...(faster)
	    if (numPicked == 2)
		break;
	}
    }
    
    if (numPicked > 1)
	distance /= float(numPicked);
    
    // now update the camera focal distance
    if (distance != 0) {
	camera->enableNotify(FALSE); // don't cause a redraw
	camera->focalDistance = distance;
	camera->enableNotify(TRUE);
    }
}



//
// redefine those generic virtual functions
//
const char *
SoXtWalkViewer::getDefaultWidgetName() const
{ return "SoXtWalkViewer"; }

const char *
SoXtWalkViewer::getDefaultTitle() const
{ return rl.walkViewer; }

const char *
SoXtWalkViewer::getDefaultIconTitle() const
{ return rl.walkViewer; }






//
////////////////////////////////////////////////////////////////////////
// static callbacks stubs
////////////////////////////////////////////////////////////////////////
//

void
SoXtWalkViewer::animationSensorCB(void *p, SoSensor *)
{ ((SoXtWalkViewer *)p)->doCameraAnimation(); }

void
SoXtWalkViewer::heightWheelCB(Widget, SoXtWalkViewer *p, XtPointer *d)
{
    if (p->camera == NULL)
	return;
    
    static SbBool firstDrag = TRUE;
    SgThumbWheelCallbackStruct *data = (SgThumbWheelCallbackStruct *) d;
    
    if (data->reason == XmCR_DRAG) {
	// for the first move, invoke the start callbacks
	if (firstDrag) {
	    p->interactiveCountInc();
	    firstDrag = FALSE;
	}
	
	// move the camera up/down
	float d = (p->heightWheelVal - data->value) * p->sceneHeight 
		    * p->viewerSpeed * WHEEL_HEIGHT_FACTOR;
	p->camera->position = p->camera->position.getValue() + d * p->upDirection;
	p->heightWheelVal = data->value;
    }
    else {
	// reason = XmCR_VALUE_CHANGED, invoke the finish callbacks
	p->interactiveCountDec();
	firstDrag = TRUE;
    }
}

//
// called by the viewer finish callback to update the camera focal point
//
void
SoXtWalkViewer::setFocalPointFinishCallback(void *data, SoXtViewer *)
{
    SoXtWalkViewer *p = (SoXtWalkViewer *) data;
    
    // if we are not at the end of a seek, update the camera focal distance
    if (! p->isSeekMode())
	p->updateCameraFocalPoint();
}
