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

#include <Inventor/SoLists.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoClipPlane.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoEnvironment.h>
#include <Inventor/nodes/SoFile.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoInfo.h>
#include <Inventor/nodes/SoLOD.h>
#include <Inventor/nodes/SoLabel.h>
#include <Inventor/nodes/SoLevelOfDetail.h>
#include <Inventor/nodes/SoLight.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoProfile.h>
#include <Inventor/nodes/SoProfileCoordinate2.h>
#include <Inventor/nodes/SoShape.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoTexture2Transform.h>
#include <Inventor/nodes/SoTextureCoordinate2.h>
#include <Inventor/nodes/SoTextureCoordinateBinding.h>
#include <Inventor/nodes/SoTransformation.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoWWWAnchor.h>
#include <Inventor/nodes/SoWWWInline.h>

#include "IfAssert.h"
#include "IfCollector.h"
#include "IfShape.h"
#include "IfTypes.h"

/////////////////////////////////////////////////////////////////////////////
//
// Constructor.
//
/////////////////////////////////////////////////////////////////////////////

IfCollector::IfCollector()
{
}

/////////////////////////////////////////////////////////////////////////////
//
// Destructor.
//
/////////////////////////////////////////////////////////////////////////////

IfCollector::~IfCollector()
{
}

/////////////////////////////////////////////////////////////////////////////
//
// Finds all shapes in the given scene, creating a list of IfShape
// instances. The number of shapes is returned.
//
/////////////////////////////////////////////////////////////////////////////

int
IfCollector::collect(SoNode *sceneRoot, IfShape *&shapeList,
		     SbBool doTexCoords)
{
    doingTexCoords = doTexCoords;

    // These are all the types that we consider to be "shapes"; in
    // other words, we do not want to go below these nodes when
    // processing a scene.
    SoTypeList shapeTypes;
    IfTypes::getShapeTypes(&shapeTypes);

    // To find all the shapes in the scene, we apply an
    // SoCallbackAction to the root. We would like to use an
    // SoSearchAction, which is much faster, but we have to search for
    // a variety of different node types, and we don't want to keep
    // searching once we have found one. If we used an SoSearchAction,
    // we would have to apply it once for each type and then somehow
    // merge the returned lists of paths.

    SoCallbackAction cba;
    int i;
    for (i = 0; i < shapeTypes.getLength(); i++)
	cba.addPreCallback(shapeTypes[i], storePathCB, this);
    cba.apply(sceneRoot);

    // Determine the number of shapes
    int numShapes = pathsToShapes.getLength();
    if (numShapes == 0) {
	shapeList = NULL;
	return 0;
    }

    // Allocate an array of IfShape instances of the correct size
    shapeList = new IfShape[numShapes];

    // For each shape, fill in the corresponding IfShape instance
    for (i = 0; i < numShapes; i++)
	collectShape(pathsToShapes[i], &shapeList[i]);

    return numShapes;
}

/////////////////////////////////////////////////////////////////////////////
//
// Stores a path to a shape found in the scene
//
/////////////////////////////////////////////////////////////////////////////

void
IfCollector::storePath(SoCallbackAction *cba)
{
    pathsToShapes.append(cba->getCurPath()->copy());
}

/////////////////////////////////////////////////////////////////////////////
//
// Collects properties for a single shape.
//
/////////////////////////////////////////////////////////////////////////////

