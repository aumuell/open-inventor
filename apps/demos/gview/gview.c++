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
// gview - displays a graph view of an Inventor scene graph.
//

#include <stdlib.h>
#include <unistd.h>

#include <X11/Intrinsic.h>
#include <Xm/CascadeBG.h>
#include <Xm/Form.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/SeparatoG.h>

#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/actions/SoBoxHighlightRenderAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/nodes/SoCallback.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoShape.h>

#include "GraphViewer.h"
#include "MotifHelp.h"
#include "../../samples/common/InventorLogo.h"

#define SCREEN(w) XScreenNumberOfScreen(XtScreen(w))

static SoSelection		*selector;
static SoXtExaminerViewer	*examViewer;
static GraphViewer		*graphViewer;

static SoSeparator		*root;

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Parses command line arguments.
//

static void
parseArgs(int argc, char *argv[], SoInput *in)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool ok = TRUE;

    // Check for filename
    if (argc == 2)
	ok = in->openFile(argv[1]);

    // Otherwise, we are reading from stdin. Make sure the user knows this!
    else if (argc == 1) {
	if (isatty(fileno(stdin)))
	    fprintf(stderr, "Reading from standard input.\n\n");
    }

    // Too many args!
    else
	ok = FALSE;

    if (! ok) {
	fprintf(stderr, "Usage: %s [filename]\n", argv[0]);
	fprintf(stderr, "Reads from standard input if no filename is given\n");
	exit(1);
    }
}

// Forward references
static void	graphSelectionCB(void *, SoPath *);
static void	graphDeselectionCB(void *, SoPath *);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Selection callback invoked when an object in the scene graph is
//    selected. The path is in the scene graph. We need to select the
//    appropriate icon in the display graph.
//

