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
 |	SoCoordinateElement
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/elements/SoCoordinateElement.h>
#include <Inventor/errors/SoDebugError.h>


SO_ELEMENT_SOURCE(SoCoordinateElement);

SbVec3f	*SoCoordinateElement::defaultCoord3 = NULL;

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoCoordinateElement::~SoCoordinateElement()
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
SoCoordinateElement::init(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    SoReplacedElement::init(state);

    // Initialize default coordinate storage if not already done
    if (defaultCoord3 == NULL) {
	defaultCoord3  = new SbVec3f;
	*defaultCoord3 = getDefault3();
    }

    // Assume 3D until told otherwise
    coordsAre3D	= TRUE;
    coords3	= defaultCoord3;
    numCoords	= 1;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the 3-D coordinates in element accessed from state.
//
// Use: public

void
SoCoordinateElement::set3(SoState *state, SoNode *node,
			  int32_t numCoords, const SbVec3f *coords)
//
////////////////////////////////////////////////////////////////////////
{
    SoCoordinateElement	*elt;

    // Get an instance we can change (pushing if necessary)
    elt = (SoCoordinateElement *) getElement(state, classStackIndex, node);

    if (elt != NULL) {
	elt->numCoords   = numCoords;
	elt->coords3     = coords;
	elt->coordsAre3D = TRUE;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the 4-D coordinates in element accessed from state.
//
// Use: public

void
SoCoordinateElement::set4(SoState *state, SoNode *node,
			  int32_t numCoords, const SbVec4f *coords)
//
////////////////////////////////////////////////////////////////////////
{
    SoCoordinateElement	*elt;

    // Get an instance we can change (pushing if necessary)
    elt = (SoCoordinateElement *) getElement(state, classStackIndex, node);

    if (elt != NULL) {
	elt->numCoords   = numCoords;
	elt->coords4     = coords;
	elt->coordsAre3D = FALSE;
    }
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the indexed coordinate from an element as an SbVec3f.
//
// Use: public

const SbVec3f &
SoCoordinateElement::get3(int index) const
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (index < 0 || index >= numCoords)
	SoDebugError::post("SoCoordinateElement::get3",
			   "Index (%d) is out of range 0 - %d",
			   index, numCoords - 1);
#endif /* DEBUG */

    if (coordsAre3D)
	return coords3[index];

    // Convert from 4-D if necessary
    else {
	// Cast the const away...
	SoCoordinateElement	*elt = (SoCoordinateElement *) this;
	const SbVec4f		&c4  = coords4[index];

	// If we can't do the projection, or we don't have to
	if (c4[3] == 0.0 || c4[3] == 1.0) {
	    elt->convert3[0] = c4[0];
	    elt->convert3[1] = c4[1];
	    elt->convert3[2] = c4[2];
	}

	else {
	    elt->convert3[0] = c4[0] / c4[3];
	    elt->convert3[1] = c4[1] / c4[3];
	    elt->convert3[2] = c4[2] / c4[3];
	}

	return convert3;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the indexed coordinate from an element as an SbVec4f.
//
// Use: public

const SbVec4f &
SoCoordinateElement::get4(int index) const
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (index < 0 || index >= numCoords)
	SoDebugError::post("SoCoordinateElement::get4",
			   "Index (%d) is out of range 0 - %d",
			   index, numCoords - 1);
#endif /* DEBUG */

    // Convert from 3-D if necessary
    if (coordsAre3D) {
	// Cast the const away...
	SoCoordinateElement	*elt = (SoCoordinateElement *) this;
	const SbVec3f		&c3  = coords3[index];

	elt->convert4[0] = c3[0];
	elt->convert4[1] = c3[1];
	elt->convert4[2] = c3[2];
	elt->convert4[3] = 1.0;

	return convert4;
    }

    else
	return coords4[index];
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
SoCoordinateElement::print(FILE *fp) const
{
    SoReplacedElement::print(fp);
}
#else  /* DEBUG */
void
SoCoordinateElement::print(FILE *) const
{
}
#endif /* DEBUG */
