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
 |   $Revision: 1.1 $
 |
 |   Classes:
 |	SoGLLazyElement
 |
 |   Author(s)		: Alan Norton, Gavin Bell  
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <GL/gl.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/elements/SoGLLazyElement.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/elements/SoSubElement.h>
#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/caches/SoGLRenderCache.h>
#include <Inventor/elements/SoGLCacheContextElement.h>
#include <Inventor/nodes/SoPackedColor.h>
#include <Inventor/fields/SoMFColor.h>
#include <Inventor/fields/SoMFFloat.h>
#include <SoDebug.h>

SO_ELEMENT_SOURCE(SoGLLazyElement);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: SoINTERNAL public
////////////////////////////////////////////////////////////////////////
SoGLLazyElement::~SoGLLazyElement()
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
SoGLLazyElement::init(SoState *state)
{
    SoLazyElement::init(state);
    // We begin with GL invalid
    // so it gets reset at first send.

    glState.GLAmbient[0]	= -1;
    glState.GLEmissive[0]	= -1;
    glState.GLSpecular[0]	= -1;

    glState.GLAmbient[1]    =	glState.GLAmbient[2]	= 0;
    glState.GLEmissive[1]   =	glState.GLEmissive[2]	= 0;
    glState.GLSpecular[1]   =	glState.GLSpecular[2]	= 0;
    
    glState.GLShininess		= -1;
    glState.GLColorMaterial	= -1;
    glState.GLblending		= -1;

    glState.GLDiffuseNodeId	= 0;
    glState.GLTranspNodeId	= 0;
    //start with stipple undefined transparency
    glState.GLStippleNum	= -1;

    // set the last fields to 1., they will stay that way:
    glState.GLAmbient[3]	= 1.0;
    glState.GLEmissive[3]	= 1.0;
    glState.GLSpecular[3]	= 1.0;
    
    // and lightModel invalid
    glState.GLLightModel       	= -1;

    // Also, begin with GL and Inventor out of synch:
    invalidBits			= ALL_MASK;
    // and nothing sent to GL
    GLSendBits			= 0;   
     
    // Determine if GL is in color index mode
    GLboolean	b;
    glGetBooleanv(GL_RGBA_MODE, &b);
    colorIndex = ! b;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pushes element, copies GL state as needed:
//
// use: SoINTERNAL public
////////////////////////////////////////////////////////////////////////
void
SoGLLazyElement::push(SoState *)
{
    SoGLLazyElement *prevElt = (SoGLLazyElement*)getNextInStack();
     
    // The push always happens before a  true set()
    
    ivState = prevElt->ivState;        
    glState = prevElt->glState;
    
    colorIndex		    = prevElt->colorIndex;      
    invalidBits		    = prevElt->invalidBits;
    GLSendBits		    = 0;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pops element, causing side effects in GL.
//
//
// Use: SoINTERNAL public
////////////////////////////////////////////////////////////////////////
void
SoGLLazyElement::pop(SoState *, const SoElement *prevTopElement)
{
    // 
    //Copy all GL parts back from previous top element.
    //Mark those that changed with invalidBits.
    
    SoGLLazyElement* prevTop = (SoGLLazyElement*)prevTopElement;
    uint32_t sendBits = prevTop->GLSendBits;
    
    //merge GLSendBits into parent element
    GLSendBits |= sendBits;
    
    //mark corresponding parts as invalid:
    invalidBits |= sendBits;
    
    //copy GL state:
    glState = prevTop->glState;
 

}

/////////////////////////////////////////////////////////////////////////
//
// Description:  set Diffuse color in element
// virtual method for GL rendering
//
// Use: private, virtual
//
////////////////////////////////////////////////////////////////////////
void
SoGLLazyElement::setDiffuseElt(SoNode *node,  
    int32_t numColors, const SbColor* colors, SoColorPacker *cPacker)
{
    if (colorIndex) return;
    ivState.diffuseColors = colors;
    ivState.numDiffuseColors = numColors;        
    ivState.diffuseNodeId = (node->getNodeId()); 
    ivState.packed = FALSE;
    ivState.packedTransparent = FALSE;

    //Pack the colors and transparencies, if necessary:
    if(!cPacker->diffuseMatch(ivState.diffuseNodeId) ||
	    (!cPacker->transpMatch(ivState.transpNodeId)))
	packColors(cPacker);
	    
    ivState.packedColors = cPacker->getPackedColors();
			   
    // for open caches,  record the fact that set was called:
    ivState.cacheLevelSetBits |= DIFFUSE_MASK;

    if (ivState.diffuseNodeId != glState.GLDiffuseNodeId)
        invalidBits |= DIFFUSE_MASK;

  }
  
/////////////////////////////////////////////////////////////////////////
//
// Description:  set transparency type
// virtual method for GL rendering
//
// Use: private, virtual
//
////////////////////////////////////////////////////////////////////////
void
SoGLLazyElement::setTranspTypeElt(int32_t type)
{
    if(ivState.transpType != type){
	ivState.transpType = type;
	// make sure transparencies send:
	invalidBits |= TRANSPARENCY_MASK;
	glState.GLStippleNum = -1;
    }
}  
  
/////////////////////////////////////////////////////////////////////////
//
// Description:  set transparency in element
// virtual method for GL rendering
//
// Use: private, virtual
//
////////////////////////////////////////////////////////////////////////
void
SoGLLazyElement::setTranspElt(SoNode *node,  
    int32_t numTrans, const float *trans, SoColorPacker *cPacker)
{
    ivState.numTransparencies = numTrans;
    ivState.transparencies = trans;

    // determine the appropriate stippleNum:
    // set to zero unless have nonzero stipple transparency.

    ivState.stippleNum = 0;
    if (trans[0] > 0.0) {
	if (ivState.transpType == SoGLRenderAction::SCREEN_DOOR){
	    ivState.stippleNum =
		(int)(trans[0]*getNumPatterns());
	}	
    }
    if (numTrans == 1 && trans[0] == 0) ivState.transpNodeId = 0;
    else ivState.transpNodeId = node->getNodeId();
    ivState.packed = FALSE;
    ivState.packedTransparent = FALSE;
    if(!cPacker->diffuseMatch(ivState.diffuseNodeId) ||
	    (!cPacker->transpMatch(ivState.transpNodeId)))
	packColors(cPacker);
	    
    ivState.packedColors = cPacker->getPackedColors();
			   
    // for open caches, record the fact that set was called:
    ivState.cacheLevelSetBits |= (TRANSPARENCY_MASK|DIFFUSE_MASK);

    if (ivState.transpNodeId != glState.GLTranspNodeId) 
	invalidBits |= DIFFUSE_MASK;
	
    if (ivState.stippleNum != glState.GLStippleNum)
        invalidBits |= TRANSPARENCY_MASK;
    else invalidBits &= ~TRANSPARENCY_MASK;

  }
/////////////////////////////////////////////////////////////////////////
//
// Description:  set color indices in element
// virtual method for GL rendering
//
// Use: private, virtual
//
////////////////////////////////////////////////////////////////////////
void
SoGLLazyElement::setColorIndexElt(SoNode *node, 
	 int32_t numIndices, const int32_t *indices)
{
    if (!colorIndex) return;
    ivState.numDiffuseColors = numIndices;
    ivState.colorIndices = indices;
      
    ivState.diffuseNodeId = (node->getNodeId()); 
    ivState.packed = FALSE;
    ivState.packedTransparent = FALSE;

    // for open caches, record the fact that set was called:
    ivState.cacheLevelSetBits |= DIFFUSE_MASK;

    if (ivState.diffuseNodeId != glState.GLDiffuseNodeId)
        invalidBits |= DIFFUSE_MASK;
    else invalidBits &= ~DIFFUSE_MASK;

}

/////////////////////////////////////////////////////////////////////////
//
// Description:  set Packed diffuse color in element
//		 sets both diffuse color and transparency.
// virtual method for GL rendering
//
// Use: private, virtual
//
////////////////////////////////////////////////////////////////////////
void
SoGLLazyElement::setPackedElt(SoNode *node, int32_t numColors, 
    const uint32_t* colors)
{
    if (colorIndex) return;
    ivState.packedColors	= colors;
    ivState.numDiffuseColors	= numColors;
    ivState.numTransparencies	= numColors;
    ivState.diffuseNodeId	= node->getNodeId();
    ivState.transpNodeId	= node->getNodeId();
    
    ivState.stippleNum = 0;	
    if ((colors[0]&0xff) != 0xff){ 	
	if(ivState.transpType == SoGLRenderAction::SCREEN_DOOR){
	    ivState.stippleNum = (int)(getNumPatterns()*
		(1.-(colors[0] & 0xff)*(1./255.)));
	}	    
    }
    
    ivState.packed		= TRUE;
    ivState.packedTransparent	= ((SoPackedColor*) node)->isTransparent();
     
    // For open caches, record the fact that set was called:
    ivState.cacheLevelSetBits |= (DIFFUSE_MASK|TRANSPARENCY_MASK);	
 
    if (invalidBits & (DIFFUSE_MASK|TRANSPARENCY_MASK)) return;

    if (ivState.diffuseNodeId != glState.GLDiffuseNodeId ||
	ivState.transpNodeId != glState.GLTranspNodeId)
        invalidBits |= DIFFUSE_MASK;
    else invalidBits &= ~DIFFUSE_MASK;
    
    if (ivState.stippleNum != glState.GLStippleNum)
        invalidBits |= TRANSPARENCY_MASK;
    else invalidBits &= ~TRANSPARENCY_MASK;

}

//////////////////////////////////////////////////////////////////////////
//
// Description:  set Ambient color in element
// virtual method for GL rendering
//
// Use: private, virtual
//
/////////////////////////////////////////////////////////////////////////
void
SoGLLazyElement::setAmbientElt(const SbColor* color)
{
    ivState.ambientColor.setValue((float*)color);

    // For open caches, record the fact that set was called:
    ivState.cacheLevelSetBits |= AMBIENT_MASK;	


    for(int i=0; i<3; i++){
	if (ivState.ambientColor[i] != glState.GLAmbient[i]){
	    invalidBits |= AMBIENT_MASK;
	    return;
	}
    }
    invalidBits &= ~AMBIENT_MASK;
}

/////////////////////////////////////////////////////////////////////////
//
// Description:  set emissive color in element
// virtual method for GL rendering
//
// Use: private, virtual
//
/////////////////////////////////////////////////////////////////////////
void
SoGLLazyElement::setEmissiveElt(const SbColor* color)
{
    ivState.emissiveColor.setValue((float*)color);

    // For open caches, record the fact that set was called:
    ivState.cacheLevelSetBits |= EMISSIVE_MASK;	
 
    for(int i=0; i<3; i++){
	if (ivState.emissiveColor[i] != glState.GLEmissive[i]){
	    invalidBits |= EMISSIVE_MASK;
	    return;
	}
    }
    invalidBits &= ~EMISSIVE_MASK;
}
/////////////////////////////////////////////////////////////////////////
//
// Description:  set specular color in element
// virtual method for GL rendering
//
// Use: private, virtual
//
/////////////////////////////////////////////////////////////////////////
void
SoGLLazyElement::setSpecularElt(const SbColor* color)
{
    ivState.specularColor.setValue((float*)color);

    // For open caches, record the fact that set was called:
    ivState.cacheLevelSetBits |= SPECULAR_MASK;	

    for(int i=0; i<3; i++){
	if (ivState.specularColor[i] != glState.GLSpecular[i]){
	    invalidBits |= SPECULAR_MASK;
	    return;
	}
    }
    invalidBits &= ~SPECULAR_MASK;
}

/////////////////////////////////////////////////////////////////////////
//
// Description:  set shininess in element
// virtual method for GL rendering
//
// Use: private, virtual
//
/////////////////////////////////////////////////////////////////////////
void
SoGLLazyElement::setShininessElt(float value)
{
    ivState.shininess = value;

    // For open caches, record the fact that set was called:
    ivState.cacheLevelSetBits |= SHININESS_MASK;	

    // set invalid bit based on value
    if (fabsf(ivState.shininess - glState.GLShininess) > SO_LAZY_SHINY_THRESHOLD)
	    invalidBits |= SHININESS_MASK;
	else invalidBits &= ~SHININESS_MASK;
    return;
}
/////////////////////////////////////////////////////////////////////////
//
// Description:  set colorMaterial in element
// virtual method for GL rendering
//
// Use: private, virtual
//
/////////////////////////////////////////////////////////////////////////
void
SoGLLazyElement::setColorMaterialElt(SbBool value)
{
    // don't turn on colorMaterial if lighting off:
    if (ivState.lightModel == BASE_COLOR) value = FALSE;
    ivState.colorMaterial = value;

    // For open caches, record the fact that set was called:
	ivState.cacheLevelSetBits |= COLOR_MATERIAL_MASK;	
 

    // set invalid bit based on value
    if (ivState.colorMaterial != glState.GLColorMaterial)
	    invalidBits |= COLOR_MATERIAL_MASK;
	else invalidBits &= ~COLOR_MATERIAL_MASK;
    return;
}
/////////////////////////////////////////////////////////////////////////
//
// Description:  set lightModel in element
// virtual method for GL rendering
//
// Use: private, virtual
//
/////////////////////////////////////////////////////////////////////////
void
SoGLLazyElement::setLightModelElt(SoState *state, int32_t value)
{
    ivState.lightModel = value;

    // For Open caches, record the fact that set was called:
	ivState.cacheLevelSetBits |= LIGHT_MODEL_MASK;	

    // also set the shapestyle version of this:
    SoShapeStyleElement::setLightModel(state, value);

    // set invalid bit based on value
    if (ivState.lightModel != glState.GLLightModel)
	    invalidBits |= LIGHT_MODEL_MASK;
	else invalidBits &= ~LIGHT_MODEL_MASK;
    // set Color Material off if necessary:
    if (ivState.lightModel == BASE_COLOR)
	setColorMaterialElt(FALSE);
    return;
}
/////////////////////////////////////////////////////////////////////////
//
// Description:  set blending in element
// virtual method for GL rendering
//
// Use: private, virtual
//
/////////////////////////////////////////////////////////////////////////
void
SoGLLazyElement::setBlendingElt(SbBool value)
{

    ivState.blending = value;

    // For open caches, record the fact that set was called:
    ivState.cacheLevelSetBits |= BLENDING_MASK;	
 
    // set invalid bit based on value
    if (ivState.blending != glState.GLblending)
	invalidBits |= BLENDING_MASK;
    else invalidBits &= ~BLENDING_MASK;

    return;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    set all materials in the element 
//    special method for SoMaterial nodes 
//
// Use: private
////////////////////////////////////////////////////////////////////////

void
SoGLLazyElement::setMaterialElt(SoNode* node, uint32_t mask,
    SoColorPacker *cPacker,  
    const SoMFColor& diffuse, const SoMFFloat& transp, 
    const SoMFColor& ambient, const SoMFColor& emissive, 
    const SoMFColor& specular, const SoMFFloat& shininess)
{
    if ((mask & DIFFUSE_MASK) && !colorIndex ){
	ivState.diffuseColors = diffuse.getValues(0);
	ivState.numDiffuseColors = diffuse.getNum();        
	ivState.diffuseNodeId = (node->getNodeId()); 
	ivState.packed = FALSE;
	ivState.packedTransparent = FALSE;	
    }
    
    if (mask & TRANSPARENCY_MASK){
        ivState.numTransparencies = transp.getNum();
	ivState.transparencies = transp.getValues(0);    
	ivState.stippleNum = 0;
	if (ivState.numTransparencies == 1 && ivState.transparencies[0] == 0.0)
	    ivState.transpNodeId = 0;
	else { 
	    ivState.transpNodeId = node->getNodeId();
	    if (ivState.transparencies[0] != 0.0 &&	   
		    ivState.transpType == SoGLRenderAction::SCREEN_DOOR){
		ivState.stippleNum =
		    (int)(ivState.transparencies[0]*getNumPatterns());
	    }	
	}
	    
	if  (ivState.stippleNum != glState.GLStippleNum)
	    invalidBits |= TRANSPARENCY_MASK;
	else invalidBits &= ~TRANSPARENCY_MASK;
    }
    
    // do combined packed color/transparency work:
    if (mask & (DIFFUSE_MASK|TRANSPARENCY_MASK)){
	if(!cPacker->diffuseMatch(ivState.diffuseNodeId) ||
		(!cPacker->transpMatch(ivState.transpNodeId)))
	    packColors(cPacker);
	 
	if (ivState.diffuseNodeId != glState.GLDiffuseNodeId ||
		ivState.transpNodeId != glState.GLTranspNodeId)
	    invalidBits |= DIFFUSE_MASK;
	else invalidBits &= ~DIFFUSE_MASK;	
	    
	ivState.packedColors = cPacker->getPackedColors(); 
    }
       
    if (mask & AMBIENT_MASK){
	ivState.ambientColor = ambient[0];
	invalidBits &= ~AMBIENT_MASK;
        for (int i=0; i<3; i++){
	    if (ivState.ambientColor[i] != glState.GLAmbient[i]){
		invalidBits |= AMBIENT_MASK;
		break;
	    }
	}   
    }  
    
    if (mask & EMISSIVE_MASK){
	ivState.emissiveColor = emissive[0];
	invalidBits &= ~EMISSIVE_MASK;
        for (int i=0; i<3; i++){
	    if (ivState.emissiveColor[i] != glState.GLEmissive[i]){
		invalidBits |= EMISSIVE_MASK;
		break;
	    }
	}   
    }
	
    if (mask & SPECULAR_MASK){
	ivState.specularColor = specular[0];
	invalidBits &= ~SPECULAR_MASK;
        for (int i=0; i<3; i++){
	    if (ivState.specularColor[i] != glState.GLSpecular[i]){
		invalidBits |= SPECULAR_MASK;
		break;
	    }
	}   
    }
    
    if (mask & SHININESS_MASK){
	ivState.shininess = shininess[0];
	if (fabsf(ivState.shininess - glState.GLShininess) 
		> SO_LAZY_SHINY_THRESHOLD){
	    invalidBits |= SHININESS_MASK;
	}      
	else invalidBits &= ~SHININESS_MASK;       
    }
    // For open caches, record the fact that set was called:
    ivState.cacheLevelSetBits |= mask;	  
}
//////////////////////////////////////////////////////////////////////////
//
// Description:
//  
//  Send a packed color to the GL, update GLstate  of current
//  GLLazyElement to indicate it.  Also do the right thing for
//  stipple transparency.
//  This is special method for sending the first color from a
//  vertexProperty node, and making that send consistent with the
//  transparency state.
//  MUST be invoked immediately after initial SoGLLazyElement::send.
//  AND that send must issue a lazy send of both diffuse color and transparency
//  or otherwise cache problems will result.
//
//  use: SoInternal, public
////////////////////////////////////////////////////////////////////////////
//
void
SoGLLazyElement::sendVPPacked(SoState* state, const unsigned char* pcolor)
{
    if (glState.GLColorMaterial || 
	(glState.GLLightModel == BASE_COLOR))
        glColor4ubv((const GLubyte*)pcolor);
    else{
        float col4[4];
	col4[3] = (pcolor[3]) * 1.0/255;
	col4[2] = (pcolor[2]) * 1.0/255;
	col4[1] = (pcolor[1]) * 1.0/255;
	col4[0] = (pcolor[0]) * 1.0/255;
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, col4);
    }
    
    //Remember what things we are sending to GL, they have to be invalidated:
    uint32_t sendMask = DIFFUSE_MASK;
    
    //Now see about sending transparency:

    // If Stipple is not being used, nothing to be done for transparency.
    if (ivState.transpType == SoGLRenderAction::SCREEN_DOOR){
    
	uint32_t trans = pcolor[3];
    
	// If transparency was off and is still off, can quit here    
	if (glState.GLStippleNum != 0 || trans != 0xff) {

	    // Setup GL stipple with the new value being specified		  
	    int newStipNum = (int)(getNumPatterns()*(1.-trans*(1.0/255.)));
	    if (newStipNum != glState.GLStippleNum){
	    
		// a real send is occurring: 
		sendMask |= TRANSPARENCY_MASK;   	       		
			
		if (newStipNum >0){
		    sendStipple(state, newStipNum);
		    // if previously was off, turn on GLStipple
		    if (glState.GLStippleNum <= 0)
			glEnable(GL_POLYGON_STIPPLE);
		} 
		else{
		    glDisable(GL_POLYGON_STIPPLE);			    
		}
		glState.GLStippleNum = newStipNum;
	
	    }
	}
    }
    
    //Now reset/invalidate the diffuse and possibly transparency components.
 
    reset(state, sendMask);
}
/////////////////////////////////////////////////////////////////////////
//
// Description:
//
// Register with the cache that a redundant set was called.
// Cache must know that a set was called, even if the set is redundant.
// Must only be invoked if cache is open.
//
// use: virtual, private, SoInternal
////////////////////////////////////////////////////////////////////////////
void
SoGLLazyElement::registerRedundantSet(SoState* state, uint32_t bitmask)
{
#ifdef DEBUG
    if (!(state->isCacheOpen())){
    	SoDebugError::post("SoGLLazyElement::registerRedundantSet",
			   "Cache is not open!"); 
    }
#endif /*DEBUG*/
    //Must obtain a writable version of element (i.e. may need to push)
    SoGLLazyElement *le = (SoGLLazyElement*)getWInstance(state);
    le->ivState.cacheLevelSetBits |= bitmask;
 
}
/////////////////////////////////////////////////////////////////////////
//
// Description:
//
// Register with the cache that a get() was called.
// Must only be invoked if cache is open.
//
// use: virtual, private, SoINTERNAL
////////////////////////////////////////////////////////////////////////////
void
SoGLLazyElement::registerGetDependence(SoState* state, uint32_t bitmask)
{
#ifdef DEBUG
    if (!(state->isCacheOpen())){
    	SoDebugError::post("SoGLLazyElement::registerGetDependence",
			   "Cache is not open!"); 
    }
#endif /*DEBUG*/
    SoGLRenderCache* thisCache = (SoGLRenderCache*)
	    SoCacheElement::getCurrentCache((SoState*)state);
    SoGLLazyElement* cacheLazyElt = thisCache->getLazyElt();
    uint32_t levelSetBits = ivState.cacheLevelSetBits;
    uint32_t didRealSendBits = cacheLazyElt->GLSendBits;
	
    //If there was no set and no real send, there is an IV dependence:
    uint32_t checkIV = bitmask & (~didRealSendBits) & (~levelSetBits);

    if (checkIV) { 
	copyIVValues(checkIV,cacheLazyElt);
	thisCache->setLazyBits(checkIV, 0, 0);
    } 
}

/////////////////////////////////////////////////////////////////////////
//
// Description:
//
// reset the element to have no knowledge of GL state
//
// use: public
/////////////////////////////////////////////////////////////////////////
void
SoGLLazyElement::reset(SoState* state, uint32_t bitmask) const
{
    //cast away const...
    SoGLLazyElement *le = (SoGLLazyElement *)this;
    le->invalidBits |= bitmask;
    //Set GLSendBits, so invalidation will persist after pop().
    le->GLSendBits |= bitmask;
     
    //  If cache is open, we must assume that there has been a GLSend
    //  of the component being reset, and set the corresponding GLSendBit.
    if (state->isCacheOpen()){
	SoGLRenderCache* thisCache = (SoGLRenderCache*)
		SoCacheElement::getCurrentCache((SoState*)state);
	SoGLLazyElement* cacheLazyElt = thisCache->getLazyElt();
	cacheLazyElt->GLSendBits |= bitmask;
    }
    	
    for(int j=0; (j< SO_LAZY_NUM_COMPONENTS) && bitmask; j++, bitmask >>=1){ 
	if (bitmask&1) {
	    switch(j) {
		case(LIGHT_MODEL_CASE):
		    le->glState.GLLightModel = -1;
		    break;
		    	
		case(COLOR_MATERIAL_CASE):
		    le->glState.GLColorMaterial = -1;
		    break;
		
		case(DIFFUSE_CASE):
		    le->glState.GLDiffuseNodeId = 1;
		    le->glState.GLTranspNodeId = 1;
		    break;
		    
		case(AMBIENT_CASE):
    		    le->glState.GLAmbient[0]= -1;		   		    
		    break;

		case(EMISSIVE_CASE):
		    le->glState.GLEmissive[0] = -1;
		    break;
		 
		case(SPECULAR_CASE):
		    le->glState.GLSpecular[0] = -1;
		    break;
    		
		case(SHININESS_CASE):
		    le->glState.GLShininess = -1;
		    break;		    
		    
		case(TRANSPARENCY_CASE):
		    le->glState.GLStippleNum = -1;
		    break;
		    
		case(BLENDING_CASE):
		    le->glState.GLblending = -1;
		    break;		    

    	    }
	}
   }
   return;
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//  used to test whether a cache can be used.  Invoked by lazyMatches.
//  
//  Compares element in state against cached element, looking for
//  matches in specified GL and/or IV state
//   
//
// Use: private
/////////////////////////////////////////////////////////////////////////

SbBool
SoGLLazyElement::fullLazyMatches(uint32_t checkGL, uint32_t checkIV, 
	const SoGLLazyElement *stateLazyElt) 

{
    // check to make sure transparency type has not changed:
    if(ivState.transpType != stateLazyElt->ivState.transpType){
#ifdef DEBUG
	if (SoDebug::GetEnv("IV_DEBUG_CACHES")) {
	    fprintf(stderr, "CACHE DEBUG: cache not valid\n ");       
	    fprintf(stderr, "transparency type match failed,\n");
	    fprintf(stderr, "prev,  current %d %d\n", 
	    ivState.transpType, stateLazyElt->ivState.transpType);
	}
#endif /*DEBUG*/       
	return FALSE;
    }
    
    int i;
    uint32_t bitmask = checkIV;
 
    for(i=0; (i< SO_LAZY_NUM_COMPONENTS)&&bitmask; i++,bitmask>>=1){

    	if (bitmask & 1){

            int j;
            switch(i){
		case(LIGHT_MODEL_CASE):
		    if (ivState.lightModel != stateLazyElt->ivState.lightModel){
#ifdef DEBUG
			if (SoDebug::GetEnv("IV_DEBUG_CACHES")) {
			    fprintf(stderr, "CACHE DEBUG: cache not valid\n ");       
			    fprintf(stderr, "lightModel match failed,\n");
			    fprintf(stderr, "prev,  current %d %d\n", 
				ivState.lightModel, 
				stateLazyElt->ivState.lightModel);
			}
#endif /*DEBUG*/	   
			return FALSE;
		    }
		    break;
		    		     
		case(COLOR_MATERIAL_CASE):
		    if (ivState.colorMaterial != 
			stateLazyElt->ivState.colorMaterial){
#ifdef DEBUG
			if (SoDebug::GetEnv("IV_DEBUG_CACHES")) {
			    fprintf(stderr, "CACHE DEBUG: cache not valid\n ");       
			    fprintf(stderr, "colorMaterial match failed,\n");
			    fprintf(stderr, "prev,  current %d %d\n", 
				ivState.colorMaterial, 
				stateLazyElt->ivState.colorMaterial);
			}
#endif /*DEBUG*/	   
			return FALSE;
		    }			
		    break;
		    
		case(DIFFUSE_CASE):
		    if (ivState.diffuseNodeId != 
			stateLazyElt->ivState.diffuseNodeId ||
			ivState.transpNodeId !=
			stateLazyElt->ivState.transpNodeId){
#ifdef DEBUG
			if (SoDebug::GetEnv("IV_DEBUG_CACHES")) {
			    fprintf(stderr, "CACHE DEBUG: cache not valid\n");       
			    fprintf(stderr, "diffuse&trans match failed,\n");
			    fprintf(stderr, "prev,  current %d %d, %d %d\n", 
				ivState.diffuseNodeId, ivState.transpNodeId,  
				stateLazyElt->ivState.diffuseNodeId, 
				stateLazyElt->ivState.transpNodeId);
			}
#endif /*DEBUG*/	   		    
			return (FALSE);
		    }
		    break;		    		    
	    	case(AMBIENT_CASE):
	            for(j=0; j<3; j++){
		    	if (ivState.ambientColor[j]!=
			    stateLazyElt->ivState.ambientColor[j]){
#ifdef DEBUG
			    if (SoDebug::GetEnv("IV_DEBUG_CACHES")) {
			       fprintf(stderr, "CACHE DEBUG: cache not valid\n");       
			       fprintf(stderr, "ambient %d match failed,\n", j);
			       fprintf(stderr, "prev,  current %f %f\n", 
				   ivState.ambientColor[j], 
				   stateLazyElt->ivState.ambientColor[j]);
			    }
#endif /*DEBUG*/				 
			    return(FALSE);
			}
  	            }	
	            break;

	    	case(EMISSIVE_CASE):
	    	    for(j=0; j<3; j++){
		    	if (ivState.emissiveColor[j]!=
			    stateLazyElt->ivState.emissiveColor[j]){
#ifdef DEBUG
			    if (SoDebug::GetEnv("IV_DEBUG_CACHES")) {
			       fprintf(stderr, "CACHE DEBUG: cache not valid\n");      
			       fprintf(stderr, "emissive %d match failed,\n", j);
			       fprintf(stderr, "prev,  current %f %f\n", 
				    ivState.emissiveColor[j], 
				    stateLazyElt->ivState.emissiveColor[j]);
			    }
#endif /*DEBUG*/			 
		    	    return(FALSE);
			}
  	            }	
	    	    break;

	    	case(SPECULAR_CASE):
	            for(j=0; j<3; j++){
		    	if (ivState.specularColor[j]!=
			    stateLazyElt->ivState.specularColor[j]){
#ifdef DEBUG
			    if (SoDebug::GetEnv("IV_DEBUG_CACHES")) {
			       fprintf(stderr, "CACHE DEBUG: cache not valid\n");       
			       fprintf(stderr, "specular %d match failed,\n", j);
			       fprintf(stderr, "prev,  current %f %f\n", 
				    ivState.specularColor[j], 
				    stateLazyElt->ivState.specularColor[j]);
			    }
#endif /*DEBUG*/				 
		             return(FALSE);
			}
  	    	    }	
	    	    break;

	    	case(SHININESS_CASE):
	            if (fabsf(ivState.shininess - stateLazyElt->ivState.shininess)
			>  SO_LAZY_SHINY_THRESHOLD){
#ifdef DEBUG
			if (SoDebug::GetEnv("IV_DEBUG_CACHES")) {
			    fprintf(stderr, "CACHE DEBUG: cache not valid\n");       
			    fprintf(stderr, "shininess match failed,\n");
			    fprintf(stderr, "prev,  current %f %f\n", 
				ivState.shininess, 
				stateLazyElt->ivState.shininess);
			}
#endif /*DEBUG*/	   			  
		        return (FALSE);
		    }
	    	    break;
		     
		case(BLENDING_CASE):
		    if (ivState.blending != stateLazyElt->ivState.blending){
#ifdef DEBUG
			if (SoDebug::GetEnv("IV_DEBUG_CACHES")) {
			    fprintf(stderr, "CACHE DEBUG: cache not valid\n");       
			    fprintf(stderr, "blend match failed,\n");
			    fprintf(stderr, "prev,  current %d %d\n", 
				ivState.blending, 
				stateLazyElt->ivState.blending);
			}
#endif /*DEBUG*/		    
			return FALSE;
		    }
		    break;
		    

		case(TRANSPARENCY_CASE):			   		    
		    if (ivState.stippleNum != 
			    stateLazyElt->ivState.stippleNum){			
#ifdef DEBUG
			if (SoDebug::GetEnv("IV_DEBUG_CACHES")) {
			    fprintf(stderr, "CACHE DEBUG: cache not valid\n");       
			    fprintf(stderr, "IVstipple match failed,\n");
			    fprintf(stderr, "prev,  current %d %d\n", 
				ivState.stippleNum, 
				stateLazyElt->ivState.stippleNum);
			}
#endif /*DEBUG*/			
			return FALSE;
		    }
		    break;
		        	
	    	default:
#ifdef DEBUG
	        SoDebugError::post("SoGLLazyElement::matches",
				   "Invalid component of element"); 
#endif /*DEBUG*/
		break;
            } 
        }
    }
    
    if (!(bitmask = checkGL)) return TRUE;
 
    for(i=0;  (i< SO_LAZY_NUM_COMPONENTS)&&bitmask; i++,bitmask>>=1){

    	if (bitmask & 1){

            int j;
            switch(i){
		case(LIGHT_MODEL_CASE):
		    if (glState.GLLightModel != 
			stateLazyElt->glState.GLLightModel){
#ifdef DEBUG
			if (SoDebug::GetEnv("IV_DEBUG_CACHES")) {
			    fprintf(stderr, "CACHE DEBUG: cache not valid\n ");       
			    fprintf(stderr, "GLLightModel match failed,\n");
			    fprintf(stderr, "prev,  current %d %d\n", 
				glState.GLLightModel, 
				stateLazyElt->glState.GLLightModel);
			}
#endif /*DEBUG*/		    
			return FALSE;
		    }
		    break;
		    	     
		case(COLOR_MATERIAL_CASE):
		    if (glState.GLColorMaterial != 
			stateLazyElt->glState.GLColorMaterial){
#ifdef DEBUG
			if (SoDebug::GetEnv("IV_DEBUG_CACHES")) {
			    fprintf(stderr, "CACHE DEBUG: cache not valid\n");       
			    fprintf(stderr, "GLColorMaterial match failed,\n");
			    fprintf(stderr, "prev,  current %d %d\n", 
				glState.GLColorMaterial, 
				stateLazyElt->glState.GLColorMaterial);
			}
#endif /*DEBUG*/		    
			return FALSE;
		    }
		    break;

		case(DIFFUSE_CASE):
		    if (glState.GLDiffuseNodeId != 
			stateLazyElt->glState.GLDiffuseNodeId ||
			glState.GLTranspNodeId != 
			stateLazyElt->glState.GLTranspNodeId){
#ifdef DEBUG
			if (SoDebug::GetEnv("IV_DEBUG_CACHES")) {
			    fprintf(stderr, "CACHE DEBUG: cache not valid\n");       
			    fprintf(stderr, "GLDiffuse&Transp match failed,\n");
			    fprintf(stderr, "prev,  current %d %d,  %d %d\n", 
				glState.GLDiffuseNodeId,
				glState.GLTranspNodeId,  
				stateLazyElt->glState.GLDiffuseNodeId, 
				stateLazyElt->glState.GLTranspNodeId);
			}
#endif /*DEBUG*/		    
			return (FALSE);
		    }
		    break;
		    
    	    	case(AMBIENT_CASE):
	            for(j=0; j<3; j++){
		    	if (glState.GLAmbient[j]!=
			    stateLazyElt->glState.GLAmbient[j]) {
#ifdef DEBUG
			    if (SoDebug::GetEnv("IV_DEBUG_CACHES")) {
			       fprintf(stderr, "CACHE DEBUG: cache not valid\n");       
			       fprintf(stderr, "GLambient %d match failed,\n",j);
			       fprintf(stderr, "prev,  current %f %f\n", 
				    glState.GLAmbient[j], 
				    stateLazyElt->glState.GLAmbient[j]);
			    }
#endif /*DEBUG*/			
			    return(FALSE);
			}
  	            }	
	            break;

	    	case(EMISSIVE_CASE):
	    	    for(j=0; j<3; j++){
		    	if (glState.GLEmissive[j]!=
			    stateLazyElt->glState.GLEmissive[j]){ 
#ifdef DEBUG
			    if (SoDebug::GetEnv("IV_DEBUG_CACHES")) {
			      fprintf(stderr, "CACHE DEBUG: cache not valid\n");       
			      fprintf(stderr, "GLemissive %d match failed,\n", j);
			      fprintf(stderr, "prev,  current %f %f\n", 
				    glState.GLEmissive[j], 
				    stateLazyElt->glState.GLEmissive[j]);
			    }
#endif /*DEBUG*/			
		    	    return(FALSE);
			}
  	            }	
	    	    break;

	    	case(SPECULAR_CASE):
	            for(j=0; j<3; j++){
		    	if (glState.GLSpecular[j]!=
			    stateLazyElt->glState.GLSpecular[j]){
#ifdef DEBUG
			    if (SoDebug::GetEnv("IV_DEBUG_CACHES")) {
			       fprintf(stderr, "CACHE DEBUG: cache not valid\n");       
			       fprintf(stderr, "GLspecular %d match failed,\n", j);
			       fprintf(stderr, "prev,  current %f %f\n", 
				    glState.GLSpecular[j], 
				    stateLazyElt->glState.GLSpecular[j]);
			    }
#endif /*DEBUG*/			 
		             return(FALSE);
			}
  	    	    }	
	    	    break;

	    	case(SHININESS_CASE):
	            if (fabsf(glState.GLShininess - 
			stateLazyElt->glState.GLShininess)> 
			    SO_LAZY_SHINY_THRESHOLD){
#ifdef DEBUG
			if (SoDebug::GetEnv("IV_DEBUG_CACHES")) {
			    fprintf(stderr, "CACHE DEBUG: cache not valid\n");       
			    fprintf(stderr, "GLshininess match failed,\n");
			    fprintf(stderr, "prev,  current %f %f\n", 
				glState.GLShininess, 
				stateLazyElt->glState.GLShininess);
			}
#endif /*DEBUG*/			     
		        return (FALSE);
		    }
	    	     break;
		     
	       	case(BLENDING_CASE):
		    if (glState.GLblending != 
			stateLazyElt->glState.GLblending){
#ifdef DEBUG
			if (SoDebug::GetEnv("IV_DEBUG_CACHES")) {
			    fprintf(stderr, "CACHE DEBUG: cache not valid\n");       
			    fprintf(stderr, "GLblending match failed,\n");
			    fprintf(stderr, "prev,  current %d %d\n", 
				glState.GLblending, 
				stateLazyElt->glState.GLblending);
			}
#endif /*DEBUG*/		    
			return FALSE;
		    }
		    break;

		    
		case(TRANSPARENCY_CASE):			
		    if (glState.GLStippleNum !=
			    stateLazyElt->glState.GLStippleNum){			       		       
#ifdef DEBUG
			if (SoDebug::GetEnv("IV_DEBUG_CACHES")) {
			    fprintf(stderr, "CACHE DEBUG: cache not valid\n");       
			    fprintf(stderr, "GLstipple match failed,\n");
			    fprintf(stderr, "prev, current  %d %d\n", 
				glState.GLStippleNum, 
				stateLazyElt->glState.GLStippleNum);
			}
#endif /*DEBUG*/	
			return FALSE;			
		    }
		    break; 
		    
	    	default:
#ifdef DEBUG
	        SoDebugError::post("SoGLLazyElement::matches",
				   "Invalid component of element"); 
#endif /*DEBUG*/
		break;
	    }
	    
        }
    }

    return(TRUE);

}

////////////////////////////////////////////////////////////////////////
//
// Description:
//     Create a copy of this instance suitable for doing cache
//     matching on.  Needs to have bitfields zeroed, and copy the
//     transparency type.
//     and "this" lazy element needs its cacheLevelSetBits zeroed.
//     and its cacheLevelSendBits zeroed.
//
// Use: public,  SoINTERNAL
////////////////////////////////////////////////////////////////////////
SoGLLazyElement *
SoGLLazyElement::copyLazyMatchInfo(SoState *state)
{
//  force a push, so we can set the setBits in a new element.
    SoGLLazyElement* newElt = 
	(SoGLLazyElement *)SoLazyElement::getWInstance(state);
    newElt->ivState.cacheLevelSetBits = 0;
    newElt->ivState.cacheLevelSendBits = 0;    
    SoGLLazyElement *result =
	(SoGLLazyElement *)getTypeId().createInstance();

    result->GLSendBits = 0;
    result->ivState.transpType = newElt->ivState.transpType;    
    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sends to GL only what needs to go: 
//
// Use: private
////////////////////////////////////////////////////////////////////////
void
SoGLLazyElement::reallySend(const SoState *state, uint32_t bitmask) 

//Send only the parts that don't match inventor state: 
// does the following:
//   copies the inventor state to the GL state
//   sends the GL value to GL
//   sets the appropriate invalidBits off.
//   sets the appropriate GLSendBits on.
//
// If a renderCache is open, it does additional work,
// identifying if this is the first send of the component in the cache,
// identifying if it has been set in the cache, etc.
// This information is used to determine whether there is a cache dependency,
// and if so, whether it is dependent on the inventor state, GL state, or
// dependent on inventor matching GL.
{  

    uint32_t sendBits = bitmask & invalidBits;
    uint32_t realSendBits = 0;
    // with base_color, don't send nondiffuse colors.
    if (ivState.lightModel == BASE_COLOR) sendBits &= ~OTHER_COLOR_MASK;

    //everything that was requested to send will be valid afterward, 
   
    invalidBits &= ~(sendBits);
    
    SbBool sendit;

    for(int j = 0; (j< SO_LAZY_NUM_COMPONENTS) && (sendBits != 0); 
		j++,sendBits >>=1){
	if (sendBits & 1){
	    int i;
	    switch(j){
		// Note that lightmodel and colormaterial have to send
		// before diffuse, so that we can force send diffuse
				
		case(LIGHT_MODEL_CASE):
		    if (glState.GLLightModel == ivState.lightModel) break;
		    if (ivState.lightModel == PHONG){
			glEnable(GL_LIGHTING);
			if (colorIndex)glShadeModel(GL_FLAT);
		    }
		    else {
		        glDisable(GL_LIGHTING);
			if (colorIndex) glShadeModel(GL_SMOOTH);
		    }
		    glState.GLLightModel = ivState.lightModel;
		    realSendBits |= LIGHT_MODEL_MASK;
		    //force-send the diffuse color:
		    sendBits |= (DIFFUSE_MASK >> LIGHT_MODEL_CASE);
		    glState.GLDiffuseNodeId = 1;
		    break;
	
		case(COLOR_MATERIAL_CASE):	       	
		    // Handle color material if light model does not change:
		    if (ivState.colorMaterial == glState.GLColorMaterial) break;					    	 		
		    realSendBits |= COLOR_MATERIAL_MASK;	       
		    glState.GLColorMaterial = ivState.colorMaterial;
		    if (ivState.colorMaterial){
			    glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
			    glEnable(GL_COLOR_MATERIAL);
      			}
		    else glDisable(GL_COLOR_MATERIAL);
#ifdef DEBUG
		    if ((glState.GLLightModel == BASE_COLOR) &&
			(glState.GLColorMaterial)){
			    SoDebugError::post("SoGLLazyElement::reallySend", 
			    "ColorMaterial being used with BASE_COLOR");
			}
#endif /*DEBUG*/		  
		    // ensure diffuse color will send:
		    sendBits |= (DIFFUSE_MASK >> COLOR_MATERIAL_CASE);
		    glState.GLDiffuseNodeId = 1;			    
		    break;
	    
		case(DIFFUSE_CASE): 		 
		    // in this case, always send color[0]
		    if (glState.GLDiffuseNodeId == ivState.diffuseNodeId &&
			glState.GLTranspNodeId == ivState.transpNodeId)
			break;
		    realSendBits |= DIFFUSE_MASK;
		    glState.GLDiffuseNodeId = ivState.diffuseNodeId;
		    glState.GLTranspNodeId = ivState.transpNodeId;
		    
		    if(colorIndex){
#ifdef DEBUG
			if (glState.GLLightModel != BASE_COLOR){
			    SoDebugError::post("SoGLLazyElement::reallySend", 
			    "PHONG shading used in colorIndex mode");
			}
#endif					  	
			glIndexi((GLint)ivState.colorIndices[0]);
			break;
		    }
		    float col4[4];		    
		 
		    if (glState.GLColorMaterial || 
			(glState.GLLightModel== BASE_COLOR))
			    glColor4ubv((GLubyte*)ivState.packedColors);	
		    else{		
			col4[3] =  (ivState.packedColors[0] & 
			    0xff)   * 1.0/255;
			col4[2] = ((ivState.packedColors[0] & 
			    0xff00) >>  8) * 1.0/255;
			col4[1] = ((ivState.packedColors[0] & 
			    0xff0000)>> 16) * 1.0/255;
			col4[0] = ((ivState.packedColors[0] & 
			    0xff000000)>>24) * 1.0/255;
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, col4);
		    }		  
		    break;
		    		      
		case(AMBIENT_CASE):     
		    sendit = FALSE;
       	    	    for(i=0; i<3; i++){
			if (glState.GLAmbient[i] != ivState.ambientColor[i]){ 
			    sendit=TRUE;
			    glState.GLAmbient[i]=ivState.ambientColor[i];
			}
		    }
		    if (!sendit) break;
		    realSendBits |= AMBIENT_MASK;
		    glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,
			(GLfloat*)glState.GLAmbient); 
		    break;

		case(EMISSIVE_CASE):
		    sendit = FALSE;
       	    	    for(i=0; i<3; i++){
			if (glState.GLEmissive[i]!=ivState.emissiveColor[i]){
			    sendit = TRUE;
			    glState.GLEmissive[i]=ivState.emissiveColor[i];
			}
		    }
		    if (!sendit) break;
		    realSendBits |= EMISSIVE_MASK;
		    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,
			(GLfloat*)glState.GLEmissive); 
		    break;

		case(SPECULAR_CASE):
		    sendit = FALSE;
       	    	    for(i=0; i<3; i++){
			if (glState.GLSpecular[i]!=ivState.specularColor[i]){
			    sendit = TRUE;
			    glState.GLSpecular[i]=ivState.specularColor[i];
			}
		    }
		    if (!sendit) break;
		    realSendBits |= SPECULAR_MASK;
		    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,
			(GLfloat*)glState.GLSpecular); 
		    break;

		case(SHININESS_CASE):
		    if (fabsf(glState.GLShininess-ivState.shininess)<
			SO_LAZY_SHINY_THRESHOLD) break;
		    realSendBits |= SHININESS_MASK;
		    glState.GLShininess=ivState.shininess;
		    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,
			(GLfloat)glState.GLShininess*128.0); 
		    break;
		    
	   	case(BLENDING_CASE):
		       		    
		    if (glState.GLblending == ivState.blending) break;
		    realSendBits |= BLENDING_MASK;		
		    glState.GLblending = ivState.blending;
		    if (ivState.blending == TRUE){
		        glEnable(GL_BLEND);		     	
		    }
		    // blend is being turned off:
       		    else glDisable(GL_BLEND);				    	      		
		    break;
		  		 
		case(TRANSPARENCY_CASE):		
			
		    if (ivState.stippleNum == glState.GLStippleNum) break;
		    
		    if (ivState.stippleNum == 0){
			glDisable(GL_POLYGON_STIPPLE);
		    }
		    else{
			sendStipple(state, ivState.stippleNum);
			if (glState.GLStippleNum <= 0)
			    glEnable(GL_POLYGON_STIPPLE);
		    }
		    glState.GLStippleNum = ivState.stippleNum;			
	    		    		
		    realSendBits |= TRANSPARENCY_MASK;	       		    
		    break;
	    }
	}
    }
    //Record the real_sends:
    GLSendBits |= realSendBits;
    
    // if cache is open, record required info:
    if (state->isCacheOpen()){
	SoGLRenderCache* thisCache = (SoGLRenderCache*)
		SoCacheElement::getCurrentCache((SoState*)state);
	SoGLLazyElement* cacheLazyElt = thisCache->getLazyElt();
	uint32_t didRealSendBits = cacheLazyElt->GLSendBits;
	uint32_t levelSetBits = ivState.cacheLevelSetBits;
	uint32_t levelSendBits = ivState.cacheLevelSendBits;
	
	// To determine which of doSend, checkGL, and checkIV bits to set,
	// All combinations of the bits
	//	A:  levelSendBits
	//	B:  levelSetBits
	//	C:  didRealSendBits
	//	D:  realSendBits
	// are considered.  The following matrix describes the resulting
	// dependencies:
	//
	//		A=0	    A=1		A=0	    A=1
	//		B=0	    B=0		B=1	    B=1
	//
	//  C=0,D=0	IV=GL	    (IV=GL)	GL	    GL
	//			    
	//  C=0,D=1	IV	    (IV)	OK	    OK
	//
	//  C=1,D=0	IV	    (IV)	OK	    OK
	//
	//  C=1,D=1	IV	    IV		OK	    OK
	//
	//  In the above, OK means no dependence, a dependence in parens
	//  means that the dependence is already implied by another
	//  send in the same scene graph.
		
	// doSend flag indicates dependencies on IV=GL; these are components
	// with no set and a non-real send in the cache.
	uint32_t doSend = bitmask&(~didRealSendBits)&(~realSendBits) 
	    &(~levelSetBits)&(~levelSendBits);
	     	    
	// checkIV flag indicates dependence on IV.  These are components that
	// did a real send not preceded by a set:
	uint32_t checkIV = bitmask&(~levelSetBits)&
	    (((~levelSendBits)&(realSendBits))|
	    ((~levelSendBits)&(didRealSendBits))|
	    ((didRealSendBits)&(realSendBits)));
	    
	//checkGL indicates a dependence on GL.  These are components that did
	//a non-real send after a real set.
	uint32_t checkGL = bitmask&(~didRealSendBits)&(~realSendBits)
	    &(levelSetBits);

	//with light model, a dependence on IV=GL is also a dependence on GL
	
	if(doSend & LIGHT_MODEL_MASK){
	    checkGL |= LIGHT_MODEL_MASK;       
	}
	//with diffuse colors, if there are multiple colors, an IV=GL 
	//dependence is also an IV dependence
	if((doSend & DIFFUSE_MASK)&&(ivState.numDiffuseColors > 1)){
	    checkIV |= DIFFUSE_MASK;
	}	    
	cacheLazyElt->GLSendBits |= realSendBits;
	if (checkGL) copyGLValues(checkGL,cacheLazyElt);  
	if (checkIV) copyIVValues(checkIV,cacheLazyElt);
 	thisCache->setLazyBits(checkIV, checkGL, doSend);
	
	//If we set a bit in levelSendBits, must do this on a writable element:
	if (ivState.cacheLevelSendBits != (ivState.cacheLevelSendBits|bitmask)){
	    SoGLLazyElement* le = 
		(SoGLLazyElement*)getWInstance((SoState *)state);
	    le->ivState.cacheLevelSendBits |= bitmask;
	}
    }	

    return;
}
///////////////////////////////////////////////////////////////////////////
//
//  Description:
//  Send color and transparency to GL.
//  This method is included for compatibility with MaterialBundles.
//  Note that this does not update entire material state, should not be the
//  first "send" of a shape node.  
//
//  use: public, SoEXTENDER
////////////////////////////////////////////////////////////////////////////
void
SoGLLazyElement::sendDiffuseByIndex(int index) const
{
    float col4[4];
#ifdef DEBUG
    //check to make sure lazy element has updated GL state;
    //this method should only be called after an initial call to
    //SoLazyElement::reallySend
    if ((invalidBits & NO_COLOR_MASK) != 0){
	SoDebugError::post("SoGLLazyElement::sendDiffuseByIndex", 
	    "Indexed send not preceded by send of lazy element");	
    }
    if (index >= ivState.numDiffuseColors){
	SoDebugError::post("SoGLLazyElement::sendDiffuseByIndex", 
	    "Not enough diffuse colors provided");
    }
    if (index >= ivState.numTransparencies && 
	    ivState.numTransparencies > 1){
	SoDebugError::post("SoGLLazyElement::sendDiffuseByIndex", 
	    "Not enough transparencies provided");
    }
#endif /*DEBUG*/
    //If in color index mode, ignore transparency.
    if (colorIndex){
	glIndexi((GLint)ivState.colorIndices[index]);
	return;
    }
    	    
    if (glState.GLColorMaterial || (glState.GLLightModel == BASE_COLOR))
	glColor4ubv((GLubyte*)(ivState.packedColors+index));
    else {
      	col4[3] =  (ivState.packedColors[index] & 0xff)   * 1.0/255;
	col4[2] = ((ivState.packedColors[index] & 0xff00) >>  8) * 1.0/255;
	col4[1] = ((ivState.packedColors[index] & 0xff0000)>> 16) * 1.0/255;
	col4[0] = ((ivState.packedColors[index] & 0xff000000)>>24) * 1.0/255;
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, col4);
    }
    return;
}
///////////////////////////////////////////////////////////////////////////
//
// Description:
//
// This is to be invoked when a cache ("child cache") is called while 
// another cache "parent cache" is open.  It sets the appropriate
// bitflags to make the lazy state of the child merge into the parent.
//
// use: SoINTERNAL public
//
////////////////////////////////////////////////////////////////////////////
void 
SoGLLazyElement::mergeCacheInfo(SoGLRenderCache* childCache, 
    SoGLRenderCache* parentCache, uint32_t doSendFlag, uint32_t checkIVFlag, 
    uint32_t checkGLFlag)
{	
    SoGLLazyElement* parentLazyElt = parentCache->getLazyElt();
    SoGLLazyElement* childLazyElt = childCache->getLazyElt();
    uint32_t parentDidRealSendBits = parentLazyElt->GLSendBits;
    uint32_t parentSetBits = ivState.cacheLevelSetBits;
    uint32_t doSend = (~parentDidRealSendBits)& doSendFlag 
	&(~ivState.cacheLevelSendBits)&(~ivState.cacheLevelSetBits);
    uint32_t checkIV =  checkIVFlag & (~ivState.cacheLevelSetBits);
    uint32_t checkGL = (~parentDidRealSendBits)& checkGLFlag;
	
  	
    if (checkGL) 
	childLazyElt->copyGLValues(checkGL, parentLazyElt);
    if (checkIV) 
	childLazyElt->copyIVValues(checkIV, parentLazyElt);  
    
    // in addition we must consider the case where the parent cache issued
    // a set, and the child issued a send which was not a real_send. 	
    uint32_t moreGL = (~parentDidRealSendBits)&(parentSetBits)&
	( checkGLFlag|doSendFlag );
    if (moreGL) copyGLValues(moreGL,parentLazyElt);
    
    parentCache->setLazyBits
	    (checkIV, checkGL|moreGL, doSend);
    // merge the child didRealSendBits into parent:
    parentLazyElt->GLSendBits |= childLazyElt->GLSendBits;
 
    
}	
////////////////////////////////////////////////////////////////////////
//
//  Description:
//  Copy GL values of state element into cacheGLState, based on GLSendBits
//  in the cacheLazyElement. 
//  This is intended to be called at 
//  cache close(), to keep track of what GL was sent in the cache.
//  "this" is the current lazy element in the state.
//
//  use: SO_INTERNAL public
//
////////////////////////////////////////////////////////////////////////
void
SoGLLazyElement::getCopyGL(SoGLLazyElement* cacheLazyElement, 
    SoGLLazyElement::GLLazyState& cacheGLState)
{
    uint32_t bitmask = cacheLazyElement->GLSendBits;
    for(int j=0; (j<SO_LAZY_NUM_COMPONENTS)&&bitmask; j++, bitmask>>=1){

    	if (bitmask&1){
	    int i;         
	    switch(j){
		case(LIGHT_MODEL_CASE):
		    cacheGLState.GLLightModel = glState.GLLightModel;
		    break;
	    
		case(COLOR_MATERIAL_CASE):
		    cacheGLState.GLColorMaterial = 
			glState.GLColorMaterial;
		    break;
	    
		case(DIFFUSE_CASE):
		    cacheGLState.GLDiffuseNodeId = 
			glState.GLDiffuseNodeId;
		    cacheGLState.GLTranspNodeId = 
			glState.GLTranspNodeId;
		    break;
		    
		case(AMBIENT_CASE):
       	    	    for(i=0; i<3; i++)
			cacheGLState.GLAmbient[i] = 
			    glState.GLAmbient[i];
		    break;

		case(EMISSIVE_CASE):
       	    	    for(i=0; i<3; i++)
			cacheGLState.GLEmissive[i] = 
			    glState.GLEmissive[i];
		    break;

		case(SPECULAR_CASE):
       	    	    for(i=0; i<3; i++)
			cacheGLState.GLSpecular[i] = 
			    glState.GLSpecular[i];
		    break;

		case(SHININESS_CASE):
		    cacheGLState.GLShininess = 
			glState.GLShininess;
		    break;
		    
		case(BLENDING_CASE):
		    cacheGLState.GLblending = 
			glState.GLblending;
		    break;
		 		      
		case(TRANSPARENCY_CASE):
		    cacheGLState.GLStippleNum =
			glState.GLStippleNum;
		    break;
    	    }
	}
    }
}
////////////////////////////////////////////////////////////////////////
//
//  Description:
//  Copy GL values to state element from cacheLazyElement, based on GLSendBits 
//  in the cacheLazyElement.  To be used after a cache is called. 
//  also sets the GLsend bits and invalid bits in the state lazy element.
//
//  use: SO_INTERNAL private
//
////////////////////////////////////////////////////////////////////////
void
SoGLLazyElement::reallyCopyBackGL(uint32_t bitmask, 
    SoGLLazyElement::GLLazyState& cacheGLState)
{
    GLSendBits |= bitmask;
    invalidBits |= bitmask;
   
    for(int j=0; (j<SO_LAZY_NUM_COMPONENTS)&&bitmask; j++, bitmask>>=1){

    	if (bitmask&1){
	    int i;         
	    switch(j){
		case(LIGHT_MODEL_CASE):
		    glState.GLLightModel = 
			cacheGLState.GLLightModel;
		    break;
	    
		case(COLOR_MATERIAL_CASE):
		    glState.GLColorMaterial = 
			cacheGLState.GLColorMaterial;
		    break;
		    
		case(DIFFUSE_CASE):
		    glState.GLDiffuseNodeId = 
			cacheGLState.GLDiffuseNodeId;
		    glState.GLTranspNodeId = 
			cacheGLState.GLTranspNodeId;
		    break;

		case(AMBIENT_CASE):
       	    	    for(i=0; i<3; i++)
			glState.GLAmbient[i] = 
			    cacheGLState.GLAmbient[i];
		    break;

		case(EMISSIVE_CASE):
       	    	    for(i=0; i<3; i++)
			glState.GLEmissive[i] = 
			    cacheGLState.GLEmissive[i];
		    break;

		case(SPECULAR_CASE):
       	    	    for(i=0; i<3; i++)
			glState.GLSpecular[i] = 
			    cacheGLState.GLSpecular[i];
		    break;

		case(SHININESS_CASE):
		    glState.GLShininess = 
			cacheGLState.GLShininess;
		    break;
   
		case(BLENDING_CASE):
		    glState.GLblending = 
			cacheGLState.GLblending;
		    break;
		  
		case(TRANSPARENCY_CASE):				
		    glState.GLStippleNum =
			cacheGLState.GLStippleNum;		   
		    break;
	    }
	}
    }
}
////////////////////////////////////////////////////////////////////////
//
//  Description:
//  Copy GL values of specified components into specified 
//  cacheLazyElement.  Invoked by this element and GLRenderCache.
//
//  use: SoINTERNAL public
//
////////////////////////////////////////////////////////////////////////

