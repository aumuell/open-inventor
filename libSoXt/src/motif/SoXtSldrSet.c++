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
|      SoXtSliderSets for creating organized groups of sliders
|
|   Author(s)          : Paul Isaacs
|
______________  S I L I C O N   G R A P H I C S   I N C .  ____________
_______________________________________________________________________
*/

#include <inttypes.h>
#include <Inventor/Xt/SoXtSliderSet.h>
#include <X11/StringDefs.h>
#include <Xm/Form.h>

static XtResource _editorWidthResource[] = {
    { XtNwidth, XtCWidth, XtRDimension, sizeof( unsigned short),
      0,  XtRImmediate, (XtPointer) 0 }
};

static XtResource _editorHeightResource[] = {
    { XtNheight, XtCHeight, XtRDimension, sizeof( unsigned short),
      0,  XtRImmediate, (XtPointer) 0 }
};

SoXtSliderSet::SoXtSliderSet(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent,
    SoNode *newEditNode)
	: SoXtSliderSetBase(
	    parent,
	    name, 
	    buildInsideParent,
	    newEditNode)
{
    _parentShellWidget = NULL;
}

SoXtSliderSet::~SoXtSliderSet()
{
}

void
SoXtSliderSet::updateLayout()
{
    int newLayoutH, newLayoutW;
    Dimension initialWidth, initialHeight;
    Dimension oldWidgetHeight, newWidgetHeight;
    int tempW, tempH;
    float topPos, bottomPos;
    Widget theSubWidget;
    Arg wargs[10];
    int nargs;

    newLayoutH = 0;
    newLayoutW = 0;

    int i;
    for (i = 0; i < _numSubComponents; i++ ) {
        _subComponentArray[i]->getLayoutSize( tempW, tempH );
	newLayoutH += tempH;
	newLayoutW = ( tempW > newLayoutW ) ? tempW : newLayoutW;
    }

    // set attachment and position for each module
    topPos = 0;
    bottomPos = 0;
    for (i = 0; i < _numSubComponents; i++ ) {

	// set its relative size in the container
	theSubWidget = _subComponentArray[i]->getWidget();
        topPos    = bottomPos;

        _subComponentArray[i]->getLayoutSize( tempW, tempH );
	bottomPos += 100.0 * ((float) tempH / (float) newLayoutH);

	nargs = 0;
	XtSetArg( wargs[nargs], XmNtopAttachment, XmATTACH_POSITION); nargs++;
	XtSetArg( wargs[nargs], XmNtopPosition, (int) topPos ); nargs++;
	XtSetArg( wargs[nargs], XmNbottomAttachment, XmATTACH_POSITION);nargs++;
	XtSetArg( wargs[nargs], XmNbottomPosition, (int) bottomPos ); nargs++;
	XtSetArg( wargs[nargs], XmNleftAttachment, XmATTACH_FORM); nargs++;
	XtSetArg( wargs[nargs], XmNrightAttachment, XmATTACH_FORM); nargs++;

	XtSetValues( theSubWidget, wargs, nargs );
    }
    
    // Now deal with adjusting SIZE of the ENTIRE EDITOR.
    if (widget == NULL)
	return;

    if ( _layoutHeight == 0 || _layoutWidth == 0 ) {
	// if first time throught, _layoutHeight and _layoutWidth will still
	// be at their initialized values of 0. In this case, just copy.
	_layoutHeight = newLayoutH;
	_layoutWidth = newLayoutW;
	// if the user has not specified a size for the window through the
	// resource system, then use the layout size.
	// otherwise, leave it alone.
	_editorHeightResource[0].default_addr = (XtPointer) (unsigned long) newLayoutH;
	_editorWidthResource[0].default_addr = (XtPointer) (unsigned long) newLayoutW;
        XtGetApplicationResources( widget, (XtPointer) &initialWidth, 
	       _editorWidthResource, XtNumber( _editorWidthResource ), NULL, 0);
        XtGetApplicationResources(widget, (XtPointer) &initialHeight, 
             _editorHeightResource, XtNumber( _editorHeightResource ), NULL, 0);
	XtSetArg( wargs[0], XmNwidth, initialWidth );
	XtSetArg( wargs[1], XmNheight, initialHeight );
	XtSetValues( widget, wargs, 2 );
	XtSetValues( _parentShellWidget, wargs, 2 );
    }
    else if ( _layoutHeight != newLayoutH ) {
	// If layout height has changed (this will happen if modules 
	// changed size, typically by opening or closing the multiSliders.)
	// Then we must calculate a new size for the editor.
	// To calculate, we keep these two ratios equal:
	// newWidgetHeight / newLayoutH = oldWidgetHeight / _layoutHeight
	XtSetArg( wargs[0], XmNheight, &oldWidgetHeight );
	XtGetValues( _parentShellWidget, wargs, 1 );

	// note that previous if statement prevents us from dividing by zero
	if (oldWidgetHeight == 0)
	    newWidgetHeight = newLayoutH;
	else
	    newWidgetHeight = (Dimension) (((float) newLayoutH )
			  * ((float) oldWidgetHeight / (float) _layoutHeight));

	XtSetArg( wargs[0], XmNheight, newWidgetHeight );
	XtSetValues( _parentShellWidget, wargs, 1 );
	XtSetValues( widget, wargs, 1 );

	_layoutHeight = newLayoutH;
	_layoutWidth = newLayoutW;
    }
}
