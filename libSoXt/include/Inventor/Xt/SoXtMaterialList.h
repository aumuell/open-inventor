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
 |	Component which displays a list of materials. 
 |
 |   Classes: 	SoXtMaterialList
 |
 |   Author(s): David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef _SO_XT_MATERIAL_LIST_H_
#define _SO_XT_MATERIAL_LIST_H_

#include <X11/Intrinsic.h>
#include <Inventor/SbBasic.h>
#include <Inventor/misc/SoCallbackList.h>
#include <Inventor/Xt/SoXtComponent.h>
#include <Inventor/nodes/SoMaterial.h>

// callback function prototypes
typedef void SoXtMaterialListCB(void *userData, const SoMaterial *mtl);

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoXtMaterialList
//
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoXtMtlList
class SoXtMaterialList : public SoXtComponent {
  public:
    // pass the home directory of the material palettes as dir
    SoXtMaterialList(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE, 
	const char *dir = NULL);
   ~SoXtMaterialList();

    // Callbacks - register functions that will be called whenever the user
    // chooses a new material from the list.
    // (This component cannot be attached to a database - it is read only)
    // C-api: name=addCB
    void    addCallback(
    	    	SoXtMaterialListCB *f,
		void *userData = NULL)
	{ callbackList->addCallback((SoCallbackListCB *) f, userData);}

    // C-api: name=removeCB
    void    removeCallback(
    	    	SoXtMaterialListCB *f,
		void *userData = NULL)
	{ callbackList->removeCallback((SoCallbackListCB *) f, userData); }
    
  protected:

    // This constructor takes a boolean whether to build the widget now.
    // Subclasses can pass FALSE, then call SoXtMaterialList::buildWidget()
    // when they are ready for it to be built.
    SoEXTENDER
    SoXtMaterialList(
	Widget parent,
	const char *name, 
	SbBool buildInsideParent, 
	const char *dir, 
	SbBool buildNow);
    
    // redefine these
    virtual const char *    getDefaultWidgetName() const;
    virtual const char *    getDefaultTitle() const;
    virtual const char *    getDefaultIconTitle() const;
    
    // Build routines
    Widget		buildWidget(Widget parent);
    Widget   	    	buildPulldownMenu(Widget parent);
    
  private:
    char    	    	*materialDir;	// the parent directory of materials
    SoCallbackList  	*callbackList;	// funcs to invoke when a mtl is picked
    SbPList 	    	menuItems;  	// Widgets
    SbPList 	    	mtlPalettes;	// list of palettes
    SbPList 	    	palette;    	// the current palette
    int	    	    	curPalette; 	// index into mtlPalettes
    Widget  	    	mtlList;	// the Motif list of materials

    // Fill up the mtlList with a new palette
    void    	    	fillInMaterialList();
    SbBool		setupPalettes();

    // Callback routines from Xt/Motif
    static void	    	menuPick(Widget, int, XtPointer);
    static void	    	listPick(Widget, SoXtMaterialList *, XtPointer);

    // this is called by both constructors
    void constructorCommon(const char *dir, SbBool buildNow);
};

#endif // _SO_XT_MATERIAL_LIST_H_
