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
 |	SoWriteAction
 |
 |   Author(s)		: Paul S. Strauss, Thad Beier, Nick Thompson
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoDB.h>
#include <Inventor/actions/SoWriteAction.h>

SO_ACTION_SOURCE(SoWriteAction);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoWriteAction::SoWriteAction()
//
////////////////////////////////////////////////////////////////////////
{
    SO_ACTION_CONSTRUCTOR(SoWriteAction);
    output = new SoOutput;
    createdOutput = TRUE;
    continuing    = FALSE;
    doOneStage    = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor that takes SoOutput to write to.
//
// Use: public

SoWriteAction::SoWriteAction(SoOutput *out)
//
////////////////////////////////////////////////////////////////////////
{
    traversalMethods = methods;
    output = out;
    createdOutput = FALSE;
    continuing    = FALSE;
    doOneStage    = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: public

SoWriteAction::~SoWriteAction()
//
////////////////////////////////////////////////////////////////////////
{
    // Get rid of the SoOutput if we created it in the constructor
    if (createdOutput)
	delete output;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Continues write action on the graph rooted by a node. This is
//    used for continuing an action on a graph attached to another
//    graph, as when a field contains a node or path pointer or a
//    connection to another node.
//
// Use: internal

void
SoWriteAction::continueToApply(SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool	saveContinuing = continuing;

    continuing = TRUE;
    apply(node);
    continuing = saveContinuing;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Continues write action on the graph defined by a path. This is
//    used for continuing an action on a graph attached to another
//    graph, as when a field contains a path pointer.
//
// Use: internal

void
SoWriteAction::continueToApply(SoPath *path)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool	saveContinuing = continuing;

    continuing = TRUE;
    apply(path);
    continuing = saveContinuing;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initiates action on a graph.
//
// Use: protected

void
SoWriteAction::beginTraversal(SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
    // If we are beginning a traversal, increment the current write
    // counter in SoBase
    if (! continuing)
	SoBase::incrementCurrentWriteCounter();

    // Handle path lists specially
    if (getWhatAppliedTo() == PATH_LIST)
	traversePathList(node);

    // If applying to a path from a path list and the doOneStage flag
    // is set, we must be writing the path. Just do that.
    else if (doOneStage)
	getPathAppliedTo()->write(this);

    // Otherwise, we are applying the action normally to a node or path
    else {

	if (getWhatAppliedTo() == PATH) {

	    // When continuing to apply the action to a path, it
	    // should just continue with the current stage
	    if (continuing) {

		// In the counting stage, add a reference to the path
		// and then traverse the nodes in it
		if (output->getStage() == SoOutput::COUNT_REFS) {
		    getPathAppliedTo()->addWriteReference(output);
		    traverse(node);
		}

		// In the writing stage, write the path
		else
		    getPathAppliedTo()->write(this);
	    }

	    // If applying for the first time, do it all
	    else {
		getPathAppliedTo()->addWriteReference(output);
		output->setStage(SoOutput::COUNT_REFS);
		traverse(node);
		output->setStage(SoOutput::WRITE);
		getPathAppliedTo()->write(this);
	    }
	}

	else {
	    // When continuing to apply the action to a node, it MUST
	    // be in the write phase, so there's no need to count first

	    if (! continuing) {
		output->setStage(SoOutput::COUNT_REFS);
		traverse(node);
		output->setStage(SoOutput::WRITE);
	    }

	    traverse(node);
	}

	// Clean up
        if (! continuing)
	    output->reset();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides this method to return FALSE unless we are applying the
//    action to an individual path list to apply the RESET_COUNTS or
//    COUNT_REFS stage.
//
// Use: protected

SbBool
SoWriteAction::shouldCompactPathLists() const
//
////////////////////////////////////////////////////////////////////////
{
    return (doOneStage && output->getStage() != SoOutput::WRITE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initiates action on a path list. If we are applying to a path
//    list, we need to make sure that we reset the counts on ALL the
//    path lists we are going to get, then count references on ALL the
//    path lists, and then write out each path from the original list.
//    This way we are sure that we will write out every node that
//    affects any path and that we can share references across paths.
//
//    Therefore, we need to save all path lists generated from the
//    original list until we get the last one. When we get the last
//    one, we can then process them all.
//
// Use: private

void
SoWriteAction::traversePathList(SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
    int	i;

    // The doOneStage flag will be TRUE (see below) if we are being
    // applied during the count stage once we have all the path lists.
    // In this case, just do normal traversal on the compact path list
    if (doOneStage) {
	const SoPathList *list = getPathListAppliedTo();

	// First, make sure each path in the list is counted
	for (i = 0; i < list->getLength(); i++)
	    (*list)[i]->addWriteReference(output);

	// Do traversal
	traverse(node);

	// When incrementing counts, we want to make sure the head
	// node is referenced more than once, since it will be used
	// for each path. Do this by hand AFTER we do the regular
	// traversal, or else we might miss some nodes below it.
	if (output->getStage() == SoOutput::COUNT_REFS &&
	    list->getLength() > 1)
	    (*list)[0]->getHead()->addWriteReference(output);
    }

    // If this is not the last list, save it for later and do nothing else
    else if (! isLastPathListAppliedTo()) {
	SoPathList	*copy = new SoPathList(*getPathListAppliedTo());
	savedLists.append(copy);
    }

    // Otherwise, process all the lists
    else {
	// First set the doOneStage flag to TRUE so that when we apply
	// the action to each path list, we know that we're supposed
	// to do only that one stage.
	doOneStage = TRUE;

	// Mark this action as continuing so we don't increment the
	// write-ref counter extra times
	SbBool saveContinuing = continuing;
	continuing = TRUE;

	// Count first; do all saved lists first, then the last one.
	// Apply the action to each path list.
	output->setStage(SoOutput::COUNT_REFS);
	for (i = 0; i < savedLists.getLength(); i++)
	    apply(* (const SoPathList *) savedLists[i], TRUE);
	apply(*getPathListAppliedTo(), TRUE);

	// Then write each path from original list by applying the
	// action to the path
	output->setStage(SoOutput::WRITE);
	for (i = 0; i < getOriginalPathListAppliedTo()->getLength(); i++)
	    apply((*getOriginalPathListAppliedTo())[i]);

	doOneStage = FALSE;

	// Free up saved lists and clear out list
	for (i = 0; i < savedLists.getLength(); i++)
	    delete (SoPathList *) savedLists[i];
	savedLists.truncate(0);

	// Restore the state of the continuing flag
	continuing = saveContinuing;
    }
}
