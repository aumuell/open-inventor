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
 *  This is an example from the Inventor Mentor
 *  chapter 7, example 2.
 *
 *  This example illustrates using texture coordindates on
 *  a Face Set.
 *------------------------------------------------------------*/

#include <stdlib.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoFaceSet.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoTextureCoordinate2.h>
#include <Inventor/nodes/SoTextureCoordinateBinding.h>

int
main(int , char **argv)
{
   Widget myWindow = SoXt::init(argv[0]);
   if(myWindow == NULL) exit(1);

   SoSeparator *root = new SoSeparator;
   root->ref();

   // Choose a texture 
   SoTexture2 *brick = new SoTexture2;
   root->addChild(brick);
   brick->filename.setValue("/usr/share/src/Inventor/examples/data/brick.1.rgb");

#ifdef IV_STRICT
   // This is the preferred code for Inventor 2.1 

   // Using the new SoVertexProperty node is more efficient
   SoVertexProperty *myVertexProperty = new SoVertexProperty;

   // Define the square's spatial coordinates
   myVertexProperty->vertex.set1Value(0, SbVec3f(-3, -3, 0));
   myVertexProperty->vertex.set1Value(1, SbVec3f( 3, -3, 0));
   myVertexProperty->vertex.set1Value(2, SbVec3f( 3,  3, 0));
   myVertexProperty->vertex.set1Value(3, SbVec3f(-3,  3, 0));

   // Define the square's normal
   myVertexProperty->normal.set1Value(0, SbVec3f(0, 0, 1));

   // Define the square's texture coordinates
   myVertexProperty->texCoord.set1Value(0, SbVec2f(0, 0));
   myVertexProperty->texCoord.set1Value(1, SbVec2f(1, 0));
   myVertexProperty->texCoord.set1Value(2, SbVec2f(1, 1));
   myVertexProperty->texCoord.set1Value(3, SbVec2f(0, 1));

   // SoTextureCoordinateBinding node is now obsolete--in Inventor 2.1,
   // texture coordinates will always be generated if none are 
   // provided.
   /*
     SoTextureCoordinateBinding *tBind =
            new SoTextureCoordinateBinding;
     root->addChild(tBind);
     tBind->value = SoTextureCoordinateBinding::PER_VERTEX;
   */
   // Define normal binding
    myVertexProperty->normalBinding = SoNormalBinding::OVERALL;

   // Define a FaceSet
   SoFaceSet *myFaceSet = new SoFaceSet;
   root->addChild(myFaceSet);
   myFaceSet->numVertices.set1Value(0, 4);

   myFaceSet->vertexProperty.setValue(myVertexProperty);

#else
   // Define the square's spatial coordinates
   SoCoordinate3 *coord = new SoCoordinate3;
   root->addChild(coord);
   coord->point.set1Value(0, SbVec3f(-3, -3, 0));
   coord->point.set1Value(1, SbVec3f( 3, -3, 0));
   coord->point.set1Value(2, SbVec3f( 3,  3, 0));
   coord->point.set1Value(3, SbVec3f(-3,  3, 0));

   // Define the square's normal
   SoNormal *normal = new SoNormal;
   root->addChild(normal);
   normal->vector.set1Value(0, SbVec3f(0, 0, 1));

   // Define the square's texture coordinates
   SoTextureCoordinate2 *texCoord = new SoTextureCoordinate2;
   root->addChild(texCoord);
   texCoord->point.set1Value(0, SbVec2f(0, 0));
   texCoord->point.set1Value(1, SbVec2f(1, 0));
   texCoord->point.set1Value(2, SbVec2f(1, 1));
   texCoord->point.set1Value(3, SbVec2f(0, 1));

   // Define normal binding
   SoNormalBinding *nBind = new SoNormalBinding;
   root->addChild(nBind);
   nBind->value.setValue(SoNormalBinding::OVERALL);

   // SoTextureCoordinateBinding node is now obsolete--in Inventor 2.1,
   // texture coordinates will always be generated if none are 
   // provided.
   /*
     SoTextureCoordinateBinding *tBind =
            new SoTextureCoordinateBinding;
     root->addChild(tBind);
     tBind->value.setValue 
            (SoTextureCoordinateBinding::PER_VERTEX);
   */

   // Define a FaceSet
   SoFaceSet *myFaceSet = new SoFaceSet;
   root->addChild(myFaceSet);
   myFaceSet->numVertices.set1Value(0, 4);
#endif

   SoXtExaminerViewer *myViewer = 
            new SoXtExaminerViewer(myWindow);
   myViewer->setSceneGraph(root);
   myViewer->setTitle("Texture Coordinates");

   // In Inventor 2.1, if the machine does not have hardware texture
   // mapping, we must override the default drawStyle to display textures.
   myViewer->setDrawStyle(SoXtViewer::STILL, SoXtViewer::VIEW_AS_IS);

   myViewer->show();

   SoXt::show(myWindow);
   SoXt::mainLoop();
}
