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

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |	SoShapeHintsElement
 |
 |   Author(s)		: Paul S. Strauss, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/elements/SoShapeHintsElement.h>
#include <Inventor/misc/SoState.h>

SO_ELEMENT_SOURCE(SoShapeHintsElement);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoShapeHintsElement::~SoShapeHintsElement()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes element.
//
// Use: public

void
SoShapeHintsElement::init(SoState *)
//
////////////////////////////////////////////////////////////////////////
{
    vertexOrdering	= getDefaultVertexOrdering();
    shapeType		= getDefaultShapeType();
    faceType		= getDefaultFaceType();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets shape hints in element accessed from state.
//
// Use: public, static

void
SoShapeHintsElement::set(SoState *state, VertexOrdering _vertexOrdering,
			 ShapeType _shapeType, FaceType _faceType)
//
////////////////////////////////////////////////////////////////////////
{
    SoShapeHintsElement	*elt;

    // Get an instance we can change (pushing if necessary)
    elt = (SoShapeHintsElement *) getElement(state, classStackIndex);

    if (elt != NULL)
	elt->setElt(_vertexOrdering, _shapeType, _faceType);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pushes the element, copying values from previous element in
//    state:
//
// Use: public, virtual

void
SoShapeHintsElement::push(SoState *)
//
////////////////////////////////////////////////////////////////////////
{
    const SoShapeHintsElement	*prevElt =
	(const SoShapeHintsElement *)getNextInStack();

    vertexOrdering	= prevElt->vertexOrdering;
    faceType		= prevElt->faceType;
    shapeType		= prevElt->shapeType;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Virtual set method; the GL version of this element overrides
//    this to send stuff to GL, too...
//
// Use: protected

void
SoShapeHintsElement::setElt(VertexOrdering _vertexOrdering,
			    ShapeType _shapeType, FaceType _faceType)
//
////////////////////////////////////////////////////////////////////////
{
    // Set the hints in the elements. If any of the hints is
    // AS_IS, leave the values alone.

    if (_vertexOrdering != ORDERING_AS_IS)
	vertexOrdering = _vertexOrdering;
    if (_shapeType != SHAPE_TYPE_AS_IS)
	shapeType = _shapeType;
    if (_faceType != FACE_TYPE_AS_IS)
	faceType = _faceType;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns shape hints from state.
//
// Use: public, static

void
SoShapeHintsElement::get(SoState *state,
			   VertexOrdering &vertexOrdering,
			   ShapeType &shapeType, FaceType &faceType)
//
////////////////////////////////////////////////////////////////////////
{
    const SoShapeHintsElement *elt;

    elt = (const SoShapeHintsElement *)
	getConstElement(state, classStackIndex);

    vertexOrdering	= elt->vertexOrdering;
    shapeType		= elt->shapeType;
    faceType		= elt->faceType;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides this method to compare shape hints.
//
// Use: public

SbBool
SoShapeHintsElement::matches(const SoElement *elt) const
//
////////////////////////////////////////////////////////////////////////
{
    const SoShapeHintsElement *shElt = (const SoShapeHintsElement *) elt;

    return (vertexOrdering == shElt->vertexOrdering	&&
	    shapeType	   == shElt->shapeType		&&
	    faceType	   == shElt->faceType);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//     Create a copy of this instance suitable for calling matches()
//     on.
//
// Use: protected

SoElement *
SoShapeHintsElement::copyMatchInfo() const
//
////////////////////////////////////////////////////////////////////////
{
    SoShapeHintsElement *result =
	(SoShapeHintsElement *)getTypeId().createInstance();

    result->vertexOrdering	= vertexOrdering;
    result->shapeType		= shapeType;
    result->faceType		= faceType;
    
    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Prints element for debugging.
//
// Use: public
//
////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
void
SoShapeHintsElement::print(FILE *fp) const
{
    const char *orderName, *shapeName, *faceName;

    SoElement::print(fp);

    switch (vertexOrdering) {
      case UNKNOWN_ORDERING:
	orderName = "UNKNOWN_ORDERING";
	break;
      case CLOCKWISE:
	orderName = "CLOCKWISE";
	break;
      case COUNTERCLOCKWISE:
	orderName = "COUNTERCLOCKWISE";
	break;
      default:	// Just to shut CC up
	break;
    }

    switch (shapeType) {
      case UNKNOWN_SHAPE_TYPE:
	shapeName = "UNKNOWN_SHAPE_TYPE";
	break;
      case SOLID:
	shapeName = "SOLID";
	break;
      default:	// Just to shut CC up
	break;
    }

    switch (faceType) {
      case UNKNOWN_FACE_TYPE:
	faceName = "UNKNOWN_FACE_TYPE";
	break;
      case CONVEX:
	faceName = "CONVEX";
	break;
      default:	// Just to shut CC up
	break;
    }

    fprintf(fp,
	    "\tShape Hints: vertex ordering = %s, shape type = %s,\n"
	    "\t\t     face type = %s\n",
	    orderName, shapeName, faceName);
}
#else  /* DEBUG */
void
SoShapeHintsElement::print(FILE *) const
{
}
#endif /* DEBUG */
