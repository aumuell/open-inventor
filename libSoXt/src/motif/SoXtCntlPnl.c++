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
|      Control panel, used as part of a 'sliderModule'
|
|   Author(s)          : Paul Isaacs
|
______________  S I L I C O N   G R A P H I C S   I N C .  ____________
_______________________________________________________________________
*/


#include <inttypes.h>
#include "SoXtSliderControlPanel.h"
#include "SoXtSliderModule.h"
#include "SoXtSliderTool.h"
#include <Inventor/Xt/SoXtSliderSet.h>
#include <X11/StringDefs.h>
#include <Xm/Form.h>
#include <Xm/ToggleB.h>
#include <Xm/PushB.h>

static XtResource _openCloseResources[] = {
    { XmNrightPosition, XmCPosition, XtRInt, sizeof( int),
      0, XtRImmediate, (XtPointer) DEFAULT_SLIDER_CONTROL_PANEL_DIVIDING_POINT }
};

static XtResource _styleLabelResource[] = {
    { XmNlabelString, XmCLabelString, XmRXmString, sizeof( char *),
      0, XtRString, (XtPointer) DEFAULT_SLIDER_CONTROL_PANEL_STYLE_LABEL }
};

SoXtSliderControlPanel::SoXtSliderControlPanel(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent)
	: SoXtSliderSetBase(
	    parent,
	    name, 
	    buildInsideParent,
	    NULL)
{
    _openCloseButton = NULL;
    _styleButton = NULL;
    _dividingPoint = 0;
    _layoutWidth = 310;
    _layoutHeight = 40; 
    _sliderModule = NULL;
    _editor = NULL;
    _style = SLIDER_TOOL_STYLE_SKINNY_CLOSED;
    
    // Build that widget!
    Widget w = buildWidget(getParentWidget());
    setBaseWidget(w);
}

SoXtSliderControlPanel::~SoXtSliderControlPanel() {}

Widget
SoXtSliderControlPanel::buildWidget( Widget parent )
{
    Arg  wargs[10];

    widget = XtCreateManagedWidget(getWidgetName(),xmFormWidgetClass,
					parent,NULL, 0);
    
    XtSetArg( wargs[0], XmNshadowThickness, 2 ); 
    _openCloseButton = XtCreateManagedWidget("openCloseButton",
					xmToggleButtonWidgetClass,
					widget,wargs, 1);
    XtSetArg( wargs[0], XmNshadowThickness, 1 ); 
    _styleButton = XtCreateManagedWidget("styleButton",
					xmPushButtonWidgetClass,
					widget,wargs, 1);

    initLayout();

    XtAddCallback( _openCloseButton, XmNvalueChangedCallback,
		    (XtCallbackProc)&SoXtSliderControlPanel::openCloseCallback, 
		    (XtPointer) this);
    XtAddCallback( _styleButton, XmNactivateCallback,
		    (XtCallbackProc)&SoXtSliderControlPanel::styleCallback, 
		    (XtPointer) this);
    return widget;
}

void
SoXtSliderControlPanel::openUp()
{
    Arg wargs[10];
    int  nargs;

    // align right edge of button with dissection pt.
    nargs = 0;
    XtSetArg(wargs[nargs], XmNrightPosition, _dividingPoint ); nargs++;
    XtSetValues(_openCloseButton, wargs, nargs);

    XtManageChild(_styleButton);
}

void
SoXtSliderControlPanel::closeDown()
{
    Arg wargs[10];
    int  nargs;

    // attach to right side of window
    nargs = 0;
    XtSetArg(wargs[nargs], XmNrightPosition, 100 ); nargs++;
    XtSetValues(_openCloseButton, wargs, nargs);

    XtUnmanageChild(_styleButton);
}

