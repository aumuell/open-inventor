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

#ifndef  _SO_SCENE_MENU_
#define  _SO_SCENE_MENU_

// -*- C++ -*-
//
// header for TopbarMenu
//

#include <X11/Intrinsic.h>
#include <Inventor/SbBasic.h>

enum MenuEntries {
    SV_FILE = 0,  // start at 0 since we use an array
    SV_FILE_ABOUT, 
    SV_FILE_OPEN, 
    SV_FILE_IMPORT, 
    SV_FILE_NEW, 
    SV_FILE_SAVE, 
    SV_FILE_SAVE_AS, 
    SV_FILE_SAVE_ENV, 
    SV_FILE_READ_ENV, 
    SV_FILE_PRINT, 
    SV_FILE_QUIT, 
    
    SV_EDIT, 
    SV_EDIT_PICK_PARENT, 
    SV_EDIT_PICK_ALL, 
    SV_EDIT_CUT, 
    SV_EDIT_COPY, 
    SV_EDIT_PASTE, 
    SV_EDIT_DELETE, 
    
    SV_VIEW, 
    SV_VIEW_PICK, 
#ifdef EXPLORER
    SV_VIEW_USER,
#endif
    SV_VIEW_EXAMINER, 
    SV_VIEW_FLY, 
    SV_VIEW_WALK, 
    SV_VIEW_PLANE, 
    SV_VIEW_SELECTION, 
    SV_VIEW_FOG, 
    SV_VIEW_ANTIALIASING, 
    SV_VIEW_SCREEN_TRANSPARENCY, 
    SV_VIEW_BLEND_TRANSPARENCY, 
    SV_VIEW_DELAY_BLEND_TRANSPARENCY, 
    SV_VIEW_SORT_BLEND_TRANSPARENCY, 
    SV_VIEW_BKG_COLOR, 
    
    SV_EDITOR, 
    SV_EDITOR_TRANSFORM, 
    SV_EDITOR_MATERIAL, 
    SV_EDITOR_COLOR, 

    SV_MANIP,  
    SV_MANIP_TRANSFORMER, 
    SV_MANIP_HANDLEBOX, 
    SV_MANIP_TRACKBALL, 
    SV_MANIP_JACK,
    SV_MANIP_CENTERBALL,
    SV_MANIP_XFBOX,
    SV_MANIP_TABBOX,

// rotateCylindrical rotateDisc RotateSpherical 
// scale1 scale2 scale2Uniform scaleUniform 
// translate1 translate2 

    SV_MANIP_NONE, 
    SV_MANIP_REPLACE_ALL, 
    
    SV_LIGHT, 
    SV_LIGHT_ADD_DIRECT, 
    SV_LIGHT_ADD_POINT, 
    SV_LIGHT_ADD_SPOT, 
    SV_LIGHT_AMBIENT_EDIT, 
    SV_LIGHT_TURN_ON, 
    SV_LIGHT_TURN_OFF, 
    SV_LIGHT_SHOW_ALL, 
    SV_LIGHT_HIDE_ALL, 
    
    SV_SELECTION, 
    SV_SEL_SINGLE_SELECT, 
    SV_SEL_TOGGLE_SELECT, 
    SV_SEL_SHIFT_SELECT, 
    
    SV_MENU_NUM    // this must be the last entry
};


// different types of menu item buttons
enum MenuItems {
    SV_SEPARATOR, 
    SV_PUSH_BUTTON, 
    SV_TOGGLE_BUTTON, 
    SV_RADIO_BUTTON
};

struct SoSceneViewerButton {
    char        *name;
    int		id;
    int  	buttonType; // PUSH, TOGGLE, RADIO
    char	*accelerator; // e.g. "Alt <Key> p" or "Ctrl <Key> u"
    char	*accelText;   // text that appears in the menu item
};
struct SoSceneViewerMenu {
    char        *name;
    int	    	id;
    struct 	SoSceneViewerButton *subMenu;
    int		subItemCount;
};


