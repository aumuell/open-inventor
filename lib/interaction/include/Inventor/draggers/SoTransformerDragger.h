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
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   Description:
 |	This file defines the SoTransformerDragger class.
 |
 | NOTE TO DEVELOPERS:
 |     For info about the structure of SoTransformerDragger:
 |     [1] compile: /usr/share/src/Inventor/samples/ivNodeKitStructure
 |     [2] type:    ivNodeKitStructure SoTransformerDragger.
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
 |      transformerOverallStyle              - overallStyle
 |
 |  NOTE: for the translators and rotators, 
 |        numbering refers to a cube-face:
 |        1=top,2=bot,3=lft,4=rit,5=fnt,6=bak
 |
 |	transformerTranslator1               - translator1
 |	transformerTranslator1Active         - translator1Active
 |      (ditto for translator2-translator6)
 |
 |      transformerRotator1                 - rotator1
 |      transformerRotator1Active           - rotator1Active
 |      (ditto for rotator2-rotator6)
 |
 |  
 |  NOTE: for the scalers, 
 |        numbers refer to a corner: 
 |        1=+x+y+z,  2=+x+y-z, 3=+x-y+z, 
 |        4=+x-y-z,  5=-x+y+z,  6=-x+y-z, 
 |        7=-x-y+z, 8=-x-y-z
 |      transformerScale1		   - scale1
 |      transformerScale1Active            - scale1Active
 |      (ditto for scale2-scale8)
 |
 |  Feedback for translation 
 |      transformerTranslateBoxFeedback             - translateBoxFeedback
 |
 |  Feedback for scaling 
 |      transformerScaleBoxFeedback             - scaleBoxFeedback
 |
 |  Feedback for <Control> + scaling
 |      transformerPosXWallFeedback                 - posXWallFeedback
 |      transformerPosYWallFeedback                 - posYWallFeedback
 |      transformerPosZWallFeedback                 - posZWallFeedback
 |      transformerNegXWallFeedback                 - negXWallFeedback
 |      transformerNegYWallFeedback                 - negYWallFeedback
 |      transformerNegZWallFeedback                 - negZWallFeedback
 |
 |  Feedback for <Control> + rotating
 |      transformerPosXRoundWallFeedback                 - posXRoundWallFeedback
 |      transformerPosYRoundWallFeedback                 - posYRoundWallFeedback
 |      transformerPosZRoundWallFeedback                 - posZRoundWallFeedback
 |      transformerNegXRoundWallFeedback                 - negXRoundWallFeedback
 |      transformerNegYRoundWallFeedback                 - negYRoundWallFeedback
 |      transformerNegZRoundWallFeedback                 - negZRoundWallFeedback
 |
 |  Feedback for translation and for 1D scaling
 |      transformerXAxisFeedbackActive              - xAxisFeedbackActive
 |      transformerYAxisFeedbackActive              - yAxisFeedbackActive
 |      transformerZAxisFeedbackActive              - zAxisFeedbackActive
 |      transformerXAxisFeedbackSelect              - xAxisFeedbackSelect
 |      transformerYAxisFeedbackSelect              - yAxisFeedbackSelect
 |      transformerZAxisFeedbackSelect              - zAxisFeedbackSelect
 |
 |  Feedback for <Control> + translation:
 |      transformerXCrosshairFeedback                    - xCrosshairFeedback
 |      transformerYCrosshairFeedback                    - yCrosshairFeedback
 |      transformerZCrosshairFeedback                    - zCrosshairFeedback
 |
 |  Feedback for uniform scaling
 |      transformerRadialFeedback                  - radialFeedback
 |
 |  Feedback for 1D rotation
 |      transformerXCircleFeedback                    - xCircleFeedback
 |      transformerYCircleFeedback                    - yCircleFeedback
 |      transformerZCircleFeedback                    - zCircleFeedback
 |
 |   Author(s): Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_TRANSFORMER_DRAGGER_
#define  _SO_TRANSFORMER_DRAGGER_

