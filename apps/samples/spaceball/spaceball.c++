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
// This demonstrates using the spaceball device
// to change a transformation node. Spaceball events
// are passed via a callback on an SoEventCallback node.
//

#include <stdlib.h>
#include <X11/Intrinsic.h>

#include <Inventor/So.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/events/SoEvents.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/devices/SoXtSpaceball.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>

static void
eventCB(void *userData, SoEventCallback *cb)
{
    const SoEvent *event = cb->getEvent();
    SoTransform *xform = (SoTransform *) userData;
    
    if (event->isOfType(SoMotion3Event::getClassTypeId())) {
	const SoMotion3Event *motion = (const SoMotion3Event *) event;
	
	xform->translation.setValue(
	    xform->translation.getValue() + motion->getTranslation().getValue());
	
	xform->rotation.setValue(
	    xform->rotation.getValue() * motion->getRotation().getValue());
    }
    
    else if (SO_SPACEBALL_PRESS_EVENT(event, PICK)) {
	// reset!
	xform->rotation.setValue(0, 0, 0, 1);
	xform->translation.setValue(0, 0, 0);
    }
}

static SoNode *
setupTest()
{
    SoSeparator *root = new SoSeparator;
    SoTransform *xform = new SoTransform;
    SoEventCallback *cbNode = new SoEventCallback;
    
    // camera and lights will be created by viewer
    root->ref();
    root->addChild(xform);
    root->addChild(new SoCube);
    root->addChild(cbNode);
    
    cbNode->addEventCallback(SoMotion3Event::getClassTypeId(), eventCB, xform);  
    cbNode->addEventCallback(SoSpaceballButtonEvent::getClassTypeId(),
			     eventCB, xform);  
    
    return root;
}

void
usage()
{
    printf("The spaceball can be used to translate and rotate the cube.\n");
    printf("Hit the spaceball pick button (located on the ball itself)\n");
    printf("to reset the transform affecting the cube.\n");
}

void
main(unsigned int, char *argv[])
{
    Widget mainWindow = SoXt::init(argv[0]);

    if (! SoXtSpaceball::exists()) {
	printf("Could not find a spaceball device connected to this display\n");
	exit(1);
    }
    
    usage();    
    
    // build and initialize the viewer
    SoXtExaminerViewer *viewer = new SoXtExaminerViewer(mainWindow);
    viewer->setSceneGraph(setupTest());
    viewer->setViewing(FALSE); // so events will pass through to us!
    
    // add spaceball device
    SoXtSpaceball *spaceball = new SoXtSpaceball;
    viewer->registerDevice(spaceball);
    
    // display the main window
    viewer->show();
    SoXt::show(mainWindow);
    
    // loop forever
    SoXt::mainLoop();
}

