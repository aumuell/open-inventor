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
 * Copyright (C) 1990,91,92   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Classes:
 |	SoSceneManager
 |
 |   Author(s): David Mott, Alain Dumesny
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoDB.h>
#include <Inventor/SbTime.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SbViewportRegion.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/SoSceneManager.h>
#include <Inventor/fields/SoSFTime.h>
#include <Inventor/sensors/SoSensor.h>
#include <Inventor/sensors/SoNodeSensor.h>
#include <Inventor/sensors/SoOneShotSensor.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoHandleEventAction.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>

#include <SoDebug.h>

// Sensor which touches realTime as soon as possible after a redraw
SoSFTime	*SoSceneManager::realTime = NULL;
SoOneShotSensor	*SoSceneManager::realTimeSensor = NULL;
SbBool		SoSceneManager::updateRealTime = TRUE;


////////////////////////////////////////////////////////////////////////
//
// Constructor 
//
SoSceneManager::SoSceneManager()
//
////////////////////////////////////////////////////////////////////////
{
    bkgColor.setValue(0,0,0);
    bkgIndex = 0;
    rgbMode = TRUE;
    graphicsInitNeeded = TRUE;
    
    // inventor specific variables
    scene   	    = NULL;
    raCreatedHere   = TRUE;
    renderAction    = new SoGLRenderAction(SbVec2s(1,1));
    heaCreatedHere  = TRUE;
    handleEventAction = new SoHandleEventAction(SbVec2s(1,1));
    renderCB	    = NULL;
    renderCBData    = NULL;
    active	    = FALSE;
    needToSendVP    = TRUE;
    
    // create the scene sensor (used for automatic rendering)
    // do not attach the sensor here. That's done when we
    // become active, and the user supplies a redraw callback.
    sceneSensor = new SoNodeSensor;
    sceneSensor->setData((void *) this);
#ifdef DEBUG
    if (SoDebug::GetEnv("IV_DEBUG_SENSORS")) {
	SoDebug::NamePtr("sceneChangeSensor", sceneSensor);
    }
#endif

    // Assume default priority now. We'll set the sensor priority
    // before we attach, just in case the user changes the priority
    // before that occurs.
    setRedrawPriority(getDefaultRedrawPriority());
    
    // setup the sensor to touch real time after a redraw
    if (! realTimeSensor) {
	realTime = (SoSFTime *) SoDB::getGlobalField("realTime");
	realTimeSensor = new SoOneShotSensor;
	realTimeSensor->setFunction((SoSensorCB *)
				 &SoSceneManager::realTimeSensorCB);
#ifdef DEBUG
	if (SoDebug::GetEnv("IV_DEBUG_SENSORS")) {
	    SoDebug::NamePtr("realTimeSensor", realTimeSensor);
	}
#endif
    }
}

////////////////////////////////////////////////////////////////////////
//
// Destructor
//
SoSceneManager::~SoSceneManager()
//
////////////////////////////////////////////////////////////////////////
{
    // delete actions
    if (raCreatedHere)
	delete renderAction;
	
    if (heaCreatedHere)
	delete handleEventAction;
    
    // detach the scene
    setSceneGraph(NULL);
    delete sceneSensor;
}

////////////////////////////////////////////////////////////////////////
//
// Set a new render action, for instance for highlighting.
//
// public
//
void
SoSceneManager::setGLRenderAction(SoGLRenderAction *ra)
//
////////////////////////////////////////////////////////////////////////
{ 
#ifdef DEBUG
    if (ra == NULL)
	SoDebugError::post("SoSceneManager::setGLRenderAction",
			"a NULL render action was passed. This is bad!");
#endif

    // Make sure the viewport region is set
    // (be paranoid and check for NULL)
    if (renderAction != NULL) {
	SbViewportRegion rgn(renderAction->getViewportRegion());
	ra->setViewportRegion(rgn);
    }
    
    // Get rid of the old render action if it was created here.
    if (raCreatedHere) {
	delete renderAction;
	raCreatedHere = FALSE;
    }
    
    // Set to the new render action.
    renderAction = ra;
}

