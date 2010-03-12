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
 *  This is an example from The Inventor Mentor
 *  chapter 17, example 1.
 *
 *  This examples shows how the user can create a custom
 *  X visual for doing color index rendering with
 *  an Inventor Viewer. This shows how to create the right
 *  visual, as well as load the color map with the wanted
 *  colors.
 *-----------------------------------------------------------*/

#include <cstdlib>
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <GL/glx.h>

// window attribute list to create a color index visual.
// This will create a double buffered color index window
// with the maximum number of bits and a zbuffer.
int attribList[] = {
   GLX_DOUBLEBUFFER, 
   GLX_BUFFER_SIZE, 1, 
   GLX_DEPTH_SIZE, 1, 
   None };

// list of colors to load in the color map
static float colors[3][3] = {{.2, .2, .2}, {.5, 1, .5}, {.5, .5, 1}};

static char *sceneBuffer = "\
#Inventor V2.0 ascii\n\
\
Separator { \
   LightModel { model BASE_COLOR } \
   ColorIndex { index 1 } \
   Coordinate3 { point [ -1 -1 -1, -1 1 -1, 1 1 1, 1 -1 1] } \
   FaceSet {} \
   ColorIndex { index 2 } \
   Coordinate3 { point [ -1 -1 1, -1 1 1, 1 1 -1, 1 -1 -1] } \
   FaceSet {} \
} ";

int
main(int , char **argv)
{
   // Initialize Inventor and Xt
   Widget myWindow = SoXt::init(argv[0]);
   
   // read the scene graph in
   SoInput in;
   SoNode *scene;
   in.setBuffer((void *)sceneBuffer, (size_t) strlen(sceneBuffer));
   if (! SoDB::read(&in, scene) || scene == NULL) {
      printf("Couldn't read scene\n");
      exit(1);
   }
   
   // create the color index visual
   XVisualInfo *vis = glXChooseVisual(XtDisplay(myWindow), 
      XScreenNumberOfScreen(XtScreen(myWindow)), attribList);
   if (! vis) {
      printf("Couldn't create visual\n");
      exit(1);
   }
   
   // allocate the viewer, set the scene, the visual and
   // load the color map with the wanted colors.
   //
   // Color 0 will be used for the background (default) while
   // color 1 and 2 are used by the objects.
   //
   SoXtExaminerViewer *myViewer = new SoXtExaminerViewer(myWindow);
   myViewer->setNormalVisual(vis);
   myViewer->setColorMap(0, 3, (SbColor *) colors);
   myViewer->setSceneGraph(scene);
   myViewer->setTitle("Color Index Mode");
   
   // Show the viewer and loop forever...
   myViewer->show();
   XtRealizeWidget(myWindow);
   SoXt::mainLoop();
}
