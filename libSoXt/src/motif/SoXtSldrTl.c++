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
* Copyright (C) 1990-93   Silicon Graphics, Inc.
*
_______________________________________________________________________
______________  S I L I C O N   G R A P H I C S   I N C .  ____________
|
|   $Revision: 1.3 $
|
|   Classes:
|      SoXtSliderTool is a component that expands the functionality of the
|      standard motif slider (they call it 'scale') widget.
|
|   Author(s)          : Paul Isaacs
|
______________  S I L I C O N   G R A P H I C S   I N C .  ____________
_______________________________________________________________________
*/



#include <inttypes.h>
#include "SoXtSliderTool.h"
#include "SoXtMultiSlider.h"
#include <X11/StringDefs.h>
#include <Xm/Form.h>

/***************************************************************************/
/* static variables used by XtGetApplicationResources to get values of     */
/* user-settable parameters.  The defaults values are used if the user     */
/* does not override them.                                                 */
/***************************************************************************/
static XtResource _leftResources[] = {
    { XmNleftPosition, XmCPosition, XtRInt, sizeof( int),
      0,
      XtRImmediate, (XtPointer) DEFAULT_SLIDER_TOOL_LEFT_DIVIDING_POINT }  
};
static XtResource _rightResources[] = {
    { XmNrightPosition, XmCPosition, XtRInt, sizeof( int),
      0,
      XtRImmediate, (XtPointer) DEFAULT_SLIDER_TOOL_RIGHT_DIVIDING_POINT }  
};
static XtResource _valueResources[] = {
    { XmNbottomPosition, XmCPosition, XtRInt, sizeof( int),
      0,
      XtRImmediate, (XtPointer) DEFAULT_SLIDER_TOOL_SLIDER_DIVIDING_POINT }  
};
static XtResource _minValueResources[] = {
    { XmNbottomPosition, XmCPosition, XtRInt, sizeof( int),
      0,
      XtRImmediate, (XtPointer) DEFAULT_SLIDER_TOOL_MIN_DIVIDING_POINT }  
};
static XtResource _minResources[] = {
    { XmNbottomPosition, XmCPosition, XtRInt, sizeof( int),
      0,
      XtRImmediate, (XtPointer) DEFAULT_SLIDER_TOOL_MIN_BOTTOM_POINT }  
};
static XtResource _maxValueResources[] = {
    { XmNbottomPosition, XmCPosition, XtRInt, sizeof( int),
      0,
      XtRImmediate, (XtPointer) DEFAULT_SLIDER_TOOL_MAX_DIVIDING_POINT }  
};
static XtResource _maxResources[] = {
    { XmNbottomPosition, XmCPosition, XtRInt, sizeof( int),
      0,
      XtRImmediate, (XtPointer) DEFAULT_SLIDER_TOOL_MAX_BOTTOM_POINT }  
};
static XtResource _borderWidthResource[] = {
    { XtNborderWidth, XtCBorderWidth, XtRDimension, sizeof( unsigned short),
      0, XtRImmediate, (XtPointer) DEFAULT_SLIDER_TOOL_BORDER_WIDTH }  
};

SoXtSliderTool::SoXtSliderTool(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent, 
    int startingMin,
    int startingMax)
	: SoXtSliderSetBase(
	    parent,
	    name, 
	    buildInsideParent,
	    NULL)
{
    _leftDividingPoint = 0;
    _rightDividingPoint = 0;
    _sliderDividingPoint = 0;
    _minDividingPoint = 0;
    _maxDividingPoint = 0;
    _minBottomPoint = 0;
    _maxBottomPoint = 0;
    _layoutWidth  = 310;
    _layoutHeight = 82;
    _title = NULL;
    _multiSlider = NULL;
    
    // Build the widget!
    Widget w = buildWidget( getParentWidget(), startingMin, startingMax);
    setBaseWidget(w);
}

SoXtSliderTool::~SoXtSliderTool()
{
    delete _value;
    delete _slider;
    delete _minValue;
    delete _maxValue;
    delete _min;
    delete _max;
    
    if (_title != NULL)
	XmStringFree(_title);
}

