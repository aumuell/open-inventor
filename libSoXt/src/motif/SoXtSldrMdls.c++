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
|      Subclasses of modules used for specific fields of specific types
|     of nodes.
|
|   Author(s)          : Paul Isaacs
|
______________  S I L I C O N   G R A P H I C S   I N C .  ____________
_______________________________________________________________________
*/

#include <inttypes.h>
#include "SoXtSliderModules.h"
#include "SoXtMultiSliders.h"
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <Inventor/Xt/SoXtResource.h>

static XtResource _labelResource[] = {
    { XmNlabelString, XmCLabelString, XmRXmString, sizeof( char * ),
      0,
      XtRString, (XtPointer) NULL }
};

static char *transSliderTitles[] = {
   SO_TRANSLATE_MODULE_LABEL,         SO_TRANSLATE_MODULE_SLIDER1_LABEL,
   SO_TRANSLATE_MODULE_SLIDER2_LABEL, SO_TRANSLATE_MODULE_SLIDER3_LABEL,
};

SoXtTransSliderModule::SoXtTransSliderModule(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent)
	: SoXtSliderModule(
	    parent,
	    name, 
	    buildInsideParent, 
	    FALSE)		// tell parent class not to build just yet
{
    


    // OK, we will build right now.
    Widget form = buildForm(getParentWidget());				
    _sliderModuleSliders = new SoXtTransMultiSlider(form, "Trans");
    SoXtSliderModule::buildWidget(form);
    //
    SoXtResource xr(form);
    char *tmp;
    if (xr.getResource( "transSliderLabel",  "TransSliderLabel", tmp ))
	transSliderTitles[0] = tmp;	
    if (xr.getResource( "transSlider1",  "TransSlider1", tmp ))
	transSliderTitles[1] = tmp;	
    if (xr.getResource( "transSlider2",  "TransSlider2", tmp ))
	transSliderTitles[2] = tmp;	
    if (xr.getResource( "transSlider3",  "TransSlider3", tmp ))
	transSliderTitles[3] = tmp;	
    //
    initLabels();
    setBaseWidget(form);
}

SoXtTransSliderModule::~SoXtTransSliderModule() {}

void
SoXtTransSliderModule::initLabels()
{
    Arg    wargs[10];
    XmString newString;
    Widget theWidget;

    /* label for the title in the control bar */
    _labelResource[0].default_addr = (XtPointer) transSliderTitles[0];
    theWidget = _sliderModuleControl->getOpenCloseButton();
    XtGetApplicationResources( theWidget, (XtPointer) &newString,
		  _labelResource, XtNumber( _labelResource ), NULL, 0); 
    XtSetArg( wargs[0], XmNlabelString, newString ); 
    XtSetValues( theWidget,  wargs, 1 );

    _sliderModuleSliders->initTitles( &(transSliderTitles[1]) );
}

static char *scaleSliderTitles[] = {
   SO_SCALE_MODULE_LABEL,         SO_SCALE_MODULE_SLIDER1_LABEL,
   SO_SCALE_MODULE_SLIDER2_LABEL, SO_SCALE_MODULE_SLIDER3_LABEL,
};

SoXtScaleSliderModule::SoXtScaleSliderModule(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent)
	: SoXtSliderModule(
	    parent,
	    name, 
	    buildInsideParent, 
	    FALSE)		// tell parent class not to build just yet
{
    // OK, we will build right now.
    Widget form = buildForm(getParentWidget());				
    _sliderModuleSliders = new SoXtScaleMultiSlider(form, "Scale");
    SoXtSliderModule::buildWidget(form);
    //
    SoXtResource xr(form);
    char *tmp;
    if (xr.getResource( "scaleSliderLabel",  "ScaleSliderLabel", tmp ))
	scaleSliderTitles[0] = tmp;	
    if (xr.getResource( "scaleSlider1",  "ScaleSlider1", tmp ))
	scaleSliderTitles[1] = tmp;	
    if (xr.getResource( "scaleSlider2",  "ScaleSlider2", tmp ))
	scaleSliderTitles[2] = tmp;	
    if (xr.getResource( "scaleSlider3",  "ScaleSlider3", tmp ))
	scaleSliderTitles[3] = tmp;	
    //
    initLabels();
    setBaseWidget(form);
}

