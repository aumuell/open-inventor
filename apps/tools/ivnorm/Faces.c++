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

//
// Some convenient routines for doing stuff with faces.
//

#include <assert.h>
#include <math.h>
#include "Faces.h"
#include "Edges.h"

#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>

//
// Find a face's normal, assuming its vertices are in
// counter-clockwise order.
//
void
Face::findNormal(const SbVec3f *verts)
{
    // Use Newell's method to find face normal.  See Newman & Sproull,
    // pg. 499.  This is better than the three-point cross-product
    // method.

    normal[0] = normal[1] = normal[2] = 0.0;

    for (int i = 0; i < nv; i++)
    {
	int iv1 = int(v[i]);
	int iv2 = int(v[(i+1)%nv]);

	if (iv1 == iv2) continue;

	SbVec3f v1 = verts[iv1];
	SbVec3f v2 = verts[iv2];

	normal[0] += (v1[1] - v2[1])*(v1[2] + v2[2]);
	normal[1] += (v1[2] - v2[2])*(v1[0] + v2[0]);
	normal[2] += (v1[0] - v2[0])*(v1[1] + v2[1]);
    }
    if (orientation == Face::CW) normal.negate();

    if (normal.length() < 0.00001)
    {
	degenerate = 1;
    }
    else
    {
	normal.normalize();
	degenerate = 0;
    }
}

//
// Set this face's orientation relative to a given edge
//
void
Face::orientFace(int v1, int v2)
{
    int v1_i;
    
    // First, figure out which index is v1
    
    for (v1_i = 0; v1_i < nv; v1_i++)
    {
	if (v[v1_i] == v1) 
	{
	    // Now just have to determine whether v2 is the next or previous
	    // vertex.
	    if (v[(v1_i+1)%nv] == v2) // Next
	    {
		orientation = CCW;
		break;
	    }
	    else if (v[(v1_i+nv-1)%nv] == v2) // Previous
	    {
		orientation = CW;
		break;
	    }
	    // Otherwise, keep on checking; we may be traversing a
	    // degenerate edge or strange facet where vertices are repeated.
	}
    }
    assert(v1_i != nv);	// Assertion: we found it.
}

FaceList::FaceList()
{
    verts = NULL;
    faceSet = NULL;
    ed = NULL;
    vd = NULL;
    convex = TRUE;
    solid = TRUE;
    verbose = FALSE;
}

FaceList::FaceList(const SbVec3f *v, EdgeDict *e)
{
    verts = v;
    ed = e;
    vd = NULL;
    faceSet = NULL;
    convex = TRUE;
    solid = TRUE;
    verbose = FALSE;
}

FaceList::FaceList(const SbVec3f *v, SoIndexedFaceSet *fs, SbBool vrb)
{
    verts = v;
    faceSet = fs; fs->ref();
    convex = TRUE;
    solid = TRUE;
    vd = NULL;
    verbose = vrb;
    
    ed = new EdgeDict(1000);

    SoMFInt32 *coord_indices = &faceSet->coordIndex;
    int ni = coord_indices->getNum();

    int32_t *indices = coord_indices->startEditing();

    int start_o_face = 0;

    // Now fill in face and edge structures
    int n_degenerate_faces = 0;
    int n_degenerate_edges = 0;
    for (int i = 0, ct=0; i < coord_indices->getNum(); i++)
    {
	if (indices[i] == SO_END_FACE_INDEX
	    || i == coord_indices->getNum()-1)
	{
	    Face *f = new Face;
	    if (indices[i] == SO_END_FACE_INDEX)
		f->nv = i - start_o_face;
	    else
		f->nv = i - start_o_face + 1;
	    f->v = indices+start_o_face;
	    f->vct = ct;	// first vertex index count for this face.
	    f->vidx = start_o_face;
	    f->vn = NULL;
	    f->orientation = Face::UNKNOWN;
	    
	    f->findNormal(verts);
	    if (f->degenerate)
	    {
		++n_degenerate_faces;
		f->orientation = Face::CCW;
	    }
	    else for (int j = 0; j < f->nv; j++)
	    {
		int i1 = (int)f->v[j];
		int i2 = (int)f->v[(j+1)%f->nv];
		if (i1 != i2)
		    ed->Add(f, i1, i2);
		else ++n_degenerate_edges;
	    }

	    start_o_face = i+1;

	    append(f);
	}
	else
	    ct++;	// "ct" counts the non-END entries, for use
			// as the index into a per_vertex normal set
    }
    if (n_degenerate_faces != 0)
    {
	if (verbose) fprintf(stderr, "Detected %d degenerate faces\n",
		n_degenerate_faces);
    }
    if (n_degenerate_edges != 0)
    {
	if (verbose) fprintf(stderr, "Detected %d degenerate edges\n",
		n_degenerate_edges);
    }
}

