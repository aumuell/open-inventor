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

/////////////////////////////////////////////////////////////////////////////
//
// IfStripper class: takes a condensed scene graph and creates longer
// triangle strips in the SoIndexedTriangleStripSet.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef  _IF_STRIPPER_
#define  _IF_STRIPPER_

#include <Inventor/SbPList.h>

class IfHolder;
class SbDict;

class IfStripper {

  public:
    IfStripper();
    ~IfStripper();

    void	strip(IfHolder *_holder);

  private:

    // A vertex
    struct StripVertex {
	int		coordIndex;	// Coordinate index
	int		normIndex;	// Normal index
	int		texCoordIndex;	// Texture coordinate index
	int		mtlIndex;	// Material index
	int		uniqueID;	// Unique ID for edge hashing
	StripVertex	*next;		// For making lists
    };

    // A triangle of 3 vertices
    struct StripTriangle {
	int		index;
	StripVertex	*v[3];		// Vertices of triangle
	StripTriangle	*t[3];		// Neighboring triangles (or NULL)
	SbBool		isUsed;		// TRUE when used in strip
	int	    numUnusedNeighbors;	// # of unused neighbor triangles
	StripTriangle	*prev, *next;	// For doubly-linked lists
    };

    // An edge of a triangle
    struct StripEdge {
	StripVertex	*v1, *v2;	// Two vertices of edge, in order
	StripTriangle	*tri;		// Triangle containing edge
	int		which;		// Which edge of triangle (0, 1, or 2)
	StripEdge	*next;		// Used to create lists
    };

    IfHolder		*holder;	// Holds most important stuff
    int			numVertices;	// Number of distinct vertices
    int			numTriangles;	// # of triangles in original strips
    SbBool		haveMaterials;	// TRUE if material indices exist
    SbBool		haveNormals;	// TRUE if normal indices exist
    SbBool		haveTexCoords;	// TRUE if texCoord indices exist
    StripVertex		*vertices;	// Array of StripVertex structures

    // This maps from the original triangle vertices into the stored
    // StripVertex instances. The index into this array is the index
    // into the coordIndex field of the original triangle strip set,
    // and the value stored is the index into the vertices array.
    int			*vertexMap;

    StripTriangle	*triangles;	// Array of StripTriangle structures
    SbDict		*edgeDict;	// Dictionary storing edges

    // List of pointers to StripVertex structures representing
    // strips. The end of a strip is indicated by a NULL pointer.
    SbPList		*vertexPtrList;
    
    // During the stripping process, it is better to begin with a
    // triangle that has few neighbor triangles. To make it easy to
    // find such triangles, we keep 4 lists, depending on the number
    // of unused neighbors (0 - 3). These lists are doubly-linked,
    // since triangles move from one list to another frequently, and
    // it has to be fast to remove and insert them.
    StripTriangle	*pendingTriList[4];

    // Fills in specialized data structures
    void		createVertexList();
    void		createTriangleList();
    
    // Finds adjacent triangles
    void		findNeighborTriangles();
    
    // Sets up the pending triangle lists
    void		setUpPendingTriangleLists();

    // Adds an edge to the list of edges, checking for neighbors
    void		addEdge(StripEdge *newEdge);

    // Hash function for edges
    uint32_t		hashEdge(StripEdge *edge);

    // Creates strips from the connected triangles
    void		createStrips();

    // Chooses and returns a good starting triangle for tstrip
    // generation. Returns FALSE if none.
    SbBool		chooseStartTriangle(StripTriangle *&tri);

    // Marks a triangle as used, removing it from the pending list and
    // changing the status of all neighbor triangles
    void		markTriangleUsed(StripTriangle *tri);

    // Adds/removes a triangle to/from the given pending list
    void		addTriangle(StripTriangle *tri, int listIndex);
    void		removeTriangle(StripTriangle *tri, int listIndex);

    // Changes the indices in the triangle strip
    void		adjustIndices();
};

#endif /* _IF_STRIPPER_ */
