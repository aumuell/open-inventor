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
 |   $Revision: 1.1.1.1 $
 |
 |   Classes	: TsField::tricklePiece
 |                TsField::stopPiece
 |                TsField::collision
 |
 |   Author	: Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

//--------------------------- Include ----------------------------------
#include <stdio.h>
#include <Inventor/SbLinear.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/sensors/SoAlarmSensor.h>
#include <Inventor/sensors/SoTimerSensor.h>
#include "TsPieces.h"
#include "TsField.h"

#define REMOVAL_HIGHLIGHT_DURATION  1.0


////////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine translates the current piece down in Y by a small
//      amount.  The amount is determined by the current level of play.
//      If the piece crosses an integer boundary, a collision test must
//      be made.  If a collision has occurred, the piece must stop at
//      the integer position, a check must be made to see if a whole
//      row may be removed, and a new piece is started.
//
// Use: public

void
TsField::tricklePiece( void *data, SoSensor * )

//
////////////////////////////////////////////////////////////////////////
{
    TsField       *field = (TsField *)data;
    SbTime        elapsedTime;
    float         tmpFraction, shiftDistance;
    time_t        clockSeconds;
    long          clockMSeconds;		// System long

    //
    // Calculate the distance the piece should fall.  This is calculated
    // given the change in time and the rate at which the piece is falling.
    // The distance is expressed as a value from 2.0 to 0.0.  This represents
    // the intermediate distance from one integer position to the next.
    //
    elapsedTime.setToTimeOfDay();
    elapsedTime -= field->wallClock;
    field->wallClock.setToTimeOfDay();
    elapsedTime.getValue (clockSeconds, clockMSeconds);
    shiftDistance = (clockSeconds + clockMSeconds * 1e-6) * field->currentRate;
    tmpFraction = field->currentFraction - shiftDistance;

    if (tmpFraction >= 0.0)
    {
        //
        // Just translate down.
        //
        field->currentFraction = tmpFraction;
    }
    else while (tmpFraction < 0.0)
    {
        int newPosition[3], localPos[4], fieldPos[4];

        //
        // The piece has crossed an integer boundary.
        // Temporarily shift the standard cube position down one level and
        // get the positions of the piece blocks at this new position.
        //
        newPosition[TS_X_AXIS] = field->standardPosition[TS_X_AXIS];
        newPosition[TS_Y_AXIS] = field->standardPosition[TS_Y_AXIS] - 1;
        newPosition[TS_Z_AXIS] = field->standardPosition[TS_Z_AXIS];
        (void)field->currentPiece->getPosition (localPos);
        field->getFieldPositions (field->numPieceBlocks, localPos, fieldPos,
                newPosition);

        //
        // Check this new position for a collision with blocks in the field
        // table and the floor. If there is a collision, stop the current
        // piece. 
        //
        if (field->collision (field->numPieceBlocks, fieldPos))
        {
            field->trickleSensor->unschedule();
            field->stopPiece();
            return;
        }

        // No collision.  Finalize the standard cube position.
        --field->standardPosition[TS_Y_AXIS];

        tmpFraction += 2.0;
        field->currentFraction = tmpFraction;
    }

    // Update the PieceYTranslation node to move the piece downwards.
    const SbVec3f &tpos =  field->pieceYTranslation->translation.getValue ();
    SbVec3f spos;
    spos[0] = tpos[0];
    spos[1] = tpos[1] - shiftDistance;
    spos[2] = tpos[2];
    field->pieceYTranslation->translation.setValue (spos);
}




////////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine places blocks representing the current piece in
//      with the other static blocks on the field and fills in the
//      correct indices in the field table corresponding to the positions
//      of the blocks of the piece. 
//
//      The routine also checks the levels in which the current piece came
//      to rest and changes those rows to be gold.  A sensor is scheduled
//      to elimate the rows.
//
//      The routine ends the game if the current piece lands at the top of
//      the field.
//
//      This routine gives the time the piece spent falling and the
//      number of rows removed to the scoreboard.
//     
//      If no rows have been removed, a new piece is put into play.

// Use: private

void
TsField::stopPiece()