void
FaceList::append(Face *f)
{
    SbPList::append((void *)f);
}

//
// This isn't accurate; we just use it to figure out if the volume is
// positive or negative to try to figure out if the surface is
// oriented correctly.
//
float
FaceList::volume()
{
    int i, j;

    int total_v = 0;
    SbVec3f average(0.0, 0.0, 0.0);

    for (j = 0; j < getLength(); j++)
    {
	Face *f = (*this)[j];
	if (f->degenerate) continue;

	for (i = 0; i < f->nv; i++)
	{
	    average += verts[f->v[i]];
	    ++total_v;
	}
    }
    average /= (float) total_v;

    float result = 0.0;

    for (j = 0; j < getLength(); j++)
    {
	Face *f = (*this)[j];
	if (f->degenerate) continue;

	for (i = 1; i < f->nv-1; i++)
	{
	    SbVec3f v1 = verts[f->v[0]] - average;
	    SbVec3f v2 = verts[f->v[i]] - average;
	    SbVec3f v3 = verts[f->v[i+1]] - average;
	    
	    float t = (v1.cross(v2)).dot(v3);
	    if (f->orientation == Face::CCW)
	    {
		result += t;
	    }
	    else if (f->orientation == Face::CW)
	    {
		result -= t;
	    }
	    else
	    {
		assert(0);
	    }
	}
    }
    return result;
}

FaceList::~FaceList()
{
    if (faceSet != NULL)
    {
	delete ed;
	faceSet->coordIndex.finishEditing();
	faceSet->unref();
	for (int i = 0; i < getLength(); i++)
	{
	    delete (*this)[i];
	}
	truncate(0);
    }

    if (vd)
	delete [] vd;
}

void
FaceList::reverseOrientation()
{
    for (int i = 0; i < getLength(); i++)
    {
	Face *f = (*this)[i];

	if (f->orientation == Face::CW)
	{
	    f->orientation = Face::CCW;
	}
	else if (f->orientation == Face::CCW)
	{
	    f->orientation = Face::CW;
	}
	else
	{
	    assert(0);
	}
    }
}

//
// This routine works by starting with a 'seed' face and assumes that
// its orientation is correct.  It then visits all neighboring faces
// and makes their orientations consistent with the seed face's.
//
void
FaceList::findOrientation()
{
    orientOutward();

    correctOrientation();
}

void
FaceList::findFacetNormals(SoNormal *n)
{
    assert(n != NULL && faceSet != NULL);

    for (int i = 0; i < getLength(); i++)
    {
	(*this)[i]->findNormal(verts);
	n->vector.set1Value(i, (*this)[i]->normal);
    }
}

//
// Assuming that the correct orientation of a 'seed' face has been
// discovered, this routine figures out the correct orientation for
// all faces connected to that face.
//
void
FaceList::recursivelyOrient(Face *seed)
{
    int i, j;
    FaceList others;

    if (seed->degenerate) return;

    for (i = 0; i < seed->nv; i++)
    {
	j = (i+1)%seed->nv;

	// Find other faces attached to this edge
	ed->OtherFaces(seed, seed->v[i], seed->v[j], others);

	for (int f = 0; f < others.getLength(); f++)
	{
	    if (others[f]->orientation == Face::UNKNOWN)
	    {
		if (seed->orientation == Face::CW)
		    others[f]->orientFace((int)seed->v[i], (int)seed->v[j]);
		else if (seed->orientation == Face::CCW)
		    others[f]->orientFace((int)seed->v[j], (int)seed->v[i]);
		else assert(0);	// Should never happen

		append(others[f]);
		recursivelyOrient(others[f]);
	    }
	}
    }
}

