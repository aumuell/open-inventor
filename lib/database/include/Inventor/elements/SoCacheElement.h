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
 |   Classes:
 |	SoCacheElement
 |
 |   Author(s)		: Paul S. Strauss, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_CACHE_ELEMENT_
#define  _SO_CACHE_ELEMENT_

#include <Inventor/elements/SoSubElement.h>

class SoCache;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoCacheElement
//
//  Element that stores the most recently opened cache.
//
//////////////////////////////////////////////////////////////////////////////

SoEXTENDER class SoCacheElement : public SoElement {

    SO_ELEMENT_HEADER(SoCacheElement);

  public:
    // Initializes element
    virtual void	init(SoState *state);

    // Sets cache in element accessed from state
    static void		set(SoState *state, SoCache *cache);

    // Returns the cache stored in an instance. This may be NULL.
    SoCache *		getCache() const { return cache; }

    // Returns TRUE if any cache is currently open in the state
    static SbBool	anyOpen(SoState *state);

    // Invalidate any open caches.  This is called by nodes that
    // should not be cached.
    static void		invalidate(SoState *state);

    // Overrides this method to unref cache
    virtual void	pop(SoState *state, const SoElement *prevTopElement);

    // Overrides this method to print an error message and return
    // FALSE. Cache elements should never be compared, since they
    // never appear in the elements-used list of caches!
    virtual SbBool	matches(const SoElement *elt) const;

    // Copy method prints error and returns NULL; see comment above.
    virtual SoElement	*copyMatchInfo() const;

    // Returns the next cache element in the stack
    SoCacheElement *	getNextCacheElement() const
	{ return (SoCacheElement *) getNextInStack(); }

  SoINTERNAL public:
    // Initializes the SoCacheElement class
    static void		initClass();

    // Adds the given element to the elements used lists of all
    // currently open caches in the state
    static void		addElement(SoState *state, const SoElement *elt);

    // Adds a dependency on the given cache to all currently open
    // caches in the state
    static void		addCacheDependency(SoState *state, SoCache *cache);

    // Sets invalidated bit, and returns its old value.  Used by
    // SoGLCacheList so auto-caching doesn't cache too much.
    static SbBool	setInvalid(SbBool newValue);

    // returns the current cache, from the top of the stack.  Does not
    // cause a cache dependence like getConstElement().
    static SoCache *	getCurrentCache(SoState *state)
	{return ((SoCacheElement*)(state->getElementNoPush(classStackIndex)))->cache;}

    virtual ~SoCacheElement();

  private:
    SoCache		*cache;			// Stores pointer to cache

    static SbBool	invalidated;		// invalidate() called?

friend class SoElement;
};

#endif /* _SO_CACHE_ELEMENT_ */
