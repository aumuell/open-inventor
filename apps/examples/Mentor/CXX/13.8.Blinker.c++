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
 *  chapter 13, example 9.
 *
 *  Blinker node.
 *  Use a blinker node to flash a neon ad sign on and off
 *------------------------------------------------------------*/

#include <stdlib.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/SoXtRenderArea.h>
#include <Inventor/nodes/SoBlinker.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoText3.h>

int
main(int , char **argv)
{
   // Initialize Inventor and Xt
   Widget myWindow = SoXt::init(argv[0]);  
   if (myWindow == NULL) exit(1);     

   // Set up camera and light
   SoSeparator *root = new SoSeparator;
   root->ref();
   SoPerspectiveCamera *myCamera = new SoPerspectiveCamera;
   root->addChild(myCamera);
   root->addChild(new SoDirectionalLight);

   // Read in the parts of the sign from a file
   SoInput myInput;
   if (!myInput.openFile("/usr/share/src/Inventor/examples/data/eatAtJosies.iv")) 
      exit (1);
   SoSeparator *fileContents = SoDB::readAll(&myInput);
   if (fileContents == NULL) 
      exit (1);

   SoSeparator *eatAt;
   eatAt = (SoSeparator *)SoNode::getByName("EatAt");
   SoSeparator *josie;
   josie = (SoSeparator *)SoNode::getByName("Josies");
   SoSeparator *frame;
   frame = (SoSeparator *)SoNode::getByName("Frame");

//////////////////////////////////////////////////////////////
// CODE FOR The Inventor Mentor STARTS HERE

   // Add the non-blinking part of the sign to the root
   root->addChild(eatAt);
   
   // Add the fast-blinking part to a blinker node
   SoBlinker *fastBlinker = new SoBlinker;
   root->addChild(fastBlinker);
   fastBlinker->speed = 2;  // blinks 2 times a second
   fastBlinker->addChild(josie);

   // Add the slow-blinking part to another blinker node
   SoBlinker *slowBlinker = new SoBlinker;
   root->addChild(slowBlinker);
   slowBlinker->speed = 0.5;  // 2 secs per cycle; 1 on, 1 off
   slowBlinker->addChild(frame);

// CODE FOR The Inventor Mentor ENDS HERE
//////////////////////////////////////////////////////////////

   // Set up and display render area 
   SoXtRenderArea *myRenderArea = new SoXtRenderArea(myWindow);
   SbViewportRegion myRegion(myRenderArea->getSize()); 
   myCamera->viewAll(root, myRegion);

   myRenderArea->setSceneGraph(root);
   myRenderArea->setTitle("Neon");
   myRenderArea->show();
   SoXt::show(myWindow);
   SoXt::mainLoop();
}

  
