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
|   $Revision: 1.4 $
|
|   Classes:
|      SoXtMinMaxSlider is a component that expands the functionality of the
|      standard motif slider (they call it 'scale') widget.
|
|   Author(s)          : Paul Isaacs
|
______________  S I L I C O N   G R A P H I C S   I N C .  ____________
_______________________________________________________________________
*/



#include <inttypes.h>
#include "SoXtMinMaxSlider.h"
#include "SoXtSliderTool.h"
#include "SoXtMultiSlider.h"
#include <X11/StringDefs.h>
#include <Xm/Scale.h>

/***************************************************************************/
/* static variables used by XtGetApplicationResources to get values of     */
/* user-settable parameters.  The defaults values are used if the user     */
/* does not override them.                                                 */
/***************************************************************************/
typedef struct {
    unsigned char     orientation;
    short   decimalPoints;
    int     maximum;
    int     minimum;
} ScaleStyleData, *ScaleStyleDataPtr;

static XtResource _scaleStyleResources[] = {
    { XmNorientation, XmCOrientation, XmROrientation, sizeof( unsigned char),
      XtOffset( ScaleStyleDataPtr, orientation ),
      XtRString, (XtPointer) "horizontal" },
    { XmNdecimalPoints, XmCDecimalPoints, XmRShort, sizeof( short),
      XtOffset( ScaleStyleDataPtr, decimalPoints ),
      XtRImmediate, (XtPointer) DEFAULT_SLIDER_DECIMAL_POINTS },
    { XmNmaximum, XmCMaximum, XmRInt, sizeof( int),
      XtOffset( ScaleStyleDataPtr, maximum ),
      XtRImmediate, (XtPointer) DEFAULT_SLIDER_SCALE_MAXIMUM },
    { XmNminimum, XmCMinimum, XmRInt, sizeof( int),
      XtOffset( ScaleStyleDataPtr, minimum ),
      XtRImmediate, (XtPointer) DEFAULT_SLIDER_SCALE_MINIMUM }
};

SoXtMinMaxSlider::SoXtMinMaxSlider(
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
    _startingMin = startingMin;
    _startingMax = startingMax;
    
    _sliderTool = NULL;
    _multiSlider = NULL;

    _layoutWidth  = 160;
    _layoutHeight = 40;

    // Build the widget!
    Widget w = buildWidget(getParentWidget());
    setBaseWidget(w);
}

SoXtMinMaxSlider::~SoXtMinMaxSlider() {}

Widget
SoXtMinMaxSlider::buildWidget( Widget parent )
{
    Arg wargs[10];
    int  nargs;
    ScaleStyleData styleData;

    nargs = 0;
    XtSetArg(wargs[nargs],XmNorientation,    XmHORIZONTAL ); nargs++;
    XtSetArg(wargs[nargs],XmNminimum,       _startingMin ); nargs++;
    XtSetArg(wargs[nargs],XmNmaximum,       _startingMax ); nargs++;
    widget = XtCreateManagedWidget(getWidgetName(),xmScaleWidgetClass,
					parent,wargs, nargs);


    /* see if user decided to override any of these values */
        /* get startingMin and startingMax as the defaults in the query */
        _scaleStyleResources[2].default_addr = (XtPointer) (unsigned long) _startingMax;
        _scaleStyleResources[3].default_addr = (XtPointer) (unsigned long) _startingMin;
        /* ask the question */
        XtGetApplicationResources( widget, (XtPointer) &styleData, 
	    	       _scaleStyleResources, XtNumber( _scaleStyleResources), 
		       NULL, 0 );
    nargs = 0;
    XtSetArg(wargs[nargs],XmNorientation,   styleData.orientation ); nargs++;
    XtSetArg(wargs[nargs],XmNdecimalPoints, styleData.decimalPoints ); nargs++;
    XtSetArg(wargs[nargs],XmNminimum,       styleData.minimum ); nargs++;
    XtSetArg(wargs[nargs],XmNmaximum,       styleData.maximum ); nargs++;
    XtSetValues( widget, wargs, nargs );

    /* establish all the callbacks */

    XtAddCallback( widget, XmNdragCallback,
		    (XtCallbackProc) &SoXtMinMaxSlider::sliderCallback,
		    (XtPointer) this );
    XtAddCallback( widget, XmNvalueChangedCallback,
		    (XtCallbackProc) &SoXtMinMaxSlider::sliderCallback,
		    (XtPointer) this );
    return widget;
}

