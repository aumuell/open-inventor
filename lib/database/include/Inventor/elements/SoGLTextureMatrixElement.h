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
 |	This file defines the SoGLTextureMatrixElement class.
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_GL_TEXTURE_MATRIX_ELEMENT
#define  _SO_GL_TEXTURE_MATRIX_ELEMENT

#include <Inventor/elements/SoTextureMatrixElement.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoGLTextureMatrixElement
//
//  Element that stores the current texture matrix in GL. Overrides the
//  virtual methods on SoTextureMatrixElement to send the matrix to GL
//  when necessary.
//
//////////////////////////////////////////////////////////////////////////////

SoEXTENDER class SoGLTextureMatrixElement : public SoTextureMatrixElement {

    SO_ELEMENT_HEADER(SoGLTextureMatrixElement);

  public:
    // Override pop() methods so side effects can occur in GL
    virtual void	pop(SoState *state, const SoElement *prevTopElement);

  SoINTERNAL public:
    // Initializes the SoGLTextureMatrixElement class
    static void		initClass();

  protected:
    // Sets the matrix to the identity matrix.
    virtual void	makeEltIdentity();

    // Multiplies into the matrix in an instance
    virtual void	multElt(const SbMatrix &matrix);

    // Each of these performs the appropriate operation on the matrix
    // in an instance
    virtual void	translateEltBy(const SbVec3f &translation);
    virtual void	rotateEltBy(const SbRotation &translation);
    virtual void	scaleEltBy(const SbVec3f &scaleFactor);

    virtual ~SoGLTextureMatrixElement();
};

#endif /* _SO_GL_TEXTURE_MATRIX_ELEMENT */
