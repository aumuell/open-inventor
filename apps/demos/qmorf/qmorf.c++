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

//
// Program to transform one quad mesh into another.  Linear
// interpolation of the vertices is done.  Transparent blending is
// also done to interpolate colors or textures on the objects (again,
// linearly).
//
// Written by Gavin Bell for Silicon Graphics
//
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/Xt/SoXtRenderArea.h>
#include <Inventor/SoDB.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/nodes/SoCallback.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoTextureCoordinate2.h>
#include <Inventor/nodes/SoTextureCoordinateBinding.h>
#include <Inventor/nodes/SoTextureCoordinatePlane.h>
#include <Inventor/sensors/SoIdleSensor.h>

#include "../../samples/common/InventorLogo.h"
#include "QuadThing.h"
#include "Background.h"

#include <Xm/Xm.h>
#include <Xm/Text.h>
#include <Xm/Form.h>
#include <Xm/ScrollBar.h>
#include <Xm/LabelG.h>
#include <Xm/PushBG.h>
#include <Xm/ToggleBG.h>

//
// The list of things we'll morph between
//

static QuadThingList objectList;

//
// The interpolated shape we'll constantly modify
//
static QuadThing *TheShape;

//
// Scale for slider bars
//
static const int SLIDER_MAX = 1000;

//
// Sensor to do automatic animation
//
static SoIdleSensor *animationSensor;

FlashBackground *background;
static int nobackground = 0;
static int notextures = 0;

//
// Create a scene graph that looks something like this:
//
//                    Separator root
//                   /              \ 
//                  /                \ 
//      Separator ToStuff         Separator FromStuff
//     /              |            |           |     |
// ToTexSwitch        |     FromTexSwitch   TransparentMaterial
//  /        \        |     /          \            |
// Texture Material   |  Texture    Material        /
//                    \                            /
//                     \                          /
//                      ...Separator for quadmesh shape...
//
// ... So the quadMesh gets drawn twice, to blend the two sets of
// colors.  The transparency field in the TransparentMaterial is used
// to control how much of the 'From' object is blended into the 'To'
// object.
//

static SoSeparator *ToStuff, *FromStuff;
static SoGroup *placeHolder;
static SoGroup *ToTexGroup, *FromTexGroup;
static SoGroup *ToMatGroup, *FromMatGroup;
static SoMaterial *TransparentMaterial;
static SoSwitch *ToTexSwitch, *FromTexSwitch;
static SoTexture2 *noopTexture;
static SoTextureCoordinatePlane *noopTextureCoord;