#include <Inventor/SoLists.h>
#include <Inventor/draggers/SoDragger.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFRotation.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/sensors/SoSensor.h>

class SbDict;
class SbPlaneProjector;
class SbLineProjector;
class SbSphereSectionProjector;
class SbCylinderPlaneProjector;
class SoFieldSensor;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoTransformerDragger
//
//  SoTransformerDragger - allows user to transform objects.
//
//////////////////////////////////////////////////////////////////////////////

class SoTransformerDragger : public SoDragger {

    SO_KIT_HEADER(SoTransformerDragger);

    SO_KIT_CATALOG_ENTRY_HEADER(overallStyle);

    // For making the dragger surround what lies above it.
    SO_KIT_CATALOG_ENTRY_HEADER(surroundScale);

    SO_KIT_CATALOG_ENTRY_HEADER(translatorSep);
    SO_KIT_CATALOG_ENTRY_HEADER(translator1Switch);
    SO_KIT_CATALOG_ENTRY_HEADER(translator1LocateGroup);
    SO_KIT_CATALOG_ENTRY_HEADER(translator1);
    SO_KIT_CATALOG_ENTRY_HEADER(translator1Active);
    SO_KIT_CATALOG_ENTRY_HEADER(translator2Switch);
    SO_KIT_CATALOG_ENTRY_HEADER(translator2LocateGroup);
    SO_KIT_CATALOG_ENTRY_HEADER(translator2);
    SO_KIT_CATALOG_ENTRY_HEADER(translator2Active);
    SO_KIT_CATALOG_ENTRY_HEADER(translator3Switch);
    SO_KIT_CATALOG_ENTRY_HEADER(translator3LocateGroup);
    SO_KIT_CATALOG_ENTRY_HEADER(translator3);
    SO_KIT_CATALOG_ENTRY_HEADER(translator3Active);
    SO_KIT_CATALOG_ENTRY_HEADER(translator4Switch);
    SO_KIT_CATALOG_ENTRY_HEADER(translator4LocateGroup);
    SO_KIT_CATALOG_ENTRY_HEADER(translator4);
    SO_KIT_CATALOG_ENTRY_HEADER(translator4Active);
    SO_KIT_CATALOG_ENTRY_HEADER(translator5Switch);
    SO_KIT_CATALOG_ENTRY_HEADER(translator5LocateGroup);
    SO_KIT_CATALOG_ENTRY_HEADER(translator5);
    SO_KIT_CATALOG_ENTRY_HEADER(translator5Active);
    SO_KIT_CATALOG_ENTRY_HEADER(translator6Switch);
    SO_KIT_CATALOG_ENTRY_HEADER(translator6LocateGroup);
    SO_KIT_CATALOG_ENTRY_HEADER(translator6);
    SO_KIT_CATALOG_ENTRY_HEADER(translator6Active);


    SO_KIT_CATALOG_ENTRY_HEADER(rotatorSep);
    SO_KIT_CATALOG_ENTRY_HEADER(rotator1Switch);
    SO_KIT_CATALOG_ENTRY_HEADER(rotator1LocateGroup);
    SO_KIT_CATALOG_ENTRY_HEADER(rotator1);
    SO_KIT_CATALOG_ENTRY_HEADER(rotator1Active);
    SO_KIT_CATALOG_ENTRY_HEADER(rotator2Switch);
    SO_KIT_CATALOG_ENTRY_HEADER(rotator2LocateGroup);
    SO_KIT_CATALOG_ENTRY_HEADER(rotator2);
    SO_KIT_CATALOG_ENTRY_HEADER(rotator2Active);
    SO_KIT_CATALOG_ENTRY_HEADER(rotator3Switch);
    SO_KIT_CATALOG_ENTRY_HEADER(rotator3LocateGroup);
    SO_KIT_CATALOG_ENTRY_HEADER(rotator3);
    SO_KIT_CATALOG_ENTRY_HEADER(rotator3Active);
    SO_KIT_CATALOG_ENTRY_HEADER(rotator4Switch);
    SO_KIT_CATALOG_ENTRY_HEADER(rotator4LocateGroup);
    SO_KIT_CATALOG_ENTRY_HEADER(rotator4);
    SO_KIT_CATALOG_ENTRY_HEADER(rotator4Active);
    SO_KIT_CATALOG_ENTRY_HEADER(rotator5Switch);
    SO_KIT_CATALOG_ENTRY_HEADER(rotator5LocateGroup);
    SO_KIT_CATALOG_ENTRY_HEADER(rotator5);
    SO_KIT_CATALOG_ENTRY_HEADER(rotator5Active);
    SO_KIT_CATALOG_ENTRY_HEADER(rotator6Switch);
    SO_KIT_CATALOG_ENTRY_HEADER(rotator6LocateGroup);
    SO_KIT_CATALOG_ENTRY_HEADER(rotator6);
    SO_KIT_CATALOG_ENTRY_HEADER(rotator6Active);

