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
 * Copyright (C) 1991-93   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.5 $
 |
 |   Classes:
 |	SoXtMaterialList
 |
 |   Author(s):	David Mott
 |
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <inttypes.h>
#include <stdio.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>

#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <Xm/CascadeBG.h>
#include <Xm/Form.h>
#include <Xm/List.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>

#include <Inventor/SoDB.h>
#include <Inventor/misc/SoCallbackList.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/SoXtMaterialList.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/errors/SoDebugError.h>

static char *bogusFile = "<none>";

#define TOGGLE_ON(BUTTON) \
    XmToggleButtonSetState((Widget) BUTTON, TRUE, FALSE);
#define TOGGLE_OFF(BUTTON) \
    XmToggleButtonSetState((Widget) BUTTON, FALSE, FALSE);

//
//	returns true if the passed file is a subdirectory in the current directory.
//	(call chdir() before calling this).
//
static SbBool
isDirectory(char *file)
{
    struct stat buf;
    SbBool is_dir = FALSE;

    if (stat(file, &buf) == 0)
	if ((buf.st_mode & S_IFMT) == S_IFDIR)
	    is_dir = 1;

    return (is_dir);
}

//
//  this returns TRUE if the file contains a material
//
static SbBool
isMtlFile(char *filename)
{
    // Make sure it's not . or ..
    if (filename[0] == '.') return FALSE;
    
    // read the file, search for SoMaterial
    SbBool fileHasMtl = FALSE;
    SoInput in;
    SoNode  *root;
    if (in.openFile(filename)) {
	if (SoDB::read(&in, root)) {
	    root->ref();
	    
	    // find the material node
	    SoSearchAction sa;
	    sa.setType(SoMaterial::getClassTypeId());
	    sa.apply(root);
	    fileHasMtl = (sa.getPath() != NULL);
	    
	    root->unref();
	}
    }
    
    return (fileHasMtl);
}


//
//  Fill in palette with material files
//
static void
createStringTable(char *dir, XmString **table, int *size)
{
    XmString *tbl;
    int numFiles = 0;

    // build the list of files
    struct dirent *direntry;
    DIR  *dirp;
    char *f;
    if (dirp = opendir(dir)) {
    	char currentDir[MAXPATHLEN];
	getcwd(currentDir, MAXPATHLEN-1);
        chdir(dir);
	
	// count the number of files so we can allocate space in the str table
        while (direntry = readdir(dirp)) {
	    f = direntry->d_name;
	    if (isMtlFile(f))
		numFiles++;
        }
        (void) closedir(dirp);
	
	// now build the string table
	if (numFiles != 0) {
	    // get space for the table
	    tbl = (XmString *) malloc(numFiles * sizeof(XmString));
	    *table = tbl;
	    *size = numFiles;
	
	    // fill the table in
	    dirp = opendir(dir);
	    chdir(dir);
	    while (direntry = readdir(dirp)) {
		f = direntry->d_name;
		if (isMtlFile(f)) {
		    *tbl = SoXt::encodeString(f);
		    tbl++;
		}
	    }
	    (void) closedir(dirp);
	}
	else {
	    // get space for one entry - bogusFile
	    tbl = (XmString *) malloc(sizeof(XmString));
	    *table = tbl;
	    *size = 1;
	    *tbl = SoXt::encodeString(bogusFile);
	}
	chdir(currentDir); // back to our working directory
    }
#ifdef DEBUG
    else {
	SoDebugError::post("SoXtMaterialList::createStringTable",
		"Cannot open directory %s.", dir);
    }
#endif
}

static void
destroyStringTable(XmString *table, int size)
{
    int i;

    // nuke the table entries
    for (i = 0; i < size; i++)
	XmStringFree(table[i]);

    free(table);
}

////////////////////////////////////////////////////////////////////////
//
// Public constructor - build the widget right now
//
SoXtMaterialList::SoXtMaterialList(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent, 
    const char *dir)
	: SoXtComponent(
	    parent,
	    name, 
	    buildInsideParent)
//
////////////////////////////////////////////////////////////////////////
{
    // In this case, this component is what the app wants, so buildNow = TRUE
    constructorCommon(dir, TRUE);
}

