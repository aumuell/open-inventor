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

#ifndef  _PULL_DOWNS_
#define  _PULL_DOWNS_

#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Xatom.h>

#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/BulletinB.h>
#include <Xm/CascadeB.h>
#include <Xm/CascadeBG.h>
#include <Xm/FileSB.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/FileSB.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/SeparatoG.h>
#include <Xm/Text.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>

#include "Interface.h"

enum MenuEntries {
    MM_FILE = 0,
    MM_FILE_NEW,
    MM_FILE_OPEN,
    MM_FILE_SAVE,
    MM_FILE_SAVE_AS,
    MM_FILE_SAVE_VANILLA,
    MM_FILE_QUIT,

    MM_EDIT,
    MM_EDIT_NEW,
    MM_EDIT_SEP,
    MM_EDIT_DELETE,
    MM_EDIT_UNDELETE,
    MM_EDIT_SEP2,
    MM_EDIT_COPY,
    MM_EDIT_COPY_ALL,

    MM_PARTS,
    MM_PARTS_SIDES,
    MM_PARTS_TOP_CAP,
    MM_PARTS_BOT_CAP,

    MM_SHAPE,
    MM_SHAPE_FACE_SET,
    MM_SHAPE_TRI_STRIP,
    MM_SHAPE_QUAD_MESH,
    MM_SHAPE_CUBIC_SPLINE,
    MM_SHAPE_CUBIC_TO_EDGE,
    MM_SHAPE_BEZIER,

    MM_MANIPS,
    MM_MANIPS_HBOX,
    MM_MANIPS_TRACKBALL,
    MM_MANIPS_NONE,

    MM_GIZMOS,
    MM_GIZMOS_TEXTURE,
    MM_GIZMOS_SURFACE,

    MM_ABOUT,
    MM_ABOUT_ABOUT,

    MM_MENU_NUM		// this must be the last entry
};

enum ButtonTypes {
    MM_SEPARATOR,
    MM_PUSH_BUTTON,
    MM_TOGGLE_BUTTON,
    MM_RADIO_BUTTON
};

static NoodleButtonInfo fileData[] = {
   {"New...",     MM_FILE_NEW,     MM_PUSH_BUTTON, 0, "Alt Shift <Key> n", "Alt+N" },
   {"Open...",    MM_FILE_OPEN,    MM_PUSH_BUTTON, 0, "Alt <Key> o", "Alt+o" },
   {"Save",       MM_FILE_SAVE,    MM_PUSH_BUTTON, 0, "Alt <Key> s", "Alt+s" },
   {"Save As...", MM_FILE_SAVE_AS, MM_PUSH_BUTTON, 0, "Alt Shift <Key> s", "Alt+S" },
   {"Save Standard Inventor...", MM_FILE_SAVE_VANILLA, MM_PUSH_BUTTON, 0, "Alt <Key> v", "Alt+v" },
   {"Quit",       MM_FILE_QUIT,    MM_PUSH_BUTTON, 0, "Alt <Key> q", "Alt+q" },
};

static NoodleButtonInfo editData[] = {
   {"New",        MM_EDIT_NEW,      MM_PUSH_BUTTON, 0, "Alt <Key> n", "Alt+n" },
   {"",           MM_EDIT_SEP, 	    MM_SEPARATOR,   0, 0, 0 },
   {"Delete",     MM_EDIT_DELETE,   MM_PUSH_BUTTON, 0, "Alt <Key> d", "Alt+d" },
   {"Undelete",   MM_EDIT_UNDELETE, MM_PUSH_BUTTON, 0, "Alt <Key> u", "Alt+u" },
   {"",           MM_EDIT_SEP2,	    MM_SEPARATOR,   0, 0, 0 },
   {"Copy",       MM_EDIT_COPY,     MM_PUSH_BUTTON, 0, "Alt <Key> c", "Alt+c" },
   {"CopyAll",    MM_EDIT_COPY_ALL, MM_PUSH_BUTTON, 0, "Alt Shift <Key> c", "Alt+C" },
};

static NoodleButtonInfo partsData[] = {
   {"Sides", MM_PARTS_SIDES, MM_TOGGLE_BUTTON, 1, 0, 0 },
   {"Top Cap", MM_PARTS_TOP_CAP, MM_TOGGLE_BUTTON, 1, 0, 0 },
   {"Bottom Cap", MM_PARTS_BOT_CAP, MM_TOGGLE_BUTTON, 1, 0, 0 },
};

static NoodleButtonInfo shapeData[] = {
   {"Face Set", MM_SHAPE_FACE_SET, MM_RADIO_BUTTON, 1, 0, 0 },
   {"Triangle Strip Set", MM_SHAPE_TRI_STRIP, MM_RADIO_BUTTON, 0, 0, 0 },
   {"Quad Mesh", MM_SHAPE_QUAD_MESH, MM_RADIO_BUTTON, 0, 0, 0 },
   {"Cubic Spline", MM_SHAPE_CUBIC_SPLINE, MM_RADIO_BUTTON, 0, 0, 0 },
   {"Cubic Spline To Edge", MM_SHAPE_CUBIC_TO_EDGE, MM_RADIO_BUTTON, 0, 0, 0 },
   {"Bezier", MM_SHAPE_BEZIER, MM_RADIO_BUTTON, 0, 0, 0 },
};

static NoodleButtonInfo manipsData[] = {
   {"HandleBox", MM_MANIPS_HBOX,       MM_RADIO_BUTTON, 1, 0, 0 },
   {"Trackball", MM_MANIPS_TRACKBALL,  MM_RADIO_BUTTON, 0, 0, 0 },
   {"None",      MM_MANIPS_NONE,       MM_RADIO_BUTTON, 0, 0, 0 },
};

static NoodleButtonInfo gizmosData[] = {
   {"Texture Gizmo", MM_GIZMOS_TEXTURE, MM_PUSH_BUTTON, 0, 0, 0 },
   {"Surface Gizmo", MM_GIZMOS_SURFACE, MM_PUSH_BUTTON, 0, 0, 0 },
};

static NoodleButtonInfo aboutData[] = {
   {"About Noodle...",   MM_ABOUT_ABOUT,   MM_PUSH_BUTTON, 0, "Alt <Key> h", "Alt+h" },
};

static NoodleMenu pulldownData[] = {
// {name, id, subMenu, subItemCount}
   {"File",  MM_FILE, fileData, XtNumber(fileData) },
   {"Edit",  MM_EDIT, editData, XtNumber(editData) },
   {"Parts",  MM_PARTS, partsData, XtNumber(partsData) },
   {"ShapeStyle",  MM_SHAPE, shapeData, XtNumber(shapeData) },
   {"Manips",  MM_MANIPS, manipsData, XtNumber(manipsData) },
   {"Gizmos",  MM_GIZMOS, gizmosData, XtNumber(gizmosData) },
   {"About",  MM_ABOUT, aboutData, XtNumber(aboutData) },
};

#endif /* _PULL_DOWNS_ */