static void
sceneSelectionCB(void *, SoPath *path)
//
////////////////////////////////////////////////////////////////////////
{
    // Disable the callbacks so we don't loop
    graphViewer->removeSelectionCallback(graphSelectionCB, NULL);
    graphViewer->removeDeselectionCallback(graphDeselectionCB, NULL);

    graphViewer->select(path);

    // Re-enable the callbacks
    graphViewer->addSelectionCallback(graphSelectionCB, NULL);
    graphViewer->addDeselectionCallback(graphDeselectionCB, NULL);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Deselection callback invoked when an object in the scene graph
//    is deselected.
//

static void
sceneDeselectionCB(void *, SoPath *)
//
////////////////////////////////////////////////////////////////////////
{
    // Disable the callback so we don't loop
    graphViewer->removeDeselectionCallback(graphDeselectionCB, NULL);

    graphViewer->deselectAll();

    // Re-enable the callback
    graphViewer->addDeselectionCallback(graphDeselectionCB, NULL);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Selection callback invoked when an icon in the display graph is
//    selected. The path is in the scene graph. We need to add the
//    SoSelection node at the top of this path and select the result
//    in the scene graph. 
//

static void
graphSelectionCB(void *, SoPath *path)
//
////////////////////////////////////////////////////////////////////////
{
    // Create a copy of the path that starts at the SoSelection node
    SoPath	*newPath = new SoPath(selector);
    int		length = path->getLength(), i;

    newPath->ref();

    // Add the root of the old path
    newPath->append(path->getHead());

    // We want a path that ends in either a shape or a group, since
    // these are the only things that can be highlighted correctly.
    // Truncate the path if it does not end in a shape or group.
    if (! path->getTail()->isOfType(SoGroup::getClassTypeId()) &&
	! path->getTail()->isOfType(SoShape::getClassTypeId()))
	length--;

    // Add all other nodes in the path, named by index
    for (i = 1; i < length; i++)
	newPath->append(path->getIndex(i));

    // Disable the callbacks so we don't loop
    selector->removeSelectionCallback(sceneSelectionCB, NULL);
    selector->removeDeselectionCallback(sceneDeselectionCB, NULL);

    selector->deselectAll();

    // If the resulting path contains no geometry, we can't highlight
    // anything, so don't select it. Test this by computing the
    // bounding box of the path.
    SoGetBoundingBoxAction bba(examViewer->getViewportRegion());
    bba.apply(newPath);
    if (! bba.getBoundingBox().isEmpty())
	selector->select(newPath);

    // Re-enable the callbacks
    selector->addSelectionCallback(sceneSelectionCB,     NULL);
    selector->addDeselectionCallback(sceneDeselectionCB, NULL);

    newPath->unref();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Deselection callback invoked when an icon in the display graph
//    is deselected. The path is in the scene graph.
//

static void
graphDeselectionCB(void *, SoPath *)
//
////////////////////////////////////////////////////////////////////////
{
    // Disable the callback so we don't loop
    selector->removeDeselectionCallback(sceneDeselectionCB, NULL);

    selector->deselectAll();

    // Re-enable the callback
    selector->addDeselectionCallback(sceneDeselectionCB, NULL);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Saves scene graph in file "gview.iv".
//

static void
saveCB(Widget, XtPointer, XtPointer)
//
////////////////////////////////////////////////////////////////////////
{
    SoWriteAction	wa;

    if (! wa.getOutput()->openFile("gview.iv"))
	return;

    wa.apply(root);

    wa.getOutput()->closeFile();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Saves display graph in file "disp.iv".
//

static void
saveDispCB(Widget, XtPointer, XtPointer)
//
////////////////////////////////////////////////////////////////////////
{
    SoWriteAction	wa;

    if (! wa.getOutput()->openFile("disp.iv"))
	return;

    wa.apply(graphViewer->getDisplayGraph());

    wa.getOutput()->closeFile();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Quits program.
//

static void
quitCB(Widget, XtPointer, XtPointer)
//
////////////////////////////////////////////////////////////////////////
{
    exit(0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Builds the top-bar menu widget.
//

static Widget
buildMenu(Widget parent)
//
////////////////////////////////////////////////////////////////////////
{
    Widget	menu, pulldown, cascade, but;
    ARG_VARS(20);

    RESET_ARGS();
    ADD_TOP_FORM(0);
    ADD_LEFT_FORM(0);
    ADD_RIGHT_FORM(0);
    menu = XmCreateMenuBar(parent, "menuBar", ARGS);

    // Create a pulldown menu in the pop-up planes
    RESET_ARGS();
    SoXt::getPopupArgs(XtDisplay(menu), SCREEN(menu), args, &argN);
    pulldown = XmCreatePulldownMenu(menu, "pulldown", ARGS);

    // register callbacks to load/unload the pulldown colormap when the
    // pulldown menu is posted.
    SoXt::registerColormapLoad(pulldown, SoXt::getShellWidget(parent));
    
    // Create a cascade button in the pulldown
    RESET_ARGS();
    ADD_ARG(XmNsubMenuId,	pulldown);
    ADD_ARG(XmNlabelString,	STRING("gview"));
    cascade = XmCreateCascadeButtonGadget(menu, "cascade", ARGS);
    XtManageChild(cascade);

    // Add buttons
    RESET_ARGS();
    ADD_ARG(XmNlabelString, STRING("Save in gview.iv"));
    but = XmCreatePushButtonGadget(pulldown, "Save in gview.iv", ARGS);
    XtAddCallback(but, XmNactivateCallback, saveCB, (XtPointer) NULL);
    XtManageChild(but);

    RESET_ARGS();
    ADD_ARG(XmNlabelString, STRING("Save display graph in disp.iv"));
    but = XmCreatePushButtonGadget(pulldown,
				   "Save display graph in disp.iv", ARGS);
    XtAddCallback(but, XmNactivateCallback, saveDispCB, (XtPointer) NULL);
    XtManageChild(but);

    RESET_ARGS();
    ADD_ARG(XmNlabelString, STRING("Quit"));
    but = XmCreatePushButtonGadget(pulldown, "Quit", ARGS);
    XtAddCallback(but, XmNactivateCallback, quitCB, (XtPointer) NULL);
    XtManageChild(but);

    return menu;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback to set up the logo viewport.
//

static void
logoCB(void *, SoAction *action)
{
    if (action->isOfType(SoGLRenderAction::getClassTypeId()))
        glViewport(0, 0, 80, 80);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets up the Inventor logo in the examiner viewer.
//

static void
setOverlayLogo(SoXtRenderArea *ra)
//
////////////////////////////////////////////////////////////////////////
{
    SoInput in;
    in.setBuffer((void *) ivLogo, ivLogoSize);
    SoSeparator *logo = SoDB::readAll(&in);
    logo->ref();

    // Add a callback node which will set the viewport
    SoCallback *cb = new SoCallback;
    cb->setCallback(logoCB);
    logo->insertChild(cb, 0);

    SbColor col(1, 1, 1);
    ra->setOverlayColorMap(1, 1, &col);
    ra->setOverlaySceneGraph(logo);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Mainline.
//

int
main(int argc, char **argv)
//
////////////////////////////////////////////////////////////////////////
{
    SoInput		in;
    Widget		formWidget, menu, examWidget, graphWidget, sepWidget;
    ARG_VARS(10);

    Widget appWindow = SoXt::init(argv[0]);
    if (appWindow == NULL)
	exit(1);

    parseArgs(argc, argv, &in);

    root = SoDB::readAll(&in);

    if (root == NULL) {
	fprintf(stderr, "Bad data. Bye!\n");
	exit(1);
    }

    in.closeFile();

    root->ref();

    // Create a selection node to handle selection
    selector = new SoSelection;
    selector->policy = SoSelection::SINGLE;
    selector->addSelectionCallback(sceneSelectionCB, NULL);
    selector->addDeselectionCallback(sceneDeselectionCB, NULL);

    // Add root of icon display graph
    selector->addChild(root);

    // Create an examiner viewer and a graph viewer. Lay them out side
    // by side in a form widget, with a separator between them and the
    // menu at the top
    formWidget = XmCreateForm(appWindow, "form", NULL, 0);

    // Create the top-bar menu
    menu = buildMenu(formWidget);

    examViewer = new SoXtExaminerViewer(formWidget);
    examViewer->setSceneGraph(selector);
    examViewer->setGLRenderAction(new SoBoxHighlightRenderAction);
    examViewer->redrawOnSelectionChange(selector);
    setOverlayLogo(examViewer);
    examWidget = examViewer->getWidget();

    graphViewer = new GraphViewer(formWidget);
    graphViewer->setSceneGraph(root);
    graphViewer->addSelectionCallback(graphSelectionCB,     NULL);
    graphViewer->addDeselectionCallback(graphDeselectionCB, NULL);
    graphWidget = graphViewer->getWidget();

    sepWidget = XmCreateSeparatorGadget(formWidget, "separator", NULL, 0);

    RESET_ARGS();
    ADD_TOP_FORM(0);
    ADD_LEFT_FORM(0);
    ADD_RIGHT_FORM(0);
    XtSetValues(menu, ARGS);

    RESET_ARGS();
    ADD_TOP_WIDGET(menu, 6);
    ADD_BOTTOM_FORM(0);
    ADD_LEFT_FORM(0);
    ADD_ARG(XmNrightAttachment,	XmATTACH_POSITION);
    ADD_ARG(XmNrightPosition,	50);
    XtSetValues(examWidget, ARGS);

    RESET_ARGS();
    ADD_ARG(XmNorientation,	XmVERTICAL);
    ADD_TOP_WIDGET(menu, 6);
    ADD_BOTTOM_FORM(0);
    ADD_LEFT_WIDGET(examWidget, 10);
    XtSetValues(sepWidget, ARGS);

    RESET_ARGS();
    ADD_TOP_WIDGET(menu, 6);
    ADD_BOTTOM_FORM(0);
    ADD_LEFT_WIDGET(sepWidget, 10);
    ADD_RIGHT_FORM(0);
    XtSetValues(graphWidget, ARGS);

    // Show the widgets
    XtManageChild(formWidget);
    XtManageChild(menu);
    XtManageChild(sepWidget);
    examViewer->show();
    graphViewer->show();

    SoXt::show(appWindow);
    SoXt::mainLoop();

    exit(0);
}