//
// This routine takes a collection of faces and trys to figure out
// which way is out.
//
void
FaceList::orientOutward()
{
    // int num_fragments = 0;

    //
    // Loop through all the faces; if we find one whose orientation
    // hasn't been determined, will try to determine its orientation.
    //
    for (int i = 0; i < getLength(); i++)
    {
	Face *f = (*this)[i];
	if (f->orientation != Face::UNKNOWN) continue;

	// First, take a wild guess...
	f->orientation = Face::CCW;

	// ++num_fragments;

	FaceList fragment(verts, ed);
	fragment.append(f);

	// Now recursively orient the faces connected to this face.
	fragment.recursivelyOrient(f);

	// Take a reasonable guess for whether or not that first face
	// is oriented correctly:
	float v = fragment.volume();

	if (v*v < 0.00001*0.00001)	// FLAT
	{
	    // Do something...
	    // fprintf(stderr, "Flat fragment found\n");
	}
	else if (v < 0.0)
	    fragment.reverseOrientation();
    }
    // fprintf(stderr, "There were %d fragments\n", num_fragments);
}

void
FaceList::correctOrientation()
{
    for (int i = 0; i < getLength(); i++)
    {
	Face *f = (*this)[i];
	if (f->orientation == Face::CW)
	{
	    for (int j = 0; j < f->nv/2; j++)
	    {
		int k = f->nv - j - 1;
		int32_t t = f->v[j];
		f->v[j] = f->v[k];
		f->v[k] = t;
	    }
	    f->orientation = Face::CCW;
	}
    }
}

void
FaceList::findShapeInfo()
{
    FaceList others;
    convex = TRUE;
    solid = TRUE;

    for (int k=0; k< getLength(); k++) {

	Face *f = (*this)[k];
	if (f->degenerate)
	    continue;

	// If a face has more than 3 vertices, assume it's convex.
	if (f->nv > 3) {
	    convex = FALSE;
	}

	// run through the edges of the face
	for (int i = 0; i < f->nv; i++) {

	    int j = (i+1)%f->nv;

	    // Find other faces attached to this edge
	    ed->OtherFaces(f, f->v[i], f->v[j], others);

	    if (others.getLength() == 0) {
		solid = FALSE;
		break;
	    }
	}

	// we can quit searching if we've found
	// out all there is to know.
	if (!convex && !solid)
	    break;
    }
}


int
FaceList::findBodies()
{
    buildVertexDict();

    // clear all faces
    int i;
    for (i=0; i< getLength(); i++)
	(*this)[i]->body = -1;

    int bodyN = 0;
    
    for (i=0; i<getLength(); i++) {
	
	Face *f = (*this)[i];

	// each time we find an unmarked face,
	// it's on a previously-undiscovered body
	if (f->body == -1) {
	    f->body = bodyN++;
	    recursivelyMarkBody(f);
	}
    }

    return bodyN;
}

void
FaceList::recursivelyMarkBody(Face *f)
{
    int i;

    assert(f->body != -1);	// only recurse on marked faces

    for (i = 0; i < f->nv; i++)
    {
	// get list of faces sharing this vertex
	const FaceList &faces = vd[f->v[i]];

	for (int j=0; j<faces.getLength(); j++) {
	    if (faces[j]->body == f->body) {
		; // nothing to do if the other face is already part of this body
	    }
	    else if (faces[j]->body == -1) {
		// the other face is still unmarked, so mark it and recurse
		faces[j]->body = f->body;
		recursivelyMarkBody(faces[j]);
	    }
	    else {
		assert(0);	// vertex dictionary isn't commutative??
	    }
	}
    }
}

void
FaceList::extractBody(int b,
		     int &bnf, int32_t *fNewFromOld, int32_t *fOldFromNew,
		     int &bnv, int32_t *vNewFromOld, int32_t *vOldFromNew)
{
    // build the face maps
    int f, bf;
    for (f=0, bf=0; f < getLength(); f++) {
	if ((*this)[f]->body == b) {
	    // f is the old face index
	    // bf is the new face index
	    fNewFromOld[f] = bf;
	    fOldFromNew[bf] = f;
	    bf++;
	}
    }
    bnf = bf;

    // build the vertex maps
    int ni, i;
    for (ni=0, i=0; i < vdSize; i++) {
	const FaceList &faces = vd[i];
	SbBool isInBody = FALSE;
	for (int j = 0; j < faces.getLength(); j++) if (faces[j]->body == b) {
	    isInBody = TRUE;
	    break;
	}
	if (isInBody) {
	    // i is the old vertex index
	    // ni is the new vertex index
	    vNewFromOld[i] = ni;
	    vOldFromNew[ni] = i;
	    ni++;
	}
	else
	    vNewFromOld[i] = -1;
    }
    bnv = ni;
}