Widget
SoXtSliderTool::buildWidget( 
    Widget parent,
    int startingMin,
    int startingMax )
{
    widget = XtCreateManagedWidget(getWidgetName(),xmFormWidgetClass,
					parent,NULL, 0);

    _value = new SoXtFloatText(widget, "value");
    _slider = new SoXtMinMaxSlider(widget, "slider", TRUE, startingMin, startingMax);
    _slider->setSliderTool( this );

    _max = new SoXtMMSliderMax( widget, "max" );
    _max->setSliderTool( this );
    _maxValue = new SoXtFloatText( widget, "maxValue" );

    _min = new SoXtMMSliderMin(widget, "min");
    _min->setSliderTool( this );
    _minValue = new SoXtFloatText( widget, "minValue" );
    
    /* take care of layout within the tool */
    initLayout();

    /* set the callbacks for the sub components */
    XtAddCallback( _value->getWidget(), XmNactivateCallback,
		   (XtCallbackProc) &SoXtSliderTool::sliderValueCallback,
		   (XtPointer) this );
    XtAddCallback( _minValue->getWidget(), XmNactivateCallback,
		   (XtCallbackProc) &SoXtSliderTool::minValueCallback,
		   (XtPointer) this );
    XtAddCallback( _maxValue->getWidget(), XmNactivateCallback,
		   (XtCallbackProc) &SoXtSliderTool::maxValueCallback,
		   (XtPointer) this );

    /* initialize values throughout the tool to be those in the slider */
    /* Right now, this winds up filling the label widgets with text    */
    toolSetValue( _slider->getSliderValue());
    toolSetMin( _slider->getSliderMin());
    toolSetMax( _slider->getSliderMax());
    return widget;
}

void
SoXtSliderTool::openMinMax()
{
    Arg wargs[10];
    int  nargs;

    _layoutWidth  = 310;
    _layoutHeight = 82;

    // set positions for widgets...
    nargs = 0;
    XtSetArg(wargs[nargs],XmNbottomPosition,   _sliderDividingPoint ); nargs++;
    XtSetArg(wargs[nargs],XmNleftPosition,     _leftDividingPoint ); nargs++;
    XtSetArg(wargs[nargs],XmNrightPosition,    _rightDividingPoint ); nargs++;
    XtSetValues( _value->getWidget(), wargs, nargs );

    nargs = 0;
    XtSetArg(wargs[nargs],XmNtopPosition,      _sliderDividingPoint ); nargs++;
    XtSetArg(wargs[nargs],XmNleftPosition,     _leftDividingPoint ); nargs++;
    XtSetArg(wargs[nargs],XmNrightPosition,    _rightDividingPoint ); nargs++;
    if ( _title )
	XtSetArg(wargs[nargs],XmNtitleString,      _title ); nargs++;
    XtSetValues( _slider->getWidget(), wargs, nargs );

    nargs = 0;
    XtSetArg(wargs[nargs],XmNbottomPosition,   _minDividingPoint ); nargs++;
    XtSetArg(wargs[nargs],XmNleftAttachment,   XmATTACH_FORM ); nargs++;
    XtSetArg(wargs[nargs],XmNrightPosition,    _leftDividingPoint ); nargs++;
    XtSetValues( _minValue->getWidget(), wargs, nargs );

    nargs = 0;
    XtSetArg(wargs[nargs],XmNbottomPosition,   _maxDividingPoint ); nargs++;
    XtSetArg(wargs[nargs],XmNleftPosition,     _rightDividingPoint); nargs++;
    XtSetArg(wargs[nargs],XmNrightAttachment,  XmATTACH_FORM ); nargs++;
    XtSetValues( _maxValue->getWidget(), wargs, nargs );

    _maxValue->show();
    _max->show();
    
    _minValue->show();
    _min->show();
}

