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
 * Copyright (C) 1990,91,92   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Class:
 |	call initClasses for all event classes
 |
 |   Author(s)		: Ronen Barzel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/events/SoEvents.h>

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes ALL Inventor event classes.
//
// Use: internal

void
SoEvent::initClasses()
//
////////////////////////////////////////////////////////////////////////
{
    SoEvent::initClass();

    SoButtonEvent::initClass();
    SoKeyboardEvent::initClass();
    SoMouseButtonEvent::initClass();
    SoSpaceballButtonEvent::initClass();

    SoLocation2Event::initClass();
    SoMotion3Event::initClass();
}
////////////////////////////////////////////////////////////////////////
//
// Class initialization
//
// SoINTERNAL public
//
void
SoButtonEvent::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    // Allocate a new node type id
    classTypeId = SoType::createType(SoEvent::getClassTypeId(), "ButtonEvent");
}

////////////////////////////////////////////////////////////////////////
//
// Class initialization
//
// SoINTERNAL public
//
void
SoEvent::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    // Allocate a new node type id
    // No real parent id, so pass 'badType' as the parent type
    classTypeId = SoType::createType(SoType::badType(), "Event");
}

////////////////////////////////////////////////////////////////////////
//
// Class initialization
//
// SoINTERNAL public
//
void
SoKeyboardEvent::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    // Allocate a new event type id
    classTypeId = SoType::createType(
	SoButtonEvent::getClassTypeId(), "KeyboardEvent");
}

////////////////////////////////////////////////////////////////////////
//
// Class initialization
//
// SoINTERNAL public
//
void
SoLocation2Event::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    // Allocate a new event type id
    classTypeId = SoType::createType(SoEvent::getClassTypeId(), "Location2Event");
}

////////////////////////////////////////////////////////////////////////
//
// Class initialization
//
// SoINTERNAL public
//
void
SoMotion3Event::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    // Allocate a new event type id
    classTypeId = SoType::createType(
	SoEvent::getClassTypeId(), "Motion3Event");
}

////////////////////////////////////////////////////////////////////////
//
// Class initialization
//
// SoINTERNAL public
//
void
SoMouseButtonEvent::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    // Allocate a new event type id
    classTypeId = SoType::createType(
	SoButtonEvent::getClassTypeId(), "MouseButtonEvent");
}

////////////////////////////////////////////////////////////////////////
//
// Class initialization
//
// SoINTERNAL public
//
void
SoSpaceballButtonEvent::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    // Allocate a new event type id
    classTypeId = SoType::createType(
	SoButtonEvent::getClassTypeId(), "SpaceballButtonEvent");
}

