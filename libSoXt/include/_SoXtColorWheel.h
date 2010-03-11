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
 |   Description:
 |	This file contains The _SoXtColorWheel class, which lets you 
 |  interactively select colors using a color wheel. 
 |
 |   Author(s)	: Alain Dumesny
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_XT_COLOR_WHEEL_
#define  _SO_XT_COLOR_WHEEL_

#include <Inventor/SbColor.h>
#include <Inventor/SbLinear.h>
#include <Inventor/misc/SoCallbackList.h>
#include <Inventor/Xt/SoXtGLWidget.h>

class SoXtMouse;

// callback function prototypes
typedef void _SoXtColorWheelCB(void *userData, const float hsv[3]);
					   
//////////////////////////////////////////////////////////////////////////////
//
//  Class: _SoXtColorWheel
//
//	Lets you interactively select colors using a color wheel. User register
//  callback(s) to be notified when a new color has been selected. There is
//  also a call to tell the color wheel what the current color is when it is
//  changed externally.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoXtColWhl
class _SoXtColorWheel : public SoXtGLWidget {

  public:
    
    _SoXtColorWheel(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE);
    ~_SoXtColorWheel();
    
    //
    // Routine to tell the color wheel what the current HSV color is.
    //
    // NOTE: if calling setBaseColor() changes the marker position the
    // valueChanged callbacks will be called with the new hsv color.
    //
    // C-api: name=setBaseCol
    void    	setBaseColor(const float hsv[3]);
    // C-api: name=getBaseCol
    const float *getBaseColor()     	{ return hsvColor; }
    
    //
    // This routine sets the WYSIWYG (What You See Is What You Get) mode.
    // When WYSIWYG is on the colors on the wheel will reflect the current
    // color intensity (i.e. get darker and brighter)
    //
    void	setWYSIWYG(SbBool trueOrFalse);    // default FALSE
    SbBool  	isWYSIWYG() 	    	{ return WYSIWYGmode; }
    
    //
    // Those routines are used to register callbacks for the different 
    // color wheel actions.
    //
    // NOTE: the start and finish callbacks are only to signal when the mouse
    // goes down and up. No valid callback data is passed (NULL passed).
    //
    // C-api: name=addStartCB
    void    addStartCallback(
		    _SoXtColorWheelCB *f,
		    void *userData = NULL)
	    { startCallbacks->addCallback((SoCallbackListCB *) f, userData); }

    // C-api: name=addValueChangedCB
    void    addValueChangedCallback(
		    _SoXtColorWheelCB *f, 
		    void *userData = NULL)
	    { changedCallbacks->addCallback((SoCallbackListCB *) f, userData); }
	
    // C-api: name=addFinishCB
    void    addFinishCallback(
		    _SoXtColorWheelCB *f,
		    void *userData = NULL)
	    { finishCallbacks->addCallback((SoCallbackListCB *) f, userData); }
				       
    // C-api: name=removeStartCB
    void    removeStartCallback(
		    _SoXtColorWheelCB *f,
		    void *userData = NULL)
	    { startCallbacks->removeCallback((SoCallbackListCB *) f, userData); }

    // C-api: name=removeValueChangedCB
    void    removeValueChangedCallback(
		    _SoXtColorWheelCB *f,
		    void *userData = NULL)
	    { changedCallbacks->removeCallback((SoCallbackListCB *) f, userData); }

    // C-api: name=removeFinishCB
    void    removeFinishCallback(
		    _SoXtColorWheelCB *f,
		    void *userData = NULL)
	    { finishCallbacks->removeCallback((SoCallbackListCB *) f, userData); }
	
	
    // true while the color is changing interactively
    SbBool  isInteractive() 	    	    	{ return interactive; }

  protected:  

    // This constructor takes a boolean whether to build the widget now.
    // Subclasses can pass FALSE, then call _SoXtColorWheel::buildWidget()
    // when they are ready for it to be built.
    SoEXTENDER
    _SoXtColorWheel(
	Widget parent,
	const char *name, 
	SbBool buildInsideParent, 
	SbBool buildNow);

    Widget	    buildWidget(Widget parent);

  private:
    
    // redefine these to do color wheel specific things
    virtual void    redraw();
    virtual void    redrawOverlay();
    virtual void    processEvent(XAnyEvent *anyevent);
    virtual void    initOverlayGraphic();
    virtual void    sizeChanged(const SbVec2s &newSize);
    
    // color wheels local variables
    SbBool	WYSIWYGmode;
    SbBool	blackMarker;
    float	hsvColor[3];
    short	cx, cy, radius;
    SbColor	*defaultColors, *colors;
    SbVec2f	*geometry;
    SoXtMouse	*mouse;
    
    // callback variables
    SoCallbackList *startCallbacks;
    SoCallbackList *changedCallbacks;
    SoCallbackList *finishCallbacks;
    SbBool  	interactive;
    
    // routines to make the wheel geometry, colors, draw it....
    void    makeWheelGeometry();
    void    makeWheelColors(SbColor *col, float intensity);
    void    drawWheelSurrounding();
    void    drawWheelColors();
    void    checkMarkerColor();
    void    drawWheelMarker();
    void    moveWheelMarker(short x, short y);

    // this is called by both constructors
    void constructorCommon(SbBool buildNow);
};


#endif  /* _SO_XT_COLOR_WHEEL_ */
