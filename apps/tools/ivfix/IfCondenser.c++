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

#include <Inventor/SbBox.h>
#include <Inventor/SbDict.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoIndexedShape.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoTextureCoordinate2.h>

#include "IfAssert.h"
#include "IfCondenser.h"
#include "IfHasher.h"
#include "IfHolder.h"

/////////////////////////////////////////////////////////////////////////////
//
// Constructor. Initializes data structures.
//
/////////////////////////////////////////////////////////////////////////////

IfCondenser::IfCondenser()
{
    holder = NULL;
}

/////////////////////////////////////////////////////////////////////////////
//
// Destructor.
//
/////////////////////////////////////////////////////////////////////////////

IfCondenser::~IfCondenser()
{
}

/////////////////////////////////////////////////////////////////////////////
//
// This takes a scene graph produced by flattening a graph and
// condenses vertices and material indices, if possible, by removing
// duplicates.
//
/////////////////////////////////////////////////////////////////////////////

void
IfCondenser::condense(IfHolder *_holder)
{
    holder = _holder;

    // Remove duplicate coordinates, normals, and texture coordinates
    condenseCoordinates();

    if (holder->doNormals)
	condenseNormals();

    if (holder->doTexCoords)
	condenseTextureCoordinates();

    // Condense the material indices if possible
    condenseMaterials();
}

/////////////////////////////////////////////////////////////////////////////
//
// Removes duplicate coordinates, updating the coordinate indices in
// the triangle strip set.
//
/////////////////////////////////////////////////////////////////////////////

void
IfCondenser::condenseCoordinates()
{
    int		  numCoords = holder->coords->point.getNum();
    const SbVec3f *coords   = holder->coords->point.getValues(0);

    // Determine the 3D extent of the coordinates so the hash
    // function can normalize coordinates into the range 0 to 1.
    SbBox3f box;
    int i;
    for (i = 0; i < numCoords; i++)
	box.extendBy(coords[i]);
    SbVec3f scale;
    box.getSize(scale[0],scale[1], scale[2]);
    for (i = 0; i < 3; i++)
	scale[i] = (scale[i] == 0.0 ? 1.0 : 1.0 / scale[i]);

    // Create a new field in which to store the uniquified coordinates
    SoMFVec3f uniqueCoords;
    uniqueCoords.setContainer(NULL);

    // Add all the coordinates, replacing the old indices with the new
    // ones
    int	 numIndices = holder->triSet->coordIndex.getNum();
    int32_t *indices   = holder->triSet->coordIndex.startEditing();

    // Create a IfHasher to store the coordinates
    IfHasher coordHasher(&uniqueCoords, numIndices, box.getMin(), scale);

    for (i = 0; i < numIndices; i++)
	if (indices[i] >= 0)
	    indices[i] = coordHasher.addVector(coords[indices[i]]);

    holder->triSet->coordIndex.finishEditing();
    coordHasher.finish();

    // Replace the coordinates with the unique ones
    holder->coords->point = uniqueCoords;
}

/////////////////////////////////////////////////////////////////////////////
//
// Removes duplicate normals, updating the normal indices in
// the triangle strip set.
//
/////////////////////////////////////////////////////////////////////////////

void
IfCondenser::condenseNormals()
{
    int		  numNormals = holder->normals->vector.getNum();
    const SbVec3f *normals   = holder->normals->vector.getValues(0);

    // Normals always lie between -1 and 1 in all dimensions
    SbVec3f min(-1.0, -1.0, -1.0);
    SbVec3f scale(0.5, 0.5, 0.5);

    // Create a new field in which to store the uniquified normals
    SoMFVec3f uniqueNormals;
    uniqueNormals.setContainer(NULL);

    // Add all the normals, replacing the old indices with the new
    // ones
    int	 numIndices = holder->triSet->normalIndex.getNum();
    int32_t *indices   = holder->triSet->normalIndex.startEditing();

    // Create a IfHasher to store the normals
    IfHasher normalHasher(&uniqueNormals, numIndices, min, scale);

    for (int i = 0; i < numIndices; i++)
	if (indices[i] >= 0)
	    indices[i] = normalHasher.addVector(normals[indices[i]]);

    holder->triSet->normalIndex.finishEditing();
    normalHasher.finish();

    // Replace the normals with the unique ones
    holder->normals->vector = uniqueNormals;

    // If the normal indices are now the same as the coordinate
    // indices, we can get rid of them
    if (sameIndices(&holder->triSet->coordIndex,
		    &holder->triSet->normalIndex))
	holder->triSet->normalIndex = -1;
}