    SO_KIT_CATALOG_ENTRY_HEADER(scaleSep);
    SO_KIT_CATALOG_ENTRY_HEADER(scale1Switch);
    SO_KIT_CATALOG_ENTRY_HEADER(scale1LocateGroup);
    SO_KIT_CATALOG_ENTRY_HEADER(scale1);
    SO_KIT_CATALOG_ENTRY_HEADER(scale1Active);
    SO_KIT_CATALOG_ENTRY_HEADER(scale2Switch);
    SO_KIT_CATALOG_ENTRY_HEADER(scale2LocateGroup);
    SO_KIT_CATALOG_ENTRY_HEADER(scale2);
    SO_KIT_CATALOG_ENTRY_HEADER(scale2Active);
    SO_KIT_CATALOG_ENTRY_HEADER(scale3Switch);
    SO_KIT_CATALOG_ENTRY_HEADER(scale3LocateGroup);
    SO_KIT_CATALOG_ENTRY_HEADER(scale3);
    SO_KIT_CATALOG_ENTRY_HEADER(scale3Active);
    SO_KIT_CATALOG_ENTRY_HEADER(scale4Switch);
    SO_KIT_CATALOG_ENTRY_HEADER(scale4LocateGroup);
    SO_KIT_CATALOG_ENTRY_HEADER(scale4);
    SO_KIT_CATALOG_ENTRY_HEADER(scale4Active);
    SO_KIT_CATALOG_ENTRY_HEADER(scale5Switch);
    SO_KIT_CATALOG_ENTRY_HEADER(scale5LocateGroup);
    SO_KIT_CATALOG_ENTRY_HEADER(scale5);
    SO_KIT_CATALOG_ENTRY_HEADER(scale5Active);
    SO_KIT_CATALOG_ENTRY_HEADER(scale6Switch);
    SO_KIT_CATALOG_ENTRY_HEADER(scale6LocateGroup);
    SO_KIT_CATALOG_ENTRY_HEADER(scale6);
    SO_KIT_CATALOG_ENTRY_HEADER(scale6Active);
    SO_KIT_CATALOG_ENTRY_HEADER(scale7Switch);
    SO_KIT_CATALOG_ENTRY_HEADER(scale7LocateGroup);
    SO_KIT_CATALOG_ENTRY_HEADER(scale7);
    SO_KIT_CATALOG_ENTRY_HEADER(scale7Active);
    SO_KIT_CATALOG_ENTRY_HEADER(scale8Switch);
    SO_KIT_CATALOG_ENTRY_HEADER(scale8LocateGroup);
    SO_KIT_CATALOG_ENTRY_HEADER(scale8);
    SO_KIT_CATALOG_ENTRY_HEADER(scale8Active);