////////////////////////////////////////////////////////////////////////
//
// SoEXTENDER constructor - the subclass tells us whether to build or not
//
SoXtMaterialList::SoXtMaterialList(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent, 
    const char *dir, 
    SbBool buildNow)
	: SoXtComponent(
	    parent,
	    name, 
	    buildInsideParent)
//
////////////////////////////////////////////////////////////////////////
{
    // In this case, this component may be what the app wants, 
    // or it may want a subclass of this component. Pass along buildNow
    // as it was passed to us.
    constructorCommon(dir, buildNow);
}

////////////////////////////////////////////////////////////////////////
//
// Called by the constructors
//
// private
//
void
SoXtMaterialList::constructorCommon(const char *dir, SbBool buildNow)
//
//////////////////////////////////////////////////////////////////////
{
    setClassName("SoXtMaterialList");
    if (dir != NULL)
	materialDir = strdup(dir);
    else materialDir = strdup(IVPREFIX "/share/data/materials");

    callbackList = new SoCallbackList;
    curPalette = -1;
    
    // Build the widget tree, and let SoXtComponent know about our base widget.
    if (buildNow) {
	Widget w = buildWidget(getParentWidget());
	if (w != NULL)
	    setBaseWidget(w);
    }
}


////////////////////////////////////////////////////////////////////////
//
//    Destructor.
//
SoXtMaterialList::~SoXtMaterialList()
//
////////////////////////////////////////////////////////////////////////
{
    delete callbackList;
    if (materialDir != NULL) free(materialDir);
    for (int i = 0; i < mtlPalettes.getLength(); i++)
    	free((char *) mtlPalettes[i]);
}

////////////////////////////////////////////////////////////////////////
//
// Get the dir name from the env variable, and set up the palettes.
// This returns TRUE if it did so without error, and sets the member
// variable curPalette to 0. Else this returns FALSE and sets
// curPalette to -1.
//
// private
//
SbBool
SoXtMaterialList::setupPalettes()
//
////////////////////////////////////////////////////////////////////////
{
    // see if SO_MATERIAL_DIR has changed.
    char *envDir = getenv("SO_MATERIAL_DIR");
    if (envDir != NULL) {
	if ((strcmp(envDir, materialDir) == 0) && (curPalette != -1))
	    return TRUE; // env dir is same as current dir, palette is already OK
	    
	if (materialDir != NULL) free(materialDir);
        materialDir = strdup(envDir);
    }
    else if (curPalette != -1)
	return TRUE; // env dir not set, but palette is already OK
    
    // read the palette!
    curPalette = 0;

    // build the list of directories
    struct dirent *direntry;
    DIR  *dirp;
    char *f;
    if (dirp = opendir(materialDir)) {
    	char currentDir[MAXPATHLEN];
	getcwd(currentDir, MAXPATHLEN-1);
        chdir(materialDir);
        while (direntry = readdir(dirp)) {
	    f = direntry->d_name;
	    /* hide '.' files */
	    if (f[0] != '.') {
		if (isDirectory(f)) {
		    mtlPalettes.append(strdup(f));
		}
	    }
        }
        (void) closedir(dirp);
	chdir(currentDir); // back to our working directory
    }
    else {
#ifdef DEBUG
	SoDebugError::post("SoXtMaterialList::setupPalettes",
		"Cannot open directory %s.  Try setting the environment variable SO_MATERIAL_DIR to a directory which has material files in it.", materialDir);
#endif
	curPalette = -1;
    }
    
    return (curPalette != -1); // if not -1, return TRUE
}

////////////////////////////////////////////////////////////////////////
//
//    This builds the pulldown menu, using materialDir as the parent
// directory.
//
// usage: private
//
Widget
SoXtMaterialList::buildPulldownMenu(Widget parent)
//
////////////////////////////////////////////////////////////////////////
{
    Widget menubar, pulldown;
    Widget submenu;
    Arg    args[5];
    int    n;
    
    n = 0;
    XtSetArg(args[n], XmNuserData, this); n++;

    menubar = XmCreateMenuBar(parent, "menuBar", NULL, 0);

    pulldown = XmCreatePulldownMenu(menubar, "controlPulldown", args, n);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, pulldown); n++;
    submenu = XtCreateManagedWidget("Palettes",
				xmCascadeButtonGadgetClass,
				menubar, args, n);

