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

/*------------------------------------------------------------
 *  This is an example from The Inventor Mentor,
 *  chapter 9, example 4.
 *
 *  Example of setting up pick actions and using the pick path.
 *  A couple of objects are displayed.  The program catches 
 *  mouse button events and determines the mouse position. 
 *  A pick action is applied and if an object is picked the
 *  pick path is printed to stdout.
 *-----------------------------------------------------------*/

#include <Inventor/SbViewportRegion.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>

#include <stdlib.h>

///////////////////////////////////////////////////////////////
// CODE FOR The Inventor Mentor STARTS HERE

SbBool
writePickedPath (SoNode *root, 
   const SbViewportRegion &viewport, 
   const SbVec2s &cursorPosition)
{
   SoRayPickAction myPickAction(viewport);

   // Set an 8-pixel wide region around the pixel
   myPickAction.setPoint(cursorPosition);
   myPickAction.setRadius(8.0);

   // Start a pick traversal
   myPickAction.apply(root);
   const SoPickedPoint *myPickedPoint = 
            myPickAction.getPickedPoint();
   if (myPickedPoint == NULL) return FALSE;

   // Write out the path to the picked object
   SoWriteAction myWriteAction;
   myWriteAction.apply(myPickedPoint->getPath());

   return TRUE;
}

// CODE FOR The Inventor Mentor ENDS HERE
///////////////////////////////////////////////////////////////

// This routine is called for every mouse button event.
void
myMousePressCB(void *userData, SoEventCallback *eventCB)
{
   SoSeparator *root = (SoSeparator *) userData;
   const SoEvent *event = eventCB->getEvent();

   // Check for mouse button being pressed
   if (SO_MOUSE_PRESS_EVENT(event, ANY)) {
      const SbViewportRegion &myRegion = 
         eventCB->getAction()->getViewportRegion();
      writePickedPath(root, myRegion,
                      event->getPosition(myRegion));
      eventCB->setHandled();
   } 
}

int
main(int, char **argv)
{
   SoMouseButtonEvent  myMouseEvent;

   // Initialize Inventor and Xt
   Widget myWindow = SoXt::init(argv[0]); 
   if (myWindow == NULL) exit(1);

   SoSeparator *root = new SoSeparator;
   root->ref();

   // Add an event callback to catch mouse button presses.
   // The callback is set up later on.
   SoEventCallback *myEventCB = new SoEventCallback;
   root->addChild(myEventCB);

   // Read object data from a file
   SoInput mySceneInput;
   if (!mySceneInput.openFile("/usr/share/src/Inventor/examples/data/star.iv")) 
      exit (1);
   SoSeparator *starObject = SoDB::readAll(&mySceneInput);
   if (starObject == NULL) exit (1);
   mySceneInput.closeFile();

   // Add two copies of the star object, one white and one red
   SoRotationXYZ *myRotation = new SoRotationXYZ;
   myRotation->axis.setValue(SoRotationXYZ::X);
   myRotation->angle.setValue(M_PI/2.2);  // almost 90 degrees
   root->addChild(myRotation);

   root->addChild(starObject);  // first star object

   SoMaterial *myMaterial = new SoMaterial;
   myMaterial->diffuseColor.setValue(1.0, 0.0, 0.0);   // red
   root->addChild(myMaterial);
   SoTranslation *myTranslation = new SoTranslation;
   myTranslation->translation.setValue(1., 0., 1.);
   root->addChild(myTranslation);
   root->addChild(starObject);  // second star object

   // Create a render area in which to see our scene graph.
   SoXtExaminerViewer *myViewer = new SoXtExaminerViewer(myWindow);

   // Turn off viewing to allow picking
   myViewer->setViewing(FALSE);

   myViewer->setSceneGraph(root);
   myViewer->setTitle("Pick Actions & Paths");
   myViewer->show();

   // Set up the event callback. We want to pass the root of the
   // entire scene graph (including the camera) as the userData,
   // so we get the scene manager's version of the scene graph
   // root.
   myEventCB->addEventCallback(
      SoMouseButtonEvent::getClassTypeId(),
      myMousePressCB,
      myViewer->getSceneManager()->getSceneGraph());

   SoXt::show(myWindow);  
   SoXt::mainLoop();      
}

