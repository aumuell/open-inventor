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
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Description:
 |	This file contains the _SoXtColorEditor class which lets you 
 |  interactively edit colors using color sliders and a color wheel.
 |	You can either use it directly to edit a color field in the database
 |  by using the attach()/detach() routines, or use it by registering a callback
 |  routine which will get called whenever the color changed (passing an 
 |  SbColor to that routine).
 |	
 |   Author(s)	: Alain Dumesny
 |
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_XT_COLOR_EDITOR_
#define  _SO_XT_COLOR_EDITOR_

#include <Xm/Xm.h>
#include <Inventor/SbColor.h>
#include <Inventor/misc/SoCallbackList.h>
#include <Inventor/Xt/SoXtComponent.h>

class SoBase;
class SoNodeSensor;
class SoMFColor;
class SoPathList;
class SoSFColor;
class SoSensor;
class SoXtClipboard;
class _SoXtColorPatch;
class _SoXtColorWheel;
class _SoXtColorSlider;

struct _ColorEditorCBData;

// callback function prototypes
typedef void _SoXtColorEditorCB(void *userData, const SbColor *color);
					   
//////////////////////////////////////////////////////////////////////////////
//
//  Class: _SoXtColorEditor
//
//	_SoXtColorEditor class which lets you interactively edit a color.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoXtColEd
class _SoXtColorEditor : public SoXtComponent {
    
  public:
    
    //
    // list of possible slider combinations, which is used to specify
    // which sliders should be displayed at any time.
    //
    enum Sliders {
    	NONE, 
    	INTENSITY,  // default
	RGB, 
	HSV, 
	RGB_V, 
	RGB_HSV
    };
    
    // UpdateFrequency is how often new values should be sent
    // to the node or the callback routine.
    enum UpdateFrequency {
	CONTINUOUS,	// send updates with every mouse motion
	AFTER_ACCEPT	// only send updates after user hits accept button
    };
    
    // Constructor/Destructor
    _SoXtColorEditor(
	    Widget parent = NULL,
	    const char *name = NULL, 
	    SbBool buildInsideParent = TRUE);
    ~_SoXtColorEditor();
    
    //
    // Routines for attaching/detaching the editor to a color field. It uses a
    // sensor on the color field to automatically update itself when the 
    // color is changed externally.
    //
    // NOTE: the node containing the field needs to also be passed to attach 
    // the sensor to it (since field sensors are not yet suported).
    //
    // NOTE: it can only be attached to either a single field or a multiple 
    // field at any given time.
    //
    // C-api: name=attachSF
    void		attach(SoSFColor *color, SoBase *node);
    // C-api: name=attachMF
    void		attach(SoMFColor *color, int index, SoBase *node);
    void		detach();
    SbBool		isAttached() 	    	{ return attached; }
    
    //
    // Additional way of using the color editor, by registering a callback
    // and setting the color. At the time dictated by setUpdateFrequency
    // the callbacks will be called with the new color.
    //
    // NOTE: this is independent to the attach/detach routines, and 
    // therefore can be used in conjunction.
    //
    // C-api: name=addColChangedCB
    inline void		addColorChangedCallback(
				_SoXtColorEditorCB *f, 
				void *userData = NULL);
    // C-api: name=removeColChangedCB
    inline void		removeColorChangedCallback(
				_SoXtColorEditorCB *f, 
				void *userData = NULL);

    //
    // Sets/gets the color displayed by the color editor.
    //
    // NOTE: setColor() will call colorChanged callbacks if the color 
    // differs.
    //
    // C-api: name=setCol
    void		setColor(const SbColor &color);
    // C-api: name=getCol
    const SbColor &	getColor()	{ return baseRGB; }
    
    //
    // Sets/gets the WYSIWYG mode. (default OFF).
    //
    void	    	setWYSIWYG(SbBool trueOrFalse);
    SbBool  	    	isWYSIWYG() 	    	{ return WYSIWYGmode; }
    
    //
    // Sets/gets which slider should be displayed. (default INTENSITY)
    //
    // C-api: name=setCurSldrs
    void    	setCurrentSliders(_SoXtColorEditor::Sliders whichSliders);
    // C-api: name=getCurSldrs
    _SoXtColorEditor::Sliders getCurrentSliders() 	{ return whichSliders; }
    
