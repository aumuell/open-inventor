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
 * Copyright (C) 1995-96   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Classes:
 |	SoLocateHighlight
 |
 |   Author(s): Alain Dumesny, Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <GL/gl.h>

#include <Inventor/SoPath.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/events/SoEvent.h>
#include <Inventor/events/SoLocation2Event.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/elements/SoLazyElement.h>
#include <Inventor/elements/SoOverrideElement.h>
#include <Inventor/elements/SoWindowElement.h>

#include <Inventor/nodes/SoLocateHighlight.h>

#ifdef DEBUG
#include <SoDebug.h>
#endif


SO_NODE_SOURCE(SoLocateHighlight);

// Hold a pointer to the current locate highlight.
// This way we can "turn off" the old highlight before
// we "turn on" the new one.
SoFullPath* SoLocateHighlight::currentHighlightPath = NULL;


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initialize the node
//
// Use: public, internal
//

void
SoLocateHighlight::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoLocateHighlight, "LocateHighlight", SoSeparator);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public
//
SoLocateHighlight::SoLocateHighlight()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoLocateHighlight);

    SO_NODE_ADD_FIELD(mode, (AUTO));
    SO_NODE_ADD_FIELD(style, (EMISSIVE));
    SO_NODE_ADD_FIELD(color, (.3,.3,.3));
    
    // Set up static info for enum fields
    SO_NODE_DEFINE_ENUM_VALUE(Styles,	EMISSIVE);
    SO_NODE_DEFINE_ENUM_VALUE(Styles,	EMISSIVE_DIFFUSE);
    SO_NODE_DEFINE_ENUM_VALUE(Modes,	AUTO);
    SO_NODE_DEFINE_ENUM_VALUE(Modes,	ON);
    SO_NODE_DEFINE_ENUM_VALUE(Modes,	OFF);
    
    // Set up info in enumerated type fields
    SO_NODE_SET_SF_ENUM_TYPE(style,	Styles);
    SO_NODE_SET_SF_ENUM_TYPE(mode,	Modes);
    
    isBuiltIn = TRUE;

    // Locate highlighting vars
    highlightingPass = FALSE;
    // make a colorPacker
    colorPacker = new SoColorPacker;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
