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
 *  chapter 5, example 6.
 *
 *  This example shows the effect of different order of
 *  operation of transforms.  The left object is first
 *  scaled, then rotated, and finally translated to the left.  
 *  The right object is first rotated, then scaled, and finally
 *  translated to the right.
 *------------------------------------------------------------*/

#include <stdlib.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/SoDB.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>

int
main(int, char **argv)
{
   // Initialize Inventor and Xt
   Widget myWindow = SoXt::init(argv[0]);
   if (myWindow == NULL) exit(1);

   SoSeparator *root = new SoSeparator;
   root->ref();

   // Create two separators, for left and right objects.
   SoSeparator *leftSep = new SoSeparator;
   SoSeparator *rightSep = new SoSeparator;
   root->addChild(leftSep);
   root->addChild(rightSep);

   // Create the transformation nodes
   SoTranslation *leftTranslation  = new SoTranslation;
   SoTranslation *rightTranslation = new SoTranslation;
   SoRotationXYZ *myRotation = new SoRotationXYZ;
   SoScale *myScale = new SoScale;

   // Fill in the values
   leftTranslation->translation.setValue(-1.0, 0.0, 0.0);
   rightTranslation->translation.setValue(1.0, 0.0, 0.0);
   myRotation->angle = M_PI/2;   // 90 degrees
   myRotation->axis = SoRotationXYZ::X;
   myScale->scaleFactor.setValue(2., 1., 3.);

   // Add transforms to the scene.
   leftSep->addChild(leftTranslation);   // left graph
   leftSep->addChild(myRotation);        // then rotated
   leftSep->addChild(myScale);           // first scaled

   rightSep->addChild(rightTranslation); // right graph
   rightSep->addChild(myScale);          // then scaled
   rightSep->addChild(myRotation);       // first rotated

   // Read an object from file. (as in example 4.2.Lights)
   SoInput myInput;
   if (!myInput.openFile("/usr/share/src/Inventor/examples/data/temple.iv")) 
      exit (1);
   SoSeparator *fileContents = SoDB::readAll(&myInput);
   if (fileContents == NULL) 
      exit (1);

   // Add an instance of the object under each separator.
   leftSep->addChild(fileContents);
   rightSep->addChild(fileContents);

   // Construct a renderArea and display the scene.
   SoXtExaminerViewer *myViewer = 
            new SoXtExaminerViewer(myWindow);
   myViewer->setSceneGraph(root);
   myViewer->setTitle("Transform Ordering");
   myViewer->viewAll();
   myViewer->show();

   SoXt::show(myWindow);
   SoXt::mainLoop();
}
