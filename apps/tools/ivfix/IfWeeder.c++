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

#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoIndexedTriangleStripSet.h>
#include <Inventor/nodes/SoVertexProperty.h>

#include "IfAssert.h"
#include "IfShape.h"
#include "IfTypes.h"
#include "IfWeeder.h"

/////////////////////////////////////////////////////////////////////////////
//
// Constructor.
//
/////////////////////////////////////////////////////////////////////////////

IfWeeder::IfWeeder()
{
}

/////////////////////////////////////////////////////////////////////////////
//
// Destructor.
//
/////////////////////////////////////////////////////////////////////////////

IfWeeder::~IfWeeder()
{
}

/////////////////////////////////////////////////////////////////////////////
//
// This weeds out unnecessary items from the given fixed scene graph.
//
/////////////////////////////////////////////////////////////////////////////

void
IfWeeder::weed(SoNode *root)
{
    // Weed out duplicate and unused materials from all SoMaterial
    // nodes in the graph
    weedMaterials(root);
}

/////////////////////////////////////////////////////////////////////////////
//
// This type is used by weedMaterials() to determine material-shape
// dependencies.
//
/////////////////////////////////////////////////////////////////////////////

struct IfWeederMaterialEntry {
    SoMaterial		*material;	// Pointer to material
    SoNodeList		shapes;		// IfShapes that use material
    SbBool		canWeed;	// TRUE if material can be weeded
};

/////////////////////////////////////////////////////////////////////////////
//
// This weeds out values in SoMaterial nodes that are not used by any
// shape. It also removes duplicate material values.
//
/////////////////////////////////////////////////////////////////////////////

void
IfWeeder::weedMaterials(SoNode *root)
{
    // Find all SoMaterial nodes in the scene graph and determine
    // which shape nodes are affected by each one. Note that because
    // materials may be multiply instanced, this is a little more
    // complicated than just looking at each subgraph with a
    // material. The resulting materials are stored in the
    // materialList, which is a list of pointers to
    // IfWeederMaterialEntry instances.
    findMaterialsAndShapes(root);

    // Weed each material in the resulting list
    int i;
    for (i = 0; i < materialList->getLength(); i++) {
	IfWeederMaterialEntry *entry = (IfWeederMaterialEntry *)
	    (*materialList)[i];

	if (entry->canWeed)
	    weedMaterial(root, entry);
    }

    // Clean up
    for (i = 0; i < materialList->getLength(); i++)
	delete (IfWeederMaterialEntry *) (*materialList)[i];
    delete materialList;
}

/////////////////////////////////////////////////////////////////////////////
//
// Finds all materials in the given graph and the shapes that depend
// on them.
//
/////////////////////////////////////////////////////////////////////////////

