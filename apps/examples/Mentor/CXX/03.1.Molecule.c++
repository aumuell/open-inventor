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
 *  chapter 3, example 1.
 *
 *  This code shows how to create a molecule out of 3 spherical
 *  atoms.  The molecule illustrates how the ordering of nodes
 *  within a scene graph affects the rendered image.
 *
 *------------------------------------------------------------*/

#include <stdlib.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoTransform.h>

SoGroup *
makeWaterMolecule()
{
//////////////////////////////////////////////////////////////
// CODE FOR The Inventor Mentor STARTS HERE

   // Construct all parts
   SoGroup *waterMolecule = new SoGroup;  // water molecule

   SoGroup *oxygen = new SoGroup;         // oxygen atom
   SoMaterial *redPlastic = new SoMaterial;
   SoSphere *sphere1 = new SoSphere;

   SoGroup *hydrogen1 = new SoGroup;      // hydrogen atoms
   SoGroup *hydrogen2 = new SoGroup;
   SoTransform *hydrogenXform1 = new SoTransform;
   SoTransform *hydrogenXform2 = new SoTransform;
   SoMaterial *whitePlastic = new SoMaterial;
   SoSphere *sphere2 = new SoSphere;
   SoSphere *sphere3 = new SoSphere;

   // Set all field values for the oxygen atom
   redPlastic->ambientColor.setValue(1.0, 0.0, 0.0);  
   redPlastic->diffuseColor.setValue(1.0, 0.0, 0.0); 
   redPlastic->specularColor.setValue(0.5, 0.5, 0.5);
   redPlastic->shininess = 0.5;
   
   // Set all field values for the hydrogen atoms
   hydrogenXform1->scaleFactor.setValue(0.75, 0.75, 0.75);  
   hydrogenXform1->translation.setValue(0.0, -1.2, 0.0);  
   hydrogenXform2->translation.setValue(1.1852, 1.3877, 0.0);
   whitePlastic->ambientColor.setValue(1.0, 1.0, 1.0);  
   whitePlastic->diffuseColor.setValue(1.0, 1.0, 1.0); 
   whitePlastic->specularColor.setValue(0.5, 0.5, 0.5);
   whitePlastic->shininess = 0.5;

   // Create a hierarchy
   waterMolecule->addChild(oxygen);   
   waterMolecule->addChild(hydrogen1);   
   waterMolecule->addChild(hydrogen2);

   oxygen->addChild(redPlastic);
   oxygen->addChild(sphere1);
   hydrogen1->addChild(hydrogenXform1);
   hydrogen1->addChild(whitePlastic);
   hydrogen1->addChild(sphere2);
   hydrogen2->addChild(hydrogenXform2);
   hydrogen2->addChild(sphere3);

// CODE FOR The Inventor Mentor ENDS HERE
//////////////////////////////////////////////////////////////

   return waterMolecule;
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
   root->addChild(makeWaterMolecule());

   SoXtExaminerViewer *myViewer = 
            new SoXtExaminerViewer(myWindow);
   myViewer->setSceneGraph(root);
   myViewer->setTitle("H two O");
   myViewer->show();
   myViewer->viewAll();

   SoXt::show(myWindow);
   SoXt::mainLoop();
}

