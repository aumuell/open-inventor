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
//  This demonstrates how to set up and use a Motif pulldown menu.
//
//  There are two menus, File and Edit.
//  The File menu has several items, including a separator line,
//  and keyboard accelerators for the menu items.
//  The Edit menu also has several items, but rather than simple push
//  button items, this menu has a toggle button and three radio buttons.
//
//  A toggle button has an on/off state.
//  A set of radio buttons has the property that only one can be on
//  at a time, the others are off.
//

#include <stdlib.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/CascadeBG.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/Separator.h>
#include <Xm/SeparatoG.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>

#include <Inventor/So.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/SoXtRenderArea.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCone.h>

#define SCREEN(w) XScreenNumberOfScreen(XtScreen(w))

//
//  Macros to set Motif toggle buttons on or off
//
#define TOGGLE_ON(BUTTON) \
    XmToggleButtonSetState((Widget) BUTTON, TRUE, FALSE)
#define TOGGLE_OFF(BUTTON) \
    XmToggleButtonSetState((Widget) BUTTON, FALSE, FALSE)

//
//  Menu item constants - each item in a menu gets a unique id
//
#define k_OPEN_ITEM 	0
#define k_SAVE_ITEM 	1
#define k_PRINT_ITEM 	2
#define k_QUIT_ITEM 	3

#define k_ONOFF_ITEM	0
#define k_ONE_ITEM  	1
#define k_TWO_ITEM  	2
#define k_THREE_ITEM 	3

//
//  The edit menu has radio buttons. Rather than use the radio
//  buttons which Motif supplies, we handle things ourselves.
//
typedef struct EditMenuItems {
    Widget *items;  // all the items in this menu
    int	   which;   // specifies which radio button is on
} EditMenuItems;


////////////////////////////////////////////////////////////////////////
//
//  This is called by Xt when a menu item is picked from the File menu.
//  The first and last parameters are not used by this routine.
//  userData is specified when the menu items are created - we set it
//  to be the unique id of the menu item.
//  
static void
fileMenuPick( 
    Widget, 	    	    // parameter not listed because it's not used
    void *userData, 	    // userData, specified in buildPulldownMenu
    XmAnyCallbackStruct *)  // parameter not listed because it's not used
//
////////////////////////////////////////////////////////////////////////
{
    int	    which = (long) userData;
    
    switch (which) {
	case k_OPEN_ITEM:   printf("Open...\n"); break;
	case k_SAVE_ITEM:   printf("Save...\n"); break;
	case k_PRINT_ITEM:  printf("Print...\n"); break;
	case k_QUIT_ITEM:   printf("Quit\n"); exit(0);
	default:    	    printf("UNKNOWN file menu item!!!\n"); break;
    }
}

////////////////////////////////////////////////////////////////////////
//
//  This is called by Xt when a menu item is picked from the Edit menu.
//  w is the menu item chosen.
//  cb is the callback data supplied by motif; in this case, toggle
//    button data
//  userData is specified when the menu items are created - we set it
//    to be the unique id of the menu item.
//  
static void
editMenuPick( 
    Widget w, 	    	    // menu item chosen
    void *userData, 	    // userData, specified in buildPulldownMenu
    XmAnyCallbackStruct *cb)// callback data
//
////////////////////////////////////////////////////////////////////////
{
    XmToggleButtonCallbackStruct *toggle = (XmToggleButtonCallbackStruct *)cb;
    int which = (long) userData;
    
    // get the user data for this menu item. 
    // we expect it to be an array of widgets for this menu.
    Arg     args[1];
    EditMenuItems *menuItems;
    XtSetArg(args[0], XmNuserData, &menuItems);
    XtGetValues(w, args, 1);

    switch (which) {
	case k_ONOFF_ITEM:  
	    printf("On off - ");
	    if (toggle->set)
	    	 printf("ON\n");
	    else printf("OFF\n");
	    break;
	    
	case k_ONE_ITEM:    
	    printf("One\n");
	    menuItems->which = k_ONE_ITEM;
	    break;
	    
	case k_TWO_ITEM:    
	    printf("Two\n");
	    menuItems->which = k_TWO_ITEM;
	    break;
	    
	case k_THREE_ITEM:  
	    printf("Three\n");
	    menuItems->which = k_THREE_ITEM;
	    break;
	    
	default:
	    printf("UNKNOWN edit menu item!!!\n");
	    break;
    }
}


////////////////////////////////////////////////////////////////////////
//
//  This is called by Xt just before the Edit menu is displayed.
//  It allows us to update the radio button toggles so the currently
//  selected button is toggled on.
//
static void
editMenuDisplay(Widget, void *userData, XtPointer)
//
////////////////////////////////////////////////////////////////////////
{
    EditMenuItems *menuItems = (EditMenuItems *) userData;
    
    // toggle all radio buttons off...
    TOGGLE_OFF(menuItems->items[k_ONE_ITEM]);
    TOGGLE_OFF(menuItems->items[k_TWO_ITEM]);
    TOGGLE_OFF(menuItems->items[k_THREE_ITEM]);
    
    // then turn the current one on...
    TOGGLE_ON(menuItems->items[menuItems->which]);
}


