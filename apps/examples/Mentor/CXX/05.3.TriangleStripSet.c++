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
 *  chapter 5, example 3.
 *
 *  This example creates a TriangleStripSet. It creates
 *  a pennant-shaped flag.
 *------------------------------------------------------------*/

#include <stdlib.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoTriangleStripSet.h>

//////////////////////////////////////////////////////////////
// CODE FOR The Inventor Mentor STARTS HERE

//
// Positions of all of the vertices:
//
static float vertexPositions[40][3] =
{
   {  0,   12,    0 }, {   0,   15,    0},
   {2.1, 12.1,  -.2 }, { 2.1, 14.6,  -.2},
   {  4, 12.5,  -.7 }, {   4, 14.5,  -.7},
   {4.5, 12.6,  -.8 }, { 4.5, 14.4,  -.8},
   {  5, 12.7,   -1 }, {   5, 14.4,   -1},
   {4.5, 12.8, -1.4 }, { 4.5, 14.6, -1.4},
   {  4, 12.9, -1.6 }, {   4, 14.8, -1.6},
   {3.3, 12.9, -1.8 }, { 3.3, 14.9, -1.8},
   {  3,   13, -2.0 }, {   3, 14.9, -2.0}, 
   {3.3, 13.1, -2.2 }, { 3.3, 15.0, -2.2},
   {  4, 13.2, -2.5 }, {   4, 15.0, -2.5},
   {  6, 13.5, -2.2 }, {   6, 14.8, -2.2},
   {  8, 13.4,   -2 }, {   8, 14.6,   -2},
   { 10, 13.7, -1.8 }, {  10, 14.4, -1.8},
   { 12,   14, -1.3 }, {  12, 14.5, -1.3},
   { 15, 14.9, -1.2 }, {  15,   15, -1.2},

   {-.5, 15,   0 }, { -.5, 0,   0},   // the flagpole
   {  0, 15,  .5 }, {   0, 0,  .5},
   {  0, 15, -.5 }, {   0, 0, -.5},
   {-.5, 15,   0 }, { -.5, 0,   0}
};


// Number of vertices in each strip.
static int32_t numVertices[2] =
{
   32, // flag
   8   // pole
};
 
// Colors for the 12 faces
static float colors[2][3] =
{
   { .5, .5,  1 }, // purple flag
   { .4, .4, .4 }, // grey flagpole
};

// Routine to create a scene graph representing a pennant.
SoSeparator *
makePennant()
{
   SoSeparator *result = new SoSeparator;
   result->ref();

   // A shape hints tells the ordering of polygons. 
   // This insures double sided lighting.
   SoShapeHints *myHints = new SoShapeHints;
   myHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
   result->addChild(myHints);

#ifdef IV_STRICT
   // This is the preferred code for Inventor 2.1 

   // Using the new SoVertexProperty node is more efficient
   SoVertexProperty *myVertexProperty = new SoVertexProperty;

   // Define colors for the strips
   for (int i=0; i<2; i++)
     myVertexProperty->orderedRGBA.set1Value(i, SbColor(colors[i]).getPackedValue());
   myVertexProperty->materialBinding = SoMaterialBinding::PER_PART;

   // Define coordinates for vertices
   myVertexProperty->vertex.setValues(0, 40, vertexPositions);

   // Define the TriangleStripSet, made of two strips.
   SoTriangleStripSet *myStrips = new SoTriangleStripSet;
   myStrips->numVertices.setValues(0, 2, numVertices);
 
   myStrips->vertexProperty.setValue(myVertexProperty);
   result->addChild(myStrips);

#else
   // Define colors for the strips
   SoMaterial *myMaterials = new SoMaterial;
   myMaterials->diffuseColor.setValues(0, 2, colors);
   result->addChild(myMaterials);
   SoMaterialBinding *myMaterialBinding = new SoMaterialBinding;
   myMaterialBinding->value = SoMaterialBinding::PER_PART;
   result->addChild(myMaterialBinding);

   // Define coordinates for vertices
   SoCoordinate3 *myCoords = new SoCoordinate3;
   myCoords->point.setValues(0, 40, vertexPositions);
   result->addChild(myCoords);

   // Define the TriangleStripSet, made of two strips.
   SoTriangleStripSet *myStrips = new SoTriangleStripSet;
   myStrips->numVertices.setValues(0, 2, numVertices);
   result->addChild(myStrips);
#endif

   result->unrefNoDelete();
   return result;
}

// CODE FOR The Inventor Mentor ENDS HERE
//////////////////////////////////////////////////////////////

int
main(int, char **argv)
{
   // Initialize Inventor and Xt
   Widget myWindow = SoXt::init(argv[0]);
   if (myWindow == NULL) exit(1);

   SoSeparator *root = makePennant();
   root->ref();

   SoXtExaminerViewer *myViewer = 
            new SoXtExaminerViewer(myWindow);
   myViewer->setSceneGraph(root);
   myViewer->setTitle("Triangle Strip Set: Pennant");
   myViewer->show();
   myViewer->viewAll();

   SoXt::show(myWindow);
   SoXt::mainLoop();
}

