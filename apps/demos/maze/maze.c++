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
//  maze - this sample program displays a maze with a steel ball that
//  the user maneuvers through to the end.
//
//  See the README file in this directory for a complete explanation.
//

#include <stdlib.h>
#include <stdio.h>
#include <X11/Intrinsic.h>
#include <math.h>

#include <Inventor/SoInput.h>
#include <Inventor/SoDB.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/events/SoLocation2Event.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCallback.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoText3.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoTextureCoordinateEnvironment.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTriangleStripSet.h>
#include <Inventor/sensors/SoAlarmSensor.h>
#include <Inventor/sensors/SoTimerSensor.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>

#include "../../samples/common/InventorLogo.h"

#include "box.h"
#include "mazes.h"
#ifndef NO_AUDIO
#include "PlayClass.h"
#endif

#define GRAVITATIONAL_CONSTANT 50.0
#define WALL_THICKNESS         0.08
#define WALL_HEIGHT            0.6
#define BALL_RADIUS            0.32
#define HOLE_RADIUS            0.38
#define GAME_WIDTH             11.0
#define GRID_RESOLUTION        11
#define GRID_RES2              5.5
#define EDGE_RESOLUTION        12
#define WALL_DAMPENING         10.0
#define HOLE_SEGMENTS          8
#define ROTATION_LIMIT         0.2
#define MAX_ELAPSED_TIME       0.06

// #define NO_HOLES

static float stdDist = WALL_THICKNESS + BALL_RADIUS;
static int   startGridRow = 0;
static int   startGridCol = 7;
static int   currentGridRow = 0;
static int   currentGridCol = 7;
static SbBool isBallFalling = FALSE;

short   mazeRows[12][12];
short   mazeColumns[12][12];
short   mazeHoles[11][11];
SbBool  isMouseDown = FALSE;
SbVec2f mouseRotation;
SbVec2s mouseLocation;
SbVec2s startLocation, finishLocation;
SoRotationXYZ *rotationX, *rotationZ;
SoTranslation *startTrans, *finishTrans;
SoTranslation *ballTranslation;
SbTime  animationTime;
SbVec2f ballPosition;
SbVec2f lastBallPosition;
SbVec2f ballVelocity;
SbVec2f ballAcceleration;
float   ballHeight = 0.0;
float   dropVelocity = 0.0;
SbVec2f oneVector;

#ifndef NO_AUDIO
PlayClass *victory;
PlayClass *ballSound;

// Audio files have moved between IRIX 5.3 and IRIX 6.2
#if defined(LIBAUDIOFILE_VERSION) && LIBAUDIOFILE_VERSION == 2
static char *myVictorySound = IVPREFIX "/share/data/sounds/prosonus/musictags/tag3.aiff";
static char *myBallSound    = IVPREFIX "/share/data/sounds/prosonus/sfx/glass_break.aiff";
#else
static char *myVictorySound = "/usr/lib/sounds/prosonus/musictags/tag3.aiff";
static char *myballSound    = "/usr/lib/sounds/prosonus/sfx/glass_break.aiff";
#endif

#endif

SbBool  doneGame = FALSE;
SoCoordinate3 *mazeCoords;
SoIndexedFaceSet *mazeFaces;
SoCoordinate3 *holeCoords;
SoTriangleStripSet *holeStrips;
SoTimerSensor *timer;
SoAlarmSensor *resetTimer;


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Generates a wall of the maze given start and endpoints on the floor
//    of the maze.
//
// Use: private

static void
readMazeFile(
    const char *filename)

