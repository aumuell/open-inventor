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
 |	SoBBoxModelMatrixElement
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/elements/SoBBoxModelMatrixElement.h>
#include <Inventor/elements/SoLocalBBoxMatrixElement.h>
#include <Inventor/errors/SoDebugError.h>

// This is used to make sure the pushMatrix/popMatrix methods are
// called correctly.
SbBool SoBBoxModelMatrixElement::pushPopCallOK = FALSE;

SO_ELEMENT_SOURCE(SoBBoxModelMatrixElement);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoBBoxModelMatrixElement::~SoBBoxModelMatrixElement()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes element
//
// Use: public

void
SoBBoxModelMatrixElement::init(SoState *_state)
//
////////////////////////////////////////////////////////////////////////
{
    // Save pointer to state so we can access the
    // SoLocalBBoxMatrixElement later
    state = _state;

    SoModelMatrixElement::init(_state);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pushes element, copying matrix and state pointer from previous
//    top instance.
//
// Use: public

void
SoBBoxModelMatrixElement::push(SoState *_state)
//
////////////////////////////////////////////////////////////////////////
{
    // Save pointer to state so we can access the
    // SoLocalBBoxMatrixElement later
    state = _state;

    SoModelMatrixElement::push(_state);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Resets the top element to identity and also resets all current
//    instances of the SoLocalBBoxMatrixElement to identity. This is
//    used by SoGetBoundingBoxAction to implement its reset path.
//
// Use: public, static

void
SoBBoxModelMatrixElement::reset(SoState *_state, SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
    SoBBoxModelMatrixElement	*elt;

    // Get an instance we can change (pushing if necessary)
    elt = (SoBBoxModelMatrixElement *)
	getElement(_state, classStackIndex);

    if (elt != NULL) {
	// Reset this element
	elt->makeEltIdentity();

	// Update node id list in element
	elt->setNodeId(node);

	// Reset all open SoLocalBBoxMatrixElement instances
	SoLocalBBoxMatrixElement::resetAll(_state);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides method on SoModelMatrixElement to also update the
//    SoLocalBBoxMatrixElement.
//
// Use: protected, virtual

void
SoBBoxModelMatrixElement::makeEltIdentity()
//
////////////////////////////////////////////////////////////////////////
{
    SoModelMatrixElement::makeEltIdentity();
    SoLocalBBoxMatrixElement::makeIdentity(state);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides method on SoModelMatrixElement to also update the
//    SoLocalBBoxMatrixElement.
//
// Use: protected, virtual

void
SoBBoxModelMatrixElement::setElt(const SbMatrix &matrix)
//
////////////////////////////////////////////////////////////////////////
{
    SoModelMatrixElement::setElt(matrix);
    SoLocalBBoxMatrixElement::set(state, matrix);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides method on SoModelMatrixElement to also update the
//    SoLocalBBoxMatrixElement.
//
// Use: protected, virtual

void
SoBBoxModelMatrixElement::multElt(const SbMatrix &matrix)
//
////////////////////////////////////////////////////////////////////////
{
    SoModelMatrixElement::multElt(matrix);
    SoLocalBBoxMatrixElement::mult(state, matrix);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides method on SoModelMatrixElement to also update the
//    SoLocalBBoxMatrixElement.
//
// Use: protected, virtual

void
SoBBoxModelMatrixElement::translateEltBy(const SbVec3f &translation)
//
////////////////////////////////////////////////////////////////////////
{
    SoModelMatrixElement::translateEltBy(translation);
    SoLocalBBoxMatrixElement::translateBy(state, translation);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides method on SoModelMatrixElement to also update the
//    SoLocalBBoxMatrixElement.
//
// Use: protected, virtual

void
SoBBoxModelMatrixElement::rotateEltBy(const SbRotation &rotation)
//
////////////////////////////////////////////////////////////////////////
{
    SoModelMatrixElement::rotateEltBy(rotation);
    SoLocalBBoxMatrixElement::rotateBy(state, rotation);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides method on SoModelMatrixElement to also update the
//    SoLocalBBoxMatrixElement.
//
// Use: protected, virtual

void
SoBBoxModelMatrixElement::scaleEltBy(const SbVec3f &scaleFactor)
//
////////////////////////////////////////////////////////////////////////
{
    SoModelMatrixElement::scaleEltBy(scaleFactor);
    SoLocalBBoxMatrixElement::scaleBy(state, scaleFactor);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//
// Because two model matrices are kept track of during the
// getBoundingBoxAction (the local model matrix, used by
// Separators to calculate their bbox caches, and the real model
// matrix), replacement routines for pushMatrix/popMatrix must be
// given; TransformSeparator::getBoundingBox uses these methods to
// correctly keep both matrices up-to-date.
//
// Use: public, static

void
SoBBoxModelMatrixElement::pushMatrix(SoState *state,
		SbMatrix &matrix, SbMatrix &localmatrix)
//
////////////////////////////////////////////////////////////////////////
{
    pushPopCallOK = TRUE;
    matrix = SoModelMatrixElement::pushMatrix(state);
    localmatrix = SoLocalBBoxMatrixElement::pushMatrix(state);
    pushPopCallOK = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Undoes a pushMatrix().
//
// Use: public, static

void
SoBBoxModelMatrixElement::popMatrix(SoState *state,
		const SbMatrix &matrix, const SbMatrix &localmatrix)
//
////////////////////////////////////////////////////////////////////////
{
    pushPopCallOK = TRUE;
    SoModelMatrixElement::popMatrix(state, matrix);
    SoLocalBBoxMatrixElement::popMatrix(state, localmatrix);
    pushPopCallOK = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides method to print error:
//
// Use: protected, virtual

SbMatrix
SoBBoxModelMatrixElement::pushMatrixElt()
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (!pushPopCallOK)
	SoDebugError::post("SoBBoxModelMatrixElement::pushMatrixElt",
		"Nodes must call SoBBoxModelMatrixElement::pushMatrix"
		" for getBoundingBox, not SoModelMatrixElement::pushMatrix");
#endif
    return SoModelMatrixElement::pushMatrixElt();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides method to print error:
//
// Use: protected, virtual

void
SoBBoxModelMatrixElement::popMatrixElt(const SbMatrix &matrix)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (!pushPopCallOK)
	SoDebugError::post("SoBBoxModelMatrixElement::popMatrixElt",
               "Nodes must call SoBBoxModelMatrixElement::popMatrix"
               " for getBoundingBox, not SoModelMatrixElement::popMatrix");
#endif
    SoModelMatrixElement::popMatrixElt(matrix);
}
