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
 |	This file defines the SoTextureQualityElement and
 |      SoTextureOverrideElement classes.
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_TEXTURE_QUALITY_ELEMENT
#define  _SO_TEXTURE_QUALITY_ELEMENT

#include <Inventor/elements/SoFloatElement.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoTextureQualityElement
//
//  Element that stores the current texture quality
//
//////////////////////////////////////////////////////////////////////////////

SoEXTENDER class SoTextureQualityElement : public SoFloatElement {

    SO_ELEMENT_HEADER(SoTextureQualityElement);

  public:
    // Initializes element
    virtual void	init(SoState *state);

    // Sets the current TextureQuality in the state
    static void		set(SoState *state, float qual)
	{ SoFloatElement::set(classStackIndex, state, qual); }

    // Returns current TextureQuality from the state
    static float	get(SoState *state)
	{ return SoFloatElement::get(classStackIndex, state); }

    // Returns the default TextureQuality
    static float	getDefault()			{ return 0.5; }

  SoINTERNAL public:
    // Initializes the SoTextureQualityElement class
    static void		initClass();

  protected:
    virtual ~SoTextureQualityElement();
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoTextureOverrideElement
//
//  Element that stores a flag for each type of element which can be
//  overridden.  Nodes implement override by setting the appropriate
//  bit if their override flag is on, and ignoring overridden elements
//  if the corresponding bit in the state's SoTextureOverrideElement is set.
//  
//////////////////////////////////////////////////////////////////////////////

SoEXTENDER class SoTextureOverrideElement : public SoElement {

    SO_ELEMENT_HEADER(SoTextureOverrideElement);

    enum {
	TEXTURE_QUALITY	= 0x1,
	TEXTURE_IMAGE	= 0x2
    };

  public:
    // Initializes element
    virtual void	init(SoState *state);

    // Comparison based on value of int32_ts
    virtual SbBool	matches(const SoElement *elt) const;

    // Overrides push() method to copy values from next instance in
    // the stack, and set up cache dependencies correctly
    virtual void	push(SoState *state);

    //
    // "get" methods for each element which can be overridden.
    //

    // Returns TRUE iff SoTextureQualityElement is overridden.
    static SbBool	getQualityOverride(SoState *state);

    // Returns TRUE iff SoTextureImageElement is overridden.
    static SbBool	getImageOverride(SoState *state);

    //
    // "set" methods for each element which can be overridden.
    //

    // set override flag for SoTextureQualityElement.
    static void		setQualityOverride(SoState *state,
					   SbBool override);
    
    // set override flag for SoTextureImageElement.
    static void		setImageOverride(SoState *state,
					 SbBool override);

    // Prints element (for debugging)
    virtual void	print(FILE *fp) const;

  SoINTERNAL public:
    // Initializes the SoTextureOverrideElement class
    static void		initClass();

    // Copy method, copies flags
    virtual SoElement 	*copyMatchInfo() const;
    
  private:
    // Used by ::print method
    void pFlag(FILE *, const char *, int) const;

    uint32_t flags;
};

#endif /* _SO_TEXTURE_QUALITY_ELEMENT */
