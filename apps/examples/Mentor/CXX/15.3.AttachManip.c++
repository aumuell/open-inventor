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
 *  This is an example from the Inventor Mentor,
 *  volume 1, chapter 15, example 3.
 *
 *  Manipulator attachment example.
 *
 *  The scene graph has an SoWrapperKit, a cube and a sphere.
 *  A file containing a luxo lamp is read in as the 'contents'
 *  part of the SoWrapperKit.
 *  When the user picks on the SoWrapperKit (lamp), the kit's 
 *  "transform" part is replaced with an SoTransformBoxManip.
 *  Alternatively, when the user picks the sphere, the
 *  sphere's associated transform is replaced by an
 *  SoHandleBoxManip.  Picking the cube causes an 
 *  SoTrackballManip to replace the cube's transform.
 *  
 *  Manipulator callbacks are used to change
 *  the color of the object being manipulated.
 *  
 *  Note that for illustration purposes, the
 *  cube and SoWrapperKit already have transform nodes 
 *  associated with them; the sphere does not. In all cases, 
 *  the routine createTransformPath() is used to find the 
 *  transform node that affects the picked object.
 *-----------------------------------------------------------*/


#include <stdlib.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/manips/SoHandleBoxManip.h>
#include <Inventor/manips/SoTrackballManip.h>
#include <Inventor/manips/SoTransformBoxManip.h>
#include <Inventor/nodekits/SoWrapperKit.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoLight.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoTransform.h>

#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>

// function prototypes
void selectionCallback(void *, SoPath *);
void deselectionCallback(void *, SoPath *);
void dragStartCallback(void *, SoDragger *);
void dragFinishCallback(void *, SoDragger *);

// global data
SoSeparator *root;
SoHandleBoxManip    *myHandleBox;
SoTrackballManip    *myTrackball;
SoTransformBoxManip *myTransformBox;
SoPath *handleBoxPath    = NULL;
SoPath *trackballPath    = NULL;
SoPath *transformBoxPath = NULL;

int
main(int, char **argv)
{
   // Initialize Inventor and Xt
   Widget myWindow = SoXt::init(argv[0]);
   if (myWindow == NULL) exit(1);

   // create and set up the selection node
   SoSelection *selectionRoot = new SoSelection;
   selectionRoot->ref();
   selectionRoot->
      addSelectionCallback(selectionCallback, NULL);
   selectionRoot->
      addDeselectionCallback(deselectionCallback, NULL);

   // create the scene graph
   root = new SoSeparator;
   selectionRoot->addChild(root);

   // Read a file into contents of SoWrapperKit 
   // Translate it to the right.
   SoWrapperKit *myWrapperKit = new SoWrapperKit;
   root->addChild(myWrapperKit);
   SoInput myInput;
   if (!myInput.openFile("/usr/share/src/Inventor/examples/data/luxo.iv")) 
      exit (1);
   SoSeparator *objectFromFile = SoDB::readAll(&myInput);
   if (objectFromFile == NULL) 
      exit (1);
   myWrapperKit->setPart("contents",objectFromFile);
   myWrapperKit->set("transform { translation 3 -1 0 }");
   SoMaterial *wrapperMat 
      = (SoMaterial *) myWrapperKit->getPart("material",TRUE);
   wrapperMat->diffuseColor.setValue(.8, .8, .8);

   // Create a cube with its own transform.
   SoSeparator *cubeRoot = new SoSeparator;
   SoTransform *cubeXform = new SoTransform;
   cubeXform->translation.setValue(-4, 0, 0);
   root->addChild(cubeRoot);
   cubeRoot->addChild(cubeXform);

   SoMaterial *cubeMat = new SoMaterial;
   cubeMat->diffuseColor.setValue(.8, .8, .8);
   cubeRoot->addChild(cubeMat);
   cubeRoot->addChild(new SoCube);

   // add a sphere node without a transform
   // (one will be added when we attach the manipulator)
   SoSeparator *sphereRoot = new SoSeparator;
   SoMaterial *sphereMat = new SoMaterial;
   root->addChild(sphereRoot);
   sphereRoot->addChild(sphereMat);
   sphereRoot->addChild(new SoSphere);
   sphereMat->diffuseColor.setValue(.8, .8, .8);

   // create the manipulators
   myHandleBox = new SoHandleBoxManip;
   myHandleBox->ref();
   myTrackball = new SoTrackballManip;
   myTrackball->ref();
   myTransformBox = new SoTransformBoxManip;
   myTransformBox->ref();

   // Get the draggers and add callbacks to them. Note
   // that you don't put callbacks on manipulators. You put
   // them on the draggers which handle events for them. 
   SoDragger *myDragger;
   myDragger = myTrackball->getDragger();
   myDragger->addStartCallback(dragStartCallback,cubeMat);
   myDragger->addFinishCallback(dragFinishCallback,cubeMat);

   myDragger = myHandleBox->getDragger();
   myDragger->addStartCallback(dragStartCallback,sphereMat);
   myDragger->addFinishCallback(dragFinishCallback,sphereMat);

   myDragger = myTransformBox->getDragger();
   myDragger->addStartCallback(dragStartCallback,wrapperMat);
   myDragger->addFinishCallback(dragFinishCallback,wrapperMat);

   SoXtExaminerViewer *myViewer 
      = new SoXtExaminerViewer(myWindow);
   myViewer->setSceneGraph(selectionRoot);
   myViewer->setTitle("Attaching Manipulators");
   myViewer->show();
   myViewer->viewAll();

   SoXt::show(myWindow);
   SoXt::mainLoop();
}