/////////////////////////////////////////////////////////////////////////////
//
// Removes duplicate texture coordinates, updating the texture
// coordinate indices in the triangle strip set.
//
/////////////////////////////////////////////////////////////////////////////

void
IfCondenser::condenseTextureCoordinates()
{
    int		  numTexCoords = holder->texCoords->point.getNum();
    const SbVec2f *texCoords   = holder->texCoords->point.getValues(0);

    // Texture Coordinates always lie between 0 and 1 in both dimensions
    SbVec2f min(0.0, 0.0);
    SbVec2f scale(1.0, 1.0);

    // Create a new field in which to store the uniquified texture coordinates
    SoMFVec2f uniqueTexCoords;
    uniqueTexCoords.setContainer(NULL);

    // Add all the texture coordinates, replacing the old indices with
    // the new ones
    int	 numIndices = holder->triSet->textureCoordIndex.getNum();
    int32_t *indices   = holder->triSet->textureCoordIndex.startEditing();

    // Create a IfHasher to store the texture coordinates
    IfHasher texCoordHasher(&uniqueTexCoords, numIndices, min, scale);

    for (int i = 0; i < numIndices; i++)
	if (indices[i] >= 0)
	    indices[i] = texCoordHasher.addVector(texCoords[indices[i]]);

    holder->triSet->textureCoordIndex.finishEditing();
    texCoordHasher.finish();

    // Replace the texture coordinates with the unique ones
    holder->texCoords->point = uniqueTexCoords;

    // If the texture coordinate indices are now the same as the
    // coordinate indices, we can get rid of them
    if (sameIndices(&holder->triSet->coordIndex,
		    &holder->triSet->textureCoordIndex))
	holder->triSet->textureCoordIndex = -1;
}

/////////////////////////////////////////////////////////////////////////////
//
// Condenses the material indices if possible.
//
/////////////////////////////////////////////////////////////////////////////

void
IfCondenser::condenseMaterials()
{
    const int32_t *mtlIndices = holder->triSet->materialIndex.getValues(0);
    int numIndices = holder->triSet->materialIndex.getNum();

    // See if all material indices are 0
    SbBool allZero = TRUE;
    for (int i = 0; i < numIndices; i++) {
	if (mtlIndices[i] > 0) {
	    allZero = FALSE;
	    break;
	}
    }

    // If so, clean up the material indices
    if (allZero) {
	// Get rid of the material indices. Note that we store a 0
	// here to differentiate this case (overall material) from the
	// case below where we share material indices.
	holder->triSet->materialIndex = 0;

	// Set the binding to overall
    	holder->materialBinding->value = SoMaterialBinding::OVERALL;
    }

    // Otherwise, see if the material indices are the same as the
    // coordinate indices
    else if (sameIndices(&holder->triSet->coordIndex,
			 &holder->triSet->materialIndex))
	holder->triSet->materialIndex = -1;
}

/////////////////////////////////////////////////////////////////////////////
//
// Returns TRUE if the two sets of indices are the same.
//
/////////////////////////////////////////////////////////////////////////////

SbBool
IfCondenser::sameIndices(const SoMFInt32 *indexField1,
		       const SoMFInt32 *indexField2)
{
    int num1 = indexField1->getNum();
    int num2 = indexField2->getNum();

    if (num1 != num2)
	return FALSE;

    const int32_t *ind1 = indexField1->getValues(0);
    const int32_t *ind2 = indexField2->getValues(0);

    for (int i = 0; i < num1; i++)
	if (ind1[i] != ind2[i])
	    return FALSE;

    return TRUE;
}
