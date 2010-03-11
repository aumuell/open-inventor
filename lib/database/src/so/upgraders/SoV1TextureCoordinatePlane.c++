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
 |	SoV1TextureCoordinatePlane
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include "SoV1TextureCoordinatePlane.h"
#include <Inventor/nodes/SoTextureCoordinatePlane.h>
#include <Inventor/nodes/SoTexture2Transform.h>

SO_NODE_SOURCE(SoV1TextureCoordinatePlane);

// Static vars:
SbVec3f SoV1TextureCoordinatePlane::sDir;
SbVec3f SoV1TextureCoordinatePlane::tDir;

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Default constructor
//
// Use: public

SoV1TextureCoordinatePlane::SoV1TextureCoordinatePlane()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoV1TextureCoordinatePlane);

    SO_NODE_ADD_FIELD(coord, (ALL));
    SO_NODE_ADD_FIELD(normal,(SbVec3f(1, 0, 0)));
    SO_NODE_ADD_FIELD(distanceFromOrigin, (0));
    SO_NODE_ADD_FIELD(repeatInterval, (1));

    // Set up static info for enumerated type field
    SO_NODE_DEFINE_ENUM_VALUE(Coordinate, S);
    SO_NODE_DEFINE_ENUM_VALUE(Coordinate, T);
    SO_NODE_DEFINE_ENUM_VALUE(Coordinate, ALL);

    // Set up info in enumerated type field
    SO_NODE_SET_SF_ENUM_TYPE(coord, Coordinate);

    // This upgrader returns a group, but it is an upgrade of a non-group
    isGroup = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoV1TextureCoordinatePlane::~SoV1TextureCoordinatePlane()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Create a version 2.0 SoTextureCoordinatePlane.
//
// Use: private

SoNode *
SoV1TextureCoordinatePlane::createNewNode()
//
////////////////////////////////////////////////////////////////////////
{
    SoTextureCoordinatePlane *tex = SO_UPGRADER_CREATE_NEW(SoTextureCoordinatePlane);
    tex->ref();

    // Convert from old enums to new fields:
    
    // The math is really pretty easy:
    SbVec3f norm = normal.getValue();
    norm.normalize();
    SbVec3f direction = norm / repeatInterval.getValue();
    
    // If translation is non-zero, we'll need to add a
    // Texture2Transform node...
    float translation = distanceFromOrigin.getValue() / 
	repeatInterval.getValue();
    
    // Figure out which coordinate(s) to set.  We set static variables
    // because it is no longer possible to inherit the s/t coordinates
    // separately across two nodes, so we'll remember the values
    // across nodes...

    if (coord.getValue() == ALL || coord.getValue() == S) {
	sDir = direction;
    }
    if (coord.getValue() == ALL || coord.getValue() == T) {
	tDir = direction;
    }
    
    tex->directionS = sDir;
    tex->directionT = tDir;

    if (translation == 0.0) {
	tex->unrefNoDelete();
	return tex;
    }
    
    // OK, have to make a group....
    SoGroup *result = SO_UPGRADER_CREATE_NEW(SoGroup);
    result->ref();
    result->addChild(tex);
    tex->unref();
    
    SoTexture2Transform *xf = SO_UPGRADER_CREATE_NEW(SoTexture2Transform);
    if (coord.getValue() == ALL || coord.getValue() == S) {
	xf->translation.setValue(translation, 0);
    }
    if (coord.getValue() == ALL || coord.getValue() == T) {
	xf->translation = xf->translation.getValue() +
	    SbVec2f(0, translation);
    }
    result->addChild(xf);
    result->unrefNoDelete();

    return result;
}