void
IfCollector::collectShape(const SoPath *pathToShape, IfShape *shape)
{
    // Find and remember the shape we're looking for. Note that if
    // there is a special group (such as an Array or MultipleCopy) node
    // in the path, we have to consider that to be the shape, since we
    // don't want to ignore its special properties
    currentShape = shape;

    const SoFullPath *fullPath = (const SoFullPath *) pathToShape;
    for (int i = 0; i < fullPath->getLength(); i++) {
	SoNode *node = fullPath->getNode(i);
	SoType type  = node->getTypeId();

	// File, LOD, line- and point-based shape nodes, and some others
	// are handled specially, since we want to make sure they are
	// never flattened
	if (IfTypes::isOpaqueGroupType(type) ||
	    IfTypes::isUnflattenableShape(type)) {
	    ASSERT(currentShape->shape == NULL);
	    currentShape->shape = node;
	    currentShape->shape->ref();
	    currentShape->dontFlatten = TRUE;
	    break;
	}

	// Handle nodes that should be considered shapes:
	if (IfTypes::isShape(type)) {
	    ASSERT(currentShape->shape == NULL);
	    currentShape->shape = node;
	    currentShape->shape->ref();
	    break;
	}
    }

    // Set up a callback to invoke at each node encountered
    SoCallbackAction ca;
    ca.addPreCallback(SoNode::getClassTypeId(), storeNodeCB, this);

    // Examine each node that affects the shape
    ca.apply((SoPath *) pathToShape);

    // If the shape is derived from SoVertexShape and has a non-NULL
    // vertexProperty field, this has to be handled specially
    if (shape->shape->isOfType(SoVertexShape::getClassTypeId())) {
	SoVertexShape *vs = (SoVertexShape *) shape->shape;
	SoVertexProperty *vp =
	    (SoVertexProperty *) vs->vertexProperty.getValue();
	if (vp != NULL) {
	    // Copy the properties from the SoVertexProperty node
	    // into separate property nodes
	    handleVertexProperty(shape, vp);

	    // Create a copy of the shape that does not have an
	    // SoVertexProperty node in it. Otherwise, the primitive
	    // generation code for the shape can get confused.
	    SoNode *shapeCopy = shape->shape->copy();
	    shapeCopy->ref();
	    ASSERT(shapeCopy->isOfType(SoVertexShape::getClassTypeId()));
	    SoVertexShape *vsCopy = (SoVertexShape *) shapeCopy;
	    vsCopy->vertexProperty = NULL;
	    shape->shape->unref();
	    shape->shape = shapeCopy;
	}
    }
}

/////////////////////////////////////////////////////////////////////////////
//
// Stores a node encountered during traversal of a path to a shape.
//
/////////////////////////////////////////////////////////////////////////////

SoCallbackAction::Response
IfCollector::storeNode(SoCallbackAction *cba, SoNode *node)
{
    //////////////////////////////////////////////////////////////////
    //
    // Make sure we stop when we hit the shape we are collecting. If
    // we do hit the shape, also get the current matrix from the
    // state.
    //

    if (node == currentShape->shape) {
	currentShape->transform = SoModelMatrixElement::get(cba->getState());
	return SoCallbackAction::PRUNE;
    }

    // Get the type of the node
    SoType nodeType = node->getTypeId();

    // Based on the type of the node, store the appropriate pointer to
    // it in the IfShape instance

#define IS_OF_TYPE(classname)						      \
    nodeType.isDerivedFrom(classname::getClassTypeId())

#define SET_TO_NULL(field)						      \
    if (currentShape->field != NULL)					      \
	currentShape->field->unref();					      \
    currentShape->field = NULL

#define SET_TO_NODE(field, classname)					      \
    if (currentShape->field != NULL)					      \
	currentShape->field->unref();					      \
    currentShape->field = (classname *) node;				      \
    currentShape->field->ref()

#define ADD_TO_LIST(field)						      \
    if (currentShape->field == NULL)					      \
	currentShape->field = new SoNodeList;				      \
    currentShape->field->append(node)

#define DO_TYPE(classname, field)					      \
    else if (IS_OF_TYPE(classname)) {					      \
	SET_TO_NODE(field, classname);					      \
    }

#define DO_LIST(classname, field)					      \
    else if (IS_OF_TYPE(classname)) {					      \
	ADD_TO_LIST(field);						      \
    }

    //////////////////////////////////////////////////////////////////
    //
    // Special cases:
    //

    // Skip over any shapes, groups, node kits, labels, info, and
    // transformations
    if (IS_OF_TYPE(SoShape)   ||
	IS_OF_TYPE(SoGroup)   ||
	IS_OF_TYPE(SoBaseKit) ||
	IS_OF_TYPE(SoNodeKitListPart) ||
	IS_OF_TYPE(SoLabel)   ||
	IS_OF_TYPE(SoInfo)    ||
	IS_OF_TYPE(SoTransformation))
	;	// Do nothing

    // These node types are kept in lists:
    DO_LIST(SoClipPlane, 		clipPlanes)
    DO_LIST(SoLight,     		lights)
    DO_LIST(SoProfile,   		profiles)

    // Special handling of texture transform nodes. If we are NOT
    // producing texture coordinates, we need to save and output these
    // transforms, so they are added to the "other" list.
    else if (IS_OF_TYPE(SoTexture2Transform)) {
	if (doingTexCoords) {
	    ADD_TO_LIST(textureTransforms);
	}
	else {
	    ADD_TO_LIST(other);
        }
    }

    // Vertex property nodes are handled specially
    else if (IS_OF_TYPE(SoVertexProperty))
	handleVertexProperty(currentShape, (SoVertexProperty *) node);

    // Materials have to be copied. We may be playing around with
    // their values later on, so we want to make sure that changing a
    // material does not affect other objects. (This is especially
    // important if fixing is done on several objects in the same
    // scene graph.)
    else if (IS_OF_TYPE(SoMaterial)) {
	if (currentShape->material != NULL)
	    currentShape->material->unref();
	currentShape->material = (SoMaterial *) node->copy();
	currentShape->material->ref();
    }

    //////////////////////////////////////////////////////////////////
    //
    // Typical cases:
    //

    DO_TYPE(SoCamera,			camera)
    DO_TYPE(SoComplexity,		complexity)
    DO_TYPE(SoCoordinate3,		coords)
    DO_TYPE(SoDrawStyle,		drawStyle)
    DO_TYPE(SoEnvironment,		environment)
    DO_TYPE(SoFont,			font)
    DO_TYPE(SoLightModel,		lightModel)
    DO_TYPE(SoMaterialBinding,		materialBinding)
    DO_TYPE(SoNormal,			normals)
    DO_TYPE(SoNormalBinding,		normalBinding)
    DO_TYPE(SoProfileCoordinate2,	profileCoords)
    DO_TYPE(SoShapeHints,		shapeHints)
    DO_TYPE(SoTexture2,			texture)
    DO_TYPE(SoTextureCoordinate2,	texCoords)
    DO_TYPE(SoTextureCoordinateBinding,	texCoordBinding)

    //////////////////////////////////////////////////////////////////
    //
    // Anything else:
    //

    else {
	ADD_TO_LIST(other);
    }

    return SoCallbackAction::CONTINUE;

#undef IS_OF_TYPE
#undef SET_TO_NULL
#undef SET_TO_NODE
#undef ADD_TO_LIST
#undef DO_TYPE
#undef DO_LIST
}