void 
SoXtSliderControlPanel::openCloseCallback( Widget, void *client_data, void *)
{
    SoXtSliderControlPanel* obj = (SoXtSliderControlPanel *) client_data;
    if (obj->_sliderModule) {

	// Insure that module is configured properly
	if ( obj->getStyle() == SLIDER_TOOL_STYLE_SKINNY_CLOSED )
	    obj->_sliderModule->makeSkinnyClosed();
	else if ( obj->getStyle() == SLIDER_TOOL_STYLE_SKINNY_OPEN )
	    obj->_sliderModule->makeSkinnyOpen();
	else if ( obj->getStyle() == SLIDER_TOOL_STYLE_OPEN )
	    obj->_sliderModule->openMinMax();
	else
	    obj->_sliderModule->closeMinMax();

	// Open or close as appropriate.
	if (XmToggleButtonGetState( obj->_openCloseButton ) )
	    obj->_sliderModule->openUp();
	else
	    obj->_sliderModule->closeDown();
    }
    if (obj->_editor )
	obj->_editor->updateLayout();
}

void 
SoXtSliderControlPanel::styleCallback( Widget, void *client_data, void *)
{
    SoXtSliderControlPanel* obj = (SoXtSliderControlPanel *) client_data;
    if (obj->_sliderModule) {

	if ( obj->getStyle() == SLIDER_TOOL_STYLE_SKINNY_CLOSED ) {
	    obj->_sliderModule->makeSkinnyOpen();
	    obj->setStyle( SLIDER_TOOL_STYLE_SKINNY_OPEN );
	}
	else if ( obj->getStyle() == SLIDER_TOOL_STYLE_SKINNY_OPEN ) {
	    obj->_sliderModule->openMinMax();
	    obj->setStyle( SLIDER_TOOL_STYLE_OPEN );
	}
	else if ( obj->getStyle() == SLIDER_TOOL_STYLE_OPEN ) {
	    obj->_sliderModule->closeMinMax();
	    obj->setStyle( SLIDER_TOOL_STYLE_CLOSED );
	}
	else {
	    obj->_sliderModule->makeSkinnyClosed();
	    obj->setStyle( SLIDER_TOOL_STYLE_SKINNY_CLOSED );
	}
    }
    if (obj->_editor )
	obj->_editor->updateLayout();
}

void
SoXtSliderControlPanel::initLayout()
{
    Arg wargs[10];
    int  nargs;
    XmString newString;

    /* see if the dividing point resource has been specified. If so, change
       the value of _dividingPoint to match it
    */ 
    XtGetApplicationResources( _openCloseButton, (XtPointer) &_dividingPoint,
			_openCloseResources, XtNumber( _openCloseResources),
			NULL, 0 );
    nargs = 0;
    XtSetArg(wargs[nargs],XmNtopAttachment,    XmATTACH_FORM ); nargs++;
    XtSetArg(wargs[nargs],XmNbottomAttachment, XmATTACH_FORM ); nargs++;
    XtSetArg(wargs[nargs],XmNleftAttachment,   XmATTACH_FORM ); nargs++;
    XtSetArg(wargs[nargs],XmNrightAttachment,  XmATTACH_POSITION ); nargs++;
    XtSetArg(wargs[nargs],XmNrightPosition,    _dividingPoint ); nargs++;
    XtSetValues( _openCloseButton, wargs, nargs );

    XtGetApplicationResources( _styleButton, (XtPointer) &newString,
			_styleLabelResource, XtNumber( _styleLabelResource),
			NULL, 0 );
    nargs = 0;
    XtSetArg(wargs[nargs],XmNtopAttachment,    XmATTACH_FORM ); nargs++;
    XtSetArg(wargs[nargs],XmNbottomAttachment, XmATTACH_FORM ); nargs++;
    XtSetArg(wargs[nargs],XmNleftAttachment,   XmATTACH_WIDGET ); nargs++;
    XtSetArg(wargs[nargs],XmNleftWidget,     _openCloseButton ); nargs++;
    XtSetArg(wargs[nargs],XmNrightAttachment,  XmATTACH_FORM ); nargs++;

    XtSetArg(wargs[nargs],XmNlabelString, newString ); nargs++;
    XtSetValues( _styleButton, wargs, nargs );
}
