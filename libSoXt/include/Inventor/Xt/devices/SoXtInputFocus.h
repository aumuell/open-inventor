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
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |	SoXtInputFocus
 |
 |   Author(s): David Mott, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_XT_FOCUS_
#define  _SO_XT_FOCUS_

#include <X11/X.h>
#include <Inventor/Xt/devices/SoXtDevice.h>

#define SO_XT_ALL_FOCUS_EVENTS \
    (EnterWindowMask | LeaveWindowMask)

// NOTE: this does not generate any SoEvent.
// It simply makes sure the passed X events occur in the widget.

// C-api: prefix=SoXtInFocus
class SoXtInputFocus : public SoXtDevice {
  public:
    //
    // valid event mask values:
    //	    EnterWindowMask	- input focus entered this window
    //	    LeaveWindowMask	- input focus left this window
    // Bitwise OR these to specify whichEvents this device should queue.
    //
    SoXtInputFocus(EventMask mask = SO_XT_ALL_FOCUS_EVENTS);
    ~SoXtInputFocus() {}
    
    // these functions will enable/disable this device for the passed widget.
    // the callback function f will be invoked when events occur in w.
    // data is the clientData which will be passed.
    virtual void        enable(Widget w, XtEventHandler f,
			       XtPointer data, Window win = 0);
    virtual void        disable(Widget w, XtEventHandler f, XtPointer data);
    
    //
    // this converts an X event into an SoEvent,
    // returning NULL if the event is not from this device.
    //
    // C-api: name=xlateEv
    virtual const SoEvent * translateEvent(XAnyEvent *xevent);
  
  private:
    EventMask		eventMask;
};

#endif /* _SO_XT_FOCUS_ */
