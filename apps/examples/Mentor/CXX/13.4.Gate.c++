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
 *  chapter 13, example 5.
 *
 *  Gate engine.
 *  Mouse button presses enable and disable a gate engine.
 *  The gate engine controls an elapsed time engine that
 *  controls the motion of the duck.
 *------------------------------------------------------------*/

#include <stdlib.h>
#include <Inventor/SoDB.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/SoXtRenderArea.h>
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
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>

void myMousePressCB(void *, SoEventCallback *);

int
main(int , char **argv)
{
   // Print out usage message
   printf("Click the left mouse button to enable/disable the duck motion\n");

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
   SoMaterial *cylMaterial = new SoMaterial;
   cylMaterial->diffuseColor.setValue(0., 0.3, 0.8);
   pond->addChild(cylMaterial);
   SoTranslation *cylTranslation = new SoTranslation;
   cylTranslation->translation.setValue(0., -6.725, 0.);
   pond->addChild(cylTranslation);
   SoCylinder *myCylinder = new SoCylinder;
   myCylinder->radius.setValue(4.0);
   myCylinder->height.setValue(0.5);
   pond->addChild(myCylinder);

/////////////////////////////////////////////////////////////
// CODE FOR The Inventor Mentor STARTS HERE  (part 1)

   // Duck group
   SoSeparator *duck = new SoSeparator;
   root->addChild(duck);

   // Read the duck object from a file and add to the group
   SoInput myInput;
   if (!myInput.openFile("/usr/share/src/Inventor/examples/data/duck.iv")) 
      exit (1);
   SoSeparator *duckObject = SoDB::readAll(&myInput);
   if (duckObject == NULL) 
      exit (1);

   // Set up the duck transformations
   SoRotationXYZ *duckRotXYZ = new SoRotationXYZ;
   duck->addChild(duckRotXYZ);
   SoTransform *initialTransform = new SoTransform;
   initialTransform->translation.setValue(0., 0., 3.);
   initialTransform->scaleFactor.setValue(6., 6., 6.);
   duck->addChild(initialTransform);

   duck->addChild(duckObject);

   // Update the rotation value if the gate is enabled.
   SoGate *myGate = new SoGate(SoMFFloat::getClassTypeId());
   SoElapsedTime *myCounter = new SoElapsedTime;
   myGate->input->connectFrom(&myCounter->timeOut); 
   duckRotXYZ->axis = SoRotationXYZ::Y;  // rotate about Y axis
   duckRotXYZ->angle.connectFrom(myGate->output);

   // Add an event callback to catch mouse button presses.
   // Each button press will enable or disable the duck motion.
   SoEventCallback *myEventCB = new SoEventCallback;
   myEventCB->addEventCallback(
            SoMouseButtonEvent::getClassTypeId(),
            myMousePressCB, myGate);
   root->addChild(myEventCB);

// CODE FOR The Inventor Mentor ENDS HERE
/////////////////////////////////////////////////////////////

   SoXtRenderArea *myRenderArea = new SoXtRenderArea(myWindow);
   myRenderArea->setSceneGraph(root);
   myRenderArea->setTitle("Duck Pond");
   myRenderArea->show();

   SoXt::show(myWindow);
   SoXt::mainLoop();
}

/////////////////////////////////////////////////////////////
// CODE FOR The Inventor Mentor STARTS HERE  (part 2)

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

// CODE FOR The Inventor Mentor ENDS HERE
/////////////////////////////////////////////////////////////
