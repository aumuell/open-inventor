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
 *  This is an example from the Inventor Toolmaker,
 *  chapter 8, example 19
 *
 *  Source file for "Coordinate3Manip"
 *----------------------------------------------------------*/

#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/actions/SoPickAction.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/draggers/SoDragPointDragger.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/nodes/SoGroup.h>

// Include file for our new class
#include "Coordinate3Manip.h"

SO_NODE_SOURCE(Coordinate3Manip);

//  Initializes the type ID for this manipulator node. This
//  should be called once after SoInteraction::init().
void
Coordinate3Manip::initClass()
{
   SO_NODE_INIT_CLASS(Coordinate3Manip, SoCoordinate3, "Coordinate3");
}


Coordinate3Manip::Coordinate3Manip()
{
   children = new SoChildList(this);

   SO_NODE_CONSTRUCTOR(Coordinate3Manip);

   // Create the new 'editIndex' field
   SO_NODE_ADD_FIELD(editIndex, (0));

   // Create the field sensor
   pointFieldSensor = new SoFieldSensor(
      &Coordinate3Manip::fieldSensorCB, this);
   pointFieldSensor->setPriority(0);
   pointFieldSensor->attach(&point);

   // Create a new SoDragPointDragger and use
   // it for our child-dragger.
   setDragger(new SoDragPointDragger);
}

Coordinate3Manip::~Coordinate3Manip()
{
   // Important to do this because dragger may have callbacks
   // to this node.
   setDragger(NULL);

   if (pointFieldSensor)
      delete pointFieldSensor;
   delete children;
}

// Sets the dragger to be the given node...
// Adds it as a child and adds a valueChangedCallback  
// on the child to tell this node when the dragger moves.
void
Coordinate3Manip::setDragger(SoDragger *newDragger)
{
   SoDragger *oldDragger = getDragger();
   if (oldDragger) {
      oldDragger->removeValueChangedCallback(
         &Coordinate3Manip::valueChangedCB,this);
      children->remove(0);
   }
	
   if (newDragger) {
      if (children->getLength() > 0)
	 children->set(0, newDragger);
      else
	 children->append(newDragger);
      // Call the fieldSensorCB to transfer our values 
      // into the new dragger.
      Coordinate3Manip::fieldSensorCB(this, NULL);
      newDragger->addValueChangedCallback(
	 &Coordinate3Manip::valueChangedCB,this);
   }
}

// Returns value of the current dragger.
SoDragger *
Coordinate3Manip::getDragger()
{
   if (children->getLength() > 0) {
      SoNode *n = (*children)[0];
      if (n->isOfType(SoDragger::getClassTypeId()))
         return ((SoDragger *) n);
      else {
#ifdef DEBUG
	 SoDebugError::post("Coordinate3Manip::getDragger",
	 "Child is not a dragger!");
#endif
      }
   }
   return    NULL;
}

// Description:
//    Replaces the tail of the path with this manipulator.
//
//    [1] Tail of fullpath must be correct type, or we return.
//    [2] If path has a nodekit, we try to use setPart() to insert manip.
//    otherwise:
//    [3] Path must be long enough, or we return without replacing.
//    [4] Second to last node must be a group, or we return without replacing.
//    [5] Copy values from node we are replacing into this manip
//    [6] Replace this manip as the child.
//    [7] Do not ref or unref anything. Assume that the user knows what
//        they're doing.
//    [8] Do not fiddle with either node's field connections. Assume that the
//        user knows what they're doing.
//
SbBool
Coordinate3Manip::replaceNode(SoPath *inPath)
{
   SoFullPath *myFullPath = (SoFullPath *) inPath;

   SoNode     *myFullPTail = myFullPath->getTail();
   if (myFullPTail->isOfType(SoCoordinate3::getClassTypeId()) == FALSE ) {
#ifdef DEBUG
      SoDebugError::post("Coordinate3Manip::replaceNode", 
         "End of path is not an SoCoordinate3");
#endif
      return FALSE;
   }

   SoNode *pTail = inPath->getTail();
   if ( pTail->isOfType(SoBaseKit::getClassTypeId())) {

      // Okay, we've got a nodekit here! Let's do this right...
      // If myFullPTail is a part in the kit, then we've got to follow
      // protocol and let the kit set the part itself.
      SoBaseKit *lastKit = (SoBaseKit *) ((SoNodeKitPath *)inPath)->getTail();
      SbString partName = lastKit->getPartString(inPath);
      if ( partName != "" ) {
         SoCoordinate3 *oldPart 
	    = (SoCoordinate3 *) lastKit->getPart(partName, TRUE); 
         if (oldPart != NULL) {

	    // detach the sensor while copying the values.
	    pointFieldSensor->detach();
            point = oldPart->point;
            Coordinate3Manip::fieldSensorCB(this, NULL);
	    pointFieldSensor->attach(&point);

            lastKit->setPart(partName, this);
            return TRUE;
         }
         else {
            // Although the part's there, we couldn't get at it.
            // Some kind of problem going on
            return FALSE;
         }
      }
      // If it's not a part, that means it's contained within a subgraph
      // underneath a part. For example, it's within the 'contents'
      // separator of an SoWrapperKit. In that case, the nodekit doesn't
      // care and we just continue on through...
   }

   if ( myFullPath->getLength() < 2 ) {
#ifdef DEBUG
      SoDebugError::post("Coordinate3Manip::replaceNode",
         "Path is too short!");
#endif
      return FALSE;
   }

   SoNode      *parent = myFullPath->getNodeFromTail( 1 );
   if (parent->isOfType( SoGroup::getClassTypeId() ) == FALSE ) {
#ifdef DEBUG
      SoDebugError::post("Coordinate3Manip::replaceNode",
         "Parent node is not a group.!");
#endif
      return FALSE;
   }

   ref();

   // detach the sensor while copying the values.
   pointFieldSensor->detach();
   point = ((SoCoordinate3 *)myFullPTail)->point;
   Coordinate3Manip::fieldSensorCB(this, NULL);
   pointFieldSensor->attach(&point);

   ((SoGroup *)parent)->replaceChild(myFullPTail, this);

   unrefNoDelete();
   return TRUE;
}

