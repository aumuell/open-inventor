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
 *  chapter 13, example 3.
 *
 *  Elapsed time engine.
 *  The output from an elapsed time engine is used to control
 *  the translation of the object.  The resulting effect is
 *  that the figure slides across the scene.
 *------------------------------------------------------------*/

#include <stdlib.h>
#include <Inventor/SoDB.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/SoXtRenderArea.h>
#include <Inventor/engines/SoCompose.h>
#include <Inventor/engines/SoElapsedTime.h>
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
   myCamera->position.setValue(-2.0, -2.0, 5.0);
   myCamera->heightAngle = M_PI/2.5; 
   myCamera->nearDistance = 2.0;
   myCamera->farDistance = 7.0;
   root->addChild(myCamera);
   root->addChild(new SoDirectionalLight);

   // Set up transformations
   SoTranslation *slideTranslation = new SoTranslation;
   root->addChild(slideTranslation);
   SoTransform *initialTransform = new SoTransform;
   initialTransform->translation.setValue(-5., 0., 0.);
   initialTransform->scaleFactor.setValue(10., 10., 10.);
   initialTransform->rotation.setValue(SbVec3f(1,0,0), M_PI/2.);
   root->addChild(initialTransform);

   // Read the figure object from a file and add to the scene
   SoInput myInput;
   if (!myInput.openFile("/usr/share/src/Inventor/examples/data/jumpyMan.iv")) 
      exit (1);
   SoSeparator *figureObject = SoDB::readAll(&myInput);
   if (figureObject == NULL) 
      exit (1);
   root->addChild(figureObject);

   // Make the X translation value change over time.
   SoElapsedTime *myCounter = new SoElapsedTime;
   SoComposeVec3f *slideDistance = new SoComposeVec3f;
   slideDistance->x.connectFrom(&myCounter->timeOut);
   slideTranslation->translation.connectFrom(
            &slideDistance->vector);

   SoXtRenderArea *myRenderArea = new SoXtRenderArea(myWindow);
   SbViewportRegion myRegion(myRenderArea->getSize()); 
   myRenderArea->setSceneGraph(root);
   myRenderArea->setTitle("Sliding Man");
   myRenderArea->show();

   SoXt::show(myWindow);
   SoXt::mainLoop();
}
