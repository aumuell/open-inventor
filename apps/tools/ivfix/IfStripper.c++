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

#include <Inventor/SbDict.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoIndexedTriangleStripSet.h>

#include "IfAssert.h"
#include "IfStripper.h"
#include "IfHolder.h"

/////////////////////////////////////////////////////////////////////////////
//
// Constructor. Initializes data structures.
//
/////////////////////////////////////////////////////////////////////////////

IfStripper::IfStripper()
{
    holder = NULL;
}

/////////////////////////////////////////////////////////////////////////////
//
// Destructor.
//
/////////////////////////////////////////////////////////////////////////////

IfStripper::~IfStripper()
{
}

/////////////////////////////////////////////////////////////////////////////
//
// This takes a scene graph produced by flattening and condensing a
// graph and produces better triangle strips, if possible.
//
/////////////////////////////////////////////////////////////////////////////

void
IfStripper::strip(IfHolder *_holder)
{
    holder = _holder;

    // There had better be an SoIndexedTriangleStripSet in the IfHolder
    ASSERT(holder->stripSet != NULL);

    // This assumes that there is currently 1 triangle per strip in
    // the SoIndexedTriangleStripSet
    ASSERT(holder->stripSet->coordIndex.getNum() % 4 == 0);
    numTriangles = holder->stripSet->coordIndex.getNum() / 4;

    // See if there are any normal, texture coord, and material indices
    haveNormals   = (holder->doNormals &&
		     holder->stripSet->normalIndex.getNum() > 1);
    haveTexCoords = (holder->doTexCoords &&
		     holder->stripSet->textureCoordIndex.getNum() > 1);
    haveMaterials = (holder->stripSet->materialIndex.getNum() > 1);

    // Coordinate, normal, and texture coordinate values have been
    // condensed, but we don't know how many distinct vertices there
    // are. If two vertices have all identical indices, they are
    // duplicates. (It's possible for two vertices to share
    // coordinates, but not normals, for example.) Determine
    // how many distinct vertices there are and allocate StripVertex
    // structures for them.
    createVertexList();

    // Allocate and fill in triangle data structures
    createTriangleList();

    // Find adjacent triangles
    findNeighborTriangles();

    // Set up the pending triangle lists
    setUpPendingTriangleLists();

    // We are going to fill in the vertex pointer list (vertexPtrList)
    // with pointers to the vertices forming the new triangle
    // strips. A NULL pointer indicates the end of a strip. First,
    // create the list with a reasonable size guess, to avoid lots of
    // reallocing.
    vertexPtrList = new SbPList(numTriangles * 2);

    // Create strips from the connected triangles
    createStrips();

    // Change the indices in the triangle strip
    adjustIndices();

    // Clean up
    delete [] vertices;
    delete [] triangles;
    delete vertexPtrList;
}

////////////////////////////////////////////////////////////////////////
//
// Fills in StripVertex structures that represent triangle
// vertices. Returns the number of distinct vertices.
//
////////////////////////////////////////////////////////////////////////

