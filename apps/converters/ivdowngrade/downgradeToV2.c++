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
 |   Author(s) : David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/nodes/SoVertexShape.h>
#include <Inventor/nodes/SoSeparator.h>

////////////////////////////////////////////////////////////////////////
//
// Before V2.1, there was no vertexProperty field on vertex shapes.
// If this field is set, replace the vertex shape with a separator,
// vertex propery node, and vertex shape node without this field.
//
SoNode *
downgradeVertexShape(SoVertexShape *vs)
//
////////////////////////////////////////////////////////////////////////
{
    SoNode *node;
    
    if (! vs->vertexProperty.isDefault()) {
	SoSeparator *sep = new SoSeparator;
	SoVertexProperty *vp = (SoVertexProperty *) vs->vertexProperty.getValue();
	sep->addChild(vp);
	
	vs->vertexProperty.setDefault(TRUE);
	sep->addChild(vs);
	
	node = sep;
    }
    else node = vs;
    
    return node;
}

////////////////////////////////////////////////////////////////////////
//
// Down-grade the passed scene to 2.0 format.
//
// NOTE: we could have written subclasses for every vertex shape node
// where each subclass would have an alternateRep field (like we did
// for the other V2 nodes, e.g. SoWWWInlineV2). But this would have
// taken more time than it was worth. Maybe later... ??? - mott
//
SoNode *
downgradeToV2(SoNode *n)
//
////////////////////////////////////////////////////////////////////////
{
    if (n == NULL)
	return NULL;

    SoNode *node = n;
    
    if (node->isOfType(SoVertexShape::getClassTypeId())) {
	node = downgradeVertexShape((SoVertexShape *) node);
    }
    else if (node->isOfType(SoGroup::getClassTypeId())) {
	// Traverse the children
	SoGroup *group = (SoGroup *) node;
	for (int i = 0; i < group->getNumChildren(); i++) {
	    SoNode *newchild = downgradeToV2(group->getChild(i));
	    if (newchild != group->getChild(i))
		group->replaceChild(i, newchild);
	}
    }
    
    return node;
}
