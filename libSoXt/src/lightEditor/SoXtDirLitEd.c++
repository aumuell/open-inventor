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
 * Copyright (C) 1991-93   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |	SoXtDirectionalLightEditor
 |
 |   Author(s):	David Mott, Paul Isaacs, Alain Dumesny
 |
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <inttypes.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <Xm/CascadeBG.h>
#include <Xm/Form.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/Separator.h>
#include <Xm/SeparatoG.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>

#include <Inventor/SoDB.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/SoXtClipboard.h>
#include <Inventor/Xt/SoXtDirectionalLightEditor.h>
#include <Inventor/sensors/SoNodeSensor.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/draggers/SoDragger.h>
#include <Inventor/manips/SoDirectionalLightManip.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoEnvironment.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/Xt/SoXtRenderArea.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/Xt/SoXtResource.h>
#include "_SoXtColorEditor.h"
#include "_SoXtColorSlider.h"

/*
 * Defines
 */
 
#define DEFAULT_WIDTH	    	200
#define DEFAULT_HEIGHT	    	250

// so far,  there is only one menu
#define k_CONTROL_MENU 0

// the menu items
enum {
    k_COLOR_EDITOR = 0, 
    k_COPY, 
    k_PASTE, 
    k_HELP
};


// Resources for labels.
typedef struct {
	char *inten;
} RES_LABELS;
static RES_LABELS rl;
static char *defaults[]={ 
	"Inten "
};


#define TOGGLE_ON(BUTTON) \
    XmToggleButtonSetState((Widget) BUTTON, TRUE, FALSE)
#define TOGGLE_OFF(BUTTON) \
    XmToggleButtonSetState((Widget) BUTTON, FALSE, FALSE)


char *SoXtDirectionalLightEditor::geomBuffer = 
"#Inventor V2.0 ascii\n\
Separator {\n\
    DEF DIR_LIGHT_EDITOR_ROTATOR Group {\n\
	LightModel { model PHONG }\n\
	Separator {\n\
	    Transform {\n\
		scaleFactor .2 .2 .2\n\
		rotation    1 0 0 -1.57079632679489661923  # PI/2\n\
		translation 0 0 1.2\n\
	    }\n\
	    Cone {}\n\
	}\n\
	Separator {\n\
	    Transform {\n\
		scaleFactor .08 .3 .08\n\
		rotation    1 0 0 -1.57079632679489661923  # PI/2\n\
		translation 0 0 1.7\n\
	    }\n\
	    Cylinder {}\n\
	}\n\
	Separator {\n\
	    Transform {\n\
		scaleFactor .1 .1 .1\n\
		rotation    1 0 0 -1.57079632679489661923  # PI/2\n\
		translation 0 0 -1.1\n\
	    }\n\
	    Cone {}\n\
	}\n\
    }\n\
    DEF dirLightEditorRotator Separator {\n\
	Material {\n\
	    diffuseColor	[ 0.5 0.5 0.5 ]\n\
	    emissiveColor	[ 0.5 0.5 0.5 ]\n\
	}\n\
	DrawStyle { lineWidth 2 }\n\
	USE DIR_LIGHT_EDITOR_ROTATOR\n\
    }\n\
    DEF dirLightEditorRotatorActive Separator {\n\
	Material {\n\
	    diffuseColor	[ 0.5 0.5 0.0 ]\n\
	    emissiveColor	[ 0.5 0.5 0.0 ]\n\
	}\n\
	DrawStyle { lineWidth 3 }\n\
	USE DIR_LIGHT_EDITOR_ROTATOR\n\
    }\n\
}\n\
";



////////////////////////////////////////////////////////////////////////
//
// Public constructor - build the widget right now
//
SoXtDirectionalLightEditor::SoXtDirectionalLightEditor(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent)
	: SoXtComponent(
	    parent,
	    name, 
	    buildInsideParent)
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
SoXtDirectionalLightEditor::SoXtDirectionalLightEditor(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent, 
    SbBool buildNow)
	: SoXtComponent(
	    parent,
	    name, 
	    buildInsideParent)
//
////////////////////////////////////////////////////////////////////////
{
    // In this case, this component may be what the app wants, 
    // or it may want a subclass of this component. Pass along buildNow
    // as it was passed to us.
    constructorCommon(buildNow);
}

