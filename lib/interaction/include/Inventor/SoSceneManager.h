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

//
// Copyright (C) 1990-93   Silicon Graphics, Inc.
//
//______________________________________________________________________
//_____________  S I L I C O N   G R A P H I C S   I N C .  ____________
//
//   $Revision: 1.1 $
//
//   Description:
//	SoSceneManager manages rendering and event handling operations
//      on a scene graph. 
//
//   Author(s): David Mott, Alain Dumesny
//
//_____________  S I L I C O N   G R A P H I C S   I N C .  ____________
//______________________________________________________________________
//

#ifndef _SO_SCENE_MANAGER_
#define _SO_SCENE_MANAGER_

#include <Inventor/SbColor.h>
#include <Inventor/actions/SoGLRenderAction.h>

class SoNodeSensor;
class SoEvent;
class SoHandleEventAction;
class SoNode;
class SoSceneManager;
class SoSensor;
class SoSFTime;
class SoOneShotSensor;

// callback function prototypes
typedef void SoSceneManagerRenderCB(void *userData, SoSceneManager *mgr);

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoSceneManager
//
//  Class which performs Inventor rendering and event handling
//  on a scene graph.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoSceneMgr
class SoSceneManager {
  public:
    // Constructor. 
    SoSceneManager();
    // Destructor.
    ~SoSceneManager();
    
    // Render and process events.
    // reinitialize is called to initialize graphics, i.e. for a new window.
    // NOTE: a window MUST be set before calling render() and processEvent().
    // The window size must also be set (call setWindowSize()).
    // C-api: expose
    virtual void	render(SbBool clearWindow = TRUE, SbBool clearZbuffer = TRUE);
    // C-api: expose
    // C-api: name=processEv
    virtual SbBool	processEvent(const SoEvent *event);
    // C-api: name=reinit
    void		reinitialize();
    
    // Schedule a redraw for some time in the near future.
    // If there is no render callback set, or this is not active,
    // no redraw will be scheduled.
    void		scheduleRedraw();
    
    //
    // Set/get the scene graph to render.
    //
    // C-api: expose
    // C-api: name=setScene
    virtual void	setSceneGraph(SoNode *newScene);
    // C-api: expose
    // C-api: name=getScene
    virtual SoNode *	getSceneGraph() const;
    
    // Set/get the size of the window in which scene mgr should render.
    // This size must be set before render() and processEvent() are called.
    // C-api: name=setWinSize
    void		setWindowSize(const SbVec2s &newSize);
    // C-api: name=getWinSize
    const SbVec2s &	getWindowSize() const;
    
    // Set/get the size and origin of the viewport within the window.
    // Default is to render the entire window region, that is, 
    // origin = (0,0), size = getWindowSize().
    void		setSize(const SbVec2s &newSize);
    const SbVec2s &	getSize() const;
    void		setOrigin(const SbVec2s &newOrigin);    
    const SbVec2s &	getOrigin() const;
    
    // Sets/gets current viewport region to use for rendering.
    // This can be used instead of setting the size and origin separately.
    // C-api: name=setVPReg
    void	setViewportRegion(const SbViewportRegion &newRegion);
    // C-api: name=getVPReg
    const SbViewportRegion &getViewportRegion() const;

    //
    // Sets/gets the window background color when in RGB mode.
    // (default to black (0,0,0));
    //
    // C-api: name=setBkgCol
    void		setBackgroundColor(const SbColor &c);
    // C-api: name=getBkgCol
    const SbColor &	getBackgroundColor() const  { return bkgColor; }
    
    //
    // Sets/gets the window background color when in color index mode.
    // (default to black (index 0)).
    //
    // C-api: name=setBkgInd
    void		setBackgroundIndex(int index);
    // C-api: name=getBkgInd
    int			getBackgroundIndex() const  { return bkgIndex; }
    
    //
    // Set/get the color mode (TRUE - rgb mode, FALSE - color map mode)
    // Default is TRUE - rgb mode.
    //
    void		setRGBMode(SbBool onOrOff);
    SbBool		isRGBMode() const	    { return rgbMode; }
    
    // The scene manager will only employ sensors while it is active.
    // C-api: expose
    virtual void	activate();
    // C-api: expose
    virtual void	deactivate();

    // Render callback - the scene manager employs a sensor to redraw
    // the scene when something changes. This callback is invoked for that
    // to happen. The callback should set up its graphics window, then
    // call the scene manager render() method. If the callback is set to NULL
    // (the default), auto-redraw is turned off.
    // C-api: name=setRenderCB
    void		setRenderCallback(
			    SoSceneManagerRenderCB *f,
			    void *userData = NULL);
    SbBool		isAutoRedraw() const { return (renderCB != NULL); }
    
    // Sets/returns the priority of the redraw sensor.
    void		setRedrawPriority(uint32_t priority);
    uint32_t	getRedrawPriority() const;

    // Returns the default priority of the redraw sensor
    static uint32_t getDefaultRedrawPriority() { return 10000; }
    
    // Enable/Disable the realTime global field update which normally
    // happen right after a redraw.
    // C-api: name=enableRTUpdate
    static void		enableRealTimeUpdate(SbBool flag);
    // C-api: name=isRTUpdate
    static SbBool	isRealTimeUpdateEnabled()	{ return updateRealTime; }
    
    //
    // Anti-aliasing methods.
    //
    void    	    setAntialiasing(SbBool smoothing, int numPasses);
    void    	    getAntialiasing(SbBool &smoothing, int &numPasses) const;
    
  SoEXTENDER public:
    // User supplied render action. Highlights fall into this category.
    // SceneManager will never delete a render action passed to this method.
    void		setGLRenderAction(SoGLRenderAction *ra);
    SoGLRenderAction	*getGLRenderAction() const { return renderAction; }

    // User supplied handle event action. This should not be done in
    // the middle of event handling. Passing NULL turns event handling off.
    // SceneManager will never delete a handle event action passed to
    // this method.
    void		setHandleEventAction(SoHandleEventAction *hea);
    SoHandleEventAction	*getHandleEventAction() const { return handleEventAction; }

  protected:
    SoSceneManagerRenderCB  *renderCB;
    void		    *renderCBData;
    SbBool		isActive() const { return active; }
    
    // This tells the app it is time to redraw by invoking the renderCB.
    void		redraw();
    
  private:
    SbBool		raCreatedHere;
    SbBool		heaCreatedHere;
    SoGLRenderAction	*renderAction;
    SoHandleEventAction	*handleEventAction;
    SoNode		*scene;
    SoNodeSensor	*sceneSensor;	// attached to root for auto-redraw
    uint32_t	redrawPriority;	// Priority of sceneSensor
    SbBool		active;		// set TRUE if we are active
    SbBool		needZbuffer;
    SbBool		needToSendVP; // TRUE when glViewport() needs to be called
    
    SbColor		bkgColor;
    int			bkgIndex;
    SbBool		graphicsInitNeeded;
    SbBool		rgbMode;
    
    // static callbacks
    static void	    	antialiasingCallback(void *);
    static void		sceneSensorCallback(SoSceneManager *, SoSensor *);
    
    // realTime field update vars and callbacks
    static SbBool	updateRealTime;
    static SoSFTime	*realTime;
    static SoOneShotSensor *realTimeSensor; // touches realTime global field
    static void		realTimeSensorCB();
};


#endif // _SO_SCENE_MANAGER_