void
SoXtSliderTool::closeMinMax()
{
    Arg wargs[10];
    int  nargs;

    _layoutWidth  = 310;
    _layoutHeight = 82;

    // redo layout for widgets
    nargs = 0;
    XtSetArg(wargs[nargs],XmNbottomPosition,   _sliderDividingPoint ); nargs++;
    XtSetArg(wargs[nargs],XmNleftPosition,     0 ); nargs++;
    XtSetArg(wargs[nargs],XmNrightPosition,    100 ); nargs++;
    XtSetValues( _value->getWidget(), wargs, nargs );

    nargs = 0;
    XtSetArg(wargs[nargs],XmNtopPosition,      _sliderDividingPoint ); nargs++;
    XtSetArg(wargs[nargs],XmNleftPosition,     0 ); nargs++;
    XtSetArg(wargs[nargs],XmNrightPosition,    100 ); nargs++;
    if ( _title )
	XtSetArg(wargs[nargs],XmNtitleString,      _title ); nargs++;
    XtSetValues( _slider->getWidget(), wargs, nargs );

    _maxValue->hide();
    _max->hide();
    
    _minValue->hide();
    _min->hide();
}

void
SoXtSliderTool::makeSkinnyOpen()
{
    Arg wargs[10];
    int  nargs;

    _layoutWidth  = 310;
    _layoutHeight = 37;

    // redo layout 
    nargs = 0;
    XtSetArg(wargs[nargs],XmNbottomPosition,   100 ); nargs++;
    XtSetArg(wargs[nargs],XmNleftPosition,     0 ); nargs++;
    XtSetArg(wargs[nargs],XmNrightPosition,    
	                  DEFAULT_SLIDER_TOOL_SKINNY_OPEN_POINT_1 ); nargs++;
    XtSetValues( _value->getWidget(), wargs, nargs );

    nargs = 0;
    XtSetArg(wargs[nargs],XmNbottomPosition,   100 ); nargs++;
    XtSetArg(wargs[nargs],XmNleftAttachment,   XmATTACH_POSITION ); nargs++;
    XtSetArg(wargs[nargs],XmNleftPosition,    
	                  DEFAULT_SLIDER_TOOL_SKINNY_OPEN_POINT_1 ); nargs++;
    XtSetArg(wargs[nargs],XmNrightPosition,
	                  DEFAULT_SLIDER_TOOL_SKINNY_OPEN_POINT_2 ); nargs++;
    XtSetValues( _minValue->getWidget(), wargs, nargs );

    // save the title before zeroing it out...
    if (_title == NULL) {
	nargs = 0;
	XtSetArg(wargs[nargs],XmNtitleString,  (XtPointer) &_title ); nargs++;
	XtGetValues( _slider->getWidget(), wargs, nargs );
    }

    nargs = 0;
    XtSetArg(wargs[nargs],XmNtopPosition,      0 ); nargs++;
    XtSetArg(wargs[nargs],XmNleftPosition,
	                  DEFAULT_SLIDER_TOOL_SKINNY_OPEN_POINT_2 ); nargs++;
    XtSetArg(wargs[nargs],XmNrightPosition,
	                  DEFAULT_SLIDER_TOOL_SKINNY_OPEN_POINT_3 ); nargs++;
    XtSetArg(wargs[nargs],XmNtitleString,      (void *) NULL ); nargs++;
    XtSetValues( _slider->getWidget(), wargs, nargs );

    nargs = 0;
    XtSetArg(wargs[nargs],XmNbottomPosition,   100 ); nargs++;
    XtSetArg(wargs[nargs],XmNleftPosition,
	                  DEFAULT_SLIDER_TOOL_SKINNY_OPEN_POINT_3 ); nargs++;
    XtSetArg(wargs[nargs],XmNrightAttachment,  XmATTACH_POSITION ); nargs++;
    XtSetArg(wargs[nargs],XmNrightPosition,   100 ); nargs++;
    XtSetValues( _maxValue->getWidget(), wargs, nargs );

    _maxValue->show();
    _max->hide();
    
    _minValue->show();
    _min->hide();
}