//
// Setup the scene graph when we change the shapes we are
// interpolating.  The topology of the scene graph is always the same,
// but we may replace nodes like materials or textures on the fly (if
// an object has no material or texture a placeholder node that does
// not affect anything is used).
//
void
setToFromStuff(int fromObject)
{
    int toObject = (fromObject+1) % objectList.getLength();

    SoNode *t;

    // Texture image:
    if ((t = objectList[fromObject]->getTexture2()) != NULL) {
	FromTexGroup->replaceChild(0, t);
	if (FromTexSwitch->whichChild.getValue() != 1)
	    FromTexSwitch->whichChild = 0;
    } else {
	FromTexGroup->replaceChild(0, noopTexture);
	if (FromTexSwitch->whichChild.getValue() != 1)
	    FromTexSwitch->whichChild = SO_SWITCH_ALL;
    }

    // Texture coordinates:
    if ((t = objectList[fromObject]->getTexCoord()) != NULL)
	FromTexGroup->replaceChild(1, t);
    else
	FromTexGroup->replaceChild(1, noopTextureCoord);

    // Texture coordinate binding:
    if ((t = objectList[fromObject]->getTexBinding()) != NULL)
	FromTexGroup->replaceChild(2, t);
    else
	FromTexGroup->replaceChild(2, placeHolder);

    // No texture, has materials (maybe)
    if ((t = objectList[fromObject]->getMaterial()) != NULL)
	FromMatGroup->replaceChild(0, t);
    else
	FromMatGroup->replaceChild(0, placeHolder);
    if ((t = objectList[fromObject]->getMatBinding()) != NULL)
	FromMatGroup->replaceChild(1, t);
    else
	FromMatGroup->replaceChild(1, placeHolder);

    if ((t = objectList[toObject]->getTexture2()) != NULL) {
	ToTexGroup->replaceChild(0, t);
	if (ToTexSwitch->whichChild.getValue() != 1)
	    ToTexSwitch->whichChild = 0;
    } else {
	ToTexGroup->replaceChild(0, noopTexture);
	if (ToTexSwitch->whichChild.getValue() != 1)
	    ToTexSwitch->whichChild = SO_SWITCH_ALL;
    }
    if ((t = objectList[toObject]->getTexCoord()) != NULL)
	ToTexGroup->replaceChild(1, t);
    else
	ToTexGroup->replaceChild(1, noopTextureCoord);
    if ((t = objectList[toObject]->getTexBinding()) != NULL)
	ToTexGroup->replaceChild(2, t);
    else
	ToTexGroup->replaceChild(2, placeHolder);
    if ((t = objectList[toObject]->getMaterial()) != NULL)
	ToMatGroup->replaceChild(0, t);
    else
	ToMatGroup->replaceChild(0, placeHolder);
    if ((t = objectList[toObject]->getMatBinding()) != NULL)
	ToMatGroup->replaceChild(1, t);
    else
	ToMatGroup->replaceChild(1, placeHolder);
}

//
// A bunch of stuff used to control the animation
//
static const int COLOR = 0;
static const int SHAPE = 1;
static int lastColor = (-1);
static double animationTime[2] = { 0.0, 0.0 };
static int whatIsAnimating[2] = { 1, 1 };
static Widget sliders[2];
static SbTime lastTime;

//
// Stop animation.  what is either COLOR or SHAPE.  Called by the UI
// widgets (the animation buttons or to stop the animation when a
// slider is moved).
//
void
stopAnimating(int what)
{
    whatIsAnimating[what] = 0;
    if (!(whatIsAnimating[0] || whatIsAnimating[1]))
	animationSensor->unschedule();
}
void
startAnimating(int what)
{
    whatIsAnimating[what] = 1;
    animationSensor->schedule();
    lastTime = SbTime::getTimeOfDay();
}

//
// Change the COLOR or SHAPE to the given value.  Called by the slider
// widgets.
//
void
setValue(int what, double value)
{
    animationTime[what] = fmod(value, objectList.getLength());

    double t = fmod(animationTime[what], 1.0);

    if (what == SHAPE)
    {
	int object1 = int(animationTime[SHAPE]);
	int object2 = (object1+1) % objectList.getLength();
	TheShape->interp(objectList[object1], objectList[object2], t);
    }
    else
    {
	// If we'll be using the colors of another shape, must change
	// scene graph:
	if (int(animationTime[COLOR]) != lastColor)
	{
	    lastColor = int(animationTime[COLOR]);
	    setToFromStuff(lastColor);
	}
	TransparentMaterial->transparency.setValue(t);
    }
}

//
// Called by a sensor to automatically change the objects into one
// another.  This also updates the slider widgets.
//
void
morfCallback(void *, SoSensor *sensor)
{
    static const double transition_time = 4.0;	// Seconds

    SbTime now = SbTime::getTimeOfDay();
    SbTime dt = (now - lastTime) / transition_time;
    static SbTime bgShape, bgColor;

    if (whatIsAnimating[COLOR])
    {
	setValue(COLOR, animationTime[COLOR] + dt.getValue());
	double t = animationTime[COLOR] / objectList.getLength();
	Arg resources[1];
	XtSetArg(resources[0], XmNvalue, t * SLIDER_MAX);
	XtSetValues(sliders[COLOR], resources, 1);
	bgColor += dt;
	if (!nobackground)
	    background->animateColor(bgColor.getValue());
    }
    if (whatIsAnimating[SHAPE])
    {
	setValue(SHAPE, animationTime[SHAPE] + dt.getValue());
	double t = animationTime[SHAPE] / objectList.getLength();
	Arg resources[1];
	XtSetArg(resources[0], XmNvalue, t * SLIDER_MAX);
	XtSetValues(sliders[SHAPE], resources, 1);
	bgShape += dt / 3.0;
	if (!nobackground)
	    background->animateShape(bgShape.getValue());
    }

    lastTime = now;
    sensor->schedule();
}

