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
 * Copyright (C) 1990,91,92   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |	SoV2Text3
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include "SoV2Text3.h"
#include "SoV2Text2.h"
#include <Inventor/nodes/SoText3.h>


SO_NODE_SOURCE(SoV2Text3);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Default constructor
//
// Use: public

SoV2Text3::SoV2Text3()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoV2Text3);

    SO_NODE_ADD_FIELD(string,	(""));
    SO_NODE_ADD_FIELD(spacing,	(1.0));
    SO_NODE_ADD_FIELD(justification,	(LEFT));
    SO_NODE_ADD_FIELD(parts,		(FRONT));

    // Set up static info for enumerated type field
    SO_NODE_DEFINE_ENUM_VALUE(Justification,	LEFT);
    SO_NODE_DEFINE_ENUM_VALUE(Justification,	RIGHT);
    SO_NODE_DEFINE_ENUM_VALUE(Justification,	CENTER);

    // Set up static info for enumerated type field
    SO_NODE_DEFINE_ENUM_VALUE(Part, SIDES);
    SO_NODE_DEFINE_ENUM_VALUE(Part, FRONT);
    SO_NODE_DEFINE_ENUM_VALUE(Part, BACK);
    SO_NODE_DEFINE_ENUM_VALUE(Part, ALL);

    // Set up info in enumerated type field
    SO_NODE_SET_SF_ENUM_TYPE(justification, Justification);
    SO_NODE_SET_SF_ENUM_TYPE(parts, Part);
    
    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoV2Text3::~SoV2Text3()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Create a version 2.1 SoText3.
//
// Use: private

SoNode *
SoV2Text3::createNewNode()
//
////////////////////////////////////////////////////////////////////////
{
    SoText3 *result = SO_UPGRADER_CREATE_NEW(SoText3);

    // If the parts field is default, change it to ALL (since the
    // default changed from ALL to FRONT from 1.0 to 2.0).
    if (parts.isDefault()) {
	result->parts.setValue(SoText3::ALL);
    } else {
	result->parts.setValue(parts.getValue());
    }

    // if european characters present they are converted to UTF-8
    for (int i = 0; i < string.getNum(); i++) {
	SbString str("");
	if (SoV2Text2::convertToUTF8(string[i], str)) {
	    result->string.set1Value(i, str);	
	}
    }

    result->spacing.setValue(spacing.getValue());
    result->justification.setValue(justification.getValue());

    return result;
}
