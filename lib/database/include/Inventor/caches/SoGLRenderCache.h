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
 |	This file defines the SoGLRenderCache class, which is used for
 |	storing caches during GL rendering.
 |
 |   Author(s)		: Paul S. Strauss, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

// C-api: end

#ifndef  _SO_GL_RENDER_CACHE
#define  _SO_GL_RENDER_CACHE

#include <GL/gl.h>
#include <Inventor/caches/SoCache.h>
#include <Inventor/elements/SoGLLazyElement.h>

////////////////////////////////////////////////////////////////////////
//
//  Class SoGLRenderCache:
//
//  A render cache stores a GL display list containing commands that
//  draw a representation of the shape(s) represented by the cache.
//  The display list id is stored in the cache instance.
//
//  Each instance has a list of all nested display lists
//  that it calls. This list maintains reference counts on the
//  instances within it.
//  
//  This version has a copy of SoGLLazyElement, which is handled differently
//  than other elements.
//
////////////////////////////////////////////////////////////////////////

class SoGLDisplayList;

SoEXTENDER class SoGLRenderCache : public SoCache {

  public:
    // Constructor and destructor
    SoGLRenderCache(SoState *state);
    ~SoGLRenderCache();

    // Maintain list of cache dependencies.  If one cache calls
    // another inside of it, we have to make sure that the inner cache
    // isn't deleted before the outer one.
    virtual void	addNestedCache(SoGLDisplayList *child);

    // Opens/closes the cache. All GL commands made between an open()
    // and a close() will be part of the the display list cache.
    // Calling open automatically sets SoCacheElement.
    void		open(SoState *state);
    void		close();

    // Calls the cache. The display list will be sent to GL.  This
    // automatically calls addCacheDependency on any open cache.
    void		call(SoState *state);

    // Returns TRUE if cache is valid with respect to the given state
    // The GL version tests for GL Enable flags, first.
    virtual SbBool      isValid(const SoState *state) const;
 
    // method for the lazy element to set flags:
    void setLazyBits(uint32_t ivFlag, uint32_t glFlag, uint32_t sendFlag)
	{checkGLFlag |= glFlag;
	checkIVFlag |= ivFlag;
	doSendFlag |= sendFlag;}
	
    SoGLLazyElement* getLazyElt() 
	{return GLCacheLazyElement;}

  protected:
    // Free display lists before deletion, if possible (if not, they
    // are added to a list to be deleted later by the
    // CacheContextElement).
    virtual void	destroy(SoState *state);

  private:
    SoGLDisplayList	*list;		// Display list structure
    SbBool		listOpen;	// Whether display list is open
    SbPList		nestedCaches;	// List of nested caches

    // Save state that opened a cache, to use when it is closed:
    SoState *		saveState;

    // Keep a copy of SoGLLazyElement for comparison
    SoGLLazyElement *	GLCacheLazyElement;
    
    // Keep a copy of the GL state for copying back after cache call:
    SoGLLazyElement::GLLazyState    cachedGLState;

    // BitFlags for maintaining Lazy Element:
    // indicates that GL must match for cache to be valid
    uint32_t		checkGLFlag;

    // indicates that IV must match for cache to be valid
    uint32_t		checkIVFlag;

    // indicates that a send must be issued prior to calling cache.
    uint32_t		doSendFlag;
    
};

#endif /* _SO_GL_RENDER_CACHE */
