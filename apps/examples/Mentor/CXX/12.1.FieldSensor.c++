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
 *  chapter 12, example 1.
 *
 *  Sense changes to a viewer's camera's position.
 *------------------------------------------------------------*/

#include <stdlib.h>
#include <Inventor/SoDB.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/sensors/SoFieldSensor.h>

// Callback that reports whenever the viewer's position changes.
static void
cameraChangedCB(void *data, SoSensor *)
{
   SoCamera *viewerCamera = (SoCamera *)data;

   SbVec3f cameraPosition = viewerCamera->position.getValue();
   printf("Camera position: (%g,%g,%g)\n",
            cameraPosition[0], cameraPosition[1],
            cameraPosition[2]); 
}

int
main(int argc, char **argv)
{
   if (argc != 2) {
      fprintf(stderr, "Usage: %s filename.iv\n", argv[0]);
      exit(1);
   }

   Widget myWindow = SoXt::init(argv[0]);
   if (myWindow == NULL) exit(1);

   SoInput inputFile;
   if (inputFile.openFile(argv[1]) == FALSE) {
      fprintf(stderr, "Could not open file %s\n", argv[1]);
      exit(1);
   }
   
   SoSeparator *root = SoDB::readAll(&inputFile);
   root->ref();

   SoXtExaminerViewer *myViewer =
            new SoXtExaminerViewer(myWindow);
   myViewer->setSceneGraph(root);
   myViewer->setTitle("Camera Sensor");
   myViewer->show();

   // Get the camera from the viewer, and attach a 
   // field sensor to its position field:
   SoCamera *camera = myViewer->getCamera();
   SoFieldSensor *mySensor = 
            new SoFieldSensor(cameraChangedCB, camera);
   mySensor->attach(&camera->position);

   SoXt::show(myWindow);
   SoXt::mainLoop();
}