void
SoXtSliderTool::makeSkinnyClosed()
{
    Arg wargs[10];
    int  nargs;

    _layoutWidth  = 310;
    _layoutHeight = 37;

    // redo layout 
    nargs = 0;
    XtSetArg(wargs[nargs],XmNbottomPosition,   100 ); nargs++;
    XtSetArg(wargs[nargs],XmNleftPosition,     0 ); nargs++;
    XtSetArg(wargs[nargs],XmNrightPosition,    
	                  DEFAULT_SLIDER_TOOL_SKINNY_CLOSED_POINT_1 ); nargs++;
    XtSetValues( _value->getWidget(), wargs, nargs );

    // save the title before zeroing it out...
    if (_title == NULL) {
	nargs = 0;
	XtSetArg(wargs[nargs],XmNtitleString,  &_title ); nargs++;
	XtGetValues( _slider->getWidget(), wargs, nargs );
    }

    nargs = 0;
    XtSetArg(wargs[nargs],XmNtopPosition,      0 ); nargs++;
    XtSetArg(wargs[nargs],XmNleftPosition,
	                  DEFAULT_SLIDER_TOOL_SKINNY_CLOSED_POINT_1 ); nargs++;
    XtSetArg(wargs[nargs],XmNrightPosition, 100 ); nargs++;
    XtSetArg(wargs[nargs],XmNtitleString,      (void *) NULL ); nargs++;
    XtSetValues( _slider->getWidget(), wargs, nargs );

    _maxValue->hide();
    _max->hide();
    
    _minValue->hide();
    _min->hide();
}

void 
SoXtSliderTool::toolSetValue( float newVal )
{
    if ( _slider->getSliderMin() > newVal )
	toolSetMin( newVal );
    if ( _slider->getSliderMax() < newVal )
	toolSetMax( newVal );

    /* send the value to the slider */
    _slider->setSliderValue( newVal );

    /* send the value to the label */
    _value->setDisplayedValue( newVal, _slider->getNumDecimals() );
}

void 
SoXtSliderTool::toolSetMin( float newVal )
{
    float valToUse;

    if ( newVal <= _slider->getSliderValue() 
	 && newVal < _slider->getSliderMax() )
	valToUse = newVal;
    else
	valToUse = _slider->getSliderMin();

    /* send the new minimum value to the slider */
    _slider->setSliderMin( valToUse );

    /* send the new minimum value to the minimum label */
    _minValue->setDisplayedValue( valToUse, _slider->getNumDecimals() );
}

void 
SoXtSliderTool::toolSetMax( float newVal )
{
    float valToUse;

    if ( newVal >= _slider->getSliderValue() 
	&& newVal > _slider->getSliderMin() )
	valToUse = newVal;
    else
	valToUse = _slider->getSliderMax();

    /* send the new maximum value to the slider */
    _slider->setSliderMax( valToUse );

    /* send the new maximum value to the maximum label */
    _maxValue->setDisplayedValue( valToUse, _slider->getNumDecimals() );
}

