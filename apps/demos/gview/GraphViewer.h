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

//  -*- C++ -*-

#include <Inventor/SbTime.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/Xt/viewers/SoXtPlaneViewer.h>

class DisplayGraph;
class GraphIcon;

class SbDict;
class SoBoxHighlightRenderAction;
class SoCallbackList;
class SoCoordinate3;
class SoEvent;
class SoEventCallback;
class SoOrthographicCamera;
class SoPath;
class SoSwitch;
class SoTransform;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: GraphViewer
//
//  Subclass of SoXtPlaneViewer that is used to view an iconic version
//  of an Inventor scene graph. This iconic version is referred to as
//  the display graph.
//
//////////////////////////////////////////////////////////////////////////////

class GraphViewer : public SoXtPlaneViewer {
 public:
    GraphViewer(Widget parent = NULL, const char *name = NULL);
    ~GraphViewer();

    // Redefine this function to set up the display graph rather than
    // the graph itself
    virtual void	setSceneGraph(SoNode *newScene);

    // Updates the display graph based on changes to the current scene
    // graph. Sets up the array of GraphIcons.
    void		update();

    // Selects the icon that corresponds to the given path from the
    // root of the scene graph to another node. The root may appear
    // anywhere in the path (i.e., there may be other nodes above it.
    void		select(SoPath *path);

    // Deselects the current selection
    void		deselectAll();

    // Adds/removes a selection callback, which is invoked immediately
    // after a graph icon is selected. The selection path passed to
    // the callback is from the root of the scene graph to the node
    // represented by the selected graph icon.
    void		addSelectionCallback(SoSelectionPathCB *f,
					     void *userData = NULL);
    void		removeSelectionCallback(SoSelectionPathCB *f,
						void *userData = NULL);

    // Adds/removes a deselection callback, which is invoked immediately
    // after a graph icon is deselected. The selection path passed to
    // the callback is from the root of the scene graph to the node
    // represented by the deselected graph icon.
    void		addDeselectionCallback(SoSelectionPathCB *f,
					       void *userData = NULL);
    void		removeDeselectionCallback(SoSelectionPathCB *f,
						  void *userData = NULL);
    // Redefine this from the base class
    virtual Widget	getWidget() const;

    // Returns the scene graph representing the display graph
    SoNode *		getDisplayGraph() const;

  private:
    SoNode		*userScene;	// Scene we are displaying
    SoGroup		*displayRoot;	// Root of display graph
    SoOrthographicCamera *camera;	// Camera used to view scene
    SoSelection		*selector;	// Selection node
    SoSwitch		*pasteSwitch;	// Root of paste feedback subgraph
    SoTransform		*pasteXform;	// Transform node in paste feedback
    SoCoordinate3	*pasteCoord;	// Coordinates for paste feedback line
    SoEventCallback	*ecb;		// Handles all mouse/kbd events
    DisplayGraph	*displayGraph;	// Display graph
    GraphIcon		*pickedIcon;	// Icon hit by last selection pick
    GraphIcon		*selectedIcon;	// Currently-selected icon
    SoNode		*bufferNode;	// Copied/cut node from scene graph
    SbBool		pasteByRef;	// TRUE if paste is by reference
					// as opposed to copying
    SbBool		isGrabbing;	// TRUE if currently grabbing events
    SbTime		prevTime;	// Time of prev mouse press
					// (to detect double-click)
    SoCallbackList  	*selCBs;	// Selection callbacks
    SoCallbackList  	*deselCBs;	// Deselection callbacks
    Widget		widget;		// Top level widget
    SoBoxHighlightRenderAction *rendAct;// Render action with box highlighting

    // Redefine this to add only full viewer buttons (not the plane
    // viewer stuff)
    virtual void	createViewerButtons(Widget parent);
    
    // Builds all widgets
    Widget		buildWidget(Widget parent);

    // Builds the top-bar menu widget
    Widget		buildMenu(Widget parent);

    // Constructs a display graph from the current array of
    // GraphIcons. Sets the viewer to display it. Takes old display
    // graph which it passes to DisplayGraph::build().
    void		buildGraph(DisplayGraph *oldDisplayGraph);

    // Counts nodes in subgraph rooted by node
    static int		countNodes(const SoNode *root, SbDict *nodeDict);

    // Callback to prune a selected path that extends to a shape in an
    // icon to end at the root of the icon
    static SoPath *	selectionFilterCB(void *data,
					  const SoPickedPoint *pick);

    // Callback to store currently-selected icon and invoke callbacks
    static void		selectionCB(void *data, SoPath *path);

    // Callback to clear currently-selected icon and invoke callbacks
    static void		deselectionCB(void *data, SoPath *path);

    // Callbacks for menu display and menu button press
    static void		menuDisplayCB(Widget, XtPointer, XtPointer);
    static void		menuButtonCB(Widget, XtPointer, XtPointer);

    // Callback for mouse button presses
    static void		mousePressCB(void *data, SoEventCallback *ecb);

    // Returns action to be initiated by mouse press
    int			getMousePressAction(SoEventCallback *ecb);

    // Processes action initiated by menu or key press
    void		processAction(int action, SoEventCallback *ecb);

    // Returns GraphIcon corresponding to current selected icon, or NULL
    GraphIcon *		getSelectedIcon() const
	{ return selectedIcon; }

    // Changes selection based on passed code: 0 = parent, 1 = first
    // child, 2 = sibling to left, 3 = sibling to right. Nothing is
    // done if the change is not valid. Returns update code.
    SbBool		changeSelection(int code);

    // Changes selection to be given icon
    void		select(GraphIcon *icon);

    // Invokes user-supplied selection or deselection callbacks
    void		invokeCallbacks(SbBool selecting);

    // Creates and returns subgraph used for selection pasting feedback
    SoNode *		createPasteFeedback();

    // Returns a path from the root of the user's scene graph down to
    // the node represented by the given icon. The path is ref'ed.
    SoPath *		buildPathToIconNode(GraphIcon *tailIcon);

    // Copies current selection
    void		copy();

    // Cuts/clears current selection
    SbBool		cut(SbBool saveSelection);

    // Begins selection pasting
    void		pasteBegin(SoEventCallback *ecb);

    // Callbacks for mouse motion and button presses during selection pasting
    static void		pasteMoveCB(void *data, SoEventCallback *ecb);
    static void		pasteEndCB(void *data, SoEventCallback *ecb);

    // Moves paste feedback to follow cursor, given event containing position
    void		movePasteFeedback(const SoEvent *event,
					  SbBool doPaste);

    // Resets paste feedback to initial state
    void		resetPasteFeedback();

    // Callback for NodeCreator
    static void		nodeCreationCB(void *data, SoNode *newNode);
};