// Is this node of a type that is influenced by transforms?
SbBool
isTransformable(SoNode *myNode)
{
   if (myNode->isOfType(SoGroup::getClassTypeId())
      || myNode->isOfType(SoShape::getClassTypeId())
      || myNode->isOfType(SoCamera::getClassTypeId())
      || myNode->isOfType(SoLight::getClassTypeId()))
      return TRUE;
   else 
      return FALSE;
}

//  Create a path to the transform node that affects the tail
//  of the input path.  Three possible cases:
//   [1] The path-tail is a node kit. Just ask the node kit for
//       a path to the part called "transform"
//   [2] The path-tail is NOT a group.  Search siblings of path
//       tail from right to left until you find a transform. If
//       none is found, or if another transformable object is 
//       found (shape,group,light,or camera), then insert a 
//       transform just to the left of the tail. This way, the 
//       manipulator only effects the selected object.
//   [3] The path-tail IS a group.  Search its children left to
//       right until a transform is found. If a transformable
//       node is found first, insert a transform just left of 
//       that node.  This way the manip will affect all nodes
//       in the group.
SoPath *
createTransformPath(SoPath *inputPath)
{
   int pathLength = inputPath->getLength();
   if (pathLength < 2) // Won't be able to get parent of tail
      return NULL;

   SoNode *tail = inputPath->getTail();

   // CASE 1: The tail is a node kit.
   // Nodekits have built in policy for creating parts.
   // The kit copies inputPath, then extends it past the 
   // kit all the way down to the transform. It creates the
   // transform if necessary.
   if (tail->isOfType(SoBaseKit::getClassTypeId())) {
      SoBaseKit *kit = (SoBaseKit *) tail;
      return kit->createPathToPart("transform",TRUE,inputPath);
   }

   SoTransform *editXf = NULL;
   SoGroup     *parent;
   SbBool      existedBefore = FALSE;

   // CASE 2: The tail is not a group.
   SbBool isTailGroup;
   isTailGroup = tail->isOfType(SoGroup::getClassTypeId());
   if (!isTailGroup) {
      // 'parent' is node above tail. Search under parent right
      // to left for a transform. If we find a 'movable' node
      // insert a transform just left of tail.  
      parent = (SoGroup *) inputPath->getNode(pathLength - 2);
      int tailIndx = parent->findChild(tail);

      for (int i = tailIndx; (i >= 0) && (editXf == NULL);i--){
         SoNode *myNode = parent->getChild(i);
         if (myNode->isOfType(SoTransform::getClassTypeId()))
            editXf = (SoTransform *) myNode;
         else if (i != tailIndx && (isTransformable(myNode)))
            break;
      }
      if (editXf == NULL) {
	 existedBefore = FALSE;
         editXf = new SoTransform;
         parent->insertChild(editXf, tailIndx);
      }
      else
	 existedBefore = TRUE;
   }
   // CASE 3: The tail is a group.
   else {
      // Search the children from left to right for transform 
      // nodes. Stop the search if we come to a movable node.
      // and insert a transform before it.
      parent = (SoGroup *) tail;
      int i;
      for (i = 0;
         (i < parent->getNumChildren()) && (editXf == NULL); 
	 i++) {
         SoNode *myNode = parent->getChild(i);
         if (myNode->isOfType(SoTransform::getClassTypeId()))
            editXf = (SoTransform *) myNode;
         else if (isTransformable(myNode))
            break;
      }
      if (editXf == NULL) {
	 existedBefore = FALSE;
         editXf = new SoTransform;
         parent->insertChild(editXf, i);
      }
      else 
	 existedBefore = TRUE;
   }

   // Create 'pathToXform.' Copy inputPath, then make last
   // node be editXf.
   SoPath *pathToXform = NULL;
   pathToXform = inputPath->copy();
   pathToXform->ref();
   if (!isTailGroup) // pop off the last entry.
      pathToXform->pop();
   // add editXf to the end
   int xfIndex   = parent->findChild(editXf);
   pathToXform->append(xfIndex);
   pathToXform->unrefNoDelete();

   return(pathToXform);
}

