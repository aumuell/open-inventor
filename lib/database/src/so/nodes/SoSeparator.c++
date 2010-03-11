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
 |	SoSeparator
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SbBox.h>
#include <Inventor/actions/SoAction.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/caches/SoBoundingBoxCache.h>
#include <Inventor/caches/SoGLCacheList.h>
#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/elements/SoGLCacheContextElement.h>
#include <Inventor/elements/SoLocalBBoxMatrixElement.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/nodes/SoSeparator.h>

#ifdef DEBUG
#include <SoDebug.h>
#endif

#include <stdlib.h>

SO_NODE_SOURCE(SoSeparator);

int SoSeparator::numRenderCaches = 2;

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoSeparator class.
//
// Use: internal

void
SoSeparator::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoSeparator, "Separator", SoGroup);

    // Enable cache element in those actions that support caching
    SO_ENABLE(SoGetBoundingBoxAction,	SoCacheElement);
    SO_ENABLE(SoGLRenderAction,		SoCacheElement);
    SO_ENABLE(SoGLRenderAction,		SoGLCacheContextElement);

    // Allow environment var to control caching:
    const char *NRC;
    if ((NRC = getenv("IV_SEPARATOR_MAX_CACHES")) != NULL) {
	numRenderCaches = atoi(NRC);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoSeparator::SoSeparator()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoSeparator);

    SO_NODE_ADD_FIELD(renderCaching,		(AUTO));
    SO_NODE_ADD_FIELD(boundingBoxCaching,	(AUTO));
    SO_NODE_ADD_FIELD(renderCulling,		(AUTO));
    SO_NODE_ADD_FIELD(pickCulling,		(AUTO));

    // Set up static info for enum fields
    SO_NODE_DEFINE_ENUM_VALUE(CacheEnabled, ON);
    SO_NODE_DEFINE_ENUM_VALUE(CacheEnabled, OFF);
    SO_NODE_DEFINE_ENUM_VALUE(CacheEnabled, AUTO);

    // Set up info in enumerated type fields
    SO_NODE_SET_SF_ENUM_TYPE(renderCaching,	CacheEnabled);
    SO_NODE_SET_SF_ENUM_TYPE(boundingBoxCaching,CacheEnabled);
    SO_NODE_SET_SF_ENUM_TYPE(renderCulling,	CacheEnabled);
    SO_NODE_SET_SF_ENUM_TYPE(pickCulling,	CacheEnabled);

    bboxCache   = NULL;
    cacheList = new SoGLCacheList(numRenderCaches);
    isBuiltIn   = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor taking approximate number of children
//
// Use: public

SoSeparator::SoSeparator(int nChildren) : SoGroup(nChildren)
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoSeparator);

    SO_NODE_ADD_FIELD(renderCaching,		(AUTO));
    SO_NODE_ADD_FIELD(boundingBoxCaching,	(AUTO));
    SO_NODE_ADD_FIELD(renderCulling,		(AUTO));
    SO_NODE_ADD_FIELD(pickCulling,		(AUTO));

    // Set up static info for enum fields
    SO_NODE_DEFINE_ENUM_VALUE(CacheEnabled, ON);
    SO_NODE_DEFINE_ENUM_VALUE(CacheEnabled, OFF);
    SO_NODE_DEFINE_ENUM_VALUE(CacheEnabled, AUTO);

    // Set up info in enumerated type fields
    SO_NODE_SET_SF_ENUM_TYPE(renderCaching,	CacheEnabled);
    SO_NODE_SET_SF_ENUM_TYPE(boundingBoxCaching,CacheEnabled);
    SO_NODE_SET_SF_ENUM_TYPE(renderCulling,	CacheEnabled);
    SO_NODE_SET_SF_ENUM_TYPE(pickCulling,	CacheEnabled);

    bboxCache   = NULL;
    cacheList = new SoGLCacheList(numRenderCaches);
    isBuiltIn   = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor has to free caches
//
// Use: private