/////////////////////////////////////////////////////////////////////////////
//
// Handles properties stored in a vertexProperty node.
//
/////////////////////////////////////////////////////////////////////////////

void
IfCollector::handleVertexProperty(IfShape *shape, SoVertexProperty *vp)
{
    int num;

    // Coordinates
    num = vp->vertex.getNum();
    if (num > 0) {
	if (shape->coords != NULL)
	    shape->coords->unref();
	shape->coords = new SoCoordinate3;
	shape->coords->ref();
	shape->coords->point.setValues(0, num, vp->vertex.getValues(0));
    }

    // Normals
    num = vp->normal.getNum();
    if (num > 0) {
	if (shape->normals != NULL) 
	    shape->normals->unref();
	shape->normals = new SoNormal;
	shape->normals->ref();
	shape->normals->vector.setValues(0, num, vp->normal.getValues(0));

	// Steal normal binding
	shape->normalBinding = new SoNormalBinding;
	shape->normalBinding->ref();
	shape->normalBinding->value = vp->normalBinding.getValue();
    }

    // Colors
    num = vp->orderedRGBA.getNum();
    if (num > 0) {
	SoMaterial *oldMaterial = shape->material;
	if (oldMaterial != NULL) {
	    shape->material = (SoMaterial *) oldMaterial->copy();
	    oldMaterial->unref();
	}
	else
	    shape->material = new SoMaterial;
	shape->material->ref();

	// Unpack the colors
	shape->material->diffuseColor.setNum(num);
	shape->material->transparency.setNum(num);
	SbColor *dc = shape->material->diffuseColor.startEditing();
	float   *tr = shape->material->transparency.startEditing();
	const uint32_t *pc = vp->orderedRGBA.getValues(0);

	for (int i = 0; i < num; i++)
	    dc[i].setPackedValue(pc[i], tr[i]);

	shape->material->diffuseColor.finishEditing();
	shape->material->transparency.finishEditing();

	// Steal material binding
	shape->materialBinding = new SoMaterialBinding;
	shape->materialBinding->ref();
	shape->materialBinding->value = vp->materialBinding.getValue();
    }

    // Texture coordinates
    num = vp->texCoord.getNum();
    if (num > 0) {
	if (shape->texCoords != NULL) 
	    shape->texCoords->unref();
	shape->texCoords = new SoTextureCoordinate2;
	shape->texCoords->ref();
	shape->texCoords->point.setValues(0, num, vp->texCoord.getValues(0));
    }
}
