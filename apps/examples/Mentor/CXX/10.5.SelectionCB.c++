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

/*-------------------------------------------------------------
 *  This is an example from The Inventor Mentor,
 *  chapter 10, example 5.
 *
 *  The scene graph has a sphere and a text 3D object. 
 *  A selection node is placed at the top of the scene graph. 
 *  When an object is selected, a selection callback is called
 *  to change the material color of that object.
 *------------------------------------------------------------*/

#include <stdlib.h>
#include <X11/Intrinsic.h>
#include <Inventor/Sb.h>
#include <Inventor/SoInput.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/SoXtRenderArea.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoText3.h>
#include <Inventor/nodes/SoTransform.h>

// global data
SoMaterial *textMaterial, *sphereMaterial;
static float reddish[] = {1.0, 0.2, 0.2};  // Color when selected
static float white[] = {0.8, 0.8, 0.8};    // Color when not selected

// This routine is called when an object gets selected. 
// We determine which object was selected, and change 
// that objects material color.
void
mySelectionCB(void *, SoPath *selectionPath)
{
   if (selectionPath->getTail()->
            isOfType(SoText3::getClassTypeId())) { 
      textMaterial->diffuseColor.setValue(reddish);
   } else if (selectionPath->getTail()->
            isOfType(SoSphere::getClassTypeId())) {
      sphereMaterial->diffuseColor.setValue(reddish);
   }
}

// This routine is called whenever an object gets deselected. 
// We determine which object was deselected, and reset 
// that objects material color.
void
myDeselectionCB(void *, SoPath *deselectionPath)
{
   if (deselectionPath->getTail()->
            isOfType(SoText3::getClassTypeId())) {
      textMaterial->diffuseColor.setValue(white);
   } else if (deselectionPath->getTail()->
            isOfType(SoSphere::getClassTypeId())) {
      sphereMaterial->diffuseColor.setValue(white);
   }
}


int
main(int argc, char **argv)
{
   // Initialize Inventor and Xt
   Widget myWindow = SoXt::init(argv[0]);
   if (myWindow == NULL) exit(1);

   // Create and set up the selection node
   SoSelection *selectionRoot = new SoSelection;
   selectionRoot->ref();
   selectionRoot->policy = SoSelection::SINGLE;
   selectionRoot-> addSelectionCallback(mySelectionCB);
   selectionRoot-> addDeselectionCallback(myDeselectionCB);

   // Create the scene graph
   SoSeparator *root = new SoSeparator;
   selectionRoot->addChild(root);

   SoPerspectiveCamera *myCamera = new SoPerspectiveCamera;
   root->addChild(myCamera);
   root->addChild(new SoDirectionalLight);

   // Add a sphere node
   SoSeparator *sphereRoot = new SoSeparator;
   SoTransform *sphereTransform = new SoTransform;
   sphereTransform->translation.setValue(17., 17., 0.);
   sphereTransform->scaleFactor.setValue(8., 8., 8.);
   sphereRoot->addChild(sphereTransform);

   sphereMaterial = new SoMaterial;
   sphereMaterial->diffuseColor.setValue(.8, .8, .8);
   sphereRoot->addChild(sphereMaterial);
   sphereRoot->addChild(new SoSphere);
   root->addChild(sphereRoot);

   // Add a text node
   SoSeparator *textRoot = new SoSeparator;
   SoTransform *textTransform = new SoTransform;
   textTransform->translation.setValue(0., -1., 0.);
   textRoot->addChild(textTransform);

   textMaterial = new SoMaterial;
   textMaterial->diffuseColor.setValue(.8, .8, .8);
   textRoot->addChild(textMaterial);
   SoPickStyle *textPickStyle = new SoPickStyle;
   textPickStyle->style.setValue(SoPickStyle::BOUNDING_BOX);
   textRoot->addChild(textPickStyle);
   SoText3 *myText = new SoText3;
   myText->string = "rhubarb";
   textRoot->addChild(myText);
   root->addChild(textRoot);

   SoXtRenderArea *myRenderArea = new SoXtRenderArea(myWindow);
   myRenderArea->setSceneGraph(selectionRoot);
   myRenderArea->setTitle("My Selection Callback");
   myRenderArea->show();

   // Make the camera see the whole scene
   const SbViewportRegion myViewport = 
            myRenderArea->getViewportRegion();
   myCamera->viewAll(root, myViewport, 2.0);

   SoXt::show(myWindow);
   SoXt::mainLoop();
}

