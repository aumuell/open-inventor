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
// to change a camera position and orientation.
// The spaceball motion events are delivered via
// a callback on the viewer.
//

#include <stdlib.h>
#include <X11/Intrinsic.h>

#include <Inventor/So.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/events/SoEvents.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/devices/SoXtMouse.h>
#include <Inventor/Xt/devices/SoXtKeyboard.h>
#include <Inventor/Xt/devices/SoXtSpaceball.h>
#include <Inventor/Xt/viewers/SoXtWalkViewer.h>

// global devices
SoXtSpaceball *spaceball = NULL;

static SbBool
processSpballEvents(void *userData, XAnyEvent *anyevent)
{
    const SoEvent *event = spaceball->translateEvent(anyevent);
    SoXtWalkViewer *viewer = (SoXtWalkViewer *) userData;
    SoCamera *camera = viewer->getCamera();
    
    if (event != NULL) {
	if (event->isOfType(SoMotion3Event::getClassTypeId())) {
	    const SoMotion3Event *motion = (const SoMotion3Event *) event;
	    
	    // move the camera position by the translation amount
	    // in the forward direction.
	    // we will ignore 'y' motion since this is the walk viewer.
	    // first, orient the spaceball data to our camera orientation.
	    float x, y, z;
	    motion->getTranslation().getValue(x, y, z);
	    SbVec3f t(x, 0, z), xlate;
	    
	    SbMatrix m;
	    m.setRotate(camera->orientation.getValue());
	    m.multVecMatrix(t, xlate);
	    
	    // now update the position
	    camera->position.setValue(camera->position.getValue() + xlate);
	
	    // change the orientation of the camera about the up direction.
	    // ignore 'x' and 'z' rotation since this is the walk viewer.
	    // first, get the spaceball rotation as an axis/angle.
	    SbVec3f axis, newAxis;
	    float angle;
	    motion->getRotation().getValue(axis, angle);
	    
	    // ignore x and z
	    axis *= angle;
	    axis.getValue(x, y, z);
	    axis.setValue(0, y, 0);
	    
	    // reset axis and angle to only be about 'y'
	    angle = axis.length();
	    axis.normalize();
	    
	    // orient the spaceball rotation to our camera orientation.
	    m.multVecMatrix(axis, newAxis);
	    SbRotation rot(newAxis, angle);
	
	    // now update the orientation
	    camera->orientation.setValue(camera->orientation.getValue() * rot);
	}
	
	else if (SO_SPACEBALL_PRESS_EVENT(event, PICK)) {
	    // reset!
	    viewer->resetToHomePosition();
	}
    }
    
    return (event != NULL); // return whether we handled the event or not
}

static SoNode *
setupTest(const char *filename)
{
    SoInput	in;
    SoNode	*node;
    SoSeparator *root = new SoSeparator;
    SbBool	ok;

    root->ref();
    if (in.openFile(filename)) {
	while ((ok = SoDB::read(&in, node)) != FALSE && node != NULL)
	    root->addChild(node);
    
	if (! ok) {
	    fprintf(stderr, "Bad data. Bye!\n");
	    return NULL;
	}
	if (root->getNumChildren() == 0) {
	    fprintf(stderr, "No data read. Bye!\n");
	    return NULL;
	}
    
	in.closeFile();
    }
    else printf("Error opening file %s\n", filename);

    return root;
}

void
msg()
{
    printf("The spaceball can be used to translate and rotate the camera.\n");
    printf("Hit the spaceball pick button (located on the ball itself)\n");
    printf("to reset the camera to its home position.\n");
}

void
main(unsigned int argc, char *argv[])
{
    if (argc != 2) {
	printf("Usage: %s filename\n", argv[0]);
	printf("(A good file to walk through is /usr/share/data/models/buildings/Barcelona.iv)\n");
	exit(0);
    }
    
    Widget mainWindow = SoXt::init(argv[0]);

    if (! SoXtSpaceball::exists()) {
	printf("Could not find a spaceball device connected to this display\n");
	exit(1);
    }
    
    msg();
    
    if (mainWindow != NULL) {
	// build and initialize the inventor render area widget
	SoXtWalkViewer *viewer = new SoXtWalkViewer(mainWindow);
    	viewer->setSceneGraph(setupTest(argv[1]));

	// handle spaceball 
	spaceball = new SoXtSpaceball;
	viewer->registerDevice(spaceball);
	viewer->setEventCallback(processSpballEvents, viewer); // viewer is userData
	
	// display the main window
        viewer->show();
	SoXt::show(mainWindow);

	// loop forever
	SoXt::mainLoop();
    }
}

