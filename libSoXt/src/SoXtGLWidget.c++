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
 * Copyright (C) 1990-93   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.5 $
 |
 |   Classes:
 |	SoXtGLWidget
 |
 |   Author(s): Alain Dumesny, David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Inventor/errors/SoDebugError.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/SoXtGLWidget.h>
#include "SoGLwMDrawA.h"

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Form.h>
#include <GL/gl.h>


/*
 * macros
 */

#define SET_BIT(mode, mask, flag) \
    if (flag) mode |= mask; \
    else mode &= ~mask;

#define SCREEN(w) XScreenNumberOfScreen( XtScreen(w) )


////////////////////////////////////////////////////////////////////////
//
// Each display has a list of GLXContext to be shared across all glx windows
typedef struct _SoContextListDisplayPair {
    Display *display;
    int     screen;
    SbPList *contextList;
} SoContextListDisplayPair;

// This class finds a context list for the passed display.
// Contexts can then be searched for, added, and removed from that list.
class SoContextListKeeper : private SbPList {
  public:
    SoContextListKeeper() {}
    SbPList *find(Display *d, int screen);
    int	whichDisplay(GLXContext ctx);
};

SbPList *
SoContextListKeeper::find(Display *d, int screen)
{
    SoContextListDisplayPair *p = NULL;
    
    // Search for a context list associated with Display
    for (int i = 0; i < getLength(); i++) {
	SoContextListDisplayPair *p =
	    (SoContextListDisplayPair *)this->get(i);
	if (p->display == d && p->screen == screen ) {
	    return p->contextList;
	}
    }
    
    // If we got to here, we didn't find it. Create one.
    p = new SoContextListDisplayPair;
    p->display = d;
    p->screen = screen;
    p->contextList = new SbPList;
    append((void *) p);
    
    return p->contextList;
}

// Returns unique integers for contexts on different displays.  Used
// to determine when display lists can/can't be shared (display lists
// are shared between contexts on the same display).
int
SoContextListKeeper::whichDisplay(GLXContext ctx)
{
    for (int i = 0; i < getLength(); i++) {
	SoContextListDisplayPair *p = (SoContextListDisplayPair *)(*this)[i];
	for (int j = 0; j < p->contextList->getLength(); j++) {
	    if (p->contextList->get(j) == (const void *)ctx)
		return i;
	}
    }
#ifdef DEBUG
    SoDebugError::post("SoContextListKeeper::whichList",
		       "Context not found!");
#endif
    return -1;
}

// When you need to find a context list, do this:
//    SbPList *contextList = contextListKeeper.find(XtDisplay(myWidget), SCREEN(myWidget));
static SoContextListKeeper contextListKeeper;

//
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//
// Constructor
//
SoXtGLWidget::SoXtGLWidget(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent, 
    int modes, 
    SbBool buildNow) : SoXtComponent(parent, name, buildInsideParent)
//
////////////////////////////////////////////////////////////////////////
{
    // window related variables
    mgrWidget = NULL;
    singleBufferWidget = doubleBufferWidget = overlayWidget = NULL;
    ctxNormal = ctxOverlay = ctxSingle = ctxDouble = 0;
    colorMap = overlayColorMap = 0;
    glModes = modes;
    windowResized = FALSE;
    glxSize.setValue(0, 0);
    borderSize = 0;
    waitForExpose = TRUE;
    drawToFrontBuffer = TRUE; // used by RA to draw new scenes to front buffer
    enableDrawToFrontBuffer = TRUE;
    
    //
    // window attribute list
    //
    int n = 0;
    attribList = new int[14];
    // insert no-op GLX_USE_GL for double/stereo if they are off since those
    // change dynamically and it is easier to set them again than it is
    // to insert them.
    attribList[n++] = (glModes & SO_GLX_DOUBLE) ? GLX_DOUBLEBUFFER : GLX_USE_GL;
    attribList[n++] = (glModes & SO_GLX_STEREO) ? GLX_STEREO : GLX_USE_GL;
    if (glModes & SO_GLX_RGB) {
	// This makes sure we get the maximum buffer configuration 
	// by showing interest the number of bits
	attribList[n++] = GLX_RGBA;
	attribList[n++] = GLX_RED_SIZE;
	attribList[n++] = 1;
	attribList[n++] = GLX_GREEN_SIZE;
	attribList[n++] = 1;
	attribList[n++] = GLX_BLUE_SIZE;
	attribList[n++] = 1;
    }
    if (glModes & SO_GLX_ZBUFFER) {
	attribList[n++] = GLX_DEPTH_SIZE;
	attribList[n++] = 1;
    }
    attribList[n++] = None;
    
    // Build the widget tree, and let SoXtComponent know about our base widget.
    if (buildNow)
	setBaseWidget(buildWidget(getParentWidget()));
}

