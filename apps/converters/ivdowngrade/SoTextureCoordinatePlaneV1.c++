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
 |	SoTextureCoordinatePlaneV1
 |
 |   Author(s): Gavin Bell, David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include "SoTextureCoordinatePlaneV1.h"
#include <Inventor/nodes/SoTextureCoordinatePlane.h>
#include <Inventor/nodes/SoGroup.h>

SO_NODE_SOURCE(SoTextureCoordinatePlaneV1);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Default constructor
//
// Use: public

SoTextureCoordinatePlaneV1::SoTextureCoordinatePlaneV1()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoTextureCoordinatePlaneV1);

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

    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoEnvironment class.
//
// Use: internal

void
SoTextureCoordinatePlaneV1::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoTextureCoordinatePlaneV1, 
			"TextureCoordinatePlane", SoNode);
}

// 
// Used by createAndDowngrade
//
static
SoTextureCoordinatePlaneV1 *
getTexCoordPlane(const SbVec3f &dir)
{
    SoTextureCoordinatePlaneV1 *tcp = new SoTextureCoordinatePlaneV1;
    SbVec3f v(dir.getValue());
    tcp->repeatInterval = 1.0/v.normalize();	// length
    tcp->normal = v;				// normalized vector
    
    return tcp;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Create a version 1.0 SoTextureCoordinatePlane.
//
// Use: static
//
SoNode *
SoTextureCoordinatePlaneV1::downgrade(
    SoTextureCoordinatePlane *tcp2)
//
////////////////////////////////////////////////////////////////////////
{
    SoNode *returnNode;
    
    // One node or two?
    const SbVec3f & sdir = tcp2->directionS.getValue();
    const SbVec3f & tdir = tcp2->directionT.getValue();
    
    if (sdir == tdir) {
	// S and T are the same - return one node
	SoTextureCoordinatePlaneV1 *tcp = getTexCoordPlane(sdir);
	tcp->coord = SoTextureCoordinatePlaneV1::ALL;
	returnNode = tcp;
    }
    else {
	// S and T are different - return two nodes under a group
	SoTextureCoordinatePlaneV1 *tcpS = getTexCoordPlane(sdir);
	tcpS->coord = SoTextureCoordinatePlaneV1::S;
	
	SoTextureCoordinatePlaneV1 *tcpT = getTexCoordPlane(tdir);
	tcpT->coord = SoTextureCoordinatePlaneV1::T;
	
	SoGroup *g = new SoGroup;
	g->addChild(tcpS);
	g->addChild(tcpT);
	
	returnNode = g;
    }

    return returnNode;
}

