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


#ifndef SO_XT_MIN_MAX_SLIDER_
#define SO_XT_MIN_MAX_SLIDER_

#include <Inventor/Xt/SoXtSliderSetBase.h>

#define DEFAULT_SLIDER_DECIMAL_POINTS 2
#define DEFAULT_SLIDER_SCALE_MINIMUM -1000
#define DEFAULT_SLIDER_SCALE_MAXIMUM 1000

class SoXtSliderTool;
class SoXtMultiSlider;

class SoXtMinMaxSlider : public SoXtSliderSetBase
{
  public:
      SoXtMinMaxSlider(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE,
	int startingMin = DEFAULT_SLIDER_SCALE_MINIMUM, 
	int startingMax = DEFAULT_SLIDER_SCALE_MAXIMUM);
      ~SoXtMinMaxSlider();
      float   getSliderValue();
      float   getSliderMin();
      float   getSliderMax();
      void    setSliderValue( float theVal );
      void    setSliderMin( float theMin );
      void    setSliderMax( float theMax );
      short   getNumDecimals();
  // internal:
      static void sliderCallback(Widget w, void *client_data, void *call_data );
      void    setSliderTool( SoXtSliderTool *newOne ) { _sliderTool = newOne; };
      void    setMultiSlider( SoXtMultiSlider *newOne ) { _multiSlider = newOne; };

  protected:

    Widget  buildWidget(Widget parent);

  private:
      int     _startingMin;
      int     _startingMax;
      float   convertSliderToFloat( int sliderValue );
      int     convertFloatToSlider( float floatValue );
      SoXtSliderTool *_sliderTool;
      SoXtMultiSlider *_multiSlider;
};

#endif /* SO_XT_MIN_MAX_SLIDER_ */
