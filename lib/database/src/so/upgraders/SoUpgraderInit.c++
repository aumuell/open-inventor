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
 |   Class:
 |	call initClasses for all upgrader classes
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include "SoUpgraders.h"

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes ALL Inventor upgrader classes.
//
// Use: internal

void
SoUpgrader::initClasses()
//
////////////////////////////////////////////////////////////////////////
{
    // Base class must be initialized first
    SoUpgrader::initClass();

    SoV1CustomNode::initClass();
    SoV1DrawStyle::initClass();
    SoV1Environment::initClass();
    SoV1IndexedTriangleMesh::initClass();
    SoV1LayerGroup::initClass();
    SoV1LightModel::initClass();
    SoV1Material::initClass();
    SoV1PackedColor::initClass();
    SoV1PickStyle::initClass();
    SoV1Separator::initClass();
    SoV1ShapeHints::initClass();
    SoV1Texture2::initClass();
    SoV1Text3::initClass();
    SoV1TextureCoordinateCube::initClass();
    SoV1TextureCoordinateCylinder::initClass();
    SoV1TextureCoordinateEnvironment::initClass();
    SoV1TextureCoordinatePlane::initClass();
    SoV1TextureCoordinateSphere::initClass();

    SoV2Text2::initClass();
    SoV2Text3::initClass();
    SoV2WWWAnchor::initClass();
    SoV2WWWInline::initClass();
    SoV2AsciiText::initClass();
    SoV2FontStyle::initClass();
    SoV2MaterialIndex::initClass();
    SoV2VertexProperty::initClass();
    SoV2LOD::initClass();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoUpgrader class.
//
// Use: internal

void
SoUpgrader::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_ABSTRACT_CLASS(SoUpgrader, "Upgrader", SoGroup);

    classTypeId.makeInternal();
    upgradeDictV1 = new SbDict(50);
    upgradeDictV2 = new SbDict(10); // there are only a few classes to upgrade
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1CustomNode class.
//
// Use: internal

void
SoV1CustomNode::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_UPGRADER_INIT_CLASS(SoV1CustomNode);
    SO_REGISTER_UPGRADER(CustomNode, 1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1DrawStyle class.
//
// Use: internal

void
SoV1DrawStyle::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_UPGRADER_INIT_CLASS(SoV1DrawStyle);
    SO_REGISTER_UPGRADER(DrawStyle, 1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1Environment class.
//
// Use: internal

void
SoV1Environment::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_UPGRADER_INIT_CLASS(SoV1Environment);
    SO_REGISTER_UPGRADER(Environment, 1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1IndexedTriangleMesh class.
//
// Use: internal

void
SoV1IndexedTriangleMesh::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_UPGRADER_INIT_CLASS(SoV1IndexedTriangleMesh);
    SO_REGISTER_UPGRADER(IndexedTriangleMesh, 1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1LayerGroup class.
//
// Use: internal

void
SoV1LayerGroup::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_UPGRADER_INIT_CLASS(SoV1LayerGroup);
    SO_REGISTER_UPGRADER(LayerGroup, 1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1LightModel class.
//
// Use: internal

void
SoV1LightModel::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_UPGRADER_INIT_CLASS(SoV1LightModel);
    SO_REGISTER_UPGRADER(LightModel, 1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1Material class.
//
// Use: internal

void
SoV1Material::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_UPGRADER_INIT_CLASS(SoV1Material);
    SO_REGISTER_UPGRADER(Material, 1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1PackedColor class.
//
// Use: internal

void
SoV1PackedColor::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    // PackedColor changed for 2.1, so we have to upgrade from both 1.0 and 2.0
    SO_UPGRADER_INIT_CLASS(SoV1PackedColor);
    SO_REGISTER_UPGRADER(PackedColor, 1.0);
    SO_REGISTER_UPGRADER(PackedColor, 2.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1PickStyle class.
//
// Use: internal

void
SoV1PickStyle::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_UPGRADER_INIT_CLASS(SoV1PickStyle);
    SO_REGISTER_UPGRADER(PickStyle, 1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1Separator class.
//
// Use: internal

void
SoV1Separator::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_UPGRADER_INIT_CLASS(SoV1Separator);
    SO_REGISTER_UPGRADER(Separator, 1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1ShapeHints class.
//
// Use: internal

void
SoV1ShapeHints::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_UPGRADER_INIT_CLASS(SoV1ShapeHints);
    SO_REGISTER_UPGRADER(ShapeHints, 1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1Text3 class.
//
// Use: internal

void
SoV1Text3::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_UPGRADER_INIT_CLASS(SoV1Text3);
    SO_REGISTER_UPGRADER(Text3, 1.0);
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV2Text3 class.
//
// Use: internal

void
SoV2Text3::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_UPGRADER_INIT_CLASS(SoV2Text3);
    SO_REGISTER_UPGRADER(Text3, 2.0);
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV2Text2 class.
//
// Use: internal

void
SoV2Text2::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_UPGRADER_INIT_CLASS(SoV2Text2);
    SO_REGISTER_UPGRADER(Text2, 2.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1Texture2 class.
//
// Use: internal

void
SoV1Texture2::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_UPGRADER_INIT_CLASS(SoV1Texture2);
    SO_REGISTER_UPGRADER(Texture2, 1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1TextureCoordinateCube class.
//
// Use: internal

void
SoV1TextureCoordinateCube::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_UPGRADER_INIT_CLASS(SoV1TextureCoordinateCube);
    SO_REGISTER_UPGRADER(TextureCoordinateCube, 1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1TextureCoordinateCylinder class.
//
// Use: internal

void
SoV1TextureCoordinateCylinder::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_UPGRADER_INIT_CLASS(SoV1TextureCoordinateCylinder);
    SO_REGISTER_UPGRADER(TextureCoordinateCylinder, 1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1TextureCoordinateEnvironment class.
//
// Use: internal

void
SoV1TextureCoordinateEnvironment::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_UPGRADER_INIT_CLASS(SoV1TextureCoordinateEnvironment);
    SO_REGISTER_UPGRADER(TextureCoordinateEnvironment, 1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1TextureCoordinatePlane class.
//
// Use: internal

void
SoV1TextureCoordinatePlane::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_UPGRADER_INIT_CLASS(SoV1TextureCoordinatePlane);
    SO_REGISTER_UPGRADER(TextureCoordinatePlane, 1.0);

    sDir = SbVec3f(1,0,0);
    tDir = SbVec3f(0,1,0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1TextureCoordinateSphere class.
//
// Use: internal

void
SoV1TextureCoordinateSphere::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_UPGRADER_INIT_CLASS(SoV1TextureCoordinateSphere);
    SO_REGISTER_UPGRADER(TextureCoordinateSphere, 1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV2WWWAnchor class.
//
// Use: internal

void
SoV2WWWAnchor::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_UPGRADER_INIT_CLASS(SoV2WWWAnchor);
    SO_REGISTER_UPGRADER(WWWAnchor, 2.0);
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV2WWWInline class.
//
// Use: internal

void
SoV2WWWInline::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_UPGRADER_INIT_CLASS(SoV2WWWInline);
    SO_REGISTER_UPGRADER(WWWInline, 2.0);
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV2AsciiText class.
//
// Use: internal

void
SoV2AsciiText::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_UPGRADER_INIT_CLASS(SoV2AsciiText);
    SO_REGISTER_UPGRADER(AsciiText, 2.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV2FontStyle class.
//
// Use: internal

void
SoV2FontStyle::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_UPGRADER_INIT_CLASS(SoV2FontStyle);
    SO_REGISTER_UPGRADER(FontStyle, 2.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV2MaterialIndex class.
//
// Use: internal

void
SoV2MaterialIndex::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_UPGRADER_INIT_CLASS(SoV2MaterialIndex);
    SO_REGISTER_UPGRADER(MaterialIndex, 2.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV2VertexProperty class.
//
// Use: internal

void
SoV2VertexProperty::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_UPGRADER_INIT_CLASS(SoV2VertexProperty);
    SO_REGISTER_UPGRADER(VertexProperty, 2.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV2LOD class.
//
// Use: internal

void
SoV2LOD::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_UPGRADER_INIT_CLASS(SoV2LOD);
    SO_REGISTER_UPGRADER(LOD, 2.0);
}
