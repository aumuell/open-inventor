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
 * Copyright (C) 1990-95   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.4 $
 |
 |   Classes	: SoXtClipboard
 |
 |   Author(s)	: David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include <Inventor/SbDict.h>
#include <Inventor/misc/SoByteStream.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoPath.h>
#include <Inventor/Xt/SoXtClipboard.h>

#include <Inventor/errors/SoDebugError.h>
#include "SoXtImportInterest.h"
#include <Xm/AtomMgr.h>
#include <X11/StringDefs.h>

// This list will contain SoXtClipboard pointers.
// The key into this dictionary is an X selection (e.g. XA_PRIMARY),
// and the return value is which SoXtClipboard owns that selection, if any.
// We need this because our import/export functions are static, and 
// thus are not tied to any one SoXtClipboard class instance.
SbDict *SoXtClipboard::selOwnerList = NULL;

#define _ATOM_(widget,name) (XmInternAtom(XtDisplay(widget),name,False))

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Constructor
//
SoXtClipboard::SoXtClipboard(
    Widget  	w,  	    	// widget for copy/paste data transfers
    Atom    	selectionAtom)  // the x selection: XA_SECONDARY, etc.
//
////////////////////////////////////////////////////////////////////////
{
    if (selOwnerList == NULL)
    	selOwnerList = new SbDict;

    // _XA_CLIPBOARD_ lets us know to use the X internal atom "CLIPBOARD".
    if (selectionAtom == _XA_CLIPBOARD_) {
	selectionAtom = XmInternAtom(XtDisplay(w), "CLIPBOARD", False);
    }

    widget  	    = w;
    clipboardAtom   = selectionAtom;
    eventTime	    = CurrentTime;
    
    // copyBuffer is used to transfer copy/paste data in binary format.
    copyBuffer      = NULL;
    
    // Default paste interest.
    pasteInterest = new SoXtImportInterestList;
    pasteInterest->append(_ATOM_(widget, "INVENTOR_2_1"),	pasteImportCB, this);
    pasteInterest->append(_ATOM_(widget, "INVENTOR_2_1_FILE"),	pasteImportCB, this);
    pasteInterest->append(_ATOM_(widget, "VRML_1_0"),		pasteImportCB, this);
    pasteInterest->append(_ATOM_(widget, "VRML_1_0_FILE"),	pasteImportCB, this);
    pasteInterest->append(_ATOM_(widget, "INVENTOR"),		pasteImportCB, this);
    pasteInterest->append(_ATOM_(widget, "INVENTOR_FILE"),	pasteImportCB, this);
    pasteInterest->append(_ATOM_(widget, "INVENTOR_2_0"),	pasteImportCB, this);
    pasteInterest->append(_ATOM_(widget, "INVENTOR_2_0_FILE"),	pasteImportCB, this);
    pasteInterest->append(XA_STRING,				pasteImportCB, this);
    
    // Default copy interest
    copyInterest = new SbPList();
    copyInterest->append((void *)_ATOM_(widget, "INVENTOR_2_1"));
    copyInterest->append((void *)_ATOM_(widget, "INVENTOR_2_1_FILE"));
    copyInterest->append((void *)_ATOM_(widget, "VRML_1_0"));
    copyInterest->append((void *)_ATOM_(widget, "VRML_1_0_FILE"));
    copyInterest->append((void *)_ATOM_(widget, "INVENTOR"));
    copyInterest->append((void *)_ATOM_(widget, "INVENTOR_FILE"));
    copyInterest->append((void *)_ATOM_(widget, "INVENTOR_2_0"));
    copyInterest->append((void *)_ATOM_(widget, "INVENTOR_2_0_FILE"));
    copyInterest->append((void *) XA_STRING);

    // By default, don't pass empty path lists to callbacks
    emptyListOK = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Destructor - if we own the X selection, lose it.
//
SoXtClipboard::~SoXtClipboard()
//
////////////////////////////////////////////////////////////////////////
{
    // see if we own the selection
    void *data = NULL;
    selOwnerList->find(clipboardAtom, data);
    
    if ((SoXtClipboard *)data == this) {
	// we own the selection. we should support persistence (i.e. the
	// data should be around for someone to paste even after we are
	// destroyed), but we don't, so lose the selection.
	XtDisownSelection(widget, clipboardAtom, CurrentTime);
	selOwnerList->remove(clipboardAtom);
    }
    
    delete pasteInterest;
    delete copyBuffer;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Copy the byte stream into our copyBuffer, and tell the X server
// that we now own the X selection identified by clipboardAtom (clipboardAtom was
// passed during construction).
//
// Use: protected
//
void
SoXtClipboard::copy(SoByteStream *byteStream, Time t)
//
////////////////////////////////////////////////////////////////////////
{
    eventTime = t;

    delete copyBuffer;
    copyBuffer = byteStream;
    copyDataType = _ATOM_(widget, "INVENTOR_2_1");
    
    // SoWWWInline won't write it's data out unless the alternateRep field is set.
    // Run through each node in each path, and do this so that the resulting file
    // has all the data it can have.
    //???
    
    // We copy Inventor 2.1 data.
    // We will convert to other formats on request.   
    if (copyBuffer != NULL) {
	// tell the X server that we own the selection
	XtOwnSelection(widget, 
		clipboardAtom,
		eventTime,
		(XtConvertSelectionProc) SoXtClipboard::exportSelection,
		(XtLoseSelectionProc) SoXtClipboard::loseSelection,
		(XtSelectionDoneProc) NULL);
		
	// make sure we own the selection
	if (XGetSelectionOwner(XtDisplay(widget), clipboardAtom) == XtWindow(widget)) {
	    // we were granted ownership - associate 'this' with the
	    // selection atom in our dictionary of X selection owners
	    selOwnerList->enter(clipboardAtom, this);
	}		
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Copy the users data into our copyBuffer, and tell the X server
// that we now own the X selection identified by clipboardAtom (clipboardAtom was
// passed during construction). The data is not necessarily Inventor.
//
// Use: protected
//
void
SoXtClipboard::copy(Atom dataType, void *data, uint32_t numBytes, Time t)
//
////////////////////////////////////////////////////////////////////////
{
    eventTime = t;

    delete copyBuffer;
    copyBuffer = new SoByteStream();
    copyBuffer->copy(data, numBytes);
    copyDataType = dataType;

    // This is unknown data to us...
    if (data != NULL && numBytes > 0) {
	// tell the X server that we own the selection
	XtOwnSelection(widget, 
		clipboardAtom,
		eventTime,
		(XtConvertSelectionProc) SoXtClipboard::exportSelection,
		(XtLoseSelectionProc) SoXtClipboard::loseSelection,
		(XtSelectionDoneProc) NULL);
		
	// make sure we own the selection
	if (XGetSelectionOwner(XtDisplay(widget), clipboardAtom) == XtWindow(widget)) {
	    // we were granted ownership - associate 'this' with the
	    // selection atom in our dictionary of X selection owners
	    selOwnerList->enter(clipboardAtom, this);
	}		
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Copy the path list onto this clipboard.
//
// Use: public
//
void
SoXtClipboard::copy(SoPathList *pathList, Time t)
//
////////////////////////////////////////////////////////////////////////
{
    SoByteStream *bs = new SoByteStream;
    bs->convert(pathList);
    copy(bs, t);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Copy the path onto this clipboard.
//
// Use: public
//
void
SoXtClipboard::copy(SoPath *path, Time t)
//
////////////////////////////////////////////////////////////////////////
{
    SoByteStream *bs = new SoByteStream;
    bs->convert(path);
    copy(bs, t);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Copy the node onto this clipboard.
//
// Use: public
//
void
SoXtClipboard::copy(SoNode *node, Time t)
//
////////////////////////////////////////////////////////////////////////
{
    SoByteStream *bs = new SoByteStream;
    bs->convert(node);
    copy(bs, t);
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Paste whatever is in the X selection identified by clipboardAtom.
// We do this by requesting the selection data from the X server,
// then return to the main event loop. We will be notified via an event
// when the data is ready.
//
// Use: public
//
void
SoXtClipboard::paste(
    Time t,
    SoXtClipboardPasteCB *pasteDoneFunc,
    void *pasteUserData)
//
////////////////////////////////////////////////////////////////////////
{
    // Save values for later
    eventTime    = t;
    callbackFunc = pasteDoneFunc;
    userData     = pasteUserData;

    // if we own the selection, we already have the data,
    // so we can bypass the server - simply paste what is in our cutbuffer
    SoXtClipboard *clipboard;
    void *data;
    selOwnerList->find(clipboardAtom, data);
    clipboard = (SoXtClipboard *)data;

    if (clipboard == this) {
    	// We own the selection! Is it INventor data?
	if (copyBuffer->isRawData()) {
	    //??? see if the app is interested in this data...
	}
	else {
	    SoPathList *pathList;
	    pathList = SoByteStream::unconvert(copyBuffer);
	    
	    // let the app know the paste is done
	    if (callbackFunc != NULL)
		(*callbackFunc)(userData, pathList);
	}
    }
    else {
    	// Find out what data targets the selection owner supports.
    	// (Of course, we have the server ask the selection owner for us.)
	XtGetSelectionValue(widget,
		clipboardAtom,
		_ATOM_(widget, "TARGETS"),
		(XtSelectionCallbackProc) SoXtClipboard::importSelectionTargets,
		(XtPointer) this,
		eventTime);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Get the TARGETS we support into a table.
//
void
SoXtClipboard::getExportTargets(
    char		**value,
    uint32_t		*length)
//
////////////////////////////////////////////////////////////////////////
{	
    // Return the selection targets which we support
    // If the user supplied the copy data type, we serve that up.
    // Otherwise, we publicize all the Inventor types we support.
    if (copyBuffer->isRawData()) {
	// User data!
	// (malloc the table since the requestor will own the data)
	Atom *table = (Atom *) malloc(sizeof(Atom));
	table[0] = copyDataType;
	
	*value = (char *) table;
	*length = 1;  // number of items in our TARGETS table
    }
    
    else {
	// Inventor data!
	// (malloc the table since the requestor will own the data)
	Atom *table = (Atom *) malloc(copyInterest->getLength() * sizeof(Atom));
	for (int i = 0; i < copyInterest->getLength(); i++)
	    table[i] = (Atom)(*copyInterest)[i];

	*value  = (char *) table;
	*length = copyInterest->getLength();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Export the copied data to the X server so another application may
// paste. This callback is registered by copy().
//
// NOTE: we do not own the data we return (i.e. *value), so we have to
// copy our data buffer here.
//
// Use: static, private
//
Boolean
SoXtClipboard::exportSelection(
    Widget		widget,
    Atom		*selectionAtom,
    Atom		*target,
    Atom		*returnTarget,
    char		**value,
    uint32_t		*length,
    int			*format)
//
////////////////////////////////////////////////////////////////////////
{
    Boolean	  	ok = False;

#ifdef DEBUG
    if (selectionAtom == NULL) {
	SoDebugError::post("SoXtClipboard::exportSelection",
			   "- selectionAtom is NULL");
	return FALSE;
    }
    if (target == NULL) {
	SoDebugError::post("SoXtClipboard::exportSelection",
			   "- target is NULL");
	return FALSE;
    }
    if (value == NULL) {
	SoDebugError::post("SoXtClipboard::exportSelection",
			   "- value is NULL");
	return FALSE;
    }
    if (length == NULL) {
	SoDebugError::post("SoXtClipboard::exportSelection",
			   "- length is NULL");
	return FALSE;
    }
    if (format == NULL) {
	SoDebugError::post("SoXtClipboard::exportSelection",
			   "- format is NULL");
	return FALSE;
    }
    if (returnTarget == NULL) {
	SoDebugError::post("SoXtClipboard::exportSelection",
			   "- returnTarget is NULL");
	return FALSE;
    }
#endif

    // get the clipboard pointer for this object
    SoXtClipboard *clipboard;
    void *data;
    if (! selOwnerList->find(*selectionAtom, data)) {
#ifdef DEBUG
	SoDebugError::post("SoXtClipboard::exportSelection",
			   "- cannot find selection owner");
#endif
	return FALSE;
    }
    clipboard = (SoXtClipboard *)data;
    
    
    // Do we have any data?
    if (clipboard->copyBuffer == NULL)
	return False;
    if (clipboard->copyBuffer->getData() == NULL || clipboard->copyBuffer->getNumBytes() <= 0)
	return False;
    
    // The most likely case - the pasting app wants to know what data types we have.
    if (*target == _ATOM_(widget, "TARGETS")) {
	clipboard->getExportTargets(value, length);
	*format = 32;
	*returnTarget = *target;
	ok = True;
    }
    
    // The xclipboard case - pasting app just wants text. Give them the
    // ascii scene graph!
    else if (*target == XA_STRING) {
	// convert the copyBuffer byte stream into a path list,
	// then convert the path list to a byte stream, but in ascii format.
	SoPathList *pathList;
	pathList = SoByteStream::unconvert(clipboard->copyBuffer);
	
	if (pathList != NULL) {	
	    SoByteStream *asciiBuffer = new SoByteStream;
	    asciiBuffer->convert(pathList, FALSE); // FALSE == ascii format
	    
	    // Copy the data buffer since the requestor will own the data
	    uint32_t numBytes = asciiBuffer->getNumBytes();
	    if (numBytes > 0) {
		char *exportData = (char *) malloc((size_t) numBytes);
		
		if (exportData != NULL) {
		    memcpy(exportData, asciiBuffer->getData(), (int) numBytes);
		    
		    *value  = exportData; 
		    *length = numBytes;
		    *format = 8;
		    *returnTarget = XA_STRING;
		    ok = True;
		}
	    }
	    delete asciiBuffer;
	    delete pathList;
	}
    }
    
    // If our copyBuffer is a user defined type, then we do no conversion.
    // Just serve it up.
    else if (clipboard->copyBuffer->isRawData()) {
	// Do we have the data that the pasting app wants?
	if (clipboard->copyDataType == *target) {
	    // Copy the data buffer since the requestor will own the data
	    char *exportData = (char *) malloc((size_t) clipboard->copyBuffer->getNumBytes());
	    if (exportData != NULL) {
		memcpy(exportData,
		       clipboard->copyBuffer->getData(),
		       (int)clipboard->copyBuffer->getNumBytes());
		*value  = exportData; 
		*length = clipboard->copyBuffer->getNumBytes();
		*format = 8;
		*returnTarget = clipboard->copyDataType;
		ok = True;
	    }	
	}
    }
    
    else {
	// If the data is one of the Inventor types, we need to convert to
	// the requested type.
	if (convertData(
		widget, 
		clipboard->copyBuffer->getData(),
		clipboard->copyBuffer->getNumBytes(),
		*target, value, length))
	{
	    *format = 8;
	    *returnTarget = *target;
	    ok = True;
	}
    }
    
    return ok;
}

////////////////////////////////////////////////////////////////////////
//
//  Convert our Inventor 2.1 data to the specified target.
//
//  static,public
//
SbBool
SoXtClipboard::convertData(
    Widget	widget, 
    void	*srcData, 
    uint32_t    srcNumBytes, 
    Atom	desiredType,
    char	**returnData,
    uint32_t    *returnNumBytes)
//
////////////////////////////////////////////////////////////////////////
{
    // Do we know how to convert to the desired target?
    if (!  (desiredType == _ATOM_(widget, "INVENTOR_2_1") ||
	    desiredType == _ATOM_(widget, "INVENTOR_2_1_FILE") ||
	    desiredType == _ATOM_(widget, "VRML_1_0") ||
	    desiredType == _ATOM_(widget, "VRML_1_0_FILE") ||
	    desiredType == _ATOM_(widget, "INVENTOR") ||
	    desiredType == _ATOM_(widget, "INVENTOR_FILE") ||
	    desiredType == _ATOM_(widget, "INVENTOR_2_0") ||
	    desiredType == _ATOM_(widget, "INVENTOR_2_0_FILE"))) {
	// It's some type we don't know
	*returnData = NULL;
	*returnNumBytes = 0;
	
	#ifdef DEBUG
	SoDebugError::post("SoXtClipboard::convertData",
			   "desired conversion type not supported");
	#endif
	return FALSE;
    }
   
    // OK, we think we can support the destination target.
    char *exportData = NULL;
    uint32_t numBytes = 0;
    
    // Run through our supported targets and see if we can convert.
    //??? It would be nice if we put conversion callbacks in the copyInterest list,
    //??? then we could just run through the list and blindly convert.
    
    //??? should we cache our results?
    
    ////////////////////////////////////////////////////////////////////////
    //
    // Inventor 2.1 - no conversion needed
    // (For VRML 1.0 destination - we should probably convert ivToVRML???)
    //
    ////////////////////////////////////////////////////////////////////////
    if (desiredType == _ATOM_(widget, "INVENTOR_2_1") ||
	desiredType == _ATOM_(widget, "VRML_1_0")) {
	exportData = (char *) malloc((size_t) srcNumBytes);
	if (exportData != NULL) {
	    memcpy(exportData, srcData, (int) srcNumBytes);
	    *returnData = exportData; 
	    *returnNumBytes = srcNumBytes;
	    return True;
	}
	else {
	    #ifdef DEBUG
	    SoDebugError::post("SoXtClipboard::convertData",
			       "could not allocate %d bytes for return buffer", srcNumBytes);
	    #endif
	    return False;
	}
    }

    // Put our 2.1 data into a file
    // Write the bytes to a tmp file which will be the input file to ivdowngrade
    static char *tmpdir = NULL;
    if (tmpdir == NULL) {
	tmpdir = getenv("TMPDIR");
	if (tmpdir == NULL)
	    tmpdir = "/tmp";
    }

    // Create temp filename
    char *filename = tempnam(tmpdir, "IV21");
    SbString tmpfile(filename);
    free(filename);
    
    // Write 2.1 data to the file
    if (! writeToFile(tmpfile, srcData, srcNumBytes)) 
	return False;
    
    ////////////////////////////////////////////////////////////////////////
    //
    // If the destination target is a 2.1 file, we're done
    // (For VRML 1.0 destination - we should probably convert ivToVRML???)
    //
    ////////////////////////////////////////////////////////////////////////
    if (desiredType == _ATOM_(widget, "INVENTOR_2_1_FILE") ||
	desiredType == _ATOM_(widget, "VRML_1_0_FILE")) {
	exportData = strdup(tmpfile.getString());
	numBytes = tmpfile.getLength() + 1; // +1 to grab the \0
	*returnData = exportData; 
	*returnNumBytes = numBytes;
	return True;
    }
    
    // Make sure ivdowngrade exists
    //???
    
    // Create temp output filename
    filename = tempnam(tmpdir, "IV20");
    SbString tmpfile2(filename);
    free(filename);
    
    // Convert to the 2.0 format
#define BUFSIZE 512
    char conversionCmd[BUFSIZE];
    const char *infile = tmpfile.getString();
    const char *outfile = tmpfile2.getString();
    sprintf(conversionCmd, IVPREFIX "/bin/ivdowngrade -v 2.0 %s %s", infile, outfile);
    
    if (system(conversionCmd) == -1) {
	#ifdef DEBUG
	SoDebugError::post("SoXtClipboard::convertData",
			   "command <%s> failed", conversionCmd);
	#endif
	
	unlink(infile);
	unlink(outfile);
	return False;
    }
    
    // Converson succeeded! Done with the input file
    //??? NOTE: we could cache this for future conversions...
    unlink(infile); // remove the file

    ////////////////////////////////////////////////////////////////////////
    //
    // If the destination target is a 2.0 file, we're done
    //
    ////////////////////////////////////////////////////////////////////////
    if (desiredType == _ATOM_(widget, "INVENTOR_FILE") ||
        desiredType == _ATOM_(widget, "INVENTOR_2_0_FILE") ) {
	exportData = strdup(tmpfile2.getString());
	numBytes = tmpfile2.getLength() + 1; // +1 to grab the \0
	*returnData = exportData; 
	*returnNumBytes = numBytes;
	return True; // success!
    }
    
    // Read the output file into the buffer
    // First, open the file.
    int fd = open(outfile, O_RDONLY);
    if (fd < 0) {
	#ifdef DEBUG
	SoDebugError::post("SoXtClipboard::convertData",
			   "could not open output file %s for reading", outfile);
	#endif
	
	unlink(outfile); // remove the file
	return False;
    }
    
    // Get file size
    uint32_t len = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET); // seek back to beginning
    
    // Get buffer to hold data
    void *buf = malloc(len);
    if (buf == NULL) {
	#ifdef DEBUG
	SoDebugError::post("SoXtClipboard::convertData",
			   "could not malloc %d bytes", len);
	#endif
	
	unlink(outfile); // remove the file
	return False;
    }
    
    // Read that file
    if (read(fd, buf, len) == -1) {
	#ifdef DEBUG
	SoDebugError::post("SoXtClipboard::convertData",
			   "could not read %d bytes from output file %s", len, outfile);
	#endif
	
	unlink(outfile); // remove the file
	return False;
    }
    
    // Done with the output file
    unlink(outfile); // remove the file

    ////////////////////////////////////////////////////////////////////////
    //
    // If the destination target is a 2.0 object, we are done!
    //
    ////////////////////////////////////////////////////////////////////////
    if (desiredType == _ATOM_(widget, "INVENTOR") ||
	desiredType == _ATOM_(widget, "INVENTOR_2_0") ) {
	*returnData = (char *)buf; 
	*returnNumBytes = len;
	return True; // success!
    }
    
    // If we get to here, we did not succeed. Bummer.
    return False;
}

////////////////////////////////////////////////////////////////////////
//
//  Write the copyBuffer to the tmp file
//
//  static protected
//
SbBool
SoXtClipboard::writeToFile(
    SbString	&tmpfile, 
    void	*srcData, 
    uint32_t    srcNumBytes)
//
////////////////////////////////////////////////////////////////////////
{
    const char *filename = tmpfile.getString();
    int fd = creat(filename, S_IRWXU); // create for reading/writing
    if (fd <= 0) {
	#ifdef DEBUG
	SoDebugError::post("SoXtClipboard::writeToFile",
			   "could not open tmp input file %s", filename);
	#endif
	
	return False;
    }

    // Write to temp file
    if (write(fd, srcData, srcNumBytes) == -1) {
	#ifdef DEBUG
	SoDebugError::post("SoXtClipboard::writeToFile",
			   "could not write %d bytes to tmp input file %s",
			   srcNumBytes, filename);
	#endif
	
	close(fd);
	unlink(filename); // remove the file
	return False;
    }
    
    close(fd);
    return True;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Import the targets supported by the selection owner. If we see
// a target we recognize, then we ask for the selection data in that
// format target. This callback is registered by pasteSelection().
//
// Use: static, private
//
void
SoXtClipboard::importSelectionTargets(
    Widget		widget,
    SoXtClipboard	*clipboard,
    Atom		*selectionAtom,
    Atom		*target,
    char		*value,
    uint32_t		*length,
    int			*format)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (clipboard == NULL) {
	SoDebugError::post("SoXtClipboard::importSelectionTargets",
			   "- clipboard is NULL");
	return;
    }
    if (target == NULL) {
	SoDebugError::post("SoXtClipboard::importSelectionTargets",
			   "- target is NULL");
	return;
    }
    if (! clipboard->emptyListOK) {
	if (value == NULL) {
	    SoDebugError::post("SoXtClipboard::importSelectionTargets",
			       "- value is NULL");
	    return;
	}
	if (length == NULL) {
	    SoDebugError::post("SoXtClipboard::importSelectionTargets",
			       "- length is NULL");
	    return;
	}
	if (format == NULL) {
	    SoDebugError::post("SoXtClipboard::importSelectionTargets",
			       "- format is NULL");
	    return;
	}
    }
#endif

    // If no targets are supported
    if (*length == 0) {

	// Invoke the callback if an empty list is ok
	if (clipboard->emptyListOK) {
	    SoPathList emptyList;
	    (*clipboard->callbackFunc)(clipboard->userData, &emptyList);
	}
	return;
    }

    Atom supportedAtom = 0;
    if (*target == _ATOM_(widget, "TARGETS") || *target == XA_ATOM) {
	if (*format == 32) {
	    Atom *lval = (Atom *) value;
	    supportedAtom = clipboard->chooseFromImportTargets(lval, *length);
	}
#ifdef DEBUG
	else {
	    SoDebugError::post("SoXtClipboard::importSelectionTargets",
			       "- received unknown format %d from clipboard owner", *format );
	}
#endif
    }
    else {
	// The selection owner does not support the TARGETS inquiry.
	// Let's see if they at least support INVENTOR.
	//??? what should we do here?
	supportedAtom = _ATOM_(widget, "INVENTOR");
    }
    
    // if the selection owner supports a target we like, ask for the data
    if (supportedAtom != 0) {
	XtGetSelectionValue(clipboard->widget,
		*selectionAtom,
		supportedAtom,
		(XtSelectionCallbackProc) SoXtClipboard::importSelection,
		(XtPointer) clipboard,
		clipboard->eventTime);
    }
    
    // According to Xt doc, the requestor (that's us) owns the data
    // and should free it when done.

//??? Put in this change as a workaround to bug #306340
//??? to prevent core dumps.  It seems that this sometimes frees
//??? memory here that it should not.

//???    XtFree(value);
}

////////////////////////////////////////////////////////////////////////
//
//  See if any of the copy apps supported targets are something we are
// interested in pasting. Algorithm is: see if the first copy target
// matches any of our paste targets. If not, check the second copy
// target, etc.
//
Atom
SoXtClipboard::chooseFromImportTargets(
    Atom *supportedTargets,
    int length)
//
////////////////////////////////////////////////////////////////////////
{    
    Atom chosenType = 0;
    int i;
    
    // See if anything in the supported targets matches our paste interest
    for (i = 0; i < length; i++) {
	for (int j = 0; j < pasteInterest->getLength(); j++) {
	    if (supportedTargets[i] == pasteInterest->getDataType(j)) {
		chosenType = supportedTargets[i];
		break;
	    }
	}
	
	if (chosenType != 0) break;
    }
    
    return chosenType;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Import the current selection from the X server, and paste it
// into the scene graph. This callback is registered by importSelectionTargets().
//
// Use: static, private
//
void
SoXtClipboard::importSelection(
    Widget		/*w*/,
    SoXtClipboard	*clipboard,
    Atom		* /*selectionAtom*/,
    Atom		*target,
    char		*value,
    uint32_t		*length,
    int			* /*format*/)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (clipboard == NULL) {
	SoDebugError::post("SoXtClipboard::importSelection",
			   "- clipboard is NULL");
	return;
    }
    if (target == NULL) {
	SoDebugError::post("SoXtClipboard::importSelection",
			   "- target is NULL");
	return;
    }
    if (! clipboard->emptyListOK) {
	if (value == NULL) {
	    SoDebugError::post("SoXtClipboard::importSelection",
			       "- value is NULL");
	    return;
	}
	if (length == NULL) {
	    SoDebugError::post("SoXtClipboard::importSelection",
			       "- length is NULL");
	    return;
	}
    }
#endif

    if (*length == 0 && ! clipboard->emptyListOK)
	return;					// nothing to paste!
    
    if (clipboard->callbackFunc == NULL) return; // no callback to invoke!

    // See if we are interested in the paste target
    const SoXtImportInterest *interest = 
	clipboard->pasteInterest->get(clipboard->pasteInterest->find(*target));

    // If there is interest, invoke the paste interest callback to read the data
    if (interest != NULL) {
	(*interest->getCallBack())(
	    interest->getUserData(),
	    *target,
	    (void *) value,
	    *length);
    }
    // If there is no interest, but an empty list is ok, invoke the
    // callback with an empty list
    else if (clipboard->emptyListOK) {
	// Invoke the callback if an empty list is ok
	SoPathList emptyList;
	(*clipboard->callbackFunc)(clipboard->userData, &emptyList);
    }
    	
    // According to Xt doc, the requestor (that's us) owns the data
    // and should free it when done.
//??? Put in this change as a workaround to bug #306340
//??? to prevent core dumps.  It seems that this sometimes frees
//??? memory here that it should not.

//???    XtFree(value);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This is called when the user drops data on this widget (drag and drop).
//
// Use: protected
//
void
SoXtClipboard::addPasteInterest(
    Atom dataType,
    SoXtClipboardImportCB *pasteImportFunc,
    void *userData)
//
////////////////////////////////////////////////////////////////////////
{
    pasteInterest->append(dataType, pasteImportFunc, userData);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This is called when the user pastes data in this widget.
//
// Use: protected
//
void
SoXtClipboard::pasteImport(Atom dataType, void *data, uint32_t numBytes)
//
////////////////////////////////////////////////////////////////////////
{
    // Did we get any data?
    SoPathList *pathList = readData(widget, dataType, data, numBytes);

    // Did we get Inventor data?
    if (pathList == NULL) {

	if (emptyListOK) {
	    SoPathList emptyList;
	    (*callbackFunc)(userData, &emptyList);
	}

#ifdef DEBUG
	else
	    SoDebugError::post("SoXtClipboard::paste",
			       "could not get Inventor data");
#endif
    }
    
    // send the data to the app
    else (*callbackFunc)(userData, pathList);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Convert target data we just imported via paste or drop to Inventor.
// This will free data when done.
//
// Use: static
//
SoPathList *
SoXtClipboard::readData(
    Widget w,
    Atom target,
    void *data,
    uint32_t numBytes)
//
////////////////////////////////////////////////////////////////////////
{
    SoPathList *pathList = NULL;
    	     
    if (data != NULL) {
	if (target == _ATOM_(w, "INVENTOR_2_1") ||
	    target == _ATOM_(w, "VRML_1_0") ||
	    target == _ATOM_(w, "INVENTOR") ||
	    target == _ATOM_(w, "INVENTOR_2_0")) {
	    
	    pathList = SoByteStream::unconvert(data, numBytes);
	}
	
	else if (target == _ATOM_(w, "INVENTOR_2_1_FILE") ||
		 target == _ATOM_(w, "VRML_1_0_FILE") ||
		 target == _ATOM_(w, "INVENTOR_FILE") ||
		 target == _ATOM_(w, "INVENTOR_2_0_FILE")) {
	    
	    readFile(pathList, (const char *) data);  
	} 
	
	else if (target == XA_STRING) {
	    // Try to read Inventor data out of this
	    // if the string is in Inventor ascii interchange format
	    if (SoDB::isValidHeader((const char *)data)) {
		// yes, the string we are importing is actually Inventor data!
		pathList = SoByteStream::unconvert(data, numBytes);
	    }
	}
	
	XtFree((char *)data);	
    }
    
    return pathList;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Read the file and add to the pathList.
//
// Use: static protected
//
void
SoXtClipboard::readFile(SoPathList *&pathList, const char *filename)
//
////////////////////////////////////////////////////////////////////////
{
//??? file could be DXF or some other format. May need to run a converter.
//??? do the routeprint stuff to figure out what to do here!

    SoInput in;
    SoNode *node;
    SoPath *path;

    // Open the file.
    if (! in.openFile(filename)) {
	#ifdef DEBUG
	SoDebugError::post("SoXtClipboard::paste", "could not open file %s", filename);
	#endif
	
	return;
    }

    // Make sure there's a pathList	
    if (pathList == NULL)
	pathList = new SoPathList;

    // Try reading nodes.	
    do {
	if (SoDB::read(&in, node)) {
	    if (node != NULL) {
		// Read a node! Add to the pathList
		path = new SoPath(node);
		pathList->append(path);
	    }
	}
    } while (node != NULL);
    
    in.closeFile();
    
    // If we succeeded in reading stuff, return.
    if (pathList->getLength() > 0)
	return;

    // Open the file again.
    if (! in.openFile(filename))
	return;

    // If couldn't read nodes, try reading paths.
    do {
	if (SoDB::read(&in, path)) {
	    if (path != NULL) {
		// Read a path! Add to the pathList
		pathList->append(path);
	    }
	}
    } while (path != NULL);
    
    in.closeFile();
    
    // If we never added to the pathList, get rid of it
    if (pathList->getLength() == 0) {
	delete pathList;
	pathList = NULL;
	
	#ifdef DEBUG
	SoDebugError::post("SoXtClipboard::paste", "could not read Inventor data from file %s", filename);
	#endif
    }
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Called by the X server when we are no longer the owner of the
// selection. We take this opportunity to nuke our local copyBuffer.
// This callback is registered by copySelection().
//
// Use: static, private
//
void
SoXtClipboard::loseSelection(
    Widget  /*w*/,
    Atom    *selectionAtom)
//
////////////////////////////////////////////////////////////////////////
{
    // get the clipboard pointer for this object
    SoXtClipboard *clipboard;
    void *data;
    selOwnerList->find(*selectionAtom, data);
    clipboard = (SoXtClipboard *)data;

    // get the clipboard pointer for this object
    if (clipboard != NULL) {
    	if (clipboard->copyBuffer != NULL) {
	    delete clipboard->copyBuffer;
	    clipboard->copyBuffer = NULL;
	}
	
	selOwnerList->remove(*selectionAtom);
    }
}