void
SoXtSliderTool::initLayout()
{
    if (widget == NULL)
	return;

    Arg wargs[10];
    int  nargs;
    Dimension  bWidth;

    /* get the dividing points from the resource manager */
    /* note: defaults will be returned if the user didn't set them. */
    /* these defaults can be found at the beginning of this file, where */
    /* _leftResources, _rightResources, etc. are defined */
    XtGetApplicationResources( widget, (XtPointer) &bWidth, 
	    	       _borderWidthResource, XtNumber( _borderWidthResource), 
		       NULL, 0 );

    XtGetApplicationResources( _slider->getWidget(), 
			(XtPointer) &_leftDividingPoint, 
	    	       _leftResources, XtNumber( _leftResources), 
		       NULL, 0 );
    XtGetApplicationResources( _slider->getWidget(),
			(XtPointer) &_rightDividingPoint, 
	    	       _rightResources, XtNumber( _rightResources), 
		       NULL, 0 );

    XtGetApplicationResources( _value->getWidget(), 
		       (XtPointer) &_sliderDividingPoint, 
	    	       _valueResources, XtNumber( _valueResources), 
		       NULL, 0 );

    XtGetApplicationResources( _minValue->getWidget(), 
		       (XtPointer) &_minDividingPoint, 
	    	       _minValueResources, XtNumber( _minValueResources), 
		       NULL, 0 );
    XtGetApplicationResources( _min->getWidget(),(XtPointer) &_minBottomPoint, 
	    	       _minResources, XtNumber( _minResources), 
		       NULL, 0 );

    XtGetApplicationResources( _maxValue->getWidget(), 
		       (XtPointer) &_maxDividingPoint, 
	    	       _maxValueResources, XtNumber( _maxValueResources), 
		       NULL, 0 );
    XtGetApplicationResources( _max->getWidget(),(XtPointer) &_maxBottomPoint, 
	    	       _maxResources, XtNumber( _maxResources), 
		       NULL, 0 );

    nargs = 0;
    XtSetArg(wargs[nargs],XmNborderWidth,    bWidth ); nargs++;
    XtSetValues( widget, wargs, nargs );

    nargs = 0;
    XtSetArg(wargs[nargs],XmNtopAttachment,    XmATTACH_FORM ); nargs++;
    XtSetArg(wargs[nargs],XmNbottomAttachment, XmATTACH_POSITION ); nargs++;
    XtSetArg(wargs[nargs],XmNbottomPosition,   _sliderDividingPoint ); nargs++;
    XtSetArg(wargs[nargs],XmNleftAttachment,   XmATTACH_POSITION ); nargs++;
    XtSetArg(wargs[nargs],XmNleftPosition,     _leftDividingPoint ); nargs++;
    XtSetArg(wargs[nargs],XmNrightAttachment,  XmATTACH_POSITION ); nargs++;
    XtSetArg(wargs[nargs],XmNrightPosition,    _rightDividingPoint ); nargs++;
    XtSetValues( _value->getWidget(), wargs, nargs );

    nargs = 0;
    XtSetArg(wargs[nargs],XmNtopAttachment,    XmATTACH_POSITION ); nargs++;
    XtSetArg(wargs[nargs],XmNtopPosition,      _sliderDividingPoint ); nargs++;
    XtSetArg(wargs[nargs],XmNbottomAttachment, XmATTACH_FORM ); nargs++;
    XtSetArg(wargs[nargs],XmNleftAttachment,   XmATTACH_POSITION ); nargs++;
    XtSetArg(wargs[nargs],XmNleftPosition,     _leftDividingPoint ); nargs++;
    XtSetArg(wargs[nargs],XmNrightAttachment,  XmATTACH_POSITION ); nargs++;
    XtSetArg(wargs[nargs],XmNrightPosition,    _rightDividingPoint ); nargs++;
    XtSetValues( _slider->getWidget(), wargs, nargs );

    nargs = 0;
    XtSetArg(wargs[nargs],XmNtopAttachment,    XmATTACH_FORM ); nargs++;
    XtSetArg(wargs[nargs],XmNbottomAttachment, XmATTACH_POSITION ); nargs++;
    XtSetArg(wargs[nargs],XmNbottomPosition,   _minDividingPoint ); nargs++;
    XtSetArg(wargs[nargs],XmNleftAttachment,   XmATTACH_FORM ); nargs++;
    XtSetArg(wargs[nargs],XmNrightAttachment,   XmATTACH_POSITION ); nargs++;
    XtSetArg(wargs[nargs],XmNrightPosition,    _leftDividingPoint ); nargs++;
    XtSetValues( _minValue->getWidget(), wargs, nargs );

    nargs = 0;
    XtSetArg(wargs[nargs],XmNtopAttachment,    XmATTACH_POSITION ); nargs++;
    XtSetArg(wargs[nargs],XmNtopPosition,      _minDividingPoint); nargs++;
    XtSetArg(wargs[nargs],XmNbottomAttachment, XmATTACH_POSITION ); nargs++;
    XtSetArg(wargs[nargs],XmNbottomPosition,   _minBottomPoint ); nargs++;
    XtSetArg(wargs[nargs],XmNleftAttachment,   XmATTACH_FORM ); nargs++;
    XtSetArg(wargs[nargs],XmNrightAttachment,   XmATTACH_POSITION ); nargs++;
    XtSetArg(wargs[nargs],XmNrightPosition,    _leftDividingPoint ); nargs++;
    XtSetValues( _min->getWidget(), wargs, nargs );

    nargs = 0;
    XtSetArg(wargs[nargs],XmNtopAttachment,    XmATTACH_FORM ); nargs++;
    XtSetArg(wargs[nargs],XmNbottomAttachment, XmATTACH_POSITION ); nargs++;
    XtSetArg(wargs[nargs],XmNbottomPosition,   _maxDividingPoint ); nargs++;
    XtSetArg(wargs[nargs],XmNleftAttachment,   XmATTACH_POSITION ); nargs++;
    XtSetArg(wargs[nargs],XmNleftPosition,     _rightDividingPoint); nargs++;
    XtSetArg(wargs[nargs],XmNrightAttachment,  XmATTACH_FORM ); nargs++;
    XtSetValues( _maxValue->getWidget(), wargs, nargs );

    nargs = 0;
    XtSetArg(wargs[nargs],XmNtopAttachment,    XmATTACH_POSITION ); nargs++;
    XtSetArg(wargs[nargs],XmNtopPosition,      _maxDividingPoint ); nargs++;
    XtSetArg(wargs[nargs],XmNbottomAttachment, XmATTACH_POSITION ); nargs++;
    XtSetArg(wargs[nargs],XmNbottomPosition,   _maxBottomPoint ); nargs++;
    XtSetArg(wargs[nargs],XmNleftAttachment,   XmATTACH_POSITION ); nargs++;
    XtSetArg(wargs[nargs],XmNleftPosition,     _rightDividingPoint); nargs++;
    XtSetArg(wargs[nargs],XmNrightAttachment,  XmATTACH_FORM ); nargs++;
    XtSetValues( _max->getWidget(), wargs, nargs );
}

