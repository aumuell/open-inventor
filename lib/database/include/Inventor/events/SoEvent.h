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

//  -*- C++ -*-

/*
 * Copyright (C) 1990,91,92   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Classes:
 |	SoEvent
 |
 |   Author(s): David Mott, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_EVENT_
#define  _SO_EVENT_

#include <Inventor/SbBasic.h>
#include <Inventor/SbViewportRegion.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SbTime.h>
#include <Inventor/SoType.h>

// C-api: prefix=SoEv
class SoEvent {
  public:
  
    // Constructor and destructor
    SoEvent();
    // C-api: expose
    virtual ~SoEvent();
    
    // returns typeId of this event
    // C-api: expose
    virtual SoType	getTypeId() const;
    
    // typeId of the class
    static SoType	getClassTypeId() { return classTypeId; }
    
    // returns TRUE if event is of given type or is derived from it
    SbBool		isOfType(SoType type) const;
    
    // returns the time stamp specifying when this event occurred
    void		setTime(SbTime t)		{ timestamp = t; }
    SbTime		getTime() const			{ return timestamp; }
    
    // set/get window pixel position of the locator when the event occurred.
    // position is relative to the lower left corner of the viewport
    // C-api: name=setPos
    void		setPosition(const SbVec2s &p)	{ position = p; }
    // C-api: name=getPos
    const SbVec2s &	getPosition() const		{ return position; }

    // get position reletive to the specified viewport bounds
    // C-api: name=getPosInVP
    const SbVec2s &	getPosition(const SbViewportRegion &vpRgn) const;

    // get position reletive to the specified viewport bounds
    // and normalize this value between 0.0 and 1.0
    // C-api: name=getNormPos
    const SbVec2f & getNormalizedPosition(const SbViewportRegion &vpRgn) const;

    // set the state of the modifier keys when the event occurred
    void		setShiftDown(SbBool isDown)	{ shiftDown = isDown; }
    void		setCtrlDown(SbBool isDown)	{ ctrlDown = isDown; }
    void		setAltDown(SbBool isDown)	{ altDown = isDown; }
    
    // return the state of the modifier keys when the event occurred
    SbBool		wasShiftDown() const		{ return shiftDown; }
    SbBool		wasCtrlDown() const		{ return ctrlDown; }
    SbBool		wasAltDown() const		{ return altDown; }
    
  SoINTERNAL public:
    // Initializes base event class
    static void		initClass();

    // Initialize ALL Inventor event classes
    static void		initClasses();
    
  private:
    // all of these are set according to when the event occurred
    SbTime		timestamp;  // time the event occurred
    SbBool		shiftDown;  // TRUE if shift key was down 
    SbBool		ctrlDown;   // TRUE if ctrl key was down 
    SbBool		altDown;    // TRUE if alt key was down 

    SbVec2s		position;   // locator position when event occurred
    SbVec2s		viewportPos;	// position relative to viewport
    SbVec2f		normalizedPos;	// normalized position

    static SoType	classTypeId; // base typeId for all events
};


#endif /* _SO_EVENT_ */
