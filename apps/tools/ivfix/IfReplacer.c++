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

#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/elements/SoLazyElement.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPackedColor.h>

#include "IfAssert.h"
#include "IfReplacer.h"

/////////////////////////////////////////////////////////////////////////////
//
// Constructor.
//
/////////////////////////////////////////////////////////////////////////////

IfReplacer::IfReplacer()
{
}

/////////////////////////////////////////////////////////////////////////////
//
// Destructor.
//
/////////////////////////////////////////////////////////////////////////////

IfReplacer::~IfReplacer()
{
}

/////////////////////////////////////////////////////////////////////////////
//
// Replaces nodes in the given scene, in place.
//
/////////////////////////////////////////////////////////////////////////////

void
IfReplacer::replace(SoNode *sceneRoot)
{
    // Replace all SoBaseColor and SoPackedColor nodes with SoMaterial
    // nodes, which are easier to deal with
    replaceMaterials(sceneRoot,   SoBaseColor::getClassTypeId());
    replaceMaterials(sceneRoot, SoPackedColor::getClassTypeId());
}

/////////////////////////////////////////////////////////////////////////////
//
// Replaces all nodes of the given type with SoMaterial nodes.
//
/////////////////////////////////////////////////////////////////////////////

void
IfReplacer::replaceMaterials(SoNode *sceneRoot, const SoType &typeToReplace)
{
    // Find all nodes of the given type
    SoSearchAction sa;
    sa.setType(typeToReplace);
    sa.setInterest(SoSearchAction::ALL);
    sa.apply(sceneRoot);

    // Replace the tail of each path with a material. To do this, we
    // need to apply an SoCallbackAction to the path to gather the
    // material components.
    for (int i = 0; i < sa.getPaths().getLength(); i++) {

	// Cast the path to a full path, just in case
	SoFullPath *path = (SoFullPath *) sa.getPaths()[i];
	ASSERT(path->getTail()->isOfType(typeToReplace));

	// The path better have at least one group above the material
	if (path->getLength() < 2 ||
	    ! path->getNodeFromTail(1)->isOfType(SoGroup::getClassTypeId()))
	    continue;

	// Create a material node that represents the material in
	// effect at the tail of the path
	SoMaterial *newMaterial = createMaterialForPath(path);
	newMaterial->ref();
	
	// Replace the tail node with that material
	SoGroup *parent = (SoGroup *) path->getNodeFromTail(1);
	parent->replaceChild(path->getTail(), newMaterial);
	       
	newMaterial->unref();
    }
}

/////////////////////////////////////////////////////////////////////////////
//
// Creates and returns a material node that represents the material in
// effect at the tail of the given path.
//
/////////////////////////////////////////////////////////////////////////////

SoMaterial *
IfReplacer::createMaterialForPath(SoPath *path)
{
    material = NULL;

    // Apply an SoCallbackAction to the path, and create the material
    // after we get to the tail node
    SoCallbackAction cba;
    cba.addPostTailCallback(materialTailCB, this);
    cba.apply(path);

    ASSERT(material != NULL);

    return material;
}

/////////////////////////////////////////////////////////////////////////////
//
// Stores a material created during traversal of a path.
//
/////////////////////////////////////////////////////////////////////////////

void
IfReplacer::storeMaterial(SoCallbackAction *cba)
{
    // Create a new material
    material = new SoMaterial;

    // Copy the values from the elements in the state into the

    SoState *state = cba->getState();
    SoLazyElement *elt = SoLazyElement::getInstance(cba->getState());

    // Most values can have only one value
    material->ambientColor  = SoLazyElement::getAmbient(state);
    material->specularColor = SoLazyElement::getSpecular(state);
    material->emissiveColor = SoLazyElement::getEmissive(state);
    material->shininess     = SoLazyElement::getShininess(state);

    // If the element contains packed colors, unpack each one into a
    // diffuse color and a transparency value. If they are not packed,
    // set the diffuse colors and transparency values separately.

    if (elt->isPacked()) {
	int num = elt->getNumDiffuse();
	material->diffuseColor.setNum(num);
	material->transparency.setNum(num);
	SbColor *colors = material->diffuseColor.startEditing();
	float *trans = material->transparency.startEditing();
	const uint32_t *eltPacked = elt->getPackedPointer();
	for (int i = 0; i < num; i++)
	    colors[i].setPackedValue(eltPacked[i], trans[i]);
	material->diffuseColor.finishEditing();
	material->transparency.finishEditing();
    }

    else {
	int i, num = elt->getNumDiffuse();
	material->diffuseColor.setNum(num);
	SbColor *colors = material->diffuseColor.startEditing();
	const SbColor *eltColors = elt->getDiffusePointer();
	for (i = 0; i < num; i++)
	    colors[i] = eltColors[i];
	material->diffuseColor.finishEditing();

	num = elt->getNumTransparencies();
	material->transparency.setNum(num);
	float *trans = material->transparency.startEditing();
	const float *eltTrans = elt->getTransparencyPointer();
	for (i = 0; i < num; i++)
	    trans[i] = eltTrans[i];
	material->transparency.finishEditing();
    }
}
