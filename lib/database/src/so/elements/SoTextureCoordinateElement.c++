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
 |   $Revision $
 |
 |   Classes:
 |	SoTextureCoordinateElement class.
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson, Thad Beier,
 |                        Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/elements/SoTextureCoordinateElement.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/elements/SoShapeStyleElement.h>

SO_ELEMENT_SOURCE(SoTextureCoordinateElement);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoTextureCoordinateElement::~SoTextureCoordinateElement()
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
SoTextureCoordinateElement::init(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    SoReplacedElement::init(state);

    whatKind  = EXPLICIT;
    funcCB    = NULL;
    numCoords = 0;
    coords2   = NULL;
    coords4   = NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Tell shapes to generate their own texture coordinates.  This is
//    the default.
//
// Use: public

void
SoTextureCoordinateElement::setDefault(SoState *state, SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
    SoTextureCoordinateElement	*elt;

    elt = (SoTextureCoordinateElement *)
	getElement(state, classStackIndex, node);

    if (elt != NULL) {
	elt->whatKind = EXPLICIT;
	elt->numCoords = 0;
	elt->coords2 = NULL;
	elt->coords4 = NULL;
    }
    //The shapeStyle element will track this value:
    SoShapeStyleElement::setTextureFunction(state, FALSE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the current coords, given a function to compute them.
//
// Use: public

void
SoTextureCoordinateElement::setFunction(SoState *state, SoNode *node,
					SoTextureCoordinateFunctionCB *func,
					void *userData)
//
////////////////////////////////////////////////////////////////////////
{
    SoTextureCoordinateElement	*elt;

    elt = (SoTextureCoordinateElement *) getElement(state,
				    classStackIndex, node);
    if (elt != NULL) {
	elt->whatKind = FUNCTION;
	elt->funcCB = func;
	elt->funcCBData = userData;
    }
    //The shapeStyle element will track this value:
    SoShapeStyleElement::setTextureFunction(state, TRUE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the current coords, given a set of 2D texture coordinates.
//
// Use: public

void
SoTextureCoordinateElement::set2(SoState *state, SoNode *node,
				 int32_t numCoords, const SbVec2f *coords)
//
////////////////////////////////////////////////////////////////////////
{
    SoTextureCoordinateElement	*elt;

    elt = (SoTextureCoordinateElement *)
	getElement(state, classStackIndex, node);

    if (elt != NULL) {
	elt->whatKind    = EXPLICIT;
	elt->numCoords   = numCoords;
	elt->coords2     = coords;
	elt->coordsAre2D = TRUE;
    }
    SoShapeStyleElement::setTextureFunction(state, FALSE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the current coords, given a set of 4D texture coordinates.
//
// Use: public

void
SoTextureCoordinateElement::set4(SoState *state, SoNode *node,
				 int32_t numCoords, const SbVec4f *coords)
//
////////////////////////////////////////////////////////////////////////
{
    SoTextureCoordinateElement	*elt;

    elt = (SoTextureCoordinateElement *)
	getElement(state, classStackIndex, node);

    if (elt != NULL) {
	elt->whatKind    = EXPLICIT;
	elt->numCoords   = numCoords;
	elt->coords4     = coords;
	elt->coordsAre2D = FALSE;
    }
     SoShapeStyleElement::setTextureFunction(state, FALSE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns code depending on which set routine was called.
//
// Use: public, static

SoTextureCoordinateElement::CoordType
SoTextureCoordinateElement::getType(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    return getInstance(state)->getType();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Non-static (virtual) version of above method.
//
// Use: public

SoTextureCoordinateElement::CoordType
SoTextureCoordinateElement::getType() const
//
////////////////////////////////////////////////////////////////////////
{
    return whatKind;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the top (current) instance of the element in the state.
//
// Use: public

const SoTextureCoordinateElement *
SoTextureCoordinateElement::getInstance(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    return (const SoTextureCoordinateElement *)
	getConstElement(state, classStackIndex);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Given a point and a normal, call the registered callback to get
//    corresponding texture coordinates:
//
// Use: public

const SbVec4f &
SoTextureCoordinateElement::get(const SbVec3f &point,
				const SbVec3f &normal) const
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (whatKind != FUNCTION)
	SoDebugError::post("SoTextureCoordinateElement::get",
			   "Functional texture coordinates were not set!");
#endif /* DEBUG */
    return (*funcCB)(funcCBData, point, normal);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the indexed coordinate from an element as an SbVec2f.
//
// Use: public

const SbVec2f &
SoTextureCoordinateElement::get2(int index) const
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (whatKind != EXPLICIT)
	SoDebugError::post("SoTextureCoordinateElement::get2",
			   "Explicit texture coordinates were not set!");

    if (index < 0 || index >= numCoords)
	SoDebugError::post("SoTextureCoordinateElement::get2",
			   "Index (%d) is out of range 0 - %d",
			   index, numCoords - 1);
#endif /* DEBUG */

    if (coordsAre2D)
	return coords2[index];

    // Convert from 4-D if necessary
    else {
	// Cast the const away...
	SoTextureCoordinateElement *elt = (SoTextureCoordinateElement *) this;
	const SbVec4f		   &c4  = coords4[index];

	// If we can't do the projection, or we don't have to
	if (c4[3] == 0.0 || c4[3] == 1.0) {
	    elt->convert2[0] = c4[0];
	    elt->convert2[1] = c4[1];
	}

	else {
	    elt->convert2[0] = c4[0] / c4[3];
	    elt->convert2[1] = c4[1] / c4[3];
	}

	return convert2;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the indexed coordinate from an element as an SbVec4f.
//
// Use: public

const SbVec4f &
SoTextureCoordinateElement::get4(int index) const
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (whatKind != EXPLICIT)
	SoDebugError::post("SoTextureCoordinateElement::get4",
			   "Explicit texture coordinates were not set!");

    if (index < 0 || index >= numCoords)
	SoDebugError::post("SoTextureCoordinateElement::get4",
			   "Index (%d) is out of range 0 - %d",
			   index, numCoords - 1);
#endif /* DEBUG */

    // Convert from 2-D if necessary
    if (coordsAre2D) {
	// Cast the const away...
	SoTextureCoordinateElement *elt = (SoTextureCoordinateElement *) this;
	const SbVec2f		   &c2  = coords2[index];

	elt->convert4[0] = c2[0];
	elt->convert4[1] = c2[1];
	elt->convert4[2] = 0.0;
	elt->convert4[3] = 1.0;

	return convert4;
    }

    else
	return coords4[index];
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Prints contents of element for debugging.
//
// Use: public
//
////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
void
SoTextureCoordinateElement::print(FILE *fp) const
{
    SoReplacedElement::print(fp);
}
#else  /* DEBUG */
void
SoTextureCoordinateElement::print(FILE *) const
{
}
#endif /* DEBUG */

