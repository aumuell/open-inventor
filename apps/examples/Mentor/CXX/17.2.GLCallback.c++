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

/*------------------------------------------------------------
 *  This is an example from The Inventor Mentor,
 *  chapter 17, example 2.
 *
 *  Example of combining Inventor and OpenGL rendering.
 *  Create an Inventor render area and draw a red cube 
 *  and a blue sphere.  Render the floor with OpenGL 
 *  through a Callback node.
 *----------------------------------------------------------*/

#include <GL/gl.h>
#include <Inventor/SbLinear.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/SoXtRenderArea.h>
#include <Inventor/nodes/SoCallback.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/elements/SoGLLazyElement.h>

float   floorObj[81][3];

// Build a scene with two objects and some light
void
buildScene(SoGroup *root)
{
   // Some light
   root->addChild(new SoLightModel);
   root->addChild(new SoDirectionalLight);

   // A red cube translated to the left and down
   SoTransform *myTrans = new SoTransform;    
   myTrans->translation.setValue(-2.0, -2.0, 0.0);
   root->addChild(myTrans);

   SoMaterial *myMtl = new SoMaterial;
   myMtl->diffuseColor.setValue(1.0, 0.0, 0.0);
   root->addChild(myMtl);
   
   root->addChild(new SoCube);

   // A blue sphere translated right
   myTrans = new SoTransform;    
   myTrans->translation.setValue(4.0, 0.0, 0.0);
   root->addChild(myTrans);

   myMtl = new SoMaterial;
   myMtl->diffuseColor.setValue(0.0, 0.0, 1.0);
   root->addChild(myMtl);
   
   root->addChild(new SoSphere);
}

// Build the floor that will be rendered using OpenGL.
void
buildFloor()
{
   int a = 0;

   for (float i = -5.0; i <= 5.0; i += 1.25) {
      for (float j = -5.0; j <= 5.0; j += 1.25, a++) {
         floorObj[a][0] = j;
         floorObj[a][1] = 0.0;
         floorObj[a][2] = i;
      }
   }
}

// Draw the lines that make up the floor, using OpenGL
void
drawFloor()
{
   int i;

   glBegin(GL_LINES);
   for (i=0; i<4; i++) {
      glVertex3fv(floorObj[i*18]);
      glVertex3fv(floorObj[(i*18)+8]);
      glVertex3fv(floorObj[(i*18)+17]);
      glVertex3fv(floorObj[(i*18)+9]);
   }

   glVertex3fv(floorObj[i*18]);
   glVertex3fv(floorObj[(i*18)+8]);
   glEnd();

   glBegin(GL_LINES);
   for (i=0; i<4; i++) {
      glVertex3fv(floorObj[i*2]);
      glVertex3fv(floorObj[(i*2)+72]);
      glVertex3fv(floorObj[(i*2)+73]);
      glVertex3fv(floorObj[(i*2)+1]);
   }
   glVertex3fv(floorObj[i*2]);
   glVertex3fv(floorObj[(i*2)+72]);
   glEnd();
}

// Callback routine to render the floor using OpenGL
void
myCallbackRoutine(void *, SoAction *action)
{
   // only render the floor during GLRender actions:
   if(!action->isOfType(SoGLRenderAction::getClassTypeId())) return;
   
   glPushMatrix();
   glTranslatef(0.0, -3.0, 0.0);
   glColor3f(0.0, 0.7, 0.0);
   glLineWidth(2);
   glDisable(GL_LIGHTING);  // so we don't have to set normals
   drawFloor();
   glEnable(GL_LIGHTING);   
   glLineWidth(1);
   glPopMatrix();
   
   //With Inventor 2.1, it's necessary to reset SoGLLazyElement after
   //making calls (such as glColor3f()) that affect material state.
   //In this case, the diffuse color and light model are being modified,
   //so the logical-or of DIFFUSE_MASK and LIGHT_MODEL_MASK is passed 
   //to SoGLLazyElement::reset().  
   //Additional information can be found in the publication
   // "Open Inventor 2.1 Porting and Performance Tips"
  
   SoState *state = action->getState();
   SoGLLazyElement* lazyElt = 
	(SoGLLazyElement*)SoLazyElement::getInstance(state);
   lazyElt->reset(state, 
	(SoLazyElement::DIFFUSE_MASK)|(SoLazyElement::LIGHT_MODEL_MASK));
   
}

int
main(int, char **)
{
   // Initialize Inventor utilities
   Widget myWindow = SoXt::init("Example 17.1");

   buildFloor();

   // Build a simple scene graph, including a camera and
   // a SoCallback node for performing some GL rendering.
   SoSeparator *root = new SoSeparator;
   root->ref();

   SoPerspectiveCamera *myCamera = new SoPerspectiveCamera;
   myCamera->position.setValue(0.0, 0.0, 5.0);
   myCamera->heightAngle  = M_PI/2.0;  // 90 degrees
   myCamera->nearDistance = 2.0;
   myCamera->farDistance  = 12.0;
   root->addChild(myCamera);

   SoCallback *myCallback = new SoCallback;
   myCallback->setCallback(myCallbackRoutine);
   root->addChild(myCallback);

   buildScene(root);
   
   // Initialize an Inventor Xt RenderArea and draw the scene.
   SoXtRenderArea *myRenderArea = new SoXtRenderArea(myWindow);
   myRenderArea->setSceneGraph(root);
   myRenderArea->setTitle("OpenGL Callback");
   myRenderArea->setBackgroundColor(SbColor(.8, .8, .8));
   myRenderArea->show();
   
   SoXt::show(myWindow);
   SoXt::mainLoop();
}