//
// These macros assume that 'pulldown' is the variable for the menu,
// and 'menuPick' is the callback function. They are merely for convenience.
//

// A push item has no on/off state - it merely gets selected
#define PUSH_ITEM(ITEM,NAME,KONST,FUNC) \
    ITEM = XtCreateManagedWidget(NAME, 	\
    	xmPushButtonGadgetClass, pulldown, args, n); 	\
    XtAddCallback(ITEM, XmNactivateCallback,\
	(XtCallbackProc) FUNC,	\
	(XtPointer) KONST)

// A toggle item has on/off state
#define TOGGLE_ITEM(ITEM,NAME,KONST,FUNC) \
    ITEM = XtCreateManagedWidget(NAME, 	\
    	xmToggleButtonGadgetClass, pulldown, args, n); 	\
    XtAddCallback(ITEM, XmNvalueChangedCallback,\
	(XtCallbackProc) FUNC,	\
	(XtPointer) KONST)

// A separator draws a line between menu items
#define SEP_ITEM(NAME) \
    (void) XtCreateManagedWidget("separator", \
    	xmSeparatorGadgetClass, pulldown, NULL, 0)


////////////////////////////////////////////////////////////////////////
//
//  This creates the File menu and all its items.
//  The items in this menu have accelerators (keyboard shortcuts).
//
Widget
buildFileMenu(Widget menubar)
//
////////////////////////////////////////////////////////////////////////
{
    Widget  items[4];
    Widget  pulldown;
    Arg     args[4];
    int	    n;
    

    // Tell motif to create the menu in the popup plane
    Arg popupargs[4];
    int popupn = 0;
    
#ifdef MENUS_IN_POPUP
    SoXt::getPopupArgs(XtDisplay(menubar), SCREEN(menubar),
		       popupargs, &popupn);
#endif

    pulldown = XmCreatePulldownMenu(menubar, "fileMenu", popupargs, popupn);
    
#ifdef MENUS_IN_POPUP
    // register callbacks to load/unload the pulldown colormap when the
    // pulldown menu is posted.
    SoXt::registerColormapLoad(pulldown, SoXt::getShellWidget(menubar));
#endif
    
    // Accelerators are keyboard shortcuts for the menu items
    char *openAccel = "Alt <Key> o";
    char *saveAccel = "Alt <Key> s";
    char *printAccel = "Alt <Key> p";
    char *quitAccel = "Alt <Key> q";
    XmString openAccelText = XmStringCreate("Alt+o", XmSTRING_DEFAULT_CHARSET);
    XmString saveAccelText = XmStringCreate("Alt+s", XmSTRING_DEFAULT_CHARSET);
    XmString printAccelText = XmStringCreate("Alt+p", XmSTRING_DEFAULT_CHARSET);
    XmString quitAccelText = XmStringCreate("Alt+q", XmSTRING_DEFAULT_CHARSET);
    
    // The "..." in "Open..." signifies that choosing this menu item
    // will bring up a dialog. (No dialog comes up in this demo,
    // but one could conceivably bring up a file browser).
    n = 0;
    XtSetArg(args[n], XmNaccelerator, openAccel); n++;
    XtSetArg(args[n], XmNacceleratorText, openAccelText); n++;
    PUSH_ITEM(items[0], "Open...", k_OPEN_ITEM, fileMenuPick);
    
    n = 0;
    XtSetArg(args[n], XmNaccelerator, saveAccel); n++;
    XtSetArg(args[n], XmNacceleratorText, saveAccelText); n++;
    PUSH_ITEM(items[1], "Save...", k_SAVE_ITEM, fileMenuPick);
    
    n = 0;
    XtSetArg(args[n], XmNaccelerator, printAccel); n++;
    XtSetArg(args[n], XmNacceleratorText, printAccelText); n++;
    PUSH_ITEM(items[2], "Print...", k_PRINT_ITEM, fileMenuPick);

    SEP_ITEM("separator");
    
    // Quit has no "..." like the other menu items, because it
    // simply executes a command - it does not bring up a dialog.
    n = 0;
    XtSetArg(args[n], XmNaccelerator, quitAccel); n++;
    XtSetArg(args[n], XmNacceleratorText, quitAccelText); n++;
    PUSH_ITEM(items[3], "Quit",    k_QUIT_ITEM, fileMenuPick);

    XtManageChildren(items, 4);
    
    return pulldown;
}

