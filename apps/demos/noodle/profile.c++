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
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoMaterial.h>

#include <Xm/Text.h>

#include "LineManip.h"
#include "GeneralizedCylinder.h"

// This file defines: 
// 
// char *myGridBuffer
// char *myProfileGridBuffer
// char *myGridXYBuffer
//
#include "MyGrids.h"

//
// The line manipulator.
//
static LineManip2 *profileLineManip = NULL;
static LineManip2 *crossSectionLineManip = NULL;
static LineManip2 *spineLineManip = NULL;
static LineManip2 *twistLineManip = NULL;

//
// This is called when the mouse leaves a line manip window.  It turns
// off that line manip's hilights.
//
static void
leaveLineManipWindow(Widget, XtPointer mydata, XEvent *, Boolean *cont)
{
    ((LineManip2 *)mydata)->removeHilights();

    *cont = TRUE;	// Continue dispatching this event
}


SoSeparator * 
createLineGraphCommon( SoPerspectiveCamera *&c, LineManip2 *&theManip, 
		       Widget theWidget, char *gridFileName, 
		       char *gridDfltBuf )
{
    SoSeparator *result = new SoSeparator;
    result->ref();

    //!!!
    // UGLY HACK! TEMPORARY BUG FIX!
    // There is some kind of memory stomping bug going on here.
    // If you call this twice, something weird happens with the old version.
    // The old line manip, which should just disappear when the old scene
    // gets unref'ed, apparently retains a reference to the old coordinate
    // node. Anyhow, I'm not sure what's wrong, but referencing the
    // whole scene graph an extra stops the core dump.
    // But it should cause a pretty bad memory leak.
    // It saves a whole scene graph it should get rid of, each time a new
    // generalized cylinder is created or selected.
    //XXX
    //???
    //!!!
    result->ref();
    //!!!
    //???
    //XXX

    c = new SoPerspectiveCamera;
    result->addChild(c);

    c->nearDistance = 0.5;
    c->farDistance = 10.0;
    c->focalDistance = 5.0;	
    c->heightAngle = 1.04;

    // Axes and labels (appear underneath the line manipulator)
    SoInput in;
    SoSeparator *g1;
    if (gridFileName != NULL && in.openFile(gridFileName, TRUE) != FALSE)
	g1 = SoDB::readAll(&in);
    else
	g1 = NULL;

    if (g1 == NULL && gridDfltBuf != NULL ) {
	// If you couldn't find the file, use the default buffer...
	g1 = new SoSeparator;
	SoInput in;
	in.setBuffer((void *) gridDfltBuf, (size_t)strlen(gridDfltBuf));
	SoNode *n;
	while ((SoDB::read(&in,n) != FALSE) && (n!=NULL))
	    g1->addChild(n);
    }
    if (g1 != NULL)
    {
	result->addChild(g1);
    }

    SoSeparator *manipGroup = new SoSeparator;
    result->addChild(manipGroup);

    SoLightModel *lm = new SoLightModel;
    manipGroup->addChild(lm);
    lm->model = SoLightModel::BASE_COLOR;
 
    SoMaterial *m = new SoMaterial;
    m->diffuseColor.setValue(0.0, 0.8, 0.8);
    manipGroup->addChild(m);

    // Remove event handler for old manip, if there was one.
    if ( theManip != NULL )
	XtRemoveEventHandler(theWidget, LeaveWindowMask,
			  FALSE, leaveLineManipWindow, (XtPointer)theManip);

    theManip = new LineManip2;
    theManip->setHilightSize(0.1);
    manipGroup->addChild(theManip);

    // Add event handler for new manip.
    XtAddEventHandler(theWidget, LeaveWindowMask,
		      FALSE, leaveLineManipWindow, (XtPointer)theManip);

    result->unrefNoDelete();

    return result;
}

//
// This creates the profile's scene graph and sets up the callback so
// the line manipulator turns off its hilights when the mouse leaves
// the profile window.
//
SoNode *
createProfileGraph(Widget profileWidget, GeneralizedCylinder *surface)
{
    SoPerspectiveCamera *c;

    SoSeparator *topNode = createLineGraphCommon( c, profileLineManip, 
			 profileWidget, "ProfileGrid.iv", myProfileGridBuffer );
    c->position.setValue(3.0, 0.0, 5.0);

    // Get the coordinate node from the surface node...
    SoCoordinate3 *c3 = NULL;
    if (surface)
	c3 = SO_GET_PART( surface, "profileCoords", SoCoordinate3 );
    profileLineManip->setCoordinate3( c3 ); 

    return topNode;
}