    SO_KIT_CATALOG_ENTRY_HEADER(axisFeedbackSep);
    SO_KIT_CATALOG_ENTRY_HEADER(axisFeedbackLocation);
    SO_KIT_CATALOG_ENTRY_HEADER(xAxisFeedbackSwitch);
    SO_KIT_CATALOG_ENTRY_HEADER(xAxisFeedbackActive);
    SO_KIT_CATALOG_ENTRY_HEADER(xAxisFeedbackSelect);
    SO_KIT_CATALOG_ENTRY_HEADER(xCrosshairFeedback);
    SO_KIT_CATALOG_ENTRY_HEADER(yAxisFeedbackSwitch);
    SO_KIT_CATALOG_ENTRY_HEADER(yAxisFeedbackActive);
    SO_KIT_CATALOG_ENTRY_HEADER(yAxisFeedbackSelect);
    SO_KIT_CATALOG_ENTRY_HEADER(yCrosshairFeedback);
    SO_KIT_CATALOG_ENTRY_HEADER(zAxisFeedbackSwitch);
    SO_KIT_CATALOG_ENTRY_HEADER(zAxisFeedbackActive);
    SO_KIT_CATALOG_ENTRY_HEADER(zAxisFeedbackSelect);
    SO_KIT_CATALOG_ENTRY_HEADER(zCrosshairFeedback);

    SO_KIT_CATALOG_ENTRY_HEADER(translateBoxFeedbackSep);
    SO_KIT_CATALOG_ENTRY_HEADER(translateBoxFeedbackSwitch);
    SO_KIT_CATALOG_ENTRY_HEADER(translateBoxFeedbackRotation);
    SO_KIT_CATALOG_ENTRY_HEADER(translateBoxFeedback);

    SO_KIT_CATALOG_ENTRY_HEADER(scaleBoxFeedbackSwitch);
    SO_KIT_CATALOG_ENTRY_HEADER(scaleBoxFeedback);

    SO_KIT_CATALOG_ENTRY_HEADER(posXWallFeedbackSwitch);
    SO_KIT_CATALOG_ENTRY_HEADER(posXWallFeedback);
    SO_KIT_CATALOG_ENTRY_HEADER(posXRoundWallFeedback);
    SO_KIT_CATALOG_ENTRY_HEADER(posYWallFeedbackSwitch);
    SO_KIT_CATALOG_ENTRY_HEADER(posYWallFeedback);
    SO_KIT_CATALOG_ENTRY_HEADER(posYRoundWallFeedback);
    SO_KIT_CATALOG_ENTRY_HEADER(posZWallFeedbackSwitch);
    SO_KIT_CATALOG_ENTRY_HEADER(posZWallFeedback);
    SO_KIT_CATALOG_ENTRY_HEADER(posZRoundWallFeedback);
    SO_KIT_CATALOG_ENTRY_HEADER(negXWallFeedbackSwitch);
    SO_KIT_CATALOG_ENTRY_HEADER(negXWallFeedback);
    SO_KIT_CATALOG_ENTRY_HEADER(negXRoundWallFeedback);
    SO_KIT_CATALOG_ENTRY_HEADER(negYWallFeedbackSwitch);
    SO_KIT_CATALOG_ENTRY_HEADER(negYWallFeedback);
    SO_KIT_CATALOG_ENTRY_HEADER(negYRoundWallFeedback);
    SO_KIT_CATALOG_ENTRY_HEADER(negZWallFeedbackSwitch);
    SO_KIT_CATALOG_ENTRY_HEADER(negZWallFeedback);
    SO_KIT_CATALOG_ENTRY_HEADER(negZRoundWallFeedback);

    SO_KIT_CATALOG_ENTRY_HEADER(radialFeedbackSwitch);
    SO_KIT_CATALOG_ENTRY_HEADER(radialFeedback);

    SO_KIT_CATALOG_ENTRY_HEADER(circleFeedbackSep);
    SO_KIT_CATALOG_ENTRY_HEADER(circleFeedbackTransformSwitch);
    SO_KIT_CATALOG_ENTRY_HEADER(circleFeedbackAntiSquish);
    SO_KIT_CATALOG_ENTRY_HEADER(circleFeedbackTransform);
    SO_KIT_CATALOG_ENTRY_HEADER(xCircleFeedbackSwitch);
    SO_KIT_CATALOG_ENTRY_HEADER(xCircleFeedback);
    SO_KIT_CATALOG_ENTRY_HEADER(yCircleFeedbackSwitch);
    SO_KIT_CATALOG_ENTRY_HEADER(yCircleFeedback);
    SO_KIT_CATALOG_ENTRY_HEADER(zCircleFeedbackSwitch);
    SO_KIT_CATALOG_ENTRY_HEADER(zCircleFeedback);


