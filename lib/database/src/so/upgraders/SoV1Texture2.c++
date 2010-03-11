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
 * Copyright (C) 1990,91,92   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |	SoV1Texture2
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include "SoV1Texture2.h"
#include <Inventor/SbPList.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoTexture2Transform.h>
#include <Inventor/sensors/SoNodeSensor.h>

SO_NODE_SOURCE(SoV1Texture2);

// 
// List of textures that we've created so we can share textures with
// the same filename, just like Inventor 1
//
SbPList *SoV1Texture2::textureList = NULL;

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Default constructor
//
// Use: public

SoV1Texture2::SoV1Texture2()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoV1Texture2);

    SO_NODE_ADD_FIELD(component, (INTENSITY));
    SO_NODE_ADD_FIELD(filename, (""));
    SO_NODE_ADD_FIELD(minFilter, (POINT));
    SO_NODE_ADD_FIELD(magFilter, (POINT));
    SO_NODE_ADD_FIELD(wrapS, (REPEAT));
    SO_NODE_ADD_FIELD(wrapT, (REPEAT));
    SO_NODE_ADD_FIELD(model, (MODULATE));
    SO_NODE_ADD_FIELD(blendColor, (SbColor(1,1,1)));
    SO_NODE_ADD_FIELD(translation, (SbVec2f(0., 0.)));
    SO_NODE_ADD_FIELD(scaleFactor, (SbVec2f(1., 1.)));
    SO_NODE_ADD_FIELD(rotation, (0.0));
    SO_NODE_ADD_FIELD(center, (SbVec2f(0.0, 0.0)));

    // Set up static info for enumerated type field
    SO_NODE_DEFINE_ENUM_VALUE(MinFilter, POINT);
    SO_NODE_DEFINE_ENUM_VALUE(MinFilter, BILINEAR);
    SO_NODE_DEFINE_ENUM_VALUE(MinFilter, MIPMAP_POINT);
    SO_NODE_DEFINE_ENUM_VALUE(MinFilter, MIPMAP_LINEAR);
    SO_NODE_DEFINE_ENUM_VALUE(MinFilter, MIPMAP_BILINEAR);
    SO_NODE_DEFINE_ENUM_VALUE(MinFilter, MIPMAP_TRILINEAR);

    SO_NODE_DEFINE_ENUM_VALUE(MagFilter, POINT);
    SO_NODE_DEFINE_ENUM_VALUE(MagFilter, BILINEAR);

    // Set up enumerations for Texture2 model
    SO_NODE_DEFINE_ENUM_VALUE(Model, MODULATE);
    SO_NODE_DEFINE_ENUM_VALUE(Model, DECAL);
    SO_NODE_DEFINE_ENUM_VALUE(Model, BLEND);

    // Set up enumerations for Texture2 component
    SO_NODE_DEFINE_ENUM_VALUE(Component, NULL_COMPONENT);
    SO_NODE_DEFINE_ENUM_VALUE(Component, INTENSITY);
    SO_NODE_DEFINE_ENUM_VALUE(Component, TRANSPARENCY);

    SO_NODE_DEFINE_ENUM_VALUE(Wrap, REPEAT);
    SO_NODE_DEFINE_ENUM_VALUE(Wrap, CLAMP);

    // Set up info in enumerated type field
    SO_NODE_SET_SF_ENUM_TYPE(minFilter, MinFilter);
    SO_NODE_SET_SF_ENUM_TYPE(magFilter, MagFilter);
    SO_NODE_SET_SF_ENUM_TYPE(model, Model);
    SO_NODE_SET_SF_ENUM_TYPE(component, Component);
    SO_NODE_SET_SF_ENUM_TYPE(wrapS, Wrap);
    SO_NODE_SET_SF_ENUM_TYPE(wrapT, Wrap);

    // This upgrader returns a group, but it is an upgrade of a non-group
    isGroup = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoV1Texture2::~SoV1Texture2()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Create a version 2.0 SoTexture2.
//
// Use: private