//
////////////////////////////////////////////////////////////////////////
{
    // Read the maze file and fill up the row and column arrays.
    FILE *fp;
    int i, j;

    if ((fp = fopen(filename, "r")) == NULL) {
        fprintf(stderr, "ERROR:  Could not initialize maze %s.\n", filename);
        exit(-1);
    }

    for (i=0; i<EDGE_RESOLUTION; i++)
        fscanf(fp, "%hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd", 
          &mazeRows[i][0], &mazeRows[i][1], &mazeRows[i][2], &mazeRows[i][3], 
          &mazeRows[i][4], &mazeRows[i][5], &mazeRows[i][6], &mazeRows[i][7], 
          &mazeRows[i][8], &mazeRows[i][9], &mazeRows[i][10], &mazeRows[i][11]);
 
    for (i=0; i<EDGE_RESOLUTION; i++) 
        fscanf(fp, "%hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd", 
          &mazeColumns[i][0], &mazeColumns[i][1], &mazeColumns[i][2], &mazeColumns[i][3], 
          &mazeColumns[i][4], &mazeColumns[i][5], &mazeColumns[i][6], &mazeColumns[i][7], 
          &mazeColumns[i][8], &mazeColumns[i][9], &mazeColumns[i][10], &mazeColumns[i][11]); 

    for (i=0; i<GRID_RESOLUTION; i++) 
        fscanf(fp, "%hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd", 
          &mazeHoles[i][0], &mazeHoles[i][1], &mazeHoles[i][2], &mazeHoles[i][3], 
          &mazeHoles[i][4], &mazeHoles[i][5], &mazeHoles[i][6], &mazeHoles[i][7], 
          &mazeHoles[i][8], &mazeHoles[i][9], &mazeHoles[i][10]); 

    // Find the starting and ending locations
    for (i=0; i<GRID_RESOLUTION; i++) {
        for (j=0; j<GRID_RESOLUTION; j++) {
            if (mazeHoles[i][j] == 2) {
                
                // This is the starting location.  Store the grid location
                // away and replace the 2 with a 0 to put floor there.
                startGridRow = i;
                startGridCol = j;
                mazeHoles[i][j] = 0;
            } else if (mazeHoles[i][j] == 3) {
                
                // This is the finishing location.  Store the grid location
                // away and replace the 3 with a 0 to put floor there.
                finishLocation[0] = i;
                finishLocation[1] = j;
                mazeHoles[i][j] = 0;
            }
        }
    }

    currentGridRow = startGridRow;
    currentGridCol = startGridCol;
    fclose(fp);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Generates a wall of the maze given start and endpoints on the floor
//    of the maze.
//
// Use: private

static void
setStandardMaze(
    int mazeNum )

//
////////////////////////////////////////////////////////////////////////
{
    int i, j;
    short *r, *c, *h;

    // Select the correct maze to build
    switch (mazeNum) {
        case 1:
            r = maze1Rows[0];
            c = maze1Columns[0];
            h = maze1Holes[0];
            break;
        case 2:
            r = maze2Rows[0];
            c = maze2Columns[0];
            h = maze2Holes[0];
            break;
        case 3:
            r = maze3Rows[0];
            c = maze3Columns[0];
            h = maze3Holes[0];
            break;
    }

    // Load the maze into the row, column, and hole arrays
    for (i=0; i<EDGE_RESOLUTION; i++)
        for (j=0; j<EDGE_RESOLUTION; j++)
            mazeRows[i][j] = *r++;

    for (i=0; i<EDGE_RESOLUTION; i++)
        for (j=0; j<EDGE_RESOLUTION; j++)
            mazeColumns[i][j] = *c++;

    for (i=0; i<GRID_RESOLUTION; i++) {
        for (j=0; j<GRID_RESOLUTION; j++, h++) {
            if (*h == 2) {
                
                // This is the starting location.  Store the grid location
                // away and replace the 2 with a 0 to put floor there.
                startGridRow = i;
                startGridCol = j;
                mazeHoles[i][j] = 0;
            } else if (*h == 3) {
                
                // This is the finishing location.  Store the grid location
                // away and replace the 3 with a 0 to put floor there.
                finishLocation[0] = i;
                finishLocation[1] = j;
                mazeHoles[i][j] = 0;
            } else {
                mazeHoles[i][j] = *h;
            }
        }
    }

    currentGridRow = startGridRow;
    currentGridCol = startGridCol;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Generates a wall of the maze given start and endpoints on the floor
//    of the maze.
//
// Use: private

static void
generateWall(
    const SbVec2f    &startCoord,
    const SbVec2f    &endCoord,
    SoCoordinate3    *coords,
    SoIndexedFaceSet *fset )

//
////////////////////////////////////////////////////////////////////////
{
    // Generate coordinates and add them to the coordinate node
    SbVec3f c;
    SbVec2f &start = (SbVec2f &)startCoord, &end = (SbVec2f &)endCoord;
    float   xwidth, zwidth;
    int     coordStartNum = coords->point.getNum();
    int     faceStartNum  = fset->coordIndex.getNum();
    int     normNum       = fset->normalIndex.getNum();
    int     cNum = coordStartNum;
    int     fNum = faceStartNum;
    int     cIndex;
    int     normShift;

    if (startCoord[1] == endCoord[1]) {

        // Wall is horizontal
        if (startCoord[0] > endCoord[0]) {
            start = (SbVec2f &)endCoord;
            end   = (SbVec2f &)startCoord;
        }
        xwidth = WALL_THICKNESS;
        zwidth = 0.0;
        normShift = 0;
    }
    else {

        // Wall is vertical
        if (startCoord[1] > endCoord[1]) {
            start = (SbVec2f &)endCoord;
            end   = (SbVec2f &)startCoord;
        }
        xwidth = 0.0;
        zwidth = WALL_THICKNESS;
        normShift = -1;
    }

    // Generate the Coordinates
    c.setValue(start[0] - xwidth - zwidth,
               0.0,
               start[1] + xwidth -zwidth);
    coords->point.set1Value(cNum++, c);
    c.setValue(end[0] + xwidth - zwidth,
               0.0,
               end[1] + xwidth + zwidth);
    coords->point.set1Value(cNum++, c);
    c.setValue(end[0] + xwidth + zwidth,
               0.0,
               end[1] - xwidth + zwidth);
    coords->point.set1Value(cNum++, c);
    c.setValue(start[0] - xwidth + zwidth,
               0.0,
               start[1] - xwidth - zwidth);
    coords->point.set1Value(cNum++, c);
    c.setValue(start[0] - xwidth - zwidth,
               WALL_HEIGHT,
               start[1] + xwidth - zwidth);
    coords->point.set1Value(cNum++, c);
    c.setValue(end[0] + xwidth - zwidth,
               WALL_HEIGHT,
               end[1] + xwidth + zwidth);
    coords->point.set1Value(cNum++, c);
    c.setValue(end[0] + xwidth + zwidth,
               WALL_HEIGHT,
               end[1] - xwidth + zwidth);
    coords->point.set1Value(cNum++, c);
    c.setValue(start[0] - xwidth + zwidth,
               WALL_HEIGHT,
               start[1] - xwidth - zwidth);
    coords->point.set1Value(cNum++, c);

    // Generate the faces  
    cIndex = coordStartNum + 3;
    fset->coordIndex.set1Value(fNum++, cIndex);
    cIndex = coordStartNum + 2;
    fset->coordIndex.set1Value(fNum++, cIndex);
    cIndex = coordStartNum + 1;
    fset->coordIndex.set1Value(fNum++, cIndex);
    cIndex = coordStartNum + 0;
    fset->coordIndex.set1Value(fNum++, cIndex);
    fset->coordIndex.set1Value(fNum++, SO_END_FACE_INDEX);
    fset->normalIndex.set1Value(normNum++, 0);

    cIndex = coordStartNum + 4;
    fset->coordIndex.set1Value(fNum++, cIndex);
    cIndex = coordStartNum + 5;
    fset->coordIndex.set1Value(fNum++, cIndex);
    cIndex = coordStartNum + 6;
    fset->coordIndex.set1Value(fNum++, cIndex);
    cIndex = coordStartNum + 7;
    fset->coordIndex.set1Value(fNum++, cIndex);
    fset->coordIndex.set1Value(fNum++, SO_END_FACE_INDEX);
    fset->normalIndex.set1Value(normNum++, 1);
 
    cIndex = coordStartNum + 0;
    fset->coordIndex.set1Value(fNum++, cIndex);
    cIndex = coordStartNum + 1;
    fset->coordIndex.set1Value(fNum++, cIndex);
    cIndex = coordStartNum + 5;
    fset->coordIndex.set1Value(fNum++, cIndex);
    cIndex = coordStartNum + 4;
    fset->coordIndex.set1Value(fNum++, cIndex);
    fset->coordIndex.set1Value(fNum++, SO_END_FACE_INDEX);
    fset->normalIndex.set1Value(normNum++, ((2 + normShift) == 1) ? 5 : 2);

    cIndex = coordStartNum + 1;
    fset->coordIndex.set1Value(fNum++, cIndex);
    cIndex = coordStartNum + 2;
    fset->coordIndex.set1Value(fNum++, cIndex);
    cIndex = coordStartNum + 6;
    fset->coordIndex.set1Value(fNum++, cIndex);
    cIndex = coordStartNum + 5;
    fset->coordIndex.set1Value(fNum++, cIndex);
    fset->coordIndex.set1Value(fNum++, SO_END_FACE_INDEX);
    fset->normalIndex.set1Value(normNum++, 3 + normShift);
        
    cIndex = coordStartNum + 2;
    fset->coordIndex.set1Value(fNum++, cIndex);
    cIndex = coordStartNum + 3;
    fset->coordIndex.set1Value(fNum++, cIndex);
    cIndex = coordStartNum + 7;
    fset->coordIndex.set1Value(fNum++, cIndex);
    cIndex = coordStartNum + 6;
    fset->coordIndex.set1Value(fNum++, cIndex);
    fset->coordIndex.set1Value(fNum++, SO_END_FACE_INDEX);
    fset->normalIndex.set1Value(normNum++, 4 + normShift);

    cIndex = coordStartNum + 3;
    fset->coordIndex.set1Value(fNum++, cIndex);
    cIndex = coordStartNum + 0;
    fset->coordIndex.set1Value(fNum++, cIndex);
    cIndex = coordStartNum + 4;
    fset->coordIndex.set1Value(fNum++, cIndex);
    cIndex = coordStartNum + 7;
    fset->coordIndex.set1Value(fNum++, cIndex);
    fset->coordIndex.set1Value(fNum++, SO_END_FACE_INDEX);
    fset->normalIndex.set1Value(normNum++, 5 + normShift);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Generates a hole in the maze given location on the floor
//    of the maze.
//
// Use: private

static void
generateHole(
    const SbVec2f      &location,
    SoCoordinate3      *coords,
    SoTriangleStripSet *sset )

//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f pt(location[0], 0.04, location[1] + HOLE_RADIUS);
    float incr = M_PI / HOLE_SEGMENTS;
    int cNum = coords->point.getNum();
 
    coords->point.set1Value(cNum++, pt);
    for (int i=1; i<HOLE_SEGMENTS; i++) {
        pt[0] = location[0] + sinf(i*incr) * HOLE_RADIUS;
        pt[2] = location[1] + cosf(i*incr) * HOLE_RADIUS;
        coords->point.set1Value(cNum++, pt);
        pt[0] = location[0] - sinf(i*incr) * HOLE_RADIUS;
        coords->point.set1Value(cNum++, pt);
    }
    pt[0] = location[0] + 0.0;
    pt[2] = location[1] - HOLE_RADIUS;
    coords->point.set1Value(cNum++, pt);
    sset->numVertices.set1Value(sset->numVertices.getNum(),
            (HOLE_SEGMENTS-1)*2+2);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Builds the maze by generating horizontal and vertical walls
//    from the maze row and column arrays.  Generate holes from
//    the array of hole positions.
//
// Use: private

static void
buildMaze(
    SoCoordinate3    *fcoords,
    SoCoordinate3    *scoords,
    SoIndexedFaceSet *fset,
    SoTriangleStripSet *sset )

//
////////////////////////////////////////////////////////////////////////
{
    int i, j, k;

    // Loop through the rows and columns arrays looking for contiguous
    // stretches of ON bits.  Form walls from the bits.
    SbVec2f startCoord, endCoord;

    for (i=0; i<EDGE_RESOLUTION; i++) {
        for (j=0; j<EDGE_RESOLUTION; j++) {
            if (mazeRows[i][j]) {
                startCoord.setValue(-GRID_RES2+(float)j, -GRID_RES2+(float)i);
                for (k=j+1; k<EDGE_RESOLUTION; k++) {
                    if (mazeRows[i][k] == 0)
                        break;                  
                }
                if (k == EDGE_RESOLUTION)
                    k--;
                endCoord.setValue(-GRID_RES2+(float)k, -GRID_RES2+(float)i);
                generateWall(startCoord, endCoord, fcoords, fset);
                j = k;
            }
        }
    }

    for (i=0; i<EDGE_RESOLUTION; i++) {
        for (j=0; j<EDGE_RESOLUTION; j++) {
            if (mazeColumns[j][i]) {
                startCoord.setValue(-GRID_RES2+(float)i, -GRID_RES2+(float)j);
                for (k=j+1; k<EDGE_RESOLUTION; k++) {
                    if (!mazeColumns[k][i])
                        break;                  
                }
                if (k == EDGE_RESOLUTION)
                    k--;
                endCoord.setValue(-GRID_RES2+(float)i, -GRID_RES2+(float)k);
                j = k;
                generateWall(startCoord, endCoord, fcoords, fset);
            }
        }
    }

#ifdef NO_HOLES
    // Loop through the array of holes, building holes for the maze
    for (i=0; i<GRID_RESOLUTION; i++)
        for (j=0; j<GRID_RESOLUTION; j++)
            mazeHoles[i][j] = 0;
#else
    // Loop through the array of holes, building holes for the maze
    SbVec3f st, ft;
    for (i=0; i<GRID_RESOLUTION; i++) {
        for (j=0; j<GRID_RESOLUTION; j++) {
            if (mazeHoles[i][j] == 1) {
                startCoord.setValue( -GRID_RES2 + (float)j + 0.5,
                                     -GRID_RES2 + (float)i + 0.5 );
                generateHole( startCoord, scoords, sset );
            }
            else if (mazeHoles[i][j] == 2) {
                st.setValue(-GRID_RES2 + j + 0.5, -GRID_RES2 + i + 0.5, 0.04 );
            }
            else if (mazeHoles[i][j] == 3) {
                ft.setValue(-GRID_RES2 + j + 0.5, -GRID_RES2 + i + 0.5, 0.04 );
            }
        }
    }
    startTrans->translation.setValue(st);
    finishTrans->translation.setValue(ft-st);
#endif
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback routine for receiving mouse button press to tilt the floor.
//    Schedule the timer sensor on mouse down.
//
// Use: private

static void
saveMouseLocation( void *, SoEventCallback *cb )

//
////////////////////////////////////////////////////////////////////////
{
    if (SO_MOUSE_PRESS_EVENT(cb->getEvent(), BUTTON1)) {

        const SbVec2s &pos = cb->getEvent()->getPosition();
        mouseLocation.setValue(pos[0], pos[1]);

        if (!timer->isScheduled()) {
            animationTime.setValue( SbTime::getTimeOfDay().getValue() );
            timer->schedule();
        }

        isMouseDown = TRUE;
        cb->setHandled();
    }
    else if (SO_MOUSE_RELEASE_EVENT(cb->getEvent(), BUTTON1)) {

        isMouseDown = FALSE;
        cb->setHandled();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback routine for receiving mouse motion to tilt the floor.
//
// Use: private

static void
computeFloorTilt( void *, SoEventCallback *cb )

//
////////////////////////////////////////////////////////////////////////
{
    if (!isMouseDown)
        return;

    const SbVec2s &pos = cb->getEvent()->getPosition();
    SbVec2s mouseMotion;
    short x = mouseLocation[0];
    short y = mouseLocation[1];

    mouseMotion[0] = x - pos[0];
    mouseMotion[1] = y - pos[1];
    mouseLocation.setValue((short)pos[0], (short)pos[1]);

    // Compute X and Z rotation angles depending on the mouse motion
    mouseRotation[0] += (float)(mouseMotion[1]) / 1000.0;
    mouseRotation[1] += (float)(mouseMotion[0]) / 1000.0;

    // Keep the rotation within bounds
    if (mouseRotation[0] > ROTATION_LIMIT)
        mouseRotation[0] = ROTATION_LIMIT;
    else if (mouseRotation[0] < -ROTATION_LIMIT)
        mouseRotation[0] = -ROTATION_LIMIT;
    if (mouseRotation[1] > ROTATION_LIMIT)
        mouseRotation[1] = ROTATION_LIMIT;
    else if (mouseRotation[1] < -ROTATION_LIMIT)
        mouseRotation[1] = -ROTATION_LIMIT;

    rotationX->angle = mouseRotation[0];
    rotationZ->angle = mouseRotation[1];
    cb->setHandled();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Resets the game parameters to their initial values to start
//    a new game.
//
// Use: private

static void
resetGame()

//
////////////////////////////////////////////////////////////////////////
{
    mouseLocation.setValue(0, 0);
    mouseRotation.setValue(0.0, 0.0);
    ballVelocity.setValue(0.0, 0.0);
    ballAcceleration.setValue(0.0, 0.0);
    ballPosition[0] = startGridCol + stdDist;
    ballPosition[1] = startGridRow + stdDist;
    lastBallPosition[0] = ballPosition[0];
    lastBallPosition[1] = ballPosition[1];
    ballHeight = 0.0;
    currentGridCol = startGridCol;
    currentGridRow = startGridRow;
    ballTranslation->translation.setValue(
        ballPosition[0] - GRID_RES2, BALL_RADIUS,
        ballPosition[1] - GRID_RES2);
    rotationX->angle = mouseRotation[0];
    rotationZ->angle = mouseRotation[1];
    isBallFalling = FALSE;
    doneGame = FALSE;
} 

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback routine for receiving keyboard events.
//
// Use: private

static void
processKeyEvents( void *, SoEventCallback *cb )

//
////////////////////////////////////////////////////////////////////////
{
    if (SO_KEY_PRESS_EVENT(cb->getEvent(), R)) {
    
        resetGame();
        cb->setHandled(); 
        return;    
    } 
    if (SO_KEY_PRESS_EVENT(cb->getEvent(), NUMBER_1)) {
    
        // Reset the game with standard maze 1
        mazeCoords->point.deleteValues(0);
        mazeFaces->coordIndex.deleteValues(0);
        mazeFaces->normalIndex.deleteValues(0);
        holeCoords->point.deleteValues(0);
        holeStrips->numVertices.deleteValues(0);
        setStandardMaze(1);
        buildMaze( mazeCoords, holeCoords, mazeFaces, holeStrips );

        resetGame();
        cb->setHandled(); 
        return;    
    } 
    if (SO_KEY_PRESS_EVENT(cb->getEvent(), NUMBER_2)) {
    
        // Reset the game with standard maze 2
        mazeCoords->point.deleteValues(0);
        mazeFaces->coordIndex.deleteValues(0);
        mazeFaces->normalIndex.deleteValues(0);
        holeCoords->point.deleteValues(0);
        holeStrips->numVertices.deleteValues(0);
        setStandardMaze(2);
        buildMaze( mazeCoords, holeCoords, mazeFaces, holeStrips );

        resetGame();
        cb->setHandled(); 
        return;    
    } 
    if (SO_KEY_PRESS_EVENT(cb->getEvent(), NUMBER_3)) {
    
        // Reset the game with standard maze 3
        mazeCoords->point.deleteValues(0);
        mazeFaces->coordIndex.deleteValues(0);
        mazeFaces->normalIndex.deleteValues(0);
        holeCoords->point.deleteValues(0);
        holeStrips->numVertices.deleteValues(0);
        setStandardMaze(3);
        buildMaze( mazeCoords, holeCoords, mazeFaces, holeStrips );

        resetGame();
        cb->setHandled(); 
        return;    
    } 
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Drop the ball into the hole.
//
// Use: private

void
dropBall( float et )

//
////////////////////////////////////////////////////////////////////////
{
    // When the ball has dropped far enough we don't need to drop the
    // ball any further.  We can also unschedule the timer sensor.
    if (ballHeight <= -2.0) {
	if (!isMouseDown)
	    timer->unschedule();

        // Schedule a timer to wait for a bit then reset the game back to
        // its initial position
        resetTimer->setTimeFromNow(SbTime(2.0));
        resetTimer->schedule();
        return;
    }
    
    // Calculate a new position for the ball
    SbVec2f newPosition;

    newPosition[0] = et * (ballVelocity[0] + 
            et * GRAVITATIONAL_CONSTANT * sinf(-rotationZ->angle.getValue()));
    newPosition[1] = et * (ballVelocity[1] + 
            et * GRAVITATIONAL_CONSTANT * sinf(rotationX->angle.getValue()));
    ballHeight     += et * (dropVelocity -  et * GRAVITATIONAL_CONSTANT);
    dropVelocity    = ballHeight / et;
    ballPosition   += newPosition;
    ballVelocity[0] = newPosition[0] / et;
    ballVelocity[1] = newPosition[1] / et;

    if (ballPosition[0] < (GRID_RES2-5.75+BALL_RADIUS))
        ballPosition[0] = GRID_RES2-5.75+BALL_RADIUS;
    if (ballPosition[0] > (GRID_RES2+5.75-BALL_RADIUS))
        ballPosition[0] =  GRID_RES2+5.75-BALL_RADIUS;
    if (ballPosition[1] < (GRID_RES2-5.75+BALL_RADIUS))
        ballPosition[1] = GRID_RES2-5.75+BALL_RADIUS;
    if (ballPosition[1] > (GRID_RES2+5.75-BALL_RADIUS))
        ballPosition[1] =  GRID_RES2+5.75-BALL_RADIUS;

    ballTranslation->translation.setValue(
            ballPosition[0] - GRID_RES2,
            BALL_RADIUS + ballHeight,
            ballPosition[1] - GRID_RES2);

#ifndef NO_AUDIO
    // play the drop sound 
    if (ballHeight <=-2.5) {
        sginap(20);
        ballSound->start();
    }
#endif
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback routine for resetting the game back to its initial state
//    after the ball has fallen into a hole.
//
// Use: private

static void
resetCB( void *, SoSensor * )

//
////////////////////////////////////////////////////////////////////////
{
    resetGame();
    resetTimer->unschedule();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback routine for animating the ball through a timer sensor.
//
// Use: private

static void
animateBall( void *, SoSensor * )

//
////////////////////////////////////////////////////////////////////////
{
    // Calculate the elapsed time since the last timer firing
    SbTime newTime;
    SbTime elapsedTime;

    newTime.setValue( SbTime::getTimeOfDay().getValue() );
    elapsedTime.setValue( (newTime - animationTime).getValue() );
    animationTime.setValue( (newTime).getValue() );
    float et = elapsedTime.getValue();
    
    // Check to see if elapsed time is too long due to a slow machine
    if (et > MAX_ELAPSED_TIME)
        et = MAX_ELAPSED_TIME;

    // If the ball is falling, perform its animation
    if (isBallFalling) {
        dropBall(et);
        return;
    }

    // Calculate a new position for the ball
    SbVec2f newPosition, tmpPosition;

    newPosition[0] = et * (ballVelocity[0] + 
            et * GRAVITATIONAL_CONSTANT * sinf(-rotationZ->angle.getValue()));
    newPosition[1] = et * (ballVelocity[1] + 
            et * GRAVITATIONAL_CONSTANT * sinf(rotationX->angle.getValue()));

    ballVelocity[0] = newPosition[0] / et;
    ballVelocity[1] = newPosition[1] / et;

    tmpPosition[0] = 1.0 + ballPosition[0] + newPosition[0];
    tmpPosition[1] = 1.0 + ballPosition[1] + newPosition[1];

    // Check the edges of the current grid cell to see if there are any 
    // walls present.  For each present wall, test the ball for intersection
    // and modify the ball position accordingly.
    
    int row, col;

    row  = currentGridRow;
    col  = currentGridCol;

    // Check for intersections against left and right grid boundaries
    if (((currentGridCol+1) != (int)(tmpPosition[0]-stdDist)) &&
        (ballVelocity[0] < 0.0)) {
        if (mazeColumns[row][col]) {
            tmpPosition[0] = 1.0 + col + stdDist;
            ballVelocity[0] /= -WALL_DAMPENING;
        }
    } else if (((currentGridCol+1) !=
                (int)(tmpPosition[0]+stdDist)) &&
                        (ballVelocity[0] > 0.0)) {
        if (mazeColumns[row][col+1]) {
            tmpPosition[0] = 1.0 + col + 1 - stdDist;
            ballVelocity[0] /= -WALL_DAMPENING;
        }
    }

    // Check for intersections against top and bottom grid boundaries
    if (((currentGridRow+1) != (int)(tmpPosition[1]-stdDist)) &&
        (ballVelocity[1] < 0.0)) {
        if (mazeRows[row][col]) {
            tmpPosition[1] = 1.0 + row + stdDist;
            ballVelocity[1] /= -WALL_DAMPENING;
        }
    } else if (((currentGridRow+1) !=
                (int)(tmpPosition[1]+stdDist)) &&
                        (ballVelocity[1] > 0.0)) {
        if (mazeRows[row+1][col]) {
            tmpPosition[1] = 1.0 + row + 1 - stdDist;
            ballVelocity[1] /= -WALL_DAMPENING;
        }
    }

    // Check for intersection against ends of other walls
    SbVec2f cornerPoint;
    SbVec2f cornerVector;

   // Check the top right corner of the grid cell for corners
    if ((mazeRows[currentGridRow][currentGridCol+1] ||
        mazeColumns[currentGridRow-1][currentGridCol+1]) &&
        !mazeRows[currentGridRow][currentGridCol] &&
        !mazeColumns[currentGridRow][currentGridCol+1]) {

        // Check the corner.
        SbVec2f center(currentGridCol+0.5, currentGridRow+0.5);
        cornerPoint[0] = currentGridCol + 1.0 - WALL_THICKNESS;
        cornerPoint[1] = currentGridRow + WALL_THICKNESS;
        cornerVector  = tmpPosition - oneVector - cornerPoint;

        // Check to see if the intersection is on the edge 
        // of the wall rather than the corner
        if ((cornerVector[1] <= 0.0) &&
            (cornerVector[0] >= -BALL_RADIUS)) {
            tmpPosition[0] = 1.0 + currentGridCol + 1 - stdDist;
            ballVelocity[0] /= -WALL_DAMPENING;
        }
        else if ((cornerVector[0] >= 0.0) && (cornerVector[1] <= BALL_RADIUS)){    
            tmpPosition[1] = 1.0 + currentGridRow + stdDist;
            ballVelocity[1] /= -WALL_DAMPENING;
        }
        else {
            float distFromCorner = fabs(cornerVector.normalize());
            float reflectionMagnitude;
            if (distFromCorner <= BALL_RADIUS) {
                reflectionMagnitude = -1.1 * cornerVector.dot(ballVelocity);
                ballVelocity += reflectionMagnitude * cornerVector;
                tmpPosition = oneVector+cornerPoint + BALL_RADIUS*cornerVector;
            }
        }
    }

    // Check the top left corner of the grid cell for corners
    if ((mazeRows[currentGridRow][currentGridCol-1] ||
        mazeColumns[currentGridRow-1][currentGridCol]) &&
        !mazeRows[currentGridRow][currentGridCol] &&
        !mazeColumns[currentGridRow][currentGridCol]) {

        // Check the corner.
        SbVec2f center(currentGridCol+0.5, currentGridRow+0.5);
        cornerPoint[0] = currentGridCol + WALL_THICKNESS;
        cornerPoint[1] = currentGridRow + WALL_THICKNESS;
        cornerVector  = tmpPosition - oneVector - cornerPoint;

        // Check to see if the intersection is on the edge 
        // of the wall rather than the corner
        if ((cornerVector[1] <= 0.0) &&
            (cornerVector[0] <= BALL_RADIUS)) {
            tmpPosition[0] = 1.0 + currentGridCol + stdDist;
            ballVelocity[0] /= -WALL_DAMPENING;
        }
        else if ((cornerVector[0] <= 0.0) && (cornerVector[1] <= BALL_RADIUS)){    
            tmpPosition[1] = 1.0 + currentGridRow + stdDist;
            ballVelocity[1] /= -WALL_DAMPENING;
        }
        else {
            float distFromCorner = fabs(cornerVector.normalize());
            float reflectionMagnitude;
            if (distFromCorner <= BALL_RADIUS) {
                reflectionMagnitude = -1.1 * cornerVector.dot(ballVelocity);
                ballVelocity += reflectionMagnitude * cornerVector;
                tmpPosition = oneVector+cornerPoint + BALL_RADIUS*cornerVector;
            }
        }
    }

     // Check the bottom right corner of the grid cell for corners
    if ((mazeRows[currentGridRow+1][currentGridCol+1] ||
        mazeColumns[currentGridRow+1][currentGridCol+1]) &&
        !mazeRows[currentGridRow+1][currentGridCol] &&
        !mazeColumns[currentGridRow][currentGridCol+1]) {

        // Check the corner.
        SbVec2f center(currentGridCol+0.5, currentGridRow+0.5);
        cornerPoint[0] = currentGridCol + 1.0 - WALL_THICKNESS;
        cornerPoint[1] = currentGridRow + 1.0 - WALL_THICKNESS;
        cornerVector  = tmpPosition - oneVector - cornerPoint;

        // Check to see if the intersection is on the edge 
        // of the wall rather than the corner
        if ((cornerVector[1] >= 0.0) &&
            (cornerVector[0] >= -BALL_RADIUS)) {
            tmpPosition[0] = 1.0 + currentGridCol + 1 - stdDist;
            ballVelocity[0] /= -WALL_DAMPENING;
        }
        else if ((cornerVector[0] >= 0.0) && (cornerVector[1] >= -BALL_RADIUS)){    
            tmpPosition[1] = 1.0 + currentGridRow + 1 - stdDist;
            ballVelocity[1] /= -WALL_DAMPENING;
        }
        else {
            float distFromCorner = fabs(cornerVector.normalize());
            float reflectionMagnitude;
            if (distFromCorner <= BALL_RADIUS) {
                reflectionMagnitude = -1.1 * cornerVector.dot(ballVelocity);
                ballVelocity += reflectionMagnitude * cornerVector;
                tmpPosition = oneVector+cornerPoint + BALL_RADIUS*cornerVector;
            }
        }
    }

    // Check the bottom left corner of the grid cell for corners
    if ((mazeRows[currentGridRow+1][currentGridCol-1] ||
        mazeColumns[currentGridRow+1][currentGridCol]) &&
        !mazeRows[currentGridRow+1][currentGridCol] &&
        !mazeColumns[currentGridRow][currentGridCol]) {

        // Check the corner.
        SbVec2f center(currentGridCol+0.5, currentGridRow+0.5);
        cornerPoint[0] = currentGridCol + WALL_THICKNESS;
        cornerPoint[1] = currentGridRow + 1.0 - WALL_THICKNESS;
        cornerVector  = tmpPosition - oneVector - cornerPoint;

        // Check to see if the intersection is on the edge 
        // of the wall rather than the corner
        if ((cornerVector[1] >= 0.0) &&
            (cornerVector[0] <= BALL_RADIUS)) {
            tmpPosition[0] = 1.0 + currentGridCol + stdDist;
            ballVelocity[0] /= -WALL_DAMPENING;
        }
        else if ((cornerVector[0] <= 0.0) && (cornerVector[1] >= -BALL_RADIUS)){    
            tmpPosition[1] = 1.0 + currentGridRow + 1 - stdDist;
            ballVelocity[1] /= -WALL_DAMPENING;
        }
        else {
            float distFromCorner = fabs(cornerVector.normalize());
            float reflectionMagnitude;
            if (distFromCorner <= BALL_RADIUS) {
                reflectionMagnitude = -1.1 * cornerVector.dot(ballVelocity);
                ballVelocity += reflectionMagnitude * cornerVector;
                tmpPosition = oneVector+cornerPoint + BALL_RADIUS*cornerVector;
            }
        }
    }

    ballPosition[0] = tmpPosition[0] - 1.0;
    ballPosition[1] = tmpPosition[1] - 1.0;

    // Check to see if the ball has moved since last time.
    // If it's still in the same place and the mouse button is up
    // then we can unschedule the timer sensor.
    if (ballPosition == lastBallPosition && !isMouseDown) {
        timer->unschedule(); 
    }

    currentGridRow = (int)(ballPosition[1]);
    currentGridCol = (int)(ballPosition[0]);

    if (!doneGame)
        if ((currentGridRow == finishLocation[1]) &&
            (currentGridCol == finishLocation[0])) {

            // Play the Victory Audio 
#ifndef NO_AUDIO   
            victory->start();
#endif
            doneGame = TRUE;
        }

    // Check to see if the ball has run into a hole
    if (mazeHoles[currentGridRow][currentGridCol] != 0) {

        SbVec2f center(currentGridCol+0.5,
                       currentGridRow+0.5);
        SbVec2f fallVector = center - ballPosition;
        float distFromHoleCenter = fabs(fallVector.length());
        if (distFromHoleCenter <= HOLE_RADIUS) {
            isBallFalling = TRUE;

            // Adjust the ball velocity to have it start dropping
            SbVec2f changeVelocity = (fallVector - ballVelocity); 
            ballHeight    = - sinf(HOLE_RADIUS-distFromHoleCenter);
            dropVelocity  = ballHeight / et;
            ballVelocity += changeVelocity * 0.5;                
        }
    }

    ballTranslation->translation.setValue(
            ballPosition[0] - GRID_RES2,
            .35 + ballHeight,
            ballPosition[1] - GRID_RES2);
 
    lastBallPosition[0] = ballPosition[0];
    lastBallPosition[1] = ballPosition[1];
}


////////////////////////////////////////////////////////////////////////
//
//  Set the Inventor logo on the screen.
//
////////////////////////////////////////////////////////////////////////

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

int
main(int argc, char *argv[])
{
    Widget              mainWindow;

    // Initialize Inventor
    mainWindow = SoXt::init("Inventor Maze");
    SoDB::setDelaySensorTimeout(SbTime(0.0));

#ifndef NO_AUDIO
    // Initialize the audio for the victory sound
    victory = new PlayClass("Victory");
    victory->setFilename(myVictorySound);
    ballSound = new PlayClass("BallSound");
    ballSound->setFilename(myBallSound);
#endif

    oneVector.setValue(1.0, 1.0);

    // Setup the scene graph for the rolling ball
    SoSeparator   *ballRoot        = new SoSeparator;
    SoSeparator   *ballCache       = new SoSeparator;
    SoMaterial    *ballMaterial    = new SoMaterial;
    SoSphere      *ball            = new SoSphere;

    ballTranslation = new SoTranslation;
    ball->radius = BALL_RADIUS;
    ballMaterial->shininess = .8;
    ballMaterial->ambientColor.setValue(SbColor(.4, .4, .4));
    ballMaterial->diffuseColor.setValue(SbColor(0.4, 0.4, 0.4));
    ballMaterial->specularColor.setValue(SbColor(1, 1, 1));
    ballCache->renderCaching = SoSeparator::ON;
    ballCache->addChild(ballMaterial);
    ballCache->addChild(ball);
    ballRoot->addChild(ballTranslation);
    ballRoot->addChild(ballCache);

    // Setup a timer sensor to animate the ball
    timer = new SoTimerSensor(animateBall, NULL);
    timer->setInterval(SbTime(0.03));
    resetTimer = new SoAlarmSensor(resetCB, NULL);

    // Generate a scene graph representing the maze specified by the array
    SoSeparator *mazeRoot          = new SoSeparator;
    SoSeparator *mazeCache         = new SoSeparator;
    SoSeparator *holeCache         = new SoSeparator;
    SoMaterial  *floorMaterial     = new SoMaterial;
    SoCube      *floor             = new SoCube;
    SoMaterial  *mazeMaterial      = new SoMaterial;
    SoMaterialBinding *mazeMtlBind = new SoMaterialBinding;
    SoNormal    *mazeNorms         = new SoNormal;
    SoNormalBinding *mazeNormBind  = new SoNormalBinding;
    SoLightModel *holeModel        = new SoLightModel;
    SoBaseColor  *holeColor        = new SoBaseColor;
    SoFont  *textFont              = new SoFont;
    SoText3 *startText             = new SoText3;
    SoText3 *finishText            = new SoText3;
    SoRotationXYZ *textRotation    = new SoRotationXYZ;
    SoBaseColor *textColor         = new SoBaseColor;

    mazeCoords      = new SoCoordinate3;
    mazeFaces       = new SoIndexedFaceSet;
    holeCoords      = new SoCoordinate3;
    holeStrips      = new SoTriangleStripSet;
    startTrans      = new SoTranslation;
    finishTrans     = new SoTranslation;
    finishLocation.setValue(10, 10);

    // Create walls for the boundary of the maze
    mazeNorms->vector.set1Value(0, SbVec3f( 0.0, -1.0,  0.0));    
    mazeNorms->vector.set1Value(1, SbVec3f( 0.0,  1.0,  0.0));    
    mazeNorms->vector.set1Value(2, SbVec3f( 0.0,  0.0,  1.0));    
    mazeNorms->vector.set1Value(3, SbVec3f( 1.0,  0.0,  0.0));    
    mazeNorms->vector.set1Value(4, SbVec3f( 0.0,  0.0, -1.0));    
    mazeNorms->vector.set1Value(5, SbVec3f(-1.0,  0.0,  0.0));
    mazeMaterial->diffuseColor.setValue(SbColor(.8, .6, .1));
    mazeMtlBind->value = SoMaterialBinding::OVERALL;
    mazeNormBind->value = SoNormalBinding::PER_FACE_INDEXED;
    mazeCoords->point.deleteValues(0);
    mazeFaces->coordIndex.deleteValues(0);
    mazeFaces->normalIndex.deleteValues(0);

#ifndef NO_HOLES
    // Create holes in the maze floor
    holeModel->model = SoLightModel::BASE_COLOR;
    holeColor->rgb.setValue(SbColor(0.0, 0.0, 0.0));
    holeModel->model = SoLightModel::BASE_COLOR;
    holeCoords->point.deleteValues(0);
    holeStrips->numVertices.deleteValues(0);
    holeCache->addChild(holeModel);
    holeCache->addChild(holeColor);
    holeCache->addChild(holeCoords);
    holeCache->addChild(holeStrips);
#endif
 
    // Parse the command line to find the maze filename
    if (argc < 2)
        setStandardMaze(1);
    else
        readMazeFile(argv[1]);

    // Build the initial maze
    buildMaze( mazeCoords, holeCoords, mazeFaces, holeStrips );

    rotationX = new SoRotationXYZ;
    rotationZ = new SoRotationXYZ;

    // Read the geometry for the game box and add it to the maze
    SoInput in;
    SoNode *n = NULL;
    SoGroup *gameBox = NULL;
    in.setBuffer( (void *)box, 1032 );
    SoDB::read(&in, n);
    gameBox = (SoGroup *) n;

    floorMaterial->diffuseColor.setValue(SbColor(.95, .8, .1));
    floorMaterial->shininess = 0.2;
    floor->width = GAME_WIDTH;
    floor->depth = GAME_WIDTH;
    floor->height = 0.05;
    rotationX->axis = SoRotationXYZ::X;
    rotationZ->axis = SoRotationXYZ::Z;
    holeCache->renderCaching = SoSeparator::ON;
    mazeCache->renderCaching = SoSeparator::ON;
    mazeCache->addChild(floorMaterial);
    mazeCache->addChild(floor);
    mazeCache->addChild(mazeMaterial);
    mazeCache->addChild(mazeCoords);
    mazeCache->addChild(mazeNorms);
    mazeCache->addChild(mazeMtlBind);
    mazeCache->addChild(mazeNormBind);
    mazeCache->addChild(mazeFaces);
    mazeRoot->addChild(gameBox);
    mazeRoot->addChild(rotationX);
    mazeRoot->addChild(rotationZ);
    mazeRoot->addChild(ballRoot);
    mazeRoot->addChild(mazeCache);
    mazeRoot->addChild(holeCache);
    
    // Create a viewer and begin the game
    SoSeparator *mainRoot = new SoSeparator;
    SoEventCallback *eventCB = new SoEventCallback;
    SoXtExaminerViewer *vwr = new SoXtExaminerViewer(mainWindow);
    SoPerspectiveCamera *cam = new SoPerspectiveCamera;

    // Setup event callbacks
    eventCB->addEventCallback(SoMouseButtonEvent::getClassTypeId(),
            saveMouseLocation, (void *)&mouseLocation);
    eventCB->addEventCallback(SoLocation2Event::getClassTypeId(),
            computeFloorTilt, (void *)&mouseLocation);
    eventCB->addEventCallback(SoKeyboardEvent::getClassTypeId(),
            processKeyEvents, NULL);

    // Setup the camera so the game is comfortably viewed
    SbVec3f pos(0.0, 15.0, 9.0);
    SbVec3f lookto(0.0, -1.7, 0.0);
    cam->position.setValue(pos);
    cam->orientation.setValue(SbRotation(SbVec3f(0.0, 0.0, -1.0),
                                         lookto-pos));
    cam->focalDistance = (lookto-pos).length();

    mainRoot->ref();
    mainRoot->addChild(eventCB);
    mainRoot->addChild(cam);
    mainRoot->addChild(mazeRoot);

    mouseLocation.setValue(0, 0);
    mouseRotation.setValue(0.0, 0.0);
    ballVelocity.setValue(0.0, 0.0);
    ballAcceleration.setValue(0.0, 0.0);
    ballPosition[0] = (float)currentGridCol + stdDist;
    ballPosition[1] = (float)currentGridRow + stdDist;
    lastBallPosition[0] = ballPosition[0];
    lastBallPosition[1] = ballPosition[1];
    ballTranslation->translation.setValue(
            ballPosition[0] - GRID_RES2, .35, ballPosition[1] - GRID_RES2);

    vwr->setSize(SbVec2s(900, 700));
    vwr->setSceneGraph(mainRoot);
    vwr->setTitle("Inventor Maze");
    vwr->setViewing(FALSE);
    vwr->setDecoration(FALSE);
    vwr->show();

    // Set the overlay graph - Inventor logo
    setOverlayLogo(vwr);

    SoXt::show(mainWindow);
    SoXt::mainLoop();
}

