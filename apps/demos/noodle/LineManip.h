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
// A very specialized node; this manipulator completely takes care of
// editing a set of line segments.
//

#include <Inventor/SbLinear.h>
#include <Inventor/SbViewportRegion.h>
#include <Inventor/nodes/SoSeparator.h>

class SbColor;
class SbPlaneProjector;
class SbLineProjector;
class SbVec3f;
class SoCoordinate3;
class SoDrawStyle;
class SoFieldSensor;
class SoHandleEventAction;
class SoLineSet;
class SoLocation2Event;
class SoMaterial;
class SoPath;
class SoPointSet;
class SoSwitch;
class SoTransform;

class LineManip2 : public SoSeparator
{
    SO_NODE_HEADER(LineManip2);

  public:
    LineManip2();
    virtual ~LineManip2();

    //
    // Return the coordinates being edited.  Put a data sensor on
    // these to get notified whenever they are changed.
    //
    SoCoordinate3 *getCoordinate3();
    void setCoordinate3( SoCoordinate3 *newNode );

  //
  // THIS MUST BE CALLED BEFORE THE CLASS IS USED!
  // (SoInteraction::init() calls this for all of the built-in
  // manipulator)
  //
    static void initClass();

    //
    // Get rid of any hilights.  This can be called when the mouse
    // leaves the window, for example.
    //
    virtual void removeHilights();

    //
    // Handle an event.  This is meant to be internal only.
    //
    virtual void handleEvent(SoHandleEventAction *);

    //
    // Controls how big the feedback is.  Default is
    // 0.05.
    //
    virtual void setHilightSize(float);

    // Sets the normal to the projection plane.
    void setPlaneNormal( const SbVec3f &newNormal );

  SoEXTENDER public:

    // Records the viewport size and viewVolume at the time when the
    // manipulator grabs the events.
    // Automatically called by the handleEventAction when this manip 
    // grabs events with ha->setGrabber(this)
    // Also turns renderCaching off during the time we are the grabber.
    virtual void grabEventsSetup();
    virtual void grabEventsCleanup();

#ifdef __C_PLUS_PLUS_2
    enum LineManipPart {
#else
    enum Part {
#endif
	POINTS,
	LINES,
	ENDPOINTS,
	NOTHING
    };

  private:
    SoCoordinate3 *coord, *hilightCoord;
    int current_coord;
    float hilightSize;
    SbVec3f current_position;
    SoPointSet *pset;
    SoDrawStyle *drawStyle;
    SoLineSet *lset;
    SbPlaneProjector *planeProj;
    SbLineProjector  *lineProj;

    SoTransform *hilightTransform;
    SoMaterial *hilightMaterial;
    SoSwitch *hilightSwitch;

    // This sensor will insure that the pset and lset nodes will have
    // the correct number of points.
    SoFieldSensor *coordFieldSensor;
    static void    coordFieldSensorCB( void *, SoSensor *);

#ifdef __C_PLUS_PLUS_2
    LineManipPart whichPart();
#else
    Part whichPart();
#endif
    SbBool dragStart();
    SbBool remove();
    void updateProjectors( const SbVec3f &curPt );
    void projectMouse(SbVec3f &);
    
    SbBool locateHilight();
    void moveCoord();
    void initHilightStuff();
    void hilightVertex(const SbVec3f &, const SbColor &);
    void hilightLine(int, const SbVec3f &, const SbColor &);

    SbVec3f planeNormal;

    CacheEnabled savedRenderCachingVal;
    SbViewVolume        myViewVolume;
    SbViewportRegion    myVpRegion;
    SoHandleEventAction *myHandleEventAction;
    void extractViewingParams( SoHandleEventAction *);
};
