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
 |	SoSpaceballButtonEvent
 |
 |   Author(s): David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/events/SoSpaceballButtonEvent.h>

SO_EVENT_SOURCE(SoSpaceballButtonEvent);

////////////////////////////////////////////////////////////////////////
//
// Constructor
//
SoSpaceballButtonEvent::SoSpaceballButtonEvent()
//
////////////////////////////////////////////////////////////////////////
{
    button = SoSpaceballButtonEvent::ANY;
}

////////////////////////////////////////////////////////////////////////
//
// Destructor
//
SoSpaceballButtonEvent::~SoSpaceballButtonEvent()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Convenience routine - this returns TRUE if the event is a spaceball button
// press event matching the passed button.
//
// static public
//
SbBool
SoSpaceballButtonEvent::isButtonPressEvent(const SoEvent *e,
				SoSpaceballButtonEvent::Button whichButton)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool isMatch = FALSE;
    
    // is it a mouse button event?
    if (e->isOfType(SoSpaceballButtonEvent::getClassTypeId())) {
	const SoSpaceballButtonEvent *se = (const SoSpaceballButtonEvent *) e;
	
	// is it a press event?
	if (se->getState() == SoButtonEvent::DOWN) {
	
	    // did the caller want any button press? or do they match?
	    if ((whichButton == SoSpaceballButtonEvent::ANY) ||
	        (se->getButton() == whichButton))
		isMatch = TRUE;
	}
    }
    
    return isMatch;
}

////////////////////////////////////////////////////////////////////////
//
// Convenience routine - this returns TRUE if the event is a spaceball button
// release event matching the passed button.
//
// static public
//
SbBool
SoSpaceballButtonEvent::isButtonReleaseEvent(const SoEvent *e,
				SoSpaceballButtonEvent::Button whichButton)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool isMatch = FALSE;
    
    // is it a mouse button event?
    if (e->isOfType(SoSpaceballButtonEvent::getClassTypeId())) {
	const SoSpaceballButtonEvent *se = (const SoSpaceballButtonEvent *) e;
	
	// is it a release event?
	if (se->getState() == SoButtonEvent::UP) {
	
	    // did the caller want any button release? or do they match?
	    if ((whichButton == SoSpaceballButtonEvent::ANY) ||
	        (se->getButton() == whichButton))
		isMatch = TRUE;
	}
    }
    
    return isMatch;
}

