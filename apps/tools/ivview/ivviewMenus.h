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


enum MenuEntries {
    IV_FILE = 0,
    IV_FILE_ABOUT,
    IV_FILE_OPEN,
    IV_FILE_QUIT,

    IV_EDIT,
    IV_EDIT_TRANSPARENCY,
    IV_EDIT_BACKGROUND_CLR,

    IV_OPTIMIZE,
    IV_OPTIMIZE_IVFIX,
    IV_OPTIMIZE_SHAPEHINTS,

    IV_MENU_NUM		// this must be the last entry
};

enum ButtonTypes {
    IV_SEPARATOR,
    IV_PUSH_BUTTON,
    IV_TOGGLE_BUTTON,
    IV_RADIO_BUTTON
};

struct ivButton {
    char *name;
    int   id;
    int   buttonType;  // PUSH, TOGGLE, RADIO
    char *accelerator; // e.g. "Alt <Key> p"
    char *accelText;   // text that appears in the menu item
};

struct ivMenuItem {
    int    id;
    Widget widget;
};

struct ivMenu {
    char  *name;
    int    id;
    struct ivButton *subMenu;
    int    subItemCount;
};

ivButton fileData[] = {
   {(char *)NULL, IV_FILE_ABOUT, IV_PUSH_BUTTON, 0, 0 },
   {(char *)NULL,  IV_FILE_OPEN,  IV_PUSH_BUTTON, (char *)NULL, (char *)NULL },
   {(char *)NULL,  IV_FILE_QUIT,  IV_PUSH_BUTTON, (char *)NULL, (char *)NULL },
};

#define IVVIEW_OFFSET(b,m) (((char*)(m)) - (char*)(b))

XtResource intl_file_resources[] = {
  { "aboutMenuLabel", NULL, XtRString, sizeof(XtRString),
    (Cardinal) ((char*)&fileData[0].name - (char*)&fileData[0]),
    XtRString, (XtPointer) "About...",},
  { "openMenuLabel", NULL, XtRString, sizeof(XtRString),
    (Cardinal) ((char*)&fileData[1].name - (char*)&fileData[0]),
    XtRString, (XtPointer) "Open...",},
  { "openMenuLabelAcc", NULL, XtRString, sizeof(XtRString),
    (Cardinal) ((char*)&fileData[1].accelerator - (char*)&fileData[0]),
    XtRString, (XtPointer) "Alt <Key> o",},
  { "openMenuLabelAccKey", NULL, XtRString, sizeof(XtRString),
    (Cardinal) ((char*)&fileData[1].accelText - (char*)&fileData[0]),
    XtRString, (XtPointer) "Alt+o",},
  { "quitMenuLabel", NULL, XtRString, sizeof(XtRString),
    (Cardinal) ((char*)&fileData[2].name - (char*)&fileData[0]),
    XtRString, (XtPointer) "Quit",},
  { "quitMenuLabelAcc", NULL, XtRString, sizeof(XtRString),
    (Cardinal) ((char*)&fileData[2].accelerator - (char*)&fileData[0]),
    XtRString, (XtPointer) "Alt <Key> q",},
  { "quitMenuLabelAccKey", NULL, XtRString, sizeof(XtRString),
    (Cardinal) ((char*)&fileData[2].accelText - (char*)&fileData[0]),
    XtRString, (XtPointer) "Alt+q",},
};
int intl_num_file_resources = XtNumber( intl_file_resources );

ivButton editData[] = {
   {(char *)NULL, IV_EDIT_TRANSPARENCY, IV_TOGGLE_BUTTON, 0, 0 },
   {(char *)NULL, IV_EDIT_BACKGROUND_CLR, IV_PUSH_BUTTON, 0, 0 },
};

XtResource intl_edit_resources[] = {
  { "transparencyMenuLabel", NULL, XtRString, sizeof(XtRString),
    (Cardinal) ((char*)&editData[0].name - (char*)&editData[0]),
    XtRString, (XtPointer) "High quality transparency",},
  { "colorMenuLabel", NULL, XtRString, sizeof(XtRString),
    (Cardinal) ((char*)&editData[1].name - (char*)&editData[0]),
    XtRString, (XtPointer) "Edit Background color...",},
};
int intl_num_edit_resources = XtNumber( intl_edit_resources );

ivButton optimizeData[] = {
   {(char *)NULL, IV_OPTIMIZE_IVFIX, IV_TOGGLE_BUTTON, 0, 0 },
   {(char *)NULL,  IV_OPTIMIZE_SHAPEHINTS,  IV_TOGGLE_BUTTON, 0, 0 },
};

XtResource intl_optimize_resources[] = {
  { "speedMenuLabel", NULL, XtRString, sizeof(XtRString),
    (Cardinal) ((char*)&optimizeData[0].name - (char*)&optimizeData[0]),
    XtRString, (XtPointer) "Optimize for speed (ivfix)",},
  { "cullMenuLabel", NULL, XtRString, sizeof(XtRString),
    (Cardinal) ((char*)&optimizeData[1].name - (char*)&optimizeData[0]),
    XtRString, (XtPointer) "Cull backfaces",},
};
int intl_num_optimize_resources = XtNumber( intl_optimize_resources );

ivMenu pulldownData[] = {
// {name, id, subMenu, subItemCount}
   {(char *)NULL,  IV_FILE, fileData, XtNumber(fileData) },
   {(char *)NULL,  IV_EDIT, editData, XtNumber(editData) },
   {(char *)NULL,  IV_OPTIMIZE,  optimizeData, XtNumber(optimizeData)},
};

XtResource intl_pulldown_resources[] = {
  { "fileMenuLabel", NULL, XtRString, sizeof(XtRString),
    (Cardinal) ((char*)&pulldownData[0].name - (char*)&pulldownData[0]),
    XtRString, (XtPointer) "File",},
  { "optionsMenuLabel", NULL, XtRString, sizeof(XtRString),
    (Cardinal) ((char*)&pulldownData[1].name - (char*)&pulldownData[0]),
    XtRString, (XtPointer) "Options",},
  { "optimizeMenuLabel", NULL, XtRString, sizeof(XtRString),
    (Cardinal) ((char*)&pulldownData[2].name - (char*)&pulldownData[0]),
    XtRString, (XtPointer) "Optimize",},
};
int intl_num_pulldown_resources = XtNumber( intl_pulldown_resources );