static SoSceneViewerButton fileData[] = {
#ifndef EXPLORER
    {(char *)NULL,	    SV_FILE_ABOUT,	SV_PUSH_BUTTON, 0, 0 },
    {0,    		    0,			SV_SEPARATOR },
    {"New",		    SV_FILE_NEW,	SV_PUSH_BUTTON, "Alt <Key> n", "Alt+n" },
    {(char *)NULL,	    SV_FILE_OPEN,	SV_PUSH_BUTTON, "Alt <Key> o", "Alt+o" },
    {(char *)NULL,	    SV_FILE_IMPORT,	SV_PUSH_BUTTON, "Alt <Key> i", "Alt+i" },
#endif /* EXPLORER */
    {(char *)NULL,	    SV_FILE_SAVE,	SV_PUSH_BUTTON, "Alt <Key> s", "Alt+s" },
    {(char *)NULL,	    SV_FILE_SAVE_AS,	SV_PUSH_BUTTON, "Alt Shift <Key> s", "Alt+S" },
#ifndef EXPLORER
    {0,    		    0,			SV_SEPARATOR },
    {(char *)NULL,          SV_FILE_READ_ENV,	SV_PUSH_BUTTON, 0, 0 },
    {(char *)NULL,          SV_FILE_SAVE_ENV,	SV_PUSH_BUTTON, 0, 0 },
    {0,    		    0,			SV_SEPARATOR },
    {(char *)NULL,	    SV_FILE_PRINT,	SV_PUSH_BUTTON, "Alt <Key> p", "Alt+p" },
    {"Quit",		    SV_FILE_QUIT,	SV_PUSH_BUTTON, "Alt <Key> q", "Alt+q" },
#endif /* EXPLORER */
};

#ifndef EXPLORER
static SoSceneViewerButton editData[] = {
    {"Pick Parent",	SV_EDIT_PICK_PARENT, SV_PUSH_BUTTON, 0, 0 },
    {"Pick All",	SV_EDIT_PICK_ALL, SV_PUSH_BUTTON, "Alt <Key> a", "Alt+a" },
    {0,    		    0,			SV_SEPARATOR },
    {"Cut",		SV_EDIT_CUT,	SV_PUSH_BUTTON, "Alt <Key> x", "Alt+x" },
    {"Copy",		SV_EDIT_COPY,	SV_PUSH_BUTTON, "Alt <Key> c", "Alt+c" },
    {"Paste",		SV_EDIT_PASTE,	SV_PUSH_BUTTON, "Alt <Key> v", "Alt+v" },
    {"Delete",		SV_EDIT_DELETE,	SV_PUSH_BUTTON, "<Key> BackSpace", 0 },
};
#endif /* EXPLORER */

static SoSceneViewerButton viewData[] = {
    {"Pick/Edit",  SV_VIEW_PICK,	       SV_TOGGLE_BUTTON, 0, 0},
#ifdef EXPLORER
    {"User Pick Mode",  SV_VIEW_USER,    SV_TOGGLE_BUTTON, 0, 0},
#endif /* EXPLORER */
    {0,    		    0,			SV_SEPARATOR },
    {"Examiner",   SV_VIEW_EXAMINER,        SV_RADIO_BUTTON, 0, 0},
    {"Fly",	   SV_VIEW_FLY,	       SV_RADIO_BUTTON, 0, 0},
    {"Walk",	   SV_VIEW_WALK,	       SV_RADIO_BUTTON, 0, 0},
    {"Plane",	   SV_VIEW_PLANE,	       SV_RADIO_BUTTON, 0, 0},
    {0,    		    0,			SV_SEPARATOR },
    {"View Selection",SV_VIEW_SELECTION,     SV_PUSH_BUTTON, 0, 0},
    {"Fog", 	   SV_VIEW_FOG, 	       SV_TOGGLE_BUTTON, 0, 0},
    {"Antialiasing",SV_VIEW_ANTIALIASING,   SV_TOGGLE_BUTTON, 0, 0},
    {0,    		    0,			SV_SEPARATOR },
    {"Screen Door Transparency", SV_VIEW_SCREEN_TRANSPARENCY,  SV_RADIO_BUTTON, 0, 0},
    {"Blended Transparency", SV_VIEW_BLEND_TRANSPARENCY,  SV_RADIO_BUTTON, 0, 0},
    {"Delayed Blended Transparency", SV_VIEW_DELAY_BLEND_TRANSPARENCY,  SV_RADIO_BUTTON, 0, 0},
    {"Sorted Blended Transparency", SV_VIEW_SORT_BLEND_TRANSPARENCY,  SV_RADIO_BUTTON, 0, 0},
    {0,    		    0,			SV_SEPARATOR },
    {(char *)NULL,  SV_VIEW_BKG_COLOR,  SV_PUSH_BUTTON, 0, 0}
};