////////////////////////////////////////////////////////////////////////
//
//  This creates the Edit menu and all its items.
//
Widget
buildEditMenu(Widget menubar)
//
////////////////////////////////////////////////////////////////////////
{
    Widget  pulldown;
    Arg     args[4];
    int	    n;
    
    // items is malloced and passed as userData with the pulldown menu items.
    // this way we can make sure the toggles keep up to date.
    EditMenuItems *menuItems = new EditMenuItems;
    menuItems->items = new Widget[4];
    menuItems->which = k_ONE_ITEM;
    
    // Tell motif to create the menu in the popup plane
    Arg popupargs[4];
    int popupn = 0;
#ifdef MENUS_IN_POPUP
    SoXt::getPopupArgs(XtDisplay(menubar), SCREEN(menubar),
		       popupargs, &popupn);
#endif
    pulldown = XmCreatePulldownMenu(menubar, "editMenu", popupargs, popupn);
    
#ifdef MENUS_IN_POPUP
    // register callbacks to load/unload the pulldown colormap when the
    // pulldown menu is posted.
    SoXt::registerColormapLoad(pulldown, SoXt::getShellWidget(menubar));
#endif
    
    // this callback is invoked just before the menu is displayed.
    // this gives us a chance to update the radio button toggles.
    XtAddCallback(pulldown, XmNmapCallback,
	(XtCallbackProc) editMenuDisplay, (XtPointer) menuItems);
    
    n = 0;
    XtSetArg(args[n], XmNuserData, menuItems); n++;
    TOGGLE_ITEM(menuItems->items[0], "On off", k_ONOFF_ITEM, editMenuPick);
    SEP_ITEM("separator");
    
    // the next three items are radio buttons - only one of the three
    // will be toggled on at any time.
    n = 0;
    XtSetArg(args[n], XmNuserData, menuItems); n++;
    XtSetArg(args[n], XmNindicatorType, XmONE_OF_MANY); n++;
    TOGGLE_ITEM(menuItems->items[1], "One",    k_ONE_ITEM, editMenuPick);
    TOGGLE_ITEM(menuItems->items[2], "Two",    k_TWO_ITEM, editMenuPick);
    TOGGLE_ITEM(menuItems->items[3], "Three",  k_THREE_ITEM, editMenuPick);

    XtManageChildren(menuItems->items, 4);
    
    return pulldown;
}

////////////////////////////////////////////////////////////////////////
//
//  This creates the pulldown menu bar and its menus.
//
Widget
buildPulldownMenu(Widget parent)
//
////////////////////////////////////////////////////////////////////////
{
    Widget  menuButtons[2];
    Widget  pulldown1, pulldown2;
    Arg     args[8];
    int	    n;

    // menu bar
    Widget menubar = XmCreateMenuBar(parent, "menuBar", NULL, 0);
    
    pulldown1 = buildFileMenu(menubar);
    pulldown2 = buildEditMenu(menubar);

    // the text in the menubar for these menus
    n = 0;
    XtSetArg(args[n], XmNsubMenuId, pulldown1); n++;
    menuButtons[0] = XtCreateManagedWidget("File",
	xmCascadeButtonGadgetClass,
	menubar, args, n);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, pulldown2); n++;
    menuButtons[1] = XtCreateManagedWidget("Edit",
	xmCascadeButtonGadgetClass,
	menubar, args, n);

    // manage the menu buttons
    XtManageChildren(menuButtons, 2);

    return menubar;
}

////////////////////////////////////////////////////////////////////////
//
//  This creates the main window contents. In this case, we have a
//  menubar at the top of the window, and a render area filling out
//  the remainder. These widgets are layed out with a Motif form widget.
//
SoXtRenderArea *
buildMainWindow(Widget parent, SoNode *sceneGraph)
//
////////////////////////////////////////////////////////////////////////
{
    Widget  form;
    Widget  menubar;
    SoXtRenderArea *renderArea;
    Arg     args[8];
    int	    n;
    
    // build the widgets
    form = XtCreateWidget("form", xmFormWidgetClass, parent, NULL, 0);
    menubar = buildPulldownMenu(form);
    
    renderArea = new SoXtRenderArea(form);
    renderArea->setSceneGraph(sceneGraph);

    // layout
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,     XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment,    XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment,   XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment,  XmATTACH_NONE); n++;
    XtSetValues(menubar, args, n);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment,     XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget,         menubar); n++;
    XtSetArg(args[n], XmNleftAttachment,    XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment,   XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment,  XmATTACH_FORM); n++;
    XtSetValues(renderArea->getWidget(), args, n);

    // manage the children
    XtManageChild(menubar);
    renderArea->show();

    // manage the form widget
    XtManageChild(form);

    return renderArea;
}

////////////////////////////////////////////////////////////////////////
//
//  main - init Inventor and Xt, set up a scene graph and the main window,
//  display this window, and loop forever...
//
void
main(unsigned int /*argc*/, char *argv[])
//
////////////////////////////////////////////////////////////////////////
{
    Widget	 mainWindow;

    // Initialize Inventor and Xt.
    mainWindow = SoXt::init(argv[0]); 

    if (mainWindow != NULL) {
	// set up a scene graph to display
	SoSeparator 	    *root = new SoSeparator;
	SoPerspectiveCamera *camera = new SoPerspectiveCamera;
	SoDirectionalLight  *light  = new SoDirectionalLight;

    	root->ref();
	root->addChild(camera);
	root->addChild(light);
	root->addChild(new SoCone);
	
	// set up the pulldown menu with a render area
	SoXtRenderArea *ra = buildMainWindow(mainWindow, root);
	
	camera->viewAll(root, ra->getViewportRegion());
	
	SoXt::show(mainWindow);
	SoXt::mainLoop();
    }
}

