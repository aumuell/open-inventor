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
 |	SoPathSwitch
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/misc/SoState.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/actions/SoPickAction.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/nodes/SoPathSwitch.h>

SO_NODE_SOURCE(SoPathSwitch);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoPathSwitch::SoPathSwitch()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoPathSwitch);
    SO_NODE_ADD_FIELD(path, (NULL));
    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor that takes approximate number of children.
//
// Use: public

SoPathSwitch::SoPathSwitch(int nChildren) : SoGroup(nChildren)
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoPathSwitch);
    SO_NODE_ADD_FIELD(path, (NULL));
    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoPathSwitch::~SoPathSwitch()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Typical action traversal.
//
// Use: extender

void
SoPathSwitch::doAction(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // If path exists and matches current path, traverse children
    if (path.getValue() != NULL &&
	matchesPath(path.getValue(), action->getCurPath()))
	SoGroup::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Traversal for GL rendering
//
// Use: extender

void
SoPathSwitch::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoPathSwitch::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Traversal for picking
//
// Use: extender

void
SoPathSwitch::pick(SoPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoPathSwitch::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Traversal for get bounding box
//
// Use: extender

void
SoPathSwitch::getBoundingBox(SoGetBoundingBoxAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoPathSwitch::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Traversal for handle event
//
// Use: extender

void
SoPathSwitch::handleEvent(SoHandleEventAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoPathSwitch::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements getMatrix action.
//
// Use: extender

void
SoPathSwitch::getMatrix(SoGetMatrixAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // If path exists and matches current path, use regular getMatrix()
    if (path.getValue() != NULL &&
	matchesPath(path.getValue(), action->getCurPath()))
	SoGroup::getMatrix(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements search action.
//
// Use: extender

void
SoPathSwitch::search(SoSearchAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // if the action is searching everything, then do so...
    if (action->isSearchingAll())
	SoGroup::search(action);

    // Otherwise, traverse according to the regular switch node rules
    else {
	// First, make sure this node is found if we are searching for
	// pathSwitches
	SoNode::search(action);

	// Recurse
	if (! action->isFound())
	    SoPathSwitch::doAction(action);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if switchPath (which came from the "path" field of
//    the pathSwitch) matches the given current path. A successful
//    match means that the N nodes in the switchPath are the same as
//    the last N nodes in the current path, not including the path
//    switch node itself.
//
// Use: private

SbBool
SoPathSwitch::matchesPath(const SoPath *_switchPath,
			  const SoPath *_currentPath) const
//
////////////////////////////////////////////////////////////////////////
{
    const SoFullPath	*switchPath  = (const SoFullPath *) _switchPath;
    const SoFullPath	*currentPath = (const SoFullPath *) _currentPath;
    int	c, s;

    // An empty switchPath (but not NULL!) is always a match
    if (switchPath->getLength() == 0)
	return TRUE;

    // There's no way they can match if the switchPath is longer than
    // the current path. Or even if they are the same length, since
    // the switchPath cannot include the pathSwitch node.
    if (switchPath->getLength() >= currentPath->getLength())
	return FALSE;

    // Ignore the last node in the current path (it's the pathSwitch).
    // Compare the rest in reverse order. Any mismatch returns FALSE.

    c = currentPath->getLength() - 2;	// Skip last node
    s =  switchPath->getLength() - 1;

    while (s >= 0) {
	// Nodes have to be the same
	if (switchPath->getNode(s)  != currentPath->getNode(c))
	    return FALSE;

	// Indices have to be the same (except for zero'th node)
	if (c > 0 && s > 0 &&
	    switchPath->getIndex(s) != currentPath->getIndex(c))
	    return FALSE;

	c--, s--;
    }

    return TRUE;
}
