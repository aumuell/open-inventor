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

#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoIndexedShape.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoShape.h>
#include <Inventor/nodes/SoTextureCoordinate2.h>
#include <Inventor/nodes/SoTextureCoordinateBinding.h>

#include "IfFlattener.h"
#include "IfHolder.h"

/////////////////////////////////////////////////////////////////////////////
//
// Constructor. Initializes data structures.
//
/////////////////////////////////////////////////////////////////////////////

IfFlattener::IfFlattener()
{
    holder = NULL;

    numVerts      = 0;
    numMtlIndices = 0;
}

/////////////////////////////////////////////////////////////////////////////
//
// Destructor.
//
/////////////////////////////////////////////////////////////////////////////

IfFlattener::~IfFlattener()
{
}

/////////////////////////////////////////////////////////////////////////////
//
// This function assumes that everything under the given root can be
// treated as a big bag o' triangles. It will store coordinates,
// normals, texture coordinates, and material indices for these
// triangles in the IfHolder's nodes.
//
/////////////////////////////////////////////////////////////////////////////

void
IfFlattener::flatten(IfHolder *_holder)
{
    holder = _holder;

    // Assume that we will find lots of triangles. We'll clean up
    // later if there are fewer. Fields will start out with this many
    // entries. We double this size whenever we run out of space. When
    // we're done, we shrink the fields down to the amount used.
    coordVals    = NULL;
    normalVals   = NULL;
    texCoordVals = NULL;
    mtlIndexVals = NULL;
    fieldSize = 20000;
    expandFields();

    // We'll apply a callback action to produce triangles from all shapes
    SoCallbackAction cba;

    // Set up a callback to call before and after shape is traversed
    cba.addPreCallback(SoShape::getClassTypeId(), preShapeCB, this);
    cba.addPostCallback(SoShape::getClassTypeId(), postShapeCB, this);

    // Set up a callback to call when a shape is traversed
    cba.addTriangleCallback(SoShape::getClassTypeId(), triangleCB, this);

    // Traverse the graph, adding to the data structures
    cba.apply(holder->origRoot);

    // Finish editing
    holder->coords->point.finishEditing();
    holder->triSet->materialIndex.finishEditing();
    if (holder->doNormals)
	holder->normals->vector.finishEditing();
    if (holder->doTexCoords)
	holder->texCoords->point.finishEditing();

    // Process the resulting structures to produce a graph
    createGraph();
}

/////////////////////////////////////////////////////////////////////////////
//
// Expands the fields in the nodes to contain fieldSize entries.
//
/////////////////////////////////////////////////////////////////////////////

void
IfFlattener::expandFields()
{
    // If already editing the fields, stop!
    if (coordVals != NULL) {
	holder->coords->point.finishEditing();
	holder->triSet->materialIndex.finishEditing();
	if (holder->doNormals)
	    holder->normals->vector.finishEditing();
	if (holder->doTexCoords)
	    holder->texCoords->point.finishEditing();
    }

    // Increase the size
    holder->coords->point.setNum(fieldSize);
    if (holder->doNormals)
	holder->normals->vector.setNum(fieldSize);
    if (holder->doTexCoords)
	holder->texCoords->point.setNum(fieldSize);
    holder->triSet->coordIndex.setNum(fieldSize);
    holder->triSet->materialIndex.setNum(fieldSize);

    // Start editing (again, maybe)
    coordVals = holder->coords->point.startEditing();
    mtlIndexVals = holder->triSet->materialIndex.startEditing();
    if (holder->doNormals)
	normalVals = holder->normals->vector.startEditing();
    if (holder->doTexCoords)
	texCoordVals = holder->texCoords->point.startEditing();
}

/////////////////////////////////////////////////////////////////////////////
//
// Prepares to process the triangles making up a shape.
//
/////////////////////////////////////////////////////////////////////////////

void
IfFlattener::prepareForShape(SoCallbackAction *cba, const SoShape *)
{
    // Get the current model matrix so we can transform the
    // coordinates and normals
    pointMatrix  = cba->getModelMatrix();
    if (holder->doNormals)
	normalMatrix = pointMatrix.inverse().transpose();
    if (holder->doTexCoords)
	textureMatrix = cba->getTextureMatrix();
}

/////////////////////////////////////////////////////////////////////////////
//
// Adds a triangle generated by the current shape.
//
/////////////////////////////////////////////////////////////////////////////