void
IfWeeder::findMaterialsAndShapes(SoNode *root)
{
    // Since we know the structure of the given scene graph (which is
    // after fixing has occurred), we can be efficient here. Just
    // search for all materials in the scene. For each material, the
    // shapes affected by it must be under the separator that is the
    // material's parent node. So just search for all shapes under
    // that separator, making sure that the path to the shape comes
    // after the material.

    // First, create a dictionary so we can tell when we've found a
    // multiple instance of a material
    SbDict materialDict;

    // Search for all materials in the scene
    SoSearchAction sa;
    sa.setType(SoMaterial::getClassTypeId());
    sa.setInterest(SoSearchAction::ALL);
    sa.apply(root);

    // Set up another search action to find all shapes using a
    // material. Note that we have to search for all node types that
    // should be considered shapes.
    SoSearchAction sa2;
    sa2.setInterest(SoSearchAction::ALL);

    // These are the shape types
    SoTypeList shapeTypes;
    IfTypes::getShapeTypes(&shapeTypes);

    // Process each material, adding new ones to the list
    materialList = new SbPList;
    for (int i = 0; i < sa.getPaths().getLength(); i++) {

	const SoPath *path = (const SoPath *) sa.getPaths()[i];

	ASSERT(path->getLength() > 1);
	ASSERT(path->getTail()->getTypeId() == SoMaterial::getClassTypeId());

	SoMaterial  *material = (SoMaterial *) path->getTail();

	// Add to the dictionary if necessary, or use the existing
	// entry
	void *entryPtr;
	IfWeederMaterialEntry *entry;
	if (materialDict.find((unsigned long) material, entryPtr)) {
	    entry = (IfWeederMaterialEntry *) entryPtr;
	    if (! entry->canWeed)
		continue;
	}
	else {
	    entry = new IfWeederMaterialEntry;
	    entry->material = material;
	    entry->canWeed  = TRUE;
	    materialDict.enter((unsigned long) material, entry);
	    materialList->append(entry);
	}

	// If any node above the material in the path is an opaque
	// group, we can't really weed this material
	int j;
	for (j = path->getLength() - 2; j >= 0; j--) {
	    if (IfTypes::isOpaqueGroupType(path->getNode(j)->getTypeId())) {
		entry->canWeed = FALSE;
		break;
	    }
	}
	if (! entry->canWeed)
	    continue;

	ASSERT(path->getNodeFromTail(1)->
	       isOfType(SoSeparator::getClassTypeId()));

	SoSeparator *parent = (SoSeparator *) path->getNodeFromTail(1);
	int materialIndex = path->getIndexFromTail(0);

	// Find all shapes using the material, adding them to the list
	// of shapes in the material's entry. Store all the paths to
	// them in this list
	SoPathList pathsToShapes;
	for (int type = 0; type < shapeTypes.getLength(); type++) {
	    sa2.setType(shapeTypes[type]);
	    sa2.apply(parent);
	    for (j = 0; j < sa2.getPaths().getLength(); j++)
		pathsToShapes.append(sa2.getPaths()[j]);
	}

	for (j = 0; j < pathsToShapes.getLength(); j++) {

	    const SoPath *shapePath = (const SoPath *) pathsToShapes[j];

	    // We can't weed the material at all if a shape other than
	    // the one we created is found
	    SoType tailType = shapePath->getTail()->getTypeId();
	    if (tailType != SoIndexedTriangleStripSet::getClassTypeId() &&
		tailType != SoIndexedFaceSet::getClassTypeId()) {
		entry->canWeed = FALSE;
		break;
	    }

	    // Make sure the shape comes after the material and does
	    // not get its materials from a vertex property
	    // node.
	    else if (shapePath->getIndex(1) > materialIndex) {
		SoIndexedShape *is = (SoIndexedShape *) shapePath->getTail();

		// ??? If the shape's materialIndex field has the
		// ??? default value, we assume that it might have to
		// ??? access all the material values. To check, we would
		// ??? have to look at the coordIndex values if the
		// ??? material binding is not OVERALL. This change could
		// ??? not be done in time for the release. See bug 311071.
		if (is->materialIndex.getNum() == 1 &&
		    is->materialIndex[0] < 0) {
		    entry->canWeed = FALSE;
		    break;
		}

		SoVertexProperty *vp =
		    (SoVertexProperty *) is->vertexProperty.getValue();
		if (vp == NULL || vp->orderedRGBA.getNum() == 0)
		    entry->shapes.append(shapePath->getTail());
	    }
	}
    }
}

/////////////////////////////////////////////////////////////////////////////
//
// Weeds the material in the given entry.
//
/////////////////////////////////////////////////////////////////////////////

void
IfWeeder::weedMaterial(SoNode *root, IfWeederMaterialEntry *entry)
{
    // If the material affects no shapes at all, get rid of it. This
    // can happen when vertex property nodes are used.
    if (entry->shapes.getLength() == 0) {
	SoSearchAction sa;
	sa.setNode(entry->material);
	sa.setInterest(SoSearchAction::ALL);
	sa.apply(root);
	for (int i = 0; i < sa.getPaths().getLength(); i++) {
	    SoPath *path = sa.getPaths()[i];
	    SoSeparator *parent = (SoSeparator *) path->getNodeFromTail(1);
	    int index = path->getIndexFromTail(0);
	    ASSERT(parent->isOfType(SoSeparator::getClassTypeId()));
	    ASSERT(parent->getChild(index) == entry->material);
	    parent->removeChild(index);
	}
    }

    // Remove all material values from the material node that are
    // not used by any dependent shapes. Adjust the indices in the
    // dependent shapes accordingly.
    removeDuplicateMaterials(entry);

    // Now remove all material values that are not used by any
    // dependent shapes. Again, adjust the indices in the dependent
    // shapes.
    removeUnusedMaterials(entry);
}

