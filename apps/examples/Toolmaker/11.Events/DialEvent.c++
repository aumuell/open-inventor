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

/*-------------------------------------------------------------
 *  This is an example from The Inventor Toolmaker,
 *  chapter 11.
 *
 *  This is the source file for the "Dial" event.
 *------------------------------------------------------------*/

#include "DialEvent.h"

SO_EVENT_SOURCE(DialEvent);

////////////////////////////////////////////////////////////////////////
//
// Class initialization
//
// SoINTERNAL public
//
void
DialEvent::initClass()
//
////////////////////////////////////////////////////////////////////////
{
   SO_EVENT_INIT_CLASS(DialEvent, SoEvent);
}

////////////////////////////////////////////////////////////////////////
//
// Constructor
//
DialEvent::DialEvent()
//
////////////////////////////////////////////////////////////////////////
{
   dial = 0;
   value = 0;
}

////////////////////////////////////////////////////////////////////////
//
// Convenience routine - this returns TRUE if the event is a dial
// turn event matching the passed dial.
//
// static public
//
SbBool
DialEvent::isDialEvent(const SoEvent *e, int whichDial)
//
////////////////////////////////////////////////////////////////////////
{
   SbBool isMatch = FALSE;
   
   // is it a dial event?
   if (e->isOfType(DialEvent::getClassTypeId())) {
      const DialEvent *de = (const DialEvent *) e;
   
      // did the caller want any dial turn? or do they match?
      if ((whichDial == -1) ||
          (de->getDial() == whichDial))
         isMatch = TRUE;
   }
   
   return isMatch;
}


