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

class SoNode;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: NodeCreator
//
//  Widget class that is used to create a new node.
//
//////////////////////////////////////////////////////////////////////////////

// Type of callback function passed to constructor. newNode is NULL
// if no valid node was created
typedef void	NodeCreatorCB(void *userData, SoNode *newNode);

class NodeCreator {

  public:
    // Constructor - takes parent widget and callback to call when a
    // node name is selected
    NodeCreator(Widget parent, NodeCreatorCB *cb, const void *userData);

    // Destructor
    ~NodeCreator();

  private:
    NodeCreatorCB	*finishCB;	// Finish callback (NULL if none)
    void		*finishData;	// User data for callback
    SbBool		deleteMe;	// If should be deleted in destroyCB
    static Widget	widget;		// Widget
    static Widget	selBox;		// Selection box widget

    // Builds widget
    void		buildWidget(Widget parent);

    // Callback for when a creator is destroyed
    static void		destroyCB(Widget, XtPointer, XtPointer);

    // Callback for when a button is pressed
    static void		acceptCB(Widget, XtPointer, XtPointer);
    static void		cancelCB(Widget, XtPointer, XtPointer);

    // Returns text part of first segment of compound string
    static char *	decodeString(XmString string);

    // Returns an array of XmStrings containing the names of all known
    // node classes
    void		getNodeNames(XmString *&strings, int &numStrings);

    // Used to sort the node names for getNodeNames()
    static int		nameCompare(const void *, const void *);

    // Creates instance of named node and calls user's callback
    // function. The name may be a NULL pointer.
    void		invokeCallback(const char *nodeName);
};
