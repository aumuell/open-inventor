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

#include <X11/Intrinsic.h>

#include <Xm/CascadeBG.h>
#include <Xm/Form.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/SeparatoG.h>
#include <Xm/ToggleBG.h>

#include <Inventor/SbDict.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/actions/SoBoxHighlightRenderAction.h>
#include <Inventor/events/SoLocation2Event.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/misc/SoCallbackList.h>
#include <Inventor/nodes/SoColorIndex.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoTransform.h>

#include "Actions.h"
#include "DisplayGraph.h"
#include "FieldEditor.h"
#include "GraphIcon.h"
#include "GraphViewer.h"
#include "Menu.h"
#include "MotifHelp.h"
#include "NodeCreator.h"

#define SCREEN(w) XScreenNumberOfScreen(XtScreen(w))

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor.
//

GraphViewer::GraphViewer(Widget parent, const char *name)
		: SoXtPlaneViewer(parent, name, TRUE,
				  SoXtFullViewer::BUILD_ALL,
				  SoXtViewer::BROWSER, FALSE)
//
////////////////////////////////////////////////////////////////////////
{
    // And "seek" should never leave the x-y plane
    setDetailSeek(FALSE);

    userScene	 = NULL;
    displayRoot	 = NULL;
    camera	 = NULL;
    displayGraph = NULL;
    selectedIcon = NULL;
    bufferNode	 = NULL;

    pasteByRef	 = FALSE;
    isGrabbing	 = FALSE;

    // First event will come after this time...
    prevTime.setValue(0, 0);

    selCBs   = new SoCallbackList;
    deselCBs = new SoCallbackList;

    // Build and set the widget
    setBaseWidget(widget = buildWidget(getParentWidget()));

    // Turn off X,Y,Z-axis buttons and persp/ortho button, since they
    // are useless to us. This assumes knowledge that these are the
    // last 4 buttons in the viewerButtonList.
    viewerButtonWidgets->truncate(viewerButtonWidgets->getLength() - 4);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//

GraphViewer::~GraphViewer()
//
////////////////////////////////////////////////////////////////////////
{
    delete displayGraph;

    if (displayRoot != NULL)
	displayRoot->unref();

    if (camera != NULL)
	camera->unref();

    delete selCBs;
    delete deselCBs;
    delete rendAct;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets up iconic view of graph to display in viewer from given
//    scene graph.
//

void
GraphViewer::setSceneGraph(SoNode *newScene)
//
////////////////////////////////////////////////////////////////////////
{
    // Get rid of old scene, if any
    if (displayRoot != NULL)
	displayRoot->unref();
    newScene->ref();
    if (userScene != NULL)
	userScene->unref();
    userScene = newScene;

    // Build new display graph
    update();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Updates the display graph based on changes to the current scene
//    graph. Creates or recreates the DisplayGraph. If it already has
//    a DisplayGraph, it passes it to the constructor so it can reuse
//    as much info as possible.
//

void
GraphViewer::update()
//
////////////////////////////////////////////////////////////////////////
{
    DisplayGraph	*oldDisplayGraph = displayGraph;

    // Make sure that the old selection doesn't hang around
    if (displayGraph != NULL)
	selector->deselectAll();

    displayGraph = new DisplayGraph(userScene);

    // Construct display graph and view it. Try to reuse some info
    // from the old graph
    buildGraph(oldDisplayGraph);

    if (oldDisplayGraph != NULL)
	delete oldDisplayGraph;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Selects the icon that corresponds to the given path from the
//    root of the scene graph to another node. The root may appear
//    anywhere in the path (i.e., there may be other nodes above it.
//

void
GraphViewer::select(SoPath *path)
//
////////////////////////////////////////////////////////////////////////
{
    SoPath	*newPath;

    // If the path starts at the root, use it as is
    if (path->getHead() == userScene) {
	newPath = path;
	newPath->ref();
    }

    // Otherwise, we have to find the root node in the path and
    // construct a new path that starts there
    else {
	int	i;

	// Find root node
	for (i = 1; i < path->getLength(); i++)
	    if (path->getNode(i) == userScene)
		break;

	// Make sure we found it
	if (i >= path->getLength()) {
	    fprintf(stderr, "ERROR in GraphViewer::select():");
	    fprintf(stderr, " path does not contain root of graph\n");
	    return;
	}

	// Construct a new path
	newPath = new SoPath(userScene);
	newPath->ref();

	// Add rest of nodes
	for (i++ ; i < path->getLength(); i++)
	    newPath->append(path->getIndex(i));
    }

    // Find the icon corresponding to the path. On the way, make sure
    // the icon is visible: no closed groups or instances.
    select(displayGraph->findFromScenePath(newPath));

    newPath->unref();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Deselects the current selection.
//

void
GraphViewer::deselectAll()
//
////////////////////////////////////////////////////////////////////////
{
    selector->deselectAll();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds a selection callback, which is invoked immediately
//    after a graph icon is selected. The selection path passed to the
//    callback is from the root of the scene graph to the node
//    represented by the selected graph icon. 
//

void
GraphViewer::addSelectionCallback(SoSelectionPathCB *f, void *userData)
//
////////////////////////////////////////////////////////////////////////
{
    selCBs->addCallback((SoCallbackListCB *) f, userData);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Removes a selection callback. 
//

void
GraphViewer::removeSelectionCallback(SoSelectionPathCB *f, void *userData)
//
////////////////////////////////////////////////////////////////////////
{
    selCBs->removeCallback((SoCallbackListCB *) f, userData);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds a deselection callback, which is invoked immediately
//    after a graph icon is deselected. The deselection path passed to the
//    callback is from the root of the scene graph to the node
//    represented by the deselected graph icon. 
//

void
GraphViewer::addDeselectionCallback(SoSelectionPathCB *f, void *userData)
//
////////////////////////////////////////////////////////////////////////
{
    deselCBs->addCallback((SoCallbackListCB *) f, userData);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Removes a deselection callback. 
//

void
GraphViewer::removeDeselectionCallback(SoSelectionPathCB *f, void *userData)
//
////////////////////////////////////////////////////////////////////////
{
    deselCBs->removeCallback((SoCallbackListCB *) f, userData);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the container widget.
//

Widget
GraphViewer::getWidget() const
//
////////////////////////////////////////////////////////////////////////
{
    return widget;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the scene graph representing the display graph.
//

SoNode *
GraphViewer::getDisplayGraph() const
//
////////////////////////////////////////////////////////////////////////
{
    return displayGraph->getRoot();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Redefine this to add only full viewer buttons (not the plane
//    viewer stuff).
//

void
GraphViewer::createViewerButtons(Widget parent)
//
////////////////////////////////////////////////////////////////////////
{
    SoXtFullViewer::createViewerButtons(parent);
}
    
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Builds and returns top-level editor widget.
//

Widget
GraphViewer::buildWidget(Widget parent)
//
////////////////////////////////////////////////////////////////////////
{
    Widget	topWidget, menu, viewer;
    ARG_VARS(10);

    // Create the top level form widget and register it with a class name
    RESET_ARGS();
    topWidget = XmCreateForm(parent, (char *) getWidgetName(), ARGS);
    registerWidget(topWidget);

    // Create the top-bar menu
    menu = buildMenu(topWidget);

    // Create the viewer widget
    viewer = SoXtPlaneViewer::buildWidget(topWidget);

    // Put 'em all together
    RESET_ARGS();
    ADD_TOP_FORM(0);
    ADD_LEFT_FORM(0);
    ADD_RIGHT_FORM(0);
    XtSetValues(menu, ARGS);

    // Put 'em all together
    RESET_ARGS();
    ADD_TOP_WIDGET(menu, 10);
    ADD_BOTTOM_FORM(0);
    ADD_LEFT_FORM(0);
    ADD_RIGHT_FORM(0);
    XtSetValues(viewer, ARGS);

    XtManageChild(menu);
    XtManageChild(viewer);

    widget = topWidget;

    return widget;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Builds the top-bar menu widget.
//

Widget
GraphViewer::buildMenu(Widget parent)
//
////////////////////////////////////////////////////////////////////////
{
    Widget		menu, pulldown, cascade, but;
    PulldownInfo	*pullInfo;
    ButtonInfo		*butInfo;
    int			p, b;
    ARG_VARS(20);

    RESET_ARGS();
    ADD_TOP_FORM(0);
    ADD_LEFT_FORM(0);
    ADD_RIGHT_FORM(0);
    menu = XmCreateMenuBar(parent, "menuBar", ARGS);
    Widget shell = SoXt::getShellWidget(parent);

    for (p = 0; p < NUM_PULLDOWNS; p++) {

	pullInfo = &pullInfos[p];

	pullInfo->viewer = this;

	// Create a pulldown menu in the pop-up planes
	RESET_ARGS();
	SoXt::getPopupArgs(XtDisplay(menu), SCREEN(menu), args, &argN);
	pulldown = XmCreatePulldownMenu(menu, "pulldown", ARGS);
	XtAddCallback(pulldown, XmNmapCallback,
		      &GraphViewer::menuDisplayCB, (XtPointer) pullInfo);

	// register callbacks to load/unload the pulldown colormap when the
	// pulldown menu is posted.
	SoXt::registerColormapLoad(pulldown, shell);
	
	// Create a cascade button in the pulldown
	RESET_ARGS();
	ADD_ARG(XmNsubMenuId,	pulldown);
	ADD_ARG(XmNlabelString,	STRING(pullInfo->name));
	cascade = XmCreateCascadeButtonGadget(menu, "cascade", ARGS);

	// Add the appropriate buttons
	for (b = 0; b < pullInfo->numButtons; b++) {

	    butInfo = &pullInfo->buttons[b];

	    RESET_ARGS();

	    // Add keyboard accelerator if there is one
	    if (butInfo->accelerator != NULL) {
		ADD_ARG(XmNaccelerator,	    butInfo->accelerator);
		ADD_ARG(XmNacceleratorText, STRING(butInfo->accelDisplay));
	    }

	    switch (butInfo->type) {

	      case SEPARATOR:
		but = XmCreateSeparatorGadget(pulldown, "separator", ARGS);
		break;

	      case PUSH:
		ADD_ARG(XmNlabelString, STRING(butInfo->name));
		but = XmCreatePushButtonGadget(pulldown, butInfo->name, ARGS);
		XtAddCallback(but, XmNactivateCallback,
			      &GraphViewer::menuButtonCB, (XtPointer) butInfo);
		break;

	      case TOGGLE:
		ADD_ARG(XmNlabelString, STRING(butInfo->name));
		but = XmCreateToggleButtonGadget(pulldown, butInfo->name,ARGS);
		XtAddCallback(but, XmNvalueChangedCallback,
			      &GraphViewer::menuButtonCB, (XtPointer) butInfo);
		break;
	    }

	    XtManageChild(but);

	    butInfo->viewer = this;
	    butInfo->widget = but;
	}

	XtManageChild(cascade);
    }

    return menu;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructs a display graph. Sets the viewer to display it.
//

void
GraphViewer::buildGraph(DisplayGraph *oldDisplayGraph)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool	firstTime = (displayRoot == NULL);

    // Create a separator group to be the root of the display graph
    displayRoot = new SoSeparator;
    displayRoot->ref();

    // Create a camera (first time only). Every other time we will
    // reuse the same camera so the view doesn't change.
    if (firstTime) {
	camera = new SoOrthographicCamera;
	camera->ref();	// Keep it around between frames
    }
    displayRoot->addChild(camera);

    // Create a selection node to handle selection
    selector = new SoSelection;
    selector->policy = SoSelection::SINGLE;
    selector->setPickFilterCallback(&GraphViewer::selectionFilterCB, this);
    selector->addSelectionCallback(&GraphViewer::selectionCB, this);
    selector->addDeselectionCallback(&GraphViewer::deselectionCB, this);
    displayRoot->addChild(selector);

    // Add root of icon display graph
    selector->addChild(displayGraph->build(oldDisplayGraph));

    // Add an event callback node to handle mouse presses
    ecb = new SoEventCallback;
    ecb->addEventCallback(SoMouseButtonEvent::getClassTypeId(),
			  &GraphViewer::mousePressCB, this);
    selector->addChild(ecb);

    // Create a render action that will take care of highlighting
    // selected objects, and make sure things get redrawn when the
    // selection changes
    rendAct = new SoBoxHighlightRenderAction;
    setGLRenderAction(rendAct);
    redrawOnSelectionChange(selector);

    SoXtPlaneViewer::setSceneGraph(displayRoot);

    if (firstTime) {
	// Set up and add the selection pasting feedback overlay graph
	// (first time only). Use color 2 to avoid interfering with
	// Inventor logo
	SbColor	pasteColor(1.0, 1.0, 0.0);	// Bright yellow
	setOverlayColorMap(2, 1, &pasteColor);
	setOverlaySceneGraph(createPasteFeedback());

	// View the entire scene (first time only)
	viewAll();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback to prune a selected path that extends to a shape in an
//    icon to end at the root of the icon.
//

SoPath *
GraphViewer::selectionFilterCB(void *userData, const SoPickedPoint *pick)
//
////////////////////////////////////////////////////////////////////////
{
    GraphViewer		*viewer = (GraphViewer *) userData;
    const SoPath	*path = pick->getPath();
    SoNode		*iconRoot;
    int			i;

    // Find root of icon subgraph for selected icon
    viewer->pickedIcon = viewer->displayGraph->find(path);
    iconRoot = viewer->pickedIcon->getIconRoot();

    // Find this node in the selected path
    for (i = path->getLength() - 1; i >= 0; --i)
	if (path->getNode(i) == iconRoot)
	    break;

    // It had better be there
    if (i < 0) {
	fprintf(stderr, "Yipes! Can't find root of selected icon graph!\n");
	return NULL;
    }

    // Return a truncated path
    return path->copy(0, i + 1);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback for when an icon is selected. Call any user-supplied
//    selection callbacks.
//

void
GraphViewer::selectionCB(void *userData, SoPath *)
//
////////////////////////////////////////////////////////////////////////
{
    GraphViewer	*viewer = (GraphViewer *) userData;

    viewer->selectedIcon = viewer->pickedIcon;

    // Call callbacks (if any)
    if (viewer->selCBs->getNumCallbacks() > 0)
	viewer->invokeCallbacks(TRUE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback to clear pointer to selected icon.
//

void
GraphViewer::deselectionCB(void *userData, SoPath *)
//
////////////////////////////////////////////////////////////////////////
{
    GraphViewer	*viewer = (GraphViewer *) userData;

    // Call callbacks (if any)
    if (viewer->deselCBs->getNumCallbacks() > 0)
	viewer->invokeCallbacks(FALSE);

    viewer->selectedIcon = NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback for when a pulldown menu is displayed. The clientData is a
//    pointer to a PulldownInfo structure.
//

void
GraphViewer::menuDisplayCB(Widget, XtPointer clientData, XtPointer)
//
////////////////////////////////////////////////////////////////////////
{
    PulldownInfo	*pullInfo = (PulldownInfo *) clientData;
    GraphViewer		*viewer   = pullInfo->viewer;
    GraphIcon		*icon = viewer->selectedIcon;
    int			index;

    // Most operations are inactive if there's no selected icon
    SbBool		gotSel = (icon != NULL);
    SbBool		onOff;
    ARG_VARS(5);

    // Make sure buttons are grayed out when they don't apply

#define ENABLE(action, firstAct, onOff)					      \
    RESET_ARGS();							      \
    ADD_ARG(XmNsensitive, (onOff));					      \
    XtSetValues(pullInfo->buttons[ACTION_INDEX(action, firstAct)].widget, ARGS)

    switch (pullInfo->actionClass) {

      case GRAPH_CLASS:
	ENABLE(GRAPH_UPDATE,	GRAPH_FIRST, TRUE);
	break;

      case EDIT_CLASS:

	ENABLE(EDIT_CUT,	EDIT_FIRST, gotSel);
	ENABLE(EDIT_COPY,	EDIT_FIRST, gotSel);
	ENABLE(EDIT_DUP,	EDIT_FIRST, gotSel);
	ENABLE(EDIT_DUP_REF,	EDIT_FIRST, gotSel);
	ENABLE(EDIT_DELETE,	EDIT_FIRST, gotSel);

	// And can't paste with no node in the buffer
	onOff = (viewer->bufferNode != NULL);
	ENABLE(EDIT_PASTE,	EDIT_FIRST, onOff);
	ENABLE(EDIT_PASTE_REF,	EDIT_FIRST, onOff);

	break;

      case SELECT_CLASS:
	index = (gotSel ? icon->getChildIndex() : -1);
	onOff = (gotSel && icon->getParent() != NULL);

	ENABLE(SELECT_LEFT_SIB,	 SELECT_FIRST, gotSel && index > 0);
	ENABLE(SELECT_PARENT,    SELECT_FIRST, onOff);
	ENABLE(SELECT_1ST_CHILD, SELECT_FIRST, gotSel && icon->isGroup());

	onOff = (onOff && index < icon->getParent()->getNumChildren() - 1);

	ENABLE(SELECT_RIGHT_SIB, SELECT_FIRST, onOff);

	break;

      case GROUP_CLASS:
	onOff = (gotSel && icon->isGroup());

	ENABLE(GROUP_OPEN,	 GROUP_FIRST, onOff && icon->isClosed());
	ENABLE(GROUP_OPEN_ALL,	 GROUP_FIRST, onOff);
	ENABLE(GROUP_CLOSE,	 GROUP_FIRST, onOff && icon->isOpen());
	ENABLE(GROUP_TOGGLE,	 GROUP_FIRST, onOff);

	break;

      case INST_CLASS:
	onOff = (gotSel && icon->isInstance());

	ENABLE(INST_TOGGLE,	 INST_FIRST, onOff);
	ENABLE(INST_SWAP,	 INST_FIRST, onOff);

	break;

      case NODE_CLASS:
	onOff = (gotSel && FieldEditor::getNumFields(icon->getNode()) > 0);

	ENABLE(NODE_CREATE,	 NODE_FIRST, TRUE);
	ENABLE(NODE_EDIT,	 NODE_FIRST, onOff);

	break;
    }

#undef ENABLE
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback for when a menu button is pressed. The clientData is a
//    pointer to a ButtonInfo structure.
//

void
GraphViewer::menuButtonCB(Widget, XtPointer clientData, XtPointer)
//
////////////////////////////////////////////////////////////////////////
{
    ButtonInfo	*butInfo = (ButtonInfo *) clientData;
    GraphViewer	*viewer  = butInfo->viewer;

    viewer->processAction(butInfo->actionItem, viewer->ecb);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback to handle mouse presses. The GraphViewer is passed in
//    as user data.
//

void
GraphViewer::mousePressCB(void *data, SoEventCallback *ecb)
//
////////////////////////////////////////////////////////////////////////
{
    GraphViewer	*viewer = (GraphViewer *) data;

    // Make sure it's a left-mouse-press
    if (SoMouseButtonEvent::isButtonPressEvent(ecb->getEvent(),
					       SoMouseButtonEvent::BUTTON1))
	viewer->processAction(viewer->getMousePressAction(ecb), ecb);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns action to be initiated by mouse button press.
//

int
GraphViewer::getMousePressAction(SoEventCallback *ecb)
//
////////////////////////////////////////////////////////////////////////
{
    ActionItem	action = NONE;
    SbTime	curTime, timeDiff;
    GraphIcon	*icon;

    // See if this mouse down constitutes a double-click (less than
    // 3/10th of a second between clicks)
    curTime  = ecb->getEvent()->getTime();
    timeDiff = curTime - prevTime;

    if (timeDiff.getValue() < .3) {

	// Find the graph icon corresponding to the selected path
	if (ecb->getPickedPoint() != NULL) {

	    icon = displayGraph->find(ecb->getPickedPoint()->getPath());

	    if (icon == NULL)
		fprintf(stderr, "Whoa! No icon for that node!!!\n");

	    // Toggle state of group icon. Update graph if necessary.
	    else if (icon->isGroup())
		action = GROUP_TOGGLE;

	    // If an instance, show what node this is an instance of
	    else if (icon->isInstance())
		action = INST_TOGGLE;

	    // If a leaf node, show fields
	    else
		action = NODE_EDIT;
	}
    }

    prevTime = curTime;

    return action;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Processes action initiated by event.
//

void
GraphViewer::processAction(int action, SoEventCallback *ecb)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool	iconsChanged  = FALSE;
    SbBool	sceneChanged = FALSE;
    NodeCreator	*nc;

    switch ((ActionItem) action) {

      case GRAPH_UPDATE:
	sceneChanged = TRUE;
	break;

      case EDIT_CUT:
	sceneChanged = cut(TRUE);
	break;

      case EDIT_COPY:
	copy();
	break;

      case EDIT_PASTE:
	pasteByRef = FALSE;
	pasteBegin(ecb);
	break;

      case EDIT_PASTE_REF:
	pasteByRef = TRUE;
	pasteBegin(ecb);
	break;

      case EDIT_DUP:
	copy();
	pasteByRef = FALSE;
	pasteBegin(ecb);
	break;

      case EDIT_DUP_REF:
	copy();
	pasteByRef = TRUE;
	pasteBegin(ecb);
	break;

      case EDIT_DELETE:
	sceneChanged = cut(FALSE);
	break;

      case SELECT_LEFT_SIB:
	iconsChanged = changeSelection(2);
	break;

      case SELECT_RIGHT_SIB:
	iconsChanged = changeSelection(3);
	break;

      case SELECT_PARENT:
	iconsChanged = changeSelection(0);
	break;

      case SELECT_1ST_CHILD:
	iconsChanged = changeSelection(1);
	break;

      case GROUP_OPEN:
	if (selectedIcon != NULL && selectedIcon->isGroup())
	    iconsChanged = selectedIcon->openGroup(FALSE);
	break;

      case GROUP_OPEN_ALL:
	if (selectedIcon != NULL && selectedIcon->isGroup())
	    iconsChanged = selectedIcon->openGroup(TRUE);
	break;

      case GROUP_CLOSE:
	if (selectedIcon != NULL && selectedIcon->isGroup())
	    iconsChanged = selectedIcon->closeGroup();
	break;

      case GROUP_TOGGLE:
	if (selectedIcon != NULL && selectedIcon->isGroup())
	    iconsChanged = selectedIcon->toggleGroup();
	break;

      case INST_TOGGLE:
	if (selectedIcon != NULL && selectedIcon->isInstance())
	    displayGraph->toggleInstance(selectedIcon);
	break;

      case INST_SWAP:
	if (selectedIcon != NULL && selectedIcon->isInstance())
	    displayGraph->swapInstance(selectedIcon);
	break;

      case NODE_CREATE:
	nc = new NodeCreator(getWidget(), &GraphViewer::nodeCreationCB, this);
	// Start grabbing events for the paste that will occur next
	ecb->grabEvents();
	isGrabbing = TRUE;
	break;

      case NODE_EDIT:
	if (selectedIcon != NULL)
	    selectedIcon->showFields();
	break;

      default:
	break;
    }	

    // "SceneChanged" indicates that the scene graph changed and the display
    // graph needs to be rebuilt
    if (sceneChanged)
	update();

    // "IconsChanged" means that the display graph just needs to be updated
    else if (iconsChanged)
	displayGraph->update();

    if (action != NONE)
	ecb->setHandled();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Changes selection based on passed code: 0 = parent, 1 = first
//    child, 2 = sibling to left, 3 = sibling to right. Nothing is
//    done if the change is not valid. Returns TRUE if update is
//    necessary.
//

SbBool
GraphViewer::changeSelection(int code)
//
////////////////////////////////////////////////////////////////////////
{
    GraphIcon	*newIcon = NULL;
    int		i;
    SbBool	update = FALSE;

    // If there is no selection already, nothing to do
    if (selectedIcon == NULL)
	return FALSE;

    // Find next graph icon to select, based on code

    switch (code) {

      case 0:		// Parent
	newIcon = selectedIcon->getParent();
	break;

      case 1:		// First child
	if (selectedIcon->isGroup() && selectedIcon->getNumChildren() > 0) {
	    newIcon = selectedIcon->getChild(0);
	    // Make sure child is visible
	    update = selectedIcon->openGroup(FALSE);
	}
	break;

      case 2:		// Sibling to left
      case 3:		// Sibling to right
	if (selectedIcon->getParent() != NULL) {

	    // Find index of selected icon in parent's list of children
	    for (i = 0; i < selectedIcon->getParent()->getNumChildren(); i++)
		if (selectedIcon->getParent()->getChild(i) == selectedIcon)
		    break;

	    // Move to appropriate sibling, if it exists
	    if (code == 2) {
		if (i > 0)
		    newIcon = selectedIcon->getParent()->getChild(i - 1);
	    }
	    else {
		if (i < selectedIcon->getParent()->getNumChildren() - 1)
		    newIcon = selectedIcon->getParent()->getChild(i + 1);
	    }
	}
	break;
    }

    // If no new icon to find, go away
    if (newIcon == NULL)
	return FALSE;

    select(newIcon);

    return update;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Changes selection to be the given icon.
//

void
GraphViewer::select(GraphIcon *icon)
//
////////////////////////////////////////////////////////////////////////
{
    SoPath	*newSelectionPath;

    pickedIcon = icon;

    // Find path to icon. This path is already ref'ed.
    newSelectionPath = displayGraph->findPathToIcon(selector, icon);

    // Select that new path, if any
    if (newSelectionPath != NULL) {
	selector->deselectAll();
	selector->select(newSelectionPath);
	newSelectionPath->unref();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Invokes user-supplied selection or deselection callbacks.
//

void
GraphViewer::invokeCallbacks(SbBool selecting)
//
////////////////////////////////////////////////////////////////////////
{
    // Build a path from the root of the user's scene graph down to
    // the node represented by the selected icon

    SoPath	*selectionPath = buildPathToIconNode(selectedIcon);

    // Pass path to all callbacks
    if (selecting)
	selCBs->invokeCallbacks(selectionPath);
    else
	deselCBs->invokeCallbacks(selectionPath);

    if (selectionPath != NULL)
	selectionPath->unref();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Creates and returns subgraph used for selection pasting feedback.
//

SoNode *
GraphViewer::createPasteFeedback()
//
////////////////////////////////////////////////////////////////////////
{
    SoLightModel	*lm;
    SoColorIndex	*ci;
    SoDrawStyle		*ds;
    SoLineSet		*lset;
    SoSeparator		*sep;
    SbVec2f		iconSpacing = displayGraph->getIconSpacing();

    pasteSwitch = new SoSwitch;

    sep = new SoSeparator;

    // Make sure the overlay graph uses the same camera as the regular scene
    sep->addChild(camera);

    lm = new SoLightModel;
    lm->model = SoLightModel::BASE_COLOR;

    ci = new SoColorIndex;
    ci->index = 2;

    ds = new SoDrawStyle;
    ds->lineWidth = 3;

    pasteCoord = new SoCoordinate3;

    lset = new SoLineSet;
    lset->numVertices = 4;

    pasteXform  = new SoTransform;
    pasteXform->scaleFactor.setValue(iconSpacing[0] / 3.0,
				     iconSpacing[1] / 3.0,
				     1.0);

    // Make sure all 4 coords are valid (not necessarily useful, though)
    resetPasteFeedback();

    pasteSwitch->addChild(sep);

    sep->addChild(lm);
    sep->addChild(ci);
    sep->addChild(ds);
    sep->addChild(pasteCoord);
    sep->addChild(lset);
    sep->addChild(pasteXform);
    sep->addChild(new SoCube);

    return pasteSwitch;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Builds and returns a path from the root of the user's scene
//    graph down to the node represented by the given icon. The path
//    is ref'ed. 
//

SoPath *
GraphViewer::buildPathToIconNode(GraphIcon *tailIcon)
//
////////////////////////////////////////////////////////////////////////
{
    // We will assume this path is no longer than 64 nodes deep (for now???)
    SoPath	*path = new SoPath(userScene);
    GraphIcon	*icon;
    int		indices[64];
    int		length, i;

    if (tailIcon == NULL)
	return NULL;

    path->ref();

    // Find length of path from selected icon to root icon
    for (icon = tailIcon, length = 1;
	 icon->getParent() != NULL;
	 icon = icon->getParent(), length++)
	;

    // Store child indices in array
    for (i = 0, icon = tailIcon; i < length; i++, icon = icon->getParent())
	indices[length - i - 1] = icon->getChildIndex();

    // Append correct indices to path
    for (i = 1; i < length; i++)
	path->append(indices[i]);

    return path;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Copies current selection.
//

void
GraphViewer::copy()
//
////////////////////////////////////////////////////////////////////////
{
    if (selectedIcon != NULL) {

	if (bufferNode != NULL)
	    bufferNode->unref();

	bufferNode = selectedIcon->getNode();

	bufferNode->ref();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Cuts current selection.
//

SbBool
GraphViewer::cut(SbBool saveSelection)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool	sceneChanged = FALSE;

    if (selectedIcon != NULL) {
	GraphIcon	*cutIcon = selectedIcon;

	deselectAll();

	// Optionally store pointer to node in buffer
	if (saveSelection) {
	    if (bufferNode != NULL)
		bufferNode->unref();
	    bufferNode = cutIcon->getNode();
	    bufferNode->ref();
	}

	// Remove node from parent (if any) - NOTE: you cannot
	// cut the root node (it does the copy, though)
	if (cutIcon->getParent() != NULL) {
	    ((SoGroup *) cutIcon->getParent()->getNode())->
		removeChild(cutIcon->getChildIndex());
	    sceneChanged = TRUE;
	}
    }

    return sceneChanged;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Begins process of selection pasting.
//

void
GraphViewer::pasteBegin(SoEventCallback *ecb)
//
////////////////////////////////////////////////////////////////////////
{

    // Make sure there is something to paste
    if (bufferNode == NULL)
	return;

    // We want to grab all events until we get a mouse press.
    // (This won't work if this was initiated from nodeCreationCB,
    // since there is no action to grab events from. This case is
    // handled before the paste occurs.)
    isGrabbing = TRUE;
    ecb->grabEvents();

    // Set up callback for mouse motion/mouse press
    ecb->removeEventCallback(SoMouseButtonEvent::getClassTypeId(),
			     &GraphViewer::mousePressCB, this);
    ecb->addEventCallback(SoLocation2Event::getClassTypeId(),
			  &GraphViewer::pasteMoveCB, this);
    ecb->addEventCallback(SoMouseButtonEvent::getClassTypeId(),
			  &GraphViewer::pasteEndCB, this);

    // Turn on the paste feedback subgraph
    pasteSwitch->whichChild = SO_SWITCH_ALL;

    // Move the feedback to where the cursor is.
    // The event may be NULL, since the paste may have been initiated
    // from within nodeCreationCB.
    if (ecb->getEvent() != NULL)
	movePasteFeedback(ecb->getEvent(), FALSE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback for mouse motion during a paste operation. Moves paste
//    icon with the cursor, highlighting where the current selection
//    would get pasted. 
//

void
GraphViewer::pasteMoveCB(void *data, SoEventCallback *ecb)
//
////////////////////////////////////////////////////////////////////////
{
    GraphViewer	*viewer = (GraphViewer *) data;

    // Move the feedback to where the cursor is
    viewer->movePasteFeedback(ecb->getEvent(), FALSE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback for mouse press during a paste operation. The selection
//    is pasted.
//

void
GraphViewer::pasteEndCB(void *data, SoEventCallback *ecb)
//
////////////////////////////////////////////////////////////////////////
{
    GraphViewer	*viewer = (GraphViewer *) data;

    // If we are grabbing, make sure that nobody else gets the
    // event. This code (and the isGrabbing variable) are needed
    // because there is no way currently to tell a RenderArea that we
    // want to grab all events. For example, a menu button that
    // initiates a paste starts a grab, but since there is no current
    // action or event in the ecb, the grab on the ecb has no effect.
    // Therefore, the grab is never done and the mouse press is
    // processed by the selection. This is a cheesy way to keep that
    // from happening.							???
    if (viewer->isGrabbing)
	ecb->setHandled();

    // Relinquish grab
    viewer->isGrabbing = FALSE;
    ecb->releaseEvents();

    // Remove our callbacks
    ecb->removeEventCallback(SoLocation2Event::getClassTypeId(),
			     &GraphViewer::pasteMoveCB, viewer);
    ecb->removeEventCallback(SoMouseButtonEvent::getClassTypeId(),
			     &GraphViewer::pasteEndCB, viewer);

    // Restore normal callback for mouse press
    ecb->addEventCallback(SoMouseButtonEvent::getClassTypeId(),
			  &GraphViewer::mousePressCB, viewer);

    // Move the feedback to where the cursor is and paste
    viewer->movePasteFeedback(ecb->getEvent(), TRUE);

    // Turn off feedback
    viewer->pasteSwitch->whichChild = SO_SWITCH_NONE;
    viewer->resetPasteFeedback();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Moves paste feedback to follow cursor, given event containing
//    position. If doPaste is TRUE, it actually performs the paste
//    operation.
//

void
GraphViewer::movePasteFeedback(const SoEvent *event, SbBool doPaste)
//
////////////////////////////////////////////////////////////////////////
{
    SbViewVolume	viewVol;
    SbVec2f		loc, cursorPos, anchorPos, endPos;
    SbLine		line;
    SbBool		gotLocation;
    GraphIcon		*parentIcon;
    int			childIndex;
    float		xFeedback, yFeedback, dy;
    SoCamera		*cam = getCamera();
    const SbViewportRegion	&vpReg = SoXtRenderArea::getViewportRegion();

    viewVol = cam->getViewVolume();
    loc = event->getNormalizedPosition(cam->getViewportBounds(vpReg));

    // Find world space line along view direction through cursor 
    viewVol.projectPointToLine(loc, line);

    // Since we have an orthographic camera, the x and y values of any
    // point on this line are the ones we want
    cursorPos[0] = line.getPosition()[0];
    cursorPos[1] = line.getPosition()[1];

    // Figure out if we are near any of the icons
    gotLocation = displayGraph->getPasteLocation(cursorPos[0], cursorPos[1],
						 parentIcon, childIndex,
						 xFeedback, yFeedback);
    if (gotLocation) {
	anchorPos     = parentIcon->getPosition();
	anchorPos[1] -= parentIcon->getSize()[1];
	endPos[0] = xFeedback;
	endPos[1] = yFeedback;
    }
    else
	anchorPos = endPos = cursorPos;

    // Set coordinates of line. Line goes straight down from parent,
    // then turns (if necessary) to left or right, then goes straight
    // down to child
    if (endPos[1] < anchorPos[1])
	dy = .5 * displayGraph->getIconSpacing()[1];
    else
	dy = 0.0;
    pasteCoord->point.set1Value(0, anchorPos[0], anchorPos[1],      0.0);
    pasteCoord->point.set1Value(1, anchorPos[0], anchorPos[1] - dy, 0.0);
    pasteCoord->point.set1Value(2, endPos[0],    anchorPos[1] - dy, 0.0);
    pasteCoord->point.set1Value(3, endPos[0],    endPos[1],         0.0);

    // Translate cube into position
    pasteXform->translation.setValue(endPos[0], endPos[1], 0.0);

    // Paste if requested
    if (doPaste && gotLocation) {
	SoNode	*nodeToPaste = pasteByRef ? bufferNode : bufferNode->copy();

	((SoGroup *) parentIcon->getNode())->insertChild(nodeToPaste,
							 childIndex);

	// Construct a path from the root of the scene graph to the
	// new node, so we can select it
	SoPath	*selectionPath = buildPathToIconNode(parentIcon);
	selectionPath->append(childIndex);

	// Make sure the display graph is rebuilt, but don't go
	// through the deselection callback
	selector->removeDeselectionCallback(&GraphViewer::deselectionCB, this);
	update();
	selector->addDeselectionCallback(&GraphViewer::deselectionCB, this);

	// Select the new icon
	select(selectionPath);

	selectionPath->unref();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Resets paste feedback to initial state.
//

void
GraphViewer::resetPasteFeedback()
//
////////////////////////////////////////////////////////////////////////
{
    pasteCoord->point.set1Value(0, 0.0, 0.0, 0.0);
    pasteCoord->point.set1Value(1, 0.0, 0.0, 0.0);
    pasteCoord->point.set1Value(2, 0.0, 0.0, 0.0);
    pasteCoord->point.set1Value(3, 0.0, 0.0, 0.0);

    pasteXform->translation.setValue(0.0, 0.0, 0.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback for node creation. The data pointer points to an
//    instance of the GraphViewer. The passed string is NULL if no
//    name was selected.
//

void
GraphViewer::nodeCreationCB(void *data, SoNode *newNode)
//
////////////////////////////////////////////////////////////////////////
{
    GraphViewer	*viewer = (GraphViewer *) data;

    // See if no node was created
    if (newNode == NULL)
	return;

    // Save the node in the buffer
    if (viewer->bufferNode != NULL)
	viewer->bufferNode->unref();
    viewer->bufferNode = newNode;
    viewer->bufferNode->ref();

    // Paste it. The grab has already been performed
    viewer->pasteByRef = TRUE;
    viewer->pasteBegin(viewer->ecb);
}
