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
 * Copyright (C) 1990,91,92   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Description:
 |	This file defines the tab plane dragger class.
 |	This dragger allows you to perform scales
 |	and translations, all in one tidy plane.
 |
 | NOTE TO DEVELOPERS:
 |     For info about the structure of SoTabPlaneDragger:
 |     [1] compile: /usr/share/src/Inventor/samples/ivNodeKitStructure
 |     [2] type:    ivNodeKitStructure SoTabPlaneDragger.
 |     [3] The program prints a diagram of the scene graph and a table with 
 |         information about each part.
 |
 |  The following parts in this dragger are created at construction time.
 |  'ResourceName' corresponds to the name of the default geometry for the
 |  part. The dragger's constructor gets the scene graph for 'ResourceName'
 |  by querying the global dictionary ( SoDB::getByName("ResourceName"); ).  
 |
 |  Resource Name:                           Part Name:
 |
 |  tabPlaneTranslator                     - translator
 |  tabPlaneScaleTabMaterial               - scaleTabMaterial
 |  tabPlaneScaleTabHints                  - scaleTabHints
 |
 |
 |   Author(s): Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_TAB_PLANE_DRAGGER_
#define  _SO_TAB_PLANE_DRAGGER_

#include <Inventor/SbLinear.h>
#include <Inventor/sensors/SoSensor.h>
#include <Inventor/projectors/SbLineProjector.h>
#include <Inventor/projectors/SbPlaneProjector.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/draggers/SoDragger.h>

class SoNode;
class SoCoordinate3;
class SoGLRenderAction;
class SoRayPickAction;
class SoFieldSensor;

// C-api: prefix=SoTabPlnDrag
// C-api: public=translation,scaleFactor
class SoTabPlaneDragger : public SoDragger
{
    // Define typeId and name stuff
    SO_KIT_HEADER(SoTabPlaneDragger);

    SO_KIT_CATALOG_ENTRY_HEADER(planeSwitch);
    SO_KIT_CATALOG_ENTRY_HEADER(translator);
    SO_KIT_CATALOG_ENTRY_HEADER(scaleTabs);
    SO_KIT_CATALOG_ENTRY_HEADER(scaleTabMaterial);
    SO_KIT_CATALOG_ENTRY_HEADER(scaleTabHints);
    SO_KIT_CATALOG_ENTRY_HEADER(scaleTabMaterialBinding);
    SO_KIT_CATALOG_ENTRY_HEADER(scaleTabNormalBinding);
    SO_KIT_CATALOG_ENTRY_HEADER(scaleTabNormal);
    SO_KIT_CATALOG_ENTRY_HEADER(edgeScaleCoords);
    SO_KIT_CATALOG_ENTRY_HEADER(edgeScaleTab0);
    SO_KIT_CATALOG_ENTRY_HEADER(edgeScaleTab1);
    SO_KIT_CATALOG_ENTRY_HEADER(edgeScaleTab2);
    SO_KIT_CATALOG_ENTRY_HEADER(edgeScaleTab3);
    SO_KIT_CATALOG_ENTRY_HEADER(cornerScaleCoords);
    SO_KIT_CATALOG_ENTRY_HEADER(cornerScaleTab0);
    SO_KIT_CATALOG_ENTRY_HEADER(cornerScaleTab1);
    SO_KIT_CATALOG_ENTRY_HEADER(cornerScaleTab2);
    SO_KIT_CATALOG_ENTRY_HEADER(cornerScaleTab3);

  public:
    // Constructor
    SoTabPlaneDragger();

    SoSFVec3f    translation;
    SoSFVec3f    scaleFactor;

    // Cause the scale tabs size to be re-adjusted. Happens automatically
    // upon dragger finish. You may want to do it upon viewer finish as 
    // well.
    void    adjustScaleTabSize();

  SoINTERNAL public:
    static void initClass();  // initialize the class

  protected:

    static void startCB( void *, SoDragger *);
    static void motionCB( void *, SoDragger *);
    static void finishCB( void *, SoDragger *);
    static void	metaKeyChangeCB( void *, SoDragger *);

    SbLineProjector	*lineProj;
    SbPlaneProjector	*planeProj;

    SoFieldSensor *translFieldSensor;
    SoFieldSensor *scaleFieldSensor;
    static void   fieldSensorCB( void *, SoSensor * );
    static void valueChangedCB( void *, SoDragger * );

    virtual void    GLRender( SoGLRenderAction *action );

    void 	dragStart();
    void 	drag();
    void 	dragFinish();

    void 	translateStart();
    void 	translateDrag();
    
    void 	edgeScaleStart();
    void 	edgeScaleDrag();
    
    void 	cornerScaleStart();
    void 	cornerScaleDrag();
    
    void 	scaleUniformStart();
    void 	scaleUniformDrag();

    SbVec3f           worldRestartPt;
    int		      currentScalePatch;

    enum State {
	    INACTIVE, TRANSLATING, EDGE_SCALING, CORNER_SCALING, UNIFORM_SCALING
    };
    SbBool              shftDown;

    State		currentState;
    State		restartState;
    int			translateDir;

    SbVec3f             scaleCenter;

    SbBool		needScaleTabAdjustment;
    void		reallyAdjustScaleTabSize(SoGLRenderAction *action);

    void getXYScreenLengths( SbVec2f &lengths, const SbMatrix &localToScreen, 
				const SbVec2s &winSize);

    // detach/attach any sensors, callbacks, and/or field connections.
    // Called by:            start/end of SoBaseKit::readInstance
    // and on new copy by:   start/end of SoBaseKit::copy.
    // Classes that redefine must call setUpConnections(TRUE,TRUE) 
    // at end of constructor.
    // Returns the state of the node when this was called.
    virtual SbBool setUpConnections( SbBool onOff, SbBool doItAlways = FALSE );

    virtual void setDefaultOnNonWritingFields();

    virtual ~SoTabPlaneDragger();

  private:
    static const char geomBuffer[];
};    

#endif  /* _SO_TAB_PLANE_DRAGGER_ */
