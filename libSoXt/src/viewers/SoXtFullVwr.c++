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
 |   $Revision: 1.4 $
 |
 |   Classes    : SoXtFullViewer
 |
 |   Author()	: Alain Dumesny
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <inttypes.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>

#include <Xm/Xm.h>
#include <Xm/LabelG.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/SeparatoG.h>
#include <Xm/CascadeB.h>
#include <Xm/CascadeBG.h>
#include <Xm/Form.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>
#include <Xm/Text.h>
#include <Sgm/ThumbWheel.h>
#include <Xm/MessageB.h>

#include <Inventor/SoPickedPoint.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/SoXtResource.h>
#include <Inventor/Xt/viewers/SoXtFullViewer.h>
#include <Inventor/Xt/SoXtIcons.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <GL/gl.h>
#include "SoXtBitmapButton.h"
#include <Inventor/errors/SoDebugError.h>

/*
 * Defines
 */
#define SCREEN(w) XScreenNumberOfScreen( XtScreen(w) )

// specifies the sizes for the decoration
#define	DECOR_SIZE	    28
#define LABEL_SPACE 	    3
#define LABEL_SEPARATION    12
#define THUMB_SPACE 	    4
#define WHEEL_DOLLY_FACTOR  0.5

// list of the different popup choices
enum popupChoices {
    VIEW_ALL = 20,  // enables the same menu routine to be used
    SET_HOME,	 // as the draw style entrie (can't overlap
    HOME,	  // with SoXtViewerDrawStyle values)
    HEADLIGHT, 
    SEEK, 
    PREF, 
    VIEWING, 
    DECORATION, 
    COPY_VIEW, 
    PASTE_VIEW, 
    HELP
};

enum drawChoices {
    AS_IS,
    HIDDEN_LINE, 
    NO_TXT, 
    LOW_RES, 
    LINE, 
    POINT, 
    BBOX, 
    
    MOVE_SAME_AS, 
    MOVE_NO_TXT, 
    MOVE_LOW_RES, 
    MOVE_LINE, 
    MOVE_LOW_LINE, 
    MOVE_POINT, 
    MOVE_LOW_POINT, 
    MOVE_BBOX, 
    
    DRAW_STYLE_NUM // specify the length
};

// list of the toggle buttons in the popumenu
enum popupToggles {
    HEADLIGHT_WIDGET = 0,    // very convenient to start at 0
    VIEWING_WIDGET,
    DECORATION_WIDGET,
    
    POPUP_TOGGLE_NUM   // specify the length
};

// list of custom push buttons
enum ViewerPushButtons {
    PICK_PUSH, 
    VIEW_PUSH, 
    HELP_PUSH, 
    HOME_PUSH,
    SET_HOME_PUSH,
    VIEW_ALL_PUSH,
    SEEK_PUSH,
    
    PUSH_NUM
};

enum ZoomSliderVars {
    ZOOM_LABEL, 
    ZOOM_SLIDER, 
    ZOOM_FIELD, 
    ZOOM_RANGE_LAB1, 
    ZOOM_RANGE_FIELD1, 
    ZOOM_RANGE_LAB2, 
    ZOOM_RANGE_FIELD2, 
    
    ZOOM_NUM
};


/*
 * Macros
 */

#define TOGGLE_ON(BUTTON) \
    XmToggleButtonSetState((Widget) BUTTON, True, False)
#define TOGGLE_OFF(BUTTON) \
    XmToggleButtonSetState((Widget) BUTTON, False, False)


static char *thisClassName = "SoXtFullViewer";


// Resources for labels.
typedef struct {
	char *seekAniTime;
	char *seekToLabel;
	char *seekDistance;
	char *cameraZoom;
	char *zoomSlider;
	char *to;
	char *nearPlane;
	char *farPlane;
	char *cameraRotation;
	char *stereoErrorTitle;
	char *stereoError;
	char *preferences;
	char *viewerMenu;
	char *viewerSpeed;
	char *increase;
	char *decrease;
} RES_LABELS;
static RES_LABELS rl;
static char *defaults[]={ 
	"Seek animation time:",
	"Seek to:",
	"Seek distance:",
	"Camera Zoom:", 
	"Zoom slider ranges from:", 
	"to:",
	"near plane:",
	"far plane:",
	"camera rotation:",
	"Stereo Error Dialog",
	"Stereo Viewing can't be set on this machine.",
	"Preferences...",
	"Viewer Menu",
	"Viewer Speed:",
	" increase ",
	" decrease "
};


////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
// Use: protected

SoXtFullViewer::SoXtFullViewer(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent,
    SoXtFullViewer::BuildFlag buildFlag, 
    SoXtViewer::Type t, 
    SbBool buildNow) 
	: SoXtViewer(
	    parent,
	    name, 
	    buildInsideParent, 
	    t, 
	    FALSE)  // buildNow
//
////////////////////////////////////////////////////////////////////////
{
    int i;
    
    setClassName(thisClassName);
    
    setSize(SbVec2s(500, 390));  // default size
    
    firstBuild = TRUE; // used to get pref sheet resources only once
    
    // init decoration vars
    decorationFlag = (buildFlag & BUILD_DECORATION) != 0;
    mgrWidget = NULL;
    leftTrimForm = bottomTrimForm = rightTrimForm = NULL;
    rightWheelStr = bottomWheelStr = leftWheelStr = NULL;
    rightWheelLabel = bottomWheelLabel = leftWheelLabel = NULL;
    zoomSldRange.setValue(1, 140);
    
    // init pref sheet vars
    prefSheetShellWidget = NULL;
    prefSheetStr = NULL;
    zoomWidgets = new Widget[ZOOM_NUM];
    for (i=0; i<ZOOM_NUM; i++)
	zoomWidgets[i] = NULL;
    
    // init popup menu vars
    popupWidget = NULL;
    popupEnabled = (buildFlag & BUILD_POPUP) != 0;
    popupTitle = NULL;
    popupToggleWidgets = new Widget[POPUP_TOGGLE_NUM];
    for (i=0; i<POPUP_TOGGLE_NUM; i++)
	popupToggleWidgets[i] = NULL;
    drawStyleWidgets = new Widget[DRAW_STYLE_NUM];
    for (i=0; i<DRAW_STYLE_NUM; i++)
	drawStyleWidgets[i] = NULL;
    bufferStyleWidgets = new Widget[3];
    for (i=0; i<3; i++)
	bufferStyleWidgets[i] = NULL;
    
    // init buttons stuff
    for (i=0; i<PUSH_NUM; i++)
	buttonList[i] = NULL;
    viewerButtonWidgets = new SbPList(PUSH_NUM);
    appButtonForm = NULL;
    appButtonList = new SbPList;
    
    // Build the widget tree, and let SoXtComponent know about our base widget.
    if (buildNow) {
	Widget w = buildWidget(getParentWidget());
	setBaseWidget(w);
    }
}

////////////////////////////////////////////////////////////////////////
//
//    Destructor.
//
// Use: protected

