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
 |   $Revision: 1.2 $
 |
 |   Classes:
 |	SoGLRenderAction
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <limits.h>
#include <GL/gl.h>
#include <Inventor/SoDB.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/elements/SoGLCacheContextElement.h>
#include <Inventor/elements/SoGLRenderPassElement.h>
#include <Inventor/elements/SoGLUpdateAreaElement.h>
#include <Inventor/elements/SoTextureImageElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/elements/SoShapeStyleElement.h>

SO_ACTION_SOURCE(SoGLRenderAction);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor. The first parameter defines the viewport region
//    into which rendering will take place. 
//
// Use: public

SoGLRenderAction::SoGLRenderAction(const SbViewportRegion &viewportRegion)
				
//
////////////////////////////////////////////////////////////////////////
{
    SO_ACTION_CONSTRUCTOR(SoGLRenderAction);

    vpRegion		= viewportRegion;
    updateOrigin.setValue(0.0, 0.0);
    updateSize.setValue(1.0, 1.0);
 
    abortCB		= NULL;

    transpType  	= SCREEN_DOOR;
    doSmooth		= FALSE;
    numPasses		= 1;
    passUpdate		= FALSE;
    passCB		= NULL;

    renderingTranspObjs	= FALSE;
    delayObjs		= FALSE;
    sortObjs		= FALSE;
    ba			= NULL;
    bboxes		= NULL;
    cacheContext	= 0;
    remoteRendering	= FALSE;

    renderingDelPaths	= FALSE;

    whatChanged		= ALL;

    // These three bits keep track of which view-volume planes we need
    // to test against; by default, all bits are 1.
    cullBits		= 7;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: public

SoGLRenderAction::~SoGLRenderAction()
//
////////////////////////////////////////////////////////////////////////
{
    if (ba != NULL)
	delete ba;

    if (bboxes != NULL)
	delete [] bboxes;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the viewport region.  This should be called when the window
//    changes size.
//
// Use: public
void
SoGLRenderAction::setViewportRegion(const SbViewportRegion &newRegion)
//
////////////////////////////////////////////////////////////////////////
{
    vpRegion = newRegion;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the rendering update area.
//
// Use: public
void
SoGLRenderAction::setUpdateArea(const SbVec2f &origin, const SbVec2f &size)
//
////////////////////////////////////////////////////////////////////////
{
    updateOrigin = origin;
    updateSize   = size;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the rendering update area.
//
// Use: public
void
SoGLRenderAction::getUpdateArea(SbVec2f &origin, SbVec2f &size) const
//
////////////////////////////////////////////////////////////////////////
{
    origin = updateOrigin;
    size   = updateSize;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets transparency quality level to use when rendering.
//
// Use: public

void
SoGLRenderAction::setTransparencyType(TransparencyType type)
//
////////////////////////////////////////////////////////////////////////
{
    if (transpType != type) {
	transpType = type;
	whatChanged |= TRANSPARENCY_TYPE;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets cache context to given value
//
// Use: public

void
SoGLRenderAction::setCacheContext(uint32_t context)
//
////////////////////////////////////////////////////////////////////////
{
    // If the cache context changes, we've changed OpenGL contexts,
    // and we should also invalidate the state:
    if (cacheContext != context) {
	invalidateState();
	cacheContext = context;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Gets cache context
//
// Use: public

uint32_t
SoGLRenderAction::getCacheContext() const
//
////////////////////////////////////////////////////////////////////////
{
    return cacheContext;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets whether rendering is remote or local.
//
// Use: public

void
SoGLRenderAction::setRenderingIsRemote(SbBool flag)
//
////////////////////////////////////////////////////////////////////////
{
    remoteRendering = flag;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Gets whether renering is remote or local.
//
// Use: public

SbBool
SoGLRenderAction::getRenderingIsRemote() const
//
////////////////////////////////////////////////////////////////////////
{
    return remoteRendering;
}

////////////////////////////////////////////////////////////////////////
//
// Invalidate the state so that it will be created again
// next time the action is applied.
//
// Use: public, virtual

void
SoGLRenderAction::invalidateState()
//
////////////////////////////////////////////////////////////////////////
{
    // Invalidate the state in the usual way
    SoAction::invalidateState();

    // Also invalidate what we think we know...
    whatChanged = ALL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Turns on/off smoothing (cheap anti-aliasing). This may change
//    the state of alpha blending to make the smoothing work.
//
// Use: public

void
SoGLRenderAction::setSmoothing(SbBool smooth)
//
////////////////////////////////////////////////////////////////////////
{
    if (doSmooth != smooth) {
	doSmooth = smooth;
	whatChanged |= SMOOTHING;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if render action handles processing of a
//    transparent object (if it is to be sorted and rendered later).
//    If this returns FALSE, the object should just go ahead and
//    render itself.
//    If the argument isTransparent is TRUE, the object is assumed to
//    be transparent, regardless of the state.
//
// Use: extender

SbBool
SoGLRenderAction::handleTransparency(SbBool isTransparent)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool	ret;

    // Nothing special to do for screen-door blending
    if (transpType == SCREEN_DOOR)
	return FALSE;

    // Determine if the object is likely to be transparent. This is
    // true if: there are several transparency values in the state or
    // the first one is non-zero; there is a texture map that affects
    // transparency; or the diffuse colors are specified as packed
    // values (which contain alpha).
    if (isTransparent ||
	(SoLazyElement::getInstance(getState())->isTransparent()) ||
	(SoTextureImageElement::containsTransparency(getState()))) {

	// If transparency is delayed, add a path to this object to
	// the list of transparent objects, and tell the shape not to
	// render
	if (delayObjs) {
	    const SoPath	*curPath = getCurPath();

	    // For some group nodes (such as Array and MultipleCopy),
	    // the children are traversed more than once. In this
	    // case, don't add the path if it is the same as any of
	    // the previous ones.
	    SbBool	isCopy = FALSE;
	    int		i;

	    for (i = 0; i < transpPaths.getLength(); i++) {
		if (*curPath == *transpPaths[i]) {
		    isCopy = TRUE;
		    break;
		}
	    }

	    // Add path if not already there
	    if (! isCopy)
		transpPaths.append(curPath->copy());	// Also refs the path

	    // We also need to make sure that any open caches are
	    // invalidated; if they aren't, they will skip this
	    // object and (since the cache replaces traversal),
	    // this object will not be rendered delayed at all.

	    if (getState()->isCacheOpen())
		SoCacheElement::invalidate(getState());

	    ret = TRUE;
	}	

	// If transparency is not delayed, enable blending
	else {
	    enableBlending(TRUE);
	    ret = FALSE;
	}
    }

    // Disable blending, otherwise
    else {
	enableBlending(FALSE);
	ret = FALSE;
    }

    return ret;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds to the list of paths to render after all other stuff
//    (including delayed/sorted transparent objects) have been
//    rendered. (Used for annotation nodes.)
//
// Use: internal

void
SoGLRenderAction::addDelayedPath(SoPath *path)
//
////////////////////////////////////////////////////////////////////////
{
    delayedPaths.append(path);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initiates action on a graph.
//
// Use: protected

void
SoGLRenderAction::beginTraversal(SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
    // This is called either from the main call to apply() that is
    // used to render a graph OR from the apply() call made while
    // rendering transparent objects or delayed objects. In the first
    // case, we want to render all passes. In the second and third
    // cases, we want to render only the current pass. We can tell
    // these cases apart by examining the flags.

    if (renderingTranspObjs || renderingDelPaths)
	traverse(node);

    else
	renderAllPasses(node);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does actual rendering of all passes starting at a node.
//
// Use: private

void
SoGLRenderAction::renderAllPasses(SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
    // If anything has changed since the last time this action was
    // applied, make sure it is set up correctly in GL.
    if (whatChanged != 0) {

	switch (transpType) {
	  case SCREEN_DOOR:
	    if (doSmooth) {
		// Blending has to be enabled for line smoothing to
		// work properly
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		enableBlending(TRUE);
	    }
	    else
		enableBlending(FALSE);
	    break;

	  case ADD:
	  case DELAYED_ADD:
	  case SORTED_OBJECT_ADD:
	    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	    break;

	  case BLEND:
	  case DELAYED_BLEND:
	  case SORTED_OBJECT_BLEND:
	    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	    break;
	}

	sortObjs = (transpType == SORTED_OBJECT_ADD ||
		    transpType == SORTED_OBJECT_BLEND);
	delayObjs = (sortObjs ||
		     transpType == DELAYED_ADD ||
		     transpType == DELAYED_BLEND);

	if (doSmooth) {
	    glEnable(GL_POINT_SMOOTH);
	    glEnable(GL_LINE_SMOOTH);
	}
	else {
	    glDisable(GL_POINT_SMOOTH);
	    glDisable(GL_LINE_SMOOTH);
	}

	// Reset flags to indicate that everything is up to date
	whatChanged = 0;
    }

    // Set the GL cache context:
    SoGLCacheContextElement::set(state, (int) cacheContext, delayObjs,
				 remoteRendering);
    
    // Set the transparency bit in the ShapeStyle element
    // and the lazy element.
    SoShapeStyleElement::setTransparencyType(state,transpType);
    SoLazyElement::setTransparencyType(state, transpType);
    
    // Simple case of one pass
    if (getNumPasses() == 1) {
	renderPass(node, 0);
	return;
    }

    int		pass;
    float	passFrac = 1.0 / (float) getNumPasses();

    for (pass = 0; pass < getNumPasses(); pass++) {

	// Stuff to do between passes:
	if (pass > 0) {
	    // Update the buffer after each pass if requested
	    if (passUpdate)
		glAccum(GL_RETURN, (float) getNumPasses() / (float) pass);

	    // If user-defined callback exists, call it. Otherwise,
	    // clear to current clear color and depth buffer clear value
	    if (passCB != NULL)
		(*passCB)(passData);
	    else
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	renderPass(node, pass);

	// Stop if rendering was aborted
	if (hasTerminated())
	    return;

	if (pass > 0)
	    glAccum(GL_ACCUM, passFrac);
	else
	    glAccum(GL_LOAD,  passFrac);
    }

    glAccum(GL_RETURN, 1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does one pass of rendering starting at a node.
//
// Use: private

void
SoGLRenderAction::renderPass(SoNode *node, int pass)
//
////////////////////////////////////////////////////////////////////////
{
    // Set the current pass in the instance and in the state
    curPass = pass;
    SoGLRenderPassElement::set(getState(), pass);

    // Set the viewport region 
    SoViewportRegionElement::set(getState(), vpRegion);
    SoGLUpdateAreaElement::set(getState(), updateOrigin, updateSize);

    // Do the actual rendering
    traverse(node);

    // For delayed (or sorted) transparency, see if any transparent
    // objects were added
    if (delayObjs && transpPaths.getLength() > 0 && ! hasTerminated()) {

	// Make sure blending is enabled if necessary
	if (transpType != SCREEN_DOOR)
	    enableBlending(TRUE);

	renderTransparentObjs();

	// Disable blending for next pass
	if (transpType != SCREEN_DOOR)
	    enableBlending(FALSE);
    }

    // Delayed paths
    if (delayedPaths.getLength() > 0 && ! hasTerminated()) {
	renderingDelPaths = TRUE;

	// Render paths to delayed objects. We know these paths obey
	// the rules for compact path lists, so let the action know
	apply(delayedPaths, TRUE);

	// Clear out the list
	delayedPaths.truncate(0);

	renderingDelPaths = FALSE;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Renders delayed objects that have been marked as transparent.
//    This sorts them if necessary. This should be called only if we
//    are delaying transparent objects and there is at least one
//    transparent object.
//
// Use: private

void
SoGLRenderAction::renderTransparentObjs()
//
////////////////////////////////////////////////////////////////////////
{
    int	i, numObjs = transpPaths.getLength(), numToDo;

    // Indicate that we are doing transparent objects so we know not
    // to render all passes
    renderingTranspObjs = TRUE;

    // Indicate that objects are not to be delayed. (So they will render.)
    delayObjs = FALSE;

    // Don't write into z buffer so that ALL transparent objects will
    // be drawn. This makes things look better, even if sorting is not
    // on or if sorting gives the incorrect order.
    glDepthMask(FALSE);

    // If not sorting, just render them in order
    if (! sortObjs)
	// Render paths to transparent objects. We know these paths
	// obey the rules for compact path lists, so let the action know.
	apply(transpPaths, TRUE);

    // Otherwise, compute bounding boxes, render objs back to front
    else {
	if (ba == NULL) {
	    ba = new SoGetBoundingBoxAction(vpRegion);

	    // Make sure bounding boxes are in camera space. This
	    // means the z coordinates of the bounding boxes indicate
	    // distance from the camera.
	    ba->setInCameraSpace(TRUE);
	}

	// Make sure there is room for the bounding boxes
	if (bboxes == NULL) {
	    bboxes = new SbBox3f[numObjs];
	    numBBoxes = numObjs;
	}
	else if (numBBoxes < numObjs) {
	    delete [] bboxes;
	    bboxes = new SbBox3f[numObjs];
	    numBBoxes = numObjs;
	}

	for (i = 0; i < numObjs; i++) {
	    ba->apply(transpPaths[i]);
	    bboxes[i] = ba->getBoundingBox();
	}

	// Render them in sorted order
	for (numToDo = numObjs; numToDo > 0; --numToDo) {
	    int		farthest;
	    float	zFar;

	    // Use selection sort, since number of objects is usually small

	    // Look for bbox with smallest zmax (farthest from camera!)
	    zFar = FLT_MAX;
	    for (i = 0; i < numObjs; i++) {
		if (bboxes[i].getMax()[2] < zFar) {
		    zFar = bboxes[i].getMax()[2];
		    farthest = i;
		}
	    }

	    // Render farthest one
	    apply(transpPaths[farthest]);

	    // Mark it as being far
	    bboxes[farthest].getMax()[2] = FLT_MAX;
	}
    }

    // Restore zwritemask to what we assume it was before...
    glDepthMask(TRUE);

    // Get ready for next time
    delayObjs = TRUE;
    transpPaths.truncate(0);
    renderingTranspObjs = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Enables or disables GL blending. Remembers previous setting to
//    avoid sending commands unnecessarily.
//
// Use: private

void
SoGLRenderAction::enableBlending(SbBool enable)
//
////////////////////////////////////////////////////////////////////////
{
    SoLazyElement::setBlending(state, enable);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if render action should abort based on calling
//    callback. This assumes the callback is not NULL.
//
// Use: private

SbBool
SoGLRenderAction::checkAbort()
//
////////////////////////////////////////////////////////////////////////
{
    SbBool doAbort;

    switch ((*abortCB)(abortData)) {

      case CONTINUE:
	doAbort = FALSE;
	break;

      case ABORT:
	// Mark the action has having terminated
	setTerminated(TRUE);
	doAbort = TRUE;
	break;

      case PRUNE:
	// Don't mark anything, but return TRUE. This will tell the
	// node not to render itself.
	doAbort = TRUE;
	break;

      case DELAY:
	// Add the current path to the list of delayed paths
	delayedPaths.append(getCurPath()->copy());	// Also refs the path
	doAbort = TRUE;
	break;
    }

    return doAbort;
}
