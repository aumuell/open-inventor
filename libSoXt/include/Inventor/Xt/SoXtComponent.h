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
 * Copyright (C) 1990-93   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.2 $
 |
 |   Description:
 |	This is an abstract base class which defines a c++ protocol
 |	for motif components. 
 |
 |   Author(s): David Mott, Alain Dumesny
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef _SO_XT_COMPONENT_H_
#define _SO_XT_COMPONENT_H_

#include <X11/Intrinsic.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SbString.h>

class SbDict;
class SoXtComponent;
class SoCallbackList;

typedef void SoXtComponentCB(void *userData, SoXtComponent *comp);
typedef void SoXtComponentVisibilityCB(void *userData, SbBool visibleFlag);

// C-api: abstract
// C-api: prefix=SoXtComp
class SoXtComponent {
  public:
    //
    // This shows and hides the component. If this is a topLevelShell 
    // component, then show() will Realize and Map the window, else it
    // will simply Manage the widget.
    // Hide calls the appropriate XUnmapWindow() or XtUnmanageChild() 
    // routines (check SoXt::show and hide man pages for detail).
    //
    // In addition, show() will also pop the component window to the top
    // and de-iconify if necessary, to make sure the component is visible
    // by the user.
    //
    // C-api: expose
    virtual void 	show();
    // C-api: expose
    virtual void 	hide();
    
    //
    // returns TRUE if this component is mapped onto the screen. For
    // a component to be visible, it's widget and the shell containing
    // this widget must be mapped (which is FALSE when the component
    // is iconified).
    //
    // Subclasses should call this routine before redrawing anything
    // and in any sensor trigger methods. Calling this will check the 
    // current visibility (which is really cheap) and invoke the
    // visibility changed callbacks if the state changes (see
    // addVisibilityChangeCallback()).
    //
    SbBool   		isVisible();
    
    //
    // This returns the base widget for this component.
    // If the component created its own shell, this returns the 
    // topmost widget beneath the shell.
    // Call getShellWidget() to obtain the shell.
    //
    Widget		getWidget() const { return _baseWidget; }
    Widget		baseWidget() const { return getWidget(); }
    
    //
    // Returns TRUE if this component is a top level shell component (has its
    // own window). Subclasses may use this to decide if they are allowed to
    // resize themselfves.
    // Also method to return the shell widget (NULL if the shell hasn't been
    // created by this component).
    //
    SbBool		isTopLevelShell() const	    { return topLevelShell; }
    Widget		getShellWidget() const
			    { return topLevelShell ? parentWidget : NULL; }

    // Return the parent widget, be it a shell or not
    Widget		getParentWidget() const	    { return parentWidget; }
    
    //
    // Convenience routines on the widget - setSize calls XtSetValue.
    //
    void		setSize(const SbVec2s &size);
    SbVec2s		getSize();
    
    inline Display  	*getDisplay();
    
    //
    // The title can be set for topLevel shell components or components
    // which are directly under a shell widget (i.e. components which 
    // have their own window).
    //
    void    	    	setTitle(const char *newTitle);
    const char *    	getTitle() const		{ return title; }
    void    	    	setIconTitle(const char *newIconTitle);
    const char *    	getIconTitle() const		{ return iconTitle; }
    
    //
    // sets which callback to call when the user closes this component
    // (double click in the upper left corner) - by default hide() is 
    // called on this component, unless a callback is set to something
    // other than NULL. A pointer to this class will be passed as the
    // callback data.
    //
    // Note: there is only one callback because the user may decide to
    // delete this component when it is closed.
    //
    // C-api: name=setWinCloseCB
    void		setWindowCloseCallback(
			    SoXtComponentCB *func, void *data = NULL)
					{ windowCloseFunc = func; windowCloseData = data; }
    
    //
    // this returns the SoXtComponent for this widget.
    // If the widget is not a Inventor component, then NULL is returned.
    //
    // C-api: name=getComp
    static SoXtComponent	*getComponent(Widget w);
    
    //
    // Widget name and class - these are used when looking up X resource
    // values for the widget (each subclass provides a className).
    //
    const char *	getWidgetName() const	{ return _name; }    
    const char *	getClassName() const { return _classname.getString(); }
    
    // C-api: expose
    virtual ~SoXtComponent();
    
  protected:
    //
    // If `parent` widget is suplied AND `buildInsideParent` is TRUE, this
    // component will build inside the given parent widget, else
    // it will create its own topLevelShell widget (component resides in 
    // its own window).
    // The topLevelShell can either be created under the given
    // parent shell (`parent` != NULL) or under the main window.
    //
    // The name is used for looking up X resource values. If NULL,
    // then this component inherits resource values defined for its class.
    //
    // Calling setBaseWidget is needed for looking up Xt like 
    // resources in the widget tree. It will use the class name of
    // the Inventor component (e.g. SoXtRenderArea) instead of
    // the class name of the Motif widget this component employs
    // (e.g. XmForm).
    // 
    // Thus apps are able to do this in their app-defaults file:
    //
    // *SoXtRenderArea*BackgroundColor: salmon
    //
    SoXtComponent(
	    Widget parent = NULL,
	    const char *name = NULL, 
	    SbBool buildInsideParent = TRUE);

