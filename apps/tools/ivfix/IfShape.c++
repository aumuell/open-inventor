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
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoEnvironment.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoMatrixTransform.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoProfileCoordinate2.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoShape.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoTexture2Transform.h>
#include <Inventor/nodes/SoTextureCoordinate2.h>
#include <Inventor/nodes/SoTextureCoordinateBinding.h>

#include "IfAssert.h"
#include "IfShape.h"

/////////////////////////////////////////////////////////////////////////////
//
// Constructor.
//
/////////////////////////////////////////////////////////////////////////////

IfShape::IfShape()
{
    // Level 1:
    camera		= NULL;

    // Level 2:
    lights		= NULL;
    clipPlanes		= NULL;
    environment		= NULL;
    lightModel		= NULL;

    // Level 3:
    texture		= NULL;

    // Level 4:
    drawStyle		= NULL;
    shapeHints		= NULL;
    material		= NULL;
    other		= NULL;

    // Level 5:
    complexity		= NULL;
    coords		= NULL;
    font		= NULL;
    materialBinding	= NULL;
    normals		= NULL;
    normalBinding	= NULL;
    profileCoords	= NULL;
    profiles		= NULL;
    texCoords		= NULL;
    texCoordBinding	= NULL;

    // IfShape:
    shape		= NULL;

    transform.makeIdentity();
    textureTransforms   = NULL;

    differenceLevel	= -1;
    differenceCode	= NONE;

    dontFlatten = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Destructor.
//
/////////////////////////////////////////////////////////////////////////////

IfShape::~IfShape()
{
    clearNodes();
}

/////////////////////////////////////////////////////////////////////////////
//
// This compares two IfShape instances and returns the first level
// (1-5) at which they differ, a comparison code (-1, 0, or +1)
// to indicate the result, and a more specific difference code.
//
/////////////////////////////////////////////////////////////////////////////

int
IfShape::compare(const IfShape *s1, const IfShape *s2,
		 int &level, DifferenceCode &diffCode)
{
    //////////////////////////////////////////////////////////////////
    //
    // The first two macros are used to make quick comparisons between
    // nodes that are unlikely to differ from one shape to the
    // next. They just compare node pointers for equality. Nodes that
    // are compared by examining values in fields are handled
    // specially, using the other macros.
    //

#define COMPARE_NODES(node, code)					      \
    if (s1->node != s2->node) {						      \
	diffCode = code;						      \
	return s2->node - s1->node;					      \
    }

#define COMPARE_LISTS(list, code)					      \
    if (s1->list != s2->list) {						      \
	int c = compareLists(s1->list, s2->list);			      \
	if (c != 0) {							      \
	    diffCode = code;						      \
	    return c;							      \
	}								      \
    }

#define COMPARE_NULL_NODE_POINTERS(node, code)				      \
	if (s1->node == NULL) {						      \
	    diffCode = code;						      \
	    return -1;							      \
	}								      \
	if (s2->node == NULL) {						      \
	    diffCode = code;						      \
	    return 1;							      \
	}

#define COMPARE_INT_FIELDS(node, field, code)				      \
    if (s1->node->field.getValue() != s2->node->field.getValue()) {	      \
	diffCode = code;						      \
	return ((int) s2->node->field.getValue() -			      \
		(int) s1->node->field.getValue());			      \
    }

#define COMPARE_FLOAT_FIELDS(node, field, code)				      \
    if (s1->node->field.getValue() != s2->node->field.getValue()) {	      \
	int c = compareFloats(s1->node->field.getValue(),		      \
			      s2->node->field.getValue());		      \
	if (c != 0) {							      \
	    diffCode = code;						      \
	    return c;							      \
	}								      \
    }

#define COMPARE_MFLOAT_FIELDS(node, field, code)			      \
    if (s1->node->field.getNum() != s2->node->field.getNum())		      \
	return s2->node->field.getNum() - s1->node->field.getNum();	      \
    if (s1->node->field.getNum() == 1) {				      \
	int c = compareFloats(s1->node->field[0], s2->node->field[0]);	      \
	if (c != 0) {							      \
	    diffCode = code;						      \
	    return c;							      \
	}								      \
    }									      \
    else {								      \
	diffCode = code;						      \
	return s2->node - s1->node;					      \
    }

#define COMPARE_MCOLOR_FIELDS(node, field, code)			      \
    if (s1->node->field.getNum() != s2->node->field.getNum())		      \
	return s2->node->field.getNum() - s1->node->field.getNum();	      \
    if (s1->node->field.getNum() == 1) {				      \
	int c = compareColors(s1->node->field[0], s2->node->field[0]);	      \
	if (c != 0) {							      \
	    diffCode = code;						      \
	    return c;							      \
	}								      \
    }									      \
    else {								      \
	diffCode = code;						      \
	return s2->node - s1->node;					      \
    }

    //////////////////////////////////////////////////////////////////

    level = 1;
    COMPARE_NODES(camera, CAMERA);

    level = 2;
    COMPARE_LISTS(lights, LIGHTS);
    COMPARE_LISTS(clipPlanes, CLIP_PLANES);
    COMPARE_NODES(environment, ENVIRONMENT);
    COMPARE_NODES(lightModel, LIGHT_MODEL);

    level = 3;
    // Comparing textures uses the file names if they are set
    if (s1->texture != s2->texture) {
	COMPARE_NULL_NODE_POINTERS(texture, TEXTURE);
	const SbString &name1 = s1->texture->filename.getValue();
	const SbString &name2 = s2->texture->filename.getValue();
	// If no names, use the texture nodes themselves
	if (! name1)
	    return s2->texture - s1->texture;
	int c = strcmp(name1.getString(), name2.getString());
	if (c != 0)
	    return c;
    }
    
    level = 4;
    // Comparing draw style tests the values in the nodes
    if (s1->drawStyle != s2->drawStyle) {
	COMPARE_NULL_NODE_POINTERS(drawStyle, DRAW_STYLE);
	COMPARE_INT_FIELDS(drawStyle,   style,		DRAW_STYLE);
	COMPARE_FLOAT_FIELDS(drawStyle, pointSize,	DRAW_STYLE);
	COMPARE_FLOAT_FIELDS(drawStyle, lineWidth,	DRAW_STYLE);
	COMPARE_INT_FIELDS(drawStyle,   linePattern,	DRAW_STYLE);
    }
    // Comparing shape hints tests the values in the nodes
    if (s1->shapeHints != s2->shapeHints) {
	COMPARE_NULL_NODE_POINTERS(shapeHints, SHAPE_HINTS);
	COMPARE_INT_FIELDS(shapeHints, vertexOrdering,	SHAPE_HINTS);
	COMPARE_INT_FIELDS(shapeHints, shapeType,	SHAPE_HINTS);
	COMPARE_INT_FIELDS(shapeHints, faceType,	SHAPE_HINTS);
	COMPARE_FLOAT_FIELDS(shapeHints, creaseAngle,	SHAPE_HINTS);
    }
    COMPARE_LISTS(other, OTHER);

    // Comparing materials tests the values in the nodes if there is
    // only one material in each.
    // NOTE: the material comparison has to come last within this
    // level so that the IfMerger stuff works properly.
    if (s1->material != s2->material) {
	COMPARE_NULL_NODE_POINTERS(material, MATERIAL);
	COMPARE_MCOLOR_FIELDS(material,  ambientColor, MATERIAL);
	COMPARE_MCOLOR_FIELDS(material,  diffuseColor, MATERIAL);
	COMPARE_MCOLOR_FIELDS(material, specularColor, MATERIAL);
	COMPARE_MCOLOR_FIELDS(material, emissiveColor, MATERIAL);
	COMPARE_MFLOAT_FIELDS(material,     shininess, MATERIAL);
	COMPARE_MFLOAT_FIELDS(material,  transparency, MATERIAL);
    }

    // If nothing else differs, the shape must. We usually consider
    // them to be the same in this case. However, if the shape is not
    // really a shape (it may be a File or similar node), we need to
    // make sure it is not "flattened" later on. To indicate this, we
    // return a level 5 difference. (We can still return that they are
    // the same, since the difference matters only after they have
    // been sorted, when the levels are computed for real.)
    if (s1->dontFlatten || s2->dontFlatten) {
	level = 5;
	return 0;
    }

    // If we got here, they're the same, except for the
    // shape. Consider them to be the same, even though they actually
    // differ at Level 5.
    level = 0;
    return 0;
    
#undef COMPARE_NODES
#undef COMPARE_LISTS
}

/////////////////////////////////////////////////////////////////////////////
//
// Compares two node lists for equality.
//
/////////////////////////////////////////////////////////////////////////////

int
IfShape::compareLists(const SoNodeList *l1, const SoNodeList *l2)
{
    if (l1 == NULL)
	return -1;

    if (l2 == NULL)
	return 1;

    if (l1->getLength() != l2->getLength())
	return l2->getLength() - l1->getLength();

    for (int i = 0; i < l1->getLength(); i++)
	if ((*l1)[i] != (*l2)[i])
	    return (*l2)[i] - (*l2)[i];

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
//
// Compares two floating point numbers for equality.
//
/////////////////////////////////////////////////////////////////////////////

int
IfShape::compareFloats(float f1, float f2)
{
    if (f1 < f2)
	return -1;
    else if (f1 > f2)
	return 1;
    else
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
//
// Compares two colors for equality.
//
/////////////////////////////////////////////////////////////////////////////

int
IfShape::compareColors(const SbColor &c1, const SbColor &c2)
{
    for (int i = 0; i < 3; i++) {
	if (c1[i] < c2[i])
	    return -1;
	else if (c1[i] > c2[i])
	    return 1;
    }
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
//
// Adds the appropriate nodes for the given level to the given
// group node.
//
/////////////////////////////////////////////////////////////////////////////

void
IfShape::addNodesForLevel(SoGroup *group, int level)
{

#define ADD_NODE(n)	if (n != NULL) group->addChild(n)

#define ADD_LIST(l)	if (l != NULL) {				      \
			    for (i = 0; i < (*l).getLength(); i++)	      \
				group->addChild((*l)[i]);		      \
			}

    int i;
    ASSERT(level > 0 && level <= 5);

    switch (level) {

      case 1:
	ADD_NODE(camera);
	break;

      case 2:
	ADD_LIST(lights);
	ADD_LIST(clipPlanes);
	ADD_NODE(environment);
	ADD_NODE(lightModel);
	break;

      case 3:
	ADD_NODE(texture);
	break;

      case 4:
	ADD_NODE(drawStyle);
	ADD_NODE(shapeHints);
	ADD_NODE(material);
	ADD_LIST(other);
	break;

      case 5:
	// For level 5, we add each set of nodes under its own
	// separator
	{
	    SoSeparator *sep = new SoSeparator;
	    group->addChild(sep);
	    group = sep;
	}
	ADD_NODE(complexity);
	ADD_NODE(coords);
	ADD_NODE(font);
	ADD_NODE(materialBinding);
	ADD_NODE(normals);
	ADD_NODE(normalBinding);
	ADD_NODE(profileCoords);
	ADD_LIST(profiles);
	ADD_NODE(texCoords);
	ADD_NODE(texCoordBinding);
	// Handle matrices specially
	if (transform != SbMatrix::identity()) {
	    SoMatrixTransform *xf = new SoMatrixTransform;
	    group->addChild(xf);
	    xf->matrix = transform;
	}
	ADD_LIST(textureTransforms);
	ADD_NODE(shape);
	break;

      default:
	fprintf(stderr, "Error in IfShape::addNodesForLevel(level %d)\n", level);
    }

#undef ADD_NODE
#undef ADD_LIST
}

/////////////////////////////////////////////////////////////////////////////
//
// Unref's non-NULL node pointers.
//
/////////////////////////////////////////////////////////////////////////////

void
IfShape::clearNodes()
{

#define CLEAR_NODE(name) if (name != NULL) { name->unref(); name = NULL; }
#define CLEAR_LIST(name) if (name != NULL) { delete name;   name = NULL; }

    CLEAR_NODE(camera);
    CLEAR_NODE(environment);
    CLEAR_NODE(lightModel);
    CLEAR_NODE(texture);
    CLEAR_NODE(drawStyle);
    CLEAR_NODE(shapeHints);
    CLEAR_NODE(material);
    CLEAR_NODE(complexity);
    CLEAR_NODE(coords);
    CLEAR_NODE(font);
    CLEAR_NODE(materialBinding);
    CLEAR_NODE(normals);
    CLEAR_NODE(normalBinding);
    CLEAR_NODE(profileCoords);
    CLEAR_NODE(texCoords);
    CLEAR_NODE(texCoordBinding);
    CLEAR_NODE(shape);

    CLEAR_LIST(lights);
    CLEAR_LIST(clipPlanes);
    CLEAR_LIST(other);
    CLEAR_LIST(profiles);

#undef CLEAR_NODE
#undef CLEAR_LIST
}
