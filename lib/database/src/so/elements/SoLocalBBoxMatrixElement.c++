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
 |	SoLocalBBoxMatrixElement
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/elements/SoLocalBBoxMatrixElement.h>
#include <Inventor/errors/SoDebugError.h>

SO_ELEMENT_SOURCE(SoLocalBBoxMatrixElement);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoLocalBBoxMatrixElement::~SoLocalBBoxMatrixElement()
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
SoLocalBBoxMatrixElement::init(SoState *)
//
////////////////////////////////////////////////////////////////////////
{
    localMatrix.makeIdentity();
    modelInverseMatrix.makeIdentity();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pushes element, copying matrix from previous top instance.
//
// Use: public

void
SoLocalBBoxMatrixElement::push(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    localMatrix = ((SoLocalBBoxMatrixElement *) getNextInStack())->localMatrix;

    // Update the modelInverseMatrix to contain the inverse of the
    // current model matrix.  We don't any caching dependencies to be
    // created, so we do this by using the state method and not the
    // element method:
    const SoModelMatrixElement *mme = (const SoModelMatrixElement *)
	state->getConstElement(SoModelMatrixElement::getClassStackIndex());
    modelInverseMatrix = mme->modelMatrix.inverse();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets local matrix to identity matrix in element accessed from state.
//
// Use: public, static

void
SoLocalBBoxMatrixElement::makeIdentity(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    SoLocalBBoxMatrixElement	*elt;

    // Get an instance we can change (pushing if necessary)
    elt = (SoLocalBBoxMatrixElement *) getElement(state, classStackIndex);

    elt->localMatrix.makeIdentity();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets local matrix to given matrix in element accessed from state.
//
// Use: public, static

void
SoLocalBBoxMatrixElement::set(SoState *state, const SbMatrix &matrix)
//
////////////////////////////////////////////////////////////////////////
{
    SoLocalBBoxMatrixElement	*elt;

    // Get an instance we can change (pushing if necessary)
    elt = (SoLocalBBoxMatrixElement *) getElement(state, classStackIndex);

    // Multiply by the inverse of the current model matrix to cancel
    // out any non-local matrix effects
    elt->localMatrix = matrix * elt->modelInverseMatrix;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Multiplies the given matrix into the local matrix
//
// Use: public, static

void
SoLocalBBoxMatrixElement::mult(SoState *state, const SbMatrix &matrix)
//
////////////////////////////////////////////////////////////////////////
{
    SoLocalBBoxMatrixElement	*elt;

    // Get an instance we can change (pushing if necessary)
    elt = (SoLocalBBoxMatrixElement *) getElement(state, classStackIndex);

    elt->localMatrix.multLeft(matrix);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Translates by the given vector.
//
// Use: public, static

void
SoLocalBBoxMatrixElement::translateBy(SoState *state,
				      const SbVec3f &translation)
//
////////////////////////////////////////////////////////////////////////
{
    SoLocalBBoxMatrixElement	*elt;
    int				i;

    // Get an instance we can change (pushing if necessary)
    elt = (SoLocalBBoxMatrixElement *) getElement(state, classStackIndex);

    for (i = 0; i < 3; i++)
	elt->localMatrix[3][i] +=
	    (elt->localMatrix[0][i] * translation[0] +
	     elt->localMatrix[1][i] * translation[1] +
	     elt->localMatrix[2][i] * translation[2]);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Rotates by the given rotation.
//
// Use: public, static

void
SoLocalBBoxMatrixElement::rotateBy(SoState *state, const SbRotation &rotation)
//
////////////////////////////////////////////////////////////////////////
{
    SoLocalBBoxMatrixElement	*elt;

    // Get an instance we can change (pushing if necessary)
    elt = (SoLocalBBoxMatrixElement *) getElement(state, classStackIndex);

    SbMatrix	m;
    rotation.getValue(m);
    elt->localMatrix.multLeft(m);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Scales by the given factors.
//
// Use: public, static

void
SoLocalBBoxMatrixElement::scaleBy(SoState *state, const SbVec3f &scaleFactor)
//
////////////////////////////////////////////////////////////////////////
{
    SoLocalBBoxMatrixElement	*elt;
    int				i;

    // Get an instance we can change (pushing if necessary)
    elt = (SoLocalBBoxMatrixElement *) getElement(state, classStackIndex);

    // It's faster to just multiply into the elements of the
    // matrix than to multiply two matrices...

    for (i = 0; i < 4; i++) {
	elt->localMatrix[0][i] *= scaleFactor[0];
	elt->localMatrix[1][i] *= scaleFactor[1];
	elt->localMatrix[2][i] *= scaleFactor[2];
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    "pushes" the current matrix.  This is used by TransformSeparator
//    to efficiently save/restore only the matrix state.
//
// Use: public, static

SbMatrix
SoLocalBBoxMatrixElement::pushMatrix(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    SoLocalBBoxMatrixElement	*elt;

    // This is NOT equivalent to a ::get, so we don't use the
    // SoElement::getConstElement method but instead use the
    // state->getElementNoPush method:
    elt = (SoLocalBBoxMatrixElement *) 
	state->getElementNoPush(classStackIndex);

    return elt->localMatrix;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Undoes a pushMatrix().
//
// Use: public, static

void
SoLocalBBoxMatrixElement::popMatrix(SoState *state, const SbMatrix &matrix)
//
////////////////////////////////////////////////////////////////////////
{
    SoLocalBBoxMatrixElement	*elt;

    // This is NOT equivalent to a ::get, so we don't use the
    // SoElement::getConstElement method but instead use the
    // state->getElementNoPush method:
    elt = (SoLocalBBoxMatrixElement *) 
	state->getElementNoPush(classStackIndex);

    elt->localMatrix = matrix;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Resets all current instances in the state to identity.
//
// Use: public, static

void
SoLocalBBoxMatrixElement::resetAll(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    SoLocalBBoxMatrixElement	*elt;

    // Get an instance we can change (pushing if necessary)
    elt = (SoLocalBBoxMatrixElement *) getElement(state, classStackIndex);

    while (elt != NULL) {
	elt->localMatrix.makeIdentity();
	elt = (SoLocalBBoxMatrixElement *) elt->getNextInStack();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns local matrix from state.
//
// Use: public, static

const SbMatrix &
SoLocalBBoxMatrixElement::get(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    const SoLocalBBoxMatrixElement *elt;

    elt = (const SoLocalBBoxMatrixElement *)
	getConstElement(state, classStackIndex);

    return elt->localMatrix;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides this method to print an error message and return
//    FALSE. See the class header comment for details.
//
// Use: public

SbBool
SoLocalBBoxMatrixElement::matches(const SoElement *) const
//
////////////////////////////////////////////////////////////////////////
{
    SoDebugError::post("SoLocalBBoxMatrixElement::matches",
		       "This method should never be called!");

    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//     Create a copy of this instance suitable for calling matches()
//     on.
//
// Use: protected

SoElement *
SoLocalBBoxMatrixElement::copyMatchInfo() const
//
////////////////////////////////////////////////////////////////////////
{
    SoDebugError::post("SoLocalBBoxMatrixElement::copyMatchInfo",
		       "This method should never be called!");

    return NULL;
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
SoLocalBBoxMatrixElement::print(FILE *fp) const
{
    SoElement::print(fp);

    fprintf(fp, "\tLocal Matrix:\n");
    localMatrix.print(fp);
}
#else  /* DEBUG */
void
SoLocalBBoxMatrixElement::print(FILE *) const
{
}
#endif /* DEBUG */
