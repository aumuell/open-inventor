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

//  -*- C++ -*-

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Description:
 |	This file defines the SoTextureMatrixElement class.
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_TEXTURE_MATRIX_ELEMENT
#define  _SO_TEXTURE_MATRIX_ELEMENT

#include <Inventor/SbLinear.h>
#include <Inventor/elements/SoAccumulatedElement.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoTextureMatrixElement
//
//  Element that stores the current texture matrix - the cumulative
//  transformation applied to subsequent shapes. Because the matrix is
//  cumulative, this class is derived from SoAccumulatedElement. The
//  set() method replaces the current matrix, while all the others
//  (mult(), translateBy(), etc.) multiply into it. Node id's of the
//  nodes that affect the element are accumulated properly.
//
//////////////////////////////////////////////////////////////////////////////

SoEXTENDER class SoTextureMatrixElement : public SoAccumulatedElement {

    SO_ELEMENT_HEADER(SoTextureMatrixElement);

  public:
    // Initializes element
    virtual void	init(SoState *state);

    // Overrides push() method to copy values from next instance in the stack
    virtual void	push(SoState *state);

    // Sets the texture matrix to the identity matrix
    static void		makeIdentity(SoState *state, SoNode *node);

    // Multiplies the given matrix into the texture matrix
    static void		mult(SoState *state, SoNode *node,
			     const SbMatrix &matrix);

    // Each of these multiplies a matrix that performs the specified
    // transformation into the texture matrix
    static void		translateBy(SoState *state, SoNode *node,
				    const SbVec3f &translation);
    static void		rotateBy(SoState *state, SoNode *node,
				 const SbRotation &rotation);
    static void		scaleBy(SoState *state, SoNode *node,
				const SbVec3f &scaleFactor);

    // Returns current matrix from the state
    static const SbMatrix &	get(SoState *state);

    // Prints element (for debugging)
    virtual void	print(FILE *fp) const;

  SoINTERNAL public:
    // Initializes the SoTextureMatrixElement class
    static void		initClass();

  protected:
    SbMatrix		textureMatrix;

    // Sets the matrix in an instance to identity
    virtual void	makeEltIdentity();

    // Multiplies into the matrix in an instance
    virtual void	multElt(const SbMatrix &matrix);

    // Each of these performs the appropriate operation on the matrix
    // in an instance
    virtual void	translateEltBy(const SbVec3f &translation);
    virtual void	rotateEltBy(const SbRotation &translation);
    virtual void	scaleEltBy(const SbVec3f &scaleFactor);

    // Gets the matrix from an instance
    virtual const SbMatrix &	getElt() const;

    virtual ~SoTextureMatrixElement();
};

#endif /* _SO_TEXTURE_MATRIX_ELEMENT */
