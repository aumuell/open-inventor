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
// Simple class that encapsulates some information about a quad-mesh.
// Used by the qmorf program and by the little programs that generate
// quad-mesh objects.
//

#include <stdlib.h>
#include <assert.h>

#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoQuadMesh.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoTextureCoordinateBinding.h>
#include <Inventor/nodes/SoTextureCoordinate2.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoSearchAction.h>

#include "QuadThing.h"

QuadThing::QuadThing(const QuadThing *from)
{
    createSceneGraph();
    
    interp(from, from, 0.0);
}

QuadThing::QuadThing(int nx, int ny, const SbVec3f *verts)
{
    createSceneGraph();

    coords->point.setValues(0, nx*ny, verts);
    qmesh->verticesPerRow.setValue(nx);
    qmesh->verticesPerColumn.setValue(ny);

    figureNormals();
}

static SoNode *
searchLastType(SoPath *p, SoType t)
{
    SoSearchAction sa;
    sa.setSearchingAll(TRUE);
    sa.setType(t);
    sa.setInterest(SoSearchAction::LAST);
    sa.apply(p);
    SoPath *outPath = sa.getPath();

    SoNode *result = NULL;
    if (outPath != NULL && (outPath->getLength() > 0) )
        result = outPath->getTail();

    return result; 
}
    