//
////////////////////////////////////////////////////////////////////////
{
    int         numBlocks, localPos[4], fieldPos[4];
    int         height, rowIndex, i, j, tmp;

    // Initialize a list used for removing completed rows;
    for (i=0; i<3; i++) removalList[i] = -1;
    numRemovals = 0;

    //
    // Calculate the height and XZ position of each block based on the
    // standard cube location in the field and the local positions of
    // the blocks.  Use the height to index into the level groups and the
    // XZ position to index into the nodelist of static block positions.
    // For each block add a child to the correct level group from the nodelist
    // of static block positions and fill in the field table for its position.
    //
    numBlocks = currentPiece->getPosition (localPos);
    getFieldPositions (numBlocks, localPos, fieldPos, standardPosition);
    for (i=0; i<numBlocks; i++)
    {
        height = fieldPos[i] / (resolution*resolution);
        rowIndex = fieldPos[i] % (resolution*resolution);

        //
        // If the top of the piece is above the total height of the game,
        // the game is over.
        //
        if (height > currentHeight) currentHeight = height;
        if (height >= totalHeight) 
        {
            isGameOver = TRUE;
            return;
        }

        SoSeparator *level;

        level = (SoSeparator *)(otherBlocks->getChild(height));
        level->addChild(staticBlock[rowIndex]);

        fieldTable[fieldPos[i]] = 1;

        //
        // Check the number of blocks now occupying the given level.
        // If the level is full, add the level on a sorted list as all
        // of its blocks will be removed.  
        //
        if (level->getNumChildren() == (resolution*resolution+2))
        {
            for (j=0; j<numRemovals; j++)
                if (removalList[j] == height) break;
            if (j == numRemovals) 
            {
                removalList[numRemovals++] = height;
                for (j=numRemovals; --j>0;)
                    if (removalList[j] < removalList[j-1])
                    {
                        tmp = removalList[j];
                        removalList[j] = removalList[j-1];
                        removalList[j-1] = tmp;
                    }                        
            }
        }
    }

    // If the piece was dropped, restore the original falling rate.
    if (isDropping)
    {
        isDropping = FALSE;
        currentRate = saveRate;
    }

    // Award some points based on where the piece ended up.
    if (pointsEarned == 0)
        pointsEarned = standardPosition[TS_Y_AXIS] + 1;

    if (numRemovals > 0)
    {
        SoSeparator *levelSep;

        // Award some more points based on how many rows were removed.
        switch (numRemovals) {
            case 1: pointsEarned += 100; break;
            case 2: pointsEarned += 200; break;
            case 3: pointsEarned += 900; break;
            case 4: pointsEarned += 1600; break;
        }

        // Remove the falling piece from the scene graph
        fieldRoot->removeChild (fallingRoot);

        //
        // For each row to be removed, substitute a new material 
        // for the normal level material.  Then, schedule an alarm
        // sensor to go off after a time which will actually remove
        // the rows and animate the remaining rows downwards.
        //
        for (i=0; i<numRemovals; i++)
        {
            levelSep = (SoSeparator *)levelGroup[removalList[i]];
            levelSep->replaceChild(levelMaterial[removalList[i]],
                                   removalMaterial);
        }
        removalSensor->setTimeFromNow(SbTime(REMOVAL_HIGHLIGHT_DURATION));
        removalSensor->schedule();

        // The row removal code will add a new piece to the field, so control
        // can just return here.
        return;
    }
    else {
        
    }


    // Add a new piece to the playing field.
    addPoints( pointsEarned );
    pointsEarned = 0;
    addPiece ();
}
   

////////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine checks to see if the current piece has collided with
//      other blocks in the field.
//
// Use: private

SbBool
TsField::collision(
    int numBlocks,     // The number of blocks in the piece
    int *fieldPos)     // The field positions of the blocks in the piece

//
////////////////////////////////////////////////////////////////////////
{
    int i;

    //
    // Check for intersection with another piece or the floor.
    // For each block in the piece, if its position is < 0 or
    // if the field is occupied at that position, a collision
    // has occurred.
    //
    for (i=0; i<numBlocks; i++)
    {
        if ((fieldPos[i] < 0) || (fieldTable[fieldPos[i]]))
            return TRUE;
    }

    return FALSE;
}



////////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine checks to see if the current piece will leave the
//      playing field if the given position of the piece is used.
//
// Use: private

SbBool
TsField::validPosition(
    int numBlocks,     // The number of blocks in the piece
    int *pos,          // The positions of the blocks in the piece
    int *standard )    // The row, col, height of the standard cube

//
////////////////////////////////////////////////////////////////////////
{
    int i;

    //
    // For each block, check to see if the placement of the standard cube
    // causes a block to be out of bounds.
    //
    for (i=0; i<numBlocks; i++)
    {
        if (standard[TS_X_AXIS] == (resolution-1))
        {
            if ((pos[i] % 3) > 0)
                return FALSE;
        }
        else if (standard[TS_X_AXIS] == (resolution-2))
        {
            if ((pos[i] % 3) > 1)
                return FALSE;
        }
        if (standard[TS_X_AXIS] == -1)
        {
            if ((pos[i] % 3) < 1)
                return FALSE;
        }
        else if (standard[TS_X_AXIS] == -2)
        {
            if ((pos[i] % 3) < 2)
                return FALSE;
        }

        if (standard[TS_Z_AXIS] == (resolution-1))
        {
            if ((pos[i] % 9) > 2)
                return FALSE;
        }
        else if (standard[TS_Z_AXIS] == (resolution-2))
        {
            if ((pos[i] % 9) > 5)
                return FALSE;
        }
        else if (standard[TS_Z_AXIS] == -1)
        {
            if ((pos[i] % 9) < 3)
                return FALSE;
        }
        else if (standard[TS_Z_AXIS] == -2)
        {
            if ((pos[i] % 9) < 6)
                return FALSE;
        }
    }

    return TRUE;
}



////////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine returns a set of global positions given a set of
//      local positions and the location of the standard cube.
//
// Use: private

void
TsField::getFieldPositions(
    int numBlocks,     // The number of blocks in the piece
    int *localPos,     // The local positions of the blocks in the piece
    int *fieldPos,     // The field positions of the blocks in the piece
    int *standard )    // The row, col, height of the standard cube

//
////////////////////////////////////////////////////////////////////////
{
    int i;

    for (i=0; i<numBlocks; i++)
    {
        int lp = localPos[i];
        fieldPos[i] =  
            (standard[TS_Y_AXIS] + lp/9)     * resolution * resolution +
            (standard[TS_Z_AXIS] + (lp%9)/3) * resolution +
            (standard[TS_X_AXIS] + lp%3);
    }
}






