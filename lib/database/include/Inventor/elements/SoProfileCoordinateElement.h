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
 |   $Revision $
 |
 |   Description:
 |	This file defines the SoProfileCoordinateElement class.
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson, Thad Beier
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_PROFILE_COORDINATE_ELEMENT
#define  _SO_PROFILE_COORDINATE_ELEMENT

#include <Inventor/SbLinear.h>
#include <Inventor/elements/SoReplacedElement.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoProfileCoordinateElement
//
//  Element storing the current profile coordinates, as either 2-D or
//  3-D points.
//
//  This class allows read-only access to the top element in the state
//  to make accessing several values in it more efficient. Individual
//  values must be accessed through this instance.
//
//////////////////////////////////////////////////////////////////////////////

SoEXTENDER class SoProfileCoordinateElement : public SoReplacedElement {

    SO_ELEMENT_HEADER(SoProfileCoordinateElement);

  public:
    // Initializes element.
    virtual void	init(SoState *state);

    // Sets the current profile coordinates
    static void		set2(SoState *state, SoNode *node,
			     int32_t numCoords, const SbVec2f *coords);
    static void		set3(SoState *state, SoNode *node,
			     int32_t numCoords, const SbVec3f *coords);

    // Returns the top (current) instance of the element in the state
    static const SoProfileCoordinateElement * getInstance(SoState *state);

    // Returns the number of coordinate points in an instance
    int32_t		getNum() const		{ return numCoords; }

    // Returns the indexed coordinate from an element as a 2- or
    // 3-vector, converting if necessary.
    const SbVec2f &	get2(int index) const;
    const SbVec3f &	get3(int index) const;

    // TRUE if set2() was called.
    SbBool		is2D() const  {	return coordsAre2D; }

    // Returns the default profile coordinate
    static SbVec2f	getDefault2() { return SbVec2f(0.0, 0.0); }
    static SbVec3f	getDefault3() { return SbVec3f(0.0, 0.0, 1.0); }

    virtual void	print(FILE *fp) const;

  SoINTERNAL public:
    // Initializes the SoCoordinateElement class
    static void		initClass();

  protected:
    int32_t 		numCoords;
    const SbVec2f	*coords2;
    const SbVec3f	*coords3;
    SbBool		coordsAre2D;

    virtual ~SoProfileCoordinateElement();

  private:
    static SbVec2f	*defaultCoord2;

    SbVec2f		convert2;	// To convert from 3-D to 2-D
    SbVec3f		convert3;	// To convert from 2-D to 3-D
};

#endif /* _SO_PROFILE_COORDINATE_ELEMENT */
