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
|      SoXtFloatText is a component that make a motif label widget that
|      displays only a single float value.
|
|   Author(s)          : Paul Isaacs
|
______________  S I L I C O N   G R A P H I C S   I N C .  ____________
_______________________________________________________________________
*/



#include <inttypes.h>
#include "SoXtFloatText.h"
#include <Xm/Text.h>

SoXtFloatText::SoXtFloatText(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent)
	: SoXtSliderSetBase(
	    parent,
	    name, 
	    buildInsideParent,
	    NULL)
{
    _layoutWidth  = 160;  // default size.
    _layoutHeight = 40;
    
    // Build now
    Widget w = buildWidget(getParentWidget());
    setBaseWidget(w);
}

// make the compiler happy
SoXtFloatText::~SoXtFloatText() {}

Widget
SoXtFloatText::buildWidget( Widget parent )
{
    widget = XtCreateManagedWidget(getWidgetName(),xmTextWidgetClass,
					parent,NULL, 0);
    return widget;
}

Boolean
SoXtFloatText::getDisplayedValue( float &answer )
{
    if (widget == NULL) {
	answer = 0.0;
	return FALSE;
    }

    Arg wargs[10];
    char   *valString;
    float  theVal;

    /* get text value from the label */

    XtSetArg( wargs[0], XmNvalue, &valString );
    XtGetValues( widget, wargs, 1 );

    if ( sscanf( valString, "%f", &theVal ) ) {
	answer = theVal;
	return TRUE;
    }
    else
	return FALSE;
}

void
SoXtFloatText::setDisplayedValue( float newVal, int numDecimals )
{
    if (widget == NULL)
	return;

    char valString[50];

    sprintf( valString, "%.*f", numDecimals, newVal );
    XmTextSetString( widget, valString );
}