SoXtScaleSliderModule::~SoXtScaleSliderModule() {}

void
SoXtScaleSliderModule::initLabels()
{
    Arg    wargs[10];
    XmString newString;
    Widget theWidget;

    /* label for the title in the control bar */
    _labelResource[0].default_addr = (XtPointer) scaleSliderTitles[0];
    theWidget = _sliderModuleControl->getOpenCloseButton();
    XtGetApplicationResources( theWidget, (XtPointer) &newString,
		  _labelResource, XtNumber( _labelResource ), NULL, 0); 
    XtSetArg( wargs[0], XmNlabelString, newString ); 
    XtSetValues( theWidget,  wargs, 1 );

    _sliderModuleSliders->initTitles( &(scaleSliderTitles[1]) );
}

///////////////////

static char *rotateSliderTitles[] = {
   SO_ROTATE_MODULE_LABEL,         SO_ROTATE_MODULE_SLIDER1_LABEL,
   SO_ROTATE_MODULE_SLIDER2_LABEL, SO_ROTATE_MODULE_SLIDER3_LABEL,
};

SoXtRotateSliderModule::SoXtRotateSliderModule(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent)
	: SoXtSliderModule(
	    parent,
	    name, 
	    buildInsideParent, 
	    FALSE)		// tell parent class not to build just yet
{
    // OK, we will build right now.
    Widget form = buildForm(getParentWidget());				
    _sliderModuleSliders = new SoXtRotateMultiSlider(form, "Rotate");
    SoXtSliderModule::buildWidget(form);
    //
    SoXtResource xr(form);
    char *tmp;
    if (xr.getResource( "rotateSliderLabel",  "RotateSliderLabel", tmp ))
	rotateSliderTitles[0] = tmp;	
    if (xr.getResource( "rotateSlider1",  "RotateSlider1", tmp ))
	rotateSliderTitles[1] = tmp;	
    if (xr.getResource( "rotateSlider2",  "RotateSlider2", tmp ))
	rotateSliderTitles[2] = tmp;	
    if (xr.getResource( "rotateSlider3",  "RotateSlider3", tmp ))
	rotateSliderTitles[3] = tmp;	
    //
    initLabels();
    setBaseWidget(form);
}

SoXtRotateSliderModule::~SoXtRotateSliderModule() {}

void
SoXtRotateSliderModule::initLabels()
{
    Arg    wargs[10];
    XmString newString;
    Widget theWidget;

    /* label for the title in the control bar */
    _labelResource[0].default_addr = (XtPointer) rotateSliderTitles[0];
    theWidget = _sliderModuleControl->getOpenCloseButton();
    XtGetApplicationResources( theWidget, (XtPointer) &newString,
		  _labelResource, XtNumber( _labelResource ), NULL, 0); 
    XtSetArg( wargs[0], XmNlabelString, newString ); 
    XtSetValues( theWidget,  wargs, 1 );

    _sliderModuleSliders->initTitles( &(rotateSliderTitles[1]) );
}

///////////////////
///////////////////

static char *scaleOrientationSliderTitles[] = {
   SO_SCALE_ORIENTATION_MODULE_LABEL, 
   SO_SCALE_ORIENTATION_MODULE_SLIDER1_LABEL,
   SO_SCALE_ORIENTATION_MODULE_SLIDER2_LABEL, 
   SO_SCALE_ORIENTATION_MODULE_SLIDER3_LABEL,
};


