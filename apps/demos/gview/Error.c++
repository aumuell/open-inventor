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

#include <X11/Intrinsic.h>

#include <Xm/MessageB.h>

#include "Error.h"
#include "MotifHelp.h"

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor - takes parent widget and error message.
//

Error::Error(Widget parent, const char *message)
//
////////////////////////////////////////////////////////////////////////
{
    ARG_VARS(10);

    deleteMe = TRUE;

    ADD_ARG(XmNmessageString,
	    XmStringCreateLtoR((char *) message, XmSTRING_DEFAULT_CHARSET));
    widget = XmCreateErrorDialog(parent, "Error", ARGS);

    // Get rid of buttons we don't need
    XtUnmanageChild(XmMessageBoxGetChild(widget, XmDIALOG_CANCEL_BUTTON));
    XtUnmanageChild(XmMessageBoxGetChild(widget, XmDIALOG_HELP_BUTTON));

    // Set up callbacks to use "this"
    XtAddCallback(widget, XmNdestroyCallback,
		  (XtCallbackProc) &Error::destroyCB, (XtPointer) this);
    XtAddCallback(widget, XmNokCallback,
		  (XtCallbackProc) &Error::okCB,  (XtPointer) this);

    // Display widget
    XtManageChild(widget);
}    

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//

Error::~Error()
//
////////////////////////////////////////////////////////////////////////
{
    // Hide widget
    XtUnmanageChild(widget);
    XtDestroyWidget(widget);
}    

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback called when the widget is destroyed. The clientData
//    pointer points to a Error instance.
//

void
Error::destroyCB(Widget, XtPointer clientData, XtPointer)
//
////////////////////////////////////////////////////////////////////////
{
    // Note that this will be called when the ok button is pressed, so
    // be careful!!!!

    Error	*err = (Error *) clientData;

    // Make sure we haven't already started the deletion of the error-box
    if (err->deleteMe) {
	err->deleteMe = FALSE;
	delete err;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback called when the "Ok" button in the error-box is
//    activated. The clientData pointer points to a Error instance.
//

void
Error::okCB(Widget, XtPointer clientData, XtPointer)
//
////////////////////////////////////////////////////////////////////////
{
    Error	*err = (Error *) clientData;

    err->deleteMe = FALSE;
    delete err;
}
