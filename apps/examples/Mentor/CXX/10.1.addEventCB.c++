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
 *  This is an example from The Inventor Mentor
 *  chapter 10, example 1.
 *
 *  The scene graph has 4 objects which may be
 *  selected by picking with the left mouse button
 *  (use shift key to extend the selection to more
 *  than one object).
 *  
 *  Hitting the up arrow key will increase the size of
 *  each selected object; hitting down arrow will decrease
 *  the size of each selected object.
 *-----------------------------------------------------------*/

#include <stdlib.h>
#include <Inventor/Sb.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/SoXtRenderArea.h>
#include <Inventor/actions/SoBoxHighlightRenderAction.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>

// Function prototypes
void myKeyPressCB(void *, SoEventCallback *);
void myScaleSelection(SoSelection *, float);

// Global data
SbViewportRegion viewportRegion;
SoTransform *cubeTransform, *sphereTransform,
            *coneTransform, *cylTransform;

int
main(int , char **argv)
{
   // Print out usage message
   printf("Left mouse button        - selects object\n");
   printf("<shift>Left mouse button - selects multiple objects\n");
   printf("Up and Down arrows       - scale selected objects\n");

   // Initialize Inventor and Xt
   Widget myWindow = SoXt::init(argv[0]);
   if (myWindow == NULL) exit(1);

   // Create and set up the selection node
   SoSelection *selectionRoot = new SoSelection;
   selectionRoot->ref();
   selectionRoot->policy = SoSelection::SHIFT;
   
   // Add a camera and some light
   SoPerspectiveCamera *myCamera = new SoPerspectiveCamera;
   selectionRoot->addChild(myCamera);
   selectionRoot->addChild(new SoDirectionalLight);

///////////////////////////////////////////////////////////////
// CODE FOR The Inventor Mentor STARTS HERE  (part 1)

   // An event callback node so we can receive key press events
   SoEventCallback *myEventCB = new SoEventCallback;
   myEventCB->addEventCallback(
            SoKeyboardEvent::getClassTypeId(), 
            myKeyPressCB, selectionRoot);
   selectionRoot->addChild(myEventCB);

// CODE FOR The Inventor Mentor ENDS HERE
///////////////////////////////////////////////////////////////

   // Add some geometry to the scene

   // a red cube
   SoSeparator *cubeRoot = new SoSeparator; 
   SoMaterial *cubeMaterial = new SoMaterial;
   cubeTransform = new SoTransform;
   cubeRoot->addChild(cubeTransform);
   cubeRoot->addChild(cubeMaterial);
   cubeRoot->addChild(new SoCube);
   cubeTransform->translation.setValue(-2, 2, 0);
   cubeMaterial->diffuseColor.setValue(.8, 0, 0);
   selectionRoot->addChild(cubeRoot);

   // a blue sphere
   SoSeparator *sphereRoot = new SoSeparator; 
   SoMaterial *sphereMaterial = new SoMaterial;
   sphereTransform = new SoTransform;
   sphereRoot->addChild(sphereTransform);
   sphereRoot->addChild(sphereMaterial);
   sphereRoot->addChild(new SoSphere);
   sphereTransform->translation.setValue(2, 2, 0);
   sphereMaterial->diffuseColor.setValue(0, 0, .8);
   selectionRoot->addChild(sphereRoot);

   // a green cone
   SoSeparator *coneRoot = new SoSeparator;
   SoMaterial *coneMaterial = new SoMaterial;
   coneTransform = new SoTransform;
   coneRoot->addChild(coneTransform);
   coneRoot->addChild(coneMaterial);
   coneRoot->addChild(new SoCone);
   coneTransform->translation.setValue(2, -2, 0);
   coneMaterial->diffuseColor.setValue(0, .8, 0);
   selectionRoot->addChild(coneRoot);

   // a magenta cylinder
   SoSeparator *cylRoot = new SoSeparator;
   SoMaterial *cylMaterial = new SoMaterial;
   cylTransform = new SoTransform;
   cylRoot->addChild(cylTransform);
   cylRoot->addChild(cylMaterial);
   cylRoot->addChild(new SoCylinder);
   cylTransform->translation.setValue(-2, -2, 0);
   cylMaterial->diffuseColor.setValue(.8, 0, .8);
   selectionRoot->addChild(cylRoot);

   // Create a render area for viewing the scene
   SoXtRenderArea *myRenderArea = new SoXtRenderArea(myWindow);
   myRenderArea->setSceneGraph(selectionRoot);
   myRenderArea->setGLRenderAction(new SoBoxHighlightRenderAction());
   myRenderArea->redrawOnSelectionChange(selectionRoot);
   myRenderArea->setTitle("Adding Event Callbacks");

   // Make the camera see the whole scene
   viewportRegion = myRenderArea->getViewportRegion();
   myCamera->viewAll(selectionRoot, viewportRegion, 2.0);

   // Show our application window, and loop forever...
   myRenderArea->show();
   SoXt::show(myWindow);
   SoXt::mainLoop();
}

///////////////////////////////////////////////////////////////
// CODE FOR The Inventor Mentor STARTS HERE  (part 2)

// If the event is down arrow, then scale down every object 
// in the selection list; if the event is up arrow, scale up.
// The userData is the selectionRoot from main().
void
myKeyPressCB(void *userData, SoEventCallback *eventCB)
{
   SoSelection *selection = (SoSelection *) userData;
   const SoEvent *event = eventCB->getEvent();

   // check for the Up and Down arrow keys being pressed
   if (SO_KEY_PRESS_EVENT(event, UP_ARROW)) {
      myScaleSelection(selection, 1.1);
      eventCB->setHandled();
   } else if (SO_KEY_PRESS_EVENT(event, DOWN_ARROW)) {
      myScaleSelection(selection, 1.0/1.1);
      eventCB->setHandled();
   }
}

// CODE FOR The Inventor Mentor ENDS HERE
///////////////////////////////////////////////////////////////

// Scale each object in the selection list
void
myScaleSelection(SoSelection *selection, float sf)
{
   SoPath *selectedPath;
   SoTransform *xform;
   SbVec3f scaleFactor;
   int i,j;

   // Scale each object in the selection list

   for (i = 0; i < selection->getNumSelected(); i++) {
      selectedPath = selection->getPath(i);
      xform = NULL;

      // Look for the shape node, starting from the tail of the
      // path.  Once we know the type of shape, we know which
      // transform to modify
      for (j=0; j < selectedPath->getLength() &&
                (xform == NULL); j++) {
         SoNode *n = (SoNode *)selectedPath->getNodeFromTail(j);         if (n->isOfType(SoCube::getClassTypeId())) {
            xform = cubeTransform;
         } else if (n->isOfType(SoCone::getClassTypeId())) {
            xform = coneTransform;
         } else if (n->isOfType(SoSphere::getClassTypeId())) {
            xform = sphereTransform;
         } else if (n->isOfType(SoCylinder::getClassTypeId())) {            xform = cylTransform;
         }
      }

      // Apply the scale
      scaleFactor = xform->scaleFactor.getValue();
      scaleFactor *= sf;
      xform->scaleFactor.setValue(scaleFactor);
   }

}