////////////////////////////////////////////////////////////////////////
//
// Called by the constructors
//
// private
//
void
SoXtDirectionalLightEditor::constructorCommon(SbBool buildNow)
//
//////////////////////////////////////////////////////////////////////
{
    setClassName("SoXtDirectionalLightEditor");
    addVisibilityChangeCallback(visibilityChangeCB, this);
    dirLight = NULL;
    clipboard = NULL;
    callbackList = new SoCallbackList;
    
    // default title and size
    setSize( SbVec2s(DEFAULT_WIDTH, DEFAULT_HEIGHT) );

    // create the dir light manip with our own geom
    SoInput in;
    in.setBuffer((void *)geomBuffer, (size_t) strlen(geomBuffer));
    SoNode *geom;
    SbBool ok = SoDB::read(&in, geom);
    dirLightManip = new SoDirectionalLightManip();
    dirLightManip->ref();

    // Set the rotator to be our arrow.
    SoDragger *liteDragger = dirLightManip->getDragger();
    liteDragger->setPart("rotator.rotator", 
			    SoNode::getByName("dirLightEditorRotator"));
    liteDragger->setPart("rotator.rotatorActive", 
			    SoNode::getByName("dirLightEditorRotatorActive"));

    // Set the other parts to NULL
    SoSeparator *dummySep = new SoSeparator;
    dummySep->ref();
    liteDragger->setPart("translator.xTranslator.translator", dummySep);
    liteDragger->setPart("translator.yTranslator.translator", dummySep);
    liteDragger->setPart("translator.zTranslator.translator", dummySep);
    liteDragger->setPart("translator.xTranslator.translatorActive", dummySep);
    liteDragger->setPart("translator.yTranslator.translatorActive", dummySep);
    liteDragger->setPart("translator.zTranslator.translatorActive", dummySep);
    liteDragger->setPart("translator.yzTranslator.translator", dummySep);
    liteDragger->setPart("translator.xzTranslator.translator", dummySep);
    liteDragger->setPart("translator.xyTranslator.translator", dummySep);
    liteDragger->setPart("translator.yzTranslator.translatorActive", dummySep);
    liteDragger->setPart("translator.xzTranslator.translatorActive", dummySep);
    liteDragger->setPart("translator.xyTranslator.translatorActive", dummySep);
    liteDragger->setPart("rotator.feedback", dummySep);
    liteDragger->setPart("rotator.feedbackActive", dummySep);
    dummySep->unref();

    liteDragger->addValueChangedCallback(
	SoXtDirectionalLightEditor::dirLightManipCB, this);

    // NULL out components. They'll be created in buildWidget().
    colorEditor = NULL;
    intensitySlider = NULL;
    renderArea = NULL;
    

    // Callbacks
    ignoreCallback = FALSE;

    // set up the light sensor
    // this tells us if someone else changed the light
    lightSensor = new SoNodeSensor;
    lightSensor->setFunction((void (*)(void *, SoSensor *))
			     SoXtDirectionalLightEditor::lightSensorCB);
    lightSensor->setData((void *) this);

    // set up the camera sensor - this will keep our camera oriented to 
    // match the scene the light is in.
    cameraSensor = new SoNodeSensor;
    cameraSensor->setFunction((void (*)(void *, SoSensor *))
			       SoXtDirectionalLightEditor::cameraSensorCB);
    cameraSensor->setData((void *) this);
    cameraToWatch = NULL;

    // Local scene graph for direct manipulation
    root    	= new SoSeparator;
    myCamera	= new SoPerspectiveCamera;
    litStuff    = new SoSeparator;
    SoEnvironment   	*environ    = new SoEnvironment;
    SoMaterial	    	*material   = new SoMaterial;
    SoComplexity    	*complexity = new SoComplexity;
    SoSphere	    	*sphere	    = new SoSphere;
    litStuff->addChild(environ);
    litStuff->addChild(material);
    litStuff->addChild(complexity);
    litStuff->addChild(sphere);
    litStuff->renderCaching = SoSeparator::AUTO;
    
    root->ref();
    root->addChild(myCamera);
    root->addChild(dirLightManip);
    root->addChild(litStuff);
    
    // some ambient light
    environ->ambientColor.setValue(1.0, 1.0, 1.0);
    environ->ambientIntensity.setValue(0.5);
    
    // let's have an interesting material!
    material->ambientColor.setValue(.2, .2, .2);
    material->diffuseColor.setValue(.55, .55, .55);
    material->specularColor.setValue(.70, .70, .70);
    material->shininess.setValue(1.0);
    
    // and some complexity
    complexity->value.setValue(0.6);
    
    // Build the widget tree, and let SoXtComponent know about our base widget.
    if (buildNow) {
	Widget w = buildWidget(getParentWidget());
	setBaseWidget(w);
    }
}


