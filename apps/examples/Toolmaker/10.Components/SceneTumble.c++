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

/*--------------------------------------------------------------
 *  This is an example from the Inventor Toolmaker,
 *  chapter 10.
 * 
 *  This shows off several concepts:
 *      + the use of 'buildNow' in the constructor
 *      + the use of the visibility change callback
 *      + registering the widget
 *      + getting resources with XtResource
 *
 *  Source file for "SceneTumble" component.
 *------------------------------------------------------------*/

#include <Xm/Form.h>
#include <Xm/Scale.h>
#include <Inventor/Xt/SoXtResource.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/sensors/SoTimerSensor.h>
#include "SceneTumble.h"

#define MIN_SPEED 0
#define MAX_SPEED 100

// speed factor is a small angle
#define SPEED_FACTOR (M_PI/3600.0)

//
// Public constructor.
//

SceneTumble::SceneTumble(
   Widget parent,
   const char *name, 
   SbBool buildInsideParent)
    : SoXtRenderArea(parent, name, buildInsideParent, 
		    TRUE, TRUE, FALSE) // FALSE = don't build just yet
{
   // Passing TRUE means build the component right now
   constructorCommon(TRUE);
}


//
// Protected constructor for subclasses to call.
//

SceneTumble::SceneTumble(
   Widget parent,
   const char *name, 
   SbBool buildInsideParent, 
   SbBool buildNow)
    : SoXtRenderArea(parent, name, buildInsideParent, FALSE, FALSE)
{
   // Subclass tells us whether to build now
   constructorCommon(buildNow);
}


//
// Actual work done at construction time.
//

void
SceneTumble::constructorCommon(SbBool buildNow)
{
   speed = MAX_SPEED/2;
   
   animationSensor = 
     new SoTimerSensor(SceneTumble::animationSensorCB, this);
   animationSensor->setInterval(1/60.0); // 60 frames per second

   userScene = NULL;
   root = new SoSeparator;
   camera = new SoPerspectiveCamera;
   rotx = new SoRotation;
   roty = new SoRotation;
   rotz = new SoRotation;
   
   root->addChild(camera);
   root->addChild(new SoDirectionalLight);
   root->addChild(rotx);
   root->addChild(roty);
   root->addChild(rotz);
   root->ref();
   
   addVisibilityChangeCallback(visibilityChangeCB, this);
   setClassName("SceneTumble");
   setTitle("Tumble");

   // If we do not build now, the subclass will build when ready
   if (buildNow) {
     Widget w = buildWidget(getParentWidget());
     setBaseWidget(w);
   }
}


//
// Destructor.
//

SceneTumble::~SceneTumble()
{
   root->unref();
   delete animationSensor;
}


//
// Set the scene graph to tumble. We add this scene graph
// to our local graph so that we can rotate our own camera
// to create the tumbling effect. Our local scene graph
// root is passed to the render area for rendering.
//

void
SceneTumble::setSceneGraph(SoNode *newScene)
{
   // Replace the existing scene with this one
   if (userScene != NULL)
        root->replaceChild(userScene, newScene);
   else root->addChild(newScene);
   userScene = newScene;
   
   // Make certain the scene is in view
   camera->viewAll(root, getViewportRegion(), 2.0);
   
   // Render area will handle redraws for us
   SoXtRenderArea::setSceneGraph(root);
}


//
// Return the users scene graph, not our local graph.
//

SoNode *
SceneTumble::getSceneGraph()
{
   return userScene;
}


//
// Build the widget - create a form widget, and place
// in it a render area and a scale slider to control
// the speed.
//

