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
 |   Classes:
 |	SoVertexPropertyV2
 |
 |   Authors: David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoPackedColor.h>
#include <Inventor/nodes/SoTextureCoordinate2.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/actions/SoWriteAction.h>
#include "SoVertexPropertyV2.h"
#include "SoPackedColorV2.h"

SO_NODE_SOURCE(SoVertexPropertyV2);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    init that class!
//
void
SoVertexPropertyV2::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoVertexPropertyV2, "VertexProperty", SoVertexProperty);

    // Tell the type system whenever it wants to create something
    // of type SoVertexProperty to create a SoVertexPropertyV2
    SoType::overrideType(
		 SoVertexProperty::getClassTypeId(), &SoVertexPropertyV2::createInstance);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
SoVertexPropertyV2::SoVertexPropertyV2()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoVertexPropertyV2);
    SO_NODE_ADD_FIELD(alternateRep, (NULL));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
SoVertexPropertyV2::~SoVertexPropertyV2()
//
////////////////////////////////////////////////////////////////////////
{
    
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    write the alternate rep for this node
//
void
SoVertexPropertyV2::write(SoWriteAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoOutput	*out = action->getOutput();

    // In write-reference counting phase
    if (out->getStage() == SoOutput::COUNT_REFS) {
	// Only do this once!
	if (alternateRep.isDefault()) {
	    // Set up the alternate representation
	    SoGroup *group = convert(this);
	    alternateRep = group;
    
	    // Data from the fields has been copied under the alternateRep.
	    // No need to write it out again, so set fields to default.
	    vertex.setDefault(TRUE);
	    normal.setDefault(TRUE);
	    orderedRGBA.setDefault(TRUE);
	    texCoord.setDefault(TRUE);
	    normalBinding.setDefault(TRUE);
	    materialBinding.setDefault(TRUE);
	}	
    }
    
    SoVertexProperty::write(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    write the alternate rep for this node
//
SoGroup *
SoVertexPropertyV2::convert(SoVertexProperty *vp)
//
////////////////////////////////////////////////////////////////////////
{
    SoGroup *group = new SoGroup;
    
    // For any non default field, create a node to contain its info
    // and put that node in the group (alternateRep). We can set the
    // field to default since its info will be written in the alternateRep
    // (no need to write it twice).
    if ( vp->vertex.getNum()>0 ) {
	SoCoordinate3 *coord = new SoCoordinate3;
	coord->point.copyFrom(vp->vertex);
	group->addChild(coord);
    }
    if (vp->normal.getNum()>0 ) {
	SoNormal *norm = new SoNormal;
	norm->vector.copyFrom(vp->normal);
	group->addChild(norm);
    }
    if ( vp->orderedRGBA.getNum()>0 ) {
	SoPackedColor *packedColor = new SoPackedColorV2;
	packedColor->orderedRGBA.copyFrom(vp->orderedRGBA);
	group->addChild(packedColor);
    }
    if ( vp->texCoord.getNum()>0 ) {
	SoTextureCoordinate2 *tex = new SoTextureCoordinate2;
	tex->point.copyFrom(vp->texCoord);
	group->addChild(tex);
    }
    if (! vp->normalBinding.isDefault()) {
	SoNormalBinding *normBind = new SoNormalBinding;
	normBind->value.copyFrom(vp->normalBinding);
	group->addChild(normBind);
    }
    if (! vp->materialBinding.isDefault()) {
	SoMaterialBinding *mtlBind = new SoMaterialBinding;
	mtlBind->value.copyFrom(vp->materialBinding);
	group->addChild(mtlBind);
    }

    return group;
}

