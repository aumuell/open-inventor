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

#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoIndexedShape.h>

#include "IfAssert.h"
#include "IfShape.h"
#include "IfMerger.h"

/////////////////////////////////////////////////////////////////////////////
//
// Constructor.
//
/////////////////////////////////////////////////////////////////////////////

IfMerger::IfMerger()
{
}

/////////////////////////////////////////////////////////////////////////////
//
// Destructor.
//
/////////////////////////////////////////////////////////////////////////////

IfMerger::~IfMerger()
{
}

/////////////////////////////////////////////////////////////////////////////
//
// This merges adjacent IfShapes in the given IfShapeList, if possible.
//
/////////////////////////////////////////////////////////////////////////////

void
IfMerger::merge(IfShapeList &shapeList)
{
    //////////////////////////////////////////////////////////////////
    //
    // We can merge materials of 2 adjacent shapes if:
    //
    //  - The IfShapes differ in material properties, only
    //
    //  - The IfShapes' shape nodes are all indexed shapes
    //
    //  - The material binding is PER_VERTEX_INDEXED for both shapes
    //

    // This is the IfShape to merge into
    IfShape *shape1 = shapeList[0];

    for (int i = 1; i < shapeList.getLength(); i++) {
	IfShape *shape2 = shapeList[i];

	if (canMergeShapes(shape1, shape2) && mergeShapes(shape1, shape2)) {
	    shape2->differenceLevel = 0;
	    shape2->differenceCode  = IfShape::NONE;
	}
	else
	    shape1 = shape2;
    }
}

/////////////////////////////////////////////////////////////////////////////
//
// Returns TRUE if materials can probably be merged from shape2 to
// shape1.
//
/////////////////////////////////////////////////////////////////////////////

SbBool
IfMerger::canMergeShapes(IfShape *shape1, IfShape *shape2)
{
    // Can't be any difference other than material
    if (shape2->differenceCode != 0 &&
	shape2->differenceCode != IfShape::MATERIAL)
	return FALSE;

    ASSERT(shape1->shape != NULL);
    ASSERT(shape2->shape != NULL);

    // Have to have indexed shapes
    if (! shape1->shape->isOfType(SoIndexedShape::getClassTypeId()) ||
	! shape2->shape->isOfType(SoIndexedShape::getClassTypeId()))
	return FALSE;

    // Have to have PER_VERTEX_INDEXED bindings
    if (shape1->materialBinding == NULL || shape2->materialBinding == NULL ||
	shape1->materialBinding->value.getValue() !=
	SoMaterialBinding::PER_VERTEX_INDEXED ||
	shape2->materialBinding->value.getValue() != 
	SoMaterialBinding::PER_VERTEX_INDEXED)
	return FALSE;

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Merges shape2 into shape1. Returns TRUE if successful.
//
/////////////////////////////////////////////////////////////////////////////

SbBool
IfMerger::mergeShapes(IfShape *shape1, IfShape *shape2)
{
    //////////////////////////////////////////////////////////////////
    //
    // Step 1: merge shape2's material into shape1's by concatenating
    // the field values at the end.
    //

    SoMaterial *m1 = shape1->material;
    SoMaterial *m2 = shape2->material;

    //
    // We try to do this only in these common cases:
    //
    // 1 - All material fields change per vertex
    // 2 - Only diffuseColor changes per vertex
    // 3 - Only diffuseColor and transparency change per vertex
    //
    // In cases 2 and 3, we can merge the materials only if all other
    // material fields have 1 value and it is the same for both
    // materials.
    //

    // First, determine which case we have for each material
    int case1 = getMaterialCase(m1);
    int case2 = getMaterialCase(m2);

    // If either is 0 or they're not the same, we can't merge them
    if (case1 == 0 || case1 != case2)
	return FALSE;

    // If we have case 2 or 3, make sure the material values that are
    // not changing are the same in the two nodes
    if (case1 == 2 || case1 == 3) {
	if (m1->ambientColor[0]  != m2->ambientColor[0]  ||
	    m1->specularColor[0] != m2->specularColor[0] ||
	    m1->emissiveColor[0] != m2->emissiveColor[0] ||
	    m1->shininess[0]     != m2->shininess[0])
	    return FALSE;
	if (case1 == 2 && m1->transparency[0] != m2->transparency[0])
	    return FALSE;
    }

    // Save the original number of values in the first shape's
    // material. This will be the offset into the second shape's
    // materials.
    int indexOffset = m1->diffuseColor.getNum();

    // Append material2's values to material 1

#define APPEND_VALS(FIELD)						      \
    m1->FIELD.setValues(m1->FIELD.getNum(), m2->FIELD.getNum(),		      \
			m2->FIELD.getValues(0))

    APPEND_VALS(diffuseColor);

    if (case1 != 2) {
	APPEND_VALS(transparency);

	if (case1 == 1) {
	    APPEND_VALS(ambientColor);
	    APPEND_VALS(specularColor);
	    APPEND_VALS(emissiveColor);
	    APPEND_VALS(shininess);
	}
    }

#undef APPEND_VALS

    //////////////////////////////////////////////////////////////////
    //
    // Modify IfShape2's shape's material indices
    //

    SoIndexedShape *iShape = (SoIndexedShape *) shape2->shape;

    // If the material indices are the same as the coordinate
    // indices, we will have to copy them first
    if (iShape->materialIndex.getNum() == 1 &&
	iShape->materialIndex[0] == -1) {

	int num = iShape->coordIndex.getNum();
	iShape->materialIndex.setNum(iShape->coordIndex.getNum());
	const int32_t *c = iShape->coordIndex.getValues(0);
	int32_t *m = iShape->materialIndex.startEditing();
	for (int i = 0; i < num; i++) {
	    if (c[i] < 0)
		m[i] = c[i];
	    else
		m[i] = c[i] + indexOffset;
	}
	iShape->materialIndex.finishEditing();
    }

    else {
	// Adjust the existing material indices by the offset
	int num = iShape->materialIndex.getNum();
	int32_t *m = iShape->materialIndex.startEditing();
	for (int i = 0; i < num; i++)
	    if (m[i] >= 0)
		m[i] += indexOffset;
	iShape->materialIndex.finishEditing();
    }

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Determines which material case (0 - 3) is true for a material.
//
/////////////////////////////////////////////////////////////////////////////

int
IfMerger::getMaterialCase(SoMaterial *material)
{
    int nam = material->ambientColor.getNum();
    int ndi = material->diffuseColor.getNum();
    int nsp = material->specularColor.getNum();
    int nem = material->emissiveColor.getNum();
    int nsh = material->shininess.getNum();
    int ntr = material->transparency.getNum();

    // Case 1: all material fields change per vertex
    if (nam == ndi && nam == nsp && nam == nem && nam == nsh && nam == ntr)
	return 1;

    // Cases 2 and 3: only diffuseColor and transparency can change
    // per vertex
    if (nam == 1 && nsp == 1 && nem == 1 && nsh == 1) {

	// Case 2: only diffuseColor changes per vertex
	if (ntr == 1)
	    return 2;

	// Case 3: diffuseColor and transparency change per vertex
	else
	    return 3;
    }

    // Any other case is case 0 - unmergeable
    return 0;
}
