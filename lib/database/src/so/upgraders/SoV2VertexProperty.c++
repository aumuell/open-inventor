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
 |	SoV2VertexProperty
 |
 |   Author(s)		: David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include "SoV2VertexProperty.h"
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoPackedColor.h>
#include <Inventor/nodes/SoTextureCoordinate2.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/errors/SoReadError.h>

SO_NODE_SOURCE(SoV2VertexProperty);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Default constructor
//
// Use: public

SoV2VertexProperty::SoV2VertexProperty()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoV2VertexProperty);

    // Initialize with dummy values using standard macro, then delete
    // all values:
    SO_NODE_ADD_FIELD(vertex, (SbVec3f(0,0,0)));
    vertex.deleteValues(0);
    SO_NODE_ADD_FIELD(normal, (SbVec3f(0,0,0)));
    normal.deleteValues(0);
    SO_NODE_ADD_FIELD(texCoord, (SbVec2f(0,0)));
    texCoord.deleteValues(0);
    SO_NODE_ADD_FIELD(orderedRGBA, (0));
    orderedRGBA.deleteValues(0);

    // Initialize these with default values.  They'll be ignored if
    // the corresponding fields have no values:
    SO_NODE_ADD_FIELD(materialBinding, (SoVertexProperty::OVERALL));
    SO_NODE_ADD_FIELD(normalBinding, (SoVertexProperty::PER_VERTEX_INDEXED));

    SO_NODE_ADD_FIELD(alternateRep, (NULL));

    // Set up static info for enumerated type field
    SO_NODE_DEFINE_ENUM_VALUE(Binding, OVERALL);
    SO_NODE_DEFINE_ENUM_VALUE(Binding, PER_VERTEX);
    SO_NODE_DEFINE_ENUM_VALUE(Binding, PER_VERTEX_INDEXED);
    SO_NODE_DEFINE_ENUM_VALUE(Binding, PER_FACE);
    SO_NODE_DEFINE_ENUM_VALUE(Binding, PER_FACE_INDEXED);
    SO_NODE_DEFINE_ENUM_VALUE(Binding, PER_PART);
    SO_NODE_DEFINE_ENUM_VALUE(Binding, PER_PART_INDEXED);

    // Set up info in enumerated type field
    SO_NODE_SET_SF_ENUM_TYPE(materialBinding, Binding);
    SO_NODE_SET_SF_ENUM_TYPE(normalBinding, Binding);

}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoV2VertexProperty::~SoV2VertexProperty()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Create a version 2.1 VertexProperty.
//
// Use: private

SoNode *
SoV2VertexProperty::createNewNode()
//
////////////////////////////////////////////////////////////////////////
{
    SoVertexProperty *result = SO_UPGRADER_CREATE_NEW(SoVertexProperty);

    //
    // Inventor 2.0 did not have VertexProperty. When we convert 2.1 to 2.0 files,
    // the VertexProperty puts all its field info into an alternateRep, then
    // sets its fields to default values.
    // We simply run through the nodes in the alternateRep and recreate the fields.
    //

    if (! alternateRep.isDefault()) {
	SoGroup *group = (SoGroup *) alternateRep.getValue();
	if (group->isOfType(SoGroup::getClassTypeId())) {
	    for (int i = 0; i < group->getNumChildren(); i++) {
		SoNode *node = group->getChild(i);
		
		// Set the fields in our node
		if (node->isOfType(SoCoordinate3::getClassTypeId()))
		    result->vertex.copyFrom(((SoCoordinate3 *)node)->point);
		    
		else if (node->isOfType(SoNormal::getClassTypeId()))
		    result->normal.copyFrom(((SoNormal *)node)->vector);
		    
		else if (node->isOfType(SoPackedColor::getClassTypeId()))
		    result->orderedRGBA.copyFrom(((SoPackedColor *)node)->orderedRGBA);
		    
		else if (node->isOfType(SoTextureCoordinate2::getClassTypeId()))
		    result->texCoord.copyFrom(((SoTextureCoordinate2 *)node)->point);
		    
		else if (node->isOfType(SoNormalBinding::getClassTypeId()))
		    result->normalBinding.copyFrom(((SoNormalBinding *)node)->value);
		    
		else if (node->isOfType(SoMaterialBinding::getClassTypeId()))
		    result->materialBinding.copyFrom(((SoMaterialBinding *)node)->value);
	    }
	}
    }

    return result;
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//   special upgrade method to read field description
//
// Use: public, internal, virtual
SbBool 
SoV2VertexProperty::upgrade(SoInput *in, const SbName &refName, SoBase *&result)
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
    SbBool ret = SoUpgrader::upgrade(in, refName, result);
    
    if (in->isBinary()){
	// Read a zero, which represents the number of children here
	int dummy;
	in->read(dummy);
    }
    
    return ret;
}
