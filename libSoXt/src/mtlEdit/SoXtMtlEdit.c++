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
 |   $Revision: 1.4 $
 |
 |   Classes:
 |	SoXtMaterialEditor
 |
 |   Author(s):	Alain Dumesny, David Mott
 |
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <inttypes.h>
#include <string.h>

#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <Xm/CascadeBG.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/Separator.h>
#include <Xm/SeparatoG.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>

#include <Inventor/sensors/SoNodeSensor.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoQuadMesh.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/SoXtClipboard.h>
#include <Inventor/Xt/SoXtMaterialList.h>
#include <Inventor/Xt/SoXtRenderArea.h>
#include <Inventor/Xt/SoXtResource.h>
#include <Inventor/Xt/SoXtMaterialEditor.h>

#include "_SoXtColorEditor.h"
#include "_SoXtColorSlider.h"

#include <GL/gl.h>
#include <Inventor/errors/SoDebugError.h>

/*
 * Defines
 */
#define SCREEN(w) XScreenNumberOfScreen( XtScreen(w) )
 
// default window height/width and layout
#define DEFAULT_WIDTH	    	420
#define DEFAULT_HEIGHT	    	210
#define SCENE_RIGHT_POSITION	36
#define OFFSET 	    	    	5    	// offset at window borders

// space between the color sliders (amb,diff,spec,emiss) and
// the other 2 sliders (shin, tranps)
#define SLIDER_SPACE 50


// menu items
enum {
    k_MATERIAL_LIST = 0, 
    k_CONTINUOUS, 
    k_AFTER_ACCEPT, 
    k_EDIT_COPY,
    k_EDIT_PASTE,
    k_EDIT_HELP,
    k_MENU_NUM     // length of menu
};

// ids used in array of sliders and toggles
// Note: the order (amb/diff/spec/Emiss then the rest) is important
// for array of sliders.
enum {
    k_AMBIENT_ID = 0, 
    k_DIFFUSE_ID,
    k_SPECULAR_ID,
    k_EMISSIVE_ID,
    k_SHININESS_ID,
    k_TRANSPARENCY_ID
};

// constants for callback functions (separate bits)
#define k_none	    	0x0000
#define k_ambient   	0x0001
#define k_diffuse   	0x0002
#define k_specular   	0x0004
#define k_emissive   	0x0008
#define k_shininess   	0x0010
#define k_transparency  0x0020

// titles for color editor (also check updateColorEditor() routine)
static char *ambientString;
static char *diffuseString;
static char *specularString;
static char *emissiveString;
static char *deftitles[]={
	"Material Ambient Color",
	"Material Diffuse Color",
	"Material Specular Color",
	"Material Emissive Color"
};

static char *mtlDir  = IVPREFIX "/share/data/materials";

static char *slider_labels[] = { (char *)NULL, (char *)NULL, (char *)NULL,
				 (char *)NULL, (char *)NULL, (char *)NULL
};
static char *slider_defs[] = { "Amb: ",  "Diff: ",       "Spec: ", 
                               "Emis: ", "Shininess: ",  "Transp: "
};

static const float tileColors[][3] = {
    {.3, .3, .3}, {.6, .6, .6},
    {.6, .6, .6}, {.3, .3, .3}
};
static const float tileCoords[][3] = {
    {-3, 3, 0}, {0, 3, 0}, {3, 3, 0},
    {-3, 0, 0}, {0, 0, 0}, {3, 0, 0},
    {-3, -3, 0}, {0, -3, 0}, {3, -3, 0}
};

static char *thisClassName = "SoXtMaterialEditor";

#define TOGGLE_ON(BUTTON) \
    XmToggleButtonSetState((Widget) BUTTON, TRUE, FALSE)
#define TOGGLE_OFF(BUTTON) \
    XmToggleButtonSetState((Widget) BUTTON, FALSE, FALSE)


