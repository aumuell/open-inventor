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
 |   $Revision: 1.6 $
 |
 |   Classes:
 |	SoXtComponent
 |
 |   Author(s): David Mott, Alain Dumesny
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */
#include <Inventor/SbDict.h>
#include <Inventor/misc/SoCallbackList.h>
#include <Inventor/Xt/SoXtComponent.h>
#include <Inventor/Xt/SoXt.h>

#include <Inventor/errors/SoDebugError.h>

#include <stdio.h>
#include <stdlib.h> // for system() and getenv()
#include <unistd.h> // for access()
#include <string.h> // for strcpy() and strcat()

#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>

#include <Xm/Xm.h>
#include <Xm/AtomMgr.h>
#include <Xm/MessageB.h>
#include <Xm/Protocols.h>

#include <GL/gl.h>


static char *helpDialogTitle = "Help Card Error Dialog";
static char *helpCardError = "Inventor Help Cards not installed.";
static char *helpPrgError = "acroread not installed on this system.";
static char *thisClassName = "SoXtComponent";

// static members
SbDict *SoXtComponent::widgetDictionary = NULL;
    
////////////////////////////////////////////////////////////////////////
//
// Constructor
//
SoXtComponent::SoXtComponent(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    // make sure Display is valid
    if (SoXt::getDisplay() == NULL) {
	SoDebugError::post("SoXtComponent::SoXtComponent",
	"SoXt::getDisplay() returned a NULL Display. Be sure to call SoXt::init() first.");
	return;
    }
#endif
    
    if (widgetDictionary == NULL)
    	widgetDictionary = new SbDict;
    
    setClassName(thisClassName);
    if (name != NULL)
	 _name = strdup(name);
    else _name = NULL;
    
    firstRealize    = TRUE; // set to FALSE after we are realized once
    title   	    = NULL;
    iconTitle	    = NULL;
    size.setValue(0, 0);
    
    // This is set by subclasses through setBaseWidget()
    _baseWidget = NULL;
    
    // create a shell window if necessary
    createdShell = (parent == NULL || !buildInsideParent);
    topLevelShell = (createdShell || (parent != NULL && XtIsShell(parent)));
    if (createdShell) {
	Widget parentShell = (parent != NULL) ? parent : SoXt::getTopLevelWidget();
	parentWidget = XtCreatePopupShell(
		    getWidgetName(),
		    topLevelShellWidgetClass, 
		    parentShell,
		    NULL, 0);
    }
    else parentWidget = parent;
    
    // don't make the close action destroy the widget, instead
    // register a custom close action which will default to
    // hide() the component (as opposed to destroy which is the
    // default shell behavior)
    if (topLevelShell) {
	XtVaSetValues(parentWidget, XmNdeleteResponse, XmDO_NOTHING, NULL);
        Atom wmDeleteAtom = XmInternAtom(XtDisplay(parentWidget),
                "WM_DELETE_WINDOW", False);
        XmAddWMProtocolCallback(parentWidget, wmDeleteAtom, 
                (XtCallbackProc) SoXtComponent::windowCloseActionCB,
		(XtPointer) this);
    }
    
    // visibility stuff (check to see if shell is already mapped)
    widgetMapped = FALSE;
    visibleState = FALSE;
    ShellMapped = XtIsRealized(SoXt::getShellWidget(parentWidget));
    visibiltyCBList = NULL; // allocate as needed
    windowCloseFunc = NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Destructor
//
SoXtComponent::~SoXtComponent()
//
////////////////////////////////////////////////////////////////////////
{
    // Remove the destroy callback before we destroy the widget.
    // Also, remove stuff we added to the shell or to our parent widget.
    if (_baseWidget != NULL) {
	XtRemoveCallback(_baseWidget, XmNdestroyCallback,
	    SoXtComponent::widgetDestroyedCB, (XtPointer) this);

	XtRemoveEventHandler(_baseWidget, StructureNotifyMask, FALSE,
	    (XtEventHandler) SoXtComponent::widgetStructureNotifyCB,
	    (XtPointer) this);
	
	Widget shellWidget = SoXt::getShellWidget(_baseWidget);
	if ((shellWidget != NULL) && (shellWidget != _baseWidget))
	    XtRemoveEventHandler(shellWidget, StructureNotifyMask, FALSE,
		(XtEventHandler) SoXtComponent::shellStructureNotifyCB,
		(XtPointer) this);
	
	if (topLevelShell && (parentWidget != NULL)) {
	    Atom wmDeleteAtom = XmInternAtom(XtDisplay(parentWidget),
		    "WM_DELETE_WINDOW", False);
	    XmRemoveWMProtocolCallback(parentWidget, wmDeleteAtom, 
		    (XtCallbackProc) SoXtComponent::windowCloseActionCB,
		    (XtPointer) this);
	}
    }
    
    // Destroy the topmost widget.
    if (getShellWidget() != NULL && createdShell)
	XtDestroyWidget(parentWidget);
    else if (_baseWidget != NULL)
	XtDestroyWidget(_baseWidget);
    
    if (_name != NULL)	    free(_name);
    if (title != NULL)	    free(title);
    if (iconTitle != NULL)  free(iconTitle);
    
    delete visibiltyCBList;
}

////////////////////////////////////////////////////////////////////////
//
// Set the base widget and install the destroy handler.
//
SbBool
SoXtComponent::isVisible()
//
////////////////////////////////////////////////////////////////////////
{
    checkForVisibilityChange();
    
    return visibleState;
}

////////////////////////////////////////////////////////////////////////
//
// check to see if visibilaty has change since last time, in which
// case we would invoke the visibilty callbacks.
//
void
SoXtComponent::checkForVisibilityChange()
//
////////////////////////////////////////////////////////////////////////
{
    SbBool prevState = visibleState;
    
    // make sure to check if the window exists (since we DON'T receive
    // unmaps events when calling XtUnrealizeWidget(shell) - see
    // the hide() method for comments.
    visibleState = (ShellMapped && widgetMapped && _baseWidget 
	&& XtWindow(_baseWidget));
    
    if (visibleState != prevState && visibiltyCBList)
	    visibiltyCBList->invokeCallbacks((void *)(unsigned long)visibleState);    
}

////////////////////////////////////////////////////////////////////////
//
// adds a callback to be called when visibility of this comp changes
//
void
SoXtComponent::addVisibilityChangeCallback(
    SoXtComponentVisibilityCB *f, void *userData)
//
////////////////////////////////////////////////////////////////////////
{
    if (visibiltyCBList == NULL)
	visibiltyCBList = new SoCallbackList;
    
    visibiltyCBList->addCallback((SoCallbackListCB *) f, userData);
}

////////////////////////////////////////////////////////////////////////
//
// removes a callback to be called when visibility of this comp changes
//
void
SoXtComponent::removeVisibilityChangeCallback(
    SoXtComponentVisibilityCB *f, void *userData)
//
////////////////////////////////////////////////////////////////////////
{
    if (visibiltyCBList == NULL)
	return;
    
    visibiltyCBList->removeCallback((SoCallbackListCB *) f, userData);
}

////////////////////////////////////////////////////////////////////////
//
// Set the base widget and install the destroy handler.
//
void
SoXtComponent::setBaseWidget(Widget w)
//
////////////////////////////////////////////////////////////////////////
{ 
    _baseWidget = w;
    
    // Set callback so we know when the widget is destroyed
    XtAddCallback (_baseWidget, XmNdestroyCallback,
		   SoXtComponent::widgetDestroyedCB, (XtPointer) this );
    
    // now add event handlers to receive map/unmap events
    // for visibility test.
    XtAddEventHandler(_baseWidget, StructureNotifyMask, FALSE,
	(XtEventHandler) SoXtComponent::widgetStructureNotifyCB,
	(XtPointer) this);
    
    Widget shellWidget = SoXt::getShellWidget(_baseWidget);
    if ((shellWidget != NULL) && (shellWidget != _baseWidget))
	XtAddEventHandler(shellWidget, StructureNotifyMask, FALSE,
	    (XtEventHandler) SoXtComponent::shellStructureNotifyCB,
	    (XtPointer) this);
}

////////////////////////////////////////////////////////////////////////
//
// define those generic virtual functions
//
////////////////////////////////////////////////////////////////////////

const char *
SoXtComponent::getDefaultWidgetName() const
{ return thisClassName; }  // return name of this class

const char *
SoXtComponent::getDefaultTitle() const
{ return "Xt Component"; }

const char *
SoXtComponent::getDefaultIconTitle() const
{ return "Xt Comp"; }

////////////////////////////////////////////////////////////////////////
//
// This is called after the widget has been realized (shown).
//
// public virtual
//
void
SoXtComponent::afterRealizeHook()
//
////////////////////////////////////////////////////////////////////////
{
    if (topLevelShell) {
	
	// set the title of the window if widget is directly under a shell
	if (title == NULL)
	    title = strdup(getDefaultTitle());
	if (iconTitle == NULL)
	    iconTitle = strdup(getDefaultIconTitle());
	XtVaSetValues(parentWidget,
	    XtNtitle, title, XmNiconName, iconTitle, NULL);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Resizes the outer most widget.
//
// Use: public
void
SoXtComponent::setSize(const SbVec2s &newSize)
//
////////////////////////////////////////////////////////////////////////
{
    if (getShellWidget() != NULL)
	SoXt::setWidgetSize(parentWidget, newSize);
    else if (_baseWidget != NULL)
	SoXt::setWidgetSize(_baseWidget, newSize);
    
    // cache value for later builds
    size = newSize;
}

////////////////////////////////////////////////////////////////////////
//
// Returns the outermost widget size.
//
// Use: public
SbVec2s
SoXtComponent::getSize()
//
////////////////////////////////////////////////////////////////////////
{
    // The real values are stored in the widget, so get and cache the
    // widget size.
    if (getShellWidget() != NULL)
	size = SoXt::getWidgetSize(parentWidget);
    else if (_baseWidget != NULL)
	size = SoXt::getWidgetSize(_baseWidget);
    
    return size;
}

////////////////////////////////////////////////////////////////////////
//
// set the title if the widget is directly under a shell widget
//
void
SoXtComponent::setTitle(const char *newTitle)
//
////////////////////////////////////////////////////////////////////////
{
    if (title != NULL) free(title);
    title = (newTitle != NULL) ? strdup(newTitle) : NULL;
    
    if (title != NULL && _baseWidget != NULL && XtIsShell(XtParent(_baseWidget)))
	XtVaSetValues(XtParent(_baseWidget), XtNtitle, title, NULL);
}

////////////////////////////////////////////////////////////////////////
//
// set the icon title if the widget is directly under a shell widget
//
void
SoXtComponent::setIconTitle(const char *newTitle)
//
////////////////////////////////////////////////////////////////////////
{
    if (iconTitle != NULL) free(iconTitle);
    iconTitle = (newTitle != NULL) ? strdup(newTitle) : NULL;
    
    if (iconTitle != NULL && _baseWidget != NULL && XtIsShell(XtParent(_baseWidget)))
	XtVaSetValues(XtParent(_baseWidget), XmNiconName, iconTitle, NULL);
}

////////////////////////////////////////////////////////////////////////
//
// show - display the component.
//
void
SoXtComponent::show()
//
////////////////////////////////////////////////////////////////////////
{
    // Display that widget!
    SoXt::show(_baseWidget);
    
    // Show the shell if we created it.
    if (topLevelShell && createdShell)
	 SoXt::show(parentWidget);
}

////////////////////////////////////////////////////////////////////////
//
// hide - hide the component.
//
void
SoXtComponent::hide()
//
////////////////////////////////////////////////////////////////////////
{
#if 0
// do not do this check - the motif slider set code assumes
// hide will unmanage the widget even if we aren't shown yet.
// this is needed because that code makes everything managed 
// at creation time, then goes through and unmanages...

    // if we are already not visible, simply return....
    if (! isVisible())
	return;
#endif
    
    if (topLevelShell) {
	// save the window position to enable the component to
	// come back at the same place next time around.
	if (XtWindow(parentWidget)) {
	    short px, py;
	    XtVaGetValues(parentWidget, XmNx, &px, XmNy, &py, NULL);
	    XSizeHints hints;
	    hints.flags = USPosition;
	    hints.x = px;
	    hints.y = py;
	    XSetNormalHints(XtDisplay(parentWidget), XtWindow(parentWidget), &hints);
	}
	
	SoXt::hide(parentWidget); // this calls XUnmapWindow() (generate unmap)
    }
    else
	SoXt::hide(getWidget()); // this calls XtUnmanageChild() (generate unmap)
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Hide() the component when users close the window, instead of
//  destroying the widget (default shell behavior)
//
// Use: virtual protected
//
void
SoXtComponent::windowCloseAction()
//
////////////////////////////////////////////////////////////////////////
{
    if (parentWidget == SoXt::getTopLevelWidget())
	exit(0);
    else
	hide();
}

////////////////////////////////////////////////////////////////////////
//
// Return the component for this widget, or NULL if w is not an SoXtComponent.
//
// Use: static, public
//
SoXtComponent *
SoXtComponent::getComponent(Widget w)
//
////////////////////////////////////////////////////////////////////////
{
    if (w == NULL)
    	return NULL;

    // find the component in the dictionary of widgets
    // key is 'widget', data is 'component'.
    void *data = NULL;
    SoXtComponent::widgetDictionary->find((unsigned long) w, data);
    
    return (SoXtComponent *) data;
}


////////////////////////////////////////////////////////////////////////
//
// Register the widget into the dictionary of all widgets.
// This is called by subclasses in their buildWidget routine.
// key is 'widget', data is 'this'.
//
// Use: protected
//
void
SoXtComponent::registerWidget(Widget w)
//
////////////////////////////////////////////////////////////////////////
{
    // make sure this widget isn't already registered
    void *data = NULL;
    if (SoXtComponent::widgetDictionary->find((unsigned long) w, data)) {
#ifdef DEBUG
    	// the widget IS registered.
	// if it's not for this component, complain.
	// otherwise remove it from the registry, and enter it again
	// (probably a subclass registering after its parent class did).
    	SoXtComponent *comp = (SoXtComponent *) data;
	if (comp != this) {
	    SoDebugError::post("SoXtComponent::registerWidget",
		"widget registered for 2 different components! old class: %s, new class: %s", comp->getClassName(), getClassName());
	}
#endif
    }
    else {
	// Register the widget with this component
	SoXtComponent::widgetDictionary->enter((unsigned long) w, this);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Unregister the widget from the dictionary of all widgets.
// This is called by subclasses in their destroyWidget routine.
//
// Use: protected
//
void
SoXtComponent::unregisterWidget(Widget w)
//
////////////////////////////////////////////////////////////////////////
{
    if (widgetDictionary != NULL)
	widgetDictionary->remove((unsigned long) w);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine is called by subclasses to open the specific help
//  card given the file name. By default the file will be searched using:
//	    1) current working directory
//	    2) SO_HELP_DIR environment variable
//	    3) $(IVPREFIX)/share/help/Inventor
//	    4) else bring a "No help card found message"
//
// Use: protected
//
void
SoXtComponent::openHelpCard(const char *cardName)
//
////////////////////////////////////////////////////////////////////////
{
    char pgrCmd[100];
    char cardPath[100];
    strcpy(pgrCmd, "acroread ");
    
#if 0
    // ??? make showcase come up single buffered on the Indigo. This should
    // ??? be the default behavior for showcase in view only mode. (bug 107547)
//???alain- check this out, make sure it'll still work.
    int rbits, gbits, bbits;
    glGetIntegerv(GL_RED_BITS,  &rbits); 
    glGetIntegerv(GL_GREEN_BITS,&gbits);
    glGetIntegerv(GL_BLUE_BITS, &bbits);
    int32_t bitnum =     rbits + gbits + bbits;


    if (bitnum < 12)
	strcat(pgrCmd, "-b ");	// single buffer mode
#endif

    char command[100];
    sprintf(command, "which acroread > /dev/null");
    if (system(command) != 0) {
	SoXt::createSimpleErrorDialog(_baseWidget, helpDialogTitle, helpPrgError);
	return;
    }
    
    // check if the file is located in current directory
    if ( access(cardName, R_OK) == 0 ) {
	strcat(pgrCmd, cardName);
	strcat(pgrCmd, " &");
	if (system(pgrCmd) != 0)
	    SoXt::createSimpleErrorDialog(_baseWidget, helpDialogTitle, helpPrgError);
	return;
    }
    
    // else check for the env variable 
    char *envPath = getenv("SO_HELP_DIR");
    if (envPath != NULL) {
	strcpy(cardPath, envPath);
	strcat(cardPath, "/");
	strcat(cardPath, cardName);
	if ( access(cardPath, R_OK) == 0 ) {
	    strcat(pgrCmd, cardPath);
	    strcat(pgrCmd, " &");
	    if (system(pgrCmd) != 0)
		SoXt::createSimpleErrorDialog(_baseWidget, helpDialogTitle, helpPrgError);
	    return;
	}
    }
    
    // else check for the default location
    strcpy(cardPath, IVPREFIX "/share/help/Inventor/");
    strcat(cardPath, cardName);
    if ( access(cardPath, R_OK) == 0 ) {
	strcat(pgrCmd, cardPath);
	strcat(pgrCmd, " &");
	if (system(pgrCmd) != 0)
	    SoXt::createSimpleErrorDialog(_baseWidget, helpDialogTitle, helpPrgError);
	return;
    }
    
    //
    // else bring an error message using a motif dialog box
    //
    SoXt::createSimpleErrorDialog(_baseWidget, helpDialogTitle, helpCardError);
}


////////////////////////////////////////////////////////////////////////
//
//  Reset pointers to NULL - the widget has been destroyed.
//
//  protected virtual
//
void
SoXtComponent::widgetDestroyed()
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    SoDebugError::post("SoXtComponent::widgetDestroyed",
	"widget was destroyed, but there is no way to build it again.", 
	"Instead, the component should be deleted.");
#endif
    _baseWidget = NULL;
}

//
////////////////////////////////////////////////////////////////////////
// static callbacks stubs
////////////////////////////////////////////////////////////////////////
//

// Called if _baseWidget gets destroyed.
void
SoXtComponent::widgetDestroyedCB(
    Widget, 
    XtPointer clientData, 
    XtPointer)
{
    SoXtComponent *c = (SoXtComponent *) clientData;
    c->widgetDestroyed();
}

//
// called whenever the shell receives a WM_DELETE_WINDOW message
// (close action from menu)
//
void
SoXtComponent::windowCloseActionCB(Widget, SoXtComponent *p, void *)
{
    if (p->windowCloseFunc != NULL)
	(*p->windowCloseFunc) (p->windowCloseData, p);
    else
	p->windowCloseAction();
}

//
// called whenever the base widget gets mapped/unmapped (and other
// stuctureNotify type of events which we don't care about)
//
void
SoXtComponent::widgetStructureNotifyCB(Widget, SoXtComponent *p, XEvent *xe, Boolean *)
{
    if (xe->type == MapNotify) {
	// Call the after-realize hook the first time we are realized
	if (p->firstRealize) {
	    p->afterRealizeHook();
	    p->firstRealize = FALSE;
	}
	
	p->widgetMapped = TRUE;
	p->checkForVisibilityChange();
    }
    else if (xe->type == UnmapNotify) {
	p->widgetMapped = FALSE;
	p->checkForVisibilityChange();
    }
}

//
// called whenever the shell widget gets mapped/unmapped (and other
// stuctureNotify type of events which we don't care about)
//
void
SoXtComponent::shellStructureNotifyCB(Widget, SoXtComponent *p, XEvent *xe, Boolean *)
{
    if (xe->type == MapNotify) {
	p->ShellMapped = TRUE;
	p->checkForVisibilityChange();
    }
    else if (xe->type == UnmapNotify) {
	p->ShellMapped = FALSE;
	p->checkForVisibilityChange();
    }
}