//
// Read in a scene graph and extract the information out of it needed
// to create a QuadThing.  This also uniformly scales the object to
// reside in a -1 to 1 bounding box.
//
QuadThing::QuadThing(const char *filename)
{
    //
    // Need to extract:
    // -- first QuadMesh; need its x,y size.
    // -- coordinates
    // -- materials
    // -- material binding
    // -- textures
    // -- texture coordinates
    // -- texture binding
    //
    // Note: these are only set to NULL in this constructor because
    // this is the only constructor that may fail to create them (all
    // the other constructors create them in the createSceneGraph
    // routine)
    //
    sceneGraph = NULL;
    coords = NULL;
    norms = NULL;
    qmesh = NULL;
    materials = NULL;
    matbind = NULL;
    texture = NULL;
    texcoords = NULL;
    texbind = NULL;

    SoInput in;
    if (!in.openFile(filename)) {
	fprintf(stderr, "Couldn't open %s\n", filename);
	exit(1);
    }
    SoSeparator *graph = SoDB::readAll(&in);
    in.closeFile();
    
    if (graph == NULL) return;
    graph->ref();

    //
    // Search for first mesh
    //
    SoSearchAction sa;
    sa.setInterest(SoSearchAction::FIRST);
    sa.setType(SoQuadMesh::getClassTypeId());
    sa.apply(graph);

    SoPath *pathToMesh = sa.getPath();
    if (pathToMesh == NULL)
    {
	fprintf(stderr, "Error reading qmesh from %s\n", filename);
	return;
    }

    pathToMesh->ref();
    SoQuadMesh *q = (SoQuadMesh *)pathToMesh->getTail();

    //
    // If there is a VertexProperty node, make a new separator containing
    // nodes with that information, and put it in the scene graph.
    //
    if (q->vertexProperty.getValue() != NULL)
    {
      SoVertexProperty *vp = (SoVertexProperty*)q->vertexProperty.getValue();
      SoSeparator *quadsep = new SoSeparator;
      int numVerts = vp->vertex.getNum();
      if (numVerts > 0)
      {
        SoCoordinate3 *newcoord = new SoCoordinate3;
        newcoord->point.setValues(0,numVerts,vp->vertex.getValues(0));
        quadsep->addChild(newcoord);
      }
      int numNormals = vp->normal.getNum();
      if (numNormals > 0)
      {
        SoNormal *newnormal = new SoNormal;
        newnormal->vector.setValues(0,numNormals,vp->normal.getValues(0));
        quadsep->addChild(newnormal);
      }
      int numTexCoords = vp->texCoord.getNum();
      if (numTexCoords > 0)
      {
        SoTextureCoordinate2 *newtexcoord = new SoTextureCoordinate2;
        newtexcoord->point.setValues(0,numTexCoords,vp->texCoord.getValues(0));
        quadsep->addChild(newtexcoord);
      }
      int numColors = vp->orderedRGBA.getNum();
      if (numColors > 0)
      {
        float transparency; SbColor color;
        SoMaterial *newmat = new SoMaterial;
        for (int i=0; i<numColors; i++)
        {
          color.setPackedValue(vp->orderedRGBA[i],transparency);
          newmat->diffuseColor.set1Value(i,color);
          newmat->transparency.set1Value(i,transparency);
        }
        quadsep->addChild(newmat);
      }
      q->vertexProperty.setValue(NULL);
      quadsep->addChild(q);
      ((SoGroup*)(pathToMesh->getNodeFromTail(1)))->replaceChild(q, quadsep);
      pathToMesh->unref();

      sa.apply(graph);
      pathToMesh = sa.getPath();
      if (pathToMesh == NULL)
      {
	 fprintf(stderr, "Error reading qmesh from %s\n", filename);
	 return;
      }
      pathToMesh->ref();
      q = (SoQuadMesh *)pathToMesh->getTail();
    }
    

    createSceneGraph();

    sceneGraph->replaceChild(qmesh, q);
    qmesh = q;

    qmesh->verticesPerRow.setValue(q->verticesPerRow.getValue());
    qmesh->verticesPerColumn.setValue(q->verticesPerColumn.getValue());

    //
    // Search for coordinates.  We look for the last coordinates on
    // the path to the mesh, and assume that the mesh will use those
    // coordinates.  This isn't quite correct; the ignore flag could
    // be set on the coordinate's point field.  We should check for
    // that and search further up the path, but for simplicity I just
    // assume that the ignore flag isn't set (certainly true for the
    // data sets I'm likely to throw at this!).
    //

    SoCoordinate3 *c = (SoCoordinate3 *)
	searchLastType(pathToMesh,
			       SoCoordinate3::getClassTypeId());
    if (c == NULL)
    {
	fprintf(stderr, "Could not find coordinates in %s!\n",
		filename);
	exit(1);
    }
    sceneGraph->replaceChild(coords, c);
    coords = c;

    //
    // And other stuff...  Again, note that the results may not be
    // correct if the ignore flag is set on any of this stuff.  Hi ho.
    //

    materials = (SoMaterial *)
	searchLastType(pathToMesh,
			       SoMaterial::getClassTypeId());
    if (materials != NULL)
    {
	materials->ref();
    }

    matbind = (SoMaterialBinding *)
	searchLastType(pathToMesh,
			       SoMaterialBinding::getClassTypeId());
    if (matbind != NULL)
    {
	matbind->ref();
    }
    texture = (SoTexture2 *)
	searchLastType(pathToMesh,
			       SoTexture2::getClassTypeId());
    if (texture != NULL)
    {
	texture->ref();
    }
    texcoords = (SoTextureCoordinate2 *)
	searchLastType(pathToMesh,
			       SoTextureCoordinate2::getClassTypeId());
    if (texcoords != NULL)
    {
	texcoords->ref();
    }
    texbind = (SoTextureCoordinateBinding *)
	searchLastType(pathToMesh,
			       SoTextureCoordinateBinding::getClassTypeId());
    if (texbind != NULL)
    {
	texbind->ref();
    }

    pathToMesh->unref();
    graph->unref();	// No longer needed

    figureNormals();
}

QuadThing::~QuadThing()
{
    //
    // Delete any of the stuff we might have found
    //
    if (sceneGraph != NULL) sceneGraph->unref();
    if (materials != NULL) materials->unref();
    if (matbind != NULL) matbind->unref();
    if (texture != NULL) texture->unref();
    if (texcoords != NULL) texcoords->unref();
    if (texbind != NULL) texbind->unref();
}

//
// Create the scene graph.  Used by the constructors.
//
void
QuadThing::createSceneGraph()
{
    sceneGraph = new SoSeparator;
    sceneGraph->ref();

    coords = new SoCoordinate3;
    sceneGraph->addChild(coords);

    norms = new SoNormal;
    sceneGraph->addChild(norms);

    SoNormalBinding *nb = new SoNormalBinding;
    nb->value = SoNormalBinding::PER_VERTEX_INDEXED;
    sceneGraph->addChild(nb);

    qmesh = new SoQuadMesh;
    sceneGraph->addChild(qmesh);
}

#define MAX(a,b) (a > b ? a : b)

