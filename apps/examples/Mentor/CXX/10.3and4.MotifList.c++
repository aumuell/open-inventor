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
 *  chapter 10, example 3.
 *
 *  The scene graph has 4 objects which may be
 *  selected by picking with the left mouse button
 *  (use shift key to extend the selection to more
 *  than one object).
 *  
 *  Hitting the up arrow key will increase the size of
 *  each selected object; hitting down arrow will decrease
 *  the size of each selected object.
 *
 *  This also demonstrates selecting objects from a Motif
 *  list, and calling select/deselect functions on the
 *  SoSelection node to change the selection. Use the Shift
 *  key to extend the selection (i.e. pick more than one
 *  item in the motif list.)
 *-----------------------------------------------------------*/

#include <stdlib.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/List.h>

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
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>

// Function prototypes
void mySelectionCB(void *, SoPath *);
void myKeyPressCB(void *, SoEventCallback *);
void myScaleSelection(SoSelection *, float);
Widget createList(Display *, SoSelection *);
void myListPickCB(Widget, char *, XmListCallbackStruct *);

// Global data
SbViewportRegion viewportRegion;
Widget motifList;
SoTransform *cubeTransform, *sphereTransform,
            *coneTransform, *cylTransform;

///////////////////////////////////////////////////////////////
// CODE FOR The Inventor Mentor STARTS HERE  

enum objects {
	CUBE,
	SPHERE,
	CONE,
	CYL,
	NUM_OBJECTS
};

static char *objectNames[] = {
	"Cube",
	"Sphere",
	"Cone",
	"Cylinder"
};

// CODE FOR The Inventor Mentor ENDS HERE  
///////////////////////////////////////////////////////////////

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
   selectionRoot->addSelectionCallback(
            mySelectionCB, (void *) TRUE);
   selectionRoot->addDeselectionCallback(
            mySelectionCB, (void *) FALSE);
   
   // Add a camera and some light
   SoPerspectiveCamera *myCamera = new SoPerspectiveCamera;
   selectionRoot->addChild(myCamera);
   selectionRoot->addChild(new SoDirectionalLight);

   // Add an event callback so we can receive key press events
   SoEventCallback *myEventCB = new SoEventCallback;
   myEventCB->addEventCallback(
            SoKeyboardEvent::getClassTypeId(), 
            myKeyPressCB, selectionRoot);
   selectionRoot->addChild(myEventCB);

   // Add some geometry to the scene
   // a red cube
   SoSeparator *cubeRoot = new SoSeparator; 
   SoMaterial *cubeMaterial = new SoMaterial;
   cubeTransform = new SoTransform;
   SoCube *cube = new SoCube;
   cubeRoot->addChild(cubeTransform);
   cubeRoot->addChild(cubeMaterial);
   cubeRoot->addChild(cube);
   cubeTransform->translation.setValue(-2, 2, 0);
   cubeMaterial->diffuseColor.setValue(.8, 0, 0);
   selectionRoot->addChild(cubeRoot);

   // a blue sphere
   SoSeparator *sphereRoot = new SoSeparator; 
   SoMaterial *sphereMaterial = new SoMaterial;
   sphereTransform = new SoTransform;
   SoSphere *sphere = new SoSphere;
   sphereRoot->addChild(sphereTransform);
   sphereRoot->addChild(sphereMaterial);
   sphereRoot->addChild(sphere);
   sphereTransform->translation.setValue(2, 2, 0);
   sphereMaterial->diffuseColor.setValue(0, 0, .8);
   selectionRoot->addChild(sphereRoot);

   // a green cone
   SoSeparator *coneRoot = new SoSeparator;
   SoMaterial *coneMaterial = new SoMaterial;
   coneTransform = new SoTransform;
   SoCone *cone = new SoCone;
   coneRoot->addChild(coneTransform);
   coneRoot->addChild(coneMaterial);
   coneRoot->addChild(cone);
   coneTransform->translation.setValue(2, -2, 0);
   coneMaterial->diffuseColor.setValue(0, .8, 0);
   selectionRoot->addChild(coneRoot);

   // a magenta cylinder
   SoSeparator *cylRoot = new SoSeparator;
   SoMaterial *cylMaterial = new SoMaterial;
   cylTransform = new SoTransform;
   SoCylinder *cyl = new SoCylinder;
   cylRoot->addChild(cylTransform);
   cylRoot->addChild(cylMaterial);
   cylRoot->addChild(cyl);
   cylTransform->translation.setValue(-2, -2, 0);
   cylMaterial->diffuseColor.setValue(.8, 0, .8);
   selectionRoot->addChild(cylRoot);

///////////////////////////////////////////////////////////////
// CODE FOR The Inventor Mentor STARTS HERE  (part 2)

   cube->setName(objectNames[CUBE]);
   sphere->setName(objectNames[SPHERE]);
   cone->setName(objectNames[CONE]);
   cyl->setName(objectNames[CYL]);

// CODE FOR The Inventor Mentor ENDS HERE  
///////////////////////////////////////////////////////////////

   // Create a render area for viewing the scene
   SoXtRenderArea *myRenderArea = new SoXtRenderArea(myWindow);
   myRenderArea->setGLRenderAction(new SoBoxHighlightRenderAction());
   myRenderArea->redrawOnSelectionChange(selectionRoot);
   myRenderArea->setSceneGraph(selectionRoot);
   myRenderArea->setTitle("Motif Selection List");

   // Make the camera see the whole scene
   viewportRegion = myRenderArea->getViewportRegion();
   myCamera->viewAll(selectionRoot, viewportRegion, 2.0);

   // Create a Motif list for selecting objects without picking
   Widget objectList = createList(
            XtDisplay(myWindow), selectionRoot);

   // Show our application window, and loop forever...
   myRenderArea->show();
   SoXt::show(myWindow);
   SoXt::show(objectList);
   SoXt::mainLoop();
}


