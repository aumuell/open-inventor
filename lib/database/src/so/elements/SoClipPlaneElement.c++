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
 |   $Revision $
 |
 |   Classes:
 |	SoClipPlaneElement
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/elements/SoClipPlaneElement.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/errors/SoDebugError.h>

SO_ELEMENT_SOURCE(SoClipPlaneElement);

//
// This structure contains all information needed for a clipping
// plane. Pointers to instances of this structure are stored in the
// "planes" SbPList.
//

struct So_ClipPlane {
    SbPlane	objPlane;	// World-space clipping plane
    SbPlane	worldPlane;	// World-space clipping plane
    SbMatrix	objToWorld;	// Converts object space to world space
    SbBool	worldPlaneValid;// TRUE if worldPlane was computed
};

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoClipPlaneElement::~SoClipPlaneElement()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes element.
//
// Use: public

void
SoClipPlaneElement::init(SoState *)
//
////////////////////////////////////////////////////////////////////////
{
    planes.truncate(0);
    startIndex = 0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds a clip plane to the current set in the state.
//
// Use: public, static

void
SoClipPlaneElement::add(SoState *state, SoNode *node, const SbPlane &plane)
//
////////////////////////////////////////////////////////////////////////
{
    SoClipPlaneElement	*elt;

    // Get an instance we can change (pushing if necessary)
    elt = (SoClipPlaneElement *) getElement(state, classStackIndex);

    if (elt != NULL) {
	elt->addToElt(plane, SoModelMatrixElement::get(state));

	// Update node id list in element
	elt->addNodeId(node);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Override push to copy the existing planes from the previous
//    element.
//
// Use: public

void
SoClipPlaneElement::push(SoState *)
//
////////////////////////////////////////////////////////////////////////
{
    const SoClipPlaneElement *elt =
	(const SoClipPlaneElement *) getNextInStack();

    // Use SbPList::operator = to copy the pointers to the existing
    // planes. Since the previous element can't be destroyed before
    // this one is, there is no problem with using pointers to
    // existing plane structures. However, any new ones we add have to
    // be freed up when this instance goes away, so we save the
    // starting index to allow us to fix this in pop().
    planes = elt->planes;
    startIndex = planes.getLength();
    nodeIds = elt->nodeIds;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Override pop to free up plane structures that were created in
//    the instance that was removed from the state.
//
// Use: public

void
SoClipPlaneElement::pop(SoState *, const SoElement *prevTopElement)
//
////////////////////////////////////////////////////////////////////////
{
    const SoClipPlaneElement *prevElt =
	(const SoClipPlaneElement *) prevTopElement;

    // Free up any plane structures that were created by prevElt
    for (int i = prevElt->startIndex; i < prevElt->planes.getLength(); i++)
	delete (So_ClipPlane *) prevElt->planes[i];
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the first (top) instance of the element in the state.
//
// Use: public, static

const SoClipPlaneElement *
SoClipPlaneElement::getInstance(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    return (const SoClipPlaneElement *)
	getConstElement(state, classStackIndex);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the number of planes in the element
//
// Use: public

int
SoClipPlaneElement::getNum() const
//
////////////////////////////////////////////////////////////////////////
{
    return planes.getLength();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the nth plane stored in an instance in object or world
//    space.
//
// Use: public

const SbPlane &
SoClipPlaneElement::get(int index, SbBool inWorldSpace) const
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (index < 0 || index >= planes.getLength())
	SoDebugError::post("SoClipPlaneElement::get",
			   "Index (%d) is out of range 0 - %d",
			   index, planes.getLength() - 1);
#endif /* DEBUG */

    So_ClipPlane *plane = (So_ClipPlane *) planes[index];

    if (! inWorldSpace)
	return plane->objPlane;

    // Transform plane into world space if necessary
    if (! plane->worldPlaneValid) {
	plane->worldPlane = plane->objPlane;
	plane->worldPlane.transform(plane->objToWorld);
    }
    return plane->worldPlane;
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
SoClipPlaneElement::print(FILE *fp) const
{
    SoAccumulatedElement::print(fp);
}
#else  /* DEBUG */
void
SoClipPlaneElement::print(FILE *) const
{
}
#endif /* DEBUG */

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds clipping plane to element.
//
// Use: protected, virtual

void
SoClipPlaneElement::addToElt(const SbPlane &plane,
			     const SbMatrix &modelMatrix)
//
////////////////////////////////////////////////////////////////////////
{
    So_ClipPlane *newPlane = new So_ClipPlane;

    newPlane->objPlane		= plane;
    newPlane->objToWorld	= modelMatrix;
    newPlane->worldPlaneValid	= FALSE;

    planes.append(newPlane);
}