// This routine is called when an object
// gets selected. We determine which object
// was selected, then call replaceNode()
// to replace the object's transform with
// a manipulator.
void
selectionCallback(
   void *, // user data is not used
   SoPath *selectionPath)
{
   // Attach the manipulator.
   // Use the convenience routine to get a path to
   // the transform that effects the selected object.
   SoPath *xformPath = createTransformPath(selectionPath);
   if (xformPath == NULL) return;
   xformPath->ref();

   // Attach the handle box to the sphere,
   // the trackball to the cube
   // or the transformBox to the wrapperKit
   if (selectionPath->getTail()->isOfType(
        SoSphere::getClassTypeId())) {
      handleBoxPath = xformPath;
      myHandleBox->replaceNode(xformPath);
   }
   else if (selectionPath->getTail()->
        isOfType(SoCube::getClassTypeId())) {
      trackballPath = xformPath;
      myTrackball->replaceNode(xformPath);
   }
   else if (selectionPath->getTail()->
        isOfType(SoWrapperKit::getClassTypeId())) {
      transformBoxPath = xformPath;
      myTransformBox->replaceNode(xformPath);
   }
}

// This routine is called whenever an object gets
// deselected. It detaches the manipulator from
// the transform node, and removes it from the 
// scene graph that will not be visible.
void
deselectionCallback(
   void *, // user data is not used
   SoPath *deselectionPath)
{
   if (deselectionPath->getTail()->
        isOfType(SoSphere::getClassTypeId())) {
      myHandleBox->replaceManip(handleBoxPath,NULL);
      handleBoxPath->unref();
   }
   else if (deselectionPath->getTail()->
        isOfType(SoCube::getClassTypeId())) {
      myTrackball->replaceManip(trackballPath,NULL);
      trackballPath->unref();
   }
   else if (deselectionPath->getTail()->
        isOfType(SoWrapperKit::getClassTypeId())) {
      myTransformBox->replaceManip(transformBoxPath,NULL);
      transformBoxPath->unref();
   }
}

// This is called when a manipulator is
// about to begin manipulation.
void
dragStartCallback(
   void *myMaterial, // user data
   SoDragger *)         // callback data not used
{
   ((SoMaterial *) myMaterial)->diffuseColor=SbColor(1,.2,.2);
}

// This is called when a manipulator is
// done manipulating.
void
dragFinishCallback(
   void *myMaterial, // user data
   SoDragger *)    // callback data not used
{
   ((SoMaterial *) myMaterial)->diffuseColor=SbColor(.8,.8,.8);
}
