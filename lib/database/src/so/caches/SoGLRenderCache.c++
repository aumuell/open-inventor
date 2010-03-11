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
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |	SoGLRenderCache
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/caches/SoGLRenderCache.h>
#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/elements/SoGLCacheContextElement.h>
#include <Inventor/errors/SoDebugError.h>
#include <SoDebug.h>


////////////////////////////////////////////////////////////////////////
//
// Description:
//
// Constructor
//
// Use: public

SoGLRenderCache::SoGLRenderCache(SoState *state) : SoCache(state)
//
////////////////////////////////////////////////////////////////////////
{
    // We haven't compiled a list yet
    list = NULL;
    listOpen = FALSE;
    saveState = NULL;
    GLCacheLazyElement = NULL;
    checkGLFlag = 0;
    checkIVFlag = 0;
    doSendFlag = 0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: public

SoGLRenderCache::~SoGLRenderCache()
//
////////////////////////////////////////////////////////////////////////
{
    // Unref all of the nested caches
    for (int i = 0; i < nestedCaches.getLength(); i++)
	((SoGLDisplayList *) nestedCaches[i])->unref();

    // delete the GLLazyElement
    if (GLCacheLazyElement != NULL)
	delete GLCacheLazyElement;
}

///////////////////////////////////////////////////////////////////////
//
// Description:
// Returns TRUE if cache is valid with respect to the given state
//
// Use: public, virtual

SbBool      
SoGLRenderCache::isValid(const SoState *state) const
{
    if (!SoCache::isValid(state)) return(FALSE);
    // get the lazy element from the state:
    SoGLLazyElement *eltInState = SoGLLazyElement::getInstance(state);
    
    // Send the components of the lazy element requiring IV=GL,
    // do this prior to checking GL matches and IV matches.
    if (doSendFlag) eltInState->send(state, doSendFlag);

    // If cache's version of lazy element doesn't match what's in the
    // state, the cache is not valid.
    
    if (!GLCacheLazyElement->
	    lazyMatches(checkGLFlag,checkIVFlag,eltInState)){
#ifdef DEBUG
	if (SoDebug::GetEnv("IV_DEBUG_CACHES")) {
	    fprintf(stderr, "CACHE DEBUG: cache(0x%x) not valid ",
		    this);
	    fprintf(stderr, "Because a lazy element match failed,\n");
	    fprintf(stderr, "GL and IV flags were %d %d\n", checkGLFlag, 
		    checkIVFlag);	
	}
#endif /*DEBUG*/	   
	return(FALSE);
    }

    return(TRUE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destroy this cache.  Called by unref(); frees up OpenGL display
//    list.
//
// Use: protected, virtual

void
SoGLRenderCache::destroy(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    if (listOpen) close();

    // Let the GLCacheContext element know that this display list can
    // be freed when the cache context is valid (this destructor may
    // be called when it is illegal to issue any GL commands!)
    if (list) {
	list->unref();
	list = NULL;
    }

    delete GLCacheLazyElement;
    GLCacheLazyElement = NULL;
    SoCache::destroy(state);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds a dependency of this instance on another cache instance.
//
// Use: public, virtual

void
SoGLRenderCache::addNestedCache(SoGLDisplayList *child)
//
////////////////////////////////////////////////////////////////////////
{
    // Add the cache to the list of nested caches
    nestedCaches.append(child);

    // Increment the reference count on the nested cache
    child->ref();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Opens the cache. All GL commands made until the next close()
//    will be part of the the display list cache.
//
// Use: public

void
SoGLRenderCache::open(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    // if there is already an open GLRender cache, bail.
    if (SoCacheElement::anyOpen(state) || listOpen) {
	SoDebugError::post("SoGLRenderCache::open",
			   "A cache is already open!");
	return;
    }
#endif /* DEBUG */

    listOpen = TRUE;

    // We are now the currently open cache...
    SoCacheElement::set(state, this);

    // save state for comparison later:
    saveState = state;

    // set up the GLLazyElement:
#ifdef DEBUG
    if (GLCacheLazyElement != NULL)
	SoDebugError::post("SoGLRenderCache::open",
		"Error reallocating CacheLazyElement");
#endif /*DEBUG*/

    //Get the top Lazy Element, and have it make a copy for the cache:
    SoGLLazyElement *le = SoGLLazyElement::getInstance(state);	
    GLCacheLazyElement = le->copyLazyMatchInfo(state);
    
    //initialize flags:
    checkGLFlag = 0;
    checkIVFlag = 0;
    doSendFlag = 0;

    // Create a new display list
    list = new SoGLDisplayList(state, SoGLDisplayList::DISPLAY_LIST, 1);
    list->ref();
    list->open(state);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Closes the cache.
//
// Use: public

void
SoGLRenderCache::close()
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (! listOpen) {
	SoDebugError::post("SoGLRenderCache::close",
			   "No cache is currently open!");
	return;
    }

#endif /* DEBUG */

    listOpen = FALSE;

    // copy final GLState into CacheLazyElement
    // for anything in lazy element that was sent
   
    SoGLLazyElement* le = (SoGLLazyElement*)
	saveState->getConstElement(SoGLLazyElement::getClassStackIndex());
    le->getCopyGL(GLCacheLazyElement, cachedGLState);       

    list->close(saveState);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Calls the cache. The display list will be sent to GL.
//
// Use: public

void
SoGLRenderCache::call(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{ 

#ifdef DEBUG
    if (! list) {
	SoDebugError::post("SoGLRenderCache::call",
			   "Cache was never compiled!");
	return;
    }
#endif /* DEBUG */

    // Make sure all open caches depend on us
    SoCacheElement::addCacheDependency(state, this);

    list->call(state);
     
    // get the current lazy element from the state 
    SoGLLazyElement* currentLazyElt = SoGLLazyElement::getInstance(state);
          
    //If this cache call occurred within a cache, must pass info to
    //parent cache.
    if (state->isCacheOpen()){
	SoGLRenderCache* parentCache = (SoGLRenderCache*)
		SoCacheElement::getCurrentCache(state);
	SoGLRenderCache* childCache = this;      
  	   
        currentLazyElt->mergeCacheInfo(childCache, parentCache, doSendFlag, 
	    checkIVFlag, checkGLFlag);
    }	
    
    // copy back the CacheLazyElement's GL State
    // also set GLSendBits and invalidBits.
    currentLazyElt->copyBackGL(GLCacheLazyElement, cachedGLState);
}
