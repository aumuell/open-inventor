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
 *  chapter 4, example 2.
 *
 *  Lights example.  
 *  Read in an object from a file.
 *  Use the ExaminerViewer to view it with two light sources.
 *  The red directional light doesn't move; the green point 
 *  light is moved back and forth using a shuttle node.
 *------------------------------------------------------------*/

#include <stdlib.h>
#include <Inventor/SoDB.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPointLight.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoShuttle.h>
#include <Inventor/nodes/SoTransformSeparator.h>

int
main(int , char **argv)
{
   // Initialize Inventor and Xt
   Widget myWindow = SoXt::init(argv[0]);
   if (myWindow == NULL) exit(1);

   SoSeparator *root = new SoSeparator;
   root->ref();

   // Add a directional light
   SoDirectionalLight *myDirLight = new SoDirectionalLight;
   myDirLight->direction.setValue(0, -1, -1);
   myDirLight->color.setValue(1, 0, 0);
   root->addChild(myDirLight);

   // Put the shuttle and the light below a transform separator.
   // A transform separator pushes and pops the transformation 
   // just like a separator node, but other aspects of the state 
   // are not pushed and popped. So the shuttle's translation 
   // will affect only the light. But the light will shine on 
   // the rest of the scene.
   SoTransformSeparator *myTransformSeparator =
       new SoTransformSeparator;
   root->addChild(myTransformSeparator);

   // A shuttle node translates back and forth between the two
   // fields translation0 and translation1.  
   // This moves the light.
   SoShuttle *myShuttle = new SoShuttle;
   myTransformSeparator->addChild(myShuttle);
   myShuttle->translation0.setValue(-2, -1, 3);
   myShuttle->translation1.setValue( 1,  2, -3);

   // Add the point light below the transformSeparator
   SoPointLight *myPointLight = new SoPointLight;
   myTransformSeparator->addChild(myPointLight);
   myPointLight->color.setValue(0, 1, 0);

   root->addChild(new SoCone);

   SoXtExaminerViewer *myViewer = 
            new SoXtExaminerViewer(myWindow);
   myViewer->setSceneGraph(root);
   myViewer->setTitle("Lights");
   myViewer->setHeadlight(FALSE);
   myViewer->show();

   SoXt::show(myWindow);
   SoXt::mainLoop();
}
