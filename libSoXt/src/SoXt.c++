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
 * Copyright (C) 1990-93   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |	SoXt
 |	SoXtEventHandler
 |
 |   Author(s): David Mott, Nick Thompson, Paul Isaacs, Alain Dumesny
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/errors/SoDebugError.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInteraction.h>
#include <Inventor/nodekits/SoNodeKit.h>
#include <Inventor/sensors/SoSensorManager.h>
#include <Inventor/Xt/SoXt.h>
#include <Xm/MessageB.h>
#include <SoDebug.h>
#include <X11/Intrinsic.h>
#include <X11/Xresource.h>
#include <X11/StringDefs.h>



// This SG_ routine is from /usr/src/X11/motif/overlay_demos.
// Someday, it may be a standard function in a lib, but for
// now it is just sample code. We include it here for convenience
// to our users.
extern "C" {
    iv_SG_getPopupArgs(Display *, int, ArgList, int *);
}



#include <SoDebug.h>


static void 	overlayMenuMappedCB(Widget w, Widget shell, XtPointer);
static void 	overlayMenuUnmappedCB(Widget w, Widget shell, XtPointer);


// this class ties Inventor sensors into Xt events 
class SoXtEventHandler {
  public:
    SoXtEventHandler(XtAppContext context);

  private:
    XtAppContext	appContext;

    SbBool		timerOn;
    XtIntervalId	xtTimer;
    SbTime		currentDeadline;

    SbBool		workProcOn;
    XtWorkProcId	xtWorkProc;

    // Callback used by the timer
    static void		xtTimerCallback(XtPointer closure, XtIntervalId *id);

    // Callback used by the workproc
    static Boolean	xtWorkProcCallback(XtPointer closure);

    // Callback for changes to the sensor manager
    static void		sensorManagerChangedCallback(void *data);

    void 		setUpCallbacks();
};

class SoExtensionEventHandler {
  public:
    SoExtensionEventHandler(
	Widget widget,
	int eventType,
	XtEventHandler proc,
	XtPointer data);

    Widget	    getWidget()		{ return widget; }
    int		    getEventType()	{ return eventType; }
    XtEventHandler  getEventHandler()	{ return proc; }
    XtPointer	    getClientData()	{ return data; }
  
  private:
    Widget	    widget;
    int		    eventType;
    XtEventHandler  proc;
    XtPointer	    data;
};

// static members
Widget		    SoXt::mainWidget = 0;
SoXtEventHandler *  SoXt::eventHandler = NULL;
SbPList *	    SoXt::handlerList = NULL;


////////////////////////////////////////////////////////////////////////
//
// Description:
//   This initializes the database, interaction, and Xt.
// It creates a returns a top level shell widget.
//
// static public
//
Widget
SoXt::init(
    const char *appName,
    const char *className)
