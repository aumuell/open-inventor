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
|   Description:
|      A component class that creates a gizmo for editing 
|      orientation of normals, crease angle, and the 
|      minimum number of rows/columns in a noodle.
|
|   Author(s)          : Paul Isaacs
|
*/

#include <X11/StringDefs.h>
#include <Xm/LabelG.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/Text.h>

#include <Inventor/nodes/SoShapeHints.h>

#include "GeneralizedCylinder.h"
#include "NoodleSurfaceGizmo.h"
#include "NoodleSlider.h"

NoodleSurfaceGizmo::NoodleSurfaceGizmo(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent) 
	: SoXtComponent(
	    parent,
	    name, 
	    buildInsideParent)
{
    // Build the widget!
    Widget w = buildWidget( getParentWidget() );
    setBaseWidget(w);

    myNoodle = NULL;
}

NoodleSurfaceGizmo::~NoodleSurfaceGizmo()
{
}

Widget
NoodleSurfaceGizmo::buildWidget( 
    Widget parent )
{
    Arg resources[20];
    int n = 0;
    SbVec2s sliderSize;

    _rowCol = XtCreateManagedWidget(getWidgetName(),xmRowColumnWidgetClass,
					parent,NULL, 0);

#define STRING(a) XmStringCreate(a,XmSTRING_DEFAULT_CHARSET)

    // Flip Normals button
    XtSetArg(resources[n], XmNlabelString, STRING("Flip Normals")); ++n;
    flipNormButton = XmCreatePushButtonGadget(_rowCol, "flip",
						 resources, n); n = 0;
    XtAddCallback(flipNormButton, XmNactivateCallback,
		  NoodleSurfaceGizmo::flipNormCallback, (XtPointer)this);
    XtManageChild(flipNormButton);
    
    // Crease Angle.
    // Label...
    XtSetArg(resources[n], XmNlabelString, STRING("Crease Angle:")); ++n;
    Widget creaseLabel = XmCreateLabelGadget(_rowCol, "Crease Angle",
						 resources, n); n = 0;
    XtManageChild(creaseLabel);
    // Slider...
    creaseSlider = new NoodleSlider(_rowCol);
    creaseSlider->addValueChangedCallback( 
		    NoodleSurfaceGizmo::creaseCallback, this );
    sliderSize = creaseSlider->getSize();
    creaseSlider->setSize( SbVec2s( 200, sliderSize[1] ) );
    creaseSlider->setMin(0.0);
    creaseSlider->setMax(90.0);
    creaseSlider->show();
    
    // Min Num rows label
    XtSetArg(resources[n], XmNlabelString, STRING("Min num rows:")); ++n;
    Widget numRowsLabel = XmCreateLabelGadget(_rowCol, "Min num rows",
						 resources, n); n = 0;
    XtManageChild(numRowsLabel);
    // Min Num rows input
    XtSetArg(resources[n], XmNcolumns, 3); ++n;
    XtSetArg(resources[n], XmNeditMode, XmSINGLE_LINE_EDIT); ++n;
    minNumRowsEdit = XmCreateText(_rowCol, "minNumRowsEdit",
				     resources, n); n = 0;
    XtAddCallback(minNumRowsEdit, XmNactivateCallback,
		  NoodleSurfaceGizmo::minNumRowsCallback, (XtPointer)this);
    XtManageChild(minNumRowsEdit);

    // Min Num cols label
    XtSetArg(resources[n], XmNlabelString, STRING("Min num cols:")); ++n;
    Widget numColsLabel = XmCreateLabelGadget(_rowCol, "Min num cols",
						 resources, n); n = 0;
    XtManageChild(numColsLabel);
    // Min Num cols input
    XtSetArg(resources[n], XmNcolumns, 3); ++n;
    XtSetArg(resources[n], XmNeditMode, XmSINGLE_LINE_EDIT); ++n;
    minNumColsEdit = XmCreateText(_rowCol, "minNumColsEdit",
				     resources, n); n = 0;
    XtAddCallback(minNumColsEdit, XmNactivateCallback,
		  NoodleSurfaceGizmo::minNumColsCallback, (XtPointer)this);
    XtManageChild(minNumColsEdit);


    return _rowCol;
}

void 
NoodleSurfaceGizmo::setNoodle( GeneralizedCylinder *newNoodle )
{
    myNoodle = newNoodle;

    // Set the values in the widgets to reflect values in the noodle.

    char str1[10];
    int iv;
    iv = (myNoodle) ? myNoodle->minNumRows.getValue() : 1;
    sprintf(str1,"%d", iv );
    XmTextSetString( minNumRowsEdit, str1 );
    char str2[10];
    iv = (myNoodle) ? myNoodle->minNumCols.getValue() : 1;
    sprintf(str2,"%d", iv );
    XmTextSetString( minNumColsEdit, str2 );

    // Set the crease angle
    float newTheta;
    if (myNoodle) {
	SoShapeHints *h = SO_GET_PART( myNoodle, "shapeHints", SoShapeHints );
	newTheta = h->creaseAngle.getValue();
	newTheta *= 180.0 / M_PI;
    }
    else 
	newTheta = M_PI / 6.0;
    creaseSlider->setValue( newTheta );
}

//
//
// Callback for the flip normals  widget 
//
void
NoodleSurfaceGizmo::flipNormCallback(Widget,  XtPointer data, XtPointer)
{
    NoodleSurfaceGizmo  *myself = (NoodleSurfaceGizmo *) data;
    GeneralizedCylinder *s = myself->myNoodle;
    if ( s->normsFlipped.getValue() == TRUE )
        s->normsFlipped = FALSE;
    else
        s->normsFlipped = TRUE;
}

void
NoodleSurfaceGizmo::minNumRowsCallback(Widget textWidget, XtPointer data, XtPointer)
{
    NoodleSurfaceGizmo  *myself = (NoodleSurfaceGizmo *) data;
    GeneralizedCylinder *s = myself->myNoodle;

    char *str = XmTextGetString((Widget)textWidget);
    float tmp;
    sscanf( str, "%f", &tmp );
    tmp = ( tmp < 1.0 ) ? 1.0 : tmp;
    s->minNumRows = (int) tmp;
    sprintf( str, "%d", (int) tmp );
    XmTextSetString((Widget)textWidget, str );
    XtFree(str);
}

void
NoodleSurfaceGizmo::minNumColsCallback(Widget textWidget, XtPointer data, XtPointer)
{
    NoodleSurfaceGizmo  *myself = (NoodleSurfaceGizmo *) data;
    GeneralizedCylinder *s = myself->myNoodle;

    char *str = XmTextGetString((Widget)textWidget);
    float tmp;
    sscanf( str, "%f", &tmp );
    tmp = ( tmp < 1.0 ) ? 1.0 : tmp;
    s->minNumCols = (int) tmp;
    sprintf( str, "%d", (int) tmp );
    XmTextSetString((Widget)textWidget, str );
    XtFree(str);
}

//
// Crease Angle callbacks
//
void
NoodleSurfaceGizmo::creaseCallback(void *userData, void *)
{
    NoodleSurfaceGizmo  *myself = (NoodleSurfaceGizmo *) userData;
    float newVal = myself->creaseSlider->getValue();
    GeneralizedCylinder *s = myself->myNoodle;
    if ( s != NULL ) {
	SoShapeHints *h = SO_GET_PART( s, "shapeHints", SoShapeHints );
	h->creaseAngle = newVal * M_PI / 180.0;
    }
}
