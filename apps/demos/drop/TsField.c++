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
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.2 $
 |
 |   Classes	: TsField
 |
 |   Author	: Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

//--------------------------- Include ----------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SoPath.h>
#include <Inventor/SoDB.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/nodes/SoAnnotation.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCallback.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoColorIndex.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoResetTransform.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/sensors/SoAlarmSensor.h>
#include <Inventor/sensors/SoDataSensor.h>
#include <Inventor/sensors/SoTimerSensor.h>
#include "../../samples/common/InventorLogo.h"
#include "TsPieces.h"
#include "TsField.h"
#include "TsViewer.h"
#include "MotifHelp.h"

#define FLD_VERTICAL_SHIFT    -10.0
#define FLD_TOTAL_ROWS           12
#define FLD_CAM_Y              35.0
#define FLD_CAM_Z              20.0

#define DROP_RATE  20.0
#define POINTS_PER_SKILL_LEVEL 10000

#define NUM_HELP_LINES 21
static char *helpMessage[] = {
    "Random pieces fall to the green floor.  Maneuver them to completely",
    "fill in the grid.  Use the ARROW KEYS to translate the piece forward",
    "and backward on the grid.  Rotate the piece as follows:",
    " ",
    "    D KEY rotates the piece clockwise around the falling axis,",
    "    F KEY rolls the piece over on its RIGHT side,",
    "    S KEY rolls the piece over on its LEFT side,",
    "    C KEY rolls the piece TOWARDS you,",
    "    E KEY rolls the piece AWAY from you.",
    " ",
    "The game is over when no more pieces can fall.",
    "Press <Enter> key to start a new game.",
    "The H key displays this message.  Press <Enter> to resume the game.",
    " ",
    "Points are awarded when pieces land on the floor and when rows",
    "are completed.  Use the <SPACEBAR> key to drop a piece before it",
    "lands.  More points are awarded for dropping the piece from a",
    "higher place.  More points are awarded when more than one row is",
    "completed at the same time.  Advance to the next level for every",
    "10,000 points scored.  Pieces fall faster at each higher level.",
    "More difficult pieces fall at the higher levels."

};

//
// This orientation table is used to define each of 24 possible orientations
// for a piece in three dimensions.  For each orientation, a rotation about
// the X, Y, or Z axis will result in a different orientation.  This table
// stores the destination orientation for each orientation resulting from a
// rotation about each of the 3 axes.  Refer to the README file for a 
// description of each orientation.
//
static int orientationTable[24][3] = {
            {  4, 16,  1},
            {  5, 17,  2},
            {  6, 18,  3},
            {  7, 19,  0},
            {  8,  5, 21},
            { 11,  6, 22},
            { 10,  7, 23},
            {  9,  4, 20},
            { 12, 22,  9},
            { 13, 21, 10},
            { 14, 20, 11},
            { 15, 23,  8},
            {  0, 13, 17},
            {  3, 14, 16},
            {  2, 15, 19},
            {  1, 12, 18},
            { 19, 10,  5},
            { 16,  9,  6},
            { 17,  8,  7},
            { 18, 11,  4},
            { 21,  0, 15},
            { 22,  1, 14},
            { 23,  2, 13},
            { 20,  3, 12}};

////////////////////////////////////////////////////////////////////////
//
// Public constructor - build the widget right now
//
TsField::TsField(
    int res,
    Widget parent,
    const char *name, 
    SbBool buildInsideParent)
        : SoXtComponent(
            parent,
            name, 
            buildInsideParent)