//
////////////////////////////////////////////////////////////////////////
{
    // see if we are already initialized!
    if (mainWidget != NULL)
    	return mainWidget;
    
    // init Xt, and store the widget it returns.
    XtAppContext appContext;
    char *argv[1];
    int argc = 1;
    argv[0] = (char *) appName;
    
    mainWidget = XtAppInitialize(
	    &appContext,		// context will be returned
	    className,			// application class
	    NULL, 0,			// options
	    (int *) &argc, argv, 	// command line args (appName!)
	    NULL,			// fallback_resources
	    NULL, 0);			// args, num_args

    // init Inventor
    SoDB::init();
    SoNodeKit::init();
    SoInteraction::init();
    
    // bind Inventor with Xt
    eventHandler = new SoXtEventHandler(XtWidgetToApplicationContext(mainWidget));
    
    return mainWidget;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   This initializes the database and interaction. Xt was initialized
// by the caller.
//
// static public
//
void
SoXt::init(Widget topLevelWidget)
//
////////////////////////////////////////////////////////////////////////
{
    // see if we are already initialized!
    if (mainWidget != NULL)
    	return;

    // see if the passed widget is ok
    if (topLevelWidget == NULL)
	return;
	
    // save the widget
    mainWidget = topLevelWidget;
    
    // init Inventor
    SoDB::init();
    SoNodeKit::init();
    SoInteraction::init();
    
    // bind Inventor with Xt
    eventHandler = new SoXtEventHandler(XtWidgetToApplicationContext(mainWidget));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   This displays the passed widget.
//
// static public
//
void
SoXt::show(Widget w)
//
////////////////////////////////////////////////////////////////////////
{
    if (w == NULL)
	return;
    
    if (XtIsShell(w)) {
	// raise and de-iconify the window if it already exists
	if (XtWindow(w) != 0)
	    XMapRaised(XtDisplay(w), XtWindow(w));
	else {
	    XtRealizeWidget(w);
	    XMapWindow(XtDisplay(w), XtWindow(w));
	}
    }
    else
	XtManageChild(w);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   This hides the passed widget.
//
// static public
//
void
SoXt::hide(Widget w)
//
////////////////////////////////////////////////////////////////////////
{
    if (w == NULL)
	return;
    
    // ??? temporary disable the idle queue timer to prevent a redraw timer
    // ??? from happening before we get chance to process all unmap events
    // ??? that will be generated from this hide(). SoXtComponents will
    // ??? depend of those unmap events to know that they are not visible.
    // ??? .....see comment below about XSync()......
    SbTime savedTime = SoDB::getDelaySensorTimeout();
    if (savedTime != SbTime::zero())
	SoDB::setDelaySensorTimeout(0.0);
    
    if (XtIsShell(w)) {
	if (XtWindow(w))
	    XUnmapWindow(XtDisplay(w), XtWindow(w));
    }
    else
	XtUnmanageChild(w);
    
    // call XSync() to force the unmap events to be received by the X server, 
    // which are needed to prevent SoXtComponents from redrawing into a 
    // window that is unmaped. Redrawing to an unmapped window seems to 
    // screw up some hardware gfx the NEXT time you redraw to the 
    // front buffer. (see bug 290854)
    XSync(XtDisplay(w), FALSE);
    
    // ??? restore the timer. This actually will give us about 1/10 second to
    // ??? process all unmap events...idealy we could use an XSendEvent() to 
    // ??? restore the timer with our onw event.
    if (savedTime != SbTime::zero())
	SoDB::setDelaySensorTimeout(savedTime);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   This adds the passed proc as an event handler for an X extension
// event.
//
// static public
//
void
SoXt::addExtensionEventHandler(
    Widget w,
    int extensionEventType,
    XtEventHandler proc, 
    XtPointer clientData)
//
////////////////////////////////////////////////////////////////////////
{
    if (handlerList == NULL)
	handlerList = new SbPList;

    // add the passed proc as an event handler for extension events on w
    SoExtensionEventHandler *data = new SoExtensionEventHandler
	(w, extensionEventType, proc, clientData);
	
    handlerList->append(data);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   This removes the passed proc as an event handler for and X extension
// event.
//
// static public
//
void
SoXt::removeExtensionEventHandler(
    Widget w,
    int extensionEventType,
    XtEventHandler proc, 
    XtPointer clientData)
//
////////////////////////////////////////////////////////////////////////
{
    // loop through each item in handlerList,
    // and see if w,eventType,proc, and data match.
    SbBool found = FALSE;
    for (int i = 0; !found && (i < handlerList->getLength()); i++) {
	SoExtensionEventHandler *handler = 
	    (SoExtensionEventHandler *) (*handlerList)[i];
	
	if ((handler->getWidget() == w) &&
	    (handler->getEventType() == extensionEventType) &&
	    (handler->getEventHandler() == proc) &&
	    (handler->getClientData() == clientData)) {
		found = TRUE;
		handlerList->remove(i);
	    }
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   This locates the widget which will handle the event.
// Input: event
// Output: w, proc, clientData
//
// static protected
//
void
SoXt::getExtensionEventHandler(
    XEvent *event, 
    Widget &w, 
    XtEventHandler &proc, 
    XtPointer &clientData)
//
////////////////////////////////////////////////////////////////////////
{
    // extension event. look up in our list of event handlers
    // for a match
    proc = NULL;
    for (int i = 0; (proc == NULL) && (i < handlerList->getLength()); i++) {
	SoExtensionEventHandler *handler = 
	    (SoExtensionEventHandler *) (*handlerList)[i];
	
	w = XtWindowToWidget(event->xany.display,
				    event->xany.window);

	if (handler->getWidget() == w) {
	    if (handler->getEventType() == event->type) {
		// we found a match!
		proc = handler->getEventHandler();
		clientData = handler->getClientData();
	    }
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   This dispatches events.
//
// static public
//
Boolean
SoXt::dispatchEvent(XEvent *event)
//
////////////////////////////////////////////////////////////////////////
{
    Boolean success = True;
     
    if(event->type >= LASTEvent) {
	XtEventHandler proc;
	XtPointer clientData;
	Widget w;
	Boolean dummy;
	
	// Get the event handling function which was
	// registered with Inventor for handling this
	// event type in this widget
	SoXt::getExtensionEventHandler(event, w, proc, clientData);
	
	// Call the event handler!
	if (proc == NULL)
	    success = False;
	else
	    (*proc) (w, clientData, event, &dummy);
	
    }
    
    // else it is not an extension event - let Xt dispatch it
    else success = XtDispatchEvent(event);
    
    return success;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   This retrieves and dispatches events (it loops forever!)
//
// static public
//
void
SoXt::mainLoop()
//
////////////////////////////////////////////////////////////////////////
{
    if (mainWidget == NULL) {
#ifdef DEBUG
	SoDebugError::post("SoXt::mainLoop",
			   "exiting -- SoXt::init never called");
#endif
	return;
    }
    
    // We cannot call XtAppMainLoop, because we need to handle
    // extension devices, like the spaceball. (Xt will ignore them).
    XtAppContext context = getAppContext();
    XEvent event;
    
    for (;;) {
	SoXt::nextEvent(context, &event);
	SoXt::dispatchEvent(&event);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   This returns the application context for the mainWidget.
//
// static public
//
XtAppContext
SoXt::getAppContext()
//
////////////////////////////////////////////////////////////////////////
{
    return mainWidget == NULL ? NULL : XtWidgetToApplicationContext(mainWidget);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   This returns the display for the mainWidget.
//
// static public
//
Display *
SoXt::getDisplay()
//
////////////////////////////////////////////////////////////////////////
{
    return mainWidget == NULL ? NULL : XtDisplay(mainWidget);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   This returns the display for the mainWidget.
//
// static public
//
Widget
SoXt::getTopLevelWidget()
//
////////////////////////////////////////////////////////////////////////
{
    return mainWidget;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Convert the char* to an XmString.
//
// static public
//
XmString
SoXt::encodeString(char *s)
//
////////////////////////////////////////////////////////////////////////
{
    XmString xs = NULL;
    
    if (s != NULL)
	xs = XmStringCreate(s, XmSTRING_DEFAULT_CHARSET);

    return xs;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Convert the XmString to a char*.
//
// static public
//
char *
SoXt::decodeString(XmString xs)
//
////////////////////////////////////////////////////////////////////////
{
    XmStringContext ctx;
    char *text = NULL;
    XmStringCharSet charset;
    XmStringDirection dirRtoL;
    Boolean separator;
    Boolean initOK = TRUE;
    Boolean getOK = TRUE;

    initOK = XmStringInitContext(&ctx, xs);
    
    if (initOK)
        getOK = XmStringGetNextSegment(ctx, &text,
					&charset, &dirRtoL, &separator);

#ifdef DEBUG
    if (!initOK)
	SoDebugError::post("SoXt::decodeString",
			   "XmStringInitContext failed.");
    if (!getOK)
	SoDebugError::post("SoXt::decodeString",
			   "XmStringGetNextSegment failed.");
#endif

    return text;
}


//----------------------------------------------------------------------
//
//  SoXtEventHandler - this class ties in sensors to Xt events
//
//----------------------------------------------------------------------

SoXtEventHandler::SoXtEventHandler(XtAppContext context)
{
    appContext  = context;
    timerOn	= FALSE;
    workProcOn	= FALSE;
    
    SoDB::getSensorManager()->setChangedCallback
	(SoXtEventHandler::sensorManagerChangedCallback,
	 this);
    // Call this once because things might already be in the timer queue.
    // If we don't call this and no new things happen, then the callbacks
    // will never be set up.
    setUpCallbacks();
}

void
SoXtEventHandler::setUpCallbacks()
{
    SbTime	nextEvent;

    // If we have a timer at some point, schedule that
    if (SoDB::getSensorManager()->isTimerSensorPending(nextEvent)) {
	// Only change the timer if its deadline is different from the
	// current one.
	if (!timerOn || nextEvent != currentDeadline) {
	    long msec;				// System long

	    if (timerOn) {	// get rid of existing timer if present
		XtRemoveTimeOut(xtTimer);
		timerOn = FALSE;
	    }

	    currentDeadline = nextEvent;
	    msec = (nextEvent - SbTime::getTimeOfDay()).getMsecValue();

	    xtTimer = XtAppAddTimeOut(appContext, msec > 0 ? msec : 0,
				      (XtTimerCallbackProc)
				      SoXtEventHandler::xtTimerCallback,
				      (XtPointer) this);
	    timerOn = TRUE;
#ifdef DEBUG
	    if (SoDebug::GetEnv("IV_DEBUG_SENSORS")) {
		SoDebug::RTPrintf("XtAppAddTimeOut %d msec\n", msec);
	    }
#endif
	}
    }
    else if (timerOn) {		// get rid of existing timer
	XtRemoveTimeOut(xtTimer);
	timerOn = FALSE;
#ifdef DEBUG
	if (SoDebug::GetEnv("IV_DEBUG_SENSORS")) {
	    SoDebug::RTPrintf("XtRemoveTimeOut\n");
	}
#endif
    }

    // If we have an idle task, schedule a workproc for it
    if (SoDB::getSensorManager()->isDelaySensorPending()) {
	// only schedule it if one is not already active
	if (!workProcOn) {
	    xtWorkProc =
		XtAppAddWorkProc(appContext,
				 (XtWorkProc)
				 SoXtEventHandler::xtWorkProcCallback,
				 (XtPointer) this);
	    workProcOn = TRUE;
#ifdef DEBUG
	    if (SoDebug::GetEnv("IV_DEBUG_SENSORS")) {
		SoDebug::RTPrintf("XtAppAddWorkProc\n");
	    }
#endif
	}
    }
    else if (workProcOn) {		// remove obsolete workproc
	XtRemoveWorkProc(xtWorkProc);
	workProcOn = FALSE;
#ifdef DEBUG
	if (SoDebug::GetEnv("IV_DEBUG_SENSORS")) {
	    SoDebug::RTPrintf("XtRemoveWorkProc\n");
	}
#endif
    }
}

void
SoXtEventHandler::xtTimerCallback(XtPointer closure, XtIntervalId *)
{
    SoXtEventHandler *eh = (SoXtEventHandler *) closure;

    // process the timer queue
    SoDB::getSensorManager()->processTimerQueue();

    // set up for the next time around
    // hide the fact that this timer exists so that setUpEvents
    // won't try to delete it (Xt will do that automatically)
    eh->timerOn = FALSE;
    eh->setUpCallbacks();
}

Boolean
SoXtEventHandler::xtWorkProcCallback(XtPointer closure)
{
    SoXtEventHandler *eh = (SoXtEventHandler *) closure;

    // process the idle queue
    SoDB::getSensorManager()->processDelayQueue(TRUE);

    // set up for the next time around
    // hide the fact that this workproc exists so that setUpEvents
    // won't try to delete it (Xt will do that automatically)
    eh->workProcOn = FALSE;
    eh->setUpCallbacks();

    // Always return TRUE, meaning delete the workproc now.  The
    // workproc will be installed again if it is necessary.  This
    // should be pretty cheap, as Xt keeps a pool of these.
    return TRUE;
}

// Callback for changes to the sensor manager
void
SoXtEventHandler::sensorManagerChangedCallback(void *data)
{
    SoXtEventHandler *eh = (SoXtEventHandler *) data;

    // update the Xt timeout and workproc if necessary
    eh->setUpCallbacks();
}

SoExtensionEventHandler::SoExtensionEventHandler(
    Widget w,
    int et,
    XtEventHandler p,
    XtPointer d)
{
    widget = w;
    eventType = et;
    proc = p;
    data = d;
}

////////////////////////////////////////////////////////////////////////
//
// Resize the given widget. It calls XtSetArg/Values on the widget.
//
// Use: static public
void
SoXt::setWidgetSize(Widget w, const SbVec2s &size)
//
////////////////////////////////////////////////////////////////////////
{
    if (w == NULL)
	return;
    
    Arg args[2];
    XtSetArg(args[0], XtNwidth, size[0]);
    XtSetArg(args[1], XtNheight, size[1]);
    XtSetValues(w, args, 2);
}

////////////////////////////////////////////////////////////////////////
//
// Returns the given widget size.
//
// Use: static public
SbVec2s
SoXt::getWidgetSize(Widget w)
//
////////////////////////////////////////////////////////////////////////
{
    if (w == NULL)
	return SbVec2s(0, 0);
    
    Arg args[2];
    short _w, _h;	// HAS to be shorts for XtGetValues to work !!
    XtSetArg(args[0], XtNheight, &_h);
    XtSetArg(args[1], XtNwidth, &_w);
    XtGetValues(w, args, 2);
    return SbVec2s(_w, _h);
}

////////////////////////////////////////////////////////////////////////
//
// Returns the shell widget which contains this widget
//
// Use: static public
Widget
SoXt::getShellWidget(Widget w)
//
////////////////////////////////////////////////////////////////////////
{
    for (; w != NULL; w = XtParent(w))
	if (XtIsShell(w))
	    return w;
    
    return NULL;
}

static void destroyDialogCB(Widget dialog, void *, void *)
{ XtDestroyWidget(dialog); }

////////////////////////////////////////////////////////////////////////
//
// creates a simple error dialog box with the given error 
//
// Use: static public
void
SoXt::createSimpleErrorDialog(Widget widget, 
    char *dialogTitle, char *str1, char *str2)
//
////////////////////////////////////////////////////////////////////////
{
    // ??? Make the motif dialog on the shell of the 'widget' variable
    // ??? instead of the 'widget' itself otherwise the color editor 
    // ??? has problem building once the dialog has been opened. 
    // ??? I can't figure that one out. Weird... (alain)
    Widget shell = SoXt::getShellWidget(widget);
    if (shell == NULL)
	return;
    
    Arg args[5];
    XmString xmstr = XmStringCreateSimple(str1);
    if (str2 != NULL) {
	xmstr = XmStringConcat(xmstr, XmStringSeparatorCreate());
	xmstr = XmStringConcat(xmstr, XmStringSeparatorCreate());
	xmstr = XmStringConcat(xmstr, XmStringCreateSimple(str2));
    }
    
    int n = 0;
    XtSetArg(args[n], XmNautoUnmanage, FALSE); n++;
    XtSetArg(args[n], XtNtitle, dialogTitle); n++;
    XtSetArg(args[n], XmNmessageString, xmstr); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
    Widget dialog = XmCreateErrorDialog(shell, "Error Dialog", args, n);
    XmStringFree(xmstr);
    
    XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
    XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));
    
    // register callback to destroy (and not just unmap) the dialog
    XtAddCallback(dialog, XmNokCallback, 
	(XtCallbackProc) destroyDialogCB, (XtPointer)NULL);
    
    XtManageChild(dialog);
}

////////////////////////////////////////////////////////////////////////
//
// Convenience routine for creating menus in the popup planes. 
//
// Use: static public
void
SoXt::getPopupArgs(
    Display *d,
    int scr,
    ArgList args,
    int *n)
//
////////////////////////////////////////////////////////////////////////
{
    iv_SG_getPopupArgs(d, scr, args, n);
}


////////////////////////////////////////////////////////////////////////
//
// register map/unmap callbacks to load/unload the given color maps
// when the pulldown menu (or popup menu) gets mapped.
//
// Use: static public
void
SoXt::registerColormapLoad(Widget w, Widget shell)
//
////////////////////////////////////////////////////////////////////////
{
    if (! w || ! shell || ! XtIsShell(shell))
	return;
    
    XtAddCallback(w, XmNmapCallback, (XtCallbackProc) overlayMenuMappedCB, shell);
    XtAddCallback(w, XmNunmapCallback, (XtCallbackProc) overlayMenuUnmappedCB, shell);
}

//
// Called when an overlay menu (pulldown or popup) is about to be 
// mapped - this will load the correct color map on the window.
//
static void
overlayMenuMappedCB(Widget w, Widget shell, XtPointer)
{
//printf("MAP overlayMenuMappedCB called\n");
    SoXt::addColormapToShell(w, shell);
}

//
// Called when an overlay menu (pulldown or popup) is no longer 
// mapped - this will unload the color map to make sure that the OGL
// overlay (really popup planes on 8 bit machine) colormap gets used.
//
static void
overlayMenuUnmappedCB(Widget w, Widget shell, XtPointer)
{
//printf("UNMAP overlayMenuUnmappedCB called\n");
    SoXt::removeColormapFromShell(w, shell);
}


////////////////////////////////////////////////////////////////////////
//
// Convenience routine to load the given colormap
//
// Use: static public
void
SoXt::addColormapToShell(Widget w, Widget shell)
//
////////////////////////////////////////////////////////////////////////
{
    if (! w || ! shell || ! XtIsShell(shell))
	return;
    
    // load the color map RIGH AWAY to reduce pulldown menu flickers
    Colormap map;
    XtVaGetValues(w, XmNcolormap, &map, NULL);
    XInstallColormap(XtDisplay(w), map);
    
    // check to see if there is already a property
    Window *windowsReturn;
    int countReturn;
    Status status = XGetWMColormapWindows(XtDisplay(shell),
	XtWindow(shell), &windowsReturn, &countReturn);
    
    // if no property, just create one
    if (!status)
    {
	Window windows[2];
	windows[0] = XtWindow(w);
	windows[1] = XtWindow(shell);
	XSetWMColormapWindows(XtDisplay(shell), XtWindow(shell),
	    windows, 2);
    }
    // else there was a property, add myself to the beginning
    else {
	Window *windows = (Window *)XtMalloc((sizeof(Window))*
					     (countReturn+1));
	windows[0] = XtWindow(w);
	for (int i=0; i<countReturn; i++)
	    windows[i+1] = windowsReturn[i];
	XSetWMColormapWindows(XtDisplay(shell), XtWindow(shell),
	    windows, countReturn+1);
	XtFree((char *)windows);
	XtFree((char *)windowsReturn);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Convenience routine to remove the given colormap
//
void
SoXt::removeColormapFromShell(Widget w, Widget shell)
//
////////////////////////////////////////////////////////////////////////
{
    if (! w || ! shell || ! XtIsShell(shell))
	return;
    
    // check to see if there is a property
    Window *windowsReturn;
    int countReturn;
    Status status = XGetWMColormapWindows(XtDisplay(shell),
	XtWindow(shell), &windowsReturn, &countReturn);
    
    // if no property, just return.  If there was a property, continue
    if (status)
    {
	// search for a match
	for (int i=0; i<countReturn; i++)
	{
	    if (windowsReturn[i] == XtWindow(w))
	    {
		// we found a match, now copu the rest down
		for (i++; i<countReturn; i++)
		    windowsReturn[i-1] = windowsReturn[i];
		
		XSetWMColormapWindows(XtDisplay(w), XtWindow(shell),
					windowsReturn, countReturn-1);
		break;	// from outer for
	    }
	}
	XtFree((char *)windowsReturn);
    }
}