SoXtScaleOrientationSliderModule::SoXtScaleOrientationSliderModule(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent)
	: SoXtSliderModule(
	    parent,
	    name, 
	    buildInsideParent, 
	    FALSE)		// tell parent class not to build just yet
{
    // OK, we will build right now.
    Widget form = buildForm(getParentWidget());				
    _sliderModuleSliders = new SoXtScaleOrientationMultiSlider(form, "ScaleOrientation");
    SoXtSliderModule::buildWidget(form);
    //
    SoXtResource xr(form);
    char *tmp;
    if (xr.getResource( "scaleOrientationSliderLabel",  "ScaleOrientationSliderLabel", tmp ))
	scaleOrientationSliderTitles[0] = tmp;	
    if (xr.getResource( "scaleOrientationSlider1",  "ScaleOrientationSlider1", tmp ))
	scaleOrientationSliderTitles[1] = tmp;	
    if (xr.getResource( "scaleOrientationSlider2",  "ScaleOrientationSlider2", tmp ))
	scaleOrientationSliderTitles[2] = tmp;	
    if (xr.getResource( "scaleOrientationSlider3",  "ScaleOrientationSlider3", tmp ))
	scaleOrientationSliderTitles[3] = tmp;	
    //
    initLabels();
    setBaseWidget(form);
}

SoXtScaleOrientationSliderModule::~SoXtScaleOrientationSliderModule() {}

void
SoXtScaleOrientationSliderModule::initLabels()
{
    Arg    wargs[10];
    XmString newString;
    Widget theWidget;

    /* label for the title in the control bar */
    _labelResource[0].default_addr = (XtPointer) scaleOrientationSliderTitles[0];
    theWidget = _sliderModuleControl->getOpenCloseButton();
    XtGetApplicationResources( theWidget, (XtPointer) &newString,
		  _labelResource, XtNumber( _labelResource ), NULL, 0); 
    XtSetArg( wargs[0], XmNlabelString, newString ); 
    XtSetValues( theWidget,  wargs, 1 );

    _sliderModuleSliders->initTitles( &(scaleOrientationSliderTitles[1]) );
}

///////////////////

static char *centerSliderTitles[] = {
   SO_CENTER_MODULE_LABEL,         SO_CENTER_MODULE_SLIDER1_LABEL,
   SO_CENTER_MODULE_SLIDER2_LABEL, SO_CENTER_MODULE_SLIDER3_LABEL,
};

SoXtCenterSliderModule::SoXtCenterSliderModule(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent)
	: SoXtSliderModule(
	    parent,
	    name, 
	    buildInsideParent, 
	    FALSE)		// tell parent class not to build just yet
{
    // OK, we will build right now.
    Widget form = buildForm(getParentWidget());				
    _sliderModuleSliders = new SoXtCenterMultiSlider(form, "Center");
    SoXtSliderModule::buildWidget(form);
    //
    SoXtResource xr(form);
    char *tmp;
    if (xr.getResource( "centerSliderLabel",  "CenterSliderLabel", tmp ))
	centerSliderTitles[0] = tmp;	
    if (xr.getResource( "centerSlider1",  "CenterSlider1", tmp ))
	centerSliderTitles[1] = tmp;	
    if (xr.getResource( "centerSlider2",  "CenterSlider2", tmp ))
	centerSliderTitles[2] = tmp;	
    if (xr.getResource( "centerSlider3",  "CenterSlider3", tmp ))
	centerSliderTitles[3] = tmp;	
    //
    initLabels();
    setBaseWidget(form);
}

SoXtCenterSliderModule::~SoXtCenterSliderModule() {}

void
SoXtCenterSliderModule::initLabels()
{
    Arg    wargs[10];
    XmString newString;
    Widget theWidget;

    /* label for the title in the control bar */
    _labelResource[0].default_addr = (XtPointer) centerSliderTitles[0];
    theWidget = _sliderModuleControl->getOpenCloseButton();
    XtGetApplicationResources( theWidget, (XtPointer) &newString,
		  _labelResource, XtNumber( _labelResource ), NULL, 0); 
    XtSetArg( wargs[0], XmNlabelString, newString ); 
    XtSetValues( theWidget,  wargs, 1 );

    _sliderModuleSliders->initTitles( &(centerSliderTitles[1]) );
}

static char *ambientColorSliderTitles[] = {
   SO_AMBIENT_COLOR_MODULE_LABEL,         SO_AMBIENT_COLOR_MODULE_SLIDER1_LABEL,
   SO_AMBIENT_COLOR_MODULE_SLIDER2_LABEL, SO_AMBIENT_COLOR_MODULE_SLIDER3_LABEL,
};

