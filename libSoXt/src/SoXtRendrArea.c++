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
 |	SoXtRenderArea
 |
 |   Author(s): David Mott, Alain Dumesny
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <Inventor/SoDB.h>
#include <Inventor/SbLinear.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/elements/SoWindowElement.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoLocateHighlight.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/Xt/SoXtResource.h>
#include <Inventor/Xt/SoXtRenderArea.h>
#include <Inventor/Xt/devices/SoXtKeyboard.h>
#include <Inventor/Xt/devices/SoXtMouse.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <stdlib.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include <Xm/Xm.h>

static char *thisClassName = "SoXtRenderArea";


////////////////////////////////////////////////////////////////////////
//
// Public constructor - build the widget right now
//
SoXtRenderArea::SoXtRenderArea(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent, 
    SbBool getMouseInput,
    SbBool getKeyboardInput)
	: SoXtGLWidget(
	    parent,
	    name, 
	    buildInsideParent, 
	    SO_GLX_RGB | SO_GLX_DOUBLE | SO_GLX_ZBUFFER | SO_GLX_OVERLAY, 
	    FALSE) // tell GLWidget not to build just yet 
//
////////////////////////////////////////////////////////////////////////
{
    // In this case, render area is what the app wants, so buildNow = TRUE
    constructorCommon(getMouseInput, getKeyboardInput, TRUE);
}

////////////////////////////////////////////////////////////////////////
//
// SoEXTENDER constructor - the subclass tells us whether to build or not
//
SoXtRenderArea::SoXtRenderArea(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent, 
    SbBool getMouseInput,
    SbBool getKeyboardInput, 
    SbBool buildNow)
	: SoXtGLWidget(
	    parent,
	    name, 
	    buildInsideParent, 
	    SO_GLX_RGB | SO_GLX_DOUBLE | SO_GLX_ZBUFFER | SO_GLX_OVERLAY, 
	    FALSE) // tell GLWidget not to build just yet 
//
////////////////////////////////////////////////////////////////////////
{
    // In this case, render area may be what the app wants, 
    // or it may want a subclass of render area. Pass along buildNow
    // as it was passed to us.
    constructorCommon(getMouseInput, getKeyboardInput, buildNow);
}

////////////////////////////////////////////////////////////////////////
//
// Called by the constructors
//
// private
//
void
SoXtRenderArea::constructorCommon(
    SbBool getMouseInput,
    SbBool getKeyboardInput, 
    SbBool buildNow)
//
////////////////////////////////////////////////////////////////////////
{    
    addVisibilityChangeCallback(visibilityChangeCB, this);
    setClassName(thisClassName);
    firstEvent = TRUE;
    
    // set up the device list
    deviceList = new SbPList;
    
    // default devices
    if (getMouseInput) {
	mouseDevice = new SoXtMouse();
	deviceList->append(mouseDevice);
    }
    else mouseDevice = NULL;
    
    if (getKeyboardInput) {
	keybdDevice = new SoXtKeyboard();
	deviceList->append(keybdDevice);
    }
    else keybdDevice = NULL;
    
    // local vars
    clearFirst = TRUE;
    clearOverlayFirst = TRUE;
    autoRedraw = TRUE;
    selection = overlaySelection = NULL;
    deviceWidget = NULL;
    
    setSize(SbVec2s(400, 400));  // default size
    setBorder(TRUE);
    
    // inventor specific variables
    appEventHandler = NULL;
    
    // the scene manager can be supplied by a subclass.,
    sceneMgr = new SoSceneManager(); 
    sceneMgr->setRenderCallback(renderCB, this);
    
    // the overlay scene manager is supplied right here.
   
    overlaySceneMgr = new SoSceneManager(); 
    overlaySceneMgr->setRenderCallback(renderOverlayCB, this);
    
    // color map variables
    mapColors = overlayMapColors = NULL;
    mapColorNum = overlayMapColorNum = 0;
    
    // Build the widget tree, and let SoXtComponent know about our base widget.
    if (buildNow)
	setBaseWidget(buildWidget(getParentWidget()));
}

