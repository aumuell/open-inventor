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
 * Copyright (C) 1991-93   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Description:
 |	Component which displays a palette of materials. 
 |
 |   Classes: 	MyMaterialPalette
 |
 |   Author(s): Alain Dumesny
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef _SO_XT_MATERIAL_PALETTE_GIZMO_H_
#define _SO_XT_MATERIAL_PALETTE_GIZMO_H_

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Inventor/Xt/SoXtComponent.h>
#include <Inventor/misc/SoCallbackList.h>
#include <Inventor/SbPList.h>

class SoMaterial;
class SoSwitch;
class SoXtRenderArea;
class SoNode;
class MySimpleMaterialEditor;
class SoAction;
class SoXtInputFocus;
class SoXtClipboard;
class SoTranslation;
class SoPathList;

struct MaterialNameStruct;

// callback function prototypes
typedef void MyMaterialPaletteCB(void *userData, const SoMaterial *mtl);

//////////////////////////////////////////////////////////////////////////////
//
//  Class: MyMaterialPalette
//
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoXtMtlPalGiz
class MyMaterialPalette : public SoXtComponent {
  public:
    // pass the home directory of the material palettes as dir
    MyMaterialPalette(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE, 
	const char *dir = NULL);
   ~MyMaterialPalette();

    // deselect the currently selected item in the palette, if something
    // is selected. This should be called when the thing the palette
    // is affecting doesn't match any of the palette's choices (because
    // the palette is read only and not an editor).
    void    deselectCurrentItem();
    
    // Callbacks - register functions that will be called whenever the user
    // chooses a new material from the palette.
    // (This component cannot be attached to a database - it is read only)
    // C-api: name=addCB
    void    addCallback(
    	    	MyMaterialPaletteCB *f,
		void *userData = NULL)
	{ callbackList.addCallback((SoCallbackListCB *) f, userData);}

    // C-api: name=removeCB
    void    removeCallback(
    	    	MyMaterialPaletteCB *f,
		void *userData = NULL)
	{ callbackList.removeCallback((SoCallbackListCB *) f, userData); }
    
    // redefine these to also show/hide material editor
    virtual void    show();
    virtual void    hide();
    
  protected:

    // This constructor takes a boolean whether to build the widget now.
    // Subclasses can pass FALSE, then call MyMaterialPalette::buildWidget()
    // when they are ready for it to be built.
    SoEXTENDER
    MyMaterialPalette(
	Widget parent,
	const char *name, 
	SbBool buildInsideParent, 
	const char *dir, 
	SbBool buildNow);
    
    // redefine these
    virtual const char *    getDefaultWidgetName() const;
    virtual const char *    getDefaultTitle() const;
    virtual const char *    getDefaultIconTitle() const;
    
  private:
    char    	    *paletteDir;
    SoCallbackList  callbackList;
    SoXtRenderArea  *ra;
    Widget	    *widgetList;
    SbPList 	    paletteList;
    MaterialNameStruct *mtlNames;
    int	    	    curPalette;
    SoSwitch	    *itemSwitch;
    int		    selectedItem, currentItem;
    MySimpleMaterialEditor *matEditor;
    SoXtInputFocus  *focus;
    Time	    prevTime;
    SoTranslation   *overlayTrans1, *overlayTrans2;
    
    void	    createSceneGraph();
    void	    getPaletteNamesAndLoad();
    void	    loadPaletteItems();
    SoNode	    *getMaterialFromFile(char *file);
    SbBool	    handleEvent(XAnyEvent *);
    void	    updateMaterialName();
    void	    updateFileMenu();
    void	    updateEditMenu();
    void	    findCurrentItem(int x, int y);
    void	    updateOverlayFeedback();
    void	    updateWindowTitle();
    void	    createNewPalette(char *name);
    void	    savePalette();
    void	    savePaletteAs(char *name);
    void	    switchPalette();
    
    // dialog routines and vars
    SbBool	    paletteChanged;
    int		    whatToDoNext, nextPalette;
    void	    createSaveDialog();
    void	    createDeleteDialog(char *title, char *str1, char *str2);
    void	    createPromptDialog(char *title, char *str);
    static void	    saveDialogCB(Widget, MyMaterialPalette *, XmAnyCallbackStruct *);
    static void	    promptDialogCB(Widget, MyMaterialPalette *, XmAnyCallbackStruct *);
    static void	    deleteDialogCB(Widget, MyMaterialPalette *, XmAnyCallbackStruct *);
    
    // component callbacks
    static SbBool   raEventCB(void *, XAnyEvent *);
    static void	    matEditorCB(void *pt, MySimpleMaterialEditor *ed);
    
    // motif static callbacks
    static void	    menuCB(Widget, int, XmAnyCallbackStruct *);
    static void	    paletteMenuCB(Widget, int, void *);

  protected:   
    // Build routines
    Widget	    buildWidget(Widget parent);
    Widget	    buildMenu(Widget parent);
    void	    buildPaletteSubMenu();
    Widget	    buildPaletteMenuEntry(int id);
 
  private:   
    // cut/copy/paste/delete vars and functions
    SoXtClipboard   *clipboard;
    void	    deleteCurrentMaterial();
    static void	    pasteDone(void *, SoPathList *);
  
    // this is called by both constructors
    void constructorCommon(const char *dir, SbBool buildNow);
};

#endif // _SO_XT_MATERIAL_PALETTE_GIZMO_H_