////////////////////////////////////////////////////////////////////////
//
// Public constructor - build the widget right now
//
SoXtMaterialEditor::SoXtMaterialEditor(
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
SoXtMaterialEditor::SoXtMaterialEditor(
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
SoXtMaterialEditor::constructorCommon(SbBool buildNow)
//
//////////////////////////////////////////////////////////////////////
{
    // init local vars
    setClassName(thisClassName);
    addVisibilityChangeCallback(visibilityChangeCB, this);
    material	    	= NULL;
    acceptButton    	= NULL;
    menuItemsList   	= new Widget[k_MENU_NUM];
    activeColor	    	= k_none;
    clipboard	    	= NULL;
    ignoreCallback 	= FALSE;
    callbackList	= new SoCallbackList;
    updateFreq		= CONTINUOUS;
    renderArea		= NULL;
    for (int i=0; i<6; i++) {
	sliders[i] = NULL;
	changedIt[i] = FALSE;
    }
    setSize( SbVec2s(DEFAULT_WIDTH, DEFAULT_HEIGHT) );
    
    // components that are not built until needed
    materialList    	= NULL;
    colorEditor     	= NULL;
    openMaterialList = FALSE;
    
    //
    // Create local scene graph for viewing materials
    //
    localMaterial   = new SoMaterial;
    light0  	    = new SoDirectionalLight;
    light1  	    = new SoDirectionalLight;
    tileColor	    = new SoBaseColor;
    root	    = new SoSeparator;
    SoOrthographicCamera *camera    = new SoOrthographicCamera;
    SoComplexity    	*complexity = new SoComplexity;
    SoSphere	    	*sphere	    = new SoSphere;
    SoCoordinate3    	*coord	    = new SoCoordinate3;
    SoQuadMesh	    	*quadMesh   = new SoQuadMesh;
    SoMaterialBinding	*matBinding = new SoMaterialBinding;
    SoLightModel    	*lightModel1 = new SoLightModel;
    SoLightModel    	*lightModel2 = new SoLightModel;
    
    root->ref();
    root->addChild(camera);
    root->addChild(lightModel1);
    root->addChild(tileColor);
    root->addChild(matBinding);
    root->addChild(coord);
    root->addChild(quadMesh);
    root->addChild(lightModel2);
    root->addChild(light0);
    root->addChild(light1);
    root->addChild(localMaterial);
    root->addChild(complexity);
    root->addChild(sphere);
    
    // setup the scene viewing
    camera->position.setValue(0, 0, 2);
    camera->nearDistance.setValue(1);
    camera->farDistance.setValue(3);
    camera->height.setValue(2);
    complexity->value.setValue(0.8);
    sphere->radius.setValue(0.85);
    
    // setup the sphere background
    tileColor->rgb.setValues(0, 4, tileColors);
    matBinding->value.setValue(SoMaterialBinding::PER_FACE);
    coord->point.setValues(0, 9, tileCoords);
    quadMesh->verticesPerColumn.setValue(3);
    quadMesh->verticesPerRow.setValue(3);
    lightModel1->model.setValue(SoLightModel::BASE_COLOR);
    lightModel2->model.setValue(SoLightModel::PHONG);
    
    // Default for lights
    light0->direction.setValue(.556, -.623, -.551);
    light1->direction.setValue(-.556, -.623, -.551);
    
    //
    // set up the material sensor - 
    // this tells us if someone else changed the material we attach to
    // (the sensor gets attached later)
    //
    sensor = new SoNodeSensor(SoXtMaterialEditor::sensorCB, this);
    
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
SoXtMaterialEditor::~SoXtMaterialEditor()
//
////////////////////////////////////////////////////////////////////////
{
    unregisterWidget(mgrWidget);
    
    if ( isAttached() )
	detach();
    
    // delete other components
    delete materialList;
    delete colorEditor;
    
    delete sensor;
    delete callbackList;
    delete clipboard;
    delete [] menuItemsList;
    delete sliders[k_AMBIENT_ID];
    delete sliders[k_DIFFUSE_ID];
    delete sliders[k_SPECULAR_ID];
    delete sliders[k_EMISSIVE_ID];
    delete sliders[k_SHININESS_ID];
    delete sliders[k_TRANSPARENCY_ID];
    delete renderArea;	// this should delete the local graph
    root->unref();
}


////////////////////////////////////////////////////////////////////////
//
//    This routine sets the update frequency.
//
// usage: virtual public
//
void
SoXtMaterialEditor::setUpdateFrequency(SoXtMaterialEditor::UpdateFrequency freq)
//
////////////////////////////////////////////////////////////////////////
{
    if (updateFreq == freq)
	return;

    updateFreq = freq;

    // show/hide the accept button
    if (acceptButton != NULL) {
	if (updateFreq == CONTINUOUS)
	    XtUnmanageChild(acceptButton);
	else
	    XtManageChild(acceptButton);
    }

    // update the attached node if we switch to continous
    if (material != NULL && updateFreq == CONTINUOUS) {
    	copyMaterial(material, index, localMaterial, 0);
	undoIgnoresIfChanged();
    }
}


////////////////////////////////////////////////////////////////////////
//
// Show the material editor and its color editor.
//
// usage: public
//
void
SoXtMaterialEditor::show()
//
////////////////////////////////////////////////////////////////////////
{
    SoXtComponent::show();
    
    // now also show the other components if there were previously shown
    if (colorEditor != NULL && activeColor != k_none)
	colorEditor->show();
    if (materialList != NULL && openMaterialList)
	materialList->show();
}

////////////////////////////////////////////////////////////////////////
//
// Hide the material editor and its color editor.
//
// usage: public
//
void
SoXtMaterialEditor::hide()
//
////////////////////////////////////////////////////////////////////////
{
    SoXtComponent::hide();
    
    if (colorEditor != NULL)
	colorEditor->hide();
    
    if (materialList != NULL)
	materialList->hide();
}

////////////////////////////////////////////////////////////////////////
//
//    This builds the pulldown menu for the material editor.
//
// usage: private
//
Widget
SoXtMaterialEditor::buildPulldownMenu(Widget parent)
//
////////////////////////////////////////////////////////////////////////
{
    Widget menubar, pulldown, buttons[1];
    Arg    args[5];
    int    n;
    Widget menu1W[20];
    int	   num1 = 0;
    
    menubar = XmCreateMenuBar(parent, "menuBar", NULL, 0);
    
    Arg popupargs[4];
    int popupn = 0;
#ifdef MENUS_IN_POPUP
    SoXt::getPopupArgs(XtDisplay(menubar), SCREEN(menubar), popupargs, &popupn);
#endif
    
    pulldown = XmCreatePulldownMenu(menubar, "editPulldown", popupargs, popupn);
#ifdef MENUS_IN_POPUP
    // register callbacks to load/unload the pulldown colormap
    SoXt::registerColormapLoad(pulldown, SoXt::getShellWidget(parent));
#endif
    XtAddCallback(pulldown, XmNmapCallback,
	(XtCallbackProc) SoXtMaterialEditor::menuDisplay, (XtPointer) this);
    
    n = 0;
    XtSetArg(args[n], XmNsubMenuId, pulldown); n++;
    buttons[0] = XtCreateWidget("Edit",
	xmCascadeButtonGadgetClass, menubar, args, n);
    
    n = 0;
    XtSetArg(args[n], XmNuserData, this); n++;

#define TOGGLE_ITEM(NAME,KONST) \
    menu1W[num1++] = menuItemsList[KONST] = XtCreateWidget(NAME, 	\
    	xmToggleButtonGadgetClass, pulldown, args, n); 	\
    XtAddCallback(menuItemsList[KONST], XmNvalueChangedCallback,\
	(XtCallbackProc) SoXtMaterialEditor::menuPick,	\
	(XtPointer) KONST)

#define PUSH_ITEM(NAME,KONST) \
    menu1W[num1++] = menuItemsList[KONST] = XtCreateWidget(NAME, 	\
    	xmPushButtonGadgetClass, pulldown, args, n); 	\
    XtAddCallback(menuItemsList[KONST], XmNactivateCallback,\
	(XtCallbackProc) SoXtMaterialEditor::menuPick,	\
	(XtPointer) KONST)


    // the items...
    PUSH_ITEM("Material List", k_MATERIAL_LIST);
    
    menu1W[num1++] = XtCreateWidget("separator",
    	xmSeparatorGadgetClass, pulldown, NULL, 0);
    
    XtSetArg(args[n], XmNindicatorType, XmONE_OF_MANY); n++;
    TOGGLE_ITEM("Continuous", k_CONTINUOUS);
    TOGGLE_ITEM("Manual", k_AFTER_ACCEPT);
    n--;
    
    menu1W[num1++] = XtCreateWidget("separator",
    	xmSeparatorGadgetClass, pulldown, NULL, 0);
	
    PUSH_ITEM("Copy",  k_EDIT_COPY);
    PUSH_ITEM("Paste", k_EDIT_PASTE);
    
    menu1W[num1++] = XtCreateWidget("separator",
    	xmSeparatorGadgetClass, pulldown, NULL, 0);
    
    PUSH_ITEM("Help",  k_EDIT_HELP);
#undef TOGGLE_ITEM
#undef PUSH_ITEM

    // manage childrens
    XtManageChildren(menu1W, num1);
    XtManageChildren(buttons, 1);

    return menubar;
}


////////////////////////////////////////////////////////////////////////
//
// This builds the sliders form (ambient to transparency sliders, 
// with toggles buttons).
//
// usage: private
//
Widget
SoXtMaterialEditor::buildSlidersForm(Widget parent)
//
////////////////////////////////////////////////////////////////////////
{
    Widget  	form, textForm, labelW[6], slidersW[6];
    int		n, i;
    Arg		args[12];
    
    // create a form to hold everything together
    n = 0;
    XtSetArg(args[n], XmNfractionBase, 1000); n++;
    form = XtCreateWidget("sliderForm", xmFormWidgetClass, parent, args, n);
    textForm = XtCreateWidget("textForm", xmFormWidgetClass, form, args, n);
    
    // build sliders
    for (i=0; i<6; i++) {
	sliders[i] = new _SoXtColorSlider(form, NULL, TRUE, _SoXtColorSlider::INTENSITY_SLIDER);
	changedIt[i] = FALSE;
	slidersW[i] = sliders[i]->getWidget();
    }
    
    // set base colors for the non-color sliders: white
    SbColor white(1, 1, 1);
    ignoreCallback = TRUE;
    sliders[k_SHININESS_ID]->setBaseColor(white.getValue());
    sliders[k_TRANSPARENCY_ID]->setBaseColor(white.getValue());
    ignoreCallback = FALSE;
    
    // Callbacks
    
#define ADD_SLIDER_CALLBACK(SLIDER,CB) \
    SLIDER->addValueChangedCallback( \
	&SoXtMaterialEditor::CB, this);

    ADD_SLIDER_CALLBACK(sliders[k_AMBIENT_ID], ambientSliderCB);
    ADD_SLIDER_CALLBACK(sliders[k_DIFFUSE_ID], diffuseSliderCB);
    ADD_SLIDER_CALLBACK(sliders[k_SPECULAR_ID], specularSliderCB);
    ADD_SLIDER_CALLBACK(sliders[k_EMISSIVE_ID], emissiveSliderCB);
    ADD_SLIDER_CALLBACK(sliders[k_SHININESS_ID], shininessSliderCB);
    ADD_SLIDER_CALLBACK(sliders[k_TRANSPARENCY_ID], transparencySliderCB);

#undef ADD_SLIDER_CALLBACK

    
    // build diamond buttons
    n = 0;
    XtSetArg(args[n], XmNuserData, this); n++;
    XtSetArg(args[n], XmNhighlightThickness, 0); n++;
    XtSetArg(args[n], XmNindicatorType, XmONE_OF_MANY); n++;
    for (i=0; i<4; i++) {
	diamondButtons[i] = XtCreateWidget("", xmToggleButtonGadgetClass, 
	    textForm, args, n);
    	XtAddCallback(diamondButtons[i], XmNvalueChangedCallback, 
	    (XtCallbackProc) SoXtMaterialEditor::diamondButtonPick, (XtPointer) (unsigned long) i);
    }
    
    // build radio buttons
    n = 0;
    XtSetArg(args[n], XmNuserData, this); n++;
    XtSetArg(args[n], XmNhighlightThickness, 0); n++;
    for (i=0; i<4; i++) {
	radioButtons[i] = XtCreateWidget("", xmToggleButtonGadgetClass, 
	    textForm, args, n);
   	XtAddCallback(radioButtons[i], XmNvalueChangedCallback, 
	    (XtCallbackProc) SoXtMaterialEditor::radioButtonPick, (XtPointer) (unsigned long) i);
    }
    
    // build label widgets
    for (i=0; i<6; i++)
    	labelW[i] = XtCreateWidget(slider_labels[i], xmLabelGadgetClass, 
	    textForm, NULL, 0);
    
    //
    // layout 
    //
    
    // layout the text form
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,	    XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment,  XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment,    XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment,   XmNONE); n++;
    XtSetValues(textForm, args, n);
    
    // layout the toggles, text and sliders
    int y1, y2;
    for (i=0; i<6; i++) {
    	
	y1 = int((i*(1000-SLIDER_SPACE))/6.0);
	y2 = int(((i+1)*(1000-SLIDER_SPACE))/6.0);
	if (i >= 4) {
	    y1 += SLIDER_SPACE;
	    y2 += SLIDER_SPACE;
	}
	
	// set the vertical attachement for all widgets
	XtSetArg(args[0], XmNtopAttachment, 	    XmATTACH_POSITION);
	XtSetArg(args[1], XmNtopPosition,           y1);
	XtSetArg(args[2], XmNbottomAttachment, 	    XmATTACH_POSITION);
	XtSetArg(args[3], XmNbottomPosition,        y2);
	
	// layout the sliders
	XtSetArg(args[4], XmNleftAttachment,	    XmATTACH_WIDGET);
	XtSetArg(args[5], XmNleftWidget,	    textForm);
	XtSetArg(args[6], XmNrightAttachment,       XmATTACH_FORM);
	XtSetValues(slidersW[i], args, 7);
	
	// layout the toggles
	if (i < 4) {
	    
	    // layout diamonds
	    XtSetArg(args[4], XmNleftAttachment,    XmATTACH_FORM);
	    XtSetValues(diamondButtons[i], args, 5);
	    
	    // layout radio
	    XtSetArg(args[4], XmNleftAttachment,    XmATTACH_WIDGET);
	    XtSetArg(args[5], XmNleftWidget,	    diamondButtons[i]);
	    XtSetValues(radioButtons[i], args, 6);
    	}
	
	// layout the text
	XtSetArg(args[4], XmNrightAttachment,	    XmATTACH_FORM);
	XtSetArg(args[5], XmNalignment,		    XmALIGNMENT_END);
	if (i < 4) {
	    XtSetArg(args[6], XmNleftAttachment,    XmATTACH_WIDGET);
	    XtSetArg(args[7], XmNleftWidget,	    radioButtons[i]);
	    XtSetValues(labelW[i], args, 8);
	}
	else
	    XtSetValues(labelW[i], args, 6);
    }
    
    // manage all children (order is important)
    XtManageChildren(diamondButtons, 4);
    XtManageChildren(radioButtons, 4);
    XtManageChildren(labelW, 6);
    XtManageChild(textForm);
    XtManageChildren(slidersW, 6);
    
    return form;
}


////////////////////////////////////////////////////////////////////////
//
// Builds  and layout the edit label with the slider form.
//
// usage: private
//
Widget
SoXtMaterialEditor::buildControls(Widget parent)
//
////////////////////////////////////////////////////////////////////////
{
    Widget  	form, sliderForm, labelW, separatorW;
    int		n;
    Arg		args[12];
    
    // create a form to hold everything together
    form = XtCreateWidget("Controls", xmFormWidgetClass, parent, NULL, 0);
    
    labelW = XtCreateWidget("Edit Color", xmLabelGadgetClass, form, NULL, 0);
    separatorW = XtCreateWidget("", xmSeparatorGadgetClass, form, NULL, 0);
    sliderForm = buildSlidersForm(form);
    
    // layout
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,	    XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment,    XmATTACH_FORM); n++;
    XtSetValues(labelW, args, n);
    
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,     XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget,         labelW); n++;
    XtSetArg(args[n], XmNleftAttachment,    XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget,        labelW); n++;
    XtSetArg(args[n], XmNrightAttachment,   XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg(args[n], XmNrightWidget,       labelW); n++;
    XtSetValues(separatorW, args, n);
    
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,     XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget,         separatorW); n++;
    XtSetArg(args[n], XmNtopOffset,         2); n++;
    XtSetArg(args[n], XmNleftAttachment,    XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment,   XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment,  XmATTACH_FORM); n++;
    XtSetValues(sliderForm, args, n);
    
    // manage children
    XtManageChild(labelW);
    XtManageChild(separatorW);
    XtManageChild(sliderForm);
    
    return form;
}

