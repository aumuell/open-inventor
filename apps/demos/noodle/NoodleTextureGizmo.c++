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
|      NoodleTextureGizmo is a component for editing textures of a noodle.
|
|   Author(s)          : Paul Isaacs
|
*/

#include <X11/StringDefs.h>
#include <Xm/LabelG.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>

#include <Inventor/nodes/SoTexture2Transform.h>

#include "GeneralizedCylinder.h"
#include "NoodleTextureGizmo.h"
#include "NoodleSlider.h"

NoodleTextureGizmo::NoodleTextureGizmo(
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

NoodleTextureGizmo::~NoodleTextureGizmo()
{
}

Widget
NoodleTextureGizmo::buildWidget( 
    Widget parent )
{
    Arg resources[20];
    int n = 0;
    SbVec2s sliderSize;

    _rowCol = XtCreateManagedWidget(getWidgetName(),xmRowColumnWidgetClass,
					parent,NULL, 0);

#define STRING(a) XmStringCreate(a,XmSTRING_DEFAULT_CHARSET)

    // Texture on/off Toggle Button
    XtSetArg(resources[n], XmNlabelString, STRING("TextureOnOff")); ++n;
    textureToggle = XmCreateToggleButtonGadget(_rowCol, "textureOnOff",
						 resources, n); n = 0;
    XtAddCallback(textureToggle, XmNvalueChangedCallback,
		  NoodleTextureGizmo::textureToggleCallback, (XtPointer)this);
    XtManageChild(textureToggle);
    
    // Texture repeat in X-direction.
    // Label...
    XtSetArg(resources[n], XmNlabelString, STRING("X repeat:")); ++n;
    Widget xRepeatLabel = XmCreateLabelGadget(_rowCol, "X repeat",
						 resources, n); n = 0;
    XtManageChild(xRepeatLabel);
    // Slider...
    xRepeatSlider = new NoodleSlider(_rowCol);
    xRepeatSlider->addValueChangedCallback( NoodleTextureGizmo::xRepeatCallback, this );
    sliderSize = xRepeatSlider->getSize();
    xRepeatSlider->setSize( SbVec2s( 200, sliderSize[1] ) );

    xRepeatSlider->setValue(1.0);
    xRepeatSlider->setMin(0.0);
    xRepeatSlider->setMax(30.0);
    xRepeatSlider->show();

    // Texture repeat in Y-direction.
    // Label...
    XtSetArg(resources[n], XmNlabelString, STRING("Y repeat:")); ++n;
    Widget yRepeatLabel = XmCreateLabelGadget(_rowCol, "Y repeat",
						 resources, n); n = 0;
    XtManageChild(yRepeatLabel);
    // Slider...
    yRepeatSlider = new NoodleSlider(_rowCol);
    yRepeatSlider->addValueChangedCallback( NoodleTextureGizmo::yRepeatCallback, this );
    sliderSize = yRepeatSlider->getSize();
    yRepeatSlider->setSize( SbVec2s( 200, sliderSize[1] ) );

    yRepeatSlider->setValue(1.0);
    yRepeatSlider->setMin(0.0);
    yRepeatSlider->setMax(30.0);
    yRepeatSlider->show();
    
    
    // Texture translate in X-direction.
    // Label...
    XtSetArg(resources[n], XmNlabelString, STRING("X transl:")); ++n;
    Widget xTranslLabel = XmCreateLabelGadget(_rowCol, "X transl",
						 resources, n); n = 0;
    XtManageChild(xTranslLabel);
    // Slider...
    xTranslSlider = new NoodleSlider(_rowCol);
    xTranslSlider->addValueChangedCallback( NoodleTextureGizmo::xTranslCallback, this );
    sliderSize = xTranslSlider->getSize();
    xTranslSlider->setSize( SbVec2s( 200, sliderSize[1] ) );

    xTranslSlider->setValue(0.0);
    xTranslSlider->setMin(-1.0);
    xTranslSlider->setMax(1.0);
    xTranslSlider->show();
    
    // Texture translate in Y-direction.
    // Label...
    XtSetArg(resources[n], XmNlabelString, STRING("Y transl:")); ++n;
    Widget yTranslLabel = XmCreateLabelGadget(_rowCol, "Y transl",
						 resources, n); n = 0;
    XtManageChild(yTranslLabel);
    // Slider...
    yTranslSlider = new NoodleSlider(_rowCol);
    yTranslSlider->addValueChangedCallback( NoodleTextureGizmo::yTranslCallback, this );
    sliderSize = yTranslSlider->getSize();
    yTranslSlider->setSize( SbVec2s( 200, sliderSize[1] ) );

    yTranslSlider->setValue(0.0);
    yTranslSlider->setMin(-1.0);
    yTranslSlider->setMax(1.0);
    yTranslSlider->show();
    

    // Texture rotation angle.
    // Label...
    XtSetArg(resources[n], XmNlabelString, STRING("Rotation:")); ++n;
    Widget texRotationLabel = XmCreateLabelGadget(_rowCol, "Rotation",
						 resources, n); n = 0;
    XtManageChild(texRotationLabel);
    // Slider...
    texRotationSlider = new NoodleSlider(_rowCol);
    texRotationSlider->addValueChangedCallback(NoodleTextureGizmo::texRotationCallback, this);
    sliderSize = texRotationSlider->getSize();
    texRotationSlider->setSize( SbVec2s( 200, sliderSize[1] ) );

    texRotationSlider->setValue(0.0);
    texRotationSlider->setMin(0.0);
    texRotationSlider->setMax(6.28);
    texRotationSlider->show();

    return _rowCol;
}

void 
NoodleTextureGizmo::setNoodle( GeneralizedCylinder *newNoodle )
{
    myNoodle = newNoodle;

    // Set the values in the widgets to reflect values in the noodle.

    SbBool bv = (newNoodle) ? newNoodle->withTextureCoords.getValue() : FALSE;
    XmToggleButtonSetState(textureToggle, bv, FALSE);

    if (newNoodle) {
	SoTexture2Transform *txXf 
	    = SO_GET_PART(newNoodle, "texture2Transform", SoTexture2Transform );
	xTranslSlider->setValue( txXf->translation.getValue()[0]);
	yTranslSlider->setValue( txXf->translation.getValue()[1]);
	xRepeatSlider->setValue( txXf->scaleFactor.getValue()[0] );
	yRepeatSlider->setValue( txXf->scaleFactor.getValue()[1] );
	texRotationSlider->setValue( txXf->rotation.getValue() );
    }
    else {
	xTranslSlider->setValue( 0 );
	yTranslSlider->setValue( 0 );
	xRepeatSlider->setValue( 1 );
	yRepeatSlider->setValue( 1 );
	texRotationSlider->setValue( 0 );
    }
}

//
// Callback for the texture on/off toggle buttons
//
void
NoodleTextureGizmo::textureToggleCallback(Widget w, XtPointer data, XtPointer)
{
    SbBool test = XmToggleButtonGetState( w );
    NoodleTextureGizmo  *myself = (NoodleTextureGizmo *) data;
    if ( myself->myNoodle != NULL )
	myself->myNoodle->withTextureCoords = test;
}

//
// Texture parameter callbacks
//
void
NoodleTextureGizmo::xTranslCallback(void *userData, void *)
{
    NoodleTextureGizmo  *myself = (NoodleTextureGizmo *) userData;
    float newVal = myself->xTranslSlider->getValue();
    if ( myself->myNoodle != NULL ) {
	SoTexture2Transform *txXf 
	    = SO_GET_PART( myself->myNoodle, "texture2Transform", SoTexture2Transform );
	SbVec2f old = txXf->translation.getValue();
	old[0] = newVal;
	txXf->translation = old;
    }
}
void
NoodleTextureGizmo::yTranslCallback(void *userData, void *)
{
    NoodleTextureGizmo  *myself = (NoodleTextureGizmo *) userData;
    float newVal = myself->yTranslSlider->getValue();
    if ( myself->myNoodle != NULL ) {
	SoTexture2Transform *txXf 
	    = SO_GET_PART( myself->myNoodle, "texture2Transform", SoTexture2Transform );
	SbVec2f old = txXf->translation.getValue();
	old[1] = newVal;
	txXf->translation = old;
    }
}
void
NoodleTextureGizmo::xRepeatCallback(void *userData, void *)
{
    NoodleTextureGizmo  *myself = (NoodleTextureGizmo *) userData;
    float newVal = myself->xRepeatSlider->getValue();
    if ( myself->myNoodle != NULL ) {
	SoTexture2Transform *txXf 
	    = SO_GET_PART( myself->myNoodle, "texture2Transform", SoTexture2Transform );
	SbVec2f old = txXf->scaleFactor.getValue();
	old[0] = newVal;
	txXf->scaleFactor = old;
    }
}
void
NoodleTextureGizmo::yRepeatCallback(void *userData, void *)
{
    NoodleTextureGizmo  *myself = (NoodleTextureGizmo *) userData;
    float newVal = myself->yRepeatSlider->getValue();
    if ( myself->myNoodle != NULL ) {
	SoTexture2Transform *txXf 
	    = SO_GET_PART( myself->myNoodle, "texture2Transform", SoTexture2Transform );
	SbVec2f old = txXf->scaleFactor.getValue();
	old[1] = newVal;
	txXf->scaleFactor = old;
    }
}
void
NoodleTextureGizmo::texRotationCallback(void *userData, void *)
{
    NoodleTextureGizmo  *myself = (NoodleTextureGizmo *) userData;
    float newVal = myself->texRotationSlider->getValue();
    if ( myself->myNoodle != NULL ) {
	SoTexture2Transform *txXf 
	    = SO_GET_PART( myself->myNoodle, "texture2Transform", SoTexture2Transform );
	txXf->rotation = newVal;
    }
}
