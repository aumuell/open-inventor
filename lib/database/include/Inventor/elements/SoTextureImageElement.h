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
 |	This file defines the SoTextureImageElement class.
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_TEXTURE_IMAGE_ELEMENT
#define  _SO_TEXTURE_IMAGE_ELEMENT

#include <Inventor/SbColor.h>
#include <Inventor/elements/SoReplacedElement.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoTextureImageElement
//
//  Element storing the current texture image.
//
//////////////////////////////////////////////////////////////////////////////

SoEXTENDER class SoTextureImageElement : public SoReplacedElement {

    SO_ELEMENT_HEADER(SoTextureImageElement);

  public:
    // Initializes element.
    virtual void	init(SoState *state);

    // sets the current image, wrap, and model:
    static void		set(SoState *state, SoNode *node,
			    const SbVec2s &size, int numComponents,
			    const unsigned char *bytes,
			    int wrapS, int wrapT, int model,
			    const SbColor &blendColor);

    static const unsigned char *get(SoState *state, SbVec2s &size,
				    int &numComponents, int &wrapS,
				    int &wrapT, int &model, 
				    SbColor &blendColor);

    // Returns TRUE if the texture contains transparency info
    static SbBool	containsTransparency(SoState *state);

    // Returns the default texture image
    static const unsigned char *getDefault(SbVec2s &s, int &nc);

    // Print info about image for debugging
    virtual void	print(FILE *fp) const;

  SoINTERNAL public:
    // Initializes the class
    static void		initClass();

  protected:
    virtual void	setElt(const SbVec2s &s, int nc,
			       const unsigned char *bytes,
			       int wrapS, int wrapT, int model,
			       const SbColor &blendColor);

    SbVec2s	size;
    int		numComponents;
    const unsigned char *bytes;
    int		wrapS, wrapT, model;
    SbColor	blendColor;

    virtual ~SoTextureImageElement();
};

#endif /* _SO_TEXTURE_IMAGE_ELEMENT */