#define MENU_ITEM(BUTTON,NAME,KONST) \
    n = 0; \
    XtSetArg(args[n], XmNuserData, this); n++; \
    XtSetArg(args[n], XmNindicatorType, XmONE_OF_MANY); n++; \
    BUTTON = XtCreateManagedWidget(NAME, \
    	    	xmToggleButtonGadgetClass, \
    	    	pulldown,args,n); \
    XtAddCallback(BUTTON, XmNvalueChangedCallback, \
	(XtCallbackProc) SoXtMaterialList::menuPick, \
	(XtPointer) KONST);


    // create the menu items ???pass in the dir names!
    Widget button;
    menuItems.truncate(0);
    for (int i = 0; i < mtlPalettes.getLength(); i++) {
    	char *dir = (char *) mtlPalettes[i];
	MENU_ITEM(button, dir, (unsigned long) i)
	menuItems.append(button); // store these buttons for future reference
    }
#undef MENU_ITEM
    
    if (menuItems.getLength() == 0) {
#ifdef DEBUG
	SoDebugError::post("SoXtMaterialList::buildPulldownMenu",
		"Directory %s has no material palettes.", materialDir);
#endif
    	curPalette = -1;
    }
    
    XtManageChild(submenu);
    
    // set the current palette toggle to on
    if (curPalette != -1)
	TOGGLE_ON(menuItems[curPalette]);

    return menubar;
}


////////////////////////////////////////////////////////////////////////
//
//  Fill in the Motif list with file names from the current palette.
//
// usage: virtual private
//
void
SoXtMaterialList::fillInMaterialList()
//
////////////////////////////////////////////////////////////////////////
{
    if (curPalette == -1)
    	return;
	
    XmString *table;
    int	   count;
    Arg    args[2];
    int    n;
    char curdir[256];
    
    // create a string table for the current palette directory
    sprintf(curdir, "%s/%s", materialDir, (char *)mtlPalettes[curPalette]);
    createStringTable(curdir, &table, &count);
    
    n = 0;
    XtSetArg(args[n], XmNitems, table); n++;
    XtSetArg(args[n], XmNitemCount, count); n++;
    XtSetValues(mtlList, args, n);

    /* free our local table - XmCreateScrolledList made a copy */
    destroyStringTable(table, count);
}

