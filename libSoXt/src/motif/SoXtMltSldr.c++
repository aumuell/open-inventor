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
|      Multiple slider within one container for grouping values
|
|   Author(s)          : Paul Isaacs
|
______________  S I L I C O N   G R A P H I C S   I N C .  ____________
_______________________________________________________________________
*/



#include "SoXtMultiSlider.h"
#include "SoXtSliderTool.h"
#include <X11/StringDefs.h>
#include <Xm/Form.h>
#include <Inventor/errors/SoDebugError.h>

////////////////////////////////////////////////////////////////////////
//
// Public constructor - build the widget right now
//
SoXtMultiSlider::SoXtMultiSlider(
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

////////////////////////////////////////////////////////////////////////
//
// SoEXTENDER constructor - the subclass tells us whether to build or not
//
SoXtMultiSlider::SoXtMultiSlider(
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
SoXtMultiSlider::constructorCommon(SbBool buildNow)
//
//////////////////////////////////////////////////////////////////////
{
    addVisibilityChangeCallback(visibilityChangeCB, this);
    inventorSensor = NULL;

    // Build the widget tree, and let SoXtComponent know about our base widget.
    if (buildNow) {
	Widget w = buildWidget(getParentWidget());
	setBaseWidget(w);
    }
}

SoXtMultiSlider::~SoXtMultiSlider()
{
    if (inventorSensor != NULL)
	delete inventorSensor;
}

// Subclasses should call this to build a form,
// then create their _subComponentArray, then 
// call buildWidget.
Widget
SoXtMultiSlider::buildForm(Widget parent)
{
    widget = XtCreateManagedWidget(
	    getWidgetName(), xmFormWidgetClass, parent, NULL, 0);
	    
    return widget;
}

Widget
SoXtMultiSlider::buildWidget( Widget parent )
{
    Arg wargs[10];
    int nargs, topPos, bottomPos;
    Widget theContainer;

#ifdef DEBUG
    // sanity check
    if (widget != parent)
	fprintf(stderr, "INTERNAL ERROR SoXtMultiSlider::buildWidget - widget != parent\n");
#endif

    nargs = 0;
    //???XtSetArg( wargs[nargs], XmNtopAttachment,    XmATTACH_POSITION); nargs++;
    XtSetArg( wargs[nargs], XmNtopAttachment,    XmATTACH_FORM); nargs++;
    XtSetArg( wargs[nargs], XmNbottomAttachment, XmATTACH_FORM); nargs++;
    XtSetArg( wargs[nargs], XmNleftAttachment,   XmATTACH_FORM); nargs++;
    XtSetArg( wargs[nargs], XmNrightAttachment,  XmATTACH_FORM); nargs++;
    XtSetValues( widget, wargs, nargs );

    // set sizes of the subComponents 
    for ( int i = 0; i < _numSubComponents; i++ ) {

	// set its relative size in the container
	theContainer = ((SoXtSliderTool *) _subComponentArray[i])->getContainer();
	topPos    = (int) (100.0 * ((float) i      /(float) _numSubComponents));
	bottomPos = (int) (100.0 * ((float) (i + 1)/(float) _numSubComponents));
	nargs = 0;
	XtSetArg( wargs[nargs], XmNtopAttachment, XmATTACH_POSITION); nargs++;
	XtSetArg( wargs[nargs], XmNtopPosition, topPos ); nargs++;
	XtSetArg( wargs[nargs], XmNbottomAttachment, XmATTACH_POSITION);nargs++;
	XtSetArg( wargs[nargs], XmNbottomPosition, bottomPos ); nargs++;
	XtSetArg( wargs[nargs], XmNleftAttachment, XmATTACH_FORM); nargs++;
	XtSetArg( wargs[nargs], XmNrightAttachment, XmATTACH_FORM); nargs++;
	XtSetValues( theContainer, wargs, nargs );
    }

    importValuesFromInventor();
    makeSensor();

    return widget;
}

void
SoXtMultiSlider::setNode( SoNode *newNode )
{
    if ((newNode != NULL) && (! validNodeType(newNode))) {
#ifdef DEBUG
	SoDebugError::post("SoXtMultiSlider::setNode",
		"- node is not valid type\n");
#endif
    	return;
    }
	
    if (newNode)            // ref the input
	newNode->ref();

    if (_editNode)          // out with the old
	_editNode->unref();

    _editNode = newNode;
    for( int i = 0; i < _numSubComponents; i++ )
	_subComponentArray[i]->setNode( newNode );

    /* connect new node */
    makeSensor();

    /* copy values from node into slider */
    if (widget)
        importValuesFromInventor();
}

// sensor callback
void
SoXtMultiSlider::inventorCallback( SoXtMultiSlider *slider, SoSensor * )
{
    if (!slider->isVisible())
	return;
    
    slider->importValuesFromInventor(); 
}

void 
SoXtMultiSlider::makeSensor()
{
    if ( widget && _editNode) {
	if ( !inventorSensor ) {
	    inventorSensor = new SoNodeSensor((SoSensorCB *)
					      &SoXtMultiSlider::inventorCallback,
					      this);
        }
	deactivate();
	activate();
    }
}

//
// called whenever the component becomes visibble or not
//
void
SoXtMultiSlider::visibilityChangeCB(void *pt, SbBool visible)
{
    SoXtMultiSlider *p = (SoXtMultiSlider *)pt;
    
    if (visible)
	p->activate();
    else
	p->deactivate();
}


////////////////////////////////////////////////////////////////////////
//
// Attach the sensor (if necessary).
//
void
SoXtMultiSlider::activate()
//
// use: private
////////////////////////////////////////////////////////////////////////
{
    // attach sensor to top node for redrawing purpose
    if ( _editNode 
	 && inventorSensor && inventorSensor->getAttachedNode() == NULL) {
        importValuesFromInventor();
	inventorSensor->attach(_editNode);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Detach the sensor.
//
void
SoXtMultiSlider::deactivate()
//
// use: private
////////////////////////////////////////////////////////////////////////
{
    // detach sensor
    if (inventorSensor != NULL && inventorSensor->getAttachedNode() != NULL) {
	inventorSensor->detach();
    }
}

void
SoXtMultiSlider::openMinMax()
{
    // openMinMax each slider
    for ( int i = 0; i < _numSubComponents; i++ )
        ((SoXtSliderTool *) _subComponentArray[i])->openMinMax();
}

void
SoXtMultiSlider::closeMinMax()
{
    // closeMinMax each slider
    for ( int i = 0; i < _numSubComponents; i++ )
        ((SoXtSliderTool *) _subComponentArray[i])->closeMinMax();
}

void
SoXtMultiSlider::makeSkinnyOpen()
{
    // treat each slider
    for ( int i = 0; i < _numSubComponents; i++ )
        ((SoXtSliderTool *) _subComponentArray[i])->makeSkinnyOpen();
}

void
SoXtMultiSlider::makeSkinnyClosed()
{
    // treat each slider
    for ( int i = 0; i < _numSubComponents; i++ )
        ((SoXtSliderTool *) _subComponentArray[i])->makeSkinnyClosed();
}

void 
SoXtMultiSlider::getLayoutSize( int &w, int &h )
{
    int tempW, tempH;

    _layoutWidth = 0;
    _layoutHeight = 0;
    for ( int i = 0; i < _numSubComponents; i++ ) {
        _subComponentArray[i]->getLayoutSize( tempW, tempH );
	_layoutWidth += tempW;
	_layoutHeight += tempH;
    }
    w = _layoutWidth;
    h = _layoutHeight;
}

static XtResource _titleResource[] = {
    { XmNtitleString, XmCTitleString, XmRXmString, sizeof( XmString ),
      0, XtRString, (XtPointer) NULL }
};

void
SoXtMultiSlider::initTitles( char **titleList )
{
    Arg            wargs[10];
    XmString       newString;
    Widget         theWidget;
    SoXtMinMaxSlider *theSlider;

    for ( int i = 0; i < _numSubComponents; i++ ) {
	theSlider = ((SoXtSliderTool *) _subComponentArray[i])->getSlider();
	theWidget = theSlider->getWidget(); 

	_titleResource[0].default_addr = (XtPointer) titleList[i];
	XtGetApplicationResources( theWidget, (XtPointer) &newString,
	      _titleResource, XtNumber( _titleResource ), NULL, 0 );

	XtSetArg( wargs[0], XmNtitleString, newString );
	XtSetValues( theWidget, wargs, 1 );
	XmStringFree(newString);
    }
}


