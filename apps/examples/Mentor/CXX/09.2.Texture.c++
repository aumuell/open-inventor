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

/*-----------------------------------------------------------
 *  This is an example from the Inventor Mentor,
 *  chapter 9, example 2.
 *
 *  Using the offscreen renderer to generate a texture map.
 *  Generate simple scene and grab the image to use as
 *  a texture map.
 *----------------------------------------------------------*/

#include <stdlib.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoOffscreenRenderer.h>
#include <Inventor/SbViewportRegion.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>


SbBool 
generateTextureMap (SoNode *root, SoTexture2 *texture, 
   short textureWidth, short textureHeight)
{
   SbViewportRegion myViewport(textureWidth, textureHeight);

   // Render the scene
   SoOffscreenRenderer *myRenderer = 
            new SoOffscreenRenderer(myViewport);
   myRenderer->setBackgroundColor(SbColor(0.3, 0.3, 0.3));
   if (!myRenderer->render(root)) {
      delete myRenderer;
      return FALSE;
   }

   // Generate the texture
   texture->image.setValue(SbVec2s(textureWidth, textureHeight),
            SoOffscreenRenderer::RGB, myRenderer->getBuffer());

   delete myRenderer;
   return TRUE; 
}

int
main(int, char **argv)
{
   // Initialize Inventor and Xt
   Widget appWindow = SoXt::init(argv[0]);
   if (appWindow == NULL)
      exit(1);

   // Make a scene from reading in a file
   SoSeparator *texRoot = new SoSeparator;
   SoInput in;
   SoNode *result;

   texRoot->ref();
   in.openFile("/usr/share/src/Inventor/examples/data/jumpyMan.iv");
   SoDB::read(&in, result);

   SoPerspectiveCamera *myCamera = new SoPerspectiveCamera;
   SoRotationXYZ *rot = new SoRotationXYZ;
   rot->axis  = SoRotationXYZ::X;
   rot->angle = M_PI_2;
   myCamera->position.setValue(SbVec3f(-0.2, -0.2, 2.0));
   myCamera->scaleHeight(0.4); 
   texRoot->addChild(myCamera);
   texRoot->addChild(new SoDirectionalLight);
   texRoot->addChild(rot);
   texRoot->addChild(result);

   // Generate the texture map
   SoTexture2 *texture = new SoTexture2;
   texture->ref();
   if (generateTextureMap(texRoot, texture, 64, 64))
      printf ("Successfully generated texture map\n");
   else
      printf ("Could not generate texture map\n");
   texRoot->unref();

   // Make a scene with a cube and apply the texture to it
   SoSeparator *root = new SoSeparator;
   root->ref();
   root->addChild(texture);
   root->addChild(new SoCube);

   // Initialize an Examiner Viewer
   SoXtExaminerViewer *viewer =
            new SoXtExaminerViewer(appWindow);
   viewer->setSceneGraph(root);
   viewer->setTitle("Offscreen Rendered Texture");

   // In Inventor 2.1, if the machine does not have hardware texture
   // mapping, we must override the default drawStyle to display textures.
   viewer->setDrawStyle(SoXtViewer::STILL, SoXtViewer::VIEW_AS_IS);

   viewer->show();

   SoXt::show(appWindow);
   SoXt::mainLoop();
}
