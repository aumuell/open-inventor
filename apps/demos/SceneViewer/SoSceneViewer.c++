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
 * Copyright (C) 1990-95   Silicon Graphics, Inc.
 *
 * 
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.11 $
 |
 |   Classes	: SoSceneViewer
 |
 |   Author(s)	: Thad Beier, David Mott, Alain Dumesny, Paul Isaacs,
 |		  Rikk Carey, Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <stdlib.h> // for system() and getenv()
#include <unistd.h> // for access()

#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Xatom.h>

#include <Xm/Protocols.h>
#include <Xm/Xm.h>
#include <Xm/BulletinB.h>
#include <Xm/CascadeB.h>
#include <Xm/CascadeBG.h>
#include <Xm/FileSB.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/FileSB.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/SeparatoG.h>
#include <Xm/Text.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <Xm/DropTrans.h>

#include <Inventor/SoDB.h>
#include <Inventor/misc/SoByteStream.h>
#include <Inventor/SoNodeKitPath.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/SoLists.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/SoXtClipboard.h>
#include <Inventor/Xt/SoXtDirectionalLightEditor.h>
#include <Inventor/Xt/SoXtMaterialEditor.h>
#include <Inventor/Xt/SoXtPrintDialog.h>
#include <Inventor/Xt/SoXtResource.h>
#include <Inventor/Xt/SoXtTransformSliderSet.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/Xt/viewers/SoXtFlyViewer.h>
#include <Inventor/Xt/viewers/SoXtPlaneViewer.h>
#include <Inventor/Xt/viewers/SoXtWalkViewer.h>
#include <Inventor/actions/SoBoxHighlightRenderAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/details/SoNodeKitDetail.h>
#include <Inventor/draggers/SoDirectionalLightDragger.h>
#include <Inventor/draggers/SoTabBoxDragger.h>
#include <Inventor/manips/SoCenterballManip.h>
#include <Inventor/manips/SoDirectionalLightManip.h>
#include <Inventor/manips/SoHandleBoxManip.h>
#include <Inventor/manips/SoJackManip.h>
#include <Inventor/manips/SoPointLightManip.h>
#include <Inventor/manips/SoSpotLightManip.h>
#include <Inventor/manips/SoTabBoxManip.h>
#include <Inventor/manips/SoTrackballManip.h>
#include <Inventor/manips/SoTransformBoxManip.h>
#include <Inventor/manips/SoTransformerManip.h>
#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/nodekits/SoCameraKit.h>
#include <Inventor/nodekits/SoSceneKit.h>
#include <Inventor/nodes/SoCallback.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoEnvironment.h>
#include <Inventor/nodes/SoLabel.h>
#include <Inventor/nodes/SoLight.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoPointLight.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoShape.h>
#include <Inventor/nodes/SoSpotLight.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoTranslation.h>

#include "SoSceneViewer.h"
#include "SoSceneMenu.h"
#include "SvManipList.h"
#include "../../samples/widgets/MyColorEditor.h"
#include "../../samples/widgets/MyFileRead.h"
#include "../../samples/widgets/MyDropSite.h"
#include "../../samples/common/InventorLogo.h"

#include <GL/gl.h>
#ifdef DEBUG
#include <assert.h>
#endif
 

//
//  Macros and constants
//

// toggle button macros
#define TOGGLE_ON(BUTTON) \
    XmToggleButtonSetState((Widget) BUTTON, TRUE, FALSE)
#define TOGGLE_OFF(BUTTON) \
    XmToggleButtonSetState((Widget) BUTTON, FALSE, FALSE)

#define	FOG_FUDGE	1.6
#define SV_NUM_LIGHTS	6

#define SWITCH_LIGHT_OFF(SWITCH) (SWITCH)->whichChild.setValue(SO_SWITCH_NONE)
#define SWITCH_LIGHT_ON(SWITCH)  (SWITCH)->whichChild.setValue(SO_SWITCH_ALL)
#define IS_LIGHT_ON(SWITCH)	 ((SWITCH)->whichChild.getValue() == SO_SWITCH_ALL)

#define SV_ENV_LABEL "SoSceneViewer Environment v3.0"

#define _ATOM_(DISPLAY,NAME) (XmInternAtom(DISPLAY, NAME, False))

#define SCREEN(w) XScreenNumberOfScreen(XtScreen(w))

//
//  Structs
//

struct SoSceneViewerData {
    int id;
    SoSceneViewer *classPt;
    Widget widget;
};

class SvLightData {
  public:
    // Constructor inits everything to NULL
    SvLightData();
    
    SoSceneViewer   *classPt;
    SoSwitch	    *lightSwitch;
    SoTranslation   *translation;   // for placing a directional light manip
    SoScale         *scale;
    SoLight	    *light;
    SoScale         *scaleInverse;
    SoTranslation   *translationInverse;
    SoType	    type;
    char	    *name;
    MyColorEditor *colorEditor;
    SbBool     	    isManip();
    SbBool     	    shouldBeManip; // Used to remember what it was when
				   // they all get turned off for writing,
				   // printing, etc.
    Widget	    cascadeWidget;
    Widget	    submenuWidget;
    Widget	    onOffWidget;
    Widget	    iconWidget;
    Widget	    editColorWidget;
    Widget	    removeWidget;
};

SvLightData::SvLightData() 
{
    classPt = NULL;
    lightSwitch = NULL;
    translation = NULL;
    scale = NULL;
    light = NULL;
    scaleInverse = NULL;
    translationInverse = NULL;
    name = NULL;
    colorEditor = NULL;
    shouldBeManip = FALSE;
    cascadeWidget = NULL;
    submenuWidget = NULL;
    onOffWidget = NULL;
    iconWidget = NULL;
    editColorWidget = NULL;
    removeWidget = NULL;
}

