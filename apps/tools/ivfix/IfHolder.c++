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

#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoIndexedShape.h>
#include <Inventor/nodes/SoIndexedTriangleStripSet.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTextureCoordinate2.h>
#include <Inventor/nodes/SoTextureCoordinateBinding.h>

#include "IfHolder.h"

/////////////////////////////////////////////////////////////////////////////
//
// Constructor.
//
/////////////////////////////////////////////////////////////////////////////

IfHolder::IfHolder(SoNode *graphRoot, SbBool _doStrips,
		   SbBool _doNormals, SbBool _doTexCoords)
{
    origRoot = graphRoot;
    origRoot->ref();

    doStrips    = _doStrips;
    doNormals   = _doNormals;
    doTexCoords = _doTexCoords;

    root = new SoSeparator(7);
    root->ref();

    coords = new SoCoordinate3;
    root->addChild(coords);

    materialBinding = new SoMaterialBinding;
    root->addChild(materialBinding);

    if (doNormals) {
	normals = new SoNormal;
	normalBinding = new SoNormalBinding;
	root->addChild(normals);
	root->addChild(normalBinding);
    }
    else {
	normals = NULL;
	normalBinding = NULL;
    }

    if (doTexCoords) {
	texCoords = new SoTextureCoordinate2;
	texCoordBinding = new SoTextureCoordinateBinding;
	root->addChild(texCoords);
	root->addChild(texCoordBinding);
    }
    else {
	texCoords = NULL;
	texCoordBinding = NULL;
    }

    if (doStrips) {
	stripSet = new SoIndexedTriangleStripSet;
	faceSet = NULL;
	triSet = stripSet;
    }
    else {
	stripSet = NULL;
	faceSet = new SoIndexedFaceSet;
	triSet = faceSet;
    }

    root->addChild(triSet);
}

/////////////////////////////////////////////////////////////////////////////
//
// Destructor.
//
/////////////////////////////////////////////////////////////////////////////

IfHolder::~IfHolder()
{
    origRoot->unref();
    root->unref();
}

/////////////////////////////////////////////////////////////////////////////
//
// Converts the scene graph to use an SoVertexProperty node for
// the properties. The given material is used in case the
// materials need to be copied into the SoVertexProperty node.
//
/////////////////////////////////////////////////////////////////////////////

void
IfHolder::convertToVertexProperty(SoMaterial *mtl)
{
    // Create a new SoVertexProperty node
    SoVertexProperty *vp = new SoVertexProperty;
    vp->ref();

    // Copy the field values from the property nodes 
    vp->vertex = coords->point;
    if (doNormals) {
	vp->normal = normals->vector;
	vp->normalBinding = normalBinding->value;
    }
    if (doTexCoords)
	vp->texCoord = texCoords->point;
    vp->materialBinding = materialBinding->value;

    // Copy the materials only if there are materials and they are not overall
    if (mtl != NULL &&
	materialBinding->value.getValue() != SoMaterialBinding::OVERALL) {

	// Need to pack the diffuse colors and transparency values
	// into the orderedRGBA field
	int num  = mtl->diffuseColor.getNum();
	int numT = mtl->transparency.getNum();
	vp->orderedRGBA.setNum(num);
	uint32_t *rgba = vp->orderedRGBA.startEditing();
	const SbColor *d = mtl->diffuseColor.getValues(0);
	const float   *t = mtl->transparency.getValues(0);
	for (int i = 0; i < num; i++)
	    rgba[i] = d[i].getPackedValue(numT == 1 ? t[0] : t[i]);
	vp->orderedRGBA.finishEditing();
    }

    // Store the SoVertexProperty node in the shape
    triSet->vertexProperty = vp;
    vp->unref();

    // Remove the old property nodes from the graph, leaving just the
    // shape
    triSet->ref();
    root->removeAllChildren();
    root->addChild(triSet);
    triSet->unref();
}