//
////////////////////////////////////////////////////////////////////////
{
    gameStarted     = FALSE;
    isGameOver      = TRUE;
    isDropping      = FALSE;
    currentPiece    = NULL;
    otherBlocks     = NULL;
    totalHeight     = FLD_TOTAL_ROWS;
    currentHeight   = 0;
    resolution      = res;
    block           = NULL;
    fieldTable      = NULL;
    skillLevel      = 0;
    pointsEarned    = 0;
    numRemovals     = 0;
    pointsNeeded    = POINTS_PER_SKILL_LEVEL;
    totalScore      = 0;
    helpDisplayed   = TRUE;
    gamePaused	    = FALSE;
    trickleOn	    = FALSE;
    shiftOn	    = FALSE;
    removalOn	    = FALSE;
    numPieceTypes   = 3;

    //
    // Skill rates 2 is not faster because that is the level in which 
    // the more complex pieces are added.  The level displayed to the user
    // is one greater than the index in the skillRates array.
    //
    skillRates[0]   = 1.4;
    skillRates[1]   = 1.7;
    skillRates[2]   = 1.7;
    skillRates[3]   = 2.0;
    skillRates[4]   = 2.3;
    skillRates[5]   = 2.6;
    skillRates[6]   = 2.9;
    currentRate     = skillRates[skillLevel];
    currentFraction = 2.0;

    Widget w = buildWidget(parent, name);
    setBaseWidget(w);
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//	Destructor for TsField.
//
// Use: public

TsField::~TsField()

//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine builds the widget of the game.
//
// Use: public

Widget
TsField::buildWidget(
    Widget       parent,
    const char * name )

//
////////////////////////////////////////////////////////////////////////
{
    viewer      = new TsViewer(parent, name);

    // Create all of the widgets for the game and put them in a form widget
    Widget widget          = XmCreateForm( parent, "form", NULL, 0 );
    Widget viewerWidget    = viewer->getWidget();

    viewer->setSize( SbVec2s( 660, 990 ) );
    SoDirectionalLight *dirLgt = viewer->getHeadlight();
    dirLgt->intensity = 1.0;

    XtManageChild (viewerWidget);

    ARG_VARS(10);

    RESET_ARGS();
    ADD_LEFT_FORM(0);
    ADD_TOP_FORM(0);
    ADD_BOTTOM_FORM(0);
    XtSetValues(viewerWidget, ARGS);

    if (block == NULL)
        buildBlock();

    return widget;
}


////////////////////////////////////////////////////////////////////////
//
//  Set the Inventor logo on the screen.
//
static void
logoCB(void *, SoAction *action)
{
    if (action->isOfType(SoGLRenderAction::getClassTypeId())) {
	static int pushedViewport = 0;
	if (!pushedViewport) {
	    pushedViewport = 1;
	    glPushAttrib(GL_VIEWPORT_BIT);
	    glViewport(0, 0, 80, 80);
	}
	else {
	    pushedViewport = 0;
	    glPopAttrib();
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine initializes the geometry of the playing field and
//      the set of pieces.
//
// Use: public

void
TsField::initGame()

// Values Returned:
//      path to root node of game field
//
////////////////////////////////////////////////////////////////////////
{
    SoBaseColor     *gridColor;
    SoSeparator     *gridRoot;
    SoMaterial      *fallingMaterial;
    TsEll           *ell;
    TsTee           *tee;
    TsZee           *zee;
    TsLTri          *ltri;
    TsMTri          *mtri;
    TsRTri          *rtri;
    int             i, j, k;

    //
    // Initialize the sensors that will be used to trickle the current piece
    // down, remove the completed rows, and shift the remaining rows down
    //
    trickleSensor = new SoTimerSensor( TsField::tricklePiece, (void *)this );
    removalSensor = new SoAlarmSensor( TsField::removeRows,   (void *)this );
    shiftSensor   = new SoTimerSensor( TsField::shiftRows,    (void *)this );
    trickleSensor->setInterval( SbTime( .03 ) );
    shiftSensor->setInterval( SbTime( .03 ) );

    //
    // Allocate and initialize the table of boolean values for each position
    // of the field.  This table will be used for intersection calculations,
    // as it specifies which positions in the field are occupied by a block.
    //
    fieldTable = new SbBool[(totalHeight+4)*resolution*resolution];
    for (i=0; i<(totalHeight+4)*resolution*resolution; i++)
        fieldTable[i] = 0;

    // 
    // Create a list of translation nodes that will be used to position
    // the blocks that make up each piece.  Pieces are defined as a set
    // of indices into a 27-cube.  This table contains 27 translations for
    // each position of this "Standard Cube".
    //
    SoTranslation *tmpTrans;
    for (i=0; i<3; i++)
        for (j=0; j<3; j++)
            for (k=0; k<3; k++)
            {
                tmpTrans = new SoTranslation;
                tmpTrans->translation.setValue (2. * k, 2. * i, -2. * j);
                standardCube.append (tmpTrans);
            }

    //
    // Build the scene graph that represents the whole field of play.
    // The root of this graph will be given to the viewer.  The graph 
    // contains a child for handling play events, the floor of the field,
    // a child for the currently falling piece, and a child for all of
    // the static pieces gathering on the floor.
    //
    fieldRoot  = new SoSeparator;
    fieldRoot->ref();
    SoEventCallback *eventNode  = new SoEventCallback;
    fieldRoot->addChild (eventNode);
    createFloorAndGrid();
    fieldRoot->addChild (floor);

    // Setup the callback functions used for receiving play events
    eventNode->addEventCallback (SoKeyboardEvent::getClassTypeId(),
            TsField::handleKeyboard, (void *)this);    

    //
    // Create the root node for the falling piece.  An instance of the grid
    // will fall with the piece.  Set up a Y translation node for the piece
    // and the grid, an XZ translation node for user control, a material
    // for the falling piece, and the falling piece.
    //
    fallingRoot        = new SoSeparator();
    pieceYTranslation  = new SoTranslation();
    pieceXZTranslation = new SoTranslation();
    gridRoot           = new SoSeparator();
    gridTranslation    = new SoTranslation();
    fallingMaterial    = new SoMaterial();
    gridColor          = new SoBaseColor();

    fallingRoot->addChild (pieceYTranslation);
    fallingRoot->addChild (gridRoot);
    fallingRoot->addChild (pieceXZTranslation);
    fallingRoot->addChild (fallingMaterial);
    fallingRoot->ref();
    gridRoot->addChild (gridTranslation);    
    gridRoot->addChild (gridColor);    
    gridRoot->addChild (grid);    

    gridColor->rgb.setValue (0.9, 0.1, 0.1);
    gridTranslation->translation.setValue (0.0, 0.0, 0.0);
    fallingMaterial->ambientColor.setValue (1.0, 1.0, 1.0);
    fallingMaterial->diffuseColor.setValue (1.0, 1.0, 1.0);
    fallingMaterial->specularColor.setValue (1.0, 1.0, 1.0);
    fallingMaterial->shininess.setValue (20.0);
    fallingMaterial->ref();

    //
    // Create three translation nodes that will be used to animate remaining
    // rows downward after rows have been removed.
    //
    for (i=0; i<3; i++)
        removalTranslation.append (new SoTranslation());

    //
    // Create the scene graph for the static pieces sitting on the floor.
    // Each level of the playing field will be rendered by a different color
    // and at a different vertical location.  As pieces fall and stop at 
    // various levels, the blocks that make up the pieces will be distributed
    // to these levels.
    // For each vertical level of the playing field, create a separator node
    // for it, along with a node for Y translation and a material node
    // Initialize the material and Y translation.  Blocks from the pieces
    // stopping at the levels will be added to the levelBlocks group.
    //
    otherBlocks = new SoGroup(totalHeight);
    fieldRoot->addChild (otherBlocks);
    {
        SoSeparator     *level;
        SoTranslation   *levelXform;
        SoMaterial      *levelMtl;
        float           mtlValue;

        for (i=0; i<totalHeight; i++)
        {
            level       = new SoSeparator();
            levelXform  = new SoTranslation();
            levelMtl    = new SoMaterial();
            otherBlocks->addChild (level);
            level->renderCaching = SoSeparator::ON;
            level->addChild (levelXform);
            level->addChild (levelMtl);
            levelGroup.append(level);

            levelXform->translation.setValue (0.0, (float)(2*i), 0.0);
            mtlValue = (float)(i%4)/3.0;
            levelMtl->ambientColor.setValue (mtlValue, mtlValue, 1.0);
            levelMtl->diffuseColor.setValue (mtlValue, mtlValue, 1.0);
            levelMtl->specularColor.setValue (mtlValue, mtlValue, 1.0);
            levelMtl->shininess.setValue (30.0);
            levelMaterial.append(levelMtl);
        }
    }

    //
    // Create a list of groups for storing the XZ location of each
    // position on the floor along with an instance of a block.  This
    // will be used to place blocks on the levels after a piece has
    // stopped falling.
    //
    {
        SoSeparator   *tblk;
        SoTranslation *XZTrans;

        for (i=0; i<resolution; i++)
            for (j=0; j<resolution; j++)
            {
                tblk    = new SoSeparator(2);
                XZTrans = new SoTranslation;
                
                tblk->addChild (XZTrans);
                tblk->addChild (block);
                XZTrans->translation.setValue ((float)(-resolution+2*j),
                        0.0, (float)(resolution-2*i));
                staticBlock.append (tblk);
            }
    }

    //
    // Create one of each piece and store pointers to them in a list
    // to be used during the game.
    //
    ell = new TsEll();
    tee = new TsTee();
    zee = new TsZee();
    ltri = new TsLTri();
    mtri = new TsMTri();
    rtri = new TsRTri();
    pieceList.append (ell);
    pieceList.append (tee);
    pieceList.append (zee);
    pieceList.append (mtri);
    pieceList.append (ltri);
    pieceList.append (rtri);

    //
    // Create the scene graph that will represent the falling piece.
    // The root of the piece will have a child for each block of the piece.
    // Each block will be rendered with a switch node pointing to the
    // correct standardCube translation and an instance to the generic
    // block.  Add the first three block to the piece, all pieces have at
    // least three blocks.
    //
    fallingPiece = new SoSeparator(4);
    fallingPiece->ref();

    SoSeparator *tmpSep;
    SoSwitch    *tmpSwitch;
    for (i=0; i<4; i++)
    {
        tmpSep    = new SoSeparator(2);
        tmpSwitch = new SoSwitch(27);
        tmpSep->addChild (tmpSwitch);
        tmpSep->addChild (block);
        pieceChild.append (tmpSep);
        for (j=0; j<27; j++)
            tmpSwitch->addChild(standardCube[j]);
    }
    fallingPiece->addChild (pieceChild[0]);
    fallingPiece->addChild (pieceChild[1]);
    fallingPiece->addChild (pieceChild[2]);
    fallingRoot->addChild (fallingPiece);

    // Create a material and transform node for use in removing rows
    removalMaterial  = new SoMaterial();
    removalMaterial->ambientColor.setValue (1.0, 0.75, 0.0);
    removalMaterial->diffuseColor.setValue (1.0, 0.75, 0.0);
    removalMaterial->specularColor.setValue (1.0, 0.75, 0.0);
    removalMaterial->shininess.setValue (50.0);
    removalMaterial->ref();

    //
    // Create a subgraph that will be used to render the score, level
    // as annontation.
    //
    overRoot   = new SoSeparator;
    SoSeparator   *scoreRoot  = new SoSeparator;
    SoFont        *overFont   = new SoFont;
    SoLightModel  *overModel  = new SoLightModel;
    SoColorIndex  *overColor  = new SoColorIndex;
    SoText2       *overText   = new SoText2;
    SoTranslation *overXl1    = new SoTranslation;
    SoTranslation *overXl2    = new SoTranslation;
    SbColor       *colorMap   = new SbColor[4];

    colorMap[0].setValue(1.0, 0.0, 0.0);
    colorMap[1].setValue(0.7, 0.7, 0.7);
    colorMap[2].setValue(0.0, 0.0, 1.0);
    colorMap[3].setValue(1.0, 1.0, 1.0);
    overModel->model = SoLightModel::BASE_COLOR;
    overFont->size   = 20;
    overColor->index = 1;
    overXl1->translation.setValue(SbVec3f(0.4,  0.9, 0.0));
    overXl2->translation.setValue(SbVec3f(0.3,  0.0, 0.0));
    overText->string.set1Value(0, SbString("Level:"));
    overText->string.set1Value(1, SbString("Score:"));
    overScore = new SoText2;
    overScore->string.set1Value(0, SbString("1"));
    overScore->string.set1Value(1, SbString("0"));
    scoreRoot->addChild(overFont);
    scoreRoot->addChild(overColor);
    scoreRoot->addChild(overXl1);
    scoreRoot->addChild(overText);
    scoreRoot->addChild(overXl2);
    scoreRoot->addChild(overScore);
    overRoot->addChild(overModel);
    overRoot->addChild(scoreRoot);

    // Build the scene graph for the help message and attach it to
    // the overRoot.
    buildHelpMessage(overRoot);

    //
    // Add Inventor logo to overlay planes
    //
    SoSeparator *logo = NULL;
    SoInput in;
    in.setBuffer((void *)ivLogo, ivLogoSize); // samples/common/InventorLogo.h
    logo = SoDB::readAll(&in);
    logo->ref();
    in.closeFile();

    SoCallback *cb = new SoCallback;	// sets GL viewport
    cb->setCallback(logoCB);
    logo->insertChild(cb, 0);
    logo->addChild(cb); // Will pop viewport

    SoColorIndex  *logoColor  = new SoColorIndex; // Logo color
    logoColor->index = 3;
    logo->insertChild(logoColor, 0);
    overRoot->addChild(logo);


    // Initialize the viewer for the field
    viewer->setAutoClipping (FALSE);
    viewer->setSceneGraph( fieldRoot );
    viewer->setOverlaySceneGraph( overRoot );
    viewer->setOverlayColorMap(0, 4, colorMap);
    SoCamera *gameCamera      = viewer->getCamera();
    gameCamera->nearDistance  = 0.6;
    gameCamera->farDistance   = 60.0;
    gameCamera->position.setValue( 0.0, FLD_CAM_Y, FLD_CAM_Z );
    gameCamera->orientation.setValue (SbVec3f(-1.0, 0.0, 0.0), 0.7);
    gameCamera->focalDistance = FLD_CAM_Z / cosf (0.7);
    gameCamera->aspectRatio   = 0.65;
}



////////////////////////////////////////////////////////////////////////
//
// Description:
//      Start a new game.
//
// Use: public

void
TsField::startNewGame()

//
////////////////////////////////////////////////////////////////////////
{
    SoSeparator *lg;
    int level, n, i;

    // Unschedule any sensors.
    trickleSensor->unschedule();
    removalSensor->unschedule();
    shiftSensor->unschedule();

    // 
    // Restore the field to a consistent state.  Make sure any removal
    // materials and removal translations are not in the scene graph.
    // Make sure the currently falling piece is attached.
    //
    for (i=0; i<numRemovals; i++)
    {
        lg = (SoSeparator *)levelGroup[removalList[i]];
        if (lg->findChild (removalMaterial) != -1)
            lg->replaceChild (removalMaterial, levelMaterial[removalList[i]]);
        if (otherBlocks->findChild(removalTranslation[i]) != -1)
            otherBlocks->replaceChild (removalTranslation[i], lg);
    }
    if (fieldRoot->findChild (fallingRoot) == -1)
        fieldRoot->insertChild (fallingRoot, 2);

    // For each level, remove all of the pieces on that level
    for (level=0; level<totalHeight; level++)
    {
        lg = (SoSeparator *)levelGroup[level];
        n = lg->getNumChildren();
        for (i=0; i<(n-2); i++)
            lg->removeChild(2);
    }

    // Clear all the values in the fieldTable
    for (i=0; i<((totalHeight+4)*resolution*resolution); i++)
        fieldTable[i] = 0;

    // Initialize game variables
    isGameOver      = FALSE;
    isDropping      = FALSE;
    currentHeight   = 0;
    skillLevel      = 0;
    currentRate     = skillRates[skillLevel];
    currentFraction = 2.0;
    pointsNeeded    = POINTS_PER_SKILL_LEVEL;
    totalScore      = 0;
    numPieceTypes   = 3;

    // Reset the scoreboare
    overScore->string.set1Value(0, SbString("1"));
    overScore->string.set1Value(1, SbString("0"));

    // Add the first piece
    addPiece();
    viewer->saveHomePosition();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//      Pause the game.
//
// Use: public

void
TsField::pauseGame()

//
////////////////////////////////////////////////////////////////////////
{
    gamePaused = TRUE;

    // Unschedule any sensors saving which was on.
    if (trickleSensor->isScheduled()) {
        trickleOn = TRUE;
        trickleSensor->unschedule();
    }
    if (removalSensor->isScheduled()) {
        removalOn = TRUE;
        removalSensor->unschedule();
    }
    if (shiftSensor->isScheduled()) {
        shiftOn = TRUE;
        shiftSensor->unschedule();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//      Resume the game.
//
// Use: public

void
TsField::resumeGame()

//
////////////////////////////////////////////////////////////////////////
{
    gamePaused = FALSE;

    // Reschedule any sensors.
    wallClock.setToTimeOfDay();
    if (trickleOn) {
        trickleOn = FALSE;
        trickleSensor->schedule();
    }
    if (removalOn) {
        removalOn = FALSE;
        removalSensor->schedule();
    }
    if (shiftOn) {
        shiftOn = FALSE;
        shiftSensor->schedule();
    }
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine adds a piece to the playing field.
//
// Use: public

void
TsField::addPiece()

//
////////////////////////////////////////////////////////////////////////
{
    double rnum;
    int    pieceNum, pieceOrientation;
    int    i;

    rnum  = drand48();
    pieceNum = (int)(rnum * (float)numPieceTypes);
    rnum  = drand48();
    pieceOrientation = (int)(rnum*24.0);

    currentPiece = (TsPiece *)pieceList[pieceNum];

    // 
    // Construct the scene graph for the new piece by adding children 
    // to the fallingPiece separator for each block in the new piece.
    // Add or remove a 4th child depending on the number of blocks in the
    // new piece.
    // 
    currentPiece->setOrientation (pieceOrientation);
    numPieceBlocks = currentPiece->getPosition (piecePositions);

    if (fallingPiece->getNumChildren() == 4)
    {
        if (numPieceBlocks == 3)
            fallingPiece->removeChild (3);
    }
    else if (numPieceBlocks == 4)
            fallingPiece->addChild (pieceChild[3]);

    //
    // Set the switch nodes for each block to point to the correct translation
    // in the standardCube.  Also, find the lowest block and position the
    // grid below it.
    //
    SoSwitch *tmpSwitch;
    int lowest = 26;
    for (i=0; i<numPieceBlocks; i++)
    {
        tmpSwitch = (SoSwitch *)((SoGroup *)(pieceChild[i]))->getChild(0);
        tmpSwitch->whichChild = (int32_t)piecePositions[i];
        if (piecePositions[i] < lowest)
            lowest = piecePositions[i];
    }
    gridTranslation->translation.setValue (0.0, 2.0*(lowest/9), 0.0);

    //
    // Set the beginning position of the standard cube.
    //
    currentFraction    = 2.0;
    currentOrientation = pieceOrientation;
    standardPosition[TS_X_AXIS] = 1;
    standardPosition[TS_Y_AXIS] = totalHeight;
    standardPosition[TS_Z_AXIS] = 1;

    pieceYTranslation->translation.setValue (
        0.0, (float)(2*totalHeight) + currentFraction, 0.0);
    pieceXZTranslation->translation.setValue (
        (float)(-resolution+2*1.0), 0.0, (float)(resolution-2*1.0));

    //
    // Start a clock going to time how long it takes a piece to fall.
    // This time will be used to help keep score.
    //
    fallingClock.setToTimeOfDay();

    //
    // Restart the timer sensor for this piece
    //
    wallClock.setToTimeOfDay();
    trickleSensor->schedule();
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine rotates the piece around the given axis by a
//      given amount.
//
// Use: public

void
TsField::rotation( int axis, int amount )

//
////////////////////////////////////////////////////////////////////////
{
    int newOrientation;

    if (isDropping) return;

    // Use the orientationTable to find a new orientation.
    newOrientation = orientationTable[currentOrientation][axis];

    if (amount < 0)
    {
        //
        // Since the orientation table only stores positive rotations, 
        // a negative rotation is made by three successive positive rotations.
        // So make two more positive rotations.
        //
        newOrientation = orientationTable[newOrientation][axis];
        newOrientation = orientationTable[newOrientation][axis];
    }

    rotatePiece (newOrientation);
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//      Rotate the current piece into the given orientation.  Checks
//      against the boundaries of the field and the stack of blocks must
//      be made before the piece can actually be rotated.
//
// Use: public

void
TsField::rotatePiece( int newOrientation )

//
////////////////////////////////////////////////////////////////////////
{
    int localPos[4], fieldPos[4];
    int i;

    // Get the block positions of the current piece given the new orientation
    (void)currentPiece->getPosition (newOrientation, localPos);

    // Check the piece against the field boundaries.
    if (!(validPosition (numPieceBlocks, localPos, standardPosition))) return;

    // Check the piece against the stack of static blocks.
    getFieldPositions (numPieceBlocks, localPos, fieldPos, standardPosition);
    if (collision (numPieceBlocks, fieldPos)) return;

    // Set the switch nodes for the current piece based on the new positions
    // Adjust the grid so it is at the bottom of the piece.
    SoSwitch *tmpSwitch;
    int      lowest = 26;
    for (i=0; i<numPieceBlocks; i++)
    {
        tmpSwitch = (SoSwitch *)((SoGroup *)(pieceChild[i]))->getChild(0);
        tmpSwitch->whichChild = (int32_t)localPos[i];
        if (localPos[i] < lowest)
            lowest = localPos[i];
    }   
    gridTranslation->translation.setValue (0.0, 2.0*(lowest/9), 0.0);

    currentPiece->setOrientation (newOrientation);
    currentOrientation = newOrientation;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine translates the piece along the given axis by the
//      given amount.
//
// Use: public

void
TsField::translation( int axis, int amount )

//
////////////////////////////////////////////////////////////////////////
{
    int newStandard[3];
    int localPos[4], fieldPos[4];

    if (isDropping) return;

    const SbVec3f &xlate = pieceXZTranslation->translation.getValue ();
    SbVec3f newXlate( xlate[0], xlate[1], xlate[2] );

    // Translate the standard cube position.
    newStandard[TS_X_AXIS] = standardPosition[TS_X_AXIS];
    newStandard[TS_Y_AXIS] = standardPosition[TS_Y_AXIS];
    newStandard[TS_Z_AXIS] = standardPosition[TS_Z_AXIS];
    newStandard[axis] += amount;

    // Get the block positions of the current piece
    (void)currentPiece->getPosition (localPos);

    // Check the piece against the field boundaries at the new position of
    // the standard cube.
    if (!(validPosition (numPieceBlocks, localPos, newStandard))) return;

    // Check the piece against the stack of static blocks.
    getFieldPositions (numPieceBlocks, localPos, fieldPos, newStandard);
    if (collision (numPieceBlocks, fieldPos)) return;

    //
    // The piece may be translated.  Set the new position of the standard
    // cube and modify the XZ translation node.
    //
    standardPosition[axis] += amount;
    if (axis == TS_Z_AXIS)
        newXlate[axis] += -2.0 * (float)amount;
    else
        newXlate[axis] += 2.0 * (float)amount;
    pieceXZTranslation->translation.setValue (newXlate);
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine drops the piece as the result of the spacebar
//      being pressed.
//
// Use: private

void
TsField::dropPiece()

//
////////////////////////////////////////////////////////////////////////
{
    if (isDropping) return;

    isDropping   = TRUE;
    saveRate     = currentRate;
    currentRate  = DROP_RATE;

    // Award some points based on where the piece was dropped from.
    pointsEarned = standardPosition[TS_Y_AXIS] * 2;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine adds points to the score and checks to see if
//      the skill level should be incremented.
//
// Use: private

void
TsField::addPoints( int points )

//
////////////////////////////////////////////////////////////////////////
{
    char tmpStr[12];

    totalScore += points * 10;
    sprintf(tmpStr, "%d", totalScore);
    overScore->string.set1Value(1, SbString(tmpStr));

    pointsNeeded -= points * 10;
    if ((pointsNeeded < 0) && (skillLevel < 7)) {
        skillLevel++;
        pointsNeeded += POINTS_PER_SKILL_LEVEL;
        currentRate   = skillRates[skillLevel];
        sprintf(tmpStr, "%d", skillLevel+1);
        overScore->string.set1Value(0, SbString(tmpStr));
        if (skillLevel == 1)
            numPieceTypes = 4;
        else if (skillLevel == 2)
            numPieceTypes = 6;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine creates the data for the floor.  It consists of a
//      grid and a polygon.
//
// Use: private

void
TsField::createFloorAndGrid()

//
////////////////////////////////////////////////////////////////////////
{
    SoCoordinate3       *gridCoords;
    SoMaterial          *floorMtl;
    SoIndexedLineSet    *floorGrid;
    SoTranslation       *floorXform;
    SoNormal            *floorNormal;
    SbVec3f             *points, tcoord;
    float               val1, val2;
    int32_t                *indices;
    int                 i, ct;

    floor   = new SoSeparator();
    grid    = new SoSeparator();
    floor->ref();
    grid->ref();

    //
    // Build the set of points describing the grid, and put them in a
    // SoCoordinate3 node.
    //
    points = new SbVec3f[4*resolution];
    tcoord.setValue ((float)(-resolution), 0.0, (float)(resolution));
    for (i=0; i<=resolution; i++)
    {
        points[i] = tcoord;
        tcoord[0] += 2.0;
    }
    tcoord[0] = (float)(-resolution);
    tcoord[2] = (float)resolution - 2.0;
    val1 = (float)(-resolution);
    val2 = (float)resolution;
    for (i=0; i<(2*(resolution-1));)
    {
        points[resolution+1+i++] = tcoord;
        tcoord[0] = val2;
        points[resolution+1+i++] = tcoord;
        tcoord[0] = val1;
        tcoord[2] -= 2.0;
    }
    for (i=3*resolution-1; i<4*resolution; i++)
    {
        points[i] = tcoord;
        tcoord[0] += 2.0;
    }
    gridCoords = new SoCoordinate3();
    gridCoords->point.setValues (0, 4*resolution, points);
    grid->addChild (gridCoords);
    delete points;

    //
    // Create nodes representing the color and linewidth of the grid
    //
    SoBaseColor         *gridColor = new SoBaseColor();
    SoDrawStyle         *gridStyle = new SoDrawStyle();
    SoLightModel        *lightModel = new SoLightModel();
    grid->addChild (gridStyle);
    grid->addChild (lightModel);
    gridColor->rgb.setValue (0.8, 0.8, 0.8);
    gridStyle->style = SoDrawStyle::LINES;
    gridStyle->lineWidth = 4;
    lightModel->model = SoLightModel::BASE_COLOR;

    //
    // Create one normal pointing up to be used by both the grid and the 
    // polygon.
    //
    tcoord.setValue (0.0, 1.0, 0.0);
    floorNormal = new SoNormal();
    floorNormal->vector.setValues (0, 1, &tcoord);
    grid->addChild (floorNormal);

    //
    // create an SoIndexedLineSet node and fill it with indices into the
    // coordinate list.
    //
    indices = new int32_t[(resolution+1)*2*3];
    indices[0] = 0;
    indices[1] = resolution;
    indices[2] = -1;
    ct = 3;
    for (i=resolution+1; i<3*resolution-1;)
    {
        indices[ct++] = i++;
        indices[ct++] = i++;
        indices[ct++] = -1;
    }
    indices[ct++] = 3*resolution-1;
    indices[ct++] = 4*resolution-1;
    indices[ct++] = -1;
    for (i=0; i<=resolution; i++)
    {
        indices[ct++] = i;
        indices[ct++] = i + 3*resolution-1;
        indices[ct++] = -1;
    }
    floorGrid = new SoIndexedLineSet();
    floorGrid->coordIndex.setValues (0, ct, indices);
    grid->addChild (floorGrid);

    //
    // create the material and transform for the floor polygon.  The polygon
    // lowered slightly beneath the grid, so no coplanar anomalies will exist.
    //
    floor->addChild (gridColor);
    floor->addChild (grid);
    floorMtl = new SoMaterial();
    floorMtl->ambientColor.setValue (0.07, 0.5, 0.1);
    floorMtl->diffuseColor.setValue (0.07, 0.5, 0.1);
    floorMtl->specularColor.setValue (0.07, 0.5, 0.1);
    floorMtl->shininess.setValue (10.0);
    floorXform = new SoTranslation();
    floorXform->translation.setValue (0.0, -0.03, 0.0);
    floor->addChild (floorMtl);
    floor->addChild (floorXform);

    // Create 4 coordinates for the floor;
    points = new SbVec3f[4];
    tcoord.setValue ((float)(-resolution), 0.0, (float)(resolution));
    points[0] = tcoord;
    tcoord[0] = (float)resolution;
    points[1] = tcoord;
    tcoord[2] = (float)-resolution;
    points[2] = tcoord;
    tcoord[0] = (float)-resolution;
    points[3] = tcoord;
    SoCoordinate3 *floorCoords = new SoCoordinate3();
    floorCoords->point.setValues (0, 4, points);
    floor->addChild (floorCoords);
    floor->addChild (floorNormal);
    SoNormalBinding  *normBind = new SoNormalBinding;
    floor->addChild (normBind);
    normBind->value = SoNormalBinding::OVERALL;
    delete points;

    //
    // create an SoIndexedFaceSet node and fill it with indices into
    // the coordinate list.
    //
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 3;
    SoIndexedFaceSet    *floorPoly = new SoIndexedFaceSet ();
    floorPoly->coordIndex.setValues (0, 4, indices);
    floor->addChild (floorPoly);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine builds the scene graph which renders the help
//      message.
//
// Use: private

void
TsField::buildHelpMessage(SoSeparator *root)

//
////////////////////////////////////////////////////////////////////////
{
    SoFont        *helpFont   = new SoFont;
    SoColorIndex  *helpColor  = new SoColorIndex;
    SoText2       *helpText   = new SoText2;
    SoTranslation *helpXfm    = new SoTranslation;

    helpRoot  = new SoSeparator;
    helpRoot->ref();

    helpGroup  = new SoGroup;
    helpGroup->ref();
    helpRoot->addChild(helpGroup);

    helpFont->size   = 20;
    helpColor->index = 3;
    helpXfm->translation.setValue(SbVec3f(-0.9, 0.7, 0.0));
    for (int i=0; i<NUM_HELP_LINES; i++)
        helpText->string.set1Value(i, SbString(helpMessage[i]));
    helpGroup->addChild(helpFont);
    helpGroup->addChild(helpColor);
    helpGroup->addChild(helpXfm);
    helpGroup->addChild(helpText);

    root->addChild(helpRoot);
}
