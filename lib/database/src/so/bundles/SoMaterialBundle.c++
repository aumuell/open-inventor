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
 |   $Revision $
 |
 |   Classes:
 |	SoMaterialBundle class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <GL/gl.h>
#include <Inventor/bundles/SoMaterialBundle.h>
#include <Inventor/elements/SoGLCacheContextElement.h>
#include <Inventor/elements/SoGLLazyElement.h>
#include <Inventor/elements/SoMaterialBindingElement.h>
#include <Inventor/elements/SoShapeStyleElement.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/errors/SoDebugError.h>

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor. Does as little as possible now to avoid making
//    unnecessary calls.
//
// Use: public

SoMaterialBundle::SoMaterialBundle(SoAction *action) : SoBundle(action)
//
////////////////////////////////////////////////////////////////////////
{

    // Remember that we haven't accessed elements yet
    firstTime = TRUE;
    lastIndex = -1;
    lazyElt = NULL;  

    // See whether we need to deal with materials or just colors
    colorOnly = ((SoLazyElement::getLightModel(state)) ==
		 SoLazyElement::BASE_COLOR);
    sendMultiple = FALSE;
    fastColor = 0;

    // Nodes that use material bundles haven't been optimized, and
    // should be render cached if possible:
    SoGLCacheContextElement::shouldAutoCache(state, TRUE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: public

SoMaterialBundle::~SoMaterialBundle()
//
////////////////////////////////////////////////////////////////////////
{    
    //If multiple diffuse colors were sent, we don't know what color
    // was left in GL.   
    if (sendMultiple)        
	lazyElt->reset(state, SoLazyElement::DIFFUSE_MASK);
    // turn off ColorMaterial, if it was turned on.
    if (fastColor)  
	SoGLLazyElement::setColorMaterial(state, FALSE);
    
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Really sends material to the GL.
//
// Use: private

void
SoMaterialBundle::reallySend(int index, int isBetweenBeginEnd,
			     SbBool avoidFastColor)
//
////////////////////////////////////////////////////////////////////////
{
    // Make sure we have accessed all required elements (first time only)
    if (firstTime){
	accessElements(isBetweenBeginEnd, avoidFastColor);
	firstTime = FALSE;
	// the first color has already been sent--
	if (index == 0) {
	    lastIndex = index;
	    return;
	}
    }

    if (lastIndex == index) return;

    //Indicate multiple colors are being sent:  This will force reset() after
    //shape is rendered
    sendMultiple = TRUE;

#ifdef DEBUG
    // Make sure the index is valid
    if (index >= numMaterials){
	SoDebugError::post("SoMaterialBundle::reallySend", 
	    "Not enough colors specified");
    }    
#endif
    lazyElt->sendDiffuseByIndex(index);
    lastIndex = index;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Accesses and stores pointers to the elements needed by
//    the bundle. This is done once per bundle construction, but is
//    delayed until the elements are actually needed. This also sets
//    up some other flags and values in the element, including the
//    number of materials
//
//
// Use: private

void
SoMaterialBundle::accessElements(SbBool isBetweenBeginEnd,
				 SbBool avoidFastColor)
//
////////////////////////////////////////////////////////////////////////
{   
    const SoLazyElement* le = SoLazyElement::getInstance(state);   

    // Initialize
    numMaterials = le->getNumDiffuse();

    // Determine if we can use "fast colors" - just sending GL colors
    // in place of the diffuse colors. This saves both time and space,
    // especially when materials are sent within GL display lists.
    // Note: we can't make the necessary GL calls if we are between a
    // glBegin() and a glEnd(), so that case is considered here.
    fastColor = (! isBetweenBeginEnd &&
		 ! colorOnly &&		
		 numMaterials > 1 );

    // Set up GL if necessary for fast color mode. When materials are
    // "forced" to send (as by SoMaterial nodes), we don't want to set
    // up fast colors. So check that case, too. 
    if (fastColor && !avoidFastColor)  
	SoGLLazyElement::setColorMaterial(state, TRUE);
    
    //Note: it's important to save the lazyElt AFTER the first set(),
    //so that subsequent lazyElt->send()'s will use top-of-stack.
    lazyElt = (const SoGLLazyElement *)SoLazyElement::getInstance(state);
    if (!colorOnly)   
	lazyElt->send(state,SoLazyElement::ALL_MASK);
    else {
	lazyElt->send(state,SoLazyElement::DIFFUSE_ONLY_MASK);
    }
}