    //
    // Set/get the update frequency of when colorChanged callbacks should 
    // be called. (default CONTINUOUS).
    //
    // C-api: name=setUpdateFreq
    void	setUpdateFrequency(_SoXtColorEditor::UpdateFrequency freq);
    // C-api: name=getUpdateFreq
    _SoXtColorEditor::UpdateFrequency getUpdateFrequency()
						    { return updateFreq; }
    
  protected:

    // This constructor takes a boolean whether to build the widget now.
    // Subclasses can pass FALSE, then call _SoXtColorEditor::buildWidget()
    // when they are ready for it to be built.
    SoEXTENDER
    _SoXtColorEditor(
	Widget parent,
	const char *name, 
	SbBool buildInsideParent, 
	SbBool buildNow);
    
    // redefine these
    virtual const char *    getDefaultWidgetName() const;
    virtual const char *    getDefaultTitle() const;
    virtual const char *    getDefaultIconTitle() const;
    
  private:
    
    // redefine these to do colorEditor specific things
    Widget		buildWidget(Widget parent);
    static void visibilityChangeCB(void *pt, SbBool visible);
    
    // local variables
    Widget		mgrWidget;	// form manages all child widgets
    SbBool		WYSIWYGmode;
    Sliders 		whichSliders;
    SbColor		baseRGB;
    float		baseHSV[3];
    SbBool  	    	ignoreCallback;
    _SoXtColorSlider	*sliders[6];
    _SoXtColorWheel	*wheel;
    _SoXtColorPatch	*current, *previous;
    _ColorEditorCBData *dataId;
    SbPList 	    	menuItems;  	// Widgets
    _SoXtColorEditor::UpdateFrequency	updateFreq;
    
    // attach/detach variables
    SbBool		attached;
    SoBase		*editNode;
    SoSFColor		*colorSF;
    SoMFColor		*colorMF;
    SoNodeSensor	*colorSensor;
    SoCallbackList	*callbackList;
    int			index;
    
    // copy/paste support
    SoXtClipboard	*clipboard;
    void    	    	copy(Time eventTime);
    void    	    	paste(Time eventTime);
    void    	    	pasteDone(SoPathList *pathList);
    static void    	pasteDoneCB(void *userData, SoPathList *pathList);
    
    // list of widgets which need to be accessed
    Widget	acceptButton, slidersForm, buttonsForm, wheelForm;
    
    // build/destroy routines
    Widget   	    	buildPulldownMenu(Widget parent);
    Widget  	    	buildControls(Widget parent);
    Widget  	    	buildSlidersForm(Widget parent);
    
    void    	    	doSliderLayout();
    void    	    	doDynamicTopLevelLayout();
    int	    	    	numberOfSliders(_SoXtColorEditor::Sliders slider);

    
    // do the updates - if attached, update the node; if callback, call it.
    void	doUpdates();
    
    // color field sensor callback and routine
    void    	fieldChanged();
    static void fieldChangedCB(void *, SoSensor *);
    
    // callbacks and actual routine from sliders, wheel, buttons, menu...
    static void wheelCallback(void *, const float hsv[3]);
    void    	wheelChanged(const float hsv[3]);
    static void sliderCallback(void *, float);
    void    	sliderChanged(short id, float value);
    static void buttonsCallback(Widget, _ColorEditorCBData *, XtPointer);
    void    	buttonPressed(short id);
    static void editMenuCallback(Widget, _ColorEditorCBData *, XmAnyCallbackStruct *);
    static void sliderMenuCallback(Widget, _ColorEditorCBData *, XtPointer);
    
    static void menuDisplay(Widget, _SoXtColorEditor *editor, XtPointer);

    // this is called by both constructors
    void constructorCommon(SbBool buildNow);
};

// Inline functions
void
_SoXtColorEditor::addColorChangedCallback(
    _SoXtColorEditorCB *f, 
    void *userData)
{ callbackList->addCallback((SoCallbackListCB *) f, userData); }

void
_SoXtColorEditor::removeColorChangedCallback(
    _SoXtColorEditorCB *f, 
    void *userData)
{ callbackList->removeCallback((SoCallbackListCB *) f, userData); }


#endif  /* _SO_XT_COLOR_EDITOR_ */