////////////////////////////////////////////////////////////////////////
//
// virtual destructor
//
SoXtGLWidget::~SoXtGLWidget()
//
////////////////////////////////////////////////////////////////////////
{
    delete [] attribList;
    
    // delete the GLX widgets (free the context and the visual too)
    
    if (doubleBufferWidget)
	destroyGLXWidget(doubleBufferWidget, ctxDouble, TRUE);
    
    if (singleBufferWidget)
	destroyGLXWidget(singleBufferWidget, ctxSingle, TRUE);
    
    if (overlayWidget)
	destroyGLXWidget(overlayWidget, ctxOverlay, FALSE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  	sets the normal window visual
//
// Use: public
void
SoXtGLWidget::setNormalVisual(XVisualInfo *newvis)
//
////////////////////////////////////////////////////////////////////////
{
    if (! newvis)
	return;
    
    Display *dpy = XtDisplay(mgrWidget);
    int val;
    
    // make sure visual supports OpenGL
    glXGetConfig(dpy, newvis, GLX_USE_GL, &val);
    if (! val) {
#ifdef DEBUG
	SoDebugError::post("SoXtGLWidget::setNormalVisual",
	    "visual does not support OpenGL.");
#endif
	return;
    }
    
    // make sure this visual is for normal frame buffer
    glXGetConfig(dpy, newvis, GLX_LEVEL, &val);
    if (val != 0) {
#ifdef DEBUG
	SoDebugError::post("SoXtGLWidget::setNormalVisual",
	    "visual is not for the normal frame buffer.");
#endif
	return;
    }
    
    // Make a copy of the XVisualInfo structure - we don't want to rely
    // on a pointer which the programmer can delete at any time.
    XVisualInfo *vis = (XVisualInfo *) XtMalloc(sizeof(XVisualInfo));
    memcpy(vis, newvis, sizeof(XVisualInfo));
    
    //
    // build a new attribute list from the given visual
    // to be used later when we toggle single/double and want to
    // keep the other settings the same.
    //
    int newAttribList[30], num=0;
    
    glXGetConfig(dpy, vis, GLX_DOUBLEBUFFER, &val);
    newAttribList[num++] = val ? GLX_DOUBLEBUFFER : GLX_USE_GL;
    SET_BIT(glModes, SO_GLX_DOUBLE, val);
    
    glXGetConfig(dpy, vis, GLX_STEREO, &val);
    newAttribList[num++] = val ? GLX_STEREO : GLX_USE_GL;
    SET_BIT(glModes, SO_GLX_STEREO, val);
    
    glXGetConfig(dpy, vis, GLX_RGBA, &val);
    SET_BIT(glModes, SO_GLX_RGB, val);
    if (val) {
	// ??? should we check what the current RGBA sizes are ?
	newAttribList[num++] = GLX_RGBA;
	newAttribList[num++] = GLX_RED_SIZE;
	newAttribList[num++] = 1;
	newAttribList[num++] = GLX_GREEN_SIZE;
	newAttribList[num++] = 1;
	newAttribList[num++] = GLX_BLUE_SIZE;
	newAttribList[num++] = 1;
    }
    
    glXGetConfig(dpy, vis, GLX_AUX_BUFFERS, &val);
    if (val) {
	newAttribList[num++] = GLX_AUX_BUFFERS;
	newAttribList[num++] = val;
    }
    
    glXGetConfig(dpy, vis, GLX_DEPTH_SIZE, &val);
    if (val) {
	newAttribList[num++] = GLX_DEPTH_SIZE;
	newAttribList[num++] = 1; // ??? should we use the size ?
    }
    
    glXGetConfig(dpy, vis, GLX_STENCIL_SIZE, &val);
    if (val) {
	newAttribList[num++] = GLX_STENCIL_SIZE;
	newAttribList[num++] = 1; // ???  should we use the size ?
    }
    
    // ??? only check the RED accumulation ?
    glXGetConfig(dpy, vis, GLX_ACCUM_RED_SIZE, &val);
    if (val) {
	newAttribList[num++] = GLX_ACCUM_RED_SIZE;
	newAttribList[num++] = 1; // ???  should we use the size ?
	newAttribList[num++] = GLX_ACCUM_GREEN_SIZE;
	newAttribList[num++] = 1;
	newAttribList[num++] = GLX_ACCUM_BLUE_SIZE;
	newAttribList[num++] = 1;
    }
    
    // copy the new attributes over
    delete [] attribList;
    attribList = new int[num+1];
    for (int i=0; i<num; i++)
	attribList[i] = newAttribList[i];
    attribList[num] = None;
    
    //
    // delete the current set of windows and create a new widget
    // with the given visual.
    //
    destroyNormalWindows();
    buildNormalGLXWidget(vis); // this MUST be called after we destroy the old contexes
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  	sets the overlay window visual
//
// Use: public
void
SoXtGLWidget::setOverlayVisual(XVisualInfo *newvis)
//
////////////////////////////////////////////////////////////////////////
{
    if (! newvis)
	return;
    
    Display *dpy = XtDisplay(mgrWidget);
    int val;
    
    // make sure visual supports OpenGL
    glXGetConfig(dpy, newvis, GLX_USE_GL, &val);
    if (! val) {
#ifdef DEBUG
	SoDebugError::post("SoXtGLWidget::setOverlayVisual",
	    "visual does not support OpenGL.");
#endif
	return;
    }
    
    // make sure this visual if for overlay frame buffer
    glXGetConfig(dpy, newvis, GLX_LEVEL, &val);
    if (val != 1) {
#ifdef DEBUG
	SoDebugError::post("SoXtGLWidget::setOverlayVisual",
	    "visual is not for the overlay frame buffer.");
#endif
	return;
    }
    
    // Make a copy of the XVisualInfo structure - we don't want to rely
    // on a pointer which the programmer can delete at any time.
    XVisualInfo *vis = (XVisualInfo *) XtMalloc(sizeof(XVisualInfo));
    memcpy(vis, newvis, sizeof(XVisualInfo));

    // delete the current overlay widget....
    if (overlayWidget)
	destroyGLXWidget(overlayWidget, ctxOverlay, FALSE);
    overlayColorMap = 0;
    
    // ...create new widget based on the given visual
    buildOverlayGLXWidget(vis);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  	gets the normal window visual
//
// Use: public
XVisualInfo *
SoXtGLWidget::getNormalVisual()
//
////////////////////////////////////////////////////////////////////////
{
    if (! getNormalWidget())
	return NULL;
    
    XVisualInfo *vis;
    XtVaGetValues(getNormalWidget(), SoGLwNvisualInfo, &vis, NULL);
    return vis;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  	gets the overlay window visual
//
// Use: public
XVisualInfo *
SoXtGLWidget::getOverlayVisual()
//
////////////////////////////////////////////////////////////////////////
{
    if (! getOverlayWidget())
	return NULL;
    
    XVisualInfo *vis;
    XtVaGetValues(getOverlayWidget(), SoGLwNvisualInfo, &vis, NULL);
    return vis;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  	Dynamically changes between single and double buffering.
//
// Use: virtual public
void
SoXtGLWidget::setDoubleBuffer(SbBool flag)
//
////////////////////////////////////////////////////////////////////////
{
    if (flag == isDoubleBuffer())
	return;
    
    // special case - if the user calls setDoubleBuffer(FALSE) BEFORE the
    // this component has been realized, then we need to prevent the
    // ginitCB routine for the old buffer to be called - so simply delete
    // the old widget alltogether - see bug 180978
    if (! getNormalContext())
	destroyNormalWindows();
    
    // set the gl mode and update the attribute list
    SET_BIT(glModes, SO_GLX_DOUBLE, flag);
    for (int i=0; attribList[i] != None; i++) {
	if (flag && attribList[i] == GLX_USE_GL) { // search for first no-op
	    attribList[i] = GLX_DOUBLEBUFFER;
	    break;
	}
	else if (! flag && attribList[i] == GLX_DOUBLEBUFFER) {
	    attribList[i] = GLX_USE_GL; // clear with a no-op
	    break;
	}
    }
    
    Widget *newWidget = (flag) ? &doubleBufferWidget : &singleBufferWidget;
    Widget *oldWidget = (flag) ? &singleBufferWidget : &doubleBufferWidget;
    
    // build and map the new window
    if (*newWidget == NULL)
    	buildNormalGLXWidget();
    else {
	XtManageChild(*newWidget);
	
	// make sure overlay window is on top
	if (overlayWidget && XtWindow(overlayWidget))
	    XRaiseWindow(XtDisplay(overlayWidget), XtWindow(overlayWidget));
	
	ctxNormal = flag ? ctxDouble : ctxSingle;
	
	// let subclasses know we changed widget
	widgetChanged(*newWidget);
    }
    
    // unmaps the old window now
    if (*oldWidget != NULL)
    	XtUnmanageChild(*oldWidget);
    
    // prevent a redraw a redraw from happening until we receive an
    // expose event - this will prevent too many redraws from happening
    // if the scene graph also changes at the same time.
    waitForExpose = TRUE;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//  	Dynamically changes stereo/mono buffering type.
//
// Use: protected
void
SoXtGLWidget::setStereoBuffer(SbBool flag)
//
////////////////////////////////////////////////////////////////////////
{
    if (flag == isStereoBuffer())
	return;
    
    // update the attribute list
    int index;
    for (index=0; attribList[index] != None; index++) {
	if (flag && attribList[index] == GLX_USE_GL) { // search for first no-op
	    attribList[index] = GLX_STEREO;
	    break;
	}
	else if (! flag && attribList[index] == GLX_STEREO) {
	    attribList[index] = GLX_USE_GL; // clear with a no-op
	    break;
	}
    }
    
    //
    // if we are turning stereo buffer on, make sure this machine
    // will support it, otherwise undo the changes.
    //
    XVisualInfo *vis = NULL;
    if (flag) {
	vis = glXChooseVisual(XtDisplay(mgrWidget), 
	    XScreenNumberOfScreen(XtScreen(mgrWidget)), attribList);
	if (! vis) {
#ifdef DEBUG
	    SoDebugError::post("SoXtGLWidget::setStereoBuffer",
			     "could not create visual");
#endif
	    // clear the stereo setting and return
	    attribList[index] = GLX_USE_GL; // clear with a no-op
	    return;
	}
    }
    
    // now set the flag and create the new window with given visual
    SET_BIT(glModes, SO_GLX_STEREO, flag);
    destroyNormalWindows();
    buildNormalGLXWidget(vis); // this MUST be called after we destroy the old contexes
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//  	Returns different integers for contexts that cannot share GL
//  display lists.
//
// Use: protected
int
SoXtGLWidget::getDisplayListShareGroup(GLXContext ctx)
//
////////////////////////////////////////////////////////////////////////
{
    return contextListKeeper.whichDisplay(ctx);
}

static void 
glxWidgetDestroyCB(Widget w, void *, void *)
{
    // delete the visual
    XVisualInfo *vis;
    XtVaGetValues(w, SoGLwNvisualInfo, &vis, NULL);
    if (vis != NULL)
	XFree(vis);
    XtVaSetValues(w, SoGLwNvisualInfo, NULL, NULL);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  	destroys the given widget (free the context, the visual and resets
//  those pointers).
//
// Use: private
void
SoXtGLWidget::destroyGLXWidget(Widget &w, GLXContext &ctx, SbBool normalWindow)
//
////////////////////////////////////////////////////////////////////////
{
    if (ctx) {
	
	// delete the context
	SbPList *contextList = contextListKeeper.find(XtDisplay(w), SCREEN(w));
	glXDestroyContext(XtDisplay(w), ctx);
	int index = contextList->find(ctx);
	if (index != -1) contextList->remove(index);
	
	// also delete the visual
	XVisualInfo *vis;
	XtVaGetValues(w, SoGLwNvisualInfo, &vis, NULL);
	if (vis != NULL)
	    XFree(vis);
	XtVaSetValues(w, SoGLwNvisualInfo, NULL, NULL);
    }
    else
	// else add a destroy callback to delete the visual later
	// because the widget will be realized BEFORE the widget
	// gets to actually be destroyed (calling XtDestroyWidget()
	// only marks the widget to later be deleted - see bug 238440)
	XtAddCallback (w, XmNdestroyCallback, glxWidgetDestroyCB, (XtPointer) NULL);
    
    // remove any callbacks we added to this widget
    // (we need to do this to prevent the ginitCB to be called if we
    // delete this widget before the component was realized - see bug 238440)
    if (normalWindow) {
	XtRemoveCallback(w, SoGLwNginitCallback, 
	    (XtCallbackProc) SoXtGLWidget::ginitCB, (XtPointer) this);
	XtRemoveCallback(w, SoGLwNresizeCallback, 
	    (XtCallbackProc) SoXtGLWidget::resizeCB, (XtPointer) this);
	XtRemoveCallback(w, SoGLwNexposeCallback, 
	    (XtCallbackProc) SoXtGLWidget::exposeCB, (XtPointer) this);
    }
    else {
	XtRemoveCallback(w, SoGLwNginitCallback, 
	    (XtCallbackProc) SoXtGLWidget::overlayGinitCB, (XtPointer) this);
	XtRemoveCallback(w, SoGLwNexposeCallback, 
	    (XtCallbackProc) SoXtGLWidget::overlayExposeCB, (XtPointer) this);
    }
    
    // delete the widget and reset the passed pointers
    XtDestroyWidget(w);
    w = NULL;
    ctx = 0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  	destroys the current set of windows (called when visual changes)
//  This doesn't affect the overly window thought.
//
// Use: private
void
SoXtGLWidget::destroyNormalWindows()
//
////////////////////////////////////////////////////////////////////////
{
    // let subclasses remove stuff on the old widget
    widgetChanged(NULL);
    
    if (doubleBufferWidget)
	destroyGLXWidget(doubleBufferWidget, ctxDouble, TRUE);
    
    if (singleBufferWidget)
	destroyGLXWidget(singleBufferWidget, ctxSingle, TRUE);
    
    // reset the other local normal window vars (don't change the overlay)
    ctxNormal = 0;
    colorMap = 0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  	This builds a form around the GLX widget. Subclasses should
// register their own event interest.
//
// Use: protected
//
Widget
SoXtGLWidget::buildWidget(Widget parent)
//
////////////////////////////////////////////////////////////////////////
{
    //
    // build a top level form to hold the GLX widgets (single/double/overlay)
    //
    
    Arg args[8];
    int n = 0;
    XtSetArg(args[n], XmNshadowType, XmSHADOW_IN); n++;
    XtSetArg(args[n], XmNshadowThickness, borderSize); n++;
    
    // pick an initial size if set
    SbVec2s size = (glxSize[0] && glxSize[1]) ? glxSize : getSize();
    if (size[0] && size[1]) {
	XtSetArg(args[n], XtNwidth, size[0] + 2*borderSize); n++;
	XtSetArg(args[n], XtNheight, size[1] + 2*borderSize); n++;
    }
    
    // ??? don't listen to resize request by children (the parent will
    // ??? force their size). This will prevent the RenderArea
    // ??? to pop to 400x400 size (default) after the user set an explicit
    // ??? smaller size.
    XtSetArg(args[n], XmNresizePolicy, XmRESIZE_NONE); n++;
    mgrWidget = XtCreateWidget(getWidgetName(), xmFormWidgetClass,
	parent, args, n);
    
    buildNormalGLXWidget();
    if (glModes & SO_GLX_OVERLAY) // make this the top window
	buildOverlayGLXWidget();
    
    return mgrWidget;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  	This builds a GLX widget of the correct current type.
//
// Use: protected
void
SoXtGLWidget::buildNormalGLXWidget(XVisualInfo *vis)
//
////////////////////////////////////////////////////////////////////////
{
    // build the visual from the attribute list if necessary
    if (! vis) {
	vis = glXChooseVisual(XtDisplay(mgrWidget), 
	    XScreenNumberOfScreen(XtScreen(mgrWidget)), attribList);
	if (! vis) {
#ifdef DEBUG
	    SoDebugError::post("SoXtGLWidget::buildNormalGLXWidget",
			     "could not create visual");
#endif
	    return;
	}
    }

    int val;
    glXGetConfig(XtDisplay(mgrWidget), vis, GLX_DOUBLEBUFFER, &val);
    SET_BIT(glModes, SO_GLX_DOUBLE, val);
    
    //
    // Build the GLX widget
    //
    Arg args[12];
    int n = 0;
    XtSetArg(args[n], SoGLwNvisualInfo, vis); n++;
    // enables XmProcessTraversal() to be called (done in RA)
    XtSetArg(args[n], XmNtraversalOn, TRUE); n++;
    
    XtSetArg(args[n], XmNleftAttachment,    XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset,	    borderSize); n++;
    XtSetArg(args[n], XmNrightAttachment,   XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset,	    borderSize); n++;
    XtSetArg(args[n], XmNtopAttachment,	    XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopOffset,	    borderSize); n++;
    XtSetArg(args[n], XmNbottomAttachment,  XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomOffset,	    borderSize); n++;
    
    Widget glx = XtCreateWidget("GlxWidget", SoglwMDrawingAreaWidgetClass, 
	mgrWidget, args, n);
    
    //
    // Add GLX specific callbacks
    //
    
    XtAddCallback(glx, SoGLwNginitCallback, 
	(XtCallbackProc) SoXtGLWidget::ginitCB, (XtPointer) this);
    XtAddCallback(glx, SoGLwNresizeCallback, 
	(XtCallbackProc) SoXtGLWidget::resizeCB, (XtPointer) this);
    XtAddCallback(glx, SoGLwNexposeCallback, 
	(XtCallbackProc) SoXtGLWidget::exposeCB, (XtPointer) this);
    
    // enables us to add the event handler on the parent instead
//    XtUninstallTranslations(glx);
    
    // assign the right widget var and manage widget
    if (isDoubleBuffer())
	doubleBufferWidget = glx;
    else
	singleBufferWidget = glx;
    
    XtManageChild(glx);
    
    // make sure overlay window is on top
    if (overlayWidget && XtWindow(overlayWidget))
	XRaiseWindow(XtDisplay(overlayWidget), XtWindow(overlayWidget));
    
    // prevent a redraw a redraw from happening until we receive an
    // expose event - this will prevent too many redraws from happening
    // if the scene graph also changes at the same time.
    waitForExpose = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  	This builds the overly GLX widget (which is static)
//
// Use: protected
void
SoXtGLWidget::buildOverlayGLXWidget(XVisualInfo *vis)
//
////////////////////////////////////////////////////////////////////////
{
    if (getenv("IV_NO_OVERLAYS") || getenv("OIV_NO_OVERLAYS"))
	return;

    // build the visual from the attribute list if necessary
    if (! vis) {
	//
	// make sure the visual can be created, else don't built the overlay
	// widget (subclasses won't assume it is always built)
	//
	int n = 0, list[5];
	list[n++] = GLX_LEVEL;	// overlay planes
	list[n++] = 1;		// overlay planes
	list[n++] = GLX_BUFFER_SIZE; // get at least 2 bits
	list[n++] = 2;
	list[n++] = None;
	
	vis = glXChooseVisual(XtDisplay(mgrWidget), 
	    XScreenNumberOfScreen(XtScreen(mgrWidget)), list);
	if (! vis) {
#ifdef DEBUG
	    SoDebugError::post("SoXtGLWidget::buildOverlayGLXWidget",
			     "could not create overlay visual");
#endif
	    return;
	}
    }
    
    Arg args[12];
    int n = 0;
    XtSetArg(args[n], SoGLwNvisualInfo, vis); n++;
    // enables XmProcessTraversal() to be called (done in RA)
    XtSetArg(args[n], XmNtraversalOn, TRUE); n++;
    
    XtSetArg(args[n], XmNleftAttachment,    XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset,	    borderSize); n++;
    XtSetArg(args[n], XmNrightAttachment,   XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset,	    borderSize); n++;
    XtSetArg(args[n], XmNtopAttachment,	    XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopOffset,	    borderSize); n++;
    XtSetArg(args[n], XmNbottomAttachment,  XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomOffset,	    borderSize); n++;
    
    overlayWidget = XtCreateWidget("OverlayGLX", SoglwMDrawingAreaWidgetClass, 
	mgrWidget, args, n);
    
    //
    // Add GLX specific callbacks
    //
    
    XtAddCallback(overlayWidget, SoGLwNginitCallback, 
	(XtCallbackProc) SoXtGLWidget::overlayGinitCB, (XtPointer) this);
    XtAddCallback(overlayWidget, SoGLwNexposeCallback, 
	(XtCallbackProc) SoXtGLWidget::overlayExposeCB, (XtPointer) this);
    
    // enables us to add the event handler on the parent instead
//    XtUninstallTranslations(overlayWidget);
    
    XtManageChild(overlayWidget);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  	turns border on/off
//
// Use: public
void
SoXtGLWidget::setBorder(SbBool flag)
//
////////////////////////////////////////////////////////////////////////
{
    if (flag == isBorder())
	return;
    
    // set the border size and update the widgets
    borderSize = flag ? 2 : 0;
    
    Arg args[4];
    int n = 0;
    XtSetArg(args[n], XmNleftOffset,	    borderSize); n++;
    XtSetArg(args[n], XmNrightOffset,	    borderSize); n++;
    XtSetArg(args[n], XmNtopOffset,	    borderSize); n++;
    XtSetArg(args[n], XmNbottomOffset,	    borderSize); n++;
    
    if (doubleBufferWidget)
	XtSetValues(doubleBufferWidget, args, n);
    if (singleBufferWidget)
	XtSetValues(singleBufferWidget, args, n);
    if (overlayWidget)
	XtSetValues(overlayWidget, args, n);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  	enables/disabled the drawToFrontBuffer functionality
//
// Use: public
void
SoXtGLWidget::setDrawToFrontBufferEnable(SbBool flag)
//
////////////////////////////////////////////////////////////////////////
{
    enableDrawToFrontBuffer = flag;
    
    // clear the flag is disabled
    if (! enableDrawToFrontBuffer)
	drawToFrontBuffer = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  	sets the size of the glx widgets
//
// Use: protected
void
SoXtGLWidget::setGlxSize(SbVec2s newSize)
//
////////////////////////////////////////////////////////////////////////
{
    if (newSize == glxSize)
	return;
    
    // now set the container widget size (children will resize automatically)
    if (mgrWidget)
	XtVaSetValues(mgrWidget, XtNwidth, glxSize[0] + 2 * borderSize, 
	    XtNheight, glxSize[1] + 2 * borderSize, NULL);
    // else we havn't built yet, so cache this initial build size
    else
	glxSize = newSize;
}

Window SoXtGLWidget::getNormalWindow()
{ return getNormalWidget() ? XtWindow(getNormalWidget()) : 0; }

Window SoXtGLWidget::getOverlayWindow()
{ return getOverlayWidget() ? XtWindow(getOverlayWidget()) : 0; }


//
////////////////////////////////////////////////////////////////////////
// static callbacks stubs
////////////////////////////////////////////////////////////////////////
//


void
SoXtGLWidget::eventHandler(Widget w, SoXtGLWidget *p, XAnyEvent *xe, Boolean *)
{

    //
    // check and remove any other motion events in the queue for
    // that same window (compress the event) - don't use
    // XCheckTypedWindowEvent() because we DON't want to remove
    // all mouse motion events in the queue (even the ones we 
    // get after other events) - instead only remove the next event
    // if it is a mouse event.
    //
    
    if (xe->type == MotionNotify) {
//XMotionEvent *me = (XMotionEvent *)xe;
//printf("0 %d\n", me->time);	
//int i=1;

	Display *display = XtDisplay(w);
	XEvent event, motion;
	SbBool valid = FALSE;
	
	while (XEventsQueued(display, QueuedAlready)) {
	    XPeekEvent(display, &event);
	    if (event.type == MotionNotify) {
		XNextEvent(display, &motion);
		valid = TRUE;
//me = (XMotionEvent *) &motion;
//printf("%d %d\n", i++, me->time);
	    }
	    else if (event.type == NoExpose)
		// toss it out - for some reason
		// those events are being generated
		// in the middle of motion (for the color picker)
		// and we need to discard them out...
		XNextEvent(display, &event);
	    else {
//printf("type %d\n", event.type);
		break;
	    }
	}
	
	if (valid)
	    p->processEvent((XAnyEvent *) &motion); // latest motion
	else
	    p->processEvent(xe); // original motion
	
	return;
    }
    
    p->processEvent(xe);
}

void
SoXtGLWidget::ginitCB(Widget glx, SoXtGLWidget *p, XtPointer pt)
{
    // set the initial size of the glx widget
    SoGLwDrawingAreaCallbackStruct *data = (SoGLwDrawingAreaCallbackStruct *) pt;
    p->glxSize.setValue(data->width, data->height);
    
    //
    // create a GLX context, and make this the current window, but before
    // make sure that we don't already have a valid context (because
    // ginitCB() will be called every time the widget is realized, not just
    // the first time).
    //
    GLXContext ctx = p->isDoubleBuffer() ? p->ctxDouble : p->ctxSingle;
    if (ctx == 0) {
	SbPList *contextList = contextListKeeper.find(XtDisplay(glx), SCREEN(glx));
	if (contextList->getLength())
	    ctx = (GLXContext) (*contextList)[0];
	XVisualInfo *vis;
	XtVaGetValues(glx, SoGLwNvisualInfo, &vis, NULL);
	ctx = glXCreateContext(XtDisplay(glx), vis, ctx, GL_TRUE);
	contextList->append(ctx);
	if (glx == p->doubleBufferWidget)
	    p->ctxDouble = ctx;
	else
	    p->ctxSingle = ctx;
    }
    p->ctxNormal = ctx;
    
    glXMakeCurrent(XtDisplay(glx), XtWindow(glx), ctx);
    
    // enable zbuffer if this visual supports it....
    if (p->glModes & SO_GLX_ZBUFFER)
	glEnable(GL_DEPTH_TEST);
    
    //
    // get the normal window color map, and allocate every colors
    // as read/write.
    //
    // Note: this only needs to be done once since the color map is
    // shared between widgets.
    //
    static Colormap previousMap = 0;
    if (! p->isRGBMode()) {
	XtVaGetValues(glx, XmNcolormap, &p->colorMap, NULL);
	if (p->colorMap != previousMap) {
	    unsigned long plane_mask[1];	// System long
	    XVisualInfo *vis;
	    XtVaGetValues(glx, SoGLwNvisualInfo, &vis, NULL);
	    int ncolors = vis->colormap_size;
	    Pixel *pixel = (Pixel *) XtMalloc(ncolors*sizeof(Pixel));
	    int result = XAllocColorCells(XtDisplay(glx), p->colorMap, True, 
		plane_mask, 0, pixel, ncolors);
	    XtFree((char *) pixel);
	    
	    previousMap = p->colorMap;
	}
    }
    
    // let subclasses know we changed widget
    p->widgetChanged(glx);
    
    // forces sizeChanged() to be at least called once
    p->windowResized = TRUE;
    
    // finally call subclass routine
    p->initGraphic();
    
    // make sure overlay window is on top
    if (p->overlayWidget && XtWindow(p->overlayWidget))
	XRaiseWindow(XtDisplay(p->overlayWidget), XtWindow(p->overlayWidget));
}

void
SoXtGLWidget::overlayGinitCB(Widget glx, SoXtGLWidget *p, XtPointer)
{
    //
    // create a GLX context, and make this the current window, but before
    // make sure that we don't already have a valid context (because
    // overlayGinitCB() will be called every time the widget is realized, 
    // not just the first time).
    //
    if (p->ctxOverlay == 0) {
	GLXContext ctx = 0;
	SbPList *contextList = contextListKeeper.find(XtDisplay(glx),SCREEN(glx));
	if (contextList->getLength())
	    ctx = (GLXContext) (*contextList)[0];
	XVisualInfo *vis;
	XtVaGetValues(glx, SoGLwNvisualInfo, &vis, NULL);
	p->ctxOverlay = glXCreateContext(XtDisplay(glx), vis, ctx, GL_TRUE);
	contextList->append(p->ctxOverlay);
    }
    
    glXMakeCurrent(XtDisplay(glx), XtWindow(glx), p->ctxOverlay);
    
    //
    // get the overlay color map, and allocate every colors
    // as read/write except transparency color 0.
    //
    // Note: this only needs to be done once since the color map is
    // shared between overlay widgets.
    //
    static Colormap previousMap = 0;
    XtVaGetValues(glx, XmNcolormap, &p->overlayColorMap, NULL);
    if (p->overlayColorMap != previousMap) {
	unsigned long plane_mask[1];		// System long
	XVisualInfo *vis;
	XtVaGetValues(glx, SoGLwNvisualInfo, &vis, NULL);
	int ncolors = vis->colormap_size - 1; // one less due to transparency
	Pixel *pixel = (Pixel *) XtMalloc(ncolors*sizeof(Pixel));
	int result = XAllocColorCells(XtDisplay(glx), p->overlayColorMap, True, 
	    plane_mask, 0, pixel, ncolors);
	XtFree((char *) pixel);
	
	previousMap = p->overlayColorMap;
    }
    
    // forces sizeChanged() to be at least called once
    p->windowResized = TRUE;
    
    // finally call subclass routine
    p->initOverlayGraphic();
}

void
SoXtGLWidget::exposeCB(Widget w, SoXtGLWidget *p, XtPointer)
{
    // check and remove any other expose events in the queue for
    // that same window (compress expose events).
    XEvent event;
    while (XCheckTypedWindowEvent(XtDisplay(w), XtWindow(w), 
	Expose, &event)) ;
    
    // check if subclasses sizeChanged() routine needs to be called.
    if (p->windowResized) {
	p->sizeChanged(p->glxSize);
	p->windowResized = FALSE;
    }
    
    // clear this flag now that we have an expose event
    p->waitForExpose = FALSE;
    
    // on expose, draw the first image to the front buffer since we
    // might have garbadge in the window anyway....
    if (p->enableDrawToFrontBuffer)
	p->drawToFrontBuffer = TRUE;
    
    p->redraw();
}

void
SoXtGLWidget::overlayExposeCB(Widget w, SoXtGLWidget *p, XtPointer)
{
    // check and remove any other expose events in the queue for
    // that same window (compress expose events).
    XEvent event;
    while (XCheckTypedWindowEvent(XtDisplay(w), XtWindow(w), 
	Expose, &event)) ;
    
    // check if subclasses sizeChanged() routine needs to be called.
    if (p->windowResized) {
	p->sizeChanged(p->glxSize);
	p->windowResized = FALSE;
    }
    
    p->redrawOverlay();
}

void
SoXtGLWidget::resizeCB(Widget, SoXtGLWidget *p, XtPointer pt)
{
    // set the new glx window size
    SoGLwDrawingAreaCallbackStruct *data = (SoGLwDrawingAreaCallbackStruct *) pt;
    p->glxSize.setValue(data->width, data->height);
    
    // ??? only set the flag right now instead of calling sizeChanged()
    // ??? because the window hasn't been mapped to it's new size yet
    // ??? (wait for expose event).
    p->windowResized = TRUE;
}



//
////////////////////////////////////////////////////////////////////////
// dummy virtual functions
////////////////////////////////////////////////////////////////////////
//

void SoXtGLWidget::redrawOverlay() {}
void SoXtGLWidget::processEvent(XAnyEvent *) {}
void SoXtGLWidget::initGraphic() {}
void SoXtGLWidget::initOverlayGraphic() {}
void SoXtGLWidget::sizeChanged(const SbVec2s &) {}
void SoXtGLWidget::widgetChanged(Widget) {}