// Create the object list widget
Widget
createList(Display *display, SoSelection *selection)
{
   Widget shell;
   Arg    args[4];
   int    i, n;

   // Create a new shell window for the list
   n = 0;
   XtSetArg(args[n], XmNtitle, "Selection");
   n++;
   shell = XtAppCreateShell("example", "Inventor",
      topLevelShellWidgetClass, display, args, n);

///////////////////////////////////////////////////////////////
// CODE FOR The Inventor Mentor STARTS HERE  (part 3)

   // Create a table of object names
   XmString *table = new XmString[NUM_OBJECTS];
   for (i=0; i<NUM_OBJECTS; i++) {
       table[i] = XmStringCreate(objectNames[i], 
				 XmSTRING_DEFAULT_CHARSET);
   }

   // Create the list widget
   n = 0;
   XtSetArg(args[n], XmNitems, table);
   n++;
   XtSetArg(args[n], XmNitemCount, NUM_OBJECTS);
   n++;
   XtSetArg(args[n], XmNselectionPolicy, XmEXTENDED_SELECT);
   n++;

   motifList = XmCreateScrolledList(shell, "funcList", args, n);
   XtAddCallback(motifList, XmNextendedSelectionCallback,
      (XtCallbackProc) myListPickCB, (XtPointer) selection);

// CODE FOR The Inventor Mentor ENDS HERE  
///////////////////////////////////////////////////////////////

   // Free the name table
   for (i = 0; i < NUM_OBJECTS; i++)
      XmStringFree(table[i]);
   free(table);

   // Manage the list and return the shell
   XtManageChild(motifList);

   return shell;
}

// This callback is invoked every time the user picks
// an item in the Motif list.
void
myListPickCB(Widget, char *userData,
            XmListCallbackStruct *listData)
{
   SoSelection *selection = (SoSelection *) userData;
   SoSearchAction mySearchAction;

   // Remove the selection callbacks so that we don't get
   // called back while we are updating the selection list
   selection->removeSelectionCallback(
            mySelectionCB, (void *) TRUE);
   selection->removeDeselectionCallback(
            mySelectionCB, (void *) FALSE);

//////////////////////////////////////////////////////////////
// CODE FOR The Inventor Mentor STARTS HERE  (part 4)

   // Clear the selection node, then loop through the list
   // and reselect
   selection->deselectAll();

   // Update the SoSelection based on what is selected in
   // the motif list.  We do this by extracting the string
   // from the selected XmString, and searching for the 
   // object of that name.
   for (int i = 0; i < listData->selected_item_count; i++) {
      mySearchAction.setName(
            SoXt::decodeString(listData->selected_items[i]));
      mySearchAction.apply(selection);
      selection->select(mySearchAction.getPath());
   }

// CODE FOR The Inventor Mentor ENDS HERE
//////////////////////////////////////////////////////////////

   // Add the selection callbacks again
   selection->addSelectionCallback(
            mySelectionCB, (void *) TRUE);
   selection->addDeselectionCallback(
            mySelectionCB, (void *) FALSE);
}


// This is called whenever an object is selected or deselected.
// if userData is TRUE, then it's a selection; else deselection.
// (we set this convention up when we registered this callback).
// The function updates the Motif list to reflect the current
// selection.
void
mySelectionCB(void *userData, SoPath *selectionPath)
{
   Arg args[1];
   SbBool isSelection = (SbBool) userData;

   // We have to temporarily change the selection policy to
   // MULTIPLE so that we can select and deselect single items.
   XtSetArg(args[0], XmNselectionPolicy, XmMULTIPLE_SELECT);
   XtSetValues(motifList, args, 1);

   SoNode *node = selectionPath->getTail();

   for (int i = 0; i < NUM_OBJECTS; i++) {
      if (node->getName() == objectNames[i]) {
         if (isSelection) {
             XmListSelectPos(motifList, i+1, False);
         } else XmListDeselectPos(motifList, i+1);
         XmUpdateDisplay(motifList);
         break;
      }
   }

   // Restore the selection policy to extended.
   XtSetArg(args[0], XmNselectionPolicy, XmEXTENDED_SELECT);
   XtSetValues(motifList, args, 1);
}

// If the event in the graphics window is down arrow, then 
// scale every object in the selection list by 1/2; if the 
// event is up arrow, scale by 2.  
// userData is the selectionRoot from main().
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

//////////////////////////////////////////////////////////////
// CODE FOR The Inventor Mentor STARTS HERE  (Example 10-4)

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
         SoNode *n = (SoNode *)selectedPath->getNodeFromTail(j);
         if (n->isOfType(SoCube::getClassTypeId())) {
            xform = cubeTransform;
         } else if (n->isOfType(SoCone::getClassTypeId())) {
            xform = coneTransform;
         } else if (n->isOfType(SoSphere::getClassTypeId())) {
            xform = sphereTransform;
         } else if (n->isOfType(SoCylinder::getClassTypeId())) {
            xform = cylTransform;
         }
      }

      // Apply the scale
      scaleFactor = xform->scaleFactor.getValue();
      scaleFactor *= sf;
      xform->scaleFactor.setValue(scaleFactor);
   }

}

// CODE FOR The Inventor Mentor ENDS HERE  
//////////////////////////////////////////////////////////////
