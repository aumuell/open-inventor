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
 *  chapter 13, example 7.
 *
 *  A calculator engine computes a closed, planar curve.
 *  The output from the engine is connected to the translation
 *  applied to a flower object, which consequently moves
 *  along the path of the curve.
 *------------------------------------------------------------*/

#include <stdlib.h>
#include <Inventor/SoDB.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/SoXtRenderArea.h>
#include <Inventor/engines/SoCalculator.h>
#include <Inventor/engines/SoElapsedTime.h>
#include <Inventor/engines/SoTimeCounter.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>

int
main(int , char **argv)
{
   // Initialize Inventor and Xt
   Widget myWindow = SoXt::init(argv[0]);  
   if (myWindow == NULL) exit(1);     

   SoSeparator *root = new SoSeparator;
   root->ref();

   // Add a camera and light
   SoPerspectiveCamera *myCamera = new SoPerspectiveCamera;
   myCamera->position.setValue(-0.5, -3.0, 19.0);
   myCamera->nearDistance = 10.0;
   myCamera->farDistance = 26.0;
   root->addChild(myCamera);
   root->addChild(new SoDirectionalLight);

   // Rotate scene slightly to get better view
   SoRotationXYZ *globalRotXYZ = new SoRotationXYZ;
   globalRotXYZ->axis = SoRotationXYZ::X;
   globalRotXYZ->angle = M_PI/7;
   root->addChild(globalRotXYZ);

   // Read the background path from a file and add to the group
   SoInput myInput;
   if (!myInput.openFile("/usr/share/src/Inventor/examples/data/flowerPath.iv")) 
      exit (1);
   SoSeparator *flowerPath = SoDB::readAll(&myInput);
   if (flowerPath == NULL) exit (1);
   root->addChild(flowerPath);

/////////////////////////////////////////////////////////////
// CODE FOR The Inventor Mentor STARTS HERE  

   // Flower group
   SoSeparator *flowerGroup = new SoSeparator;
   root->addChild(flowerGroup);

   // Read the flower object from a file and add to the group
   if (!myInput.openFile("/usr/share/src/Inventor/examples/data/flower.iv")) 
      exit (1);
   SoSeparator *flower= SoDB::readAll(&myInput);
   if (flower == NULL) exit (1);

   // Set up the flower transformations
   SoTranslation *danceTranslation = new SoTranslation;
   SoTransform *initialTransform = new SoTransform;
   flowerGroup->addChild(danceTranslation);
   initialTransform->scaleFactor.setValue(10., 10., 10.);
   initialTransform->translation.setValue(0., 0., 5.);
   flowerGroup->addChild(initialTransform);
   flowerGroup->addChild(flower);

   // Set up an engine to calculate the motion path:
   // r = 5*cos(5*theta); x = r*cos(theta); z = r*sin(theta)
   // Theta is incremented using a time counter engine,
   // and converted to radians using an expression in
   // the calculator engine.
   SoCalculator *calcXZ = new SoCalculator; 
   SoTimeCounter *thetaCounter = new SoTimeCounter;

   thetaCounter->max = 360;
   thetaCounter->step = 4;
   thetaCounter->frequency = 0.075;

   calcXZ->a.connectFrom(&thetaCounter->output);    
   calcXZ->expression.set1Value(0, "ta=a*M_PI/180"); // theta
   calcXZ->expression.set1Value(1, "tb=5*cos(5*ta)"); // r
   calcXZ->expression.set1Value(2, "td=tb*cos(ta)"); // x 
   calcXZ->expression.set1Value(3, "te=tb*sin(ta)"); // z 
   calcXZ->expression.set1Value(4, "oA=vec3f(td,0,te)"); 
   danceTranslation->translation.connectFrom(&calcXZ->oA);

// CODE FOR The Inventor Mentor ENDS HERE
/////////////////////////////////////////////////////////////

   SoXtRenderArea *myRenderArea = new SoXtRenderArea(myWindow);
   myRenderArea->setSceneGraph(root);
   myRenderArea->setTitle("Flower Dance");
   myRenderArea->show();

   SoXt::show(myWindow);
   SoXt::mainLoop();
}

