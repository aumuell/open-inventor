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
|   Classes:
|      NoodleSlider is a component that expands the functionality of the
|      standard motif slider (they call it 'scale') widget.
|
|   Author(s)          : Paul Isaacs
|
*/



#include "NoodleSlider.h"
#include <X11/StringDefs.h>
#include <Xm/Form.h>
#include <Xm/Text.h>
#include <Xm/Scale.h>

NoodleSlider::NoodleSlider(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent, 
    int startingMin,
    int startingMax)
	: SoXtComponent(
	    parent,
	    name, 
	    buildInsideParent)
{
    // Build the widget!
    Widget w = buildWidget( getParentWidget(), startingMin, startingMax);
    setBaseWidget(w);

    valueChangedCallbacks = new SoCallbackList;
}

NoodleSlider::~NoodleSlider()
{
    delete valueChangedCallbacks;
}

Widget
NoodleSlider::buildWidget( 
    Widget parent,
    int startingMin,
    int startingMax )
{
    Arg wargs[10];
    int  nargs;

    _form = XtCreateManagedWidget(getWidgetName(),xmFormWidgetClass,
					parent,NULL, 0);

    _value = XtCreateManagedWidget("value",xmTextWidgetClass,_form,NULL,0);

    nargs = 0;
    XtSetArg(wargs[nargs],XmNorientation,    XmHORIZONTAL ); nargs++;
    XtSetArg(wargs[nargs],XmNdecimalPoints,  2 ); nargs++;
    XtSetArg(wargs[nargs],XmNminimum,        startingMin ); nargs++;
    XtSetArg(wargs[nargs],XmNmaximum,        startingMax ); nargs++;
    _slider = XtCreateManagedWidget("slider",xmScaleWidgetClass,
				    _form,wargs,nargs);

    /* set the callbacks for the sub components */
    XtAddCallback( _value, XmNactivateCallback,
		   (XtCallbackProc) &NoodleSlider::valueWidgetCallback,
		   (XtPointer) this );
    XtAddCallback( _slider, XmNdragCallback,
		    (XtCallbackProc) &NoodleSlider::sliderWidgetCallback,
		    (XtPointer) this );
    XtAddCallback( _slider, XmNvalueChangedCallback,
		    (XtCallbackProc) &NoodleSlider::sliderWidgetCallback,
		    (XtPointer) this );

    /* take care of layout within the NoodleSlider */
    initLayout();

    /* initialize values throughout the NoodleSlider to be those in the scale */
    /* Right now, this winds up filling the label widgets with text    */
    setValue( getValue());
    setMin( getMin());
    setMax( getMax());
    return _form;
}

void
NoodleSlider::initLayout()
{
    if (_form == NULL)
	return;

    Arg wargs[10];
    int  nargs;
    nargs = 0;
    XtSetArg(wargs[nargs],XmNborderWidth,    2 ); nargs++;
    XtSetValues( _form, wargs, nargs );

    nargs = 0;
    XtSetArg(wargs[nargs],XmNtopAttachment,    XmATTACH_FORM ); nargs++;
    XtSetArg(wargs[nargs],XmNbottomAttachment, XmATTACH_POSITION ); nargs++;
    XtSetArg(wargs[nargs],XmNbottomPosition,   100 ); nargs++;
    XtSetArg(wargs[nargs],XmNleftAttachment,   XmATTACH_POSITION ); nargs++;
    XtSetArg(wargs[nargs],XmNleftPosition,     0 ); nargs++;
    XtSetArg(wargs[nargs],XmNrightAttachment,  XmATTACH_POSITION ); nargs++;
    XtSetArg(wargs[nargs],XmNrightPosition,    30 ); nargs++;
    XtSetValues( _value, wargs, nargs );

    nargs = 0;
    XtSetArg(wargs[nargs],XmNtopAttachment,    XmATTACH_POSITION ); nargs++;
    XtSetArg(wargs[nargs],XmNtopPosition,      0 ); nargs++;
    XtSetArg(wargs[nargs],XmNbottomAttachment, XmATTACH_FORM ); nargs++;
    XtSetArg(wargs[nargs],XmNleftAttachment,   XmATTACH_POSITION ); nargs++;
    XtSetArg(wargs[nargs],XmNleftPosition,     30 ); nargs++;
    XtSetArg(wargs[nargs],XmNrightAttachment,  XmATTACH_POSITION ); nargs++;
    XtSetArg(wargs[nargs],XmNrightPosition,    100 ); nargs++;
    XtSetArg(wargs[nargs],XmNtitleString,      (void *) NULL ); nargs++;
    XtSetValues( _slider, wargs, nargs );
}

