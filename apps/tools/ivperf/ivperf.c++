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
 * Copyright (C) 1990,91,92,93,94   Silicon Graphics, Inc.
 *
 _____________________________________________________________
 _________  S I L I C O N   G R A P H I C S   I N C .  _______
 |
 |   $Revision: 1.3 $
 |
 |   Description:
 |	Benchmarks rendering by spinning scene graph.
 |      See printUsage() for usage.
 |	Based on older ivRender by Gavin Bell and Paul Strauss
 |
 |   Author(s)		: Ajay Sreekanth
 |
 _________  S I L I C O N   G R A P H I C S   I N C .  _______
 _____________________________________________________________
 */

#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <Inventor/Sb.h>
#include <Inventor/SbTime.h>
#include <Inventor/SbViewportRegion.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInteraction.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/actions/SoActions.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoSubAction.h>
#include <Inventor/fields/SoSFTime.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoFile.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoPackedColor.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoShape.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoTransform.h>

// default window size
#define WINDOW_X	400	
#define WINDOW_Y	400	

// number of frame to draw before autocaching kicks in
// typically needs only 2 but we use 4 just in case
#define NUM_FRAMES_AUTO_CACHING	4

// default number of frames 
#define NUM_FRAMES	60

// the name used in the scene graph to indicate that the application
// will modify something below this separator
// the children of these nodes are touched to destroy the caches there
#define NO_CACHE_NAME	"NoCache"

// the name of the transformation node added to spin the scene
#define SCENE_XFORM_NAME   	"SCENE_XFORM_#####"


struct Options {
    // fields initialized by the user
    SbBool	showBars;
    int		numFrames;
    const char	*inputFileName;
    unsigned int windowX, windowY;
    // fields set based on structure of scene graph
    SbBool	hasLights;
    SbBool	hasTextures;
    // fields used internally
    SbBool	noClear;
    SbBool	noMaterials;
    SbBool	noTextures;
    SbBool	oneTexture;
    SbBool	noXforms;
    SbBool	noFill;
    SbBool	noVtxXforms;
    SbBool	noLights;
    SbBool	freeze;
};


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback used to count triangles.
//

static void
countTriangleCB(void *userData, SoCallbackAction *,
		const SoPrimitiveVertex *,
		const SoPrimitiveVertex *,
		const SoPrimitiveVertex *)