////////////////////////////////////////////////////////////////////////
//
//    Destructor.
//
SoXtDirectionalLightEditor::~SoXtDirectionalLightEditor()
//
////////////////////////////////////////////////////////////////////////
{
    if (isAttached())
	detach();

    delete callbackList;
    delete colorEditor;
    delete intensitySlider;
    delete renderArea;
    root->unref();
    dirLightManip->unref();
    delete clipboard;
}

void
SoXtDirectionalLightEditor::show()
{
    SoXtComponent::show();
    
    // now also show the other components if there were previously shown
    if (colorEditor != NULL && colorEditor->getWidget() != NULL)
	colorEditor->show();
}

void
SoXtDirectionalLightEditor::hide()
{
    SoXtComponent::hide();
    if (colorEditor != NULL)
	colorEditor->hide();
}

////////////////////////////////////////////////////////////////////////
//
//    This builds the pulldown menu for the dir light editor.
//
// usage: private
//
Widget
SoXtDirectionalLightEditor::buildPulldownMenu(Widget parent)
//
////////////////////////////////////////////////////////////////////////
{
    Widget  menubar, pulldown, buttons[1];
    Arg	    args[5];
    int	    n;
    Widget  menuW[20];
    int	    num = 0;
    
    menubar = XmCreateMenuBar(parent, "menuBar", NULL, 0);
    
    pulldown = XmCreatePulldownMenu(menubar, "controlPulldown", NULL, 0);
    
    n = 0;
    XtSetArg(args[n], XmNsubMenuId, pulldown); n++;
    buttons[0] = XtCreateWidget("Edit",
	xmCascadeButtonGadgetClass, menubar, args, n);
    
    n = 0;
    XtSetArg(args[n], XmNuserData, this); n++;
    
#define PUSH_ITEM(NAME,KONST) \
    menuW[num] = XtCreateWidget(NAME, 	\
    	xmPushButtonGadgetClass, pulldown, args, n); 	\
    XtAddCallback(menuW[num], XmNactivateCallback,\
	(XtCallbackProc) SoXtDirectionalLightEditor::menuPick,	\
	(XtPointer) KONST); \
    num++;
    
    PUSH_ITEM("Color Editor", k_COLOR_EDITOR)
    
    menuW[num++] = XtCreateWidget("separator",
    	xmSeparatorGadgetClass, pulldown, NULL, 0);
    
    PUSH_ITEM("Copy", k_COPY)
    PUSH_ITEM("Paste", k_PASTE)

    menuW[num++] = XtCreateWidget("separator",
    	xmSeparatorGadgetClass, pulldown, NULL, 0);
    
    PUSH_ITEM("Help", k_HELP)
#undef PUSH_ITEM

    // manage children
    XtManageChildren(menuW, num);
    XtManageChildren(buttons, 1);

    return menubar;
}

