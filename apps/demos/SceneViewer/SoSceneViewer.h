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

//  -*- C++ -*-

/*
 * Copyright (C) 1990-93   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.2 $
 |
 |   Classes	: SoSceneViewer
 |
 |   Author(s)	: Thad Beier, Alain Dumesny, David Mott, Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SCENE_VIEWER_
#define  _SCENE_VIEWER_

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/RowColumn.h>

#include <Inventor/nodes/SoNode.h>
#include <Inventor/SbPList.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/actions/SoGLRenderAction.h>	// transparency levels
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/Xt/viewers/SoXtFullViewer.h>

// components
class	MyColorEditor;
class	SoXtMaterialEditor;
class	SoXtPrintDialog;
class	SoXtTransformSliderSet;
class	SoXtClipboard;
class	SoXtDirectionalLightEditor;

// interaction
class	SoBoxHighlightRenderAction;
class	SoSelection;
class	SoPickedPoint;

// nodes
class	SoDirectionalLight;
class	SoEnvironment;
class	SoGroup;
class	SoLabel;
class	SoMaterial;
class	SoSceneViewer;
class   SoSwitch;
class	SoLight;
class	SoTransform;

// misc.
class	SvManipList;

enum SvEViewer {
    SV_VWR_EXAMINER = 0, 
    SV_VWR_FLY, 
    SV_VWR_WALK, 
    SV_VWR_PLANE 
};

enum SvEManipMode {
    SV_NONE,                         // None
    SV_TRANSFORMER,                  // Transformer manip
    SV_TRACKBALL,                    // Trackball manip
    SV_HANDLEBOX,                    // Handlebox manip
    SV_JACK,                         // Jack manip
    SV_CENTERBALL,                   // Centerball manip
    SV_XFBOX,                        // TransformBox manip
    SV_TABBOX                        // TabBox manip
};

struct	SoSceneViewerData;
struct	SvLightData;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoSceneViewer
//
//      The SceneViewer component lets you select between different viewers
//  (Examiner, Navigator,..) and lets you perform  some simple attribute
//  assignment, and some manipulation of data.
//
//////////////////////////////////////////////////////////////////////////////

class SoSceneViewer : public SoXtComponent {

  public:    
    // Constructor:
    // envFile is the environment file to use. This is where the camera,
    // environment node, and lights are found. 
    // Passing NULL means use the default environment.
    //
    // Save will save all the children of root, not including root itself.
    // Save Environment will save the lights and camera environment into envFile
    //
    SoSceneViewer(
	Widget parent = NULL, 
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE, 
	SoSelection *root = NULL,
	const char *envFile = NULL);
    ~SoSceneViewer();

  protected:  
    // This constructor takes a boolean whether to build the widget now.
    // Subclasses can pass FALSE, then call SoSceneViewer::buildWidget()
    // when they are ready for it to be built.
    SoEXTENDER
    SoSceneViewer(
	    Widget parent,
	    const char *name, 
	    SbBool buildInsideParent, 
	    SoSelection *root,
	    const char *envFile, 
	    SbBool buildNow);
	    

  public:
    //
    // Camera operation routines
    //
    void	viewAll()	    
		    { if (currentViewer != NULL) currentViewer->viewAll(); }
    void       	viewSelection();
    void	saveHomePosition()  
		    { if (currentViewer != NULL) currentViewer->saveHomePosition(); }
    void    	setCamera(SoCamera *cam) 
		    { if (currentViewer != NULL) currentViewer->setCamera(cam); }
    // NOTE: because the camera may be changed dynamically (switch between ortho
    // and perspective), the user shouldn't cache the camera.
    SoCamera	*getCamera()	    
		    { return (currentViewer) ? currentViewer->getCamera() : NULL; }
    
    //
    // Before new data is sent to the viewer, the newData method should
    // be called to disconnect all manipulators and highlights
    //
    void	        newData();
    
    // Show/hide the pulldown menu bar (default shown)
    void		showMenu(SbBool onOrOff);
    SbBool		isMenuShown()	    { return showMenuFlag; }
    
    // Show/hide the viewer component trims (default shown)
    void    	    	setDecoration(SbBool onOrOff)	
		    { if (currentViewer != NULL) currentViewer->setDecoration(onOrOff); }
    SbBool  	    	isDecoration()	    
		    { return currentViewer != NULL ? currentViewer->isDecoration() : FALSE; }
    
    // Show/hide headlight (default on) and get to the headlight node.
    void    	    	setHeadlight(SbBool onOrOff)	
		    { if (currentViewer != NULL) currentViewer->setHeadlight(onOrOff); }
    SbBool  	    	isHeadlight()	    
		    { return currentViewer != NULL ? currentViewer->isHeadlight() : FALSE; }
    SoDirectionalLight	*getHeadlight()	    
		    { return currentViewer != NULL ? currentViewer->getHeadlight() : NULL; }
    
    //
    // Sets/gets the current drawing style in the main view - The user
    // can specify the INTERACTIVE draw style (draw style used when 
    // the scene changes) independently from the STILL style.
    //
    // (default VIEW_AS_IS for both STILL and INTERACTIVE)
    //
    void    	    setDrawStyle(SoXtViewer::DrawType type, 
				SoXtViewer::DrawStyle style)
			    { currentViewer->setDrawStyle(type, style); }
    SoXtViewer::DrawStyle getDrawStyle(SoXtViewer::DrawType type)
			    { return currentViewer->getDrawStyle(type); }
    
    //
    // Sets/gets the current buffering type in the main view
    // (default BUFFER_INTERACTIVE on Indigo, BUFFER_DOUBLE otherwise)
    //
    void    	    	setBufferingType(SoXtViewer::BufferType type)
			    { currentViewer->setBufferingType(type); }
    SoXtViewer::BufferType getBufferingType()
			    { return currentViewer->getBufferingType(); }

    // Turn viewing on/off (Default to on) in the viewers.
    void    	    	setViewing(SbBool onOrOff)  { currentViewer->setViewing(onOrOff); }
    SbBool  	    	isViewing()	{ return currentViewer->isViewing(); }
    
    // Set/get the level of transparency type
    void		setTransparencyType(SoGLRenderAction::TransparencyType type)
				    { currentViewer->setTransparencyType(type); }
    SoGLRenderAction::TransparencyType	getTransparencyType()
				    { return currentViewer->getTransparencyType(); }

    // returns the current render area widget
    Widget		getRenderAreaWidget()	{ return currentViewer->getRenderAreaWidget(); }
    
#ifdef EXPLORER
    // set the EXPLORER user mode callback routine
    void		setUserModeEventCallback(SoXtRenderAreaEventCB *fcn);
#endif /* EXPLORER */

 protected:
    // redefine these
    virtual const char *    getDefaultWidgetName() const;
    virtual const char *    getDefaultTitle() const;
    virtual const char *    getDefaultIconTitle() const;

 private:
    // Scene graph data
    SoSelection	    	*selection;	    	// the same user supplied graph
    SoSeparator 	*sceneGraph;    	// user supplied scene graph
    
    // Lights, camera, environment!
    SoGroup 	    	*lightsCameraEnvironment;
    SoLabel 	    	*envLabel;
    SoEnvironment   	*environment;
    SoGroup 	    	*lightGroup;
    
    void	    	createLightsCameraEnvironment();
    
    // Selection highlight
    SoBoxHighlightRenderAction *highlightRA;
    
    //
    // Widgets and menus
    //
    
    Widget		mgrWidget;  // our topmost form widget
    Widget		menuWidget; // topbar menu widget
    SbBool		showMenuFlag;
    SoSceneViewerData	*menuItems; // list of menu items data
    
    Widget 		buildWidget(Widget parent);
    void		buildAndLayoutMenu(Widget parent);
    void		buildAndLayoutViewer(SoXtFullViewer *vwr);

    // callback for all menu buttons to perform action
    static void	    	processTopbarEvent(Widget, SoSceneViewerData *,
				   XmAnyCallbackStruct *);
    // callback when a menu is displayed
    static void    	menuDisplay(Widget, SoSceneViewerData *, XtPointer);
    
    // this is called after objects are added/deleted or the selection changes
    void		updateCommandAvailability();
    
    //
    // File
    //
    
    // file reading methods
    int		fileMode;
    char	*fileName;
    SbBool	useShowcaseBrowser;
    void	getFileName();
    SbBool	doFileIO(const char *filename);
    SbBool    	readFile(const char *filename);
    SbBool	writeFile(const char *filename);
    SbBool	readEnvFile(const char *filename);
    SbBool	writeEnvFile(const char *filename);
    void	deleteScene();
    void	save();

    // cameras
    void	removeCameras(SoGroup *root);
    // Returns the last scene kit camera in a scene.
    SoCamera    *getLastSceneKitCamera(SoGroup *root);
    // Copies our camera into the last scene kit camera in a scene.
    void        setLastSceneKitCamera(SoGroup *root, SoCamera *theCam);
    // Switches off/ restores camera num settings of scenekits.
    void        switchOffSceneKitCameras(SoGroup *root);
    void        restoreSceneKitCameras(SoGroup *root);
    // Copies from given camera into this viewer's camera, if types match.
    void        setCameraFields( SoCamera *fromCam );
    SbDict      *sceneKitCamNumDict;
    
    // motif vars for file reading/writting
    Widget		fileDialog;
    static void     	fileDialogCB(Widget, SoSceneViewer *,
    	    	    	    	    XmFileSelectionBoxCallbackStruct *);
        
    // printing vars
    SoXtPrintDialog   	*printDialog;
    void    	    	print();
    static void         beforePrintCallback(void *uData, SoXtPrintDialog *);
    static void         afterPrintCallback(void *uData, SoXtPrintDialog *);


    SbBool		feedbackShown;
    
    void		showAboutDialog();
    
    //
    // Edit
    //
    
    // Select parent, if there is one; select everything.
    void		pickParent();
    void		pickAll();

    // for copy and paste
    SoXtClipboard	*clipboard; 	// copy/paste 3d data
    void		destroySelectedObjects();

    // Paste callback - invoked when paste data transfer is complete
    void    	    	pasteDone(SoPathList *pathList);
    static void    	pasteDoneCB(void *userData, SoPathList *pathList);
    
    // Drag and drop support (allowing dropping of data into SceneViewer)
    static SbBool    	iconDropCB(void *userData, const char *filename);
    static SbBool	objDropCB(
			    void *userData,
			    Atom dataType,
			    void *data, uint32_t numBytes);
    
    
    //
    // Viewing
    //
    
    SoXtFullViewer	*viewerList[4];	// list of viewers
    SvEViewer		whichViewer;	// current viewer ID
    SoXtFullViewer	*currentViewer;	// current viewer pt
    
    void    	    	switchToViewer(SvEViewer newViewer);
    
    // Environment: fog, antialiasing
    SbBool		fogFlag;		// Fog on/off
    void		setFog(SbBool onOrOff);	// Turns fog on/off
    SbBool		antialiasingFlag;	// AA-ing on/off
    void		setAntialiasing(SbBool onOrOff); // Turns AA-ing on/off
    
    // Background color
    MyColorEditor   	*backgroundColorEditor;
    const SbColor &	getBackgroundColor()	{ return currentViewer->getBackgroundColor(); }
    void		editBackgroundColor();	// Invokes color editor on bkg
    static void	    	backgroundColorCallback(void *userData,
				const SbColor *color);
    
    //
    // Editors
    //
    
    SbBool  	    	ignoreCallback;
    SoMaterial     	*findMaterialForAttach( const SoPath *target );
    SoPath     		*findTransformForAttach( const SoPath * target );
    // callback used by Accum state action created by findMaterialForAttach
    static SoCallbackAction::Response findMtlPreTailCB(void *data, 
					    SoCallbackAction *accum,
					    const SoNode *);
    
    // transform slider set
    SoXtTransformSliderSet *transformSliderSet;
    void		createTransformSliderSet();

    // Material editor
    SoXtMaterialEditor  	*materialEditor;
    void    	    	createMaterialEditor();
    
    // Color editor
    MyColorEditor   	*colorEditor;
    void		createColorEditor();


    //
    // Manips
    //
    SvEManipMode curManip;
    SbBool       curManipReplaces;
    SvManipList	 *maniplist;	// list of selection/manip/xfPath triplets.

    // replaces manips with the given type for all selected objects.
    void    replaceAllManips( SvEManipMode manipType );

    // attaches a manipulator
    void    attachManip( SvEManipMode manipType, SoPath *p );
    void    attachManipToAll( SvEManipMode manipType );

    // detaches a manipulator
    void    detachManip( SoPath *p );
    void    detachManipFromAll();

    // Temporarily remove manips for writing, printing, copying, etc.
    // and restore later
    void    removeManips();
    void    restoreManips();

    // Callback to adjust size of scale tabs. Used only for SoTabBoxManip
    // Added to viewer as a finishCallback.
    static void adjustScaleTabSizeCB( void *, SoXtViewer *);

    //
    // Lighting
    //
    
    SbPList		lightDataList;
    SoXtDirectionalLightEditor *headlightEditor;
    SvLightData		*headlightData;
    void		addLight(SoLight *light);
    SvLightData		*addLightEntry(SoLight *light, SoSwitch *lightSwitch);
    void		addLightMenuEntry(SvLightData *);
    void		turnLightOnOff(SvLightData *data, SbBool flag);
    static void		lightToggleCB(Widget, SvLightData *, void *);
    void		editLight(SvLightData *data, SbBool flag);
    static void		editLightToggleCB(Widget, SvLightData *, void *);
    static void		editLightColorCB(Widget, SvLightData *, void *);
    void		removeLight(SvLightData *);
    static void		removeLightCB(Widget, SvLightData *, void *);
    static void		lightSubmenuDisplay(Widget, SvLightData *, void *);
    void 		transferDirectionalLightLocation( SvLightData *data );
    
    // vars to make the light manips all the same size
    void		calculateLightManipSize();
    float		lightManipSize;
    SbBool		calculatedLightManipSize;
    
    // temporary remove/add the light manip geometry of the attached manips
    // (used for file writting and printing)
    void		removeAttachedLightManipGeometry();
    void		addAttachedLightManipGeometry();
    
    // Ambient lighting color
    MyColorEditor   	*ambientColorEditor;
    void		editAmbientColor();	// Invokes color editor on amb
    static void	    	ambientColorCallback(void *userData,
				const SbColor *color);
    
    //
    // Selection
    //
    // manages changes in the selection.
    static void 	deselectionCallback(void *userData, SoPath *obj);
    static void 	selectionCallback(void *userData, SoPath *obj);
    static SoPath *	pickFilterCB(void *userData, const SoPickedPoint *pick);
    
    //
    // Convenience routines
    //
    static SbBool    isAffectedByTransform( SoNode *node );
    static SbBool    isAffectedByMaterial( SoNode *node );

#ifdef EXPLORER
    //
    // user pick function
    //
    SoXtRenderAreaEventCB   *userModeCB;
    void		    *userModedata;
    SbBool		    userModeFlag;
#endif /* EXPLORER */

    void constructorCommon(SoSelection *root, const char *envFile, SbBool buildNow);
};

#endif  /* _SCENE_VIEWER_ */