#ifdef DEBUG
// ??? add XK_Print event to dump the scene to stdout
#include <X11/keysym.h>
#include <Inventor/actions/SoWriteAction.h>
static SbBool dump(void *p, XAnyEvent *xe)
{
    
    if (xe->type == KeyPress && XLookupKeysym((XKeyEvent *)xe, 0) == XK_Print) {
	SoWriteAction wa;
	wa.getOutput()->setFilePointer(stderr);
	fprintf(stderr, "\n___SoXtMaterialEditor____scene____\n");
	wa.apply( ((SoXtRenderArea *)p)->getSceneGraph() );
    }
    
    return FALSE;
}
#endif


////////////////////////////////////////////////////////////////////////
//
//    This routine builds all the widgets, sets up callback routines,
//  and does the layout using motif.
//
// usage: private
//
Widget
SoXtMaterialEditor::buildWidget(Widget parent)
//
////////////////////////////////////////////////////////////////////////
{
    int n;
    Arg args[12];
    
    //
    // create a top level form to hold everything together
    //
    SbVec2s size = getSize();
    n = 0;
    if ((size[0] != 0) && (size[1] != 0)) {
	XtSetArg(args[n], XtNwidth, size[0]); n++;
	XtSetArg(args[n], XtNheight, size[1]); n++;
    }
    
    // create the top level widget, and register it with a class name
    mgrWidget = XtCreateWidget(getWidgetName(), xmFormWidgetClass, parent, args, n);
    registerWidget(mgrWidget);
    
    //
    // read the X resource for this widget (1)
    //
    SoXtResource xr(mgrWidget);

    if (!xr.getResource("ambientLabel", "AmbientLabel", ambientString))
         ambientString  = deftitles[0];
    if (!xr.getResource("diffuseLabel", "DiffuseLabel", diffuseString))
         diffuseString  = deftitles[1];
    if (!xr.getResource("specularLabel","SpecularLabel",specularString))
         specularString = deftitles[2];
    if (!xr.getResource("emissiveLabel","EmissiveLabel",emissiveString))
         emissiveString = deftitles[3];
    if (!xr.getResource("slideLabel1",  "SlideLabel1",  slider_labels[0]))
         slider_labels[0] = slider_defs[0];
    if (!xr.getResource("slideLabel2",  "SlideLabel2",  slider_labels[1]))
         slider_labels[1] = slider_defs[1];
    if (!xr.getResource("slideLabel3",  "SlideLabel3",  slider_labels[2]))
         slider_labels[2] = slider_defs[2];
    if (!xr.getResource("slideLabel4",  "SlideLabel4",  slider_labels[3]))
         slider_labels[3] = slider_defs[3];
    if (!xr.getResource("slideLabel5",  "SlideLabel5",  slider_labels[4]))
         slider_labels[4] = slider_defs[4];
    if (!xr.getResource("slideLabel6",  "SlideLabel6",  slider_labels[5]))
         slider_labels[5] = slider_defs[5];


    //
    // build the subcomponents
    //
    Widget menubar = buildPulldownMenu(mgrWidget);

    // create the render area
    renderArea = new SoXtRenderArea(mgrWidget);
    renderArea->setSceneGraph(root);
    renderArea->setTransparencyType(SoGLRenderAction::BLEND); // sphere is last
    renderArea->setClearBeforeRender(FALSE); // reduce flicker

#ifdef DEBUG
    // ??? add XK_Print event to dump the scene to stdout
    renderArea->setEventCallback(dump, renderArea);
#endif

#if 0
    int32_t bitnum = getgdesc(GD_BITS_NORM_DBL_RED) 
		+ getgdesc(GD_BITS_NORM_DBL_GREEN) 
    	    	+ getgdesc(GD_BITS_NORM_DBL_BLUE);
    if (bitnum < 12)
	renderArea->setDoubleBuffer(FALSE);
#endif

    Widget controls = buildControls(mgrWidget);
    n = 0;
    XtSetArg(args[n], XmNhighlightThickness, 0); n++;
    acceptButton = XtCreateWidget("Accept", xmPushButtonGadgetClass, mgrWidget, args, n);
    XtAddCallback(acceptButton, XmNactivateCallback, 
	(XtCallbackProc) &SoXtMaterialEditor::acceptButtonCB, 
	(XtPointer) this);
    
    //
    // Layout 
    //
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,     XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment,    XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment,   XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition,     SCENE_RIGHT_POSITION); n++;
    XtSetValues(menubar, args, n);
    
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,	    XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget,         menubar); n++;
    XtSetArg(args[n], XmNleftAttachment,    XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment,   XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition,     SCENE_RIGHT_POSITION); n++;
    XtSetArg(args[n], XmNbottomAttachment,  XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomOffset,      34); n++;
    XtSetValues(renderArea->getWidget(), args, n);
    
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,     XmATTACH_NONE); n++;
    XtSetArg(args[n], XmNleftAttachment,    XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, 	    SCENE_RIGHT_POSITION/2 - 7); n++;
    XtSetArg(args[n], XmNrightAttachment,   XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition,     SCENE_RIGHT_POSITION/2 + 7); n++;
    XtSetArg(args[n], XmNbottomAttachment,  XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomOffset,      OFFSET); n++;
    XtSetValues(acceptButton, args, n);
    
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,     XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopOffset,         OFFSET); n++;
    XtSetArg(args[n], XmNleftAttachment,    XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition,	    SCENE_RIGHT_POSITION); n++;
    XtSetArg(args[n], XmNleftOffset,        OFFSET); n++;
    XtSetArg(args[n], XmNrightAttachment,   XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightOffset,       OFFSET); n++;
    XtSetArg(args[n], XmNbottomAttachment,  XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomOffset,      OFFSET); n++;
    XtSetValues(controls, args, n);
    
    //
    // manage all children
    //
    XtManageChild(menubar);
    renderArea->show();
    XtManageChild(controls);
    
    //
    // read the X resource for this widget (2)
    //
    char *val;
    SbColor c;
    if (xr.getResource("tile1Color", "Tile1Color", c)) {
	tileColor->rgb.setValues(0, 1, &c);
	tileColor->rgb.setValues(3, 1, &c);
    }
    if (xr.getResource("tile2Color", "Tile2Color", c)) {
	tileColor->rgb.setValues(1, 1, &c);
	tileColor->rgb.setValues(2, 1, &c);
    }
    if (xr.getResource("light1Color", "Light1Color", c)) {
	light0->color.setValue(c);
    }
    if (xr.getResource("light2Color", "Light2Color", c)) {
	light1->color.setValue(c);
    }
    if (xr.getResource("updateFrequency", "UpdateFrequency", val)) {
	 if      (strcmp(val, "continuous") == 0)  
	     updateFreq = CONTINUOUS;
	 else if (strcmp(val, "manual") == 0)  
	     updateFreq = AFTER_ACCEPT;
    }
    if (updateFreq == AFTER_ACCEPT)
	XtManageChild(acceptButton);
    
    // update the sliders based on local scene
    updateLocalComponents();

    return mgrWidget;
}