short
SoXtMinMaxSlider::getNumDecimals()
{
    if ( widget == NULL)
	return 0;

    Arg wargs[10];
    short decimals;

    XtSetArg( wargs[0], XmNdecimalPoints, &decimals );
    XtGetValues( widget, wargs, 1 );

    return decimals;
}

float 
SoXtMinMaxSlider::convertSliderToFloat( int sliderValue )
{
    short decimals;
    float theVal;
    int   count;

    /* shift over the appropriate number of decimal places */
    decimals = getNumDecimals();
    if (decimals < 0)
	decimals = 0;
    for( count = 0, theVal = sliderValue; count < decimals; count++ )
        theVal /= 10.0;
    return( theVal );
}

int 
SoXtMinMaxSlider::convertFloatToSlider( float floatValue )
{
    short decimals;
    float convertedFloat;
    int   count, returnValue;

    /* shift over the appropriate number of decimal places */
    decimals = getNumDecimals();
    if (decimals < 0)
	decimals = 0;
    for( count = 0, convertedFloat = floatValue; count < decimals; count++ )
        convertedFloat *= 10.0;
    returnValue = (int) convertedFloat;
    return( returnValue );
}

float
SoXtMinMaxSlider::getSliderValue()
{
    if (widget == NULL)
	return 0.0;

    Arg wargs[10];
    int   motifIntValue;

    /* get motifIntValue */
    XtSetArg( wargs[0], XmNvalue, &motifIntValue );
    XtGetValues( widget, wargs, 1 );

    return ( convertSliderToFloat( motifIntValue ) );
}

float
SoXtMinMaxSlider::getSliderMin()
{
    if (widget == NULL)
	return 0.0;

    Arg wargs[10];
    int   motifIntValue;

    /* get motifIntValue */
    XtSetArg( wargs[0], XmNminimum, &motifIntValue );
    XtGetValues( widget, wargs, 1 );

    return ( convertSliderToFloat( motifIntValue ) );
}

float
SoXtMinMaxSlider::getSliderMax()
{
    if (widget == NULL)
	return 0.0;

    Arg wargs[10];
    int   motifIntValue;

    /* get motifIntValue */
    XtSetArg( wargs[0], XmNmaximum, &motifIntValue );
    XtGetValues( widget, wargs, 1 );

    return ( convertSliderToFloat( motifIntValue ) );
}

void
SoXtMinMaxSlider::setSliderValue( float theVal )
{
    if (widget == NULL)
	return;

    int motifIntValue;

    motifIntValue = convertFloatToSlider( theVal );
    XmScaleSetValue( widget, motifIntValue );
}

void
SoXtMinMaxSlider::setSliderMin( float theMin )
{
    if (widget == NULL)
	return;

    Arg wargs[10];
    int motifIntValue;

    motifIntValue = convertFloatToSlider( theMin );
    XtSetArg( wargs[0], XmNminimum, motifIntValue );
    XtSetValues( widget, wargs, 1 );
}

void
SoXtMinMaxSlider::setSliderMax( float theMax )
{
    if (widget == NULL)
	return;

    Arg wargs[10];
    int motifIntValue;

    motifIntValue = convertFloatToSlider( theMax );
    XtSetArg( wargs[0], XmNmaximum, motifIntValue );
    XtSetValues( widget, wargs, 1 );
}

void
SoXtMinMaxSlider::sliderCallback( Widget, void *client_data, void * )
{
    float        theVal;
    SoXtMinMaxSlider      *obj    = (SoXtMinMaxSlider *) client_data;

    /* Get the value from the slider */
    theVal = obj->getSliderValue();

    // If you can find a SliderTool in the ancestry...
    if ( obj->_sliderTool ) {
	/* Set the value throughout the SliderTool */
	obj->_sliderTool->toolSetValue( theVal );
    }

    // If you can find a SoMultiSlider in the ancestry...
    if ( obj->_multiSlider )
	obj->_multiSlider->exportValuesToInventor(); // send the values out
}