//
// These two callbacks cooperate to figure out if rendering with
// textures turned on is too slow.  The first is called from a
// callback node at the top of the scene graph, and notes the time
// when rendering starts.  The second is a render abort callback, and
// will turn off texturing if it is determined to be too slow.
// They both only check the first few frames so they don't slow
// traversal down excessively.
//
static int TimeSet = 0;
static SbTime renderTime;

static void
setTimeCallback(void *, SoAction *action)
{
    if (action->isOfType(SoGLRenderAction::getClassTypeId()) &&
	TimeSet < 4) {

	renderTime.setToTimeOfDay();

	if (TimeSet == 0) {
	    animationSensor->schedule();
	    lastTime = renderTime;
	}

	// This node must not be cached!
	SoCacheElement::invalidate(action->getState());

	++TimeSet;
    }
}
static SoGLRenderAction::AbortCode
checkTimeCallback(void *data)
{
    static int turnedOff = 0;

    // If textures aren't turned off already...
    if ((TimeSet > 1) && (TimeSet < 4) && !turnedOff) {
	SbTime delta = SbTime::getTimeOfDay() - renderTime;
	if (delta > SbTime(.5)) {
	    fprintf(stderr, "Disabling textures, "
		    "they are too slow\n");

	    // Turn off textures...
	    FromTexSwitch->whichChild = 1;
	    ToTexSwitch->whichChild = 1;
	    turnedOff = TRUE;
	    // And turn off render abort callback
	    SoGLRenderAction *ra = (SoGLRenderAction *)data;
	    ra->setAbortCallback(NULL, NULL);
	    return SoGLRenderAction::ABORT;
	}
    }
    return SoGLRenderAction::CONTINUE;
}

void
parse_args(int argc, char **argv)
{
    int err = 0;	// Flag: error in options?
    int c;

    // Note: optind and optarg are declared in getopt.h

    while ((c = getopt(argc, argv, "bth")) != -1)
    {
	switch(c)
	{
	  case 'b':
	    nobackground = 1;
	    break;
	  case 't':
	    notextures = 1;
	    break;
	  case 'h':	// Help
	  default:
	    err = 1;
	    break;
	}
    }
// Handle optional filenames
    for (; optind < argc; optind++)
    {
	QuadThing *q = new QuadThing(argv[optind]);
	if (q->getSceneGraph() != NULL)
	    objectList.append(q);
	else delete q;
    }
    if (objectList.getLength() < 2)
    {
	err = 1;
    }

    if (err)
    {
	fprintf(stderr, "Usage: %s [-bth] file file [file ...]\n", argv[0]);
	fprintf(stderr, "-b : No background\n");
	fprintf(stderr, "-t : No textures\n");
	fprintf(stderr, "-h : This message (help)\n");
	fprintf(stderr, "At least two files must be given, and "
		"they must contain QuadMeshes (qmorf can\n");
	fprintf(stderr, "only morph QuadMesh nodes).\n");
	fprintf(stderr, "The directory "
		IVPREFIX "/share/data/models/CyberHeads "
		"contains good data to morph.\n");

	exit(7);
    }
}

//
// Callback for quit button
//
void
quitCallback(Widget, XtPointer, XtPointer)
{
    exit(0);
}

