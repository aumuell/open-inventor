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
 * Copyright (C) 1990-93   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |	SoV1ShapeHints
 |
 |   Author(s): Gavin Bell, David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include "Util.h"
#include "SoShapeHintsV1.h"
#include <Inventor/nodes/SoShapeHints.h>

SO_NODE_SOURCE(SoShapeHintsV1);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Default constructor
//
// Use: public

SoShapeHintsV1::SoShapeHintsV1()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoShapeHintsV1);

    SO_NODE_ADD_FIELD(hints,       (SURFACE | UNORDERED | CONCAVE));
    SO_NODE_ADD_FIELD(creaseAngle, (0.0));

    // Set up static info for enumerated type field
    SO_NODE_DEFINE_ENUM_VALUE(Hint, SOLID);
    SO_NODE_DEFINE_ENUM_VALUE(Hint, ORDERED);
    SO_NODE_DEFINE_ENUM_VALUE(Hint, CONVEX);
    SO_NODE_DEFINE_ENUM_VALUE(Hint, SURFACE);
    SO_NODE_DEFINE_ENUM_VALUE(Hint, UNORDERED);
    SO_NODE_DEFINE_ENUM_VALUE(Hint, CONCAVE);

    // Set up info in enumerated type field
    SO_NODE_SET_SF_ENUM_TYPE(hints, Hint);

    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoEnvironment class.
//
// Use: internal

void
SoShapeHintsV1::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoShapeHintsV1, "ShapeHints", SoNode);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Create a version 1.0 SoShapeHints.
//
// Use: static

SoNode *
SoShapeHintsV1::downgrade(SoShapeHints *shints2)
//
////////////////////////////////////////////////////////////////////////
{
    SoShapeHintsV1 *shints1 = new SoShapeHintsV1;
    
    // Convert from 2.0 fields to 1.0 enums
    int enumval = 0;
    
    if ((! shints2->shapeType.isIgnored()) &&
	shints2->shapeType.getValue() == SoShapeHints::SOLID)
	enumval |= SOLID;
	
    if ((! shints2->vertexOrdering.isIgnored()) &&
	shints2->vertexOrdering.getValue() == SoShapeHints::COUNTERCLOCKWISE)
	enumval |= ORDERED;
	
    if ((! shints2->faceType.isIgnored()) &&
	(shints2->faceType.isDefault() || 
	shints2->faceType.getValue() == SoShapeHints::CONVEX))
	enumval |= CONVEX;
	
    shints1->hints = enumval;

    COPY_FIELD(shints1, shints2, creaseAngle);
    
    return shints1;
}

