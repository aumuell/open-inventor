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
 |	This file defines the SoNormalElement class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_NORMAL_ELEMENT
#define  _SO_NORMAL_ELEMENT



#include <Inventor/SbLinear.h>
#include <Inventor/elements/SoReplacedElement.h>
#include <Inventor/errors/SoDebugError.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoNormalElement
//
//  Element that stores the current surface normals.
//
//  This class allows read-only access to the top element in the state
//  to make accessing several values in it more efficient. Individual
//  values must be accessed through this instance.
//
//////////////////////////////////////////////////////////////////////////////

SoEXTENDER class SoNormalElement : public SoReplacedElement {

    SO_ELEMENT_HEADER(SoNormalElement);

  public:
    // Initializes element
    virtual void	init(SoState *state);

    // Sets the current normals
    static void		set(SoState *state, SoNode *node,
			    int32_t numNormals, const SbVec3f *normals);

    // Returns the top (current) instance of the element in the state
    static const SoNormalElement * getInstance(SoState *state)
    	{return (const SoNormalElement *) 
	    getConstElement(state, classStackIndex);}

    // Returns the number of normal vectors in an instance
    int32_t		getNum() const		{ return numNormals; }

    // Returns the indexed normal from an element
    const SbVec3f &	get(int index) const
    	{
#ifdef DEBUG
    if (index < 0 || index >= numNormals)
	SoDebugError::post("SoNormalElement::get",
			   "Index (%d) is out of range 0 - %d",
			   index, numNormals - 1);
#endif /* DEBUG */ 
	    return normals[index];
	}

    // Prints element (for debugging)
    virtual void	print(FILE *fp) const;

  SoINTERNAL public:
    // Initializes the SoNormalElement class
    static void		initClass();

  protected:
    int32_t		numNormals;
    const SbVec3f	*normals;

    virtual ~SoNormalElement();

  private:
    // This stores a pointer to the default normal so that we can set
    // "normals" to point to it if no other normal has been set.
    static SbVec3f	*defaultNormal;
};

#endif /* _SO_NORMAL_ELEMENT */