////////////////////////////////////////////////////////////////////////
//
//    This routine builds all the widgets, sets up callback routines,
//  and does the layout using motif.
//
// usage: protected
//
Widget
SoXtMaterialList::buildWidget(Widget parent)
//
////////////////////////////////////////////////////////////////////////
{
    // setup palette if needed. return NULL on error.
    if (! setupPalettes())
	return NULL;

    Widget  	menubar;
    int		n;
    Arg		wargs[10];

    // create the top level widget
    Widget widget = XtCreateWidget(getWidgetName(),xmFormWidgetClass,parent,NULL,0);

    // build the subcomponents
    menubar = buildPulldownMenu(widget);
    if (curPalette == -1) {
    	XtDestroyWidget(menubar);
    	XtDestroyWidget(widget);
	widget = NULL;
    	return NULL;
    }

    XmString *table;
    int	   count;
    char curdir[256];
    
    // create a string table for the current palette directory
    sprintf(curdir, "%s/%s", materialDir, (char *)mtlPalettes[curPalette]);
    createStringTable(curdir, &table, &count);

    // Layout
    n = 0;
    XtSetArg(wargs[n], XmNtopAttachment,    XmATTACH_FORM); n++;
    XtSetArg(wargs[n], XmNleftAttachment,   XmATTACH_FORM); n++;
    XtSetArg(wargs[n], XmNrightAttachment,  XmATTACH_FORM); n++;
    XtSetValues(menubar, wargs, n);

    //??? BUG IN LIST WIDGET - I cannot build the widget,  and then
    // call XtSetValues to set up the layout. I must do it all at once!
    // This seems to me like a bug in motif!
    n = 0;
XtSetArg(wargs[n], XmNitems, table); n++;
XtSetArg(wargs[n], XmNitemCount, count); n++;
XtSetArg(wargs[n], XmNvisibleItemCount, 8); n++;
XtSetArg(wargs[n], XmNselectionPolicy, XmSINGLE_SELECT); n++;
    XtSetArg(wargs[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(wargs[n], XmNleftAttachment,   XmATTACH_FORM); n++;
    XtSetArg(wargs[n], XmNrightAttachment,  XmATTACH_FORM); n++;
    XtSetArg(wargs[n], XmNtopAttachment,    XmATTACH_WIDGET); n++;
    XtSetArg(wargs[n], XmNtopWidget,        menubar); n++;
    //???XtSetValues(mtlList, wargs, n);
    
mtlList = XmCreateScrolledList(widget, "materialList", wargs, n);
XtAddCallback(mtlList,
    XmNsingleSelectionCallback,
    (XtCallbackProc) SoXtMaterialList::listPick,
    (XtPointer) this);
    
/* free our local table - XmCreateScrolledList made a copy */
destroyStringTable(table, count);



    /* manage those children! */
    XtManageChild(mtlList);
    XtManageChild(menubar);

    return widget;
}

////////////////////////////////////////////////////////////////////////
//
//  Called by Xt when a menu item is picked.
//
//  Use: static private
//
void
SoXtMaterialList::menuPick(Widget w, int id, XtPointer)
//
////////////////////////////////////////////////////////////////////////
{
    SoXtMaterialList *ml;
    Arg	args[1];
    
    XtSetArg(args[0], XmNuserData, &ml);
    XtGetValues(w, args, 1);

#ifdef DEBUG
    if (ml == NULL) {
	SoDebugError::post("SoXtMaterialList::menuPick",
		"ml is NULL");
	return;
    }
#endif

    if (id != ml->curPalette) {
	// toggle the previous current palette item to OFF
    	TOGGLE_OFF(ml->menuItems[ml->curPalette]);
	ml->curPalette = id;
	
	// and fill in the mtlList with the new current palette
	ml->fillInMaterialList();
    }
    
    // make sure the current palette item is set to ON
    TOGGLE_ON(ml->menuItems[ml->curPalette]);
}

////////////////////////////////////////////////////////////////////////
//
//  Called by Xt when a list item is picked.
//
//  Use: static private
//
void
SoXtMaterialList::listPick(Widget, SoXtMaterialList *ml, XtPointer p)
//
////////////////////////////////////////////////////////////////////////
{
    XmListCallbackStruct *listdata = (XmListCallbackStruct *) p;
    char *name;
    char filename[256];
    
    name = SoXt::decodeString(listdata->item);
    if (strcmp(name, bogusFile) == 0)
    	return;
    
    // construct the file name from all our information
    sprintf(filename, "%s/%s/%s",
    	ml->materialDir,
	(char *) ml->mtlPalettes[ml->curPalette],
	name);

    // read the file, search for SoMaterial, and pass data to callbacks
    SoInput in;
    SoNode  *root;
    if (in.openFile(filename)) {
	if (SoDB::read(&in, root)) {
	    root->ref();
	    
	    // find the material node
	    SoSearchAction sa;
	    SoFullPath *fullPath;
	
	    // Look for existing camera
	    sa.setType(SoMaterial::getClassTypeId());
	    sa.apply(root);
	
	    fullPath = (SoFullPath *) sa.getPath();
	    if (fullPath != NULL) {
	    	// invoke the callbacks with this material!
		fullPath->ref();
	    	SoMaterial *mtl = (SoMaterial *) fullPath->getTail();
		ml->callbackList->invokeCallbacks(mtl);
		fullPath->unref();
	    }
#ifdef DEBUG
	    else {
		SoDebugError::post("SoXtMaterialList::listPick",
		    "%s has no Material node.", filename);
	    }
#endif
	    
	    root->unref();
	}
#ifdef DEBUG
	else {
	    SoDebugError::post("SoXtMaterialList::listPick",
		    "Cannot read file %s.", filename);
	}
#endif
    }
#ifdef DEBUG
    else {
	SoDebugError::post("SoXtMaterialList::listPick",
		"Cannot open file %s.", filename);
    }
#endif
    
    free(name);
}

//
// redefine those generic virtual functions
//
const char *
SoXtMaterialList::getDefaultWidgetName() const
{ return "SoXtMaterialList"; }

const char *
SoXtMaterialList::getDefaultTitle() const
{ return "Material List"; }

const char *
SoXtMaterialList::getDefaultIconTitle() const
{ return "Mat List"; }

