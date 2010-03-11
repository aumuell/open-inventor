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
 |	Simplified Material Editor.
 |
 |   Classes	: MySimpleMaterialEditor
 |
 |   Author(s)	: Alain Dumesny
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef _MY_MATERIAL_EDITOR_
#define _MY_MATERIAL_EDITOR_

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Inventor/Xt/SoXtComponent.h>
#include <Inventor/misc/SoCallbackList.h>
#include <Inventor/SbColor.h>

class SoMaterial;
class SoXtRenderArea;
class MyColorWheel;
class MyColorSlider;
class SoXtClipboard;
class SoPathList;
class MySimpleMaterialEditor;


// callback function prototypes
typedef void MySimpleMaterialEditorCB(void *userData, 
			    MySimpleMaterialEditor *editor);

//////////////////////////////////////////////////////////////////////////////
//
//  Class: MySimpleMaterialEditor
//
//  This editor Gizmo lets you interactively edit a material
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoXtMtlEd
class MySimpleMaterialEditor : public SoXtComponent {
  public:
    MySimpleMaterialEditor(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE, 
	SbBool showMaterialName = FALSE);
   ~MySimpleMaterialEditor();
    
    // sets/gets the current material
    // C-api: name=setMtl
    void    	    	setMaterial(const SoMaterial *mtl);
    // C-api: name=getMtl
    const SoMaterial *	getMaterial() const	    { return material; }
    
    // specify whether the material name is being displayed
    // as an editable text field. The user can then use the set/get
    // material name methods to display and retreive the name.
    // (default set in constructor to FALSE).
    //
    // C-api: name=isMtlNameVisible
    SbBool		isMaterialNameVisible()   { return nameVisible; }
    
    // set/gets the material name which will be displayed (if set to
    // be visible) in the text field.
    // C-api: name=setMtlName
    void    	    	setMaterialName(const char *name);
    // C-api: name=getMtlName
    const char *	getMaterialName() const	    { return materialName; }
    
    // Callbacks - register functions that will be called whenever the user
    // chooses a new material (accept button)
    // C-api: name=addCB
    void    addCallback(
    	    	MySimpleMaterialEditorCB *f,
		void *userData = NULL)
	{ callbackList.addCallback((SoCallbackListCB *) f, userData);}
    
    // C-api: name=removeCB
    void    removeCallback(
    	    	MySimpleMaterialEditorCB *f,
		void *userData = NULL)
	{ callbackList.removeCallback((SoCallbackListCB *) f, userData); }
    
  protected:

    // This constructor takes a boolean whether to build the widget now.
    // Subclasses can pass FALSE, then call MySimpleMaterialEditor::buildWidget()
    // when they are ready for it to be built.
    SoEXTENDER
    MySimpleMaterialEditor(
	Widget parent,
	const char *name, 
	SbBool buildInsideParent, 
	SbBool showMaterialName, 
	SbBool buildNow);
    
    // redefine these
    virtual const char *    getDefaultWidgetName() const;
    virtual const char *    getDefaultTitle() const;
    virtual const char *    getDefaultIconTitle() const;

    // build routines
    Widget	    buildWidget(Widget parent);
       
  private:
    SoCallbackList  callbackList;
    SbBool	    ignoreCallback;
    SbBool	    fieldChanged;
    SoMaterial	    *material, *savedMaterial;
    char	    *materialName, *savedMaterialName;
    SoXtRenderArea  *renderArea;
    MyColorWheel  *colWheel;
    MyColorSlider *colSlider;
    Widget	    sldWidgets[3], nameField;
    float	    metalness, savedMetalness, smoothness, savedSmoothness;
    SbColor	    baseColor, savedBaseColor;
    SoXtClipboard   *clipboard;
    SbBool	    nameVisible;
    
    void	    copyMaterial(SoMaterial *mat1, const SoMaterial *mat2);
    void	    updateMaterial();
    void	    calculateMaterialFactors();
    void	    updateMaterialUI();
    void	    saveMaterialFactors();
    
    static void	    colWheelCB(void *, const float hsv[3]);
    static void	    colSliderCB(void *, float v);
    static void	    nameFieldCB(Widget, MySimpleMaterialEditor *, void *);
    static void	    fieldChangedCB(Widget, MySimpleMaterialEditor *, void *);
    static void	    sldWidgetsCB(Widget, MySimpleMaterialEditor *, void *);
    static void	    applyCB(Widget, MySimpleMaterialEditor *, void *);
    static void	    resetCB(Widget, MySimpleMaterialEditor *, void *);
    static void	    copyCB(Widget, MySimpleMaterialEditor *, XmAnyCallbackStruct *);
    static void	    pasteCB(Widget, MySimpleMaterialEditor *, XmAnyCallbackStruct *);
    static void	    pasteDone(void *, SoPathList *);
  
    // this is called by both constructors
    void constructorCommon(SbBool showMaterialName, SbBool buildNow);
};


#endif // _MY_MATERIAL_EDITOR_