////////////////////////////////////////////////////////////////////////
//
//    This routine attaches itself to a material node and edit the given
//  material index.
//
// usage: public
//
void
SoXtMaterialEditor::attach(SoMaterial *mtl, int ind)
//
////////////////////////////////////////////////////////////////////////
{
    if ( isAttached() )
	detach();
    
    if (mtl != NULL && ind >= 0) {
	material = mtl;
	material->ref();
	index = ind;
	
	// set all parameters as unchanged by the editor
	for (int i=0; i<6; i++)
	    changedIt[i] = FALSE;
	
	if (isVisible())
	    activate();  // updates and attaches sensor
    }
}

////////////////////////////////////////////////////////////////////////
//
//    This routine detaches itself from the material node.
//
// usage: public

void
SoXtMaterialEditor::detach()
//
////////////////////////////////////////////////////////////////////////
{
    if ( ! isAttached() ) 
    	return;

    deactivate();      // detaches sensor
    material->unref();
    material = NULL;
}

////////////////////////////////////////////////////////////////////////
//
//    This routine activates the editor's sensor, if it is indeed attached.
//    It does not affect the editor's attachment status.
//
// usage: private
//
void
SoXtMaterialEditor::activate()
//
////////////////////////////////////////////////////////////////////////
{
    // attach sensor to its material
    if ( isAttached() && sensor->getAttachedNode() == NULL) {
	copyMaterial(localMaterial, 0, material, index);
	updateLocalComponents();
	sensor->attach(material); // attach AFTER update
    }
}

