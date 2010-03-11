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
 * Copyright (C) 1994-95   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Description:
 |  	This class supports the "drop" in "drag and drop". Meaning,  if
 |   a file or object is dropped on a widget, this will do all the Motif
 |   and Inventor stuff necessary to import the data.
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef _MY_DROP_SITE_
#define _MY_DROP_SITE_


#include <X11/Intrinsic.h>

class SbStringList;

// Callbacks should return TRUE if it accepted the dropped data.
// The x,y value is the position of the cursor when the drop occured.

// Typedef for callback invoked for each dropped file.
typedef SbBool	MyIconDropCB(void *userData, const char *filename);

// Typedef for callback invoked for each dropped object
typedef SbBool	MyObjectDropCB(
		    void *userData, 
		    Atom dataType,
		    void *data, uint32_t numBytes);

////////////////////////////////////////////////////////////////////////
//
//  This class has all static methods.
//  registerCallback() should be called just once.
//
////////////////////////////////////////////////////////////////////////

class MyDropSite {
  public:
    // Registers a callback to invoke for each dropped file
    static void	registerCallback(
		    Widget site,
		    MyIconDropCB *iconDropCB,
		    MyObjectDropCB *objDropCB,
		    void *data = NULL);

  private:
    static Widget	    siteWidget;
    static MyIconDropCB	    *iconDropCB;
    static MyObjectDropCB   *objDropCB;
    static void		    *dropCBData;
    
    static Atom		    importList[10];
    static int		    numImportTargets;


    // This callback is invoked when the drop occurs
    static void handleDrop(Widget w, 
			   XtPointer client_data, XtPointer call_data,
			   XtPointer proc);

    // Reads the files/objects that were dropped
    static void transferIconProc(Widget, XtPointer, Atom *, Atom *type,
			     XtPointer value, unsigned long *length, int);
    static void transferObjectProc(Widget, XtPointer, Atom *, Atom *type,
			     XtPointer value, unsigned long *length, int);

    // Find a match between the senders export list and our import list.
    static Atom chooseTarget(
			    Atom *exportList,
			    int numExportTargets);

    // Parses the icon information from the drop action and
    // extracts a list of file names to open.
    static void parseIcon(Atom *type, XtPointer value, unsigned long *length,
			  SbStringList *fileList);
};

#endif /* _MY_DROP_SITE_ */
