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
 *  This is the header file for the "ButtonBox" event.
 *------------------------------------------------------------*/


#ifndef  _BUTTON_BOX_EVENT_
#define  _BUTTON_BOX_EVENT_

#include <Inventor/SbBasic.h>
#include <Inventor/events/SoButtonEvent.h>
#include <Inventor/events/SoSubEvent.h>

// some convenience macros for determining if an event matches
#define BUTTON_BOX_PRESS_EVENT(EVENT,BUTTON) \
   (ButtonBoxEvent::isButtonPressEvent(EVENT,BUTTON))

#define BUTTON_BOX_RELEASE_EVENT(EVENT,BUTTON) \
   (ButtonBoxEvent::isButtonReleaseEvent(EVENT,BUTTON))

// The ButtonBoxEvent class
class ButtonBoxEvent : public SoButtonEvent {

   SO_EVENT_HEADER();
   
  public:
   // constructor
   ButtonBoxEvent();
   
   // which button generated the event, e.g. ButtonBoxEvent::BUTTON1
   void     setButton(int b) { button = b; }
   int	    getButton() const  { return button; }
   
   // convenience routines to see if an SoEvent is a press or release
   // of the passed button box button. Passing -1 matches any button.
   static SbBool         isButtonPressEvent(
                           const SoEvent *e,
                           int whichButton = -1);
			    
   static SbBool         isButtonReleaseEvent(
                           const SoEvent *e,
                           int whichButton = -1);
    
  SoINTERNAL public:
   static void		initClass();
   
  private:
   int   		button;		    // which button
};

#endif /* _BUTTON_BOX_EVENT_ */
