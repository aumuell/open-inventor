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
 |	This file defines the SoBBoxModelMatrixElement class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_BBOX_MODEL_MATRIX_ELEMENT
#define  _SO_BBOX_MODEL_MATRIX_ELEMENT

#include <Inventor/elements/SoModelMatrixElement.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoBBoxModelMatrixElement
//
//  Element that stores the current model matrix for use with the
//  SoGetBoundingBoxAction. Overrides the virtual methods on
//  SoModelMatrixElement to also set the current
//  SoLocalBBoxMatrixElement.
//
//  This class relies on SoModelMatrixElement to store the matrix in
//  the instance, and inherits most of its methods from it.
//
//////////////////////////////////////////////////////////////////////////////

SoEXTENDER class SoBBoxModelMatrixElement : public SoModelMatrixElement {

    SO_ELEMENT_HEADER(SoBBoxModelMatrixElement);

  public:
    // Initializes element
    virtual void	init(SoState *state);

    // Overrides push() method to copy values from next instance in the stack
    virtual void	push(SoState *state);

    // Allows the SoGetBoundingBoxAction to reset the current model
    // matrix to identity and all currently-open local matrices to
    // identity
    static void		reset(SoState *state, SoNode *node);

    // Because two model matrices are kept track of during the
    // getBoundingBoxAction (the local model matrix, used by
    // Separators to calculate their bbox caches, and the real model
    // matrix), replacement routines for pushMatrix/popMatrix must be
    // given; TransformSeparator::getBoundingBox uses these methods to
    // correctly keep both matrices up-to-date.
    static void		pushMatrix(SoState *state,
				   SbMatrix &matrix,
				   SbMatrix &localmatrix);
    static void		popMatrix(SoState *state,
				   const SbMatrix &matrix,
				   const SbMatrix &localmatrix);

  SoINTERNAL public:
    // Initializes the SoBBoxModelMatrixElement class
    static void		initClass();

  protected:
    // These all override the methods on SoModelMatrixElement to also
    // set the SoLocalBBoxMatrixElement
    virtual void	makeEltIdentity();
    virtual void	setElt(const SbMatrix &matrix);
    virtual void	multElt(const SbMatrix &matrix);
    virtual void	translateEltBy(const SbVec3f &translation);
    virtual void	rotateEltBy(const SbRotation &translation);
    virtual void	scaleEltBy(const SbVec3f &scaleFactor);

    // These must never be called; they print debug errors if called:
    virtual SbMatrix	pushMatrixElt();
    virtual void	popMatrixElt(const SbMatrix &m);

    virtual ~SoBBoxModelMatrixElement();

  private:
    // Stores pointer to state so we can access the SoLocalBBoxMatrixElement
    SoState		*state;

    // This is used to make sure the pushMatrix/popMatrix methods are
    // called correctly.
    static SbBool	pushPopCallOK;
};

#endif /* _SO_BBOX_MODEL_MATRIX_ELEMENT */
