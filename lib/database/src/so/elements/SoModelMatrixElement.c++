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
 |	SoModelMatrixElement
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/elements/SoModelMatrixElement.h>

#ifdef DEBUG
#include <SoDebug.h>
#include <Inventor/errors/SoDebugError.h>
#endif

SO_ELEMENT_SOURCE(SoModelMatrixElement);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoModelMatrixElement::~SoModelMatrixElement()
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
SoModelMatrixElement::init(SoState *)
//
////////////////////////////////////////////////////////////////////////
{
    modelMatrix.makeIdentity();
    flags.isModelIdentity = TRUE;
    flags.haveCullMatrix = FALSE;
    flags.haveModelCull = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pushes element, copying matrix from previous top instance.
//
// Use: public

void
SoModelMatrixElement::push(SoState *)
//
////////////////////////////////////////////////////////////////////////
{
    SoModelMatrixElement *mtxElt = (SoModelMatrixElement *) getNextInStack();

    modelMatrix = mtxElt->modelMatrix;
    flags.isModelIdentity  = mtxElt->flags.isModelIdentity;
    flags.haveCullMatrix = FALSE;
    flags.haveModelCull = FALSE;
    nodeIds = mtxElt->nodeIds;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets model matrix to identity matrix in element accessed from state.
//
// Use: public, static

void
SoModelMatrixElement::makeIdentity(SoState *state, SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
    SoModelMatrixElement	*elt;

    // Get an instance we can change (pushing if necessary)
    elt = (SoModelMatrixElement *) getElement(state, classStackIndex);

    elt->makeEltIdentity();

    // Update node id list in element
    elt->setNodeId(node);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets model matrix to given matrix in element accessed from state.
//
// Use: public, static

void
SoModelMatrixElement::set(SoState *state, SoNode *node,
			  const SbMatrix &matrix)
//
////////////////////////////////////////////////////////////////////////
{
    SoModelMatrixElement	*elt;

    // Get an instance we can change (pushing if necessary)
    elt = (SoModelMatrixElement *) getElement(state, classStackIndex);

    elt->setElt(matrix);

    // Update node id list in element
    elt->setNodeId(node);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    "pushes" the current matrix.  This is used by TransformSeparator
//    to efficiently save/restore only the matrix state.  For caching
//    to work properly, the caller MUST NOT do anything with the
//    matrix besides pass it in to the ::popMatrix call.
//
// Use: public, static

SbMatrix
SoModelMatrixElement::pushMatrix(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    SoModelMatrixElement	*elt;

    // This is NOT equivalent to a ::get, so we don't use the
    // SoElement::getConstElement method but instead use the
    // state->getElementNoPush method:
    elt = (SoModelMatrixElement *) 
	state->getElementNoPush(classStackIndex);

    return elt->pushMatrixElt();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Undoes a pushMatrix().
//
// Use: public, static

void
SoModelMatrixElement::popMatrix(SoState *state, const SbMatrix &matrix)
//
////////////////////////////////////////////////////////////////////////
{
    SoModelMatrixElement	*elt;

    // This is NOT equivalent to a ::get, so we don't use the
    // SoElement::getConstElement method but instead use the
    // state->getElementNoPush method:
    elt = (SoModelMatrixElement *) 
	state->getElementNoPush(classStackIndex);

    elt->popMatrixElt(matrix);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Virtual method that does matrix push.  GL version overrides to
//    make glPushMatrix call:
//
// Use: public, static

SbMatrix
SoModelMatrixElement::pushMatrixElt()
//
////////////////////////////////////////////////////////////////////////
{
    return modelMatrix;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Virtual method that does matrix pop.  GL version overrides to
//    make glPopMatrix call:
//
// Use: public, static

void
SoModelMatrixElement::popMatrixElt(const SbMatrix &matrix)
//
////////////////////////////////////////////////////////////////////////
{
    modelMatrix = matrix;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Multiplies the given matrix into the model matrix
//
// Use: public, static

void
SoModelMatrixElement::mult(SoState *state, SoNode *node,
			   const SbMatrix &matrix)
//
////////////////////////////////////////////////////////////////////////
{
    SoModelMatrixElement	*elt;

    // Get an instance we can change (pushing if necessary)
    elt = (SoModelMatrixElement *) getElement(state, classStackIndex);

    elt->multElt(matrix);

    // Update node id list in element
    elt->addNodeId(node);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Translates by the given vector.
//
// Use: public, static

void
SoModelMatrixElement::translateBy(SoState *state, SoNode *node,
				  const SbVec3f &translation)
//
////////////////////////////////////////////////////////////////////////
{
    SoModelMatrixElement	*elt;

    // Get an instance we can change (pushing if necessary)
    elt = (SoModelMatrixElement *) getElement(state, classStackIndex);

    elt->translateEltBy(translation);

    // Update node id list in element
    elt->addNodeId(node);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Rotates by the given rotation.
//
// Use: public, static

void
SoModelMatrixElement::rotateBy(SoState *state, SoNode *node,
			       const SbRotation &rotation)
//
////////////////////////////////////////////////////////////////////////
{
    SoModelMatrixElement	*elt;

    // Get an instance we can change (pushing if necessary)
    elt = (SoModelMatrixElement *) getElement(state, classStackIndex);

    elt->rotateEltBy(rotation);

    // Update node id list in element
    elt->addNodeId(node);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Scales by the given factors.
//
// Use: public, static

void
SoModelMatrixElement::scaleBy(SoState *state, SoNode *node,
			      const SbVec3f &scaleFactor)
//
////////////////////////////////////////////////////////////////////////
{
    SoModelMatrixElement	*elt;

    // Get an instance we can change (pushing if necessary)
    elt = (SoModelMatrixElement *) getElement(state, classStackIndex);

    elt->scaleEltBy(scaleFactor);

    // Update node id list in element
    elt->addNodeId(node);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns model matrix from state.
//
// Use: public, static

const SbMatrix &
SoModelMatrixElement::get(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    const SoModelMatrixElement *elt;

    elt = (const SoModelMatrixElement *)
	getConstElement(state, classStackIndex);

    return elt->modelMatrix;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns model matrix from state, sets given flag to TRUE if
//    matrix is known to be identity.
//
// Use: public, static

const SbMatrix &
SoModelMatrixElement::get(SoState *state, SbBool &isIdent)
//
////////////////////////////////////////////////////////////////////////
{
    const SoModelMatrixElement *elt;

    elt = (const SoModelMatrixElement *)
	getConstElement(state, classStackIndex);

    isIdent = elt->flags.isModelIdentity;

    return elt->modelMatrix;
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
SoModelMatrixElement::print(FILE *fp) const
{
    SoAccumulatedElement::print(fp);
}
#else  /* DEBUG */
void
SoModelMatrixElement::print(FILE *) const
{
}
#endif /* DEBUG */

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets model matrix in element to identity matrix.
//
// Use: protected, virtual

void
SoModelMatrixElement::makeEltIdentity()
//
////////////////////////////////////////////////////////////////////////
{
    modelMatrix.makeIdentity();
    flags.isModelIdentity = TRUE;
    flags.haveModelCull = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets model matrix in element to given matrix.
//
// Use: protected, virtual

void
SoModelMatrixElement::setElt(const SbMatrix &matrix)
//
////////////////////////////////////////////////////////////////////////
{
    modelMatrix = matrix;
    flags.isModelIdentity  = FALSE;	// Assume the worst
    flags.haveModelCull = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Multiplies into model matrix in element.
//
// Use: protected, virtual

void
SoModelMatrixElement::multElt(const SbMatrix &matrix)
//
////////////////////////////////////////////////////////////////////////
{
    modelMatrix.multLeft(matrix);
    flags.isModelIdentity  = FALSE;	// Assume the worst
    flags.haveModelCull = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Translates model matrix in element by the given vector.
//
// Use: public, virtual

void
SoModelMatrixElement::translateEltBy(const SbVec3f &translation)
//
////////////////////////////////////////////////////////////////////////
{
    int i;

    // It's faster to just add to the translation elements of the
    // matrix than to multiply two matrices...

    for (i = 0; i < 3; i++)
	modelMatrix[3][i] += (modelMatrix[0][i] * translation[0] +
			      modelMatrix[1][i] * translation[1] +
			      modelMatrix[2][i] * translation[2]);

    flags.isModelIdentity  = FALSE;	// Assume the worst
    flags.haveModelCull = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Rotates model matrix in element by the given rotation.
//
// Use: public, virtual

void
SoModelMatrixElement::rotateEltBy(const SbRotation &rotation)
//
////////////////////////////////////////////////////////////////////////
{
    SbMatrix	m;

    rotation.getValue(m);
    modelMatrix.multLeft(m);

    flags.isModelIdentity  = FALSE;	// Assume the worst
    flags.haveModelCull = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Scales model matrix in element by the given factors.
//
// Use: public, virtual

void
SoModelMatrixElement::scaleEltBy(const SbVec3f &scaleFactor)
//
////////////////////////////////////////////////////////////////////////
{
    int i;

    // It's faster to just multiply into the elements of the
    // matrix than to multiply two matrices...

    for (i = 0; i < 4; i++) {
	modelMatrix[0][i] *= scaleFactor[0];
	modelMatrix[1][i] *= scaleFactor[1];
	modelMatrix[2][i] *= scaleFactor[2];
    }

    flags.isModelIdentity  = FALSE;	// Assume the worst
    flags.haveModelCull = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets cull matrix to given matrix
//
// Use: public, static

void
SoModelMatrixElement::setCullMatrix(SoState *state, SoNode *node,
				    const SbMatrix &matrix)
//
////////////////////////////////////////////////////////////////////////
{
    SoModelMatrixElement	*elt;

    // Get an instance we can change (pushing if necessary)
    elt = (SoModelMatrixElement *) getElement(state, classStackIndex);

    elt->cullMatrix = matrix;
    elt->flags.haveCullMatrix = TRUE;

    // Update node id list in element
    elt->addNodeId(node);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Get combined model+cull matrices
//
// Use: public, static

const SbMatrix &
SoModelMatrixElement::getCombinedCullMatrix(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    const SoModelMatrixElement	*elt;

    elt = (const SoModelMatrixElement *) getConstElement(state,
							 classStackIndex);
    if (!elt->flags.haveModelCull) {
	// Cast const away:
	SoModelMatrixElement *m_elt = (SoModelMatrixElement *)elt;

	if (!elt->flags.haveCullMatrix) {
	    // Grab cull matrix from one of our parent elements:
	    const SoModelMatrixElement *parent = 
		(const SoModelMatrixElement *)elt->getNextInStack();

	    while (parent && !parent->flags.haveCullMatrix)
		parent = 
		    (const SoModelMatrixElement *)parent->getNextInStack();

	    if (parent) {
		m_elt->cullMatrix = parent->cullMatrix;
		m_elt->flags.haveCullMatrix = TRUE;
	    }
	    else {
		// Uh-oh, no cull matrix.
#ifdef DEBUG
		SoDebugError::post("SoModelMatrixElement::getCombinedCullMatrix",
				   "No cull matrix set (culling Separator "
				   "traversed before camera)");
#endif
		m_elt->cullMatrix = SbMatrix::identity();
	    }
	}
#ifdef DEBUG
	if (SoDebug::GetEnv("IV_DEBUG_MATRIX_CULL"))
	    SoDebug::RTPrintf("  M*VP calculated\n");
#endif

	m_elt->modelCullMatrix = elt->modelMatrix*elt->cullMatrix;
	m_elt->flags.haveModelCull = TRUE;
    }
#ifdef DEBUG
    else if (SoDebug::GetEnv("IV_DEBUG_MATRIX_CULL"))
	SoDebug::RTPrintf("  MVP valid\n");
#endif

    return elt->modelCullMatrix;
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides this method to return FALSE if flags.haveCullMatrix is TRUE.
//    That way, culling always breaks caches.
//
// Use: public

SbBool
SoModelMatrixElement::matches(const SoElement *elt) const
//
////////////////////////////////////////////////////////////////////////
{
    const SoModelMatrixElement	*mmElt = (const SoModelMatrixElement *) elt;

    if (mmElt->flags.haveCullMatrix)
	return FALSE;

    return (SoAccumulatedElement::matches(elt));
}
