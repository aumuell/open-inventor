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
 |   Methods	: TsField::handleKeyboard
 |
 |   Author	: Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

//--------------------------- Include ----------------------------------
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/events/SoSpaceballButtonEvent.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoSeparator.h>
#include "TsField.h"

////////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine handles all keyboard events that move the pieces as
//      they fall.
//
// Use: private, static

void
TsField::handleKeyboard( void *data, SoEventCallback *node )

//
////////////////////////////////////////////////////////////////////////
{
    TsField       *field = (TsField *)data;

    if (SO_KEY_PRESS_EVENT(node->getEvent(), RETURN)) {
        field->gameStarted = TRUE;
        field->handleReturnKey();
        node->setHandled();
        return;
    }

    // If the game has been paused, don't accept any input.  Just return.
    // (Also, don't accept input if the game has not started.)
    if (field->gamePaused || !field->gameStarted) {
        node->setHandled();
        return;
    }
    
    if (SO_KEY_PRESS_EVENT(node->getEvent(), LEFT_ARROW))
        field->translation (TS_X_AXIS, -1);
    else if (SO_KEY_PRESS_EVENT(node->getEvent(), RIGHT_ARROW))
        field->translation (TS_X_AXIS,  1);
    else if (SO_KEY_PRESS_EVENT(node->getEvent(), UP_ARROW))
        field->translation (TS_Z_AXIS,  1);
    else if (SO_KEY_PRESS_EVENT(node->getEvent(), DOWN_ARROW))
        field->translation (TS_Z_AXIS, -1);
    else if (SO_KEY_PRESS_EVENT(node->getEvent(), F))
        field->rotation (TS_Z_AXIS,   1);
    else if (SO_KEY_PRESS_EVENT(node->getEvent(), S))
        field->rotation (TS_Z_AXIS,  -1);
    else if (SO_KEY_PRESS_EVENT(node->getEvent(), D))
        field->rotation (TS_Y_AXIS,  1);
    else if (SO_KEY_PRESS_EVENT(node->getEvent(), C))
        field->rotation (TS_X_AXIS,  -1);
    else if (SO_KEY_PRESS_EVENT(node->getEvent(), E))
        field->rotation (TS_X_AXIS,   1);
    else if (SO_KEY_PRESS_EVENT(node->getEvent(), SPACE))
        field->dropPiece();
    else if (SO_KEY_PRESS_EVENT(node->getEvent(), H)) {
        field->handleHelpKey();
    }

    node->setHandled();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine handles the return key press.
//
// Use: public

void
TsField::handleReturnKey()

//
////////////////////////////////////////////////////////////////////////
{
    // If help had been displayed, either start the first game
    // or resume a game in progress.
    if (helpDisplayed) {
        helpDisplayed = FALSE;
        helpRoot->removeChild(helpGroup); //old:overRoot->removeChild(helpRoot)
        if (isGameOver) 
            startNewGame();
        else
            resumeGame();
    }
    else {
        // Start a new game
        startNewGame();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine handles the H key press.
//
// Use: public

void
TsField::handleHelpKey()

//
////////////////////////////////////////////////////////////////////////
{
    // If the help message is not displayed, add the help message geometry
    // to the scene graph and pause the game.
    if (!helpDisplayed) {
        helpRoot->addChild(helpGroup); //old: overRoot->addChild(helpRoot);
        helpDisplayed = TRUE;
        pauseGame();
    }
}
