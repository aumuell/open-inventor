#ifndef _FACES_
#define _FACES_

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
// A convenient representation for faces
//

#include <Inventor/misc/SoBasic.h>
#include <Inventor/SbPList.h>

class EdgeDict;
class SbVec3f;
class SoNormal;
class SoIndexedFaceSet;
class SoMFVec3f;

class Face
{
  public:
    int nv;	// Number of vertices
    int32_t *v;	// Pointers to indices, which are vertices...
    int32_t *vn;	// Indices for vertex normals
    int	vct;	// count of the first vertex in the original IFS node
    int vidx;	// index of the first vertex in the original IFS node
    int degenerate;	// Is it degenerate? (set by findNormal)
    int body;	// which body does it belong to?

    SbVec3f normal;	// Face normal for the whole face

    enum Orientation {
	UNKNOWN,
	CW,
	CCW
    };
    Orientation orientation;

    void findNormal(const SbVec3f *verts);
    void orientFace(int, int);

};

class FaceList : public SbPList
{
  public:
    FaceList();
    FaceList(const SbVec3f *, SoIndexedFaceSet *, SbBool verbose=FALSE);
    ~FaceList();

    void append(Face *f);

    Face * operator[](int i) const
        { return (Face *)SbPList::operator[](i);}

    void findOrientation();
    void correctOrientation();
    void findFacetNormals(SoNormal *);
    void findVertexNormals(SoNormal *, SoIndexedFaceSet *, float);

    void findShapeInfo();	// sets isSolid() and isConvex() appropriately
    int isSolid() { return solid; }
    int isConvex() { return convex; }

    int findBodies();		// sets faces' body field.  returns # bodies
    void extractBody(int b,
		int &nFaces, int32_t *fNewFromOld, int32_t *fOldFromNew,
		int &nVertices, int32_t *vNewFromOld, int32_t *vOldFromNew);
    SoIndexedFaceSet	*getFaceSet() { return faceSet; }

    void setVerbose(SbBool v) { verbose = v; }
    SbBool isVerbose() const { return verbose; }


  private:
    FaceList(const SbVec3f *, EdgeDict *);
    float volume();
    void reverseOrientation();	// reverse orientation of all on list
    void recursivelyOrient(Face *);
    void recursivelyMarkBody(Face *);
    void orientOutward();
    void averageNormals(SoMFVec3f &, SbVec3f &, float, int);
    void buildVertexDict();

    int convex;
    int solid;
    const SbVec3f *verts;
    EdgeDict *ed;
    FaceList	*vd;	// vertex dictionary
    int		vdSize;
    SoIndexedFaceSet *faceSet;

    SbBool verbose;

    static int getIdx(SoMFVec3f &mf, const SbVec3f &p);
};

//
// v2 vs v3.1 compiler hacks
//
#ifndef INVENTOR1
typedef Face::Orientation	FaceOrientation; 	// v3.1
#else
typedef Orientation		FaceOrientation;	// v2
#endif

#endif
