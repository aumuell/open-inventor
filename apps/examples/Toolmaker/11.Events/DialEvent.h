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
 *  This is the header file for the "Dial" event.
 *------------------------------------------------------------*/


#ifndef  _DIAL_EVENT_
#define  _DIAL_EVENT_

#include <Inventor/SbBasic.h>
#include <Inventor/events/SoEvent.h>
#include <Inventor/events/SoSubEvent.h>

// convenience macro for determining if an event matches
#define DIAL_EVENT(EVENT, WHICH) \
   (DialEvent::isDialEvent(EVENT, WHICH))


// The DialEvent class
class DialEvent : public SoEvent {

   SO_EVENT_HEADER();
   
  public:
   // constructor
   DialEvent();
   
   // which dial generated the event, 1-8
   void     setDial(int d)   { dial = d; }
   int	    getDial() const  { return dial; }
    
   // value of the dial turned
   void     setValue(int v)  { value = v; }
   int	    getValue() const { return value; }
   
   // convenience routines to see if an SoEvent is a turn of
   // the passed dial. Passing -1 matches any button.
   static SbBool	isDialEvent(const SoEvent *e, int which = -1);
   
  SoINTERNAL public:
   static void		initClass();
   
  private:
   int   		dial;		    // which dial
   int			value;		    // value of dial
};

#endif /* _DIAL_EVENT_ */
