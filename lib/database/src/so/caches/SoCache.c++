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
 |	SoCache base class (SoEXTENDER).
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <stdlib.h>

#include <Inventor/caches/SoCache.h>
#include <Inventor/elements/SoElement.h>
#include <Inventor/misc/SoState.h>

#include <SoDebug.h>

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor. Takes the state in effect when the cache is
//    created; it is assumed that the state is pushed before the cache
//    is created.
//
// Use: public

SoCache::SoCache(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    refCount = 0;
    invalidated = FALSE;

    depth = state->getDepth();

    int bytesNeeded = (SoElement::getNumStackIndices()+7)/8;

    // Set the size of the elementsUsedFlags to be the maximum number
    // of elements currently enabled.  This is safe because if more
    // elements are enabled later, there is no way this cache could
    // care about them-- if a node that used the new element was added
    // below this cache, it would cause notification and would cause
    // the cache to be blown.
    elementsUsedFlags = new unsigned char[bytesNeeded];
    memset(elementsUsedFlags, 0, bytesNeeded);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.  Unreferences any elements this cache depends on.
//
// Use: private

SoCache::~SoCache()
//
////////////////////////////////////////////////////////////////////////
{
    int	i;

    // Get rid of all the elements in list
    for (i = 0; i < elementsUsed.getLength(); i++)
	delete (SoElement *) elementsUsed[i];

    delete[] elementsUsedFlags;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destroy this cache.  Called by unref(); by default, just calls the
//    destructor.  SoGLRenderCaches use this method to free up display
//    lists, if they can.
//
// Use: protected, virtual

void
SoCache::destroy(SoState *)
//
////////////////////////////////////////////////////////////////////////
{
    delete this;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Increments reference count.
//
// Use: public

void
SoCache::ref()
//
////////////////////////////////////////////////////////////////////////
{
    refCount++;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Decrements reference count, destroying instance if count is now 0.
//
// Use: public

void
SoCache::unref(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    if (--refCount == 0)
	destroy(state);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds an element to elements-used list if not already there and
//    its depth is less than the depth of this cache
//
// Use: public

void
SoCache::addElement(const SoElement *elt)
//
////////////////////////////////////////////////////////////////////////
{
    // See if element is already on list
    int byte = elt->getStackIndex() / 8;
    int bit = elt->getStackIndex() % 8;
    if (elt->getDepth() >= depth
		|| ((elementsUsedFlags[byte] >> bit) & 1))
	return;

    // Make a copy we can later call matches() on:
    SoElement *newElt = elt->copyMatchInfo();
    newElt->setDepth(elt->getDepth());

    // Add it to the list
    elementsUsed.append((void *) newElt);
    elementsUsedFlags[byte] |= (1 << bit);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds a dependency of this instance on another cache instance.
//    The default method makes sure this cache depends on any elements
//    that the sub-cache depends on.
//
// Use: public, virtual

void
SoCache::addCacheDependency(const SoState *state, SoCache *subCache)
//
////////////////////////////////////////////////////////////////////////
{
    for (int i = 0; i < subCache->elementsUsed.getLength(); i++) {
	const SoElement *eltInCache =
	    (const SoElement *) subCache->elementsUsed[i];

	//
	// This is pretty subtle: the depth of the element in the
	// cache may not match the depth of the (matching) element in
	// the state (a cache may be built at a node that is instanced
	// at two different depths, for example).  Because the depths
	// must be correct for the elementsUsed list to work
	// correctly, we must depend on the element in the state.
	//
	addElement(state->getConstElement(eltInCache->getStackIndex()));
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if the cache is valid with resepect to the given
//    state. To be valid, each element in the elements-used list in
//    the cache must match (using the matches() method) the
//    corresponding element in the state, AND the list of overridden
//    elements in the cache must be exactly the same as the
//    corresponding list in the state.
//
// Use: public

SbBool
SoCache::isValid(const SoState *state) const
//
////////////////////////////////////////////////////////////////////////
{
    // If explicitly invalidated, always return FALSE.
    if (invalidated) {
#ifdef DEBUG
	if (SoDebug::GetEnv("IV_DEBUG_CACHES")) {
	    fprintf(stderr, "CACHE DEBUG: cache(0x%x) not valid ",
		    this);
	    fprintf(stderr, "because SoCache::invalidate was called, ");
	    fprintf(stderr, "probably because a field or node\n");
	    fprintf(stderr, "underneath a node with a cache changed ");
	    fprintf(stderr, "or because a node with a cache contained\n");
	    fprintf(stderr, "an uncacheable node.\n");
	}	    
#endif
	return FALSE;
    }

    int	i;

    // Compare used elements for match
    for (i = 0; i < elementsUsed.getLength(); i++) {
	const SoElement *eltInCache = (const SoElement *) elementsUsed[i];
	const SoElement *eltInState =
	    state->getConstElement(eltInCache->getStackIndex());

	// If cache's version of element doesn't match what's in the
	// state, the cache is not valid.
	if (!eltInCache->matches(eltInState)) {

#ifdef DEBUG
	    if (SoDebug::GetEnv("IV_DEBUG_CACHES")) {
		fprintf(stderr, "CACHE DEBUG: cache(0x%x) not valid",
			this);
		fprintf(stderr, " because element %s does not match:\n",
			eltInState->getTypeId().getName().getString());
		fprintf(stderr, "------\nElement in state:\n");
		eltInState->print(stderr);
		fprintf(stderr, "------\nElement in cache:\n");
		eltInCache->print(stderr);
	    }
#endif
	    return FALSE;
	}	    
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the first element in the state that doesn't match an
//    element on the cache's dependency list.  Returns NULL if they
//    all match.
//
// Use: public

const SoElement *
SoCache::getInvalidElement(const SoState *state) const
//
////////////////////////////////////////////////////////////////////////
{
    int	i;

    // Compare used elements for match
    for (i = 0; i < elementsUsed.getLength(); i++) {
	const SoElement *eltInCache = (const SoElement *) elementsUsed[i];
	const SoElement *eltInState =
	    state->getConstElement(eltInCache->getStackIndex());

	if (eltInCache != eltInState && !
	    eltInCache->matches(eltInState)) {

	    return eltInState;
	}	    
    }

    return NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Invalidates this cache.  Nodes sometime invalidate open caches. 
//
// Use: public

void
SoCache::invalidate()
//
////////////////////////////////////////////////////////////////////////
{
    invalidated = TRUE;
}