////////////////////////////////////////////////////////////////////////
//
// Set a new handle event action. Passing NULL will shut off event
// handling.
//
// public
//
void
SoSceneManager::setHandleEventAction(SoHandleEventAction *hea)
//
////////////////////////////////////////////////////////////////////////
{
    // Make sure the viewport region is set
    if ((handleEventAction != NULL) && (hea != NULL)) {
	SbViewportRegion rgn(handleEventAction->getViewportRegion());
	hea->setViewportRegion(rgn);
    }
    
    // Get rid of the old render action if it was created here.
    if (heaCreatedHere) {
	delete handleEventAction;
	heaCreatedHere = FALSE;
    }
    
    // Set to the new render action.
    handleEventAction = hea;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  	Make the new user supplied scene graph the rendering root.
//
// use: virtual public
//
void
SoSceneManager::setSceneGraph(SoNode *newScene)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool currentlyActive = isActive();
    deactivate();
    
    // ref the new scene
    if (newScene != NULL)
	newScene->ref();

    // check if there already is a scene graph
    if (scene != NULL)
	scene->unref();

    // now set the new scene graph
    scene = newScene;
    
    if (currentlyActive)
	activate();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  	Return the user supplied scene graph.
//
// use: virtual public
//
SoNode *
SoSceneManager::getSceneGraph() const
//
////////////////////////////////////////////////////////////////////////
{ return scene; }
	    

////////////////////////////////////////////////////////////////////////
//
// Description:
//  	Process the passed X event.
//
// use: virtual protected
//
SbBool
SoSceneManager::processEvent(const SoEvent *event)
//
////////////////////////////////////////////////////////////////////////
{
    if ((scene != NULL) && (handleEventAction != NULL)) {    
	handleEventAction->setEvent(event);
	handleEventAction->apply(scene);
	
	return handleEventAction->isHandled();
    }
    else
	return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Initialize the window for rendering.
// A window MUST be set before this is called.
//
// use: public
//
void
SoSceneManager::reinitialize()
//
////////////////////////////////////////////////////////////////////////
{
    graphicsInitNeeded = TRUE;
    
    // we have a new window, so re-init the render action
    renderAction->invalidateState();
}


////////////////////////////////////////////////////////////////////////
//
//  Set the size of the window.
//
//  use: public
//
void
SoSceneManager::setWindowSize(const SbVec2s &newSize)
//
////////////////////////////////////////////////////////////////////////
{
    SbViewportRegion rgn(renderAction->getViewportRegion());
    rgn.setWindowSize(newSize);
    
    if (renderAction != NULL)
	renderAction->setViewportRegion(rgn);
	
    if (handleEventAction != NULL)
	handleEventAction->setViewportRegion(rgn);
    
    // make sure to call glViewport() with the new size
    needToSendVP = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
//  Set the size of the window.
//
//  use: public
//
const SbVec2s &
SoSceneManager::getWindowSize() const
//
////////////////////////////////////////////////////////////////////////
{
    return renderAction->getViewportRegion().getWindowSize();
}

////////////////////////////////////////////////////////////////////////
//
//  Set the size of the viewport within the window.
//
//  use: public
//
void
SoSceneManager::setSize(const SbVec2s &newSize)
//
////////////////////////////////////////////////////////////////////////
{
    SbViewportRegion rgn(renderAction->getViewportRegion());
    const SbVec2s &origin = rgn.getViewportOriginPixels();
    rgn.setViewportPixels(origin, newSize);
           
    if (renderAction != NULL)
	renderAction->setViewportRegion(rgn);
    
    if (handleEventAction != NULL)
	handleEventAction->setViewportRegion(rgn);
}

////////////////////////////////////////////////////////////////////////
//
//  Set origin relative to the window.
//
//  use: public
//
void
SoSceneManager::setOrigin(const SbVec2s &newOrigin)
//
////////////////////////////////////////////////////////////////////////
{
    SbViewportRegion rgn(renderAction->getViewportRegion());
    const SbVec2s &size = rgn.getViewportSizePixels();
    rgn.setViewportPixels(newOrigin, size);
           
    if (renderAction != NULL)
	renderAction->setViewportRegion(rgn);
    
    if (handleEventAction != NULL)
	handleEventAction->setViewportRegion(rgn);
}

////////////////////////////////////////////////////////////////////////
//
//  Get viewport size.
//
//  use: public
//
const SbVec2s &
SoSceneManager::getSize() const
//
////////////////////////////////////////////////////////////////////////
{
    return renderAction->getViewportRegion().getViewportSizePixels();
}

////////////////////////////////////////////////////////////////////////
//
//  Get origin relative to the window.
//
//  use: public
//
const SbVec2s &
SoSceneManager::getOrigin() const
//
////////////////////////////////////////////////////////////////////////
{
    return renderAction->getViewportRegion().getViewportOriginPixels();
}

////////////////////////////////////////////////////////////////////////
//
//  Set the viewport region instead of setting size and origin separately.
//
//  use: public
//
void
SoSceneManager::setViewportRegion(const SbViewportRegion &rgn)
//
////////////////////////////////////////////////////////////////////////
{
    if (renderAction != NULL)
	renderAction->setViewportRegion(rgn);
    
    if (handleEventAction != NULL)
	handleEventAction->setViewportRegion(rgn);
}

////////////////////////////////////////////////////////////////////////
//
//  Get the viewport region.
//
//  use: public
//
const SbViewportRegion &
SoSceneManager::getViewportRegion() const
//
////////////////////////////////////////////////////////////////////////
{
    return renderAction->getViewportRegion();
}

////////////////////////////////////////////////////////////////////////
//
//  Set to RGB mode or color map mode.
//
//  use: public
//
void
SoSceneManager::setRGBMode(SbBool onOrOff)
//
////////////////////////////////////////////////////////////////////////
{
    rgbMode = onOrOff;

    // the render action determines color map/rgb at initialization
//???pauli    renderAction->reinitialize();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Sets antialiasing on GL rendering action.
//
// use: public
//
void
SoSceneManager::setAntialiasing(SbBool smoothing, int numPasses)
//
////////////////////////////////////////////////////////////////////////
{
    renderAction->setSmoothing( smoothing );
    renderAction->setNumPasses( numPasses );
    
    // Set render action callback for multiple pass rendering.  This
    // makes sure to clear background to correct color between frames.
    if ( numPasses > 1 )
	renderAction->setPassCallback(antialiasingCallback, this);
    else
	renderAction->setPassCallback(NULL, NULL);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Gets antialiasing on GL rendering action.
//
// use: public
//
void
SoSceneManager::getAntialiasing(SbBool &smoothing, int &numPasses) const
//
////////////////////////////////////////////////////////////////////////
{
    smoothing = renderAction->isSmoothing();
    numPasses = renderAction->getNumPasses();
}

////////////////////////////////////////////////////////////////////////
//
// This routine is called to render the scene graph.
// A window MUST be set before this is called.
//
// use: public
//
void
SoSceneManager::render(SbBool clearWindow, SbBool clearZbuffer)
//
////////////////////////////////////////////////////////////////////////
{
    // reinitialize if necessary
    if (graphicsInitNeeded) {
	int numBits[1];
	glGetIntegerv(GL_DEPTH_BITS, numBits);
	needZbuffer = (numBits[0] != 0); // FALSE for overlay windows !
	if (needZbuffer)
	    glDepthFunc(GL_LEQUAL); // needed for hidden line rendering
	graphicsInitNeeded = FALSE;
    }
    
    //
    // when the window changes size, we need to call glViewport() before
    // we can do a color clear.
    //
    if (needToSendVP) {
	const SbViewportRegion &theRegion = renderAction->getViewportRegion();
	SbVec2s size   = theRegion.getViewportSizePixels();
	SbVec2s origin = theRegion.getViewportOriginPixels();
	glViewport(origin[0], origin[1], size[0], size[1]);
	needToSendVP = FALSE;
    }
    
    //
    // clear to the background color and clear the zbuffer
    //
    if (clearWindow) {
	if (rgbMode)
	     glClearColor(bkgColor[0], bkgColor[1], bkgColor[2], 0);
	else glClearIndex(bkgIndex);
	
	// clear the color+zbuffer at the same time if we can
	if (needZbuffer && clearZbuffer)
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	else
	    glClear(GL_COLOR_BUFFER_BIT);
    }
    // check to see if only the zbuffer is needed...
    else if (needZbuffer && clearZbuffer)
	glClear(GL_DEPTH_BUFFER_BIT);
    
    // render the scene graph!
    if (scene != NULL)
    	renderAction->apply(scene);
    
    // sensor doesn't need to fire again if it's still scheduled
    sceneSensor->unschedule();
    
    // schedule the realTime One shot sensor to update the real time
    // as soon as we can now that we have rendered the scene. This will
    // enable us to render things that are animating with a consistent
    // time across mutiple renderAreas, while providing the maximum
    // optainable frame rate (much better than a hard coded 30 or 60 
    // times/sec timer sensor).
    if (updateRealTime)
	realTimeSensor->schedule();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Callback to update the realTime global field.
//
// Use: static private

void
SoSceneManager::realTimeSensorCB()
//
////////////////////////////////////////////////////////////////////////
{
    realTime->setValue(SbTime::getTimeOfDay());
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Enables/Disable the realTime update after each redraw
//
// Use: static public

void
SoSceneManager::enableRealTimeUpdate(SbBool flag)
//
////////////////////////////////////////////////////////////////////////
{
    if (updateRealTime == flag)
	return;
    
    updateRealTime = flag;
    
    if (updateRealTime)
	realTimeSensor->schedule();
    else
	realTimeSensor->unschedule();
}

////////////////////////////////////////////////////////////////////////
//
//  The scene manager will only employ sensors while it is active.
//
//  use: public, virtual
//
void
SoSceneManager::activate()
{
    // attach sceneSensor to top node for redrawing purpose
    // only if the user has specified a redraw callback (i.e.
    // auto-redraw is enabled)
    if (renderCB != NULL) {
	if (scene != NULL && sceneSensor->getAttachedNode() == NULL) {
	    sceneSensor->setFunction((SoSensorCB *)
			 &SoSceneManager::sceneSensorCallback);
	    sceneSensor->attach(scene);
	}
    }
    active = TRUE;
}
void
SoSceneManager::deactivate()
{
    sceneSensor->detach();
    sceneSensor->setFunction(NULL);
    active = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
//  Set the rendering callback. The app can supply this to enable
//  automatic redrawing of the scene. This callback should perform
//  window specific graphics initialization, then call the scene manager
//  render() method.
//
//  use: public
//
void
SoSceneManager::setRenderCallback(
    SoSceneManagerRenderCB *f,
    void *userData)
//
////////////////////////////////////////////////////////////////////////
{
    renderCB = f;
    renderCBData = userData;
    
    if (f != NULL) {
	// if we are active, attach the scene sensor
	if (active) {
	    if (scene != NULL && sceneSensor->getAttachedNode() == NULL) {
		sceneSensor->setFunction((SoSensorCB *)
			     &SoSceneManager::sceneSensorCallback);
		sceneSensor->attach(scene);
	    }
	}
    }
    else {
	// detach the scene sensor (whether active or not)
	sceneSensor->detach();
	sceneSensor->setFunction(NULL);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Priority of the redraw sensor
//
// public 
//
void
SoSceneManager::setRedrawPriority(uint32_t priority)
//
////////////////////////////////////////////////////////////////////////
{
    sceneSensor->setPriority(priority);
}

////////////////////////////////////////////////////////////////////////
//
// Priority of the redraw sensor
//
// public 
//
uint32_t
SoSceneManager::getRedrawPriority() const
//
////////////////////////////////////////////////////////////////////////
{
    return sceneSensor->getPriority();
}


//
////////////////////////////////////////////////////////////////////////
// static callbacks stubs
////////////////////////////////////////////////////////////////////////
//

void
SoSceneManager::antialiasingCallback( void *r )
{
    SoSceneManager *mgr = (SoSceneManager *) r;
    
    if (mgr->rgbMode)
	 glClearColor(mgr->bkgColor[0], mgr->bkgColor[1], mgr->bkgColor[2], 0);
    else glClearIndex( mgr->bkgIndex );
    
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void
SoSceneManager::sceneSensorCallback(SoSceneManager *mgr, SoSensor *)
{
#ifdef DEBUG
    if (mgr->renderCB == NULL) {
	SoDebugError::post("SoSceneManager::sceneSensorCallback",
	"Callback was fired,  but auto-redraw is turned off.");
	return;
    }
    if (! mgr->isActive()) {
	SoDebugError::post("SoSceneManager::sceneSensorCallback",
	"Callback was fired,  but scene manager not active.");
	return;
    }
#endif

    // redraw that scene graph!
    mgr->redraw(); 
}

//
////////////////////////////////////////////////////////////////////////
// real short methods
////////////////////////////////////////////////////////////////////////
//

void
SoSceneManager::redraw()
{
    if (renderCB != NULL)
	(*renderCB)(renderCBData, this);
}

void
SoSceneManager::scheduleRedraw()
{
    // The sceneSensor will only schedule itself if a CB func is set.
    // We only set the sensor CB func if autoRedraw is on and we are active.
    // Thus, there are no flags to check here. Just call schedule, and
    // the sensor will do the right thing.
    sceneSensor->schedule();
}

void
SoSceneManager::setBackgroundColor(const SbColor &c)
{
    bkgColor = c;
    if (isRGBMode())
	scheduleRedraw();
}

void
SoSceneManager::setBackgroundIndex(int index)
{
    bkgIndex = index;
    if (! isRGBMode())
	scheduleRedraw();
}
