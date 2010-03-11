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
 |	SoPickStyleV1
 |
 |   Author(s): Paul S. Strauss, David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include "SoPickStyleV1.h"
#include <Inventor/nodes/SoPickStyle.h>

SO_NODE_SOURCE(SoPickStyleV1);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Default constructor
//
// Use: public

SoPickStyleV1::SoPickStyleV1()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoPickStyleV1);

    SO_NODE_ADD_FIELD(style, (PICKABLE));

    // Set up static info for enumerated type field
    SO_NODE_DEFINE_ENUM_VALUE(Style, PICKABLE);
    SO_NODE_DEFINE_ENUM_VALUE(Style, UNPICKABLE);

    // Set up info in enumerated type field
    SO_NODE_SET_SF_ENUM_TYPE(style, Style);

    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoEnvironment class.
//
// Use: internal

void
SoPickStyleV1::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoPickStyleV1, "PickStyle", SoNode);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Downgrade the passed 2.0 node to this 1.0 node.
//
SoNode *
SoPickStyleV1::downgrade(SoPickStyle *pstyle2)
//
////////////////////////////////////////////////////////////////////////
{
    SoPickStyleV1 *pstyle1 = new SoPickStyleV1;

    // Convert from old enum to new enum
    if (! pstyle2->style.isDefault()) {
	switch ((Style) pstyle2->style.getValue()) {

	  case SoPickStyle::SHAPE:
	  case SoPickStyle::BOUNDING_BOX:
	    pstyle1->style = PICKABLE;
	    break;

	  case SoPickStyle::UNPICKABLE:
	    pstyle1->style = UNPICKABLE;
	    break;
	}

	if (pstyle2->style.isIgnored())
	    pstyle1->style.setIgnored(TRUE);
    }
    
    return pstyle1;
}
