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
 |	This file contains the MyThumbWheel class which is a GL wheel 
 |  viewed on it's side. It is mostly used to control things like rotation
 |  where a thumb wheel is better than a regular slider.
 |  	
 |   Classes:
 |	MyThumbWheel
 |	
 |   Author(s)	: Alain Dumesny
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_XT_THUMB_WHEEL_
#define  _SO_XT_THUMB_WHEEL_

#include <Inventor/Xt/SoXtGLWidget.h>
#include <Inventor/misc/SoCallbackList.h>


class SoXtMouse;
class MyFloatCallbackList;

// callback function prototypes
typedef void MyThumbWheelCB(void *userData, float val);

//////////////////////////////////////////////////////////////////////////////
//
//  Class: MyThumbWheel
//
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=MyThumbWhl
class MyThumbWheel : public SoXtGLWidget {

  public:
    
    MyThumbWheel(SbBool horizontal = TRUE);
    MyThumbWheel(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE, 
	SbBool horizontal = TRUE);
    ~MyThumbWheel();
    
    //
    // Routines to specify the wheel value (a rotation given in radians) and get
    // the current wheel value.
    //
    // NOTE: setValue() will call valueChanged callbacks if the value differs.
    //
    void    	setValue(float radians);
    float   	getValue()			{ return value; }
    
    //
    // Those routines are used to register callbacks for the different thumb wheel
    // actions.
    //
    // NOTE: the start and finish callbacks are only to signal when the mouse
    // goes down and up. No valid callback data is passed (NULL passed).
    //
    // C-api: name=addStartCB
    void	addStartCallback(
			MyThumbWheelCB *f,
			void *userData = NULL);
    // C-api: name=addValueChangedCB
    void	addValueChangedCallback(
			MyThumbWheelCB *f,
			void *userData = NULL);
    // C-api: name=addFinishCB
    void	addFinishCallback(
			MyThumbWheelCB *f,
			void *userData = NULL);

    // C-api: name=removeStartCB
    void	removeStartCallback(
			MyThumbWheelCB *f,
			void *userData = NULL);
    // C-api: name=removeValueChangedCB
    void	removeValueChangedCallback(
			MyThumbWheelCB *f,
			void *userData = NULL);
    // C-api: name=removeFinishCB
    void	removeFinishCallback(
			MyThumbWheelCB *f,
			void *userData = NULL);

    // true while the value is changing interactively
    SbBool  isInteractive() 	    	    	{ return interactive; }
  
  protected:

    // This constructor takes a boolean whether to build the widget now.
    // Subclasses can pass FALSE, then call SoXtPushButton::buildWidget()
    // when they are ready for it to be built.
    SoEXTENDER
    MyThumbWheel(
	Widget parent,
	const char *name, 
	SbBool buildInsideParent,
	SbBool horizontal, 
	SbBool buildNow);

    Widget	    buildWidget(Widget parent);
    
  private:
    // redefine these to do thumb wheel specific things
    virtual void    redraw();
    virtual void    processEvent(XAnyEvent *anyevent);
    virtual void    sizeChanged(const SbVec2s &newSize);
    
    // local variables
    SbBool  	horizontal;
    float	value;
    int	    	lastPosition;
    SoXtMouse	*mouse;
    
    // callback variables
    MyFloatCallbackList *startCallbacks;
    MyFloatCallbackList *changedCallbacks;
    MyFloatCallbackList *finishCallbacks;
    SbBool  	interactive;

    // this is called by both constructors
    void constructorCommon(SbBool horizontal, SbBool buildNow);
};

#endif  /* _SO_XT_THUMB_WHEEL_ */
