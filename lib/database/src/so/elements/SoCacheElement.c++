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
 |	SoCacheElement
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/caches/SoCache.h>
#include <Inventor/caches/SoGLRenderCache.h>
#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/misc/SoState.h>

// Static var:
SbBool SoCacheElement::invalidated;

SO_ELEMENT_SOURCE(SoCacheElement);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoCacheElement::~SoCacheElement()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes element
//
// Use: public

void
SoCacheElement::init(SoState *)
//
////////////////////////////////////////////////////////////////////////
{
    cache = NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets cache in element accessed from state.
//
// Use: public, static

void
SoCacheElement::set(SoState *state, SoCache *cache)
//
////////////////////////////////////////////////////////////////////////
{
    SoCacheElement	*elt;

#ifdef DEBUG
    if (cache == NULL) {
	SoDebugError::post("SoCacheElement::set",
			   "NULL argument illegal");
	return;
    }
#endif    

    // Get an instance we can change (pushing if necessary)
    elt = (SoCacheElement *) getElement(state, classStackIndex);

    if (elt == NULL) {
	SoDebugError::post("SoCacheElement::set", "unable to access element");
	return;
    }

    elt->cache = cache;
    elt->cache->ref();

    // Let the state know a cache is open.
    state->setCacheOpen(TRUE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if any cache is currently open in the state.
//
// Use: public, static

SbBool
SoCacheElement::anyOpen(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    const SoCacheElement *elt = (const SoCacheElement *)
	state->getConstElement(classStackIndex);

    // Assume that nobody has set a cache to NULL; that means the only
    // NULL cache is the first instance on the stack, which doesn't
    // represent a real cache.
    return (elt->cache != NULL);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Invalidates any open caches.  Called by nodes that can't be
//    cached for some reason.
//
// Use: public, static

void
SoCacheElement::invalidate(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    const SoCacheElement	*cacheElt;

    invalidated = TRUE;

    cacheElt = (const SoCacheElement *)
	state->getConstElement(classStackIndex);

    while (cacheElt != NULL && cacheElt->cache != NULL) {
	cacheElt->cache->invalidate();
	cacheElt = (const SoCacheElement *) cacheElt->getNextInStack();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the previous invalidated state, and sets the invalidated
//    flag to TRUE.  Used by SoGLCacheList to avoid auto-caching
//    things that call ::invalidate().
//
// Use: internal, static

SbBool
SoCacheElement::setInvalid(SbBool newValue)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool oldValue = invalidated;
    invalidated = newValue;
    return oldValue;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pops element, unref'ing the cache.
//
// Use: public

void
SoCacheElement::pop(SoState *state, const SoElement *prevTopElement)
//
////////////////////////////////////////////////////////////////////////
{
    // The previous element is gone, so get rid of its cache
    ((SoCacheElement *) prevTopElement)->cache->unref();

    // Let the state know whether a cache is still open. NOTE: this
    // assumes there can't be an element with a NULL cache deeper than
    // an element with a non-NULL cache.
    state->setCacheOpen(cache != NULL);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides this method to print an error message and return
//    FALSE. Cache elements should never be compared, since they never
//    appear in the elements-used list of caches!
//
// Use: public

SbBool
SoCacheElement::matches(const SoElement *) const
//
////////////////////////////////////////////////////////////////////////
{
    SoDebugError::post("SoCacheElement::matches",
		       "cache elements should never be compared!!!");

    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//     Override method to spit out error
//
// Use: protected

SoElement *
SoCacheElement::copyMatchInfo() const
//
////////////////////////////////////////////////////////////////////////
{
    SoDebugError::post("SoCacheElement::copyMatchInfo",
		       "cache elements should never be copied!");

    return NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds the given element to the elements used lists of all
//    currently open caches in the state.
//
// Use: internal

void
SoCacheElement::addElement(SoState *state, const SoElement *elt)
//
////////////////////////////////////////////////////////////////////////
{
    // Each currently open caching separator has a corresponding
    // SoCacheElement in the stack. Run through each instance in the
    // stack, adding the element if the element's depth is smaller
    // than the depth of the cache element instance.

    const SoCacheElement	*cacheElt;

    cacheElt = (const SoCacheElement *)
	state->getConstElement(classStackIndex);

    while (cacheElt != NULL && elt->getDepth() < cacheElt->getDepth()) {
	cacheElt->cache->addElement(elt);
	cacheElt = (const SoCacheElement *) cacheElt->getNextInStack();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds a dependency on the given cache to all currently open
//    caches in the state.  This is called when a cache is used, to
//    make sure any dependencies it has are propogated to any
//    parent cache(s).
//
// Use: internal

void
SoCacheElement::addCacheDependency(SoState *state, SoCache *cache)
//
////////////////////////////////////////////////////////////////////////
{
    // Each currently open caching separator has a corresponding
    // SoCacheElement in the stack.  

    const SoCacheElement	*cacheElt;

    cacheElt = (const SoCacheElement *)
	state->getConstElement(classStackIndex);

    while (cacheElt != NULL && cacheElt->cache != NULL) {
	cacheElt->cache->addCacheDependency(state, cache);
	cacheElt = (const SoCacheElement *) cacheElt->getNextInStack();
    }
}
