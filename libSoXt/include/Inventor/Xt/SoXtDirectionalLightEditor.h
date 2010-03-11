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
 |	Xt component (widget) for editing directional lights; includes
 |	color, intensity, and direction editting. 
 |
 |   Classes: 	SoXtDirectionalLightEditor
 |
 |   Author(s): David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef _SO_XT_DIRECTIONAL_LIGHT_EDITOR_
#define _SO_XT_DIRECTIONAL_LIGHT_EDITOR_

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Inventor/SbBasic.h>
#include <Inventor/SbColor.h>
#include <Inventor/misc/SoCallbackList.h>
#include <Inventor/Xt/SoXtComponent.h>


class SoBase;
class SoCamera;
class SoDirectionalLight;
class SoDirectionalLightManip;
class SoDragger;
class SoGroup;
class SoNodeSensor;
class SoPath;
class SoPathList;
class SoPerspectiveCamera;
class SoSensor;
class SoSeparator;
class SoXtClipboard;
class _SoXtColorEditor;
class _SoXtColorSlider;
class SoXtRenderArea;


// callback function prototypes
typedef void SoXtDirectionalLightEditorCB(void *userData, const SoDirectionalLight *light);

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoXtDirectionalLightEditor
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoXtDirLtEd
class SoXtDirectionalLightEditor : public SoXtComponent {
  public:
    SoXtDirectionalLightEditor(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE);
   ~SoXtDirectionalLightEditor();

    // Edit a directional light node - 
    void		attach(SoPath *pathToLight);
    void		detach();
    SbBool		isAttached()	{ return (dirLight != NULL); }
    
    // Set new values in the light editor
    // C-api: name=setLt
    void    	    	setLight(const SoDirectionalLight &newLight);
    // C-api: name=getLt
    const SoDirectionalLight &getLight() const { return *dirLight; }
    
    //
    // Additional way of using the material editor, by registering a callback
    // and setting the material. At the time dictated by setUpdateFrequency
    // the callbacks will be called with the new material.
    //
    // C-api: name=addLtChangedCB
    inline void		addLightChangedCallback(
				SoXtDirectionalLightEditorCB *f, 
				void *userData = NULL);
    // C-api: name=removeLtChangedCB
    inline void		removeLightChangedCallback(
				SoXtDirectionalLightEditorCB *f, 
				void *userData = NULL);

    // Redefined here since there are two windows to deal with -
    // the color editor and the light manipulator
    virtual void    	show();
    virtual void    	hide();

  protected:

    // This constructor takes a boolean whether to build the widget now.
    // Subclasses can pass FALSE, then call SoXtDirectionalLightEditor::buildWidget()
    // when they are ready for it to be built.
    SoEXTENDER
    SoXtDirectionalLightEditor(
	Widget parent,
	const char *name, 
	SbBool buildInsideParent, 
	SbBool buildNow);

    // redefine these
    virtual const char *    getDefaultWidgetName() const;
    virtual const char *    getDefaultTitle() const;
    virtual const char *    getDefaultIconTitle() const;
    
    SoDirectionalLight	*dirLight;  	    // light we are editing
    SoSeparator	    	*root;	    	    // root of local scene graph
    SoSeparator	    	*litStuff;    	    // what's on display under thelight

    SoPerspectiveCamera *myCamera;
    SoCamera            *cameraToWatch;
    
    // components, manips, sensors
    _SoXtColorEditor	*colorEditor;
    _SoXtColorSlider	*intensitySlider;
    SoXtRenderArea      *renderArea;    
    SoNodeSensor    	*lightSensor;
    SoNodeSensor    	*cameraSensor;
    SoDirectionalLightManip 	*dirLightManip;
    static char		*geomBuffer;
    SbBool  	    	ignoreCallback; // TRUE while callback should be ignored
    SoCallbackList	*callbackList;
    
    // Copy/paste support
    SoXtClipboard	*clipboard;
    
    // copy the src light values to the dst light values
    void		copyLight(SoDirectionalLight *dst, 
			    const SoDirectionalLight *src);
			    
    void    	    	updateLocalComponents();

  private:
    // Callback routines from components and manipulators
    static void		colorEditorCB(void *, const SbColor *);
    static void		intensitySliderCB(void *, float);
    static void	    	pasteDoneCB(void *, SoPathList *);
    static void		dirLightManipCB(void *, SoDragger *);

    // Callback routines from Xt/Motif
    static void	    	menuPick(Widget, int, XmAnyCallbackStruct *);
    
    // Sensor callbacks
    static void	    	cameraSensorCB(SoXtDirectionalLightEditor *, SoSensor *);
    static void	    	lightSensorCB(SoXtDirectionalLightEditor *, SoSensor *);

    // Build routines
    Widget		buildWidget(Widget parent);
    Widget   	    	buildPulldownMenu(Widget parent);

    static void visibilityChangeCB(void *pt, SbBool visible);
    void        activate();		// connects the sensor
    void        deactivate();		// disconnects the sensor
    
    // this is called by both constructors
    void constructorCommon(SbBool buildNow);
};

// Inline functions
void
SoXtDirectionalLightEditor::addLightChangedCallback(
    SoXtDirectionalLightEditorCB *f, 
    void *userData)
{ callbackList->addCallback((SoCallbackListCB *) f, userData); }

void
SoXtDirectionalLightEditor::removeLightChangedCallback(
    SoXtDirectionalLightEditorCB *f, 
    void *userData)
{ callbackList->removeCallback((SoCallbackListCB *) f, userData); }

#endif // _SO_XT_DIRECTIONAL_LIGHT_EDITOR_
