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
|   $Revision: 1.1.1.1 $
|
|   Classes:
|      A grouping of a control panel and a multi slider that work together
|      to control a set of related parameters.
|
|   Author(s)          : Paul Isaacs
|
______________  S I L I C O N   G R A P H I C S   I N C .  ____________
_______________________________________________________________________
*/


#include <inttypes.h>
#include "SoXtSliderModules.h"
#include "SoXtSliderTool.h"
#include <Xm/Form.h>
#include <Xm/ToggleB.h>

////////////////////////////////////////////////////////////////////////
//
// Public constructor - build the widget right now
//
SoXtSliderModule::SoXtSliderModule(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent)
	: SoXtSliderSetBase(
	    parent,
	    name, 
	    buildInsideParent,
	    NULL)
//
////////////////////////////////////////////////////////////////////////
{
    // In this case, this component is what the app wants, so buildNow = TRUE
    constructorCommon(TRUE);
}

SoXtSliderModule::~SoXtSliderModule()
{
    if (_sliderModuleSliders != NULL)
	delete _sliderModuleSliders;
    if (_sliderModuleControl != NULL)
	delete _sliderModuleControl;
}

////////////////////////////////////////////////////////////////////////
//
// SoEXTENDER constructor - the subclass tells us whether to build or not
//
SoXtSliderModule::SoXtSliderModule(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent, 
    SbBool buildNow)
	: SoXtSliderSetBase(
	    parent,
	    name, 
	    buildInsideParent,
	    NULL)
//
////////////////////////////////////////////////////////////////////////
{
    // In this case, this component may be what the app wants, 
    // or it may want a subclass of this component. Pass along buildNow
    // as it was passed to us.
    constructorCommon(buildNow);
}

///////////////////////////////////////////////////////////////////////
//
// 	Called by the constructors.
//
// Use: public
void
SoXtSliderModule::constructorCommon(SbBool buildNow)
//
//////////////////////////////////////////////////////////////////////
{
    _isOpenedUp   = FALSE;

    _editor = NULL;
    _sliderModuleSliders    = NULL;

    _sliderModuleControl   = NULL;

    // Build the widget tree, and let SoXtComponent know about our base widget.
    if (buildNow) {
	Widget w = buildWidget(getParentWidget());
	setBaseWidget(w);
    }
}

// Subclasses should call this to build a form,
// then create their _sliderModuleSliders, then 
// call buildWidget.
Widget
SoXtSliderModule::buildForm(Widget parent)
{
    widget = XtCreateManagedWidget(getWidgetName(),
		    xmFormWidgetClass, parent, NULL, 0);
    return widget;
}

Widget
SoXtSliderModule::buildWidget( Widget )
{	
    _sliderModuleControl = new  SoXtSliderControlPanel( widget );
    _sliderModuleControl->setSliderModule( this);

    if ( XmToggleButtonGetState( _sliderModuleControl->getOpenCloseButton() ))
	openUp();
    else
	closeDown();

    if ( _sliderModuleControl->getStyle() == SLIDER_TOOL_STYLE_CLOSED )
	closeMinMax();
    else if ( _sliderModuleControl->getStyle() == SLIDER_TOOL_STYLE_OPEN )
	openMinMax();
    else if ( _sliderModuleControl->getStyle() == SLIDER_TOOL_STYLE_SKINNY_OPEN)
	makeSkinnyOpen();
    else
	makeSkinnyClosed();

    return widget;
}

