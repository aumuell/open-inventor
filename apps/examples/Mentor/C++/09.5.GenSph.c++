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

/*-----------------------------------------------------------
 *  This is an example from The Inventor Mentor,
 *  chapter 9, example 5.
 *
 *  Using a callback for generated primitives.
 *  A simple scene with a sphere is created.
 *  A callback is used to write out the triangles that
 *  form the sphere in the scene.
 *----------------------------------------------------------*/

#include <Inventor/SoDB.h>
#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSphere.h>

// Function prototypes
void printSpheres(SoNode *);
SoCallbackAction::Response printHeaderCallback(void *, 
   SoCallbackAction *, const SoNode *);
void printTriangleCallback(void *, SoCallbackAction *,
   const SoPrimitiveVertex *, const SoPrimitiveVertex *,
   const SoPrimitiveVertex *);
void printVertex(const SoPrimitiveVertex *);

//////////////////////////////////////////////////////////////
// CODE FOR The Inventor Mentor STARTS HERE

void
printSpheres(SoNode *root)
{
   SoCallbackAction myAction;

   myAction.addPreCallback(SoSphere::getClassTypeId(), 
            printHeaderCallback, NULL);
   myAction.addTriangleCallback(SoSphere::getClassTypeId(), 
            printTriangleCallback, NULL);

   myAction.apply(root);
}

SoCallbackAction::Response
printHeaderCallback(void *, SoCallbackAction *, 
      const SoNode *node)
{
   printf("\n Sphere ");
   // Print the node name (if it exists) and address
   if (! !node->getName())
      printf("named \"%s\" ", node->getName().getString());
   printf("at address %#x\n", node);

   return SoCallbackAction::CONTINUE;
}

void
printTriangleCallback(void *, SoCallbackAction *,
   const SoPrimitiveVertex *vertex1,
   const SoPrimitiveVertex *vertex2,
   const SoPrimitiveVertex *vertex3)
{
   printf("Triangle:\n");
   printVertex(vertex1);
   printVertex(vertex2);
   printVertex(vertex3);
}

void
printVertex(const SoPrimitiveVertex *vertex)
{
   const SbVec3f &point = vertex->getPoint();
   printf("\tCoords     = (%g, %g, %g)\n", 
               point[0], point[1], point[2]);

   const SbVec3f &normal = vertex->getNormal();
   printf("\tNormal     = (%g, %g, %g)\n", 
               normal[0], normal[1], normal[2]);
}

// CODE FOR The Inventor Mentor ENDS HERE
///////////////////////////////////////////////////////////////

main(int, char **)
{
   // Initialize Inventor
   SoDB::init();

   // Make a scene containing a red sphere
   SoSeparator *root = new SoSeparator;
   SoPerspectiveCamera *myCamera = new SoPerspectiveCamera;
   SoMaterial *myMaterial = new SoMaterial;
   root->ref();
   root->addChild(myCamera);
   root->addChild(new SoDirectionalLight);
   myMaterial->diffuseColor.setValue(1.0, 0.0, 0.0);   // Red
   root->addChild(myMaterial);
   root->addChild(new SoSphere);
   root->ref();

   // Write out the triangles that form the sphere in the scene
   printSpheres(root);

   root->unref();
   return 0;
}
