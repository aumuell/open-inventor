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
 |   Classes    : SoXtViewer
 |
 |   Author(s)	: Alain Dumesny
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <inttypes.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

#ifdef __sgi
#include <X11/extensions/SGIStereo.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <Inventor/SbBox.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SbPList.h>
#include <Inventor/SbViewportRegion.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoPath.h>
#include <Inventor/SoPath.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodes/SoPackedColor.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoResetTransform.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoLocateHighlight.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include <Inventor/fields/SoSFTime.h>
#include <Inventor/Xt/devices/SoXtInputFocus.h>
#include <Inventor/Xt/SoXtClipboard.h>
#include <Inventor/Xt/viewers/SoXtViewer.h>

// keep a pointer to global time, since we are going to access it a lot.
SoSFTime *SoXtViewer::viewerRealTime = NULL;


////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
// Use: protected

SoXtViewer::SoXtViewer(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent,
    SoXtViewer::Type t, 
    SbBool buildNow) 
	: SoXtRenderArea(
	    parent,
	    name, 
	    buildInsideParent, 
	    TRUE,   // getMouseInput
	    TRUE,   // getKeyboardInput
	    FALSE)  // buildNow
//
////////////////////////////////////////////////////////////////////////
{
    // init local vars
    type = t;
    camera = NULL;
    cameraType = SoPerspectiveCamera::getClassTypeId();
    createdCamera = FALSE;
    viewingFlag = TRUE;
    altSwitchBack = FALSE;
    cursorEnabledFlag = TRUE;
    interactiveFlag = FALSE;
    startCBList = new SoCallbackList;
    finishCBList = new SoCallbackList;
    interactiveCount = 0;
    bufferType = isDoubleBuffer() ? BUFFER_DOUBLE : BUFFER_SINGLE;
    stereoOffset = 3.0;
#ifdef __sgi
    useSGIStereoExt = FALSE;
#endif
    sceneSize = 0.0;	// not computed yet.
    viewerSpeed = 1.0;	// default. SoXtFullViewer add UI to increase/decrease 
    
    // add Enter and Leave notify events for the viewers
    inputFocus = new SoXtInputFocus;
    registerDevice(inputFocus);
    
    if (! viewerRealTime)
	viewerRealTime = (SoSFTime *) SoDB::getGlobalField("realTime");
    
    // init auto clipping stuff
    autoClipFlag = TRUE;
    minimumNearPlane = 0.001;
    autoClipBboxAction = new SoGetBoundingBoxAction(SbVec2s(1,1));  // ??? no valid size yet
    
    // copy/paste support
    clipboard = NULL;
    
    // init seek animation variables
    seekDistance = 50.0;
    seekDistAsPercentage = TRUE;
    seekModeFlag = FALSE;
    detailSeekFlag = TRUE;
    seekAnimTime = 2.0;
    seekAnimationSensor = new SoFieldSensor(SoXtViewer::seekAnimationSensorCB, this);
    
    //
    // build the small internal graph (nodes used for draw style stuff)
    //
    sceneRoot		= new SoSeparator(4);
    drawStyleSwitch	= new SoSwitch(6);
    drawStyleNode   	= new SoDrawStyle;
    lightModelNode  	= new SoLightModel;
    colorNode		= new SoPackedColor;
    matBindingNode	= new SoMaterialBinding;
    complexityNode	= new SoComplexity;
    sceneGraph 	    	= NULL;
    
    // note: we cannot setSceneGraph on the renderArea in the constructor
    // since it calls virtual functions, and all of our members aren't
    // initialized yet. We'll call it the first time our setSceneGraph
    // is called.
    sceneRoot->ref();
    sceneRoot->renderCaching.setValue(SoSeparator::OFF); // no caching there
    sceneRoot->renderCulling.setValue(SoSeparator::OFF); // no culling there
    sceneRoot->addChild(drawStyleSwitch);
    drawStyleSwitch->addChild(drawStyleNode);
    drawStyleSwitch->addChild(lightModelNode);
    drawStyleSwitch->addChild(colorNode);
    drawStyleSwitch->addChild(matBindingNode);
    drawStyleSwitch->addChild(complexityNode);
    
    // set the draw style vars and fields that don't change - once we
    // have a context, will will use glGetString() to pick a better default
    // draw style.
    stillDrawStyle = VIEW_AS_IS;
    interactiveDrawStyle = VIEW_SAME_AS_STILL;
    checkForDrawStyle = TRUE;
    drawStyleSwitch->whichChild = SO_SWITCH_NONE;
    
    drawStyleNode->setOverride(TRUE); // only use style field
    drawStyleNode->pointSize = 3.0;
    drawStyleNode->lineWidth.setIgnored(TRUE);
    drawStyleNode->linePattern.setIgnored(TRUE);
    
    lightModelNode->setOverride(TRUE);
    
    colorNode->setOverride(TRUE);
    
    matBindingNode->setOverride(TRUE);
    matBindingNode->value = SoMaterialBinding::OVERALL;
    
    complexityNode->setOverride(TRUE);
    complexityNode->textureQuality = 0; // always turn texture off under switch
    complexityNode->value = 0.15;
    
    addStartCallback(SoXtViewer::drawStyleStartCallback);
    addFinishCallback(SoXtViewer::drawStyleFinishCallback);
    
    //
    // headlightGroup - we have a rotation which keeps the headlight
    // moving whenever the camera moves,  and a reset xform so
    // that the rest of the scene is not affected by the first rot.
    // these leaves the direction field in the headlight open for the
    // user to edit, allowing for the direction to change w.r.t. the camera.
    //
    headlightGroup  = new SoGroup(3);
    headlightRot    = new SoRotation;
    headlightNode   = new SoDirectionalLight;
    headlightGroup->ref();
    headlightGroup->addChild(headlightRot);
    headlightGroup->addChild(headlightNode);
    headlightGroup->addChild(new SoResetTransform);
    headlightNode->direction.setValue(SbVec3f(.2, -.2, -.9797958971));
    headlightFlag = TRUE;
    
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
// Use: protected

SoXtViewer::~SoXtViewer()
//
////////////////////////////////////////////////////////////////////////
{
    // detach everything
    if ( sceneGraph != NULL )
	setSceneGraph(NULL);
    sceneRoot->unref();
    
    // delete everything
    delete inputFocus;
    delete seekAnimationSensor;
    delete clipboard;
    delete autoClipBboxAction;
    delete startCBList;
    delete finishCBList;
    headlightGroup->unref();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  	Set a new user supplied scene graph.
//
// use: virtual public
//
void
SoXtViewer::setSceneGraph(SoNode *newScene)
//
////////////////////////////////////////////////////////////////////////
{
    // if we haven't already given the render area a scene graph sceneRoot,
    // give it the scene graph now. This is a one shot deal, which
    // cannot be done in the constructor.
    if (SoXtRenderArea::getSceneGraph() == NULL)
	SoXtRenderArea::setSceneGraph(sceneRoot);
    
    // draw new scene graphs to the front buffer by default since
    // the scene will be different (we might has well see something
    // happening for the first redraw).
    if (isDrawToFrontBufferEnable())
	drawToFrontBuffer = TRUE;
    
    //
    // detach everything that depends on the old sceneGraph
    //
    if ( sceneGraph != NULL ) {
	setCamera(NULL);
	sceneRoot->removeChild(sceneGraph);
    }
    
    sceneGraph = newScene;
    
    //
    // now assign the new sceneGraph, find or create the new camera 
    // and attach things back.
    //
    if ( sceneGraph != NULL ) {
	sceneRoot->addChild(sceneGraph);
	
	// search for first camera in the scene
	SoSearchAction sa;
	sa.setType(SoCamera::getClassTypeId());
	sa.setSearchingAll(FALSE); // don't look under off switches
	sa.apply(sceneGraph);
	
	SoCamera *newCamera = NULL;
	if (sa.getPath())
	    newCamera = (SoCamera *)((SoFullPath *)sa.getPath())->getTail();
	
	// if no camera found create one of the right kind...
	if ( newCamera == NULL ) {
	    
	    newCamera = (SoCamera*) cameraType.createInstance();
	    if (newCamera == NULL) {
#ifdef DEBUG
		SoDebugError::post("SoXtViewer::setSceneGraph",
		    "unknown camera type!");
#endif
		// ??? what should we do here ?
		cameraType = SoPerspectiveCamera::getClassTypeId();
		newCamera = new SoPerspectiveCamera;
	    }
	    createdCamera = TRUE;
	    
	    if (type == SoXtViewer::BROWSER)
		// add camera after drawstyle stuff
		sceneRoot->insertChild(newCamera, 1);
	    else {
		// check to make sure scene starts with at least a group node
		if ( sceneGraph->isOfType(SoGroup::getClassTypeId()) )
		    ((SoGroup *)sceneGraph)->insertChild(newCamera, 0);
		else {
		    // make scene start with a group node
		    SoGroup *group = new SoGroup;
		    group->addChild(newCamera);
		    group->addChild(sceneGraph);
		    sceneRoot->addChild(group);
		    sceneRoot->removeChild(sceneGraph);
		    sceneGraph = group;
		}
	    }
	    
	    newCamera->viewAll(sceneGraph, SbViewportRegion(getGlxSize()));
	}
	
	setCamera(newCamera);
    }
    
    // recompute the scene size variables...
    recomputeSceneSize();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  	Recomputes the scene sizes...
//
// use: virtual public

void
SoXtViewer::recomputeSceneSize()
//
////////////////////////////////////////////////////////////////////////
{
    if (! sceneGraph || ! sceneRoot) {
	sceneSize = 0.0;
	return;
    }
    
    // Use assignment notation to disambiguate from expression (edison)
    SoGetBoundingBoxAction bboxAct = SoGetBoundingBoxAction(SbViewportRegion(getGlxSize()));
    bboxAct.apply(sceneRoot);
    SbBox3f bbox = bboxAct.getBoundingBox();
    
    if (bbox.isEmpty()) {
	sceneSize = 0.0;
	return;
    }
    
    float x, y, z;
    bbox.getSize(x, y, z);
    sceneSize = (x > z) ? x : z;
    if (y > sceneSize)
	sceneSize = y;
    if (sceneSize <= 0.0)
	sceneSize = 0.0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  	Return the user supplied scene graph.
//
// use: public, virtual

SoNode *
SoXtViewer::getSceneGraph()
//
////////////////////////////////////////////////////////////////////////
{
    return sceneGraph;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the camera to use.
//
// Use: virtual public
void
SoXtViewer::setCamera(SoCamera *newCamera)
//
////////////////////////////////////////////////////////////////////////
{
    // check for trivual return
    if (camera == newCamera)
	return;
    
    //
    // detach everything that depended on the old camera
    //
    if ( camera != NULL ) {
	
        if (headlightFlag) {
	    setHeadlight(FALSE);
	    headlightFlag = TRUE;  // can later be turned on
        }
    	
	if (viewingFlag) {
	    setViewing(FALSE);
	    viewingFlag = TRUE;  // can later be turned on
	}
	
	// remove the camera if we created one outside of the
	// scene graph.
    	if (createdCamera && type == SoXtViewer::BROWSER) {
	    if (sceneRoot->findChild(camera) >= 0)
    		sceneRoot->removeChild(camera);
	    createdCamera = FALSE;
	}
	
    	camera->unref();
    }
    
    camera = newCamera;
    
    //
    // attach everything that depends on the new camera
    //
    if ( camera != NULL) {
	camera->ref();
	
	if (headlightFlag) {
	    headlightFlag = FALSE;  // enables the routine to be called
	    setHeadlight(TRUE);
	}
	
	if (viewingFlag) {
	    viewingFlag = FALSE;  // enables the routine to be called
	    setViewing(TRUE);
	}
	
	saveHomePosition();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Set the camera type to create.
//
// Use: virtual public
void
SoXtViewer::setCameraType(SoType type)
//
////////////////////////////////////////////////////////////////////////
{
    if (type.isDerivedFrom(SoPerspectiveCamera::getClassTypeId()) ||
	type.isDerivedFrom(SoOrthographicCamera::getClassTypeId()))
	cameraType = type;
#ifdef DEBUG
    else
	SoDebugError::post("SoXtViewer::setCameraType",
			"unknown camera type!");
#endif
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    See the whole scene from the camera
//
// Use: public
void
SoXtViewer::viewAll()
//
////////////////////////////////////////////////////////////////////////
{
    if ( camera != NULL )
	camera->viewAll(sceneGraph,SbViewportRegion(getGlxSize()));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Sets the viewing mode.
//
// Use: virtual public
void
SoXtViewer::setViewing(SbBool flag)
//
////////////////////////////////////////////////////////////////////////
{
    if (flag == viewingFlag)
	return;
    
    viewingFlag = flag;
    
    // if we are goind into viewing mode, then de-highlight any
    // currently highlighted nodes (since the object will never receive
    // any motion events).
    if (viewingFlag) {
	SoGLRenderAction *glAct = getGLRenderAction();
	if (glAct)
	    SoLocateHighlight::turnOffCurrentHighlight(glAct);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Enables/Disable the viewer cursor on the window.
//
// Use: virtual public
void
SoXtViewer::setCursorEnabled(SbBool flag)
//
////////////////////////////////////////////////////////////////////////
{
    cursorEnabledFlag = flag;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Sets the auto clipping mode
//
// Use: public
void
SoXtViewer::setAutoClipping(SbBool flag)
//
////////////////////////////////////////////////////////////////////////
{
    if (autoClipFlag == flag)
	return;
    
    autoClipFlag = flag;
    
    // cause a redraw to correctly place the near and far plane now that
    // auto clipping is on.
    if (autoClipFlag)
	scheduleRedraw();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	sets stereo mode
//
// Use: virtual public
void
SoXtViewer::setStereoViewing(SbBool flag)
//
////////////////////////////////////////////////////////////////////////
{
    if (flag == isStereoViewing())
	return;
    
    // First, check to see if the OpenGL stereo visual can be created
    setStereoBuffer(flag);
    
#ifdef __sgi
    // since OpenGL stereo failed, see if the SGI extension will work
    // by checking whether the X server supports it....
    int first_event, first_error;
    if (flag != isStereoViewing() && 
	XSGIStereoQueryExtension(getDisplay(), &first_event, &first_error)) {
	
	if (flag) {
	    // make sure the current window will support stereo
	    // ??? if we havn't been managed yet, just assume this visual
	    // ??? will support stereo viewing (see bug 
	    if (! getNormalWindow())
		useSGIStereoExt = TRUE;
	    else if (XSGIQueryStereoMode(getDisplay(), getNormalWindow()) !=
		X_STEREO_UNSUPPORTED)
		// stereo will be turned on in the rendering....
		useSGIStereoExt = TRUE;
	    
	    // save the camera original aspect ratio since it will be updated
	    // during rendering to strech the objects. We will restore it
	    // when stereo if OFF.
	    camStereoOrigAspect = camera->aspectRatio.getValue();
	    camStereoOrigVPMapping = camera->viewportMapping.getValue();
	}
	else {
	    // turn stereo off on the window
	    useSGIStereoExt = FALSE;
	    
	    // clear the left/right buffers to prevent gost images from
	    // the other view...(until the user resets the monitor with setmon)
	    if (isRGBMode()) {
		SbColor color = getBackgroundColor();
		glClearColor(color[0], color[1], color[2], 0);
	    }
	    else
		glClearIndex(getBackgroundIndex());
	    
	    glDrawBuffer(GL_FRONT_AND_BACK);
	    
	    XSGISetStereoBuffer(getDisplay(), getNormalWindow(), STEREO_BUFFER_LEFT);
	    XSync(getDisplay(), False);
	    glClear(GL_COLOR_BUFFER_BIT);
	    
	    XSGISetStereoBuffer(getDisplay(), getNormalWindow(), STEREO_BUFFER_RIGHT);
	    XSync(getDisplay(), False);
	    glClear(GL_COLOR_BUFFER_BIT);
	    
	    glDrawBuffer( isDoubleBuffer() ? GL_BACK : GL_FRONT);
	    
	    // restore the camera original aspect ratio (saved above)
	    camera->aspectRatio = camStereoOrigAspect;
	    camera->viewportMapping = camStereoOrigVPMapping;
	}
	
	// now cause a redraw to see the affect since we havn't changed
	// the actual visual (unlike OpenGL)
	if (flag == isStereoViewing())
	    scheduleRedraw();
    }
#endif
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	gets stereo mode
//
// Use: virtual public
SbBool
SoXtViewer::isStereoViewing()
//
////////////////////////////////////////////////////////////////////////
{
#ifdef __sgi
    return (isStereoBuffer() || useSGIStereoExt);
#else
    // done in SoXtGLWidget
    return isStereoBuffer();
#endif
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    adds a directional light to the scene graph that is a headlight
//    positioned over the left shoulder of the camera.  It has a sensor
//    on the camera, so that it always is pointing in the same direction
//    as the camera.  The sensor is not delayed, so that it is "always"
//    accurate.
//
// Use: virtual public
//
void
SoXtViewer::setHeadlight(SbBool insertFlag)
//
////////////////////////////////////////////////////////////////////////
{
    // check for trivial return
    if (camera == NULL || headlightFlag == insertFlag) {
	headlightFlag = insertFlag;
	return;
    }
    
    //
    // find the camera parent to insert/remove the headlight
    //
    SoSearchAction sa;
    if (insertFlag)
	sa.setNode(camera);
    else {
	sa.setNode(headlightGroup);
	sa.setSearchingAll(TRUE); // find under OFF switches for removal
    }
    sa.apply(sceneRoot);
    SoFullPath *fullPath = (SoFullPath *) sa.getPath();
    if (!fullPath) {
#if DEBUG
	SoDebugError::post("SoXtViewer::setHeadlight",
			    insertFlag ? "ERROR: cannot find camera in graph" :
			    "ERROR: cannot find headlight in graph");
#endif
	return;
    }
    SoGroup *parent = (SoGroup *) fullPath->getNodeFromTail(1);
    
    headlightFlag = insertFlag;
    
    //
    // inserts/remove the headlight group node
    //
    if (headlightFlag) {
	int camIndex;
	
	// check to make sure that the camera parent is not a switch node
	// (VRML camera viewpoints are kept under a switch node). Otherwise
	// we will insert the headlight right before the switch node.
	if (parent->isOfType(SoSwitch::getClassTypeId())) {
	    SoNode *switchNode = parent;
	    parent = (SoGroup *) fullPath->getNodeFromTail(2);
	    camIndex = parent->findChild(switchNode);
	}
	else
	    camIndex = parent->findChild(camera);
	
	// return if headlight is already there (this should be an error !)
	if (parent->findChild(headlightGroup) >= 0)
	    return;
	
	// insert the light group right after the camera
	if (camIndex >= 0)
	    parent->insertChild(headlightGroup, camIndex+1);
    }
    else {
	if (parent->findChild(headlightGroup) >= 0)
	    parent->removeChild(headlightGroup);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   	Sets the drawing style.
//
// Use: virtual public
void
SoXtViewer::setDrawStyle(
	    SoXtViewer::DrawType type, SoXtViewer::DrawStyle style)
//
////////////////////////////////////////////////////////////////////////
{
    // prevent us from picking a default draw style if the user already
    // has done so...
    checkForDrawStyle = FALSE;
    
    if (type == STILL) {
	if (stillDrawStyle == style)
	    return;
	if (style == VIEW_SAME_AS_STILL) {
#ifdef DEBUG
	    SoDebugError::post("SoXtViewer::setDrawStyle", 
		"illegal VIEW_SAME_AS_STILL draw style passed for STILL !");
#endif
	    return; 
	}
	stillDrawStyle = style;
	
	if (! interactiveFlag || interactiveDrawStyle == VIEW_SAME_AS_STILL
	    || (interactiveDrawStyle == VIEW_NO_TEXTURE && style != VIEW_AS_IS))
		    setCurrentDrawStyle(style);
	else if (interactiveFlag && interactiveDrawStyle == VIEW_NO_TEXTURE && 
	    style == VIEW_AS_IS)
		    setCurrentDrawStyle(interactiveDrawStyle);
    }
    else {
	// else it type == INTERACTIVE
	
	if (interactiveDrawStyle == style)
	    return;
	interactiveDrawStyle = style;
	
	if (interactiveFlag) {
	    if (style == VIEW_SAME_AS_STILL || 
	       (style == VIEW_NO_TEXTURE && stillDrawStyle != VIEW_AS_IS))
		setCurrentDrawStyle(stillDrawStyle);
	    else
		setCurrentDrawStyle(style);
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   	Sets the current drawing style. This only changes the nodes to
//  match what is passed (called from multiple places) and doesn't
//  affect the current state.
//
// Use: private
void
SoXtViewer::setCurrentDrawStyle(SoXtViewer::DrawStyle style)
//
////////////////////////////////////////////////////////////////////////
{
    if (style != VIEW_AS_IS)
	drawStyleSwitch->whichChild = SO_SWITCH_ALL;
    
    switch(style) {
	case VIEW_AS_IS:
	    drawStyleSwitch->whichChild = SO_SWITCH_NONE;
	    break;
	    
	case VIEW_HIDDEN_LINE:
	    // texture is always off under the switch node.
	    // List only stuff common to both rendering passes
	    // (the rest is done when rendering)
	    drawStyleNode->style.setIgnored(FALSE);
	    drawStyleNode->pointSize.setIgnored(TRUE);
	    lightModelNode->model = SoLightModel::BASE_COLOR;
	    lightModelNode->model.setIgnored(FALSE);
	    complexityNode->type.setIgnored(TRUE);
	    complexityNode->value.setIgnored(TRUE);
	    break;
	    
	case VIEW_NO_TEXTURE:
	case VIEW_LOW_COMPLEXITY:
	    // texture is always off under the switch node
	    drawStyleNode->style.setIgnored(TRUE);
	    drawStyleNode->pointSize.setIgnored(TRUE);
	    lightModelNode->model.setIgnored(TRUE);
	    colorNode->orderedRGBA.setIgnored(TRUE);
	    matBindingNode->value.setIgnored(TRUE);
	    complexityNode->type.setIgnored(TRUE);
	    complexityNode->value.setIgnored(style != VIEW_LOW_COMPLEXITY);
	    break;
	    
	case VIEW_LINE:
	case VIEW_LOW_RES_LINE:
	case VIEW_POINT:
	case VIEW_LOW_RES_POINT:
	    // texture is always off under the switch node
	    drawStyleNode->style = (style == VIEW_LINE || style == VIEW_LOW_RES_LINE) ? 
		SoDrawStyle::LINES : SoDrawStyle::POINTS;
	    drawStyleNode->style.setIgnored(FALSE);
	    drawStyleNode->pointSize.setIgnored(style != VIEW_POINT && style != VIEW_LOW_RES_POINT);
	    lightModelNode->model = SoLightModel::BASE_COLOR;
	    lightModelNode->model.setIgnored(FALSE);
	    colorNode->orderedRGBA.setIgnored(TRUE);
	    matBindingNode->value.setIgnored(TRUE);
	    
	    // Force a lower complexity for the low res draw styles
	    // ??? this only works if the object didn't have
	    // ??? something lower in the first place...
	    if (style == VIEW_LOW_RES_LINE || style == VIEW_LOW_RES_POINT) {
		complexityNode->type = SoComplexity::OBJECT_SPACE;
		complexityNode->type.setIgnored(FALSE);
		complexityNode->value.setIgnored(FALSE);
	    }
	    else {
		complexityNode->type.setIgnored(TRUE);
		complexityNode->value.setIgnored(TRUE);
	    }
	    break;
	    
	case VIEW_BBOX:
	    // texture is always off under the switch node
	    drawStyleNode->style = SoDrawStyle::LINES;
	    drawStyleNode->style.setIgnored(FALSE);
	    drawStyleNode->pointSize.setIgnored(TRUE);
	    lightModelNode->model = SoLightModel::BASE_COLOR;
	    lightModelNode->model.setIgnored(FALSE);
	    colorNode->orderedRGBA.setIgnored(TRUE);
	    matBindingNode->value.setIgnored(TRUE);
	    complexityNode->type = SoComplexity::BOUNDING_BOX;
	    complexityNode->type.setIgnored(FALSE);
	    complexityNode->value.setIgnored(TRUE);
	    break;
	    
	case VIEW_SAME_AS_STILL:
#ifdef DEBUG
	    SoDebugError::post("SoXtViewer::setCurrentDrawStyle", "VIEW_SAME_AS_STILL was passed !");
#endif
	    break;
    }
    
    setZbufferState();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   	Gets the drawing style.
//
// Use: public
SoXtViewer::DrawStyle
SoXtViewer::getDrawStyle(SoXtViewer::DrawType type)
//
////////////////////////////////////////////////////////////////////////
{
    return (type == STILL ? stillDrawStyle : interactiveDrawStyle);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   	redefine this routine to also correctly set the buffering type
//
// Use: virtual public
void
SoXtViewer::setNormalVisual(XVisualInfo *vis)
//
////////////////////////////////////////////////////////////////////////
{
    // call parent class
    SoXtRenderArea::setNormalVisual(vis);
    
    // now update the buffering type
    if (isDoubleBuffer())
	setBufferingType(BUFFER_DOUBLE);
    else
	setBufferingType(BUFFER_SINGLE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	redefine this routine from SoXtGLWidget to call the viewer
//  SoXtViewer::setBufferingType() method which is a superset.
//
//  Use: virtual public
//
void
SoXtViewer::setDoubleBuffer(SbBool flag)
//
////////////////////////////////////////////////////////////////////////
{
    setBufferingType(flag ? SoXtViewer::BUFFER_DOUBLE : SoXtViewer::BUFFER_SINGLE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   	Sets the buffering style.
//
// Use: virtual public
void
SoXtViewer::setBufferingType(SoXtViewer::BufferType type)
//
////////////////////////////////////////////////////////////////////////
{
    if (bufferType == type)
    	return;
    
    // remove interactive callback
    if (bufferType == BUFFER_INTERACTIVE) {
	removeStartCallback(SoXtViewer::bufferStartCallback);
	removeFinishCallback(SoXtViewer::bufferFinishCallback);
    }
    
    bufferType = type;
    
    switch(bufferType) {
	case BUFFER_SINGLE:
	    SoXtRenderArea::setDoubleBuffer(FALSE);
	    break;
	case BUFFER_DOUBLE:
	    SoXtRenderArea::setDoubleBuffer(TRUE);
	    break;
	case BUFFER_INTERACTIVE:
	    SoXtRenderArea::setDoubleBuffer(FALSE);
	    addStartCallback(SoXtViewer::bufferStartCallback);
	    addFinishCallback(SoXtViewer::bufferFinishCallback);
	    break;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Externally set the viewer into/out off seek mode (default OFF). Actual
//  seeking will not happen until the viewer decides to (ex: mouse click).
//
//  Note: setting the viewer out of seek mode while the camera is being
//  animated will stop the animation to the current location.
//
// use: virtual protected

void
SoXtViewer::setSeekMode(SbBool flag)
//
////////////////////////////////////////////////////////////////////////
{
    if (!isViewing())
	return;
    
    // check if seek is being turned off while seek animation is happening
    if ( !flag && seekAnimationSensor->getAttachedField() ) {
	seekAnimationSensor->detach();
	seekAnimationSensor->unschedule();
	interactiveCountDec();
    }
    
    seekModeFlag = flag;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Adjust the camera clipping planes before a redraw.
//
// use: virtual protected.

void
SoXtViewer::actualRedraw()
//
////////////////////////////////////////////////////////////////////////
{
    if (isAutoClipping() && ! isStereoViewing())
	adjustCameraClippingPlanes();
    
    // update the headlight if necessary
    if (headlightFlag && camera)
	headlightRot->rotation.setValue(camera->orientation.getValue());
    
    // make sure that we have a valid sceneSize value - but don't compute
    // a new sceneSize value for every redraw since the walking speed should
    // really be constant.
    if (sceneSize == 0.0)
	recomputeSceneSize();
    
    //
    // Check to see if we are in stereo mode, if so draw the scene
    // twice with the camera offseted between the two views, else
    // do a simple redraw.
    //
    
    if ( isStereoViewing() && camera != NULL) {
	
	// Check the camera type, since stereo is different:
	//
	// Ortho camera: setereo is accomplished by simply rorating
	// the camera (around the point of interest) by 6 degree. 
	//
	// Perspective camera: we translate the camera and rotate
	// them to look at the same point of interest (idealy we also would
	// make sure the plane of convergence is exactly the same for
	// both perspective views, unfortunatly we cannot do this with
	// the current symetric view volumes).
	//
	
	// save the camera original values to restore the camera after
	// both views are rendered. This means we will use this in between
	// left and right view for things like picking.
	SbVec3f	    camOrigPos = camera->position.getValue();
	SbRotation  camOrigRot = camera->orientation.getValue();
	
	// get the camera focal point
	SbMatrix mx;
	mx = camOrigRot;
	SbVec3f forward( -mx[2][0], -mx[2][1], -mx[2][2]);
	float radius = camera->focalDistance.getValue();
	SbVec3f center = camOrigPos + radius * forward;
	
#ifdef __sgi
	//
	// if we are splitting the screen in half (loose vertical resolution)
	// then change the aspect ratio to squish the objects to make them 
	// look square again through the stereo glasses. This is done for
	// every redraw since we need to manually update the aspect ourself.
	//
	if (useSGIStereoExt) {
	    SbVec2s windowSize = getGlxSize();
	    camera->aspectRatio = 0.5 * windowSize[0] / (float) windowSize[1];
	    camera->viewportMapping = SoCamera::LEAVE_ALONE;
	}
#endif
	
	//
	// change the camera for the LEFT eye view, and render
	//
#ifdef __sgi
	if (useSGIStereoExt) {
	    XSGISetStereoBuffer(getDisplay(), getNormalWindow(), STEREO_BUFFER_LEFT);
	    XSync(getDisplay(), False);
	}
	else
#endif
	    glDrawBuffer( (isDoubleBuffer() && !drawToFrontBuffer) ? 
						    GL_BACK_LEFT : GL_FRONT_LEFT);
	// rotate the camera by - stereoOffset/2 degrees
	camera->orientation = 
	    SbRotation(SbVec3f(0, 1, 0), - stereoOffset * M_PI / 360.0) * camOrigRot;
	
	// reposition camera to look at pt of interest
	mx = camera->orientation.getValue();
	forward.setValue( -mx[2][0], -mx[2][1], -mx[2][2]);
	camera->position = center - radius * forward;
	
	if (isAutoClipping())
	    adjustCameraClippingPlanes();
	doRendering();
	
	//
	// change the camera for the RIGHT eye view, and render
	//
#ifdef __sgi
	if (useSGIStereoExt) {
	    XSGISetStereoBuffer(getDisplay(), getNormalWindow(), STEREO_BUFFER_RIGHT);
	    XSync(getDisplay(), False);
	}
	else
#endif
	    glDrawBuffer( (isDoubleBuffer() && !drawToFrontBuffer) ? 
						    GL_BACK_RIGHT : GL_FRONT_RIGHT);
	// rotate the camera by + stereoOffset/2 degrees
	camera->orientation = 
	    SbRotation(SbVec3f(0, 1, 0), stereoOffset * M_PI / 360.0) * camOrigRot;
	
	// reposition camera to look at pt of interest
	mx = camera->orientation.getValue();
	forward.setValue( -mx[2][0], -mx[2][1], -mx[2][2]);
	camera->position = center - radius * forward;
	
	if (isAutoClipping())
	    adjustCameraClippingPlanes();
	doRendering();
	
	
	//
	// reset the camera original values now that we are done rendering
	// the stereo views (leave aspect ratio to do correct picking).
	camera->enableNotify(FALSE); // don't cause a redraw
	camera->position = camOrigPos;
	camera->orientation = camOrigRot;
	camera->enableNotify(TRUE);
	
#ifdef __sgi
	if (! useSGIStereoExt)
#endif
	    // restore to draw to both buffer (viewer feedback)
	    glDrawBuffer( (isDoubleBuffer() && !drawToFrontBuffer) ? 
						    GL_BACK : GL_FRONT);
    }
    //
    // else not stereo viewing, so do the regular rendering....
    //
    else
	doRendering();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Do a multiple pass rendering if necessary, else simply call
//  SoXtRenderAre::actualRedraw() method.
//
// use: private
void
SoXtViewer::doRendering()
//
////////////////////////////////////////////////////////////////////////
{
    //
    // check if we need two pass rendering for hidden line rendering
    //
    
    SbBool drawHiddenLine = 
	(stillDrawStyle == VIEW_HIDDEN_LINE && (! interactiveFlag ||
				interactiveDrawStyle == VIEW_NO_TEXTURE ||
				interactiveDrawStyle == VIEW_LOW_COMPLEXITY ||
				interactiveDrawStyle == VIEW_SAME_AS_STILL)) 
	|| (interactiveFlag && interactiveDrawStyle == VIEW_HIDDEN_LINE);
	
    if (camera != NULL && drawHiddenLine) {
	
	// ??? what do we do about highlights ??
	
	// the smaller the near clipping plane is relative to the far
	// plane, the smaller the zbuffer offset needs to be (because
	// the granularity will be pretty big). The closer the clipping
	// planes are relative to each other, the bigger the zbuffer offset
	// needs to be (because the zbuffer granularity will be small).
	// The scale factor was found empirically to work best with the
	// current settings of near/far.
	float zOffset = camera->nearDistance.getValue() / 
	    (40 * camera->farDistance.getValue());
	
	//
	// render the first pass as solid, using the background color
	// for the object base color.
	//
	
	drawStyleNode->style = SoDrawStyle::FILLED;
	colorNode->orderedRGBA = getBackgroundColor().getPackedValue();
	colorNode->orderedRGBA.setIgnored(FALSE);
	matBindingNode->value.setIgnored(FALSE);
	
	// ??? this should match the SoXtRenderArea::actualRedraw()
	// ??? method exactly (apart for not clearing the z-buffer)
	glDepthRange(zOffset, 1); // enable wireframe to be draw on top
	getSceneManager()->render(isClearBeforeRender(), TRUE);
	
	//
	// render the second pass as wireframe
	// (the first pass rendered the objects solid with base color
	// set to the background color to set the zbuffer values)
	//
	
	drawStyleNode->style = SoDrawStyle::LINES;
	colorNode->orderedRGBA.setIgnored(TRUE);
	matBindingNode->value.setIgnored(TRUE);
	
	// ??? this should match the SoXtRenderArea::actualRedraw()
	// ??? method exactly (apart for not clearing the color and z-buffer)
	glDepthRange(0,1-zOffset); // enable wireframe to be draw on top
	getSceneManager()->render(FALSE, FALSE);
	
	glDepthRange(0, 1); // restore the range
    }
    else {
	// ??? this should match the SoXtRenderArea::actualRedraw()
	// ??? method exactly (apart for not clearing the z-buffer)
	getSceneManager()->render(isClearBeforeRender(), ! isZbufferOff());
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	This is called when we are first mapped - this will check the
//  configuration of the machine we are running on and decide on what
//  draw style we should pick.
//
// use: virtual protected

void
SoXtViewer::afterRealizeHook()
//
////////////////////////////////////////////////////////////////////////
{
    // call the base class
    SoXtRenderArea::afterRealizeHook();
    
    // only do this once and only IF the user hasn't overwritten this
    if (! checkForDrawStyle)
	return;
    checkForDrawStyle = FALSE;
    
    SbBool useTexture = TRUE;	// true by default (and for new machines)
    const char *renderer = (const char *) glGetString(GL_RENDERER);
    
    //
    // On the following SGI machines we don't want texture rendering 
    // to be turned on by default (for speed reason) - machines not listed
    // below are assumed to be fast enough to leave VIEW_AS_IS draw style.
    //
    
    if (strncmp((const char *)glGetString(GL_VENDOR), "SGI", 3) == 0) {
	
	// Indy and XL
	if (useTexture && strncmp(renderer, "NEWPORT", 7) == 0)
	    useTexture = FALSE;
	// Personal Iris
	if (useTexture && strncmp(renderer, "GR1", 3) == 0)
	    useTexture = FALSE;
	// VGX and VGXT
	if (useTexture && strncmp(renderer, "VGX", 3) == 0)
	    useTexture = FALSE;
	// Indigo Entry
	if (useTexture && 
	    strncmp(renderer, "LG1", 3) == 0 ||
	    strncmp(renderer, "LIGHT", 5) == 0)
	    useTexture = FALSE;
	// XS, XZ, Elan, and Extreme
	if (useTexture && (
	    strncmp(renderer, "GR2", 3) == 0 || 
	    strncmp(renderer, "GR3", 3) == 0 ||
	    strncmp(renderer, "GU1", 3) == 0))
	    useTexture = FALSE;
    }
    
    if (! useTexture)
	setDrawStyle(SoXtViewer::INTERACTIVE, SoXtViewer::VIEW_NO_TEXTURE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if the zbuffer should be off (based on the viewer
//  draw styles).
//
// Use: private

SbBool
SoXtViewer::isZbufferOff()
//
////////////////////////////////////////////////////////////////////////
{
    DrawStyle style = (interactiveFlag ? interactiveDrawStyle : stillDrawStyle);
    if (interactiveFlag && interactiveDrawStyle == VIEW_SAME_AS_STILL)
	style = stillDrawStyle;
    
    // for these draw styles, turn the zbuffer off
    return (style == VIEW_LOW_RES_LINE || style == VIEW_LOW_RES_POINT 
	|| style == VIEW_BBOX);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the zbuffer state on the current window. This is called whenever
//  the windows changes (called by SoXtGLWidget::widgetChanged()) or when
//  the viewer draw style changes.
//
// Use: private

void
SoXtViewer::setZbufferState()
//
////////////////////////////////////////////////////////////////////////
{
    if (getNormalWindow() == (Window) NULL)
	return;
    
    glXMakeCurrent(getDisplay(), getNormalWindow(), getNormalContext());
    
    if (isZbufferOff())
	glDisable(GL_DEPTH_TEST);
    else
	glEnable(GL_DEPTH_TEST);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    saves the camera values for later restore.
//
// Use: virtual public

void
SoXtViewer::saveHomePosition()
//
////////////////////////////////////////////////////////////////////////
{
    if (camera == NULL)
	return;
    
    origPosition = camera->position.getValue();
    origOrientation = camera->orientation.getValue();
    origNearDistance = camera->nearDistance.getValue();
    origFarDistance = camera->farDistance.getValue();
    origFocalDistance = camera->focalDistance.getValue();
    
    // save camera height (changed by zooming)
    if (camera->isOfType(SoPerspectiveCamera::getClassTypeId()))
	origHeight = ((SoPerspectiveCamera *)camera)->heightAngle.getValue();
    else if (camera->isOfType(SoOrthographicCamera::getClassTypeId()))
	origHeight = ((SoOrthographicCamera *)camera)->height.getValue();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    reset the camera to it's saved values.
//
// Use: virtual public

void
SoXtViewer::resetToHomePosition()
//
////////////////////////////////////////////////////////////////////////
{
    if (camera == NULL)
	return;
    
    camera->position = origPosition;
    camera->orientation = origOrientation;
    camera->nearDistance = origNearDistance;
    camera->farDistance = origFarDistance;
    camera->focalDistance = origFocalDistance;
    
    // restore camera height (changed by zooming)
    if (camera->isOfType(SoPerspectiveCamera::getClassTypeId()))
	((SoPerspectiveCamera *)camera)->heightAngle.setValue(origHeight);
    else if (camera->isOfType(SoOrthographicCamera::getClassTypeId()))
	((SoOrthographicCamera *)camera)->height.setValue(origHeight);
}

////////////////////////////////////////////////////////////////////////
//
// Description: _ HACK BUG WORKAOUND _
//
//  This updates the event own internal knowledge about which buttons and
//  modifiers keys are down based of the event itself.
//
//  ??? for some reason Xt does NOT update the event button and modifier
//  ??? state until AFTER the event has been sent to the widget. Knowing
//  ??? which mouse buttons and which modifiers keys are down when receiving
//  ??? ANY event (uncluding the event itself) is VERY useful and make
//  ??? the viewer mode chaging code MUCH simpler.
//
// Use: private - BUG WORKAROUND 
static void
updateEventState(XAnyEvent *xe)
//
////////////////////////////////////////////////////////////////////////
{
    XButtonEvent    *be;
    XKeyEvent	    *ke;
    KeySym	    keysym;
    
    switch(xe->type) {
	case ButtonPress:
	    be = (XButtonEvent *)xe;
	    if (be->button == Button1)
		be->state = (be->state | Button1Mask);
	    else if (be->button == Button2)
		be->state = (be->state | Button2Mask);
	    break;
	    
	case ButtonRelease:
	    be = (XButtonEvent *)xe;
	    if (be->button == Button1)
		be->state = (be->state & ~Button1Mask);
	    else if (be->button == Button2)
		be->state = (be->state & ~Button2Mask);
	    break;
	    
	case KeyPress:
	case KeyRelease:
	    ke = (XKeyEvent *)xe;
	    keysym = XLookupKeysym(ke, 0);
	    
	    if (keysym == XK_Control_L || keysym == XK_Control_R) {
		if (xe->type == KeyPress)
		    ke->state = (ke->state | ControlMask);
		else
		    ke->state = (ke->state & ~ControlMask);
	    }
	    break;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Process a common set of events which are shared accross all 
//  viewers. Returning TRUE if the event was processed.
//
// Use: protected
SbBool
SoXtViewer::processCommonEvents(XAnyEvent *xe)
//
////////////////////////////////////////////////////////////////////////
{
    KeySym keysym;
    
    // check if the application wants to handle the event itself
    // instead of giving it to the viewer. This can be used to disable
    // some simple viewer functionality (like the Arrow keys or Esc key).
    // ??? this is a simple work around for bug #113991 - Xt translation
    // ??? tables would be better than dealing with events directly.
    if (SoXtRenderArea::invokeAppCB(xe))
	    return TRUE;
    
    //
    // check for special key which turns viewing on/off
    //
    if (xe->type == KeyPress) {
	XKeyEvent *ke = (XKeyEvent *)xe;
	keysym = XLookupKeysym(ke, 0);
	if (keysym == XK_Escape) {
	    setViewing( !isViewing() );  // toggle the viewing mode...
	    return TRUE;
	}
	else if (!isViewing() && (keysym == XK_Alt_L || keysym == XK_Alt_R)
	    && !(ke->state & Button1Mask || ke->state & Button2Mask)) {
	    // Alt-key goes from PICK to VIEW if
	    // 1] we are not in VIEW mode already
	    // 2] no mouse buttons are pressed
	    //
	    altSwitchBack = TRUE;   // later return back
	    setViewing(TRUE);
	    return TRUE;
	}
    }
    else if (xe->type == KeyRelease) {
	keysym = XLookupKeysym((XKeyEvent *)xe, 0);
	if (altSwitchBack && (keysym == XK_Alt_L || keysym == XK_Alt_R)) {
	    // if Alt-key, then return to PICK (if we had switched)
	    setViewing(FALSE);
	    altSwitchBack = FALSE;  // clear the flag
	    return TRUE;
	}
    }
    else if (xe->type == EnterNotify) {
	XCrossingEvent *ce = (XCrossingEvent *)xe;
	//
	// because the application might use Alt-key for motif menu
	// accelerators we might not receive a key-up event, so make sure
	// to reset any Alt mode (temporary viewing) when the mouse re-enters
	// the window.
	//
	if (! isViewing() && ce->state & Mod1Mask) {
	    altSwitchBack = TRUE;   // later return back
	    setViewing(TRUE);
	}
	else if (altSwitchBack && !(ce->state & Mod1Mask)) {
	    setViewing(FALSE);
	    altSwitchBack = FALSE;  // clear the flag
	}
    }
    
    // send the event to the scene graph if viewing is off
    if ( !isViewing() ) {
	// prevent renderArea from sending the event to the app twice 
	// since it is done above...
	SoXtRenderAreaEventCB *saveFunc = appEventHandler;
	appEventHandler = NULL;
	SoXtRenderArea::processEvent(xe);
	appEventHandler = saveFunc;
	
	return TRUE;
    }
    
    // if no camera discard events
    if (camera == NULL)
	return TRUE;
    
    SbBool handled = TRUE;
    
    // ??? workaround what seem to be an Xt bug...
    updateEventState(xe);
    
    switch(xe->type) {
	case KeyPress:
	    switch ( keysym ) {
		case XK_Home:
		    resetToHomePosition();
		    break;
		case XK_s:
		    setSeekMode( !isSeekMode() );
		    // ??? this is kind of a hack, but it is needed
		    // ??? until a better solution is found
		    if ( isSeekMode() && interactiveCount != 0 ) {
			interactiveCount = 0;
			finishCBList->invokeCallbacks(this);
		    }
		    break;
		case XK_Left:
		case XK_Up:
		case XK_Right:
		case XK_Down:
		    arrowKeyPressed(keysym);
		    break;
		default:
		    handled = FALSE;
		    break;
	    }
	    break;
	    
	default:
	    handled = FALSE;
	    break;
    }
    
    return handled;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Increment the intercative viewing counter.
//
// Use: protected.

void
SoXtViewer::interactiveCountInc()
//
////////////////////////////////////////////////////////////////////////
{
    interactiveCount++;
    
    if (interactiveCount == 1)
	startCBList->invokeCallbacks(this);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Decrement the intercative viewing counter.
//
// Use: protected.

void
SoXtViewer::interactiveCountDec()
//
////////////////////////////////////////////////////////////////////////
{
    if (interactiveCount > 0) {
	interactiveCount--;
	if (interactiveCount == 0)
	    finishCBList->invokeCallbacks(this);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
// This routine is used by subclasses to initiate the seek animation. Given a
// screen mouse location, this routine will return the picked point
// and the normal at that point. It will also schedule the sensor to animate
// if necessary. The routine retuns TRUE if something got picked...
//
// Note: if detailSeek is on, the point and normal correspond to the exact 
//	 3D location under the cursor.
//	 if detailSeek if off, the object bbox center and the camera 
//	 orientation are instead returned.
//
// Use: protected.

SbBool
SoXtViewer::seekToPoint(const SbVec2s &mouseLocation)
//
////////////////////////////////////////////////////////////////////////
{
    if (camera == NULL) {
	setSeekMode(FALSE);
	return FALSE;
    }
    
    // do the picking
    // Use assignment notation to disambiguate from expression (edison)
    SoRayPickAction pick = SoRayPickAction(SbViewportRegion(getGlxSize()));
    pick.setPoint(mouseLocation);
    pick.setRadius(1.0);
    pick.setPickAll(FALSE); // pick only the closest object
    pick.apply(sceneRoot);
    
    // makes sure something got picked
    SoPickedPoint *pp = pick.getPickedPoint();
    if ( pp == NULL ) {
	setSeekMode(FALSE);
	return FALSE;
    }
    
    //
    // Get picked point and normal if detailtSeek
    //
    if (detailSeekFlag) {
	
	seekPoint = pp->getPoint();
	seekNormal = pp->getNormal();
	
	// check to make sure normal points torward the camera, else
	// flip the normal around
	if ( seekNormal.dot(camera->position.getValue() - seekPoint) < 0 )
	    seekNormal.negate();
    }
    //
    // else get object bounding box as the seek point and the camera
    // orientation as the normal.
    //
    else {
	// get center of object's bounding box
	// Use assignment notation to disambiguate from expression (edison)
	SoGetBoundingBoxAction bba = SoGetBoundingBoxAction(SbViewportRegion(getGlxSize()));
	bba.apply(pp->getPath());
	SbBox3f bbox = bba.getBoundingBox();
	seekPoint = bbox.getCenter();
	
	// keep the camera oriented the same way
	SbMatrix mx;
	mx = camera->orientation.getValue();
	seekNormal.setValue(mx[2][0], mx[2][1], mx[2][2]);
    }
    
    
    //
    // now check if animation sensor needs to be scheduled
    //
    
    computeSeekVariables = TRUE;
    if (seekAnimTime == 0) {
	
	// jump to new location, no animation needed
	interpolateSeekAnimation(1.0);
    }
    else {
	// schedule sensor and call viewer start callbacks
	if ( ! seekAnimationSensor->getAttachedField() ) {
	    seekAnimationSensor->attach(viewerRealTime);
	    seekAnimationSensor->schedule();
	    interactiveCountInc();
	}
	
	seekStartTime = viewerRealTime->getValue();
    }
    
    return TRUE;    // successfull
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	computes what the final camera seek orientation should be.
//  
// Use: virtual protected

void
SoXtViewer::computeSeekFinalOrientation()
//
////////////////////////////////////////////////////////////////////////
{
    SbMatrix mx;
    SbVec3f viewVector;
    
    // find the camera final orientation
    if ( isDetailSeek() ) {
	
	// get the camera new orientation
	mx = camera->orientation.getValue();
	viewVector.setValue(-mx[2][0], -mx[2][1], -mx[2][2]);
	SbRotation changeOrient;
	changeOrient.setValue(viewVector, seekPoint - camera->position.getValue());
	newCamOrientation = camera->orientation.getValue() * changeOrient;
    }
    else
	newCamOrientation = camera->orientation.getValue();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  
// Use: virtual protected

void
SoXtViewer::interpolateSeekAnimation(float t)
//
////////////////////////////////////////////////////////////////////////
{
    if (camera == NULL)
	return;
    
    //
    // check if camera new and old position/orientation have already
    // been computed.
    //
    if (computeSeekVariables) {
	SbMatrix mx;
	SbVec3f viewVector;
	
	// save camera starting point
	oldCamPosition = camera->position.getValue();
	oldCamOrientation = camera->orientation.getValue();
	
	// compute the distance the camera will be from the seek point
	// and update the camera focalDistance.
	float dist;
	if ( seekDistAsPercentage ) {
	    SbVec3f seekVec(seekPoint - camera->position.getValue());
	    dist = seekVec.length() * (seekDistance / 100.0);
	}
	else
	    dist = seekDistance;
	camera->focalDistance = dist;
	
	// let subclasses have a chance to redefine what the
	// camera final orientation should be.
	computeSeekFinalOrientation();
	
	// find the camera final position based on orientation and distance
	mx = newCamOrientation;
	viewVector.setValue(-mx[2][0], -mx[2][1], -mx[2][2]);
	newCamPosition = seekPoint - dist * viewVector;
	
	computeSeekVariables = FALSE;
    }
    
    
    //
    // Now position the camera according to the animation time
    //
    
    // use and ease-in ease-out approach
    float cos_t = 0.5 - 0.5 * cosf(t * M_PI);
    
    // get camera new rotation
    camera->orientation = SbRotation::slerp(oldCamOrientation, newCamOrientation, cos_t);
    
    // get camera new position
    camera->position = oldCamPosition + (newCamPosition - oldCamPosition) * cos_t;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Adjust the camera clipping planes based on the scene bounding 
//  box. (called before every redraws)
//
// use: virtual protected

void
SoXtViewer::adjustCameraClippingPlanes()
//
////////////////////////////////////////////////////////////////////////
{
    if (camera == NULL)
	return;
    
    // get the scene bounding box
    autoClipBboxAction->setViewportRegion(SbViewportRegion(getGlxSize()));
    autoClipBboxAction->apply(sceneRoot);

    SbXfBox3f xfbbox = autoClipBboxAction->getXfBoundingBox();
    
    // get camera transformation and apply to xfbbox
    // to align the bounding box to camera space.
    // This will enable us to simply use the z values of the
    // transformed bbox for near and far plane values.
    SbMatrix mx;
    mx.setTranslate(- camera->position.getValue());
    xfbbox.transform(mx);
    mx = camera->orientation.getValue().inverse();
    xfbbox.transform(mx);
    
    // get screen align bbox and figure the near and far plane values
    SbBox3f bbox = xfbbox.project();
    // take negative value and opposite to what one might think 
    // because the camera points down the -Z axis
    float far = - bbox.getMin()[2];
    float near = - bbox.getMax()[2];
    
    // scene is behind the camera so don't change the planes
    if (far < 0)
	return;
    
    // check for minimum near plane value (Value will be negative 
    // when the camera is inside the bounding box).
    // Note: there needs to be a minimum near value for perspective
    // camera because of zbuffer resolution problem (plus the values
    // has to be positive). There is no such restriction for
    // an Orthographic camera (you can see behind you).
    if (! camera->isOfType(SoOrthographicCamera::getClassTypeId())) {
	if (near < (minimumNearPlane * far))
	    near = minimumNearPlane * far;
    }
    
    // give the near and far distances a little bit of slack in case
    // the object lies against the bounding box, otherwise the object
    // will be poping in and out of view.
    // (example: a cube is the same as it's bbox)
    near *= 0.999;
    far *= 1.001;
    
    // finally assign camera plane values
    if (camera->nearDistance.getValue() != near)
	camera->nearDistance = near;
    if (camera->farDistance.getValue() != far)
	camera->farDistance = far;
}

////////////////////////////////////////////////////////////////////////
//
//  Copy the camera onto the clipboard.
//
//  Use: private
//
void
SoXtViewer::copyView(Time eventTime)
//
////////////////////////////////////////////////////////////////////////
{
    if (camera == NULL)
	return;
    
    if (clipboard == NULL)
    	clipboard = new SoXtClipboard(getWidget());
    
    clipboard->copy(camera, eventTime);
}

////////////////////////////////////////////////////////////////////////
//
//  Retrieve the selection from the X server and paste it when it
//  arrives (in our pasteDone callback).
//
//  Use: private
//
void
SoXtViewer::pasteView(Time eventTime)
//
////////////////////////////////////////////////////////////////////////
{
    if (clipboard == NULL)
    	clipboard = new SoXtClipboard(getWidget());
    
    clipboard->paste(eventTime, SoXtViewer::pasteDoneCB, this);
}


////////////////////////////////////////////////////////////////////////
//
//  This is called by Xt when the data is ready to be pasted.
//
//  Use: static, private
//
void 
SoXtViewer::pasteDoneCB(void *userData, SoPathList *pathList)
//
////////////////////////////////////////////////////////////////////////
{
    SoCamera *newCamera = NULL;
    
    // search for a camera in the paste data
    for (int i = 0; i < pathList->getLength(); i++) {
	SoFullPath *fullP = (SoFullPath *) (*pathList)[i];
	if (fullP->getTail()->isOfType(SoCamera::getClassTypeId())) {
	    newCamera = (SoCamera *) fullP->getTail();
	    break;
	}
    }

    if (newCamera != NULL)
	((SoXtViewer *) userData)->changeCameraValues(newCamera);
    
    // We delete the callback data when done with it.
    delete pathList;
}

////////////////////////////////////////////////////////////////////////
//
//  Change the values of our camera to newCamera.
//??? animate from old values to new?
//
//  Use: virtual, protected
//
void 
SoXtViewer::changeCameraValues(SoCamera *newCamera)
//
////////////////////////////////////////////////////////////////////////
{
    if (camera == NULL)
	return;
    
    // only paste cameras of the same type
    if (camera->getTypeId() != newCamera->getTypeId())
	return;

    // give our camera the values of the new camera
    camera->position	    = newCamera->position;
    camera->orientation	    = newCamera->orientation;
    camera->nearDistance    = newCamera->nearDistance;
    camera->farDistance	    = newCamera->farDistance;
    camera->focalDistance   = newCamera->focalDistance;

    // get the height or heightAngle
    if (camera->isOfType(SoPerspectiveCamera::getClassTypeId()))
	((SoPerspectiveCamera *)camera)->heightAngle = 
		((SoPerspectiveCamera *)newCamera)->heightAngle;
    else
	((SoOrthographicCamera *)camera)->height = 
		((SoOrthographicCamera *)newCamera)->height;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Toggles the current camera type (perspective <--> orthographic)
//
//  Use: virtual protected
//
void
SoXtViewer::toggleCameraType()
//
////////////////////////////////////////////////////////////////////////
{
    if (camera == NULL)
	return;
    
    // create the camera of the opposite kind and compute the wanted height
    // or heightAngle of the new camera.
    SoCamera *newCam;
    if (camera->isOfType(SoPerspectiveCamera::getClassTypeId())) {
	float angle = ((SoPerspectiveCamera *)camera)->heightAngle.getValue();
	float height = camera->focalDistance.getValue() * tanf(angle/2);
	newCam = new SoOrthographicCamera;
	((SoOrthographicCamera *)newCam)->height = 2 * height;
    }
    else if (camera->isOfType(SoOrthographicCamera::getClassTypeId())) {
	float height = ((SoOrthographicCamera *)camera)->height.getValue() / 2;
	float angle = atanf(height / camera->focalDistance.getValue());
	newCam = new SoPerspectiveCamera;
	((SoPerspectiveCamera *)newCam)->heightAngle = 2 * angle;
    }
    else {
#ifdef DEBUG
	SoDebugError::post("SoXtViewer::toggleCameraType", "unknown camera type!");
#endif
	return;
    }
    
    newCam->ref();
    
    // copy common stuff from the old to the new camera
    newCam->viewportMapping = camera->viewportMapping.getValue();
    newCam->position = camera->position.getValue();
    newCam->orientation = camera->orientation.getValue();
    newCam->aspectRatio = camera->aspectRatio.getValue();
    newCam->focalDistance = camera->focalDistance.getValue();
    
    // search for the old camera and replace it by the new camera
    SoSearchAction sa;
    sa.setNode(camera);
    sa.apply(sceneRoot);
    SoFullPath *fullCamPath = (SoFullPath *) sa.getPath();
    if (fullCamPath) {
	SoGroup *parent = (SoGroup *)fullCamPath->getNode(fullCamPath->getLength() - 2);
	parent->insertChild(newCam, parent->findChild(camera));
	SoCamera *oldCam = camera;
	setCamera(newCam);
	
	// remove the old camera if it is still there (setCamera() might
	// have removed it) and set the created flag to true (for next time)
	if (parent->findChild(oldCam) >= 0)
	    parent->removeChild(oldCam);
	createdCamera = TRUE;
    }
#ifdef DEBUG
    else
	SoDebugError::post("SoXtViewer::toggleCameraType", "camera not found!");
#endif
    
    newCam->unref();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Called by the processCommonEvent routine whenever the arrow keys
//  are pressed. Translate the camera in the viewing plane in the arrow
//  direction half a screen at a time.
//
//  Use: private
//
void
SoXtViewer::arrowKeyPressed(KeySym key)
//
////////////////////////////////////////////////////////////////////////
{
    if (camera == NULL)
	return;
    
    // get the camera near plane height value
    float dist;
    if (camera->isOfType(SoPerspectiveCamera::getClassTypeId())) {
	float angle = ((SoPerspectiveCamera *)camera)->heightAngle.getValue();
	float length = camera->nearDistance.getValue();
	dist = length * tanf(angle);
    }
    else if (camera->isOfType(SoOrthographicCamera::getClassTypeId()))
	dist = ((SoOrthographicCamera *)camera)->height.getValue();
    dist /= 2.0;
    
    // get camera right/left/up/down direction
    SbMatrix mx;
    mx = camera->orientation.getValue();
    SbVec3f dir;
    switch(key) {
	case XK_Up:
	    dir.setValue(mx[1][0], mx[1][1], mx[1][2]);
	    break;
	case XK_Down:
	    dir.setValue(-mx[1][0], -mx[1][1], -mx[1][2]); 
	    break;
	case XK_Right:
	    dir.setValue(mx[0][0], mx[0][1], mx[0][2]);
	    dist *= camera->aspectRatio.getValue();
	    break;
	case XK_Left:
	    dir.setValue(-mx[0][0], -mx[0][1], -mx[0][2]);
	    dist *= camera->aspectRatio.getValue();
	    break;
    }
    
    // finally reposition the camera
    camera->position = camera->position.getValue() + dist * dir;
}

//
////////////////////////////////////////////////////////////////////////
// static callbacks stubs
////////////////////////////////////////////////////////////////////////
//



void
SoXtViewer::bufferStartCallback(void *, SoXtViewer *v)
{
    v->SoXtRenderArea::setDoubleBuffer(TRUE);
}

void
SoXtViewer::bufferFinishCallback(void *, SoXtViewer *v)
{
    v->SoXtRenderArea::setDoubleBuffer(FALSE);
}

void
SoXtViewer::drawStyleStartCallback(void *, SoXtViewer *v)
{
    v->interactiveFlag = TRUE;  // must happen first
    
    // if still and move draw styles are the same, return...
    if (v->interactiveDrawStyle == VIEW_SAME_AS_STILL ||
	v->interactiveDrawStyle == v->stillDrawStyle)
	return;
    
    // if we have "MOVE NO TEXTURE", then we have nothing
    // to do if we have a current draw style (since they all have 
    // texturing turned off in the first place).
    if (v->interactiveDrawStyle == VIEW_NO_TEXTURE &&
	v->stillDrawStyle != VIEW_AS_IS)
	return;
    
    v->setCurrentDrawStyle(v->interactiveDrawStyle);
}

void
SoXtViewer::drawStyleFinishCallback(void *, SoXtViewer *v)
{
    v->interactiveFlag = FALSE;  // must happen first
    
    // if still and move draw styles are the same, return...
    if (v->interactiveDrawStyle == VIEW_SAME_AS_STILL ||
	v->interactiveDrawStyle == v->stillDrawStyle)
	return;
    
    // if we have "MOVE NO TEXTURE", then we have nothing
    // to do if we have a current draw style (since they all have 
    // texturing turned off in the first place).
    if (v->interactiveDrawStyle == VIEW_NO_TEXTURE &&
	v->stillDrawStyle != VIEW_AS_IS)
	return;
    
    v->setCurrentDrawStyle(v->stillDrawStyle);
}

////////////////////////////////////////////////////////////////////////
//
//	Called whenever the seek animation sensor fires. Finds the amount 
//  of time since we started the seek and call the subclasses routine
//  to do the correct interpolation.
//
//  Use: static private
//
void
SoXtViewer::seekAnimationSensorCB(void *p, SoSensor *)
//
////////////////////////////////////////////////////////////////////////
{
    SoXtViewer *v = (SoXtViewer *)p;
    
    // get the time difference
    SbTime time = viewerRealTime->getValue();
    float sec = float((time - v->seekStartTime).getValue());
    if (sec == 0.0)
	sec = 1.0/72.0;	// at least one frame (needed for first call)
    float t = (sec / v->seekAnimTime);
    
    // check to make sure the values are correctly clipped
    if (t > 1.0)
    	t = 1.0;
    else if ((1.0 - t) < 0.0001)
	t = 1.0; // this will be the last one...
    
    // call subclasses to interpolate the animation
    v->interpolateSeekAnimation(t);
    
    // stops seek if this was the last interval
    if (t == 1.0)
	v->setSeekMode(FALSE);
}



//
////////////////////////////////////////////////////////////////////////
// viewer feedback convenience routines
////////////////////////////////////////////////////////////////////////
//



/*
 * Defines
 */

// color used in feedback
#define DARK_COLOR	glColor3ub(90, 90, 90)
#define LIGHT_COLOR	glColor3ub(230, 230, 230)

#define LINE_THIN   3	// line thickness used in feedback
#define	LINE_THICK  (LINE_THIN + 2)
#define CROSS 	    8	// size of cross hair at screen center for Roll
#define RADIUS	    15	// radius of center circle (in pix) for Roll
#define ANGLE_LEN   14   // angular size in degrees of Roll anchor


/*
 * Globals
 */

#define ARROW_SIZE  6.0	// size in pix of arrow head

// anchor arrow head description
static float arrow_data[3][3] = {
    -ARROW_SIZE, 0, 0,
    0, 2*ARROW_SIZE, 0,
    ARROW_SIZE, 0, 0
};


/*
 * Macros
 */

#define	DRAW_ARROW_MACRO    \
    DARK_COLOR;	\
    glBegin(GL_POLYGON);    \
    glVertex3fv(arrow_data[0]);	\
    glVertex3fv(arrow_data[1]);	\
    glVertex3fv(arrow_data[2]);	\
    glEnd();	\
    LIGHT_COLOR;	\
    glLineWidth(1); \
    glBegin(GL_LINE_LOOP);	\
    glVertex3fv(arrow_data[0]);	\
    glVertex3fv(arrow_data[1]);	\
    glVertex3fv(arrow_data[2]);	\
    glEnd();

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the default ortho projection when doing viewer feedback. The 
//  zbuffer/lighting is automatically turned off.
//
//  Use: static protected
//
void
SoXtViewer::setFeedbackOrthoProjection(const SbVec2s &size)
//
////////////////////////////////////////////////////////////////////////
{
    // push the gl state to revert it back later....
    glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT | GL_LINE_BIT);
    
    // ??? should we worry about restoring this matrix later ?
    glViewport(0, 0, size[0], size[1]);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, size[0], 0, size[1], -1, 1);
    
    // disable zbuffer and lighting....
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    restores the state that was changed when setFeedbackOrthoProjection()
//  is called.
//
//  Use: static protected
//
void
SoXtViewer::restoreGLStateAfterFeedback()
//
////////////////////////////////////////////////////////////////////////
{
    // restore the gl state that were saved in setFeedbackOrthoProjection()
    glPopAttrib();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Draws a simple 2 colored cross at the given location.
//
//  Use: static protected
//
void
SoXtViewer::drawViewerCrossFeedback(SbVec2s loc)
//
////////////////////////////////////////////////////////////////////////
{
    LIGHT_COLOR;
    glLineWidth(4);
    glBegin(GL_LINES);
    glVertex2s(loc[0]-CROSS, loc[1]);
    glVertex2s(loc[0]+CROSS, loc[1]);
    glVertex2s(loc[0], loc[1]-CROSS);
    glVertex2s(loc[0], loc[1]+CROSS);
    glEnd();
    
    DARK_COLOR;
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2s(loc[0]-CROSS+1, loc[1]);
    glVertex2s(loc[0]+CROSS-1, loc[1]);
    glVertex2s(loc[0], loc[1]-CROSS+1);
    glVertex2s(loc[0], loc[1]+CROSS-1);
    glEnd();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    draws the anchor roll feedback given the point of rotation and the
//  current mouse location.
//
//  Use: static protected
//
void
SoXtViewer::drawViewerRollFeedback(SbVec2s center, SbVec2s loc)
//
////////////////////////////////////////////////////////////////////////
{
    // get angle and distance of mouse from center of rotation
    float ang, dist;
    float vx = loc[0] - center[0];
    float vy = loc[1] - center[1];
    if (vx==0 && vy==0) {
	ang = 0;
	    dist = 3; // minimum size (given the circle thickness)
    }
    else {
	ang = atan2(vy, vx) * 180.0 / M_PI;
	dist = sqrtf(vx*vx + vy*vy);
	if (dist < 3)
	    dist = 3; // minimum size (given the circle thickness)
    }
    float cirAng = -ang + 90; // gluPartialDisk() angle is REALLY backward !!
    
    static GLUquadricObj *quad = NULL;
    if (! quad)	quad = gluNewQuadric();
    
    // draw all of the circles (first inner, then outer)
    glTranslatef(center[0], center[1], 0);
    LIGHT_COLOR;
    gluDisk(quad, RADIUS, RADIUS+LINE_THICK, 20, 2);
    gluPartialDisk(quad, dist-2, dist+LINE_THICK-2, 20, 2, cirAng - ANGLE_LEN, 2 * ANGLE_LEN);
    DARK_COLOR;
    gluDisk(quad, RADIUS+1, RADIUS+LINE_THICK-1, 20, 2);
    gluPartialDisk(quad, dist-1, dist+LINE_THICK-3, 20, 2, cirAng - ANGLE_LEN, 2 * ANGLE_LEN);
    glTranslatef(-center[0], -center[1], 0); // undo the translation
    
    // draw connecting line from center to outer circle
    glLineWidth(LINE_THICK);
    LIGHT_COLOR;
    glBegin(GL_LINES);
    glVertex2s(center[0], center[1]);
    glVertex2s(loc[0], loc[1]);
    glEnd();
    glLineWidth(LINE_THIN);
    DARK_COLOR;
    glBegin(GL_LINES);
    glVertex2s(center[0], center[1]);
    glVertex2s(loc[0], loc[1]);
    glEnd();
    
    // draw the CCW arrow
    glPushMatrix();
    glTranslatef(center[0], center[1], 0);
    glRotatef(ang+ANGLE_LEN, 0, 0, 1);
    glTranslatef(dist, 0, 0);
    DRAW_ARROW_MACRO
    glPopMatrix();
    
    // draw the CW arrow
    glPushMatrix();
    glTranslatef(center[0], center[1], 0);
    glRotatef(ang-ANGLE_LEN, 0, 0, 1);
    glTranslatef(dist, 0, 0);
    glScalef(1, -1, 1);
    DRAW_ARROW_MACRO
    glPopMatrix();
}

