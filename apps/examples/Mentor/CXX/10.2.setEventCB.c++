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

/*-------------------------------------------------------------
 *  This is an example from The Inventor Mentor
 *  chapter 10, example 2.
 *
 *  This demonstrates using SoXtRenderArea::setEventCallback().
 *  which causes events to be sent directly to the application
 *  without being sent into the scene graph.
 *  
 * Clicking the left mouse button and dragging will draw 
 *       points in the xy plane beneath the mouse cursor.
 * Clicking middle mouse and holding causes the point set 
 *       to rotate about the Y axis. 
 * Clicking right mouse clears all points drawn so far out 
 *       of the point set.
 *-----------------------------------------------------------*/

#include <stdlib.h>
#include <X11/Intrinsic.h>

#include <Inventor/Sb.h>
#include <Inventor/Xt/SoXtRenderArea.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoPointSet.h> 
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/sensors/SoTimerSensor.h>

// Timer sensor 
// Rotate 90 degrees every second, update 30 times a second
SoTimerSensor *myTicker;
#define UPDATE_RATE 1.0/30.0
#define ROTATION_ANGLE M_PI/60.0

void
myProjectPoint(SoXtRenderArea *myRenderArea, 
   int mousex, int mousey, SbVec3f &intersection)
{
   // Take the x,y position of mouse, and normalize to [0,1].
   // X windows have 0,0 at the upper left,
   // Inventor expects 0,0 to be the lower left.
   SbVec2s size = myRenderArea->getSize();
   float x = float(mousex) / size[0];
   float y = float(size[1] - mousey) / size[1];
   
   // Get the camera and view volume
   SoGroup *root = (SoGroup *) myRenderArea->getSceneGraph();
   SoCamera *myCamera = (SoCamera *) root->getChild(0);
   SbViewVolume myViewVolume;
   myViewVolume = myCamera->getViewVolume();
   
   // Project the mouse point to a line
   SbVec3f p0, p1;
   myViewVolume.projectPointToLine(SbVec2f(x,y), p0, p1);
   
   // Midpoint of the line intersects a plane thru the origin
   intersection = (p0 + p1) / 2.0;
}

void
myAddPoint(SoXtRenderArea *myRenderArea, const SbVec3f point)
{
   SoGroup *root = (SoGroup *) myRenderArea->getSceneGraph();
   SoCoordinate3 *coord = (SoCoordinate3 *) root->getChild(2);
   SoPointSet *myPointSet = (SoPointSet *) root->getChild(3);
   
   coord->point.set1Value(coord->point.getNum(), point);
   myPointSet->numPoints.setValue(coord->point.getNum());
}

void
myClearPoints(SoXtRenderArea *myRenderArea)
{
   SoGroup *root = (SoGroup *) myRenderArea->getSceneGraph();
   SoCoordinate3 *coord = (SoCoordinate3 *) root->getChild(2);
   SoPointSet *myPointSet = (SoPointSet *) root->getChild(3);
   
   // Delete all values starting from 0
   coord->point.deleteValues(0); 
   myPointSet->numPoints.setValue(0);
}

void
tickerCallback(void *userData, SoSensor *)
{
   SoCamera *myCamera = (SoCamera *) userData;
   SbRotation rot;
   SbMatrix mtx;
   SbVec3f pos;
   
   // Adjust the position
   pos = myCamera->position.getValue();
   rot = SbRotation(SbVec3f(0,1,0), ROTATION_ANGLE);
   mtx.setRotate(rot);
   mtx.multVecMatrix(pos, pos);
   myCamera->position.setValue(pos);
   
   // Adjust the orientation
   myCamera->orientation.setValue(
            myCamera->orientation.getValue() * rot);
}

///////////////////////////////////////////////////////////////
// CODE FOR The Inventor Mentor STARTS HERE  (part 1)