void
IfStripper::createVertexList()
{
    // Step 0: Access the fields we will need to use
    int         numCoordIndices = holder->stripSet->coordIndex.getNum();
    const int32_t *coordIndices    = holder->stripSet->coordIndex.getValues(0);
    const int32_t *normalIndices	= (! haveNormals ? NULL :
				   holder->stripSet->normalIndex.getValues(0));
    const int32_t *texCoordIndices	= (! haveTexCoords ? NULL :
				   holder->stripSet->
				   textureCoordIndex.getValues(0));
    const int32_t *mtlIndices = (! haveMaterials ? NULL :
			      holder->stripSet->materialIndex.getValues(0));

    // Step 1: Assume the worst: all coordinates are distinct
    int maxNumVertices = numTriangles * 3;

    // Step 2: Allocate an array of StripVertex instances big enough
    // to hold all of them
    StripVertex *allVertices = new StripVertex[maxNumVertices];

    // Step 3: Create an array of pointers to StripVertex instances,
    // indexed by coordinate index. This will allow use to find
    // duplicates very easily. (Note that the maximum coordinate index
    // we can find is the number of coordinate values - 1.)
    int i, numCoords = holder->coords->point.getNum();
    StripVertex **table = new StripVertex *[numCoords];
    for (i = 0; i < numCoords; i++)
	table[i] = NULL;

    // Step 4: Also create an array of indices into the StripVertex
    // instances, indexed by the index into the coordIndex field. This
    // is used later on to create triangles from the StripVertex
    // instances.
    vertexMap = new int [numCoordIndices];

    // Step 5: Fill in the StripVertex instances with actual indices,
    // checking for duplicates. If we find a duplicate, we don't save
    // it.
    StripVertex *curVert = allVertices;
    for (i = 0; i < numCoordIndices; i++) {

	int coordIndex = coordIndices[i];

	if (coordIndex < 0) {
	    vertexMap[i] = -1;
	    continue;
	}

	// Fill in the next StripVertex instance
	curVert->coordIndex	= coordIndex;
	curVert->normIndex	= (haveNormals   ?   normalIndices[i] : -1);
	curVert->texCoordIndex	= (haveTexCoords ? texCoordIndices[i] : -1);
	curVert->mtlIndex	= (haveMaterials ?      mtlIndices[i] : -1);
	curVert->uniqueID	= curVert - allVertices;

	// See if we have any entries at that slot in the table
	StripVertex *oldVert;
	for (oldVert = table[coordIndex]; oldVert != NULL;
	     oldVert = oldVert->next) {

	    if (oldVert->normIndex     == curVert->normIndex &&
		oldVert->texCoordIndex == curVert->texCoordIndex &&
		oldVert->mtlIndex      == curVert->mtlIndex)
		break;
	}

	// If we found a match, re-use the old vertex
	if (oldVert != NULL)
	    vertexMap[i] = oldVert->uniqueID;

	// Otherwise, store this vertex in the table and prepare for
	// the next vertex
	else {
	    curVert->next = table[coordIndex];
	    table[coordIndex] = curVert;
	    vertexMap[i] = curVert->uniqueID;
	    curVert++;
	}
    }

    // Step 6: The number of vertices we stored is the number of
    // distinct vertices
    numVertices = curVert - allVertices;

    // Step 7: Copy the distinct vertices into the real array
    vertices = new StripVertex[numVertices];
    for (i = 0; i < numVertices; i++)
	vertices[i] = allVertices[i];

    // Step 8: Get rid of the stuff we don't need any more
    delete [] table;
    delete [] allVertices;
}

////////////////////////////////////////////////////////////////////////
//
// Creates structures that will be easier to deal with when creating
// triangle strips.
//
////////////////////////////////////////////////////////////////////////

void
IfStripper::createTriangleList()
{
    const int32_t *coordIndices = holder->stripSet->coordIndex.getValues(0);

    // Allocate triangles
    triangles = new StripTriangle[numTriangles];

    // Fill in triangle structures, using the vertexMap created in
    // createVertexList
    for (int i = 0; i < numTriangles; i++) {

	StripTriangle *tri = &triangles[i];

	tri->index = i;

	for (int j = 0; j < 3; j++) {
	    ASSERT(coordIndices[4*i + j] >= 0);
	    int vertexIndex = vertexMap[4*i + j];
	    ASSERT(vertexIndex >= 0);
	    ASSERT(vertexIndex < numVertices);
	    tri->v[j] = &vertices[vertexIndex];
	}

	// No neighbors yet
	tri->t[0] = tri->t[1] = tri->t[2] = NULL;

	// Not used in a real strip yet
	tri->isUsed = FALSE;

	// We have no neighbor info yet. We will compute this later.
	tri->numUnusedNeighbors = -1;

	// Not in a list yet
	tri->prev = tri->next = NULL;
    }

    // We don't need this any more
    delete [] vertexMap;
}

/////////////////////////////////////////////////////////////////////////////
//
// Finds adjacent triangles
//
/////////////////////////////////////////////////////////////////////////////

