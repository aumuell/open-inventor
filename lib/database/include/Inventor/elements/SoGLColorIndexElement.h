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
 |	This file defines the SoGLColorIndexElement class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */
#ifdef  IV_STRICT
#error SoGLColorIndexElement is obsolete. See So[GL]LazyElement.h instead.
#endif /*IV_STRICT*/
#ifndef  _SO_GL_COLOR_INDEX_ELEMENT
#define  _SO_GL_COLOR_INDEX_ELEMENT

#include <Inventor/SbColor.h>
#include <Inventor/elements/SoLazyElement.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoGLColorIndexElement
//
//  This class is being superceded by the SoLazyElement class.
//  All methods are converted inline to SoLazyElement methods for
//  compatibility.   
//
//////////////////////////////////////////////////////////////////////////////

SoEXTENDER class SoGLColorIndexElement {


  public:

    // Sets the current indexed color(s)
    static void		set(SoState *state, SoNode * node,
			    int32_t numIndices, const int32_t *indices)
	{
	    SoLazyElement::setColorIndices(state, node, numIndices, indices);
	}


    // Returns the top (current) instance of the element in the state
    static const SoGLColorIndexElement * getInstance(SoState *state)
	{
	    SoGLColorIndexElement* cie = new SoGLColorIndexElement;
	    cie->saveState = state;
	    return(cie);
	}


    // Returns the number of color indices in current instance
    int32_t		getNum() const		
    {
	return(SoLazyElement::getInstance(saveState)->getNumColorIndices());
    }
	
    // Returns a current color index 
    int32_t	get(int index) const
    {
	return(SoLazyElement::getColorIndex(saveState, index));
    }
	
    // Returns the default index
    static int32_t		getDefault()		
    { return SoLazyElement::getDefaultColorIndex(); }

    private:
    SoState* saveState;
};
#endif /* _SO_GL_COLOR_INDEX_ELEMENT */
