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
 |	Component for editing materials. 
 |
 |   Classes	: SoXtMaterialEditor
 |
 |   Author(s)	: Alain Dumesny, David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef _SO_XT_MATERIAL_EDITOR_
#define _SO_XT_MATERIAL_EDITOR_

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Inventor/SbBasic.h>
#include <Inventor/misc/SoCallbackList.h>
#include <Inventor/Xt/SoXtComponent.h>

class SoBaseColor;
class SoXtClipboard;
class _SoXtColorEditor;
class SoNodeSensor;
class SoSensor;
class SoDirectionalLight;
class SoMaterial;
class SoXtMaterialList;
class SoPathList;
class SoXtRenderArea;
class SoSelection;
class _SoXtColorSlider;
class SoMFColor;
class SbColor;
class SoBase;
class SoSeparator;

// callback function prototypes
typedef void SoXtMaterialEditorCB(void *userData, const SoMaterial *mtl);

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoXtMaterialEditor
//
//  This editor lets you interactively edit a material
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoXtMtlEd
class SoXtMaterialEditor : public SoXtComponent {
  public:
    // UpdateFrequency is how often new values should be sent
    // to the node or the callback routine.
    enum UpdateFrequency {
	CONTINUOUS,	// send updates with every mouse motion
	AFTER_ACCEPT 	// only send updates after user hits accept button
    };
    
    SoXtMaterialEditor(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE);
   ~SoXtMaterialEditor();
    
    // attach the editor to the given node and edits the material
    // of the given index (default is the first material)
    void		attach(SoMaterial *material, int index = 0);
    void		detach();
    SbBool		isAttached()	{ return (material != NULL); }
    
    //
    // Additional way of using the material editor, by registering a callback
    // and setting the material. At the time dictated by setUpdateFrequency
    // the callbacks will be called with the new material.
    //
    // C-api: name=addMtlChangedCB
    inline void		addMaterialChangedCallback(
				SoXtMaterialEditorCB *f, 
				void *userData = NULL);
    // C-api: name=removeMtlChangedCB
    inline void		removeMaterialChangedCallback(
				SoXtMaterialEditorCB *f, 
				void *userData = NULL);
    
    //
    // Set/get the update frequency of when materialChanged callbacks should 
    // be called. (default CONTINUOUS).
    //
    // C-api: name=setUpdateFreq
    void	setUpdateFrequency(SoXtMaterialEditor::UpdateFrequency freq);
    // C-api: name=getUpdateFreq
    SoXtMaterialEditor::UpdateFrequency getUpdateFrequency()
						    { return updateFreq; }
    
    // Redefine these since there are multiple windows -
    // the color editor, directional light editor, and this component
    virtual void    	show();
    virtual void    	hide();
    
    // Set new values for the material editor, or get current values
    // C-api: name=setMtl
    void    	    	setMaterial(const SoMaterial &mtl);
    // C-api: name=getMtl
    const SoMaterial &	getMaterial() const { return *localMaterial; }
    
  protected:

    // This constructor takes a boolean whether to build the widget now.
    // Subclasses can pass FALSE, then call SoXtMaterialEditor::buildWidget()
    // when they are ready for it to be built.
    SoEXTENDER
    SoXtMaterialEditor(
	Widget parent,
	const char *name, 
	SbBool buildInsideParent, 
	SbBool buildNow);
    
    Widget		    buildWidget(Widget parent);
    
    // redefine these
    virtual const char *    getDefaultWidgetName() const;
    virtual const char *    getDefaultTitle() const;
    virtual const char *    getDefaultIconTitle() const;
    
  private:
    // attach vars
    SoMaterial		*material;  // material we are editing
    int			index;	    // material index number
    SoNodeSensor    	*sensor;
    SoCallbackList	*callbackList;
    Widget  	    	acceptButton, diamondButtons[4], radioButtons[4];
    SoXtMaterialEditor::UpdateFrequency	updateFreq;
    
