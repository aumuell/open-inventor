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
 *  chapter 6, example 3.
 *
 *  This example renders arguments as text within an
 *  SoXTExaminerViewer.  It is a little fancier than 6.2.
 *------------------------------------------------------------*/

#include <stdlib.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoLinearProfile.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoProfileCoordinate2.h>
#include <Inventor/nodes/SoText3.h>

int
main(int argc, char **argv)
{

   Widget myWindow = SoXt::init(argv[0]);
   if(myWindow == NULL) exit(1);

   SoGroup *root = new SoGroup;
   root->ref();

   // Set up camera 
   SoPerspectiveCamera	*myCamera = new SoPerspectiveCamera;
   myCamera->position.setValue(0, -(argc - 1) / 2, 10);
   myCamera->nearDistance.setValue(5.0);
   myCamera->farDistance.setValue(15.0);
   root->addChild(myCamera);

   // Let's make the front of the text white, 
   // and the sides and back shiny yellow
   SoMaterial *myMaterial = new SoMaterial;
   SbColor colors[3];
   // diffuse
   colors[0].setValue(1, 1, 1);
   colors[1].setValue(1, 1, 0);
   colors[2].setValue(1, 1, 0);
   myMaterial->diffuseColor.setValues(0, 3, colors);

   // specular
   colors[0].setValue(1, 1, 1);
   // Note: Inventor 2.1 doesn't support multiple specular colors.
   /* 
     colors[1].setValue(1, 1, 0);
     colors[2].setValue(1, 1, 0);
     myMaterial->specularColor.setValues(0, 3, colors);
   */
   myMaterial->specularColor.setValue(colors[0]);
   myMaterial->shininess.setValue(.1);
   root->addChild(myMaterial);

   // Choose a font likely to exist.
   SoFont *myFont = new SoFont;
   myFont->name.setValue("Times-Roman");
   root->addChild(myFont);

   // Specify a beveled cross-section for the text
   SoProfileCoordinate2 *myProfileCoords = 
            new SoProfileCoordinate2;
   SbVec2f coords[4];
   coords[0].setValue( .00, .00);
   coords[1].setValue( .25, .25);
   coords[2].setValue(1.25, .25);
   coords[3].setValue(1.50, .00);
   myProfileCoords->point.setValues(0, 4, coords);
   root->addChild(myProfileCoords);

   SoLinearProfile *myLinearProfile = new SoLinearProfile;
   int32_t	index[4] ;
   index[0] = 0;
   index[1] = 1;
   index[2] = 2;
   index[3] = 3;
   myLinearProfile->index.setValues(0, 4, index);
   root->addChild(myLinearProfile);

   // Set the material binding to PER_PART
   SoMaterialBinding *myMaterialBinding = new SoMaterialBinding;
   myMaterialBinding->
            value.setValue(SoMaterialBinding::PER_PART);
   root->addChild(myMaterialBinding);

   // Add the text
   SoText3 *myText3 = new SoText3;
   myText3->string.setValue("Beveled Text");
   myText3->justification.setValue(SoText3::CENTER);
   myText3->parts.setValue(SoText3::ALL);
   
   root->addChild(myText3);

   SoXtExaminerViewer *myViewer = 
            new SoXtExaminerViewer(myWindow);
   myViewer->setSceneGraph(root);
   myViewer->setTitle("Complex 3D Text");
   myViewer->show();
   myViewer->viewAll();
   
   SoXt::show(myWindow);
   SoXt::mainLoop();
}
