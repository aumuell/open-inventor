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
 *  chapter 3, example 2.
 *
 *  This code shows how to create a robot out of various nodes.
 *  It introduces shared instancing of nodes to create two legs
 *  using two instances of the same subgraph.
 *
 *------------------------------------------------------------*/

#include <stdlib.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoTransform.h>

SoSeparator *
makeRobot()
{
//////////////////////////////////////////////////////////////
// CODE FOR The Inventor Mentor STARTS HERE
   // Robot with legs

   // Construct parts for legs (thigh, calf and foot)
   SoCube *thigh = new SoCube;     
   thigh->width = 1.2;
   thigh->height = 2.2;
   thigh->depth = 1.1;

   SoTransform *calfTransform = new SoTransform;
   calfTransform->translation.setValue(0, -2.25, 0.0);

   SoCube *calf = new SoCube;
   calf->width = 1;
   calf->height = 2.2;
   calf->depth = 1;

   SoTransform *footTransform = new SoTransform;
   footTransform->translation.setValue(0, -1.5, .5);

   SoCube *foot = new SoCube;
   foot->width = 0.8;
   foot->height = 0.8;
   foot->depth = 2;

   // Put leg parts together
   SoGroup *leg = new SoGroup;      
   leg->addChild(thigh);
   leg->addChild(calfTransform);
   leg->addChild(calf);
   leg->addChild(footTransform);
   leg->addChild(foot);

   SoTransform *leftTransform = new SoTransform;
   leftTransform->translation = SbVec3f(1, -4.25, 0);

   // Left leg
   SoSeparator *leftLeg = new SoSeparator;   
   leftLeg->addChild(leftTransform);
   leftLeg->addChild(leg);

   SoTransform *rightTransform = new SoTransform;
   rightTransform->translation.setValue(-1, -4.25, 0);

   // Right leg
   SoSeparator *rightLeg = new SoSeparator;   
   rightLeg->addChild(rightTransform);
   rightLeg->addChild(leg);

   // Parts for body
   SoTransform *bodyTransform = new SoTransform;    
   bodyTransform->translation.setValue(0.0, 3.0, 0.0);

   SoMaterial *bronze = new SoMaterial;
   bronze->ambientColor.setValue(.33, .22, .27);
   bronze->diffuseColor.setValue(.78, .57, .11);
   bronze->specularColor.setValue(.99, .94, .81);
   bronze->shininess = .28;

   SoCylinder *bodyCylinder = new SoCylinder;
   bodyCylinder->radius = 2.5;
   bodyCylinder->height = 6;

   // Construct body out of parts 
   SoSeparator *body = new SoSeparator;  
   body->addChild(bodyTransform);      
   body->addChild(bronze);
   body->addChild(bodyCylinder);
   body->addChild(leftLeg);
   body->addChild(rightLeg);

   // Head parts
   SoTransform *headTransform = new SoTransform;   
   headTransform->translation.setValue(0, 7.5, 0);
   headTransform->scaleFactor.setValue(1.5, 1.5, 1.5);

   SoMaterial *silver = new SoMaterial;
   silver->ambientColor.setValue(.2, .2, .2);
   silver->diffuseColor.setValue(.6, .6, .6);
   silver->specularColor.setValue(.5, .5, .5);
   silver->shininess = .5;

   SoSphere *headSphere = new SoSphere;

   // Construct head
   SoSeparator *head = new SoSeparator;      
   head->addChild(headTransform);
   head->addChild(silver);
   head->addChild(headSphere);
   
   // Robot is just head and body
   SoSeparator *robot = new SoSeparator;  
   robot->addChild(body);               
   robot->addChild(head);

// CODE FOR The Inventor Mentor ENDS HERE
//////////////////////////////////////////////////////////////

   return robot;
}

int
main(int , char **argv)
{
   // Initialize Inventor and Xt
   Widget myWindow = SoXt::init(argv[0]);
   if (myWindow == NULL) exit(1);

   SoSeparator *root = new SoSeparator;
   root->ref();

   // This function contains our code fragment.
   root->addChild(makeRobot());

   SoXtExaminerViewer *myViewer = 
            new SoXtExaminerViewer(myWindow);
   myViewer->setSceneGraph(root);
   myViewer->setTitle("Robot");
   myViewer->show();
   myViewer->viewAll();

   SoXt::show(myWindow);
   SoXt::mainLoop();
}


