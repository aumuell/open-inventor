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

//  -*- C++ -*-

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.2 $
 |
 |   Description:
 |	This file defines the SoGLLazyElement class.
 |
 |   Author(s)		: Alan Norton,  Gavin Bell 
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_GL_LAZY_ELEMENT
#define  _SO_GL_LAZY_ELEMENT

#include <Inventor/elements/SoLazyElement.h>
#include <Inventor/actions/SoGLRenderAction.h>
class SoGLRenderCache;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoGLLazyElement
//
//  Element that manages the GL state for the SoLazyElement. 
//
//  Note that this class relies on SoLazyElement to store the
//  inventor color(s), etc. in the instance.
//  This element keeps GL shadow copies of colors
//
//////////////////////////////////////////////////////////////////////////////

SoEXTENDER class SoGLLazyElement : public SoLazyElement {

    SO_ELEMENT_HEADER(SoGLLazyElement);

  public:
    // Initializes element
    virtual void	init(SoState *state);

    // Override push() and pop() methods to maintain GL state
    virtual void	push(SoState *state);
    virtual void	pop(SoState *state, const SoElement *prevTopElement);

    //Static sends, always send top-of-stack.  Intended for extender use.  
    static void		sendAllMaterial(SoState *state)
	{SoGLLazyElement *le = getInstance(state);
	if ((le->invalidBits)||(state->isCacheOpen()))  
	    le->reallySend(state, ALL_MASK);}
	    
    static void		sendNoMaterial(SoState *state)
	{SoGLLazyElement *le = getInstance(state);
	if ((NO_COLOR_MASK & le->invalidBits)||(state->isCacheOpen()))  
	    le->reallySend(state, NO_COLOR_MASK);}
	    
    static void		sendOnlyDiffuseColor(SoState *state)
	{SoGLLazyElement *le = getInstance(state);
	if ((DIFFUSE_ONLY_MASK & le->invalidBits)||(state->isCacheOpen()))  
	    le->reallySend(state, DIFFUSE_ONLY_MASK);}
	    
    // Send diffuse color and transparency, by index
    // Included for compatibility with SoMaterialBundle
    // To be used by a shape to send additional colors after first send()
    // Should NEVER be the first send of a shape! 
    void sendDiffuseByIndex(int index) const; 
    
    // Destroy knowledge of GL state
    void reset(SoState* state,  uint32_t bitmask) const;
    
    static SbBool	isColorIndex(SoState *state)
    	{return(getInstance(state)->colorIndex);}
	
    // Return the top (current) instance of the element in the state
    // Note it does NOT cause cache dependency!
    // It also casts away the const.   
    static  SoGLLazyElement * getInstance(const SoState *state)
	{return (SoGLLazyElement *) 
	(state->getConstElement(classStackIndex));}
	        	        
  SoINTERNAL public:
    // Initializes the SoGLLazyElement class
    static void		initClass();
    
    // Sends indicated component(s) to GL: 
    // Only sends if value is not already in GL.
    // note: has side effects, cannot really be const.
    // however will not necessarily cause cache dependency.
    void		send(const SoState *state, uint32_t mask) const
	{if ((mask & invalidBits)||(state->isCacheOpen()))  
	    ((SoGLLazyElement*)(this))->reallySend(state, mask);} 
	    
    // note: matches, copyMatchinfo not used by this element.
    // they are replaced by lazyMatches, copyLazyMatchInfo  
    virtual SbBool	matches(const SoElement *) const
	{return FALSE;}
    virtual SoElement	*copyMatchInfo() const 
	{return NULL;}
	
    // Replace  matches() for this element:
    // matches "this" element GL or IV state with values in eltInState
    SbBool lazyMatches(uint32_t checkGLFlag, uint32_t checkIVFlag,
	const SoGLLazyElement* eltInState){
	if (checkGLFlag || checkIVFlag ||
	    (ivState.transpType != eltInState->ivState.transpType))
	    return (fullLazyMatches (checkGLFlag, checkIVFlag, eltInState));
	else return(TRUE);
	} 

   //provide a public typedef for GLLazyState, so that GLRenderCache can use it:
   typedef struct {
	uint32_t	GLDiffuseNodeId;
	uint32_t	GLTranspNodeId;
	float		GLAmbient[4];
	float		GLEmissive[4];
	float		GLSpecular[4];
	float		GLShininess;
	SbBool		GLColorMaterial;	
	int32_t		GLLightModel;
	SbBool		GLblending;
	int32_t		GLStippleNum;
    } GLLazyState; 
    
    // Replaces copyMatchInfo for this element:
    // makes a GLLazyElement with some initialization.
    // also initializes cacheSetBits to zero.
    SoGLLazyElement* copyLazyMatchInfo(SoState* state);

    // method that makes a copy of GL state at cache close(). 
    // goes from state="this" to cacheGLState 
    // only copies if bitmask (GLSend of cache lazy element) is set.
    void getCopyGL(SoGLLazyElement* cacheLazyElement, 
	SoGLLazyElement::GLLazyState& cacheGLState);

    // method that copies GL state back into "this" element
    // after cache has been called.
    // only copies if bit in bitmask is set.
    // also sets invalidBits to FALSE for these components.
    void copyBackGL(SoGLLazyElement* cacheLazyElement, 
	SoGLLazyElement::GLLazyState& cacheGLState) 
	{ if (cacheLazyElement->GLSendBits) 
	    reallyCopyBackGL(cacheLazyElement->GLSendBits, cacheGLState);}
	      
    // Send a packed color (not in state).  Send transparency
    // as stipple if necessary. 
    void sendVPPacked(SoState* state, const unsigned char* pcolor);
          
    //  Note: destructor is public, so cache can delete its copy.
    virtual ~SoGLLazyElement();
    
    void mergeCacheInfo(SoGLRenderCache* childCache, SoGLRenderCache* 
	parentCache, uint32_t doSendFlag, uint32_t checkIVFlag, 
	uint32_t checkGLFlag);
  private:	   
    // Make copy of GL values into cacheLazyElement
    void copyGLValues(uint32_t bitmask, SoGLLazyElement* cacheLazyElement);
    // Make copy of IV values into cacheLazyElement
    void copyIVValues(uint32_t bitmask, SoGLLazyElement* cacheLazyElement);
    
    // non-inline send when something has to be sent:  
    void reallySend(const SoState *state, uint32_t bitmask);
    
    //private version of lazyMatches, does actual work when needed:   
    SbBool fullLazyMatches(uint32_t checkGLFlag, uint32_t checkIVFlag,
	const SoGLLazyElement* eltInState);

    //Pack the current diffuse and transparency into an SoColorPacker 
    void packColors(SoColorPacker *cPacker);
	
    // virtual set() methods that track GL state
    // as well as inventor state
    virtual void	setDiffuseElt(SoNode *node, int32_t numColors, 
			    const SbColor *colors,  SoColorPacker *cPacker);

    virtual void	setTranspElt(SoNode *node,  int32_t numTrans,  
			    const float *trans, SoColorPacker *cPacker);
			    
    virtual void	setTranspTypeElt(int32_t type);	
    virtual void	setPackedElt( SoNode *node, 
			    int32_t numColors,  const uint32_t *packedColors);

    virtual void	setColorIndexElt( SoNode*, int32_t numIndices, 
			    const int32_t *indices);
				    			 
    virtual void	setAmbientElt(const SbColor* color);
    virtual void	setEmissiveElt( const SbColor* color);
    virtual void	setSpecularElt(const SbColor* color);
    virtual void	setShininessElt( float color);
    virtual void	setColorMaterialElt( SbBool value);
    virtual void	setBlendingElt( SbBool value);
    virtual void	setLightModelElt(SoState *, int32_t model);
    virtual void	setMaterialElt(SoNode *, uint32_t bitmask, 
	SoColorPacker *cPacker, const SoMFColor&, 
	const SoMFFloat&, const SoMFColor&,   	
	const SoMFColor&, const SoMFColor&, const SoMFFloat&);

    //register a redundant set with the cache:	
    virtual void	registerRedundantSet(SoState *, uint32_t bitmask);
    //register a get() with the cache:
    virtual void	registerGetDependence(SoState *, uint32_t bitmask);

    //really does the copying of GL state (invoked by copyBackGL)
    void reallyCopyBackGL(uint32_t bitmask, SoGLLazyElement::GLLazyState&  );
    
    // Copy of what has been sent to GL:
    //struct SoGLLazyElement::GLLazyState glState;
    SoGLLazyElement::GLLazyState glState;
  
    // BitMap indicating what GL sends have been made:
    uint32_t GLSendBits;
    
    // Indicator of whether in colorIndex mode or not:
    SbBool colorIndex;

    // Private storage associated with stipple patterns:
    // Holds defined 32x32 bit stipple patterns. Each is defined as 32
    // rows of 4 bytes (32 bits) each.
    static u_char	patterns[64+1][32 * 4];

    // Indicates whether patterns were created and stored yet
    static SbBool	patternsCreated;

    // Holds flags to indicate whether we defined a display list for
    // the corresponding stipple pattern
    static SbBool	patternListDefined[64+1];

    // Stores base display list index for patterns
    static int		patternListBase;

    // Stores cache context in which display lists were created
    static int		patternListContext;

    // Set to TRUE in getInstance()??? if a cache is currently being
    // built, meaning we can't build a new display list
    SbBool		cacheOpen;

    // Creates and sends the stipple pattern to GL
    void		sendStipple(const SoState *state, int transpIndex);
    
    // Fills in "patterns" arrays with polygon stipples that simulate
    // transparency levels, using a standard dither matrix
    static void		createPatterns();  
};

#endif /* _SO_GL_LAZY_ELEMENT */
