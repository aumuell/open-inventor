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

#include <stdlib.h>

#include <X11/Intrinsic.h>

#include <Xm/Form.h>
#include <Xm/SelectioB.h>
#include <Xm/Text.h>
#include <Xm/ToggleBG.h>

#include <Inventor/SoType.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/draggers/SoDragger.h>
#include <Inventor/misc/upgraders/SoUpgrader.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoSelection.h>

#include "Error.h"
#include "NodeCreator.h"
#include "MotifHelp.h"

// The widget is static so we can leave it around between invocations
Widget	NodeCreator::widget = NULL;
Widget	NodeCreator::selBox;

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor - takes parent widget and callback to call when a
//    node name is selected.
//

NodeCreator::NodeCreator(Widget parent,
			 NodeCreatorCB *cb, const void *userData)
//
////////////////////////////////////////////////////////////////////////
{
    // Set callback
    finishCB   = cb;
    finishData = (void *) userData;

    deleteMe = TRUE;

    if (widget == NULL)
	buildWidget(parent);

    // Get rid of old callbacks that used old "this" pointers
    else {
	XtRemoveAllCallbacks(selBox, XmNdestroyCallback);
	XtRemoveAllCallbacks(selBox, XmNokCallback);
	XtRemoveAllCallbacks(selBox, XmNcancelCallback);
    }

    // Set up callbacks to use "this"
    XtAddCallback(selBox, XmNdestroyCallback,
		  (XtCallbackProc) &NodeCreator::destroyCB, (XtPointer) this);
    XtAddCallback(selBox, XmNokCallback,
		  (XtCallbackProc) &NodeCreator::acceptCB,  (XtPointer) this);
    XtAddCallback(selBox, XmNcancelCallback,
		  (XtCallbackProc) &NodeCreator::cancelCB,  (XtPointer) this);

    // Display widget
    XtManageChild(widget);
}    

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//

NodeCreator::~NodeCreator()
//
////////////////////////////////////////////////////////////////////////
{
    // Hide widget
    XtUnmanageChild(widget);
}    

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Builds widget.
//

void
NodeCreator::buildWidget(Widget parent)
//
////////////////////////////////////////////////////////////////////////
{
    XmString	*strings;
    int		numStrings;
    ARG_VARS(10);

    // Get array of all valid node names as XmStrings
    getNodeNames(strings, numStrings);

    // Build widget
    RESET_ARGS();
    ADD_ARG(XmNautoUnmanage,		FALSE);
    ADD_ARG(XmNlistItems,		strings);
    ADD_ARG(XmNlistItemCount,		numStrings);
    ADD_ARG(XmNlistLabelString,		STRING("Available node types"));
    ADD_ARG(XmNokLabelString,		STRING("Accept"));
    selBox = XmCreateSelectionDialog(parent, "NodeCreator", ARGS);

    // Get rid of buttons we don't need
    XtUnmanageChild(XmSelectionBoxGetChild(selBox, XmDIALOG_APPLY_BUTTON));
    XtUnmanageChild(XmSelectionBoxGetChild(selBox, XmDIALOG_HELP_BUTTON));

    widget = selBox;
}    

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback called when the widget is destroyed. The clientData
//    pointer points to a NodeCreator instance.
//