////////////////////////////////////////////////////////////////////////
//
//    This routine deactivates the editor's sensor without affecting 
//    it's attachent status.
//
// usage: private

void
SoXtMaterialEditor::deactivate()
//
////////////////////////////////////////////////////////////////////////
{
    sensor->detach();
}

////////////////////////////////////////////////////////////////////////
//
//    Callback routine from the color editor, this changes the color
//  of the material node we are attached to.
//
// usage: static, private
//
void
SoXtMaterialEditor::colorEditorCB(
    void *userData, 
    const SbColor *rgbColor)
//
////////////////////////////////////////////////////////////////////////
{
    SoXtMaterialEditor *me = (SoXtMaterialEditor *) userData;
    SbColor finalColor;
    SbBool  updateMaterial = (me->material != NULL && me->updateFreq == CONTINUOUS);
    
    if (me->ignoreCallback)
    	return;
    
    //
    // check which slider(s) needs to be updated
    //
    // update the slider base color, the local material
    // and the attached material. The final material color is
    // 	    finalColor = baseColor * sliderScaleValue.
    //

    // detach sensor while we update the fields and prevent slider callbacks
    if (updateMaterial)
	me->sensor->detach();
    me->ignoreCallback = TRUE;
    
    if (me->activeColor & k_ambient) {
	me->sliders[k_AMBIENT_ID]->setBaseColor(rgbColor->getValue());
	finalColor = rgbColor->getValue();
	finalColor *= me->sliders[k_AMBIENT_ID]->getValue();
	me->localMaterial->ambientColor.setValue(finalColor);
	me->changedIt[k_AMBIENT_ID] = TRUE;
	if (updateMaterial) {
	    me->material->ambientColor.set1Value(me->index, finalColor);
	    if ( me->material->ambientColor.isIgnored() )
		 me->material->ambientColor.setIgnored( FALSE );
	}
    }

    if (me->activeColor & k_diffuse) {
	me->sliders[k_DIFFUSE_ID]->setBaseColor(rgbColor->getValue());
	finalColor = rgbColor->getValue();
	finalColor *= me->sliders[k_DIFFUSE_ID]->getValue();
	me->localMaterial->diffuseColor.setValue(finalColor);
	me->changedIt[k_DIFFUSE_ID] = TRUE;
	if (updateMaterial) {
	    me->material->diffuseColor.set1Value(me->index, finalColor);
	    if ( me->material->diffuseColor.isIgnored() )
		 me->material->diffuseColor.setIgnored( FALSE );
	}
    }

    if (me->activeColor & k_specular) {
	me->sliders[k_SPECULAR_ID]->setBaseColor(rgbColor->getValue());
	finalColor = rgbColor->getValue();
	finalColor *= me->sliders[k_SPECULAR_ID]->getValue();
	me->localMaterial->specularColor.setValue(finalColor);
	me->changedIt[k_SPECULAR_ID] = TRUE;
	if (updateMaterial) {
	    me->material->specularColor.set1Value(me->index, finalColor);
	    if ( me->material->specularColor.isIgnored() )
		 me->material->specularColor.setIgnored( FALSE );
	}
    }

    if (me->activeColor & k_emissive) {
	me->sliders[k_EMISSIVE_ID]->setBaseColor(rgbColor->getValue());
	finalColor = rgbColor->getValue();
	finalColor *= me->sliders[k_EMISSIVE_ID]->getValue();
	me->localMaterial->emissiveColor.setValue(finalColor);
	me->changedIt[k_EMISSIVE_ID] = TRUE;
	if (updateMaterial) {
	    me->material->emissiveColor.set1Value(me->index, finalColor);
	    if ( me->material->emissiveColor.isIgnored() )
		 me->material->emissiveColor.setIgnored( FALSE );
	}
    }
    
    // invoke the callbacks with the new material
    if (me->updateFreq == CONTINUOUS)	
	me->callbackList->invokeCallbacks(me->localMaterial);

    // re-attach the sensor and callbacks
    if (updateMaterial)
    	me->sensor->attach(me->material);
    me->ignoreCallback = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Callback invoked when shininess slider changes value.
//
// usage: static, private
//
void
SoXtMaterialEditor::shininessSliderCB(
    void *userData, 
    float value)
//
////////////////////////////////////////////////////////////////////////
{
    SoXtMaterialEditor *me = (SoXtMaterialEditor *) userData;
    
    if (me->ignoreCallback)
    	return;
    
    if (me->material != NULL && me->updateFreq == CONTINUOUS) {
    	// detach the sensor while we update the field
	me->sensor->detach();
	me->material->shininess.set1Value(me->index, value);
	if ( me->material->shininess.isIgnored() )
	     me->material->shininess.setIgnored( FALSE );
	me->sensor->attach(me->material);
    }
    me->localMaterial->shininess.setValue(value);
    me->changedIt[k_SHININESS_ID] = TRUE;

    // invoke the callbacks with the new material
    if (me->updateFreq == CONTINUOUS)	
	me->callbackList->invokeCallbacks(me->localMaterial);
}

////////////////////////////////////////////////////////////////////////
//
// Callback invoked when transparency slider changes value.
//
// usage: static, private
//
void
SoXtMaterialEditor::transparencySliderCB(
    void *userData, 
    float value)
//
////////////////////////////////////////////////////////////////////////
{
    SoXtMaterialEditor *me = (SoXtMaterialEditor *) userData;
    
    if (me->ignoreCallback)
    	return;
    
    if (me->material != NULL && me->updateFreq == CONTINUOUS) {
    	// detach the sensor while we update the field
	me->sensor->detach();
	me->material->transparency.set1Value(me->index, value);
	if ( me->material->transparency.isIgnored() )
	     me->material->transparency.setIgnored( FALSE );
	me->sensor->attach(me->material);
    }
    me->localMaterial->transparency.setValue(value);
    me->changedIt[k_TRANSPARENCY_ID] = TRUE;

    // invoke the callbacks with the new material
    if (me->updateFreq == CONTINUOUS)	
	me->callbackList->invokeCallbacks(me->localMaterial);
}

////////////////////////////////////////////////////////////////////////
//
// Convenience routine invoked when ambient/diffuse/specualr/emissive
// sliders value changed.
//
// usage: private
//
void
SoXtMaterialEditor::updateMaterialColor(
    SoMFColor	    	*editMtlColor,
    SoMFColor	    	*localMtlColor,
    const float	    	*rgb,
    float   	    	intensity)
//
////////////////////////////////////////////////////////////////////////
{
    SbColor intensityColor(rgb);

    intensityColor *= intensity;

    if (editMtlColor != NULL && updateFreq == CONTINUOUS) {
    	// detach the sensor while we update the field
	sensor->detach();
	editMtlColor->set1Value(index, intensityColor);
	if ( editMtlColor->isIgnored() )
	     editMtlColor->setIgnored( FALSE );
	sensor->attach(material);
    }
    localMtlColor->setValue(intensityColor);

    // invoke the callbacks with the new material
    if (updateFreq == CONTINUOUS)	
	callbackList->invokeCallbacks(localMaterial);
}

////////////////////////////////////////////////////////////////////////
//
// Callback routine invoked when ambient slider value changed.
// We use the ambient slider base color and slider value to determine
// the new color for the edit and local material ambientColor field.
//
// usage: static, private
//
void
SoXtMaterialEditor::ambientSliderCB(
    void *userData, 
    float intensity)
//
////////////////////////////////////////////////////////////////////////
{
    SoXtMaterialEditor *me = (SoXtMaterialEditor *) userData;
    
    if (me->ignoreCallback)
    	return;
    
    me->updateMaterialColor(
    	(me->material != NULL) ? &me->material->ambientColor : NULL,
	&me->localMaterial->ambientColor,
	me->sliders[k_AMBIENT_ID]->getBaseColor(),
    	intensity);
    me->changedIt[k_AMBIENT_ID] = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Callback routine invoked when diffuse slider value changed.
// We use the diffuse slider base color and slider value to determine
// the new color for the edit and local material diffuseColor field.
//
// usage: static, private
//
void
SoXtMaterialEditor::diffuseSliderCB(
    void *userData, 
    float intensity)
//
////////////////////////////////////////////////////////////////////////
{
    SoXtMaterialEditor *me = (SoXtMaterialEditor *) userData;
    
    if (me->ignoreCallback)
    	return;
    
    me->updateMaterialColor(
    	(me->material != NULL) ? &me->material->diffuseColor : NULL,
	&me->localMaterial->diffuseColor,
	me->sliders[k_DIFFUSE_ID]->getBaseColor(),
    	intensity);
    me->changedIt[k_DIFFUSE_ID] = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Callback routine invoked when specular slider value changed.
// We use the specular slider base color and slider value to determine
// the new color for the edit and local material specularColor field.
//
// usage: static, private
//
void
SoXtMaterialEditor::specularSliderCB(
    void *userData, 
    float intensity)
//
////////////////////////////////////////////////////////////////////////
{
    SoXtMaterialEditor *me = (SoXtMaterialEditor *) userData;
    
    if (me->ignoreCallback)
    	return;
    
    me->updateMaterialColor(
    	(me->material != NULL) ? &me->material->specularColor : NULL,
	&me->localMaterial->specularColor,
	me->sliders[k_SPECULAR_ID]->getBaseColor(),
    	intensity);
    me->changedIt[k_SPECULAR_ID] = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Callback routine invoked when emissive slider value changed.
// We use the emissive slider base color and slider value to determine
// the new color for the edit and local material emissiveColor field.
//
// usage: static, private
//
void
SoXtMaterialEditor::emissiveSliderCB(
    void *userData, 
    float intensity)
//
////////////////////////////////////////////////////////////////////////
{
    SoXtMaterialEditor *me = (SoXtMaterialEditor *) userData;
    
    if (me->ignoreCallback)
    	return;
    
    me->updateMaterialColor(
    	(me->material != NULL) ? &me->material->emissiveColor : NULL,
	&me->localMaterial->emissiveColor,
	me->sliders[k_EMISSIVE_ID]->getBaseColor(),
    	intensity);
    me->changedIt[k_EMISSIVE_ID] = TRUE;
}


////////////////////////////////////////////////////////////////////////
//
//  Convenience routine which updates the given slider, from the given
//  material color (have to split the base color and intensity).
//
//  Use: private
//
void
SoXtMaterialEditor::updateColorSlider(
    	    _SoXtColorSlider *slider,
	    const float rgb[3])
//
////////////////////////////////////////////////////////////////////////
{
    float max;
    float baseColor[3];

    // get color intensity
    max = (rgb[0] > rgb[1]) ? 
    	    ((rgb[0] > rgb[2]) ? rgb[0] : rgb[2]) :
	    ((rgb[1] > rgb[2]) ? rgb[1] : rgb[2]);

    if (max == 0.0)	// handle special case
	baseColor[0] = baseColor[1] = baseColor[2] = 1.0;
    else {
	// scale the color to full intensity
	float scale = 1.0 / max;
	baseColor[0] = rgb[0] * scale;
	baseColor[1] = rgb[1] * scale;
	baseColor[2] = rgb[2] * scale;
    }

    // now set the slider to the right baseColor and value
    ignoreCallback = TRUE;
    slider->setBaseColor(baseColor);
    slider->setValue(max);
    ignoreCallback = FALSE;
}


////////////////////////////////////////////////////////////////////////
//
//  Copies material2 onto material1
//
//  Use: private
//
void
SoXtMaterialEditor::copyMaterial(SoMaterial *mat1, int ind1, 
			    const SoMaterial *mat2, int ind2)
//
////////////////////////////////////////////////////////////////////////
{
    mat1->ambientColor.set1Value(ind1, mat2->ambientColor[ind2]);
    mat1->diffuseColor.set1Value(ind1, mat2->diffuseColor[ind2]);
    mat1->specularColor.set1Value(ind1, mat2->specularColor[ind2]);
    mat1->emissiveColor.set1Value(ind1, mat2->emissiveColor[ind2]);
    mat1->shininess.set1Value(ind1, mat2->shininess[ind2]);
    mat1->transparency.set1Value(ind1, mat2->transparency[ind2]);
}

////////////////////////////////////////////////////////////////////////
//
//  For each of the 6 sliders (or sets of sliders) sets the ignore flag 
//  of the material node being editted to FALSE if it has been changed.
//
//  Use: private
//
void
SoXtMaterialEditor::undoIgnoresIfChanged()
//
////////////////////////////////////////////////////////////////////////
{
    if ( changedIt[k_AMBIENT_ID] )
	material->ambientColor.setIgnored( FALSE );
    if ( changedIt[k_DIFFUSE_ID] )
	material->diffuseColor.setIgnored( FALSE );
    if ( changedIt[k_SPECULAR_ID] )
	material->specularColor.setIgnored( FALSE );
    if ( changedIt[k_EMISSIVE_ID] )
	material->emissiveColor.setIgnored( FALSE );
    if ( changedIt[k_SHININESS_ID] )
	material->shininess.setIgnored( FALSE );
    if ( changedIt[k_TRANSPARENCY_ID] )
	material->transparency.setIgnored( FALSE );
}


////////////////////////////////////////////////////////////////////////
//
//  Update the sliders and colorEditor based on the local material.
//
//  Use: private
//
void
SoXtMaterialEditor::updateLocalComponents()
//
////////////////////////////////////////////////////////////////////////
{
    // Update the color sliders to reflect these values
    updateColorSlider(sliders[k_AMBIENT_ID], localMaterial->ambientColor[0].getValue());
    updateColorSlider(sliders[k_DIFFUSE_ID], localMaterial->diffuseColor[0].getValue());
    updateColorSlider(sliders[k_SPECULAR_ID], localMaterial->specularColor[0].getValue());
    updateColorSlider(sliders[k_EMISSIVE_ID], localMaterial->emissiveColor[0].getValue());

    // update non color sliders
    ignoreCallback = TRUE;
    sliders[k_SHININESS_ID]->setValue(localMaterial->shininess[0]);
    sliders[k_TRANSPARENCY_ID]->setValue(localMaterial->transparency[0]);
    ignoreCallback = FALSE;
    
    updateColorEditor();
}


////////////////////////////////////////////////////////////////////////
//
// updates the colorEditor (show/hide and update color) based on 
// the activeColor flag.
//
// usage: private
//
void
SoXtMaterialEditor::updateColorEditor(SbBool updateTitle)
//
////////////////////////////////////////////////////////////////////////
{
    // show/hide the colorEditor
    if (activeColor == k_none) {
    	if (colorEditor == NULL || ! colorEditor->isVisible())
	    return;
	colorEditor->hide();
	return;
    }
    else {
	if (colorEditor == NULL) {
	    // build comp under our shell widget
	    colorEditor = new _SoXtColorEditor(
		    SoXt::getShellWidget(getWidget()), 
		    NULL, FALSE);
	    colorEditor->setCurrentSliders(_SoXtColorEditor::NONE);
	    colorEditor->addColorChangedCallback(
		SoXtMaterialEditor::colorEditorCB, this);
	    
	    // add a close window callback so that toggles
	    // can reflect the change (turn them off)
	    colorEditor->setWindowCloseCallback(
		SoXtMaterialEditor::colorEditorCloseCB, this);
	}
	
    	colorEditor->show();
    }
    
    // sets the right color, ignoring the colorEditor callback
    ignoreCallback = TRUE;
    switch (activeColor) {
	case k_none:
	    break;
	case k_ambient:
	    colorEditor->setColor(sliders[k_AMBIENT_ID]->getBaseColor());
	    if (updateTitle)
	    	colorEditor->setTitle(ambientString);
	    break;
	case k_diffuse:
	    colorEditor->setColor(sliders[k_DIFFUSE_ID]->getBaseColor());
	    if (updateTitle)
	    	colorEditor->setTitle(diffuseString);
	    break;
	case k_specular:
	    colorEditor->setColor(sliders[k_SPECULAR_ID]->getBaseColor());
	    if (updateTitle)
	    	colorEditor->setTitle(specularString);
	    break;
	case k_emissive:
	    colorEditor->setColor(sliders[k_EMISSIVE_ID]->getBaseColor());
	    if (updateTitle)
	    	colorEditor->setTitle(emissiveString);
	    break;
	default:
	    // ??? this is not really perfect because the colorEditor
	    // ??? doesn't yet support a mutiple attach state (where sliders
	    // ??? also reflect the conficting colors)
	    if (updateTitle) {
		char str[50];
		
		strcpy(str, "Material ");
		if (activeColor & k_ambient)
		    strcat(str, "Amb/");
		if (activeColor & k_diffuse)
		    strcat(str, "Diff/");
		if (activeColor & k_specular)
		    strcat(str, "Spec/");
		if (activeColor & k_emissive)
		    strcat(str, "Emis/");
		str[strlen(str) - 1] = ' ';
		strcat(str, "Color");
		
	    	colorEditor->setTitle(str);
	    }
	    break;
    }
    ignoreCallback = FALSE;
}


////////////////////////////////////////////////////////////////////////
//
//  Sensor callback, invoked whenever the edited material node changes
//  (other than when we change it).
//
//  Use: private
//
void
SoXtMaterialEditor::sensorCB(void *p, SoSensor *)
//
////////////////////////////////////////////////////////////////////////
{
    SoXtMaterialEditor *me = (SoXtMaterialEditor *)p;
    if (!me->isVisible())
	return;
    
    // copy edited material over and update sliders/colorEditor
    me->copyMaterial(me->localMaterial, 0, me->material, me->index);
    me->updateLocalComponents();
}

////////////////////////////////////////////////////////////////////////
//
//  Called by Xt when a menu is about to be displayed.
//  This gives us a chance to update any items in the menu.
//
//  Use: private
//
void
SoXtMaterialEditor::menuDisplay(Widget, SoXtMaterialEditor *me, XtPointer)
//
////////////////////////////////////////////////////////////////////////
{
    if (me->updateFreq == CONTINUOUS) {
    	TOGGLE_ON(me->menuItemsList[k_CONTINUOUS]);
    	TOGGLE_OFF(me->menuItemsList[k_AFTER_ACCEPT]);
    }
    else {
    	TOGGLE_OFF(me->menuItemsList[k_CONTINUOUS]);
    	TOGGLE_ON(me->menuItemsList[k_AFTER_ACCEPT]);
    }
}

////////////////////////////////////////////////////////////////////////
//
//  Called by Xt when a menu item is picked.
//
//  Use: private
//
void
SoXtMaterialEditor::menuPick(
    Widget w,
    int id,
    XmAnyCallbackStruct *cb)
//
////////////////////////////////////////////////////////////////////////
{
    SoXtMaterialEditor	*p;

    XtVaGetValues(w, XmNuserData, &p, NULL);

    Time eventTime = cb->event->xbutton.time;

    switch (id) {
    	case k_CONTINUOUS:
	    p->setUpdateFrequency(CONTINUOUS);
	    break;

	case k_AFTER_ACCEPT:
	    p->setUpdateFrequency(AFTER_ACCEPT);
	    break;

	case k_MATERIAL_LIST:
	    // build comp under our shell widget
	    if (p->materialList == NULL) {
		p->materialList = new SoXtMaterialList(
		    SoXt::getShellWidget(p->getWidget()), 
		    NULL, FALSE, mtlDir);
		p->materialList->addCallback(
		    SoXtMaterialEditor::materialListCB, p);
		p->materialList->setWindowCloseCallback(
		    SoXtMaterialEditor::materialListCloseCB, p);
	    }
	    p->materialList->show();
	    p->openMaterialList = TRUE;
	    break;

    	case k_EDIT_COPY:
	        if (p->clipboard == NULL)
		    p->clipboard = new SoXtClipboard(p->getWidget());
		p->clipboard->copy(p->localMaterial, eventTime);
	    break;
	    
    	case k_EDIT_PASTE:
		if (p->clipboard == NULL)
		    p->clipboard = new SoXtClipboard(p->getWidget());
	    
		p->clipboard->paste(eventTime,
					 SoXtMaterialEditor::pasteDone,
					 p);
	    break;
	    
	default:
#if DEBUG
	    SoDebugError::post("SoXtMaterialEditor::menuPick",
		"id %d", id);
#endif
	    break;
	    
	case k_EDIT_HELP:
	    p->openHelpCard("SoXtMaterialEditor.help");
	    break;
    }
}


////////////////////////////////////////////////////////////////////////
//
//  Called by Xt when a diamond radio button is picked.
//
//  Use: static private
//
void
SoXtMaterialEditor::diamondButtonPick(Widget w, int ID, XtPointer)
//
////////////////////////////////////////////////////////////////////////
{
    SoXtMaterialEditor *p;
    Arg	args[1];
    int i;
    
    XtSetArg(args[0], XmNuserData, &p);
    XtGetValues(w, args, 1);
    
    if (XmToggleButtonGetState(w)) {	// select only this slider
	
	// clear all other toggles
	for (i=0; i<4; i++) {
	    if (i == ID)
	    	TOGGLE_ON(p->radioButtons[i]);
	    else {
	    	TOGGLE_OFF(p->diamondButtons[i]);
	    	TOGGLE_OFF(p->radioButtons[i]);
	    }
    	}
	
	// set the activeColor flag
	switch(ID) {
	    case k_AMBIENT_ID:	p->activeColor = k_ambient;    break;
	    case k_DIFFUSE_ID:	p->activeColor = k_diffuse;    break;
	    case k_SPECULAR_ID:	p->activeColor = k_specular;   break;
	    case k_EMISSIVE_ID:	p->activeColor = k_emissive;   break;
	}
    }
    else {  // no slider selected, clear the activeColor flag
	
	TOGGLE_OFF(p->radioButtons[ID]);
	p->activeColor = k_none;
    }
    
    // finally update the colorEditor
    p->updateColorEditor(TRUE);
}


////////////////////////////////////////////////////////////////////////
//
//  Called by Xt when a radio button is picked.
//
//  Use: static private
//
void
SoXtMaterialEditor::radioButtonPick(Widget w, int ID, XtPointer)
//
////////////////////////////////////////////////////////////////////////
{
    SoXtMaterialEditor *p;
    Arg	args[1];
    int i;
    
    XtSetArg(args[0], XmNuserData, &p);
    XtGetValues(w, args, 1);
    
    if (XmToggleButtonGetState(w)) { 	// select this slider
	
	if (p->activeColor == k_none)
	    TOGGLE_ON(p->diamondButtons[ID]);
	else {
	    for (i=0; i<4; i++)
	    	TOGGLE_OFF(p->diamondButtons[i]);
    	}
	
	// set the right bit
	switch(ID) {
	    case k_AMBIENT_ID:	p->activeColor |= k_ambient;   break;
	    case k_DIFFUSE_ID:	p->activeColor |= k_diffuse;   break;
	    case k_SPECULAR_ID:	p->activeColor |= k_specular;  break;
	    case k_EMISSIVE_ID:	p->activeColor |= k_emissive;  break;
	}
    }
    else {  // unselect this slider
	
	TOGGLE_OFF(p->diamondButtons[ID]);
	
	// clear the right bit
	switch(ID) {
	    case k_AMBIENT_ID:	p->activeColor &= ~k_ambient;  break;
	    case k_DIFFUSE_ID:	p->activeColor &= ~k_diffuse;  break;
	    case k_SPECULAR_ID:	p->activeColor &= ~k_specular; break;
	    case k_EMISSIVE_ID:	p->activeColor &= ~k_emissive; break;
	}
	
	// check if a diamond button can be set (i.e. only one slider selected)
	switch (p->activeColor) {
	    case k_ambient: 	TOGGLE_ON(p->diamondButtons[k_AMBIENT_ID]);    break;
	    case k_diffuse: 	TOGGLE_ON(p->diamondButtons[k_DIFFUSE_ID]);    break;
	    case k_specular:	TOGGLE_ON(p->diamondButtons[k_SPECULAR_ID]);   break;
	    case k_emissive:	TOGGLE_ON(p->diamondButtons[k_EMISSIVE_ID]);   break;
	}
    }
    
    // finally update the colorEditor
    p->updateColorEditor(TRUE);
}


////////////////////////////////////////////////////////////////////////
//
// called when the accept button gets pressed.
//
// usage: static, private
//
void
SoXtMaterialEditor::acceptButtonCB(Widget, SoXtMaterialEditor *me, XtPointer)
//
////////////////////////////////////////////////////////////////////////
{
    // copy the local material to the edited material
    if (me->material != NULL) {
    	// detach the sensor while we update the fields
	me->sensor->detach();
	me->copyMaterial(me->material, me->index, me->localMaterial, 0);
	me->undoIgnoresIfChanged();
	me->sensor->attach(me->material);
    }

    // invoke the callbacks with the new material
    me->callbackList->invokeCallbacks(me->localMaterial);
}


////////////////////////////////////////////////////////////////////////
//
// Set the local material and the edited material to the passed value.
//
// usage: public
//
void
SoXtMaterialEditor::setMaterial(const SoMaterial &mtl)
//
////////////////////////////////////////////////////////////////////////
{
    // copy the new material locally and update the sliders
    copyMaterial(localMaterial, 0, &mtl, 0);
    updateLocalComponents();
    // signal that values have been loaded into all fields
    for (int i = 0; i < 6; i++)
	changedIt[i] = TRUE;

    // update the attached material
    if (material != NULL && updateFreq == CONTINUOUS) {
    	// detach the sensor while we update the fields
	sensor->detach();
	copyMaterial(material, index, &mtl, 0);
	undoIgnoresIfChanged();
	sensor->attach(material);
    }

    // invoke the callbacks with the new material
    if (updateFreq == CONTINUOUS)	
	callbackList->invokeCallbacks(localMaterial);
}

////////////////////////////////////////////////////////////////////////
//
// Callback invoked when a material is chosen from whithin the material list
//
// usage: static, private
//
void
SoXtMaterialEditor::materialListCB(
    void *me,
    const SoMaterial *mtl)
//
////////////////////////////////////////////////////////////////////////
{
    ((SoXtMaterialEditor *)me)->setMaterial(*mtl);
}

////////////////////////////////////////////////////////////////////////
//
//  The X server has finished getting the selection data, and the
//  paste is complete. Look through the paste data for a material node.
//
//  Use: static private
//
void
SoXtMaterialEditor::pasteDone(void *pt, SoPathList *pathList)
//
////////////////////////////////////////////////////////////////////////
{
    SoXtMaterialEditor *p = (SoXtMaterialEditor *) pt;
    
    SoSearchAction sa;
    SoPath *path = NULL;
    
    //
    // search for first material in that pasted scene
    //
    sa.setType(SoMaterial::getClassTypeId());
    for (int i=0; i < pathList->getLength(); i++) {
	sa.apply( (*pathList)[i] );
	if ( (path = sa.getPath()) != NULL) {
	    
	    // assign new material
	    p->setMaterial(*((SoMaterial *) path->getTail()));
	    break;
	}
    }
    
    // ??? what if a color is pasted. I guess they should paste into the
    // ??? color editor used by the component, rather than us trying
    // ??? to create a material based on the given color. (alain)
     
    // ??? We delete the callback data when done with it.
    delete pathList;
}

//
// called when the color editor gets closed by the user (turn the toggle off
// and hide the component).
//
void
SoXtMaterialEditor::colorEditorCloseCB(void *pt, SoXtComponent *comp)
{
    SoXtMaterialEditor *p = (SoXtMaterialEditor *)pt;
    
    for (int i=0; i<4; i++) {
	TOGGLE_OFF(p->diamondButtons[i]);
	TOGGLE_OFF(p->radioButtons[i]);
    }
    
    p->activeColor = k_none;
    comp->hide();
}

//
// called when the material list gets closed by the user.
//
void
SoXtMaterialEditor::materialListCloseCB(void *pt, SoXtComponent *comp)
{
    SoXtMaterialEditor *p = (SoXtMaterialEditor *)pt;
    
    p->openMaterialList = FALSE;
    comp->hide();
}

//
// called whenever the component becomes visibble or not
//
void
SoXtMaterialEditor::visibilityChangeCB(void *pt, SbBool visible)
{
    SoXtMaterialEditor *p = (SoXtMaterialEditor *)pt;
    
    if (visible)
	p->activate();
    else
	p->deactivate();
}



//
// redefine those generic virtual functions
//
const char *
SoXtMaterialEditor::getDefaultWidgetName() const
{ return thisClassName; }

const char *
SoXtMaterialEditor::getDefaultTitle() const
{ return "Material Editor"; }

const char *
SoXtMaterialEditor::getDefaultIconTitle() const
{ return "Mat Editor"; }