Widget
SceneTumble::buildWidget(Widget parent)
{
   Arg args[8];
   int n;

   // Create a form widget as the container.
   Widget form = XtCreateWidget(getWidgetName(), xmFormWidgetClass, 
             parent, NULL, 0);
   
   // Register the widget, so we can get resources
   registerWidget(form);
   
   // Get our starting speed from the resource.
   // Resource file should say:
   //    *SceneTumble*speed: <int between 0 and 100>
   short s;
   SoXtResource xr(form);
   if (xr.getResource("speed", "Speed", s)) {
      if (s > MAX_SPEED)
         speed = MAX_SPEED;
      else if (s < MIN_SPEED)
         speed = MIN_SPEED;
      else 
         speed = s;
   }
     
   // Create render area
   Widget raWidget = SoXtRenderArea::buildWidget(form);

   // Create slider to control speed
   n = 0;
   XtSetArg(args[n], XmNminimum, MIN_SPEED); n++;
   XtSetArg(args[n], XmNmaximum, MAX_SPEED); n++;
   XtSetArg(args[n], XmNvalue, speed); n++;
   XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
   speedSlider =
     XtCreateWidget("Speed", xmScaleWidgetClass, form, args, n);

   // Callbacks on the slider
   XtAddCallback(speedSlider, XmNdragCallback,
     SceneTumble::speedCB, this);
   XtAddCallback(speedSlider, XmNvalueChangedCallback,
     SceneTumble::speedCB, this);
   
   // Layout 
   n = 0;
   XtSetArg(args[n], XmNtopAttachment,	    XmNONE); n++;
   XtSetArg(args[n], XmNleftAttachment,	    XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_FORM); n++;
   XtSetValues(speedSlider, args, n);
   
   n = 0;
   XtSetArg(args[n], XmNtopAttachment,	    XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNleftAttachment,	    XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNrightAttachment,    XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNbottomAttachment,   XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNbottomWidget,	    speedSlider); n++;
   XtSetValues(raWidget, args, n);
   
   // Make the widgets visible
   XtManageChild(speedSlider);
   XtManageChild(raWidget);
   
   return form;
}


// 
// Do the tumble animation. This entails updating our three
// rotation nodes, one each for the x,y,and z axes.
//

void
SceneTumble::doTumbleAnimation()
{ 
   SbRotation r;
   float angle;
   
   // Rotate about three axes in three speeds
   angle = speed * SPEED_FACTOR;
   r = rotx->rotation.getValue() * SbRotation(SbVec3f(1, 0, 0), angle);
   rotx->rotation.setValue(r);

   angle = speed * SPEED_FACTOR * 1.5;
   r = roty->rotation.getValue() * SbRotation(SbVec3f(0, 1, 0), angle);
   roty->rotation.setValue(r);

   angle = speed * SPEED_FACTOR * 2.0;
   r = rotz->rotation.getValue() * SbRotation(SbVec3f(0, 0, 1), angle);
   rotz->rotation.setValue(r);
}


//
// Turn tumbling on and off. We simply schedule or unschedule
// the animation sensor.
//

void
SceneTumble::setTumbling(SbBool onOff)
{ 
   if (onOff) 
        animationSensor->schedule();
   else animationSensor->unschedule();
}


// 
// Return whether we are tumbling.
//

SbBool
SceneTumble::isTumbling() const
{
   return animationSensor->isScheduled();
}


// 
// This is called when the render area visibility changes
// because it is shown, hidden, or iconified. If the 
// component is not visible, we turn off the tumble animation.
// 

void
SceneTumble::visibilityChangeCB(void *userData, SbBool visible)
{
   // Set tumbling on when the component is visible,
   // and set it off when the component is not visible.
   SceneTumble *tumbler = (SceneTumble *) userData;   
   tumbler->setTumbling(visible);
}


// 
// Animation sensor callback keeps the tumbling going.
//

void
SceneTumble::animationSensorCB(void *userData, SoSensor *)
{ 
   ((SceneTumble *) userData)->doTumbleAnimation();
}


// 
// This is invoked when the speed slider changes value.
// We use the value of the slider to change the tumble speed.
//

void
SceneTumble::speedCB(Widget, XtPointer userData, XtPointer clientData)
{
   SceneTumble *tumbler = (SceneTumble *) userData;
   XmScaleCallbackStruct *data = (XmScaleCallbackStruct *) clientData;
   tumbler->setSpeed(data->value);
}

