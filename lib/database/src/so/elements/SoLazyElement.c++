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
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision 2.0 $
 |
 |   Classes:
 |	SoLazyElement,  SoColorPacker
 |
 |   Author(s)		: Alan Norton, Gavin Bell 
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
*/

#include <Inventor/elements/SoLazyElement.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/elements/SoSubElement.h>
#include <Inventor/elements/SoShapeStyleElement.h>
#include <Inventor/nodes/SoPackedColor.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/fields/SoMFFloat.h>
#include <Inventor/fields/SoMFColor.h>

SO_ELEMENT_SOURCE(SoLazyElement);
static	SbColor unpacker(0, 0, 0);

SbColor	*SoLazyElement::defaultDiffuseColor = NULL;
float	*SoLazyElement::defaultTransparency = NULL;
int32_t	*SoLazyElement::defaultColorIndices = NULL;
uint32_t	*SoLazyElement::defaultPackedColor  = NULL;
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private
////////////////////////////////////////////////////////////////////////

SoLazyElement::~SoLazyElement()

{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes element.
//
// Use: public
////////////////////////////////////////////////////////////////////////
void
SoLazyElement::init(SoState *)

{
    //  Set to GL defaults:
    ivState.ambientColor	= getDefaultAmbient();
    ivState.emissiveColor	= getDefaultEmissive();
    ivState.specularColor	= getDefaultSpecular();
    ivState.shininess		= getDefaultShininess();
    ivState.colorMaterial	= FALSE;
    ivState.blending		= FALSE;
    ivState.lightModel		= PHONG;
    
    // Initialize default color storage if not already done
    if (defaultDiffuseColor == NULL) {
	defaultDiffuseColor	= new SbColor;
	*defaultDiffuseColor	= getDefaultDiffuse();
	defaultTransparency	= new float;
	*defaultTransparency	= getDefaultTransparency();
	defaultColorIndices	= new int32_t;
	*defaultColorIndices	= getDefaultColorIndex();
	defaultPackedColor	= new uint32_t;
	*defaultPackedColor	= getDefaultPacked();
    }
    
    //following value will be matched with the default color, must
    //differ from 1 (invalid) and any  legitimate nodeid. 
    ivState.diffuseNodeId	= 0;
    ivState.transpNodeId	= 0;
    //zero corresponds to transparency off (default).
    ivState.stippleNum		= 0;
    ivState.diffuseColors	= defaultDiffuseColor;
    ivState.transparencies	= defaultTransparency;
    ivState.colorIndices	= defaultColorIndices;
    ivState.packedColors	= defaultPackedColor;

    ivState.numDiffuseColors	= 1;
    ivState.numTransparencies	= 1;
    ivState.packed		= FALSE;
    ivState.packedTransparent	= FALSE;
    ivState.transpType		= SoGLRenderAction::SCREEN_DOOR;  
    ivState.cacheLevelSetBits	= 0;
    ivState.cacheLevelSendBits	= 0;
    
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    get the indexed diffuse color in the element 
//
// Use: public, static
////////////////////////////////////////////////////////////////////////
const SbColor &
SoLazyElement::getDiffuse(SoState* state, int index) 
{
    SoLazyElement* curElt = getInstance(state);
    if(state->isCacheOpen()) curElt->registerGetDependence(state, DIFFUSE_MASK);  
#ifdef DEBUG
    if (index > curElt->ivState.numDiffuseColors || index < 0){
	SoDebugError::post("SoLazyElement::getDiffuse", 
			"invalid index");
        return(*defaultDiffuseColor);
    }
#endif
    if (!curElt->ivState.packed) return (curElt->ivState.diffuseColors[index]);
    unpacker = SbColor( 
       ((curElt->ivState.packedColors[index] & 0xff000000) >> 24) * 1.0/255,  
       ((curElt->ivState.packedColors[index] & 0xff0000) >> 16) * 1.0/255,  		
       ((curElt->ivState.packedColors[index] & 0xff00)>> 8) * 1.0/255); 
    return unpacker;
      
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    get the indexed transparency in the element 
//
// Use: public, static
////////////////////////////////////////////////////////////////////////
float
SoLazyElement::getTransparency(SoState* state, int index)  
{
    SoLazyElement* curElt = getInstance(state);
    if(state->isCacheOpen()) curElt->registerGetDependence(state, DIFFUSE_MASK);  
#ifdef DEBUG
    if (index > curElt->ivState.numTransparencies || index < 0){
	SoDebugError::post("SoLazyElement::getTransparency", 
			"invalid index");
        return(*curElt->defaultTransparency);
    }
#endif
    if (!curElt->ivState.packed) return (curElt->ivState.transparencies[index]);
    return( 1.0 - ((curElt->ivState.packedColors[index] & 0xff) * 1.0/255));
             
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    get the color index from the element 
//
// Use: public, static
////////////////////////////////////////////////////////////////////////
int32_t
SoLazyElement::getColorIndex(SoState* state, int index)  
{
    SoLazyElement* curElt = getInstance(state);
    if(state->isCacheOpen()) curElt->registerGetDependence(state, DIFFUSE_MASK);  
#ifdef DEBUG
    if (index > curElt->ivState.numDiffuseColors || index < 0){
	SoDebugError::post("SoLazyElement::getColorIndex", 
			"invalid index");
        return(curElt->getDefaultColorIndex());
    }
#endif
    return (curElt->ivState.colorIndices[index]);
             
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    get the packed color from the element 
//
// Use: public, static
////////////////////////////////////////////////////////////////////////
const uint32_t*
SoLazyElement::getPackedColors(SoState* state) 
{ 
    SoLazyElement* curElt = getInstance(state);
    if(state->isCacheOpen()) curElt->registerGetDependence(state, DIFFUSE_MASK);    
    return curElt->ivState.packedColors;
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    get the color index from the element 
//
// Use: public, static
////////////////////////////////////////////////////////////////////////
const int32_t *
SoLazyElement::getColorIndices(SoState* state) 
{   
    SoLazyElement* curElt = getInstance(state);
    if(state->isCacheOpen()) curElt->registerGetDependence(state, DIFFUSE_MASK);           
    return curElt->ivState.colorIndices;
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    get the ambient color from the element 
//
// Use: public, static
////////////////////////////////////////////////////////////////////////
const SbColor&
SoLazyElement::getAmbient(SoState* state) 
{
    SoLazyElement* curElt = getInstance(state);
    if(state->isCacheOpen()) curElt->registerGetDependence(state, AMBIENT_MASK);      
    return curElt->ivState.ambientColor;
} 
////////////////////////////////////////////////////////////////////////
//
// Description:
//    get the emissive color from the element 
//
// Use: public, static
////////////////////////////////////////////////////////////////////////
const SbColor&
SoLazyElement::getEmissive(SoState* state) 
{ 
    SoLazyElement* curElt = getInstance(state);
    if(state->isCacheOpen()) curElt->registerGetDependence(state, EMISSIVE_MASK);   
    return curElt->ivState.emissiveColor;
} 
////////////////////////////////////////////////////////////////////////
//
// Description:
//    get the specular color from the element 
//
// Use: public, static
////////////////////////////////////////////////////////////////////////
const SbColor&
SoLazyElement::getSpecular(SoState* state) 
{   
    SoLazyElement* curElt = getInstance(state);
    if(state->isCacheOpen()) curElt->registerGetDependence(state, SPECULAR_MASK); 
    return curElt->ivState.specularColor;
} 
////////////////////////////////////////////////////////////////////////
//
// Description:
//    get the shininess from the element 
//
// Use: public, static
////////////////////////////////////////////////////////////////////////
float
SoLazyElement::getShininess(SoState* state) 
{	
    SoLazyElement* curElt = getInstance(state);
    if(state->isCacheOpen()) 
	curElt->registerGetDependence(state, SHININESS_MASK);     
    return curElt->ivState.shininess;
} 
////////////////////////////////////////////////////////////////////////
//
// Description:
//    get the color material state from the element 
//
// Use: public, static
////////////////////////////////////////////////////////////////////////
SbBool
SoLazyElement::getColorMaterial(SoState* state) 
{   
    SoLazyElement* curElt = getInstance(state);
    if(state->isCacheOpen()) 
	curElt->registerGetDependence(state, COLOR_MATERIAL_MASK);  
    return curElt->ivState.colorMaterial;
}  
////////////////////////////////////////////////////////////////////////
//
// Description:
//    get the blending state from the element 
//
// Use: public, static
////////////////////////////////////////////////////////////////////////
SbBool
SoLazyElement::getBlending(SoState* state) 
{   
    SoLazyElement* curElt = getInstance(state);
    if(state->isCacheOpen()) curElt->registerGetDependence(state, BLENDING_MASK);  
    return curElt->ivState.blending;
}  
////////////////////////////////////////////////////////////////////////
//
// Description:
//    get the light model from the element 
//
// Use: public, static
////////////////////////////////////////////////////////////////////////
int32_t
SoLazyElement::getLightModel(SoState* state) 
{   
    SoLazyElement* curElt = getInstance(state);
    if(state->isCacheOpen()) 
	curElt->registerGetDependence(state, LIGHT_MODEL_MASK);  
    return curElt->ivState.lightModel;
}   

///////////////////////////////////////////////////////////////////////
//
// Description: static set() method for diffuse color
//
// use:  public, SoEXTENDER, static
//
///////////////////////////////////////////////////////////////////////  
void	
SoLazyElement::setDiffuse(SoState *state, SoNode *node, int32_t numColors, 
	    const SbColor *colors, SoColorPacker *cPacker)
{
    SoLazyElement *curElt = SoLazyElement::getInstance(state);
    //Because we are getting the transparency value from state, there
    //is a get-dependence
    if(state->isCacheOpen())curElt->registerGetDependence(state, DIFFUSE_MASK);
    if (curElt->ivState.diffuseNodeId !=  node->getNodeId() ||
	 (!cPacker->transpMatch(curElt->ivState.transpNodeId))){
	getWInstance(state)->setDiffuseElt(node,  numColors, colors, cPacker);
    }
    else if (state->isCacheOpen()){       
	curElt->registerRedundantSet(state, DIFFUSE_MASK);
    }
}
///////////////////////////////////////////////////////////////////////
//
// Description: static set() method for transparency 
//
// use:  public, SoEXTENDER, static
//
///////////////////////////////////////////////////////////////////////  
void	
SoLazyElement::setTransparency(SoState *state, SoNode *node, int32_t numTransp, 
	    const float *transp, SoColorPacker *cPacker)
{
    SoLazyElement *curElt = SoLazyElement::getInstance(state);
    //Because we are getting the diffuse value from state, there
    //is a get-dependence
    if(state->isCacheOpen())curElt->registerGetDependence(state, DIFFUSE_MASK);
    
    uint32_t testNodeId;
    if(numTransp == 1 && transp[0] == 0.0) testNodeId = 0;
    else testNodeId = node->getNodeId();
    
    if ((curElt->ivState.transpNodeId != testNodeId) ||	
	(!cPacker->diffuseMatch(curElt->ivState.diffuseNodeId)))
	getWInstance(state)->setTranspElt(node, numTransp, transp, cPacker);
    else if (state->isCacheOpen()) 
	curElt->registerRedundantSet(state, TRANSPARENCY_MASK|DIFFUSE_MASK); 
}	    
///////////////////////////////////////////////////////////////////////
//
// Description: static set() method for color indices
//
// use:  public, SoEXTENDER, static
//
///////////////////////////////////////////////////////////////////////  
void	
SoLazyElement::setColorIndices(SoState *state, SoNode *node, int32_t numIndices, 
	    const int32_t *indices)
{
    SoLazyElement *curElt = SoLazyElement::getInstance(state);
    if (curElt->ivState.diffuseNodeId !=  node->getNodeId())
	getWInstance(state)->setColorIndexElt(node, numIndices, indices);
    else if (state->isCacheOpen()) 
	curElt->registerRedundantSet(state, DIFFUSE_MASK); 
}	    
///////////////////////////////////////////////////////////////////////
//
// Description: static set() method for transparencyType 
//
// use:  public, SoINTERNAL, static
//
///////////////////////////////////////////////////////////////////////  
void	
SoLazyElement::setTransparencyType(SoState *state, int32_t type)
{
    SoLazyElement *curElt = SoLazyElement::getInstance(state);
    if (curElt->ivState.transpType != type)
	curElt->setTranspTypeElt( type);  
}
///////////////////////////////////////////////////////////////////////
//
// Description: static set() method for packed colors 
//
// use:  public, SoEXTERNAL, static
//
///////////////////////////////////////////////////////////////////////  
void	
SoLazyElement::setPacked(SoState *state, SoNode *node,
	    int32_t numColors, const uint32_t *colors)
{
    SoLazyElement *curElt = SoLazyElement::getInstance(state);
    if (curElt->ivState.diffuseNodeId != (node->getNodeId()) ||	    
	    (!(curElt->ivState.packed)) || 
	    (curElt->ivState.packedColors != colors)){
	getWInstance(state)->setPackedElt( node, numColors, colors);
    } 
    else if (state->isCacheOpen()) 
        curElt->registerRedundantSet(state, DIFFUSE_MASK|TRANSPARENCY_MASK);
}
///////////////////////////////////////////////////////////////////////
//
// Description: static set() method for ambient color 
//
// use:  public, SoEXTERNAL, static
//
///////////////////////////////////////////////////////////////////////  
void	
SoLazyElement::setAmbient(SoState *state, const SbColor* color)
{    
    SoLazyElement *curElt = SoLazyElement::getInstance(state);
    if (*color != curElt->ivState.ambientColor){
	getWInstance(state)->setAmbientElt(color);
    }  
    else if (state->isCacheOpen()){	    
	curElt->registerRedundantSet(state, AMBIENT_MASK);
    }
}	
///////////////////////////////////////////////////////////////////////
//
// Description: static set() method for emissive color 
//
// use:  public, SoEXTERNAL, static
//
///////////////////////////////////////////////////////////////////////  
void	
SoLazyElement::setEmissive(SoState *state, const SbColor* color)
{
    SoLazyElement *curElt = SoLazyElement::getInstance(state);
    if (*color != curElt->ivState.emissiveColor)
	getWInstance(state)->setEmissiveElt(color);
    else if  (state->isCacheOpen())	    
	curElt->registerRedundantSet(state, EMISSIVE_MASK);
}	
///////////////////////////////////////////////////////////////////////
//
// Description: static set() method for specular color 
//
// use:  public, SoEXTERNAL, static
//
///////////////////////////////////////////////////////////////////////  
void	
SoLazyElement::setSpecular(SoState *state, const SbColor* color)
{
    SoLazyElement *curElt = SoLazyElement::getInstance(state);
    if (*color != curElt->ivState.specularColor)
	getWInstance(state)->setSpecularElt(color);	    
    else if   (state->isCacheOpen()) 	    
	curElt->registerRedundantSet(state, SPECULAR_MASK);
}
///////////////////////////////////////////////////////////////////////
//
// Description: static set() method for shininess 
//
// use:  public, SoEXTERNAL, static
//
///////////////////////////////////////////////////////////////////////  
void	
SoLazyElement::setShininess(SoState *state, float value)
{
    SoLazyElement *curElt = SoLazyElement::getInstance(state);
    if (fabsf(value - curElt->ivState.shininess)> SO_LAZY_SHINY_THRESHOLD)
	getWInstance(state)->setShininessElt(value);
    else if (state->isCacheOpen())
	curElt->registerRedundantSet(state, SHININESS_MASK);
}
///////////////////////////////////////////////////////////////////////
//
// Description: static set() method for blending 
//
// use:  public, SoEXTERNAL, static
//
///////////////////////////////////////////////////////////////////////  
void	
SoLazyElement::setBlending(SoState *state,  SbBool value)
{
    SoLazyElement *curElt = SoLazyElement::getInstance(state);
    if (value != curElt->ivState.blending)
	getWInstance(state)->setBlendingElt( value);
    else if (state->isCacheOpen())
	curElt->registerRedundantSet(state, BLENDING_MASK);
}
///////////////////////////////////////////////////////////////////////
//
// Description: static set() method for light model 
//
// use:  public, SoEXTERNAL, static
//
///////////////////////////////////////////////////////////////////////  
void	
SoLazyElement::setLightModel(SoState *state, const int32_t model)
{
    SoLazyElement *curElt = SoLazyElement::getInstance(state);
    if (model != curElt->ivState.lightModel)
	getWInstance(state)->setLightModelElt(state,  model);
    else if (state->isCacheOpen())
	curElt->registerRedundantSet(state, LIGHT_MODEL_MASK);
}
///////////////////////////////////////////////////////////////////////
//
// Description: static set() method for color material 
//
// use:  public, SoEXTERNAL, static
//
///////////////////////////////////////////////////////////////////////  
void	
SoLazyElement::setColorMaterial(SoState *state, SbBool value)
{
    SoLazyElement *curElt = SoLazyElement::getInstance(state);
    if (value != curElt->ivState.colorMaterial)
	getWInstance(state)->setColorMaterialElt(value);
    else if (state->isCacheOpen())
	curElt->registerRedundantSet(state, COLOR_MATERIAL_MASK);
}
///////////////////////////////////////////////////////////////////////
//
// Description: static set() method for all materials
//
// use:  public, SoEXTERNAL, static
//
///////////////////////////////////////////////////////////////////////  
void	
SoLazyElement::setMaterials(SoState *state,  SoNode* node, 
    uint32_t bitmask, SoColorPacker *cPacker,  
    const SoMFColor& diffuse, const SoMFFloat& transp, const SoMFColor& ambient,
    const SoMFColor& emissive, const SoMFColor& specular, 
    const SoMFFloat& shininess)
{
    uint32_t realSet = 0;    
    SoLazyElement *curElt = SoLazyElement::getInstance(state);
    
    // If we are setting transparency and not diffuse, or vice-versa,
    // then there is a get-dependence:
    if(state->isCacheOpen()){
	uint32_t tempMask = bitmask & (DIFFUSE_MASK | TRANSPARENCY_MASK);
	if (tempMask  && tempMask != (DIFFUSE_MASK | TRANSPARENCY_MASK))
	    curElt->registerGetDependence(state, DIFFUSE_MASK);
    }
    // build a mask (realSet) indicating what really will be set in the state:
    if ((bitmask & EMISSIVE_MASK)&&(emissive[0] != curElt->ivState.emissiveColor))
	realSet |= EMISSIVE_MASK;
    if ((bitmask & SPECULAR_MASK)&&(specular[0] != curElt->ivState.specularColor))
	realSet |= SPECULAR_MASK; 
    if ((bitmask & AMBIENT_MASK)&&(ambient[0] != curElt->ivState.ambientColor))
	realSet |= AMBIENT_MASK;
    if ((bitmask & SHININESS_MASK) &&
	    fabsf(shininess[0] - curElt->ivState.shininess)> 
	    SO_LAZY_SHINY_THRESHOLD) realSet |= SHININESS_MASK;
	    
    uint32_t nodeId = node->getNodeId();
    if ((bitmask & DIFFUSE_MASK) && 
	nodeId != curElt->ivState.diffuseNodeId) realSet |= DIFFUSE_MASK;

    //For transparency nodeid, opaque nodes are identified as nodeId = 0:       
    if(transp.getNum() == 1 && transp[0] == 0.0) nodeId = 0;
    
    if (curElt->ivState.transpNodeId != nodeId && (bitmask & TRANSPARENCY_MASK))  
	realSet |= TRANSPARENCY_MASK;
	
    if (realSet){ 
	curElt = getWInstance(state);
	curElt->setMaterialElt(node, realSet, cPacker,  
	    diffuse, transp, ambient, emissive, specular, shininess);
    }
    //Indicate redundant set for colors that matched the one in the state:
    if (state->isCacheOpen()){ 
	uint32_t notRealSet = bitmask & (~realSet);
	if(notRealSet) curElt->registerRedundantSet(state, notRealSet);
    }  
    
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    set the Diffuse color in the element 
//    virtual, to be overridden
//
// Use: protected
////////////////////////////////////////////////////////////////////////

void
SoLazyElement::setDiffuseElt(SoNode * node,  int32_t numColors,  
	const SbColor *colors, SoColorPacker*)
{

    ivState.diffuseNodeId = node->getNodeId();
    ivState.diffuseColors = colors;
    ivState.numDiffuseColors = numColors;
  
    ivState.packed=FALSE;
    ivState.packedTransparent = FALSE;
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    set the transparency in the element 
//    virtual, to be overridden
//
// Use: protected
////////////////////////////////////////////////////////////////////////

void
SoLazyElement::setTranspElt(SoNode *node, int32_t numTrans, 
    const float *transpar, SoColorPacker* )
{

    ivState.numTransparencies = numTrans;
    ivState.transparencies = transpar;
    ivState.stippleNum = 0;
    if (transpar[0] > 0.0) {
	if (ivState.transpType == SoGLRenderAction::SCREEN_DOOR){
	    ivState.stippleNum =
		(int)(transpar[0]*getNumPatterns());
	}	
    }
    if (numTrans == 1 && transpar[0] == 0.0) ivState.transpNodeId = 0;
	else ivState.transpNodeId = node->getNodeId();
    ivState.packed=FALSE;
    ivState.packedTransparent = FALSE;
  

}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    set the color indices in the element 
//    virtual, to be overridden
//
// Use: protected
////////////////////////////////////////////////////////////////////////

void
SoLazyElement::setColorIndexElt( SoNode * node,  int32_t numIndices,  
	const int32_t *indices)
{

    ivState.diffuseNodeId = node->getNodeId();
    ivState.numDiffuseColors = numIndices;
    ivState.colorIndices = indices;
    ivState.packed=FALSE;
    ivState.packedTransparent = FALSE;

}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    set the transparency type in the element 
//    (not virtual)
//
// Use: protected
////////////////////////////////////////////////////////////////////////

void
SoLazyElement::setTranspTypeElt(  int32_t type)  

{
    ivState.transpType = type;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    set the packed diffuse color in the element 
//    virtual, to be overridden
//
// Use: protected
////////////////////////////////////////////////////////////////////////

void
SoLazyElement::setPackedElt( SoNode *node,  int32_t numColors,  
	const uint32_t* colors)
{  
    ivState.diffuseNodeId   = node->getNodeId();
    ivState.numDiffuseColors = numColors;
    ivState.numTransparencies = numColors;
    ivState.stippleNum = 0;	
    if ((ivState.transpType == SoGLRenderAction::SCREEN_DOOR) &&
	    ((colors[0]&0xff) != 0xff)){ 	
	ivState.stippleNum = (int)(getNumPatterns()*
		(1.-(colors[0] & 0xff)*(1./255.)));	    
    }   
    ivState.packedColors = colors;
    ivState.packed = TRUE;
    ivState.packedTransparent = ((SoPackedColor*)node)->isTransparent();
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    set the Ambient color in the element 
//    virtual, to be overridden
//
// Use: protected
////////////////////////////////////////////////////////////////////////

void
SoLazyElement::setAmbientElt(const SbColor* color )

{
    ivState.ambientColor.setValue((float*)color);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    set the Emissive color in the element 
//    virtual, to be overridden
//
// Use: protected
////////////////////////////////////////////////////////////////////////

void
SoLazyElement::setEmissiveElt(const SbColor* color )

{
    ivState.emissiveColor.setValue((float*)color);
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    set the Specular color in the element 
//    Virtual, to be overridden
//
// Use: protected
////////////////////////////////////////////////////////////////////////

void
SoLazyElement::setSpecularElt(const SbColor* color )
//
{
    ivState.specularColor.setValue((float*)color);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    set the Shininess in the element 
//    virtual, to be overridden 
//
// Use: protected
////////////////////////////////////////////////////////////////////////

void
SoLazyElement::setShininessElt(float value )

{
    ivState.shininess = value;
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    set the ColorMaterial in the element 
//    virtual, to be overridden 
//
// Use: protected
////////////////////////////////////////////////////////////////////////

void
SoLazyElement::setColorMaterialElt( SbBool value )

{
    if (ivState.lightModel == BASE_COLOR) value = FALSE;
    ivState.colorMaterial = value;
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    set the blending enablement in the element 
//    virtual, to be overridden 
//
// Use: protected
////////////////////////////////////////////////////////////////////////

void
SoLazyElement::setBlendingElt(SbBool value )

{
    ivState.blending = value;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    set the light model in the element 
//    virtual, to be overridden 
//
// Use: protected
////////////////////////////////////////////////////////////////////////

void
SoLazyElement::setLightModelElt(SoState *state, int32_t model)

{
    ivState.lightModel = model;
    // also set the shapestyle version of this:
    SoShapeStyleElement::setLightModel(state, model);
    if (model == BASE_COLOR) setColorMaterialElt(FALSE);    
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    set all materials in the element 
//    virtual, to be overridden 
//
// Use: protected
////////////////////////////////////////////////////////////////////////

void
SoLazyElement::setMaterialElt(SoNode* node, uint32_t mask, SoColorPacker*,  
    const SoMFColor& diffuse, const SoMFFloat& transp, 
    const SoMFColor& ambient, const SoMFColor& emissive, 
    const SoMFColor& specular, const SoMFFloat& shininess)
{
    if (mask & DIFFUSE_MASK){
	ivState.diffuseNodeId = node->getNodeId();
	ivState.diffuseColors = diffuse.getValues(0);
	ivState.numDiffuseColors = diffuse.getNum(); 
	ivState.packed=FALSE;
	ivState.packedTransparent = FALSE;
    }
    if (mask&TRANSPARENCY_MASK){
	ivState.numTransparencies = transp.getNum();
	ivState.transparencies = transp.getValues(0);
	ivState.stippleNum = 0;
	if ((ivState.transparencies[0]> 0.0) &&
		(ivState.transpType == SoGLRenderAction::SCREEN_DOOR)) {
	    ivState.stippleNum = 
		(int)(ivState.transparencies[0]*getNumPatterns());
	}
	ivState.packed=FALSE;
	ivState.packedTransparent = FALSE;
    }
    if (mask&AMBIENT_MASK)
	ivState.ambientColor = ambient[0];  
    
    if (mask&EMISSIVE_MASK)
	ivState.emissiveColor = emissive[0];
	
    if (mask&SPECULAR_MASK)
	ivState.specularColor = specular[0];
    
    if (mask&SHININESS_MASK)
	ivState.shininess = shininess[0];  
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pushes element, copies inventor state
//
// public, virtual
////////////////////////////////////////////////////////////////////////

void
SoLazyElement::push(SoState *)
{
    SoLazyElement *prevElt = (SoLazyElement*)getNextInStack();
  
    ivState = prevElt->ivState;
   
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Prints element for debugging.
//
// Use: public
//
////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
void
SoLazyElement::print(FILE *fp) const
{
    SoElement::print(fp);
}
#else  /* DEBUG */
void
SoLazyElement::print(FILE *) const
{
}
#endif /* DEBUG */
/////////////////////////////////////////////////////////////////////////
//
// Description:
//  matches is not used by this element:
//
/////////////////////////////////////////////////////////////////////////
SbBool
SoLazyElement::matches(const SoElement*) const
{
#ifdef DEBUG
    SoDebugError::post("SoLazyElement::matches", 
	    "Should never be called\n");
#endif
    return TRUE;
}
/////////////////////////////////////////////////////////////////////////
//
// Description:
//  copyMatchInfo is not used by this element:
//
/////////////////////////////////////////////////////////////////////////
SoElement*
SoLazyElement::copyMatchInfo() const
{
#ifdef DEBUG
    SoDebugError::post("SoLazyElement::copyMatchInfo", 
	    "Should never be called\n");
#endif
    return NULL;
}
/////////////////////////////////////////////////////////////////////////////
//
//  Description:
//
//  virtual method does no work (GL version does more)
//
////////////////////////////////////////////////////////////////////////////
void
SoLazyElement::registerRedundantSet(SoState* , uint32_t)
{
}
/////////////////////////////////////////////////////////////////////////////
//
//  Description:
//
//  virtual method does no work (GL version does more)
//
////////////////////////////////////////////////////////////////////////////
void
SoLazyElement::registerGetDependence(SoState* , uint32_t)
{
}
///////////////////////////////////////////////////////////////////////////
//
// class:  SoColorPacker
//
// Maintains a packed color array to store current colors.  Intended to
// be used by all property nodes that can issue setDiffuse or setTransparency
/////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//
// Description:
//  constructor for  SoColorPacker
//
// use: public
//
////////////////////////////////////////////////////////////////////////////
SoColorPacker::
SoColorPacker()
{
    packedColors = NULL;
    packedArraySize = 0;
    //Assign nodeids that can never occur in practice:
    diffuseNodeId = transpNodeId = 2;
}

/////////////////////////////////////////////////////////////////////////////
//
// Description:
//  destructor
//
//  use: public
////////////////////////////////////////////////////////////////////////////
SoColorPacker::~SoColorPacker()
{
    if(packedColors != NULL) delete [] packedColors;
}

////////////////////////////////////////////////////////////////////////////
//
// Description:
//
//  reallocate packed color array for SoColorPacker
//
// use: public, SoINTERNAL
//
////////////////////////////////////////////////////////////////////////////
void
SoColorPacker::reallocate(int32_t size)
{
    if (packedColors != NULL) delete [] packedColors;
    packedColors = new uint32_t[size];
    packedArraySize = size;
}