void
FaceList::buildVertexDict()
{
    // already built?
    if (vd) return;

    int biggest_index = 0;
    int i;
    for (i = 0; i < getLength(); i++) {
	Face *f = (*this)[i];
	for (int j = 0; j < f->nv; j++) {
	    if (f->v[j] > biggest_index) biggest_index = f->v[j];
	}
    }

    vd = new FaceList[biggest_index+1];
    vdSize = biggest_index+1;

    // Build list of faces around each vertex
    for (i = 0; i < getLength(); i++)
    {
	Face *f = (*this)[i];

	for (int j = 0; j < f->nv; j++)
	{
	    vd[f->v[j]].append(f);
	}
    }

}


void
FaceList::findVertexNormals(SoNormal *norm, SoIndexedFaceSet *ifs,
			    float creaseAngle)
{
    buildVertexDict();

    // Initialize all faces
    int i;
    for (i = 0; i < getLength(); i++)
    {
	Face *f = (*this)[i];

	f->findNormal(verts);
	f->vn = new int32_t[f->nv];
	for (int j = 0; j < f->nv; j++)
	{
	    f->vn[j] = -1;
	}
    }
    
    // Finally, create normals
    norm->vector.deleteValues(0);	// get rid of default value
    int count = 0;
    for (i = 0; i < getLength(); i++)
    {
	Face *f = (*this)[i];

	for (int j = 0; j < f->nv; j++)
	{
	    if (f->degenerate)
	    {
		f->vn[j] = getIdx(norm->vector, SbVec3f(0,0,0));
	    }
	    else if (f->vn[j] == -1)
	    {
		SbVec3f t;
		vd[f->v[j]].averageNormals(norm->vector, f->normal,
					      creaseAngle,
					      f->v[j]);
	    }
	    ifs->normalIndex.set1Value(count, f->vn[j]);
	    ++count;
	}
	ifs->normalIndex.set1Value(count, SO_END_FACE_INDEX);
	++count;
    }
}

//
// get the index of a vector in a SFVec3f;
// adds the vector if needed.
//
// use: private
//

int
FaceList::getIdx(SoMFVec3f &mf, const SbVec3f &p)
{
    int n = mf.getNum();
    SbVec3f *v = mf.startEditing();

    // search from the end, since recent vectors are likely to be reused
    for (int i=n-1; i>=0; i--)
	if (p == v[i]) {
	    mf.finishEditing();
	    return i;
	}

    mf.finishEditing();
    mf.set1Value(n, p);
    return n;
}

//
// Average the normals around a vertex to get a vertex normal.  Skip
// faces that are too different (as defined by creaseAngle)
//
void
FaceList::averageNormals(SoMFVec3f &norms, SbVec3f &reference,
			 float creaseAngle, int whichV)
{
    SbVec3f average;
    average.setValue(0.0, 0.0, 0.0);

    float ca = cos(creaseAngle);

    int num = 0;
    int max = getLength();

    // first, loop through and compute the average
    int i;
    for (i = 0; i < max; i++)
    {
	Face *f = (*this)[i];
	if (f->degenerate) continue;

	float dp = reference.dot(f->normal);
	if (dp >= ca)
	{
	    average += f->normal; ++num;

	}
    }
    assert(num != 0);
    average /= (float)num;
    average.normalize();

    // insert the average into the mfield
    int index = getIdx(norms, average);

    // loop through again and insert the average's index
    // into the face arrays
    for (i = 0; i < max; i++)
    {
	Face *f = (*this)[i];
	if (f->degenerate) continue;

	float dp = reference.dot(f->normal);
	if (dp >= ca)
	{
	    for (int j = 0; j < f->nv; j++)
	    {
		if (f->v[j] == whichV)
		    f->vn[j] = index;
	    }
	}
    }
    assert(num != 0);
    average /= (float)num;
    average.normalize();
}
