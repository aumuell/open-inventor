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

/*--------------------------------------------------------------
 *  This is an example from the Inventor Mentor
 *  chapter 13, example 6.
 *
 *  Boolean engine.  Derived from example 13.5.
 *  The smaller duck stays still while the bigger duck moves,
 *  and starts moving as soon as the bigger duck stops.
 *------------------------------------------------------------*/

#include <stdlib.h>
#include <Inventor/SoDB.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/SoXtRenderArea.h>
#include <Inventor/engines/SoBoolOperation.h>
#include <Inventor/engines/SoCompose.h>
#include <Inventor/engines/SoElapsedTime.h>
#include <Inventor/engines/SoGate.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>

void myMousePressCB(void *, SoEventCallback *);

int
main(int , char **argv)
{
   // Print out usage message
   printf("Only one duck can move at a time.\n");
   printf("Click the left mouse button to toggle between the two ducks.\n");

   // Initialize Inventor and Xt
   Widget myWindow = SoXt::init(argv[0]);  
   if (myWindow == NULL) exit(1);     

   SoSeparator *root = new SoSeparator;
   root->ref();

   // Add a camera and light
   SoPerspectiveCamera *myCamera = new SoPerspectiveCamera;
   myCamera->position.setValue(0., -4., 8.0);
   myCamera->heightAngle = M_PI/2.5; 
   myCamera->nearDistance = 1.0;
   myCamera->farDistance = 15.0;
   root->addChild(myCamera);
   root->addChild(new SoDirectionalLight);

   // Rotate scene slightly to get better view
   SoRotationXYZ *globalRotXYZ = new SoRotationXYZ;
   globalRotXYZ->axis = SoRotationXYZ::X;
   globalRotXYZ->angle = M_PI/9;
   root->addChild(globalRotXYZ);

   // Pond group
   SoSeparator *pond = new SoSeparator; 
   root->addChild(pond);
   SoTranslation *pondTranslation = new SoTranslation;
   pondTranslation->translation.setValue(0., -6.725, 0.);
   pond->addChild(pondTranslation);
   // water
   SoMaterial *waterMaterial = new SoMaterial;
   waterMaterial->diffuseColor.setValue(0., 0.3, 0.8);
   pond->addChild(waterMaterial);
   SoCylinder *waterCylinder = new SoCylinder;
   waterCylinder->radius.setValue(4.0);
   waterCylinder->height.setValue(0.5);
   pond->addChild(waterCylinder);
   // rock
   SoMaterial *rockMaterial = new SoMaterial;
   rockMaterial->diffuseColor.setValue(0.8, 0.23, 0.03);
   pond->addChild(rockMaterial);
   SoSphere *rockSphere = new SoSphere;
   rockSphere->radius.setValue(0.9);
   pond->addChild(rockSphere);

   // Read the duck object from a file and add to the group
   SoInput myInput;
   if (!myInput.openFile("/usr/share/src/Inventor/examples/data/duck.iv")) 
      exit (1);
   SoSeparator *duckObject = SoDB::readAll(&myInput);
   if (duckObject == NULL) 
      exit (1);

/////////////////////////////////////////////////////////////
// CODE FOR The Inventor Mentor STARTS HERE  

   // Bigger duck group
   SoSeparator *bigDuck = new SoSeparator;
   root->addChild(bigDuck);
   SoRotationXYZ *bigDuckRotXYZ = new SoRotationXYZ;
   bigDuck->addChild(bigDuckRotXYZ);
   SoTransform *bigInitialTransform = new SoTransform;
   bigInitialTransform->translation.setValue(0., 0., 3.5);
   bigInitialTransform->scaleFactor.setValue(6., 6., 6.);
   bigDuck->addChild(bigInitialTransform);
   bigDuck->addChild(duckObject);

   // Smaller duck group
   SoSeparator *smallDuck = new SoSeparator;
   root->addChild(smallDuck);
   SoRotationXYZ *smallDuckRotXYZ = new SoRotationXYZ;
   smallDuck->addChild(smallDuckRotXYZ);
   SoTransform *smallInitialTransform = new SoTransform;
   smallInitialTransform->translation.setValue(0., -2.24, 1.5);
   smallInitialTransform->scaleFactor.setValue(4., 4., 4.);
   smallDuck->addChild(smallInitialTransform);
   smallDuck->addChild(duckObject);

   // Use a gate engine to start/stop the rotation of 
   // the bigger duck.
   SoGate *bigDuckGate = new SoGate(SoMFFloat::getClassTypeId());
   SoElapsedTime *bigDuckTime = new SoElapsedTime;
   bigDuckGate->input->connectFrom(&bigDuckTime->timeOut); 
   bigDuckRotXYZ->axis = SoRotationXYZ::Y;  // Y axis
   bigDuckRotXYZ->angle.connectFrom(bigDuckGate->output);

   // Each mouse button press will enable/disable the gate 
   // controlling the bigger duck.
   SoEventCallback *myEventCB = new SoEventCallback;
   myEventCB->addEventCallback(
            SoMouseButtonEvent::getClassTypeId(),
            myMousePressCB, bigDuckGate);
   root->addChild(myEventCB);

   // Use a Boolean engine to make the rotation of the smaller
   // duck depend on the bigger duck.  The smaller duck moves
   // only when the bigger duck is still.
   SoBoolOperation *myBoolean = new SoBoolOperation;
   myBoolean->a.connectFrom(&bigDuckGate->enable);
   myBoolean->operation = SoBoolOperation::NOT_A;

   SoGate *smallDuckGate = new SoGate(SoMFFloat::getClassTypeId());
   SoElapsedTime *smallDuckTime = new SoElapsedTime;
   smallDuckGate->input->connectFrom(&smallDuckTime->timeOut); 
   smallDuckGate->enable.connectFrom(&myBoolean->output); 
   smallDuckRotXYZ->axis = SoRotationXYZ::Y;  // Y axis
   smallDuckRotXYZ->angle.connectFrom(smallDuckGate->output);

// CODE FOR The Inventor Mentor ENDS HERE
/////////////////////////////////////////////////////////////

   SoXtRenderArea *myRenderArea = new SoXtRenderArea(myWindow);
   myRenderArea->setSceneGraph(root);
   myRenderArea->setTitle("Duck and Duckling");
   myRenderArea->show();

   SoXt::show(myWindow);
   SoXt::mainLoop();
}

// This routine is called for every mouse button event.
void
myMousePressCB(void *userData, SoEventCallback *eventCB)
{
   SoGate *gate = (SoGate *) userData;
   const SoEvent *event = eventCB->getEvent();

   // Check for mouse button being pressed
   if (SO_MOUSE_PRESS_EVENT(event, ANY)) {

      // Toggle the gate that controls the duck motion
      if (gate->enable.getValue()) 
         gate->enable.setValue(FALSE);
      else 
         gate->enable.setValue(TRUE);

      eventCB->setHandled();
   } 
}

