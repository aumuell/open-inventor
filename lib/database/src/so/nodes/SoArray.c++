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
 |	SoArray
 |
 |   Author(s)		: Paul S. Strauss
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
#include <Inventor/nodes/SoArray.h>
#include <Inventor/nodes/SoSwitch.h>

SO_NODE_SOURCE(SoArray);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoArray::SoArray()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoArray);

    SO_NODE_ADD_FIELD(numElements1,	(1));
    SO_NODE_ADD_FIELD(numElements2,	(1));
    SO_NODE_ADD_FIELD(numElements3,	(1));
    SO_NODE_ADD_FIELD(separation1,	(SbVec3f(1.0, 0.0, 0.0)));
    SO_NODE_ADD_FIELD(separation2,	(SbVec3f(0.0, 1.0, 0.0)));
    SO_NODE_ADD_FIELD(separation3,	(SbVec3f(0.0, 0.0, 1.0)));
    SO_NODE_ADD_FIELD(origin,		(FIRST));

    // Set up static info for enumerated type field
    SO_NODE_DEFINE_ENUM_VALUE(Origin, FIRST);
    SO_NODE_DEFINE_ENUM_VALUE(Origin, CENTER);
    SO_NODE_DEFINE_ENUM_VALUE(Origin, LAST);

    // Set up info in enumerated type field
    SO_NODE_SET_SF_ENUM_TYPE(origin, Origin);

    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoArray::~SoArray()
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
SoArray::affectsState() const
//
////////////////////////////////////////////////////////////////////////
{
    return FALSE;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements typical action traversal.
//
// Use: extender

void
SoArray::doAction(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    int		numIndices;
    const int	*indices;
    int		lastChild;
    int		n1, n2, n3, i1, i2, i3, curIndex;
    SbVec3f	sepVec1, sepVec2, sepVec3;
    SbBool	translateArray, gettingBBox;

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
	// This differs from SoGroup: if the array is not on the
	// path, don't bother traversing its children. Effectively the
	// same as a separator to the rest of the graph.
	return;
    }

    n1 = numElements1.getValue();
    n2 = numElements2.getValue();
    n3 = numElements3.getValue();

    translateArray = (! origin.isIgnored() && origin.getValue() != FIRST);

    if (translateArray) {
	SbVec3f vecToCenter = -(separation1.getValue() * (n1 - 1) +
				separation2.getValue() * (n2 - 1) +
				separation3.getValue() * (n3 - 1));

	if (origin.getValue() == CENTER)
	    vecToCenter *= 0.5;

	action->getState()->push();

	// Use model matrix to translate the array to the correct place
	SoModelMatrixElement::translateBy(action->getState(),
					  this, vecToCenter);
    }

    curIndex = 0;
    sepVec3.setValue(0.0, 0.0, 0.0);
    for (i3 = 0; i3 < n3; i3++) {

	sepVec2 = sepVec3;
	for (i2 = 0; i2 < n2; i2++) {

	    sepVec1 = sepVec2;
	    for (i1 = 0; i1 < n1; i1++) {

		action->getState()->push();

		// Set value in switch element to current index
		SoSwitchElement::set(action->getState(), curIndex++);

		// Translate element to correct place
		SoModelMatrixElement::translateBy(action->getState(),
						  this, sepVec1);

		// Set the center correctly after each child
		if (gettingBBox) {
		    SoGetBoundingBoxAction *bba =
			(SoGetBoundingBoxAction *) action;

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

		sepVec1 += separation1.getValue();
	    }
	    sepVec2 += separation2.getValue();
	}
	sepVec3 += separation3.getValue();
    }

    // Restore state if it was pushed because of centering translation
    if (translateArray)
	action->getState()->pop();

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
SoArray::callback(SoCallbackAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoArray::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Traversal for GL rendering
//
// Use: extender

void
SoArray::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoArray::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Traversal for picking
//
// Use: extender

void
SoArray::pick(SoPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // Disable pick culling. If it is enabled, this is what happens:
    // if one of our children is a separator, we traverse it N times
    // to do the pick. For each of those times, the separator (if it
    // does pick culling) applies an SoGetBoundingBoxAction to the
    // current path (which goes through this array). The array then
    // computes the bbox of all N elements, so we get NxN behavior,
    // which takes too long.

    SbBool saveCullingFlag = action->isCullingEnabled();

    action->enableCulling(FALSE);

    SoArray::doAction(action);

    action->enableCulling(saveCullingFlag);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Traversal for get bounding box
//
// Use: extender

void
SoArray::getBoundingBox(SoGetBoundingBoxAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // We can't do any tricks that assume that the extents of the
    // bounding box of the array are formed by the corner elements,
    // since some child may do something a little weird, such as using
    // a switch to translate one element to an extreme position. We'll
    // just do the standard multi-element traversal.

    SoArray::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Traversal for handle event
//
// Use: extender

void
SoArray::handleEvent(SoHandleEventAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // Again, as in getBoundingBox(), someone could use a switch to
    // determine whether to handle an event. So we need to traverse
    // all of our children multiple times. But the matrix elements are
    // not enabled for this action, so we don't want to do any translation.

    int		numIndices;
    const int	*indices;
    int		lastChild;
    int		n1, n2, n3, i1, i2, i3, curIndex;

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

    n1 = numElements1.getValue();
    n2 = numElements2.getValue();
    n3 = numElements3.getValue();

    curIndex = 0;

    for (i3 = 0; i3 < n3; i3++) {
	for (i2 = 0; i2 < n2; i2++) {
	    for (i1 = 0; i1 < n1; i1++) {
		action->getState()->push();
		SoSwitchElement::set(action->getState(), curIndex++);
		children->traverse(action, 0, lastChild);
		action->getState()->pop();
	    }
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements getMatrix action.
//
// Use: extender

void
SoArray::getMatrix(SoGetMatrixAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    int		numIndices;
    const int	*indices;

    // Only need to compute matrix if array is a node in middle of
    // current path chain. We don't need to push or pop the state,
    // since this shouldn't have any effect on other nodes being
    // traversed.

    if (action->getPathCode(numIndices, indices) == SoAction::IN_PATH) {

	// Translate entire array if necessary
	if (! origin.isIgnored() && origin.getValue() != FIRST) {

	    int	n1 = numElements1.getValue();
	    int	n2 = numElements2.getValue();
	    int	n3 = numElements3.getValue();

	    SbVec3f vecToCenter = -(separation1.getValue() * (n1 - 1) +
				    separation2.getValue() * (n2 - 1) +
				    separation3.getValue() * (n3 - 1));

	    if (origin.getValue() == CENTER)
		vecToCenter *= 0.5;

	    // Translate the matrices in the action
	    SbMatrix m;
	    m.setTranslate(vecToCenter);
	    action->getMatrix().multLeft(m);
	    m.setTranslate(-vecToCenter);
	    action->getInverse().multRight(m);
	}

	children->traverse(action, 0, indices[numIndices - 1]);
    }
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
SoArray::search(SoSearchAction *action)
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
    // array is relevant to the search path

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