static SoSceneViewerButton editorData[] = {
    {(char *)NULL,    SV_EDITOR_MATERIAL,    SV_PUSH_BUTTON, 0, 0},
    {(char *)NULL,       SV_EDITOR_COLOR,       SV_PUSH_BUTTON, 0, 0},
    {(char *)NULL,  SV_EDITOR_TRANSFORM,   SV_PUSH_BUTTON, 0, 0},
};

static SoSceneViewerButton manipData[] = {
    {"Transformer", SV_MANIP_TRANSFORMER,    SV_RADIO_BUTTON, 0, 0},
    {"Trackball",   SV_MANIP_TRACKBALL,      SV_RADIO_BUTTON, 0, 0},
    {"HandleBox",   SV_MANIP_HANDLEBOX,      SV_RADIO_BUTTON, 0, 0},
    {"Jack",        SV_MANIP_JACK,           SV_RADIO_BUTTON, 0, 0},
    {"Centerball",  SV_MANIP_CENTERBALL,     SV_RADIO_BUTTON, 0, 0},
    {"TransformBox",SV_MANIP_XFBOX,          SV_RADIO_BUTTON, 0, 0},
    {"TabBox",      SV_MANIP_TABBOX,         SV_RADIO_BUTTON, 0, 0},
    {"None",        SV_MANIP_NONE,           SV_RADIO_BUTTON, 0, 0},
    {0,    		    0,			SV_SEPARATOR },
    {"Replace",     SV_MANIP_REPLACE_ALL,    SV_TOGGLE_BUTTON, 0, 0}
};

static SoSceneViewerButton lightData[] = {
    {"Create Dir Light",	    SV_LIGHT_ADD_DIRECT,    SV_PUSH_BUTTON, 0, 0},
    {"Create Point Light",	    SV_LIGHT_ADD_POINT,	    SV_PUSH_BUTTON, 0, 0},
    {"Create Spot Light",	    SV_LIGHT_ADD_SPOT,	    SV_PUSH_BUTTON, 0, 0},
    {0,    		    0,			SV_SEPARATOR },
    {(char *)NULL,	    SV_LIGHT_AMBIENT_EDIT,  SV_PUSH_BUTTON, 0, 0},
    {0,    		    0,			SV_SEPARATOR },
    {"Turn all ON",		    SV_LIGHT_TURN_ON,	    SV_PUSH_BUTTON, 0, 0},
    {"Turn all OFF",		    SV_LIGHT_TURN_OFF,	    SV_PUSH_BUTTON, 0, 0},
    {"Show all Icons",		    SV_LIGHT_SHOW_ALL,	    SV_PUSH_BUTTON, 0, 0},
    {"Hide all Icons",		    SV_LIGHT_HIDE_ALL,	    SV_PUSH_BUTTON, 0, 0},
    {0,    		    0,			SV_SEPARATOR },
};

#ifndef EXPLORER
static SoSceneViewerButton selData[] = {
    {"Single Selection", SV_SEL_SINGLE_SELECT,  SV_RADIO_BUTTON, 0, 0},
    {"Toggle Selection", SV_SEL_TOGGLE_SELECT,  SV_RADIO_BUTTON, 0, 0},
    {"Shift Selection",	SV_SEL_SHIFT_SELECT,	SV_RADIO_BUTTON, 0, 0},
};
#endif /* EXPLORER */

