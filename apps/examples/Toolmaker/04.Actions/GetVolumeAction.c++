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
 *  chapter 4, example 2.
 *
 *  Source file for "GetVolumeAction" action.
 *------------------------------------------------------------*/

#include <Inventor/elements/SoComplexityElement.h>
#include <Inventor/elements/SoComplexityTypeElement.h>
#include <Inventor/elements/SoCoordinateElement.h>
#include <Inventor/elements/SoElements.h>
#include <Inventor/elements/SoFontNameElement.h>
#include <Inventor/elements/SoFontSizeElement.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoProfileCoordinateElement.h>
#include <Inventor/elements/SoProfileElement.h>
#include <Inventor/elements/SoSwitchElement.h>
#include <Inventor/elements/SoUnitsElement.h>
#include <Inventor/elements/SoViewVolumeElement.h>
#include <Inventor/elements/SoViewingMatrixElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoCoordinate4.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoProfile.h>
#include <Inventor/nodes/SoProfileCoordinate2.h>
#include <Inventor/nodes/SoProfileCoordinate3.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoTransformation.h>
#include "GetVolumeAction.h"

SO_ACTION_SOURCE(GetVolumeAction);

//
// Initializes the GetVolumeAction class. This is a one-time
// thing that is done after database initialization and before
// any instance of this class is constructed.
//

void
GetVolumeAction::initClass()
{
   // Initialize the run-time type variables
   SO_ACTION_INIT_CLASS(GetVolumeAction, SoAction);

   // Enable elements that are involved in volume computation.
   // Most of these deal with geometrix properties
   // (coordinates, profiles) or transformations (model matrix,
   // units). Some are needed for certain groups (switches,
   // level-of-detail) to function correctly.
   SO_ENABLE(GetVolumeAction, SoModelMatrixElement);
   SO_ENABLE(GetVolumeAction, SoComplexityElement);
   SO_ENABLE(GetVolumeAction, SoComplexityTypeElement);
   SO_ENABLE(GetVolumeAction, SoCoordinateElement);
   SO_ENABLE(GetVolumeAction, SoFontNameElement);
   SO_ENABLE(GetVolumeAction, SoFontSizeElement);
   SO_ENABLE(GetVolumeAction, SoProfileCoordinateElement);
   SO_ENABLE(GetVolumeAction, SoProfileElement);
   SO_ENABLE(GetVolumeAction, SoSwitchElement);
   SO_ENABLE(GetVolumeAction, SoUnitsElement);
   SO_ENABLE(GetVolumeAction, SoViewVolumeElement);
   SO_ENABLE(GetVolumeAction, SoViewingMatrixElement);
   SO_ENABLE(GetVolumeAction, SoViewportRegionElement);

   // Now we need to register methods to implement this action
   // for various node classes. We have created implementations
   // for two specific shape nodes, SoCube and SoSphere, so we
   // can register specific methods for those two classes. We
   // also want to make sure that group classes traverse their
   // children correctly for this action, so we will use a
   // method that calls doAction() to handle groups. Finally,
   // we need to make sure that relevant property nodes set up
   // the state correctly; we can use the same method that
   // calls doAction() for these classes, as well. We will use
   // the SO_ACTION_ADD_METHOD() macro to make this easier.

   // This registers a method to call for SoNode, so it will be
   // used for any node class that does not have a more
   // specific method registered for it. This makes sure that
   // there is always a method to call for any node. The
   // "nullAction" method is defined on SoAction for use in
   // cases like this.
   SO_ACTION_ADD_METHOD(SoNode,               nullAction);

   // These register methods for the two shapes that can
   // really handle the action
   SO_ACTION_ADD_METHOD(SoCube,               cubeVolume);
   SO_ACTION_ADD_METHOD(SoSphere,             sphereVolume);

   // Register the method that calls doAction() for all group
   // classes and for relevant properties (transformations,
   // coordinates, profiles, and so on).
   SO_ACTION_ADD_METHOD(SoCamera,             callDoAction);
   SO_ACTION_ADD_METHOD(SoComplexity,         callDoAction);
   SO_ACTION_ADD_METHOD(SoCoordinate3,        callDoAction);
   SO_ACTION_ADD_METHOD(SoCoordinate4,        callDoAction);
   SO_ACTION_ADD_METHOD(SoFont,               callDoAction);
   SO_ACTION_ADD_METHOD(SoGroup,              callDoAction);
   SO_ACTION_ADD_METHOD(SoProfile,            callDoAction);
   SO_ACTION_ADD_METHOD(SoProfileCoordinate2, callDoAction);
   SO_ACTION_ADD_METHOD(SoProfileCoordinate3, callDoAction);
   SO_ACTION_ADD_METHOD(SoTransformation,     callDoAction);
}

