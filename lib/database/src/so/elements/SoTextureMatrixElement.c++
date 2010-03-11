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
 |	SoTextureMatrixElement
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/elements/SoTextureMatrixElement.h>

SO_ELEMENT_SOURCE(SoTextureMatrixElement);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoTextureMatrixElement::~SoTextureMatrixElement()
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
SoTextureMatrixElement::init(SoState *)
//
////////////////////////////////////////////////////////////////////////
{
    textureMatrix.makeIdentity();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pushes element, copying matrix from previous top instance.
//
// Use: public

void
SoTextureMatrixElement::push(SoState *)
//
////////////////////////////////////////////////////////////////////////
{
    SoTextureMatrixElement *elt = (SoTextureMatrixElement*)getNextInStack();
    textureMatrix = elt->textureMatrix;
    nodeIds = elt->nodeIds;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets Texture matrix to identity matrix in element accessed from state.
//
// Use: public, static

void
SoTextureMatrixElement::makeIdentity(SoState *state, SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
    SoTextureMatrixElement	*elt;

    // Get an instance we can change (pushing if necessary)
    elt = (SoTextureMatrixElement *) getElement(state, classStackIndex);

    if (elt != NULL) {
	elt->makeEltIdentity();

	// Update node id list in element
	elt->setNodeId(node);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Multiplies the given matrix into the Texture matrix
//
// Use: public, static

void
SoTextureMatrixElement::mult(SoState *state, SoNode *node,
			   const SbMatrix &matrix)
//
////////////////////////////////////////////////////////////////////////
{
    SoTextureMatrixElement	*elt;

    // Get an instance we can change (pushing if necessary)
    elt = (SoTextureMatrixElement *) getElement(state, classStackIndex);

    if (elt != NULL) {
	elt->multElt(matrix);

	// Update node id list in element
	elt->addNodeId(node);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Translates by the given vector.
//
// Use: public, static

void
SoTextureMatrixElement::translateBy(SoState *state, SoNode *node,
				  const SbVec3f &translation)
//
////////////////////////////////////////////////////////////////////////
{
    SoTextureMatrixElement	*elt;

    // Get an instance we can change (pushing if necessary)
    elt = (SoTextureMatrixElement *) getElement(state, classStackIndex);

    if (elt != NULL) {
	elt->translateEltBy(translation);

	// Update node id list in element
	elt->addNodeId(node);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Rotates by the given rotation.
//
// Use: public, static

void
SoTextureMatrixElement::rotateBy(SoState *state, SoNode *node,
			       const SbRotation &rotation)
//
////////////////////////////////////////////////////////////////////////
{
    SoTextureMatrixElement	*elt;

    // Get an instance we can change (pushing if necessary)
    elt = (SoTextureMatrixElement *) getElement(state, classStackIndex);

    if (elt != NULL) {
	elt->rotateEltBy(rotation);

	// Update node id list in element
	elt->addNodeId(node);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Scales by the given factors.
//
// Use: public, static

void
SoTextureMatrixElement::scaleBy(SoState *state, SoNode *node,
			      const SbVec3f &scaleFactor)
//
////////////////////////////////////////////////////////////////////////
{
    SoTextureMatrixElement	*elt;

    // Get an instance we can change (pushing if necessary)
    elt = (SoTextureMatrixElement *) getElement(state, classStackIndex);

    if (elt != NULL) {
	elt->scaleEltBy(scaleFactor);

	// Update node id list in element
	elt->addNodeId(node);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns Texture matrix from state.
//
// Use: public, static

const SbMatrix &
SoTextureMatrixElement::get(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    const SoTextureMatrixElement *elt;

    elt = (const SoTextureMatrixElement *)
	getConstElement(state, classStackIndex);

    return elt->getElt();
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
SoTextureMatrixElement::print(FILE *fp) const
{
    SoAccumulatedElement::print(fp);
}
#else  /* DEBUG */
void
SoTextureMatrixElement::print(FILE *) const
{
}
#endif /* DEBUG */

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets Texture matrix in element to identity matrix.
//
// Use: protected, virtual

void
SoTextureMatrixElement::makeEltIdentity()
//
////////////////////////////////////////////////////////////////////////
{
    textureMatrix.makeIdentity();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Multiplies into Texture matrix in element.
//
// Use: protected, virtual

void
SoTextureMatrixElement::multElt(const SbMatrix &matrix)
//
////////////////////////////////////////////////////////////////////////
{
    textureMatrix.multLeft(matrix);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Translates Texture matrix in element by the given vector.
//
// Use: public, virtual

void
SoTextureMatrixElement::translateEltBy(const SbVec3f &translation)
//
////////////////////////////////////////////////////////////////////////
{
    int i;

    // It's faster to just add to the translation elements of the
    // matrix than to multiply two matrices...

    for (i = 0; i < 3; i++)
	textureMatrix[3][i] += (textureMatrix[0][i] * translation[0] +
			      textureMatrix[1][i] * translation[1] +
			      textureMatrix[2][i] * translation[2]);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Rotates Texture matrix in element by the given rotation.
//
// Use: public, virtual

void
SoTextureMatrixElement::rotateEltBy(const SbRotation &rotation)
//
////////////////////////////////////////////////////////////////////////
{
    SbMatrix	m;

    rotation.getValue(m);
    textureMatrix.multLeft(m);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Scales Texture matrix in element by the given factors.
//
// Use: public, virtual

void
SoTextureMatrixElement::scaleEltBy(const SbVec3f &scaleFactor)
//
////////////////////////////////////////////////////////////////////////
{
    int i;

    // It's faster to just multiply into the elements of the
    // matrix than to multiply two matrices...

    for (i = 0; i < 4; i++) {
	textureMatrix[0][i] *= scaleFactor[0];
	textureMatrix[1][i] *= scaleFactor[1];
	textureMatrix[2][i] *= scaleFactor[2];
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns Texture matrix from element.
//
// Use: protected, virtual

const SbMatrix &
SoTextureMatrixElement::getElt() const
//
////////////////////////////////////////////////////////////////////////
{
    return textureMatrix;
}
