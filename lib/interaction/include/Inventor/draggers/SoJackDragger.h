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
 |	This file defines the jack dragger class, so named because
 |	with the default geometry, it looks like the children's toy.
 |	This dragger allows you to perform rotations,uniform scales
 |	and translations, all in one spiffy gadget.
 |
 |      Translation is done with a dragPointDragger, rotation with a 
 |	rotateSphericalDragger, and scaling with a scaleUniformDragger.
 |
 | NOTE TO DEVELOPERS:
 |     For info about the structure of SoJackDragger:
 |     [1] compile: /usr/share/src/Inventor/samples/ivNodeKitStructure
 |     [2] type:    ivNodeKitStructure SoJackDragger.
 |     [3] The program prints a diagram of the scene graph and a table with 
 |         information about each part.
 |
 |  The following parts in this dragger are created at construction time.
 |  'ResourceName' corresponds to the name of the default geometry for the
 |  part. The dragger's constructor gets the scene graph for 'ResourceName'
 |  by querying the global dictionary ( SoDB::getByName("ResourceName"); ).  
 |
 |  Resource Name:                      Part Name:
 |
 |   jackScalerScaler                 - scaler.scaler
 |   jackScalerScalerActive           - scaler.scalerActive
 |   jackScalerFeedback               - scaler.feedback
 |   jackScalerFeedbackActive         - scaler.feedbackActive
 |
 |   jackRotatorRotator               - rotator.rotator
 |   jackRotatorRotatorActive         - rotator.rotatorActive
 |   jackRotatorFeedback              - rotator.feedback
 |   jackRotatorFeedbackActive        - rotator.feedbackActive
 |
 |   jackTranslatorLineTranslator     - 
 |                                    - translator.xTranslator.translator
 |                                    - translator.yTranslator.translator
 |                                    - translator.zTranslator.translator
 |
 |   jackTranslatorLineTranslatorActive  - 
 |                                    - translator.xTranslator.translatorActive
 |                                    - translator.yTranslator.translatorActive
 |                                    - translator.zTranslator.translatorActive
 |
 |   jackTranslatorPlaneTranslator    - 
 |                                    - translator.yzTranslator.translator
 |                                    - translator.xzTranslator.translator
 |                                    - translator.xyTranslator.translator
 |
 |   jackTranslatorPlaneTranslatorActive  - 
 |                                    - translator.yzTranslator.translatorActive
 |                                    - translator.xzTranslator.translatorActive
 |                                    - translator.xyTranslator.translatorActive
 |
 |   jackTranslatorXFeedback          - translator.xFeedback
 |   jackTranslatorYFeedback          - translator.yFeedback
 |   jackTranslatorZFeedback          - translator.zFeedback
 |
 |   jackTranslatorXYFeedback         - translator.xyFeedback
 |   jackTranslatorXZFeedback         - translator.xzFeedback
 |   jackTranslatorYZFeedback         - translator.yzFeedback
 |
 |   Author(s): Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_JACK_DRAGGER_
#define  _SO_JACK_DRAGGER_

#include <Inventor/SbLinear.h>
#include <Inventor/draggers/SoDragger.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/fields/SoSFRotation.h>
#include <Inventor/sensors/SoSensor.h>

class SbDict;
class SoFieldSensor;

// C-api: prefix=SoJackDrag
// C-api: public=rotation,translation,scaleFactor
class SoJackDragger : public SoDragger
{
    SO_KIT_HEADER(SoJackDragger);

    // For making the dragger surround what lies above it.
    SO_KIT_CATALOG_ENTRY_HEADER(surroundScale);
    // For keeping the dragger even size in all 3 dimensions
    SO_KIT_CATALOG_ENTRY_HEADER(antiSquish);

    SO_KIT_CATALOG_ENTRY_HEADER(scaler);
    SO_KIT_CATALOG_ENTRY_HEADER(rotator);
    SO_KIT_CATALOG_ENTRY_HEADER(translator);

  public:
  
    // Constructors
    SoJackDragger();

    SoSFRotation rotation;
    SoSFVec3f    translation;
    SoSFVec3f    scaleFactor;

  SoINTERNAL public:
    static void initClass();  // initialize the class

  protected:

    SoFieldSensor *rotFieldSensor;
    SoFieldSensor *translFieldSensor;
    SoFieldSensor *scaleFieldSensor;
    static void   fieldSensorCB( void *, SoSensor * );
    static void valueChangedCB( void *, SoDragger * );

    static void invalidateSurroundScaleCB(  void *, SoDragger * );

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


    virtual ~SoJackDragger();

  private:
    static const char geomBuffer[];
};    

#endif  /* _SO_JACK_DRAGGER_ */