void
IfFlattener::addTriangle(SoCallbackAction *, const SoPrimitiveVertex *verts[3])
{
    // Make sure there's enough room in the fields
    if (numMtlIndices + 4 >= fieldSize) {
	fieldSize *= 2;
	expandFields();
    }

    // For each of the three vertices
    for (int i = 0; i < 3; i++) {

	addCoordinate(verts[i]->getPoint());

	if (holder->doNormals)
	    addNormal(verts[i]->getNormal());

	if (holder->doTexCoords)
	    addTextureCoordinate(verts[i]->getTextureCoords());

	addMaterialIndex(verts[i]->getMaterialIndex());

	numVerts++;
	numMtlIndices++;
    }

    // Add a material index to skip (for the end-of-triangle marker)
    addMaterialIndex(-1);
    numMtlIndices++;
}

/////////////////////////////////////////////////////////////////////////////
//
// Finishes processing of the current shape.
//
/////////////////////////////////////////////////////////////////////////////

void
IfFlattener::finishShape(SoCallbackAction *, const SoShape *)
{
}

/////////////////////////////////////////////////////////////////////////////
//
// Adds a point to the list of coordinates.
//
/////////////////////////////////////////////////////////////////////////////

void
IfFlattener::addCoordinate(const SbVec3f &point)
{
    SbVec3f transformedPoint;
    pointMatrix.multVecMatrix(point, transformedPoint);
    coordVals[numVerts] = transformedPoint;
}

/////////////////////////////////////////////////////////////////////////////
//
// Adds a normal to the list of normals.
//
/////////////////////////////////////////////////////////////////////////////

void
IfFlattener::addNormal(const SbVec3f &normal)
{
    SbVec3f transformedNormal;
    normalMatrix.multDirMatrix(normal, transformedNormal);
    transformedNormal.normalize();
    normalVals[numVerts] = transformedNormal;
}

/////////////////////////////////////////////////////////////////////////////
//
// Adds a material index to the list of indices.
//
/////////////////////////////////////////////////////////////////////////////

void
IfFlattener::addMaterialIndex(int materialIndex)
{
    mtlIndexVals[numMtlIndices] = materialIndex;
}

/////////////////////////////////////////////////////////////////////////////
//
// Adds a texture coordinate to the list.
//
/////////////////////////////////////////////////////////////////////////////

void
IfFlattener::addTextureCoordinate(const SbVec4f &texCoord)
{
    // Ignore everything except S and T
    SbVec3f stTexCoord(texCoord[0], texCoord[1], 0.0);

    SbVec3f transformedTexCoord;
    textureMatrix.multVecMatrix(stTexCoord, transformedTexCoord);

    SbVec2f t(transformedTexCoord[0], transformedTexCoord[1]);
    texCoordVals[numVerts] = t;
}

/////////////////////////////////////////////////////////////////////////////
//
// Processes the resulting structures to produce a graph.
//
/////////////////////////////////////////////////////////////////////////////

void
IfFlattener::createGraph()
{
    // The coordinates, normals, and texture coordinates should
    // already be set. All we need to do here is set up the
    // indices in the SoIndexedShape.
    int numTris = numVerts / 3;
    holder->triSet->coordIndex.setNum(numTris * 4);
    int32_t *cindex = holder->triSet->coordIndex.startEditing();
    int ci = 0, v = 0;
    for (int t = 0; t < numTris; t++) {
	cindex[ci++] = v++;
	cindex[ci++] = v++;
	cindex[ci++] = v++;
	cindex[ci++] = -1;
    }
    holder->triSet->coordIndex.finishEditing();

    // The normal and texture coordinate indices start out the same as
    // the coordinate indices
    if (holder->doNormals)
	holder->triSet->normalIndex = holder->triSet->coordIndex;
    if (holder->doTexCoords)
	holder->triSet->textureCoordIndex = holder->triSet->coordIndex;

    // Decrease the sizes of the fields if there is any extra space
    if (numVerts < fieldSize) {
	holder->coords->point.setNum(numVerts);
	if (holder->doNormals)
	    holder->normals->vector.setNum(numVerts);
	if (holder->doTexCoords)
	    holder->texCoords->point.setNum(numVerts);
    }
    if (numMtlIndices < fieldSize)
	holder->triSet->materialIndex.setNum(numMtlIndices);

    // Assume that the normals, materials, and texture coordinates are
    // bound per vertex, using the indices
    if (holder->doNormals)
	holder->normalBinding->value = SoNormalBinding::PER_VERTEX_INDEXED;
    holder->materialBinding->value = SoMaterialBinding::PER_VERTEX_INDEXED;
    if (holder->doTexCoords)
	holder->texCoordBinding->value =
	    SoTextureCoordinateBinding::PER_VERTEX_INDEXED;
}
