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
// Simple class that encapsulates some information about a quad-mesh
//
#include <Inventor/SbLinear.h>
#include <Inventor/SbPList.h>

class SoNode;
class SoCoordinate3;
class SoNormal;
class SoMaterial;
class SoMaterialBinding;
class SoTexture2;
class SoTextureCoordinate2;
class SoTextureCoordinateBinding;
class SoQuadMesh;
class SoSeparator;
class SoCallbackAction;

class QuadThing
{
  public:

    //
    // Create a quad-mesh from another quad-mesh
    //
    QuadThing(const QuadThing *);

    //
    // Create a quad-mesh with the given dimensions and vertices.
    //
    QuadThing(int nx, int ny, const SbVec3f *verts);

    //
    // Read in a scene graph from a file and fill in the QuadThing
    // appropriately.  If there is a problem reading the file (it
    // doesn't contain a QuadMesh, for example) this returns FALSE.
    //
    QuadThing(const char *filename);

    ~QuadThing();

    //
    // Interpolate between two things.  This also figures out new
    // normals.
    //
    void interp(const QuadThing *from, const QuadThing *to, float time);

    //
    // Get the scene graph representation for this QuadThing.  If the
    // QuadThing is later changed (via setVertices or interp) this
    // scene graph is automatically changed, too.  This scene graph
    // does not include materials or textures (so GL tricks can be
    // used to interpolate them).
    //
    SoSeparator *getSceneGraph();

    //
    // Get the materials node.  Returns NULL if there were no
    // materials defined.
    //
    SoMaterial *getMaterial();
    SoMaterialBinding *getMatBinding();

    //
    // Get the texture and texture coordinates.  Again, returns NULL
    // if there are none.
    //
    SoTexture2 *getTexture2();
    SoTextureCoordinate2 *getTexCoord();
    SoTextureCoordinateBinding *getTexBinding();

  private:
    SoSeparator *sceneGraph;	// Scene graph components
    SoCoordinate3 *coords;
    SoNormal *norms;
    SoQuadMesh *qmesh;

    SoMaterial *materials;
    SoMaterialBinding *matbind;

    SoTexture2 *texture;
    SoTextureCoordinate2 *texcoords;
    SoTextureCoordinateBinding *texbind;

    void createSceneGraph();
    void figureNormals();
    void averageNormals(SbVec3f *, int row, int column);
    void newell4(SbVec3f &n, int v1, int v2, int v3, int v4);
    void scaleBBox();
};

class QuadThingList : public SbPList
{
  public:
    void	append(QuadThing *q)
	{ ((SbPList *) this)->append((void *) q); }

    // Inserts given integer in list before integer with given index
    void	insert(QuadThing *q, int addBefore)
	{ ((SbPList *) this)->insert((void *) q,
		addBefore); }

    QuadThing *   operator [](int i) const
	{ return ( (QuadThing *) ( (*(const SbPList *) this) [i] ) ); }
};
    