SoXtAmbientColorSliderModule::SoXtAmbientColorSliderModule(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent)
	: SoXtSliderModule(
	    parent,
	    name, 
	    buildInsideParent, 
	    FALSE)		// tell parent class not to build just yet
{
    // OK, we will build right now.
    Widget form = buildForm(getParentWidget());				
    _sliderModuleSliders = new SoXtAmbientColorMultiSlider(form);
    SoXtSliderModule::buildWidget(form);
    initLabels();
    setBaseWidget(form);
}

SoXtAmbientColorSliderModule::~SoXtAmbientColorSliderModule() {}

void
SoXtAmbientColorSliderModule::initLabels()
{
    Arg    wargs[10];
    XmString newString;
    Widget theWidget;

    /* label for the title in the control bar */
    _labelResource[0].default_addr = (XtPointer) ambientColorSliderTitles[0];
    theWidget = _sliderModuleControl->getOpenCloseButton();
    XtGetApplicationResources( theWidget, (XtPointer) &newString,
		  _labelResource, XtNumber( _labelResource ), NULL, 0); 
    XtSetArg( wargs[0], XmNlabelString, newString ); 
    XtSetValues( theWidget,  wargs, 1 );

    _sliderModuleSliders->initTitles( &(ambientColorSliderTitles[1]) );
}

static char *diffuseColorSliderTitles[] = {
   SO_DIFFUSE_COLOR_MODULE_LABEL,         SO_DIFFUSE_COLOR_MODULE_SLIDER1_LABEL,
   SO_DIFFUSE_COLOR_MODULE_SLIDER2_LABEL, SO_DIFFUSE_COLOR_MODULE_SLIDER3_LABEL,
};

SoXtDiffuseColorSliderModule::SoXtDiffuseColorSliderModule(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent)
	: SoXtSliderModule(
	    parent,
	    name, 
	    buildInsideParent, 
	    FALSE)		// tell parent class not to build just yet
{
    // OK, we will build right now.
    Widget form = buildForm(getParentWidget());				
    _sliderModuleSliders = new SoXtDiffuseColorMultiSlider(form);
    SoXtSliderModule::buildWidget(form);
    initLabels();
    setBaseWidget(form);
}

SoXtDiffuseColorSliderModule::~SoXtDiffuseColorSliderModule() {}

void
SoXtDiffuseColorSliderModule::initLabels()
{
    Arg    wargs[10];
    XmString newString;
    Widget theWidget;

    /* label for the title in the control bar */
    _labelResource[0].default_addr = (XtPointer) diffuseColorSliderTitles[0];
    theWidget = _sliderModuleControl->getOpenCloseButton();
    XtGetApplicationResources( theWidget, (XtPointer) &newString,
		  _labelResource, XtNumber( _labelResource ), NULL, 0); 
    XtSetArg( wargs[0], XmNlabelString, newString ); 
    XtSetValues( theWidget,  wargs, 1 );

    _sliderModuleSliders->initTitles( &(diffuseColorSliderTitles[1]) );
}

static char *specularColorSliderTitles[] = {
   SO_SPECULAR_COLOR_MODULE_LABEL,       SO_SPECULAR_COLOR_MODULE_SLIDER1_LABEL,
  SO_SPECULAR_COLOR_MODULE_SLIDER2_LABEL,SO_SPECULAR_COLOR_MODULE_SLIDER3_LABEL,
};

SoXtSpecularColorSliderModule::SoXtSpecularColorSliderModule(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent)
	: SoXtSliderModule(
	    parent,
	    name, 
	    buildInsideParent, 
	    FALSE)		// tell parent class not to build just yet
{
    // OK, we will build right now.
    Widget form = buildForm(getParentWidget());				
    _sliderModuleSliders = new SoXtSpecularColorMultiSlider(form);
    SoXtSliderModule::buildWidget(form);
    initLabels();
    setBaseWidget(form);
}

SoXtSpecularColorSliderModule::~SoXtSpecularColorSliderModule() {}

