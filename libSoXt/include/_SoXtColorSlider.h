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
 * Copyright (C) 1990,91,92   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Description:
 |	This file contains the _SoXtColorSlider class which is an _SoXtSlider
 |  (GL slider within an GLX widget) with build in policies. You can only
 |  build a set of pre-defined color sliders (see Type enum below).
 |
 |   Classes:
 |	_SoXtColorSlider
 |	
 |   Author(s)	: Alain Dumesny
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_XT_COLOR_SLIDER_
#define  _SO_XT_COLOR_SLIDER_

#include "_SoXtSlider.h"

class SbColor;
class SbVec2f;


//////////////////////////////////////////////////////////////////////////////
//
//  Class: _SoXtColorSlider
//
//	This class defines the color slider which is a set of predefined
//  sliders (R, G, B, H, S, V,..) with special behaviors.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoXtColSldr
class _SoXtColorSlider : public _SoXtSlider {
    
 public:
    
    //
    // list of color sliders types
    //
    enum Type {
	RED_SLIDER, 
	GREEN_SLIDER, 
	BLUE_SLIDER, 
	HUE_SLIDER, 
	SATURATION_SLIDER, 
	VALUE_SLIDER, 
	INTENSITY_SLIDER // independent of the slider value
    };
    
    // Constructor/Destructor
    _SoXtColorSlider(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE, 
	_SoXtColorSlider::Type type = INTENSITY_SLIDER);
    ~_SoXtColorSlider();
    
    //
    // Routine used to specify the slider value and also the top region
    // color (WYSIWYGmode in most cases). Users should call this routine 
    // instead of the base class setValue() routine if they want the color 
    // slider top region to correctly reflect the current color.
    // (code is optimized for WYSIWYGmode off)
    //
    // NOTE: routine expects
    //	    an rgb color for R, G or B slider type
    //	    an hsv color for H, S, or V slider type
    //	    an rgb color for Intensity slider type
    //
    // NOTE: if calling setBaseColor() changes the thumb position the
    // valueChanged callbacks will be called with the new value.
    //
    // C-api: name=setBaseCol
    void	setBaseColor(const float rgbOrHsv[3]);
    // C-api: name=getBaseCol
    const float *getBaseColor()     	{ return color; }
    
    //
    // This routine sets the WYSIWYG (What You See Is What You Get) mode
    // to be true or false (default FALSE). Immediate mode.
    // This has no effect on monochrome sliders.
    //
    void	setWYSIWYG(SbBool trueOrFalse);    // default FALSE
    SbBool  	isWYSIWYG() 	    	{ return WYSIWYGmode; }
    
    // returns the type of the slider
    _SoXtColorSlider::Type getType()	{ return type; }
    
  protected:

    // This constructor takes a boolean whether to build the widget now.
    // Subclasses can pass FALSE, then call _SoXtSlider::buildWidget()
    // when they are ready for it to be built.
    SoEXTENDER
    _SoXtColorSlider(
	Widget parent,
	const char *name, 
	SbBool buildInsideParent, 
	_SoXtColorSlider::Type type, 
	SbBool buildNow);
    
    // redefine this routine to cache slider top geometry
    virtual void    sizeChanged(const SbVec2s &newSize);
    
    // redefine this routine to do the actual slider top region drawing.
    // This routine is automatically called by the _SoXtSlider::redraw() routine.
    virtual void    drawSliderTopRegion();
    
  private:
    
    // local vars
    Boolean		WYSIWYGmode;
    Type		type;
    float		color[3];
    SbColor		*defaultColors, *colors;
    SbVec2f		*geometry;
    
    // routine that make the default and WYSIWYG colors
    void    makeDefaultColors();
    void    makeWYSIWYGcolors();
    
    static void	    sliderChangedCB(void *, float val);

    // this is called by both constructors
    void constructorCommon(_SoXtColorSlider::Type type, SbBool buildNow);
};


#endif  /* _SO_XT_COLOR_SLIDER_ */