void
IfStripper::findNeighborTriangles()
{
    // Allocate edge structures
    StripEdge *edges = new StripEdge[3 * numTriangles];

    // Create a dictionary for finding edges
    edgeDict = new SbDict(1235);

    // Run through triangles, storing edge structures and finding neighbors
    StripEdge *edge = edges;
    for (int i = 0; i < numTriangles; i++) {

	StripTriangle *tri = &triangles[i];

	// Look for a neighbor edge for each of the 3 edges. If found,
	// mark the two triangles as neighbors and remove the neighbor
	// edge from the list of unpaired edges. If not found, add the
	// edge to the list.

#define DO_EDGE(VA, VB, WHICH)						      \
	edge->v1 = tri->v[VA];						      \
	edge->v2 = tri->v[VB];						      \
	edge->tri = tri;						      \
	edge->which = WHICH;						      \
	edge->next  = NULL;						      \
	addEdge(edge);							      \
	edge++

	DO_EDGE(1, 2,  0);
	DO_EDGE(2, 0,  1);
	DO_EDGE(0, 1,  2);

#undef DO_EDGE
    }

    // We created the edges only to set up the neighbor relationships
    // in triangles. Now that that is done, we don't need the edges
    // any more
    delete [] edges;
    delete edgeDict;
}

/////////////////////////////////////////////////////////////////////////////
//
// Adds an edge to the list of edges, checking for neighbors
//
/////////////////////////////////////////////////////////////////////////////

void
IfStripper::addEdge(StripEdge *newEdge)
{
    // See if any edges like this one are already in the dictionary
    uint32_t key = hashEdge(newEdge);
    void *listPtr;
    if (edgeDict->find(key, listPtr)) {

	// Look through edge list for a neighbor
	SbBool foundNeighbor = FALSE;
	StripEdge *prevEdge = NULL;
	for (StripEdge *edge = (StripEdge *) listPtr;
	     edge != NULL;
	     edge = edge->next) {

	    // See if the edges are neighbors
	    if (edge->v1 == newEdge->v2 && edge->v2 == newEdge->v1) {

		// Store the neighbor relationship in both triangles
		edge->tri->t[edge->which]	= newEdge->tri;
		newEdge->tri->t[newEdge->which]	= edge->tri;

		// Remove the neighbor edge from the dictionary
		if (prevEdge != NULL)
		    prevEdge->next = edge->next;
		else {
		    listPtr = edge->next;

		    // If nothing left in list, remove the whole list
		    // from dictionary
		    if (listPtr == NULL)
			edgeDict->remove(key);
		}

		// No need to keep looking
		foundNeighbor = TRUE;
		break;
	    }

	    prevEdge = edge;
	}

	// Add the new edge to the end of the list if no neighbor was found
	if (! foundNeighbor)
	    prevEdge->next = newEdge;
    }

    // If no list exists yet for this hash key, create a new one
    else
	edgeDict->enter(key, newEdge);
}

/////////////////////////////////////////////////////////////////////////////
//
// Hash function for edges. This function is guaranteed to return the
// same value for two edges containing the same vertices, regardless
// of direction; i.e., it's symmetric.
//
/////////////////////////////////////////////////////////////////////////////

uint32_t
IfStripper::hashEdge(StripEdge *edge)
{
    // Use the unique ID in each vertex to create the hashing key.
    // Since we have to maintain symmetry, use the smaller index first.
    int i1 = edge->v1->uniqueID;
    int i2 = edge->v2->uniqueID;
    if (i2 < i1) {
	int t = i1;
	i1 = i2;
	i2 = t;
    }

    // Note: this function is not guaranteed to produce all unique
    // keys (if there are > 65536 unique vertices), but that's no
    // problem, because there is a list of edges in each hash slot.
    return (i1 << 16) + i2;
}

/////////////////////////////////////////////////////////////////////////////
//
// Sets up the pending triangle lists.
//
/////////////////////////////////////////////////////////////////////////////

