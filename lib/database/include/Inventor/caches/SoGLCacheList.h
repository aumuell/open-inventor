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
 |   $Revision: 1.1 $
 |
 |   Description:
 |	This file defines the SoGLCacheList class, which can be used
 |      is used by nodes like Separator to maintain a list of render
 |      caches that might be used.  It also implements an auto caching
 |      scheme that tries to be smart about only building caches when
 |      things are not changing.
 |
 |   Typical use:
 |      In a node's GLRender method:
 |         if (cacheList->call(state)) return;
 |         cacheList->open(state, TRUE);
 |         ... do normal children stuff...
 |         cacheList->close();
 |
 |     In a node's notify method:
 |         cacheList->invalidateAll();
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#ifndef  _SO_GL_CACHE_LIST
#define  _SO_GL_CACHE_LIST

#include <Inventor/SbBasic.h>

// C-api: end

struct SoGLCacheListEntry;
class SoGLRenderAction;
class SoState;
class SoElement;

SoEXTENDER class SoGLCacheList {
    
  public:
    // Constructor.  Takes the maximum number of caches to build.
    SoGLCacheList(int numCaches = 2);

    // Destructor.
    ~SoGLCacheList();
    
    // Given an SoGLRenderAction, try to use one of the caches we've
    // built.  If none of the caches can be used, this returns FALSE.
    // If this returns TRUE, then a cache has been called and all the
    // GL commands in it have already been done.
    SbBool call(SoGLRenderAction *);
    
    // Open a new cache, maybe.  If the autoCache flag is TRUE, a
    // heuristic will be used to figure out whether or not caching
    // should be done (based on when caches were built, when they were
    // used, etc).  If the flag is FALSE, always try to open a cache
    // (and possibly delete an old cache).
    void open(SoGLRenderAction *, SbBool autoCache = TRUE);

    // Close an open cache, if any.  If open() didn't open a cache,
    // this routine does nothing.
    void close(SoGLRenderAction *);
    
    // Mark all caches as invalid.  This should be called when a node
    // is notified of changes in its children or fields.
    void invalidateAll();
    
  private:
    SoGLCacheListEntry *getLRU();
    void setMRU(SoGLCacheListEntry *e);
    void setLRU(SoGLCacheListEntry *e);
    void blow(SoState *, SoGLCacheListEntry *);

    // MRU is the head of a looped list of caches.  Every time
    // a cache is used, it is added to the front of the list.  When we
    // decide to replace a cache, the end cache on the list is used.
    SoGLCacheListEntry *MRU;
    int maxCaches, numCaches;
    SoGLCacheListEntry *openCache;
    int threshold;	// Number of frames to wait before trying to
			// build a cache.  This is increased when caches
			// are found to be invalid, and decreased when a
			// cache is successfully used.
    int mightBeUsed;	// When we can't use a cache, this counts the
			// number of times open() is called and we
			// think we might have been able to use a cache, if
    			// there was one built.  The bigger this gets,
			// the more likely we are to use a cache.
    SoElement *invalidElement;
			// This is the element that caused the most
			// recently used cache to be considered
			// invalid.  If it doesn't change, a cache
			// will be built (if it does continue to
			// change, we'll avoid building a cache).
    SbBool saveInvalid;	// Used to remember whether or not a node
			// called SoCacheElement::invalidate() between
			// open() and close().  This scheme assumes
			// that nodes won't apply() another action
			// that uses the CacheElement during
			// rendering (that would screw up caching
			// anyway, so that is a good assumption).
    int saveACacheBits;	// At cache->open() time we get these from the
			// SoGLCacheContextElement, clear them before
			// traversing children, and reset them at
			// cache->close() time.

};

// C-api: begin

#endif /* _SO_GL_CACHE_LIST */
