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
 |	This file defines the SoShapeHintsElement class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_SHAPE_HINTS_ELEMENT
#define  _SO_SHAPE_HINTS_ELEMENT

#include <Inventor/elements/SoReplacedElement.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoShapeHintsElement
//
//  Element that stores current shape hints.
//  All three hints must be set at the same time; to
//  leave any hint as is, use the "AS_IS" enum value.
//
//////////////////////////////////////////////////////////////////////////////

SoEXTENDER class SoShapeHintsElement : public SoElement {

    SO_ELEMENT_HEADER(SoShapeHintsElement);

  public:
    enum VertexOrdering {	// Hints about ordering of face vertices:
	UNKNOWN_ORDERING,	//	No ordering info is known
	CLOCKWISE,		//	Vertices are ordered CW around faces
	COUNTERCLOCKWISE,	//	Vertices are ordered CCW around faces
	ORDERING_AS_IS		//	Indicates to set() to leave as is
    };

    enum ShapeType {		// Hints about entire shape:
	UNKNOWN_SHAPE_TYPE,	//	Nothing is known about shape
	SOLID,			//	Shape is known to be solid
	SHAPE_TYPE_AS_IS	//	Indicates to set() to leave as is
    };

    enum FaceType {		// Hints about faces of shape:
	UNKNOWN_FACE_TYPE,	//	Nothing is known about faces
	CONVEX,			//	Faces are all convex
	FACE_TYPE_AS_IS		//	Indicates to set() to leave as is
    };

    // Initializes element
    virtual void	init(SoState *state);

    // Sets the current shape hints in the state.  Calls virtual
    // method that subclasses can override.
    static void		set(SoState *state, VertexOrdering vertexOrdering,
			    ShapeType shapeType, FaceType faceType);

#ifndef IV_STRICT
    static void         set(SoState *state, SoNode *, 
			    VertexOrdering vertexOrdering,
                            ShapeType shapeType, FaceType faceType)
        { set(state, vertexOrdering, shapeType, faceType); }
#endif

    // Returns current shape hints from the state
    static void		get(SoState *state,
			    VertexOrdering &vertexOrdering,
			    ShapeType &shapeType, FaceType &faceType);

    // Returns each default hint
    static VertexOrdering getDefaultVertexOrdering() {return UNKNOWN_ORDERING;}
    static ShapeType	  getDefaultShapeType()  { return UNKNOWN_SHAPE_TYPE; }
    static FaceType	  getDefaultFaceType()   { return CONVEX; }

    // Push copies the values from the previous element, so set
    // doesn't have to set them if they are set AS_IS.
    virtual void	push(SoState *state);

    // Returns TRUE if the hints match in both elements
    virtual SbBool	matches(const SoElement *elt) const;

    // Create and return a copy of this element
    virtual SoElement	*copyMatchInfo() const;

    // Prints element (for debugging)
    virtual void	print(FILE *fp) const;

  SoINTERNAL public:
    // Initializes the SoShapeHintsElement class
    static void		initClass();

  protected:
    // Virtual set methods that subclasses can override.
    virtual void	setElt(VertexOrdering vertexOrdering,
			       ShapeType shapeType, FaceType faceType);

    VertexOrdering	vertexOrdering;
    ShapeType		shapeType;
    FaceType		faceType;

    virtual ~SoShapeHintsElement();
};

#endif /* _SO_SHAPE_HINTS_ELEMENT */

