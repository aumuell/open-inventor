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
 *  chapter 7, example 3.
 *
 *  This example illustrates using texture functions to
 *  generate texture coordinates on a sphere.
 *  It draws three texture mapped spheres, each with a 
 *  different repeat frequency as defined by the fields of the 
 *  SoTextureCoordinatePlane node.
 *------------------------------------------------------------*/

#include <stdlib.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoTexture2Transform.h>
#include <Inventor/nodes/SoTextureCoordinatePlane.h>
#include <Inventor/nodes/SoTranslation.h>

#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>

int
main(int , char **argv)
{
   Widget myWindow = SoXt::init(argv[0]);
   if(myWindow == NULL) exit(1);

   SoSeparator *root = new SoSeparator;
   root->ref();

   // Choose a texture.
   SoTexture2 *faceTexture = new SoTexture2;
   root->addChild(faceTexture);
   faceTexture->filename.setValue("/usr/share/src/Inventor/examples/data/sillyFace.rgb");

   // Make the diffuse color pure white
   SoMaterial *myMaterial = new SoMaterial;
   myMaterial->diffuseColor.setValue(1,1,1);
   root->addChild(myMaterial);

   // This texture2Transform centers the texture about (0,0,0) 
   SoTexture2Transform *myTexXf = new SoTexture2Transform;
   myTexXf->translation.setValue(.5,.5);
   root->addChild(myTexXf);

   // Define a texture coordinate plane node.  This one will 
   // repeat with a frequency of two times per unit length.
   // Add a sphere for it to affect.
   SoTextureCoordinatePlane *texPlane1 = new SoTextureCoordinatePlane;
   texPlane1->directionS.setValue(SbVec3f(2,0,0));
   texPlane1->directionT.setValue(SbVec3f(0,2,0));
   root->addChild(texPlane1);
   root->addChild(new SoSphere);

   // A translation node for spacing the three spheres.
   SoTranslation            *myTranslation = new SoTranslation;
   myTranslation->translation.setValue(2.5,0,0);

   // Create a second sphere with a repeat frequency of 1.
   SoTextureCoordinatePlane *texPlane2 = new SoTextureCoordinatePlane;
   texPlane2->directionS.setValue(SbVec3f(1,0,0));
   texPlane2->directionT.setValue(SbVec3f(0,1,0));
   root->addChild(myTranslation);
   root->addChild(texPlane2);
   root->addChild(new SoSphere);

   // The third sphere has a repeat frequency of .5
   SoTextureCoordinatePlane *texPlane3 = new SoTextureCoordinatePlane;
   texPlane3->directionS.setValue(SbVec3f(.5,0,0));
   texPlane3->directionT.setValue(SbVec3f(0,.5,0));
   root->addChild(myTranslation);
   root->addChild(texPlane3);
   root->addChild(new SoSphere);

   SoXtExaminerViewer *myViewer = 
            new SoXtExaminerViewer(myWindow);
   myViewer->setSceneGraph(root);
   myViewer->setTitle("Texture Coordinate Plane");

   // In Inventor 2.1, if the machine does not have hardware texture
   // mapping, we must override the default drawStyle to display textures.
   myViewer->setDrawStyle(SoXtViewer::STILL, SoXtViewer::VIEW_AS_IS);

   myViewer->show();

   SoXt::show(myWindow);
   SoXt::mainLoop();
}