////////////////////////////////////////////////////////////////////////
//
// Destructor
//
SoXtRenderArea::~SoXtRenderArea()
//
////////////////////////////////////////////////////////////////////////
{
    // Remove our callback from old selection node
    if (selection != NULL) {
	selection->removeChangeCallback(SoXtRenderArea::selectionChangeCB, this);
	
	// Unref the old selection node
	selection->unref(); 
    }

    unregisterWidget(getGlxMgrWidget());
    
    if (mapColors != NULL)
	free(mapColors);
    if (overlayMapColors != NULL)
	free(overlayMapColors);

    if (mouseDevice != NULL) delete mouseDevice;
    if (keybdDevice != NULL) delete keybdDevice;
    delete sceneMgr;
    delete overlaySceneMgr;
    delete deviceList;
    //??? delete the mouse and keybd if we created them!
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  	Make the new user supplied scene graph the rendering root.
//
// use: virtual public
//
void
SoXtRenderArea::setSceneGraph(SoNode *newScene)
//
////////////////////////////////////////////////////////////////////////
{
    // Deactivate while we change the scene so that our sensors
    // get unhooked before the data changes beneath us.
    sceneMgr->deactivate();
    
    sceneMgr->setSceneGraph(newScene);
    
    // draw new scene graphs to the front buffer by default since
    // the scene will be different (we might has well see something
    // happening for the first redraw).
    if (isDrawToFrontBufferEnable())
	drawToFrontBuffer = TRUE;
    
    // we activate only if we are visible.
    // after all, if we're not on screen, the visibility change
    // callback will invoke activate() once we are on screen.
    if (isVisible() && autoRedraw) {
	sceneMgr->activate();
	sceneMgr->scheduleRedraw();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  	sets the overlay plane scene graph.
//
// use: public
//
void
SoXtRenderArea::setOverlaySceneGraph(SoNode *newScene)
//
////////////////////////////////////////////////////////////////////////
{
    if (! getOverlayWidget())
	return;

    // Deactivate while we change the scene so that our sensors
    // get unhooked before the data changes beneath us.
    overlaySceneMgr->deactivate();
    
    overlaySceneMgr->setSceneGraph(newScene);
    
    // we activate only if we are visible.
    // after all, if we're not on screen, the visibility change
    // callback will invoke activate() once we are on screen.
    if (isVisible() && autoRedraw) {
	overlaySceneMgr->activate();
	overlaySceneMgr->scheduleRedraw();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  	Return the rendering root.
//
// use: public, virtual
//
SoNode *
SoXtRenderArea::getSceneGraph()
//
////////////////////////////////////////////////////////////////////////
{
    return sceneMgr->getSceneGraph();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  	Loads the color map with the given colors.
//
// use: public
//
void
SoXtRenderArea::setColorMap(int startIndex, int num, const SbColor *colors)
//
////////////////////////////////////////////////////////////////////////
{
    // save those colors for future uses (if the widget hasn't been
    // built yet, or next time it gets built)
    if (mapColors != NULL)
	free(mapColors);
    mapColors = (XColor *) malloc(sizeof(XColor) * num);
    mapColorNum = num;
    XColor *xcol = mapColors;
    for (int i=0; i<num; i++, xcol++) {
	xcol->red   = (unsigned short) (colors[i][0] * 65535);
	xcol->green = (unsigned short) (colors[i][1] * 65535);
	xcol->blue  = (unsigned short) (colors[i][2] * 65535);
	xcol->flags = DoRed|DoGreen|DoBlue;
	xcol->pixel = startIndex + i;
    }

    // now load those colors into the color map
    if (colorMap != 0)
	XStoreColors(getDisplay(), colorMap, mapColors, mapColorNum);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  	Loads the overlay color map with the given colors.
//
// use: public
//
void
SoXtRenderArea::setOverlayColorMap(int startIndex, int num, const SbColor *colors)
//
////////////////////////////////////////////////////////////////////////
{
    if (! getOverlayWidget())
	return;

    // save those colors for future uses (if the widget hasn't been
    // built yet, or next time it gets built)
    if (overlayMapColors != NULL)
	free(overlayMapColors);
    overlayMapColors = (XColor *) malloc(sizeof(XColor) * num);
    overlayMapColorNum = num;
    XColor *xcol = overlayMapColors;
    for (int i=0; i<num; i++, xcol++) {
	xcol->red   = (unsigned short) (colors[i][0] * 65535);
	xcol->green = (unsigned short) (colors[i][1] * 65535);
	xcol->blue  = (unsigned short) (colors[i][2] * 65535);
	xcol->flags = DoRed|DoGreen|DoBlue;
	xcol->pixel = startIndex + i;
    }

    // now load those colors into the color map
    if (overlayColorMap != 0)
	XStoreColors(getDisplay(), overlayColorMap, overlayMapColors, overlayMapColorNum);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  	Sets the transparency type
//
// use: public
//
void
SoXtRenderArea::setTransparencyType(SoGLRenderAction::TransparencyType type)
//
////////////////////////////////////////////////////////////////////////
{
    sceneMgr->getGLRenderAction()->setTransparencyType(type);
    sceneMgr->scheduleRedraw();
}

////////////////////////////////////////////////////////////////////////
//
// Just call buildWidget on the base class (SoXtGLWidget).
//
// use: protected
//
Widget
SoXtRenderArea::buildWidget(Widget parent)
//
////////////////////////////////////////////////////////////////////////
{
    // Have the parent class build the mgrWidget.
    // Register the class here, though, since this is where resources
    // are needed (other GL widgets don't have resources)
    SoXtGLWidget::buildWidget(parent);
    registerWidget(getGlxMgrWidget());

    // get the resources for this component
    SoXtResource xr(getGlxMgrWidget());
    SbColor c;
    if (xr.getResource("backgroundColor", "BackgroundColor", c))
	 setBackgroundColor(c);

    // get the transparent color index
#ifdef GLX_TRANSPARENT_INDEX_VALUE_EXT
    XVisualInfo *vis = getOverlayVisual();
    if (vis != NULL) {
	int index;
	glXGetConfig(XtDisplay(parent), vis, GLX_TRANSPARENT_INDEX_VALUE_EXT,
	    &index);
	overlaySceneMgr->setBackgroundIndex(index);
    }
#endif

    return getGlxMgrWidget();
}

////////////////////////////////////////////////////////////////////////
//
// Called by the base class when the frontmost widget changes
// (i.e. from single -> double buffer or vice versa).
//
// use: virtual protected
//
void
SoXtRenderArea::widgetChanged(Widget newWidget)
//
////////////////////////////////////////////////////////////////////////
{
    // init the render actions whenever we change windows
    if (newWidget)
	sceneMgr->reinitialize();
    
    //
    // Work around bug #164424
    // if we don't use the overlay window, register the devices/handler
    // on the new normal widget.
    //
    if (! getOverlayWidget())
	reinstallDevices(newWidget);
}

////////////////////////////////////////////////////////////////////////
//
// Register interest to handle events for the passed device.
// If device is already in the list, then make sure it's enabled.
//
// use: public
//
void
SoXtRenderArea::registerDevice(SoXtDevice *device)
//
////////////////////////////////////////////////////////////////////////
{
    // Add the device if it's not already in the list of devices
    int index = deviceList->find(device);
    // ??? mott, shouldn't we return if the device is found ?
    if (index == -1)
	deviceList->append(device);

    // tell the device the window size
    device->setWindowSize(getGlxSize());

    // Tell the device to register event interest for our widget
    Widget w = getOverlayWidget() ? getOverlayWidget() : getNormalWidget();
    if ((w != NULL) && (XtWindow(w) != (Window) NULL))
	device->enable(
	    w,
	    (XtEventHandler) SoXtGLWidget::eventHandler,
	    (XtPointer) this,
	    XtWindow(w));
}

////////////////////////////////////////////////////////////////////////
//
// Unregister interest in the passed device.
//
// use: public
//
void
SoXtRenderArea::unregisterDevice(SoXtDevice *device)
//
////////////////////////////////////////////////////////////////////////
{
    // Find the device in our device list
    int index = deviceList->find(device);
    if (index == -1)
	return;

    // Remove the device from the list of devices
    deviceList->remove(index);

    // Tell the device to register event interest for our widget
    Widget w = getOverlayWidget() ? getOverlayWidget() : getNormalWidget();
    if (w)
	device->disable(
	    w,
	    (XtEventHandler) SoXtGLWidget::eventHandler,
	    (XtPointer) this);
}

////////////////////////////////////////////////////////////////////////
//
// disable devices on the old widget, and enables them on the new widget.
//
// use: private
//
void
SoXtRenderArea::reinstallDevices(Widget newWidget)
//
////////////////////////////////////////////////////////////////////////
{
    for (int i = 0; i < deviceList->getLength(); i++) {
	SoXtDevice *device = (SoXtDevice *) (*deviceList)[i];
	
	// disable on the old widget...
	if (deviceWidget)
	    device->disable(
		deviceWidget, 
		(XtEventHandler) SoXtGLWidget::eventHandler,
		(XtPointer) this);
	
	// enable on the new widget....
	if (newWidget) {
	    device->setWindowSize(getGlxSize());
	    device->enable(
		newWidget, 
		(XtEventHandler) SoXtGLWidget::eventHandler,
		(XtPointer) this,
		XtWindow(newWidget));
	}
    }
    
    // ??? add an event handler to receive EnterNotify events to make
    // ??? sure the render area has the current keyboard focus, otherwise
    // ??? keyboard events will go to whichever text field has focus (black
    // ??? outline around widget). This is the prefered behavior since we
    // ??? don't want to force the keyboardFocus to be POINTER or require
    // ??? the user to press a mouse button down.
    //
    // We also need a EnterNotify and LeaveNotify to correctly update
    // stuff for SoLocateHighlight (keep track of current window).
    //
    if (deviceWidget)
	XtRemoveEventHandler(deviceWidget, (EnterWindowMask | LeaveWindowMask), FALSE,
	    (XtEventHandler) SoXtRenderArea::windowEventCB, 
	    (XtPointer) this);
    if (newWidget)
	XtAddEventHandler(newWidget, (EnterWindowMask | LeaveWindowMask), FALSE,
	    (XtEventHandler) SoXtRenderArea::windowEventCB, 
	    (XtPointer) this);
    
    deviceWidget = newWidget;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  	Invoke the app callback and return what the callback returns.
//
// use: protected
//
SbBool
SoXtRenderArea::invokeAppCB(XAnyEvent *anyevent)
//
////////////////////////////////////////////////////////////////////////
{
    // if app wants event, send event to application
    if (appEventHandler != NULL)
	return (*appEventHandler)(appEventHandlerData, anyevent);
    else
	return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  	Process the passed X event.
//
// use: virtual protected
//
void
SoXtRenderArea::processEvent(XAnyEvent *anyevent)
//
////////////////////////////////////////////////////////////////////////
{
    // check if app wants the event...
    if (invokeAppCB(anyevent))
	return;
    
    // ...else send it to the scene
    
    // to do this, translate the X event to an SoEvent
    // by finding a device to perform the translation 
    const SoEvent *soevent = NULL;
    for (int i = 0; (soevent == NULL) && (i < deviceList->getLength()); i++) {
	SoXtDevice *device = (SoXtDevice *) (*deviceList)[i];
	soevent = device->translateEvent(anyevent);
    }
    
    // no device found, so return...
    if (! soevent)
	return;
    
    // now send the event first to the overlay scene graph, elses to
    // the regular scene graph.
    SbBool handled = overlaySceneMgr->processEvent(soevent);
    if (! handled) {
	sceneMgr->processEvent(soevent);
	
	// now check to make sure that we updated the handle event action
	// with the current window the very first time. This is needed
	// because the SoState does not exists until the action is
	// applied, and we only update those during enter/leave notify.
	if (firstEvent) {
	    SoState *state = sceneMgr->getHandleEventAction()->getState();
	    if (state) {
		SoWindowElement::set(state, getNormalWindow(), 
		    getNormalContext(), getDisplay(), getGLRenderAction());
		firstEvent = FALSE;
	    }
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Initialize the GLX window for rendering.
// glXMakeCurrent() should have been called before invoking this routine.
//
// use: virtual protected
//
void
SoXtRenderArea::initGraphic()
//
////////////////////////////////////////////////////////////////////////
{
    sceneMgr->reinitialize();
    sceneMgr->setRGBMode(isRGBMode());
    
    SoGLRenderAction *ra = sceneMgr->getGLRenderAction();
    ra->setCacheContext(getDisplayListShareGroup(getNormalContext()));
    ra->setRenderingIsRemote(!glXIsDirect(getDisplay(), getNormalContext()));

    // load the color map
    if (! isRGBMode() && colorMap != 0 && mapColorNum != 0)
	XStoreColors(getDisplay(), colorMap, mapColors, mapColorNum);
}

////////////////////////////////////////////////////////////////////////
//
// Initialize the GLX window for rendering.
// glXMakeCurrent() should have been called before invoking this routine.
//
// use: virtual protected
//
void
SoXtRenderArea::initOverlayGraphic()
//
////////////////////////////////////////////////////////////////////////
{
    overlaySceneMgr->reinitialize();
    overlaySceneMgr->setRGBMode(FALSE);
    
    SoGLRenderAction *ra = overlaySceneMgr->getGLRenderAction();
    ra->setCacheContext(getDisplayListShareGroup(getOverlayContext()));
    ra->setRenderingIsRemote(!glXIsDirect(getDisplay(), getOverlayContext()));

    // load the color map
    if (overlayColorMap != 0 && overlayMapColorNum != 0)
	XStoreColors(getDisplay(), overlayColorMap, overlayMapColors, overlayMapColorNum);
    
    // enable the devices on the overlay window
    reinstallDevices(getOverlayWidget());
}

////////////////////////////////////////////////////////////////////////
//
// Called by the base class (SoXtGLWidget) when the widget has changed size.
//
// use: virtual protected
//
void
SoXtRenderArea::sizeChanged(const SbVec2s &newSize)
//
////////////////////////////////////////////////////////////////////////
{
    sceneMgr->setWindowSize(newSize);
    overlaySceneMgr->setWindowSize(newSize);
    
    // tell each device the new window size
    for (int i = 0; i < deviceList->getLength(); i++) {
	SoXtDevice *device = (SoXtDevice *) (*deviceList)[i];
	device->setWindowSize(newSize);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Attach the sensor (if necessary).
//
// use: private
//
void
SoXtRenderArea::activate()
//
////////////////////////////////////////////////////////////////////////
{
    // if autoRedraw is off, then don't attach the scene sensor
    if (! autoRedraw)
	return;
    
    // Activate the scene manager
    sceneMgr->activate();
    overlaySceneMgr->activate();
}

////////////////////////////////////////////////////////////////////////
//
// Detach the sensor.
//
// use: private
//
void
SoXtRenderArea::deactivate()
//
////////////////////////////////////////////////////////////////////////
{
    // detach sensors
    sceneMgr->deactivate();
    overlaySceneMgr->deactivate();
}

////////////////////////////////////////////////////////////////////////
//
// Turn auto-redraw on or off. If turning on, then activate the scene
// sensor.
//
// use: public
//
void
SoXtRenderArea::setAutoRedraw(SbBool flag)
//
////////////////////////////////////////////////////////////////////////
{
    if (flag == autoRedraw)
	return;

    autoRedraw = flag;

    if (autoRedraw) {
	if (isVisible())
	    activate();
    }
    else
	deactivate();
}

////////////////////////////////////////////////////////////////////////
//
// Have the scene manager redraw the scene.
//
// use: virtual protected
//
void
SoXtRenderArea::actualRedraw()
//
////////////////////////////////////////////////////////////////////////
{
    sceneMgr->render(clearFirst);
}

////////////////////////////////////////////////////////////////////////
//
// Have the scene manager redraw the overlay scene.
//
// use: virtual protected
//
void
SoXtRenderArea::actualOverlayRedraw()
//
////////////////////////////////////////////////////////////////////////
{
    overlaySceneMgr->render(clearOverlayFirst);    
}

////////////////////////////////////////////////////////////////////////
//
// This routine draws the scene graph (called by expose events and the
// scene graph sensor)
//
// use: virtual protected
//
void
SoXtRenderArea::redraw()
//
////////////////////////////////////////////////////////////////////////
{
    // return if we are not visible or if we need to wait for an expose
    // event (case when the sensor triggers but we know that an expose
    // event will come along and we don't want 2 redraws).
    if (!isVisible() || waitForExpose)
	return;
    
    // set the window
    glXMakeCurrent(getDisplay(), getNormalWindow(), getNormalContext());
    
    // see if we need to temporary draw to the front buffer (which
    // is set when we display a new scene or get an expose event)
    if (drawToFrontBuffer && isDrawToFrontBufferEnable() && isDoubleBuffer()) {
	
	// ??? workaround bug 301010 - it seems that redrawing to the front
	// ??? of a window that might not be on the screen ( isVisible() might
	// ??? be incorect in a few cases) will confuse the GFX hardware
	// ??? of some machine (Xtrem and Indy24).
	// 
	// ??? Note: this also fixes 298058 (redrawing to the front window
	// ??? on indigo starter gfx, which happened all the time).
	//
	if (isRGBMode()) {
	    SbColor color = getBackgroundColor();
	    glClearColor(color[0], color[1], color[2], 0);
	} else
	    glClearIndex(getBackgroundIndex());
	glClear(GL_COLOR_BUFFER_BIT);
	glXSwapBuffers(getDisplay(), getNormalWindow());
	// ??? end of BUG workaround
	
	glReadBuffer(GL_FRONT); // Needed for acbuf antialiasing
	glDrawBuffer(GL_FRONT);
    }
    
    // draw that scene! (subclasses may redefine...)
    actualRedraw();
    
    // swap those buffers!
    if (isDoubleBuffer()) {
	if (drawToFrontBuffer && isDrawToFrontBufferEnable()) {
	    // no need to swap here - instead restore the buffer and 
	    // clear the flag now that we have drawn to the front buffer
	    glReadBuffer(GL_BACK); // Needed for acbuf antialiasing
	    glDrawBuffer(GL_BACK);
	    glFlush();
	}
	else
	    glXSwapBuffers(getDisplay(), getNormalWindow());
    }
    else
	glFlush();
    
    // clear this flag now that we have drawn
    drawToFrontBuffer = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Sets antialiasing on GL rendering action.
//
// use: public
//
void
SoXtRenderArea::setAntialiasing(SbBool smoothing, int numPasses)
//
////////////////////////////////////////////////////////////////////////
{
    SoGLRenderAction *ra = getGLRenderAction();

    if (smoothing == ra->isSmoothing() && numPasses == ra->getNumPasses())
	return;
    
    // The visual needs to change if we are now enabling or disabling
    // the accumulation buffer
    SbBool needToChangeVisual = ((numPasses >  1 && ra->getNumPasses() == 1) ||
				 (numPasses == 1 && ra->getNumPasses() >  1));

    //
    // now create a visual with the right info - this tries to
    // preserve as many setting as the original visual, with just
    // the antialiasing changed.
    //
    if (needToChangeVisual) {
	
	int n = 0;
	int attribList[20];
	
	if (isRGBMode()) {
	    attribList[n++] = GLX_RGBA;
	    attribList[n++] = GLX_RED_SIZE;
	    attribList[n++] = 1;
	    attribList[n++] = GLX_GREEN_SIZE;
	    attribList[n++] = 1;
	    attribList[n++] = GLX_BLUE_SIZE;
	    attribList[n++] = 1;
	}
	// there is always zbuffer...
	attribList[n++] = GLX_DEPTH_SIZE;
	attribList[n++] = 1;
	if (isDoubleBuffer())
	    attribList[n++] = GLX_DOUBLEBUFFER;
	
	// now add the antialiasing stuff
	if (numPasses > 1) {
	    attribList[n++] = GLX_ACCUM_RED_SIZE;
	    attribList[n++] = 1;
	    attribList[n++] = GLX_ACCUM_GREEN_SIZE;
	    attribList[n++] = 1;
	    attribList[n++] = GLX_ACCUM_BLUE_SIZE;
	    attribList[n++] = 1;
	}

	// check for stencil buffer
	Widget w = getWidget();
	XVisualInfo *normalVis = getNormalVisual();
	if (normalVis) {
	    int val;
	    glXGetConfig(XtDisplay(w), normalVis, GLX_STENCIL_SIZE, &val);
	    if (val) {
		attribList[n++] = GLX_STENCIL_SIZE;
		attribList[n++] = val;
	    }
	}

	attribList[n++] = (int) None;
	
	// create the visual
	XVisualInfo *vis = glXChooseVisual(XtDisplay(w), 
	    XScreenNumberOfScreen(XtScreen(w)), attribList);
	
	if (! vis) {
#ifdef DEBUG
	    SoDebugError::post("SoXtRenderArea::setAntialiasing",
			     "could not create ACCUM visual");
#endif
	    return;
	}
	
	// now set this as the current visual
	setNormalVisual(vis);
	
	// GL widget made a copy - we can free this
	XFree(vis);
    }
    else // only the number of passes changed, so just redraw
	sceneMgr->scheduleRedraw();
    
    // finally set the stuff on the render action
    sceneMgr->setAntialiasing(smoothing, numPasses);
}

////////////////////////////////////////////////////////////////////////
//
// This routine draws the scene graph for the overlay bit planes (called
// by the expose event and scene graph sensor).
//
// use: virtual protected
//
void
SoXtRenderArea::redrawOverlay()
//
////////////////////////////////////////////////////////////////////////
{
    if (!isVisible() || getOverlayWindow() == (Window) NULL)
	return;
    
    // set the window
    glXMakeCurrent(getDisplay(), getOverlayWindow(), getOverlayContext());
    
    // draw that scene! (subclasses may redefine...)
    actualOverlayRedraw();
    
    glFlush();
}

////////////////////////////////////////////////////////////////////////
//
// Schedule a redraw to happen sometime soon.
//
// use: public
//
void
SoXtRenderArea::scheduleRedraw()
//
////////////////////////////////////////////////////////////////////////
{
    if (isAutoRedraw())
	sceneMgr->scheduleRedraw();
    else
	redraw(); 
}

////////////////////////////////////////////////////////////////////////
//
// Schedule a redraw to happen sometime soon.
//
// use: public
//
void
SoXtRenderArea::scheduleOverlayRedraw()
//
////////////////////////////////////////////////////////////////////////
{
    if (isAutoRedraw())
	overlaySceneMgr->scheduleRedraw();
    else
	redrawOverlay(); 
}

////////////////////////////////////////////////////////////////////////
//
// Convenience to set up the redraw mechansim whenever the selection
// changes.
//
// public
//
void
SoXtRenderArea::redrawOnSelectionChange(SoSelection *s)
//
////////////////////////////////////////////////////////////////////////
{
    // Ref the new selection node
    if (s != NULL)
	s->ref();
	
    // Remove our callback from old selection node
    if (selection != NULL) {
	selection->removeChangeCallback(SoXtRenderArea::selectionChangeCB, this);
	
	// Unref the old selection node
	selection->unref(); 
    }

    selection = s;
    
    // Add our callback to this selection node. (We've already ref'd this new sel node)
    if (selection != NULL)
	selection->addChangeCallback(SoXtRenderArea::selectionChangeCB, this);
}

////////////////////////////////////////////////////////////////////////
//
// Convenience to set up the redraw mechansim whenever the selection
// changes.
//
// public
//
void
SoXtRenderArea::redrawOverlayOnSelectionChange(SoSelection *s)
//
////////////////////////////////////////////////////////////////////////
{
    // Remove our callback from old selection node
    if (overlaySelection != NULL)
	overlaySelection->removeChangeCallback(SoXtRenderArea::overlaySelectionChangeCB, this);

    overlaySelection = s;
    
    // Add our callback to this selection node
    if (overlaySelection != NULL)
	overlaySelection->addChangeCallback(SoXtRenderArea::overlaySelectionChangeCB, this);
}



//
// redefine those generic virtual functions
//
const char *
SoXtRenderArea::getDefaultWidgetName() const
{ return thisClassName; }

const char *
SoXtRenderArea::getDefaultTitle() const
{ return "Xt RenderArea"; }

const char *
SoXtRenderArea::getDefaultIconTitle() const
{ return "Xt RenderArea"; }


//
////////////////////////////////////////////////////////////////////////
// static callbacks stubs
////////////////////////////////////////////////////////////////////////
//




#include <Inventor/Xt/SoXt.h>
//
// Called whenever an enter/leave window event occurs. This forces the
// render area to get/loose keyboard Focus (as opposed to requiring
// POINTER focus policy, or requiring the user to press a mouse
// button down).
//
// We also need a EnterNotify and LeaveNotify to correctly update
// stuff for SoLocateHighlight (keep track of current window).
//
void
SoXtRenderArea::windowEventCB(Widget w, SoXtRenderArea *p, XAnyEvent *xe, Boolean *)
{
    if (xe->type == EnterNotify) {
	// get keyboard focus....
	if (p->getOverlayWidget())
	    XmProcessTraversal(p->getOverlayWidget(), XmTRAVERSE_CURRENT);
	else
	    XmProcessTraversal(p->getNormalWidget(), XmTRAVERSE_CURRENT);
	
	//
	// update the windowElement for the handleEventAction as well
	// as the GLRenderAction to point to this window.
	//
	// Note: don't touch the windowElement if we are in the middle
	// of rendering (if we process event during render abort).
	//
	
	SoState *state = p->sceneMgr->getHandleEventAction()->getState();
	if (state)
	    SoWindowElement::set(state, p->getNormalWindow(), 
		p->getNormalContext(), p->getDisplay(), p->getGLRenderAction());
	state = p->sceneMgr->getGLRenderAction()->getState();
	if (state && state->getDepth() == 1)
	    SoWindowElement::set(state, p->getNormalWindow(), 
		p->getNormalContext(), p->getDisplay(), p->getGLRenderAction());
    }
    else if (xe->type == LeaveNotify) {
	// loose keyboard focus...
	XmProcessTraversal(SoXt::getShellWidget(w), XmTRAVERSE_CURRENT);
	
	//
	// clear the windowElement from the actions now that we are
	// leaving the window.
	//
	// Note: don't touch the windowElement if we are in the middle
	// of rendering (if we process event during render abort).
	//
	
	// but first clear any currently highlighted object
	SoGLRenderAction *glAct = p->sceneMgr->getGLRenderAction();
	if (glAct)
	    SoLocateHighlight::turnOffCurrentHighlight(glAct);
	
	SoState *state = p->sceneMgr->getHandleEventAction()->getState();
	if (state)
	    SoWindowElement::set(state, (Window) NULL, NULL, NULL, NULL);
	state = p->sceneMgr->getGLRenderAction()->getState();
	if (state && state->getDepth() == 1)
	    SoWindowElement::set(state, (Window) NULL, NULL, NULL, NULL);
    }
}

//
// called whenever the component becomes visible or not
//
void
SoXtRenderArea::visibilityChangeCB(void *pt, SbBool visible)
{
    SoXtRenderArea *p = (SoXtRenderArea *)pt;
    
    if (visible)
	p->activate();
    else {
	// we are becoming hidden - so wait for an expose event
	// (which we now will come later) before enabling a redraw to
	// prevent too many redraws (scene graph changes + expose)
	p->waitForExpose = TRUE;
	
	p->deactivate();
    }
}


void
SoXtRenderArea::selectionChangeCB(void *p, SoSelection *)
{
    ((SoXtRenderArea *)p)->scheduleRedraw();
}

void
SoXtRenderArea::overlaySelectionChangeCB(void *p, SoSelection *)
{
    ((SoXtRenderArea *)p)->scheduleOverlayRedraw();
}

void
SoXtRenderArea::renderCB(void *p, SoSceneManager *)
{
    ((SoXtRenderArea *)p)->redraw();
}

void
SoXtRenderArea::renderOverlayCB(void *p, SoSceneManager *)
{
    ((SoXtRenderArea *)p)->redrawOverlay();
}
