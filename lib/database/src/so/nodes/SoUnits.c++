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
 |      SoUnits
 |
 |   Author(s)          : Dave Immel, Thad Beier
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoPickAction.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoUnitsElement.h>
#include <Inventor/nodes/SoUnits.h>

SO_NODE_SOURCE(SoUnits);

const float SoUnits::factor[14] = {
    1,
    1e2,
    1e3,
    1e6,
    1e6,
    1e9,
    1e10,
    1e-3,
    3.280840,
    3.937008e01,
    2.834646e03,
    1.093613,
    6.213712e-04,
    5.399568e-04
};

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoUnits::SoUnits()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoUnits);
    SO_NODE_ADD_FIELD(units, (SoUnits::METERS));
    isBuiltIn = TRUE;

    SO_NODE_DEFINE_ENUM_VALUE(Units, METERS);
    SO_NODE_DEFINE_ENUM_VALUE(Units, CENTIMETERS);
    SO_NODE_DEFINE_ENUM_VALUE(Units, MILLIMETERS);
    SO_NODE_DEFINE_ENUM_VALUE(Units, MICROMETERS);
    SO_NODE_DEFINE_ENUM_VALUE(Units, MICRONS);
    SO_NODE_DEFINE_ENUM_VALUE(Units, NANOMETERS);
    SO_NODE_DEFINE_ENUM_VALUE(Units, ANGSTROMS);
    SO_NODE_DEFINE_ENUM_VALUE(Units, KILOMETERS);
    SO_NODE_DEFINE_ENUM_VALUE(Units, FEET);
    SO_NODE_DEFINE_ENUM_VALUE(Units, INCHES);
    SO_NODE_DEFINE_ENUM_VALUE(Units, POINTS);
    SO_NODE_DEFINE_ENUM_VALUE(Units, YARDS);
    SO_NODE_DEFINE_ENUM_VALUE(Units, MILES);
    SO_NODE_DEFINE_ENUM_VALUE(Units, NAUTICAL_MILES);

    // Set up info in enumerated type field
    SO_NODE_SET_SF_ENUM_TYPE(units, Units);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor (necessary since inline destructor is too complex)
//
// Use: private

SoUnits::~SoUnits()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the units element and scales by the quotient of this unit
//    and the previous unit
//
// Use: extender

void
SoUnits::doAction(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoState			*state = action->getState();

    if (! units.isIgnored()) {
	float 	oldFactor = SoUnits::factor[(int) SoUnitsElement::get(state)];
	float	newFactor = SoUnits::factor[(int) units.getValue()];
	float	ratio	  = oldFactor / newFactor;

	SbVec3f	sc(ratio, ratio, ratio);

	SoModelMatrixElement::scaleBy(state, this, sc);
	SoUnitsElement::set(state,
			    (SoUnitsElement::Units) units.getValue());
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Handles callback action
//
// Use: extender

void
SoUnits::callback(SoCallbackAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoUnits::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Handles GL render action
//
// Use: extender

void
SoUnits::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoUnits::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Handles get bounding box action
//
// Use: extender

void
SoUnits::getBoundingBox(SoGetBoundingBoxAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoUnits::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Updates state for get matrix action
//
// Use: extender

void
SoUnits::getMatrix(SoGetMatrixAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoState *state = action->getState();

    if (! units.isIgnored()) {
	float 	oldFactor = SoUnits::factor[(int) SoUnitsElement::get(state)];
	float	newFactor = SoUnits::factor[(int) units.getValue()];
	float	ratio	  = oldFactor / newFactor;

	SbVec3f		sc(ratio, ratio, ratio);
	SbVec3f		si(1.0/ratio, 1.0/ratio, 1.0/ratio);
	SbMatrix	&ctm = action->getMatrix();
	SbMatrix	&inv = action->getInverse();
	SbMatrix	m;

	m.setScale(sc);
	ctm.multLeft(m);
	m.setScale(si);
	inv.multRight(m);

	SoUnitsElement::set(state,
			    (SoUnitsElement::Units) units.getValue());
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Handles pick action.
//
// Use: extender

void
SoUnits::pick(SoPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoUnits::doAction(action);
}
