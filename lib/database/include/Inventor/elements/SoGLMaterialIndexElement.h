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
 |	This file defines the SoGLMaterialIndexElement class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_GL_MATERIAL_INDEX_ELEMENT
#define  _SO_GL_MATERIAL_INDEX_ELEMENT

#include <Inventor/elements/SoReplacedElement.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoGLMaterialIndexElement
//
//  Element that stores the indices into the current GL material map
//  of the ambient, diffuse, and specular components of the current
//  surface material, as defined by GL's color index lighting model.
//
//  This class allows read-only access to the top element in the state
//  to make accessing several values in it more efficient. Individual
//  values must be accessed through this instance.
//
//////////////////////////////////////////////////////////////////////////////

SoEXTENDER class SoGLMaterialIndexElement : public SoReplacedElement {

    SO_ELEMENT_HEADER(SoGLMaterialIndexElement);

  public:
    // Initializes element
    virtual void	init(SoState *state);

    // Sets the current material indices. Note that the given number
    // of indices must be valid for the ambient, diffuse, and specular
    // components.
    static void		set(SoState *state, SoNode *node,
			    int32_t numIndices,
			    const int32_t *ambientIndices,
			    const int32_t *diffuseIndices,
			    const int32_t *specularIndices);

    // Returns the top (current) instance of the element in the state
    static const SoGLMaterialIndexElement * getInstance(SoState *state);

    // Returns the number of indices in an instance
    int32_t		getNum() const		{ return numIndices; }

    // Returns the indexed indices from an element
    int32_t		getAmbient(int index) const;
    int32_t		getDiffuse(int index) const;
    int32_t		getSpecular(int index) const;

    // Returns the default indices
    static int32_t	getDefaultAmbient()		{ return 1; }
    static int32_t	getDefaultDiffuse()		{ return 2; }
    static int32_t	getDefaultSpecular()		{ return 3; }

    // Override push() and pop() methods to maintain GL state
    virtual void	push(SoState *state);
    virtual void	pop(SoState *state, const SoElement *prevTopElement);

    // Override matches() to return TRUE only if node-id's match AND
    // last values sent to GL match
    virtual SbBool	matches(const SoElement *elt) const;

    // Create and return a copy of this element
    virtual SoElement	*copyMatchInfo() const;

    // Prints element (for debugging)
    virtual void	print(FILE *fp) const;

    // Sends indexed values to GL
    void		send(int index) const;

  SoINTERNAL public:
    // Initializes the SoGLMaterialIndexElement class
    static void		initClass();

  protected:
    int32_t		numIndices;
    const int32_t	*ambientIndices, *diffuseIndices, *specularIndices;

    virtual ~SoGLMaterialIndexElement();

  private:
    // These store the default indices so that we can set the
    // appropriate "indices" members to point to them if no other
    // indices have been set.
    static int32_t	defaultAmbientIndex;
    static int32_t	defaultDiffuseIndex;
    static int32_t	defaultSpecularIndex;

    // Stores index of last indices sent to GL. We use this to avoid
    // re-sending values when possible.
    int			lastIndex;
};

#endif /* _SO_GL_MATERIAL_INDEX_ELEMENT */
