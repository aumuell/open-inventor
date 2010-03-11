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
|      Defines the Control panel class for controling a multiple slider 
|      component
|
|   Author(s)          : Paul Isaacs
|
______________  S I L I C O N   G R A P H I C S   I N C .  ____________
_______________________________________________________________________
*/


#ifndef SO_XT_SLIDER_CONTROL_PANEL_
#define SO_XT_SLIDER_CONTROL_PANEL_

#include <Inventor/Xt/SoXtSliderSetBase.h>

// the default value for the *special* resource called 'dividingPoint' 
#define DEFAULT_SLIDER_CONTROL_PANEL_DIVIDING_POINT 75

#define DEFAULT_SLIDER_CONTROL_PANEL_STYLE_LABEL "style"

class SoXtSliderSet;
class SoXtSliderModule;

class SoXtSliderControlPanel : public SoXtSliderSetBase {
  public:
      SoXtSliderControlPanel(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE);
      ~SoXtSliderControlPanel();
      Widget getControlPanelContainer() { return widget; };
      Widget getOpenCloseButton()       { return _openCloseButton; };
      void openUp();
      void closeDown();
      int  getStyle() { return _style; };
      void setStyle( int styleNum ) { _style = styleNum; };
  // internal:
      static void openCloseCallback( Widget w, void *client_data, 
			             void *call_data );
      static void styleCallback( Widget w, void *client_data, void *call_data);
      void setSliderModule( SoXtSliderModule *newOne ) { _sliderModule = newOne;};
      void setEditor( SoXtSliderSet *newOne )          { _editor = newOne; };

  protected:

      Widget   buildWidget(Widget parent);

  private:
      Widget _openCloseButton;
      Widget _styleButton;
      int    _dividingPoint;          // percent of the window (horizontally)
				      // 'openCloseButton' if the mimMaxButton
				      // is also being displayed.
      int            _style;	
      void           initLayout();
      SoXtSliderModule *_sliderModule;
      SoXtSliderSet    *_editor;
};

#endif // SO_XT_SLIDER_CONTROL_PANEL_
