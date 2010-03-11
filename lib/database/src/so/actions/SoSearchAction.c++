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
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |	SoSearchAction
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/errors/SoDebugError.h>

SO_ACTION_SOURCE(SoSearchAction);

SbBool SoSearchAction::duringSearchAll = FALSE;

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor. Sets up default values: search for first node, no
//    exact match necessary.
//
// Use: public

SoSearchAction::SoSearchAction()
//
////////////////////////////////////////////////////////////////////////
{
    SO_ACTION_CONSTRUCTOR(SoSearchAction);

    retPath = NULL;
    reset();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: public

SoSearchAction::~SoSearchAction()
//
////////////////////////////////////////////////////////////////////////
{
    if (retPath != NULL)
	retPath->unref();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reset everything back to just-constructed state.
//
// Use: public

void
SoSearchAction::reset()
//
////////////////////////////////////////////////////////////////////////
{
    if (retPath)
	retPath->unref();

    retPath = NULL;
    retPaths.truncate(0);

    setType(SoType::badType());
    setNode(NULL);
    setFind(0);
    setInterest(FIRST);
    setSearchingAll(FALSE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the node to search for.
//
// Use: public

void
SoSearchAction::setNode(SoNode *n)
//
////////////////////////////////////////////////////////////////////////
{
    node = n;

    if (n == NULL)
	lookingFor &= ~NODE;
    else
	lookingFor |= NODE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the search to look for nodes of a specific type, or to look
//    for no type if BadType is passed in.  The derivedOk flag sets
//    whether or not the types need to match exactly.
//
// Use: public

void
SoSearchAction::setType(SoType t, SbBool dOk)
//
////////////////////////////////////////////////////////////////////////
{
    derivedOk = dOk;
    type = t;

    if (t.isBad())
	lookingFor &= ~TYPE;

    else {
#ifdef DEBUG
	if (! t.isDerivedFrom(SoNode::getClassTypeId()))
	    SoDebugError::postWarning("SoSearchAction::setType",
				      "Type %s is not derived from SoNode",
				      t.getName().getString());
#endif /* DEBUG */

	lookingFor |= TYPE;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the name of the node to look for.
//
// Use: public

void
SoSearchAction::setName(const SbName &n)
//
////////////////////////////////////////////////////////////////////////
{
    name = n;
    lookingFor |= NAME;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the path to return (interest == FIRST or LAST) or adds to
//    the list of paths to return (interest == ALL).
//
// Use: extender

void
SoSearchAction::addPath(SoPath *path)
//
////////////////////////////////////////////////////////////////////////
{
    if (interest == ALL)
	retPaths.append(path);

    else {
	if (path != NULL)
	    path->ref();

	if (retPath != NULL)
	    retPath->unref();

	retPath = path;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initiates action on a graph.
//
// Use: protected

void
SoSearchAction::beginTraversal(SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (getFind() == 0) {
	SoDebugError::post("SoSearchAction::apply",
			   "No node type, node instance, or node name "
			   "specified for search");
	return;
    }
#endif /* DEBUG */

    // Empty things so we can tell when we have a match
    if (interest == ALL)
	retPaths.truncate(0);
    else
	addPath(NULL);

    // Set duringSearchAll flag (being careful to preserve prev value
    // in case of nested searches) so Switches are traversed properly:
    SbBool prevDuring = duringSearchAll;
    duringSearchAll = searchingAll;

    traverse(node);

    duringSearchAll = prevDuring;
}