void
SoXtSpecularColorSliderModule::initLabels()
{
    Arg    wargs[10];
    XmString newString;
    Widget theWidget;

    /* label for the title in the control bar */
    _labelResource[0].default_addr = (XtPointer) specularColorSliderTitles[0];
    theWidget = _sliderModuleControl->getOpenCloseButton();
    XtGetApplicationResources( theWidget, (XtPointer) &newString,
		  _labelResource, XtNumber( _labelResource ), NULL, 0); 
    XtSetArg( wargs[0], XmNlabelString, newString ); 
    XtSetValues( theWidget,  wargs, 1 );

    _sliderModuleSliders->initTitles( &(specularColorSliderTitles[1]) );
}

static char *emissiveColorSliderTitles[] = {
   SO_EMISSIVE_COLOR_MODULE_LABEL,       SO_EMISSIVE_COLOR_MODULE_SLIDER1_LABEL,
  SO_EMISSIVE_COLOR_MODULE_SLIDER2_LABEL,SO_EMISSIVE_COLOR_MODULE_SLIDER3_LABEL,
};

SoXtEmissiveColorSliderModule::SoXtEmissiveColorSliderModule(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent)
	: SoXtSliderModule(
	    parent,
	    name, 
	    buildInsideParent, 
	    FALSE)		// tell parent class not to build just yet
{
    // OK, we will build right now.
    Widget form = buildForm(getParentWidget());				
    _sliderModuleSliders = new SoXtEmissiveColorMultiSlider(form);
    SoXtSliderModule::buildWidget(form);
    initLabels();
    setBaseWidget(form);
}

SoXtEmissiveColorSliderModule::~SoXtEmissiveColorSliderModule() {}

void
SoXtEmissiveColorSliderModule::initLabels()
{
    Arg    wargs[10];
    XmString newString;
    Widget theWidget;

    /* label for the title in the control bar */
    _labelResource[0].default_addr = (XtPointer) emissiveColorSliderTitles[0];
    theWidget = _sliderModuleControl->getOpenCloseButton();
    XtGetApplicationResources( theWidget, (XtPointer) &newString,
		  _labelResource, XtNumber( _labelResource ), NULL, 0); 
    XtSetArg( wargs[0], XmNlabelString, newString ); 
    XtSetValues( theWidget,  wargs, 1 );

    _sliderModuleSliders->initTitles( &(emissiveColorSliderTitles[1]) );
}

static char *shininessSliderTitles[] = {
   SO_SHININESS_MODULE_LABEL,         SO_SHININESS_MODULE_SLIDER1_LABEL
};

SoXtShininessSliderModule::SoXtShininessSliderModule(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent)
	: SoXtSliderModule(
	    parent,
	    name, 
	    buildInsideParent, 
	    FALSE)		// tell parent class not to build just yet
{
    // OK, we will build right now.
    Widget form = buildForm(getParentWidget());				
    _sliderModuleSliders = new SoXtShininessMultiSlider(form);
    SoXtSliderModule::buildWidget(form);
    initLabels();
    setBaseWidget(form);
}

SoXtShininessSliderModule::~SoXtShininessSliderModule() {}

void
SoXtShininessSliderModule::initLabels()
{
    Arg    wargs[10];
    XmString newString;
    Widget theWidget;

    /* label for the title in the control bar */
    _labelResource[0].default_addr = (XtPointer) shininessSliderTitles[0];
    theWidget = _sliderModuleControl->getOpenCloseButton();
    XtGetApplicationResources( theWidget, (XtPointer) &newString,
		  _labelResource, XtNumber( _labelResource ), NULL, 0); 
    XtSetArg( wargs[0], XmNlabelString, newString ); 
    XtSetValues( theWidget,  wargs, 1 );

    _sliderModuleSliders->initTitles( &(shininessSliderTitles[1]) );
}

static char *transparencySliderTitles[] = {
   SO_TRANSPARENCY_MODULE_LABEL,         SO_TRANSPARENCY_MODULE_SLIDER1_LABEL
};

SoXtTransparencySliderModule::SoXtTransparencySliderModule(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent)
	: SoXtSliderModule(
	    parent,
	    name, 
	    buildInsideParent, 
	    FALSE)		// tell parent class not to build just yet
{
    // OK, we will build right now.
    Widget form = buildForm(getParentWidget());				
    _sliderModuleSliders = new SoXtTransparencyMultiSlider(form);
    SoXtSliderModule::buildWidget(form);
    initLabels();
    setBaseWidget(form);
}

