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
 |   $Revision: 1.2 $
 |
 |   Classes:
 |	SoMultipleCopy
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/actions/SoPickAction.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoSwitchElement.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/nodes/SoMultipleCopy.h>
#include <Inventor/nodes/SoSwitch.h>

SO_NODE_SOURCE(SoMultipleCopy);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoMultipleCopy::SoMultipleCopy()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoMultipleCopy);
    SO_NODE_ADD_FIELD(matrix, (SbMatrix::identity()));
    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoMultipleCopy::~SoMultipleCopy()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides method in SoNode to return FALSE.
//
// Use: public

SbBool
SoMultipleCopy::affectsState() const
//
////////////////////////////////////////////////////////////////////////
{
    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Typical action traversal.
//
// Use: extender

void
SoMultipleCopy::doAction(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    int		numIndices;
    const int	*indices;
    int		lastChild;
    int		index;
    SbBool	gettingBBox;

    SbVec3f	totalCenter(0,0,0);		// For bbox
    int		numCenters = 0, i;		// For bbox

    // We have to do some extra stuff here for computing bboxes
    gettingBBox = action->isOfType(SoGetBoundingBoxAction::getClassTypeId());

    // Determine which children to traverse, if any
    switch (action->getPathCode(numIndices, indices)) {
      case SoAction::NO_PATH:
      case SoAction::BELOW_PATH:
	lastChild = getNumChildren() - 1;
	break;

      case SoAction::IN_PATH:
	// If this node is in a path, that means the path goes to one
	// of its children. There's no need to traverse this path more
	// than once in this case.
	lastChild = indices[numIndices - 1];
	action->getState()->push();
	children->traverse(action, 0, lastChild);
	action->getState()->pop();
	return;

      case SoAction::OFF_PATH:
	// This differs from SoGroup: if the node is not on the
	// path, don't bother traversing its children. Effectively the
	// same as a separator to the rest of the graph.
	return;
    }

    for (index = 0; index < matrix.getNum(); index++) {
	action->getState()->push();

	// Set value in switch element to current index
	SoSwitchElement::set(action->getState(), index);

	// Transform
	SoModelMatrixElement::mult(action->getState(), this, matrix[index]);

	// Set the center correctly after each child
	if (gettingBBox) {
	    SoGetBoundingBoxAction *bba = (SoGetBoundingBoxAction *) action;

	    for (i = 0; i <= lastChild; i++) {
		children->traverse(action, i, i);
		if (bba->isCenterSet()) {
		    totalCenter += bba->getCenter();
		    numCenters++;
		    bba->resetCenter();
		}
	    }
	}
	else
	    children->traverse(action, 0, lastChild);

	action->getState()->pop();
    }

    if (gettingBBox && numCenters > 0)
	((SoGetBoundingBoxAction *) action)->setCenter(totalCenter/numCenters,
						       FALSE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Traversal for callback action.
//
// Use: extender

void
SoMultipleCopy::callback(SoCallbackAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoMultipleCopy::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Traversal for GL rendering
//
// Use: extender

void
SoMultipleCopy::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoMultipleCopy::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Traversal for picking
//
// Use: extender

void
SoMultipleCopy::pick(SoPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // Disable pick culling. If it is enabled, this is what happens:
    // if one of our children is a separator, we traverse it N times
    // to do the pick. For each of those times, the separator (if it
    // does pick culling) applies an SoGetBoundingBoxAction to the
    // current path (which goes through this multcopy). The multcopy
    // then computes the bbox of all N elements, so we get NxN
    // behavior, which takes too long.

    SbBool saveCullingFlag = action->isCullingEnabled();

    action->enableCulling(FALSE);

    SoMultipleCopy::doAction(action);

    action->enableCulling(saveCullingFlag);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Traversal for get bounding box
//
// Use: extender

void
SoMultipleCopy::getBoundingBox(SoGetBoundingBoxAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoMultipleCopy::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Traversal for handle event
//
// Use: extender

void
SoMultipleCopy::handleEvent(SoHandleEventAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // We need to traverse all of our children multiple times, since
    // someone could do weird things with switches that depends on
    // multiple traversal. But the matrix elements are not enabled for
    // this action, so we don't want to do any transformations.

    int		numIndices;
    const int	*indices;
    int		lastChild;
    int		index;

    // Determine which children to traverse, if any
    switch (action->getPathCode(numIndices, indices)) {
      case SoAction::NO_PATH:
      case SoAction::BELOW_PATH:
	lastChild = getNumChildren() - 1;
	break;

      case SoAction::IN_PATH:
	lastChild = indices[numIndices - 1];
	break;

      case SoAction::OFF_PATH:
	return;
    }

    for (index = 0; index < matrix.getNum(); index++) {
	action->getState()->push();
	SoSwitchElement::set(action->getState(), index);
	children->traverse(action, 0, lastChild);
	action->getState()->pop();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements getMatrix action.
//
// Use: extender

void
SoMultipleCopy::getMatrix(SoGetMatrixAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    int		numIndices;
    const int	*indices;

    // Only need to compute matrix if multipleCopy is a node in middle of
    // current path chain. We don't need to push or pop the state,
    // since this shouldn't have any effect on other nodes being
    // traversed.

    if (action->getPathCode(numIndices, indices) == SoAction::IN_PATH)
	children->traverse(action, 0, indices[numIndices - 1]);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements search action. If we are not searching all the
//    children we have to set the switch element correctly in case any
//    child contains a switch node that inherits the switch value. The
//    best approximation we can make here is to set it to traverse all
//    children. This is preferable to missing children.
//
// Use: extender

void
SoMultipleCopy::search(SoSearchAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    int		numIndices;
    const int	*indices;
    int		lastChild;

    // First see if the caller is searching for this node
    SoNode::search(action);

    if (action->isFound())
	return;

    // See if we're supposed to search only if the stuff under the
    // multipleCopy is relevant to the search path

    switch (action->getPathCode(numIndices, indices)) {

      case SoAction::NO_PATH:
      case SoAction::BELOW_PATH:
	lastChild = getNumChildren() - 1;
	break;

      case SoAction::IN_PATH:
	lastChild = indices[numIndices - 1];
	break;

      case SoAction::OFF_PATH:
	if (! action->isSearchingAll())
	    return;
	lastChild = getNumChildren() - 1;
	break;
    }

    action->getState()->push();

    // Set value in switch element to traverse all children
    SoSwitchElement::set(action->getState(), SO_SWITCH_ALL);

    children->traverse(action, 0, lastChild);

    action->getState()->pop();
}
