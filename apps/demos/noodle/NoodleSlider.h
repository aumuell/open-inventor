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
|   Description:
|      A component class that creates a more functionally extensive
|      slider than that given by motif.
|
|   Author(s)          : Paul Isaacs
|
*/


#ifndef _NOODLE_SLIDER_
#define _NOODLE_SLIDER_

#include <Xm/Xm.h>
#include <Inventor/Xt/SoXtComponent.h>
#include <Inventor/misc/SoCallbackList.h>

class NoodleSlider : public SoXtComponent {
  public:
    NoodleSlider( 
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE,
	int startingMin = -1000, 
	int startingMax = 1000);
    ~NoodleSlider();

      // add callbacks to the slider to be called when the value changes,
      // either from the slider or the text widget.
      void addValueChangedCallback(SoCallbackListCB *f, void *userData = NULL);
      void removeValueChangedCallback(SoCallbackListCB *f,void *userData =NULL);

      // return the current value, min, or max.
      float   getValue();
      float   getMin();
      float   getMax();

      // set the value, min, or max.
      void    setValue(  float newVal );
      void    setMin( float newVal );
      void    setMax( float newVal );

  // internal:
      static  void valueWidgetCallback(Widget w,
				      void *client_data, void *call_data );
      static  void sliderWidgetCallback(Widget w,
				      void *client_data, void *call_data );
      Widget buildWidget( Widget parent, int startingMin, int startingMax);

  private:
      Widget      _form;
      Widget      _value;
      Widget      _slider;

      float convertSliderToFloat( int sliderValue );
      int convertFloatToSlider( float floatValue );
      short getNumDecimals();
      void      initLayout();

      SoCallbackList *valueChangedCallbacks;
};

#endif /* _NOODLE_SLIDER_ */
