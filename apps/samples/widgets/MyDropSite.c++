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

#include <libgen.h>
#include <sys/param.h>

#include <Xm/AtomMgr.h>
#include <Xm/DragDrop.h>
#include <Xm/Xm.h>

#include <Inventor/SbPList.h>
#include <Inventor/SbString.h>
#include <Inventor/misc/SoByteStream.h>

#include "MyDropSite.h"

Widget		MyDropSite::siteWidget = NULL;
MyIconDropCB	*MyDropSite::iconDropCB = NULL;
MyObjectDropCB	*MyDropSite::objDropCB = NULL;
void		*MyDropSite::dropCBData = NULL;
Atom		MyDropSite::importList[10];
int		MyDropSite::numImportTargets;

#define _ATOM_(NAME) (XmInternAtom(XtDisplay(siteWidget), NAME, False))

/////////////////////////////////////////////////////////////////////////////
//
// Registers a callback to invoke for each dropped file.
//
/////////////////////////////////////////////////////////////////////////////

void
MyDropSite::registerCallback(
    Widget site,
    MyIconDropCB *iconCB,
    MyObjectDropCB *objCB,
    void *data)
{
    siteWidget = site;
    iconDropCB = iconCB;
    objDropCB = objCB;
    dropCBData = data;

    //
    // _SGI_ICON is an icon on the desktop being dragged into our app.
    // We have to figure out what type of file it is.
    // All the other targets are different flavors of Inventor.
    // A _FILE suffix means the dropped data is the name of a file to read.
    // Without _FILE, the actual data is being dropped for us to read from a buffer.
    //
    int n = 0;
    importList[n] = _ATOM_("_SGI_ICON");	    n++;
    importList[n] = _ATOM_("INVENTOR_2_1");	    n++;
    importList[n] = _ATOM_("INVENTOR_2_1_FILE");    n++;
    importList[n] = _ATOM_("VRML_1_0");		    n++;
    importList[n] = _ATOM_("VRML_1_0_FILE");	    n++;
    importList[n] = _ATOM_("INVENTOR");		    n++;
    importList[n] = _ATOM_("INVENTOR_FILE");	    n++;
    importList[n] = _ATOM_("INVENTOR_2_0");	    n++;
    importList[n] = _ATOM_("INVENTOR_2_0_FILE");    n++;
    numImportTargets = n;
    
    Arg args[10];
    n = 0;
    XtSetArg(args[n], XmNimportTargets,		importList);	    n++;
    XtSetArg(args[n], XmNnumImportTargets,	numImportTargets);  n++;
    XtSetArg(args[n], XmNdropSiteOperations,	XmDROP_COPY);	    n++;
    XtSetArg(args[n], XmNdropProc,		handleDrop);	    n++;
    
    XmDropSiteRegister(siteWidget, args, n);
}

/////////////////////////////////////////////////////////////////////////////
//
// This callback is invoked when the drop occurs.
//
/////////////////////////////////////////////////////////////////////////////

void 
MyDropSite::handleDrop(
    Widget,
    XtPointer client_data,
    XtPointer call_data,
    XtPointer)
{
    Arg args[10];
    int n = 0;

    XmDropProcCallback DropData = (XmDropProcCallback) call_data;
    Widget wDragContext = DropData->dragContext;

    // Set the transfer resources
    if ((DropData->dropAction != XmDROP) ||
	(DropData->operations != XmDROP_COPY)) {
	XtSetArg(args[n], XmNtransferStatus, XmTRANSFER_FAILURE); n++;
	XtSetArg(args[n], XmNnumDropTransfers, 0); n++;
	XmDropTransferStart(wDragContext, args, n);
	return;
    }
    else {
	// Check the sender's drag targets
	Atom *senderExportList;
	Cardinal numSenderTargets;
	int m = 0;
	XtSetArg(args[m], XmNexportTargets, &senderExportList); m++;
	XtSetArg(args[m], XmNnumExportTargets, &numSenderTargets); m++;
	XtGetValues(wDragContext, args, m);

	// If no targets, cannot drop. (error)
	if (numSenderTargets == 0) {
	    XtSetArg(args[n], XmNtransferStatus, XmTRANSFER_FAILURE); n++;
	    XtSetArg(args[n], XmNnumDropTransfers, 0); n++;
	    XmDropTransferStart(wDragContext, args, n);
	    return;
	}

	// Look for a target we want to import.
	Atom desiredTarget = chooseTarget(senderExportList, numSenderTargets);

	// If nothing desired, bail.
	if (desiredTarget == 0) {
	    XtSetArg(args[n], XmNtransferStatus, XmTRANSFER_FAILURE); n++;
	    XtSetArg(args[n], XmNnumDropTransfers, 0); n++;
	    XmDropTransferStart(wDragContext, args, n);
	    return;
	}

	// Tell the sender what we want
	XmDropTransferEntryRec transferEntries[2];
	transferEntries[0].target = desiredTarget;
	transferEntries[0].client_data = client_data;
	XmDropTransferEntry transferList = transferEntries;
	
	XtSetArg(args[n], XmNdropTransfers,	transferList);	n++;
	XtSetArg(args[n], XmNnumDropTransfers,	1);		n++;
	
	if (desiredTarget == _ATOM_("_SGI_ICON")) {
	    XtSetArg(args[n], XmNtransferProc,	transferIconProc);	n++;
	}
	else {
	    XtSetArg(args[n], XmNtransferProc,	transferObjectProc);	n++;
	}
    }
    
    XmDropTransferStart(wDragContext, args, n);
}