SoNode *
SoV1Texture2::createNewNode()
//
////////////////////////////////////////////////////////////////////////
{
    SoTexture2 *tex;
    SbBool alreadyExists = findTexture2(tex);

    tex->ref();

    if (!alreadyExists) {
	// Convert from old enums to new fields:

	// Easy ones first:
	SO_UPGRADER_COPY_FIELD(blendColor, tex);

	// Enum values for model changed between 1.0/2.0:
	if (!model.isDefault()) {
	    switch (model.getValue()) {
	      case MODULATE:
		tex->model = SoTexture2::MODULATE;
		break;
	      case DECAL:
		tex->model = SoTexture2::DECAL;
		break;
	      case BLEND:
		tex->model = SoTexture2::BLEND;
		break;
	    }
	}
	if (model.isIgnored()) tex->model.setIgnored(TRUE);
    
	// Enum values for wrapS changed between 1.0/2.0:
	if (!wrapS.isDefault()) {
	    switch (wrapS.getValue()) {
	      case REPEAT:
		tex->wrapS = SoTexture2::REPEAT;
		break;
	      case CLAMP:
		tex->wrapS = SoTexture2::CLAMP;
		break;
	    }
	}
	if (wrapS.isIgnored()) tex->wrapS.setIgnored(TRUE);

	// Enum values for wrapT changed between 1.0/2.0:
	if (!wrapT.isDefault()) {
	    switch (wrapT.getValue()) {
	      case REPEAT:
		tex->wrapT = SoTexture2::REPEAT;
		break;
	      case CLAMP:
		tex->wrapT = SoTexture2::CLAMP;
		break;
	    }
	}
	if (wrapT.isIgnored()) tex->wrapT.setIgnored(TRUE);


	// Convert name to string in filename field
	if (! filename.isDefault())
	    tex->filename = filename.getValue().getString();
	if (filename.isIgnored())
	    tex->filename.setIgnored(TRUE);

	// Ignore component field

	// And ignore min/magFilters; I could create a Complexity node
	// with textureQuality set and value/type ignored, but I don't
	// think it is worth the effort...
    }

    // These require us to create a Texture2Transform also:
    if (!translation.isDefault() || !scaleFactor.isDefault() ||
	!rotation.isDefault()) {

	SoGroup *g = SO_UPGRADER_CREATE_NEW(SoGroup);
	g->ref();

	SoTexture2Transform *xf = SO_UPGRADER_CREATE_NEW(SoTexture2Transform);
	g->addChild(xf);
	SO_UPGRADER_COPY_FIELD(translation, xf);
	SO_UPGRADER_COPY_FIELD(scaleFactor, xf);
	SO_UPGRADER_COPY_FIELD(rotation, xf);
	SO_UPGRADER_COPY_FIELD(center, xf);

	g->addChild(tex);
	tex->unref();
	g->unrefNoDelete();

	return g;
    }	
    tex->unrefNoDelete();

    return tex;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Get a texture2 node with the right stuff in it.  This imitates
//    the Inventor 1.0 behavior of sharing GL texture maps by
//    instancing Texture2 nodes with the same fields.  It does this by
//    maintaining a list of all Texture2 nodes created by the
//    upgrader, and searching through the list for a match.
//
// Use: private

SbBool
SoV1Texture2::findTexture2(SoTexture2 *&tex)
//
////////////////////////////////////////////////////////////////////////
{
    if (textureList == NULL) {
	textureList = new SbPList;
    }
    for (int i = 0; i < textureList->getLength(); i++) {
	tex = (SoTexture2 *)(*textureList)[i];
	if (matches(tex))
	    return TRUE;
    }
    // Not found, create a new one and add to list:
    tex = SO_UPGRADER_CREATE_NEW(SoTexture2);
    textureList->append(tex);

    // And add a node sensor so we can remove the texture from the
    // list if it is deleted:
    SoNodeSensor *sensor = new SoNodeSensor;
    sensor->attach(tex);
    sensor->setDeleteCallback(nodeDeletedCB, tex);

    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if the given Texture2 node's fields match this
//    upgrader's fields.
//
// Use: private, static

SbBool
SoV1Texture2::matches(SoTexture2 *compare)
//
////////////////////////////////////////////////////////////////////////
{
    if (compare->filename.getValue() !=
	filename.getValue().getString()) return FALSE;
    
    int compareModel = compare->model.getValue();
    switch (model.getValue()) {
      case MODULATE:
	if (compareModel != SoTexture2::MODULATE) return FALSE;
	break;
      case DECAL:
	if (compareModel != SoTexture2::DECAL) return FALSE;
	break;
      case BLEND:
	if (compareModel != SoTexture2::BLEND) return FALSE;
	break;
    }
    int compareWrap = compare->wrapS.getValue();
    switch (wrapS.getValue()) {
      case REPEAT:
	if (compareWrap != SoTexture2::REPEAT) return FALSE;
	break;
      case CLAMP:
	if (compareWrap != SoTexture2::CLAMP) return FALSE;
	break;
    }
    compareWrap = compare->wrapT.getValue();
    switch (wrapT.getValue()) {
      case REPEAT:
	if (compareWrap != SoTexture2::REPEAT) return FALSE;
	break;
      case CLAMP:
	if (compareWrap != SoTexture2::CLAMP) return FALSE;
	break;
    }
    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Called by node sensor when a Texture2 we are keeping track of is
//    deleted.  Removes the node from the list and deletes the sensor.
//
// Use: private, static

void
SoV1Texture2::nodeDeletedCB(void *node, SoSensor *sensor)
//
////////////////////////////////////////////////////////////////////////
{
    textureList->remove(textureList->find(node));
    delete sensor;
}