//
// Callback for 'About...' button
//
void
showAboutDialog(Widget, XtPointer, XtPointer)
{
    if (access(IVPREFIX "/demos/Inventor/qmorf.about", R_OK) != 0)
    {
	system("xmessage 'Sorry, could not find "
	       IVPREFIX "/demos/Inventor/qmorf.about' > /dev/null");
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

    sprintf(command, "acroread " IVPREFIX "/demos/Inventor/qmorf.about &");
    system(command);
}	

//
// Callback for the animation buttons
//
static void
toggleAnimation(Widget, XtPointer mydata, XtPointer)
{
    if ((long)mydata == COLOR)
    {
	if (whatIsAnimating[COLOR])
	    stopAnimating(COLOR);
	else startAnimating(COLOR);
    }
    else
    {
	if (whatIsAnimating[SHAPE])
	    stopAnimating(SHAPE);
	else startAnimating(SHAPE);
    }
}

//
// Callbacks for color and shape sliders
//
static void
dragColorSlider(Widget, XtPointer mydata, XtPointer cbstruct)
{
    if (whatIsAnimating[COLOR])
    {
	stopAnimating(COLOR);
	Arg resources[1];
	XtSetArg(resources[0], XmNset, 0);
	XtSetValues((Widget)mydata, resources, 1);
    }
    int value = ((XmScrollBarCallbackStruct *)cbstruct)->value;
    double fv = value * objectList.getLength() / (double)SLIDER_MAX;
    setValue(COLOR, fv);
}
static void
dragShapeSlider(Widget, XtPointer mydata, XtPointer cbstruct)
{
    if (whatIsAnimating[SHAPE])
    {
	stopAnimating(SHAPE);
	Arg resources[1];
	XtSetArg(resources[0], XmNset, 0);
	XtSetValues((Widget)mydata, resources, 1);
    }
    int value = ((XmScrollBarCallbackStruct *)cbstruct)->value;
    double fv = value * objectList.getLength() / (double)SLIDER_MAX;
    setValue(SHAPE, fv);
}


////////////////////////////////////////////////////////////////////////
//
//  Draw the Inventor logo in the overlays.
//
////////////////////////////////////////////////////////////////////////

static void
logoCB(void *, SoAction *action)
{
    if (action->isOfType(SoGLRenderAction::getClassTypeId())) 
	glViewport(0, 0, 80, 80);	// See Dave Mott for details!
}
static void
setOverlayLogo(SoXtRenderArea *ra)
{
    static SoSeparator *logo = NULL;

    if (logo == NULL) {
        SoInput in;
        in.setBuffer((void *)ivLogo, ivLogoSize); // see common directory
        logo = SoDB::readAll(&in);
        logo->ref();
        SoCallback *cb = new SoCallback;    // sets GL viewport
        cb->setCallback(logoCB);
        logo->insertChild(cb, 0);
    }
    SbColor col(1, 1, 1);
    ra->setOverlayColorMap(1, 1, &col);
    ra->setOverlaySceneGraph(logo);
}


//////////////////////////////////////////////////////////////////////
//
// Create and layout all of the UI stuff (I'm really not very fond of
// Motif).
//
static SoXtExaminerViewer *
buildUI( Widget appWindow, 
	 SoNode *root, 
	 SoPerspectiveCamera *c)
//////////////////////////////////////////////////////////////////////
{
    //
    // Form that everything is part of...
    //
    Arg resources[20]; int n = 0;
    XtSetArg(resources[n], "width", 600); n++;
    XtSetArg(resources[n], "height", 600); n++;
    Widget form = XmCreateForm(appWindow, "form", resources, n); n = 0;

    //
    // 8 widgets at bottom of window:
    // 2 buttons, 2 labels, 2 sliders, and 2 toggle buttons
    //
    Widget w[8];

#define STRING(a) XmStringCreateSimple(a)
    XtSetArg(resources[n], XmNlabelString, STRING("Animate")); ++n;
    XtSetArg(resources[n], XmNrightAttachment, XmATTACH_FORM); ++n;
    XtSetArg(resources[n], XmNbottomAttachment, XmATTACH_FORM); ++n;
    XtSetArg(resources[n], XmNset, 1); ++n;
    w[2] = XmCreateToggleButtonGadget(form, "colorAnimate", resources,
				      n);
    XtAddCallback(w[2], XmNvalueChangedCallback,
		  toggleAnimation, (XtPointer)(unsigned long)COLOR);
    n = 0;
    XtSetArg(resources[n], XmNlabelString, STRING("Animate")); ++n;
    XtSetArg(resources[n], XmNrightAttachment, XmATTACH_FORM); ++n;
    XtSetArg(resources[n], XmNbottomAttachment, XmATTACH_WIDGET); ++n;
    XtSetArg(resources[n], XmNbottomWidget, w[2]); n++;
    XtSetArg(resources[n], XmNset, 1); ++n;
    w[5] = XmCreateToggleButtonGadget(form, "shapeAnimate", resources,
				      n);
    XtAddCallback(w[5], XmNvalueChangedCallback,
		  toggleAnimation, (XtPointer)(unsigned long)SHAPE);
    n = 0;

    XtSetArg(resources[n], XmNlabelString, STRING("Quit")); ++n;
    XtSetArg(resources[n], XmNleftAttachment, XmATTACH_FORM); ++n;
    XtSetArg(resources[n], XmNbottomAttachment, XmATTACH_FORM); ++n;
    w[7] = XmCreatePushButtonGadget(form, "quit", resources, n);
    XtAddCallback(w[7], XmNactivateCallback,
		  quitCallback, NULL);
    n = 0;
    XtSetArg(resources[n], XmNlabelString, STRING("About...")); ++n;
    XtSetArg(resources[n], XmNleftAttachment, XmATTACH_FORM); ++n;
    XtSetArg(resources[n], XmNbottomAttachment, XmATTACH_WIDGET); ++n;
    XtSetArg(resources[n], XmNbottomWidget, w[7]); ++n;
    w[6] = XmCreatePushButtonGadget(form, "about", resources, n);
    XtAddCallback(w[6], XmNactivateCallback,
		  showAboutDialog, NULL);
    n = 0;

    XtSetArg(resources[n], XmNlabelString, STRING("Color")); ++n;
    XtSetArg(resources[n], XmNleftAttachment, XmATTACH_WIDGET); ++n;
    XtSetArg(resources[n], XmNleftWidget, w[6]); n++;
    XtSetArg(resources[n], XmNbottomAttachment, XmATTACH_FORM); ++n;
    w[0] = XmCreateLabelGadget(form, "colorLabel", resources, n);
    n = 0;
    XtSetArg(resources[n], XmNlabelString, STRING("Shape")); ++n;
    XtSetArg(resources[n], XmNleftAttachment, XmATTACH_WIDGET); ++n;
    XtSetArg(resources[n], XmNleftWidget, w[6]); n++;
    XtSetArg(resources[n], XmNbottomAttachment, XmATTACH_WIDGET); ++n;
    XtSetArg(resources[n], XmNbottomWidget, w[2]); n++;
    XtSetArg(resources[n], XmNbottomOffset, 4); ++n;
    w[3] = XmCreateLabelGadget(form, "shapeLabel", resources, n);
    n = 0;

    XtSetArg(resources[n], XmNmaximum, SLIDER_MAX + SLIDER_MAX/20); ++n;
    XtSetArg(resources[n], XmNsliderSize, SLIDER_MAX/20); ++n;
    XtSetArg(resources[n], XmNincrement, SLIDER_MAX/40); ++n;
    XtSetArg(resources[n], XmNpageIncrement, SLIDER_MAX/20); ++n;
    XtSetArg(resources[n], XmNshowArrows, FALSE); ++n;
    XtSetArg(resources[n], XmNorientation, XmHORIZONTAL); ++n;
    int tn = n;
    XtSetArg(resources[n], XmNleftAttachment, XmATTACH_WIDGET); ++n;
    XtSetArg(resources[n], XmNleftWidget, w[3]); n++;
    XtSetArg(resources[n], XmNrightAttachment, XmATTACH_WIDGET); ++n;
    XtSetArg(resources[n], XmNrightWidget, w[2]); n++;
    XtSetArg(resources[n], XmNbottomAttachment, XmATTACH_FORM); ++n;
    XtSetArg(resources[n], XmNbottomOffset, 4); ++n;
    w[1] = XmCreateScrollBar(form, "colorScrollbar", resources, n);
    sliders[COLOR] = w[1];
    XtAddCallback(w[1], XmNdragCallback,
		  dragColorSlider, (XtPointer)w[2]);
    XtAddCallback(w[1], XmNvalueChangedCallback,
		  dragColorSlider, (XtPointer)w[2]);
    n = tn;
    XtSetArg(resources[n], XmNleftAttachment, XmATTACH_WIDGET); ++n;
    XtSetArg(resources[n], XmNleftWidget, w[3]); n++;
    XtSetArg(resources[n], XmNrightAttachment, XmATTACH_WIDGET); ++n;
    XtSetArg(resources[n], XmNrightWidget, w[5]); n++;
    XtSetArg(resources[n], XmNbottomAttachment, XmATTACH_WIDGET); ++n;
    XtSetArg(resources[n], XmNbottomWidget, w[1]); n++;
    XtSetArg(resources[n], XmNbottomOffset, 10); ++n;
    w[4] = XmCreateScrollBar(form, "shapeScrollbar", resources, n);
    sliders[SHAPE] = w[4];
    XtAddCallback(w[4], XmNdragCallback,
		  dragShapeSlider, (XtPointer)w[5]);
    XtAddCallback(w[4], XmNvalueChangedCallback,
		  dragShapeSlider, (XtPointer)w[5]);
    n = 0;
    
    //
    // Create viewer:
    //
    SoXtExaminerViewer *viewer = new SoXtExaminerViewer(form);
    viewer->setSceneGraph(root);
    viewer->setCamera(c);
    viewer->setTransparencyType(SoGLRenderAction::BLEND);
    viewer->setDecoration(FALSE);
    viewer->setPopupMenuEnabled(FALSE);
    viewer->setBorder(TRUE);
    viewer->setFeedbackVisibility(FALSE);
    viewer->setAutoClipping(FALSE);
    // Set the draw style to as is, otherwise we'll never see
    // any texture mapping on systems without graphics hardware
    viewer->setDrawStyle(SoXtViewer::STILL, SoXtViewer::VIEW_AS_IS);

    // Add Inventor logo to viewer
    setOverlayLogo( viewer );

    XtSetArg(resources[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(resources[n], XmNbottomWidget, w[6]); n++;
    XtSetValues(viewer->getWidget(), resources, n); n = 0;

    viewer->show();

    XtManageChildren(w, 8);

    XtManageChild(form);

    return viewer;
}

int
main(int argc, char **argv)
{
    Widget appWindow = SoXt::init(argv[0]);
    if ( appWindow == NULL ) exit( 1 );

    parse_args(argc, argv);

    TheShape = new QuadThing(objectList[0]);
    TheShape->interp(objectList[0], objectList[1], 0.0);

    SoSeparator *root = new SoSeparator;
    root->ref();

    // Add a shapeHints node
    SoShapeHints *hints = new SoShapeHints;
    hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    hints->faceType = SoShapeHints::CONVEX;
    root->insertChild(hints, 0);

    // Set up a callback node at the beginning of the scene graph that
    // will set renderTime, which is used to figure out if texturing
    // is too slow and should be turned off.
    SoCallback *setTime = new SoCallback;
    root->addChild(setTime);
    setTime->setCallback(setTimeCallback, NULL);

    SoPerspectiveCamera *camera = new SoPerspectiveCamera;
    root->addChild(camera);

    // And set up a no-op texture node.  The GL doesn't guarantee
    // pixel exactness if textured and non-textured polygons are
    // blended together, so we'll make sure all shapes are textured by
    // default (with a 1 by 1 all-white modulated texture, which won't
    // affect the shape's color at all!).
    noopTexture = new SoTexture2;
    noopTexture->ref();
    unsigned char whitePix[1]; whitePix[0] = 0xff;
    noopTexture->image.setValue(SbVec2s(1,1), 1, whitePix);
    // And an aribrary function for coords:
    noopTextureCoord = new SoTextureCoordinatePlane;
    noopTextureCoord->ref();

    ToStuff = new SoSeparator;
    root->addChild(ToStuff);
    FromStuff = new SoSeparator;
    root->addChild(FromStuff);
    placeHolder = new SoGroup;
    placeHolder->ref();
    ToTexSwitch = new SoSwitch;
    if (notextures)
	ToTexSwitch->whichChild = 1;
    else
	ToTexSwitch->whichChild = 0;
    ToStuff->addChild(ToTexSwitch);
    ToTexGroup = new SoGroup;
    ToTexSwitch->addChild(ToTexGroup);
    ToTexGroup->addChild(noopTexture);
    ToTexGroup->addChild(noopTextureCoord);
    ToTexGroup->addChild(placeHolder);  // Room for texCoordBinding
    ToMatGroup = new SoGroup;
    ToTexSwitch->addChild(ToMatGroup);
    ToMatGroup->addChild(placeHolder);  // Room for material
    ToMatGroup->addChild(placeHolder);  // Room for MaterialBinding
    
    ToStuff->addChild(TheShape->getSceneGraph());

    FromTexSwitch = new SoSwitch;
    FromStuff->addChild(FromTexSwitch);
    FromTexGroup = new SoGroup;
    FromTexSwitch->addChild(FromTexGroup);
    if (notextures)
	FromTexSwitch->whichChild = 1;
    else
	FromTexSwitch->whichChild = 0;
    FromTexGroup->addChild(noopTexture);
    FromTexGroup->addChild(noopTextureCoord);
    FromTexGroup->addChild(placeHolder); // Room for texCoordBinding
    FromMatGroup = new SoGroup;
    FromTexSwitch->addChild(FromMatGroup);
    FromMatGroup->addChild(placeHolder); // Room for material
    FromMatGroup->addChild(placeHolder); // Room for materialBinding

    TransparentMaterial = new SoMaterial;
    TransparentMaterial->diffuseColor.setIgnored(TRUE);
    TransparentMaterial->ambientColor.setIgnored(TRUE);
    TransparentMaterial->specularColor.setIgnored(TRUE);
    TransparentMaterial->emissiveColor.setIgnored(TRUE);
    TransparentMaterial->shininess.setIgnored(TRUE);
    TransparentMaterial->transparency.setValue(0.0);
    FromStuff->addChild(TransparentMaterial);
    FromStuff->addChild(TheShape->getSceneGraph());

    setToFromStuff(0);

    camera->viewAll(root, SbVec2s(1,1));
    camera->nearDistance = 0.1;
    camera->farDistance = 500.0;

    if (!nobackground)
    {
	background = new FlashBackground(20);
	root->addChild(background->getSceneGraph());
    }

    SoXtExaminerViewer *viewer = buildUI(appWindow, root, camera);

    // Setup a render abort to turn off texturing if it is too slow:
    viewer->getGLRenderAction()->setAbortCallback(checkTimeCallback,
					  viewer->getGLRenderAction());

    animationSensor = new SoIdleSensor(morfCallback, NULL);
    // The animation sensor is scheduled the first time the scene is
    // rendered.

    SoXt::show( appWindow );
    SoXt::mainLoop();

    return 0;
}
