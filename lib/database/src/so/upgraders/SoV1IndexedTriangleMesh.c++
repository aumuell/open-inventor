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
 |   Classes:
 |	SoV1IndexedTriangleMesh
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include "SoV1IndexedTriangleMesh.h"
#include <Inventor/nodes/SoIndexedTriangleStripSet.h>

SO_NODE_SOURCE(SoV1IndexedTriangleMesh);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Default constructor
//
// Use: public

SoV1IndexedTriangleMesh::SoV1IndexedTriangleMesh()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoV1IndexedTriangleMesh);

    SO_NODE_ADD_FIELD(coordIndex, (-1));
    SO_NODE_ADD_FIELD(materialIndex, (-1));
    SO_NODE_ADD_FIELD(normalIndex, (-1));
    SO_NODE_ADD_FIELD(textureCoordIndex, (-1));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoV1IndexedTriangleMesh::~SoV1IndexedTriangleMesh()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Convert the triangle mesh to an IndexedFaceSet.
//
// Use: private

SoNode *
SoV1IndexedTriangleMesh::createNewNode()
//
////////////////////////////////////////////////////////////////////////
{
    SoIndexedTriangleStripSet *result = SO_UPGRADER_CREATE_NEW(SoIndexedTriangleStripSet);

    if (coordIndex.getNum() < 3) return result;

    result->coordIndex.deleteValues(0);

    // First, guess whether or not the other index fields are
    // PER_VERTEX_INDEXED; we'll assume they are if they have the same
    // (or greater) number of indices as the coordIndex field.  That
    // is a fair assumption, since the other indexed binding
    // (PER_FACE_INDEXED) requires fewer indices.
    inorm = FALSE;
    imat = FALSE;
    itex = FALSE;

    if (normalIndex.getNum() >= coordIndex.getNum()) inorm = TRUE;
    else { // Copy over field...
	result->normalIndex.setValues(0, normalIndex.getNum(),
				      normalIndex.getValues(0));
    }
    if (materialIndex.getNum() >= coordIndex.getNum()) imat = TRUE;
    else { // Copy over field...
	result->materialIndex.setValues(0, materialIndex.getNum(),
					materialIndex.getValues(0));
    }
    if (textureCoordIndex.getNum() >= coordIndex.getNum()) itex = TRUE;
    else { // Copy over field...
	result->textureCoordIndex.setValues(0, textureCoordIndex.getNum(),
				      textureCoordIndex.getValues(0));
    }

    // Converting is only tricky because of swaps.  We can get the
    // same effect as a swap by re-sending the previous vertex, to
    // form one degenerate triangle.  This is almost always more
    // efficient than ending the strip and beginning a new one.
    //
    // If there are swaps, PER_VERTEX bindings will come out wrong,
    // since vertices are repeated.

    // These keep track of the last two indices in the last triangle
    // formed:
    int triVerts[2];
    triVerts[0] = triVerts[1] = -1;

    for (int i = 0; i < coordIndex.getNum(); i++) {

	// Repeat last vertex:
	if (coordIndex[i] == SO_SWAP_MESH_INDEX) {
	    if (triVerts[0] != -1) {
		addVertex(result, triVerts[0]);

		// Emulate swap:
		int temp = triVerts[0];
		triVerts[0] = triVerts[1];
		triVerts[1] = temp;
	    }
	    else {
		// Do nothing
#ifdef DEBUG
		SoDebugError::post("SoV1IndexedTriangleMesh",
		   "Got an SO_SWAP_MESH_INDEX before any vertices");
#endif
	    }
	}
	// At the end of the mesh, reset triVerts and add END_INDEX
	// to indexedTriStrip:
	else if (coordIndex[i] == SO_END_MESH_INDEX) {
	    addVertex(result, SO_END_MESH_INDEX);

	    triVerts[0] = triVerts[1] = -1;
	}
	// Normal case, got a new vertex:
	else {
	    if (coordIndex[i] < 0) {
#ifdef DEBUG
		SoDebugError::post("SoV1IndexedTriangleMesh",
		   "coordIndex[%d]=%d out of range",
				   i, coordIndex[i]);
#endif
	    }
	    else {
		addVertex(result, i);

		triVerts[0] = triVerts[1];
		triVerts[1] = i;
	    }
	}
    }
    return result;
}

// Helper routine:
void
SoV1IndexedTriangleMesh::addVertex(SoIndexedTriangleStripSet *result,
				   int v)
{
    int index = result->coordIndex.getNum();

    if (v < 0) {  // Have an END_MESH
	result->coordIndex.set1Value(index, v);
	if (inorm)
	    result->normalIndex.set1Value(index, v);
	if (imat)
	    result->materialIndex.set1Value(index, v);
	if (itex)
	    result->textureCoordIndex.set1Value(index, v);
    } else {
	result->coordIndex.set1Value(index, coordIndex[v]);
	if (inorm)
	    result->normalIndex.set1Value(index, normalIndex[v]);
	if (imat)
	    result->materialIndex.set1Value(index, materialIndex[v]);
	if (itex)
	    result->textureCoordIndex.set1Value(index,
						textureCoordIndex[v]);
    }
}