void
NodeCreator::destroyCB(Widget, XtPointer clientData, XtPointer)
//
////////////////////////////////////////////////////////////////////////
{
    // Note that this will be called when the accept or cancel buttons
    // is pressed, so be careful!!!!

    NodeCreator	*creator = (NodeCreator *) clientData;

    // Make sure the widget gets recreated next time
    widget = NULL;

    // Make sure we haven't already started the deletion of the creator
    if (creator->deleteMe) {
	creator->deleteMe = FALSE;
	delete creator;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback called when the "Accept" button in the creator is
//    activated. The clientData pointer points to a NodeCreator instance.
//

void
NodeCreator::acceptCB(Widget, XtPointer clientData, XtPointer callData)
//
////////////////////////////////////////////////////////////////////////
{
    NodeCreator	*creator = (NodeCreator *) clientData;
    XmSelectionBoxCallbackStruct *cbInfo =
	(XmSelectionBoxCallbackStruct *) callData;

    // Get the current string and pass to the callback
    char *str = decodeString(cbInfo->value);
    creator->invokeCallback(str[0] == '\0' ? NULL : str);

    // Remove the node creator
    creator->deleteMe = FALSE;
    delete creator;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback called when the "Cancel" button in the creator is
//    activated. The clientData pointer points to a NodeCreator instance.
//

void
NodeCreator::cancelCB(Widget, XtPointer clientData, XtPointer)
//
////////////////////////////////////////////////////////////////////////
{
    NodeCreator	*creator = (NodeCreator *) clientData;

    creator->invokeCallback(NULL);

    // Remove the node creator
    creator->deleteMe = FALSE;
    delete creator;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns text part of first segment of compound string. Note that
//    segments after first are ignored.
//

char *
NodeCreator::decodeString(XmString string)
//
////////////////////////////////////////////////////////////////////////
{
    XmStringContext	ctx;
    char		*text = NULL;
    XmStringCharSet	charset;
    XmStringDirection	dirRtoL;
    Boolean		separator;

    if (! XmStringInitContext(&ctx, string) ||
	! XmStringGetNextSegment(ctx, &text, &charset, &dirRtoL, &separator)) {
	fprintf(stderr, "Can't decode string\n");
        exit(1);
    }

    return text;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns an array of XmStrings containing the names of all known
//    node classes.
//

void
NodeCreator::getNodeNames(XmString *&strings, int &numStrings)
//
////////////////////////////////////////////////////////////////////////
{
    SoTypeList	allNodes;
    const char	**typeNames;
    int		numTypes, curType, i;
    SoType	type;

    // Find all types derived from SoNode
    numTypes = SoType::getAllDerivedFrom(SoNode::getClassTypeId(), allNodes);

    // Allocate an array to hold names as character strings
    typeNames = (const char **) new char *[numTypes];

    // Gather all valid names into array
    curType = 0;
    for (i = 0; i < numTypes; i++) {

	type = allNodes[i];

	// See if the type corresponds to a non-abstract node class.
	// We also don't want no stinkin' upgraders or draggers in our
	// list!
	if (type.canCreateInstance() &&
	    ! type.isDerivedFrom(SoUpgrader::getClassTypeId()) &&
	    ! type.isDerivedFrom(SoDragger::getClassTypeId()))
	    typeNames[curType++] = type.getName().getString();
    }

    numStrings = curType;

    // Sort the array
    qsort(typeNames, numStrings, sizeof(char *), &NodeCreator::nameCompare);

    // Store the results in an array of XmStrings
    strings = new XmString[numStrings];

    for (i = 0; i < numStrings; i++)
	strings[i] = STRING((char *) typeNames[i]);

    delete [] typeNames;
}    

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Used to sort the node names for getNodeNames().
//

int
NodeCreator::nameCompare(const void *s1, const void *s2)
//
////////////////////////////////////////////////////////////////////////
{
    return strcmp(* (const char **) s1, * (const char **) s2);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Creates instance of named node and calls user's callback
//    function. The name may be a NULL pointer.
//

void
NodeCreator::invokeCallback(const char *nodeName)
//
////////////////////////////////////////////////////////////////////////
{
    SoType	type;
    SoNode	*newNode;

    if (finishCB == NULL)
	return;

    if (nodeName == NULL)
	newNode = NULL;

    else {
	// See if there is a node class with the given name
	type = SoType::fromName(nodeName);
	if (type.isBad() || ! type.isDerivedFrom(SoNode::getClassTypeId())) {

	    char	buf[200];

	    sprintf(buf, "There is no node class named \"%s\".", nodeName);
	    Error *err = new Error(XtParent(XtParent(widget)), buf);

	    return;
	}

	// Can't create an instance if the node class is abstract
	if (! type.canCreateInstance()) {
	    char	buf[200];

	    sprintf(buf, "The node class \"%s\" is abstract\n%s.",
		    nodeName, "and cannot be instantiated");
	    Error *err = new Error(XtParent(XtParent(widget)), buf);

	    return;
	}

	newNode = (SoNode *) type.createInstance();
    }

    (*finishCB)(finishData, newNode);
}
