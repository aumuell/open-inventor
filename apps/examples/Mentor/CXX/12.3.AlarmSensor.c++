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
 *  chapter 12, example 3.
 *
 *  Alarm sensor that raises a flag after 10 minutes
 *------------------------------------------------------------*/

#include <stdlib.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/sensors/SoAlarmSensor.h>

///////////////////////////////////////////////////////////
// CODE FOR The Inventor Mentor STARTS HERE

static void
raiseFlagCallback(void *data, SoSensor *)
{
   // We know data is really a SoTransform node:
   SoTransform *flagAngleXform = (SoTransform *)data;

   // Rotate flag by 90 degrees about the Z axis:
   flagAngleXform->rotation.setValue(SbVec3f(0,0,1), M_PI/2);
}

// CODE FOR The Inventor Mentor ENDS HERE
///////////////////////////////////////////////////////////

int
main(int , char **argv)
{
   Widget myWindow = SoXt::init(argv[0]); // pass the app name
   if (myWindow == NULL) exit(1);

   ///////////////////////////////////////////////////////////
   // CODE FOR The Inventor Mentor STARTS HERE

   SoTransform *flagXform = new SoTransform;

   // Create an alarm that will call the flag-raising callback:
   SoAlarmSensor *myAlarm =
       new SoAlarmSensor(raiseFlagCallback, flagXform);
   myAlarm->setTimeFromNow(12.0);  // 12 seconds
   myAlarm->schedule();

   // CODE FOR The Inventor Mentor ENDS HERE
   ///////////////////////////////////////////////////////////

   SoSeparator *root = new SoSeparator;
   root->ref();
   root->addChild(flagXform);
   SoCone *myCone = new SoCone;
   myCone->bottomRadius = 0.1;
   root->addChild(myCone);

   SoXtExaminerViewer *myViewer = new
      SoXtExaminerViewer(myWindow);

   // Put our scene in myViewer, change the title
   myViewer->setSceneGraph(root);
   myViewer->setTitle("Raise The Cone");
   myViewer->show();

   SoXt::show(myWindow);  // Display main window
   SoXt::mainLoop();      // Main Inventor event loop
}

