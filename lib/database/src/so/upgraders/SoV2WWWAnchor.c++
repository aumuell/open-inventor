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
 * Copyright (C) 1995-96   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |	SoV2WWWAnchor
 |
 |   Author(s)		: David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include "SoV2WWWAnchor.h"
#include <Inventor/nodes/SoWWWAnchor.h>
#include <Inventor/errors/SoReadError.h>

SO_NODE_SOURCE(SoV2WWWAnchor);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Default constructor
//
// Use: public

SoV2WWWAnchor::SoV2WWWAnchor()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoV2WWWAnchor);

    SO_NODE_ADD_FIELD(name, ("<Undefined URL>"));
    SO_NODE_ADD_FIELD(description, (""));
    SO_NODE_ADD_FIELD(map, (NONE));
    SO_NODE_ADD_FIELD(alternateRep, (NULL));

    // Set up static info for enumerated type field
    SO_NODE_DEFINE_ENUM_VALUE(Mapping, NONE);
    SO_NODE_DEFINE_ENUM_VALUE(Mapping, POINT);

    // Set up info in enumerated type field
    SO_NODE_SET_SF_ENUM_TYPE(map, Mapping);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoV2WWWAnchor::~SoV2WWWAnchor()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Create a version 2.1 WWWAnchor.
//
// Use: private

SoNode *
SoV2WWWAnchor::createNewNode()
//
////////////////////////////////////////////////////////////////////////
{
    SoWWWAnchor *result = SO_UPGRADER_CREATE_NEW(SoWWWAnchor);

    //
    // Inventor 2.0 did not have WWWAnchor, When we convert 2.1 to 2.0 files,
    // the WWWAnchor gets written with an alternateRep (as well as written
    // with its children). When we read that 2.0 file back into 2.1, we can
    // discard the alternateRep. Programs written with 2.0 will gladly use
    // the alternateRep.
    //

    // Copy all fields as is (except alternateRep which is ignored)
    SO_UPGRADER_COPY_FIELD(name,         result);
    SO_UPGRADER_COPY_FIELD(description,  result);
    SO_UPGRADER_COPY_FIELD(map,          result);

    return result;
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//   special upgrade method to read field description
//
// Use: public, internal, virtual
SbBool 
SoV2WWWAnchor::upgrade(SoInput *in, const SbName &refName, SoBase *&result)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool isBinary = in->isBinary();
    if (in->isBinary()){
	SbString unknownString;
	SbBool  readOK = in->read(unknownString);
	if (!readOK || (unknownString != "fields" )) {
	    SoReadError::post(in, "Problem upgrading vertex property ") ;
	    return FALSE;
	}
    }
    return SoUpgrader::upgrade(in, refName, result);    
    
}
