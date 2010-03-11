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
 |	SoGLCacheList, SoGLCacheListEntry
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <assert.h>

#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/caches/SoGLCacheList.h>
#include <Inventor/caches/SoGLRenderCache.h>
#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/elements/SoGLCacheContextElement.h>
#include <Inventor/misc/SoState.h>

#include <SoDebug.h>

static const int maxThreshold = 100;

struct SoGLCacheListEntry {
    SoGLCacheListEntry() { cache = NULL;
			   prev = next = this;
		       }
    SoGLRenderCache *cache;
    SoGLCacheListEntry *prev, *next;
};    

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoGLCacheList::SoGLCacheList(int num)
//
////////////////////////////////////////////////////////////////////////
{
    maxCaches = num;
    numCaches = 0;

    MRU = NULL;
    openCache = NULL;

    threshold = 1;
    mightBeUsed = 0;  // Wait one frame to build caches, to allow text,
		      // textures, or separators with renderCaching ON
		      // to build caches.
    invalidElement = NULL;

    saveACacheBits = SoGLCacheContextElement::DO_AUTO_CACHE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: public

SoGLCacheList::~SoGLCacheList()
//
////////////////////////////////////////////////////////////////////////
{
    if (MRU != NULL) {
	// Break loop:
	MRU->prev->next = NULL;
	for (SoGLCacheListEntry *e = MRU; e != NULL;) {
	    SoGLCacheListEntry *t = e;
	    e = t->next;
	    if (t->cache != NULL) t->cache->unref(NULL);
	    delete t;
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Search through the list of caches, and call the first one that
//    is valid.  If a valid cache was found, return TRUE; else return
//    FALSE.
//
// Use: public

SbBool
SoGLCacheList::call(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoState *state = action->getState();

    for (SoGLCacheListEntry *c = MRU; c != NULL;
	 c = (c->next == MRU ? NULL : c->next)) {

	if (c->cache != NULL && c->cache->isValid(state)) {

#ifdef DEBUG
	    if (SoDebug::GetEnv("IV_DEBUG_CACHELIST")) {
		SoNode *tail = action->getCurPath()->getTail();
		const char *tailName = tail->getName().getString();
		if (!tailName || tailName[0]=='\0') tailName = "<noName>";
		SoDebug::RTPrintf("GLCacheList: Using cache 0x%x, node %s\n",
				  c->cache, tailName);
	    }
#endif
	    c->cache->call(state);

	    // Mark this cache as the most-recently used
	    setMRU(c);
	    
	    // Keep track of some stuff used for auto-caching:
	    // See the big comment below...
	    mightBeUsed = 0;
	    if (invalidElement) {
		delete invalidElement;
		invalidElement = NULL;
	    }
	    // Decrease threshold.  This is kind of wacky--
	    // threshold is doubled when a cache is blown, and
	    // 3/4'ed when a cache is used.  So
	    // we penalize blowing more than using...
	    threshold = (threshold*3)/4;
	    if (threshold < 1)
		threshold = 1;
	    return TRUE;
	}
    }
    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Try to open a cache.  If the autoCache flag is TRUE, we notice
//    when elements outside the cache are changing and try to build
//    caches only when they aren't changing (we actually only keep
//    track of one element, on the assumption that most of the time
//    the same elements are all changing together).  We also keep
//    track of when caches are blown and used, and if caches blow too
//    much our threshold for number of frames to wait before
//    rebuilding is increased (it is decreased when caches are used).
//
// Use: public

void
SoGLCacheList::open(SoGLRenderAction *action, SbBool autoCache)
//
////////////////////////////////////////////////////////////////////////
{
    assert (openCache == NULL);

    SoState *state = action->getState();

    // If there is already an open GLRender cache, bail.
    if (SoCacheElement::anyOpen(state))
	return;

    // If no render caches are to be used, do nothing
    if (maxCaches <= 0)
	return;

    if (!autoCache) {
	// Blow the least recently used cache unconditionally.
	openCache = getLRU();
    }
// The interesting case.  There are several common cases that we are
// trying to accomodate here:
// 1. Viewing an unchanging scene.  We want caches to be built the
//    first frame.
// 2. Viewing with an 'interactive' drawstyle-- e.g. move low
//    complexity.  We want two caches to be built immediately, the
//    first frame they are used.
// 3. Viewing an animating scene.  There should be no caches built
//    above things that are changing (notifying).
// 4. Interactively changing an overridden property at the top of the
//    scene graph (e.g. changing a Complexity slider).  We want to
//    avoid building caches while it is changing.
// 5. Texture and text nodes, and Separators with renderCaching
//    explicitly turned ON (e.g. so instances share the same cache)
//    should have a chance to get built so that we don't build display
//    lists that are bigger than they should be.
//
// We compromise in the first and second cases to acommodate nodes
// that want a chance to build caches; caches are built the second
// frame by initializing mightBeUsed to 0 and threshold to 1.
//
// We rely on animation notification happening before rendering for
// the third case to be handled correctly, and assume that
// notification is happening before every render so caches are never
// built (invalidateAll sets mightBeUsed to zero).
//
// For the last case we have to compromise, since the first couple of
// frames are indistinguishable from the second case.  We remember the
// first element that caused the most-recently-used cache to be
// considered invalid, and only increment mightBeUsed if that element
// stays the same.
//
// In any case, the threshold used to determine how many times the
// cache might be used is increased a lot when a cache is destroyed
// because elements above changed, and decreased a little whenever a
// cache is used.  This will give reasonable behavior even in cases
// where (for example) two elements are interacting, with each staying
// the same for a couple of frames, but each out of sync with the
// other (a rare case, but it could happen...).
//
// Also, we only do this if there is some node underneath us that has
// requested auto-caching AND no node underneath use has requested
// that it NOT be cached:
//
    else if (saveACacheBits == SoGLCacheContextElement::DO_AUTO_CACHE) {
	if (invalidElement == NULL) {
	    if (MRU && MRU->cache) {
		const SoElement *eltInState =
		    MRU->cache->getInvalidElement(state);
		if (eltInState)
		    invalidElement = eltInState->copyMatchInfo();
	    }
	    ++mightBeUsed;
	} else {
	    const SoElement *eltInState =
		state->getConstElement(invalidElement->getStackIndex());
	    if (invalidElement->matches(eltInState)) {
		++mightBeUsed;
	    } else {
		// update element-last-blown:
		delete invalidElement;
		invalidElement = eltInState->copyMatchInfo();

		// Still changing, reset mightBeUsed
		mightBeUsed = 0;
	    }
	}

	if (mightBeUsed > threshold) {
	    // Replace the least recently used cache.
	    openCache = getLRU();
	}
    }

    // If we've decided to replace a cache...
    if (openCache) {
	blow(state, openCache);

	openCache->cache = new SoGLRenderCache(state);
	openCache->cache->ref();
	openCache->cache->open(state);
#ifdef DEBUG
	if (SoDebug::GetEnv("IV_DEBUG_CACHELIST")) {
	    SoNode *tail = action->getCurPath()->getTail();
	    const char *tailName = tail->getName().getString();
	    if (!tailName || tailName[0]=='\0') tailName = "<noName>";
	    SoDebug::RTPrintf("GLCacheList: New cache 0x%x for node %s:"
			      "mightBeUsed(%d), threshold(%d)\n",
			      openCache->cache, tailName, mightBeUsed,
			      threshold);
	}
#endif
	mightBeUsed = 0;
    } else {
	// No cache, and no cache already open (the anyOpen() test
	// takes care of that); save away the cache element's invalid
	// state (to be restored in ::close()) so we can tell if
	// anything is invalidating caches:
	saveInvalid = SoCacheElement::setInvalid(FALSE);
    }
    saveACacheBits = SoGLCacheContextElement::resetAutoCacheBits(state);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Close the cache opened by open().  If there is no open cache but
//    something is invalidating caches, set mightBeUsed to zero to
//    avoid building caches unnecessarily.
//
// Use: public

void
SoGLCacheList::close(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // If there is a cache open, close it, remember that is has been
    // used once, and remember that it is the most recently used
    // cache.

    if (openCache) {
	// Check for NULL case. This can happen if there was notification
	// during the time the cache was being filled.
	if (openCache->cache) {
	    openCache->cache->close();
	    setMRU(openCache);
	}
	openCache = NULL;
    }
    else {
	SbBool wasInvalidated = SoCacheElement::setInvalid(saveInvalid);
	if (wasInvalidated) {
	    // Pass info up to parents...
	    (void)SoCacheElement::setInvalid(TRUE);
	    mightBeUsed = 0;
	}
    }

    // Do some bit gymnastics; saveACacheBits will be the bits as set
    // by traversal of children between open() and close().
    SoState *state = action->getState();
    int prevBits = saveACacheBits;
    saveACacheBits = SoGLCacheContextElement::resetAutoCacheBits(state);
    SoGLCacheContextElement::setAutoCacheBits(state,
		prevBits | saveACacheBits);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Free up all caches.  Called by Separator::notify.
//
// Use: public

void
SoGLCacheList::invalidateAll()
//
////////////////////////////////////////////////////////////////////////
{
    for (SoGLCacheListEntry *c = MRU; c != NULL;
	 c = (c->next == MRU ? NULL : c->next)) {
	if (c->cache) {
#ifdef DEBUG
	    if (SoDebug::GetEnv("IV_DEBUG_CACHELIST")) {
		SoDebug::RTPrintf("GLCacheList: Freeing cache 0x%x\n",
				  c->cache);
	    }
#endif
	    if (threshold < maxThreshold)
		threshold = (threshold+1)*2;

	    c->cache->unref(NULL);
	    c->cache = NULL;
	}
    }
    mightBeUsed = 0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Returns the least recently used cache, which is the one that we
//   will choose to blow if we need to blow one.
//
// Use: public

SoGLCacheListEntry *
SoGLCacheList::getLRU()
//
////////////////////////////////////////////////////////////////////////
{
    if (MRU == NULL) {
	MRU = new SoGLCacheListEntry;
	numCaches = 1;
    }
    else if (numCaches != maxCaches) {
	SoGLCacheListEntry *t = new SoGLCacheListEntry;
	setLRU(t);
	++numCaches;
    }

    return MRU->prev;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Mark a cache as the most recently used cache in the list.
//
// Use: public

void
SoGLCacheList::setMRU(SoGLCacheListEntry *e)
//
////////////////////////////////////////////////////////////////////////
{
    if (e == MRU) return;

    // First, remove e from the list:
    e->prev->next = e->next;
    e->next->prev = e->prev;

    // Now add it as MRU:
    e->next = MRU;
    e->prev = MRU->prev;
    MRU->prev->next = e;
    MRU->prev = e;
    MRU = e;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Mark a cache as the least recently used cache in the list.
//
// Use: private

void
SoGLCacheList::setLRU(SoGLCacheListEntry *e)
//
////////////////////////////////////////////////////////////////////////
{
    // Use a little trickery here-- make the entry the MRU, then bump
    // the MRU pointer and e is suddenly the end of the list...
    if (e->next != MRU) {
	setMRU(e);
	MRU = e->next;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Blow (destroy, delete, get rid of) a cache.  Called by
//    by open() when it decides it will be worthwhile nuking an old
//    cache.
//
// Use: public

void
SoGLCacheList::blow(SoState *state, SoGLCacheListEntry *t)
//
////////////////////////////////////////////////////////////////////////
{
    if (t->cache) {

#ifdef DEBUG
	if (SoDebug::GetEnv("IV_DEBUG_CACHELIST")) {
	    SoDebug::RTPrintf("GLCacheList: Blowing cache 0x%x\n",
			      t->cache);
	}
#endif

    	t->cache->unref(state);
	t->cache = NULL;
	setLRU(t);
	mightBeUsed = 0;
	if (threshold < maxThreshold)
	    threshold = (threshold+1)*2;   // Penalize!
    }
}