SoNode *
createCrossSectionGraph(Widget crossSectionWidget, GeneralizedCylinder *surface)
{
    SoPerspectiveCamera *c;

    SoSeparator *topNode = createLineGraphCommon( c, crossSectionLineManip, 
			     crossSectionWidget, "GridXY.iv", myGridXYBuffer );
    c->position.setValue(0.0, 5.0, 0.0);
    c->orientation.setValue(SbVec3f(1,0,0), -1.57079 );

    crossSectionLineManip->setPlaneNormal(SbVec3f( 0, 1, 0 ) );

    // Get the coordinate node from the surface node...
    SoCoordinate3 *c3 = NULL;
    if (surface)
	c3 = SO_GET_PART( surface, "crossSectionCoords", SoCoordinate3 );
    crossSectionLineManip->setCoordinate3( c3 ); 

    return topNode;
}
SoNode *
createSpineGraph(Widget spineWidget, GeneralizedCylinder *surface)
{
    SoPerspectiveCamera *c;

    SoSeparator *topNode = createLineGraphCommon( c, spineLineManip, 
				     spineWidget, "Grid.iv", myGridBuffer );
    c->position.setValue(0.0, 0.0, 5.0);

    // Get the coordinate node from the surface node...
    SoCoordinate3 *c3 = NULL;
    if (surface)
	c3 = SO_GET_PART( surface, "spineCoords", SoCoordinate3 );
    spineLineManip->setCoordinate3( c3 ); 

    return topNode;
}
SoNode *
createTwistGraph(Widget twistWidget, GeneralizedCylinder *surface)
{
    SoPerspectiveCamera *c;

    SoSeparator *topNode = createLineGraphCommon( c, twistLineManip, 
				     twistWidget, "Grid.iv", myGridBuffer );
    c->position.setValue(0.0, 0.0, 5.0);

    // Get the coordinate node from the surface node...
    SoCoordinate3 *c3 = NULL;
    if (surface)
	c3 = SO_GET_PART( surface, "twistCoords", SoCoordinate3 );
    twistLineManip->setCoordinate3( c3 ); 

    return topNode;
}

//
// Called to get rid of all the points added so far.
//
void
clearProfilePoints()
{
    SoCoordinate3 *coord = profileLineManip->getCoordinate3();
    coord->point.deleteValues(0);
}

void
clearCrossSectionPoints()
{
    SoCoordinate3 *coord = crossSectionLineManip->getCoordinate3();
    coord->point.deleteValues(0);
}

void
clearSpinePoints()
{
    SoCoordinate3 *coord = spineLineManip->getCoordinate3();
    coord->point.deleteValues(0);
}
void
clearTwistPoints()
{
    SoCoordinate3 *coord = twistLineManip->getCoordinate3();
    coord->point.deleteValues(0);
}
void
makeReverseSection()
{
    SoCoordinate3 *coord = crossSectionLineManip->getCoordinate3();

    int numCoords = coord->point.getNum();

    SbVec3f temp;

    for (int first = 0, last = numCoords-1;
	 first < last;
	first++, last-- ){

	temp = coord->point[first];
	coord->point.set1Value(first, coord->point[last]);
	coord->point.set1Value(last, temp);
    }
}

void
makeCircularSection()
{
    SoCoordinate3 *coord = crossSectionLineManip->getCoordinate3();
    coord->point.enableNotify(FALSE);
    coord->point.deleteValues(0);

    int numSegments = 35;
    float maxAngle = 2 * 3.14159;

    coord->point.insertSpace(0,numSegments);

    SbVec3f newPoint(0,0,0);
    float   theta;
    for (int i = 0; i < numSegments; i++ ) {
	theta = ((float)i/(float)(numSegments-1)) * maxAngle;
	newPoint[0] = cos( theta );
	newPoint[2] = sin( theta );
	coord->point.set1Value(i,newPoint);
    }
    coord->point.enableNotify(TRUE);
    coord->point.touch();
}

void
makeSemiCircularSpine()
{
    SoCoordinate3 *coord = spineLineManip->getCoordinate3();
    coord->point.enableNotify(FALSE);
    coord->point.deleteValues(0);

    int numSegments = 50;
    float maxAngle = 3.14159;

    coord->point.insertSpace(0,numSegments);

    SbVec3f newPoint(0,0,0);
    float   theta;
    for (int i = 0; i < numSegments; i++ ) {
	theta = ((float)i / (float)(numSegments-1)) * maxAngle;
	newPoint[0] = cos( theta );
	newPoint[1] = sin( theta );
	coord->point.set1Value(i,newPoint);
    }
    coord->point.enableNotify(TRUE);
    coord->point.touch();
}
void
makeCircularSpine()
{
    SoCoordinate3 *coord = spineLineManip->getCoordinate3();
    coord->point.enableNotify(FALSE);
    coord->point.deleteValues(0);

    int numSegments = 50;
    float maxAngle = 2 * 3.14159;

    coord->point.insertSpace(0,numSegments);

    SbVec3f newPoint(0,0,0);
    float   theta;
    for (int i = 0; i < numSegments; i++ ) {
	theta = ((float)i/(float)(numSegments-1)) * maxAngle;
	newPoint[0] = cos( theta );
	newPoint[1] = sin( theta );
	coord->point.set1Value(i,newPoint);
    }
    coord->point.enableNotify(TRUE);
    coord->point.touch();
}
