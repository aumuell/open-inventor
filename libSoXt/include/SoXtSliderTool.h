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
|      A component class that creates a more functionally extensive
|      slider than that given by motif.
|
|   Author(s)          : Paul Isaacs
|
______________  S I L I C O N   G R A P H I C S   I N C .  ____________
_______________________________________________________________________
*/


#ifndef SO_XT_SLIDER_TOOL_
#define SO_XT_SLIDER_TOOL_

#include "Xm/Xm.h"
#include <Inventor/Xt/SoXtSliderSetBase.h>
#include "SoXtMinMaxSlider.h"
#include "SoXtFloatText.h"
#include "SoXtMMSliderMin.h"
#include "SoXtMMSliderMax.h"

#define DEFAULT_SLIDER_TOOL_LEFT_DIVIDING_POINT 25
#define DEFAULT_SLIDER_TOOL_RIGHT_DIVIDING_POINT 75

#define DEFAULT_SLIDER_TOOL_SLIDER_DIVIDING_POINT 40
#define DEFAULT_SLIDER_TOOL_MIN_DIVIDING_POINT 40
#define DEFAULT_SLIDER_TOOL_MAX_DIVIDING_POINT 40

#define DEFAULT_SLIDER_TOOL_MIN_BOTTOM_POINT 80
#define DEFAULT_SLIDER_TOOL_MAX_BOTTOM_POINT 80

#define DEFAULT_SLIDER_TOOL_SKINNY_OPEN_POINT_1 22
#define DEFAULT_SLIDER_TOOL_SKINNY_OPEN_POINT_2 44
#define DEFAULT_SLIDER_TOOL_SKINNY_OPEN_POINT_3 78

#define DEFAULT_SLIDER_TOOL_SKINNY_CLOSED_POINT_1 30

#define DEFAULT_SLIDER_TOOL_BORDER_WIDTH     2


#define SLIDER_TOOL_STYLE_CLOSED        0
#define SLIDER_TOOL_STYLE_OPEN          1
#define SLIDER_TOOL_STYLE_SKINNY_OPEN   2
#define SLIDER_TOOL_STYLE_SKINNY_CLOSED 3

class SoXtSliderTool : public SoXtSliderSetBase {
  public:
    SoXtSliderTool( 
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE,
	int startingMin = DEFAULT_SLIDER_SCALE_MINIMUM, 
	int startingMax = DEFAULT_SLIDER_SCALE_MAXIMUM);
      ~SoXtSliderTool();
      void openMinMax();
      void closeMinMax();
      void makeSkinnyOpen();
      void makeSkinnyClosed();

      // return various widgets from the sliderTool	
      Widget       getContainer() { return widget; };
      SoXtMinMaxSlider *getSlider() { return _slider; };
      SoXtFloatText  *getValue() { return _value; };
      SoXtFloatText  *getMinValue() { return _minValue; };
      SoXtFloatText  *getMaxValue() { return _maxValue; };

      // return the value that the sliderTool is currently set at
      float   getSliderValue() { return _slider->getSliderValue(); };

      // set the sliderTool's value, min, or max.
      void    toolSetValue(  float newVal );
      void    toolSetMin( float newVal );
      void    toolSetMax( float newVal );

  // internal:
      static  void sliderValueCallback(Widget w,
				      void *client_data, void *call_data );
      static  void minValueCallback(Widget w,
				      void *client_data, void *call_data );
      static  void maxValueCallback(Widget w,
				      void *client_data, void *call_data );
      void    setMultiSlider( SoXtMultiSlider *newOne );

      Widget buildWidget( Widget parent, int startingMin, int startingMax);

  private:
      SoXtFloatText      *_value;
      SoXtMinMaxSlider   *_slider;
      SoXtFloatText      *_minValue;
      SoXtFloatText      *_maxValue;
      SoXtMMSliderMin      *_min;
      SoXtMMSliderMax      *_max;
      XmString           _title;
      int           _leftDividingPoint; // left edge (in percents) of the 
					  // slider area when all three sections
					  // are shown.
      int           _rightDividingPoint; // right edge (in percents) of the 
					  // slider area when all three sections
					  // are shown.
      int   _sliderDividingPoint;	
      int   _minDividingPoint;	
      int   _maxDividingPoint;	
      int   _minBottomPoint;
      int   _maxBottomPoint;

      void      initLayout();
      SoXtMultiSlider *_multiSlider;
};

#endif /* SO_XT_SLIDER_TOOL_ */