void
SoGLLazyElement::copyGLValues(uint32_t bitmask,SoGLLazyElement* lazyElt)
{
    for(int j=0; (j<SO_LAZY_NUM_COMPONENTS)&&bitmask; j++, bitmask>>=1){

    	if (bitmask&1){
	    int i;         
	    switch(j){
		case(LIGHT_MODEL_CASE):
		    lazyElt->glState.GLLightModel = glState.GLLightModel;
		    break;
		
		case(COLOR_MATERIAL_CASE):
		    lazyElt->glState.GLColorMaterial = glState.GLColorMaterial;
		    break;
		    
		case(DIFFUSE_CASE):
		    lazyElt->glState.GLDiffuseNodeId = glState.GLDiffuseNodeId;
		    lazyElt->glState.GLTranspNodeId = glState.GLTranspNodeId;
		    break;

		case(AMBIENT_CASE):
       	    	    for(i=0; i<3; i++)
			lazyElt->glState.GLAmbient[i]=glState.GLAmbient[i];
		    break;

		case(EMISSIVE_CASE):
       	    	    for(i=0; i<3; i++)
			lazyElt->glState.GLEmissive[i]=glState.GLEmissive[i];
		    break;

		case(SPECULAR_CASE):
       	    	    for(i=0; i<3; i++)
			lazyElt->glState.GLSpecular[i]=glState.GLSpecular[i];
		    break;

		case(SHININESS_CASE):
		    lazyElt->glState.GLShininess=glState.GLShininess;
		    break;
		    
		case(BLENDING_CASE):
		    lazyElt->glState.GLblending = glState.GLblending;
		    break;
	        		    
		case(TRANSPARENCY_CASE):		  
		    lazyElt->glState.GLStippleNum = glState.GLStippleNum;
		    break;    
	    }
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
//  Description:
//  Copy Inventor values of specified components into specified 
//  cacheLazyElement
//
//  use: SoINTERNAL public
// 
////////////////////////////////////////////////////////////////////////
void
SoGLLazyElement::copyIVValues(uint32_t bitmask,SoGLLazyElement* lazyElt)
{
    for(int j=0; (j<SO_LAZY_NUM_COMPONENTS)&&bitmask; j++, bitmask>>=1){

    	if (bitmask&1){
	    int i;         
	    switch(j){
		
		case(LIGHT_MODEL_CASE):
		    lazyElt->ivState.lightModel = ivState.lightModel;
		    break;
		
		case(COLOR_MATERIAL_CASE):
		    lazyElt->ivState.colorMaterial = ivState.colorMaterial;
		    break;
		    
		case(DIFFUSE_CASE):
		    lazyElt->ivState.diffuseNodeId = ivState.diffuseNodeId;
		    lazyElt->ivState.transpNodeId = ivState.transpNodeId;
		    break;
		    
		case(AMBIENT_CASE):
       	    	    for(i=0; i<3; i++)
			lazyElt->ivState.ambientColor[i] = 
			    ivState.ambientColor[i];
		    break;

		case(EMISSIVE_CASE):
       	    	    for(i=0; i<3; i++)
			lazyElt->ivState.emissiveColor[i] = 
			    ivState.emissiveColor[i];
		    break;

		case(SPECULAR_CASE):
       	    	    for(i=0; i<3; i++)
			lazyElt->ivState.specularColor[i] = 
			    ivState.specularColor[i];
		    break;

		case(SHININESS_CASE):
		    lazyElt->ivState.shininess = ivState.shininess;
		    break;
		    
		case(BLENDING_CASE):
		    lazyElt->ivState.blending = ivState.blending;
		    break;

		case(TRANSPARENCY_CASE):		
		    lazyElt->ivState.stippleNum = ivState.stippleNum;		
		    break;
	    }
	}
    }
}
////////////////////////////////////////////////////////////////////////
//
//  Description:
//  Put the current diffuse diffuse color and transparency into a packed
//  color array, associated with specified SoColorPacker.
//
//  use: private
// 
////////////////////////////////////////////////////////////////////////
void 
SoGLLazyElement::packColors(SoColorPacker *cPacker)
{
    //First determine if we have enough space:
    if (cPacker->getSize() < ivState.numDiffuseColors)
	cPacker->reallocate(ivState.numDiffuseColors);

    uint32_t *packedArray = cPacker->getPackedColors();
    SbBool multTrans = (ivState.numTransparencies >= ivState.numDiffuseColors);    
    int indx = 0;
    uint32_t transp;
    for (int i=0; i< ivState.numDiffuseColors; i++){
	if (isPacked()){
	    if (i == 0 || multTrans) 
		transp = (uint32_t)((1.0 - ivState.transparencies[i])*255.);	  
	    packedArray[i] = (ivState.packedColors[i] & 0xffffff00)|
		(transp & 0xff);			    	       
	}
	else{
	    if (multTrans) indx = i;
	    packedArray[i] = (ivState.diffuseColors + i)->
		getPackedValue(ivState.transparencies[indx]);	    
	}
	
    }
    cPacker->setNodeIds(ivState.diffuseNodeId, ivState.transpNodeId);
}
////////////////////////////////////////////////////////////////////////////////
// Data associated with stipple patterns
// Holds defined 32x32 bit stipple patterns. Each is defined as 32
// rows of 4 bytes (32 bits) each.
////////////////////////////////////////////////////////////////////////////////
u_char	SoGLLazyElement::patterns[64+1][32 * 4];

// Indicates whether patterns were created and stored yet
SbBool	SoGLLazyElement::patternsCreated = FALSE;

// Holds flags to indicate whether we defined a display list for the
// corresponding stipple pattern
SbBool	SoGLLazyElement::patternListDefined[64+1];

// Stores base display list index for patterns. Initialized to -1 to
// indicate that no pattern lists were allocated.
int	SoGLLazyElement::patternListBase = -1;

// Stores cache context in which display lists were created. We can't
// call a list from a context other than this one.
int	SoGLLazyElement::patternListContext;




////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sends  stipple transparency.  Assumes it really needs to send it.
//
// Use: private
////////////////////////////////////////////////////////////////////////
void
SoGLLazyElement::sendStipple(const SoState *state, int patIndex )
{
 
#ifdef DEBUG
    if (patIndex <= 0 || patIndex > getNumPatterns()){
	SoDebugError::post("SoGLLazyElement::sendStipple", 
	    		   "Invalid pattern index");
    }
#endif /*DEBUG*/

	    // Create pattern arrays if not already done
	    if (! patternsCreated) {
		createPatterns();
		patternsCreated = TRUE;

		// Make sure we know that no patterns were sent
		int	i;
		for (i = 0; i <= getNumPatterns(); i++)
		    patternListDefined[i] = FALSE;
	    }

	    // Determine the current cache context
	    int currentContext = SoGLCacheContextElement::get((SoState*)state);

	    // If we already have a display list for this pattern
	    if (patternListDefined[patIndex]) {

		// If the cache context is valid, just call the list
		if (currentContext == patternListContext)
		    glCallList(patternListBase + patIndex);

		// Otherwise, just send the pattern
		else
		    glPolygonStipple(patterns[patIndex]);
	    }

	    // If we are in the middle of building a cache or the
	    // context is different from the one in which we allocated
	    // the display lists, we can't create a new display list,
	    // so just send the pattern as is
	    else if (state->isCacheOpen() ||
		     (patternListBase >= 0 &&
		      currentContext != patternListContext))
		glPolygonStipple(patterns[patIndex]);

	    // Otherwise, build a display list and send it
	    else {

		// If we haven't allocated the pattern list indices, do so
		if (patternListBase < 0) {
		    patternListBase = (int) glGenLists(getNumPatterns() + 1);
		    patternListContext = currentContext;
		}

		// Create and send the list
		glNewList(patternListBase + patIndex, GL_COMPILE_AND_EXECUTE);
		glPolygonStipple(patterns[patIndex]);
		glEndList();

		patternListDefined[patIndex] = TRUE;
	    }
	
}

/////////////////////////////////////////////////////////////////////////////
//
// Description:
//    This fills in the "patterns" array with polygon stipples that
//    simulate transparency levels using a standard dither matrix.
//    This code was adapted from Foley, van Dam, Feiner, and Hughes,
//    pages 569-572.
//
// Use: private, static
//////////////////////////////////////////////////////////////////////////
void
SoGLLazyElement::createPatterns()
{
    static short	ditherMatrix[8][8] = {
	 0, 32,  8, 40,  2, 34, 10, 42,
	48, 16, 56, 24, 50, 18, 58, 26,
	12, 44,  4, 36, 14, 46,  6, 38,
	60, 28, 52, 20, 62, 30, 54, 22,
	 3, 35, 11, 43,  1, 33,  9, 41,
	51, 19, 59, 27, 49, 17, 57, 25,
	15, 47,  7, 39, 13, 45,  5, 37,
	63, 31, 55, 23, 61, 29, 53, 21,
    };

    u_char		pat[8];
    int			pattern, x, y;

    // For each pattern
    for (pattern = 0; pattern <= 64; pattern++) {

	// Set up an 8x8 pixel pattern in "pat", 1 bit per pixel
	for (y = 0; y < 8; y++) {
	    pat[y] = 0;
	    for (x = 0; x < 8; x++)
		if (ditherMatrix[y][x] >= pattern)
		    pat[y] |= (1 << (7 - x));
	}

	// Store the 8x8 pattern in the correct slot in "patterns".
	// Since we need a 32x32, replicate the pattern 4 times in
	// each dimension.

#define PAT_INDEX(x, y)		((y) * 4 + x)

	for (y = 0; y < 8; y++) {
	    for (x = 0; x < 4; x++) {
		patterns[pattern][PAT_INDEX(x, y +  0)] = pat[y];
		patterns[pattern][PAT_INDEX(x, y +  8)] = pat[y];
		patterns[pattern][PAT_INDEX(x, y + 16)] = pat[y];
		patterns[pattern][PAT_INDEX(x, y + 24)] = pat[y];
	    }
	}

#undef PAT_INDEX

    }
}
