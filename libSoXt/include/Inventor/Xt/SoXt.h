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
|   $Revision: 1.2 $
|
|   This static class provides routines for Inventor/Xt compatibility.
|
|   Author(s): Nick Thompson, Paul Isaacs, David Mott, Gavin Bell
|		Alain Dumesny
|
______________  S I L I C O N   G R A P H I C S   I N C .  ____________
_______________________________________________________________________
*/

#ifndef _SO_XT_
#define _SO_XT_

#include <X11/Intrinsic.h>
#include <X11/extensions/XI.h>
#include <Xm/Xm.h>
#include <Inventor/SbBasic.h>
#include <Inventor/SbLinear.h>

class SbPList;
class SoEvent;
class SoXtEventHandler;

class SoXt {
  public:
    // This binds Inventor with Xt so that they may work together.
    // It calls SoDB::init(), SoNodeKit::init(), SoInteraction::init
    // and XtAppInitialize, and returns the top level shell widget.
    static Widget           init(const char *appName,
                                 const char *className = "Inventor");

    // This alternate form of init allows the app to initialize Xt.
    // This calls SoDB::init(), SoNodeKit::init(), SoInteraction::init
    // C-api: name=initWithWidget
    static void             init(Widget topLevelWidget);

    // This retrieves and dispatches events (loops forever).
    // Unlike Xt which ignores extension devices, this mainLoop() 
    // will dispatch events from input extension devices like the spaceball.
    // It calls SoXt::nextEvent() and SoXt::dispatchEvent() to do this.
    static void             mainLoop();
    
    // Get the nextEvent() by calling XtAppNextEvent().
    // appContext can come from SoXt::getAppContext().
    static void		    nextEvent(XtAppContext appContext, XEvent *event)
				{ XtAppNextEvent(appContext, event); }

    // Dispatch the passed event to a handler. Normal events are dispatched
    // by calling XtDispatchEvent(). Events from input extension devices
    // are dispatched by code found here.
    // This returns True if a handler was found, else it returns False.
    static Boolean	    dispatchEvent(XEvent *event);

    // These are access routines
    static XtAppContext     getAppContext();
    static Display *        getDisplay();
    static Widget           getTopLevelWidget();
    
    //
    // Convenience routines
    //
    
    // Convenience routine which shows/hides the passed widget.
    //
    // for a shell widget, this is equivalent to calling:
    //		XtRealizeWidget() + XMapWindow()
    //	    or	XMapRaised() if window already exists (raise and de-iconify)
    //	    hide() will then call XUnmapWindow() (instead of XUnrealizeWidget
    //	    which doesn't generate unmap events and destroys the windows).
    //
    // for a subwidget, those simply calls XtManageChild() and XtUnmanageChild().
    //
    static void		    show(Widget widget);
    static void		    hide(Widget widget);
    
    // Convenience routines to convert a char* to an XmString and back.
    // Encoding uses the XmSTRING_DEFAULT_CHARSET.
    // The app is responsible for freeing memory afterwards.
    // (Call XmStringFree() to free an XmString, and free() to free the char*.)
    // C-api: name=encodeStr
    static XmString	    encodeString(char *s);
    // C-api: name=decodeStr
    static char *	    decodeString(XmString xs);
    
    // Convenience routine which sets/gets the size of the given widget
    // (Equivalent to calling XtSetValues()/XtGetValues() on XtNheight
    // and XtNwidth widget resources).
    static void		    setWidgetSize(Widget w, const SbVec2s &size);
    static SbVec2s	    getWidgetSize(Widget w);
    
    // Convenience routine which will return the ShellWidget 
    // containing the given widget. The widget tree is traversed up until
    // a shell widget is found (using XtIsShell()).
    static Widget	    getShellWidget(Widget w);
    
    // Convenience routine which brings a simple motif error dialog
    // box displaying the given error string(s) and window title. The OK
    // button, which distroys the dialog, is the only button displayed.
    // The argument widget is used to create the error dialog (using 
    // XmCreateErrorDialog()) which will be centered around
    // C-api: name=createSimpleErrDlog
    static void		    createSimpleErrorDialog(
				    Widget widget, char *dialogTitle, 
				    char *errorStr1, char *errorStr2 = NULL);
    
    // Convenience routine which gets visual args for the popup planes.
    // These args can then be passed in to XmCreatePulldownMenu or
    // XmCreatePopupMenu to create menus in the popup planes.
    // registerColormapLoad() must be called for each pulldown menu to
    // properly load/unload the overlay color map on the shell when needed.
    static void		    getPopupArgs(
				Display *d, int scr,
				ArgList args, int *n);
    
    // Convenience routine which will register map/unmap callbacks to 
    // load and unload the pulldown menu color map right before the menu
    // is about to be posted. This should be called when creating 
    // pulldown or popup menus in the overlays. This will make sure that
    // the right color map is loaded into the shell widget (to make
    // the pulldown/popup look right) and remove it when no longer needed 
    // (to make sure the OpenGL overlay widget color map is correctly loaded
    // for 8 bit machines).
    // C-api: name=regColMapLoad
    static void		    registerColormapLoad(Widget widget, Widget shell);
    
    // Convenience routine to insert/remove the given widget colormap onto the
    // supplied shell widget. This will not replace the existing installed
    // colormaps (or list of windows), but instead insert the new 
    // colormap first into the existing list using XGetWMColormapWindows() and 
    // XSetWMColormapWindows().
    static void		    addColormapToShell(Widget widget, Widget shell);
    // C-api: name=remColMapFromShell
    static void		    removeColormapFromShell(Widget widget, Widget shell);

SoINTERNAL public:
    // Add/remove the passed event handler for X extension device events
    // (Xt does not handle extension events.)
    // Extension event types are queried from the server at runtime.
    static void		    addExtensionEventHandler(
				Widget w,
				int extensionEventType,
				XtEventHandler proc, 
				XtPointer clientData);
    static void		    removeExtensionEventHandler(
				Widget w,
				int extensionEventType,
				XtEventHandler proc, 
				XtPointer clientData);
  protected:
    static void		    getExtensionEventHandler(
				XEvent *event, 
				Widget &w, 
				XtEventHandler &proc, 
				XtPointer &clientData);

  private:
    static Widget           mainWidget;
    static SoXtEventHandler *eventHandler;
    static SbPList	    *handlerList;
};

#endif  /* _SO_XT_ */
