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
 *  This is an example from the Inventor Mentor,
 *  chapter 12, example 4.
 *
 *  Timer sensors.  An object is rotated by a timer sensor.
 *  (called "rotatingSensor").  The interval between calls 
 *  controls how often it rotates.
 *  A second timer (called "schedulingSensor") goes off
 *  every 5 seconds and changes the interval of 
 *  "rotatingSensor".  The interval alternates between
 *  once per second and 10 times per second.
 *  This example could also be done using engines.
 *------------------------------------------------------------*/

#include <stdlib.h>
#include <Inventor/SoDB.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/sensors/SoTimerSensor.h>

///////////////////////////////////////////////////////////
// CODE FOR The Inventor Mentor STARTS HERE

// This function is called either 10 times/second or once every
// second; the scheduling changes every 5 seconds (see below):
static void
rotatingSensorCallback(void *data, SoSensor *)
{
   // Rotate an object...
   SoRotation *myRotation = (SoRotation *)data;
   SbRotation currentRotation = myRotation->rotation.getValue();
   currentRotation = SbRotation(
            SbVec3f(0,0,1), M_PI/90.0) * currentRotation;
   myRotation->rotation.setValue(currentRotation);
}

// This function is called once every 5 seconds, and
// reschedules the other sensor.
static void
schedulingSensorCallback(void *data, SoSensor *)
{
   SoTimerSensor *rotatingSensor = (SoTimerSensor *)data;
   rotatingSensor->unschedule();
   if (rotatingSensor->getInterval() == 1.0)
      rotatingSensor->setInterval(1.0/10.0);
   else rotatingSensor->setInterval(1.0);
   rotatingSensor->schedule();
}

// CODE FOR The Inventor Mentor ENDS HERE
///////////////////////////////////////////////////////////

int
main(int argc, char **argv)
{
   if (argc != 2) {
     fprintf(stderr, "Usage: %s filename.iv\n", argv[0]);
     exit(1);
   }

   Widget myWindow = SoXt::init(argv[0]);
   if (myWindow == NULL) exit(1);

   SoSeparator *root = new SoSeparator;
   root->ref();
   
   ///////////////////////////////////////////////////////////
   // CODE FOR The Inventor Mentor STARTS HERE

   SoRotation *myRotation = new SoRotation;
   root->addChild(myRotation);

   SoTimerSensor *rotatingSensor =
      new SoTimerSensor(rotatingSensorCallback, myRotation);
   rotatingSensor->setInterval(1.0); // scheduled once per second
   rotatingSensor->schedule();

   SoTimerSensor *schedulingSensor =
      new SoTimerSensor(schedulingSensorCallback, rotatingSensor);
   schedulingSensor->setInterval(5.0); // once per 5 seconds
   schedulingSensor->schedule();

   // CODE FOR The Inventor Mentor ENDS HERE
   ///////////////////////////////////////////////////////////

   SoInput inputFile;
   if (inputFile.openFile(argv[1]) == FALSE) {
     fprintf(stderr, "Could not open file %s\n", argv[1]);
     exit(1);
   }
   root->addChild(SoDB::readAll(&inputFile));

   SoXtExaminerViewer *myViewer =
               new SoXtExaminerViewer(myWindow);
   myViewer->setSceneGraph(root);
   myViewer->setTitle("Two Timers");
   myViewer->show();

   SoXt::show(myWindow);  // Display main window
   SoXt::mainLoop();        // Main Inventor event loop
}
