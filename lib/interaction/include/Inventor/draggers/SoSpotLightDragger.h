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

////////////////////////////////////////////////////////////////////////
//
// Description:
//  This is the include file for the SoSpotLightDragger. 
//  This is a composite dragger which allows independent rotation,
//  translation, and beam spread editting of a spot light.
//
//  It is composed of an SoRotateSphericalDragger (for rotation), 
//  an SoDragPointDragger (for translation), and it creates its own projector
//  handles mouse events for doing it's own dragging of the beam angle.
//
//  The beam is editted by behaving like an SoRotateDiscDragger, but the 
//  plane of the disc is re-defined every time a drag is initiated.
//  The plane always passes through the z axis and the selected point.
//  When the rotation angle is determined, however, the beam is not rotated,
//  but scaled so it looks like an opening or closing umbrella. This is done
//  by scaling evenly in x and y, and a different amount in z, so the distance
//  between the selected point and the origin remains constant.
//
// NOTE TO DEVELOPERS:
//     For info about the structure of SoSpotLightDragger:
//     [1] compile: /usr/share/src/Inventor/samples/ivNodeKitStructure
//     [2] type:    ivNodeKitStructure SoSpotLightDragger.
//     [3] The program prints a diagram of the scene graph and a table with 
//         information about each part.
//
//
//  The following parts in this dragger are created at construction time.
//  'ResourceName' corresponds to the name of the default geometry for the
//  part. The dragger's constructor gets the scene graph for 'ResourceName'
//  by querying the global dictionary ( SoDB::getByName("ResourceName"); ).  
//
//  Resource Name:                      Part Name:
//
//  spotLightOverallMaterial          - material
//
//  spotLightTranslatorLineTranslator - 
//                                    - translator.xTranslator.translator
//                                    - translator.yTranslator.translator
//                                    - translator.zTranslator.translator
//  spotLightTranslatorLineTranslatorActive  - 
//                                    - translator.xTranslator.translatorActive
//                                    - translator.yTranslator.translatorActive
//                                    - translator.zTranslator.translatorActive
//
//  spotLightTranslatorPlaneTranslator- 
//                                    - translator.yzTranslator.translator
//                                    - translator.xzTranslator.translator
//                                    - translator.xyTranslator.translator
//  spotLightTranslatorPlaneTranslatorActive  - 
//                                    - translator.yzTranslator.translatorActive
//                                    - translator.xzTranslator.translatorActive
//                                    - translator.xyTranslator.translatorActive
//
//  spotLightRotatorRotator           - rotator.rotator      
//  spotLightRotatorRotatorActive     - rotator.rotatorActive
//  spotLightRotatorFeedback          - rotator.feedback      
//  spotLightRotatorFeedbackActive    - rotator.feedbackActive
//
//  spotLightBeam                     - beam
//  spotLightBeamActive               - beamActive
//
//  NOTE: This is a translation node that 
//        moves the beam origin relative
//        to the rest of the dragger.
//  spotLightBeamPlacement            - beamPlacement
//
////////////////////////////////////////////////////////////////////////

 
#ifndef  _SO_SPOT_LIGHT_DRAGGER_
#define  _SO_SPOT_LIGHT_DRAGGER_

#include <Inventor/SbLinear.h>
#include <Inventor/draggers/SoDragger.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFRotation.h>
#include <Inventor/sensors/SoSensor.h>

class SoFieldSensor;
class SbPlaneProjector;

// C-api: prefix=SoSpotLtDrag
// C-api: public=rotation,translation,angle
class SoSpotLightDragger : public SoDragger
{
    SO_KIT_HEADER(SoSpotLightDragger);

    // This gives the dragger an overall material.  It is edited by lightManips
    // to make its dragger match the color of the light.  Any materials within 
    // other parts will override this one. 
    SO_KIT_CATALOG_ENTRY_HEADER(material);

    // The translator is kept under a separator along with a
    // rotation that is maintained as the inverse to the rotation of the
    // light. This means that using the rotator does not rotate the
    // coordinate system that we translate the base of the dragger in.
    SO_KIT_CATALOG_ENTRY_HEADER(translatorSep);
    SO_KIT_CATALOG_ENTRY_HEADER(translatorRotInv);
    SO_KIT_CATALOG_ENTRY_HEADER(translator);
    SO_KIT_CATALOG_ENTRY_HEADER(rotator);
    // Beneath a separator, the beamPlacement part places the beam's local
    // space relative to the other parts.
    // The beamSwitch is flipped when the beam is dragged. 
    SO_KIT_CATALOG_ENTRY_HEADER(beamSep);
    SO_KIT_CATALOG_ENTRY_HEADER(beamPlacement);
    SO_KIT_CATALOG_ENTRY_HEADER(beamScale);
    SO_KIT_CATALOG_ENTRY_HEADER(beamSwitch);
    SO_KIT_CATALOG_ENTRY_HEADER(beam);
    SO_KIT_CATALOG_ENTRY_HEADER(beamActive);

  public:
    // Constructors
    SoSpotLightDragger();

    SoSFRotation rotation;
    SoSFVec3f    translation;
    SoSFFloat    angle;

  SoINTERNAL public:
    static void initClass(); // Initialize the class. 

  protected:

    SbPlaneProjector	*planeProj;  // used during interaciton with beam

    static void startCB( void *, SoDragger * );
    static void motionCB( void *, SoDragger * );
    static void doneCB( void *, SoDragger * );

    void	dragStart();
    void	drag();
    void	dragFinish();

    SoFieldSensor *rotFieldSensor;
    SoFieldSensor *translFieldSensor;
    SoFieldSensor *angleFieldSensor;
    static void   fieldSensorCB( void *, SoSensor * );
    static void   valueChangedCB( void *, SoDragger * );

    // Returns scaleFactor for beamScale part to display beamAngle.
    void setBeamScaleFromAngle(float beamAngle);

    // detach/attach any sensors, callbacks, and/or field connections.
    // Also set geometry of childDraggers to be our default instead of their
    // regular default, using our resources.
    // Called by:            start/end of SoBaseKit::readInstance
    // and on new copy by:   start/end of SoBaseKit::copy.
    // Classes that redefine must call setUpConnections(TRUE,TRUE) 
    // at end of constructor.
    // Returns the state of the node when this was called.
    virtual SbBool setUpConnections( SbBool onOff, SbBool doItAlways = FALSE );

    virtual void setDefaultOnNonWritingFields();

    virtual ~SoSpotLightDragger();

  private:
    static const char geomBuffer[];
};    

#endif  /* _SO_SPOT_LIGHT_DRAGGER_ */
