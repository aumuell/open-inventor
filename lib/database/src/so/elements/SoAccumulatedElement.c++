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
 |	SoAccumulatedElement
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/errors/SoDebugError.h>
#include <Inventor/elements/SoAccumulatedElement.h>
#include <Inventor/nodes/SoNode.h>

// Can't use normal SO_ELEMENT_ABSTRACT_SOURCE method because we need
// to set flag in constructor.
SO__ELEMENT_ABSTRACT_VARS(SoAccumulatedElement);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: protected

SoAccumulatedElement::SoAccumulatedElement()
//
////////////////////////////////////////////////////////////////////////
{
    accumulatesWithParentFlag = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoAccumulatedElement::~SoAccumulatedElement()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides this method to return TRUE if the all of the node-id's
//    of the two elements match.
//
// Use: public

SbBool
SoAccumulatedElement::matches(const SoElement *elt) const
//
////////////////////////////////////////////////////////////////////////
{
    const SoAccumulatedElement	*accElt = (const SoAccumulatedElement *) elt;
    int				i;

    if (accElt->nodeIds.getLength() != nodeIds.getLength())
	return FALSE;

    for (i = 0; i < nodeIds.getLength(); i++)
	if (nodeIds[i] != accElt->nodeIds[i])
	    return FALSE;

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Clears out the list of node id's
//
// Use: protected

void
SoAccumulatedElement::clearNodeIds()
//
////////////////////////////////////////////////////////////////////////
{
    nodeIds.truncate(0);
    accumulatesWithParentFlag = FALSE;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//     Adds the id of the given node to the current list. Keeps the
//     list sorted by increasing node-id.
//
// Use: protected

void
SoAccumulatedElement::addNodeId(const SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (node == NULL) {
	SoDebugError::post("SoAccumulatedElement::addNodeId",
			   "NULL node pointer passed");
	return;
    }
#endif /* DEBUG */

    int			i;
    unsigned long	id = (unsigned long)node->getNodeId();

    // Search through list for correct place for id
    for (i = 0; i < nodeIds.getLength(); i++)
	if (id <= (unsigned long) nodeIds[i])
	    break;

    // Otherwise, i will contain the index where the new element belongs
    if (i >= nodeIds.getLength())
	nodeIds.append((void *) id);
	
    // Insert it in the list if it is not already there:
    else { 
	if (id != (unsigned long) nodeIds[i])
	    nodeIds.insert((void *) id, i);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//     Sets the node id list to JUST the id of the given node.
//
// Use: protected

void
SoAccumulatedElement::setNodeId(const SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (node == NULL) {
	SoDebugError::post("SoAccumulatedElement::setNodeId",
			   "NULL node pointer passed");
	return;
    }
#endif /* DEBUG */

    nodeIds.truncate(0);
    nodeIds.append((void *) (unsigned long) node->getNodeId());

    accumulatesWithParentFlag = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//     Create a copy of this instance suitable for calling matches()
//     on.
//
// Use: protected

SoElement *
SoAccumulatedElement::copyMatchInfo() const
//
////////////////////////////////////////////////////////////////////////
{
    SoAccumulatedElement *result =
	(SoAccumulatedElement *)getTypeId().createInstance();

    result->nodeIds = nodeIds;

    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    "Captures" this element for caching purposes.  When
//    accumulating, all the elements up to the last reset (when the
//    nodeId list was cleared) must be captured in the cache.
//
// Use: virtual, protected

void
SoAccumulatedElement::captureThis(SoState *state) const
//
////////////////////////////////////////////////////////////////////////
{
    SoElement::captureThis(state);  // Capture this...

    if (accumulatesWithParentFlag) {
	SoAccumulatedElement *parent =
	    (SoAccumulatedElement *)getNextInStack();
	if (parent)
	    parent->captureThis(state);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Prints element for debugging.
//
// Use: public
//
////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
void
SoAccumulatedElement::print(FILE *fp) const
{
    SoElement::print(fp);

    fprintf(fp, "NodeIds: [ ");
    for (int i = 0; i < nodeIds.getLength(); i++) {
	fprintf(fp, "%d, ", nodeIds[i]);
    }
    fprintf(fp, "]\n");
}
#else  /* DEBUG */
void
SoAccumulatedElement::print(FILE *) const
{
}
#endif /* DEBUG */
