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
 |	SoV1TextureCoordinateSphere
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include "SoV1TextureCoordinateSphere.h"
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodes/SoInfo.h>

SO_NODE_SOURCE(SoV1TextureCoordinateSphere);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Default constructor
//
// Use: public

SoV1TextureCoordinateSphere::SoV1TextureCoordinateSphere()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoV1TextureCoordinateSphere);

    SO_NODE_ADD_FIELD(coord, (ALL));
    SO_NODE_ADD_FIELD(center,(SbVec3f(0, 0, 0)));
    SO_NODE_ADD_FIELD(rotation, (SbRotation()));

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

SoV1TextureCoordinateSphere::~SoV1TextureCoordinateSphere()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Create a version 2.0 SoTextureCoordinateSphere.
//
// Use: private

SoNode *
SoV1TextureCoordinateSphere::createNewNode()
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    // Warn that this is obsolete...
    SoDebugError::postWarning(
	"SoV1TextureCoordinateSphere::createNewNode",
	"The TextureCoordinateSphere class is obsolete, replacing "
	"with an Info node");
#endif	
    SoInfo *info = SO_UPGRADER_CREATE_NEW(SoInfo);
    
    info->string = "1.0 TextureCoordinateSphere was here";

    return info;
}

