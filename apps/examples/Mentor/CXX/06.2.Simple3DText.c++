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
 *  chapter 6, example 2.
 *
 *  This example renders a globe and uses 3D text to label the
 *  continents Africa and Asia.
 *------------------------------------------------------------*/

#include <stdlib.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoText3.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTexture2Transform.h>

int
main(int, char **argv)
{
   Widget myWindow = SoXt::init(argv[0]);
   if(myWindow == NULL) exit(1);

   SoGroup *root = new SoGroup;
   root->ref();

   // Choose a font
   SoFont *myFont = new SoFont;
   myFont->name.setValue("Times-Roman");
   myFont->size.setValue(.2);
   root->addChild(myFont);

   // We'll color the front of the text white, and the sides 
   // dark grey. So use a materialBinding of PER_PART and
   // two diffuseColor values in the material node.
   SoMaterial        *myMaterial = new SoMaterial;
   SoMaterialBinding *myBinding = new SoMaterialBinding;
   myMaterial->diffuseColor.set1Value(0,SbColor(1,1,1));
   myMaterial->diffuseColor.set1Value(1,SbColor(.1,.1,.1));
   myBinding->value = SoMaterialBinding::PER_PART;
   root->addChild(myMaterial);
   root->addChild(myBinding);

   // Create the globe
   SoSeparator *sphereSep = new SoSeparator;
   SoTexture2  *myTexture2 = new SoTexture2;
   SoComplexity *sphereComplexity = new SoComplexity;
   sphereComplexity->value = 0.55;
   root->addChild(sphereSep);
   sphereSep->addChild(myTexture2);
   sphereSep->addChild(sphereComplexity);
   sphereSep->addChild(new SoSphere);
   myTexture2->filename = "/usr/share/src/Inventor/examples/data/globe.rgb";

   // Add Text3 for AFRICA, transformed to proper location.
   SoSeparator *africaSep = new SoSeparator;
   SoTransform *africaTransform = new SoTransform;
   SoText3 *africaText = new SoText3;
   africaTransform->rotation.setValue(SbVec3f(0,1,0),.4);
   africaTransform->translation.setValue(.25,.0,1.25);
   africaText->parts = SoText3::ALL;
   africaText->string = "AFRICA";
   root->addChild(africaSep);
   africaSep->addChild(africaTransform);
   africaSep->addChild(africaText);

   // Add Text3 for ASIA, transformed to proper location.
   SoSeparator *asiaSep = new SoSeparator;
   SoTransform *asiaTransform = new SoTransform;
   SoText3 *asiaText = new SoText3;
   asiaTransform->rotation.setValue(SbVec3f(0,1,0),1.5);
   asiaTransform->translation.setValue(.8,.6,.5);
   asiaText->parts = SoText3::ALL;
   asiaText->string = "ASIA";
   root->addChild(asiaSep);
   asiaSep->addChild(asiaTransform);
   asiaSep->addChild(asiaText);

   SoXtExaminerViewer *myViewer = 
            new SoXtExaminerViewer(myWindow);
   myViewer->setSceneGraph(root);
   myViewer->setTitle("3D Text");

   // In Inventor 2.1, if the machine does not have hardware texture
   // mapping, we must override the default drawStyle to display textures.
   myViewer->setDrawStyle(SoXtViewer::STILL, SoXtViewer::VIEW_AS_IS);

   myViewer->show();
   myViewer->viewAll();
   
   SoXt::show(myWindow);
   SoXt::mainLoop();
}