SbBool SvLightData::isManip() 
{
    if (light == NULL)
	return FALSE;

    if (light->isOfType( SoDirectionalLightManip::getClassTypeId()))
	return TRUE;
    else if (light->isOfType( SoPointLightManip::getClassTypeId()) )
	return TRUE;
    else if (light->isOfType( SoSpotLightManip::getClassTypeId()) )
	return TRUE;

    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
//  Set the Inventor logo on the screen.
//
static void
logoCB(void *, SoAction *action)
{
    if (action->isOfType(SoGLRenderAction::getClassTypeId())) {
	glViewport(0, 0, 80, 80);
    }
}

static void
setOverlayLogo(SoXtRenderArea *ra)
{
    static SoSeparator *logo = NULL;
    
    if (logo == NULL) {
	SoInput in;
	in.setBuffer((void *)ivLogo, ivLogoSize);
	logo = SoDB::readAll(&in);
	logo->ref();
	
	// Add a callback node which will set the viewport
	SoCallback *cb = new SoCallback;
	cb->setCallback(logoCB);
	logo->insertChild(cb, 0);
    }
    
    SbColor col(1, 1, 1);
    ra->setOverlayColorMap(1, 1, &col);
    ra->setOverlaySceneGraph(logo);	
}
//
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//
// Public constructor - build the widget right now
//
SoSceneViewer::SoSceneViewer(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent, 
    SoSelection *inputGraph,
    const char *envFile) 
	: SoXtComponent(
	    parent,
	    name, 
	    buildInsideParent) 
//
////////////////////////////////////////////////////////////////////////
{
    // In this case, render area is what the app wants, so buildNow = TRUE
    constructorCommon(inputGraph, envFile, TRUE);
}

////////////////////////////////////////////////////////////////////////
//
// SoEXTENDER constructor - the subclass tells us whether to build or not
//
SoSceneViewer::SoSceneViewer(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent, 
    SoSelection *inputGraph,
    const char *envFile, 
    SbBool buildNow)
	: SoXtComponent(
	    parent,
	    name, 
	    buildInsideParent) 
//
////////////////////////////////////////////////////////////////////////
{
    // In this case, render area may be what the app wants, 
    // or it may want a subclass of render area. Pass along buildNow
    // as it was passed to us.
    constructorCommon(inputGraph, envFile, buildNow);
}

////////////////////////////////////////////////////////////////////////
//
// Called by the constructors
//
// private
//
void
SoSceneViewer::constructorCommon(
    SoSelection *inputGraph,
    const char *envFile, 
    SbBool buildNow)
//
////////////////////////////////////////////////////////////////////////
{
    int	    i;
    
    // getting resources.
    XtGetSubresources(  getParentWidget(), &fileData[0], "intl", "Intl",
                        intl_file_resources, intl_num_file_resources,
			NULL, 0 );
    XtGetSubresources(  getParentWidget(), &editData[0], "intl", "Intl",
                        intl_edit_resources, intl_num_edit_resources,
			NULL, 0 );
    XtGetSubresources(  getParentWidget(), &manipData[0], "intl", "Intl",
                        intl_manip_resources, intl_num_manip_resources,
			NULL, 0 );
    XtGetSubresources(  getParentWidget(), &selData[0], "intl", "Intl",
                        intl_sel_resources, intl_num_sel_resources,
			NULL, 0 );
    XtGetSubresources(  getParentWidget(), &viewData[0], "intl", "Intl",
                        intl_view_resources, intl_num_view_resources,
			NULL, 0 );
    XtGetSubresources(  getParentWidget(), &editorData[0], "intl", "Intl",
                        intl_editor_resources, intl_num_editor_resources,
			NULL, 0 );
    XtGetSubresources(  getParentWidget(), &lightData[0], "intl", "Intl",
                        intl_light_resources, intl_num_light_resources,
			NULL, 0 );
    XtGetSubresources(  getParentWidget(), &Light, "intl", "Intl",
                        intl_lightType_resources, intl_num_lightType_resources,
			NULL, 0 );
    XtGetSubresources(  getParentWidget(), &pulldownData[0], "intl", "Intl",
                        intl_pulldown_resources, intl_num_pulldown_resources,
			NULL, 0 );

    setClassName("SoSceneViewer");
    setSize( SbVec2s(520, 510) );

    // selection is the users scene graph.
    selection  = inputGraph;
    currentViewer = NULL;
    createLightsCameraEnvironment();
    
    // the scene viewer supplies its own camera and lights.
    // in fact, we remove any cameras that might be in the users graph.
    // NOTE: since the camera may be switched by the viewer (ortho/perspective toggle)
    // make sure to get the camera from the viewer (and not cache the camera).
    sceneGraph = new SoSeparator();
    sceneGraph->ref();	// must ref it
    sceneGraph->addChild(lightsCameraEnvironment);
    sceneGraph->addChild(selection);

    // Create the dictionary that keeps track of original camera
    // settings in scenekits
    sceneKitCamNumDict = new SbDict(5);;
#ifndef EXPLORER
    SoCamera *camFromScene = getLastSceneKitCamera(selection);
    switchOffSceneKitCameras(selection);
    removeCameras(selection);
#endif
    
    //
    // Widget and menu variables
    //
    mgrWidget = NULL;
    showMenuFlag = TRUE;
    menuWidget = NULL;
    menuItems = new SoSceneViewerData[SV_MENU_NUM];
    for (i=0; i<SV_MENU_NUM; i++) {
	menuItems[i].id = i;
	menuItems[i].classPt = this;
	menuItems[i].widget = NULL;
    }
    
    //
    // File
    //
    fileName	 = NULL;
    fileDialog	 = NULL;
    printDialog	 = NULL;
    
    //
    // Viewing
    //
    // Allocate only one viewer at a time. The other viewers will
    // be allocated as needed to increase speed and save memory.
    //
    for (i=0; i<4; i++)
	viewerList[i] = NULL;
	
    
    // fog
    fogFlag = FALSE;
    environment->fogType.setValue( SoEnvironment::NONE );    

    antialiasingFlag = FALSE;
    backgroundColorEditor = NULL;

    //
    // Selection
    //     These callbacks are used to update the SceneViewer state after
    //     the current selection changes (e.g. attach/detach editors and manips).
    //
    selection->addSelectionCallback(SoSceneViewer::selectionCallback, this);
    selection->addDeselectionCallback(SoSceneViewer::deselectionCallback, this);
    selection->setPickFilterCallback(SoSceneViewer::pickFilterCB, this);

    highlightRA = new SoBoxHighlightRenderAction;
    
    //
    // Editors
    //
    ignoreCallback = FALSE;
    materialEditor  = NULL;
    colorEditor	    = NULL;
    transformSliderSet = NULL;

    //
    // Manips
    //
    curManip = SV_NONE;
    highlightRA->setVisible(TRUE); // highlight visible when no manip
    curManipReplaces = TRUE;
    maniplist = new SvManipList;
    
#ifdef EXPLORER
    //
    // User callback
    //
    userModeCB = NULL;
    userModedata = NULL;
    userModeFlag = FALSE;
#endif /* EXPLORER */
    
    //
    // Lights
    //
    ambientColorEditor = NULL;
    headlightData = new SvLightData;
    headlightData->classPt = this;
    headlightData->name = Light.headlight;
    headlightData->type = SoDirectionalLight::getClassTypeId();
    headlightData->colorEditor = NULL;
    headlightData->shouldBeManip = FALSE;
    headlightEditor = NULL;
    calculatedLightManipSize = FALSE;
    
    // Build the widget tree, and let SoXtComponent know about our base widget.
    if (buildNow) {
	Widget w = buildWidget(getParentWidget());
	setBaseWidget(w);
    }
    
    // do this after everything else has been set up (viewer needs to exists 
    // because the camera is gotten from the viewer (not cached here))
    if (envFile != NULL)
    	readEnvFile(envFile);

    // If scene had sceneKit with a camera, copy its values into our camera.
    if (camFromScene)
	setCameraFields( camFromScene );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: public

SoSceneViewer::~SoSceneViewer()
//
////////////////////////////////////////////////////////////////////////
{
    // Delete the dictionary that keeps track of original camera
    // settings in scenekits
    delete sceneKitCamNumDict;

    // detach and delete the manips
    detachManipFromAll();
    delete maniplist;

    // detach and delete the viewers
    currentViewer->setSceneGraph(NULL);
    delete (SoXtExaminerViewer *) viewerList[SV_VWR_EXAMINER];
    delete (SoXtFlyViewer *) viewerList[SV_VWR_FLY];
    delete (SoXtWalkViewer *) viewerList[SV_VWR_WALK];
    delete (SoXtPlaneViewer *) viewerList[SV_VWR_PLANE];
    
    // delete menu items data
    delete [ /*SV_MENU_NUM*/ ] menuItems;
    delete headlightData;
    delete headlightEditor;
    
    delete printDialog;
    
    // Editor components
    delete materialEditor;
    delete colorEditor;
    delete transformSliderSet;
    delete ambientColorEditor;
    delete backgroundColorEditor;

    sceneGraph->unref();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	New data is going to be coming into the viewer.  Time to disconnect all
//  manipulators and picking, and wait for new information.  Might as well go
//  into a viewing mode as well, this gets rid of the manipulators, and puts
//  the user in control of viewing when new data shows up.
//
// Use: public
void
SoSceneViewer::newData()
//
////////////////////////////////////////////////////////////////////////
{
    selection->deselectAll();
}

#ifdef EXPLORER
////////////////////////////////////////////////////////////////////////
//
// Description:
//	This sets the user mode callack routine
//
// Use: public
void 
SoSceneViewer::setUserModeEventCallback(SoXtRenderAreaEventCB *fcn)
//
////////////////////////////////////////////////////////////////////////
{
    userModeCB = fcn;
    userModedata = currentViewer->getSceneGraph();
    if (userModeFlag)
	currentViewer->setEventCallback(userModeCB, userModedata);
}
#endif /* EXPLORER */

////////////////////////////////////////////////////////////////////////
//
// Description:
//	switches from the current viewer to the given viewer. The new
//  viewer will automatically be allocated if needed, and set to have 
//  the same settings the current viewer has (drawing style, buffer 
//  type, etc..).
//
// Use: private
void
SoSceneViewer::switchToViewer(SvEViewer newViewer)
//
////////////////////////////////////////////////////////////////////////
{
    if (whichViewer == newViewer)
    	return;
    
    // remove the sensor + scene graph from the old viewer (to prevent
    // an unecessary redraw of the old viewer)
    currentViewer->setAutoRedraw(FALSE);
    currentViewer->setSceneGraph(NULL);
    
    // allocate the viewer if needed and set the window title.
    // all viewers share the same highlight render action.
    switch (newViewer) {
	case SV_VWR_EXAMINER:
	    // examiner vwr is already created in build()
	    setTitle("SceneViewer (Examiner)");
	    break;
	case SV_VWR_FLY:
	    if ( viewerList[newViewer] == NULL ) {
		viewerList[newViewer] = new SoXtFlyViewer(mgrWidget);
		viewerList[newViewer]->setGLRenderAction(highlightRA);
		viewerList[newViewer]->redrawOnSelectionChange(selection);
	    }
	    setTitle("SceneViewer (Fly)");
	    break;
	case SV_VWR_WALK:
	    if ( viewerList[newViewer] == NULL ) {
		viewerList[newViewer] = new SoXtWalkViewer(mgrWidget);
		viewerList[newViewer]->setGLRenderAction(highlightRA);
		viewerList[newViewer]->redrawOnSelectionChange(selection);
	    }
	    setTitle("SceneViewer (Walk)");
	    break;
	case SV_VWR_PLANE:
	    if ( viewerList[newViewer] == NULL ) {
		viewerList[newViewer] = new SoXtPlaneViewer(mgrWidget);
		viewerList[newViewer]->setGLRenderAction(highlightRA);
		viewerList[newViewer]->redrawOnSelectionChange(selection);
	    }
	    setTitle("SceneViewer (Plane)");
	    break;
    }
    SoXtFullViewer *newVwr = viewerList[newViewer];
    
    // re-init the render action (since it is shared between viewers) now that
    // we are changing windows + set the new scene graph
    newVwr->setAutoRedraw(TRUE);
    newVwr->setSceneGraph(sceneGraph);
    newVwr->getGLRenderAction()->invalidateState();
    
    //
    // make sure the new viewer has all the same settings as 
    // the current viewer. 
    //
    
    // XtRenderArea methods
    //???can't each viewer have it's own background color? especially
    //???if we nuke the background color editor. The user might set the
    //???colors to be different in the app-defaults file.
    //???newVwr->setBackgroundColor( currentViewer->getBackgroundColor() );
    
    environment->fogColor.setValue( newVwr->getBackgroundColor() );
    
    newVwr->setClearBeforeRender( currentViewer->isClearBeforeRender() );
    // XtViewer methods
    newVwr->setHeadlight( currentViewer->isHeadlight() );
    newVwr->setDrawStyle( 
		SoXtViewer::STILL,
		currentViewer->getDrawStyle(SoXtViewer::STILL) );
    newVwr->setDrawStyle( 
		SoXtViewer::INTERACTIVE,
		currentViewer->getDrawStyle(SoXtViewer::INTERACTIVE) );
    newVwr->setBufferingType( currentViewer->getBufferingType() );
    newVwr->setViewing( currentViewer->isViewing() );
    newVwr->setAutoClipping( currentViewer->isAutoClipping() );
    newVwr->setSeekTime( currentViewer->getSeekTime() );
    // XtFullViewer methods
    newVwr->setDecoration( currentViewer->isDecoration() );

    // The tabBoxManip adds a finish callback to the viewer.
    // We need to remove from the old and add to the new.
    for (int m = 0; m < maniplist->getLength(); m++ ) {
	SoTransformManip *manip = maniplist->getManip(m);	
	if ( manip->isOfType( SoTabBoxManip::getClassTypeId() ) ) {

	    currentViewer->removeFinishCallback( 
		&SoSceneViewer::adjustScaleTabSizeCB, manip->getDragger() );
	    newVwr->addFinishCallback( 
	        &SoSceneViewer::adjustScaleTabSizeCB, manip->getDragger() );
	}
    }
    
#ifdef EXPLORER
    if (userModeFlag)
	newVwr->setEventCallback(userModeCB, userModedata);
    else
	newVwr->setEventCallback(NULL, NULL);
#endif
    
    // build and layout the new viewer
    buildAndLayoutViewer(newVwr);
    
    // finally switch to the new viewer by showing the new viewer, 
    // and hidding the old viewer (hide is done last to reduce flicker).
    newVwr->show();
    currentViewer->hide();
    whichViewer = newViewer;
    currentViewer = newVwr;
    setOverlayLogo(currentViewer);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Move up the picked path to the parent group.
//
// Use: public

void
SoSceneViewer::pickParent()
//
////////////////////////////////////////////////////////////////////////
{
    SoFullPath  *pickPath;
    int	    	parentIndex;

    // We'll pick the parent of the last selection in the list...
    pickPath = (SoFullPath *) (*selection)[selection->getNumSelected() - 1];	
    if(pickPath == NULL || pickPath->getLength() < 2)
	return;

    // Get actual node that is the current selection:
    SoNode *tail     = pickPath->getTail();
    SoNode *kitTail  = ((SoNodeKitPath *)pickPath)->getTail();
    SoType nkt = SoBaseKit::getClassTypeId();
    if ( kitTail->isOfType( nkt ) )
	tail = kitTail;
    else
	kitTail = NULL;

    // If kitTail is at top of path, we've already gone as high as we can go.
    if ( kitTail == pickPath->getHead() )
	return;

    // Get index of parent of selection.
    if ( kitTail != NULL ) {
        // Look for first kit above tail. If none, use direct parent of kitTail.
	SoNode *aboveTail = ((SoNodeKitPath *)pickPath)->getNodeFromTail( 1 );
	SbBool aboveIsKit = aboveTail->isOfType( nkt );
	for (int i = pickPath->getLength() - 1; i >= 0; i-- ) {
	    if ( aboveIsKit ) {
		if (pickPath->getNode(i) == aboveTail ) {
		    parentIndex = i;
		    break;
		}
	    }
	    else if ( pickPath->getNode(i) == kitTail ) {
		parentIndex = i - 1;
		break;
	    }
	}
    }
    else {
	// If tail is not a nodkeit, parentIndex is just parent of tail...
	parentIndex = pickPath->getLength() - 2;
    }
    
    // cannot select the selection node (make sure we're not)
    if (pickPath->getNode(parentIndex) == selection) {
    	fprintf(stderr, "No more parents to pick (cannot pick above the selection node)\n");
    	return;
    }
	
    pickPath->ref();				// need to ref it, because
						// selection->clear unref's it
    selection->deselectAll();
    pickPath->truncate(parentIndex + 1);		// Make path end at parentIndex
    selection->select(pickPath);		// add path back in
    pickPath->unref();				// now we can unref it, again
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Pick all group nodes and shapes under selection.
//
// Use: private
void
SoSceneViewer::pickAll()
//
////////////////////////////////////////////////////////////////////////
{
    selection->deselectAll();

#ifdef DEBUG
    assert(selection != NULL);
#endif

    SoPathList myPaths;

    // Our callbacks on the selection's 'select' method may add 
    // more children to the selection node (by making a trackball or handlebox)
    // Therefore, we must first determine the selections by storing
    // paths to them.
    // Following this, we call 'select' on each path, in turn.

    //
    // Create paths from the selection node to all of it's children
    // that are groups or shapes.
    //
    for (int i = 0; i < selection->getNumChildren(); i++) {
    	SoNode *node = selection->getChild(i);
	if ((node->isOfType(SoGroup::getClassTypeId()) ||
	     node->isOfType(SoShape::getClassTypeId())) )
	{
	    SoPath *thisPath = new SoPath(selection);
	    thisPath->append(i);

	    myPaths.append( thisPath );
	}
    }

    //
    // Select each path in 'myPaths'
    //
    for (int j = 0; j < myPaths.getLength(); j++)
	selection->select(myPaths[j]);
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine first detaches manipulators from all selected objects,
//      then attaches a manipulator to all selected objects.
//
// Use: private

void
SoSceneViewer::replaceAllManips( 
	SvEManipMode manipMode )		// Current manipulator
//
////////////////////////////////////////////////////////////////////////
{
    detachManipFromAll();
    attachManipToAll( manipMode );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine attaches a manipulator to all selected objects.
//
// Use: private

void
SoSceneViewer::attachManipToAll( 
	SvEManipMode manipMode )		// Current manipulator
//
////////////////////////////////////////////////////////////////////////
{
    int	    i;

    for ( i = 0; i < selection->getNumSelected(); i++ ) {
	SoPath *p = (*selection)[i];
	attachManip( manipMode, p );
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine attaches and activates a manipulator.  
//
// Use: private

void
SoSceneViewer::attachManip( 
	SvEManipMode manipMode,		// Current manipulator
	SoPath *selectedPath )	// Which selection to attach to
//
////////////////////////////////////////////////////////////////////////
{
    SoTransformManip 	*theXfManip;
    SoPath		*xfPath;

    //
    // Attach to a manipulator.
    //

    if ( manipMode == SV_NONE )
	return;

    xfPath = findTransformForAttach( selectedPath );
    xfPath->ref();
    theXfManip = NULL;

    switch( manipMode ) {
      case SV_TRANSFORMER:
	theXfManip = new SoTransformerManip;
	break;

      case SV_TRACKBALL:
	theXfManip = new SoTrackballManip;
	break;

      case SV_HANDLEBOX:
	theXfManip = new SoHandleBoxManip;

  	break;

      case SV_JACK:
	theXfManip = new SoJackManip;
  	break;

      case SV_CENTERBALL:
	theXfManip = new SoCenterballManip;
  	break;

      case SV_XFBOX:
	theXfManip = new SoTransformBoxManip;
  	break;

      case SV_TABBOX:
	theXfManip = new SoTabBoxManip;
  	break;

      case SV_NONE:
	return;
    }

    if ( theXfManip ) {

	SoFullPath *fp = (SoFullPath *) xfPath;

#ifdef DEBUG
	if ( !fp->getTail()->isOfType( SoTransform::getClassTypeId() ) ) {
	    fprintf(stderr,"DBG> Fatal Error: in SoSceneViewer::attachManip\n");
	    fprintf(stderr,"   > end of path is not a transform\n");
	}
#endif
	SoTransform *oldXf = (SoTransform *)fp->getTail();
	oldXf->ref();
	theXfManip->ref();

	if ( !theXfManip->replaceNode( xfPath ) ) {
	    theXfManip->unref();
#ifdef DEBUG
	    fprintf(stderr,"DBG> Fatal Error: in SoSceneViewer::attachManip\n");
	    fprintf(stderr,"   > manip->replaceNode() failed!\n" );
#endif
	}

	// If the transformSliderSet is attached to the oldXf, then attach 
	// it to the new manip instead.
	if ( transformSliderSet && transformSliderSet->isVisible()
	     && transformSliderSet->getNode() == oldXf)
		transformSliderSet->setNode(theXfManip);

	// Add manip and paths to the maniplist (maniplist will ref/unref)
	maniplist->append(selectedPath, theXfManip, xfPath );

	theXfManip->unref();
	oldXf->unref();

	if ( manipMode == SV_TABBOX ) {
	    // Special case!  When using a  tab box, we want to adjust the
	    // scale tabs upon viewer finish.
	    currentViewer->addFinishCallback( 
	       &SoSceneViewer::adjustScaleTabSizeCB,theXfManip->getDragger());
	}
	if ( manipMode == SV_JACK ) {
	    // Special case! For jack manip, we want it so that clicking on the
	    // selected object initiates 2-dimensional translation.  Other
	    // parts of the jack manip should use the default resource geometry.
	    // So, we replace the parts that do planar motion. 
	    // We need to replace a total of six parts:
	    //     'translator.yzTranslator.translator':
	    //     'translator.xzTranslator.translator': 
	    //     'translator.xyTranslator.translator': 
	    //     'translator.yzTranslator.translatorActive': 
	    //     'translator.xzTranslator.translatorActive': 
	    //     'translator.xyTranslator.translatorActive': 
	    // In the SoJackDragger, 'translator' is an SoDragPointDragger, 
	    // which takes care of all translations in 3 dimensions for jack.
	    // In the SoDragPointDragger there are 3 planar translation parts 
	    // (each is an SoTranslate2Dragger) and 3 linear translation parts 
	    // (each is an SoTranslate1Dragger).  At any given time, dragPoint
	    // displays one of each kind of dragger. We leave the linear 
	    // translators as the default geometry (a cylinder along the axis 
	    // of motion), but replace the geometry in the planar translators.
	    // Within the SoDragPointDragger, the planar translators are named
	    // 'yzTranslator', 'xzTranslator', and 'xyTranslator'.  
	    // Each of these is an SoTranslate2Dragger, which has two parts
	    // for its geometry, 'translator' and 'translatorActive.' Clicking
	    // on the 'translator' is what initiates the 2D translation.
	    // Once begun, the 'translatorActive' part is displayed. We
	    // replace both of these parts with a path to the selected object.

	    // When we call setPartAsPath, we need to prune the path if our
	    // selected geometry lays inside a nodekit.
	    // For example, let's say a dumbBellKit contains 1 bar (a cylinder)
	    // and 2 end (spheres).  Since this is the lowest-level kit 
	    // containing these 3 shapes, they are considered a single object
	    // by the SceneViewer.
	    // So we need to pass a path to the kit, not the individual piece
	    // that is selected.  This way, subsequent clicks on any piece of 
	    // the dumbbell will cause 2D translation.
		// First, is a nodekit on the path? If so, find the last one.
		SoFullPath *jackP = (SoFullPath *) selectedPath;
		SoType     bkType = SoBaseKit::getClassTypeId();
		int        lastKitInd = -1;
		for (int i = jackP->getLength() - 1; i >= 0; i--) {
		    if (jackP->getNode(i)->isOfType(bkType)) {
			lastKitInd = i;
			break;
		    }
		}
		// If there's a lastKitInd, make jackP be a copy of 
		// selectedPath, but only up to lastKitInd.
		if ( lastKitInd != -1)
		    jackP = (SoFullPath *) selectedPath->copy(0,lastKitInd + 1);

	    // Get the dragger from the manip (the manip contains the dragger,
	    // and the dragger has the parts).
		SoDragger *d = theXfManip->getDragger();

	    // Use jackP to set the translator parts, then discard (unref) it:
		jackP->ref();
		d->setPartAsPath("translator.yzTranslator.translator", jackP );
		d->setPartAsPath("translator.xzTranslator.translator", jackP );
		d->setPartAsPath("translator.xyTranslator.translator", jackP );
		d->setPartAsPath(
			    "translator.yzTranslator.translatorActive", jackP);
		d->setPartAsPath(
			    "translator.xzTranslator.translatorActive", jackP);
		d->setPartAsPath(
			    "translator.xyTranslator.translatorActive", jackP);
		jackP->unref();
	}
    }

    xfPath->unref();
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine detaches the manipulators from all selected objects.
//
// Use: private
void
SoSceneViewer::detachManipFromAll() 
//
////////////////////////////////////////////////////////////////////////
{
    //
    // Loop from the end of the list to the start.
    //
    for (int i = selection->getNumSelected() - 1; i >= 0 ; i-- ) {
	SoPath *p = (SoPath *) (*selection)[i];
	detachManip( p );
    }
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine detaches a manipulator.  
//
// Use: private

void
SoSceneViewer::detachManip(
	SoPath *p )		// Selection object that is being removed
//
////////////////////////////////////////////////////////////////////////
{
    //
    // Detach manip and remove from scene graph.
    //
    int which = maniplist->find(p);
    // See if this path is registered in the manip list.
    if (which != -1) {
	// remove from scene graph
	SoTransformManip *manip = maniplist->getManip(which);

	if ( manip->isOfType(SoTabBoxManip::getClassTypeId() )) {
	    // Special case!  When using a  tab box, we want to adjust the
	    // scale tabs upon viewer finish.
	    currentViewer->removeFinishCallback( 
		&SoSceneViewer::adjustScaleTabSizeCB, manip->getDragger() );
	}

	SoPath *xfPath = maniplist->getXfPath(which);
	SoTransform *newXf = new SoTransform;
	newXf->ref();
	manip->ref();

	// replace the manip
	manip->replaceManip( xfPath, newXf );

	// If the transformSliderSet is attached to the manip, then attach 
	// it to the new node instead.
	if ( transformSliderSet && transformSliderSet->isVisible()
	     && transformSliderSet->getNode() == manip)
		transformSliderSet->setNode(newXf);

	manip->unref();
	newXf->unref();
	
	// remove from maniplist
	maniplist->remove(which);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Added as a finish callback to the current viewer. It makes sure
//      the scale tab size gets changed when a viewer gesture is 
//      completed.
//
// Use: public
void
SoSceneViewer::adjustScaleTabSizeCB( void *userData, SoXtViewer *)
//
////////////////////////////////////////////////////////////////////////
{
    SoTabBoxDragger *dragger = (SoTabBoxDragger *) userData;
    dragger->adjustScaleTabSize();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	See the selection from the camera
//
// Use: public
void
SoSceneViewer::viewSelection()
//
////////////////////////////////////////////////////////////////////////
{
    if (selection->getNumSelected() == 0) {
	viewAll();
	return;
    }
    
    SoPath *path = (*selection)[0];
    if(path != NULL) {
	getCamera()->viewAll(path, currentViewer->getViewportRegion());
    }
    else {
	viewAll();
	return;
    }
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//	Create a color editor for the currently selected object.
//      Attachment code copied from SoXformManip.c++
//
// Use: private
void
SoSceneViewer::createColorEditor()
//
////////////////////////////////////////////////////////////////////////
{
    if (colorEditor == NULL) {
	colorEditor = new MyColorEditor;
	colorEditor->setWYSIWYG(TRUE);
	colorEditor->setTitle("Diffuse Color");
    }

    SoMaterial *editMaterial = findMaterialForAttach( NULL );
    
    colorEditor->attach(&(editMaterial->diffuseColor), 0, editMaterial);

    colorEditor->show();
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//   Find the appropriate material node in the scene graph to attach a material
//   editor to.
//
//   Two possible cases:
//        [1] The path-tail is NOT a group.  We search the siblings of the path
//            tail (including the tail itself) from right to left for a node
//	      that is affected by materials (shapes or groups).
//            We stop the search if we come to a material node to the left of the
//	      pathTail.  If we find a node that IS affected by material, we will
//	      insert a material node just before the path-tail. This is
//            because the editor should not affect nodes that appear
//            before attachPath in the scene graph.
//        [2] The path-tail IS a group.  We search the children from left to
//            right for material nodes.
//            We stop the search if we come to a material node.
//            If we find a node that is affected by materials, we will insert a
//	      material just before this node. This is because the editor for a
//	      group should affect ALL nodes within that group.
//
// NOTE: For the purposes of this routine, we consider SoSwitch as different
//       from other types of group. This is because we don't want to put
//       the new node underneath the switch, but next to it.
//
// Use: private
//
SoMaterial *
SoSceneViewer::findMaterialForAttach(
			const SoPath *target )	// path to start search from
//
////////////////////////////////////////////////////////////////////////
{
    int 		pathLength;
    SoPath 		*selectionPath;
    SoMaterial		*editMtl;

    SbBool		madeNewMtl = FALSE; // did we create a new material
					    // node within this method?


    if ( ( selectionPath = (SoPath *) target ) == NULL ) {
	//
	//  If no selection path is specified, then use the LAST path in the
	//  current selection list.
	//
	selectionPath = (*selection)[selection->getNumSelected() - 1];	// last guy
    }
    pathLength = selectionPath->getLength();

    if ( pathLength <= 0 ) {
	fprintf( stderr, "No objects currently selected...\n" );
	return NULL;
    }

#ifdef DEBUG
    if ( pathLength < 2 ) {
	fprintf( stderr, "Picked object has no parent...\n" );
	return NULL;
    }
#endif


    // find 'group' and try to find 'editMtl'
    SoGroup     *group;
    SoNode      *node;
    int         index, i;
    SbBool	ignoreNodekit = FALSE;

    editMtl = NULL;

    if ( selectionPath->getTail()->isOfType( SoBaseKit::getClassTypeId() )) {
	// Nodekits have their own built in policy for creating new material
	// nodes. Allow them to contruct and return it.
	// Get the last nodekit in the path:
	SoBaseKit *kit = (SoBaseKit *) 
			 ((SoNodeKitPath *)selectionPath)->getTail();
	// SO_CHECK_PART returns NULL if the part doesn't exist yet...
	editMtl = SO_GET_PART( kit, "material", SoMaterial );
	if ( editMtl == NULL ) {
	    // This nodekit does not have a material part.
	    // Ignore the fact that this is a nodekit.
	    ignoreNodekit = TRUE;
	}
    }

    SbBool isTailGroup 	= 
      selectionPath->getTail()->isOfType( SoGroup::getClassTypeId()) &&
      (!selectionPath->getTail()->isOfType( SoSwitch::getClassTypeId()));
    
    if ((editMtl == NULL) && ( !isTailGroup )) {
	//
        //    CASE 1: The path-tail is not a group.
        //    'group'      becomes the second to last node in the path.
        //    We search the path tail and its siblings from right to left for a
        //    mtl node.
        //    We stop the search if we come to a shape node or a group node
        //    to the left of the pathTail.  If we find a shape or group, we
        //    will insert a mtl just before the path-tail. This is
        //    because the manipulator should not affect objects that appear
        //    before selectionPath in the scene graph.
	//
        group      = (SoGroup *) selectionPath->getNode(pathLength - 2);
        index      = group->findChild( selectionPath->getTail() );

        for (i = index; (i >= 0) && (editMtl == NULL); i--){
            node = group->getChild(i);
            if (node->isOfType(SoMaterial::getClassTypeId())) // found SoMaterial
                editMtl = (SoMaterial *) node;
            else if ( i != index ) { 
                if ( isAffectedByMaterial( node ) )
                    break;
            }
        }

        if ( editMtl == NULL ) {
            editMtl = new SoMaterial;
            group->insertChild( editMtl, index );
	    madeNewMtl = TRUE;
        }
    }
    else if (editMtl == NULL) {
        //    CASE 2: The path-tail is a group.
        //    'group'      becomes the path tail
        //      We search the children from left to right for mtl nodes.
        //      We stop the search if we come to a shape node or a group node.
        //      If we find a shape or group, we will insert a mtl just
        //      before this shape or group. This is because the editor
        //      for a group should affect ALL objects within that group.
	//
        group = (SoGroup *) selectionPath->getTail();
        for (i = 0; (i < group->getNumChildren()) && (editMtl == NULL); i++ ) {
            node = group->getChild(i);
            if (node->isOfType(SoMaterial::getClassTypeId()))
                editMtl = (SoMaterial *) node;
            else if ( isAffectedByMaterial( node ) )
                break;
        }

        if ( editMtl == NULL ) {
            editMtl = new SoMaterial;
            group->insertChild( editMtl, i );
	    madeNewMtl = TRUE;
        }
    }

    // If we just created the material node here, then set the ignore
    // flags for all fields in the node.  This will cause the fields
    // to be inherited from their ancestors. The material editor will
    // undo these flags whenever it changes the value of a field
    if ( madeNewMtl == TRUE ) {
	editMtl->ambientColor.setIgnored( TRUE );
	editMtl->diffuseColor.setIgnored( TRUE );
	editMtl->specularColor.setIgnored( TRUE );
	editMtl->emissiveColor.setIgnored( TRUE );
	editMtl->shininess.setIgnored( TRUE );
	editMtl->transparency.setIgnored( TRUE );
    }

    // If any of the fields is ignored, then fill the value with the value
    // inherited from the rest of the scene graph
    if ( editMtl->ambientColor.isIgnored() 
	|| editMtl->diffuseColor.isIgnored() 
	|| editMtl->specularColor.isIgnored() 
	|| editMtl->emissiveColor.isIgnored() 
	|| editMtl->shininess.isIgnored() 
	|| editMtl->transparency.isIgnored() ){

	// Create a path to the material
	SoPath *mtlPath;
	if ( (! ignoreNodekit) && selectionPath->getTail()->isOfType( SoBaseKit::getClassTypeId() )) {
	    SoBaseKit *kit = (SoBaseKit *) 
			     ((SoNodeKitPath *)selectionPath)->getTail();
	    mtlPath = kit->createPathToPart( "material", TRUE, selectionPath );
	    mtlPath->ref();
	}
	else {
	    if ( !isTailGroup ) {
		// CASE 1: path-tail was NOT 'group' -- copy all but last entry
		mtlPath = selectionPath->copy(0, pathLength - 1);
	    }
	    else {
		// CASE 2: path-tail was 'group' -- copy all of editPath
		mtlPath = selectionPath->copy(0, pathLength);
	    }
	    mtlPath->ref();
	    // add the material to the end of the path
	    int mtlIndex    = group->findChild(editMtl);
	    mtlPath->append( mtlIndex );
	}

	// Pass the material node to an accumulate state callback
	// that will load any 'ignored' values with their inherited values.
	SoCallbackAction cba;
	cba.addPreTailCallback( SoSceneViewer::findMtlPreTailCB, editMtl);
	cba.apply( mtlPath );

	mtlPath->unref();
    }


    return( editMtl );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Callback used by 'findMaterialForAttach' as part of the accumulate state
//   action. Returns 'PRUNE', which tells the action not to draw the
//   shape as part of the accum state action.
//   editor to.
//
// Use: private
//
SoCallbackAction::Response
SoSceneViewer::findMtlPreTailCB( void *data, SoCallbackAction *accum, 
				 const SoNode * )
//
////////////////////////////////////////////////////////////////////////
{
    SoMaterial *mtl = (SoMaterial *) data;

    SbColor ambient, diffuse, specular, emissive;
    float   shininess, transparency;

    accum->getMaterial( ambient, diffuse, specular, emissive, 
			shininess, transparency ); 

    // inherit the accumulated values only in those fields being ignored.
    if ( mtl->ambientColor.isIgnored() )
	 mtl->ambientColor.setValue( ambient );
    if ( mtl->diffuseColor.isIgnored() )
	 mtl->diffuseColor.setValue( diffuse );
    if ( mtl->specularColor.isIgnored() )
	 mtl->specularColor.setValue( specular );
    if ( mtl->emissiveColor.isIgnored() )
	 mtl->emissiveColor.setValue( emissive );
    if ( mtl->shininess.isIgnored() )
	 mtl->shininess.setValue( shininess );
    if ( mtl->transparency.isIgnored() )
	 mtl->transparency.setValue( transparency );

    return SoCallbackAction::ABORT;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Find the appropriate transform node in the scene graph for attaching a 
//   transform editor or manipulator.
//
//   How we treat the 'center' field of the transform node:
//   If we need to create a new transform node:
//       set the 'center' to be the geometric center of all objects 
//       affected by that transform. 
//   If we find a transform node that already exists:
//       'center' will not be changed.
//
//   Three possible cases:
//        [1] The path-tail is a node kit. Just ask the node kit for a path
//            to the part called "transform"
//        [2] The path-tail is NOT a group.  We search the siblings of the path
//            tail (including the tail itself) from right to left for a node
//	      that is affected by transforms (shapes, groups, lights,cameras).
//            We stop the search if we come to a transform node to the left of 
//	      the pathTail.  If we find a node that IS affected by transform, 
//	      we will insert a transform node just before the path-tail. This is
//            because the editor should not affect nodes that appear
//            before attachPath in the scene graph.
//        [3] The path-tail IS a group.  We search the children from left to
//            right for transform nodes.
//            We stop the search if we come to a transform node.
//            If we find a node that is affected by transform, we will insert a
//	      transform just before this node. This is because the editor for a
//	      group should affect ALL nodes within that group.
//
// NOTE: For the purposes of this routine, we consider SoSwitch as different
//       from other types of group. This is because we don't want to put
//       the new node underneath the switch, but next to it.
//
// Use: private
//
SoPath *
SoSceneViewer::findTransformForAttach(
			const SoPath *target )	// path to start search from
//
////////////////////////////////////////////////////////////////////////
{
    int 		pathLength;
    SoPath 		*selectionPath;
    SoTransform		*editXform;

    
    if ( ( selectionPath = (SoPath *) target ) == NULL ) {
	//
	//  If no selection path is specified, then use the LAST path in the
	//  current selection list.
	//
	selectionPath = (*selection)[selection->getNumSelected() - 1];
    }
    pathLength = selectionPath->getLength();

    if ( pathLength <= 0 ) {
	fprintf( stderr, "No objects currently selected...\n" );
	return NULL;
    }

#ifdef DEBUG
    if ( pathLength < 2 ) {
	fprintf( stderr, "Picked object has no parent...\n" );
	return NULL;
    }
#endif

    // find 'group' and try to find 'editXform'
    SoGroup     *group;
    SoNode      *node;
    int         index, i;
    SbBool      isTailGroup,  isTailKit;
    SbBool      existedBefore = FALSE;
    SoPath      *pathToXform = NULL;

    editXform = NULL;

    isTailGroup =
	    (   selectionPath->getTail()->isOfType(SoGroup::getClassTypeId() )
	    && !selectionPath->getTail()->isOfType(SoSwitch::getClassTypeId()));

    isTailKit = selectionPath->getTail()->isOfType(SoBaseKit::getClassTypeId());

    //    CASE 1: The path-tail is a node kit.
    if ( isTailKit ) {

	// Nodekits have their own built in policy for creating new transform
	// nodes. Allow them to contruct and return a path to it.
	SoBaseKit *kit = (SoBaseKit *) 
			 ((SoNodeKitPath *)selectionPath)->getTail();

	// Before creating path, see if the transform part exists yet:
	if (SO_CHECK_PART(kit, "transform", SoTransform) != NULL)
	    existedBefore = TRUE;

	if ((editXform = SO_GET_PART(kit, "transform", SoTransform)) != NULL) {
	    pathToXform = kit->createPathToPart( "transform", TRUE, selectionPath );
	    pathToXform->ref();
	}
	else {
	    // This nodekit has no transform part. 
	    // Treat the object as if it were not a nodekit.
	    isTailKit = FALSE;
	}
    }
    
    if ( !isTailGroup && !isTailKit ) {
	//
        //    CASE 2: The path-tail is not a group.
        //    'group'      becomes the second to last node in the path.
        //    We search the path tail and its siblings from right to left for a
        //    transform node.
        //    We stop the search if we come to a 'movable' node
        //    to the left of the pathTail.  If we find a movable node, we
        //    will insert a transform just before the path-tail. This is
        //    because the manipulator should not affect objects that appear
        //    before selectionPath in the scene graph.
	//
        group      = (SoGroup *) selectionPath->getNode(pathLength - 2);
        index      = group->findChild( selectionPath->getTail() );

        for (i = index; (i >= 0) && (editXform == NULL); i--){
            node = group->getChild(i);
            if (node->isOfType(SoTransform::getClassTypeId()))  // found an SoMaterial
                editXform = (SoTransform *) node;
            else if ( i != index ) { 
		if ( isAffectedByTransform( node ) )
		    break;
            }
        }

        if ( editXform == NULL ) {
	    existedBefore = FALSE;
            editXform = new SoTransform;
            group->insertChild( editXform, index );
        }
	else
	    existedBefore = TRUE;
    }
    else if ( !isTailKit ) {
        //    CASE 3: The path-tail is a group.
        //    'group'      becomes the path tail
        //      We search the children from left to right for transform nodes.
        //      We stop the search if we come to a movable node.
        //      If we find a movable node, we will insert a transform just
        //      before this node. This is because the editor
        //      for a group should affect ALL objects within that group.
	//
        group = (SoGroup *) selectionPath->getTail();
        for (i = 0; (i < group->getNumChildren()) && (editXform == NULL); i++ ) {
            node = group->getChild(i);
            if (node->isOfType(SoTransform::getClassTypeId()))
                editXform = (SoTransform *) node;
            else if ( isAffectedByTransform( node ) )
                break;
        }

        if ( editXform == NULL ) {
	    existedBefore = FALSE;
            editXform = new SoTransform;
            group->insertChild( editXform, i );
        }
	else
	    existedBefore = TRUE;
    }

    // If we don't have a path yet (i.e., we weren't handed a nodekit path)
    // create the 'pathToXform'
    // by copying editPath and making the last node in the path be editXform
    if ( pathToXform == NULL ) {
	if ( !isTailGroup )
	    // CASE 2: path-tail was NOT 'group' -- copy all but last entry
	    pathToXform = selectionPath->copy(0, pathLength - 1);
	else
	    // CASE 3: path-tail was 'group' -- copy all of editPath
	    pathToXform = selectionPath->copy(0, pathLength);
	pathToXform->ref();

	// add the transform to the end
	int xfIndex    = group->findChild(editXform);
	pathToXform->append( xfIndex );
    }


    // Now. If we created the transform node right here, right now, then
    // we will set the 'center' field based on the geometric center. We 
    // don't do this if we didn't create the transform, because "maybe it
    // was that way for a reason."
    if ( existedBefore == FALSE ) {
	// First, find 'applyPath' by popping nodes off the path until you 
	// reach a separator. This path will contain all nodes affected by
	// the transform at the end of 'pathToXform'
	SoFullPath *applyPath = (SoFullPath *) pathToXform->copy();
	applyPath->ref();
	for (int i = (applyPath->getLength() - 1); i >0; i-- ) {
	    if (applyPath->getNode(i)->isOfType( SoSeparator::getClassTypeId()))
		break;
	    applyPath->pop();
	}

	// Next, apply a bounding box action to applyPath, and reset the
	// bounding box just before the tail of 'pathToXform' (which is just
	// the editXform). This will assure that the only things included in 
	// the resulting bbox will be those affected by the editXform.
	SoGetBoundingBoxAction bboxAction(currentViewer->getViewportRegion());
	bboxAction.setResetPath(pathToXform,TRUE,SoGetBoundingBoxAction::BBOX );
	bboxAction.apply(applyPath);

	applyPath->unref();

	// Get the center of the bbox in world space...
	SbVec3f worldBoxCenter = bboxAction.getBoundingBox().getCenter();

	// Convert it into local space of the transform...
	SbVec3f localBoxCenter;
	SoGetMatrixAction ma(currentViewer->getViewportRegion());
	ma.apply( pathToXform );
	ma.getInverse().multVecMatrix( worldBoxCenter, localBoxCenter );

	// Finally, set the center value...
	editXform->center.setValue( localBoxCenter );
    }

    pathToXform->unrefNoDelete();
    return( pathToXform );
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//	Create a material editor for the currently selected object.
//
// Use: private
void
SoSceneViewer::createMaterialEditor()
//
////////////////////////////////////////////////////////////////////////
{
    if (materialEditor == NULL)
    	materialEditor = new SoXtMaterialEditor;
    materialEditor->show();
    
    materialEditor->attach( findMaterialForAttach( NULL ) );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Create a transform editor for the currently selected object
//
// Use: private
void
SoSceneViewer::createTransformSliderSet()
//
////////////////////////////////////////////////////////////////////////
{
    SoPath      *editTransformPath;
    SoTransform *editTransform;

    // get path to a transform to edit
    if ( ( editTransformPath = findTransformForAttach( NULL )) == NULL )
	return;

    // the tail of the path is a transform for us!
    editTransformPath->ref();
    editTransform =(SoTransform *) ((SoFullPath *)editTransformPath)->getTail();
    editTransformPath->unref();
	
    // Nuke the old slider set and get a new one
    if (transformSliderSet == NULL)
	transformSliderSet = new SoXtTransformSliderSet();
    transformSliderSet->setNode(editTransform);
    transformSliderSet->show();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//      Set fog on/off. Leave density alone - when default, the environment
// node will do something useful to make the fog look good.
//
// Use: private
void
SoSceneViewer::setFog(SbBool flag)
//
////////////////////////////////////////////////////////////////////////
{
    fogFlag = flag;
    
    if (fogFlag)
         environment->fogType.setValue( SoEnvironment::HAZE ); // purple ?
    else environment->fogType.setValue( SoEnvironment::NONE );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//      Set AA-ing on/off.
//
// Use: private
void
SoSceneViewer::setAntialiasing(SbBool flag)
//
////////////////////////////////////////////////////////////////////////
{
    antialiasingFlag = flag;
    
    if (antialiasingFlag)
	currentViewer->setAntialiasing( TRUE, 3 );
    else
	currentViewer->setAntialiasing( FALSE, 1 );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//      Invokes color editor on ambient lighting color.
void
SoSceneViewer::editAmbientColor()
//
////////////////////////////////////////////////////////////////////////
{ 
    if ( ambientColorEditor == NULL ) {
    	ambientColorEditor = new MyColorEditor;
	ambientColorEditor->setTitle( "Ambient Lighting" );
	ambientColorEditor->addColorChangedCallback(
	    SoSceneViewer::ambientColorCallback, this );
    }
    
    // Normalize ambient intensity
    SbColor ambCol;
    ambCol = environment->ambientColor.getValue();
    ambCol *= environment->ambientIntensity.getValue();
    environment->ambientIntensity.setValue( 1.0 );
    environment->ambientColor.setValue( ambCol );
    
    ignoreCallback = TRUE;
    ambientColorEditor->setColor( environment->ambientColor.getValue() );
    ignoreCallback = FALSE;
    ambientColorEditor->show();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  Callback proc invoked by the color editor, this changes the scene's
//  ambient lighting color.
//
//  Use: static, private
//
void
SoSceneViewer::ambientColorCallback(void *userData, const SbColor *color)
//
////////////////////////////////////////////////////////////////////////
{
    SoSceneViewer *sv = (SoSceneViewer *) userData;
    
    if (sv->ignoreCallback)
    	return;
    
    sv->environment->ambientColor.setValue( *color );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//      Invokes color editor on background color.
void
SoSceneViewer::editBackgroundColor()
//
////////////////////////////////////////////////////////////////////////
{ 
    if ( backgroundColorEditor == NULL ) {
    	backgroundColorEditor = new MyColorEditor;
	backgroundColorEditor->setTitle( "Background Color" );
	backgroundColorEditor->addColorChangedCallback(
	    SoSceneViewer::backgroundColorCallback, this );
    }
    ignoreCallback = TRUE;
    backgroundColorEditor->setColor(getBackgroundColor());
    ignoreCallback = FALSE;
    backgroundColorEditor->show();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  Callback proc invoked by the color editor, this changes the current
//  viewer's background color.
//
//  Use: static, private
//
void
SoSceneViewer::backgroundColorCallback(void *userData, const SbColor *c)
//
////////////////////////////////////////////////////////////////////////
{
//??? should be using the Roxy color editor, not the So color editor

    SoSceneViewer *sv = (SoSceneViewer *) userData;
    
    if (sv->ignoreCallback)
    	return;
    
    sv->currentViewer->setBackgroundColor( *c );
    
    // keep fog color up to date with bkg color
    sv->environment->fogColor.setValue( *c );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	This will:
//        [a] find the last sceneKit camera that is being looked through
//            in the scene and return  a pointer to it.
//
// Use: private
SoCamera *
SoSceneViewer::getLastSceneKitCamera(SoGroup *root)
//
////////////////////////////////////////////////////////////////////////
{
    // Find scene kits under the root node and loop backwards:
    SoSearchAction sa;
    sa.setType(SoSceneKit::getClassTypeId());
    sa.setInterest(SoSearchAction::ALL);
    sa.apply(root);

    SoPathList paths = sa.getPaths();
    for (int i = paths.getLength() - 1; i >= 0; i--) {
	SoPath *p = paths[i];
	if (p->getTail()->isOfType( SoSceneKit::getClassTypeId() ) ) {
	    SoSceneKit *sceneKit = (SoSceneKit *) p->getTail();
	    int camNum = sceneKit->getCameraNumber();

	    SoNodeKitListPart *camList = 
		(SoNodeKitListPart *) sceneKit->getPart("cameraList",FALSE);
	    if (camList && camNum >= 0) {
	        SoCameraKit *camKit 
		    = (SoCameraKit *)camList->getChild( camNum );
		if (camKit)
		    return( (SoCamera *) camKit->getPart( "camera", FALSE ));
	    }
	}
    }
    return NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	This will:
//        [a] find the last sceneKit, its camera number, and replace the
//            given camera for that node.
//
// Use: private
void
SoSceneViewer::setLastSceneKitCamera(SoGroup *root, SoCamera *theCam)
//
////////////////////////////////////////////////////////////////////////
{
    // Find scene kits under the root node and loop backwards:
    SoSearchAction sa;
    sa.setType(SoSceneKit::getClassTypeId());
    sa.setInterest(SoSearchAction::ALL);
    sa.apply(root);

    SoPathList paths = sa.getPaths();
    for (int i = paths.getLength() - 1; i >= 0; i--) {
	SoPath *p = paths[i];
	if (p->getTail()->isOfType( SoSceneKit::getClassTypeId() ) ) {
	    SoSceneKit *sceneKit = (SoSceneKit *) p->getTail();
	    int camNum = sceneKit->getCameraNumber();

	    SoNodeKitListPart *camList = 
		(SoNodeKitListPart *) sceneKit->getPart("cameraList",FALSE);
	    if (camList && camNum >= 0) {
	        SoCameraKit *camKit 
		    = (SoCameraKit *)camList->getChild( camNum );
		if (camKit) {
		    camKit->setPart("camera",theCam);
		    return;
		}
	    }
	}
    }
    return;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	This will:
//        [a] find scene kits if under the root node.
//        [b] Save the camera number settings of the kits in a dictionary.
//        [c] Switch off the scenekit cameras.
//
// Use: private
void
SoSceneViewer::switchOffSceneKitCameras(SoGroup *root)
//
////////////////////////////////////////////////////////////////////////
{
    // [a] find scene kits if under the root node.
    SoSearchAction sa;
    sa.setType(SoSceneKit::getClassTypeId());
    sa.setInterest(SoSearchAction::ALL);
    sa.apply(root);
    
    // Deal with each kit:
    SoPathList paths = sa.getPaths();
    for (int i = paths.getLength() - 1; i >= 0; i--) {
	SoPath *p = paths[i];
	if (p->getTail()->isOfType( SoSceneKit::getClassTypeId() ) ) {
	    SoSceneKit *sceneKit = (SoSceneKit *) p->getTail();
	    int *camNumPtr = new int;
	    *camNumPtr = sceneKit->getCameraNumber();

	    // [b] Save the camera number settings of the kits in a dictionary.
	    sceneKitCamNumDict->enter((unsigned long)sceneKit,
				      (void *)camNumPtr);

	    // [c] Switch off the scenekit cameras.
	    sceneKit->setCameraNumber( SO_SWITCH_NONE );
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	This will:
//        [a] find scene kits if under the root node.
//        [b] Find the camera number settings of the kits from a dictionary.
//        [c] Restore the sceneKit camera numbers.
//
// Use: private
void
SoSceneViewer::restoreSceneKitCameras(SoGroup *root)
//
////////////////////////////////////////////////////////////////////////
{
    // [a] find scene kits if under the root node.
    SoSearchAction sa;
    sa.setType(SoSceneKit::getClassTypeId());
    sa.setInterest(SoSearchAction::ALL);
    sa.apply(root);
    
    // Deal with each kit:
    SoPathList paths = sa.getPaths();
    for (int i = paths.getLength() - 1; i >= 0; i--) {
	SoPath *p = paths[i];
	if (p->getTail()->isOfType( SoSceneKit::getClassTypeId() ) ) {
	    SoSceneKit *sceneKit = (SoSceneKit *) p->getTail();
	    void *myPtr;
	    int camNum;
	    if (sceneKitCamNumDict->find((unsigned long)sceneKit, myPtr))
		camNum = *((int *)myPtr);
	    else
		camNum = SO_SWITCH_NONE;

	    sceneKit->setCameraNumber( camNum );
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
// Copies from given camera into this viewer's camera, if types match.
//
// Use: private
void        
SoSceneViewer::setCameraFields( SoCamera *fromCam )
//
////////////////////////////////////////////////////////////////////////
{
    SoCamera *myCam = getCamera();

    if ( ! myCam || ! fromCam )
	return;

    // Types must match!
    if (  myCam->getTypeId() != fromCam->getTypeId() )
	return;

    myCam->viewportMapping = fromCam->viewportMapping.getValue();
    myCam->position = fromCam->position.getValue();
    myCam->orientation = fromCam->orientation.getValue();
    myCam->aspectRatio = fromCam->aspectRatio.getValue();
    myCam->nearDistance = fromCam->nearDistance.getValue();
    myCam->farDistance = fromCam->farDistance.getValue();
    myCam->focalDistance = fromCam->focalDistance.getValue();

    if (   myCam->isOfType( SoPerspectiveCamera::getClassTypeId() )) {
	((SoPerspectiveCamera *)myCam)->heightAngle 
	    = ((SoPerspectiveCamera *)fromCam)->heightAngle.getValue();
    }
    else if (   myCam->isOfType( SoOrthographicCamera::getClassTypeId() )) {
	((SoOrthographicCamera *)myCam)->height 
	    = ((SoOrthographicCamera *)fromCam)->height.getValue();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	This will remove any cameras under root.
//
// Use: private
void
SoSceneViewer::removeCameras(SoGroup *root)
//
////////////////////////////////////////////////////////////////////////
{
    SoSearchAction sa;
    sa.setType(SoCamera::getClassTypeId());
    sa.setInterest(SoSearchAction::ALL);
    sa.apply(root);
    
    // remove those cameras!
    SoPathList paths = sa.getPaths();
    for (int i = 0; i < paths.getLength(); i++) {
	SoPath *p = paths[i];
	int cam = p->getIndexFromTail(0);
	SoGroup  *group = (SoGroup *) p->getNodeFromTail(1);
	group->removeChild(cam);
	
	// See if the parent of this camera is a switch node with whichChild set
	// to render the camera. With the camera gone, the switch has nothing
	// to render.
	if (group->isOfType(SoSwitch::getClassTypeId())) {
	    SoSwitch *sw = (SoSwitch *) group;
	    if (sw->whichChild.getValue() == cam)
		sw->whichChild.setValue(SO_SWITCH_NONE);
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Reads the given file and insert the geometry under the selection
//  node. If the node didn't have any children, the viewAll() method is
//  automatically called.
//
// Use: private
SbBool
SoSceneViewer::readFile(const char *filename)
//
////////////////////////////////////////////////////////////////////////
{    
    SbBool hadNoChildren = (selection->getNumChildren() == 0);
    
    // Try reading the file
    SbString errorMessage;
    SoSeparator *sep = MyFileRead(filename, errorMessage);

    // add nodes under selection, not sceneGraph
    if (sep != NULL)
	selection->addChild(sep);
    
    // display error dialog if there were reading errors
    else {
	SoXt::createSimpleErrorDialog(mgrWidget, "File Error Dialog", 
		    (char *) errorMessage.getString());
	return FALSE;
    }
    
    // remove any cameras under selection which were just added
    SoCamera *camFromScene = getLastSceneKitCamera(selection);
    switchOffSceneKitCameras(selection);
    removeCameras(selection);
    
    if (hadNoChildren) {
	viewAll();
	saveHomePosition();
    }

    // If scene had sceneKit with a camera, copy its values into our camera.
    if (camFromScene)
	setCameraFields( camFromScene );
    
    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Read environment data. We expect the following nodes:
//
//  Group {
//    Label { "SoSceneViewer Environment v3.0" }
//    Camera {}
//    Environment {}
//    LightGroup {
//      Switch { DirectionalLight }  # 1
//  	...
//      Switch { DirectionalLight }  # 6
//    }
//    DirectionalLight {}   	# optional headlight
//  }
//
// Use: private
SbBool
SoSceneViewer::readEnvFile(const char *filename)
//
////////////////////////////////////////////////////////////////////////
{
    SoInput in;
    if (! in.openFile(filename)) {
	// display an error dialog
	char str[100];
	strcpy(str, "Error opening file: ");
	strcat(str, filename);
	SoXt::createSimpleErrorDialog(mgrWidget, "File Error Dialog", str);
	return FALSE;
    }
    
    SoNode *n;
    SoLabel *l = NULL;
    SbBool isValid = FALSE;
    SbBool ok;
    
    if ((ok = SoDB::read(&in, n)) && n != NULL) {
	// we expect a label first
	n->ref();
	if (n->isOfType(SoLabel::getClassTypeId())) {
	    l = (SoLabel *) n;
	    isValid = (strcmp(l->label.getValue().getString(),
			      SV_ENV_LABEL) == 0);
	}
	n->unref();
    }
    else if (!ok) {
	// display error dialog if there were reading errors
	char str[100];
	strcpy(str, "Error reading file: ");
	strcat(str, filename);
	SoXt::createSimpleErrorDialog(mgrWidget, "File Error Dialog", str);
	return FALSE;
    }

    // if ok, read the rest.
    if (isValid) {
	// Camera
	if (SoDB::read(&in, n) != FALSE && (n != NULL)) {
	    n->ref();
	    if (n->isOfType(SoCamera::getClassTypeId())) {
		// replace the old camera with the new camera and
		// re-attach the viewer.
		SoCamera *newCamera = (SoCamera *) n;
		SoCamera *oldCamera = getCamera();
		SoSearchAction sa;
		sa.setNode(oldCamera);
		sa.apply(sceneGraph);
		SoFullPath *fullCamPath = (SoFullPath *) sa.getPath();
		if (fullCamPath) {
		    SoGroup *parent = 
			(SoGroup *)fullCamPath->getNode(fullCamPath->getLength() - 2);
		    parent->insertChild(newCamera, parent->findChild(oldCamera));
		    setCamera(newCamera);
		    if (parent->findChild(oldCamera) >= 0)
			parent->removeChild(oldCamera);
		}
#if DEBUG
		else
		    SoDebugError::post("SoSceneViewer::readEnvFile",
			    "cannot find camera in graph");
#endif
	    }
	    n->unref();
	}
	// Environment
	if (SoDB::read(&in, n) != FALSE && (n != NULL)) {
	    n->ref();
	    if (n->isOfType(SoEnvironment::getClassTypeId())) {
		lightsCameraEnvironment->replaceChild(environment, n);
		environment = (SoEnvironment *) n;
	    }
	    n->unref();
	}
	// Light group
	if (SoDB::read(&in, n) != FALSE && (n != NULL)) {
	    n->ref();
	    if (n->isOfType(SoGroup::getClassTypeId())) {
		
		// remove all of the existing lights
		int i;
		for (i = lightDataList.getLength(); i > 0; i--)
		    removeLight( (SvLightData *) lightDataList[i-1] );
		
		lightsCameraEnvironment->replaceChild(lightGroup, n);
		lightGroup = (SoGroup *) n;
		
		// This was busted. It was looking for a light as child 0,
		// but the scale and scaleInverse made it think no light was 
		// there. So now, we do this right...
		// We'll just check for the light as any old child.
		// This way it's okay to add a translation node under that 
		// switch too, so we can translate the manips as well.
		// This allows as to place the directional light manips.
		for (i=0; i < lightGroup->getNumChildren(); i++) {
		    SoNode *node = lightGroup->getChild(i);
		    if (node->isOfType(SoSwitch::getClassTypeId())) {
			SoSwitch *sw = (SoSwitch *) node;
			SbBool addedIt = FALSE;
			for (int j = 0; 
			     addedIt == FALSE && j < sw->getNumChildren(); 
			     j++ ) {
			    node = sw->getChild(j);
			    if (node->isOfType(SoLight::getClassTypeId())) {
				addLightEntry((SoLight *)node, sw);
				addedIt = TRUE;
			    }
			}
		    }
		}
	    }
	    n->unref();
	}
	// Headlight (optional) - if not there, turn headlight off
	if (SoDB::read(&in, n) != FALSE && (n != NULL)) {
	    n->ref();
	    if (n->isOfType(SoDirectionalLight::getClassTypeId())) {
		SoDirectionalLight *headlight = getHeadlight();
		SoDirectionalLight *newLight = (SoDirectionalLight *) n;
		if (headlight != NULL) {
		    headlight->intensity.setValue(newLight->intensity.getValue());
		    headlight->color.setValue(newLight->color.getValue());
		    headlight->direction.setValue(newLight->direction.getValue());
		    setHeadlight(TRUE);
		}
	    }
	    n->unref();
	}
	else setHeadlight(FALSE);
    }
    else {
	fprintf(stderr, "Sorry, file is not formatted correctly\n");
    }
    
    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine is called to get a file name using the
// standard file dialog.
//
// Use: private
void
SoSceneViewer::getFileName()
//
////////////////////////////////////////////////////////////////////////
{
    // use a motif file selection dialog
    if (fileDialog == NULL) {
	Arg args[5];
	int n = 0;
	
	// unmanage when ok/cancel are pressed
	XtSetArg(args[n], XmNautoUnmanage, TRUE); n++;
	fileDialog = XmCreateFileSelectionDialog(
	    XtParent(mgrWidget), "File Dialog", args, n);
	
	XtAddCallback(fileDialog, XmNokCallback,
		      (XtCallbackProc)SoSceneViewer::fileDialogCB,
		      (XtPointer)this);
    }
    
    // manage the dialog
    XtManageChild(fileDialog);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Motif file dialog callback.
//
void
SoSceneViewer::fileDialogCB(Widget, SoSceneViewer *sv,
		      XmFileSelectionBoxCallbackStruct *data)
//
////////////////////////////////////////////////////////////////////////
{
    // Get the file name
    char *filename;
    XmStringGetLtoR(data->value,
	(XmStringCharSet) XmSTRING_DEFAULT_CHARSET, &filename);
    
    // Use that file
    sv->doFileIO(filename);
    
    XtFree(filename);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	detach everything and nuke the existing scene.
//
// Use: private
void
SoSceneViewer::deleteScene()
//
////////////////////////////////////////////////////////////////////////
{
    // deselect everything (also detach manips)
    selection->deselectAll();
    
    // remove the geometry under the selection node
    for (int i = selection->getNumChildren(); i>0; i--)
	selection->removeChild(i-1);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Read/Write to the given file name, given the current file mode.
//
// Use: private
SbBool
SoSceneViewer::doFileIO(const char *file)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool okFile = TRUE;
    
    switch (fileMode) {
	case SV_FILE_OPEN:
	    deleteScene();
	    okFile = readFile(file);
	    // If we've already made lights, re-calculate size for light 
	    // manips based on size of new scene.
	    if (calculatedLightManipSize)
		calculateLightManipSize();
	    break;
	case SV_FILE_IMPORT:
	    okFile = readFile(file);
	    // If we've already made lights, re-calculate size for light 
	    // manips based on size of new scene.
	    if (calculatedLightManipSize)
		calculateLightManipSize();
	    break;
	case SV_FILE_SAVE_AS:
	    okFile = writeFile(file);
	    break;
	case SV_FILE_READ_ENV:
	    readEnvFile(file);
	    break;
	case SV_FILE_SAVE_ENV:
	    {
	    
	    // Run through the lights. If any light is a directionalLightManip,
	    // transfer its translation into the SvLightData, since that
	    // info will not write out.
	    for (int i=0; i < lightDataList.getLength(); i++)
		transferDirectionalLightLocation((SvLightData *) lightDataList[i]);

	    writeEnvFile(file);
	    }

	    break;
	default:
	    fprintf(stderr, "Wrong file mode %d passed!\n", fileMode);
	    return FALSE;
    }
    
    // save the new file name so we can simply use "Save" instead of
    // "Save As" next time around.
    if (fileMode == SV_FILE_OPEN || fileMode == SV_FILE_SAVE_AS) {
	
	// save the current file name
	delete fileName;
	if (okFile && file != NULL)
	    fileName = strdup(file);
	else
	    fileName = NULL;
    }
    
    // enable/disable cmd key shortcuts and menu items
    updateCommandAvailability();
    
    return okFile;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Saves the scene to the current file.
//
// Use: private
void
SoSceneViewer::save()
//
////////////////////////////////////////////////////////////////////////
{
    if (fileName != NULL) {
	SbBool ok = writeFile(fileName);
	if (!ok) {
	    delete fileName;
	    fileName = NULL;
	}
    }
    else {
	fileMode = SV_FILE_SAVE_AS;
	getFileName();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Removes the attached light manips geometry from the scene. This
//  is used for file writting,...
//
// Use: private
void
SoSceneViewer::removeAttachedLightManipGeometry()
//
////////////////////////////////////////////////////////////////////////
{
    for (int i = 0; i < lightDataList.getLength(); i++ ) {
	
	SvLightData *data = (SvLightData *) lightDataList[i];

	// We'll be putting everything back later, so make a note of this...
	data->shouldBeManip = data->isManip();

	if ( data->isManip() == TRUE )
	    editLight(data, FALSE);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Add the attached light manips geometry back into the scene. This
// is called after the geometry has been temporaly revomed (used for file
// writting).
//
// Use: private
void
SoSceneViewer::addAttachedLightManipGeometry()
//
////////////////////////////////////////////////////////////////////////
{
    for (int i = 0; i < lightDataList.getLength(); i++ ) {
	
	SvLightData *data = (SvLightData *) lightDataList[i];

	if ( data->isManip() != data->shouldBeManip )
	    editLight(data, data->shouldBeManip);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Write the nodes under the selection node to the given file name.
//
// Use: private
SbBool
SoSceneViewer::writeFile(const char *filename)
//
////////////////////////////////////////////////////////////////////////
{
    SoWriteAction   wa;
    
    if (! wa.getOutput()->openFile(filename)) {
	
	// display an error dialog
	char str[100];
	strcpy(str, "Error creating file: ");
	strcat(str, filename);
	SoXt::createSimpleErrorDialog(mgrWidget, "File Error Dialog", str);
	
	return FALSE;
    }
    
    // temporarily replace all manips with regular transform nodes.
    removeManips();

    // restore the cameraNum settings of any scenekits.  
    restoreSceneKitCameras(selection);
    // Copy our camera into the last sceneKit.
    setLastSceneKitCamera(selection, (SoCamera *)getCamera()->copy());

    // write out all the children of the selection node
    for (int i = 0; i < selection->getNumChildren(); i++)
	wa.apply(selection->getChild(i));
    wa.getOutput()->closeFile();
    
    // Switch the sceneKit cameras back off:
    switchOffSceneKitCameras(selection);

    // Now put the manips back in the scene graph.
    restoreManips();
    
    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Write the Enviroment nodes (camera and lights) to the given 
//  file name.
//
// Use: private
SbBool
SoSceneViewer::writeEnvFile(const char *filename)
//
////////////////////////////////////////////////////////////////////////
{
    SoWriteAction	wa;
    
    if (! wa.getOutput()->openFile(filename)) {
	
	// display an error dialog
	char str[100];
	strcpy(str, "Error creating file: ");
	strcat(str, filename);
	SoXt::createSimpleErrorDialog(mgrWidget, "File Error Dialog", str);

	return FALSE;
    }
    
    // write out the environment including the headlight
    wa.apply(envLabel);
    wa.apply(getCamera());
    wa.apply(environment);
    wa.apply(lightGroup);
    if (isHeadlight())
	wa.apply(getHeadlight());
    
    wa.getOutput()->closeFile();
    
    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Print the scene using a custom print dialog.
//
// Use: private
void
SoSceneViewer::print()
//
////////////////////////////////////////////////////////////////////////
{
    if (printDialog == NULL) {
    	printDialog = new SoXtPrintDialog;
	printDialog->setTitle("SceneViewer Printing");
	printDialog->setBeforePrintCallback(
	    SoSceneViewer::beforePrintCallback, (void *) this);
	printDialog->setAfterPrintCallback(
	    SoSceneViewer::afterPrintCallback, (void *) this);
    }
    
    //
    // Send the render area size and scene graph to the print dialog
    //
    Widget widget = getRenderAreaWidget();
    if (widget != NULL) {
        Arg args[2];
        int n = 0;
        SbVec2s sz;
        XtSetArg(args[n], XtNwidth, &sz[0]); n++;
        XtSetArg(args[n], XtNheight, &sz[1]); n++;
        XtGetValues(widget, args, n);
        printDialog->setPrintSize(sz);
    }
    
    printDialog->show();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Temporarily remove manips from the scene.
// Restore them with a call to restoreManips().
//
// Use: private
void
SoSceneViewer::removeManips()
//
////////////////////////////////////////////////////////////////////////
{
    // temporarily replace all manips with regular transform nodes.
    for (int m = 0; m < maniplist->getLength(); m++ ) {
	SoTransformManip *manip = maniplist->getManip(m);	
	SoPath *xfPath = maniplist->getXfPath(m);
	manip->replaceManip(xfPath, NULL);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Restore manips that were removed with removeManips().
//
// Use: private
void
SoSceneViewer::restoreManips()
//
////////////////////////////////////////////////////////////////////////
{
    // Now put the manips back in the scene graph.
    for (int m = 0; m < maniplist->getLength(); m++ ) {
	SoTransformManip *manip = maniplist->getManip(m);	
	SoPath *xfPath = maniplist->getXfPath(m);
	manip->replaceNode(xfPath);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Temporarily remove manips from the scene. They
// will all be restored after the printing is done.
//
// Use: private, static
void
SoSceneViewer::beforePrintCallback(void *uData, SoXtPrintDialog *)
//
////////////////////////////////////////////////////////////////////////
{
    SoSceneViewer *sv = (SoSceneViewer *)uData;
    
    // temporarily replace all manips with regular transforms.
    sv->removeManips();

    // Do the same for all the light manips
    sv->removeAttachedLightManipGeometry();
    
    // if the current viewer is the examiner viewer, turn the 
    // feedback axis off while we print
    if (sv->whichViewer == SV_VWR_EXAMINER) {
	SoXtExaminerViewer *exam = (SoXtExaminerViewer *) sv->currentViewer;
	sv->feedbackShown = exam->isFeedbackVisible();
	exam->setFeedbackVisibility(FALSE);
    }
    
    // set the scene to print
    sv->printDialog->setSceneGraph(sv->sceneGraph);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Called after printing is done. Add the manips back into the
// scene.
//
// Use: private, static
void
SoSceneViewer::afterPrintCallback(void *uData, SoXtPrintDialog *)
//
////////////////////////////////////////////////////////////////////////
{
    SoSceneViewer *sv = (SoSceneViewer *)uData;
    
    // put the manips back in the scene graph.
    sv->restoreManips();
    
    // put the light manips back in the scene graph.
    sv->addAttachedLightManipGeometry();
    
    // restor the examiner feedback
    if (sv->whichViewer == SV_VWR_EXAMINER) {
	SoXtExaminerViewer *exam = (SoXtExaminerViewer *) sv->currentViewer;
	exam->setFeedbackVisibility( sv->feedbackShown );
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Static routine for processing topbar menu events.
//  When the menu is created, it stores pointer to the SoSceneViewer
//  in the client_data, so that we can tell which SoSceneViewer needs
//  the event.
//
// Use: private, static
//
void
SoSceneViewer::processTopbarEvent(
	Widget,				// Which widget?  I don't care
	SoSceneViewerData *data,	// Pointer to button/SoSceneViewer
	XmAnyCallbackStruct *cb )	// X garbage
//
////////////////////////////////////////////////////////////////////////
{
    SoSceneViewer *sv = data->classPt;
    Time eventTime = cb->event->xbutton.time;
    
    switch (data->id) {
    
    //
    // File
    //
    
    case SV_FILE_ABOUT:
	sv->showAboutDialog();
	break;
	
    case SV_FILE_NEW:
	sv->deleteScene();
	delete sv->fileName;
	sv->fileName = NULL;
	break;
	
    case SV_FILE_OPEN:
    case SV_FILE_IMPORT:
    case SV_FILE_SAVE_AS:
    case SV_FILE_READ_ENV:
    case SV_FILE_SAVE_ENV:
	sv->fileMode = data->id;
	sv->getFileName();
	break;
	
    case SV_FILE_SAVE:
	sv->save();
	break;
	
    case SV_FILE_PRINT:
    	sv->print();
	break;
    case SV_FILE_QUIT:
	delete sv;
	exit(0);
	break;
	
    //
    // Edit
    //
    
    case SV_EDIT_PICK_PARENT:
	sv->pickParent();
	break;
    case SV_EDIT_PICK_ALL:
	sv->pickAll();
	break;
    case SV_EDIT_CUT:
	// Remove manipulators before cutting the selection.
	sv->detachManipFromAll();
	sv->clipboard->copy((SoPathList *)sv->selection->getList(), eventTime);
	sv->destroySelectedObjects();
	sv->updateCommandAvailability();
	break;
    case SV_EDIT_COPY:
	// Remove manipulators while copying the selection.
	sv->removeManips();
	sv->clipboard->copy((SoPathList *)sv->selection->getList(),  eventTime);
	sv->restoreManips();
	break;
    case SV_EDIT_PASTE:
	sv->clipboard->paste(eventTime, pasteDoneCB, sv);
	break;
    case SV_EDIT_DELETE:
	sv->destroySelectedObjects();
	sv->updateCommandAvailability();
	break;
    
    //
    // Viewing
    //
    
    case SV_VIEW_PICK:
	sv->setViewing(! sv->isViewing());
	break;
#ifdef EXPLORER
    case SV_VIEW_USER:
	sv->userModeFlag = !sv->userModeFlag;
	if (sv->userModeFlag)
	    sv->currentViewer->setEventCallback(sv->userModeCB, sv->userModedata);
	else
	    sv->currentViewer->setEventCallback(NULL, NULL);
	break;
#endif
    
    case SV_VIEW_EXAMINER:
	sv->switchToViewer(SV_VWR_EXAMINER);
	break;
    case SV_VIEW_WALK:
	sv->switchToViewer(SV_VWR_WALK);
	break;
    case SV_VIEW_PLANE:
	sv->switchToViewer(SV_VWR_PLANE);
	break;
    case SV_VIEW_FLY:
	sv->switchToViewer(SV_VWR_FLY);
	break;
	
    case SV_VIEW_SELECTION:
        sv->viewSelection();
        break;

    case SV_VIEW_SCREEN_TRANSPARENCY:
	sv->setTransparencyType(SoGLRenderAction::SCREEN_DOOR);
	break;
    case SV_VIEW_BLEND_TRANSPARENCY:
	sv->setTransparencyType(SoGLRenderAction::BLEND);
	break;
    case SV_VIEW_DELAY_BLEND_TRANSPARENCY:
	sv->setTransparencyType(SoGLRenderAction::DELAYED_BLEND);
	break;
    case SV_VIEW_SORT_BLEND_TRANSPARENCY:
	sv->setTransparencyType(SoGLRenderAction::SORTED_OBJECT_BLEND);
	break;

    case SV_VIEW_FOG:
        sv->setFog(! sv->fogFlag);
        break;
    case SV_VIEW_ANTIALIASING:
        sv->setAntialiasing(! sv->antialiasingFlag);
        break;
    case SV_VIEW_BKG_COLOR:
        sv->editBackgroundColor();
        break;


    //
    // Editors
    //
    case SV_EDITOR_MATERIAL:
	sv->createMaterialEditor();
	break;

    case SV_EDITOR_TRANSFORM:
	sv->createTransformSliderSet();
	break;

    case SV_EDITOR_COLOR:
	sv->createColorEditor();
	break;

    //
    // Selection
    //
    case SV_SEL_SINGLE_SELECT:
    	sv->selection->policy.setValue(SoSelection::SINGLE);
	break;

    case SV_SEL_TOGGLE_SELECT:
    	sv->selection->policy.setValue(SoSelection::TOGGLE);
	break;

    case SV_SEL_SHIFT_SELECT:
    	sv->selection->policy.setValue(SoSelection::SHIFT);
	break;

    //
    // Manips
    //
    case SV_MANIP_TRANSFORMER:
	sv->highlightRA->setVisible(FALSE); // highlight visible when no manip
	sv->curManip = (sv->curManip == SV_TRANSFORMER) ? SV_NONE : SV_TRANSFORMER;
	if ( sv->curManipReplaces )
	    sv->replaceAllManips( sv->curManip );
        break;

    case SV_MANIP_TRACKBALL:
	sv->highlightRA->setVisible(FALSE); // highlight visible when no manip
	sv->curManip = (sv->curManip == SV_TRACKBALL) ? SV_NONE : SV_TRACKBALL;
	if ( sv->curManipReplaces )
	    sv->replaceAllManips( sv->curManip );
        break;

    case SV_MANIP_HANDLEBOX:
	sv->highlightRA->setVisible(FALSE); // highlight visible when no manip
	sv->curManip = (sv->curManip == SV_HANDLEBOX) ? SV_NONE : SV_HANDLEBOX;
	if ( sv->curManipReplaces )
	    sv->replaceAllManips( sv->curManip );
        break;

    case SV_MANIP_JACK:
	sv->highlightRA->setVisible(FALSE); // highlight visible when no manip
	sv->curManip = (sv->curManip == SV_JACK) ? SV_NONE : SV_JACK;
	if ( sv->curManipReplaces )
	    sv->replaceAllManips( sv->curManip );
        break;

    case SV_MANIP_CENTERBALL:
	sv->highlightRA->setVisible(FALSE); // highlight visible when no manip
	sv->curManip =(sv->curManip == SV_CENTERBALL) ? SV_NONE : SV_CENTERBALL;
	if ( sv->curManipReplaces )
	    sv->replaceAllManips( sv->curManip );
        break;

    case SV_MANIP_XFBOX:
	sv->highlightRA->setVisible(FALSE); // highlight visible when no manip
	sv->curManip = (sv->curManip == SV_XFBOX) ? SV_NONE : SV_XFBOX;
	if ( sv->curManipReplaces )
	    sv->replaceAllManips( sv->curManip );
        break;

    case SV_MANIP_TABBOX:
	sv->highlightRA->setVisible(FALSE); // highlight visible when no manip
	sv->curManip = (sv->curManip == SV_TABBOX) ? SV_NONE : SV_TABBOX;
	if ( sv->curManipReplaces )
	    sv->replaceAllManips( sv->curManip );
        break;

    case SV_MANIP_NONE:
	sv->highlightRA->setVisible(TRUE); // highlight visible when no manip
	sv->curManip = SV_NONE;
	if ( sv->curManipReplaces )
	    sv->detachManipFromAll();
        break;
    case SV_MANIP_REPLACE_ALL:
	// Toggle the value of 'curManipReplaces'
	sv->curManipReplaces = ( sv->curManipReplaces == TRUE) ? FALSE : TRUE;

	if ( sv->curManipReplaces )
	    sv->replaceAllManips( sv->curManip );
        break;
	
    //
    // Lights
    //
    case SV_LIGHT_AMBIENT_EDIT:	sv->editAmbientColor(); break;
    case SV_LIGHT_ADD_DIRECT:	sv->addLight(new SoDirectionalLightManip); break;
    case SV_LIGHT_ADD_POINT:	sv->addLight(new SoPointLightManip); break;
    case SV_LIGHT_ADD_SPOT:	
	{
	    // Set the dropOffRate to be non-zero, or it will always work
	    // like a point light.
	    SoSpotLightManip *newSpotManip = new SoSpotLightManip;
	    newSpotManip->dropOffRate = .01;
	    sv->addLight(newSpotManip); 
	}
	break;
    
    case SV_LIGHT_TURN_ON:
    case SV_LIGHT_TURN_OFF:
	{
	SbBool onFlag = (data->id == SV_LIGHT_TURN_ON);
	for (int i=0; i < sv->lightDataList.getLength(); i++)
	    sv->turnLightOnOff((SvLightData *) sv->lightDataList[i], onFlag);
	sv->turnLightOnOff( sv->headlightData, onFlag);
	}
	break;
    case SV_LIGHT_SHOW_ALL:
    case SV_LIGHT_HIDE_ALL:
	{
	SbBool onFlag = (data->id == SV_LIGHT_SHOW_ALL);
	for (int i=0; i < sv->lightDataList.getLength(); i++)
	    sv->editLight((SvLightData *) sv->lightDataList[i], onFlag);
	}
	break;
    
    } // endswitch( topbar button )
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Adds the given light to the scene and to the menu.
//
// Use: private
//
void
SoSceneViewer::addLight(SoLight *light)
//
////////////////////////////////////////////////////////////////////////
{
    // Calculate the light manip size if necessary.
    // Do this before adding the light into the scene,
    // since if the light is a manip, its default size might influence the
    // scene size.
    if (!calculatedLightManipSize)
	calculateLightManipSize();

    // create the switch and light node and add it to the scene
    SoSwitch *lightSwitch = new SoSwitch;
    lightGroup->addChild(lightSwitch);
    lightSwitch->addChild(light);
    SWITCH_LIGHT_ON(lightSwitch);
    
    // add the light entry for the new light
    SvLightData *data = addLightEntry(light, lightSwitch);

    //
    // Try to come up with some meaningfull default position base
    // of the current camera view volume.
    //
    SoCamera *vwrCamera = getCamera(); // don't cache this in the class
    SbViewVolume vv = vwrCamera->getViewVolume(0.0);
    SbVec3f forward = - vv.zVector();
    SbVec3f center = vwrCamera->position.getValue() + forward * 
	(vwrCamera->nearDistance.getValue() + vwrCamera->farDistance.getValue()) / 2.0;
    SbVec3f position( vv.ulf + forward * vv.nearToFar * .25 );
//XXX ??? XXX
//XXX this algorithm should be replaced. Perhaps instead of using
//XXX 'forward' we could go a little up and to the left?
//XXX ??? XXX
		
    if (data->type.isDerivedFrom(SoDirectionalLight::getClassTypeId())) {
	SoDirectionalLight *myLight = (SoDirectionalLight *) data->light;
	// the position of the light can't be given to the light itself.
	// So we use the translation and translation inverse to 
	// get it to go where we want.
	data->translation->translation = position;
	data->translationInverse->translation = -position;
	myLight->direction = center - position;
    }
    else {
	// The data->scale will influence the position we set.
	// So we need to prepare for this. Note, it's not a prolem for
	// directional lights since they use the translation node,
	// which is outside the scale and scaleInverse grouping
	SbVec3f invrs = data->scaleInverse->scaleFactor.getValue();
	SbVec3f scaledLoc = position;
	scaledLoc *= invrs[0]; 

	if (data->type.isDerivedFrom(SoPointLight::getClassTypeId())) {
	    SoPointLight *myLight = (SoPointLight *) data->light;
	    myLight->location = scaledLoc;
	    // no direction for this light
        }
        else if (data->type.isDerivedFrom(SoSpotLight::getClassTypeId())) {
	    SoSpotLight *myLight = (SoSpotLight *) data->light;
	    myLight->location = scaledLoc;
	    myLight->direction = center - position;
        }
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Creates and append the light data struct, and adds a menu entry
//  for the light.
//
// Use: private
//
SvLightData *
SoSceneViewer::addLightEntry(SoLight *light, SoSwitch *lightSwitch)
//
////////////////////////////////////////////////////////////////////////
{
    //
    // create the light data
    //
    
    SvLightData *data = new SvLightData;
    lightDataList.append(data);

    light->ref();
    data->light = light;

    data->lightSwitch = lightSwitch;

    // Try and find the scale, scaleInverse, translation, and 
    // translationInverse.
    data->scale = NULL;
    data->scaleInverse = NULL;
    data->translation = NULL;
    data->translationInverse = NULL;
    SbBool gotLight = FALSE;
    for ( int i = 0; i < lightSwitch->getNumChildren(); i++ ) {
	SoNode *n = lightSwitch->getChild(i);
	if (n == light)
	    gotLight = TRUE;
	else if (n->isOfType(SoScale::getClassTypeId())){
	    if (data->scale == NULL && gotLight == FALSE)
		data->scale = (SoScale *) n;
	    else if (data->scaleInverse == NULL && gotLight == TRUE)
		data->scaleInverse = (SoScale *) n;
	}
	else if (n->isOfType(SoTranslation::getClassTypeId())){
	    if (data->translation == NULL && gotLight == FALSE)
		data->translation = (SoTranslation *) n;
	    else if (data->translationInverse == NULL && gotLight == TRUE)
		data->translationInverse = (SoTranslation *) n;
	}
    }

    // Now install any missing nodes...
    if (data->scale == NULL) {
	data->scale = new SoScale;
	int lightInd = lightSwitch->findChild(light);
        lightSwitch->insertChild( data->scale, lightInd );
    }
    if (data->scaleInverse == NULL) {
        data->scaleInverse = new SoScale;
	int lightInd = lightSwitch->findChild(light);
        lightSwitch->insertChild( data->scaleInverse, lightInd + 1 );
    }
    if (data->translation == NULL) {
	data->translation = new SoTranslation;
	int scaleInd = lightSwitch->findChild(data->scale);
        lightSwitch->insertChild( data->translation, scaleInd );
    }
    if (data->translationInverse == NULL) {
        data->translationInverse = new SoTranslation;
	int scaleInvInd = lightSwitch->findChild(data->scaleInverse);
        lightSwitch->insertChild( data->translationInverse, scaleInvInd+1 );
    }
    // See if the size was already calculated (this happens when we read 
    // .env files)...
    SbVec3f oldScale = data->scale->scaleFactor.getValue();
    if ( calculatedLightManipSize == FALSE 
	 && oldScale != SbVec3f(1,1,1) ) {
	lightManipSize = oldScale[0];
	calculatedLightManipSize = TRUE;
    }

    data->classPt = this;
    data->colorEditor = NULL;
    data->type = light->getTypeId();
    
    // set the correct label name
    char *str;
    if (data->type.isDerivedFrom( SoDirectionalLight::getClassTypeId()))
	str = Light.directionallight;
    else if (data->type.isDerivedFrom( SoPointLight::getClassTypeId()) )
	str = Light.pointlight;
    else if (data->type.isDerivedFrom( SoSpotLight::getClassTypeId()) )
	str = Light.spotlight;
    else str = "??? ";
    data->name = strdup(str);
    
    // Set the size for the light manips.
    if ( !calculatedLightManipSize )
	calculateLightManipSize();

    data->scale->scaleFactor.setValue( lightManipSize, lightManipSize, 
				       lightManipSize );
    float invSz = (lightManipSize == 0.0) ? 1.0 : 1.0 / lightManipSize;
    data->scaleInverse->scaleFactor.setValue( invSz, invSz, invSz );
    
    //
    // add the menu entry
    //
    addLightMenuEntry(data);
    
    return data;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	build the light menu entry for the given light.
//
// Use: private
//
void
SoSceneViewer::addLightMenuEntry(SvLightData *data)
//
////////////////////////////////////////////////////////////////////////
{
    //
    // create the motif menu entry
    //
    
    Widget menu = menuItems[SV_LIGHT].widget;
    
    // makes sure menu has been built
    if (menu == NULL)
	return;
    
    // create the submenu widget, adding a callback to update the toggles
    Arg args[8];
    int argnum = 0;
#ifdef MENUS_IN_POPUP
    SoXt::getPopupArgs(XtDisplay(menu), SCREEN(menu), args, &argnum);
#endif
    data->submenuWidget = XmCreatePulldownMenu(menu, "submenu", args, argnum);
    
    XtAddCallback(data->submenuWidget, XmNmapCallback,
	(XtCallbackProc) SoSceneViewer::lightSubmenuDisplay,
	(XtPointer) data);
    
    // create a cascade menu entry which will bring the submenu
    XtSetArg(args[0], XmNsubMenuId, data->submenuWidget);
    data->cascadeWidget = XtCreateWidget(data->name, 
	xmCascadeButtonGadgetClass, menu, args, 1);
    
    // add "on/off" toggle
    data->onOffWidget = XtCreateWidget("On/Off", xmToggleButtonGadgetClass, 
	data->submenuWidget, NULL, 0);
    XtAddCallback(data->onOffWidget, XmNvalueChangedCallback,
	(XtCallbackProc) SoSceneViewer::lightToggleCB, (XtPointer) data);
    
    // add "Icon" toggle
    data->iconWidget = XtCreateWidget("Icon", xmToggleButtonGadgetClass, 
	data->submenuWidget, NULL, 0);
    XtAddCallback(data->iconWidget, XmNvalueChangedCallback,
	(XtCallbackProc) SoSceneViewer::editLightToggleCB, (XtPointer) data);
    
    // add "Edit Color" toggle
    data->editColorWidget = XtCreateWidget("Edit Color", xmPushButtonGadgetClass, 
	data->submenuWidget, NULL, 0);
    XtAddCallback(data->editColorWidget, XmNactivateCallback,
	(XtCallbackProc) SoSceneViewer::editLightColorCB, (XtPointer) data);
    
    // add "Remove" entry
    data->removeWidget = XtCreateWidget("Remove", xmPushButtonGadgetClass, 
	data->submenuWidget, NULL, 0);
    XtAddCallback(data->removeWidget, XmNactivateCallback,
	(XtCallbackProc) SoSceneViewer::removeLightCB, (XtPointer) data);
    
    // manage children
    XtManageChild(data->onOffWidget);
    XtManageChild(data->iconWidget);
    XtManageChild(data->editColorWidget);
    XtManageChild(data->removeWidget);
    XtManageChild(data->cascadeWidget);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Called by "On/Off" light menu entry when toggle changes.
//
// Use: static private
//
void
SoSceneViewer::lightToggleCB(Widget toggle, SvLightData *data, void *)
//
////////////////////////////////////////////////////////////////////////
{
    data->classPt->turnLightOnOff(data, XmToggleButtonGetState(toggle));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Turn the given light on or off.
//
// Use: private
//
void
SoSceneViewer::turnLightOnOff(SvLightData *data, SbBool flag)
//
////////////////////////////////////////////////////////////////////////
{
    // check if it is the headlight
    if (data == headlightData)
	setHeadlight( flag );
    else {
	if ( flag )
	    SWITCH_LIGHT_ON(data->lightSwitch);
	else
	    SWITCH_LIGHT_OFF(data->lightSwitch);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	"Edit" light menu entry callback.
//
// Use: static private
//
void
SoSceneViewer::editLightToggleCB(Widget toggle, SvLightData *data, void *)
//
////////////////////////////////////////////////////////////////////////
{
    data->classPt->editLight( data, XmToggleButtonGetState(toggle) );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Attach/detach the correct manipulator on the given light.
//
// Use: private
//
void
SoSceneViewer::editLight(SvLightData *data, SbBool flag)
//
////////////////////////////////////////////////////////////////////////
{
    // ??? check if this is for the headlight, which is special cased
    // ??? since a manipulator cannot be used (aligned to camera).
    SbBool forHeadlight = (data == data->classPt->headlightData);

    SoType dataType = data->type;
    SoType dirLightType = SoDirectionalLight::getClassTypeId();
    SoType ptLightType = SoPointLight::getClassTypeId();
    SoType spotLightType = SoSpotLight::getClassTypeId();
    
    //
    // attach the manip to the light and add it to the scene
    //
    if (flag) {

	if (forHeadlight) {

	    if (headlightEditor == NULL) {
		headlightEditor = new SoXtDirectionalLightEditor;
		headlightEditor->setTitle("Headlight Editor");
	    }

	    // Make sure we have the current viewer's headlight
	    SoLight *l = data->classPt->getHeadlight();
	    l->ref();
	    if (data->light)
		data->light->unref();
	    data->light = l;

	    // attach the dir light editor
	    // ??? don't use the path from the root to the headlight 
	    // ??? since we want the light to be relative to the 
	    // ??? camera (i.e. moving the camera shouldn't affect
	    // ??? the arrow in the editor since that direction
	    // ??? is relative to the camera).
	    SoPath *littlePath = new SoPath( data->light );
	    headlightEditor->attach( littlePath );
	    headlightEditor->show();
	}
	else if (data->isManip() == FALSE) {

	    // NOTE: if isManip() == TRUE, then the light is already a manip
	    // and doesn't need to be changed.

	    SoLight *newManip = NULL;
	
	    // allocate the right manipulator type if needed
	    if ( dataType.isDerivedFrom( dirLightType ) ) {
		newManip = new SoDirectionalLightManip;
		newManip->ref();
	    }
	    else if ( dataType.isDerivedFrom( ptLightType ) ) {
		newManip = new SoPointLightManip;
		newManip->ref();
	    }
	    else if ( dataType.isDerivedFrom( spotLightType ) ) {
		newManip = new SoSpotLightManip;
		newManip->ref();
	        // Set dropOffRate non-zero, or it will look like a pointLight.
		((SoSpotLightManip *)newManip)->dropOffRate = .01;
	    }

	    // get the path from the root to the light node
	    SoSearchAction sa;
	    sa.setNode( data->light );
	    sa.apply( currentViewer->getSceneGraph() );
	    SoPath *path = sa.getPath();
	    // ??? light is probably turned off so we don't 
	    // ??? need to print a warning message. Just don't
	    // ??? do anything
	    if (path == NULL) {
		newManip->unref();
		return;
	    }

	    path->ref();

	    // Set the size for the light manips.
	    if ( !calculatedLightManipSize )
		calculateLightManipSize();

	    data->scale->scaleFactor.setValue( lightManipSize, lightManipSize, 
					       lightManipSize );
	    float invSz = (lightManipSize == 0.0) ? 1.0 : 1.0 / lightManipSize;
	    data->scaleInverse->scaleFactor.setValue( invSz, invSz, invSz );
	    
	    // Put the manip into the scene.
	    if ( dataType.isDerivedFrom( ptLightType ) )
		((SoPointLightManip *)newManip)->replaceNode(path);
	    else if (dataType.isDerivedFrom(dirLightType) )
		((SoDirectionalLightManip *)newManip)->replaceNode(path);
	    else if (dataType.isDerivedFrom(spotLightType))
		((SoSpotLightManip *)newManip)->replaceNode(path);

	    // Okay, now that we stuck that manip in there,
	    // we better make a note of it...
	    path->unref();
	    data->light->unref();
	    data->light = newManip;

	}
    }
    //
    // detach the manip from the light and remove it from the scene
    //
    else {
	if (forHeadlight) {
	    // detach editor from light
	    if (headlightEditor != NULL) {
		headlightEditor->detach();
		headlightEditor->hide();
	    }
	}
	else if (data->isManip() == TRUE ) {

	    // replace the lightManip node with a regular light node
	    // get the path from the root to the lightManip node
	    SoSearchAction sa;
	    sa.setNode( data->light );
	    sa.apply( currentViewer->getSceneGraph() );
	    SoPath *path = sa.getPath();

	    if (path != NULL ) {
		path->ref();

		SoLight *newLight;

		if (dataType.isDerivedFrom(ptLightType)) {
		    newLight = new SoPointLight;
		    newLight->ref();
		    ((SoPointLightManip *)data->light)->replaceManip(
					path, (SoPointLight *)newLight );
		}
		else if (dataType.isDerivedFrom(dirLightType)) {
		    // when removing a directional light manip, 
		    // we don't want to lose the translation of the manip.
		    // Since the regular light can't hold this value,
		    // get it from the manip and move it to the translation.

		    transferDirectionalLightLocation(data);

		    newLight = new SoDirectionalLight;
		    newLight->ref();
		    ((SoDirectionalLightManip *)data->light)->replaceManip(
				path, (SoDirectionalLight *)newLight );
		}
		else if (dataType.isDerivedFrom(spotLightType)) {
		    newLight = new SoSpotLight;
		    newLight->ref();
		    ((SoSpotLightManip *)data->light)->replaceManip(
					path, (SoSpotLight *)newLight );
		}
		path->unref();
		data->light->unref();
		data->light = newLight;
	    }
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//     When removing or writing directional light manip, 
//     we don't want to lose the translation of the manip.
//     Since the regular light can't hold this value,
//     get it from the manip and move it to the translation.
//
// Use: private
//
void
SoSceneViewer::transferDirectionalLightLocation( SvLightData *data )
//
////////////////////////////////////////////////////////////////////////
{
    if ( !data->light )
	return;

    if ( !data->light->isOfType(SoDirectionalLightManip::getClassTypeId() ))
	return;

    // when removing a directional light manip, 
    // we don't want to lose the translation of the manip.
    // Since the regular light can't hold this value,
    // get it from the manip and move it to the translation.

    SoDirectionalLightManip *manip 
	= (SoDirectionalLightManip *) data->light;
    SoDirectionalLightDragger *dragger 
	= (SoDirectionalLightDragger *) manip->getDragger();

    SbVec3f lightTrans(0,0,0); 

    if (dragger)
	lightTrans += dragger->translation.getValue();

    SbVec3f scl = data->scale->scaleFactor.getValue();
    lightTrans *= scl[0];

    lightTrans += data->translation->translation.getValue();
    data->translation->translation        = lightTrans;
    data->translationInverse->translation = -lightTrans;

    // Now zero out the translation in the dragger itself:
    dragger->translation = SbVec3f(0,0,0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Called by "Edit Color" light menu entry.
//
// Use: static private
//
void
SoSceneViewer::editLightColorCB(Widget, SvLightData *data, void *)
//
////////////////////////////////////////////////////////////////////////
{
    // create the color editor with the right title
    if (data->colorEditor == NULL) {
	data->colorEditor = new MyColorEditor;
	char str[50];
	strcpy(str, data->name);
	strcat(str, " Light Color");
	data->colorEditor->setTitle(str);
    }
    
    if ( !data->colorEditor->isAttached() ) {
	// if this is for the headlight, make sure we have the
	// current viewer headlight
	if (data == data->classPt->headlightData) {
	    SoLight *l = data->classPt->getHeadlight();
	    l->ref();
	    if (data->light)
		data->light->unref();
	    data->light = l;
	}
	
	// normalize the light intensity
	SbColor col;
	col = data->light->color.getValue();
	col *= data->light->intensity.getValue();
	data->light->intensity.setValue( 1.0 );
	data->light->color.setValue( col );
	
	data->colorEditor->attach( &data->light->color, data->light );
    }
    
    data->colorEditor->show();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	remove button menu entry callback.
//
// Use: static private
//
void
SoSceneViewer::removeLightCB(Widget, SvLightData *data, void *)
//
////////////////////////////////////////////////////////////////////////
{
    data->classPt->removeLight(data);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	removes the light from the scene, and removes the light data 
//  and pulldown menu entry.
//
// Use: private
//
void
SoSceneViewer::removeLight(SvLightData *data)
//
////////////////////////////////////////////////////////////////////////
{
    // delete the color editor and manip
    delete data->colorEditor;

// note: deleted  code that dealt with the manip.
// Since the light and the manip are one and the same now.
// unrefing the light also removes the manip

    // unref the light (or manip) for this entry
    if (data->light)
	data->light->unref();
    
    // remove the light from the scene
    lightGroup->removeChild( data->lightSwitch );
    
    // nuke the menu entry
    if (data->cascadeWidget != NULL)
	XtDestroyWidget( data->cascadeWidget );
    
    // remove from list and delete the struct
    lightDataList.remove( lightDataList.find(data) );
    delete data;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//     Calculate the size for the light manips.
//	Called when the first lightmanip is created.
//      Also called when a scene is read or imported, assuming lights
//      have already been created.
//
// Use: private
//
void
SoSceneViewer::calculateLightManipSize()
//
////////////////////////////////////////////////////////////////////////
{
    // Set the size for the light manips.
    // Calculate a good size, based on the size of the scene.
    // Once this size is determined, use it for all other light manips.
    // (We need to save the value because the scene size will change
    //  over time, but we want all light manips to be the same size.
    
    // This routine will also get called again if a new scene is read in.
    
    // Run a bounding box action on the scene...
    SoGetBoundingBoxAction ba(currentViewer->getViewportRegion());
    ba.apply( currentViewer->getSceneGraph() );
    SbBox3f sceneBox = ba.getBoundingBox();
    SbVec3f size;
    if (sceneBox.isEmpty()) {
	// Oh, just take a guess. Default scene would be 2x2x2
	size.setValue(2,2,2);
    }
    else
	size = sceneBox.getMax() - sceneBox.getMin();
//XXX pick a good size!
    lightManipSize = .025 * size.length();

    // Visit every light entry and set the scale and scaleInverse
    // nodes to have the correct lightManipSize.
    for (int i = 0; i < lightDataList.getLength(); i++ ) {
	
	SvLightData *data = (SvLightData *) lightDataList[i];

	if (data->scale != NULL) {
	    if (data->scale->scaleFactor.getValue()[0] != lightManipSize)
		data->scale->scaleFactor.setValue( lightManipSize, 
				    lightManipSize, lightManipSize );
	}
	if (data->scaleInverse != NULL) {
	    float invSz = (lightManipSize == 0.0) ? 1.0 : 1.0 / lightManipSize;
	    if (data->scaleInverse->scaleFactor.getValue()[0] != invSz)
		data->scaleInverse->scaleFactor.setValue( invSz, invSz, invSz );
	}
    }

    calculatedLightManipSize = TRUE;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//	Called whenever a light submenu is mapped on screen (update 
//  the toggles)
//
// Use: static private
//
void
SoSceneViewer::lightSubmenuDisplay(Widget, SvLightData *data, void *)
//
////////////////////////////////////////////////////////////////////////
{
    SoSceneViewer *sv = data->classPt;
    SbBool set;
    
    //
    // update the "on/off" toggle
    //
    if (data == sv->headlightData)
	set = sv->isHeadlight();
    else
	set = IS_LIGHT_ON(data->lightSwitch);
    if (set)
	TOGGLE_ON(data->onOffWidget);
    else
	TOGGLE_OFF(data->onOffWidget);
    
    //
    // update the "Edit" toggle
    //
    if (data == sv->headlightData)
	set = (sv->headlightEditor != NULL && sv->headlightEditor->isVisible());
    else
	set = (data->isManip() == TRUE );
    if (set)
	TOGGLE_ON(data->iconWidget);
    else
	TOGGLE_OFF(data->iconWidget);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Called after a paste operation has completed.
//
// Use: static, private
//
void
SoSceneViewer::pasteDoneCB(void *userData, SoPathList *pathList)
//
////////////////////////////////////////////////////////////////////////
{
    SoSceneViewer *sv = (SoSceneViewer *) userData;
    sv->pasteDone(pathList);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Called after a drop operation has completed.
//
// Use: static, private
//
SbBool
SoSceneViewer::iconDropCB(void *userData, const char *filename)
//
////////////////////////////////////////////////////////////////////////
{
    SoSceneViewer *sv = (SoSceneViewer *) userData;
    sv->fileMode = SV_FILE_IMPORT;
    return sv->doFileIO(filename);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Called during a drop operation of INVENTOR objects.
//
// Use: static, private
//
SbBool
SoSceneViewer::objDropCB(
    void *userData,
    Atom target,
    void *data, uint32_t numBytes)
//
////////////////////////////////////////////////////////////////////////
{
    SoSceneViewer *sv = (SoSceneViewer *) userData;
    SoPathList *pathList = NULL;
    SbBool success = FALSE;
    Display *d = SoXt::getDisplay();
    	     
    if (data != NULL) {
	if (target == _ATOM_(d, "INVENTOR_2_1") ||
	    target == _ATOM_(d, "VRML_1_0") ||
	    target == _ATOM_(d, "INVENTOR") ||
	    target == _ATOM_(d, "INVENTOR_2_0")) {
	    
	    pathList = SoByteStream::unconvert(data, numBytes);
	    if (pathList != NULL && pathList->getLength() > 0) {
		sv->pasteDone(pathList);
		success = TRUE;
		// pathList will be deleted by pasteDone...
	    }
	}
    }
    
    return success;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Called after a paste operation has completed, this adds the
// pasted data to our scene graph.
//
// Use: private
//
void
SoSceneViewer::pasteDone(SoPathList *pathList)
//
////////////////////////////////////////////////////////////////////////
{
    if (pathList->getLength() <= 0)
    	return;

    // If this is the only obj, we will view all
    SbBool doViewAll = (selection->getNumChildren() == 0);
    
    // first, detach manips from all selected objects
    detachManipFromAll();
    
    // now, turn off the sel/desel callbacks.
    // we'll turn them on again after we've adjusted the selection
    selection->removeSelectionCallback( SoSceneViewer::selectionCallback, this );
    selection->removeDeselectionCallback( SoSceneViewer::deselectionCallback, this );
    
    // now deselect all, and build up a selection from the pasted paths
    selection->deselectAll();
    
    // Add every path in the path list as a child under selection.
    // Then select each of these paths.
    for (int i = 0; i < pathList->getLength(); i++) {
    
    	// if the head of the path is a selection node, then don't
	// paste the head - rather, paste all of its children. 
	// this makes sure we don't have more than 1 selection node.
	// While we're adding the paths as children, select each path.
    	SoPath *p = (*pathList)[i];
	SoNode *head = p->getHead();
	SoPath *selpath;
	if (head->isOfType(SoSelection::getClassTypeId())) {
	    for (int j = 0; j < ((SoSelection *)head)->getNumChildren(); j++) {
	    	selection->addChild(((SoSelection *)head)->getChild(j));
		
		// create a path from selection to this child
		// and select the path.
		selpath = new SoPath(selection);
		selpath->append(selection->getNumChildren() - 1);
		selection->select(selpath);
	    }
	}
	else {
	    // not a selection node, so just add it.
	    selection->addChild(p->getHead());
	    
	    // create a path from selection to this child
	    // and select the path.
	    selpath = new SoPath(selection);
	    selpath->append(selection->getNumChildren() - 1);
	    selection->select(selpath);
	}
    }
    
    // now add manips to all the selected objects
    attachManipToAll(curManip);
    
    // and turn the sel/desel callbacks back on
    selection->addSelectionCallback( SoSceneViewer::selectionCallback, this );
    selection->addDeselectionCallback( SoSceneViewer::deselectionCallback, this );
    
    // enable/disable keyboard shortcuts
    updateCommandAvailability();
    
    // View all?
    if (doViewAll)
	viewAll();

    delete pathList;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Build routine for SceneViewer.  This creates all of the X widgets
//
// Use: public, virtual

Widget
SoSceneViewer::buildWidget(Widget parent)
//
////////////////////////////////////////////////////////////////////////
{
    int	    n;
    Arg	    args[10];
    
    // create a form to hold everything together
    SbVec2s size = getSize();
    n = 0;
    if (size[0] != 0 && size[1] != 0) {
	XtSetArg(args[n], XtNwidth, size[0]); n++;
	XtSetArg(args[n], XtNheight, size[1]); n++;
    }
    mgrWidget = XtCreateWidget(getWidgetName(), xmFormWidgetClass, parent, args, n);
    registerWidget(mgrWidget);
    
    // create the topbar menu
    if (showMenuFlag)
	buildAndLayoutMenu(mgrWidget);
    
    // build and layout the current viewer
    whichViewer = SV_VWR_EXAMINER;
    setTitle("SceneViewer (Examiner)");
    currentViewer = viewerList[whichViewer] = new SoXtExaminerViewer(mgrWidget);
    currentViewer->setSceneGraph(sceneGraph);
    currentViewer->setGLRenderAction(highlightRA);
    currentViewer->redrawOnSelectionChange(selection);
    
    setOverlayLogo(currentViewer);
    
    // Fog
    environment->fogColor.setValue( currentViewer->getBackgroundColor() );

    // since we created the camera, do a view all and save this
    // as the starting point (don't want default camera values).
    viewAll();
    saveHomePosition();


    buildAndLayoutViewer(currentViewer);
    
    // manage those children
    if (showMenuFlag)
	XtManageChild(menuWidget);
    currentViewer->show();

    // clipboard is for copy/paste of 3d data.
    //??? what if this SceneViewer had its widget destroyed and rebuilt?
    //??? we need to destroy the clipboards when that happens.
    clipboard = new SoXtClipboard(mgrWidget);
    
    MyDropSite::registerCallback(
	mgrWidget, iconDropCB, objDropCB, this);
    
    return mgrWidget;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Builds and layout the given viewer.
//
// Use: private
void
SoSceneViewer::buildAndLayoutViewer(SoXtFullViewer *vwr)
//
////////////////////////////////////////////////////////////////////////
{
    if (mgrWidget == NULL)
	return;
    
    // build the viewer if necessary
    if (vwr->getWidget() == NULL) {
	fprintf(stderr, "ERROR - need to create the viewer widget before it can be layed out\n");
	return;
    }
    
    // layout the viewer to be attached under the topbar menu
    // (if the pulldown menu is shown)
    Arg args[12];
    int n = 0;
    if ( showMenuFlag ) {
	XtSetArg(args[n], XmNtopAttachment,	XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopWidget,		menuWidget); n++;
    }
    else {
	XtSetArg(args[n], XmNtopAttachment,	XmATTACH_FORM); n++;
    }
    XtSetArg(args[n], XmNbottomAttachment,	XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment,	XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment,	XmATTACH_FORM); n++;
    XtSetValues(vwr->getWidget(), args, n);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Create topbar menu.  Invalid buttons are rendered gray.
//      Each button's callback include a structure with the ID
//      of the button and a pointer to the SoSceneViewer that created
//      it.
//
// Use: private
//
void
SoSceneViewer::buildAndLayoutMenu(Widget parent)
//
////////////////////////////////////////////////////////////////////////
{
    if (menuWidget != NULL)
	return;
    
    Arg			args[8];
    int			i, j, n, id;
    WidgetList		buttons, subButtons;
    int			itemCount, subItemCount;
    WidgetClass	    	widgetClass;
    String  	    	callbackReason;
    XmString		xmstr;
    
    // create topbar menu
    menuWidget = XmCreateMenuBar(parent, "menuBar", NULL, 0);

    Arg popupargs[4];
    int popupn = 0;
#ifdef MENUS_IN_POPUP
    Widget shell = SoXt::getShellWidget(mgrWidget);
    SoXt::getPopupArgs(XtDisplay(menuWidget), SCREEN(shell), popupargs, &popupn);
#endif

    itemCount = XtNumber(pulldownData);
    buttons = (WidgetList) XtMalloc(itemCount * sizeof(Widget));

    for (i = 0; i < itemCount; i++) {
	// Make Topbar menu button
	Widget subMenu = XmCreatePulldownMenu(menuWidget, "subMenu", popupargs, popupn);
	
#ifdef MENUS_IN_POPUP
	// register callbacks to load/unload the pulldown colormap when the
	// pulldown menu is posted.
	SoXt::registerColormapLoad(subMenu, shell);
#endif
	
	id = pulldownData[i].id;
	menuItems[id].widget = subMenu;
    	XtAddCallback(subMenu, XmNmapCallback,
	    (XtCallbackProc) SoSceneViewer::menuDisplay,
	    (XtPointer) &menuItems[id]);

    	XtSetArg(args[0], XmNsubMenuId, subMenu);
	buttons[i] = XtCreateWidget(pulldownData[i].name,
	    xmCascadeButtonGadgetClass, menuWidget, args, 1);

	// Make subMenu buttons
	subItemCount = pulldownData[i].subItemCount;
	subButtons = (WidgetList) XtMalloc(subItemCount * sizeof(Widget));
	
	for (j = 0; j < subItemCount; j++) {
	    if (pulldownData[i].subMenu[j].buttonType == SV_SEPARATOR)
		subButtons[j] = XtCreateWidget(NULL, xmSeparatorGadgetClass, 
		    subMenu, NULL, 0);
	    else {
    	    	switch (pulldownData[i].subMenu[j].buttonType) {
		    case SV_PUSH_BUTTON:
			widgetClass = xmPushButtonGadgetClass;
			callbackReason = XmNactivateCallback;
			n = 0;
		    	break;
		    case SV_TOGGLE_BUTTON:
			widgetClass = xmToggleButtonGadgetClass;
			callbackReason = XmNvalueChangedCallback;
			n = 0;
		    	break;
		    case SV_RADIO_BUTTON:
			widgetClass = xmToggleButtonGadgetClass;
			callbackReason = XmNvalueChangedCallback;
			XtSetArg(args[0], XmNindicatorType, XmONE_OF_MANY);
			n = 1;
		    	break;
		    default:
			fprintf(stderr, "SceneViewer INTERNAL ERROR: bad buttonType\n");
		    	break;
		}
		
		// check for keyboard accelerator
		char *accel = pulldownData[i].subMenu[j].accelerator;
		char *accelText = pulldownData[i].subMenu[j].accelText;
		xmstr = NULL;
		if (accel != NULL) {
		    XtSetArg(args[n], XmNaccelerator, accel); n++;
		    
		    if (accelText != NULL) {
			xmstr = XmStringCreate(accelText,
					 XmSTRING_DEFAULT_CHARSET);
			XtSetArg(args[n], XmNacceleratorText, xmstr); n++;
		    }
		}
		
		subButtons[j] = XtCreateWidget(
		    pulldownData[i].subMenu[j].name,
		    widgetClass,
		    subMenu, args, n);
		
		if (xmstr != NULL)
		    XmStringFree(xmstr);
		id = pulldownData[i].subMenu[j].id;
		menuItems[id].widget = subButtons[j];
		XtAddCallback(subButtons[j], callbackReason,
		    (XtCallbackProc)SoSceneViewer::processTopbarEvent,
		    (XtPointer) &menuItems[id]);
	    }
	}
	XtManageChildren(subButtons, subItemCount);
	XtFree((char *)subButtons);
    }
    XtManageChildren(buttons, itemCount);
    XtFree((char *)buttons);
    
    //
    // layout the menu bar
    //
    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetValues(menuWidget, args, n);
    
    //
    // Add the light items which are dynamically created
    //    
    // first add the headlight
    addLightMenuEntry(headlightData);
    XtUnmanageChild(headlightData->removeWidget);
    XtUnmanageChild(headlightData->editColorWidget);
    xmstr = XmStringCreate( Light.editlight, XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[0], XmNlabelString, xmstr);
    XtSetValues(headlightData->iconWidget, args, 1);
    XmStringFree(xmstr);
    
    // now the regular lights
    for (i=0; i < lightDataList.getLength(); i++)
	addLightMenuEntry( (SvLightData *) lightDataList[i] );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Show/hide the pulldown menu bar.
//
// Use: public
void
SoSceneViewer::showMenu(SbBool flag)
//
////////////////////////////////////////////////////////////////////////
{
    if (showMenuFlag == flag || mgrWidget == NULL) {
	showMenuFlag = flag;
	return;
    }
    
    showMenuFlag = flag;
    
    if ( showMenuFlag ) {
	
	// turn topbar menu on
	if (menuWidget == NULL)
	    buildAndLayoutMenu(mgrWidget);
	XtManageChild(menuWidget);
    	
	// attach viewer to bottom of menu
	Arg args[2];
	int n = 0;
	XtSetArg(args[n], XmNtopAttachment,	XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopWidget,		menuWidget);    n++;
	XtSetValues(currentViewer->getWidget(), args, n);
    }
    else {
	// attach viewer to form
	Arg args[2];
	int n = 0;
	XtSetArg(args[n], XmNtopAttachment,	XmATTACH_FORM); n++;
	XtSetValues(currentViewer->getWidget(), args, n);
    	
	// turn topbar menu off
	if (menuWidget != NULL)
	    XtUnmanageChild(menuWidget);
    }
}

////////////////////////////////////////////////////////////////////////
//
//  Manage the changes in the selected node(s)
//
//  Use: private, static
//
SoPath *
SoSceneViewer::pickFilterCB(void *userData, const SoPickedPoint *pick)
//
////////////////////////////////////////////////////////////////////////
{
    SoSceneViewer *sv = (SoSceneViewer *) userData;
    SoPath *filteredPath = NULL;
    
    // If there are any transform manips along the path, check if they
    // belong to our personal set of manips. 
    // If so, change the path so it points to the object the manip
    // is attached to.
    SoFullPath *fullP = (SoFullPath *) pick->getPath();
    SoNode     *n;
    for (int i = 0; i < fullP->getLength(); i++ ) {
	n = fullP->getNode(i);
	if (n->isOfType(SoTransformManip::getClassTypeId())) {
	    int which = sv->maniplist->find((SoTransformManip *) n);
	    if (which != -1) {
		filteredPath = sv->maniplist->getSelectionPath(which);
		return filteredPath;
	    }
	}
    }

    // If we didn't pick one of our manipulators, then return the pickPath
    filteredPath = pick->getPath();
    return filteredPath;
}

////////////////////////////////////////////////////////////////////////
//
//  Manage the changes in the selected node(s)
//
//  Use: private, static
//
void
SoSceneViewer::deselectionCallback( void *userData,	   	     // my data
				    SoPath *deselectedPath ) // object
//
////////////////////////////////////////////////////////////////////////
{
    SoSceneViewer *sv = (SoSceneViewer *) userData;

    // remove the manip
    sv->detachManip( deselectedPath );
    
    // Remove editors
    if (sv->materialEditor)
	sv->materialEditor->detach();
    
    if (sv->colorEditor)
    	sv->colorEditor->detach();

    if (sv->transformSliderSet)
	sv->transformSliderSet->setNode( NULL );

    // enable/disable cmd key shortcuts and menu items
    sv->updateCommandAvailability();
}

////////////////////////////////////////////////////////////////////////
//
//  Manage the changes in the selected node(s)
//
//  Use: private, static
//
void
SoSceneViewer::selectionCallback( void *userData,		 // my data
				  SoPath *selectedPath ) // object
//
////////////////////////////////////////////////////////////////////////
{
    SoSceneViewer *sv = (SoSceneViewer *) userData;

    // attach the manip
    sv->attachManip( sv->curManip, selectedPath );

    //
    // If active, attach editors to new selection.
    //
    SoMaterial *mtl = NULL;
    if ( sv->materialEditor && sv->materialEditor->isVisible()) {
	mtl = sv->findMaterialForAttach(selectedPath);
	sv->materialEditor->attach(mtl);
    }
    
    if (sv->colorEditor && sv->colorEditor->isVisible()) {
    	if (mtl == NULL)
	    mtl = sv->findMaterialForAttach(selectedPath);
    	sv->colorEditor->attach(&(mtl->diffuseColor), 0, mtl);
    }
    
    if ( sv->transformSliderSet && sv->transformSliderSet->isVisible() ) {
	SoPath      *editTransformPath;
	editTransformPath = sv->findTransformForAttach( selectedPath );
	if ( editTransformPath == NULL ) {
	    sv->transformSliderSet->setNode( NULL );
	}
	else  {
	    editTransformPath->ref();
	    sv->transformSliderSet->setNode(((SoFullPath *)editTransformPath)->getTail() );
	    editTransformPath->unref();
	}
    }

    // enable/disable cmd key shortcuts and menu items
    sv->updateCommandAvailability();
}

////////////////////////////////////////////////////////////////////////
//
//  Remove selected objects from the scene graph. 
//  In this demo, we don't really know how the graphs are set up,
//  so act conservatively, and simply remove the node which is the
//  tail of the path from its parent. Note if the node is instanced,
//  all instances will be destroyed. Then travel up the path to a
//  parent separator. If there are no other shapes under the separator,
//  destroy it too.
//
//  Other applications might delete selected objects a different way,
//  depending on how the data is organized in the scene graph.
//
//  Use: protected
//
void
SoSceneViewer::destroySelectedObjects()
//
////////////////////////////////////////////////////////////////////////
{
    for (int i = selection->getNumSelected() - 1; i >= 0; i--) {
	SoPath *p = (*selection)[i];
	p->ref();
	
	// Deselect this path
	selection->deselect(i);

	// Remove the tail node from the graph
	SoGroup *g = (SoGroup *) p->getNodeFromTail(1);
	g->removeChild(p->getTail());
	
	// Travel up the path to separators, and see if this was
	// the only shape node under the sep. If so, delete the sep too.
	// (Don't go all the way up to the selection node).
	SbBool shapeFound = FALSE;
	int j = 0;
	while ((! shapeFound) && (j < p->getLength() - 1)) {
	    SoNode *n = p->getNodeFromTail(j);
	    if (n->isOfType(SoSeparator::getClassTypeId())) {
		// Search for other shape nodes
		SoSearchAction sa;
		sa.setFind(SoSearchAction::TYPE);
		sa.setType(SoShape::getClassTypeId());
		sa.apply(n);
		
		// If no other shapes under this separator, delete it!
		if (sa.getPath() == NULL) {
		    g = (SoGroup *) p->getNodeFromTail(j + 1);
		    g->removeChild(n);
		    
		    // Reset j since we have a new end of path
		    j = 0;
		}
		else shapeFound = TRUE;
	    }
	    // Else a group with no children?
	    else if (n->isOfType(SoGroup::getClassTypeId()) &&
		(((SoGroup *)n)->getNumChildren() == 0)) {
		    g = (SoGroup *) p->getNodeFromTail(j + 1);
		    g->removeChild(n);
		    
		    // Reset j since we have a new end of path
		    j = 0;
	    }
	    // Else continue up the path looking for separators
	    else j++;
	}
	
	p->unref();
    }
}

////////////////////////////////////////////////////////////////////////
//
//  This enables/disables cmd key shortcuts and menu items
//  based on whether there are any objects, and/or any selected objects
//  in the scene graph.
//
//  Use: static private
//
// 
void
SoSceneViewer::updateCommandAvailability()
//
////////////////////////////////////////////////////////////////////////
{
    Arg args[1];

    // enable/disable based on the number of child objects in scene
    if (selection->getNumChildren() == 0)
	 XtSetArg(args[0], XmNsensitive, False);
    else XtSetArg(args[0], XmNsensitive, True);
    
    // save (if no children, nothing to save)
    XtSetValues(menuItems[SV_FILE_SAVE].widget, args, 1);
    XtSetValues(menuItems[SV_FILE_SAVE_AS].widget, args, 1);
    
    // pickAll (if no children, nothing to pick)
    XtSetValues(menuItems[SV_EDIT_PICK_ALL].widget, args, 1);


    // enable/disable based on the number of selected objects
    if (selection->getNumSelected() == 0)
	 XtSetArg(args[0], XmNsensitive, False);
    else XtSetArg(args[0], XmNsensitive, True);
    
    // if nothing selected, then cannot pick parent, cut, copy, delete,
    // view selection, bring up editors
    XtSetValues(menuItems[SV_EDIT_PICK_PARENT].widget, args, 1);
    XtSetValues(menuItems[SV_EDIT_CUT].widget, args, 1);
    XtSetValues(menuItems[SV_EDIT_COPY].widget, args, 1);
    XtSetValues(menuItems[SV_EDIT_DELETE].widget, args, 1);
    XtSetValues(menuItems[SV_VIEW_SELECTION].widget, args, 1);
    XtSetValues(menuItems[SV_EDITOR_TRANSFORM].widget, args, 1);
    XtSetValues(menuItems[SV_EDITOR_MATERIAL].widget, args, 1);
    XtSetValues(menuItems[SV_EDITOR_COLOR].widget, args, 1);
}

////////////////////////////////////////////////////////////////////////
//
//  Called by Xt when a menu is about to be displayed.
//  This gives us a chance to update any items in the menu.
//
//  Use: static private
//
void
SoSceneViewer::menuDisplay(Widget, SoSceneViewerData *data, XtPointer)
//
////////////////////////////////////////////////////////////////////////
{
    SoSceneViewer *sv = data->classPt;
    Arg args[1];
    char str[100];
    XmString xmstr;
    
    switch (data->id) {
	case SV_FILE:
	    // disable saving if there isn't any geometry
	    if (sv->selection->getNumChildren() == 0)
	    	XtSetArg(args[0], XmNsensitive, False);
	    else 
	    	XtSetArg(args[0], XmNsensitive, True);
	    
	    XtSetValues(sv->menuItems[SV_FILE_SAVE].widget, args, 1);
	    XtSetValues(sv->menuItems[SV_FILE_SAVE_AS].widget, args, 1);
	    
	    // update the "Save" menu entry to reflect the current file name
#ifndef EXPLORER 
	    strcpy(str,fileData[5].name );
#else
	    strcpy(str,fileData[0].name );
#endif
	    if (sv->fileName != NULL) {
		// get the file name withought the entire path
		char *pt = strrchr(sv->fileName, '/');   // last occurance of '/'
		pt = (pt == NULL) ? sv->fileName : pt + 1;
		strcat(str, " -> ");
		strcat(str, pt);
	    }
	    xmstr = XmStringCreate(str, XmSTRING_DEFAULT_CHARSET);
	    XtSetArg(args[0], XmNlabelString, xmstr);
	    XtSetValues(sv->menuItems[SV_FILE_SAVE].widget, args, 1);
	    XmStringFree(xmstr);
	    break;
	    
#ifndef EXPLORER
    	case SV_EDIT:
	    // disable cut, copy, delete, pickParent if there is no selection
	    if(sv->selection->getNumSelected() == 0)
	    	XtSetArg(args[0], XmNsensitive, False);
	    else 
	    	XtSetArg(args[0], XmNsensitive, True);
	    
	    XtSetValues(sv->menuItems[SV_EDIT_PICK_PARENT].widget, args, 1);
	    XtSetValues(sv->menuItems[SV_EDIT_CUT].widget, args, 1);
	    XtSetValues(sv->menuItems[SV_EDIT_COPY].widget, args, 1);
	    XtSetValues(sv->menuItems[SV_EDIT_DELETE].widget, args, 1);
	    
	    // disable pick all if there are nothing to pick
	    if (sv->selection->getNumChildren() == 0)
	    	XtSetArg(args[0], XmNsensitive, False);
	    else 
	    	XtSetArg(args[0], XmNsensitive, True);
	    XtSetValues(sv->menuItems[SV_EDIT_PICK_ALL].widget, args, 1);
	    break;
#endif /* EXPLORER */
	    
	case SV_VIEW:
	    // set pick/edit toggle
	    if ( sv->isViewing() )
		 TOGGLE_OFF(sv->menuItems[SV_VIEW_PICK].widget);
	    else TOGGLE_ON(sv->menuItems[SV_VIEW_PICK].widget);
	    
#ifdef EXPLORER
	    // set user pick toggle
	    if ( sv->userModeFlag )
		 TOGGLE_ON(sv->menuItems[SV_VIEW_USER].widget);
	    else TOGGLE_OFF(sv->menuItems[SV_VIEW_USER].widget);
#endif
	    // Set the correct viewer
	    TOGGLE_OFF(sv->menuItems[SV_VIEW_EXAMINER].widget);
	    TOGGLE_OFF(sv->menuItems[SV_VIEW_WALK].widget);
	    TOGGLE_OFF(sv->menuItems[SV_VIEW_PLANE].widget);
	    TOGGLE_OFF(sv->menuItems[SV_VIEW_FLY].widget);
	    switch ( sv->whichViewer ) {
		case SV_VWR_EXAMINER:
		    TOGGLE_ON(sv->menuItems[SV_VIEW_EXAMINER].widget);
		    break;
		case SV_VWR_WALK:
		    TOGGLE_ON(sv->menuItems[SV_VIEW_WALK].widget);
		    break;
		case SV_VWR_FLY:
		    TOGGLE_ON(sv->menuItems[SV_VIEW_FLY].widget);
		    break;
		case SV_VWR_PLANE:
		    TOGGLE_ON(sv->menuItems[SV_VIEW_PLANE].widget);
		    break;
	    }
	    
	    // set the correct transparency type
	    TOGGLE_OFF(sv->menuItems[SV_VIEW_SCREEN_TRANSPARENCY].widget);
	    TOGGLE_OFF(sv->menuItems[SV_VIEW_BLEND_TRANSPARENCY].widget);
	    TOGGLE_OFF(sv->menuItems[SV_VIEW_DELAY_BLEND_TRANSPARENCY].widget);
	    TOGGLE_OFF(sv->menuItems[SV_VIEW_SORT_BLEND_TRANSPARENCY].widget);
	    switch( sv->getTransparencyType() ) {
		case SoGLRenderAction::SCREEN_DOOR:
		    TOGGLE_ON(sv->menuItems[SV_VIEW_SCREEN_TRANSPARENCY].widget);
		    break;
		case SoGLRenderAction::BLEND:
		    TOGGLE_ON(sv->menuItems[SV_VIEW_BLEND_TRANSPARENCY].widget);
		    break;
		case SoGLRenderAction::DELAYED_BLEND:
		    TOGGLE_ON(sv->menuItems[SV_VIEW_DELAY_BLEND_TRANSPARENCY].widget);
		    break;
		case SoGLRenderAction::SORTED_OBJECT_BLEND:
		    TOGGLE_ON(sv->menuItems[SV_VIEW_SORT_BLEND_TRANSPARENCY].widget);
		    break;
	    }
	    
	    // disable view selection if nothing is selected
	    if ( sv->selection->getNumSelected() == 0 )
	    	XtSetArg(args[0], XmNsensitive, False);
	    else 
	    	XtSetArg(args[0], XmNsensitive, True);
	    XtSetValues(sv->menuItems[SV_VIEW_SELECTION].widget, args, 1);
	    
	    // set fog toggle
	    if ( sv->fogFlag )
		 TOGGLE_ON(sv->menuItems[SV_VIEW_FOG].widget);
	    else TOGGLE_OFF(sv->menuItems[SV_VIEW_FOG].widget);
	    
	    // set antialiasing toggle
	    if ( sv->antialiasingFlag )
		 TOGGLE_ON(sv->menuItems[SV_VIEW_ANTIALIASING].widget);
	    else TOGGLE_OFF(sv->menuItems[SV_VIEW_ANTIALIASING].widget);
	    
	    break;
	    
#ifndef EXPLORER
    	case SV_SELECTION:
	    // mirror the selection policy
	    TOGGLE_OFF(sv->menuItems[SV_SEL_SINGLE_SELECT].widget);
	    TOGGLE_OFF(sv->menuItems[SV_SEL_TOGGLE_SELECT].widget);
	    TOGGLE_OFF(sv->menuItems[SV_SEL_SHIFT_SELECT].widget);
	    switch ( sv->selection->policy.getValue() ) {
		case SoSelection::SINGLE:
		    TOGGLE_ON (sv->menuItems[SV_SEL_SINGLE_SELECT].widget);
		    break;
	    	case SoSelection::TOGGLE:
		    TOGGLE_ON (sv->menuItems[SV_SEL_TOGGLE_SELECT].widget);
		    break;
	    	case SoSelection::SHIFT:
		    TOGGLE_ON (sv->menuItems[SV_SEL_SHIFT_SELECT].widget);
		    break;
		default:
		    fprintf(stderr, "INTERNAL ERROR, unknown selection policy\n");
		    break;
	    }
		
	    break;
#endif /* EXPLORER */
	    
    	case SV_EDITOR:
	    // disable items if there is no selection
	    if(sv->selection->getNumSelected() == 0)
	    	XtSetArg(args[0], XmNsensitive, False);
	    else 
	    	XtSetArg(args[0], XmNsensitive, True);
	    
	    XtSetValues(sv->menuItems[SV_EDITOR_TRANSFORM].widget, args, 1);
	    XtSetValues(sv->menuItems[SV_EDITOR_MATERIAL].widget, args, 1);
	    XtSetValues(sv->menuItems[SV_EDITOR_COLOR].widget, args, 1);
	    break;
	    
   	case SV_MANIP:

	    // First, the section with the different types of manipulators.
	    TOGGLE_OFF( sv->menuItems[SV_MANIP_HANDLEBOX].widget );
	    TOGGLE_OFF( sv->menuItems[SV_MANIP_TRANSFORMER].widget );
	    TOGGLE_OFF( sv->menuItems[SV_MANIP_TRACKBALL].widget );
	    TOGGLE_OFF( sv->menuItems[SV_MANIP_JACK].widget );
	    TOGGLE_OFF( sv->menuItems[SV_MANIP_CENTERBALL].widget );
	    TOGGLE_OFF( sv->menuItems[SV_MANIP_XFBOX].widget );
	    TOGGLE_OFF( sv->menuItems[SV_MANIP_TABBOX].widget );
	    TOGGLE_OFF( sv->menuItems[SV_MANIP_NONE].widget );
	    
	    // Turn appropriate radio button on
	    if (sv->curManip == SV_HANDLEBOX)
		TOGGLE_ON(sv->menuItems[SV_MANIP_HANDLEBOX].widget);
	    else if (sv->curManip == SV_TRACKBALL)
		TOGGLE_ON(sv->menuItems[SV_MANIP_TRACKBALL].widget);
	    else if (sv->curManip == SV_TRANSFORMER)
		TOGGLE_ON(sv->menuItems[SV_MANIP_TRANSFORMER].widget);
	    else if (sv->curManip == SV_JACK)
		TOGGLE_ON(sv->menuItems[SV_MANIP_JACK].widget);
	    else if (sv->curManip == SV_CENTERBALL)
		TOGGLE_ON(sv->menuItems[SV_MANIP_CENTERBALL].widget);
	    else if (sv->curManip == SV_XFBOX)
		TOGGLE_ON(sv->menuItems[SV_MANIP_XFBOX].widget);
	    else if (sv->curManip == SV_TABBOX)
		TOGGLE_ON(sv->menuItems[SV_MANIP_TABBOX].widget);
	    else 
		TOGGLE_ON(sv->menuItems[SV_MANIP_NONE].widget);

	    // Next, the toggle that says whether we replace current 
	    // manipulators every time we change the type given in the menu.
	    if (sv->curManipReplaces == TRUE ) 
		TOGGLE_ON(  sv->menuItems[SV_MANIP_REPLACE_ALL].widget );
	    else
		TOGGLE_OFF( sv->menuItems[SV_MANIP_REPLACE_ALL].widget );

	    break;
	    
	case SV_LIGHT:
	    // disable the add light entries if we have more than 8 lights
	    if (sv->lightDataList.getLength() < 8)
	    	XtSetArg(args[0], XmNsensitive, True);
	    else 
	    	XtSetArg(args[0], XmNsensitive, False);
	    
	    XtSetValues(sv->menuItems[SV_LIGHT_ADD_DIRECT].widget, args, 1);
	    XtSetValues(sv->menuItems[SV_LIGHT_ADD_POINT].widget, args, 1);
	    XtSetValues(sv->menuItems[SV_LIGHT_ADD_SPOT].widget, args, 1);
	    
	    // update the headlight label (show on/off with '*')
	    sv->isHeadlight() ? strcpy(str, "* ") : strcpy(str, "  ");
	    strcat(str, sv->headlightData->name);
	    xmstr = XmStringCreate(str, XmSTRING_DEFAULT_CHARSET);
	    XtSetArg(args[0], XmNlabelString, xmstr);
	    XtSetValues(sv->headlightData->cascadeWidget, args, 1);
	    XmStringFree(xmstr);
	    
	    // update the lights label (show on/off with '*')
	    {
		for (int i=0; i < sv->lightDataList.getLength(); i++) {
		    SvLightData *data = (SvLightData *) sv->lightDataList[i];
		    IS_LIGHT_ON(data->lightSwitch) ? strcpy(str, "* ") : strcpy(str, "  ");
		    strcat(str, data->name);
		    xmstr = XmStringCreate(str, XmSTRING_DEFAULT_CHARSET);
		    XtSetArg(args[0], XmNlabelString, xmstr);
		    XtSetValues(data->cascadeWidget, args, 1);
		    XmStringFree(xmstr);
		}
	    }
	    break;
	    
	default:
	    break;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Determines whether a given node is affected by a transform.
//
// Use: static, public
//
SbBool
SoSceneViewer::isAffectedByTransform(
	SoNode *theNode )		// node to be affected?
//
////////////////////////////////////////////////////////////////////////
{
    if ( theNode->isOfType( SoGroup::getClassTypeId() )
	    || theNode->isOfType( SoShape::getClassTypeId() )
	    || theNode->isOfType( SoCamera::getClassTypeId() )
	    || theNode->isOfType( SoLight::getClassTypeId() ) )  {
	return TRUE;
    }
    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Determines whether a given node is affected by material node.
//
// Use: static, public
//
SbBool
SoSceneViewer::isAffectedByMaterial(
	SoNode *theNode )		// node to be affected?
//
////////////////////////////////////////////////////////////////////////
{
    if ( theNode->isOfType( SoGroup::getClassTypeId() )
	    || theNode->isOfType( SoShape::getClassTypeId() ) ) {
	return TRUE;
    }
    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Create the lights and camera environment structure.
//
// Use: private
//
void
SoSceneViewer::createLightsCameraEnvironment()
//
////////////////////////////////////////////////////////////////////////
{
    // Group {
    //	  Label { "SoSceneViewer Environment v3.0" }
    //    Camera {}
    //    Environment {}
    //    Group {
    //        Switch { Light 1 }    	# switch is child 0, light is child 0
    //        Switch { Light 2 }    	# switch is child 1, light is child 0
    //        ...
    //    }
    // }
    //
    // NOTE: since the camera may be switched by the viewer (ortho/perspective toggle)
    // make sure to get the camera from the viewer (and not cache the camera).
    
    lightsCameraEnvironment = new SoGroup;
    environment     	    = new SoEnvironment;
    lightGroup 	    	    = new SoGroup;
    envLabel	  	    = new SoLabel;

    envLabel->label.setValue(SV_ENV_LABEL);
    lightsCameraEnvironment->addChild(envLabel);
#ifndef EXPLORER
    // Explorer SceneViewer doesn't remove cameras from the Scene, so
    // don't add any...
    lightsCameraEnvironment->addChild(new SoPerspectiveCamera);
#endif
    lightsCameraEnvironment->addChild(environment);
    lightsCameraEnvironment->addChild(lightGroup);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Brings up the "About..." dialog (same code as gavin demo programs)
//
// Use: private
//
void
SoSceneViewer::showAboutDialog()
//
////////////////////////////////////////////////////////////////////////
{
    if (access(IVPREFIX "/demos/Inventor/SceneViewer.about", R_OK) != 0)
    {
	system("xmessage 'Sorry, could not find "
	       IVPREFIX "/demos/Inventor/SceneViewer.about' > /dev/null");
	return;
    }

    char command[100];
    sprintf(command, "which acroread > /dev/null");

    int err = system(command);
    if (err)
    {
	system("xmessage 'You must install acroread"
	       " for this function to work' > /dev/null");
	return;
    }

    sprintf(command, "acroread "
            IVPREFIX "/demos/Inventor/SceneViewer.about &");
    system(command);
}

//
// define those generic virtual functions
//
const char *
SoSceneViewer::getDefaultWidgetName() const
{ return "SoSceneViewer"; }

const char *
SoSceneViewer::getDefaultTitle() const
{ return "SceneViewer"; }

const char *
SoSceneViewer::getDefaultIconTitle() const
{ return "SceneViewer"; }