void
SoXtSliderTool::sliderValueCallback( Widget, void *client_data, void *)
{
    float    theVal;
    SoXtMinMaxSlider     *theSlider;
    SoXtSliderTool *theSliderTool = (SoXtSliderTool *) client_data;

    if (theSliderTool) {
	// If you can't get a legible value from the value label... */
	if ( !theSliderTool->getValue()->getDisplayedValue( theVal ) ) {
	    // get if from the slider itself
	    theSlider = theSliderTool->getSlider();
	    if (theSlider)
		theVal = theSlider->getSliderValue();
	}
	// set the value throughout the SoXtSliderTool
	theSliderTool->toolSetValue( theVal );

	// If you can find a tripleSlider in the ancestry...
	if ( theSliderTool->_multiSlider )
	    theSliderTool->_multiSlider->exportValuesToInventor();
    }
}

void
SoXtSliderTool::minValueCallback( Widget, void *client_data, void * )
{
    float    theVal;
    SoXtMinMaxSlider     *theSlider;
    SoXtSliderTool *theSliderTool = (SoXtSliderTool *) client_data;

    if (theSliderTool) {
	// If you can't get a legible value from the min value label... */
	if ( !theSliderTool->getMinValue()->getDisplayedValue( theVal ) ) {
	    // get if from the slider itself
	    theSlider = theSliderTool->getSlider();
	    if (theSlider)
		theVal = theSlider->getSliderMin();
	}
	// set the value throughout the SoXtSliderTool
	theSliderTool->toolSetMin( theVal );
    }
}

void
SoXtSliderTool::maxValueCallback( Widget, void *client_data, void * )
{
    float    theVal;
    SoXtMinMaxSlider     *theSlider;
    SoXtSliderTool *theSliderTool = (SoXtSliderTool *) client_data;

    if (theSliderTool) {
	// If you can't get a legible value from the max value label... */
	if ( !theSliderTool->getMaxValue()->getDisplayedValue( theVal ) ) {
	    // get if from the slider itself
	    theSlider = theSliderTool->getSlider();
	    if (theSlider)
		theVal = theSlider->getSliderMax();
	}
	// set the value throughout the SoXtSliderTool
	theSliderTool->toolSetMax( theVal );
    }
}

void
SoXtSliderTool::setMultiSlider( SoXtMultiSlider *newOne )
{
    _multiSlider = newOne;
    if (_slider)
	_slider->setMultiSlider( newOne );
}
