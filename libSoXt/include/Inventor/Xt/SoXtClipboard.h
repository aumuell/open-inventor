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
 * Copyright (C) 1990-95   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Description:
 |  	This class handles data transfers for copy and paste operations
 |   between X clients.
 |
 |   Author(s)	: David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_XT_CLIPBOARD_
#define  _SO_XT_CLIPBOARD_

#include <inttypes.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <Inventor/SbBasic.h>

class SbDict;
class SoByteStream;
class SoNode;
class SoPath;
class SoPathList;
class SoSelection;
class SoXtImportInterestList;
class SbString;
class SbPList;


// callback function prototypes
typedef void SoXtClipboardPasteCB(void *userData, SoPathList *pathList);
typedef void SoXtClipboardImportCB(
		    void *userData,
		    Atom dataType,
		    void *data,
		    uint32_t numBytes);


// The CLIPBOARD selection atom is not a predefined atom in X11 R4.
// However, it is widely recognized. We define it to 0 here for
// convenience. Internally, when SoXtClipboard sees _XA_CLIPBOARD_,
// it will use XInternAtom(d, "CLIPBOARD", False).

#define _XA_CLIPBOARD_ ((Atom) 0)

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoXtClipboard
//
//	
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoClpBrd
class SoXtClipboard {
  public:
    // Constructor.
    // The selection atom determines which X selection atom data transfers
    // should happen through. Default is _XA_CLIPBOARD_. (wsh uses XA_PRIMARY).
    
    SoXtClipboard(Widget w, Atom selectionAtom = _XA_CLIPBOARD_);
    
    // Destructor.
    ~SoXtClipboard();

    //
    // These methods transfer inventor scene graphs as the data.
    //

    // Copy - these routines copy the passed data into a byte stream,
    // and make the data available to any X client which requests it.
    // The eventTime should be the time stamp from the event which
    // triggered the copy request.
    //
    // The standard Inventor targets will be exported:
    //	    INVENTOR_2_1
    //	    INVENTOR_2_1_FILE
    //	    VRML_1_0
    //	    VRML_1_0_FILE
    //	    INVENTOR
    //	    INVENTOR_FILE
    //	    INVENTOR_2_0
    //	    INVENTOR_2_0_FILE
    
    // This copies the passed node.
    // C-api: name=copyNode
    void    copy(SoNode *node, Time eventTime);
    
    // This copies path.
    // C-api: name=copyPath
    void    copy(SoPath *path, Time eventTime);

    // This copies all the paths in pathList.
    // C-api: name=copyPathList
    void    copy(SoPathList *pathList, Time eventTime);
    
    // This copies non-Inventor data to the clipboard
    // C-api: name=copyData
    void    copy(Atom dataType, void *data, uint32_t numBytes, Time eventTime);

    // Paste - make a request to the X server so we can import data for paste.
    // A paste is asynchronous - when this routine is called, it simply
    // makes a request to the X server for data to paste, then returns.
    // Once the data is delivered, the pasteDoneFunc will be called and passed
    // the user data along with a list of paths that were pasted. The app
    // should delete this path list when it is done with it.
    // The eventTime should be the time stamp from the event which
    // triggered the paste request.
    //
    // The standard Inventor targets will be recognized:
    //	    INVENTOR_2_1
    //	    INVENTOR_2_1_FILE
    //	    VRML_1_0
    //	    VRML_1_0_FILE
    //	    INVENTOR
    //	    INVENTOR_FILE
    //	    INVENTOR_2_0
    //	    INVENTOR_2_0_FILE
    //	    XA_STRING - if the string describes ascii Inventor data
    
    void    paste(Time eventTime,
		  SoXtClipboardPasteCB *pasteDoneFunc,
		  void *userData = NULL);
    
    // This extends the paste interest to recognize an additional target as
    // specified by 'dataType'. The pasteImportFunc will be called when this
    // type of data is pasted. This may be called multiple times to register
    // interest in more than one extension.
    //
    // Passing NULL as the pasteImportFunc will remove paste interest for that
    // data type (even if the data type is one of the defaults).
    //
    // This should be called before paste() so that paste() will look for data
    // types that have been specified here.
    //
    // The dataType atom can be created with XmInternAtom, e.g.:
    //     XmInternAtom(XtDisplay(widget),"INVENTOR",False);
    //
    // The order is important - first things added to the list are the first
    // things searched for at paste time. (The default interest types come first.)
    
