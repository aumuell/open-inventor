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
 * Copyright (C) 1990-94   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |	SoKeyboardEvent
 |
 |   Author(s): David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/events/SoKeyboardEvent.h>

SO_EVENT_SOURCE(SoKeyboardEvent);

////////////////////////////////////////////////////////////////////////
//
// Constructor
//
SoKeyboardEvent::SoKeyboardEvent()
//
////////////////////////////////////////////////////////////////////////
{
    key = SoKeyboardEvent::ANY;
}

////////////////////////////////////////////////////////////////////////
//
// Destructor
//
SoKeyboardEvent::~SoKeyboardEvent()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Convenience routine - this returns TRUE if the event is a key press
// event matching the passed key.
//
// static public
//
SbBool
SoKeyboardEvent::isKeyPressEvent(const SoEvent *e,
				 SoKeyboardEvent::Key whichKey)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool isMatch = FALSE;
    
    // is it a keyboard event?
    if (e->isOfType(SoKeyboardEvent::getClassTypeId())) {
	const SoKeyboardEvent *ke = (const SoKeyboardEvent *) e;
	
	// is it a press event?
	if (ke->getState() == SoButtonEvent::DOWN) {
	
	    // did the caller want any key press? or do they match?
	    if ((whichKey == SoKeyboardEvent::ANY) ||
	        (ke->getKey() == whichKey))
		isMatch = TRUE;
	}
    }
    
    return isMatch;
}

////////////////////////////////////////////////////////////////////////
//
// Convenience routine - this returns TRUE if the event is a key release
// event matching the passed key.
//
// static public
//
SbBool
SoKeyboardEvent::isKeyReleaseEvent(const SoEvent *e,
				   SoKeyboardEvent::Key whichKey)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool isMatch = FALSE;
    
    // is it a keyboard event?
    if (e->isOfType(SoKeyboardEvent::getClassTypeId())) {
	const SoKeyboardEvent *ke = (const SoKeyboardEvent *) e;
	
	// is it a release event?
	if (ke->getState() == SoButtonEvent::UP) {
	
	    // did the caller want any key release? or do they match?
	    if ((whichKey == SoKeyboardEvent::ANY) ||
	        (ke->getKey() == whichKey))
		isMatch = TRUE;
	}
    }
    
    return isMatch;
}


////////////////////////////////////////////////////////////////////////
//
// Convenience routine that returns the character representing the
// key, if it's printable. If not, this returns NUL ('\0').
//
// public
//

char
SoKeyboardEvent::getPrintableCharacter() const
//
////////////////////////////////////////////////////////////////////////
{
    int		offset;

    // These store characters corresponding to shifted numeric keys,
    // symbol keys, and shifted symbol keys:
    static char	shiftNumberChars[]	= ")!@#$%^&*(";
    static char	symbolChars[]		= " ',-./;=[\\]`";
    static char	shiftSymbolChars[]	= " \"<_>?:+{|}~";

    switch (getKey()) {

      case SoKeyboardEvent::A:
      case SoKeyboardEvent::B:
      case SoKeyboardEvent::C:
      case SoKeyboardEvent::D:
      case SoKeyboardEvent::E:
      case SoKeyboardEvent::F:
      case SoKeyboardEvent::G:
      case SoKeyboardEvent::H:
      case SoKeyboardEvent::I:
      case SoKeyboardEvent::J:
      case SoKeyboardEvent::K:
      case SoKeyboardEvent::L:
      case SoKeyboardEvent::M:
      case SoKeyboardEvent::N:
      case SoKeyboardEvent::O:
      case SoKeyboardEvent::P:
      case SoKeyboardEvent::Q:
      case SoKeyboardEvent::R:
      case SoKeyboardEvent::S:
      case SoKeyboardEvent::T:
      case SoKeyboardEvent::U:
      case SoKeyboardEvent::V:
      case SoKeyboardEvent::W:
      case SoKeyboardEvent::X:
      case SoKeyboardEvent::Y:
      case SoKeyboardEvent::Z:

	// This relies on the letter codes being consecutive:
	offset = getKey() - SoKeyboardEvent::A;
	return offset + (wasShiftDown() ? 'A' : 'a');

      case SoKeyboardEvent::NUMBER_0:
      case SoKeyboardEvent::NUMBER_1:
      case SoKeyboardEvent::NUMBER_2:
      case SoKeyboardEvent::NUMBER_3:
      case SoKeyboardEvent::NUMBER_4:
      case SoKeyboardEvent::NUMBER_5:
      case SoKeyboardEvent::NUMBER_6:
      case SoKeyboardEvent::NUMBER_7:
      case SoKeyboardEvent::NUMBER_8:
      case SoKeyboardEvent::NUMBER_9:

	// This relies on the number codes being consecutive:
	offset = getKey() - SoKeyboardEvent::NUMBER_0;
	return wasShiftDown() ? shiftNumberChars[offset] : ('0' + offset);

      case SoKeyboardEvent::SPACE:
      case SoKeyboardEvent::APOSTROPHE:
      case SoKeyboardEvent::COMMA:
      case SoKeyboardEvent::MINUS:
      case SoKeyboardEvent::PERIOD:
      case SoKeyboardEvent::SLASH:
      case SoKeyboardEvent::SEMICOLON:
      case SoKeyboardEvent::EQUAL:
      case SoKeyboardEvent::BRACKETLEFT:
      case SoKeyboardEvent::BACKSLASH:
      case SoKeyboardEvent::BRACKETRIGHT:
      case SoKeyboardEvent::GRAVE:
	// This relies on these codes being consecutive:
	offset = getKey() - SoKeyboardEvent::SPACE;
	return wasShiftDown() ? shiftSymbolChars[offset] : symbolChars[offset];

      default:
	return '\0';
    }
}
