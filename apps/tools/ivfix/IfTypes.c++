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

#include <Inventor/SoLists.h>
#include <Inventor/nodes/SoArray.h>
#include <Inventor/nodes/SoFile.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoLOD.h>
#include <Inventor/nodes/SoLevelOfDetail.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoMultipleCopy.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoShape.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoWWWAnchor.h>
#include <Inventor/nodes/SoWWWInline.h>

#include "IfTypes.h"

/////////////////////////////////////////////////////////////////////////////
//
// Fills in the given type list with all node types that should be
// considered "shapes" when collecting nodes.
//
/////////////////////////////////////////////////////////////////////////////

void
IfTypes::getShapeTypes(SoTypeList *types)
{
    types->append(SoShape::getClassTypeId());
    types->append(SoArray::getClassTypeId());
    types->append(SoMultipleCopy::getClassTypeId());
    types->append(SoFile::getClassTypeId());
    types->append(SoLevelOfDetail::getClassTypeId());
    types->append(SoLOD::getClassTypeId());
    types->append(SoWWWAnchor::getClassTypeId());
    types->append(SoWWWInline::getClassTypeId());
}

/////////////////////////////////////////////////////////////////////////////
//
// Returns TRUE if the given type is a group that should
// never be processed underneath.
//
/////////////////////////////////////////////////////////////////////////////

SbBool
IfTypes::isOpaqueGroupType(const SoType &type)
{
    return (type.isDerivedFrom(SoFile::getClassTypeId())		||
	    type.isDerivedFrom(SoLevelOfDetail::getClassTypeId())	||
	    type.isDerivedFrom(SoLOD::getClassTypeId())			||
	    type.isDerivedFrom(SoWWWAnchor::getClassTypeId()));
}

/////////////////////////////////////////////////////////////////////////////
//
// Returns TRUE if the given type is a shape that cannot be
// flattened.
//
/////////////////////////////////////////////////////////////////////////////

SbBool
IfTypes::isUnflattenableShape(const SoType &type)
{
    return (type.isDerivedFrom(SoLineSet::getClassTypeId())		||
	    type.isDerivedFrom(SoIndexedLineSet::getClassTypeId())	||
	    type.isDerivedFrom(SoPointSet::getClassTypeId())		||
	    type.isDerivedFrom(SoText2::getClassTypeId()));
}

/////////////////////////////////////////////////////////////////////////////
//
// Returns TRUE if the given type should be considered a shape.
//
/////////////////////////////////////////////////////////////////////////////

SbBool
IfTypes::isShape(const SoType &type)
{
    return (type.isDerivedFrom(SoShape::getClassTypeId())		||
	    type.isDerivedFrom(SoArray::getClassTypeId())		||
	    type.isDerivedFrom(SoMultipleCopy::getClassTypeId())	||
	    type.isDerivedFrom(SoWWWInline::getClassTypeId()));
}
