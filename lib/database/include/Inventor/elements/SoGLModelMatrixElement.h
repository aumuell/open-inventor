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
 |	This file defines the SoGLModelMatrixElement class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_GL_MODEL_MATRIX_ELEMENT
#define  _SO_GL_MODEL_MATRIX_ELEMENT

#include <Inventor/elements/SoModelMatrixElement.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoGLModelMatrixElement
//
//  Element that stores the current model matrix in GL. Overrides the
//  virtual methods on SoModelMatrixElement to send the matrix to GL
//  when necessary.
//
//  Note that this class relies on SoModelMatrixElement to store the
//  matrix in the instance. This is less expensive in the long run
//  than asking GL for the matrix when it is needed.
//
//  Because GL stores the model and view matrices in one matrix, this
//  has to do a little extra work when setting the model matrix to
//  identity or another matrix. (It has to set the GL model-view
//  matrix correctly.)
//
//////////////////////////////////////////////////////////////////////////////

SoEXTENDER class SoGLModelMatrixElement : public SoModelMatrixElement {

    SO_ELEMENT_HEADER(SoGLModelMatrixElement);

  public:
    // Initializes element
    virtual void	init(SoState *state);

    // Override push()/pop() methods so side effects can occur in GL
    virtual void	push(SoState *state);
    virtual void	pop(SoState *state, const SoElement *prevTopElement);

  SoINTERNAL public:
    // Initializes the SoGLModelMatrixElement class
    static void		initClass();

  protected:
    // Overrides SoModelMatrixElement::makeEltIdentity() to print an
    // error message
    virtual void	makeEltIdentity();

    // Sets the matrix in an instance to the given matrix. Assumes
    // that the passed matrix contains the correct combination of view
    // and model matrices.
    virtual void	setElt(const SbMatrix &matrix);

    // Multiplies into the matrix in an instance
    virtual void	multElt(const SbMatrix &matrix);

    // Each of these performs the appropriate operation on the matrix
    // in an instance
    virtual void	translateEltBy(const SbVec3f &translation);
    virtual void	rotateEltBy(const SbRotation &translation);
    virtual void	scaleEltBy(const SbVec3f &scaleFactor);
    virtual SbMatrix	pushMatrixElt();
    virtual void	popMatrixElt(const SbMatrix &m);

    virtual ~SoGLModelMatrixElement();

  private:
    // We need to save the state to access the viewing matrix
    SoState		*state;

    // And we need to remember the nodeId of the viewing matrix
    // element to see if it changes between pushMatrixElt() and
    // popMatrixElt().
    uint32_t		viewEltNodeId;
};

#endif /* _SO_GL_MODEL_MATRIX_ELEMENT */