/////////////////////////////////////////////////////////////////////////////
//
// Choose a target to import from the passed sender's export list.
// Algorithm: see if the first Atom in the sender's export list matches
// any of the Atoms we are interested in importing. If not, repeat with
// the second Atom in the sender's list, and so on.
//
/////////////////////////////////////////////////////////////////////////////

Atom
MyDropSite::chooseTarget(
    Atom *exportList,
    int numExportTargets)
{
    Atom chosenType = 0;

    // Run through our import list and the sender's export list and find a match
    for (int i = 0; i < numExportTargets; i++) {
	for (int j = 0; j < numImportTargets; j++) {
	    if (exportList[i] == importList[j]) {
		chosenType = exportList[i];
		break;
	    }
	}
	
	if (chosenType != 0) break;
    }

    return chosenType;
}

/////////////////////////////////////////////////////////////////////////////
//
// Reads the objects that were dropped.
//
/////////////////////////////////////////////////////////////////////////////

void 
MyDropSite::transferObjectProc(Widget w, XtPointer, Atom *, Atom *type,
		       XtPointer value, unsigned long *length, int)
{
    SbBool success = FALSE;
    
    // If the object is really a file, invoke the icon callback which 
    // takes a filename
    if (*type == _ATOM_("INVENTOR_2_1_FILE") ||
	*type == _ATOM_("VRML_1_0_FILE") ||
	*type == _ATOM_("INVENTOR_FILE") ||
	*type == _ATOM_("INVENTOR_2_0_FILE"))
    {
	// Invoke icon drop callback with a filename
	success = (*iconDropCB)(dropCBData, (const char *) value);
    }
    else {
	// Invoke object drop callback with the object data
	success = (*objDropCB)(dropCBData, *type, value, *length);
    }
    
    // Do the yellow pullback lines if any of the objects failed
    if (! success)
	XtVaSetValues(w, XmNtransferStatus, XmTRANSFER_FAILURE, 0);
}

/////////////////////////////////////////////////////////////////////////////
//
// Reads the files that were dropped.
//
/////////////////////////////////////////////////////////////////////////////

void 
MyDropSite::transferIconProc(Widget w, XtPointer, Atom *, Atom *type,
		       XtPointer value, unsigned long *length, int)
{
    SbStringList fileList;
    parseIcon(type, value, length, &fileList);
    
    SbBool anyFailures = (fileList.getLength() == 0);

    for (int i = 0; i < fileList.getLength(); i++) {

	// Invoke the callback
	if (! (*iconDropCB)(dropCBData, fileList[i]->getString()))
	    anyFailures = TRUE;

	// Free up the allocated strings
	delete fileList[i];
    }

    // Do the yellow pullback lines if any of the icons failed
    if (anyFailures)
	XtVaSetValues(w, XmNtransferStatus, XmTRANSFER_FAILURE, 0);
}

/////////////////////////////////////////////////////////////////////////////
//
// Parses the icon information from the drop action and
// extracts a list of file names to open.
//
/////////////////////////////////////////////////////////////////////////////

void
MyDropSite::parseIcon(Atom *type, XtPointer value, unsigned long *length,
		    SbStringList *fileList)
{
    unsigned long currentLength = 0;
    char *currString = (char *) value;

    while ((currentLength + 1) < *length) {
	if (*type == _ATOM_("_SGI_ICON")) {
	    char category[30];
	    char name[256];
	    sscanf(currString, "Category:%s Name:%s", category, name);

	    if (! strcmp(category, "File"))
		fileList->append(new SbString(name));
	}
	currentLength = currentLength + strlen((char *) currString) + 1;
	currString = (char *) value + currentLength;
    }
}

