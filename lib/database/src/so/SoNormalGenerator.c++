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
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Classes:
 |      SoNormalGenerator
 |
 |   Author(s)          : Thad Beier, Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#ifdef __linux__
#include <machine.h>
#endif
#include <Inventor/SbBox.h>
#include <Inventor/misc/SoNormalGenerator.h>

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoNormalGenerator::SoNormalGenerator(SbBool _isCCW)
//
////////////////////////////////////////////////////////////////////////
{
    // Make some room
    maxPoints	   = 16;
    maxVertNormals = 0;

    numPoints = numVertNormals = 0;

    points      = new SbVec3f[maxPoints];
    faceNormals = new SbVec3f[maxPoints];
    vertNormals = NULL;

    isCCW = _isCCW;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor that takes hint about number of vertices
//
// Use: public

SoNormalGenerator::SoNormalGenerator(SbBool _isCCW, int approxNumVertices)
//
////////////////////////////////////////////////////////////////////////
{
    // Protect against bad number of vertices
    maxPoints	   = (approxNumVertices <= 0 ? 16 : approxNumVertices);
    maxVertNormals = 0;

    numPoints = numVertNormals = 0;

    points      = new SbVec3f[maxPoints];
    faceNormals = new SbVec3f[maxPoints];
    vertNormals = NULL;

    isCCW = _isCCW;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: public

SoNormalGenerator::~SoNormalGenerator()
//
////////////////////////////////////////////////////////////////////////
{
    if (points != NULL)
	delete [] points;

    if (vertNormals != faceNormals)
	delete [] faceNormals;

    // Do NOT delete vertNormals. The caller is responsible for this.
}
    
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Prepares to receive vertices of a polygon
//
// Use: public

void
SoNormalGenerator::beginPolygon()
//
////////////////////////////////////////////////////////////////////////
{
    beginPolygonIndex = numPoints;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sends a vertex of the current polygon
//
// Use: public

void
SoNormalGenerator::polygonVertex(const SbVec3f &point)
//
////////////////////////////////////////////////////////////////////////
{
    // Make sure there's enough room for a new vertex point and face normal
    if (numPoints == maxPoints) {
	SbVec3f *newArray;

	newArray = new SbVec3f [2 * maxPoints];
	bcopy(points, newArray, (int) (maxPoints * sizeof(SbVec3f)));
	delete [] points;
	points = newArray;

	newArray = new SbVec3f [maxPoints * 2];
	bcopy(faceNormals, newArray, (int) (maxPoints * sizeof(SbVec3f)));
	delete [] faceNormals;
	faceNormals = newArray;

	maxPoints *= 2;
    }

    // Add the new point
    points[numPoints++] = point;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Closes the current polygon. Causes the face normal to be
//    computed and stored.
//
// Use: public

void
SoNormalGenerator::endPolygon()
//
////////////////////////////////////////////////////////////////////////
{
    int32_t	numVertices = numPoints - beginPolygonIndex;
    int32_t	i, j;
    SbVec3f	sum(0.0, 0.0, 0.0);

    // Calculate a normal for this polygon.  Use Newell's method.
    // Newman & Sproull, pg. 499
    // We've gotta be careful of small polygons very far away from the
    // origin-- floating point errors can get really big.  So we'll
    // translate the first vertex of the polygon to the origin and
    // pull all the other vertices along with it:
    const SbVec3f &firstPoint = points[beginPolygonIndex];
    for (i = 0; i < numVertices; i++) {
	j = i + 1;
	if (j == numVertices)
	    j = 0;
	sum +=
	    (points[beginPolygonIndex + i]-firstPoint).cross(
	     points[beginPolygonIndex + j]-firstPoint);
    }

    // Store the face normal for all of these points
    sum.normalize();

    // Invert if face is clockwise
    if (!isCCW) {
	sum.negate();
    }

    for (i = 0; i < numVertices; i++)
	faceNormals[beginPolygonIndex + i] = sum;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Send one triangle.  Uses polygon routines above...
//
// Use: public

void
SoNormalGenerator::triangle(const SbVec3f &p1,
			    const SbVec3f &p2,
			    const SbVec3f &p3)
//
////////////////////////////////////////////////////////////////////////
{
    beginPolygon();
    polygonVertex(p1);
    polygonVertex(p2);
    polygonVertex(p3);
    endPolygon();
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    The hash function for vertices.  See comments below in generate
//    for details.
//
// Use: internal

static inline int32_t
hash(const SbVec3f &vertex, const SbVec3f &scale, const SbVec3f &base,
     int32_t numPoints)
//
////////////////////////////////////////////////////////////////////////
{
    int32_t result;
    result = (int32_t) floor((vertex+base).dot(scale));

    if (result < 0) return 0;
    if (result >= numPoints) return numPoints-1;
    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Calculates the vertex normals once all vertices have been sent.
//
// Use: public

void
SoNormalGenerator::generate(float creaseAngle)
//
////////////////////////////////////////////////////////////////////////
{
    // First, check for fast case of normal-per-face:
    if (creaseAngle < 0.01) {
	vertNormals = faceNormals;
	numVertNormals = numPoints;
	return;
    }

    SbBox3f	box;
    SbVec3f	hashScale, sum, base;
    float	tolerance, cosCreaseAngle = cos(creaseAngle);
    int32_t	i, j, hashValue, lowHashValue, highHashValue, hv;
    int32_t	*hashTable, *hashNext, *indirect;
    SbBool	found;

    // Compute the bounding box of all vertices
    for (i = 0; i < numPoints; i++)
	box.extendBy(points[i]);

    // We will use a hash function to determine which vertices are
    // coincident within some tolerance. The tolerance is a function
    // of the size of the bounding box. The hash function is a linear
    // function of x, y, and z such that one corner of the bounding
    // box maps to the key "0" and the opposite corner maps to the key
    // "numPoints".  [As Thad says, we would have to be pretty unlucky
    // for this to be slow: all the points would have to be on the
    // exact wrong diagonal plane through the bounding box for them to
    // hash to the same value.]
    box.getSize(hashScale[0], hashScale[1], hashScale[2]);
    tolerance = (hashScale[0] + hashScale[1] + hashScale[2]) / 10000;
    SbVec3f toleranceVec(tolerance, tolerance, tolerance);
    if (hashScale[0] != 0.0)
	hashScale[0] = .333 * numPoints / hashScale[0];
    if (hashScale[1] != 0.0)
	hashScale[1] = .333 * numPoints / hashScale[1];
    if (hashScale[2] != 0.0)
	hashScale[2] = .333 * numPoints / hashScale[2];

    // Compute the base for the hash function, which is just the
    // minimum point of the bounding box:
    base = -box.getMin();

    // Make a hash table.  There are numPoints entries in the hash
    // table. Each table entry points to the first point in the list
    // of points that hash to the corresponding key. Each entry in the
    // "hashNext" array points to the next point in the list. The
    // "indirect" table is a circularly linked list of indices that
    // are within tolerance of each other.
    hashTable = new int32_t[numPoints];
    hashNext  = new int32_t[numPoints];
    indirect  = new int32_t[numPoints];
    for (i = 0; i < numPoints; i++) {
	hashTable[i] = -1;
	hashNext[i]  = -1;
	indirect[i]  = -1;
    }

    // Insert all points into the hash table.  Find common vertices.
    for (i = 0; i < numPoints; i++) {
	// Compute hash key
	hashValue = hash(points[i], hashScale, base, numPoints);

	// Set up "next" link
	hashNext[i] = hashTable[hashValue];

	// Enter in table
	hashTable[hashValue] = i;

	// Find all other vertices that are within tolerance
	found = FALSE;
	lowHashValue  = hash(points[i] - toleranceVec, hashScale,
			     base, numPoints);
	highHashValue  = hash(points[i] + toleranceVec, hashScale,
			      base, numPoints);

	for (hv = lowHashValue; hv <= highHashValue; hv++) {
	    for (j = hashTable[hv]; found == FALSE && j >= 0; j = hashNext[j]){
		if (i != j && equal(points[j], points[i], tolerance)) {
		    // Splice into the circularly linked list
		    indirect[i] = indirect[j];
		    indirect[j] = i;
		    found = TRUE;
		    break;
		}
	    }

	    // If no match found, link point to itself
	    if (found == FALSE)
		indirect[i] = i;
	}
    }

    // At this point, we're done with points[]; re-use storage to hold
    // vertNormals (memory optimization):
    vertNormals = points;
    numVertNormals = numPoints;
    points = NULL;

    // Calculate normals for all polygons
    SbVec3f zeroVec(0,0,0);
    for(i = 0; i < numPoints; i++) {
	sum = faceNormals[i];

	// This vertex is part of a degenerate face if its normal is
	// (mostly) the same as the zero vector.
	// We use a fixed tolerance for normals (suggested by Tim Wiegand)
	// since normals are unit length
	SbBool isDegenerate = equal(zeroVec, sum, 1.e-4);

	// Smooth normals if face normals are within crease angle
	for (j = indirect[i]; j != i; j = indirect[j]) {

	    // If this vertex is part of a degenerate face, we always
	    // want to smooth to get the normal:
	    if (isDegenerate ||
		faceNormals[i].dot(faceNormals[j]) > cosCreaseAngle) 
		sum += faceNormals[j];
	}
	sum.normalize();
	vertNormals[i] = sum;
    }

    delete [] hashTable;
    delete [] hashNext;
    delete [] indirect;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Allows shape to change number of normals after generation
//
// Use: public

void
SoNormalGenerator::setNumNormals(int newNum)
//
////////////////////////////////////////////////////////////////////////
{   
    if (newNum > numVertNormals) setNormal(newNum, SbVec3f(0,0,0));
    else if (newNum < numVertNormals) numVertNormals = newNum;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Allows shape to change or rearrange normals after generation.
//
// Use: public

void
SoNormalGenerator::setNormal(int32_t index, const SbVec3f &newNormal)
//
////////////////////////////////////////////////////////////////////////
{   
    // Make sure there's enough room for the new normal
    if (index >= numVertNormals) {
	int32_t    newNumVertNormals = numVertNormals;
	
	if (newNumVertNormals <= 0) newNumVertNormals = index + 1;
		
	while (index >= newNumVertNormals)
	    newNumVertNormals *= 2;

	SbVec3f *newVertNormals = new SbVec3f [newNumVertNormals];
	bcopy(vertNormals, newVertNormals,
	      (int) (numVertNormals * sizeof(SbVec3f)));
	if (vertNormals != faceNormals)
	    delete [] vertNormals;
	vertNormals    = newVertNormals;
	numVertNormals = newNumVertNormals;
    }

    // Store new normal
    vertNormals[index] = newNormal;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if the two points are the same within given
//    tolerance.
//
// Use: public

SbBool
SoNormalGenerator::equal(const SbVec3f &a, const SbVec3f &b, float tolerance)
//
////////////////////////////////////////////////////////////////////////
{
    float	diff;

    diff = a[0] - b[0];
    if ((diff < 0.0 ? -diff : diff) > tolerance)
	return FALSE;

    diff = a[1] - b[1];
    if ((diff < 0.0 ? -diff : diff) > tolerance)
	return FALSE;

    diff = a[2] - b[2];
    if ((diff < 0.0 ? -diff : diff) > tolerance)
	return FALSE;

    return TRUE;
}