static SoSceneViewerMenu pulldownData[] = {
//  {name, 	id,	    	subMenu,    subItemCount}
    {"File",    SV_FILE, 	fileData,   XtNumber(fileData)   },
#ifndef EXPLORER
    {"Edit",    SV_EDIT, 	editData,   XtNumber(editData)   },
#endif /* EXPLORER */
    {"Viewing", SV_VIEW, 	viewData,   XtNumber(viewData)   },
#ifndef EXPLORER
    {"Selection", SV_SELECTION, selData, XtNumber(selData)	 },
#endif /* EXPLORER */
    {"Editors", SV_EDITOR, 	editorData, XtNumber(editorData) },
    {"Manips",  SV_MANIP, 	manipData,  XtNumber(manipData)  },
    {"Lights",  SV_LIGHT, 	lightData,  XtNumber(lightData)  }
};

#endif

static struct {
char *headlight;
char *editlight;
char *directionallight;
char *spotlight;
char *pointlight;
} Light;


XtResource intl_resources[] = {  /* CAUTION: Mixing #ifdef and #ifndef EXPLORER */
#ifndef EXPLORER                 
  { "aboutMenuLabel", NULL, XtRString, sizeof(XtRString),
    (Cardinal) &fileData[0].name, XtRString, "About...",},
  { "openMenuLabel", NULL, XtRString, sizeof(XtRString),
    (Cardinal) &fileData[3].name, XtRString, "Open...",},
  { "importMenuLabel", NULL, XtRString, sizeof(XtRString),
    (Cardinal) &fileData[4].name, XtRString, "Import...",},
  { "saveMenuLabel", NULL, XtRString, sizeof(XtRString),
    (Cardinal) &fileData[5].name, XtRString, "Save",},
  { "saveAsMenuLabel", NULL, XtRString, sizeof(XtRString),
    (Cardinal) &fileData[6].name, XtRString, "Save As...",},
  { "readEnvMenuLabel", NULL, XtRString, sizeof(XtRString),
    (Cardinal) &fileData[8].name, XtRString, "Read Environment...",},
  { "saveEnvMenuLabel", NULL, XtRString, sizeof(XtRString),
    (Cardinal) &fileData[9].name, XtRString, "Save Environment...",},
  { "printMenuLabel", NULL, XtRString, sizeof(XtRString),
    (Cardinal) &fileData[11].name, XtRString, "Print...",},
  { "editBgColor", NULL, XtRString, sizeof(XtRString),
    (Cardinal) &viewData[16].name, XtRString, "Edit Background Color...",},
#else
  { "saveMenuLabel", NULL, XtRString, sizeof(XtRString),
    (Cardinal) &fileData[0].name, XtRString, "Save",},
  { "saveAsMenuLabel", NULL, XtRString, sizeof(XtRString),
    (Cardinal) &fileData[1].name, XtRString, "Save As...",},
  { "editBgColor", NULL, XtRString, sizeof(XtRString),
    (Cardinal) &viewData[17].name, XtRString, "Edit Background Color...",},
#endif
  { "materialEditor", NULL, XtRString, sizeof(XtRString),
    (Cardinal) &editorData[0].name, XtRString, "Material Editor...",},
  { "colorEditor", NULL, XtRString, sizeof(XtRString),
    (Cardinal) &editorData[1].name, XtRString, "Color Editor...",},
  { "transformSliders", NULL, XtRString, sizeof(XtRString),
    (Cardinal) &editorData[2].name, XtRString, "Transform Sliders...",},
  { "ambientLighting", NULL, XtRString, sizeof(XtRString),
    (Cardinal) &lightData[4].name, XtRString, "Ambient Lighting...",},
  { "headlight", NULL, XtRString, sizeof(XtRString),
    (Cardinal) &Light.headlight, XtRString, "Headlight ",},
  { "editlight", NULL, XtRString, sizeof(XtRString),
    (Cardinal) &Light.editlight, XtRString, "Edit",},
  { "directionallight", NULL, XtRString, sizeof(XtRString),
    (Cardinal) &Light.directionallight, XtRString, "Directional ",},
  { "spotlight", NULL, XtRString, sizeof(XtRString),
    (Cardinal) &Light.spotlight, XtRString, "Spot ",},
  { "pointlight", NULL, XtRString, sizeof(XtRString),
    (Cardinal) &Light.pointlight, XtRString, "Point ",},
};
int intl_num_resources = XtNumber( intl_resources );