    void	    addPasteInterest(
			Atom dataType,
			SoXtClipboardImportCB *pasteImportFunc,
			void *userData = NULL);

  SoEXTENDER public:
  		
    // Convert the passed Inventor 2.1 data to a different format, specified by	
    // the Atom desiredType. The data will be malloc'd here and placed in returnData,
    // with returnNumBytes describing the data length. This returns TRUE if successful.
    static SbBool  convertData(
			Widget	    widget, 
			void	    *srcData, 
			uint32_t    srcNumBytes, 
			Atom	    desiredType,
			char	    **returnData,
			uint32_t    *returnNumBytes);
    
  SoINTERNAL public:
    // Sets a flag indicating whether the paste callback can and
    // should be passed an empty path list after a paste request is
    // made. For compatibility, this flag is FALSE by default, meaning
    // that the callback will never be invoked with an empty path list.
    void	setEmptyListOK(SbBool flag)	{ emptyListOK = flag; }
    SbBool	isEmptyListOK() const		{ return emptyListOK; }
    
  protected:
    Widget  	widget;	    	// the widget to associated data with
    Atom    	clipboardAtom;	// which selection: XA_PRIMARY, XA_SECONDARY, etc.
    Time    	eventTime;  	// time of the event which caused the copy/paste
    
    // Paste callback info
    SoXtClipboardPasteCB    *callbackFunc;
    void		    *userData;
    
    // There can only be one owner of each X selection at any one time.
    // We use the selection atom as the key, and 'this' as the data.
    // We set the owner for each selection in this list for exportSelection.
    static SbDict	    *selOwnerList;
    
    // All the overloaded copy functions call this one.
    void    		    copy(SoByteStream *byteStream, Time t);
    
    // This describes what data types we look for during paste.
    SoXtImportInterestList  *pasteInterest;
    SbPList		    *copyInterest;

    // This is called by exportSelection
    void		    getExportTargets(
				char	    **value,
				uint32_t    *length);
				
    Atom		    chooseFromImportTargets(
				Atom *supportedTargets,
				int length);
				
    static SbBool	    writeToFile(
				SbString    &tmpfile, 
				void	    *srcData, 
				uint32_t    srcNumBytes);

    // Read the file if it is in Inventor format and add its data to the pathList.
    static void	    readFile(SoPathList *&pathList, const char *filename);

    // Read target data we just imported via paste or drop if it is in Inventor format.
    // This returns a newly allocated path list. This will free data when done.
    static SoPathList *readData(
			Widget w,
			Atom target,
			void *data,
			uint32_t numBytes);
  private:
    SoByteStream	    *copyBuffer; 	// copy/paste storage buffer
    Atom		    copyDataType;
    SbBool		    emptyListOK;
    
    // This handles the default cases of paste
    void	    pasteImport(Atom dataType, void *data, uint32_t numBytes);
    static void	    pasteImportCB(
			void *userData, Atom dataType,
			void *data, uint32_t numBytes)
		{ ((SoXtClipboard *)userData)->pasteImport(dataType, data, numBytes); }

    // Copy and paste callback functions - these are called by the X server.
    // importSelection is called when we import data to paste.
    // exportSelection is called when we send data for someone else to paste.
    // loseSelection is called when we no longer own the selection.
    static void		importSelection(
			    Widget		w,
			    SoXtClipboard    	*clipboard,
			    Atom		*selAtom,
			    Atom		*type,
			    char		*value,
			    uint32_t	*length,
			    int			*format);

    static Boolean	exportSelection(
			    Widget		w,
			    Atom		*xselection,
			    Atom		*target,
			    Atom		*type,
			    char		**value,
			    uint32_t	*length,
			    int			*format);

    static void		loseSelection(
			    Widget		w,
			    Atom		*xselection);
			    
    static void		importSelectionTargets(
			    Widget		w,
			    SoXtClipboard    	*clipboard,
			    Atom		*selAtom,
			    Atom		*type,
			    char		*value,
			    uint32_t	*length,
			    int			*format);
};

#endif /* _SO_XT_CLIPBOARD_ */