void 
NoodleSlider::setValue( float newVal )
{
    if ( getMin() > newVal )
	setMin( newVal );
    if ( getMax() < newVal )
	setMax( newVal );

    /* send the value to the slider */
    if (_slider) {
	int motifIntValue;
	motifIntValue = convertFloatToSlider( newVal );
	XmScaleSetValue( _slider, motifIntValue );
    }

    /* send the value to the label */
    char valString[50];
    sprintf( valString, "%.*f", getNumDecimals(), newVal );
    XmTextSetString( _value, valString );
}

void 
NoodleSlider::setMin( float newVal )
{
    float valToUse;

    if ( newVal <= getValue() 
	 && newVal < getMax() )
	valToUse = newVal;
    else
	valToUse = getMin();

    /* send the new minimum value to the slider */
    if ( _slider == NULL )
	return;

    Arg wargs[10];
    int motifIntValue;

    motifIntValue = convertFloatToSlider( valToUse );
    XtSetArg( wargs[0], XmNminimum, motifIntValue );
    XtSetValues( _slider, wargs, 1 );
}

void 
NoodleSlider::setMax( float newVal )
{
    float valToUse;

    if ( newVal >= getValue() 
	&& newVal > getMin() )
	valToUse = newVal;
    else
	valToUse = getMax();

    /* send the new maximum value to the slider */
    if ( _slider == NULL )
	return;

    Arg wargs[10];
    int motifIntValue;

    motifIntValue = convertFloatToSlider( valToUse );
    XtSetArg( wargs[0], XmNmaximum, motifIntValue );
    XtSetValues( _slider, wargs, 1 );
}

float 
NoodleSlider::getValue()
{
    if ( _slider == NULL )
	return 0.0;

    Arg wargs[10];
    int motifIntValue;

    XtSetArg( wargs[0], XmNvalue, &motifIntValue );
    XtGetValues( _slider, wargs, 1 );

    return( convertSliderToFloat( motifIntValue ));
}

float 
NoodleSlider::getMin()
{
    if ( _slider == NULL )
	return 0.0;

    Arg wargs[10];
    int motifIntValue;

    XtSetArg( wargs[0], XmNminimum, &motifIntValue );
    XtGetValues( _slider, wargs, 1 );

    return( convertSliderToFloat( motifIntValue ));
}


float 
NoodleSlider::getMax()
{
    if ( _slider == NULL )
	return 0.0;

    Arg wargs[10];
    int motifIntValue;

    XtSetArg( wargs[0], XmNmaximum, &motifIntValue );
    XtGetValues( _slider, wargs, 1 );

    return( convertSliderToFloat( motifIntValue ));
}

short
NoodleSlider::getNumDecimals()
{
    if (_slider == NULL)
	return 0;
    Arg wargs[10];
    short decimals;
    
    XtSetArg( wargs[0], XmNdecimalPoints, &decimals);
    XtGetValues( _slider, wargs, 1 );

    return decimals;
}

float
NoodleSlider::convertSliderToFloat( int sliderValue )
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
NoodleSlider::convertFloatToSlider( float floatValue )
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


void
NoodleSlider::valueWidgetCallback( Widget, void *client_data, void *)
{
    float    theVal;
    NoodleSlider *theNdlSlider = (NoodleSlider *) client_data;
    Arg wargs[10];
    char *valString;

    if (theNdlSlider && theNdlSlider->_value ) {
	// get text value from the label 
	XtSetArg( wargs[0], XmNvalue, &valString );
	XtGetValues( theNdlSlider->_value, wargs, 1 );
	if ( sscanf( valString, "%f", &theVal ) ) {
	    // set the value throughout the NoodleSlider
	    theNdlSlider->setValue( theVal );
	}
	theNdlSlider->valueChangedCallbacks->invokeCallbacks(theNdlSlider);
    }
}
void
NoodleSlider::sliderWidgetCallback( Widget, void *client_data, void *)
{
    float theVal;
    NoodleSlider *theNdlSlider = (NoodleSlider *) client_data;

    // Get the value from the slider
    theVal = theNdlSlider->getValue();

    theNdlSlider->setValue(theVal);

    theNdlSlider->valueChangedCallbacks->invokeCallbacks(theNdlSlider);
}

void 
NoodleSlider::addValueChangedCallback(SoCallbackListCB *f, void *userData )
{ valueChangedCallbacks->addCallback( f, userData ); }

void 
NoodleSlider::removeValueChangedCallback(SoCallbackListCB *f, void *userData )
{ valueChangedCallbacks->removeCallback( f, userData ); }