//
// Modify the vertices so the bounding boxes of all of the objects are
// the same
//
void
QuadThing::scaleBBox()
{
    SoGetBoundingBoxAction bba(SbVec2s(1,1));
    bba.apply(sceneGraph);
    SbBox3f bbox = bba.getBoundingBox();

    SbVec3f center = bbox.getCenter();
    float x, y, z, scale;
    bbox.getSize(x, y, z);
    scale = 2.0 / MAX( x, MAX(y,z) );

    SbVec3f *vertices = coords->point.startEditing();

    // Now modify vertices...
    int nverts = (int) (qmesh->verticesPerColumn.getValue() *
	qmesh->verticesPerRow.getValue());
    for (int i = 0; i < nverts; i++)
    {
	vertices[i] = (vertices[i]-center)*scale;
    }
    coords->point.finishEditing();
}

//
// Newell's formula for the normal of a polygon-- this version is
// hardwired for quads.  n will be close to zero if the quad is
// degenerate.  This does NOT normalize the result.
//
void
QuadThing::newell4(SbVec3f &n, int v1, int v2, int v3, int v4)
{
    const SbVec3f *v = coords->point.getValues(0);
    n[0] = (v[v1][1] - v[v2][1])*(v[v1][2] + v[v2][2]);
    n[1] = (v[v1][2] - v[v2][2])*(v[v1][0] + v[v2][0]);
    n[2] = (v[v1][0] - v[v2][0])*(v[v1][1] + v[v2][1]);

    n[0] += (v[v2][1] - v[v3][1])*(v[v2][2] + v[v3][2]);
    n[1] += (v[v2][2] - v[v3][2])*(v[v2][0] + v[v3][0]);
    n[2] += (v[v2][0] - v[v3][0])*(v[v2][1] + v[v3][1]);

    n[0] += (v[v3][1] - v[v4][1])*(v[v3][2] + v[v4][2]);
    n[1] += (v[v3][2] - v[v4][2])*(v[v3][0] + v[v4][0]);
    n[2] += (v[v3][0] - v[v4][0])*(v[v3][1] + v[v4][1]);

    n[0] += (v[v4][1] - v[v1][1])*(v[v4][2] + v[v1][2]);
    n[1] += (v[v4][2] - v[v1][2])*(v[v4][0] + v[v1][0]);
    n[2] += (v[v4][0] - v[v1][0])*(v[v4][1] + v[v1][1]);
}

//
// Average the normals of the faces around the vertex at the
// given row, column to figure out its normal.  This actually weights
// the polygons' contributions by their area.
//
void
QuadThing::averageNormals(SbVec3f *normals, int r, int c)
{
    int numx = (int)qmesh->verticesPerRow.getValue();

    int i = r*numx+c;

    SbVec3f n;

    newell4(n, i-numx-1, i-1, i, i-numx);
    normals[i] = n;
    newell4(n, i-numx, i, i+1,  i-numx+1);
    normals[i] += n;
    newell4(n, i, i+numx, i+numx+1,  i+1);
    normals[i] += n;
    newell4(n, i-1, i+numx-1, i+numx,  i);
    normals[i] += n;

    float d = normals[i].length();
    if (d > 0.00001) normals[i] /= d;
    else normals[i].setValue(0.0, 0.0, 1.0);
}

