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

#include <Inventor/SoDB.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/details/SoCylinderDetail.h>
#include <Inventor/details/SoFaceDetail.h>
#include <Inventor/details/SoDetail.h>
#include <Inventor/nodes/SoNodes.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>


main(int argc, char **argv)
{
    SoInput sceneInput;
    SoGroup *root;

    SoDB::init();

    // Open and read input scene graph
    if (! sceneInput.openFile(argv[1])) {
        fprintf(stderr, "Cannot open %s\n", argv[1]);
        return 1;
    }

    root = SoDB::readAll(&sceneInput);
    if (root == NULL) {
        fprintf(stderr, "Problem reading data\n");
        return 1;
    }


{ 
   SbViewportRegion myViewport;
   SoTransform *myTransform;

   SoGetBoundingBoxAction bboxAction(myViewport);
   bboxAction.apply(root);
   myTransform->center = bboxAction.getCenter(); 
}


{ 
   SoWriteAction myAction;
   FILE *fp;

   myAction.getOutput()->setBinary(TRUE);
   myAction.getOutput()->setFilePointer(fp);
   myAction.apply(root);
}

{
   SoWriteAction myAction;
   
   myAction.getOutput()->setBinary(FALSE);
   myAction.getOutput()->openFile("myFile.iv");
   myAction.apply(root);
   myAction.getOutput()->closeFile();
}


{ // Picking fragments

// 
   SbViewportRegion myViewport;
   SoRayPickAction myPickAction(myViewport);

// **tested**
   SoPath *pathToPickedObject;

   const SoPickedPoint *myPickedPoint = 
            myPickAction.getPickedPoint();
   if (myPickedPoint != NULL)
      pathToPickedObject = myPickedPoint->getPath();

// **tested**
   const SoDetail *pickDetail = myPickedPoint->getDetail();
   if (pickDetail != NULL && pickDetail->getTypeId() == 
                        SoCylinderDetail::getClassTypeId()) {
      // Picked object is a cylinder
      SoCylinderDetail *cylDetail = 
               (SoCylinderDetail *) pickDetail;

      // See if top of the cylinder was hit
      if (cylDetail->getPart() == SoCylinder::TOP) {
         printf("Top of cylinder was hit\n");
      }
   }

}

}

//   This function finds the closest vertex to an intersection 
//   point on a shape made of faces, passed in the "pickedPoint"
//   argument. It returns the SoCoordinate3 node containing 
//   the vertex's coordinates in the "coordNode" argument and 
//   the index of the vertex in that node in the "closestIndex" 
//   argument. If the shape is not made of faces or there were 
//   any other problems, this returns FALSE.

static SbBool
findClosestVertex(const SoPickedPoint *pickedPoint,
            SoCoordinate3 *&coordNode, int &closestIndex)
{
   const SoDetail *pickDetail = pickedPoint->getDetail();

   if (pickDetail != NULL && pickDetail->getTypeId() == 
                             SoFaceDetail::getClassTypeId()) {
      // Picked object is made of faces
      SoFaceDetail *faceDetail = (SoFaceDetail *) pickDetail;

      // Find the coordinate node that is used for the faces. 
      // Assume that it's the last SoCoordinate3 node traversed 
      // before the picked shape.
      SoSearchAction  mySearchAction;
      mySearchAction.setType(SoCoordinate3::getClassTypeId());
      mySearchAction.setInterest(SoSearchAction::LAST);
      mySearchAction.apply(pickedPoint->getPath());

      if (mySearchAction.getPath() != NULL) {  // We found one
         coordNode = (SoCoordinate3 *) 
                     mySearchAction.getPath()->getTail();

         // Get the intersection point in the object space 
         // of the picked shape
         SbVec3f objIntersect = pickedPoint->getObjectPoint();

         // See which of the points of the face is the closest 
         // to the intersection point
         float minDistance = 1e12;
         closestIndex = -1;
         for (int i = 0; i < faceDetail->getNumPoints(); i++) {
            int pointIndex = 
                  faceDetail->getPoint(i)->getCoordinateIndex();
            float curDistance = (coordNode->point[pointIndex] -
                  objIntersect).length();
            if (curDistance < minDistance) {
               closestIndex = pointIndex;
               minDistance    = curDistance;
            }
         }
   
         if (closestIndex >= 0)
            return TRUE;
      }
   }

   return FALSE;
}