/////////////////////////////////////////////////////////////////////////////
//
// Finds and removes duplicate materials, adjusting material
// indices in all dependent shapes.
//
/////////////////////////////////////////////////////////////////////////////

void
IfWeeder::removeDuplicateMaterials(IfWeederMaterialEntry *entry)
{
    ////////////////////////////////////////////
    //
    // To find duplicate materials, we create a hash table of material
    // values. Each entry in the hash table contains an index into the
    // material node's values. The entries in the table use the
    // material values as the hash key, so that duplicate materials
    // will be in the same slot in the table. If we find a duplicate,
    // we store that fact in a table of index duplicates. For example,
    // if material index 32 is the same as material index 21, we store
    // 21 in entry 32 in the duplicates array. When we are done with
    // the hash table, the duplicates array will tell us which
    // material values we need to keep in the material node and also
    // how to adjust the material indices in the SoIndexedShapes.
    //

    // Instances of this structure is stored in the hash table.
    struct IfWeederMaterialInfo {
	int			index;	// Index of material value
	IfWeederMaterialInfo	*next;	// For making lists in the hash table
    };

    // Access the fields of the material node for faster referencing
    curMaterial = entry->material;
    ambi = curMaterial->ambientColor.getValues(0);
    diff = curMaterial->diffuseColor.getValues(0);
    spec = curMaterial->specularColor.getValues(0);
    emis = curMaterial->emissiveColor.getValues(0);
    shin = curMaterial->shininess.getValues(0);
    tran = curMaterial->transparency.getValues(0);

    int numMaterials = curMaterial->diffuseColor.getNum();

    // Create an array of structures to insert into the table
    IfWeederMaterialInfo *infos = new IfWeederMaterialInfo[numMaterials];

    // Create an array to hold the duplicate info
    int *duplicates = new int [numMaterials];

    // Create a hash table of a reasonable size
    SbDict *hashTable = new SbDict(1 + numMaterials / 4);

    // Add each material index to the hash table, looking for
    // duplicates
    int i, numMatches = 0;

    for (i = 0; i < numMaterials; i++) {

	// Assume that there is no duplicate. We'll overwrite this if
	// we find one below
	duplicates[i] = -1;

	// Prepare an info instance to insert
	infos[i].index = i;
	infos[i].next  = NULL;

	// Compute a hash value for the material based on the material
	// values
	uint32_t hashKey = computeMaterialHashKey(i);

	// See if there is anything in the hash table with that key
	void *entry;
	if (hashTable->find(hashKey, entry)) {
	    // Look for an exact match
	    IfWeederMaterialInfo *info, *prevInfo = NULL;
	    for (info = (IfWeederMaterialInfo *) entry;
		 info != NULL; info = info->next) {

		// If there's a match, record the duplicate
		if (isSameMaterial(info->index, i)) {
		    duplicates[i] = info->index;
		    numMatches++;
		    break;
		}

		prevInfo = info;
	    }

	    // If we didn't find a duplicate, add a new entry to the
	    // end of the list
	    if (duplicates[i] == -1) {
		ASSERT(prevInfo != NULL);
		prevInfo->next = &infos[i];
	    }
	}

	// If there was nothing in the table for this key, add a new entry
	else
	    hashTable->enter(hashKey, &infos[i]);
    }

    // We're done with the hash table
    delete hashTable;
    delete [] infos;

    // Now we can adjust the material indices in the shapes affected
    // by the material. Don't bother doing this unless we weeded at
    // least one material.
    if (numMatches > 0) {

	for (i = 0; i < entry->shapes.getLength(); i++) { 

	    // Adjust material indices to use only original materials,
	    // not duplicates
	    SoIndexedShape *iShape = (SoIndexedShape *) entry->shapes[i];
	    int num = iShape->materialIndex.getNum();
	    int32_t *m = iShape->materialIndex.startEditing();
	    for (int i = 0; i < num; i++) {

		// If the material index is off the end, cycle it
		if (m[i] >= numMaterials)
		    m[i] %= numMaterials;

		// Now check for duplicates
		if (m[i] >= 0 && duplicates[m[i]] != -1)
		    m[i] = duplicates[m[i]];
	    }
	    iShape->materialIndex.finishEditing();
	}
    }

    delete [] duplicates;
}

