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
 *  This is the source file for the "DialNButton" device.
 *------------------------------------------------------------*/

#include <X11/Xlib.h>
#include <X11/extensions/XI.h>

#include <Inventor/SbTime.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/events/SoButtonEvent.h>

#include "ButtonBoxEvent.h"
#include "DialEvent.h"
#include "DialNButton.h"

extern "C" {
XDeviceInfo *XListInputDevices(Display *, int *);
XDevice	    *XOpenDevice(Display *, XID);
int	    XSelectExtensionEvent(Display *, Window, XEventClass *, int);
}

#define DEVICE_NAME "dial+buttons"

// There are 3 event classes for this device:
// motion, button down, button up
static const int numEventClasses = 3;

// static members
SbBool DialNButton::firstTime = TRUE;
int DialNButton::motionEventType;
int DialNButton::buttonPressEventType;
int DialNButton::buttonReleaseEventType;
XEventClass DialNButton::eventClasses[3];
int DialNButton::eventTypes[3];
XDevice *DialNButton::device;

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Initialize the dial+button device. We only need to do this once.
//
// public
//
void
DialNButton::init(Display *display)
//
////////////////////////////////////////////////////////////////////////
{    
   // If already initialized, return.
   if (! firstTime) return;
   
   firstTime = FALSE;
   
   // get the list of input devices that are attached to the display now
   XDeviceInfoPtr  list;
   int		    numDevices;
   
   list = (XDeviceInfoPtr) XListInputDevices(display, &numDevices);
   
   // now run through the list looking for the device
   device = NULL;
   for (int i = 0; (i < numDevices) && (device == NULL); i++) {
      // Open the device - the device id is set at runtime.
      if (strcmp(list[i].name, DEVICE_NAME) == 0) {
         device = XOpenDevice(display, list[i].id);
      }
   }
   
   // make sure we found the device
   if (device == NULL) {
      fprintf(stderr, "DialNButton::init",
         "Sorry there is no dial and button attached to this display");
      return;
   }
    
   // Query the event types and classes
   uint32_t eventClass;
   
   DeviceMotionNotify(device, motionEventType, eventClass);
   eventClasses[0] = eventClass;
   eventTypes[0] = motionEventType;
   
   DeviceButtonPress(device, buttonPressEventType, eventClass);
   eventClasses[1] = eventClass;
   eventTypes[1] = buttonPressEventType;

   DeviceButtonRelease(device, buttonReleaseEventType, eventClass);
   eventClasses[2] = eventClass;
   eventTypes[2] = buttonReleaseEventType; 
   
   // Init all dial values to 0
   static int vals[8] = {0, 0, 0, 0, 0, 0, 0, 0};
   XSetDeviceValuators(display, device, vals, 0, 8);  
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Constructor using default display.
//
// public
//
DialNButton::DialNButton()
//
////////////////////////////////////////////////////////////////////////
{    
   init(SoXt::getDisplay());

   buttonEvent = new ButtonBoxEvent;
   dialEvent = new DialEvent;
}
   
////////////////////////////////////////////////////////////////////////
//
// Description:
//   Constructor.
//
// public
//
DialNButton::DialNButton(Display *d)
//
////////////////////////////////////////////////////////////////////////
{    
   init(d);

   buttonEvent = new ButtonBoxEvent;
   dialEvent = new DialEvent;
}
   
////////////////////////////////////////////////////////////////////////
//
// Description:
//   Destructor.
//
// public
//
DialNButton::~DialNButton()
//
////////////////////////////////////////////////////////////////////////
{
   delete buttonEvent;
   delete dialEvent;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   returns whether the dial+button device exists for use or not.
//
// static, public
//
SbBool
DialNButton::exists(Display *display)
//
////////////////////////////////////////////////////////////////////////
{
   // get the list of input devices that are attached to the display now
   XDeviceInfoPtr  list;
   int		    numDevices;
   
   list = (XDeviceInfoPtr) XListInputDevices(display, &numDevices);
   
   // now run through the list looking for the device
   for (int i = 0; (i < numDevices) &&
                   (strcmp(list[i].name, DEVICE_NAME) != 0); i++)
      ; // keep looping

   // if we broke out of the loop before i reached numDevices,
   // then the does in fact exist. 
   return (i < numDevices);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   This selects input for dial+button device events which occur in w.
// The callback routine is proc, and the callback data is clientData.
//
// virtual public
//
void
DialNButton::enable(
   Widget w,
   XtEventHandler proc, 
   XtPointer clientData,
   Window window)
//
////////////////////////////////////////////////////////////////////////
{
   if (numEventClasses == 0) return;
   
   Display *display = XtDisplay(w);
   if (display == NULL) {
      fprintf(stderr, "DialNButton::enable",
         "SoXt::init not properly called (Display is NULL).");
      return;
   }
   
   if (w == NULL) {
      fprintf(stderr, "DialNButton::enable",
         "widget is NULL.");
      return;
   }
   
   if (window == NULL) {
      fprintf(stderr, "DialNButton::enable",
         "widget must be realized (Window is NULL).");
      return;
   }
   
   // select extension events for the dial+button which the user wants
   XSelectExtensionEvent(display, window,
      eventClasses, numEventClasses);
   
   // tell Inventor about these extension events!
   for (int i = 0; i < numEventClasses; i++)
      SoXt::addExtensionEventHandler(
         w, eventTypes[i], proc, clientData);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
// This unselects input for dial+button device events which occur in w,
// i.e. dial+button events will no longer be recognized.
//
// virtual public
//
void
DialNButton::disable(
   Widget w,
   XtEventHandler proc, 
   XtPointer clientData)
//
////////////////////////////////////////////////////////////////////////
{
   // tell Inventor to forget about these classes
   for (int i = 0; i < numEventClasses; i++)
      SoXt::removeExtensionEventHandler(
         w, eventTypes[i], proc, clientData);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Translate X events into Inventor events.
//
// virtual public
//
const SoEvent *
DialNButton::translateEvent(XAnyEvent *xevent)
//
////////////////////////////////////////////////////////////////////////
{
   SoEvent *event = NULL;
   
   // see if this is a dial+button event
   if (xevent->type == motionEventType) {
      XDeviceMotionEvent *me = (XDeviceMotionEvent *) xevent;
      if (me->deviceid == device->device_id)
         event = translateMotionEvent(me);
   }
   else if (xevent->type == buttonPressEventType) {
      XDeviceButtonEvent *be = (XDeviceButtonEvent *) xevent;
      if (be->deviceid == device->device_id)
         event = translateButtonEvent(be, SoButtonEvent::DOWN);
   }
   else if (xevent->type == buttonReleaseEventType) {
      XDeviceButtonEvent *be = (XDeviceButtonEvent *) xevent;
      if (be->deviceid == device->device_id)
         event = translateButtonEvent(be, SoButtonEvent::UP);
   }
   
   return event;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   This returns a DialEvent for the passed X event.
//
// private
//
DialEvent *
DialNButton::translateMotionEvent(XDeviceMotionEvent *me)
//
////////////////////////////////////////////////////////////////////////
{
   setEventPosition(dialEvent, me->x, me->y);
   dialEvent->setTime(SbTime(0, 1000*me->time));
   dialEvent->setShiftDown(me->state & ShiftMask);
   dialEvent->setCtrlDown(me->state & ControlMask);
   dialEvent->setAltDown(me->state & Mod1Mask);
   
   // the dial that turned is stored as first_axis in the X event.
   // the value is always in axis_data[0].
   dialEvent->setDial(me->first_axis);
   dialEvent->setValue(me->axis_data[0]);
   
   return dialEvent;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   This returns a ButtonBoxEvent for the passed X event.
//
// private
//
ButtonBoxEvent *
DialNButton::translateButtonEvent(
   XDeviceButtonEvent *be,
   SoButtonEvent::State whichState)
//
////////////////////////////////////////////////////////////////////////
{
   setEventPosition(buttonEvent, be->x, be->y);
   buttonEvent->setTime(SbTime(0, 1000*be->time));
   buttonEvent->setShiftDown(be->state & ShiftMask);
   buttonEvent->setCtrlDown(be->state & ControlMask);
   buttonEvent->setAltDown(be->state & Mod1Mask);
   
   // set which button along with its state.
   buttonEvent->setButton(be->button);
   buttonEvent->setState(whichState);
   
   return buttonEvent;
}