////////////////////////////////////////////////////////////////////////
//
//    This routine builds all the widgets, sets up callback routines,
//  and does the layout using motif.
//
// usage: virtual public
//
Widget
SoXtDirectionalLightEditor::buildWidget(Widget parent)
//
////////////////////////////////////////////////////////////////////////
{
    Widget  	menubar;
    int		n;
    Arg		args[10];
    
    // get resources...
       SoXtResource xr(parent);
       if (!xr.getResource( "inten", "Inten", rl.inten ))
	   rl.inten = defaults[0];

    //
    // create a top level form to hold everything together
    //
    SbVec2s size = getSize();
    n = 0;
    if ((size[0] != 0) && (size[1] != 0)) {
	XtSetArg(args[n], XtNwidth, size[0]); n++;
	XtSetArg(args[n], XtNheight, size[1]); n++;
    }
    
    // build the topmost widget
    Widget widget = XtCreateWidget(getWidgetName(), xmFormWidgetClass, parent, args, n);
    
    // build the subcomponents
    intensitySlider = new _SoXtColorSlider(widget, NULL, TRUE, _SoXtColorSlider::INTENSITY_SLIDER);
    intensitySlider->setLabel( rl.inten );
    intensitySlider->addValueChangedCallback(
    	SoXtDirectionalLightEditor::intensitySliderCB, this);

    renderArea = new SoXtRenderArea(widget);
    renderArea->setSceneGraph(root);
    myCamera->viewAll(litStuff, SbViewportRegion(renderArea->getSize()), 2.0);
    
    menubar = buildPulldownMenu(widget);
    
    // Layout
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,	    XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment,    XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment,   XmATTACH_FORM); n++;
    XtSetValues(menubar, args, n);
    
    n = 0;
    XtSetArg(args[n], XmNleftAttachment,    XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment,   XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment,     XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget,         menubar); n++;
    XtSetArg(args[n], XmNbottomAttachment,  XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNbottomPosition,    90); n++;
    XtSetValues(renderArea->getWidget(), args, n);
    
    n = 0;
    XtSetArg(args[n], XmNbottomAttachment,  XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment,    XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment,   XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment,     XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget,         renderArea->getWidget()); n++;
    XtSetValues(intensitySlider->getWidget(), args, n);
    
    // manage those children
    XtManageChild(menubar);
    renderArea->show();
    intensitySlider->show();
    
    // the intensity slider should reflect the current RGB color
    ignoreCallback = TRUE;
    intensitySlider->setBaseColor(dirLightManip->color.getValue().getValue());
    ignoreCallback = FALSE;
    
    return widget;
}


////////////////////////////////////////////////////////////////////////
//
//    This routine attaches itself to a single color field.
//
// usage: public
//
void
SoXtDirectionalLightEditor::attach(SoPath *pathToLight )
//
////////////////////////////////////////////////////////////////////////
{
    if (isAttached())
	detach();

    if ( pathToLight == NULL )
	return;

    pathToLight->ref();
    SoFullPath *fullPathToLight = (SoFullPath *) pathToLight;

    if ( fullPathToLight->getTail()->isOfType(
		SoDirectionalLight::getClassTypeId() ) ) {

	dirLight = (SoDirectionalLight *) fullPathToLight->getTail();
	dirLight->ref();
	
	// set values in dirLightManip
	ignoreCallback = TRUE; // ignore resulting manip callback
	copyLight(dirLightManip, dirLight); // dst,src
	ignoreCallback = FALSE;
	
	// If the path contains a camera, put a sensor on camera, 
	// to keep our view the same as the
	// view of the light
	SoSearchAction sa;
	sa.setType( SoCamera::getClassTypeId() );
	sa.apply( fullPathToLight );
	if ( sa.getPath() != NULL  ) {
    	    cameraToWatch = (SoCamera *)((SoFullPath *)sa.getPath())->getTail();
	    cameraToWatch->ref();
	    cameraSensorCB( this, NULL );
	}
	else {
	    // Otherwise, set camera to original place.
	    if ( cameraToWatch != NULL )
		cameraToWatch->unref();
	    cameraToWatch = NULL;  
	    // put the camera back to the origin...
	    myCamera->orientation.setValue( 0, 0, 0, 1 );
	    myCamera->position.setValue( 0, 0, 0 );
	    
	    if (renderArea != NULL) {
		myCamera->viewAll(litStuff,
			    SbViewportRegion(renderArea->getSize()),
			    2.0);	// slack = 2.0
	    }
	}
	
	if (isVisible())
	    activate();
    }
    pathToLight->unref();
}


////////////////////////////////////////////////////////////////////////
//
//    This routine detaches itself from the color field.
//
// usage: public

void
SoXtDirectionalLightEditor::detach()
//
////////////////////////////////////////////////////////////////////////
{
    if (isAttached()) {
	deactivate();
	if ( cameraToWatch != NULL ) {
	    cameraToWatch->unref();
	    cameraToWatch = NULL;
	}

	dirLight->unref();
	dirLight = NULL;
    }
}