// Replaces tail of path (which should be this manipulator)
// with the given SoCoordinate3 node.
//
//    [1] Tail of fullpath must be this node, or we return.
//    [2] If path has a nodekit, we try to use setPart() to insert new node.
//    otherwise:
//    [3] Path must be long enough, or we return without replacing.
//    [4] Second to last node must be a group, or we return without replacing.
//    [5] Copy values from node we are replacing into this manip
//    [6] Replace this manip as the child.
//    [7] Do not ref or unref anything. Assume that the user knows what
//        they're doing.
//    [8] Do not fiddle with either node's field connections. Assume that the
//        user knows what they're doing.
//
SbBool
Coordinate3Manip::replaceManip(SoPath *path, 
   SoCoordinate3 *newOne) const
{
   SoFullPath *myFullPath = (SoFullPath *) path;

   SoNode     *myFullPTail = myFullPath->getTail();
   if (myFullPTail != this ) {
#ifdef DEBUG
      SoDebugError::post("Coordinate3Manip::replaceManip",
         "Child to replace is not this manip!");
#endif
      return FALSE;
   }

   SoNode *pTail = path->getTail();
   if ( pTail->isOfType(SoBaseKit::getClassTypeId())) {

      // Okay, we've got a nodekit here! Let's do this right...
      // If myFullPTail is a part in the kit, then we've got to follow
      // protocol and let the kit set the part itself.
      SoBaseKit *lastKit = (SoBaseKit *) ((SoNodeKitPath *)path)->getTail();
      SbString partName = lastKit->getPartString(path);
      if ( partName != "" ) {
   
         if (newOne == NULL)
	    newOne = new SoCoordinate3;
	 newOne->ref();

	 newOne->point = point;
      
	 lastKit->setPart(partName, newOne);
	 newOne->unrefNoDelete();
         return TRUE;
      }
      // If it's not a part, that means it's contained within a subgraph
      // underneath a part. For example, it's within the 'contents'
      // separator of an SoWrapperKit. In that case, the nodekit doesn't
      // care and we just continue on through...
   }

   if ( myFullPath->getLength() < 2 ) {
#ifdef DEBUG
      SoDebugError::post("Coordinate3Manip::replaceManip", 
         "Path is too short!");
#endif
      return FALSE;
   }
   SoNode      *parent = myFullPath->getNodeFromTail(1);
   if (!parent->isOfType(SoGroup::getClassTypeId())) {
#ifdef DEBUG
      SoDebugError::post("Coordinate3Manip::replaceManip",
         "Parent node is not a group.!");
#endif
      return FALSE;
   }

   if (newOne == NULL)
      newOne = new SoCoordinate3;
   newOne->ref();
   newOne->point         = point;
   ((SoGroup *)parent)->replaceChild((Coordinate3Manip *)this, newOne);
   newOne->unrefNoDelete();

   return TRUE;
}


