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
 |	SoV1TextureCoordinateEnvironment
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include "SoV1TextureCoordinateEnvironment.h"
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodes/SoTextureCoordinateEnvironment.h>
#include <Inventor/nodes/SoTexture2Transform.h>

SO_NODE_SOURCE(SoV1TextureCoordinateEnvironment);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Default constructor
//
// Use: public

SoV1TextureCoordinateEnvironment::SoV1TextureCoordinateEnvironment()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoV1TextureCoordinateEnvironment);

    SO_NODE_ADD_FIELD(coord, (ALL));

    // Set up static info for enumerated type field
    SO_NODE_DEFINE_ENUM_VALUE(Coordinate, S);
    SO_NODE_DEFINE_ENUM_VALUE(Coordinate, T);
    SO_NODE_DEFINE_ENUM_VALUE(Coordinate, ALL);

    // Set up info in enumerated type field
    SO_NODE_SET_SF_ENUM_TYPE(coord, Coordinate);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoV1TextureCoordinateEnvironment::~SoV1TextureCoordinateEnvironment()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Create a version 2.0 SoTextureCoordinateEnvironment.
//
// Use: private

SoNode *
SoV1TextureCoordinateEnvironment::createNewNode()
//
////////////////////////////////////////////////////////////////////////
{
    SoTextureCoordinateEnvironment *tex = 
	SO_UPGRADER_CREATE_NEW(SoTextureCoordinateEnvironment);
    tex->ref();

#ifdef DEBUG
    // Warn if coord != ALL
    if (coord.getValue() != ALL) {
	SoDebugError::postWarning(
	    "SoV1TextureCoordinateEnvironment::createNewNode",
	    "Independent setting of s/t coordinates no longer "
	    "supported (coord will be treated as ALL)");
    }
#endif	
    
    tex->unrefNoDelete();
    
    return tex;
}