  public:
    // Constructor
    SoTransformerDragger();

    SoSFRotation    rotation;
    SoSFVec3f       translation;
    SoSFVec3f       scaleFactor;
    SoSFFloat       minDiscRotDot; // 0-1, specifies minimum dot product 
				   // between eyeDir and rotPlane normal before
				   // switching to cylinder rotation model
				   // (default = 0.025)

    // Tells the dragger to unsquish its rotation and scale knobs during 
    // the next traversal.
    void unsquishKnobs();

    // Controls whether or not locate highlighting is used.
    SbBool isLocateHighlighting() { return locateHighlightOn; }
    void setLocateHighlighting( SbBool onOff );

    static void setColinearThreshold(int newVal) { colinearThreshold = newVal; }
    static int getColinearThreshold() { return colinearThreshold; }

  SoEXTENDER public:

    // These convert points and directions from the space of the unit
    // box to world space. Can be useful during callbacks.
    SbVec3f getBoxPointInWorldSpace( const SbVec3f &pointOnUnitBox );
    SbVec3f getBoxDirInWorldSpace( const SbVec3f &dirOnUnitBox );
    SbVec3f getWorldPointInBoxSpace( const SbVec3f &pointInWorldSpace );
    SbVec2f getWorldPointInPixelSpace( const SbVec3f &thePoint );

    // Callbacks would like to know this sometimes.
    // It's the location in bbox space of the point about which the 
    // transformer is rotating or scaling.  It's different depending on 
    // which part is picked (determined by getCurrentState()) and
    // which modifier keys are down.
    SbVec3f getInteractiveCenterInBoxSpace() 
		{ return interactiveCenterInBoxSpace; }

  SoINTERNAL public:
    static void		initClass();	// initialize the class

    enum State
	{ INACTIVE, 
	   RIT_X_ROTATE,  TOP_Y_ROTATE,  FNT_Z_ROTATE, 
	   LFT_X_ROTATE,  BOT_Y_ROTATE,  BAK_Z_ROTATE, 

	  PX_PY_PZ_3D_SCALE, PX_PY_NZ_3D_SCALE, PX_NY_PZ_3D_SCALE, 
	  PX_NY_NZ_3D_SCALE, NX_PY_PZ_3D_SCALE, NX_PY_NZ_3D_SCALE, 
	  NX_NY_PZ_3D_SCALE, NX_NY_NZ_3D_SCALE, 

	  RIT_TRANSLATE,  TOP_TRANSLATE,  FNT_TRANSLATE,
	  LFT_TRANSLATE,  BOT_TRANSLATE,  BAK_TRANSLATE };

    State getCurrentState() { return currentState; }

  protected:

    // Callbacks for drag start, motion, and finish
    static void startCB( void *, SoDragger * );
    static void motionCB( void *, SoDragger * );
    static void finishCB( void *, SoDragger * );
    
    SoFieldSensor *translFieldSensor;
    SoFieldSensor *scaleFieldSensor;
    SoFieldSensor *rotateFieldSensor;
    static void   fieldSensorCB( void *, SoSensor * );
    static void valueChangedCB( void *, SoDragger * );

    // Callbacks for pressing and releasing the meta keys
    static void	metaKeyChangeCB( void *, SoDragger *);

    // These really do the work during startCB, motionCB, and finishCB
    void	dragStart();
    void	drag();
    void	dragFinish();

    // Sets switch values for all parts in the scale, rotate, and translate
    // assemblies.
    void setAllPartSwitches( int     scaleAssemblyWhich,
			     int    rotateAssemblyWhich,
			     int translateAssemblyWhich );

