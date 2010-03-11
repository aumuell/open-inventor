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
 * Copyright (C) 1990-93   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Classes	: SoXtPlaneViewer
 |
 |   Author(s)	: Alain Dumesny
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_XT_PLANE_VIEWER_
#define  _SO_XT_PLANE_VIEWER_

#include <Inventor/Xt/viewers/SoXtFullViewer.h>
#include <Inventor/SbLinear.h>

class SoXtBitmapButton;


//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoXtPlaneViewer
//
//  Plane Viewer - allows the user to move the camera in a plane, zoom in
//	and out, as well as rotate the viewing plane.
//
//	Keys used by this viewer:
//	-------------------------
//
//     Left Mouse: Dolly in and out (gets closer to and further away from the
//     object).
//
//     Middle Mouse: 
//     Ctrl + Left Mouse: Translate up, down, left and right.
//
//     Ctrl + Middle Mouse: Used for roll action (rotates around the viewer
//     forward direction).
//
//     <s> + click: Alternative to the Seek button. Press (but do not hold down)
//     the <s> key, then click on a target object.
//
//     Right Mouse: Open the popup menu.
//  
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoXtPlnVwr
class SoXtPlaneViewer : public SoXtFullViewer {
 public:
    SoXtPlaneViewer(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE, 
	SoXtFullViewer::BuildFlag flag = BUILD_ALL, 
	SoXtViewer::Type type = BROWSER);
    // C-api: interpret #define SoXtPlnVwrCreateStd(parent,name) SoXtPlnVwrCreate(parent,name,TRUE,SO_XT_FULL_VWR_BUILD_ALL,SO_XT_VWR_BROWSER)
    ~SoXtPlaneViewer();
    
    //
    // redefine these to add Plane viewer functionality
    //
    virtual void	setViewing(SbBool onOrOff);
    virtual void	setCamera(SoCamera *cam);
    virtual void	setCursorEnabled(SbBool onOrOff);
    
 protected:
  
    // This constructor takes a boolean whether to build the widget now.
    // Subclasses can pass FALSE, then call SoXtPlaneViewer::buildWidget()
    // when they are ready for it to be built.
    SoEXTENDER
    SoXtPlaneViewer(
	Widget parent,
	const char *name, 
	SbBool buildInsideParent, 
	SoXtFullViewer::BuildFlag flag, 
	SoXtViewer::Type type, 
	SbBool buildNow);

    Widget		buildWidget(Widget parent);
        
    // redefine these
    virtual const char *    getDefaultWidgetName() const;
    virtual const char *    getDefaultTitle() const;
    virtual const char *    getDefaultIconTitle() const;
    
    // redefine those routines to do viewer specific stuff
    virtual void	processEvent(XAnyEvent *anyevent);
    virtual void	setSeekMode(SbBool onOrOff);
    virtual void	actualRedraw();
    
    // Define those thumb wheels to translate in the viewer plane
    virtual void    	bottomWheelMotion(float newVal);
    virtual void    	leftWheelMotion(float newVal);
    virtual void    	rightWheelMotion(float newVal);
    
    // add viewer preference stuff
    virtual void	createPrefSheet();
    
    // add some viewer buttons
    virtual void	createViewerButtons(Widget parent);
    
    // Define this to bring the viewer help card
    virtual void	openViewerHelpCard();
    
    // Redefine this to keep the same camera rotation when seeking
    virtual void	computeSeekFinalOrientation();

 private:
    // viewer state variables
    int		    mode;
    SbBool	    createdCursors;
    Cursor	    transCursor, dollyCursor, seekCursor;
    SbVec2s	    locator; // mouse position
    
    // camera translation vars
    SbVec3f	    locator3D;
    SbPlane	    focalplane;
    float	    transXspeed, transYspeed;
    
    // push button vars and callbacks
    SoXtBitmapButton  *buttonList[10];
    static void	    pushButtonCB(Widget, int id, void *);
    
    void	    updateViewerMode(unsigned int state);
    void	    switchMode(int newMode);
    void	    updateCursor();
    void	    defineCursors();
    void	    rollCamera(const SbVec2s &newLocator);
    void	    translateCamera(const SbVec2f &newLocator);
    void	    dollyCamera(const SbVec2s &newLocator);
    void	    setPlane(const SbVec3f &newNormal, const SbVec3f &newRight);
    
    // redefine these to also call computeTranslateValues()
    virtual void    bottomWheelStart();
    virtual void    leftWheelStart();
    void	    computeTranslateValues();
    
    // this is called by both constructors
    void constructorCommon(SbBool buildNow);
};

#endif  /* _SO_XT_PLANE_VIEWER_ */
