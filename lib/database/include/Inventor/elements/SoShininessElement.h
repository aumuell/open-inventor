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
 |	This file defines the SoShininessElement class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */
#ifdef IV_STRICT
#error SoShininessElement is obsolete. See SoLazyElement.h instead.
#endif /*IV_STRICT*/
#ifndef  _SO_SHININESS_ELEMENT
#define  _SO_SHININESS_ELEMENT


#include <Inventor/elements/SoLazyElement.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/errors/SoDebugError.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoShininessElement
//
//  Mostly superceded by SoLazyElement.
//  get() and set() methods retained for compatibility
//
//////////////////////////////////////////////////////////////////////////////

SoEXTENDER class SoShininessElement {


  public:

    // Sets the current shininess(es)
    static void		set(SoState *state, SoNode *,
			    int32_t numShininesses, const float *shininesses)
	{
	    SoLazyElement::setShininess(state, *shininesses);
#ifdef DEBUG
	    if(numShininesses>1){
		SoDebugError::post("SoShininessElement::set",
			"multiple shininess not supported");
	    }
#endif /*DEBUG*/
	}

    // Returns the top (current) instance of the element in the state
    static const SoShininessElement * getInstance(SoState *state)
	{
	    SoShininessElement* se = new SoShininessElement;
	    se->saveState = state;
	    return(se);
	}

    // Returns the number of shininess values in an instance
    int32_t		getNum() const		{ return 1; }

    // Returns the indexed shininess from an element
    float		get(int index) const
    	{
#ifdef DEBUG
	if(index >1)
	    	SoDebugError::post("SoShininessElement::get",
				"multiple shininess values not supported");
#endif /*DEBUG*/
	return(SoLazyElement::getShininess(saveState));
    	}


    // Returns the default shininess
    static float	getDefault()		
    { return SoLazyElement::getDefaultShininess(); }


  private:
    SoState * saveState;
};

#endif /* _SO_SHININESS_ELEMENT */