SoXtFullViewer::~SoXtFullViewer()
//
////////////////////////////////////////////////////////////////////////
{
    // unregister the widget
    unregisterWidget(mgrWidget);
        
    // delete decoration stuff
    if (rightWheelStr != NULL) free(rightWheelStr);
    if (bottomWheelStr != NULL) free(bottomWheelStr);
    if (leftWheelStr != NULL) free(leftWheelStr);
    
    // delete  popup stuff
    if (popupTitle != NULL) free(popupTitle);
    delete [] popupToggleWidgets;
    delete [] drawStyleWidgets;
    delete [] bufferStyleWidgets;
    
    // delete push button stuff
    for (int i=0; i<PUSH_NUM; i++)
	delete buttonList[i];
    delete viewerButtonWidgets;
    delete appButtonList;
    
    // delete pref sheet stuff
    delete [] zoomWidgets;
    if (prefSheetStr != NULL) free(prefSheetStr);
    if (prefSheetShellWidget != NULL) {
	XtRemoveCallback(prefSheetShellWidget, XtNdestroyCallback,
	    (XtCallbackProc) SoXtFullViewer::prefSheetDestroyCB,
	    (XtPointer) this);
	XtDestroyWidget(prefSheetShellWidget);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    redefine this to also update the UI
//
// Use: virtual public
void
SoXtFullViewer::setViewing(SbBool flag)
//
////////////////////////////////////////////////////////////////////////
{
    if (flag == viewingFlag)
	return;
    
    // call the base class
    SoXtViewer::setViewing(flag);
    
    // update the push buttons
    if (buttonList[VIEW_PUSH])
	buttonList[VIEW_PUSH]->select(viewingFlag);
    if (buttonList[PICK_PUSH])
	buttonList[PICK_PUSH]->select(! viewingFlag);
    
    // update the popup menu entry
    if (popupToggleWidgets[VIEWING_WIDGET])
	XmToggleButtonSetState(popupToggleWidgets[VIEWING_WIDGET], 
					viewingFlag, False);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    redefine this to also update the popup menu
//
// Use: virtual public
void
SoXtFullViewer::setHeadlight(SbBool flag)
//
////////////////////////////////////////////////////////////////////////
{
    if (flag == isHeadlight())
	return;
    
    // call base class routine
    SoXtViewer::setHeadlight(flag);
    
    // update the popup menu entry
    if (popupToggleWidgets[HEADLIGHT_WIDGET])
	XmToggleButtonSetState(popupToggleWidgets[HEADLIGHT_WIDGET], 
					isHeadlight(), False);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    redefine this to also update the popup menu
//
// Use: virtual public
void
SoXtFullViewer::setDrawStyle(SoXtViewer::DrawType type, 
				SoXtViewer::DrawStyle style)
//
////////////////////////////////////////////////////////////////////////
{
    // call base class routine
    SoXtViewer::setDrawStyle(type, style);
    
    // update the popup menu entries
    if (drawStyleWidgets[0]) {
	for (int i = 0; i < DRAW_STYLE_NUM; i++)
	    TOGGLE_OFF(drawStyleWidgets[i]);
	switch(getDrawStyle(SoXtViewer::STILL)) {
	    case SoXtViewer::VIEW_AS_IS: TOGGLE_ON(drawStyleWidgets[AS_IS]); break;
	    case SoXtViewer::VIEW_HIDDEN_LINE: TOGGLE_ON(drawStyleWidgets[HIDDEN_LINE]); break;
	    case SoXtViewer::VIEW_NO_TEXTURE: TOGGLE_ON(drawStyleWidgets[NO_TXT]); break;
	    case SoXtViewer::VIEW_LOW_COMPLEXITY: TOGGLE_ON(drawStyleWidgets[LOW_RES]); break;
	    case SoXtViewer::VIEW_LINE: TOGGLE_ON(drawStyleWidgets[LINE]); break;
	    case SoXtViewer::VIEW_POINT: TOGGLE_ON(drawStyleWidgets[POINT]); break;
	    case SoXtViewer::VIEW_BBOX: TOGGLE_ON(drawStyleWidgets[BBOX]); break;
	}
	switch(getDrawStyle(SoXtViewer::INTERACTIVE)) {
	    case SoXtViewer::VIEW_SAME_AS_STILL: TOGGLE_ON(drawStyleWidgets[MOVE_SAME_AS]); break;
	    case SoXtViewer::VIEW_NO_TEXTURE: TOGGLE_ON(drawStyleWidgets[MOVE_NO_TXT]); break;
	    case SoXtViewer::VIEW_LOW_COMPLEXITY: TOGGLE_ON(drawStyleWidgets[MOVE_LOW_RES]); break;
	    case SoXtViewer::VIEW_LINE: TOGGLE_ON(drawStyleWidgets[MOVE_LINE]); break;
	    case SoXtViewer::VIEW_LOW_RES_LINE: TOGGLE_ON(drawStyleWidgets[MOVE_LOW_LINE]); break;
	    case SoXtViewer::VIEW_POINT: TOGGLE_ON(drawStyleWidgets[MOVE_POINT]); break;
	    case SoXtViewer::VIEW_LOW_RES_POINT: TOGGLE_ON(drawStyleWidgets[MOVE_LOW_POINT]); break;
	    case SoXtViewer::VIEW_BBOX: TOGGLE_ON(drawStyleWidgets[MOVE_BBOX]); break;
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    redefine this to also update the popup menu
//
// Use: virtual public
void
SoXtFullViewer::setBufferingType(SoXtViewer::BufferType type)
//
////////////////////////////////////////////////////////////////////////
{
    // call base class routine
    SoXtViewer::setBufferingType(type);
    
    // update the popup menu entries
    if (bufferStyleWidgets[0]) {
	for (int i = 0; i < 3; i++)
	    TOGGLE_OFF(bufferStyleWidgets[i]);
	TOGGLE_ON(bufferStyleWidgets[getBufferingType()]);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the camera to use (done in base class) and makes sure to
// detach and re-attach() the zoom slider sensor.
//
// Use: virtual public
void
SoXtFullViewer::setCamera(SoCamera *newCamera)
//
////////////////////////////////////////////////////////////////////////
{
    // call base class routine
    SoXtViewer::setCamera(newCamera);
    
    // check if the zoom slider needs to be enabled
    if (zoomWidgets[ZOOM_SLIDER] != NULL) {
	
	SbBool enable = camera != NULL && camera->isOfType(SoPerspectiveCamera::getClassTypeId());
	for (int i=0; i<ZOOM_NUM; i++)
	    XtVaSetValues(zoomWidgets[i], XmNsensitive, enable, NULL);
	
	// update the UI if enabled
	if (enable) {
	    float zoom = getCameraZoom();
	    setZoomSliderPosition(zoom);
	    setZoomFieldString(zoom);
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    shows/hides the decoration.
//
// Use: public
void
SoXtFullViewer::setDecoration(SbBool flag)
//
////////////////////////////////////////////////////////////////////////
{
    if (mgrWidget == NULL || flag == decorationFlag) {
    	decorationFlag = flag;
	return;
    }
    
    int     n;
    Arg     args[12];
    
    decorationFlag = flag;
    
    if (decorationFlag) {
	
	// set renderArea offset
	n = 0;
	XtSetArg(args[n], XmNbottomOffset,  DECOR_SIZE); n++;
	XtSetArg(args[n], XmNleftOffset,    DECOR_SIZE); n++;
	XtSetArg(args[n], XmNrightOffset,   DECOR_SIZE); n++;
	XtSetValues(raWidget, args, n);
	
	// check if decoration needs to be built
	// ??? just need to check one the decoration form widget ?
	if (leftTrimForm == NULL)
	    buildDecoration(mgrWidget);
	
	// show the decoration
	XtManageChild(leftTrimForm);
	XtManageChild(bottomTrimForm);
	XtManageChild(rightTrimForm);
    }
    else {
	
	// hide the decoration, making sure it was first built
	// (just need to check one the decoration form widget)
	if (leftTrimForm != NULL) {
	    XtUnmanageChild(leftTrimForm);
	    XtUnmanageChild(bottomTrimForm);
	    XtUnmanageChild(rightTrimForm);
	}
	
	// set renderArea offset
	n = 0;
	XtSetArg(args[n], XmNbottomOffset,  0); n++;
	XtSetArg(args[n], XmNleftOffset,    0); n++;
	XtSetArg(args[n], XmNrightOffset,   0); n++;
	XtSetValues(raWidget, args, n);
    }
    
    // update the popup menu entry
    if (popupToggleWidgets[DECORATION_WIDGET])
	XmToggleButtonSetState(popupToggleWidgets[DECORATION_WIDGET], 
						decorationFlag, False);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    enables/disables the popup menu.
//
// Use: virtual public
void
SoXtFullViewer::setPopupMenuEnabled(SbBool flag)
//
////////////////////////////////////////////////////////////////////////
{
    // chech for trivial return
    if (mgrWidget == NULL || flag == popupEnabled) {
	popupEnabled = flag;
	return;
    }
    
    popupEnabled = flag;
    
    if (popupEnabled)
	buildPopupMenu();
    else
    	destroyPopupMenu();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Add a button to the end of the application list
//
// Use: public
void
SoXtFullViewer::addAppPushButton(Widget newButton)
//
////////////////////////////////////////////////////////////////////////
{
    // add the button to the end of the list
    appButtonList->append(newButton);
    
    // redo the layout again
    doAppButtonLayout(appButtonList->getLength() - 1);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Insert a button in the application list at the given index
//
// Use: public
void
SoXtFullViewer::insertAppPushButton(Widget newButton, int index)
//
////////////////////////////////////////////////////////////////////////
{
    // add the button at the specified index
    appButtonList->insert(newButton, index);
    
    // redo the layout again
    doAppButtonLayout( appButtonList->find(newButton) );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Remove a button from the application list
//
// Use: public
void
SoXtFullViewer::removeAppPushButton(Widget oldButton)
//
////////////////////////////////////////////////////////////////////////
{
    // find the index where the button is
    int index = appButtonList->find(oldButton);
    if (index == -1)
	return;
    
    // remove from the list and redo the layout
    int lastIndex = appButtonList->getLength() - 1;
    appButtonList->remove(index);
    if (index != lastIndex)
	doAppButtonLayout(index);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Redefines this to also hide the preference sheet.
//
// Use: virtual public
void
SoXtFullViewer::hide()
//
////////////////////////////////////////////////////////////////////////
{
    // call the parent class
    SoXtViewer::hide();
    
    // destroy the pref sheet if it is currently on the screen
    if (prefSheetShellWidget != NULL)
	XtDestroyWidget(prefSheetShellWidget);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Sets the popup menu string
//
// use: protected
void
SoXtFullViewer::setPopupMenuString(const char *str)
//
////////////////////////////////////////////////////////////////////////
{
    if (popupTitle != NULL) free(popupTitle);
    popupTitle = (str != NULL) ? strdup(str) : NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Sets the decoration bottom wheel string
//
// use: protected
void
SoXtFullViewer::setBottomWheelString(const char *str)
//
////////////////////////////////////////////////////////////////////////
{
    if (bottomWheelStr != NULL) free(bottomWheelStr);
    bottomWheelStr = (str != NULL) ? strdup(str) : NULL;
    if (bottomWheelStr != NULL && bottomWheelLabel != NULL) {
	Arg args[1];
	XmString xmstr = XmStringCreateLocalized(bottomWheelStr);
	XtSetArg(args[0], XmNlabelString, xmstr);
	XtSetValues(bottomWheelLabel, args, 1);
	XmStringFree(xmstr);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Sets the decoration left wheel string
//
// use: protected
void
SoXtFullViewer::setLeftWheelString(const char *str)
//
////////////////////////////////////////////////////////////////////////
{
    if (leftWheelStr != NULL) free(leftWheelStr);
    leftWheelStr = (str != NULL) ? strdup(str) : NULL;
    if (leftWheelStr != NULL && leftWheelLabel != NULL) {
	Arg args[1];
	XmString xmstr = XmStringCreateLocalized(leftWheelStr);
	XtSetArg(args[0], XmNlabelString, xmstr);
	XtSetValues(leftWheelLabel, args, 1);
	XmStringFree(xmstr);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Sets the decoration right wheel string
//
// use: protected
void
SoXtFullViewer::setRightWheelString(const char *str)
//
////////////////////////////////////////////////////////////////////////
{
    if (rightWheelStr != NULL) free(rightWheelStr);
    rightWheelStr = (str != NULL) ? strdup(str) : NULL;
    if (rightWheelStr != NULL && rightWheelLabel != NULL) {
	Arg args[1];
	XmString xmstr = XmStringCreateLocalized(rightWheelStr);
	XtSetArg(args[0], XmNlabelString, xmstr);
	XtSetValues(rightWheelLabel, args, 1);
	XmStringFree(xmstr);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Builds the viewer popup menu.
//
// Use: virtual protected

void
SoXtFullViewer::buildPopupMenu()
//
////////////////////////////////////////////////////////////////////////
{
    int     n, butnum = 0;
    Arg     args[12];
    Widget  buttons[15];
    
    // create popup and register routine to pop the menu
    n = 0;
#ifdef MENUS_IN_POPUP
    SoXt::getPopupArgs(XtDisplay(mgrWidget), SCREEN(mgrWidget), args, &n);
#endif
    popupWidget = XmCreatePopupMenu(mgrWidget, "menu", args, n);
    
#ifdef MENUS_IN_POPUP
    // register callbacks to load/unload the pulldown colormap when the
    // pulldown menu is posted.
    SoXt::registerColormapLoad(popupWidget, SoXt::getShellWidget(mgrWidget));
#endif
    
    XtAddEventHandler(mgrWidget, ButtonPressMask, FALSE, 
	(XtEventHandler) &SoXtFullViewer::popMenuCallback, (XtPointer)this);
    
    // make a title label for the popup menu
    if (popupTitle == NULL)
    	popupTitle = strdup( rl.viewerMenu );
    buttons[butnum++] = XtCreateWidget(popupTitle, xmLabelGadgetClass, popupWidget, NULL, 0);
    buttons[butnum++] = XtCreateWidget("sep", xmSeparatorGadgetClass, popupWidget, NULL, 0);
    
    //
    // create the submenus
    //
    buttons[butnum++] = buildFunctionsSubmenu(popupWidget);
    buttons[butnum++] = buildDrawStyleSubmenu(popupWidget);
    
    //
    // add the toggle buttons
    //
    n = 0;
    XtSetArg(args[n], XmNuserData, this); n++;
    
#define ADD_TOGGLE(NAME, W, ID, STATE)	\
    XtSetArg(args[n], XmNset, STATE); \
    buttons[butnum++] = popupToggleWidgets[W] = XtCreateWidget(NAME, \
	xmToggleButtonGadgetClass, popupWidget, args, n+1); \
    XtAddCallback(popupToggleWidgets[W], XmNvalueChangedCallback,   \
	(XtCallbackProc) SoXtFullViewer::menuPick, (XtPointer) ID);    
    
    ADD_TOGGLE("Viewing", VIEWING_WIDGET, VIEWING, isViewing())
    ADD_TOGGLE("Decoration", DECORATION_WIDGET, DECORATION, isDecoration())
    ADD_TOGGLE("Headlight", HEADLIGHT_WIDGET, HEADLIGHT, isHeadlight())
#undef ADD_TOGGLE
    
    //
    // add some more regular buttons
    //
#define ADD_ENTRY(NAME, ID)   \
    buttons[butnum] = XtCreateWidget(NAME, xmPushButtonGadgetClass, popupWidget, args, n); \
    XtAddCallback(buttons[butnum], XmNactivateCallback, \
	(XtCallbackProc) SoXtFullViewer::menuPick, (XtPointer) ID); \
    butnum++;
    
    ADD_ENTRY( rl.preferences, PREF)
#undef ADD_ENTRY
    
    // manage children
    XtManageChildren(buttons, butnum);    
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Builds function submenu - this include all of the viewer push 
//  buttons plus any useful entries.
//
// Use: protected

Widget
SoXtFullViewer::buildFunctionsSubmenu(Widget popup)
//
////////////////////////////////////////////////////////////////////////
{
    int     n, butnum = 0;
    Arg     args[12];
    Widget  buttons[15];
    
    // create a cascade menu entry which will bring the submenu
    Widget cascade = XtCreateWidget("Functions", xmCascadeButtonGadgetClass, 
    	popup, NULL, 0);
    
    // create the submenu widget
    n = 0;
#ifdef MENUS_IN_POPUP
    SoXt::getPopupArgs(XtDisplay(popup),SCREEN(popup), args, &n);
#endif
    Widget submenu = XmCreatePulldownMenu(popup, "functions", args, n);
    
    XtSetArg(args[0], XmNsubMenuId, submenu);
    XtSetValues(cascade, args, 1);
    
    //
    // create the menu entries
    //
    n = 0;
    XtSetArg(args[n], XmNuserData, this); n++;
    
#define ADD_ENTRY(NAME, ID)   \
    buttons[butnum] = XtCreateWidget(NAME, xmPushButtonGadgetClass, submenu, args, n); \
    XtAddCallback(buttons[butnum], XmNactivateCallback, \
	(XtCallbackProc) SoXtFullViewer::menuPick, (XtPointer) ID); \
    butnum++;
    
    ADD_ENTRY("Help", HELP)
    ADD_ENTRY("Home", HOME)
    ADD_ENTRY("Set Home", SET_HOME)
    ADD_ENTRY("View All", VIEW_ALL)
    ADD_ENTRY("Seek", SEEK)
    
    buttons[butnum++] = XtCreateWidget("sep", xmSeparatorGadgetClass, submenu, NULL, 0);
    
    ADD_ENTRY("Copy View", COPY_VIEW)
    ADD_ENTRY("Paste View", PASTE_VIEW)
#undef ADD_ENTRY
    
    // manage children
    XtManageChildren(buttons, butnum);
    
    return cascade;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Builds drawing style submenu
//
// Use: protected

Widget
SoXtFullViewer::buildDrawStyleSubmenu(Widget popup)
//
////////////////////////////////////////////////////////////////////////
{
    int     n, butnum = 0;
    Arg     args[12];
    Widget  buttons[30];
    
    // create a cascade menu entry which will bring the submenu
    Widget cascade = XtCreateWidget("Draw Style", xmCascadeButtonGadgetClass, 
    	popup, NULL, 0);
    
    // create the submenu widget
    n = 0;
#ifdef MENUS_IN_POPUP
    SoXt::getPopupArgs(XtDisplay(popup), SCREEN(popup), args, &n);
#endif
    Widget submenu = XmCreatePulldownMenu(popup, "draw style", args, n);
    
    XtSetArg(args[0], XmNsubMenuId, submenu);
    XtSetValues(cascade, args, 1);
    
    //
    // create the first part of this sub menu
    //
    n = 0;
    XtSetArg(args[n], XmNuserData, this); n++;
    XtSetArg(args[n], XmNindicatorType, XmONE_OF_MANY); n++;
    
#define ADD_ENTRY(NAME, ID, STATE) \
    XtSetArg(args[n], XmNset, STATE); \
    buttons[butnum++] = drawStyleWidgets[ID] = XtCreateWidget(NAME, \
	xmToggleButtonGadgetClass, submenu, args, n+1); \
    XtAddCallback(drawStyleWidgets[ID], XmNvalueChangedCallback, \
	(XtCallbackProc) SoXtFullViewer::drawStyleMenuPick, (XtPointer) ID);
    
    int drawType = getDrawStyle(SoXtViewer::STILL);
    ADD_ENTRY("as is", AS_IS, drawType == SoXtViewer::VIEW_AS_IS)
    ADD_ENTRY("hidden line", HIDDEN_LINE, drawType == SoXtViewer::VIEW_HIDDEN_LINE)
    ADD_ENTRY("no texture", NO_TXT, drawType == SoXtViewer::VIEW_NO_TEXTURE)
    ADD_ENTRY("low resolution", LOW_RES, drawType == SoXtViewer::VIEW_LOW_COMPLEXITY)
    ADD_ENTRY("wireframe", LINE, drawType == SoXtViewer::VIEW_LINE)
    ADD_ENTRY("points", POINT, drawType == SoXtViewer::VIEW_POINT)
    ADD_ENTRY("bounding box (no depth)", BBOX, drawType == SoXtViewer::VIEW_BBOX)
    
    buttons[butnum++] = XtCreateWidget("sep", xmSeparatorGadgetClass, submenu, NULL, 0);
    
    drawType = getDrawStyle(SoXtViewer::INTERACTIVE);
    ADD_ENTRY("move same as still", MOVE_SAME_AS, drawType == SoXtViewer::VIEW_SAME_AS_STILL)
    ADD_ENTRY("move no texture", MOVE_NO_TXT, drawType == SoXtViewer::VIEW_NO_TEXTURE)
    ADD_ENTRY("move low res", MOVE_LOW_RES, drawType == SoXtViewer::VIEW_LOW_COMPLEXITY)
    ADD_ENTRY("move wireframe", MOVE_LINE, drawType == SoXtViewer::VIEW_LINE)
    ADD_ENTRY("move low res wireframe (no depth)", MOVE_LOW_LINE, drawType == SoXtViewer::VIEW_LOW_RES_LINE)
    ADD_ENTRY("move points", MOVE_POINT, drawType == SoXtViewer::VIEW_POINT)
    ADD_ENTRY("move low res points (no depth)", MOVE_LOW_POINT, drawType == SoXtViewer::VIEW_LOW_RES_POINT)
    ADD_ENTRY("move bounding box (no depth)", MOVE_BBOX, drawType == SoXtViewer::VIEW_BBOX)
#undef ADD_ENTRY
    
    buttons[butnum++] = XtCreateWidget("sep", xmSeparatorGadgetClass, submenu, NULL, 0);
    
    //
    // create the second part of this sub menu
    //
#define ADD_ENTRY(NAME, ID) \
    XtSetArg(args[n], XmNset, bufType == ID); \
    buttons[butnum++] = bufferStyleWidgets[ID] = XtCreateWidget(NAME, \
	xmToggleButtonGadgetClass, submenu, args, n+1); \
    XtAddCallback(bufferStyleWidgets[ID], XmNvalueChangedCallback, \
	(XtCallbackProc) SoXtFullViewer::bufferStyleMenuPick, (XtPointer) ID);
    
    int bufType = getBufferingType();
    ADD_ENTRY("single buffer", SoXtViewer::BUFFER_SINGLE)
    ADD_ENTRY("double buffer", SoXtViewer::BUFFER_DOUBLE)
    ADD_ENTRY("interactive buffer", SoXtViewer::BUFFER_INTERACTIVE)
#undef ADD_ENTRY
    
    // manage children
    XtManageChildren(buttons, butnum);
    
    return cascade;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Deletes the viewer popup menu.
//
// Use: protected

void
SoXtFullViewer::destroyPopupMenu()
//
////////////////////////////////////////////////////////////////////////
{
    int i;
    
    // remove callback to pop it up
    XtRemoveEventHandler(mgrWidget, ButtonPressMask, FALSE, 
	(XtEventHandler) &SoXtFullViewer::popMenuCallback, (XtPointer)this);
    
    // destroy the popup menu and reset the menu variables...
    XtDestroyWidget(popupWidget);
    popupWidget = NULL;
    for (i=0; i<POPUP_TOGGLE_NUM; i++)
	popupToggleWidgets[i] = NULL;
    for (i=0; i<DRAW_STYLE_NUM; i++)
	drawStyleWidgets[i] = NULL;
    for (i=0; i<3; i++)
	bufferStyleWidgets[i] = NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Get X resources for the widget.
//
// Use: private
void
SoXtFullViewer::getResources(SoXtResource *xr)
//
////////////////////////////////////////////////////////////////////////
{
    // Decoration
    xr->getResource("decoration", "Decoration", decorationFlag);
    
    // Get resources for preference sheet items.
    float  val;
    SbBool flag;
    char   *str;
    
    // seek...
    if (xr->getResource("seekAnimationTime", "SeekAnimationTime", val))
	setSeekTime(val);
    if (xr->getResource("seekTo", "SeekTo", str)) {
	if (strcasecmp(str,"point") == 0)
	    setDetailSeek(TRUE);
	else if (strcasecmp(str,"object") == 0)
	    setDetailSeek(FALSE);
    }
    if (xr->getResource("seekDistanceUsage", "SeekDistanceUsage", str)) {
	if (strcasecmp(str,"percentage") == 0)
	    seekDistAsPercentage = TRUE;
	else if (strcasecmp(str,"absolute") == 0)
	    seekDistAsPercentage = FALSE;
    }
    
    // zoom slider...
    if (xr->getResource("zoomMin", "ZoomMin", val))
	zoomSldRange[0] = val;
    if (xr->getResource("zoomMax", "ZoomMax", val))
	zoomSldRange[1] = val;
    
    // auto clipping planes...
    if (xr->getResource("autoClipping", "AutoClipping", flag))
	setAutoClipping(flag);
	
    // manual clipping planes...
    //??? what if camera is NULL? should we save the values somewhere?
    if (camera != NULL) {
	if (xr->getResource("nearDistance", "NearDistance", val))
	    camera->nearDistance = val;
	if (xr->getResource("farDistance", "FarDistance", val))
	    camera->farDistance = val;
    }

    // labels and messages.
    //
    if (!xr->getResource( "seekAniTime",    "SeekAniTime",    rl.seekAniTime ))
	rl.seekAniTime    = defaults[0];
    if (!xr->getResource( "seekToLabel",    "SeekToLabel",    rl.seekToLabel ))
	rl.seekToLabel    = defaults[1];
    if (!xr->getResource( "seekDistance",   "SeekDistance",   rl.seekDistance ))
	rl.seekDistance   = defaults[2];
    if (!xr->getResource( "cameraZoom",     "CameraZoom",     rl.cameraZoom ))
	rl.cameraZoom     = defaults[3];
    if (!xr->getResource( "zoomSlider",     "ZoomSlider",     rl.zoomSlider ))
	rl.zoomSlider     = defaults[4];
    if (!xr->getResource( "to",             "To",             rl.to ))
	rl.to             = defaults[5];
    if (!xr->getResource( "nearPlane",      "NearPlane",      rl.nearPlane ))
	rl.nearPlane      = defaults[6];
    if (!xr->getResource( "farPlane",       "FarPlane",       rl.farPlane ))
	rl.farPlane       = defaults[7];
    if (!xr->getResource( "cameraRotation", "CameraRotation", rl.cameraRotation ))
	rl.cameraRotation = defaults[8];
    //
    if (!xr->getResource( "stereoErrorTitle", "StereoErrorTitle", rl.stereoErrorTitle ))
	rl.stereoErrorTitle = defaults[9];
    if (!xr->getResource( "stereoError",      "StereoError",      rl.stereoError ))
	rl.stereoError      = defaults[10];
    //
    if (!xr->getResource( "preferences",      "Preferences",      rl.preferences ))
	rl.preferences      = defaults[11];
    if (!xr->getResource( "viewerMenu",       "ViewerMenu",       rl.viewerMenu ))
	rl.viewerMenu       = defaults[12];
    if (!xr->getResource( "viewerSpeed",      "ViewerSpeed",      rl.viewerSpeed ))
	rl.viewerSpeed      = defaults[13];
    if (!xr->getResource( "increase",          "Increase",         rl.increase ))
	rl.increase         = defaults[14];
    if (!xr->getResource( "decrease",          "Decrease",         rl.decrease ))
	rl.decrease         = defaults[15];


}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Builds the basic Viewer Component widget, complete with
// functionality of a SoXtFullViewerManip, pop-up menu, sliders, etc.
// Builds all subwidgets, and does layout using motif
//
// Use: protected
Widget
SoXtFullViewer::buildWidget(Widget parent)
//
////////////////////////////////////////////////////////////////////////
{
    //
    // create a top level form to hold everything together
    //
    
    Arg args[8];
    int n = 0;
    SbVec2s size = getSize();
    if ((size[0] != 0) && (size[1] != 0)) {
	XtSetArg(args[n], XtNwidth, size[0]); n++;
	XtSetArg(args[n], XtNheight, size[1]); n++;
    }
    
    // ??? don't listen to resize request by children - because the
    // ??? form widget layout will force the size down. This will prevent 
    // ??? the RenderArea to pop to 400x400 size (default ) after the user 
    // ??? set an explicit smaller size.
    XtSetArg(args[n], XmNresizePolicy, XmRESIZE_NONE); n++;
    
    // Create the root widget and register it with a class name
    mgrWidget = XtCreateWidget(getWidgetName(), xmFormWidgetClass, parent, args, n);
    registerWidget(mgrWidget);
    
    // Get widget resources
    if (firstBuild) {
	SoXtResource xr(mgrWidget);    
	getResources(&xr);
	firstBuild = FALSE;
    }

    // build the components
    raWidget = SoXtRenderArea::buildWidget(mgrWidget);
    if (decorationFlag)
    	buildDecoration(mgrWidget);
    
    //
    // Layout
    //
    n = 0;
    XtSetArg(args[n], XmNleftAttachment,   XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment,  XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment,    XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetValues(raWidget, args, n);
    
    // manage children
    decorationFlag = !decorationFlag;   // enable routine to be called
    setDecoration(! decorationFlag);
    XtManageChild(raWidget);
    
    // build the popup menu 
    if (popupEnabled)
    	buildPopupMenu();

    return mgrWidget;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//	Builds the Decoration trim (thumbwheel, text, slider, buttons, ..).
//
// Use: virtual protected
void
SoXtFullViewer::buildDecoration(Widget parent)
//
////////////////////////////////////////////////////////////////////////
{
    int		n;
    Arg		args[12];
    
    // build the trim sides
    leftTrimForm = buildLeftTrim(parent);
    bottomTrimForm = buildBottomTrim(parent);
    rightTrimForm = buildRightTrim(parent);
    
    //
    // layout
    //
    
    n = 0;
    XtSetArg(args[n], XmNtopAttachment, 	XmNONE); n++;
    XtSetArg(args[n], XmNbottomAttachment,  	XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment,   	XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, 	XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNheight, 		DECOR_SIZE); n++;
    XtSetValues(bottomTrimForm, args, n);
    
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,     	XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment,  	XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomOffset,     	DECOR_SIZE); n++;
    XtSetArg(args[n], XmNleftAttachment,   	XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, 	XmNONE); n++;
    XtSetArg(args[n], XmNwidth, 		DECOR_SIZE); n++;
    XtSetValues(leftTrimForm, args, n);
    
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,     	XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment,  	XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomOffset,     	DECOR_SIZE); n++;
    XtSetArg(args[n], XmNleftAttachment, 	XmNONE); n++;
    XtSetArg(args[n], XmNrightAttachment, 	XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNwidth, 		DECOR_SIZE); n++;
    XtSetValues(rightTrimForm, args, n);
    
    // ??? children are managed by setDecoration()
    // ??? which is called after this routine by buildWidget()
 }

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Builds the left thumbwheel
//
// Use: protected
void
SoXtFullViewer::buildLeftWheel(Widget parent)
//
////////////////////////////////////////////////////////////////////////
{
    int		n;
    Arg		args[12];
    
    n = 0;
    XtSetArg(args[n], XmNvalue, 0); n++;
    XtSetArg(args[n], SgNangleRange, 0); n++;
    XtSetArg(args[n], SgNunitsPerRotation, 360); n++;
    XtSetArg(args[n], SgNshowHomeButton, FALSE); n++;
    XtSetArg(args[n], XmNhighlightThickness, 0); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    leftWheel = SgCreateThumbWheel(parent, NULL, args, n);
    
    XtAddCallback(leftWheel, XmNvalueChangedCallback, 
	(XtCallbackProc) SoXtFullViewer::leftWheelCB, (XtPointer) this);
    XtAddCallback(leftWheel, XmNdragCallback, 
	(XtCallbackProc) SoXtFullViewer::leftWheelCB, (XtPointer) this);
    leftWheelVal = 0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Builds the left trim decoration
//
// Use: virtual protected
Widget
SoXtFullViewer::buildLeftTrim(Widget parent)
//
////////////////////////////////////////////////////////////////////////
{
    int		n;
    Arg		args[12];
    
    // create a form to hold all the parts
    Widget form = XtCreateWidget("LeftTrimForm", xmFormWidgetClass, parent, NULL, 0);
    
    // create all the parts
    buildLeftWheel(form);
    Widget butForm = buildAppButtons(form);
    
    //
    // layout
    //
    
    n = 0;
    XtSetArg(args[n], XmNrightAttachment,     	XmNONE); n++;
    XtSetArg(args[n], XmNleftAttachment,  	XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset,  	    	THUMB_SPACE); n++;
    XtSetArg(args[n], XmNbottomAttachment,   	XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, 	XmNONE); n++;
    XtSetValues(leftWheel, args, n);
    
    n = 0;
    XtSetArg(args[n], XmNrightAttachment,     	XmNONE); n++;
    XtSetArg(args[n], XmNleftAttachment,  	XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, 	XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment,   	XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNbottomWidget,   	leftWheel); n++;
    XtSetValues(butForm, args, n);
    
    // manage children
    XtManageChild(leftWheel);
    XtManageChild(butForm);
    
    return form;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Builds the bottom trim decoration
//
// Use: virtual protected
Widget
SoXtFullViewer::buildBottomTrim(Widget parent)
//
////////////////////////////////////////////////////////////////////////
{
    int		n;
    Arg		args[12];
    
    // create a form to hold all the parts
    Widget form = XtCreateWidget("BottomTrimForm", xmFormWidgetClass, parent, NULL, 0);
    
    // create all the parts
    if (rightWheelStr == NULL)
    	rightWheelStr = strdup("Motion Z");
    rightWheelLabel = XtCreateWidget(rightWheelStr, xmLabelGadgetClass, form, NULL, 0);
    if (bottomWheelStr == NULL)
    	bottomWheelStr = strdup("Motion X");
    bottomWheelLabel = XtCreateWidget(bottomWheelStr, xmLabelGadgetClass, form, NULL, 0);
    if (leftWheelStr == NULL)
    	leftWheelStr = strdup("Motion Y");
    leftWheelLabel = XtCreateWidget(leftWheelStr, xmLabelGadgetClass, form, NULL, 0);

    n = 0;
    XtSetArg(args[n], XmNvalue, 0); n++;
    XtSetArg(args[n], SgNangleRange, 0); n++;
    XtSetArg(args[n], SgNunitsPerRotation, 360); n++;
    XtSetArg(args[n], SgNshowHomeButton, FALSE); n++;
    XtSetArg(args[n], XmNhighlightThickness, 0); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    bottomWheel = SgCreateThumbWheel(form, NULL, args, n);
    
    XtAddCallback(bottomWheel, XmNvalueChangedCallback, 
	(XtCallbackProc) SoXtFullViewer::bottomWheelCB, (XtPointer) this);
    XtAddCallback(bottomWheel, XmNdragCallback, 
	(XtCallbackProc) SoXtFullViewer::bottomWheelCB, (XtPointer) this);
    bottomWheelVal = 0;
    
    //
    // layout
    //
    
    // left corner
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,     	XmNONE); n++;
    XtSetArg(args[n], XmNbottomAttachment,  	XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomOffset,     	5); n++;
    XtSetArg(args[n], XmNleftAttachment,   	XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset,   	    	THUMB_SPACE); n++;
    XtSetArg(args[n], XmNrightAttachment, 	XmNONE); n++;
    XtSetValues(leftWheelLabel, args, n);
    
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,     	XmNONE); n++;
    XtSetArg(args[n], XmNbottomAttachment,  	XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomOffset,     	5); n++;
    XtSetArg(args[n], XmNleftAttachment,   	XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget,   	    	leftWheelLabel); n++;
    XtSetArg(args[n], XmNleftOffset,   	    	LABEL_SEPARATION); n++;
    XtSetArg(args[n], XmNrightAttachment, 	XmNONE); n++;
    XtSetValues(bottomWheelLabel, args, n);
    
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,     	XmNONE); n++;
    XtSetArg(args[n], XmNbottomAttachment,  	XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomOffset,  	THUMB_SPACE); n++;
    XtSetArg(args[n], XmNleftAttachment,   	XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget,   	    	bottomWheelLabel); n++;
    XtSetArg(args[n], XmNleftOffset,   	    	LABEL_SPACE); n++;
    XtSetArg(args[n], XmNrightAttachment, 	XmNONE); n++;
    XtSetValues(bottomWheel, args, n);
    
    // right corner
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,  	XmNONE); n++;
    XtSetArg(args[n], XmNbottomAttachment,     	XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomOffset,     	5); n++;
    XtSetArg(args[n], XmNrightAttachment,   	XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset,   	THUMB_SPACE); n++;
    XtSetArg(args[n], XmNleftAttachment, 	XmNONE); n++;
    XtSetValues(rightWheelLabel, args, n);
    
    // manage children (order important)
    XtManageChild(leftWheelLabel);
    XtManageChild(bottomWheelLabel);
    XtManageChild(bottomWheel);
    XtManageChild(rightWheelLabel);
    
    return form;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Builds the right trim decoration
//
// Use: virtual protected
Widget
SoXtFullViewer::buildRightTrim(Widget parent)
//
////////////////////////////////////////////////////////////////////////
{
    int		n;
    Arg		args[12];
    
    // create a form to hold all the parts
    Widget form = XtCreateWidget("RightTrimForm", xmFormWidgetClass, parent, NULL, 0);
    
    // create all the parts
    n = 0;
    XtSetArg(args[n], XmNvalue, 0); n++;
    XtSetArg(args[n], SgNangleRange, 0); n++;
    XtSetArg(args[n], SgNunitsPerRotation, 360); n++;
    XtSetArg(args[n], SgNshowHomeButton, FALSE); n++;
    XtSetArg(args[n], XmNhighlightThickness, 0); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    rightWheel = SgCreateThumbWheel(form, NULL, args, n);
    
    XtAddCallback(rightWheel, XmNvalueChangedCallback, 
	(XtCallbackProc) SoXtFullViewer::rightWheelCB, (XtPointer) this);
    XtAddCallback(rightWheel, XmNdragCallback, 
	(XtCallbackProc) SoXtFullViewer::rightWheelCB, (XtPointer) this);
    rightWheelVal = 0;
    
    Widget buttonForm = buildViewerButtons(form);
    
    //
    // layout
    //
    
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,  	XmNONE); n++;
    XtSetArg(args[n], XmNbottomAttachment, 	XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment,   	XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset,   	THUMB_SPACE); n++;
    XtSetArg(args[n], XmNleftAttachment,     	XmNONE); n++;
    XtSetValues(rightWheel, args, n);
    
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,  	XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment,     	XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNbottomWidget,     	rightWheel); n++;
    XtSetArg(args[n], XmNrightAttachment,   	XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, 	XmNONE); n++;
    XtSetValues(buttonForm, args, n);
    
    // manage children
    XtManageChild(rightWheel);
    XtManageChild(buttonForm);
    
    return form;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Builds the viewer buttons (all within a form)
//
// Use: protected
Widget
SoXtFullViewer::buildViewerButtons(Widget parent)
//
////////////////////////////////////////////////////////////////////////
{
    int i, num;
    Widget form, *list;
    Arg args[12];
    
    // create a form to hold everything
    form = XtCreateWidget(NULL, xmFormWidgetClass, parent, NULL, 0);
    
    createViewerButtons(form);
    
    // get all the button widgets
    num = viewerButtonWidgets->getLength();
    list = new Widget[num];
    for (i=0; i<num; i++)
	list[i] = (Widget) ((*viewerButtonWidgets)[i]);
    
    //
    // layout
    //
    int n = 0;
    XtSetArg(args[n], XmNleftAttachment,     	XmNONE); n++;
    XtSetArg(args[n], XmNrightAttachment,  	XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment,   	XmNONE); n++;
    
    XtSetArg(args[n], XmNtopAttachment, 	XmATTACH_FORM);
    XtSetValues(list[0], args, n+1);
    
    XtSetArg(args[n], XmNtopAttachment, 	XmATTACH_WIDGET); n++;
    for (i=1; i<num; i++) {
	XtSetArg(args[n], XmNtopWidget,   	list[i-1]);
	XtSetValues(list[i], args, n+1);
    }
    
    // manage children
    XtManageChildren(list, num);
    delete [] list;
    
    return form;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	creates the default viewer buttons
//
// Use: virtual protected
void
SoXtFullViewer::createViewerButtons(Widget parent)
//
////////////////////////////////////////////////////////////////////////
{
    // allocate the custom buttons
    for (int i=0; i<PUSH_NUM; i++) {
	buttonList[i] = new SoXtBitmapButton(parent, (i == 0 || i == 1));
	Widget w = buttonList[i]->getWidget();
	XtVaSetValues(w, XmNuserData, this, NULL);
	XtAddCallback(w, XmNactivateCallback,
	    (XtCallbackProc) SoXtFullViewer::pushButtonCB, (XtPointer) (unsigned long) i);
	
	// add this button to the list...
	viewerButtonWidgets->append(w);
    }
    
    // set the button images
    buttonList[PICK_PUSH]->setIcon(so_xt_pick_bits, so_xt_icon_width, so_xt_icon_height);
    buttonList[VIEW_PUSH]->setIcon(so_xt_view_bits, so_xt_icon_width, so_xt_icon_height);
    buttonList[HELP_PUSH]->setIcon(so_xt_help_bits, so_xt_icon_width, so_xt_icon_height);
    buttonList[HOME_PUSH]->setIcon(so_xt_home_bits, so_xt_icon_width, so_xt_icon_height);
    buttonList[SET_HOME_PUSH]->setIcon(so_xt_set_home_bits, so_xt_icon_width, so_xt_icon_height);
    buttonList[VIEW_ALL_PUSH]->setIcon(so_xt_see_all_bits, so_xt_icon_width, so_xt_icon_height);
    buttonList[SEEK_PUSH]->setIcon(so_xt_seek_bits, so_xt_icon_width, so_xt_icon_height);
    
    // show the pick/view state
    if (isViewing())
	buttonList[VIEW_PUSH]->select(TRUE);
    else
	buttonList[PICK_PUSH]->select(TRUE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Builds the app buttons form and any putton the application supplied
//
// Use: protected
Widget
SoXtFullViewer::buildAppButtons(Widget parent)
//
////////////////////////////////////////////////////////////////////////
{
    // create a form to hold the buttons
    appButtonForm = XtCreateWidget("AppButtForm", xmFormWidgetClass, parent, NULL, 0);
    
    // build all the buttons
    if ( appButtonList->getLength() > 0 )
	doAppButtonLayout(0);
    
    return appButtonForm;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Do the app push button build/layout/show withing the button's
//  form widget. Start at the given index
//
// Use: private
void
SoXtFullViewer::doAppButtonLayout(int start)
//
////////////////////////////////////////////////////////////////////////
{
    int	    i, num;
    Arg	    args[12];
    Widget  *widgetList, prevWidget;
    
    num = appButtonList->getLength() - start;
    widgetList = new Widget[num];
    
    // build all the buttons
    for (i=0; i<num; i++)
	widgetList[i] = (Widget) ((*appButtonList)[i+start]);
    
    // unmage any managed widget before the new layout, 
    // starting from the end of the list
    for (i=num-1; i>=0; i--) {
	if ( XtIsManaged(widgetList[i]) )
	    XtUnmanageChild(widgetList[i]);
    }
    
    if (start != 0)
	prevWidget = (Widget) ((*appButtonList)[start-1]);
    
    //
    // layout
    //
    XtSetArg(args[0], XmNrightAttachment,     	XmNONE);
    XtSetArg(args[1], XmNleftAttachment,  	XmATTACH_FORM);
    XtSetArg(args[2], XmNbottomAttachment,   	XmNONE);
    
    for (i=0; i<num; i++) {
	if (i == 0 && start == 0) {
	    XtSetArg(args[3], XmNtopAttachment,	    XmATTACH_FORM);
	    XtSetValues(widgetList[i], args, 4);
	}
	else {
	    XtSetArg(args[3], XmNtopAttachment,	    XmATTACH_WIDGET);
	    if (i == 0)
		XtSetArg(args[4], XmNtopWidget,	    prevWidget);
	    else
		XtSetArg(args[4], XmNtopWidget,	    widgetList[i-1]);
	    XtSetValues(widgetList[i], args, 5);
	}
    }
    
    // manage all children
    XtManageChildren(widgetList, num);
    delete [] widgetList;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	This creates the preference sheet in a separate window. It
//  calls other routines to create the actual content of the sheet.
//
// Use: virtual protected
void
SoXtFullViewer::createPrefSheet()
//
////////////////////////////////////////////////////////////////////////
{
    // create the preference sheet shell and form widget
    Widget shell, form;
    createPrefSheetShellAndForm(shell, form);
    
    // create all of the default parts
    Widget widgetList[10];
    int num = 0;
    createDefaultPrefSheetParts(widgetList, num, form);
    
    layoutPartsAndMapPrefSheet(widgetList, num, form, shell);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Sets the pref sheet string
//
// use: protected
void
SoXtFullViewer::setPrefSheetString(const char *str)
//
////////////////////////////////////////////////////////////////////////
{
    if (prefSheetStr != NULL) free(prefSheetStr);
    prefSheetStr = (str != NULL) ? strdup(str) : NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	This creates the preference sheet outer shell.
//
// Use: protected
void
SoXtFullViewer::createPrefSheetShellAndForm(Widget &shell, Widget &form)
//
////////////////////////////////////////////////////////////////////////
{
    Arg args[12];
    int n;
    
    if (prefSheetStr == NULL)
	prefSheetStr = strdup("Viewer Preference Sheet");
    
    // create a top level shell widget
    n = 0;
    XtSetArg(args[n], XtNtitle, prefSheetStr); n++;
    XtSetArg(args[n], XmNiconName, "Pref Sheet"); n++;
    XtSetArg(args[n], XmNallowShellResize, TRUE); n++;
    prefSheetShellWidget = shell = XtCreatePopupShell("preferenceSheet", 
	topLevelShellWidgetClass, SoXt::getShellWidget(mgrWidget), 
	args, n);
    
    // create a form to hold all the parts
    n = 0;
    XtSetArg(args[n], XmNmarginHeight, 10); n++;
    XtSetArg(args[n], XmNmarginWidth, 10); n++;
    form = XtCreateWidget("", xmFormWidgetClass, shell, args, n);
    
    // register destroy callback to init pref sheet pointers
    XtAddCallback(prefSheetShellWidget, XtNdestroyCallback,
	(XtCallbackProc) SoXtFullViewer::prefSheetDestroyCB,
	(XtPointer) this);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	This simply creates the default parts of the pref sheet.
//
// Use: protected
void
SoXtFullViewer::createDefaultPrefSheetParts(Widget widgetList[], 
				    int &num, Widget form)
//
////////////////////////////////////////////////////////////////////////
{
    widgetList[num++] = createSeekPrefSheetGuts(form);
    widgetList[num++] = createSeekDistPrefSheetGuts(form);
    widgetList[num++] = createZoomPrefSheetGuts(form);
    widgetList[num++] = createClippingPrefSheetGuts(form);
    widgetList[num++] = createStereoPrefSheetGuts(form);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Given a widget list for the preference sheet and it's lenght
//  lay them out one after the other and manage them all. The dialog
//  is them mapped onto the screen.
//
// Use: protected
void
SoXtFullViewer::layoutPartsAndMapPrefSheet(Widget widgetList[], 
			    int num, Widget form, Widget shell)
//
////////////////////////////////////////////////////////////////////////
{
    Arg args[12];
    int n;
    
    // layout
    for (int i=0; i<num; i++) {
	n = 0;
	XtSetArg(args[n], XmNleftAttachment,	    XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNrightAttachment,	    XmATTACH_FORM); n++;
	if (i == 0) {
	    XtSetArg(args[n], XmNtopAttachment,	    XmATTACH_FORM); n++;
	}
	else {
	    XtSetArg(args[n], XmNtopAttachment,	    XmATTACH_WIDGET); n++;
	    XtSetArg(args[n], XmNtopWidget,	    widgetList[i-1]); n++;
	    XtSetArg(args[n], XmNtopOffset,	    10); n++;
	}
	if (i == (num - 1) ) {
	    XtSetArg(args[n], XmNbottomAttachment,  XmATTACH_FORM); n++;
	}
	XtSetValues(widgetList[i], args, n);
    }
    
    XtManageChildren(widgetList, num);
    
    // pop the pref sheet window on the screen
    XtManageChild(form);
    XtRealizeWidget(shell);
    XMapWindow(XtDisplay(shell), XtWindow(shell));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	This creates the seek preference sheet stuff.
//
// Use: protected
Widget
SoXtFullViewer::createSeekPrefSheetGuts(Widget parent)
//
////////////////////////////////////////////////////////////////////////
{
    Widget widgetList[6];
    Arg args[12];
    int n;
    
    // create a form to hold verything together
    Widget form = XtCreateWidget("", xmFormWidgetClass, 
	parent, NULL, 0);
    
    // create the first line
    widgetList[0] = XtCreateWidget( rl.seekAniTime, 
	xmLabelGadgetClass, form, NULL, 0);
    
    n = 0;
    XtSetArg(args[n], XmNhighlightThickness, 1); n++;
    XtSetArg(args[n], XmNcolumns, 5); n++;
    char str[10];
    sprintf(str, "%.2f", getSeekTime());
    XtSetArg(args[n], XmNvalue, str); n++;
    widgetList[1] = XtCreateWidget("", xmTextWidgetClass, 
	form, args, n);
    XtAddCallback(widgetList[1], XmNactivateCallback, 
	(XtCallbackProc) SoXtFullViewer::seekPrefSheetFieldCB, (XtPointer) this);
    
    widgetList[2] = XtCreateWidget("seconds", 
	xmLabelGadgetClass, form, NULL, 0);
    
    // create the second line
    widgetList[3] = XtCreateWidget( rl.seekToLabel, 
	xmLabelGadgetClass, form, NULL, 0);
    
    n = 0;
    XtSetArg(args[n], XmNuserData, this); n++;
    XtSetArg(args[n], XmNindicatorType, XmONE_OF_MANY); n++;
    XtSetArg(args[n], XmNhighlightThickness, 0); n++;
    widgetList[4] = XtCreateWidget("point", 
	xmToggleButtonGadgetClass, form, args, n);
    widgetList[5] = XtCreateWidget("object", 
	xmToggleButtonGadgetClass, form, args, n);
    XmToggleButtonSetState(widgetList[4], isDetailSeek(), FALSE);
    XmToggleButtonSetState(widgetList[5], !isDetailSeek(), FALSE);
    XtAddCallback(widgetList[4], XmNvalueChangedCallback, 
	(XtCallbackProc) SoXtFullViewer::seekPrefSheetToggle1CB,
	(XtPointer) widgetList[5]);
    XtAddCallback(widgetList[5], XmNvalueChangedCallback, 
	(XtCallbackProc) SoXtFullViewer::seekPrefSheetToggle2CB,
	(XtPointer) widgetList[4]);
    
    // layout
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,	    XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopOffset,	    5); n++;
    XtSetValues(widgetList[0], args, n);
    
    n = 0;
    XtSetArg(args[n], XmNleftAttachment,    XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget,   	    widgetList[0]); n++;
    XtSetArg(args[n], XmNleftOffset,   	    10); n++;
    XtSetValues(widgetList[1], args, n);
    
    n = 0;
    XtSetArg(args[n], XmNleftAttachment,    XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget,   	    widgetList[1]); n++;
    XtSetArg(args[n], XmNleftOffset,	    5); n++;
    XtSetArg(args[n], XmNbottomAttachment,  XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNbottomWidget,	    widgetList[0]); n++;
    XtSetValues(widgetList[2], args, n);
    
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,	    XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget,	    widgetList[1]); n++;
    XtSetArg(args[n], XmNtopOffset,	    10); n++;
    XtSetValues(widgetList[3], args, n);
    
    n = 0;
    XtSetArg(args[n], XmNleftAttachment,    XmATTACH_WIDGET); n++;
    XtSetArg(args[1], XmNleftWidget,   	    widgetList[3]); n++;
    XtSetArg(args[n], XmNleftOffset,	    10); n++;
    XtSetArg(args[n], XmNbottomAttachment,  XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNbottomWidget,	    widgetList[3]); n++;
    XtSetArg(args[n], XmNbottomOffset,	    -2); n++;
    XtSetValues(widgetList[4], args, n);
    XtSetArg(args[1], XmNleftWidget,   	    widgetList[4]);
    XtSetValues(widgetList[5], args, n);
    
    XtManageChildren(widgetList, 6);
    
    return form;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	This creates the seek distance setting preference sheet stuff.
//
// Use: protected
Widget
SoXtFullViewer::createSeekDistPrefSheetGuts(Widget parent)
//
////////////////////////////////////////////////////////////////////////
{
    Widget text, thumb, label, toggles[2];
    Arg args[12];
    int n;
    
    // create a form to hold everything together
    Widget form = XtCreateWidget("", xmFormWidgetClass, 
	parent, NULL, 0);
    
    // create the first line
    label = XtCreateWidget( rl.seekDistance, 
	xmLabelGadgetClass, form, NULL, 0);
    
    n = 0;
    XtSetArg(args[n], XmNvalue, 0); n++;
    XtSetArg(args[n], SgNangleRange, 0); n++;
    XtSetArg(args[n], SgNunitsPerRotation, 360); n++;
    XtSetArg(args[n], SgNshowHomeButton, FALSE); n++;
    XtSetArg(args[n], XmNhighlightThickness, 0); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    thumb = SgCreateThumbWheel(form, NULL, args, n);
    
    XtAddCallback(thumb, XmNdragCallback, 
	(XtCallbackProc) SoXtFullViewer::seekDistWheelCB, (XtPointer) this);
    seekDistWheelVal = 0;
    
    n = 0;
    char str[15];
    sprintf(str, "%.3g", seekDistance);
    XtSetArg(args[0], XmNvalue, str); n++;
    XtSetArg(args[n], XmNhighlightThickness, 1); n++;
    XtSetArg(args[n], XmNcolumns, 6); n++;
    seekDistField = text = XtCreateWidget("", xmTextWidgetClass, 
	form, args, n);
    XtAddCallback(text, XmNactivateCallback, 
	(XtCallbackProc) SoXtFullViewer::seekDistFieldCB,
	(XtPointer) this);
    
    // create the second line
    n = 0;
    XtSetArg(args[n], XmNuserData, this); n++;
    XtSetArg(args[n], XmNindicatorType, XmONE_OF_MANY); n++;
    XtSetArg(args[n], XmNhighlightThickness, 0); n++;
    toggles[0] = XtCreateWidget("percentage", 
	xmToggleButtonGadgetClass, form, args, n);
    toggles[1] = XtCreateWidget("absolute", 
	xmToggleButtonGadgetClass, form, args, n);
    
    XmToggleButtonSetState(toggles[0], seekDistAsPercentage, FALSE);
    XmToggleButtonSetState(toggles[1], !seekDistAsPercentage, FALSE);
    XtAddCallback(toggles[0], XmNvalueChangedCallback, 
	(XtCallbackProc) SoXtFullViewer::seekDistPercPrefSheetToggleCB,
	(XtPointer) toggles[1]);
    XtAddCallback(toggles[1], XmNvalueChangedCallback, 
	(XtCallbackProc) SoXtFullViewer::seekDistAbsPrefSheetToggleCB,
	(XtPointer) toggles[0]);
    
    // layout
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,	    XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopOffset,	    5); n++;
    XtSetValues(label, args, n);
    
    n = 0;
    XtSetArg(args[n], XmNleftAttachment,    XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget,   	    label); n++;
    XtSetArg(args[n], XmNleftOffset,   	    5); n++;
    XtSetArg(args[n], XmNbottomAttachment,  XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNbottomWidget,	    label); n++;
    XtSetValues(thumb, args, n);
    
    n = 0;
    XtSetArg(args[n], XmNleftAttachment,    XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget,   	    thumb); n++;
    XtSetArg(args[n], XmNleftOffset,   	    3); n++;
    XtSetArg(args[n], XmNtopAttachment,	    XmATTACH_FORM); n++;
    XtSetValues(text, args, n);
    
    n = 0;
    XtSetArg(args[n], XmNleftAttachment,    XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset,	    30); n++;
    XtSetArg(args[n], XmNtopAttachment,	    XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget,	    text); n++;
    XtSetArg(args[n], XmNtopOffset,	    2); n++;
    XtSetValues(toggles[0], args, n);
    
    n = 0;
    XtSetArg(args[n], XmNleftAttachment,    XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget,	    toggles[0]); n++;
    XtSetArg(args[n], XmNleftOffset,	    10); n++;
    XtSetArg(args[n], XmNbottomAttachment,  XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNbottomWidget,	    toggles[0]); n++;
    XtSetValues(toggles[1], args, n);
    
    // manage children
    XtManageChild(label);
    XtManageChild(thumb);
    XtManageChild(text);
    XtManageChildren(toggles, 2);
    
    return form;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	This creates the zoom slider preference sheet stuff.
//
// Use: protected
Widget
SoXtFullViewer::createZoomPrefSheetGuts(Widget parent)
//
////////////////////////////////////////////////////////////////////////
{
    Arg args[12];
    int n;
    
    // create a form to hold verything together
    Widget form = XtCreateWidget("ZoomForm", xmFormWidgetClass, 
	parent, NULL, 0);
    
    // create all the parts
    zoomWidgets[ZOOM_LABEL] = XtCreateWidget( rl.cameraZoom, 
	xmLabelGadgetClass, form, NULL, 0);
    zoomWidgets[ZOOM_RANGE_LAB1] = XtCreateWidget( rl.zoomSlider, 
	xmLabelGadgetClass, form, NULL, 0);
    zoomWidgets[ZOOM_RANGE_LAB2] = XtCreateWidget( rl.to, 
	xmLabelGadgetClass, form, NULL, 0);
    
    n = 0;
    XtSetArg(args[n], XmNwidth, 130); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNmaximum, 1000); n++;
    XtSetArg(args[n], XmNhighlightThickness, 0); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    zoomWidgets[ZOOM_SLIDER] = XtCreateWidget("ZoomSlider", 
	xmScaleWidgetClass, form, args, n);
    
    XtAddCallback(zoomWidgets[ZOOM_SLIDER], XmNvalueChangedCallback, 
	(XtCallbackProc) &SoXtFullViewer::zoomSliderCB, (XtPointer) this);
    XtAddCallback(zoomWidgets[ZOOM_SLIDER], XmNdragCallback, 
	(XtCallbackProc) &SoXtFullViewer::zoomSliderCB, (XtPointer) this);
    
    n = 0;
    char str[15];
    XtSetArg(args[n], XmNhighlightThickness, 1); n++;
    XtSetArg(args[n], XmNcolumns, 5); n++;
    
    zoomWidgets[ZOOM_FIELD] = XtCreateWidget("ZoomField", 
	xmTextWidgetClass, form, args, n);
    
    sprintf(str, "%.1f", zoomSldRange[0]);
    XtSetArg(args[n], XmNvalue, str);
    zoomWidgets[ZOOM_RANGE_FIELD1] = XtCreateWidget("zoomFrom", 
	xmTextWidgetClass, form, args, n+1);
    
    sprintf(str, "%.1f", zoomSldRange[1]);
    XtSetArg(args[n], XmNvalue, str);
    zoomWidgets[ZOOM_RANGE_FIELD2] = XtCreateWidget("zoomTo", 
	xmTextWidgetClass, form, args, n+1);
    
    XtAddCallback(zoomWidgets[ZOOM_FIELD], XmNactivateCallback, 
	(XtCallbackProc) &SoXtFullViewer::zoomFieldCB, 
	(XtPointer) this);
    XtAddCallback(zoomWidgets[ZOOM_RANGE_FIELD1], XmNactivateCallback, 
	(XtCallbackProc) SoXtFullViewer::zoomPrefSheetMinFieldCB,
	(XtPointer) this);
    XtAddCallback(zoomWidgets[ZOOM_RANGE_FIELD2], XmNactivateCallback, 
	(XtCallbackProc) SoXtFullViewer::zoomPrefSheetMaxFieldCB,
	(XtPointer) this);
    
    // layout
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,     	XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopOffset,     	5); n++;
    XtSetArg(args[n], XmNleftAttachment,  	XmATTACH_FORM); n++;
    XtSetValues(zoomWidgets[ZOOM_LABEL], args, n);
    
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,     	XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopOffset,     	8); n++;
    XtSetArg(args[n], XmNleftAttachment,   	XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget, 	    	zoomWidgets[ZOOM_LABEL]); n++;
    XtSetArg(args[n], XmNleftOffset, 	    	5); n++;
    XtSetValues(zoomWidgets[ZOOM_SLIDER], args, n);
    
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,     	XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment,   	XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget, 		zoomWidgets[ZOOM_SLIDER]); n++;
    XtSetArg(args[n], XmNleftOffset, 		5); n++;
    XtSetValues(zoomWidgets[ZOOM_FIELD], args, n);
    
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,     	XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget,     	zoomWidgets[ZOOM_LABEL]); n++;
    XtSetArg(args[n], XmNtopOffset,     	15); n++;
    XtSetArg(args[n], XmNleftAttachment,  	XmATTACH_FORM); n++;
    XtSetValues(zoomWidgets[ZOOM_RANGE_LAB1], args, n);
    
    n = 0;
    XtSetArg(args[n], XmNleftAttachment,   	XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget,   		zoomWidgets[ZOOM_RANGE_LAB1]); n++;
    XtSetArg(args[n], XmNleftOffset,   		5); n++;
    XtSetArg(args[n], XmNbottomAttachment,   	XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNbottomWidget,   	zoomWidgets[ZOOM_RANGE_LAB1]); n++;
    XtSetArg(args[n], XmNbottomOffset,   	-5); n++;
    XtSetValues(zoomWidgets[ZOOM_RANGE_FIELD1], args, n);
    
    n = 0;
    XtSetArg(args[n], XmNleftAttachment,   	XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget,   		zoomWidgets[ZOOM_RANGE_FIELD1]); n++;
    XtSetArg(args[n], XmNleftOffset,   		5); n++;
    XtSetArg(args[n], XmNbottomAttachment,   	XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNbottomWidget,   	zoomWidgets[ZOOM_RANGE_LAB1]); n++;
    XtSetValues(zoomWidgets[ZOOM_RANGE_LAB2], args, n);
    
    n = 0;
    XtSetArg(args[n], XmNleftAttachment,   	XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget,   		zoomWidgets[ZOOM_RANGE_LAB2]); n++;
    XtSetArg(args[n], XmNleftOffset,   		5); n++;
    XtSetArg(args[n], XmNbottomAttachment,   	XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNbottomWidget,   	zoomWidgets[ZOOM_RANGE_FIELD1]); n++;
    XtSetValues(zoomWidgets[ZOOM_RANGE_FIELD2], args, n);
    
    XtManageChildren(zoomWidgets, ZOOM_NUM);
    
    //
    // finally update the UI
    //
    float zoom = getCameraZoom();
    setZoomSliderPosition(zoom);
    setZoomFieldString(zoom);
    XtSetArg(args[0], XmNsensitive, (camera != NULL && 
	camera->isOfType(SoPerspectiveCamera::getClassTypeId())));
    for (int i=0; i<ZOOM_NUM; i++)
	XtSetValues(zoomWidgets[i], args, 1);
    
    return form;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	This creates the clipping plane preference sheet stuff.
//
// Use: protected
Widget
SoXtFullViewer::createClippingPrefSheetGuts(Widget dialog)
//
////////////////////////////////////////////////////////////////////////
{
    Arg args[12];
    int n;
    
    // create a form to hold everything together
    Widget form = XtCreateWidget("", xmFormWidgetClass, dialog, NULL, 0);
    
    // create all the parts
    n = 0;
    XtSetArg(args[n], XmNuserData, this); n++;
    XtSetArg(args[n], XmNsensitive, camera != NULL); n++;
    XtSetArg(args[n], XmNset, isAutoClipping()); n++;
    XtSetArg(args[n], XmNspacing, 0); n++;
    XtSetArg(args[n], XmNhighlightThickness, 0); n++;
    Widget toggle = XtCreateWidget("", 
	xmToggleButtonGadgetClass, form, args, n);
    n = 0;
    XtSetArg(args[n], XmNsensitive, camera != NULL); n++;
    Widget label = XtCreateWidget("Auto clipping planes", 
	xmLabelGadgetClass, form, args, n);
    XtAddCallback(toggle, XmNvalueChangedCallback, 
	(XtCallbackProc) SoXtFullViewer::clipPrefSheetToggleCB, 
	(XtPointer) form);
    
    if ( !isAutoClipping() && camera)
	SoXtFullViewer::clipPrefSheetToggleCB(toggle, form, NULL);
    
    // layout
    n = 0;
    XtSetArg(args[n], XmNleftAttachment,    XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget,	    toggle); n++;
    XtSetArg(args[n], XmNtopAttachment,	    XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget,	    toggle); n++;
    XtSetArg(args[n], XmNbottomAttachment,  XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNbottomWidget,	    toggle); n++;
    XtSetValues(label, args, n);
    
    // manage children
    XtManageChild(toggle);
    XtManageChild(label);
    
    return form;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	This creates the stereo viewing preference sheet stuff.
//
// Use: protected
Widget
SoXtFullViewer::createStereoPrefSheetGuts(Widget dialog)
//
////////////////////////////////////////////////////////////////////////
{
    Arg args[12];
    int n;
    
    // create a form to hold everything together
    Widget form = XtCreateWidget("", xmFormWidgetClass, dialog, NULL, 0);
    
    // create the toggle
    n = 0;
    XtSetArg(args[n], XmNuserData, this); n++;
    XtSetArg(args[n], XmNset, isStereoViewing()); n++;
    XtSetArg(args[n], XmNspacing, 0); n++;
    XtSetArg(args[n], XmNhighlightThickness, 0); n++;
    Widget toggle = XtCreateWidget("", 
	xmToggleButtonGadgetClass, form, args, n);
    XtAddCallback(toggle, XmNvalueChangedCallback, 
	(XtCallbackProc) SoXtFullViewer::stereoPrefSheetToggleCB, 
	(XtPointer) form);
    
    // toggle text
    stereoLabel = XtCreateWidget("Stereo Viewing", 
	xmLabelGadgetClass, form, NULL, 0);
    
    // layout
    n = 0;
    XtSetArg(args[n], XmNleftAttachment,    XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget,	    toggle); n++;
    XtSetArg(args[n], XmNtopAttachment,	    XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget,	    toggle); n++;
    XtSetArg(args[n], XmNbottomAttachment,  XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNbottomWidget,	    toggle); n++;
    XtSetValues(stereoLabel, args, n);
    
    // manage children
    XtManageChild(toggle);
    XtManageChild(stereoLabel);
    
    // call this routine to bring the additional UI (making it look like
    // the user pressed the toggle).
    stereoWheelForm = NULL;
    if ( isStereoViewing() )
	SoXtFullViewer::stereoPrefSheetToggleCB(toggle, form, NULL);
    
    return form;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Creates the viewer speed pref sheet stuff
//
// Use: protected
Widget
SoXtFullViewer::createSpeedPrefSheetGuts(Widget parent)
//
////////////////////////////////////////////////////////////////////////
{
    Widget widgetList[3];
    Arg args[12];
    int n;
    
    // create a form to hold everything together
    Widget form = XtCreateWidget("", xmFormWidgetClass, 
	parent, NULL, 0);
    
    // create all the parts
    widgetList[0] = XtCreateWidget( rl.viewerSpeed, 
	xmLabelGadgetClass, form, NULL, 0);
    
    n = 0;
    XtSetArg(args[n], XmNhighlightThickness, 0); n++;
    widgetList[1] = XtCreateWidget( rl.increase, xmPushButtonGadgetClass, 
	form, args, n);
    widgetList[2] = XtCreateWidget( rl.decrease, xmPushButtonGadgetClass, 
	form, args, n);
    XtAddCallback(widgetList[1], XmNactivateCallback, 
	(XtCallbackProc) SoXtFullViewer::speedIncPrefSheetButtonCB,
	(XtPointer) this);
    XtAddCallback(widgetList[2], XmNactivateCallback, 
	(XtCallbackProc) SoXtFullViewer::speedDecPrefSheetButtonCB,
	(XtPointer) this);
    
    // layout
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,     	XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment,  	XmATTACH_FORM); n++;
    XtSetValues(widgetList[0], args, n);
    
    n = 0;
    XtSetArg(args[n], XmNleftAttachment,   	XmATTACH_WIDGET); n++;
    XtSetArg(args[1], XmNleftWidget,		widgetList[0]); n++;
    XtSetArg(args[n], XmNleftOffset,   		10); n++;
    XtSetValues(widgetList[1], args, n);
    XtSetArg(args[1], XmNleftWidget,		widgetList[1]);
    XtSetValues(widgetList[2], args, n);
    
    XtManageChildren(widgetList, 3);
    
    return form;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the camera given zoom value (in degree for perspective cameras).
//
// Use: private

void
SoXtFullViewer::setCameraZoom(float zoom)
//
////////////////////////////////////////////////////////////////////////
{
    if (camera == NULL)
	return;
    
    if ( camera->isOfType(SoPerspectiveCamera::getClassTypeId()) )
    	((SoPerspectiveCamera *)camera)->heightAngle = zoom * M_PI / 180.0;
    else if ( camera->isOfType(SoOrthographicCamera::getClassTypeId()) )
    	((SoOrthographicCamera *)camera)->height = zoom;
#if DEBUG
    else
	SoDebugError::post("SoXtFullViewer::setCameraZoom",
		"unknown camera type");
#endif
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Gets the camera current zoom value. The value is returned in degrees
//  for a perspective camera.
//
// Use: private

float
SoXtFullViewer::getCameraZoom()
//
////////////////////////////////////////////////////////////////////////
{
    if (camera == NULL)
	return 0;
    
    if ( camera->isOfType(SoPerspectiveCamera::getClassTypeId()) )
    	return ((SoPerspectiveCamera *)camera)->heightAngle.getValue() * 180.0 / M_PI;
    else if ( camera->isOfType(SoOrthographicCamera::getClassTypeId()) )
    	return ((SoOrthographicCamera *)camera)->height.getValue();
    else {
#if DEBUG
	SoDebugError::post("SoXtFullViewer::getCameraZoom",
			    "unknown camera type");
#endif
	return 0;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the zoom slider position based on the camera values using 
//  the square root for the actual position.
//
// Use: private

void
SoXtFullViewer::setZoomSliderPosition(float zoom)
//
////////////////////////////////////////////////////////////////////////
{
    if (zoomWidgets[ZOOM_SLIDER] == NULL)
	return;
    
    // find the slider position, using a square root distance to make the
    // slider smoother and less sensitive when close to zero.
    float f = (zoom - zoomSldRange[0]) / (zoomSldRange[1] - zoomSldRange[0]);
    f = (f < 0) ? 0 : ((f > 1) ? 1 : f);
    f = sqrtf(f);
    
    // finally position the slider
    Arg args[1];
    int val = int(f * 1000);
    XtSetArg(args[0], XmNvalue, val);
    XtSetValues(zoomWidgets[ZOOM_SLIDER], args, 1);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the zoom field value based on the current camera zoom value.
//
// Use: private

void
SoXtFullViewer::setZoomFieldString(float zoom)
//
////////////////////////////////////////////////////////////////////////
{
    if (zoomWidgets[ZOOM_FIELD] == NULL)
	return;
    
    Arg args[1];
    char str[15];
    sprintf(str, "%.1f", zoom);
    XtSetArg(args[0], XmNvalue, str);
    XtSetValues(zoomWidgets[ZOOM_FIELD], args, 1);
}


//
// Virtual thumb wheels methods which subclasses can redefine
//
void SoXtFullViewer::rightWheelStart()	    { interactiveCountInc(); }
void SoXtFullViewer::bottomWheelStart()	    { interactiveCountInc(); }
void SoXtFullViewer::leftWheelStart()	    { interactiveCountInc(); }
void SoXtFullViewer::rightWheelFinish()	    { interactiveCountDec(); }
void SoXtFullViewer::bottomWheelFinish()    { interactiveCountDec(); }
void SoXtFullViewer::leftWheelFinish()	    { interactiveCountDec(); }

void SoXtFullViewer::rightWheelMotion(float)	{}
void SoXtFullViewer::bottomWheelMotion(float)	{}
void SoXtFullViewer::leftWheelMotion(float)	{}
void SoXtFullViewer::openViewerHelpCard()	{}



//
////////////////////////////////////////////////////////////////////////
// static callbacks stubs
////////////////////////////////////////////////////////////////////////
//

//
// This static variable is used to detect start/finish callbacks
// for the SgThumbwheel and the XmScale widgets.
//
static SbBool firstDrag = TRUE;


// thumb wheel static value changed callbacks
void SoXtFullViewer::rightWheelCB(Widget, SoXtFullViewer *v, XtPointer *d)
{
    SgThumbWheelCallbackStruct *data = (SgThumbWheelCallbackStruct *) d;
    
    if (data->reason == XmCR_DRAG) {
	// for the first move, invoke the start callbacks
	if (firstDrag) {
	    v->rightWheelStart();
	    firstDrag = FALSE;
	}
	
	v->rightWheelMotion( - data->value * M_PI / 180.0);
    }
    else {
	// reason = XmCR_VALUE_CHANGED, invoke the finish callbacks
	v->rightWheelFinish();
	firstDrag = TRUE;
    }
}

void SoXtFullViewer::bottomWheelCB(Widget, SoXtFullViewer *v, XtPointer *d)
{
    SgThumbWheelCallbackStruct *data = (SgThumbWheelCallbackStruct *) d;
    
    if (data->reason == XmCR_DRAG) {
	// for the first move, invoke the start callbacks
	if (firstDrag) {
	    v->bottomWheelStart();
	    firstDrag = FALSE;
	}
	
	v->bottomWheelMotion(data->value * M_PI / 180.0);
    }
    else {
	// reason = XmCR_VALUE_CHANGED, invoke the finish callbacks
	v->bottomWheelFinish();
	firstDrag = TRUE;
    }
}

void SoXtFullViewer::leftWheelCB(Widget, SoXtFullViewer *v, XtPointer *d)
{
    SgThumbWheelCallbackStruct *data = (SgThumbWheelCallbackStruct *) d;
    
    if (data->reason == XmCR_DRAG) {
	// for the first move, invoke the start callbacks
	if (firstDrag) {
	    v->leftWheelStart();
	    firstDrag = FALSE;
	}
	
	v->leftWheelMotion( - data->value * M_PI / 180.0);
    }
    else {
	// reason = XmCR_VALUE_CHANGED, invoke the finish callbacks
	v->leftWheelFinish();
	firstDrag = TRUE;
    }
}



//
// viewer push button callbacks
//
void
SoXtFullViewer::pushButtonCB(Widget w, int id, void *)
{
    SoXtFullViewer *v;
    XtVaGetValues(w, XmNuserData, &v, NULL);
    
    switch (id) {
	case PICK_PUSH:	v->setViewing(FALSE); break;
	case VIEW_PUSH: v->setViewing(TRUE); break;
	case HELP_PUSH:	v->openViewerHelpCard(); break;
	case HOME_PUSH: v->resetToHomePosition(); break;
	case SET_HOME_PUSH: v->saveHomePosition(); break;
	case VIEW_ALL_PUSH: v->viewAll(); break;
	case SEEK_PUSH: v->setSeekMode(! v->isSeekMode()); break;
    }
}


void
SoXtFullViewer::prefSheetDestroyCB(Widget, SoXtFullViewer *v, void *)
{
    // reset our vars when the dialog gets destroyed....
    v->prefSheetShellWidget = NULL;
    for (int i=0; i<ZOOM_NUM; i++)
	v->zoomWidgets[i] = NULL;
}

void
SoXtFullViewer::seekPrefSheetFieldCB(Widget field, SoXtFullViewer *v, void *)
{
    // get text value from the label
    char *str = XmTextGetString(field);
    float val;
    if ( sscanf(str, "%f", &val) ) {
	if (val < 0)
	    val = 0;
	v->setSeekTime(val);
    }
    free(str);
    
    // reformat text field
    char valStr[10];
    sprintf(valStr, "%.2f", v->getSeekTime());
    XmTextSetString(field, valStr);
    
    // make the text field loose the focus
    XmProcessTraversal(XtParent(field), XmTRAVERSE_CURRENT);
}

void
SoXtFullViewer::seekPrefSheetToggle1CB(Widget tog1, Widget tog2, void *)
{
    XmToggleButtonSetState(tog2, !XmToggleButtonGetState(tog1), FALSE);
    
    // get viewer pointer and set seek detail state
    SoXtFullViewer *v;
    Arg	args[1];
    XtSetArg(args[0], XmNuserData, &v);
    XtGetValues(tog1, args, 1);
    v->setDetailSeek( XmToggleButtonGetState(tog1) );
}

void
SoXtFullViewer::seekPrefSheetToggle2CB(Widget tog2, Widget tog1, void *)
{
    XmToggleButtonSetState(tog1, !XmToggleButtonGetState(tog2), FALSE);
    
    // get viewer pointer and set seek detail state
    SoXtFullViewer *v;
    Arg	args[1];
    XtSetArg(args[0], XmNuserData, &v);
    XtGetValues(tog1, args, 1);
    v->setDetailSeek( XmToggleButtonGetState(tog1) );
}

void
SoXtFullViewer::seekDistPercPrefSheetToggleCB(Widget tog1, Widget tog2, void *)
{
    XmToggleButtonSetState(tog2, !XmToggleButtonGetState(tog1), FALSE);
    
    // get viewer pointer and set seek distance state
    SoXtFullViewer *v;
    Arg	args[1];
    XtSetArg(args[0], XmNuserData, &v);
    XtGetValues(tog1, args, 1);
    v->seekDistAsPercentage = XmToggleButtonGetState(tog1);
}

void
SoXtFullViewer::seekDistAbsPrefSheetToggleCB(Widget tog2, Widget tog1, void *)
{
    XmToggleButtonSetState(tog1, !XmToggleButtonGetState(tog2), FALSE);
    
    // get viewer pointer and set seek distance state
    SoXtFullViewer *v;
    Arg	args[1];
    XtSetArg(args[0], XmNuserData, &v);
    XtGetValues(tog1, args, 1);
    v->seekDistAsPercentage = XmToggleButtonGetState(tog1);
}

////////////////////////////////////////////////////////////////////////
//
//  Called whenever the user changes the zoom slider position.
//
//  Use: static private
//
void
SoXtFullViewer::zoomSliderCB(Widget, SoXtFullViewer *v, XtPointer *d)
//
////////////////////////////////////////////////////////////////////////
{
    XmScaleCallbackStruct *data = (XmScaleCallbackStruct *) d;
    
    // for the first move, invoke the start callbacks
    if (data->reason == XmCR_DRAG && firstDrag) {
	v->interactiveCountInc();
	firstDrag = FALSE;
    }
    
    // if the slider is being dragged OR the slider jumps around
    // (user clicked left mouse on the side which causes the slider
    // to animate) update the camera zoom value.
    if (data->reason == XmCR_DRAG || 
	(data->reason == XmCR_VALUE_CHANGED && firstDrag)) {
	
	// get the slider zoom value, taking the square value since we
	// are using the square root to make the slider smoother to use.
	float f = data->value / 1000.0;
	f *= f;
	float zoom = v->zoomSldRange[0] + f * (v->zoomSldRange[1] - v->zoomSldRange[0]);
	
	// now update the camera and text field
	v->setCameraZoom(zoom);
	v->setZoomFieldString(zoom);
    }
    
    // reason = XmCR_VALUE_CHANGED, invoke the finish callbacks
    if (data->reason == XmCR_VALUE_CHANGED && ! firstDrag) {
	v->interactiveCountDec();
	firstDrag = TRUE;
    }
}

////////////////////////////////////////////////////////////////////////
//
//  Called whenever the zoom slider field has a new value typed in.
//
//  Use: static private
//
void
SoXtFullViewer::zoomFieldCB(Widget field, SoXtFullViewer *v, XtPointer *)
//
////////////////////////////////////////////////////////////////////////
{
    // get value from the label
    char *str = XmTextGetString(field);
    float zoom;
    if ( sscanf(str, "%f", &zoom) && zoom > 0) {
	
	// check for valid perspective camera range
	if ( v->camera != NULL && 
	    v->camera->isOfType(SoPerspectiveCamera::getClassTypeId()) ) {
	    zoom = (zoom < 0.01) ? 0.01 : ((zoom > 179.99) ? 179.99 : zoom);
	}
	
	// check if the newly typed value changed the slider range
	if (zoom < v->zoomSldRange[0])
	    v->zoomSldRange[0] = zoom;
	else if (zoom > v->zoomSldRange[1])
	    v->zoomSldRange[1] = zoom;
	
	// update the slider and camera zoom values.
	v->setCameraZoom(zoom);
	v->setZoomSliderPosition(zoom);
    }
    else
    	zoom = v->getCameraZoom();
    free(str);
    
    // always reformat text field
    v->setZoomFieldString(zoom);
    
    // make the text field loose the focus
    XmProcessTraversal(SoXt::getShellWidget(field), XmTRAVERSE_CURRENT);
}

////////////////////////////////////////////////////////////////////////
//
//  This routine opens up the popup menu.
//
//  Use: static private
//
void
SoXtFullViewer::popMenuCallback(Widget, SoXtFullViewer *v, XEvent *event, Boolean *)
//
////////////////////////////////////////////////////////////////////////
{
    Arg args[1];
    int button;
    
    XtSetArg(args[0], XmNwhichButton, &button);
    XtGetValues(v->popupWidget, args,1);
    if (event->xbutton.button == button) {
	XmMenuPosition(v->popupWidget, (XButtonPressedEvent *) event);
	XtManageChild(v->popupWidget);
    }
}

////////////////////////////////////////////////////////////////////////
//
//  Called by Xt when a main menu item is picked.
//
//  Use: static private
//
void
SoXtFullViewer::menuPick(Widget w, int id, XmAnyCallbackStruct *cb)
//
////////////////////////////////////////////////////////////////////////
{
    Time eventTime = cb->event->xbutton.time;
    SoXtFullViewer *v;
    Arg	args[1];
    
    XtSetArg(args[0], XmNuserData, &v);
    XtGetValues(w, args, 1);
    
    switch(id) {
	case HELP: 	v->openViewerHelpCard(); break;
	case VIEW_ALL: 	v->viewAll(); break;
	case SET_HOME: 	v->saveHomePosition(); break;
	case HOME: 	v->resetToHomePosition(); break;
	case SEEK:  	v->setSeekMode(! v->isSeekMode()); break;
	case PREF: 
	    if (v->prefSheetShellWidget == NULL)
		v->createPrefSheet();
	    else
		SoXt::show(v->prefSheetShellWidget);
	    break;
	case HEADLIGHT:	v->setHeadlight(! v->isHeadlight()); break;
	case VIEWING:	v->setViewing(! v->isViewing()); break;
	case DECORATION: v->setDecoration(! v->decorationFlag); break;
	case COPY_VIEW:  v->copyView(eventTime); break;
	case PASTE_VIEW: v->pasteView(eventTime); break;
    }
}

////////////////////////////////////////////////////////////////////////
//
//  Called by Xt when a menu item is picked in the drawStyle menu.
//
//  Use: static private
//
void
SoXtFullViewer::drawStyleMenuPick(Widget w, int id, void *)
//
////////////////////////////////////////////////////////////////////////
{
    SoXtFullViewer *v;
    XtVaGetValues(w, XmNuserData, &v, NULL);
    
    switch(id) {
	case AS_IS:	    v->setDrawStyle(SoXtViewer::STILL, SoXtViewer::VIEW_AS_IS); break;
	case HIDDEN_LINE:   v->setDrawStyle(SoXtViewer::STILL, SoXtViewer::VIEW_HIDDEN_LINE); break;
	case NO_TXT:	    v->setDrawStyle(SoXtViewer::STILL, SoXtViewer::VIEW_NO_TEXTURE); break;
	case LOW_RES:	    v->setDrawStyle(SoXtViewer::STILL, SoXtViewer::VIEW_LOW_COMPLEXITY); break;
	case LINE:	    v->setDrawStyle(SoXtViewer::STILL, SoXtViewer::VIEW_LINE); break;
	case POINT:	    v->setDrawStyle(SoXtViewer::STILL, SoXtViewer::VIEW_POINT); break;
	case BBOX:	    v->setDrawStyle(SoXtViewer::STILL, SoXtViewer::VIEW_BBOX); break;
	
	case MOVE_SAME_AS:  v->setDrawStyle(SoXtViewer::INTERACTIVE, SoXtViewer::VIEW_SAME_AS_STILL); break;
	case MOVE_NO_TXT:   v->setDrawStyle(SoXtViewer::INTERACTIVE, SoXtViewer::VIEW_NO_TEXTURE); break;
	case MOVE_LOW_RES:  v->setDrawStyle(SoXtViewer::INTERACTIVE, SoXtViewer::VIEW_LOW_COMPLEXITY); break;
	case MOVE_LINE:	    v->setDrawStyle(SoXtViewer::INTERACTIVE, SoXtViewer::VIEW_LINE); break;
	case MOVE_LOW_LINE: v->setDrawStyle(SoXtViewer::INTERACTIVE, SoXtViewer::VIEW_LOW_RES_LINE); break;
	case MOVE_POINT:    v->setDrawStyle(SoXtViewer::INTERACTIVE, SoXtViewer::VIEW_POINT); break;
	case MOVE_LOW_POINT: v->setDrawStyle(SoXtViewer::INTERACTIVE, SoXtViewer::VIEW_LOW_RES_POINT); break;
	case MOVE_BBOX:	    v->setDrawStyle(SoXtViewer::INTERACTIVE, SoXtViewer::VIEW_BBOX); break;
    }
}

////////////////////////////////////////////////////////////////////////
//
//  Called by Xt when a menu item in the buffer style menu is picked.
//
//  Use: static private
//
void
SoXtFullViewer::bufferStyleMenuPick(Widget w, int id, void *)
//
////////////////////////////////////////////////////////////////////////
{
    SoXtFullViewer *v;
    XtVaGetValues(w, XmNuserData, &v, NULL);
    
    v->setBufferingType((SoXtViewer::BufferType)id);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Called by the auto clipping preference sheet to toggle auto clipping
//  and show/hide the extra manual set thumbwheels.
//
// Use: static private

void
SoXtFullViewer::clipPrefSheetToggleCB(Widget toggle, Widget parent, void *)
//
////////////////////////////////////////////////////////////////////////
{
    // get the viewer pointer
    SoXtFullViewer *v;
    Arg	args[1];
    XtSetArg(args[0], XmNuserData, &v);
    XtGetValues(toggle, args, 1);
    
    v->setAutoClipping( XmToggleButtonGetState(toggle) );
    
    // check if toggle button is on or off
    if ( v->isAutoClipping() ) {
	XtDestroyWidget( v->clipWheelForm );
    }
    else {
	Widget label[2], thumb[2], text[2];
	Arg args[12];
	int n;
	
	// create a form to hold everything together
	Widget form = XtCreateWidget("", xmFormWidgetClass, 
	    parent, NULL, 0);
	v->clipWheelForm = form;
	
	// create the labels
	label[0] = XtCreateWidget( rl.nearPlane, xmLabelGadgetClass, 
	    form, NULL, 0);
	label[1] = XtCreateWidget( rl.farPlane, xmLabelGadgetClass, 
	    form, NULL, 0);
	
	// allocate the thumbwheels
	n = 0;
	XtSetArg(args[n], XmNvalue, 0); n++;
	XtSetArg(args[n], SgNangleRange, 0); n++;
	XtSetArg(args[n], SgNunitsPerRotation, 360); n++;
	XtSetArg(args[n], SgNshowHomeButton, FALSE); n++;
	XtSetArg(args[n], XmNhighlightThickness, 0); n++;
	XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
	
	thumb[0] = SgCreateThumbWheel(form, NULL, args, n);
	thumb[1] = SgCreateThumbWheel(form, NULL, args, n);
	
	XtAddCallback(thumb[0], XmNvalueChangedCallback, 
	    (XtCallbackProc) SoXtFullViewer::clipNearWheelCB, (XtPointer) v);
	XtAddCallback(thumb[0], XmNdragCallback, 
	    (XtCallbackProc) SoXtFullViewer::clipNearWheelCB, (XtPointer) v);
	XtAddCallback(thumb[1], XmNvalueChangedCallback, 
	    (XtCallbackProc) SoXtFullViewer::clipFarWheelCB, (XtPointer) v);
	XtAddCallback(thumb[1], XmNdragCallback, 
	    (XtCallbackProc) SoXtFullViewer::clipFarWheelCB, (XtPointer) v);
	v->clipNearWheelVal = 0;
	v->clipFarWheelVal = 0;
	
	// allocate the text fields
	n = 0;
	char str[15];
	float val = (v->camera != NULL) ? v->camera->nearDistance.getValue() : 0;
	sprintf(str, "%g", val);
	XtSetArg(args[0], XmNvalue, str); n++;
	XtSetArg(args[n], XmNhighlightThickness, 1); n++;
	XtSetArg(args[n], XmNcolumns, 11); n++;
	v->clipNearField = text[0] = XtCreateWidget("", xmTextWidgetClass, 
	    form, args, n);
	val = (v->camera != NULL) ? v->camera->farDistance.getValue() : 0;
	sprintf(str, "%g", val);
	XtSetArg(args[0], XmNvalue, str);
	v->clipFarField = text[1] = XtCreateWidget("", xmTextWidgetClass, 
	    form, args, n);
	XtAddCallback(text[0], XmNactivateCallback, 
	    (XtCallbackProc) SoXtFullViewer::clipFieldCB,
	    (XtPointer) v);
	XtAddCallback(text[1], XmNactivateCallback, 
	    (XtCallbackProc) SoXtFullViewer::clipFieldCB,
	    (XtPointer) v);
	
	// layout
	n = 0;
	XtSetArg(args[n], XmNleftAttachment,	XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNleftOffset,	20); n++;
	XtSetArg(args[n], XmNtopAttachment,	XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopWidget,		toggle); n++;
	XtSetArg(args[n], XmNtopOffset,		2); n++;
	XtSetValues(form, args, n);
	
	n = 0;
	XtSetArg(args[n], XmNrightAttachment,   XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNtopAttachment,	XmATTACH_FORM); n++;
	XtSetValues(text[0], args, n);
	n = 0;
	XtSetArg(args[n], XmNrightAttachment,   XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNtopAttachment,	XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopWidget,		text[0]); n++;
	XtSetValues(text[1], args, n);
	
	n = 0;
	XtSetArg(args[n], XmNbottomAttachment,  XmATTACH_OPPOSITE_WIDGET); n++;
	XtSetArg(args[1], XmNbottomWidget,	text[0]); n++;
	XtSetArg(args[n], XmNbottomOffset,	3); n++;
	XtSetArg(args[n], XmNrightAttachment,   XmATTACH_WIDGET); n++;
	XtSetArg(args[4], XmNrightWidget,	text[0]); n++;
	XtSetArg(args[n], XmNrightOffset,	3); n++;
	XtSetValues(thumb[0], args, n);
	XtSetArg(args[1], XmNbottomWidget,	text[1]);
	XtSetArg(args[4], XmNrightWidget,	text[1]);
	XtSetValues(thumb[1], args, n);
	
	n = 0;
	XtSetArg(args[n], XmNbottomAttachment,  XmATTACH_OPPOSITE_WIDGET); n++;
	XtSetArg(args[1], XmNbottomWidget,	thumb[0]); n++;
	XtSetArg(args[n], XmNrightAttachment,   XmATTACH_WIDGET); n++;
	XtSetArg(args[3], XmNrightWidget,	thumb[0]); n++;
	XtSetArg(args[n], XmNrightOffset,	5); n++;
	XtSetValues(label[0], args, n);
	XtSetArg(args[1], XmNbottomWidget,	thumb[1]);
	XtSetArg(args[3], XmNrightWidget,	thumb[1]);
	XtSetValues(label[1], args, n);
	
	// manage children
	XtManageChild(form);
	XtManageChildren(text, 2);
	XtManageChildren(thumb, 2);
	XtManageChildren(label, 2);
    }
}

void
SoXtFullViewer::clipNearWheelCB(Widget, SoXtFullViewer *v, XtPointer *d)
{
    if (v->camera == NULL)
	return;
    
    SgThumbWheelCallbackStruct *data = (SgThumbWheelCallbackStruct *) d;
    
    if (data->reason == XmCR_DRAG) {
	// for the first move, invoke the start callbacks
	if (firstDrag) {
	    v->interactiveCountInc();
	    firstDrag = FALSE;
	}
	
	// shorter/grow the near plane distance given the wheel rotation
	float dist = v->camera->nearDistance.getValue();
	dist *= powf(80.0, (data->value - v->clipNearWheelVal) / 360.0);
	v->clipNearWheelVal = data->value;
	
	// change the camera and update the text field
	v->camera->nearDistance = dist;
	char str[15];
	sprintf(str, "%g", dist);
	XmTextSetString(v->clipNearField, str);
    }
    else {
	// reason = XmCR_VALUE_CHANGED, invoke the finish callbacks
	v->interactiveCountDec();
	firstDrag = TRUE;
    }
}

void
SoXtFullViewer::clipFarWheelCB(Widget, SoXtFullViewer *v, XtPointer *d)
{
    if (v->camera == NULL)
	return;
    
    SgThumbWheelCallbackStruct *data = (SgThumbWheelCallbackStruct *) d;
    
    if (data->reason == XmCR_DRAG) {
	// for the first move, invoke the start callbacks
	if (firstDrag) {
	    v->interactiveCountInc();
	    firstDrag = FALSE;
	}
	
	// shorter/grow the near plane distance given the wheel rotation
	float dist = v->camera->farDistance.getValue();
	dist *= powf(80.0, (data->value - v->clipFarWheelVal) / 360.0);
	v->clipFarWheelVal = data->value;
	
	// change the camera and update the text field
	v->camera->farDistance = dist;
	char str[15];
	sprintf(str, "%g", dist);
	XmTextSetString(v->clipFarField, str);
    }
    else {
	// reason = XmCR_VALUE_CHANGED, invoke the finish callbacks
	v->interactiveCountDec();
	firstDrag = TRUE;
    }
}

void
SoXtFullViewer::clipFieldCB(Widget field, SoXtFullViewer *v, void *)
{
    if (v->camera == NULL)
	return;
    
    // get text value from the label and update camera
    char *str = XmTextGetString(field);
    float val;
    if ( sscanf(str, "%f", &val) && 
	    (val > 0 || v->camera->isOfType(SoOrthographicCamera::getClassTypeId()))) {
	if (field == v->clipNearField)
	    v->camera->nearDistance = val;
	else
	    v->camera->farDistance = val;
    }
    else {
	if (field == v->clipNearField)
	    val = v->camera->nearDistance.getValue();
	else
	    val = v->camera->farDistance.getValue();
    }
    free(str);
    
    // reformat text field
    char valStr[15];
    sprintf(valStr, "%g", val);
    XmTextSetString(field, valStr);
    
    // make the text field loose the focus
    XmProcessTraversal(SoXt::getShellWidget(field), XmTRAVERSE_CURRENT);
}

#ifdef __sgi
static void destroyStereoInfoDialogCB(Widget dialog, void *, void *)
{ XtDestroyWidget(dialog); }

// Resources for messages.
typedef struct {
	char *str1;
	char *str2;
	char *str3;
	char *str4;
	char *str5;
	char *str6;
	char *title;
} RES_MSGS;
static RES_MSGS rm;
static char *defmsgs[]={ 
	"Please refer to the setmon man pages to set and restore the",
	"monitor stereo mode.",
	"On RealityEngine, try '/usr/gfx/setmon -n 1025x768_96s'",
	"On Indy/Indigo, try '/usr/gfx/setmon -n STR_TOP' (or STR_BOT, ",
	"depending on which half of the screen the viewer is).",
	"To restore the monitor try '/usr/gfx/setmon -n 72HZ' (or 60HZ).",
	"Stereo Usage Dialog"
};

static void createStereoInfoDialog(Widget shell)
{
    Widget dialog = XmCreateWarningDialog(shell, "Stereo Dialog", NULL, NULL);

    SoXtResource xr(dialog);

    if (!xr.getResource( "fvsimsgs1", "FvMsgs1", rm.str1 ))
	rm.str1 =  defmsgs[0];
    if (!xr.getResource( "fvsimsgs2", "FvMsgs2", rm.str2 ))
	rm.str2 =  defmsgs[1];
    if (!xr.getResource( "fvsimsgs3", "FvMsgs3", rm.str3 ))
	rm.str3 =  defmsgs[2];
    if (!xr.getResource( "fvsimsgs4", "FvMsgs4", rm.str4 ))
	rm.str4 =  defmsgs[3];
    if (!xr.getResource( "fvsimsgs5", "FvMsgs5", rm.str5 ))
	rm.str5 =  defmsgs[4];
    if (!xr.getResource( "fvsimsgs6", "FvMsgs6", rm.str6 ))
	rm.str6 =  defmsgs[5];
    if (!xr.getResource( "fvsititle", "FvTitle", rm.title ))
	rm.title = defmsgs[6];

    Arg args[5];
    XmString 
    xmstr = XmStringCreateLocalized(rm.str1);
    xmstr = XmStringConcat(xmstr, XmStringSeparatorCreate());
    xmstr = XmStringConcat(xmstr, XmStringCreateLocalized(rm.str2));
    xmstr = XmStringConcat(xmstr, XmStringSeparatorCreate());
    xmstr = XmStringConcat(xmstr, XmStringSeparatorCreate());
    xmstr = XmStringConcat(xmstr, XmStringCreateLocalized(rm.str3));
    xmstr = XmStringConcat(xmstr, XmStringSeparatorCreate());
    xmstr = XmStringConcat(xmstr, XmStringSeparatorCreate());
    xmstr = XmStringConcat(xmstr, XmStringCreateLocalized(rm.str4));
    xmstr = XmStringConcat(xmstr, XmStringSeparatorCreate());
    xmstr = XmStringConcat(xmstr, XmStringCreateLocalized(rm.str5));
    xmstr = XmStringConcat(xmstr, XmStringSeparatorCreate());
    xmstr = XmStringConcat(xmstr, XmStringSeparatorCreate());
    xmstr = XmStringConcat(xmstr, XmStringCreateLocalized(rm.str6));
    
    int n = 0;
    XtSetArg(args[n], XmNautoUnmanage, FALSE); n++;
    XtSetArg(args[n], XtNtitle, rm.title); n++;
    XtSetArg(args[n], XmNmessageString, xmstr); n++;
    XtSetValues( dialog, args, n );
    XmStringFree(xmstr);
    
    XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
    XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));
    
    // register callback to destroy (and not just unmap) the dialog
    XtAddCallback(dialog, XmNokCallback, 
	(XtCallbackProc) destroyStereoInfoDialogCB, (XtPointer)NULL);
    
    XtManageChild(dialog);
}
#endif

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Called by the stereo preference sheet to toggle stereo viewing
//  and show/hide the extra offset thumbwheel.
//
// Use: static private

void
SoXtFullViewer::stereoPrefSheetToggleCB(Widget toggle, Widget parent, void *)
//
////////////////////////////////////////////////////////////////////////
{
    // get the viewer pointer
    SoXtFullViewer *v;
    XtVaGetValues(toggle, XmNuserData, &v, NULL);
    
    //
    // checks to make sure stereo viewing can be set, else
    // grey the UI and bring and error message.
    //
    SbBool toggleState = XmToggleButtonGetState(toggle);
    SbBool sameState = (toggleState == v->isStereoViewing());
    if (! sameState)
	v->setStereoViewing(toggleState);
    if (toggleState && ! v->isStereoViewing()) {
	TOGGLE_OFF(toggle);
	XtVaSetValues(toggle, XmNsensitive, FALSE, NULL);
	XtVaSetValues(v->stereoLabel, XmNsensitive, FALSE, NULL);
	SoXt::createSimpleErrorDialog(toggle, rl.stereoErrorTitle, rl.stereoError);
	return;
    }
    
    // show/hide the eye spacing thumbwheel
    if ( ! v->isStereoViewing() ) {
	if (v->stereoWheelForm != NULL) {
	    XtDestroyWidget( v->stereoWheelForm );
	    v->stereoWheelForm = NULL;
	}
    }
    else {
	if (v->stereoWheelForm != NULL)
	    return;
	Widget label, thumb, text;
	Arg args[12];
	int n;
	
	// create a form to hold everything together
	Widget form = XtCreateWidget("Stereo thumb form", xmFormWidgetClass, 
	    parent, NULL, 0);
	v->stereoWheelForm = form;
	
	// create the label
	label = XtCreateWidget( rl.cameraRotation, xmLabelGadgetClass, 
	    form, NULL, 0);
	
	// allocate the thumbwheel
	n = 0;
	XtSetArg(args[n], XmNvalue, 0); n++;
	XtSetArg(args[n], SgNangleRange, 0); n++;
	XtSetArg(args[n], SgNunitsPerRotation, 360); n++;
	XtSetArg(args[n], SgNshowHomeButton, FALSE); n++;
	XtSetArg(args[n], XmNhighlightThickness, 0); n++;
	XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
	thumb = SgCreateThumbWheel(form, NULL, args, n);
	
	XtAddCallback(thumb, XmNvalueChangedCallback, 
	    (XtCallbackProc) SoXtFullViewer::stereoWheelCB, (XtPointer) v);
	XtAddCallback(thumb, XmNdragCallback, 
	    (XtCallbackProc) SoXtFullViewer::stereoWheelCB, (XtPointer) v);
	v->stereoWheelVal = 0;
	
	// allocate the text field
	n = 0;
	char str[15];
	sprintf(str, "%.4f", v->getStereoOffset());
	XtSetArg(args[n], XmNvalue, str); n++;
	XtSetArg(args[n], XmNhighlightThickness, 1); n++;
	XtSetArg(args[n], XmNcolumns, 6); n++;
	v->stereoField = text = XtCreateWidget("", xmTextWidgetClass, 
	    form, args, n);
	XtAddCallback(text, XmNactivateCallback, 
	    (XtCallbackProc) SoXtFullViewer::stereoFieldCB,
	    (XtPointer) v);
	
	// layout
	n = 0;
	XtSetArg(args[n], XmNleftAttachment,	XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNleftOffset,	20); n++;
	XtSetArg(args[n], XmNtopAttachment,	XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopWidget,		toggle); n++;
	XtSetArg(args[n], XmNtopOffset,		2); n++;
	XtSetValues(form, args, n);
	
	n = 0;
	XtSetArg(args[n], XmNrightAttachment,   XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNtopAttachment,	XmATTACH_FORM); n++;
	XtSetValues(text, args, n);
	
	n = 0;
	XtSetArg(args[n], XmNbottomAttachment,  XmATTACH_OPPOSITE_WIDGET); n++;
	XtSetArg(args[n], XmNbottomWidget,	text); n++;
	XtSetArg(args[n], XmNbottomOffset,	3); n++;
	XtSetArg(args[n], XmNrightAttachment,   XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNrightWidget,	text); n++;
	XtSetArg(args[n], XmNrightOffset,	3); n++;
	XtSetValues(thumb, args, n);
	
	n = 0;
	XtSetArg(args[n], XmNbottomAttachment,  XmATTACH_OPPOSITE_WIDGET); n++;
	XtSetArg(args[n], XmNbottomWidget,	thumb); n++;
	XtSetArg(args[n], XmNrightAttachment,   XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNrightWidget,	thumb); n++;
	XtSetArg(args[n], XmNrightOffset,	5); n++;
	XtSetValues(label, args, n);
	
	// manage children
	XtManageChild(form);
	XtManageChild(text);
	XtManageChild(thumb);
	XtManageChild(label);
	
#ifdef __sgi
	// bring a dialog to tell the user to look at setmon to set
	// the monitor to stereo mode
	if (! sameState)
	    createStereoInfoDialog(SoXt::getShellWidget(toggle));
#endif
    }
}

void
SoXtFullViewer::stereoWheelCB(Widget, SoXtFullViewer *v, XtPointer *d)
{
    SgThumbWheelCallbackStruct *data = (SgThumbWheelCallbackStruct *) d;
    
    if (data->reason == XmCR_DRAG) {
	// for the first move, invoke the start callbacks
	if (firstDrag) {
	    v->interactiveCountInc();
	    firstDrag = FALSE;
	}
	
	// shorter/grow the stereo camera offset
	v->setStereoOffset( v->getStereoOffset() * 
	    powf(80.0, (data->value - v->stereoWheelVal) / 360.0) );
	v->stereoWheelVal = data->value;
	
	// update the text field
	char str[15];
	sprintf(str, "%.4f", v->getStereoOffset());
	XmTextSetString(v->stereoField, str);
	
	v->scheduleRedraw();
    }
    else {
	// reason = XmCR_VALUE_CHANGED, invoke the finish callbacks
	v->interactiveCountDec();
	firstDrag = TRUE;
    }
    
}

void
SoXtFullViewer::stereoFieldCB(Widget field, SoXtFullViewer *v, void *)
{
    // get text value from the label and update camera
    char *str = XmTextGetString(field);
    float val;
    if ( sscanf(str, "%f", &val) && val > 0) {
	v->setStereoOffset(val);
	v->scheduleRedraw();
    }
    free(str);
    
    // reformat text field
    char valStr[10];
    sprintf(valStr, "%.4f", v->getStereoOffset());
    XmTextSetString(field, valStr);
    
    // make the text field loose the focus
    XmProcessTraversal(SoXt::getShellWidget(field), XmTRAVERSE_CURRENT);
}

void
SoXtFullViewer::seekDistWheelCB(Widget, SoXtFullViewer *v, XtPointer *d)
{
    SgThumbWheelCallbackStruct *data = (SgThumbWheelCallbackStruct *) d;
    
    // shorter/grow the seek distance given the wheel rotation
    v->seekDistance *= powf(80.0, (data->value - v->seekDistWheelVal) / 360.0);
    v->seekDistWheelVal = data->value;
    
    // update the text field
    char str[15];
    sprintf(str, "%.3g", v->seekDistance);
    XmTextSetString(v->seekDistField, str);
}

void
SoXtFullViewer::seekDistFieldCB(Widget field, SoXtFullViewer *v, void *)
{
    // get text value from the label
    char *str = XmTextGetString(field);
    float val;
    if ( sscanf(str, "%f", &val) && val > 0)
	v->seekDistance = val;
    else
	val = v->seekDistance;
    free(str);
    
    // reformat text field
    char valStr[15];
    sprintf(valStr, "%.3g", val);
    XmTextSetString(field, valStr);
    
    // make the text field loose the focus
    XmProcessTraversal(SoXt::getShellWidget(field), XmTRAVERSE_CURRENT);
}

void
SoXtFullViewer::zoomPrefSheetMinFieldCB(Widget field, SoXtFullViewer *v, void *)
{
    // get text value from the label
    char *str = XmTextGetString(field);
    float val;
    if ( sscanf(str, "%f", &val) && val >= 0) {
	
	// check for valid perspective camera range
	if ( v->camera != NULL && 
	    v->camera->isOfType(SoPerspectiveCamera::getClassTypeId()) ) {
	    val = (val < 0.01) ? 0.01 : ((val > 178.99) ? 178.99 : val);
	}
	
	// finally update the slider to reflect the changes
	v->zoomSldRange[0] = val;
	v->setZoomSliderPosition( v->getCameraZoom() );
    }
    else
	val = v->zoomSldRange[0];
    free(str);
    
    // reformat text field
    char valStr[15];
    sprintf(valStr, "%.1f", val);
    XmTextSetString(field, valStr);
    
    // make the text field loose the focus
    XmProcessTraversal(SoXt::getShellWidget(field), XmTRAVERSE_CURRENT);
}

void
SoXtFullViewer::zoomPrefSheetMaxFieldCB(Widget field, SoXtFullViewer *v, void *)
{
    // get text value from the field
    char *str = XmTextGetString(field);
    float val;
    if ( sscanf(str, "%f", &val) && val >= 0) {
	
	// check for valid perspective camera range
	if ( v->camera != NULL && 
	    v->camera->isOfType(SoPerspectiveCamera::getClassTypeId()) ) {
	    val = (val < 1.01) ? 1.01 : ((val > 179.99) ? 179.99 : val);
	}
	
	// finally update the slider to reflect the changes
	v->zoomSldRange[1] = val;
	v->setZoomSliderPosition( v->getCameraZoom() );
    }
    else
	val = v->zoomSldRange[1];
    free(str);
    
    // reformat text field
    char valStr[15];
    sprintf(valStr, "%.1f", val);
    XmTextSetString(field, valStr);
    
    // make the text field loose the focus
    XmProcessTraversal(SoXt::getShellWidget(field), XmTRAVERSE_CURRENT);
}

void
SoXtFullViewer::speedIncPrefSheetButtonCB(Widget, SoXtFullViewer *p, void *)
{ p->viewerSpeed *= 2.0; }

void
SoXtFullViewer::speedDecPrefSheetButtonCB(Widget, SoXtFullViewer *p, void *)
{ p->viewerSpeed /= 2.0; }

#undef SCREEN