//
// Constructor
//

GetVolumeAction::GetVolumeAction()
{
   SO_ACTION_CONSTRUCTOR(GetVolumeAction);
}

//
// Destructor. Does nothing.
//

GetVolumeAction::~GetVolumeAction()
{
}

//
// Initiates action on a graph. This is called when the action
// is applied to a node, a path, or a path list. It gives us a
// chance to initialize things before beginning traversal.
//

void
GetVolumeAction::beginTraversal(SoNode *node)
{
   // Initialize volume to 0
   volume = 0.0;

   // Begin traversal at the given root node.
   traverse(node);
}

//
// This method implements the action for an SoCube node.
//

void
GetVolumeAction::cubeVolume(SoAction *action, SoNode *node)
{
   // The action is really an instance of GetVolumeAction
   GetVolumeAction *volumeAct = (GetVolumeAction *) action;

   // And the node pointer is really a cube:
   const SoCube    *cube = (const SoCube *) node;

   // Find the dimensions of the cube
   float width    = (cube->width.isIgnored()  ? 2.0 :
                     cube->width.getValue());
   float height   = (cube->height.isIgnored() ? 2.0 :
                     cube->height.getValue());
   float depth    = (cube->depth.isIgnored()  ? 2.0 :
                     cube->depth.getValue());

   // ...and the volume
   float cubeVol = width * height * depth;

   // Add the volume to the accumulated volume in the action
   volumeAct->addVolume(cubeVol);
}

//
// This method implements the action for an SoSphere node.
//

void
GetVolumeAction::sphereVolume(SoAction *action, SoNode *node)
{
   // The action is really an instance of GetVolumeAction
   GetVolumeAction *volumeAct = (GetVolumeAction *) action;

   // And the node pointer is really a sphere:
   const SoSphere  *sphere = (const SoSphere *) node;

   // Find the radius of the sphere
   float radius = (sphere->radius.isIgnored() ? 1.0 :
                   sphere->radius.getValue());

   // Compute the volume using our favorite formula that we all
   // remember from our math classes, right?
   float sphereVol = 4./3. * M_PI * radius * radius * radius;

   // Add the volume to the accumulated volume in the action
   volumeAct->addVolume(sphereVol);
}

//
// This method implements the action for all of the relevant
// non-shape node classes.
//

void
GetVolumeAction::callDoAction(SoAction *action, SoNode *node)
{
   node->doAction(action);
}

//
// This adds the given object-space volume to the total, first
// converting it to world space using the current model matrix.
//

void
GetVolumeAction::addVolume(float objectSpaceVolume)
{
   // Find the current modeling matrix
   const SbMatrix &modelMatrix =
      SoModelMatrixElement::get(state);

   // The determinant of the upper-left 3x3 of this matrix is
   // the conversion factor we need to go from object-space
   // volume to world space. Pretty cool, indeed.
   float objectToWorldFactor = modelMatrix.det3();

   // Add in the converted volume to our current volume
   volume += objectToWorldFactor * objectSpaceVolume;
}