SbBool
myAppEventHandler(void *userData, XAnyEvent *anyevent)
{
   SoXtRenderArea *myRenderArea = (SoXtRenderArea *) userData;
   XButtonEvent *myButtonEvent;
   XMotionEvent *myMotionEvent;
   SbVec3f vec;
   SbBool handled = TRUE;

   switch (anyevent->type) {
   
   case ButtonPress:
      myButtonEvent = (XButtonEvent *) anyevent;
      if (myButtonEvent->button == Button1) {
         myProjectPoint(myRenderArea, 
                  myButtonEvent->x, myButtonEvent->y, vec);
         myAddPoint(myRenderArea, vec);
      } else if (myButtonEvent->button == Button2) {
         myTicker->schedule();  // start spinning the camera
      } else if (myButtonEvent->button == Button3) {
         myClearPoints(myRenderArea);  // clear the point set
      }
      break;
      
   case ButtonRelease:
      myButtonEvent = (XButtonEvent *) anyevent;
      if (myButtonEvent->button == Button2) {
         myTicker->unschedule();  // stop spinning the camera
      }
      break;
      
   case MotionNotify:
      myMotionEvent = (XMotionEvent *) anyevent;
      if (myMotionEvent->state & Button1Mask) {  
         myProjectPoint(myRenderArea, 
                  myMotionEvent->x, myMotionEvent->y, vec);
         myAddPoint(myRenderArea, vec);
      }
      break;
      
   default:
      handled = FALSE;
      break;
   }
   
   return handled;
}

// CODE FOR The Inventor Mentor ENDS HERE
////////////////////////////////////////////////////////////

int
main(int argc, char **argv)
{
   // Print out usage instructions
   printf("Mouse buttons:\n");
   printf("\tLeft (with mouse motion): adds points\n");
   printf("\tMiddle: rotates points about the Y axis\n");
   printf("\tRight: deletes all the points\n");

   // Initialize Inventor and Xt
   Widget appWindow = SoXt::init(argv[0]);
   if (appWindow == NULL) exit(1);

   // Create and set up the root node
   SoSeparator *root = new SoSeparator;
   root->ref();

   // Add a camera
   SoPerspectiveCamera *myCamera = new SoPerspectiveCamera;
   root->addChild(myCamera);                 // child 0
   
   // Use the base color light model so we don't need to 
   // specify normals
   SoLightModel *myLightModel = new SoLightModel;
   myLightModel->model = SoLightModel::BASE_COLOR;
   root->addChild(myLightModel);               // child 1
   
   // Set up the camera view volume
   myCamera->position.setValue(0, 0, 4);
   myCamera->nearDistance.setValue(1.0);
   myCamera->farDistance.setValue(7.0);
   myCamera->heightAngle.setValue(M_PI/3.0);   
   
   // Add a coordinate and point set
   SoCoordinate3 *myCoord = new SoCoordinate3;
   SoPointSet *myPointSet = new SoPointSet;
   root->addChild(myCoord);                    // child 2
   root->addChild(myPointSet);                 // child 3

   // Timer sensor to tick off time while middle mouse is down
   myTicker = new SoTimerSensor(tickerCallback, myCamera);
   myTicker->setInterval(UPDATE_RATE);

   // Create a render area for viewing the scene
   SoXtRenderArea *myRenderArea = new SoXtRenderArea(appWindow);
   myRenderArea->setSceneGraph(root);
   myRenderArea->setTitle("My Event Handler");

//////////////////////////////////////////////////////////////
// CODE FOR The Inventor Mentor STARTS HERE  (part 2)

   // Have render area send events to us instead of the scene 
   // graph.  We pass the render area as user data.
   myRenderArea->setEventCallback(
               myAppEventHandler, myRenderArea);

// CODE FOR The Inventor Mentor ENDS HERE
//////////////////////////////////////////////////////////////

   // Show our application window, and loop forever...
   myRenderArea->show();
   SoXt::show(appWindow);
   SoXt::mainLoop();
}