////////////////////////////////////////////////////////////////////////
//
//    Callback routine from the color editor, this changes the color
//  of the light node we are attached to.
//
// usage: static, private
//
void
SoXtDirectionalLightEditor::colorEditorCB(
    void *p, 
    const SbColor *rgb)
//
////////////////////////////////////////////////////////////////////////
{
    SoXtDirectionalLightEditor *editor = (SoXtDirectionalLightEditor *)p;
    
    if (editor->ignoreCallback)
    	return;
    
    if (editor->dirLight != NULL) {
    	// detach the sensor while we update the field
	editor->lightSensor->detach();
	editor->dirLight->color.setValue(*rgb);
	editor->lightSensor->attach(editor->dirLight);
    }
    
    // update the intensity to reflect the color
    editor->ignoreCallback = TRUE;
    editor->intensitySlider->setBaseColor(rgb->getValue());
    editor->ignoreCallback = FALSE;
    
    // update our local light
    editor->ignoreCallback = TRUE; // ignore resulting manip callback
    editor->dirLightManip->color.setValue(*rgb);
    editor->ignoreCallback = FALSE;
    
    // invoke the callbacks with the new values
    editor->callbackList->invokeCallbacks(editor->dirLightManip);
}

////////////////////////////////////////////////////////////////////////
//
//    Callback routine from the intensity slider, this changes the
//  intensity of the light node we are attached to.
//
// usage: static, private
//
void
SoXtDirectionalLightEditor::intensitySliderCB(
    void *p, 
    float intensity)
//
////////////////////////////////////////////////////////////////////////
{
    SoXtDirectionalLightEditor *editor = (SoXtDirectionalLightEditor *)p;
    
    if (editor->ignoreCallback)
    	return;
    
    if (editor->dirLight != NULL) {
    	// detach the sensor while we update the field
	editor->lightSensor->detach();
	editor->dirLight->intensity.setValue(intensity);
	editor->lightSensor->attach(editor->dirLight);
    }
    
    // update our local light
    editor->ignoreCallback = TRUE; // ignore resulting manip callback
    editor->dirLightManip->intensity.setValue(intensity);
    editor->ignoreCallback = FALSE;
    
    // invoke the callbacks with the new values
    editor->callbackList->invokeCallbacks(editor->dirLightManip);
}


////////////////////////////////////////////////////////////////////////
//
//  Update the editor components based on the values of dirLightManip.
//
//  Use: private
//
void
SoXtDirectionalLightEditor::updateLocalComponents()
//
////////////////////////////////////////////////////////////////////////
{
    if (colorEditor != NULL) {
    	ignoreCallback = TRUE;
	colorEditor->setColor(dirLightManip->color.getValue());
	ignoreCallback = FALSE;
    }
	
    if (intensitySlider != NULL) {
	// update the intensity to reflect the color
    	ignoreCallback = TRUE;
	intensitySlider->setValue(dirLightManip->intensity.getValue());
	intensitySlider->setBaseColor(dirLightManip->color.getValue().getValue());
	ignoreCallback = FALSE;
    }
}

////////////////////////////////////////////////////////////////////////
//
//  Set the values of our local light and the light we are editing.
//
//  Use: public
//
void
SoXtDirectionalLightEditor::setLight(const SoDirectionalLight &newValues)
//
////////////////////////////////////////////////////////////////////////
{
    if (dirLight != NULL) {
	lightSensor->detach();
	copyLight(dirLight, &newValues); // dst,src
	lightSensor->attach(dirLight);
    }
	
    // set values in dirLightManip
    ignoreCallback = TRUE; // ignore resulting manip callback
    copyLight(dirLightManip, &newValues); // dst,src
    ignoreCallback = FALSE;
    
    // set values in local components (colorEditor, and intensitySlider)
    updateLocalComponents();
    
    // invoke the callbacks
    callbackList->invokeCallbacks(dirLightManip);
}

////////////////////////////////////////////////////////////////////////
//
//  Sensor callback, invoked whenever the dirLight node changes.
//
//  Use: private
//
void
SoXtDirectionalLightEditor::lightSensorCB(SoXtDirectionalLightEditor *editor,
					  SoSensor *)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    // sanity check
    if (editor->dirLight == NULL) {
	SoDebugError::post("SoXtDirectionalLightEditor::lightSensorCB",
		"dirLight is NULL\n");
	return;
    }
