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
 |	This file defines the SoProfileElement class.
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson, Thad Beier,
 |                        Dave Immel, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_PROFILE_ELEMENT
#define  _SO_PROFILE_ELEMENT

#include <Inventor/SoLists.h>
#include <Inventor/SbLinear.h>
#include <Inventor/elements/SoAccumulatedElement.h>

class SoProfile;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoProfileElement
//
//  Element storing 2D profiles for nurbs and 3d text
//
//////////////////////////////////////////////////////////////////////////////

SoEXTENDER class SoProfileElement : public SoAccumulatedElement {

    SO_ELEMENT_HEADER(SoProfileElement);

  public:
    enum Profile {
	START_FIRST,
	START_NEW,
	ADD_TO_CURRENT
    };

    // Initializes element.
    virtual void	init(SoState *state);

    // Add to the current profile list
    static void		add(SoState *state, SoProfile *profile);

    // Overrides push() method to copy values from next instance in the stack
    virtual void	push(SoState *state);

    // Returns the current list of profile nodes from the state
    static const SoNodeList &	get(SoState *state);

    virtual void	print(FILE *fp) const;

  SoINTERNAL public:

    // Initializes the SoCoordinateElement class
    static void		initClass();

  protected:
    // list of profile nodes
    SoNodeList	        profiles;

    virtual ~SoProfileElement();
};

#endif /* _SO_PROFILE_ELEMENT */