SoLocateHighlight::~SoLocateHighlight()
//
////////////////////////////////////////////////////////////////////////
{
    // If we're being deleted and we're the current highlight,
    // NULL out that variable
    if (currentHighlightPath != NULL &&
	(!currentHighlightPath->getTail()->isOfType(SoLocateHighlight::getClassTypeId()))) {
#ifdef DEBUG
//???    fprintf(stderr,"Removing current highlight because node was deleted\n");
#endif
	currentHighlightPath->unref();
	currentHighlightPath = NULL;
    }
    delete colorPacker;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    redefine this from SoSeparator to also highlight.
//
// Use: extender

void
SoLocateHighlight::GLRenderBelowPath(SoGLRenderAction *action)

////////////////////////////////////////////////////////////////////////
{
    // Set up state for locate highlighting (if necessary)
    GLint oldDepthFunc;
    SbBool drawHighlighted = preRender(action, oldDepthFunc);
    
    // now invoke the parent method
    SoSeparator::GLRenderBelowPath(action);
    
    // Restore old depth buffer model if needed
    if (drawHighlighted || highlightingPass)
	glDepthFunc((GLenum)oldDepthFunc);
    
    // Clean up state if needed
    if (drawHighlighted)
	action->getState()->pop();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    redefine this from SoSeparator to also highlight.
//
// Use: extender

void
SoLocateHighlight::GLRenderInPath(SoGLRenderAction *action)

////////////////////////////////////////////////////////////////////////
{
    // Set up state for locate highlighting (if necessary)
    GLint oldDepthFunc;
    SbBool drawHighlighted = preRender(action, oldDepthFunc);
    
    // now invoke the parent method
    SoSeparator::GLRenderInPath(action);
    
    // Restore old depth buffer model if needed
    if (drawHighlighted || highlightingPass)
	glDepthFunc((GLenum)oldDepthFunc);
    
    // Clean up state if needed
    if (drawHighlighted)
	action->getState()->pop();
}

///////////////////////////////////////////////////////////////////
// Description:
//    called just before rendering - this will setup highlighting 
//  stuff if needed.
//
// Use: private

SbBool
SoLocateHighlight::preRender(SoGLRenderAction *action, GLint &oldDepthFunc)
//
////////////////////////////////////////////////////////////////////////
{
    // If not performing locate highlighting, just return.
    if (mode.getValue() == OFF)
	return FALSE;
    
    SoState *state = action->getState();
    
    // ??? prevent caching at this level - for some reason the 
    // ??? SoWindowElement::copyMatchInfo() method get called, which should
    // ??? never be called. We are not caching this node correctly yet....
    SoCacheElement::invalidate(state);
    
    SbBool drawHighlighted = (mode.getValue() == ON || isHighlighted(action));
    
    if (drawHighlighted) {
	
	// prevent diffuse & emissive color from leaking out...
	state->push(); 
	
	SbColor col = color.getValue();
	
	// Emissive Color
	SoOverrideElement::setEmissiveColorOverride(state, this, TRUE);
	SoLazyElement::setEmissive(state, &col);
	
	// Diffuse Color
	if (style.getValue() == EMISSIVE_DIFFUSE) {
	    SoOverrideElement::setDiffuseColorOverride(state, this, TRUE);
	    SoLazyElement::setDiffuse(state, this, 1, &col, colorPacker);
	}
    }

    // Draw on top of other things at same z-buffer depth if:
    // [a] we're highlighted
    // [b] this is the highlighting pass. This occurs when changing from
    //     non-hilit to lit OR VICE VERSA.
    // Otherwise, leave it alone...
    if (drawHighlighted || highlightingPass) {
	glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFunc);
	if (oldDepthFunc != GL_LEQUAL)
	    glDepthFunc(GL_LEQUAL);
    }
    
    return drawHighlighted;
}

///////////////////////////////////////////////////////////////////
// handleEvent
//
//   Overloaded from SoSeparator to handle locate highlighting.
//

void
SoLocateHighlight::handleEvent(SoHandleEventAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // If we don't need to pick for locate highlighting, 
    // then just behave as separator and return.
    // NOTE: we still have to pick for ON even though we don't have
    // to re-render, because the app needs to be notified as the mouse
    // goes over locate highlight nodes.
    if ( mode.getValue() == OFF ) {
	SoSeparator::handleEvent( action );
	return;
    }

    // get event from the action
    const SoEvent *event = action->getEvent();

    //
    // If this is a mouseMotion event, then check for locate highlighting
    //
    if (event->isOfType(SoLocation2Event::getClassTypeId())) {

	// check to see if the mouse is over our geometry...
	SbBool underTheMouse = FALSE;
	const SoPickedPoint *pp = action->getPickedPoint();
	SoFullPath *pPath = (pp != NULL) ? (SoFullPath *) pp->getPath() : NULL;
	if (pPath && pPath->containsPath(action->getCurPath())) {
	    // Make sure I'm the lowest LocHL in the pick path!
	    underTheMouse = TRUE;
	    for (int i = 0; i < pPath->getLength(); i++) {
		SoNode *node = pPath->getNodeFromTail(i);
		if (node->isOfType(SoLocateHighlight::getClassTypeId())) {
		    if (node != this)
			underTheMouse = FALSE;
		    break; // found the lowest LocHL - look no further
		}
	    }
	}
	
	// Am I currently highlighted?
	if (isHighlighted(action)) {
	    if ( ! underTheMouse)
		// re-draw the object with it's normal color
		redrawHighlighted(action, FALSE);
	    else
		action->setHandled();
	}
	// Else I am not currently highlighted
	else {
	    // If under the mouse, then highlight!
	    if (underTheMouse)
		// draw this object highlighted
		redrawHighlighted(action, TRUE);
	}
    }
	
    // Let the base class traverse the children.
    if ( action->getGrabber() != this )
	SoSeparator::handleEvent(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//     This is called by the handleEvent routine to hightlight/de-highlight
// ourself in the current window the mouse is over.
//
// Usage: private
void
SoLocateHighlight::redrawHighlighted(
    SoAction *action, SbBool doHighlight)
//
////////////////////////////////////////////////////////////////////////
{
    // If we are about to highlight, and there is something else highlighted,
    // that something else needs to unhighlight.
    if (doHighlight && currentHighlightPath != NULL && 
        !(*((SoFullPath *)action->getCurPath()) == *currentHighlightPath)) {
	
	SoNode *tail = currentHighlightPath->getTail();
	if (tail->isOfType( SoLocateHighlight::getClassTypeId()))
	    ((SoLocateHighlight *)tail)->redrawHighlighted(action, FALSE);
	else {
	    // Just get rid of the path. It's no longer valid for redraw.
	    currentHighlightPath->unref();
	    currentHighlightPath = NULL;
	}
    }
    
    SoPath *pathToRender;	
    // save the path to ourself for later de-highlight
    if (doHighlight) {

	if (currentHighlightPath != NULL)
	    currentHighlightPath->unref();
	currentHighlightPath = (SoFullPath *) action->getCurPath()->copy();
	currentHighlightPath->ref();
	
	// We will be rendering this new path to highlight it
	pathToRender = currentHighlightPath;
	pathToRender->ref();
    }
    // delete our path if we are no longer highlighted
    else {

	// We will be rendering this old path to unhighlight it
	pathToRender = currentHighlightPath;
	pathToRender->ref();
	
	currentHighlightPath->unref();
	currentHighlightPath = NULL;
    }

    // If highlighting is forced on for this node, we don't need this special render.
    if (mode.getValue() != AUTO) {
	pathToRender->unref();
	return;
    }
    
    SoState *state = action->getState();
    
    Window window;
    GLXContext context;
    Display *display;
    SoGLRenderAction *glAction;
    SoWindowElement::get(state, window, context, display, glAction);
    
    // If we don't have a current window, then simply return...
    if (window == 0 || context == NULL || display == NULL || glAction == NULL)
	return;
    
    // set the current window
    glXMakeCurrent(display, window, context);
    
    // render into the front buffer (save the current buffering type)
    GLint whichBuffer;
    glGetIntegerv(GL_DRAW_BUFFER, &whichBuffer);
    if (whichBuffer != GL_FRONT)
	glDrawBuffer(GL_FRONT);

    highlightingPass = TRUE;
    glAction->apply(pathToRender);
    highlightingPass = FALSE;

    // restore the buffering type
    if (whichBuffer != GL_FRONT)
	glDrawBuffer((GLenum)whichBuffer);
    glFlush();
    
    pathToRender->unref();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//     This will de-highlight the currently highlighted node if any.
//
// Usage: static public
void
SoLocateHighlight::turnOffCurrentHighlight(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    if (currentHighlightPath == NULL)
	return;
    
    SoNode *tail = currentHighlightPath->getTail();
    if (tail->isOfType(SoLocateHighlight::getClassTypeId())) {
	
	// don't redraw if we already are in the middle of rendering
	// (processing events during render abort might cause this)
	SoState *state = action->getState();
	if (state && state->getDepth() == 1)
	    ((SoLocateHighlight *)tail)->redrawHighlighted(action, FALSE);
    }
    else {
	// Just get rid of the path. It's no longer valid for redraw.
	currentHighlightPath->unref();
	currentHighlightPath = NULL;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//     Returns TRUE if this node should be highlighted.
//
// Usage: private
SbBool
SoLocateHighlight::isHighlighted(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoFullPath *actionPath = (SoFullPath *) action->getCurPath();
    return (currentHighlightPath != NULL &&
	    currentHighlightPath->getTail() == actionPath->getTail() && // nested SoHL!
	    *currentHighlightPath == *actionPath);
}
