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
 |   $Revision: 1.1 $
 |
 |   Classes:
 |	SoGLCacheContextElement
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <stdlib.h>
#include <Inventor/caches/SoGLRenderCache.h>
#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/elements/SoGLCacheContextElement.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/SbString.h>
#include <Inventor/SbPList.h>

SO_ELEMENT_SOURCE(SoGLCacheContextElement);

SbPList		*SoGLCacheContextElement::waitingToBeFreed = NULL;
SbPList		*SoGLCacheContextElement::extensionList = NULL;
SbIntList	*SoGLCacheContextElement::mipmapSupportList = NULL;

// Internal struct:
struct extInfo {
    SbString string;
    SbIntList support;
};

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoGLCacheContextElement::~SoGLCacheContextElement()
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
SoGLCacheContextElement::init(SoState *)
//
////////////////////////////////////////////////////////////////////////
{
    context = 0;
    is2PassTransp = FALSE;
    autoCacheBits = 0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets cache context
//
// Use: public, static

void
SoGLCacheContextElement::set(SoState *state, int ctx,
			     SbBool is2PassTransparency,
			     SbBool remoteRender)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (state->getDepth() != 1) {
	SoDebugError::post("SoGLCacheContextElement::set",
			   "must not be set during traversal");
    }
#endif

    SoGLCacheContextElement *elt = (SoGLCacheContextElement *)
	state->getElementNoPush(classStackIndex);

    elt->context = ctx;
    elt->is2PassTransp = is2PassTransparency;
    elt->isRemoteRendering = remoteRender;
    if (remoteRender) elt->autoCacheBits = DO_AUTO_CACHE;
    else elt->autoCacheBits = 0;

    // Look through the list of display lists waiting to be freed, and
    // free any that match the context:
    for (int i = waitingToBeFreed->getLength()-1; i >= 0; i--) {
	SoGLDisplayList *dl = (SoGLDisplayList *)(*waitingToBeFreed)[i];
	if (dl->getContext() == ctx) {
	    waitingToBeFreed->remove(i);
	    delete dl;
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Gets the cache context.  GL render caches do this when they are
//    opened (they need the context to free themselves properly).
//
// Use: public, static

int
SoGLCacheContextElement::get(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    const SoGLCacheContextElement	*elt;

    elt = (const SoGLCacheContextElement *)
	getConstElement(state, classStackIndex);

    return elt->context;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Frees up the given display lists right away, if possible.  If
//    not possible (because the state passed in is NULL or has a
//    different cache context than the display lists' state), this
//    adds the given display list/count to the list of display lists
//    that need to be freed the next time the given context is valid.
//    This method is necessary because nodes with caches can be
//    deleted at any time, but we can't necessarily send GL commands
//    to free up a display list at any time.
//
// Use: public, static

void
SoGLCacheContextElement::freeList(SoState *state,
				  SoGLDisplayList *dl)
//
////////////////////////////////////////////////////////////////////////
{
    if (state != NULL  &&  get(state) == dl->getContext()) {
	delete dl;
    } else {
	waitingToBeFreed->append(dl);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Converts an extension string to a unique integer to be used for
//    faster lookup (to avoid string comparisons)
//
// Use: public

int
SoGLCacheContextElement::getExtID(const char *str)
//
////////////////////////////////////////////////////////////////////////
{
    for (int i = 0; i < extensionList->getLength(); i++) {
	extInfo *e = (extInfo *)(*extensionList)[i];
	if (e->string == str) return i;
    }
    extInfo *e = new extInfo;
    e->string = str;
    extensionList->append(e);
    return extensionList->getLength()-1;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Given a unique extension ID, return TRUE if that extension is
//    supported in this rendering context.
//
SbBool
SoGLCacheContextElement::extSupported(SoState *state, int ext)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (ext >= extensionList->getLength()) {
	SoDebugError::post("SoGLCacheContextElement::extSupported",
			   "Bad extension ID passed; "
			   "you MUST use SoGLCacheContextElement::getExtID");
    }
#endif
    extInfo *e = (extInfo *)(*extensionList)[ext];
    int ctx = get(state);

    // The support list is a list of context,flag pairs (flag is TRUE
    // if the render context supports the extension).  This linear
    // search assumes that there will be a small number of render
    // contexts.
    for (int i = 0; i < e->support.getLength(); i+=2) {
	if (e->support[i] == ctx) return e->support[i+1];
    }
    // Ask GL if supported:
    SbBool supported = 
	strstr((const char *)glGetString(GL_EXTENSIONS),
	       e->string.getString()) 
	    != NULL;
    e->support.append(ctx);
    e->support.append(supported);

    return supported;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if the current rendering context does 
//    mip-mapped textures quickly.
//
SbBool
SoGLCacheContextElement::areMipMapsFast(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    int ctx = get(state);

    // See if we already know:
    for (int i = 0; i < mipmapSupportList->getLength(); i+=2) {
	if ((*mipmapSupportList)[i] == ctx)
	    return (*mipmapSupportList)[i+1];
    }
    // Don't know, figure it out:

    SbBool result;
    if (strncmp((const char *)glGetString(GL_VENDOR), "SGI", 3) == 0) {
	
	const char *renderer = (const char *)glGetString(GL_RENDERER);
	// Indy and XL
	if (strncmp(renderer, "NEWPORT", 7) == 0)
	    result = FALSE;
	// Personal Iris, XS, XZ, Extreme...
	else if (strncmp(renderer, "GR", 2) == 0)
	    result = FALSE;
	else if (strncmp(renderer, "GU", 2) == 0)
	    result = FALSE;
	// VGX and VGXT
	else if (strncmp(renderer, "VGX", 3) == 0)
	    result = FALSE;
	// Indigo Entry
	else if (strncmp(renderer, "LIGHT", 5) == 0)
	    result = FALSE;
	// IndigoII Impact
	else if (strncmp(renderer, "IMPACT", 6) == 0)
	    result = TRUE;
	else
	    result = TRUE; // Re's are fast, assume all future SGI
			   // machines will do fast texturing.
    }
    else result = FALSE;  // Assume non-SGI machines do texturing slowly

    mipmapSupportList->append(ctx);
    mipmapSupportList->append(result);
    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides this method to compare contexts.
//
// Use: public

SbBool
SoGLCacheContextElement::matches(const SoElement *elt) const
//
////////////////////////////////////////////////////////////////////////
{
    
    const SoGLCacheContextElement *cacheElt;

    cacheElt = (const SoGLCacheContextElement *) elt;

    return (context       == cacheElt->context &&
	    is2PassTransp == cacheElt->is2PassTransp);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//     Create a copy of this instance suitable for calling matches()
//     on.
//
// Use: protected

SoElement *
SoGLCacheContextElement::copyMatchInfo() const
//
////////////////////////////////////////////////////////////////////////
{
    SoGLCacheContextElement *result =
	(SoGLCacheContextElement *)getTypeId().createInstance();

    result->context = context;
    result->is2PassTransp = is2PassTransp;

    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Prints element for debugging.
//
// Use: public
//
////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
void
SoGLCacheContextElement::print(FILE *fp) const
{
    SoElement::print(fp);

    fprintf(fp, "\tCache context = %uL, is2PassTransp = %s\n",
	    context, is2PassTransp ? "TRUE" : "FALSE");
}
#else  /* DEBUG */
void
SoGLCacheContextElement::print(FILE *) const
{
}
#endif /* DEBUG */


int SoGLDisplayList::texture_object_extensionID = -1;

//
// Workaround for nasty OpenGL bug:  we don't use the texture object
// extension if the user sets the IV_NO_TEXTURE_OBJECT environment
// variable, because texture objects are not correctly shared between
// contexts on RE and impact so multi-windowed applications will
// display textures in only one window.
// -- gavin 10/3/95
//
static int never_use_texture_object = -1;  // "don't know" initial value

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoGLDisplayList::SoGLDisplayList(SoState *state, Type _type,
				 int numToAllocate)
//
////////////////////////////////////////////////////////////////////////
{
    refCount = 0;

    num = numToAllocate;

    // We must depend on the GL cache context; we can't assume that a
    // cache is valid between any two render actions, since the render
    // actions could be directed at different X servers on different
    // machines (with different ideas about which display lists have
    // been created).
    context = SoGLCacheContextElement::get(state);

#ifdef GL_EXT_texture_object
    if (texture_object_extensionID == -1) {
	texture_object_extensionID =
	    SoGLCacheContextElement::getExtID("GL_EXT_texture_object");
    }
    int texObjSupported = SoGLCacheContextElement::extSupported(state,
					texture_object_extensionID);

    if (never_use_texture_object == -1) {
	if (getenv("IV_NO_TEXTURE_OBJECT"))
	    never_use_texture_object = 1;
	else
	    never_use_texture_object = 0;
    }	
    if (never_use_texture_object) texObjSupported = 0;

#else
    int texObjSupported = FALSE;
#endif

    // Always use display lists if texture objects are not supported:
    if (_type == TEXTURE_OBJECT && !texObjSupported)
	type = DISPLAY_LIST;
    else
	type = _type;

    if (type == TEXTURE_OBJECT) {
#ifdef GL_EXT_texture_object
	glGenTexturesEXT(1, &startIndex);
#ifdef DEBUG
	if (num != 1)
	    SoDebugError::post("SoGLDisplayList", "Sorry, can only "
			       "construct 1 texture object at a time");
#endif
#else
#ifdef DEBUG
	SoDebugError::post("SoGLDisplayList", "Texture objects not "
			   "supported, but type not reset!");
#endif
#endif
    } else {
	startIndex = glGenLists(num);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//
//
// Use: public

void
SoGLDisplayList::ref()
//
////////////////////////////////////////////////////////////////////////
{
    ++refCount;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//
//
// Use: public

void
SoGLDisplayList::unref(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    --refCount;
    if (refCount <= 0) {
	// Let the CacheContextElement delete us:
	SoGLCacheContextElement::freeList(state, this);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//
//
// Use: public

void
SoGLDisplayList::open(SoState *, int index)
//
////////////////////////////////////////////////////////////////////////
{
    if (type == TEXTURE_OBJECT) {
#ifdef GL_EXT_texture_object
	glBindTextureEXT(GL_TEXTURE_2D, startIndex+index);
#endif
    } else {
	glNewList(startIndex+index, GL_COMPILE_AND_EXECUTE);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//
//
// Use: public

void
SoGLDisplayList::close(SoState *)
//
////////////////////////////////////////////////////////////////////////
{
    if (type != TEXTURE_OBJECT) {
	glEndList();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//
//
// Use: public

void
SoGLDisplayList::call(SoState *state, int index)
//
////////////////////////////////////////////////////////////////////////
{
    if (type == TEXTURE_OBJECT) {
#ifdef GL_EXT_texture_object
	glBindTextureEXT(GL_TEXTURE_2D, startIndex+index);
#endif
    } else {
	glCallList(startIndex+index);
    }
    addDependency(state);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//
//
// Use: public

void
SoGLDisplayList::addDependency(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    if (state->isCacheOpen()) {
	SoGLRenderCache *c = (SoGLRenderCache *)
	    SoCacheElement::getCurrentCache(state);
	c->addNestedCache(this);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//
//
// Use: public

SoGLDisplayList::~SoGLDisplayList()
//
////////////////////////////////////////////////////////////////////////
{
    if (type == TEXTURE_OBJECT) {
#ifdef GL_EXT_texture_object
	glDeleteTexturesEXT(1, &startIndex);
#endif
    } else {
	glDeleteLists(startIndex, num);
    }
}
