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
 *  This is an example from the Inventor Toolmaker,
 *  chapter 2, example 6.
 *
 *  Source file for "Alternate" group node.
 *------------------------------------------------------------*/

#include <Inventor/misc/SoChildList.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/actions/SoPickAction.h>
#include <Inventor/actions/SoSearchAction.h>
#include "Alternate.h"

SO_NODE_SOURCE(Alternate);

//
// This initializes the Alternate class.
//

void
Alternate::initClass()
{
   // Initialize type id variables
   SO_NODE_INIT_CLASS(Alternate, SoGroup, "Group"); 
}

//
// Constructor
//

Alternate::Alternate()
{
   SO_NODE_CONSTRUCTOR(Alternate);
}

//
// Constructor that takes approximate number of children
//

Alternate::Alternate(int numChildren) : SoGroup(numChildren)
{
   SO_NODE_CONSTRUCTOR(Alternate);
}

//
// Destructor
//

Alternate::~Alternate()
{
}

//
// Each of these implements an action by calling the standard
// traversal method. Note that (as in the Glow node source) we prefix
// the call to doAction() with the name of the class, to avoid
// problems with derived classes.
//

void
Alternate::getBoundingBox(SoGetBoundingBoxAction *action)
{
   Alternate::doAction(action);
}

void
Alternate::GLRender(SoGLRenderAction *action)
{
   Alternate::doAction(action);
}

void
Alternate::handleEvent(SoHandleEventAction *action)
{
   Alternate::doAction(action);
}

void
Alternate::pick(SoPickAction *action)
{
   Alternate::doAction(action);
}

//
// This implements the traversal for the SoGetMatrixAction,
// which is handled a little differently: it does not traverse
// below the root node or tail of the path it is applied to.
// Therefore, we need to compute the matrix only if this group
// is in the middle of the current path chain or is off the path
// chain (since the only way this could be true is if the group
// is under a group that affects the chain).
//

void
Alternate::getMatrix(SoGetMatrixAction *action)
{
   int         numIndices;
   const int   *indices;

   // Use SoAction::getPathCode() to determine where this group
   // is in relation to the path being applied to (if any). (see
   // the comments in doAction() for details.)
   switch (action->getPathCode(numIndices, indices)) {

     case SoAction::NO_PATH:
     case SoAction::BELOW_PATH:
      // If there's no path, or we're off the end, do nothing
      break;

     case SoAction::OFF_PATH:
     case SoAction::IN_PATH:
      // If we are in the path chain or we affect nodes in the
      // path chain, traverse the children
      Alternate::doAction(action);
      break;
   }
}

//
// This implements the traversal for the SoSearchAction, which
// is also a little different. The search action is set to
// either traverse all nodes in the graph or just those that
// would be traversed during normal traversal. We need to check
// that flag before traversing our kids.
//

void
Alternate::search(SoSearchAction *action)
{
   // if the action is searching everything, then traverse all
   // of our children as SoGroup would
   if (action->isSearchingAll())
      SoGroup::search(action);

   else {
      // First, make sure this node is found if we are searching
      // for Alternate (or group) nodes
      SoNode::search(action);

      // Traverse the children in our usual way
      Alternate::doAction(action);
   }
}

//
// This implements typical action traversal for an Alternate
// node, skipping every other child.
//

void
Alternate::doAction(SoAction *action)
{
   // SoAction has a method called "getPathCode()" that returns
   // a code indicating how this node is related to the path(s)
   // the action is being applied to. This code is one of the
   // following:
   //
   // NO_PATH    = Not traversing a path (action was applied
   //                to a node) 
   // IN_PATH    = This node is in the path chain, but is not
   //                the tail node
   // BELOW_PATH = This node is the tail of the path chain or
   //                is below the tail
   // OFF_PATH   = This node is off to the left of some node in
   //                the path chain
   //
   // If getPathCode() returns IN_PATH, it returns (in its two
   // arguments) the indices of the next nodes in the paths.
   // (Remember that an action can be applied to a list of
   // paths.)

   // For the IN_PATH case, these will be set by getPathCode()
   // to contain the number of child nodes that are in paths and
   // the indices of those children, respectively. In the other
   // cases, they are not meaningful.
   int         numIndices;
   const int   *indices;

   // This will be set to the index of the last (rightmost)
   // child to traverse
   int         lastChildIndex;

   // If this node is in a path, see which of our children are
   // in paths, and traverse up to and including the rightmost
   // of these nodes (the last one in the "indices" array).
   if (action->getPathCode(numIndices, indices) ==
       SoAction::IN_PATH)
      lastChildIndex = indices[numIndices - 1];

   // Otherwise, consider all of the children
   else
      lastChildIndex = getNumChildren() - 1;

   // Now we are ready to traverse the children, skipping every
   // other one. For the SoGetBoundingBoxAction, however, we
   // have to do some extra work in between each pair of
   // children - we have to make sure the center points get
   // averaged correctly.
   if (action->isOfType(
            SoGetBoundingBoxAction::getClassTypeId())) {
      SoGetBoundingBoxAction *bba =
         (SoGetBoundingBoxAction *) action;
      SbVec3f  totalCenter(0.0, 0.0, 0.0);
      int      numCenters = 0;

      for (int i = 0; i <= lastChildIndex; i += 2) {
         children->traverse(bba, i);

         // If the traversal set a center point in the action,
         // add it to the total and reset for the next child.
         if (bba->isCenterSet()) {
            totalCenter += bba->getCenter();
            numCenters++;
            bba->resetCenter();
         }
      }
      // Now, set the center to be the average. Since the
      // centers were already transformed, there's no need to
      // transform the average.
      if (numCenters != 0)
         bba->setCenter(totalCenter / numCenters, FALSE);
   }

   // For all other actions, just traverse every other child
   else
      for (int i = 0; i <= lastChildIndex; i += 2)
         children->traverse(action, i);
}
