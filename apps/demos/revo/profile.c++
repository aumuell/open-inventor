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
// Some code for setting up and dealing with the profile window.
//

#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>

#include <Inventor/Xt/SoXt.h>
#include <Inventor/SoDB.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/sensors/SoNodeSensor.h>
#include <Inventor/sensors/SoSensor.h>

#include <Xm/Text.h>

#include "LineManip.h"
#include "Useful.h"
#include "RevClass.h"

//
// The line manipulator.
//
static LineManip2 *TheLineManip;

//
// The default coordinates for the line manipulator.
//
static float defaultCoords[2][3] =
{
    { 0.5,  0.2, 0.0 },
    { 0.5, -0.2, 0.0 },
};

//
// Grid for window:
//
static char *GridString =
"#Inventor V2.0 ascii\n"
"Separator {"
"	PickStyle { style UNPICKABLE }"
"	LightModel { model BASE_COLOR }"
"	BaseColor { rgb 0.2 0.2 0.2 }"
"	Array {"
"		numElements1 19"
"		separation1 .1 0 0"
"		origin CENTER"
"		Coordinate3 { point [ 0 -1 0, 0 1 0 ] }"
"		LineSet { numVertices [ 2 ] }"
"	}"
"	Array {"
"		numElements1 19"
"		separation1 0 .1 0"
"		origin CENTER"
"		Coordinate3 { point [ -1 0 0, 1 0 0 ] }"
"		LineSet { numVertices [ 2 ] }"
"	}"
"	BaseColor { rgb 0.4 0.0 0.0 }"
"	Coordinate3 { point [ -1.0 0.0 0.0, 1.0 0.0 0.0,"
"			0.0 -1.0 0.0, 0.0 1.0 0.0 ]"
"	}"
"	LineSet { numVertices [ 2, 2 ] }"
"}";

//
// This is called when the mouse leaves the profile window.  It turns
// off the line manip's hilights.
//
static void
leaveProfileWindow(Widget, XtPointer mydata, XEvent *, Boolean *cont)
{
    ((LineManip2 *)mydata)->removeHilights();

    *cont = TRUE;	// Continue dispatching this event
}

//
// Called to get rid of all the points added so far.
//
void
clearPoints()
{
    SoCoordinate3 *coord = TheLineManip->getCoordinate3();
    coord->point.deleteValues(0);
}

//
// Called whenever the profile is edited.  It tells the surface to
// recalculate itself.  Note that this could be more efficient by
// noticing exactly what changed and just recomputing what is
// necessary (e.g. when adding a point to the end, only two sets of
// normals needs to be generated, etc).
//
static void
profileChangedCallback(void *data, SoSensor *)
{
    RevolutionSurface *s = (RevolutionSurface *)data;

    SoCoordinate3 *coord = TheLineManip->getCoordinate3();
    s->createSurface(&coord->point);
}

//
// This creates the profile's scene graph and sets up the callback so
// the line manipulator turns off its hilights when the mouse leaves
// the profile window.
//
SoNode *
createProfileGraph(Widget profileWidget, RevolutionSurface *surface)
{
    SoSeparator *result = new SoSeparator;
    result->ref();

    SoOrthographicCamera *c = new SoOrthographicCamera;
    result->addChild(c);
    c->position.setValue(0.5, 0.0, 1.0);
    c->nearDistance = 0.5;
    c->farDistance = 1.5;
    c->focalDistance = 1.0;	
    c->height = 1.0;

    // Axes and labels (appear underneath the line manipulator)
    SoInput in;
    in.setBuffer(GridString, strlen(GridString));
    SoNode *node;	
    SoDB::read(&in, node);
    SoSeparator *g1 = (SoSeparator *)node;
    if (g1 != NULL)
    {
	result->addChild(g1);
   	g1->renderCaching = SoSeparator::ON;
   	g1->boundingBoxCaching = SoSeparator::ON;
    }

    SoSeparator *manipGroup = new SoSeparator;
    result->addChild(manipGroup);

    SoLightModel *lm = new SoLightModel;
    manipGroup->addChild(lm);
    lm->model = SoLightModel::BASE_COLOR;
 
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue(0.0, 0.8, 0.8);
    manipGroup->addChild(m);

    TheLineManip = new LineManip2;
    TheLineManip->setHilightSize(0.02);
    manipGroup->addChild(TheLineManip);
    SoCoordinate3 *coord = TheLineManip->getCoordinate3();
    SoNodeSensor *d = new SoNodeSensor(profileChangedCallback, surface);
    d->attach(coord);
    coord->point.setValues(0, 2, defaultCoords);

    //
    // Make sure surface of revolution and line manip both agree on
    // the default coordinates
    //
    surface->createSurface(&coord->point);

    XtAddEventHandler(profileWidget, LeaveWindowMask,
		      FALSE, leaveProfileWindow, (XtPointer)TheLineManip);

    result->unrefNoDelete();

    return result;
}