    // Subclasses need to call this method passing the top most
    // widget after it has been created.
    void		setBaseWidget(Widget w);

    // Subclasses need to set the class name in the constructor
    // before the widget is built.
    void		setClassName(const char *n) { _classname = n; }

    // this routine is called whenever the top level shell widget receives
    // a close action (WM_DELETE_WINDOW message) from the window manager.
    // Instead of destroying the widget (default shell behavior) this
    // routine is used, which by default calls exit(0) if it is the main
    // window else calls hide() on the component.
    //
    virtual void	windowCloseAction();
    
    // Support for doing things right after the widget is realized
    // for the first time. 
    // The base class will set the window and icon title for shell widgets.
    virtual void	afterRealizeHook();
    SbBool		firstRealize;

    //
    // Subclasses should redefine these routines to return the appropriate
    // default information. Those are used when creating the widget to set 
    // the name (used for resources), window title and window icon
    // name. Those default values are only used if the user didn't 
    // explicitly specify them.
    //
    virtual const char *    getDefaultWidgetName() const;
    virtual const char *    getDefaultTitle() const;
    virtual const char *    getDefaultIconTitle() const;
    
    //
    // Register widget - should be called by subclasses after
    // they have created their top most widget (which is passed here),
    // and before they build any child widgets. Calling this method
    // ensures that the widgets name and class will be used when
    // calls are made to get X resource values for this widget.
    //
    // *** NOTE *** 
    // ALL subclasses should register their top most widget within the 
    // component, whether they retrieve resources or not, so that children 
    // widgets can get X resources properly.
    // Unregister the widget when the widget is destroyed.
    //
    void    	    	registerWidget(Widget w);
    void		unregisterWidget(Widget w);
    
    // subclasses can add a callback to be called whenever the component
    // becomes visible or become hidden (like when it is iconified).
    // Sublcasses should use this to attach or detach any sensors they
    // have, or stop any ongoing anymation.
    void		addVisibilityChangeCallback(
				SoXtComponentVisibilityCB *func, 
				void *userData = NULL);
    void		removeVisibilityChangeCallback(
				SoXtComponentVisibilityCB *func, 
				void *userData = NULL);
    
    //
    // This method can be used by subclasses to open a component help
    // card. The name of the file should be supplied withought a path
    // name. By default the file will be searched using:
    //	    1) current working directory
    //	    2) SO_HELP_DIR environment variable
    //	    3) $(IVPREFIX)/share/help/Inventor
    //	    4) else bring a no help card found dialog
    //
    void    openHelpCard(const char *cardName);
    
  private:
    // widgetDestroyed is called when the widget is destroyed.
    // There is no way to reconstruct the widget tree, so calling
    // this simply generates an error. The component should be
    // deleted to dispose of the widget.
    virtual void	widgetDestroyed();
    
    SbBool	topLevelShell;	// TRUE if in its own window
    SbBool	createdShell;	// TRUE if we created that toplevel shell
    Widget	parentWidget;	// topLevel shell if in its own window
    Widget	_baseWidget;	// returned by getWidget()
    char	*_name;		// name of this widget
    char	*title;		// title for window if in its own window
    char	*iconTitle;	// title for icon if in its own window
    SbVec2s	size;		// size of the '_baseWidget' and 'shell' (if toplevel)
    SbString	_classname;
    
    // visibiltity stuff
    SbBool	    visibleState;
    SbBool	    ShellMapped, widgetMapped;
    SoCallbackList  *visibiltyCBList;
    void	    checkForVisibilityChange();
    static void	    widgetStructureNotifyCB(Widget, SoXtComponent *, XEvent *, Boolean *);
    static void	    shellStructureNotifyCB(Widget, SoXtComponent *, XEvent *, Boolean *);
    static void	    widgetDestroyedCB(Widget, XtPointer, XtPointer);
    
    // window close action data
    SoXtComponentCB	*windowCloseFunc;
    void		*windowCloseData;
    static void		windowCloseActionCB(Widget, SoXtComponent *, void *);
    
    // The widget dictionary maps widgets to SoXtComponents. It's used
    // by getComponent(), and kept up to date by registerWidget().
    static SbDict   	*widgetDictionary;
};

// Inline routines
Display *
SoXtComponent::getDisplay()
{ return (_baseWidget != NULL ? XtDisplay(_baseWidget) : NULL); }

#endif // _SO_XT_COMPONENT_H_
