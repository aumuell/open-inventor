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
 * Copyright (C) 1995-96   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.2 $
 |
 |   Classes:
 |      SoVertexProperty,  SoVertexCache
 |
 |   Author(s)          : Alan Norton,  Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoPickAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/elements/SoGLCoordinateElement.h>
#include <Inventor/elements/SoGLNormalElement.h>
#include <Inventor/elements/SoOverrideElement.h>
#include <Inventor/elements/SoGLTextureCoordinateElement.h>
#include <Inventor/elements/SoTextureCoordinateBindingElement.h>
#include <Inventor/elements/SoMaterialBindingElement.h>
#include <Inventor/elements/SoNormalBindingElement.h>
#include <Inventor/nodes/SoTextureCoordinate2.h>
#include <Inventor/elements/SoGLLazyElement.h>
#include <machine.h>

SO_NODE_SOURCE(SoVertexProperty);


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoVertexProperty::SoVertexProperty()
//
////////////////////////////////////////////////////////////////////////
{

    SO_NODE_CONSTRUCTOR(SoVertexProperty);

    // Initialize with dummy values using standard macro, then delete
    // all values:
    SO_NODE_ADD_FIELD(vertex, (SbVec3f(0,0,0)));
    vertex.deleteValues(0);
    vertex.setDefault(TRUE);
    SO_NODE_ADD_FIELD(normal, (SbVec3f(0,0,0)));
    normal.deleteValues(0);
    normal.setDefault(TRUE);
    SO_NODE_ADD_FIELD(texCoord, (SbVec2f(0,0)));
    texCoord.deleteValues(0);
    texCoord.setDefault(TRUE);
    SO_NODE_ADD_FIELD(orderedRGBA, (0));    
    orderedRGBA.deleteValues(0);
    orderedRGBA.setDefault(TRUE);

    // Initialize these with default values.  They'll be ignored if
    // the corresponding fields have no values:
    SO_NODE_ADD_FIELD(materialBinding, (SoVertexProperty::OVERALL));
    SO_NODE_ADD_FIELD(normalBinding, (SoVertexProperty::PER_VERTEX_INDEXED));

    // Set up static info for enum type fields:

    SO_NODE_DEFINE_ENUM_VALUE(Binding, OVERALL);
    SO_NODE_DEFINE_ENUM_VALUE(Binding, PER_VERTEX);
    SO_NODE_DEFINE_ENUM_VALUE(Binding, PER_VERTEX_INDEXED);
    SO_NODE_DEFINE_ENUM_VALUE(Binding, PER_FACE);
    SO_NODE_DEFINE_ENUM_VALUE(Binding, PER_FACE_INDEXED);
    SO_NODE_DEFINE_ENUM_VALUE(Binding, PER_PART);
    SO_NODE_DEFINE_ENUM_VALUE(Binding, PER_PART_INDEXED);

    SO_NODE_SET_SF_ENUM_TYPE(materialBinding, Binding);
    SO_NODE_SET_SF_ENUM_TYPE(normalBinding, Binding);

    isBuiltIn = TRUE;
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    initialize class  
//
void
SoVertexProperty::initClass()
{
    SO__NODE_INIT_CLASS(SoVertexProperty, "VertexProperty", SoNode);

    // enable elements for vertex:
    SO_ENABLE(SoCallbackAction,		SoCoordinateElement);
    SO_ENABLE(SoGLRenderAction, 	SoGLCoordinateElement);
    SO_ENABLE(SoGetBoundingBoxAction,	SoCoordinateElement);
    SO_ENABLE(SoPickAction,		SoCoordinateElement);

    // Enable elements for normal:
    SO_ENABLE(SoGLRenderAction, SoGLNormalElement);
    SO_ENABLE(SoCallbackAction, SoNormalElement);
    SO_ENABLE(SoPickAction,	SoNormalElement);

    // Enable elements for normal binding:
    SO_ENABLE(SoGLRenderAction, SoNormalBindingElement);
    SO_ENABLE(SoCallbackAction, SoNormalBindingElement);
    SO_ENABLE(SoPickAction, SoNormalBindingElement);

    // Enable elements for material:
    SO_ENABLE(SoCallbackAction, SoLazyElement);
    SO_ENABLE(SoPickAction, SoLazyElement);
    SO_ENABLE(SoGLRenderAction, SoGLLazyElement);

    // Enable elements for MaterialBinding:    
    SO_ENABLE(SoGLRenderAction, SoMaterialBindingElement);
    SO_ENABLE(SoCallbackAction, SoMaterialBindingElement);
    SO_ENABLE(SoPickAction,     SoMaterialBindingElement);

    // Enable elements for textureCoordinates:
    SO_ENABLE(SoCallbackAction, SoTextureCoordinateElement);
    SO_ENABLE(SoGLRenderAction, SoGLTextureCoordinateElement);
    SO_ENABLE(SoPickAction,     SoTextureCoordinateElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoVertexProperty::~SoVertexProperty()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Handles any action derived from SoAction.
//
// Use: extender

void
SoVertexProperty::doAction(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoState		*state = action->getState();
    //  Note, we are not supporting isIgnored flags:
    if (vertex.getNum() > 0)
	SoCoordinateElement::set3(state, this,
				  vertex.getNum(), vertex.getValues(0));

    if (normal.getNum() > 0){
	SoNormalElement::set(state, this,
			     normal.getNum(), normal.getValues(0));
    }
    SoNormalBindingElement::set(state, 
	(SoNormalBindingElement::Binding)normalBinding.getValue());

    // set diffuse color
    if (orderedRGBA.getNum() > 0
	&& ! SoOverrideElement::getDiffuseColorOverride(state)) {
	if (isOverride()) {
	    SoOverrideElement::setDiffuseColorOverride(state, this, TRUE);
	}			
	SoLazyElement::setPacked(state, this,
		orderedRGBA.getNum(), orderedRGBA.getValues(0));
    }
    // set material binding 
    SoMaterialBindingElement::set(state, 
	(SoMaterialBindingElement::Binding)materialBinding.getValue());

    if ( texCoord.getNum() > 0) {
	SoTextureCoordinateElement::set2(state, this,
			 texCoord.getNum(), texCoord.getValues(0));
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does GL render action.
//
// Use: extender

void
SoVertexProperty::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    if ( texCoord.getNum() > 0) {
    	SoState *state = action->getState();
	SoGLTextureCoordinateElement::setTexGen(state, this, NULL);
    }

    SoVertexProperty::doAction(action);

}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Handles get bounding box action.
//
// Use: extender

void
SoVertexProperty::getBoundingBox(SoGetBoundingBoxAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // Note that Bounding box only needs to look at coordinates:
    if (vertex.getNum() > 0)
        SoCoordinateElement::set3(action->getState(), this,
                                  vertex.getNum(), vertex.getValues(0));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does callback action thing.
//
// Use: extender

void
SoVertexProperty::callback(SoCallbackAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoVertexProperty::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does pick action...
//
// Use: extender

void
SoVertexProperty::pick(SoPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoVertexProperty::doAction(action);
}

void vp_glColor4ubv(const GLubyte *v)
{
    GLubyte _v[4];
    DGL_HTON_INT32(*((int32_t*)_v), *((int32_t*)v));

    glColor4ubv(_v);
}

void
SoVertexPropertyCache::fillInCache(const SoVertexProperty *vp,
		 SoState *state)
{
    renderCase = 0;

    // Coordinates:
    if (vp && (numVerts = vp->vertex.getNum()) != 0) {
	vertexFunc = (SoVPCacheFunc *)glVertex3fv;
	vertexStride = sizeof(SbVec3f);
	vertexPtr = (const char *)vp->vertex.getValues(0);
	needFromState &= ~COORD_FROM_STATE_BIT;
    } else {
	const SoCoordinateElement *ce =
	    SoCoordinateElement::getInstance(state);
	numVerts = ce->getNum();
	if (ce->is3D()) {
	    vertexFunc = (SoVPCacheFunc *)glVertex3fv;
	    vertexStride = sizeof(SbVec3f);
	    vertexPtr = (const char *) &ce->get3(0);
	} else {
	    vertexFunc = (SoVPCacheFunc *)glVertex4fv;
	    vertexStride = sizeof(SbVec4f);
	    vertexPtr = (const char *) &ce->get4(0);
	}	    
	needFromState |= COORD_FROM_STATE_BIT;
    }

    // Diffuse colors:
    transpIsInVP = FALSE;
    SbBool colorOverride = ((SoOverrideElement::getDiffuseColorOverride(state))||
		    (SoOverrideElement::getTransparencyOverride(state)));
    if (SoGLLazyElement::isColorIndex(state)) {
	const SoLazyElement *le = SoLazyElement::getInstance(state);	
	numColors = le->getNumDiffuse();
	colorFunc = (SoVPCacheFunc *)glIndexiv;
	colorStride = sizeof(int32_t);
	colorPtr = (const char *)le->getColorIndexPointer();
	needFromState |= COLOR_FROM_STATE_BITS;
    } else {
	if ((!colorOverride) &&		
	    (vp && (numColors = vp->orderedRGBA.getNum()) != 0)) {
	    colorPtr = (const char *)vp->orderedRGBA.getValues(0);
	    colorFunc = (SoVPCacheFunc *)vp_glColor4ubv;
	    colorStride = sizeof(uint32_t);
	    needFromState &= ~COLOR_FROM_STATE_BITS;
	    colorIsInVP = TRUE;
	    //Check to see if there really is a non-opaque transparency:
	    for(int i = 0; i< vp->orderedRGBA.getNum(); i++){
		if ((vp->orderedRGBA[i] & 0xff) != 0xff){
		    transpIsInVP = TRUE;
		    break;
		}
	    }
	} else {  //Get color from state
	    const SoLazyElement *le = SoLazyElement::getInstance(state);
	    colorIsInVP = FALSE;
	    numColors = le->getNumDiffuse();
	    needFromState |= COLOR_FROM_STATE_BITS;
	    colorPtr = (const char *) le->getPackedPointer();
	    colorFunc = (SoVPCacheFunc *)vp_glColor4ubv;
	    colorStride = sizeof(uint32_t);
	} 

    }

    //setup materialBinding
    if (needFromState & COLOR_FROM_STATE_BITS ||
	    SoOverrideElement::getMaterialBindingOverride(state)) {
	materialBinding = SoMaterialBindingElement::get(state);	
    }
    else {
	materialBinding = (SoMaterialBindingElement::Binding)
	    vp->materialBinding.getValue();	
    }
    
    switch (materialBinding) {
      case SoMaterialBindingElement::OVERALL:
	// 00... bits, nothing to do.  But if color is overridden, still
	// needFromState should remain on, to guarantee that we will
	// continue to fill in the cache.
	if(!colorOverride) needFromState &= ~COLOR_FROM_STATE_BITS;
	numColors = 1;
	break;
      case SoMaterialBindingElement::PER_PART:
	renderCase |= PART_COLOR;
	break;
      case SoMaterialBindingElement::PER_PART_INDEXED:
	renderCase |= PART_COLOR;
	break;
      case SoMaterialBindingElement::PER_FACE:
	renderCase |= FACE_COLOR;
	break;
      case SoMaterialBindingElement::PER_FACE_INDEXED:
	renderCase |= FACE_COLOR;
	break;
      case SoMaterialBindingElement::PER_VERTEX:
	renderCase |= VERTEX_COLOR;
	break;
      case SoMaterialBindingElement::PER_VERTEX_INDEXED:
	renderCase |= VERTEX_COLOR;
	break;
    }

    // Normals:
    // Setup as if normals are needed, even if they aren't
   
    normalFunc = (SoVPCacheFunc *)glNormal3fv;
    normalStride = sizeof(SbVec3f);

    if (vp && (numNorms = vp->normal.getNum()) != 0) {
	normalPtr = (const char *)vp->normal.getValues(0);
	needFromState &= ~NORMAL_FROM_STATE_BITS;
	generateNormals = FALSE;
    } else {
	needFromState |= NORMAL_FROM_STATE_BITS;

	const SoNormalElement *ne =
	    SoNormalElement::getInstance(state);
	if ((numNorms = ne->getNum()) > 0) {
	    normalPtr = (const char *) &ne->get(0);
	    generateNormals = FALSE;
	} else {
	    generateNormals = TRUE;
	}	    
    }

    // set up normalBinding
    if (generateNormals) {
	normalBinding = SoNormalBindingElement::PER_VERTEX;
    }
    else if (needFromState & NORMAL_FROM_STATE_BITS) {
	normalBinding = SoNormalBindingElement::get(state);
    }
    else {
	normalBinding = (SoNormalBindingElement::Binding)
	     vp->normalBinding.getValue();
    }
	
    switch (normalBinding) {
	case SoNormalBindingElement::OVERALL:
	    // 00... bits, nothing to do
	    break;
	case SoNormalBindingElement::PER_PART:
	    renderCase |= PART_NORMAL;
	    break;
        case SoNormalBindingElement::PER_PART_INDEXED:
	    renderCase |= PART_NORMAL;
	    break;
	case SoNormalBindingElement::PER_FACE:
	    renderCase |= FACE_NORMAL;
	    break;
	case SoNormalBindingElement::PER_FACE_INDEXED:
	    renderCase |= FACE_NORMAL;
	    break;
	case SoNormalBindingElement::PER_VERTEX:
	    renderCase |= VERTEX_NORMAL;
	    break;
	case SoNormalBindingElement::PER_VERTEX_INDEXED:
	    renderCase |= VERTEX_NORMAL;
	    break;
    }
    
  
    
    //Setup as if texture coords are needed, even if they are not:
    if (vp && (numTexCoords = vp->texCoord.getNum()) != 0) {
	texCoordFunc = (SoVPCacheFunc *)glTexCoord2fv;
	texCoordStride = sizeof(SbVec2f);
	texCoordPtr = (const char *)vp->texCoord.getValues(0);
	needFromState &= ~TEXCOORD_FROM_STATE_BIT;
	needFromState |= TEXTURE_FUNCTION_BIT;
	texCoordBinding =
		SoTextureCoordinateBindingElement::PER_VERTEX_INDEXED;
	renderCase |= TEXCOORD_BIT;
	generateTexCoords = FALSE;
    } else {
	const SoTextureCoordinateElement *tce =
		SoTextureCoordinateElement::getInstance(state);
	if ((numTexCoords = tce->getNum()) > 0) {
	    if (tce->is2D()) {
		texCoordFunc = (SoVPCacheFunc *)glTexCoord2fv;
		texCoordStride = sizeof(SbVec2f);
		texCoordPtr = (const char *) &tce->get2(0);
	    } else {
		texCoordFunc = (SoVPCacheFunc *)glTexCoord4fv;
		texCoordStride = sizeof(SbVec4f);
		texCoordPtr = (const char *) &tce->get4(0);
	    }
		texCoordBinding =
		    SoTextureCoordinateBindingElement::get(state);
		renderCase |= TEXCOORD_BIT;
		generateTexCoords = FALSE;
	    } else {
		texCoordFunc = NULL;
		generateTexCoords = TRUE;
		renderCase &= ~TEXCOORD_BIT;  // No normals, use glTexGen
	    }
	needFromState |= TEXCOORD_FROM_STATE_BIT;
	needFromState &= ~TEXTURE_FUNCTION_BIT;
    }    
}

SoVertexPropertyCache::SoVertexPropertyCache()
{
    vertexFunc = normalFunc = colorFunc = texCoordFunc = NULL;
    vertexPtr = normalPtr = colorPtr = texCoordPtr = NULL;
    vertexStride = normalStride = colorStride = texCoordStride = 0;
    numVerts = numNorms = numColors = numTexCoords = 0;
    colorIsInVP = transpIsInVP = FALSE;
    generateNormals = generateTexCoords = FALSE;
    needFromState = ALL_FROM_STATE_BITS;
    renderCase = 0;
}



