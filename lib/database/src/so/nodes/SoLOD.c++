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

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   Classes:
 |	SoLOD
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/misc/SoState.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/elements/SoGLCacheContextElement.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoViewVolumeElement.h>
#include <Inventor/nodes/SoLOD.h>
#include <Inventor/nodes/SoShape.h>

SO_NODE_SOURCE(SoLOD);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoLOD::SoLOD()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoLOD);
    SO_NODE_ADD_FIELD(center, (SbVec3f(0,0,0)));
    SO_NODE_ADD_FIELD(range, (0));
    range.deleteValues(0);

    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor that takes approximate number of children.
//
// Use: public

SoLOD::SoLOD(int nChildren) : SoGroup(nChildren)
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoLOD);
    SO_NODE_ADD_FIELD(center, (SbVec3f(0,0,0)));
    SO_NODE_ADD_FIELD(range, (0));
    range.deleteValues(0);

    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoLOD::~SoLOD()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements typical traversal, determining child to traverse.
//
// Use: extender

void
SoLOD::doAction(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    int numIndices;
    const int *indices;
    SoAction::PathCode pc = action->getPathCode(numIndices,indices);
    if (pc == SoAction::IN_PATH) {
	// IN_PATH, let SoChildList take care of it:
	children->traverse(action);
    } else {
	int	childToTraverse = whichToTraverse(action);
	// Traverse just the one kid
	if (childToTraverse >= 0)
	    children->traverse(action, childToTraverse, childToTraverse);
    }
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does the get bounding box action.  This takes care of averaging
//    the centers of all children to get a combined center.
//
// Use: extender

void
SoLOD::getBoundingBox(SoGetBoundingBoxAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f	totalCenter(0,0,0);
    int		numCenters = 0;
    int		numIndices;
    const int	*indices;
    SoState	*state = action->getState();
    SoAction::PathCode pc = action->getPathCode(numIndices,indices);
    
    //If no children, quit:
    if ( getNumChildren() < 1 ) return;

    //If BELOW_PATH or NO_PATH, traverse all children to widen bbox 
    // do a push and pop before each traverse so as to not accumulate state
    if (pc == SoAction::NO_PATH || pc == SoAction::BELOW_PATH){
	for (int i = 1; i < getNumChildren(); i++) {
	    state->push();
	    children->traverse(action, i, i);
	    if (action->isCenterSet()) {
		totalCenter += action->getCenter();
		numCenters++;
		action->resetCenter();
	    }
	    state->pop();
	}
	// State from the first child is allowed to leak out.  NOTE!
	// CALLING whichToTraverse() IN THIS METHOD RESULTS IN A
	// SEVERE PERFORMANCE DEGRADATION!  Because calling
	// whichToTraverse causes dependencies on the model and view
	// matrices, which makes bounding box caches invalid whenever
	// the view changes, which makes render culling very slow.
	children->traverse(action, 0, 0);
	if (action->isCenterSet()) {
	    totalCenter += action->getCenter();
	    numCenters++;
	    action->resetCenter();
	}

	// Now, set the center to be the average. Don't re-transform the
	// average, which should already be transformed.
	if (numCenters != 0)
	    action->setCenter(totalCenter / numCenters, FALSE);
    }
    
    // if IN_PATH, traverse the (first) child in path to accumulate state:    	   
    if (pc == SoAction::IN_PATH){
#ifdef DEBUG
	if(numIndices > 1){
	    SoDebugError::post("SoLOD::getBoundingBox", 
		"IN_PATH traversal should not traverse multiple children");
	}
#endif /*DEBUG*/
	SoNode *kid = (SoNode *)children->get(indices[0]);
	if (numIndices>0 && kid->affectsState()) {
	    children->traverse(action, indices[0], indices[0]);
	}
	return;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements callback action for LOD nodes.
//
// Use: extender

void
SoLOD::callback(SoCallbackAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoLOD::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Traversal for rendering. Uses specific method, depending on path
//    code.
//
// Use: extender

void
SoLOD::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    int numIndices;
    const int *indices;

    switch (action->getPathCode(numIndices, indices)) {

      case SoAction::NO_PATH:
      case SoAction::BELOW_PATH:
	SoLOD::GLRenderBelowPath(action);
	break;

      case SoAction::IN_PATH:
	SoLOD::GLRenderInPath(action);
	break;

      case SoAction::OFF_PATH:
	SoLOD::GLRenderOffPath(action);
	break;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Rendering below a path:
//
// Use: extender

void
SoLOD::GLRenderBelowPath(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    int childToTraverse = whichToTraverse(action);

    if (childToTraverse >= 0) {
	action->pushCurPath(childToTraverse);
	((SoNode *)children->get(childToTraverse))->GLRenderBelowPath(action);
	action->popCurPath();
    }

    // Don't auto-cache above LOD nodes:
    SoGLCacheContextElement::shouldAutoCache(action->getState(), 
		SoGLCacheContextElement::DONT_AUTO_CACHE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Rendering in a path:
//
// Use: extender

void
SoLOD::GLRenderInPath(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    int numIndices;
    const int *indices;
    SoAction::PathCode pc = action->getPathCode(numIndices,indices);

    if (pc == SoAction::IN_PATH) {
	for (int i = 0; i < numIndices && !action->hasTerminated(); i++) {
	    action->pushCurPath(indices[i]);
	    SoNode *kid = (SoNode *)children->get(indices[i]);
	    kid->GLRenderInPath(action);
	    action->popCurPath(pc);
	}
    } else if (pc == SoAction::BELOW_PATH) { // This must be tail node
        SoLOD::GLRenderBelowPath(action);
    } else { // This should NEVER happen:
#ifdef DEBUG
	SoDebugError::post("SoLOD::GLRenderInPath",
			   "PathCode went to NO_PATH or OFF_PATH!");
#endif
    }

}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Rendering off a path:
//
// Use: extender

void
SoLOD::GLRenderOffPath(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    int childToTraverse = whichToTraverse(action);

    if (childToTraverse >= 0) {
	SoNode *kid = (SoNode *)children->get(childToTraverse);
	if (kid->affectsState()) {
	    action->pushCurPath(childToTraverse);
	    kid->GLRenderBelowPath(action);
	    action->popCurPath();
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements ray picking.
//
// Use: extender

void
SoLOD::rayPick(SoRayPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoLOD::doAction(action);
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Heart of this node; decides which child will be traversed
//
// Use: private

int
SoLOD::whichToTraverse(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    int		childToTraverse;
    int		numKids  = getNumChildren();
    int		numRange = range.getNum();
    SoState	*state   = action->getState();

    // If no children or 1 child, decision is easy
    if (numKids == 0)
	return -1;
    if (numKids == 1 || numRange == 0)
	childToTraverse = 0;

    // Need to decide on range and LOD stress
    else {
	// Transform center of LOD into world space:
	const SbMatrix &modelMtx = SoModelMatrixElement::get(state);
	SbVec3f worldSpaceCenter;
	modelMtx.multVecMatrix(center.getValue(), worldSpaceCenter);

	// And get eyepoint in world space:
	const SbVec3f &eyePoint =
	    SoViewVolumeElement::get(state).getProjectionPoint();

	SbVec3f diff = worldSpaceCenter - eyePoint;
	float d_sq = diff.dot(diff);

	// Get "stress", which is based on current complexity, size of
	// window, and field-of-view:
	// ??? MUST IMPLEMENT!

	// Figure out which child to use:
	for (childToTraverse = 0; childToTraverse < numRange;
	     childToTraverse++) {
	
	    float r = range[childToTraverse];
	    if (d_sq < r*r)
		break;
	}

	// Make sure don't run off the end:
	if (childToTraverse >= numKids)
	    childToTraverse = numKids-1;
    }
    return childToTraverse;
}