SoSeparator::~SoSeparator()
//
////////////////////////////////////////////////////////////////////////
{
    if (bboxCache != NULL)
	bboxCache->unref();

    delete cacheList;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides method in SoNode to return FALSE.
//
// Use: public

SbBool
SoSeparator::affectsState() const
//
////////////////////////////////////////////////////////////////////////
{
    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Set the number of render caches a newly created separator can
//    have.
//
// Use: public, static

void
SoSeparator::setNumRenderCaches(int howMany)
//
////////////////////////////////////////////////////////////////////////
{
    numRenderCaches = howMany;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the number of render caches newly created separators
//    will have.
//
// Use: public, static

int
SoSeparator::getNumRenderCaches()
//
////////////////////////////////////////////////////////////////////////
{
    return numRenderCaches;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Turn off notification on fields to avoid notification when
//    reading, so that caching works properly:
//
// Use: protected

SbBool
SoSeparator::readInstance(SoInput *in, unsigned short flags)
//
////////////////////////////////////////////////////////////////////////
{
    int i;
    SoFieldList myFields;
    getFields(myFields);
    for (i = 0; i < myFields.getLength(); i++) {
	myFields[i]->enableNotify(FALSE);
    }

    SbBool result = SoGroup::readInstance(in, flags);

    for (i = 0; i < myFields.getLength(); i++) {
	myFields[i]->enableNotify(TRUE);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Passes on notification after invalidating all caches and
//    recording that notification has passed through a separator
//
// Use: internal

void
SoSeparator::notify(SoNotList *list)
//
////////////////////////////////////////////////////////////////////////
{
    // Destroy all caches, if present
    if (bboxCache != NULL) {
	bboxCache->unref();
	bboxCache = NULL;
    }
    cacheList->invalidateAll();

    // Then do the usual stuff
    SoGroup::notify(list);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements typical traversal.
//
// Use: extender

void
SoSeparator::doAction(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    int		numIndices;
    const int	*indices;

    // This differs from SoGroup: if the separator is not on the
    // path, don't bother traversing its children

    switch (action->getPathCode(numIndices, indices)) {

      case SoAction::NO_PATH:
      case SoAction::BELOW_PATH:
	action->getState()->push();
	children->traverse(action);
	action->getState()->pop();
	break;

      case SoAction::IN_PATH:
	action->getState()->push();
	children->traverse(action, 0, indices[numIndices - 1]);
	action->getState()->pop();
	break;

      case SoAction::OFF_PATH:
	break;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements callback action for separator nodes.
//
// Use: extender

void
SoSeparator::callback(SoCallbackAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoSeparator::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Traversal for computing bounding box.  This also takes care of
//    creating the bounding box cache.
//
// Use: extender

void
SoSeparator::getBoundingBox(SoGetBoundingBoxAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool	canCache;
    SoState	*state = action->getState();

    int		numIndices;
    const int	*indices;

    switch (action->getPathCode(numIndices, indices)) {
      case SoAction::OFF_PATH:
	// If off the path, don't need to do anything.
	return;

      case SoAction::NO_PATH:
      case SoAction::BELOW_PATH:
	canCache = (boundingBoxCaching.getValue() != OFF &&
		    ! action->isInCameraSpace() &&
		    ! action->isResetPath());
	break;

      case SoAction::IN_PATH:
	canCache = FALSE;
	break;
    }

    // If we have a valid cache already, just use it
    if (canCache && bboxCache != NULL && bboxCache->isValid(state)) {

	SoCacheElement::addCacheDependency(state, bboxCache);

	action->extendBy(bboxCache->getBox());

	// We want the center to be transformed by the current local
	// transformation, just as if we were a shape node
	if (bboxCache->isCenterSet())
	    action->setCenter(bboxCache->getCenter(), TRUE);

	// If our cache has lines or points, set the flag in any open
	// caches above us
	if (bboxCache->hasLinesOrPoints())
	    SoBoundingBoxCache::setHasLinesOrPoints(state);
    }

    // If we can't cache, just do what group does, with push/pop added
    else if (! canCache) {
	state->push();
	SoGroup::getBoundingBox(action);
	state->pop();
    }

    // Otherwise, we have to do some extra work
    else {
	// Save the current bounding box from the action and empty it.
	// (We can assume the center has not been set, or else some
	// group is not doing its job.)
	SbXfBox3f	savedBBox = action->getXfBoundingBox();
	action->getXfBoundingBox().makeEmpty();

	state->push();

	// Set the local bbox matrix to identity, so shapes' bounding
	// boxes will be transformed into our local space
	SoLocalBBoxMatrixElement::makeIdentity(state);

	// Build cache. We've already tested for a valid cache, so the
	// only other possibility is for a NULL cache or an invalid one
	if (bboxCache != NULL)
	    bboxCache->unref();

	// Create a new cache:
	bboxCache = new SoBoundingBoxCache(state);
	bboxCache->ref();
	SoCacheElement::set(state, bboxCache);

	// Traverse the kids
	SoGroup::getBoundingBox(action);

	// This has to be done before the extendBy
	state->pop();

	// Save the bounding box around our kids and save the center
	SbXfBox3f	kidsBBox      = action->getXfBoundingBox();
	SbVec3f		kidsCenter    = action->getCenter();
	SbBool		kidsCenterSet = action->isCenterSet();

	// Store it in the cache
	// Note: bboxCache might be NULL if notification happened
	// during traversal.
	if (bboxCache != NULL)
	    bboxCache->set(kidsBBox, kidsCenterSet, kidsCenter);
#ifdef DEBUG
	else {
	    SoDebugError::post("SoSeparator::getBoundingBox",
			       "Bbox cache was destroyed during "
			       "traversal, meaning a change was "
			       "made to the scene during a "
			       "getBoundingBox action.  If you "
			       "must change the scene during an "
			       "action traversal, you should disable "
			       "notification first using methods "
			       "on SoFieldContainer or SoField.");
	}
#endif
	
	// If the bounding box was reset by one of our children, we
	// don't want to restore the previous bounding box. Instead,
	// we just set it to the children's bounding box so that the
	// current local transformation is multiplied in. Otherwise,
	// we have to restore the previous bounding box and extend it
	// by the children's bounding box.
	if (action->isResetPath() &&
	    (action->getWhatReset() & SoGetBoundingBoxAction::BBOX) != 0 &&
	    action->getResetPath()->containsNode(this))
	    action->getXfBoundingBox().makeEmpty();
	else
	    action->getXfBoundingBox() = savedBBox;

	// Extend the bounding box by the one our kids returned -
	// this will multiply in the current local transformation
	action->extendBy(kidsBBox);
	
	// Set the center to be the computed center of our kids,
	// transformed by the current local transformation
	if (kidsCenterSet) {
	    action->resetCenter();
	    action->setCenter(kidsCenter, TRUE);
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements getMatrix action.
//
// Use: extender

void
SoSeparator::getMatrix(SoGetMatrixAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    int		numIndices;
    const int	*indices;

    // Only need to compute matrix if separator is a node in middle of
    // current path chain. We don't need to push or pop the state,
    // since this shouldn't have any effect on other nodes being
    // traversed.

    if (action->getPathCode(numIndices, indices) == SoAction::IN_PATH)
	children->traverse(action, 0, indices[numIndices - 1]);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Traversal for rendering.  This is different from generic
//    traversal because we may need to do caching.
//
// Use: extender

void
SoSeparator::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    int numIndices;
    const int *indices;
    SoAction::PathCode pc = action->getPathCode(numIndices,indices);
    if (pc == SoAction::NO_PATH || pc == SoAction::BELOW_PATH)
	GLRenderBelowPath(action);
    else if (pc == SoAction::IN_PATH)
	GLRenderInPath(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Renders all children.  Also does caching and culling.
//
// Use: extender

void
SoSeparator::GLRenderBelowPath(SoGLRenderAction *action)

////////////////////////////////////////////////////////////////////////
{
    SoState *state = action->getState();

    // Do a cull test, if culling is turned on:
    int savedCullBits;

    // For now, just do culling if turned ON explicitly.  Eventually,
    // we might want to change this to:
    //  (cullFlag == AUTO && !state->isCacheOpen()) || (cullFlag == ON)
    SbBool doCullTest = (renderCulling.getValue() == ON);

    if (doCullTest) {
	int cullBits = savedCullBits = action->getCullTestResults();

	if (cullBits) {
#ifdef DEBUG
	    static int printCullInfo = -1;
	    if (printCullInfo == -1)
		printCullInfo =
		    SoDebug::GetEnv("IV_DEBUG_RENDER_CULL") != NULL;
	    if (printCullInfo) {
		if (getName().getLength() != 0)
		    SoDebug::RTPrintf("Separator named %s",
				      getName().getString());
		else
		    SoDebug::RTPrintf("Separator 0x%x", this);
	    }
#endif
	    if (cullTest(action, cullBits)) {
#ifdef DEBUG
		if (printCullInfo)
		    SoDebug::RTPrintf("  render culled\n");
#endif
		// Don't cache above if doing culling:
		SoGLCacheContextElement::shouldAutoCache(state,
		    SoGLCacheContextElement::DONT_AUTO_CACHE);
		return;
	    }
#ifdef DEBUG
	    if (printCullInfo)
		printf(" render cull results: %c%c%c\n",
		       cullBits&1 ? 'S' : 'i',
		       cullBits&2 ? 'S' : 'i',
		       cullBits&4 ? 'S' : 'i');
#endif
	    action->setCullTestResults(cullBits);
	}
    }

    SbBool canCallCache = (renderCaching.getValue() != OFF);
    SbBool canBuildCache = (canCallCache  && ! state->isCacheOpen());

    state->push();

    // if we can't call a cache:
    if (canCallCache && cacheList->call(action)) {
	// Just pop the state
	state->pop();
    } else {
	if (canBuildCache) {
	    // Let the cacheList open a new cache, if it can.  This
	    // HAS to come after push() so that the cache element can
	    // be set correctly.
	    cacheList->open(action, renderCaching.getValue() == AUTO);
	}

	action->pushCurPath();
	const int numKids = children->getLength();
	for (int i = 0; i < numKids && !action->hasTerminated(); i++) {
	    action->popPushCurPath(i);
	    if (! action->abortNow())
		((SoNode *)children->get(i))->GLRenderBelowPath(action);
	    else
		SoCacheElement::invalidate(action->getState());
	}
	action->popCurPath();
	state->pop();
	if (canBuildCache) {
	    // Let the cacheList close the cache, if it decided to
	    // open one.  This HAS to come after the pop() so that any
	    // GL commands executed by pop() are part of the display
	    // list.
	    cacheList->close(action);
	}
    }
    // Reset cull bits, if did a cull test:
    if (doCullTest) {
	action->setCullTestResults(savedCullBits);

	// Don't cache above if doing culling:
	SoGLCacheContextElement::shouldAutoCache(state,
		SoGLCacheContextElement::DONT_AUTO_CACHE);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does the GL render action
//
// Use: extender

void
SoSeparator::GLRenderInPath(SoGLRenderAction *action)

////////////////////////////////////////////////////////////////////////
{
    int numIndices;
    const int *indices;
    SoAction::PathCode pc = action->getPathCode(numIndices, indices);
    if (pc == SoAction::IN_PATH) { // still rendering in path:
	SoState *state = action->getState();
	state->push();
        int whichChild = 0;
        for (int i = 0; i < numIndices && !action->hasTerminated(); i++) {
	    while (whichChild < indices[i] && !action->hasTerminated()) {
		SoNode *kid = (SoNode *)children->get(whichChild);
                if (kid->affectsState()) {
                    action->pushCurPath(whichChild);
		    if (! action->abortNow())
			kid->GLRenderOffPath(action);
		    else
			SoCacheElement::invalidate(action->getState());
                    action->popCurPath(pc);
                }
                ++whichChild;
	    }
	    action->pushCurPath(whichChild);
	    if (action->abortNow())
		SoCacheElement::invalidate(action->getState());
	    else
		((SoNode *)children->get(whichChild))->GLRenderInPath(action);
	    action->popCurPath(pc);
	    ++whichChild;
        }
	state->pop();
    } else if (pc == SoAction::BELOW_PATH) { // This must be tail node
        GLRenderBelowPath(action);
    } else { // This should NEVER happen:
#ifdef DEBUG
	SoDebugError::post("SoSeparator::GLRenderInPath",
			   "PathCode went to NO_PATH or OFF_PATH!");
#endif
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does the GL render action
//
// Use: extender

void
SoSeparator::GLRenderOffPath(SoGLRenderAction *)

////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    SoDebugError::post("SoSeparator::GLRenderOffPath",
			   "affectsState() is FALSE");
#endif
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements handle event action for separator nodes.
//
// Use: extender

void
SoSeparator::handleEvent(SoHandleEventAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoSeparator::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Figure out if this separator is culled or not.  Returns TRUE if
//    the separator is culled.
//
// Use: extender

SbBool
SoSeparator::cullTest(SoGLRenderAction *action, int &cullBits)
//
////////////////////////////////////////////////////////////////////////
{
    // Don't bother if bbox caching is turned off:
    if (boundingBoxCaching.getValue() == OFF) return FALSE;

    SoState *state = action->getState();
    
    // Next, get our bounding box.  We do this in a way that is a
    // little dangerous and hacky-- we use the state from the
    // renderAction and pass to bounding box cache's isValid.  This
    // assumes that the state for the bounding box and render action
    // can be compared, which happens to be true (all elements needed
    // for getBoundingBox are also needed for glRender).

    if (bboxCache == NULL || !bboxCache->isValid(state)) {
	static SoGetBoundingBoxAction *bba = NULL;
	if (!bba) 
	    bba = new SoGetBoundingBoxAction(
		SoViewportRegionElement::get(state));
	else
	    bba->setViewportRegion(SoViewportRegionElement::get(state));

	bba->apply((SoPath *)action->getCurPath());
    }

    if (bboxCache == NULL) return FALSE;

    const SbBox3f &bbox = bboxCache->getProjectedBox();

    const SbMatrix &cullMatrix =
	SoModelMatrixElement::getCombinedCullMatrix(state);

    return bbox.outside(cullMatrix, cullBits);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements ray picking.
//
// Use: extender

void
SoSeparator::rayPick(SoRayPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    int			numIndices;
    const int		*indices;

    // Bail out if there is nothing to traverse...
    if (action->getPathCode(numIndices, indices) == SoAction::OFF_PATH)
	return;
    
    // Note: even if we are only traversing some of our children we
    // can still use the bounding box cache  computed for all of our
    // children for culling, because the bounding box for all of our
    // children is guaranteed to be at least as big as the bounding
    // box for some of them.

    // The action may not have yet computed a world space ray. (This
    // will be true if the pick was specified as a window-space point
    // and no camera has yet been traversed. This could happen if
    // there is no camera or the camera appears under this separator.)
    // In this case, don't bother trying to intersect the ray with a
    // cached bounding box, sine there ain't no ray!
    if (action->isCullingEnabled() && pickCulling.getValue() != OFF &&
	action->hasWorldSpaceRay()) {

	// If we don't have a valid cache, try to build one by
	// applying an SoGetBoundingBoxAction to the current path to
	// the separator. (Testing if the cache is invalid uses the
	// state of the pick action; this assumes that any element
	// that is valid in a bounding box action will also be valid
	// in a pick action.)
	if (bboxCache == NULL || ! bboxCache->isValid(action->getState())) {
	    SoGetBoundingBoxAction	ba(action->getViewportRegion());
	    ba.apply((SoPath *) action->getCurPath());
	}

	// It's conceivable somehow that the cache was not built, so
	// check it again
	if (bboxCache != NULL) {

	    // Test the bounding box in the cache for intersection
	    // with the pick ray. If none, traverse no further.
	    // If there are no lines or points in the cache, we can
	    // use a faster intersection test: intersect the picking
	    // ray with the bounding box. Otherwise, we have to use
	    // the picking view volume to make sure we pick near lines
	    // and points
	    action->setObjectSpace();

#ifdef DEBUG
	    static int printCullInfo = -1;
	    if (printCullInfo == -1)
		printCullInfo =
		    SoDebug::GetEnv("IV_DEBUG_PICK_CULL") != NULL;
	    if (printCullInfo) {
		if (getName().getLength() != 0)
		    SoDebug::RTPrintf("Separator named %s",
				      getName().getString());
		else
		    SoDebug::RTPrintf("Separator 0x%x", this);
	    }
#endif
	    if (! action->intersect(bboxCache->getBox().project(),
				    bboxCache->hasLinesOrPoints())) {
#ifdef DEBUG
		if (printCullInfo)
		    SoDebug::RTPrintf("  pick culled\n");
#endif
		return;
	    }
#ifdef DEBUG
	    else if (printCullInfo) {
		SoDebug::RTPrintf("  pick cull test passed\n");
	    }
#endif
	}
    }

    // If we got here, we're supposed to traverse our children
    action->getState()->push();
    SoGroup::rayPick(action);
    action->getState()->pop();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements search action for separator nodes. This determines if
//    the separator should be searched. If so, this calls the search
//    method for SoGroup to do the work.
//
// Use: extender

void
SoSeparator::search(SoSearchAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool	doSearch = TRUE;

    // See if we're supposed to search only if the stuff under the
    // separator is relevant to the search path

    if (! action->isSearchingAll()) {
	int		numIndices;
	const int	*indices;

	// Search through this separator node only if not searching along
	// a path or this node is on the path
	if (action->getPathCode(numIndices, indices) == SoAction::OFF_PATH)
	    doSearch = FALSE;
    }

    if (doSearch) {
	action->getState()->push();
	SoGroup::search(action);
	action->getState()->pop();
    }
}
