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
|   $Revision: 1.2 $
|
|   Classes:
|      SoXtMMSliderMin is a component that expands the functionality of the
|      standard motif slider (they call it 'scale') widget.
|
|   Author(s)          : Paul Isaacs
|
______________  S I L I C O N   G R A P H I C S   I N C .  ____________
_______________________________________________________________________
*/



#include <inttypes.h>
#include "SoXtSliderTool.h"
#include "SoXtMMSliderMin.h"
#include <X11/StringDefs.h>
#include <Xm/PushB.h>

/***************************************************************************/
/* static variables used by XtGetApplicationResources to get values of     */
/* user-settable parameters.  The defaults values are used if the user     */
/* does not override them.                                                 */
/***************************************************************************/
static XtResource _minLabelResources[] = {
    { XmNlabelString, XmCLabelString, XmRXmString, sizeof( char *),
      0,
      XtRString, (XtPointer) DEFAULT_SLIDER_TOOL_MIN_LABEL }  
};

SoXtMMSliderMin::SoXtMMSliderMin(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent)
	: SoXtSliderSetBase(
	    parent,
	    name, 
	    buildInsideParent,
	    NULL)
{
    _sliderTool = NULL;

    _layoutWidth  = 75;
    _layoutHeight = 40;

    // Build the widget!
    Widget w = buildWidget(getParentWidget());
    setBaseWidget(w);
}

SoXtMMSliderMin::~SoXtMMSliderMin() {}

Widget
SoXtMMSliderMin::buildWidget( Widget parent )
{
    Arg wargs[10];
    XmString minString;

    widget = XtCreateManagedWidget(getWidgetName(),xmPushButtonWidgetClass,
					parent,NULL, 0);

    /* establish all the callbacks */
    XtAddCallback( widget, XmNactivateCallback,
		    (XtCallbackProc) &SoXtMMSliderMin::minCallback,
		    (XtPointer) this );

    /* give the button a title string */
    XtGetApplicationResources( widget, (XtPointer) &minString, 
	    	       _minLabelResources, XtNumber( _minLabelResources), 
		       NULL, 0 );
    XtSetArg(wargs[0],XmNlabelString,      minString );
    XtSetValues( widget, wargs, 1 );
    return widget;
}

void
SoXtMMSliderMin::minCallback( Widget, void *client_data, void * )
{
    float      theVal;
    SoXtMinMaxSlider     *theSlider;
    SoXtMMSliderMin  *obj = (SoXtMMSliderMin *) client_data;

    if (obj->_sliderTool)
	theSlider = obj->_sliderTool->getSlider();

    /* Get the value from the slider */
    if (theSlider)
        theVal = theSlider->getSliderValue();

    /* Set this to be the new MINIMUM value throughout the SliderTool */
    if (obj->_sliderTool)
        obj->_sliderTool->toolSetMin( theVal );
}
