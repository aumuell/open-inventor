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
|      Subclass of SoXtSliderSet that acts on a light node
|      SoXtLightSliderSet
|
|   Author(s)          : Paul Isaacs
|
______________  S I L I C O N   G R A P H I C S   I N C .  ____________
_______________________________________________________________________
*/

//--------------------------------Include------------------------------
#include <inttypes.h>
#include "SoXtSliderModules.h"
#include <Inventor/Xt/SoXtLightSliderSet.h>
#include <X11/StringDefs.h>
#include <Xm/Form.h>

static XtResource _borderWidthResource[] = {
    { XtNborderWidth, XtCBorderWidth, XtRDimension, sizeof( unsigned short),
      0,  XtRImmediate, (XtPointer) SLIDER_SET_DEFAULT_BORDER_WIDTH }
};


////////////////////////////////////////////////////////////////////////
//
// Public constructor - build the widget right now
//
SoXtLightSliderSet::SoXtLightSliderSet(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent,
    SoNode *newEditNode)
	: SoXtSliderSet(
	    parent,
	    name, 
	    buildInsideParent,
	    newEditNode)
//
////////////////////////////////////////////////////////////////////////
{
    // In this case, this component is what the app wants, so buildNow = TRUE
    constructorCommon(TRUE);
}

SoXtLightSliderSet::~SoXtLightSliderSet() {}

////////////////////////////////////////////////////////////////////////
//
// SoEXTENDER constructor - the subclass tells us whether to build or not
//
SoXtLightSliderSet::SoXtLightSliderSet(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent, 
    SoNode *newEditNode,
    SbBool buildNow)
	: SoXtSliderSet(
	    parent,
	    name, 
	    buildInsideParent,
	    newEditNode)
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
// Description:
// 	Called by the constructors.
//	Constructor for the SoXtLightSliderSet.  Creates the 5 slider modules
//      that make up the editor. The base class constructor will create
//      the container widget, etc.
//      This constructor gives a default name.
// Use: public
void
SoXtLightSliderSet::constructorCommon(SbBool buildNow)
//
//////////////////////////////////////////////////////////////////////
{
    // Create the two modules used by this editor.
    _numSubComponents = 2;
    _subComponentArray = new ( SoXtSliderSetBase * [ _numSubComponents ] );

    _subComponentArray[0] = NULL;
    _subComponentArray[1] = NULL;

    // Build the widget tree, and let SoXtComponent know about our base widget.
    if (buildNow) {
	Widget w = buildWidget(getParentWidget());
	setBaseWidget(w);
    }
}

Widget
SoXtLightSliderSet::buildWidget( Widget parent)
{
    Arg  wargs[10];
    Dimension newBorderWidth;
    Widget theWidget;

    _parentShellWidget = parent;

    widget = XtCreateManagedWidget(getWidgetName(),xmFormWidgetClass, parent,NULL, 0);

    _subComponentArray[0] = new SoXtLightIntensitySliderModule( widget, "lightIntensityEditor" );
    _subComponentArray[1] = new SoXtLightColorSliderModule( widget, "lightColorEditor" );

    for (int i = 0; i < _numSubComponents; i++ ) {
	(( SoXtSliderModule *) _subComponentArray[i])->setEditor( this );
	theWidget = _subComponentArray[i]->getWidget();
	XtGetApplicationResources( theWidget, (XtPointer) &newBorderWidth,
		_borderWidthResource, XtNumber( _borderWidthResource ),NULL, 0);        XtSetArg( wargs[0], XmNborderWidth, newBorderWidth );
	XtSetValues( theWidget, wargs, 1 );
    }

    updateLayout();

    return widget;
}