    // widgets to edit the material
    Widget		mgrWidget;	// topmost widget
    _SoXtColorEditor	*colorEditor;
    _SoXtColorSlider	*sliders[6];
    SbBool              changedIt[6];
    SoXtMaterialList  	*materialList;
    SbBool  	    	ignoreCallback; // TRUE while callback should be ignored
    SbBool		openMaterialList;
    static void     	colorEditorCloseCB(void *, SoXtComponent *);
    static void     	materialListCloseCB(void *, SoXtComponent *);
    
    // copy/paste support
    SoXtClipboard	*clipboard;
    
    // local scene vars
    SoXtRenderArea    	*renderArea;
    SoMaterial		*localMaterial;	    // local copy of the material
    SoDirectionalLight	*light0;
    SoDirectionalLight	*light1;
    SoBaseColor	    	*tileColor;
    SoSeparator	    	*root;
    
    // keep the menu items around so we can update them before menu display
    Widget  	    	*menuItemsList;
    
    int32_t	    	activeColor;	    // field which color editor edits
    
    // update the sliders/colorEditor based on the local material
    void    	    	updateLocalComponents();
    
    // update the colorEditor based on the activeColor flag
    void    	    	updateColorEditor(SbBool updateTitle = FALSE);
    
    // update a color slider (amb/diff/spec/emiss) based of a material color
    // (split the base color from the intensity).
    void    	    	updateColorSlider(_SoXtColorSlider *, const float rgb[3]);
    
    // update a material field when a color slider changes.
    void    	    	updateMaterialColor(
    	    	    	    SoMFColor	*editMtlColor,
			    SoMFColor	*localMtlColor,
			    const float	*rgb,
			    float   	intensity);
    
    // copies mat2 onto mat1
    void    	    	copyMaterial(SoMaterial *mat1, int index1, 
				    const SoMaterial *mat2, int index2);
    
    // For each of the 6 sliders (or sets of sliders) sets the ignore flag of 
    // the material node being editted to FALSE if it has been changed.
    void		undoIgnoresIfChanged();
        
    // copy/paste support
    static void    	pasteDone(void *userData, SoPathList *pathList);

    // Callback routines from components
    static void	    	materialListCB(void *, const SoMaterial *);
    static void		colorEditorCB(void *, const SbColor *);
    static void		ambientSliderCB(void *, float);
    static void		diffuseSliderCB(void *, float);
    static void		specularSliderCB(void *, float);
    static void		emissiveSliderCB(void *, float);
    static void		shininessSliderCB(void *, float);
    static void		transparencySliderCB(void *, float);
    
    // Callback routines from Xt/Motif
    static void	    	menuPick(Widget, int, XmAnyCallbackStruct *);
    static void	    	menuDisplay(Widget, SoXtMaterialEditor *, XtPointer);
    static void	    	radioButtonPick(Widget, int, XtPointer);
    static void	    	diamondButtonPick(Widget, int, XtPointer);
    static void     	acceptButtonCB(Widget, SoXtMaterialEditor *, XtPointer);
    
    // Sensor callbacks
    static void	    	sensorCB(void *, SoSensor *);

    // Build routines
    Widget   	    	buildPulldownMenu(Widget parent);
    Widget  	    	buildControls(Widget parent);
    Widget  	    	buildSlidersForm(Widget parent);
    
    static void visibilityChangeCB(void *pt, SbBool visible);
    void        activate();		// connects the sensor
    void        deactivate();		// disconnects the sensor

    // this is called by both constructors
    void constructorCommon(SbBool buildNow);
};


// Inline functions
void
SoXtMaterialEditor::addMaterialChangedCallback(
    SoXtMaterialEditorCB *f, 
    void *userData)
{ callbackList->addCallback((SoCallbackListCB *) f, userData); }

void
SoXtMaterialEditor::removeMaterialChangedCallback(
    SoXtMaterialEditorCB *f, 
    void *userData)
{ callbackList->removeCallback((SoCallbackListCB *) f, userData); }

#endif // _SO_XT_MATERIAL_EDITOR_