//    Creates and returns an exact copy...
SoNode *
Coordinate3Manip::copy(SbBool copyConnections) const
{
   // Create a copy of the node and fieldData
   Coordinate3Manip *newManip = (Coordinate3Manip *) 
      SoCoordinate3::copy(copyConnections);

   // Copy the children
   for (int i = 0; i < children->getLength(); i++)
      newManip->children->append(
	 (*children)[i]->copy(copyConnections));

   return newManip;
}

//    Returns the child list...
SoChildList *
Coordinate3Manip::getChildren() const
{
    return children;
}

void 
Coordinate3Manip::doAction(SoAction *action)
{
   int         numIndices;
   const int   *indices;

   if (action->getPathCode(numIndices, indices) 
      == SoAction::IN_PATH)
      children->traverse(action, 0, indices[numIndices - 1]);
   else
      children->traverse(action);
}

// These functions implement all actions for Coordinate3Manip.
void
Coordinate3Manip::getMatrix(SoGetMatrixAction *action)
{
   int         numIndices;
   const int   *indices;

   switch (action->getPathCode(numIndices, indices)) {
      case SoAction::NO_PATH:
         break;
      case SoAction::IN_PATH:
         children->traverse(action, 0,indices[numIndices - 1]);
         break;
      case SoAction::BELOW_PATH:
         break;
      case SoAction::OFF_PATH:
         children->traverse(action);
         break;
   }
}

void 
Coordinate3Manip::callback(SoCallbackAction *action)
{ 
   Coordinate3Manip::doAction(action);
   SoCoordinate3::callback(action);
}

void 
Coordinate3Manip::getBoundingBox(
   SoGetBoundingBoxAction *action)
{ 
   SbVec3f     totalCenter(0,0,0);
   int         numCenters = 0;
   int         numIndices;
   const int   *indices;
   int         lastChild;

   if (action->getPathCode(numIndices, indices) 
      == SoAction::IN_PATH)
      lastChild = indices[numIndices - 1];
   else
      lastChild = getNumChildren() - 1;

   // Traverse the children
   for (int i = 0; i <= lastChild; i++) {
      children->traverse(action, i, i);
      if (action->isCenterSet()) {
         totalCenter += action->getCenter();
         numCenters++;
         action->resetCenter();
      }
   }

   // Traverse this as an SoCoordinate3
   SoCoordinate3::getBoundingBox(action);
   if (action->isCenterSet()) {
      totalCenter += action->getCenter();
      numCenters++;
      action->resetCenter();
   }

   // Now, set the center to be the average:
   if (numCenters != 0)
      action->setCenter(totalCenter / numCenters, FALSE);
}

void 
Coordinate3Manip::GLRender(SoGLRenderAction *action)
{
   Coordinate3Manip::doAction(action); 
   SoCoordinate3::GLRender(action);
}

void 
Coordinate3Manip::handleEvent(SoHandleEventAction *action)
{ 
   Coordinate3Manip::doAction(action); 
   SoCoordinate3::handleEvent(action);
}

void 
Coordinate3Manip::pick(SoPickAction *action)
{ 
  Coordinate3Manip::doAction(action); 
  SoCoordinate3::pick(action);
}

void 
Coordinate3Manip::search(SoSearchAction *action)
{ 
    // See if the caller is searching for this
    SoCoordinate3::search(action);
    // DONT TRAVERSE CHILDREN DURING SEARCH!!! THEY ARE PRIVATE
}

void
Coordinate3Manip::valueChangedCB(void *inManip, 
   SoDragger *inDragger)
{
   Coordinate3Manip *manip = (Coordinate3Manip *) inManip;

   SbMatrix motMat = inDragger->getMotionMatrix();
   SbVec3f location = motMat[3];

   // Disconnect the field sensor
   manip->pointFieldSensor->detach();

   int ind = (int) manip->editIndex.getValue();

   // Set value of the point if it's different.
   if (ind < manip->point.getNum()) {
      if (manip->point[ind] != location)
         manip->point.set1Value(ind,location);
   }

   // Reconnect the field sensors
   manip->pointFieldSensor->attach(&(manip->point));
}

void
Coordinate3Manip::fieldSensorCB(void *inManip, SoSensor *)
{
   Coordinate3Manip *manip = (Coordinate3Manip *) inManip;

   int ind = manip->editIndex.getValue();

   // Set value of the point if it's different.
   if (ind < manip->point.getNum()) {

      SoDragger *dragger = manip->getDragger();

      if (dragger) {
         SbVec3f location = manip->point[ind];
         SbMatrix newMat = dragger->getMotionMatrix();
         newMat[3][0] = location[0];
         newMat[3][1] = location[1];
         newMat[3][2] = location[2];

         dragger->setMotionMatrix(newMat);
      }
   }
}

