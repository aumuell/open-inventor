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

/////////////////////////////////////////////////////////////////////////////
//
// Types of menu buttons.
//

enum ButtonType {
    SEPARATOR, 
    PUSH, 
    TOGGLE
};

/////////////////////////////////////////////////////////////////////////////
//
// Button description structure.
//

struct ButtonInfo {
    char	*name;		// Name on button
    ActionItem	actionItem;	// Corresponding action code
    ButtonType	type;		// Type of button
    char	*accelerator;	// Passed to Motif
    char	*accelDisplay;	// Displayed in button
    GraphViewer	*viewer;	// Pointer to viewer instance
    Widget	widget;		// Button widget
};

/////////////////////////////////////////////////////////////////////////////
//
// Pulldown description structure.
//

struct PulldownInfo {
    char	*name;		// Name on top of menu
    ActionClass	actionClass;	// Class of actions in menu
    ButtonInfo	*buttons;	// Array of button info
    int		numButtons;	// Number of buttons
    GraphViewer	*viewer;	// Pointer to viewer instance
};

/////////////////////////////////////////////////////////////////////////////
//
// Button descriptions for each pulldown menu. The order of the
// buttons within a pulldown MUST match the order of the action codes.
//

static ButtonInfo	graphButtons[] = {
    { "Update", GRAPH_UPDATE,	PUSH,		"Alt <Key> u",	"Alt+u"	},
};

static ButtonInfo	editButtons[] = {
    { "Cut", 	     EDIT_CUT,	     PUSH,	"Alt <Key> x",	"Alt+x"	},
    { "Copy", 	     EDIT_COPY,	     PUSH,	"Alt <Key> c",	"Alt+c"	},
    { "Paste",	     EDIT_PASTE,     PUSH,	"Alt <Key> v",	"Alt+v"	},
    { "Paste Reference",
		     EDIT_PASTE_REF, PUSH,"Alt Shift <Key> v",	"Alt+V"	},
    { "Duplicate",   EDIT_DUP,	     PUSH,	"Alt <Key> d",	"Alt+d"	},
    { "Duplicate by Reference",
		     EDIT_DUP_REF,   PUSH,"Alt Shift <Key> d",	"Alt+D"	},
    { "Delete",      EDIT_DELETE,    PUSH,  "<Key> BackSpace",	"BackSpace" },
};

static ButtonInfo	selectButtons[] = {
    { "Left Sibling", SELECT_LEFT_SIB, PUSH, "<Key> Left",	"Left Arrow" },
    { "Right Sibling",SELECT_RIGHT_SIB,PUSH, "<Key> Right",	"Right Arrow"},
    { "Parent",	      SELECT_PARENT,   PUSH, "<Key> Up",	"Up Arrow" },
    { "First Child",  SELECT_1ST_CHILD,PUSH, "<Key> Down",	"Down Arrow"},
};

static ButtonInfo	groupButtons[] = {
    { "Open", 	 GROUP_OPEN,	PUSH,		"Alt <Key> o",	"Alt+o"	},
    { "Open All",GROUP_OPEN_ALL,PUSH,	  "Alt Shift <Key> o",	"Alt+O"	},
    { "Close", 	 GROUP_CLOSE,	PUSH,		"Alt <Key> k",	"Alt+k"	},
    { "Toggle",	 GROUP_TOGGLE,	PUSH,		"Alt <Key> t",	"Alt+t"	},
};

static ButtonInfo	instButtons[] = {
    { "Toggle",	INST_TOGGLE,	PUSH,		"Alt <Key> i",	"Alt+i"	},
    { "Swap", 	INST_SWAP,	PUSH,		"Alt <Key> w",	"Alt+w"	},
};

static ButtonInfo	nodeButtons[] = {
    { "Create",	     NODE_CREATE, PUSH,		"Alt <Key> n",	"Alt+n"	},
    { "Edit Fields", NODE_EDIT,	  PUSH,		"Alt <Key> f",	"Alt+f"	},
};

/////////////////////////////////////////////////////////////////////////////
//
// Pulldown descriptions.
//

static PulldownInfo	pullInfos[] = {
    { "Graph",	GRAPH_CLASS,	graphButtons,	XtNumber(graphButtons)	},
    { "Edit",	EDIT_CLASS,	editButtons,	XtNumber(editButtons)	},
    { "Select",	SELECT_CLASS,	selectButtons,	XtNumber(selectButtons)	},
    { "Group",	GROUP_CLASS,	groupButtons,	XtNumber(groupButtons)	},
    { "Inst",	INST_CLASS,	instButtons,	XtNumber(instButtons)	},
    { "Node",	NODE_CLASS,	nodeButtons,	XtNumber(nodeButtons)	},
};

/////////////////////////////////////////////////////////////////////////////
//
// Number of pulldowns.
//

#define NUM_PULLDOWNS 	XtNumber(pullInfos)