//
// Figure out the normals for this thing.  This uses Newell's formula
// on the 4 vertices surrounding each vertex.  This will be fast and
// fairly robust (edges are handled by using the vertex itself, so
// corners get calculated correctly).  If the polygon is degenerate,
// an arbitrary normal is assigned to the vertex.
//
void
QuadThing::figureNormals()
{
    int row, column;

    int numx = (int)qmesh->verticesPerRow.getValue();
    int numy = (int)qmesh->verticesPerColumn.getValue();

    int nn = norms->vector.getNum();
    if (nn < numx*numy)
    {
	norms->vector.insertSpace(0, numx*numy-nn);
    }

    SbVec3f *normals = norms->vector.startEditing();

    //
    // Do interior vertices.  We take the normal of the plane through
    // the neighboring vertices for speed.
    //
    for (row = 1; row < numy-1; row++)
    {
	for (column = 1; column < numx-1; column++)
	{
	    int i = row*numx+column;
	    newell4(normals[i], i-numx, i-1, i+numx, i+1);

	    //
	    // Normalize; if using the neighboring points failed for
	    // any reason, average the normals of the surrounding
	    // faces:
	    // 
	    float d = normals[i].length();
	    if (d > 0.00001) normals[i] /= d;
	    else averageNormals(normals, row, column);
	}
    }
    //
    // For the border quads, average the normals of surrounding faces.
    // First, bottom and top:
    //
    SbVec3f lastBot(0.0, 0.0, 0.0);
    SbVec3f lastTop(0.0, 0.0, 0.0);
    int i;
    for (i = 0; i < numx; i++)
    {
	int top = (numy-1)*numx+i;
	normals[i] = lastBot;
	normals[top] = lastTop;

	if (i+1 < numx)
	{
	    newell4(lastBot, i, i+numx, i+numx+1,  i+1);
	    newell4(lastTop, top-numx, top, top+1,  top-numx+1);

	    normals[i] += lastBot;
	    normals[top] += lastTop;
	}
	float d = normals[i].length();
	if (d > 0.00001) normals[i] /= d;
	else normals[i].setValue(0.0, 0.0, 1.0);
	d = normals[top].length();
	if (d > 0.00001) normals[top] /= d;
	else normals[top].setValue(0.0, 0.0, 1.0);
    }
    //
    // Next, left and right sides.  There are no special cases
    // because the corners were done in the previous loop.
    //
    SbVec3f lastLeft;
    SbVec3f lastRight;
    newell4(lastLeft, 0, numx, numx+1,  1);
    newell4(lastRight, numx-2, 2*numx-2, 2*numx-1,  numx-1);
    for (i = 1; i < numy-1; i++)
    {
	int left = i*numx;
	int right = left+numx-1;
	normals[left] = lastLeft;
	normals[right] = lastRight;

	newell4(lastLeft, left, left+numx, left+numx+1,  left+1);
	newell4(lastRight, right-1, right+numx-1, right+numx,  right);

	normals[left] += lastLeft;
	normals[right] += lastRight;

	float d = normals[left].length();
	if (d > 0.00001) normals[left] /= d;
	else normals[left].setValue(0.0, 0.0, 1.0);
	d = normals[right].length();
	if (d > 0.00001) normals[right] /= d;
	else normals[right].setValue(0.0, 0.0, 1.0);
    }
    norms->vector.finishEditing();
}
    
//
// Interpolate between two things.  This does simple linear interpolation.
//
void
QuadThing::interp(const QuadThing *from, const QuadThing *to, float time)
{
    int fnx = (int)from->qmesh->verticesPerRow.getValue();
    int fny = (int)from->qmesh->verticesPerColumn.getValue();
    int tnx = (int)to->qmesh->verticesPerRow.getValue();
    int tny = (int)to->qmesh->verticesPerColumn.getValue();

    int nx = fnx > tnx ? fnx : tnx;
    int ny = fny > tny ? fny : tny;

    int nv = coords->point.getNum();
    if (nv < nx*ny)
    {
	coords->point.insertSpace(0, (int)(nx*ny-nv));
    }

    qmesh->verticesPerRow.setValue(nx);
    qmesh->verticesPerColumn.setValue(ny);

    SbVec3f *vertices = coords->point.startEditing();
    const SbVec3f *f_vertices = from->coords->point.getValues(0);
    const SbVec3f *t_vertices = to->coords->point.getValues(0);

    // Use linear interpolation for now
    for (int row = 0; row < ny; row++)
    {
	int from_row = (row * fny) / ny;
	int to_row = (row * tny) / ny;
	for (int col = 0; col < nx; col++)
	{
	    int i = row*nx+col;
	    int from_i = from_row*fnx+col;
	    int to_i = to_row*tnx+col;
	    vertices[i] = f_vertices[from_i] * (1.0 - time) +
		t_vertices[to_i] * time;
	}
    }
    coords->point.finishEditing();
    figureNormals();
}

//
// Some access routines.
//
SoSeparator *
QuadThing::getSceneGraph()
{
    return sceneGraph;
}

SoMaterial *
QuadThing::getMaterial()
{
    return materials;
}

SoMaterialBinding *
QuadThing::getMatBinding()
{
    return matbind;
}

SoTexture2 *
QuadThing::getTexture2()
{
    return texture;
}

SoTextureCoordinate2 *
QuadThing::getTexCoord()
{
    return texcoords;
}

SoTextureCoordinateBinding *
QuadThing::getTexBinding()
{
    return texbind;
}
