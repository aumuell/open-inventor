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
 |	This file defines the SoCoordinateElement class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_COORDINATE_ELEMENT
#define  _SO_COORDINATE_ELEMENT

#include <Inventor/SbLinear.h>
#include <Inventor/elements/SoReplacedElement.h>


//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoCoordinateElement
//
//  Element that stores the current coordinates. Coordinates may be
//  specified as 3-D or 4-D vectors. This element remembers the last
//  type of value stored.
//
//  This class allows read-only access to the top element in the state
//  to make accessing several values in it more efficient. Individual
//  values must be accessed through this instance.
//
//////////////////////////////////////////////////////////////////////////////

SoEXTENDER class SoCoordinateElement : public SoReplacedElement {

    SO_ELEMENT_HEADER(SoCoordinateElement);

  public:
    // Initializes element
    virtual void	init(SoState *state);

    // Sets the current coordinates in the state as 3-vectors or 4-vectors
    static void		set3(SoState *state, SoNode *node,
			     int32_t numCoords, const SbVec3f *coords);
    static void		set4(SoState *state, SoNode *node,
			     int32_t numCoords, const SbVec4f *coords);

    // Returns the top (current) instance of the element in the state
    static const SoCoordinateElement * getInstance(SoState *state)
        {return (const SoCoordinateElement *)
	    getConstElement(state, classStackIndex);}

    // Returns the number of coordinate points in an instance
    int32_t		getNum() const		{ return numCoords; }

    // Returns TRUE if the coordinates were specified as 3-vectors,
    // FALSE if 4-vectors
    SbBool		is3D() const		{ return coordsAre3D; }

    // Returns the indexed coordinate from an element as a 3- or
    // 4-vector, converting if necessary. A returned reference may be
    // invalid after the next call to either of these methods.
    const SbVec3f &	get3(int index) const;
    const SbVec4f &	get4(int index) const;

    // Returns the default 3-D or 4-D coordinate
    static SbVec3f getDefault3()		{ return SbVec3f(0, 0, 0); }
    static SbVec4f getDefault4()		{ return SbVec4f(0, 0, 0, 1); }

    // Prints element (for debugging)
    virtual void	print(FILE *fp) const;

  SoINTERNAL public:
    // Initializes the SoCoordinateElement class
    static void		initClass();

  protected:
    int32_t		numCoords;
    const SbVec3f	*coords3;
    const SbVec4f	*coords4;
    SbBool		coordsAre3D;

    virtual ~SoCoordinateElement();

  private:
    // This stores a pointer to the default coordinates so that we can
    // set "coords3" to point to them if no other values have been set.
    static SbVec3f	*defaultCoord3;

    SbVec3f		convert3;	// To convert from 4-D to 3-D
    SbVec4f		convert4;	// To convert from 3-D to 4-D
};

#endif /* _SO_COORDINATE_ELEMENT */