SoXtTransparencySliderModule::~SoXtTransparencySliderModule() {}

void
SoXtTransparencySliderModule::initLabels()
{
    Arg    wargs[10];
    XmString newString;
    Widget theWidget;

    /* label for the title in the control bar */
    _labelResource[0].default_addr = (XtPointer) transparencySliderTitles[0];
    theWidget = _sliderModuleControl->getOpenCloseButton();
    XtGetApplicationResources( theWidget, (XtPointer) &newString,
		  _labelResource, XtNumber( _labelResource ), NULL, 0); 
    XtSetArg( wargs[0], XmNlabelString, newString ); 
    XtSetValues( theWidget,  wargs, 1 );

    _sliderModuleSliders->initTitles( &(transparencySliderTitles[1]) );
}

static char *lightIntensitySliderTitles[] = {
   SO_LIGHT_INTENSITY_MODULE_LABEL,     SO_LIGHT_INTENSITY_MODULE_SLIDER1_LABEL
};

SoXtLightIntensitySliderModule::SoXtLightIntensitySliderModule(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent)
	: SoXtSliderModule(
	    parent,
	    name, 
	    buildInsideParent, 
	    FALSE)		// tell parent class not to build just yet
{
    // OK, we will build right now.
    Widget form = buildForm(getParentWidget());				
    _sliderModuleSliders = new SoXtLightIntensityMultiSlider(form);
    SoXtSliderModule::buildWidget(form);
    initLabels();
    setBaseWidget(form);
}

SoXtLightIntensitySliderModule::~SoXtLightIntensitySliderModule() {}

void
SoXtLightIntensitySliderModule::initLabels()
{
    Arg    wargs[10];
    XmString newString;
    Widget theWidget;

    /* label for the title in the control bar */
    _labelResource[0].default_addr = (XtPointer) lightIntensitySliderTitles[0];
    theWidget = _sliderModuleControl->getOpenCloseButton();
    XtGetApplicationResources( theWidget, (XtPointer) &newString,
		  _labelResource, XtNumber( _labelResource ), NULL, 0); 
    XtSetArg( wargs[0], XmNlabelString, newString ); 
    XtSetValues( theWidget,  wargs, 1 );

    _sliderModuleSliders->initTitles( &(lightIntensitySliderTitles[1]) );
}

static char *lightColorSliderTitles[] = {
   SO_LIGHT_COLOR_MODULE_LABEL,         SO_LIGHT_COLOR_MODULE_SLIDER1_LABEL,
   SO_LIGHT_COLOR_MODULE_SLIDER2_LABEL, SO_LIGHT_COLOR_MODULE_SLIDER3_LABEL,
};

SoXtLightColorSliderModule::SoXtLightColorSliderModule(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent)
	: SoXtSliderModule(
	    parent,
	    name, 
	    buildInsideParent, 
	    FALSE)		// tell parent class not to build just yet
{
    // OK, we will build right now.
    Widget form = buildForm(getParentWidget());				
    _sliderModuleSliders = new SoXtLightColorMultiSlider(form);
    SoXtSliderModule::buildWidget(form);
    initLabels();
    setBaseWidget(form);
}

SoXtLightColorSliderModule::~SoXtLightColorSliderModule() {}

void
SoXtLightColorSliderModule::initLabels()
{
    Arg    wargs[10];
    XmString newString;
    Widget theWidget;

    /* label for the title in the control bar */
    _labelResource[0].default_addr = (XtPointer) lightColorSliderTitles[0];
    theWidget = _sliderModuleControl->getOpenCloseButton();
    XtGetApplicationResources( theWidget, (XtPointer) &newString,
		  _labelResource, XtNumber( _labelResource ), NULL, 0); 
    XtSetArg( wargs[0], XmNlabelString, newString ); 
    XtSetValues( theWidget,  wargs, 1 );

    _sliderModuleSliders->initTitles( &(lightColorSliderTitles[1]) );
}