void
IfStripper::setUpPendingTriangleLists()
{
    int i;
    for (i = 0; i < 4; i++)
	pendingTriList[i] = NULL;

    // Now that all neighbors are known, figure out how many neighbors
    // each triangle has and set numUnusedNeighbors to that
    // number. Then add the triangle to the appropriate list.
    for (i = 0; i < numTriangles; i++) {
	StripTriangle *tri = &triangles[i];

	tri->numUnusedNeighbors = 0;

	for (int j = 0; j < 3; j++)
	    if (tri->t[j] != NULL)
		tri->numUnusedNeighbors++;

	addTriangle(tri, tri->numUnusedNeighbors);
    }
}

/////////////////////////////////////////////////////////////////////////////
//
// Creates strips from the connected triangles.
//
/////////////////////////////////////////////////////////////////////////////

void
IfStripper::createStrips()
{
    StripTriangle *tri;

    // Repeat until there are no more unmarked triangles
    while (chooseStartTriangle(tri)) {

	// Find a shared edge (if any)
	int shared = 3;
	for (int j = 0; j < 3; j++) {
	    if (tri->t[j] != NULL && ! tri->t[j]->isUsed) {
		shared = j;
		break;
	    }
	}

	// Add 3 vertices of triangle, starting with vertex opposite
	// shared edge to strip
	switch (shared) {
	  case 0:
	    vertexPtrList->append(tri->v[0]);
	    vertexPtrList->append(tri->v[1]);
	    vertexPtrList->append(tri->v[2]);
	    break;
	  case 1:
	    vertexPtrList->append(tri->v[1]);
	    vertexPtrList->append(tri->v[2]);
	    vertexPtrList->append(tri->v[0]);
	    break;
	  case 2:
	  case 3:    // No shared edges; order doesn't matter
	    vertexPtrList->append(tri->v[2]);
	    vertexPtrList->append(tri->v[0]);
	    vertexPtrList->append(tri->v[1]);
	    break;
	}

	// No shared edges? Mark triangle as used, terminate strip,
	// and try another triangle
	if (shared == 3) {
	    markTriangleUsed(tri);
	    vertexPtrList->append(NULL);
	    continue;
	}

	// This is used to choose which edge to go to next. It
	// alternates after each triangle, to form a proper strip.
	SbBool	oddFace = TRUE;

	while (TRUE) {

	    markTriangleUsed(tri);

	    // Get the next triangle
	    ASSERT(shared != 3);
	    StripTriangle *nextTri = tri->t[shared];

	    // Stop if there isn't any next triangle
	    if (nextTri == NULL || nextTri->isUsed)
		break;

	    // Add the vertex of this triangle opposite the shared edge
	    if (nextTri->t[0] == tri) {
		vertexPtrList->append(nextTri->v[0]);
		shared = (oddFace ? 2 : 1);
	    }
	    else if (nextTri->t[1] == tri) {
		vertexPtrList->append(nextTri->v[1]);
		shared = (oddFace ? 0 : 2);
	    }
	    else if (nextTri->t[2] == tri) {
		vertexPtrList->append(nextTri->v[2]);
		shared = (oddFace ? 1 : 0);
	    }

	    // It's possible (if more than 2 triangles share an edge)
	    // that nextTri does not consider tri to be a neighbor. If
	    // this happens, end the current strip and start another.
	    else
		break;

	    // Prepare for next triangle
	    tri = nextTri;
	    oddFace = ! oddFace;
	}

	// Terminate the strip
	vertexPtrList->append(NULL);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Chooses and returns a good starting triangle for tstrip generation.
//
////////////////////////////////////////////////////////////////////////

SbBool
IfStripper::chooseStartTriangle(StripTriangle *&tri)
{
    // Choose the triangle with the fewest unused neighbors

    for (int i = 0; i < 4; i++) {
	if (pendingTriList[i] != NULL) {
	    tri = pendingTriList[i];
	    ASSERT(! tri->isUsed);
	    return TRUE;
	}
    }

    tri = NULL;
    return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Marks a triangle as used, removing it from the pending list and
// changing the status of all neighbor triangles.
//
/////////////////////////////////////////////////////////////////////////////

void
IfStripper::markTriangleUsed(StripTriangle *tri)
{
    ASSERT(! tri->isUsed);

    tri->isUsed = TRUE;

    // Remove it from its current list
    removeTriangle(tri, tri->numUnusedNeighbors);

    // Decrement the numUnusedNeighbors in all neighbors and move them
    // to the correct list
    for (int i = 0; i < 3; i++) {
	StripTriangle *neigh = tri->t[i];
	if (neigh != NULL && ! neigh->isUsed) {
	    ASSERT(neigh->numUnusedNeighbors > 0);
	    removeTriangle(neigh, neigh->numUnusedNeighbors);
	    addTriangle(neigh, --neigh->numUnusedNeighbors);
	}
    }
}

/////////////////////////////////////////////////////////////////////////////
//
// Adds a triangle to the given pending list.
//
/////////////////////////////////////////////////////////////////////////////

void
IfStripper::addTriangle(StripTriangle *tri, int listIndex)
{
    StripTriangle *first = pendingTriList[listIndex];

    if (first != NULL)
	first->prev = tri;
    tri->next = first;
    tri->prev = NULL;
    pendingTriList[listIndex] = tri;
}

/////////////////////////////////////////////////////////////////////////////
//
// Removes a triangle from the given pending list.
//
/////////////////////////////////////////////////////////////////////////////

void
IfStripper::removeTriangle(StripTriangle *tri, int listIndex)
{
    if (tri->prev != NULL)
	tri->prev->next = tri->next;
    else
	pendingTriList[listIndex] = tri->next;

    if (tri->next != NULL)
	tri->next->prev = tri->prev;
}

/////////////////////////////////////////////////////////////////////////////
//
// Changes the indices in the triangle strip.
//
/////////////////////////////////////////////////////////////////////////////

void
IfStripper::adjustIndices()
{
    int numVertices = vertexPtrList->getLength();

    // Fill in the coordinate indices
    holder->stripSet->coordIndex.setNum(numVertices);
    int32_t *indices = holder->stripSet->coordIndex.startEditing();
    for (int i = 0; i < numVertices; i++) {
	StripVertex *vert = (StripVertex *) (*vertexPtrList)[i];
	indices[i] = (vert == NULL ? -1 : vert->coordIndex);
    }
    holder->stripSet->coordIndex.finishEditing();

    // Do same for normal indices, if necessary
    if (haveNormals) {
	holder->stripSet->normalIndex.setNum(numVertices);
	indices = holder->stripSet->normalIndex.startEditing();
	for (int i = 0; i < numVertices; i++) {
	    StripVertex *vert = (StripVertex *) (*vertexPtrList)[i];
	    indices[i] = (vert == NULL ? -1 : vert->normIndex);
	}
	holder->stripSet->normalIndex.finishEditing();
    }

    // Do same for texture coordinate indices, if necessary
    if (haveTexCoords) {
	holder->stripSet->textureCoordIndex.setNum(numVertices);
	indices = holder->stripSet->textureCoordIndex.startEditing();
	for (int i = 0; i < numVertices; i++) {
	    StripVertex *vert = (StripVertex *) (*vertexPtrList)[i];
	    indices[i] = (vert == NULL ? -1 : vert->texCoordIndex);
	}
	holder->stripSet->textureCoordIndex.finishEditing();
    }

    // Do same for material indices, if necessary
    if (haveMaterials) {
	holder->stripSet->materialIndex.setNum(numVertices);
	indices = holder->stripSet->materialIndex.startEditing();
	for (int i = 0; i < numVertices; i++) {
	    StripVertex *vert = (StripVertex *) (*vertexPtrList)[i];
	    indices[i] = (vert == NULL ? -1 : vert->mtlIndex);
	}
	holder->stripSet->materialIndex.finishEditing();
    }
}
