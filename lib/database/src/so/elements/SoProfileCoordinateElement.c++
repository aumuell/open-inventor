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
 |	SoProfileCoordinateElement
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/elements/SoProfileCoordinateElement.h>
#include <Inventor/errors/SoDebugError.h>

SO_ELEMENT_SOURCE(SoProfileCoordinateElement);

SbVec2f	*SoProfileCoordinateElement::defaultCoord2 = NULL;

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoProfileCoordinateElement::~SoProfileCoordinateElement()
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
SoProfileCoordinateElement::init(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    SoReplacedElement::init(state);

    // Initialize default coordinate storage if not already done
    if (defaultCoord2 == NULL) {
	defaultCoord2  = new SbVec2f;
	*defaultCoord2 = getDefault2();
    }

    // Assume 2D until told otherwise
    coordsAre2D	= TRUE;
    coords2	= defaultCoord2;
    numCoords	= 1;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the 2-D coordinates in element accessed from state.
//
// Use: public

void
SoProfileCoordinateElement::set2(SoState *state, SoNode *node,
			  int32_t numCoords, const SbVec2f *coords)
//
////////////////////////////////////////////////////////////////////////
{
    SoProfileCoordinateElement	*elt;

    // Get an instance we can change (pushing if necessary)
    elt = (SoProfileCoordinateElement *)
	getElement(state, classStackIndex, node);

    if (elt != NULL) {
	elt->numCoords   = numCoords;
	elt->coords2     = coords;
	elt->coordsAre2D = TRUE;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the 3-D coordinates in element accessed from state.
//
// Use: public

void
SoProfileCoordinateElement::set3(SoState *state, SoNode *node,
			  int32_t numCoords, const SbVec3f *coords)
//
////////////////////////////////////////////////////////////////////////
{
    SoProfileCoordinateElement	*elt;

    // Get an instance we can change (pushing if necessary)
    elt = (SoProfileCoordinateElement *)
	getElement(state, classStackIndex, node);

    if (elt != NULL) {
	elt->numCoords   = numCoords;
	elt->coords3     = coords;
	elt->coordsAre2D = FALSE;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the top (current) instance of the element in the state.
//
// Use: public

const SoProfileCoordinateElement *
SoProfileCoordinateElement::getInstance(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    return (const SoProfileCoordinateElement *)
	getConstElement(state, classStackIndex);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the indexed coordinate from an element as an SbVec2f.
//
// Use: public

const SbVec2f &
SoProfileCoordinateElement::get2(int index) const
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (index < 0 || index >= numCoords)
	SoDebugError::post("SoProfileCoordinateElement::get2",
			   "Index (%d) is out of range 0 - %d",
			   index, numCoords - 1);
#endif /* DEBUG */

    if (coordsAre2D)
	return coords2[index];

    // Convert from 3-D if necessary
    else {
	// Cast the const away...
	SoProfileCoordinateElement *elt = (SoProfileCoordinateElement *) this;
	const SbVec3f		&c3  = coords3[index];

	elt->convert2[0] = c3[0] / c3[2];
	elt->convert2[1] = c3[1] / c3[2];

	return convert2;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the indexed coordinate from an element as an SbVec3f.
//
// Use: public

const SbVec3f &
SoProfileCoordinateElement::get3(int index) const
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (index < 0 || index >= numCoords)
	SoDebugError::post("SoProfileCoordinateElement::get3",
			   "Index (%d) is out of range 0 - %d",
			   index, numCoords - 1);
#endif /* DEBUG */

    // Convert from 2-D if necessary
    if (coordsAre2D) {
	// Cast the const away...
	SoProfileCoordinateElement *elt = (SoProfileCoordinateElement *) this;
	const SbVec2f		&c2  = coords2[index];

	elt->convert3[0] = c2[0];
	elt->convert3[1] = c2[1];
	elt->convert3[2] = 0.0;

	return convert3;
    }

    else
	return coords3[index];
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
SoProfileCoordinateElement::print(FILE *fp) const
{
    SoReplacedElement::print(fp);
}
#else  /* DEBUG */
void
SoProfileCoordinateElement::print(FILE *) const
{
}
#endif /* DEBUG */