#endif
    
    if (!editor->isVisible())
	return;
    
    // set values in dirLightManip
    editor->ignoreCallback = TRUE; // ignore resulting manip callback
    editor->copyLight(editor->dirLightManip, editor->dirLight); // dst,src
    editor->ignoreCallback = FALSE;
    
    editor->updateLocalComponents();
    
    // invoke the callbacks
    editor->callbackList->invokeCallbacks(editor->dirLightManip);
}

////////////////////////////////////////////////////////////////////////
//
//  Sensor callback, invoked whenever the cameraToWatch node changes.
//
//  Use: private
//
void
SoXtDirectionalLightEditor::cameraSensorCB(SoXtDirectionalLightEditor *editor, SoSensor *)
//
////////////////////////////////////////////////////////////////////////
{
    if ( editor->cameraToWatch != NULL ) {
	SbRotation newRot = editor->cameraToWatch->orientation.getValue();
	editor->myCamera->orientation.setValue( newRot );
	editor->myCamera->viewAll( editor->litStuff, 
			SbViewportRegion(editor->renderArea->getSize()), 2.0 );
    }
}

////////////////////////////////////////////////////////////////////////
//
// Attach the lightSensor and the cameraSensor (if necessary).
//
void
SoXtDirectionalLightEditor::activate()
//
// use: private
////////////////////////////////////////////////////////////////////////
{
    // make sure the components are in sync with our local light
    updateLocalComponents();
    
    if (dirLight == NULL)
	return;
	
    // attach lightSensor 
    if ( lightSensor->getAttachedNode() == NULL) {
	lightSensor->attach(dirLight); // attach AFTER update
    }
    if ( cameraToWatch != NULL ) {
	cameraSensorCB( this, NULL );
	cameraSensor->attach(cameraToWatch);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Detach the lightSensor and cameraSensor.
//
void
SoXtDirectionalLightEditor::deactivate()
//
// use: private
////////////////////////////////////////////////////////////////////////
{
    // detach lightSensor
    lightSensor->detach();
    cameraSensor->detach();
}


////////////////////////////////////////////////////////////////////////
//
//  Called by Xt when a menu item is picked.
//
//  Use: private
//
void
SoXtDirectionalLightEditor::menuPick(
    Widget w,
    int id,
    XmAnyCallbackStruct *cb)
//
////////////////////////////////////////////////////////////////////////
{
    SoXtDirectionalLightEditor *editor;
    Arg	args[1];
    
    XtSetArg(args[0], XmNuserData, &editor);
    XtGetValues(w, args, 1);

#ifdef DEBUG
    if (editor == NULL) {
	SoDebugError::post("SoXtDirectionalLightEditor::menuPick",
		"- editor is NULL\n");
	return;
    }
#endif

    Time eventTime = cb->event->xbutton.time;

    switch (id) {
	case k_COLOR_EDITOR:
	    // build comp under our shell widget
	    if (editor->colorEditor == NULL) {
		_SoXtColorEditor *ce;
		editor->colorEditor = ce = new _SoXtColorEditor(
		    SoXt::getShellWidget(editor->getWidget()), NULL, FALSE);
		ce->setCurrentSliders(_SoXtColorEditor::NONE);
		ce->setTitle("Directional Light Color");
		ce->addColorChangedCallback(
		    SoXtDirectionalLightEditor::colorEditorCB, editor);
	    }
	    
	    editor->colorEditor->show();
	    break;
	    
	case k_COPY:
	    {
		if (editor->clipboard == NULL)
		    editor->clipboard = new SoXtClipboard(editor->getWidget());
		// Make a directional light with the same values as our manip.
		// Then send it to the copy buffer.
		SoDirectionalLight *dl = new SoDirectionalLight;
		dl->ref();
		dl->intensity = editor->dirLightManip->intensity;
		dl->color     = editor->dirLightManip->color;
		dl->direction = editor->dirLightManip->direction;
		editor->clipboard->copy(dl, eventTime);
		dl->unref();
	    }
	    break;
	    
	case k_PASTE:
	    if (editor->clipboard == NULL)
		editor->clipboard = new SoXtClipboard(editor->getWidget());
	
	    editor->clipboard->paste(eventTime,
				     SoXtDirectionalLightEditor::pasteDoneCB,
				     editor);
	    break;
	    
	case k_HELP:
	    editor->openHelpCard("SoXtDirectionalLightEditor.help");
	    break;
    }
}

////////////////////////////////////////////////////////////////////////
//
//  The X server has finished getting the selection data, and the
//  paste is complete. Look through the paste data for a dir light node.
//
//  Use: static private
//
void
SoXtDirectionalLightEditor::pasteDoneCB(void *userData, SoPathList *pathList)
//
////////////////////////////////////////////////////////////////////////
{
    SoXtDirectionalLightEditor *le = (SoXtDirectionalLightEditor *) userData;
    
    // Search for a directional light or base color.
    SoDirectionalLight	*newLight = NULL;
    SoBaseColor	    	*newColor = NULL;
    for (int i = 0; i < pathList->getLength(); i++) {
	SoFullPath *fullP = (SoFullPath *) (*pathList)[i];
	if (fullP->getTail()->isOfType(SoDirectionalLight::getClassTypeId())) {
	    newLight = (SoDirectionalLight *) fullP->getTail();
	    break;
	}
	if (fullP->getTail()->isOfType(SoBaseColor::getClassTypeId())) {
	    newColor = (SoBaseColor *) fullP->getTail();
	    break;
	}
    }
    
    if (newLight != NULL) {
	// pasted a light - update everything!
	le->setLight(*newLight);
    }
    else if (newColor != NULL) {
	// pasted a color - update the dirLight color, and the editor
	if (le->dirLight != NULL) {
	    // detach the sensor while we update the field
	    le->lightSensor->detach();
	    le->dirLight->color.setValue(newColor->rgb[0]);
	    le->lightSensor->attach(le->dirLight);
	}
	le->ignoreCallback = TRUE; // ignore resulting manip callback
	le->dirLightManip->color.setValue(newColor->rgb[0]);
	le->ignoreCallback = FALSE;

	le->updateLocalComponents();
    
	// invoke the callbacks with the pasted light values
	le->callbackList->invokeCallbacks(le->dirLightManip);
    }
    
    // We delete the callback data when done with it.
    delete pathList;
}

////////////////////////////////////////////////////////////////////////
//
//  The X server has finished getting the selection data, and the
//  paste is complete. Look through the paste data for a dir light node.
//
//  Use: private
//
void
SoXtDirectionalLightEditor::copyLight(
    SoDirectionalLight *dst,
    const SoDirectionalLight *src)
//
////////////////////////////////////////////////////////////////////////
{
    // set values in dst
    dst->color.setValue(src->color.getValue());
    dst->intensity.setValue(src->intensity.getValue());
    dst->direction.setValue(src->direction.getValue());
}

////////////////////////////////////////////////////////////////////////
//
//  The dir light manip has just changed the dirLightManip direction.
//  We now update the attached light, and invoke the callbacks.
//
//  Use: static private
//
void
SoXtDirectionalLightEditor::dirLightManipCB(void *userData, SoDragger *)
//
////////////////////////////////////////////////////////////////////////
{
    SoXtDirectionalLightEditor *ed = (SoXtDirectionalLightEditor *) userData;
    
    if (ed->ignoreCallback)
	return;
	
    // update the attached light, if it exists
    if (ed->dirLight != NULL) {
	ed->lightSensor->detach();
	ed->dirLight->direction.setValue(
				ed->dirLightManip->direction.getValue());
	ed->lightSensor->attach(ed->dirLight);
    }
    
    // invoke the callbacks
    ed->callbackList->invokeCallbacks(ed->dirLightManip);
}

//
// called whenever the component becomes visibble or not
//
void
SoXtDirectionalLightEditor::visibilityChangeCB(void *pt, SbBool visible)
{
    SoXtDirectionalLightEditor *p = (SoXtDirectionalLightEditor *)pt;
    
    if (visible)
	p->activate();
    else
	p->deactivate();
}


//
// redefine those generic virtual functions
//
const char *
SoXtDirectionalLightEditor::getDefaultWidgetName() const
{ return "SoXtDirectionalLightEditor"; }

const char *
SoXtDirectionalLightEditor::getDefaultTitle() const
{ return "Dir Light Editor"; }

const char *
SoXtDirectionalLightEditor::getDefaultIconTitle() const
{ return "Dir Light Ed"; }
