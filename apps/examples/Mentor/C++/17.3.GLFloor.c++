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
 *  This is an example from The Inventor Mentor,
 *  chapter 17, example 3.
 * 
 * This example draws the same scene as Example 17.2, 
 *  but using a GLX window.
 *---------------------------------------------------------*/

#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <unistd.h>

#include <Inventor/SoDB.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/elements/SoGLLazyElement.h>

#define WINWIDTH	400	
#define WINHEIGHT	400	

float   floorObj[81][3];

// Build an Inventor scene with two objects and some light
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


// Build a floor that will be rendered using OpenGL.
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

// Callback used by GLX window
static Bool
waitForNotify(Display *, XEvent *e, char *arg)
{
   return (e->type == MapNotify) && 
            (e->xmap.window == (Window) arg);
}

// Create and initialize GLX window.
void
openWindow(Display *&display, Window &window)
{
   XVisualInfo	*vi;
   Colormap cmap;
   XSetWindowAttributes swa;
   GLXContext cx;
   XEvent event;
   static int attributeList[] = {
            GLX_RGBA,
            GLX_RED_SIZE, 1,
            GLX_GREEN_SIZE, 1,
            GLX_BLUE_SIZE, 1,
            GLX_DEPTH_SIZE, 1,
            GLX_DOUBLEBUFFER,	
            None,
   };

   // Open the X display 
   display = XOpenDisplay(0);

   // Initialize the GLX visual and context
   vi = glXChooseVisual(display, 
            DefaultScreen(display), attributeList);
   cx = glXCreateContext(display, vi, 0, GL_TRUE);

   // Create the X color map
   cmap = XCreateColormap(display, 
            RootWindow(display, vi->screen), 
            vi->visual, AllocNone);

   // Create and map the X window
   swa.colormap = cmap;
   swa.border_pixel = 0;
   swa.event_mask = StructureNotifyMask;
   window = XCreateWindow(display, 
            RootWindow(display, vi->screen), 100, 100, WINWIDTH,            
            WINHEIGHT, 0, vi->depth, InputOutput, vi->visual, 
            (CWBorderPixel | CWColormap | CWEventMask), &swa);
   XMapWindow(display, window);
   XIfEvent(display, &event, waitForNotify, (char *) window);

   // Attach the GLX context to the window
   glXMakeCurrent(display, window, cx);
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


main(int, char **)
{
   // Initialize Inventor
   SoDB::init();

   // Build a simple scene graph
   SoSeparator *root = new SoSeparator;
   root->ref();
   buildScene(root);

   // Build the floor geometry
   buildFloor();

   // Create and initialize window
   Display *display;
   Window window;
   openWindow(display, window);
   glEnable(GL_DEPTH_TEST);
   glClearColor(0.8, 0.8, 0.8, 1.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // Set up the camera using OpenGL.
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(90.0, 1.0, 2.0, 12.0);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glTranslatef(0.0, 0.0, -5.0);

   // Render the floor using OpenGL
   glPushMatrix();
   glTranslatef(0.0, -3.0, 0.0);
   glColor3f(0.7, 0.0, 0.0);
   glLineWidth(2.0);
   glDisable(GL_LIGHTING);
   drawFloor();
   glEnable(GL_LIGHTING);
   glPopMatrix();

   // Render the scene
   SbViewportRegion myViewport(WINWIDTH, WINHEIGHT);
   SoGLRenderAction myRenderAction(myViewport);
 
   myRenderAction.apply(root);
   glXSwapBuffers(display, window); 
   
   //With inventor 2.1, it's necessary to reset the lazy element
   //any time GL calls are made outside of inventor.  In this example,
   //between the first and second rendering, the inventor state must
   //have both diffuse color and light model reset, since these are
   //modified by the GLX rendering code.  For more information about
   //the lazy element, see the publication,
   //  "Open Inventor 2.1 Porting and Performance Tips"

   //To reset the lazy element, first we obtain the state
   //from the action, then obtain the lazy element from the state, 
   //and finally apply a reset to that lazy element.
   
   SoState* state = myRenderAction.getState();
   SoGLLazyElement* lazyElt = 
	(SoGLLazyElement*)SoLazyElement::getInstance(state);
   lazyElt->reset(state, 
	(SoLazyElement::DIFFUSE_MASK)|(SoLazyElement::LIGHT_MODEL_MASK));
    
   sleep(5);
   
   //Rerender the floor using OpenGL again:
   glClearColor(0.8, 0.8, 0.8, 1.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
   glPushMatrix();
   glTranslatef(0.0, -3.0, 0.0);
   glColor3f(0.0, 0.7, 0.0);
   glLineWidth(2.0);
   glDisable(GL_LIGHTING);
   drawFloor();
   glEnable(GL_LIGHTING);
   glPopMatrix();
   
   //Redraw the rest of the scene: 
   myRenderAction.apply(root);
   glXSwapBuffers(display, window);

   sleep (10); 
   root->unref();
   return 0;
}
