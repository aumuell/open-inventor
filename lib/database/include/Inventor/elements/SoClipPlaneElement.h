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
 |	This file defines the SoClipPlaneElement class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_CLIP_PLANE_ELEMENT
#define  _SO_CLIP_PLANE_ELEMENT

#include <Inventor/SbLinear.h>
#include <Inventor/elements/SoAccumulatedElement.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoClipPlaneElement
//
//  Element that stores the current set of clipping planes, specified
//  as SbPlanes.
//
//  When a plane is added, this element gets the current model matrix
//  from the state and stores it in the instance. This allows the
//  get() method to return the clip plane in object space (the plane
//  as originally defined) or world space (after being transformed by
//  the model matrix).
//
//////////////////////////////////////////////////////////////////////////////

SoEXTENDER class SoClipPlaneElement : public SoAccumulatedElement {

    SO_ELEMENT_HEADER(SoClipPlaneElement);

  public:
    // Initializes element
    virtual void	init(SoState *state);

    // Adds a clip plane to the current set in the state
    static void		add(SoState *state, SoNode *node,
			    const SbPlane &plane);

    // Overrides push() method to copy values from next instance in the stack
    virtual void	push(SoState *state);

    // Overrides pop() method to free up planes that were added
    virtual void	pop(SoState *state, const SoElement *prevTopElement);

    // Returns the top (current) instance of the element in the state
    static const SoClipPlaneElement * getInstance(SoState *state);

    // Returns the number of planes in an instance
    int			getNum() const;

    // Returns the indexed plane an element as an SbPlane. The plane
    // can be returned in object or world space.
    const SbPlane &	get(int index, SbBool inWorldSpace = TRUE) const;

    // Prints element (for debugging)
    virtual void	print(FILE *fp) const;

  SoINTERNAL public:
    // Initializes the SoClipPlaneElement class
    static void		initClass();

  protected:
    SbPList		planes;		// List of plane structures
    int			startIndex;	// Index of 1st plane created
					// in this instance

    // Adds the clipping plane to an instance. Takes the new plane and
    // the current model matrix
    virtual void	addToElt(const SbPlane &plane,
				 const SbMatrix &modelMatrix);

    virtual ~SoClipPlaneElement();
};

#endif /* _SO_CLIP_PLANE_ELEMENT */