void
SoXtSliderModule::splitFormBetweenPanelAndSliders()
{
    Arg wargs[10];
    int  nargs;

    int cntlW, cntlH, sldrW, sldrH, splitPoint;

    _sliderModuleControl->getLayoutSize( cntlW, cntlH );
    _sliderModuleSliders->getLayoutSize( sldrW, sldrH );

    splitPoint = (int) (100.0 * ((float) cntlH / (float) (cntlH + sldrH)));

    nargs = 0;
    XtSetArg(wargs[nargs],XmNtopAttachment, XmATTACH_FORM ); nargs++;
    XtSetArg(wargs[nargs],XmNbottomAttachment, XmATTACH_POSITION ); nargs++;
    XtSetArg(wargs[nargs],XmNbottomPosition, splitPoint ); nargs++;
    XtSetArg(wargs[nargs],XmNleftAttachment, XmATTACH_FORM ); nargs++;
    XtSetArg(wargs[nargs],XmNrightAttachment, XmATTACH_FORM ); nargs++;
    XtSetValues(_sliderModuleControl->getControlPanelContainer(), wargs, nargs);

    nargs = 0;
    XtSetArg(wargs[nargs],XmNtopAttachment, XmATTACH_POSITION ); nargs++;
    XtSetArg(wargs[nargs],XmNtopPosition, splitPoint ); nargs++;
    XtSetArg(wargs[nargs],XmNbottomAttachment, XmATTACH_FORM ); nargs++;
    XtSetArg(wargs[nargs],XmNleftAttachment, XmATTACH_FORM ); nargs++;
    XtSetArg(wargs[nargs],XmNrightAttachment, XmATTACH_FORM ); nargs++;
    XtSetValues(_sliderModuleSliders->getContainer(), wargs, nargs);
}

void
SoXtSliderModule::fillFormWithPanel()
{
    Arg wargs[10];
    int  nargs;

    nargs = 0;
    XtSetArg(wargs[nargs],XmNtopAttachment, XmATTACH_FORM ); nargs++;
    XtSetArg(wargs[nargs],XmNbottomAttachment, XmATTACH_FORM ); nargs++;
    XtSetArg(wargs[nargs],XmNleftAttachment, XmATTACH_FORM ); nargs++;
    XtSetArg(wargs[nargs],XmNrightAttachment, XmATTACH_FORM ); nargs++;
    XtSetValues(_sliderModuleControl->getControlPanelContainer(), wargs, nargs);
}

void
SoXtSliderModule::openUp()
{
    _isOpenedUp = TRUE;
    splitFormBetweenPanelAndSliders();
    _sliderModuleSliders->show();
    _sliderModuleControl->openUp();
}

void
SoXtSliderModule::closeDown()
{
    _isOpenedUp = FALSE;
    fillFormWithPanel();
    _sliderModuleSliders->hide();
    _sliderModuleControl->closeDown();
}

void
SoXtSliderModule::openMinMax()
{
    _sliderModuleSliders->openMinMax();
    if ( _isOpenedUp )
	splitFormBetweenPanelAndSliders();
}

void
SoXtSliderModule::closeMinMax()
{
    _sliderModuleSliders->closeMinMax();
    if ( _isOpenedUp )
	splitFormBetweenPanelAndSliders();
}

void
SoXtSliderModule::makeSkinnyOpen()
{
    _sliderModuleSliders->makeSkinnyOpen();
    if ( _isOpenedUp )
	splitFormBetweenPanelAndSliders();
}

void
SoXtSliderModule::makeSkinnyClosed()
{
    _sliderModuleSliders->makeSkinnyClosed();
    if ( _isOpenedUp )
	splitFormBetweenPanelAndSliders();
}

void 
SoXtSliderModule::setNode( SoNode *newNode )
{
    if (newNode)            // ref the new before unref'ing the old
	newNode->ref();

    if (_editNode)          // out with the old
	_editNode->unref();

    _editNode = newNode;
    for( int i = 0; i < _numSubComponents; i++ )
	_subComponentArray[i]->setNode( newNode );

    _sliderModuleSliders->setNode( newNode );
}

void
SoXtSliderModule::getLayoutSize( int &w, int &h )
{
    int tempW, tempH;

    _layoutWidth = 0;
    _layoutHeight = 0;

    _sliderModuleControl->getLayoutSize( tempW, tempH );
    _layoutWidth += tempW;
    _layoutHeight += tempH;

    if ( _isOpenedUp ) {
	_sliderModuleSliders->getLayoutSize( tempW, tempH );
	_layoutWidth += tempW;
	_layoutHeight += tempH;
    }

    w = _layoutWidth;
    h = _layoutHeight;
}

void
SoXtSliderModule::setEditor( SoXtSliderSet *newOne )
{
    _editor = newOne;
    _sliderModuleControl->setEditor( newOne );
}