/////////////////////////////////////////////////////////////////////////////
//
// Computes a hash key for the material value with the given index.
//
/////////////////////////////////////////////////////////////////////////////

uint32_t
IfWeeder::computeMaterialHashKey(int index)
{
    // Just use the diffuse color to compute a key, since it's most
    // likely to vary between materials

    // Since we care only about exact matches, we can use any hash
    // function we like here. Just pick one that distributes the
    // materials in the range 0-10000. We can do this because we know
    // the color values range from 0 to 1.

    return (uint32_t) (diff[index][0] * 8871 +
		       diff[index][1] * 1104 +
		       diff[index][2] *   25);
}

/////////////////////////////////////////////////////////////////////////////
//
// Returns TRUE if the two material values with the given indices
// are the same.
//
/////////////////////////////////////////////////////////////////////////////

SbBool
IfWeeder::isSameMaterial(int i1, int i2)
{
    // Trivial case of same indices
    if (i1 == i2)
	return TRUE;

    // Diffuse colors always have to be the same
    if (diff[i1] != diff[i2])
	return FALSE;

#define TEST_FIELD(FIELD,VAR)						      \
    if (curMaterial->FIELD.getNum() > 1 && VAR[i1] != VAR[i2])		      \
	return FALSE

    // The rest depends on what varies
    TEST_FIELD(ambientColor,  ambi);
    TEST_FIELD(specularColor, spec);
    TEST_FIELD(emissiveColor, emis);
    TEST_FIELD(shininess,     shin);
    TEST_FIELD(transparency,  tran);

#undef TEST_FIELD

    // If we got here, they're the same
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Removes any unused material values and updates the indices in all
// dependent shapes.
//
/////////////////////////////////////////////////////////////////////////////

void
IfWeeder::removeUnusedMaterials(IfWeederMaterialEntry *entry)
{
    SoMaterial *material = entry->material;
    int numMaterials = material->diffuseColor.getNum();
    if (numMaterials == 0)
	return;

    ////////////////////////////////////////////
    //
    // Scan the lists of material indices in each shape to see which
    // (and how many) materials are used. Store the results in an
    // array of flags and a counter.

    int i, numUsed = 0;
    SbBool *materialUsed = new SbBool[numMaterials];
    for (i = 0; i < numMaterials; i++)
	materialUsed[i] = FALSE;
    for (i = 0; i < entry->shapes.getLength(); i++) { 
	SoIndexedShape *iShape = (SoIndexedShape *) entry->shapes[i];
	int numIndices = iShape->materialIndex.getNum();
	const int32_t *oldIndices = iShape->materialIndex.getValues(0);
	if (numIndices == 1) {

	    // If there is only one material index, that means the
	    // shape uses overall material binding, so we can set the
	    // materialIndex field to the default value and skip the
	    // shape.
	    if (! materialUsed[0]) {
		materialUsed[0] = TRUE;
		numUsed++;
	    }
	    iShape->materialIndex = -1;
	    iShape->materialIndex.setDefault(TRUE);
	    continue;
	}
	for (int j = 0; j < numIndices; j++) {
	    int mi = oldIndices[j];
	    // The material index should have already been brought
	    // into the correct range
	    ASSERT(mi < numMaterials);
	    if (mi >= 0 && ! materialUsed[mi]) {
		materialUsed[mi] = TRUE;
		numUsed++;
	    }
	}
    }

    // If all materials are used, stop
    ASSERT(numUsed <= numMaterials);
    if (numUsed == numMaterials) {
	delete [] materialUsed;
	return;
    }

    ////////////////////////////////////////////
    //
    // Set up an array of correspondences from old material indices
    // to new ones. Store -1 for those materials that are not used.

    int *materialIndex = new int[numMaterials];
    int curNewIndex = 0;
    for (i = 0; i < numMaterials; i++) {
	if (materialUsed[i])
	    materialIndex[i] = curNewIndex++;
	else
	    materialIndex[i] = -1;
    }
    ASSERT(curNewIndex == numUsed);

    ////////////////////////////////////////////
    //
    // Update the material indices in the shapes

    for (i = 0; i < entry->shapes.getLength(); i++) { 
	SoIndexedShape *iShape = (SoIndexedShape *) entry->shapes[i];
	int numIndices = iShape->materialIndex.getNum();
	if (numIndices == 1) {

	    // If there is only one material index and its value is 0
	    // or default, that means the shape uses overall material
	    // binding, so we can skip the shape.
	    if (iShape->materialIndex.isDefault() ||
		iShape->materialIndex[0] == 0)
		continue;

	    numIndices = iShape->coordIndex.getNum();
	    iShape->materialIndex.setValues(0, numIndices,
					    iShape->coordIndex.getValues(0));
	}
	int32_t *newIndices = iShape->materialIndex.startEditing();
	for (i = 0; i < numIndices; i++) {
	    int mi = newIndices[i];
	    if (mi >= 0) {
		// The material index should have already been brought
		// into the correct range
		ASSERT(mi < numMaterials);
		newIndices[i] = materialIndex[mi];
		ASSERT(newIndices[i] >= 0);
	    }
	}
	iShape->materialIndex.finishEditing();
    }

    ////////////////////////////////////////////
    //
    // Now we need to compress the material values themselves. Save
    // the existing fields, then copy the needed values into the real
    // fields. Do each field that has more than 1 value.
    //

    SoMFColor saveColors;
    SoMFFloat saveFloats;
    saveColors.setContainer(NULL);
    saveFloats.setContainer(NULL);

#define COMPRESS_COLOR(FIELD)						      \
    if (material->FIELD.getNum() > 1) {					      \
	saveColors = material->FIELD;					      \
	material->FIELD.setNum(numUsed);				      \
	const SbColor *oldColors = saveColors.getValues(0);		      \
	SbColor *newColors = material->FIELD.startEditing();		      \
	curNewIndex = 0;						      \
	for (i = 0; i < numMaterials; i++)				      \
	    if (materialUsed[i])					      \
		newColors[curNewIndex++] = oldColors[i];		      \
	material->FIELD.finishEditing();				      \
    }

#define COMPRESS_FLOAT(FIELD)						      \
    if (material->FIELD.getNum() > 1) {					      \
	saveFloats = material->FIELD;					      \
	material->FIELD.setNum(numUsed);				      \
	const float *oldFloats = saveFloats.getValues(0);		      \
	float *newFloats = material->FIELD.startEditing();		      \
	curNewIndex = 0;						      \
	for (i = 0; i < numMaterials; i++)				      \
	    if (materialUsed[i])					      \
		newFloats[curNewIndex++] = oldFloats[i];		      \
	material->FIELD.finishEditing();				      \
    }

    COMPRESS_COLOR(ambientColor);
    COMPRESS_COLOR(diffuseColor);
    COMPRESS_COLOR(specularColor);
    COMPRESS_COLOR(emissiveColor);
    COMPRESS_FLOAT(shininess);
    COMPRESS_FLOAT(transparency);

    // Clean up
    delete [] materialUsed;
    delete [] materialIndex;

#undef COMPRESS_COLOR
#undef COMPRESS_FLOAT
}
