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
 |	SoIndexedTriangleMeshV1
 |
 |   Author(s)		: Gavin Bell, David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/nodes/SoIndexedTriangleStripSet.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include "SoIndexedTriangleMeshV1.h"
#include "SoVertexPropertyV2.h"

SO_NODE_SOURCE(SoIndexedTriangleMeshV1);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Default constructor
//
// Use: public

SoIndexedTriangleMeshV1::SoIndexedTriangleMeshV1()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoIndexedTriangleMeshV1);

    SO_NODE_ADD_FIELD(coordIndex, (-1));
    SO_NODE_ADD_FIELD(materialIndex, (-1));
    SO_NODE_ADD_FIELD(normalIndex, (-1));
    SO_NODE_ADD_FIELD(textureCoordIndex, (-1));
    
    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoIndexedTriangleMeshV1 class.
//
// Use: internal

void
SoIndexedTriangleMeshV1::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoIndexedTriangleMeshV1, "IndexedTriangleMesh", SoNode);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Convert the 2.0 tri strip set to a 1.0 tmesh.
//
SoNode *
SoIndexedTriangleMeshV1::downgrade(SoIndexedTriangleStripSet *tstrip)
//
////////////////////////////////////////////////////////////////////////
{
    SoIndexedTriangleMeshV1 *tmesh = new SoIndexedTriangleMeshV1;
    
    // just copy all the fields (only the name of this class needs
    // to change. But there is no mechanism to change a class name,
    // so we copy all the data to this class which has the name we want.)
    int num;
    
#define COPY_MFINT32(toNode, fromNode, field)				    \
    if (! fromNode->field.isDefault()) {			    \
	if ((num = fromNode->field.getNum()) > 0)		    \
	    toNode->field.setValues(0, num, fromNode->field.getValues(0));  \
    }								    \
    if (fromNode->field.isIgnored())				    \
	toNode->field.setIgnored(TRUE)					    \
	
	
    COPY_MFINT32(tmesh, tstrip, coordIndex);
    COPY_MFINT32(tmesh, tstrip, materialIndex);
    COPY_MFINT32(tmesh, tstrip, normalIndex);
    COPY_MFINT32(tmesh, tstrip, textureCoordIndex);
    
    // If the vertexProperty field is set, then we need to do more work
    SoNode *node;
    if (! tstrip->vertexProperty.isDefault()) {
	SoSeparator *sep = new SoSeparator;
	SoVertexProperty *vp = (SoVertexProperty *) tstrip->vertexProperty.getValue();
	
	SoGroup *vpgroup = SoVertexPropertyV2::convert(vp);
	sep->addChild(vpgroup);
	sep->addChild(tmesh);
	
	node = sep;
    }
    else node = tmesh;
    
    return node;
}
