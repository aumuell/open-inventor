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
 |	Subclass of TextureCoordinateElement that also support GL
 |   rendering-- it has send() methods, and has a getType() that can
 |   return NONE, meaning shapes don't have to do anything to get
 |   texture coordinates generated for them (the GL's TexGen will do
 |   it for them).
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_GL_TEXTURE_COORDINATE_ELEMENT
#define  _SO_GL_TEXTURE_COORDINATE_ELEMENT

#include <Inventor/SbLinear.h>
#include <Inventor/elements/SoTextureCoordinateElement.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoGLTextureCoordinateElement
//
//  Element storing the current gltexture coordinates
//
//////////////////////////////////////////////////////////////////////////////

//
// The TextureCoordinateFunction nodes that use the GL texgen function
// to generate texture coordinates must register a callback that makes
// the appropriate GL calls with the element so pop() can reset the GL
// state back to what it should be.  The TextureCoordinateFunction
// nodes that can't use the GL texgen function must register a NULL
// callback, in which case the element will disable the GL's automatic
// texture coordinate generation.
//
typedef void SoTexCoordTexgenCB(void *userdata);

SoEXTENDER class SoGLTextureCoordinateElement :
				public SoTextureCoordinateElement {

    SO_ELEMENT_HEADER(SoGLTextureCoordinateElement);

  public:
    // Initializes element.
    virtual void	init(SoState *state);

    // TextureCoordinateFunction nodes that use texgen must define and
    // register a callback that makes the appropriate GL calls.
    // TextureCoordinateFunction nodes that don't use texgen must
    // register a NULL callback to turn off texgen.
    // The texGen arguments point to a function that will issue GL
    // TexGen calls; the func arguments must point to a static
    // function that will return the results of the texgen function
    // (needed because some nodes render by using their
    // generatePrimitives method).
    static void		setTexGen(SoState *state, SoNode *node, 
	    SoTexCoordTexgenCB *texGenFunc, void *texGenData = NULL,
	    SoTextureCoordinateFunctionCB *func = NULL,
	    void *funcData = NULL);

    // Returns code indicating what has been set in state/element
    virtual CoordType	getType() const;

    // Returns the top (current) instance of the element in the state.
    // The send routines are called on an instance
    // because it is more efficient than calling a static method that
    // looks up the element in the state for every coordinate.
    static const SoGLTextureCoordinateElement * getInstance(SoState *state);

    // Send routine for EXPLICIT case:

    void		send(int index) const;

    // Override push() method to set up new instance
    virtual void	push(SoState *state);

    // Override pop() method to maintain GL state
    virtual void	pop(SoState *state, const SoElement *prevTopElement);

  SoINTERNAL public:
    // Initializes the SoGLTextureCoordinateElement class
    static void		initClass();

  protected:
    void	setElt(SoTexCoordTexgenCB *function,
		       void *userData = NULL);

    virtual ~SoGLTextureCoordinateElement();

  private:
    SoTexCoordTexgenCB	*texgenCB;
    void		*texgenCBData;
    SoState 		*copiedFromParent;
};

#endif /* _SO_GL_TEXTURE_COORDINATE_ELEMENT */