    // detach/attach any sensors, callbacks, and/or field connections.
    // Called by:            start/end of SoBaseKit::readInstance
    // and on new copy by:   start/end of SoBaseKit::copy.
    // Classes that redefine must call setUpConnections(TRUE,TRUE) 
    // at end of constructor.
    // Returns the state of the node when this was called.
    virtual SbBool setUpConnections( SbBool onOff, SbBool doItAlways = FALSE );

    virtual void setDefaultOnNonWritingFields();

    virtual ~SoTransformerDragger();

    // Finds all SoAntiSquish nodes in subgraph and puts them in list.
    void updateAntiSquishList();
    // Tells all nodes in the antiSquishList to recalculate next time through.
    SoNodeList antiSquishList;

    int    getMouseGestureDirection(SbBool xAllowed, SbBool yAllowed, 
				    SbBool zAllowed);

    static int getIgnoreAxis(SbVec2f axis[3][2], SbBool xAllowed, 
			     SbBool yAllowed, SbBool zAllowed );

    static void makeMinorAxisPerpendicularIfColinear( SbVec2f origin, 
		    SbVec2f axisEnds[3][2], int indexA, int indexB );

    static SbBool isColinear(SbVec2f a1[2], SbVec2f a2[2], int pixels);

  private:

    State		currentState;     
    int                 currentDir;   // Used for 1-D operations,
				        // this variable records the current
				        // axis being used.
    // when doing 1-D rot, can rotate as disc (like spinning a turntable) or 
    // cylinder (like rolling a rolling pin). This flag says which.
    SbBool              rotatingAsDisc;

    SbPlaneProjector           *planeProj;
    SbLineProjector            *lineProj;
    SbSphereSectionProjector   *sphereProj;
    SbCylinderPlaneProjector   *cylProj;

    State	    restartState;   // used by meta callback. 
    SbVec3f         worldRestartPt; // used during interaction

    SbMatrix prevMotionMatrix; // used during rotate interaction
    SbVec3f  prevWorldHitPt;   // used during rotate and translate interaction
    SbVec3f  interactiveCenterInBoxSpace; // used during rotations.

    // used to reset prevWorldHitPt after initializing the cylinder projector.
    // Need to store during rotateInit() since SoDragger provides no method 
    // to retrieve.
    SbVec2f  startNormalizedLocaterPosition; 

    SbBool          altDown, ctlDown, shftDown;  // used to keep track of
						 // which meta keys were down.
    SbBool constraining; // Depends on state of shftDown and which part picked.

    // functions which do all the work
    void	setHighlights();
    void	setFeedback();
    void	setFeedbackForTranslate();
    void	setFeedbackForScale();
    void	setFeedbackForRotate();

    State               getStateFromPick();
    SbBool		translateInit();
    SbBool		rotateInit();
    SbBool		scaleInit();

    SbBool		translateDrag();
    SbBool		scaleDrag();

    SbBool		rotateDrag();
    SbBool		rotateConstrainedDrag();
    SbBool		rotateConstrainedDiscDrag();
    SbBool		rotateConstrainedCylindricalDrag();

    void initSphereProjector();
    void initDiscProjector();
    void initCylinderProjector();

    SbBool getShouldRotateAsDisc();
    int getConstrainedRotationAxis();

    // character strings from which the shared geometry is read
    static const char geomBuffer[];		

    void setAllDefaultParts();

    SbBool locateHighlightOn;

    static int colinearThreshold;

    // These subroutines construct the catalog. They normally would just
    // sit in the constructor, but the compiler doesn't like such huge
    // methods so we broke it up into small chunks.
    void makeCatalog();
    void makeTranslaterCatalogParts();
    void makeRotaterCatalogParts();
    void makeScalerCatalogParts();
    void makeAxisFeedbackCatalogParts();
    void makeBoxFeedbackCatalogParts();
    void makeWallFeedbackCatalogParts();
    void makeRadialFeedbackCatalogParts();
    void makeCircleFeedbackCatalogParts();
};

#endif /* _SO_TRANSFORMER_DRAGGER_ */
