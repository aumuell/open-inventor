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

/*----------------------------------------------------------------
 *  This is an example from the Inventor Mentor
 *  chapter 14, example 2.
 *
 *  Use nodekits to create a scene with a desk into an 
 *  SoWrapperKit.  Then, add a material editor for the desk and 
 *  a light editor on the light.
 *  
 *  The scene is organized using an SoSceneKit, which contains
 *  lists for grouping lights (lightList), cameras (cameraList), 
 *  and objects (childList) in a scene.
 *  
 *  Once the scene is created, a material editor is attached to 
 *  the wrapperKit's 'material' part, and a directional light editor
 *  is attached to the light's 'directionalLight' part.
 *----------------------------------------------------------------*/

#include <stdlib.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/SoXtDirectionalLightEditor.h>
#include <Inventor/Xt/SoXtMaterialEditor.h>
#include <Inventor/Xt/SoXtRenderArea.h>
#include <Inventor/nodekits/SoCameraKit.h>
#include <Inventor/nodekits/SoLightKit.h>
#include <Inventor/nodekits/SoSceneKit.h>
#include <Inventor/nodekits/SoWrapperKit.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoSeparator.h>

int
main(int , char **argv)
{
   // Initialize Inventor and Xt
   Widget myWindow = SoXt::init(argv[0]);
   if (myWindow == NULL) exit(1);

   // SCENE!
   SoSceneKit *myScene = new SoSceneKit;
   myScene->ref();

   // LIGHTS! Add an SoLightKit to the "lightList." The 
   // SoLightKit creates an SoDirectionalLight by default.
   myScene->setPart("lightList[0]", new SoLightKit);

   // CAMERA!! Add an SoCameraKit to the "cameraList." The 
   // SoCameraKit creates an SoPerspectiveCamera by default.
   myScene->setPart("cameraList[0]", new SoCameraKit);
   myScene->setCameraNumber(0);

   // Read an object from file. 
   SoInput myInput;
   if (!myInput.openFile("/usr/share/src/Inventor/examples/data/desk.iv")) 
      exit (1);
   SoSeparator *fileContents = SoDB::readAll(&myInput);
   if (fileContents == NULL) 
      exit (1);

   // OBJECT!! Create an SoWrapperKit and set its contents to
   // be what you read from file.
   SoWrapperKit *myDesk = new SoWrapperKit();
   myDesk->setPart("contents", fileContents);
   myScene->setPart("childList[0]", myDesk);

   // Give the desk a good starting color
   myDesk->set("material { diffuseColor .8 .3 .1 }");

   // MATERIAL EDITOR!!  Attach it to myDesk's material node.
   // Use the SO_GET_PART macro to get this part from myDesk.
   SoXtMaterialEditor *mtlEditor = new SoXtMaterialEditor();
   SoMaterial *mtl = SO_GET_PART(myDesk,"material",SoMaterial);
   mtlEditor->attach(mtl);
   mtlEditor->setTitle("Material of Desk");
   mtlEditor->show();

   // DIRECTIONAL LIGHT EDITOR!! Attach it to the 
   // SoDirectionalLight node within the SoLightKit we made.
   SoXtDirectionalLightEditor *ltEditor = 
                 new SoXtDirectionalLightEditor();
   SoPath *ltPath = myScene->createPathToPart(
      "lightList[0].light", TRUE);
   ltEditor->attach(ltPath);
   ltEditor->setTitle("Lighting of Desk");
   ltEditor->show();

   SoXtRenderArea *myRenderArea = new SoXtRenderArea(myWindow);

   // Set up Camera with ViewAll...
   // -- use the SO_GET_PART macro to get the camera node.
   // -- viewall is a method on the 'camera' part of 
   //    the cameraKit, not on the cameraKit itself.  So the part
   //    we ask for is not 'cameraList[0]' (which is of type 
   //    SoPerspectiveCameraKit), but 
   //    'cameraList[0].camera' (which is of type 
   //    SoPerspectiveCamera).
   SoPerspectiveCamera *myCamera = SO_GET_PART(myScene,
      "cameraList[0].camera", SoPerspectiveCamera);
   SbViewportRegion myRegion(myRenderArea->getSize());
   myCamera->viewAll(myScene, myRegion);

   myRenderArea->setSceneGraph(myScene);
   myRenderArea->setTitle("Main Window: Desk In A Scene Kit");
   myRenderArea->show();

   SoXt::show(myWindow);
   SoXt::mainLoop();
}