//
////////////////////////////////////////////////////////////////////////
{
    int32_t *curCount = (int32_t *) userData;

    (*curCount)++;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback used to count Line Segments.
//

static void
countLinesCB(void *userData, SoCallbackAction *,
		const SoPrimitiveVertex *,
		const SoPrimitiveVertex *)
//
////////////////////////////////////////////////////////////////////////
{
    int32_t *curCount = (int32_t *) userData;

    (*curCount)++;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback used to count points.
//

static void
countPointsCB(void *userData, SoCallbackAction *,
		const SoPrimitiveVertex *)
//
////////////////////////////////////////////////////////////////////////
{
    int32_t *curCount = (int32_t *) userData;

    (*curCount)++;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback used to count nodes.
//

static SoCallbackAction::Response
countNodesCB(void *userData, SoCallbackAction *,
		const SoNode *)
//
////////////////////////////////////////////////////////////////////////
{
    int32_t *curCount = (int32_t *) userData;

    (*curCount)++;

    return SoCallbackAction::CONTINUE;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Counts triangles/lines/points in given graph using primitive generation,
//    returning total.
//

void
countPrimitives(SoNode *root, int32_t &numTris, int32_t &numLines, int32_t &numPoints,
		int32_t &numNodes)
//
////////////////////////////////////////////////////////////////////////
{
    numTris = 0;
    numLines = 0;
    numPoints = 0;
    numNodes = 0;
    SoCallbackAction	ca;

    ca.addPointCallback(SoShape::getClassTypeId(),
			   countPointsCB,
			   (void *) &numPoints);
    ca.addLineSegmentCallback(SoShape::getClassTypeId(),
			   countLinesCB,
			   (void *) &numLines);
    ca.addTriangleCallback(SoShape::getClassTypeId(),
			   countTriangleCB,
			   (void *) &numTris);
    ca.addPreCallback( SoNode::getClassTypeId(),
			countNodesCB, (void *)&numNodes
		    );
    ca.apply(root);
}


//////////////////////////////////////////////////////////////
//
// Description:
//    Parses command line arguments, setting options.
//

static SbBool
parseArgs(int argc, char *argv[], Options &options)
//
//////////////////////////////////////////////////////////////
{
    SbBool	ok = TRUE;
    int		c, curArg;

    // Initialize options
    options.showBars		= FALSE;
    options.numFrames		= NUM_FRAMES;
    options.inputFileName	= NULL;
    options.windowX		= WINDOW_X;
    options.windowY		= WINDOW_Y;
    options.hasLights		= FALSE;
    options.hasTextures		= FALSE;
    options.noClear		= FALSE;
    options.noMaterials		= FALSE;
    options.noTextures		= FALSE;
    options.oneTexture		= FALSE;
    options.noXforms		= FALSE;
    options.noFill		= FALSE;
    options.noVtxXforms		= FALSE;
    options.freeze		= FALSE;
    options.noLights		= FALSE;

    while ((c = getopt(argc, argv, "bf:w:")) != -1) {
	switch (c) {
	  case 'b':
	    options.showBars = TRUE;
	    break;
	  case 'f':
	    options.numFrames = atoi(optarg);
	    break;
	  case 'w':
	    sscanf(optarg, " %d , %d", &options.windowX, &options.windowY);
	    break;
	  default:
	    ok = FALSE;
	    break;
	}
    }

    curArg = optind;

    // Check for input filename at end
    if (curArg < argc)
	options.inputFileName = argv[curArg++];

    // Not enough or extra arguments? Error!
    if (options.inputFileName == NULL || curArg < argc)
	ok = FALSE;

    // Report options and file names
    if (ok) {
	printf("\n");
	system("hinv -c processor"); printf("\n");
	system("hinv -t memory"); printf("\n");
	system("hinv -c graphics"); printf("\n");
	printf("Reading graph from %s\n", options.inputFileName);
	printf("Number of frames: %d\n", options.numFrames);
	printf("Window size: %d x %d pixels\n", options.windowX, 
	       options.windowY);
	printf("\n");
    }

    return ok;
}

//////////////////////////////////////////////////////////////
//
// Description:
//    Callback used by openWindow() and by main()
//

static Bool
waitForNotify(Display *, XEvent *e, char *arg)
//
//////////////////////////////////////////////////////////////
{
    return ((e->type == MapNotify) || (e->type == UnmapNotify)) &&
	(e->xmap.window == (Window) arg);
}

//////////////////////////////////////////////////////////////
//
// Description:
//    Creates and initializes GL/X window.
//

static void
openWindow(Display *&display, Window &window, unsigned int windowX,
	   unsigned int windowY, char *title)
//
//////////////////////////////////////////////////////////////
{
    XVisualInfo			*vi;
    Colormap			cmap;
    XSetWindowAttributes	swa;
    GLXContext			cx;
    XEvent			event;
    static int			attributeList[] = {
	GLX_RGBA,
	GLX_RED_SIZE, 1,
	GLX_GREEN_SIZE, 1,
	GLX_BLUE_SIZE, 1,
	GLX_DEPTH_SIZE, 1,
	None,		// May be replaced w/GLX_DOUBLEBUFFER
	None,
    };

    display = XOpenDisplay(0);
    vi   = glXChooseVisual(display,
			   DefaultScreen(display),
			   attributeList);
    cx   = glXCreateContext(display, vi, 0, GL_TRUE);
    cmap = XCreateColormap(display,
			   RootWindow(display, vi->screen),
			   vi->visual, AllocNone);
    swa.colormap	= cmap;
    swa.border_pixel	= 0;
    swa.event_mask	= StructureNotifyMask;
    window = XCreateWindow(display,
	   RootWindow(display, vi->screen),
	   10, 10, windowX, windowY,
	   0, vi->depth, InputOutput, vi->visual,
	   (CWBorderPixel | CWColormap | CWEventMask), &swa);

    // Make the window appear in the lower left corner
    XSizeHints *xsh = XAllocSizeHints();
    xsh->flags = USPosition;
    XSetWMNormalHints(display, window, xsh);
    XSetStandardProperties(display, window, title, title, None, 0, 0, 0);
    XFree(xsh);

    XMapWindow(display, window);
    XIfEvent(display, &event, waitForNotify, (char *) window);
    glXMakeCurrent(display, window, cx);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5, 0.5, 0.5, 1);
    
    // clear the graphics window and depth planes
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//////////////////////////////////////////////////////////////
//
// Description:
//    Creates and returns scene graph containing given
//    scene. Adds a perspective camera, a directional
//    light, and a transform.
//    Returns NULL on error.
//

static SoSeparator *
setUpGraph(const SbViewportRegion &vpReg,
	   SoInput *sceneInput,
	   Options &options)
//
//////////////////////////////////////////////////////////////
{

    // Create a root separator to hold everything. Turn
    // caching off, since the transformation will blow
    // it anyway.
    SoSeparator *root = new SoSeparator;
    root->ref();
    root->renderCaching = SoSeparator::OFF;

    // Add a camera to view the scene
    SoPerspectiveCamera *camera = new SoPerspectiveCamera;
    root->addChild(camera);

    // Add a transform node to spin the scene
    SoTransform *sceneTransform = new SoTransform;
    sceneTransform->setName(SCENE_XFORM_NAME);
    root->addChild(sceneTransform);

    // Read and add input scene graph
    SoSeparator *inputRoot = SoDB::readAll(sceneInput);
    if (inputRoot == NULL) {
	fprintf(stderr, "Cannot read scene graph\n");
	root->unref();
	exit(1);
    }
    root->addChild(inputRoot);

    SoPath 	*path;
    SoGroup	*parent, *group;
    SoSearchAction	act;

    // expand out all File nodes and replace them with groups
    //    containing the children
    SoFile 	*fileNode;
    act.setType(SoFile::getClassTypeId());
    act.setInterest(SoSearchAction::FIRST);
    act.apply(inputRoot);
    while ((path = act.getPath()) != NULL) {
	fileNode = (SoFile *) path->getTail();
	path->pop();
	parent = (SoGroup *) path->getTail();
	group = fileNode->copyChildren();
	if (group) {
	    parent->replaceChild(fileNode, group);
	    // apply action again and continue
	    act.apply(inputRoot);
	}
    }

    // expand out all node kits and replace them with groups
    //    containing the children
    SoBaseKit	*kitNode;
    SoChildList	*childList;
    act.setType(SoBaseKit::getClassTypeId());
    act.setInterest(SoSearchAction::FIRST);
    act.apply(inputRoot);
    while ((path = act.getPath()) != NULL) {
	kitNode = (SoBaseKit *) path->getTail();
	path->pop();
	parent = (SoGroup *) path->getTail();
	group = new SoGroup;
	childList = kitNode->getChildren();
	for (int i=0; i<childList->getLength(); i++) 
	    group->addChild((*childList)[i]);
	parent->replaceChild(kitNode, group);
	act.apply(inputRoot);
    }

    // check to see if there are any lights
    // if no lights, add a directional light to the scene
    act.setType(SoLight::getClassTypeId());
    act.setInterest(SoSearchAction::FIRST);
    act.apply(inputRoot);
    if (act.getPath() == NULL) { // no lights
	SoDirectionalLight *light = new SoDirectionalLight;
	root->insertChild(light, 1);
    }
    else 
	options.hasLights = TRUE;

    // check to see if there are any texures in the scene
    act.setType(SoTexture2::getClassTypeId());
    act.setInterest(SoSearchAction::FIRST);
    act.apply(inputRoot);
    if (act.getPath() != NULL)
	options.hasTextures = TRUE;

    camera->viewAll(root, vpReg);

    // print out information about the scene graph

    int32_t numTris, numLines, numPoints, numNodes;
    countPrimitives( inputRoot, numTris, numLines, numPoints, numNodes );
    printf("Number of nodes in scene graph:     %d\n", numNodes );
    printf("Number of triangles in scene graph: %d\n", numTris );
    printf("Number of lines in scene graph:     %d\n", numLines );
    printf("Number of points in scene graph:    %d\n\n", numPoints );

    // Make the center of rotation the center of
    // the scene
    SoGetBoundingBoxAction	bba(vpReg);
    bba.apply(root);
    sceneTransform->center = bba.getBoundingBox().getCenter();

    return root;
}


//////////////////////////////////////////////////////////////
//
// Description:
//    Replaces all group nodes in the graph with new ones
//    to reset autocaching threshold in separators.  Recursive.
//

SoNode *
replaceSeparators(SoNode *root)
//
//////////////////////////////////////////////////////////////
{
    //
    // if it's a group, make a new group and copy its
    //  children over
    //
    if (root->isOfType(SoGroup::getClassTypeId())) {
	SoGroup *group = (SoGroup *) root;
        SoGroup *newGroup = (SoGroup *) group->getTypeId().createInstance();
        newGroup->SoNode::copyContents(group, FALSE);

	int	i;
	for (i=0; i<group->getNumChildren(); i++) {
	    SoNode *child = replaceSeparators(group->getChild(i));
	    newGroup->addChild(child);
	}
	return newGroup;
    }
    //
    // if not a group, return the node
    //
    else 
	return root;
}

//////////////////////////////////////////////////////////////
//
// Description:
//    Removes nodes of the given type from the given graph.
//

void
removeNodes(SoGroup *root, SoType type)
//
//////////////////////////////////////////////////////////////
{
    SoSearchAction act;
    act.setInterest(SoSearchAction::ALL);
    act.setType(type);
    act.apply(root);
    SoPathList &paths = act.getPaths();
    for (int i = 0; i < paths.getLength(); i++) {
	SoNode *kid = paths[i]->getTail();
	paths[i]->pop();
	SoGroup *parent = (SoGroup *)paths[i]->getTail();
	parent->removeChild(kid);
    }
}

//////////////////////////////////////////////////////////////
//
// Description:
//    Prints usage message.
//

static void
printUsage(const char *progName)
//
//////////////////////////////////////////////////////////////
{
    fprintf(stderr,
	    "Usage: %s [-b] [-f N] [-w X,Y] datafile\n",
	    progName);
    fprintf(stderr,
	    "\t-b      display results as bar chart\n"
	    "\t-f N    render N frames for each test\n"
	    "\t-w X,Y  make window size X by Y pixels\n");
}


//////////////////////////////////////////////////////////////
//
// Description:
//    Returns the time taken PER FRAME to render the scene 
//    graph anchored at root
//    Different types of rendering performance are measured
//    depending on options
//    

static float
timeRendering(Options &options,
	      const SbViewportRegion &vpr,
	      SoSeparator *&root)
//
//////////////////////////////////////////////////////////////
{
    SbTime 		timeDiff, startTime;
    int 		frameIndex;
    SoTransform		*sceneTransform;
    SoGLRenderAction 	ra(vpr);
    SoNodeList		noCacheList;
    SoSeparator 	*newRoot;

    // clear the window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //
    // reset autocaching threshold before each experiment
    //   done by replacing every separator in the scene graph
    //   with a new one
    //
    newRoot = (SoSeparator *) replaceSeparators(root);
    newRoot->ref();
    newRoot->renderCaching = SoSeparator::OFF;

    // get a list of separators marked as being touched by the application
    newRoot->getByName(NO_CACHE_NAME, noCacheList);

    // find the transform node that spins the scene
    SoNodeList	xformList;
    newRoot->getByName(SCENE_XFORM_NAME, xformList);
    sceneTransform = (SoTransform *) xformList[0];

    if (options.noMaterials) {  // nuke material node
	removeNodes(newRoot, SoMaterial::getClassTypeId());
	removeNodes(newRoot, SoPackedColor::getClassTypeId());
	removeNodes(newRoot, SoBaseColor::getClassTypeId());
    }

    if (options.noXforms) {  // nuke transforms
	removeNodes(newRoot, SoTransformation::getClassTypeId());
    }

    if (options.noTextures || options.oneTexture) {  // override texture node

	removeNodes(newRoot, SoTexture2::getClassTypeId());

	if (options.oneTexture) {
	    // texture node with simple texture
	    static unsigned char img[] = {
		255, 255, 0, 0,
		255, 255, 0, 0,
		0, 0, 255, 255,
		0, 0, 255, 255
		};
	    SoTexture2 *overrideTex = new SoTexture2;	
	    overrideTex->image.setValue(SbVec2s(4, 4), 1, img);
	    newRoot->insertChild(overrideTex, 1);
	}
    }

    if (options.noFill) {  // draw as points
	SoDrawStyle *overrideFill = new SoDrawStyle;
	overrideFill->style.setValue(SoDrawStyle::POINTS);
	overrideFill->lineWidth.setIgnored(TRUE);
	overrideFill->linePattern.setIgnored(TRUE);
	overrideFill->setOverride(TRUE);
	newRoot->insertChild(overrideFill, 0);

	// cull backfaces so that extra points don't get drawn
	SoShapeHints *cullBackfaces = new SoShapeHints;
	cullBackfaces->shapeType = SoShapeHints::SOLID;
	cullBackfaces->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
	cullBackfaces->setOverride(TRUE);
	newRoot->insertChild(cullBackfaces, 0);
    }

    if (options.noVtxXforms) {  // draw invisible
	SoDrawStyle *overrideVtxXforms = new SoDrawStyle;
	overrideVtxXforms->style.setValue(SoDrawStyle::INVISIBLE);
	overrideVtxXforms->setOverride(TRUE);
	newRoot->insertChild(overrideVtxXforms, 0);
    }

    if (options.noLights) {  // set lighting model to base color
	SoLightModel *baseColor = new SoLightModel;
	baseColor->model = SoLightModel::BASE_COLOR;
	newRoot->insertChild(baseColor, 0);
    }
 
    for (frameIndex = 0; ; frameIndex++) {

	// wait till autocaching has kicked in then start timing
	if (frameIndex == NUM_FRAMES_AUTO_CACHING)
	    startTime = SbTime::getTimeOfDay();

	// stop timing and exit loop when requisite number of
	//    frames have been drawn
	if (frameIndex == options.numFrames + NUM_FRAMES_AUTO_CACHING) {
	    glFinish();
	    timeDiff = SbTime::getTimeOfDay() - startTime;
	    break;
	}
	    
	// if not frozen, update realTime and destroy labelled caches
	if (! options.freeze) { 

	    // update realTime 
	    SoSFTime *realTime = (SoSFTime *) SoDB::getGlobalField("realTime");
	    realTime->setValue(SbTime::getTimeOfDay());

	    // touch the separators marked NoCache 
	    for (int i=0; i<noCacheList.getLength(); i++)
		((SoSeparator *) noCacheList[i])->getChild(0)->touch();
	}

	// Rotate the scene
	sceneTransform->rotation.setValue(SbVec3f(1, 1, 1), 
                           frameIndex * 2 * M_PI / options.numFrames);

	if (! options.noClear)
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ra.apply(newRoot);
    }

    // Get rid of newRoot
    newRoot->unref();

    return (timeDiff.getValue() / options.numFrames);
}



#include "BarChart.h"

//////////////////////////////////////////////////////////////
//
// Description:
//    Uses Chris Marrin's BarChart node to display timing info
//

static void
drawBar(float asisTime, float noClearTime, float noMatTime,
	float noXformTime, float noTexTime, float oneTexTime,
	float noLitTime, float ptsTime, float invisTime, float freezeTime)
//
//////////////////////////////////////////////////////////////
{
    float barValues[10];
    static const char *barXLabels[] = { "Total", "Clear", "Trav", 
				  "Mat", "Xform", "Tex",
				  "TMgmt", "Lit", "Pipe",
				  "Fill" };
    static const char *barYLabels[] = { "seconds/frame" };
    int i;

    Widget barWindow = SoXt::init("Timing display");
    
    BarChart::initClass();

    SoSeparator *root = new SoSeparator;
    SoTransform *xform = new SoTransform;
    BarChart *bar = new BarChart;

    root->ref();
    root->addChild(xform);
    root->addChild(bar);

    xform->scaleFactor.setValue(2.0, 1.0, 0.5);

    barValues[0] = asisTime;
    barValues[1] = asisTime - noClearTime;
    barValues[2] = asisTime - freezeTime;
    barValues[3] = asisTime - noMatTime;
    barValues[4] = asisTime - noXformTime;
    barValues[5] = oneTexTime - noTexTime;
    barValues[6] = asisTime - oneTexTime;
    barValues[7] = asisTime - noLitTime;
    barValues[8] = ptsTime - invisTime;
    barValues[9] = asisTime - ptsTime;

    for (i=1; i<10; i++) {
	bar->valueColors.set1Value(i, 1, 1, 0, 0);
	bar->xLabelColors.set1Value(i, 1, 1, 0, 0);
	if (barValues[i] < 0) barValues[i] = 0;
    }
    bar->values.setValues(0, 10, barValues);
    bar->xLabels.setValues(0, 10, barXLabels);
    bar->yLabels.setValues(0, 1, barYLabels);
    bar->xDimension = 10;
    bar->yDimension = 1;
    bar->minValue = 0.0;
    bar->maxValue = asisTime;
    bar->xLabelScale.setValue(0.6, 1, 1);
    bar->yLabelScale.setValue(2, 1, 1);
    bar->zLabelIncrement = asisTime / 4;
	
    SoXtExaminerViewer *viewer = new SoXtExaminerViewer(barWindow);
    viewer->setTitle("Timing Display");
    viewer->setSceneGraph(root);
    viewer->show();
    
    SoXt::show(barWindow);
    SoXt::mainLoop(); 
}



//////////////////////////////////////////////////////////////
//
// Description:
//    Mainline
//

main(int argc, char **argv)
//
//////////////////////////////////////////////////////////////
{
    Options		options;
    SoSeparator		*root;
    Display		*display;
    Window		window;
    float		asisTime, ptsTime, invisTime, freezeTime,
                        noMatTime, noTexTime, noLitTime, noXformTime,
                        noClearTime, oneTexTime;

    // Init Inventor
    SoInteraction::init();

    // Parse arguments
    if (! parseArgs(argc, argv, options)) {
	printUsage(argv[0]);
	return 1;
    }

    // Open scene graphs
    SoInput	sceneInput;
    if (! sceneInput.openFile(options.inputFileName)) {
	fprintf(stderr,
		"Cannot open %s\n", options.inputFileName);
	return 1;
    }

    SbViewportRegion vpr(options.windowX, options.windowY);
    root = setUpGraph(vpr, &sceneInput, options);

    // Create and initialize window
    openWindow(display, window, options.windowX, options.windowY, argv[0]);

    // Timing tests
   
    printf("\t\t\tseconds/frame\tframes/second\n");
    // as is rendering
    asisTime = timeRendering(options, vpr, root);
    printf("As-Is rendering:\t%10.3f\t%10.2f\n", asisTime, 1.0/asisTime);


    // time for rendering without clear
    options.noClear = TRUE;
    noClearTime = timeRendering(options, vpr, root);
    options.noClear = FALSE;
    printf("No Clear:\t\t%10.3f\t%10.2f\n", noClearTime, 1.0/noClearTime);
    
    // time for rendering without materials
    options.noMaterials = TRUE;
    noMatTime = timeRendering(options, vpr, root);
    options.noMaterials = FALSE;
    printf("No Materials:\t\t%10.3f\t%10.2f\n", noMatTime, 1.0/noMatTime);
    
    // time for rendering without xforms
    options.noXforms = TRUE;
    noXformTime = timeRendering(options, vpr, root);
    options.noXforms = FALSE;
    printf("No Transforms:\t\t%10.3f\t%10.2f\n", noXformTime, 1.0/noXformTime);

    if (options.hasTextures) { // do tests only if scene has textures

	// time for rendering without any textures
	options.noTextures = TRUE;
	noTexTime = timeRendering(options, vpr, root);
	options.noTextures = FALSE;
	printf("No Textures:\t\t%10.3f\t%10.2f\n", noTexTime, 1.0/noTexTime);

	// time for rendering without only one texture
	options.oneTexture = TRUE;
	oneTexTime = timeRendering(options, vpr, root);
	options.oneTexture = FALSE;
	printf("One Texture:\t\t%10.3f\t%10.2f\n", oneTexTime, 1.0/oneTexTime);
    }
    else 
	noTexTime = oneTexTime = asisTime;


    // time for rendering without fill
    options.noFill = TRUE;
    ptsTime = timeRendering(options, vpr, root);
    options.noFill = FALSE;    
    printf("No Fills:\t\t%10.3f\t%10.2f\n", ptsTime, 1.0/ptsTime);

    if (options.hasLights) { // do test only if scene has lights
	// time for rendering with lights turned off
	options.noLights = TRUE;
	noLitTime = timeRendering(options, vpr, root);
	options.noLights = FALSE;
	printf("No Lights:\t\t%10.3f\t%10.2f\n", noLitTime, 1.0/noLitTime);
    }
    else
	noLitTime = asisTime;

    printf("\n");

    // time for rendering without vertex xforms
    options.noVtxXforms = TRUE;
    invisTime = timeRendering(options, vpr, root);
    options.noVtxXforms = FALSE;
    printf("Time taken by vertex transformations:\t%10.3f seconds/frame\n", 
	   ptsTime-invisTime);
    
    // time for rendering with scene graph frozen
    options.freeze = TRUE;
    freezeTime = timeRendering(options, vpr, root);
    options.freeze = FALSE;
    printf("Time taken to traverse scene graph:\t%10.3f seconds/frame\n\n", 
	   asisTime-freezeTime);


    // kill window
    XEvent 	event;
    XUnmapWindow(display, window);
    XIfEvent(display, &event, waitForNotify, (char *) window);

    // draw timing bars
    if (options.showBars) 
	drawBar(asisTime, noClearTime, noMatTime,
		noXformTime, noTexTime, oneTexTime,
		noLitTime, ptsTime, invisTime, freezeTime);

    return 0;
}
