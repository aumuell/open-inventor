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
 *  chapter 4, example 1.
 *
 *  Camera example.  
 *  A blinker node is used to switch between three 
 *  different views of the same scene. The cameras are 
 *  switched once per second.
 *------------------------------------------------------------*/

#include <cstdlib>
#include <Inventor/SbLinear.h>

#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/SoXtRenderArea.h>
#include <Inventor/nodes/SoBlinker.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>

int
main(int, char **argv)
{
   // Initialize Inventor and Xt
   Widget myWindow = SoXt::init(argv[0]);
   if (myWindow == NULL) exit(1);

   SoSeparator *root = new SoSeparator;
   root->ref();

   // Create a blinker node and put it in the scene. A blinker
   // switches between its children at timed intervals.
   SoBlinker *myBlinker = new SoBlinker;
   root->addChild(myBlinker);

   // Create three cameras. Their positions will be set later.
   // This is because the viewAll method depends on the size
   // of the render area, which has not been created yet.
   SoOrthographicCamera *orthoViewAll =new SoOrthographicCamera;
   SoPerspectiveCamera *perspViewAll = new SoPerspectiveCamera;
   SoPerspectiveCamera *perspOffCenter =new SoPerspectiveCamera;
   myBlinker->addChild(orthoViewAll);
   myBlinker->addChild(perspViewAll);
   myBlinker->addChild(perspOffCenter);

   // Create a light
   root->addChild(new SoDirectionalLight);

   // Read the object from a file and add to the scene
   SoInput myInput;
   if (!myInput.openFile("/usr/share/src/Inventor/examples/data/parkbench.iv")) 
      exit (1);
   SoSeparator *fileContents = SoDB::readAll(&myInput);
   if (fileContents == NULL) 
      exit (1);

   SoMaterial *myMaterial = new SoMaterial;
   myMaterial->diffuseColor.setValue(0.8, 0.23, 0.03); 
   root->addChild(myMaterial);
   root->addChild(fileContents);

   SoXtRenderArea *myRenderArea = new SoXtRenderArea(myWindow);

   // Establish camera positions. 
   // First do a viewAll on all three cameras.  
   // Then modify the position of the off-center camera.
   SbViewportRegion myRegion(myRenderArea->getSize());
   orthoViewAll->viewAll(root, myRegion);
   perspViewAll->viewAll(root, myRegion);
   perspOffCenter->viewAll(root, myRegion);
   SbVec3f initialPos; 
   initialPos = perspOffCenter->position.getValue();
   float x, y, z;
   initialPos.getValue(x,y,z);
   perspOffCenter->position.setValue(x+x/2., y+y/2., z+z/4.);

   myRenderArea->setSceneGraph(root);
   myRenderArea->setTitle("Cameras");
   myRenderArea->show();

   SoXt::show(myWindow);
   SoXt::mainLoop();
}
