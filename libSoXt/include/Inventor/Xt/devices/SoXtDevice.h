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
 |	SoXtDevice
 |
 |   Author(s): David Mott, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_XT_DEVICE_
#define  _SO_XT_DEVICE_

#include <X11/Intrinsic.h>
#include <Inventor/SbLinear.h>
#include <Inventor/events/SoEvent.h>

// C-api: name=SoXtDev
class SoXtDevice {
  public:
    // these functions will enable/disable this device for the passed widget.
    // the callback function f will be invoked when events occur in w.
    // data is the clientData which will be passed.
    // For enable, the window is also passed, since Glx widgets can have
    // more than one window (normal, overlay, popup)
    // C-api: expose
    virtual void        enable(Widget w, XtEventHandler f,
			       XtPointer data, Window win = 0) = 0;
    // C-api: expose
    virtual void        disable(Widget w, XtEventHandler f, XtPointer data) = 0;
    
    // this converts an X event into an SoEvent.
    // this returns NULL if the event is not from this device.
    // C-api: expose
    // C-api: name=xlateEv
    virtual const SoEvent * translateEvent(XAnyEvent *xevent) = 0;

    // set the window size so that the device can correctly convert X
    // window coordinates (origin at top,left) into Inventor window
    // coordinates (origin at bottom,left).
    // C-api: name=setWinSize
    void		setWindowSize(const SbVec2s &s)	{ winSize = s; }
    // C-api: name=getWinSize
    const SbVec2s &	getWindowSize() const		{ return winSize; }

  protected:
    // set the window coordinate from the passed x,y value. This will flip
    // the y value to convert from X coordinates to So coordinates.
    inline void		setEventPosition(SoEvent *event, int x, int y) const;
    
  private:
    SbVec2s		winSize;    // size of the window this device works in
};

// Inline functions
void
SoXtDevice::setEventPosition(SoEvent *event, int x, int y) const
{
    event->setPosition(SbVec2s(x, (winSize[1] - 1) - y));
}

#endif /* _SO_XT_DEVICE_ */
