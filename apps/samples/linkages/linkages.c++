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
//  This sample program reads in a inventor data file, and
//  uses a examiner viewer to view the data.
//
//  It initializes special classes designed for creating linkages.
//
//  See the specific viewer header file for directions on how to 
//  use the viewer.
//
//    Paul Isaacs
//
//    $Revision: 1.2 $
//

#include <stdlib.h>
#include <Inventor/SoDB.h>	// for file reading
#include <Inventor/SoInput.h>	// for file reading
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/Xt/SoXt.h>	// For SoXt::init()
#include <Inventor/nodes/SoSeparator.h>

#include <Xm/PushBG.h>


// list of viewers
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>

// New classes for linkages.
#include "LinkEngines.h"
#include "LinkClasses.h"

void usage(char *name)
{
    printf("Usage:  %s [-m] [file.iv]\n", name);
    printf("  -m  : include menus for adding new linkage nodes\n");
    exit(0);
}

//
// This just reads in the file and doesn't hold on to what it reads.
// Anything with a DEF label will get installed in the global dictionary
// and saved as a resource, however
//
void
readResources()
{
    SoInput	in;

    if ( in.openFile( "linkagesResourceFile.iv", TRUE )) {

	SoSeparator *root = SoDB::readAll( &in );
	if (root == NULL) {
	    SoDebugError::postWarning("main::readResources",
	    "Result of reading resources is NULL");
	}
	else
	    root->ref();

    }
    else {
	SoDebugError::postWarning("main::readResources",
	    "Can't read resources since the file linkageResourceFile.iv was not found");
    }
}

static void newLinkCB( void *data, SoDragger * )
{
    SoSeparator *scene = (SoSeparator *) data;
    scene->addChild( new Link );
}

static void newCrankCB( void *data, SoDragger * )
{
    SoSeparator *scene = (SoSeparator *) data;
    scene->addChild( new Crank );
}

static void newHingeCB( void *data, SoDragger * )
{
    SoSeparator *scene = (SoSeparator *) data;
    scene->addChild( new RivetHinge );
}


static void newDoubleLinkCB( void *data, SoDragger * )
{
    SoSeparator *scene = (SoSeparator *) data;
    scene->addChild( new DoubleLink );
}

static void newPistonCB( void *data, SoDragger * )
{
    SoSeparator *scene = (SoSeparator *) data;
    scene->addChild( new Piston );
}

void
addMenuButtonCallbacks( SoSeparator *scene )
{
    Button *myButton;

    //  new link button
    myButton = (Button *) SoNode::getByName("NEW_LINK_BUTTON");
    if (myButton)
	myButton->addFinishCallback( &newLinkCB, scene );

    //  new crank button
    myButton = (Button *) SoNode::getByName("NEW_CRANK_BUTTON");
    if (myButton)
	myButton->addFinishCallback( &newCrankCB, scene );

    //  new hinge button
    myButton = (Button *) SoNode::getByName("NEW_HINGE_BUTTON");
    if (myButton)
	myButton->addFinishCallback( &newHingeCB, scene );

    //  new double link button
    myButton = (Button *) SoNode::getByName("NEW_DOUBLE_LINK_BUTTON");
    if (myButton)
	myButton->addFinishCallback( &newDoubleLinkCB, scene );

    //  new piston button
    myButton = (Button *) SoNode::getByName("NEW_PISTON_BUTTON");
    if (myButton)
	myButton->addFinishCallback( &newPistonCB, scene );
}

static void
writeButtonCB(Widget, XtPointer data, XtPointer )
{
    SoSeparator *scene = (SoSeparator *) data;

    SoWriteAction wa;
    wa.apply( scene );
}

// Make a push button for writing out what's on screen...
Widget
makeWriteButton( Widget parentWidget, SoSeparator *scene )
{
    int n = 0;
    Arg resources[4];

    XtSetArg( resources[n], XmNmarginHeight, 0);n++;
    XtSetArg( resources[n], XmNmarginWidth, 0);n++;
    XtSetArg( resources[n], XmNshadowThickness, 2);n++;
    XtSetArg( resources[n], XmNhighlightThickness, 0);n++;

    Widget writeButton = XmCreatePushButtonGadget(parentWidget,"writeButton",
						  resources,n);
    XtAddCallback( writeButton, XmNactivateCallback, 
		   (XtCallbackProc) writeButtonCB, (XtPointer) scene );
    return writeButton;
}

void main(int argc, char *argv[])
{
    SoXtExaminerViewer *myViewer;
    SoXtExaminerViewer *myMenuViewer;
    SbBool shouldMakeMenus = FALSE;
    
    // Initialize Inventor and Xt
    Widget mainWindow = SoXt::init(argv[0]);
    
    Button::initClass();

    EndPointFromParamsEngine::initClass();
    LinkEngine::initClass();
    RivetHingeEngine::initClass();
    DoubleLinkMoveOriginEngine::initClass();
    DoubleLinkMoveSharedPtEngine::initClass();
    PistonErrorEngine::initClass();
    ZAngleFromRotationEngine::initClass();

    LinkBase::initClass();
    SimpleLink::initClass();
    GroundedSimpleLink::initClass();
    Link::initClass();
    SizedLink::initClass();
    RivetHinge::initClass();
    Crank::initClass();
    DoubleLink::initClass();
    Piston::initClass();

    // See if you should write out
    int curArg = 1;
    if (argc > curArg && !strcmp(argv[curArg],"-m")) {
	shouldMakeMenus = TRUE;
	curArg++;
    }

    char *filename = NULL;
    int  defaultFile = TRUE;
    if (argc > curArg) {
	filename = argv[curArg];
	curArg++;
	defaultFile = FALSE;
    }
    else 
	filename = "motorizedMechanism.iv";

    if (argc > curArg)
	usage(argv[0]);
    
    // Create the viewer
    myViewer = new SoXtExaminerViewer(mainWindow);
    
    //
    // Read file in
    //
    SoInput in;
    if (! in.openFile(filename)) {
	if ( defaultFile ) {
	    SoDebugError::post("main::main", 
	    "could not open the default file %s.", filename );
	}
	else {
	    SoDebugError::post("main::main", 
	    "could not open file %s.", filename );
	}
	exit(1);
    }

    // Read the resource file that should contain 
    // resources for this program.
    readResources();

    SoSeparator *scene;
    scene = SoDB::readAll(&in);
    if( scene == NULL)
	exit(1);

    // Write out the scene you just read...
    scene->ref();

    // Add a write button to the viewer.
    Widget buttonParentW = myViewer->getAppPushButtonParent();
    Widget writeButton = makeWriteButton(buttonParentW, scene);
    myViewer->addAppPushButton( writeButton );

    // attach and show viewer
    myViewer->setSceneGraph(scene);
    myViewer->setTitle("Interactive Linkage Program");
    myViewer->show();
    XtRealizeWidget(mainWindow);
    
    if ( shouldMakeMenus ) {
	SoInput in2;
	if (! in2.openFile("menu.iv"))
	    exit(1);
	SoSeparator *menuScene;
	menuScene = SoDB::readAll(&in2);
	if(menuScene == NULL)
	    exit(1);
	menuScene->ref();
	myMenuViewer = new SoXtExaminerViewer();
	myMenuViewer->setSceneGraph(menuScene);
	myMenuViewer->viewAll();
	myMenuViewer->setDecoration(FALSE);
	myMenuViewer->setViewing(FALSE);
	myMenuViewer->show();

	// Add callbacks to buttons in menu...
	addMenuButtonCallbacks( scene );
    }

    // Loop forever
    SoXt::mainLoop();
}

