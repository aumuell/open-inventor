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
 |	This file defines the abstract SoCache class, the base class
 |	for all caches in Inventor.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_CACHE
#define  _SO_CACHE

////////////////////////////////////////////////////////////////////////
//
//  Class SoCache:
//
//  This is the base class for all types of caches. Each cache
//  maintains the following information:
//
//      A reference count, used to allow sharing of cache instances.
//      ref() and unref() methods increment and decrement the count.
//
//      An elements-used list. This is a list of elements used in the
//      cache that are set outside it. A cache is invalidated if any
//      of these elements has changed since the cache was created.
//      There is also an elements-flag array so we can quickly tell if
//      an element type has already been added to the elements-used
//      list.
//
////////////////////////////////////////////////////////////////////////

#include <Inventor/SoLists.h>

class SoElement;
class SoState;

SoEXTENDER class SoCache {

  public:

    // Constructor. Takes the state in effect when the cache is used;
    // it is assumed that the state is pushed before the cache is
    // created. 
    SoCache(SoState *state);

    // Reference/unreference
    void		ref();
    void		unref(SoState *state = NULL);

    // Adds an element to elements-used list if not already there
    void		addElement(const SoElement *elt);

    // Adds a dependency of this instance on another cache instance.
    // The default method takes care of adding dependencies from the
    // child cache.
    virtual void	addCacheDependency(const SoState *state,
					   SoCache *cache);

    // Returns TRUE if cache is valid with respect to the given state
    virtual SbBool	isValid(const SoState *state) const;

    // Assuming isValid() returns FALSE, this will return the first
    // element that is invalid (auto-caching uses this in its
    // heuristic).  Returns NULL if the cache is not invalid because
    // of an element or if the cache is valid.
    const SoElement *	getInvalidElement(const SoState *state) const;

    // Make this cache invalid
    void		invalidate();

  protected:
    // Subclasses can use this to clean up just before they are
    // deleted.  Render caches use this to free display lists.  Note
    // that the state will be NULL if the cache is no deleted during
    // the application of an action.
    virtual void	destroy(SoState *state);

    // Destructor
    virtual ~SoCache();

  private:
    SbPList		elementsUsed;		// Elements used in cache
    unsigned char	*elementsUsedFlags;	// Which elements on list

    int			refCount;		// Reference count
    int			invalidated;		// True if invalidate called
    int			depth;			// Depth of state
};

#endif /* _SO_CACHE */
