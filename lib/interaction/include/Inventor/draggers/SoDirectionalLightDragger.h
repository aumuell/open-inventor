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
 |   $Revision: 1.1 $
 |
 |   Description:
 |	This is the include file for the SoDirectionalLightDragger.
 |
 |   This is a composite dragger which allows independent rotation,
 |   and translation for dragging a directional light.
 |   When applied to a directional light, the translation will only serve
 |   to move the dragger. This translation will have no effect on
 |   the directional light being dragged.
 |
 |  It is composed of an SoRotateSphericalDragger (for rotation), 
 |  and an SoDragPointDragger (for translation).
 |
 | NOTE TO DEVELOPERS:
 |     For info about the structure of SoDirectionalLightDragger:
 |     [1] compile: /usr/share/src/Inventor/samples/ivNodeKitStructure
 |     [2] type:    ivNodeKitStructure SoDirectionalLightDragger.
 |     [3] The program prints a diagram of the scene graph and a table with 
 |         information about each part.
 |
 |  The following parts in this dragger are created at construction time.
 |  'ResourceName' corresponds to the name of the default geometry for the
 |  part. The dragger's constructor gets the scene graph for 'ResourceName'
 |  by querying the global dictionary ( SoDB::getByName("ResourceName"); ).  
 |
 |  Resource Name:                             Part Name:
 |
 |  directionalLightOverallMaterial          - material
 |
 |  directionalLightTranslatorLineTranslator - 
 |				      - translator.xTranslator.translator
 |				      - translator.yTranslator.translator
 |				      - translator.zTranslator.translator
 |  directionalLightTranslatorLineTranslatorActive  - 
 |				      - translator.xTranslator.translatorActive
 |				      - translator.yTranslator.translatorActive
 |				      - translator.zTranslator.translatorActive
 |
 |  directionalLightTranslatorPlaneTranslator        - 
 |				      - translator.yzTranslator.translator
 |				      - translator.xzTranslator.translator
 |				      - translator.xyTranslator.translator
 |  directionalLightTranslatorPlaneTranslatorActive  - 
 |				      - translator.yzTranslator.translatorActive
 |				      - translator.xzTranslator.translatorActive
 |				      - translator.xyTranslator.translatorActive
 |
 |  directionalLightRotatorRotator        - rotator.rotator
 |  directionalLightRotatorRotatorActive  - rotator.rotatorActive
 |  directionalLightRotatorFeedback       - rotator.feedback
 |  directionalLightRotatorFeedbackActive - rotator.feedbackActive
 |
 |   Author(s): Paul Isaacs, David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_DIRECTIONAL_LIGHT_DRAGGER_
#define  _SO_DIRECTIONAL_LIGHT_DRAGGER_

#include <Inventor/SbLinear.h>
#include <Inventor/draggers/SoDragger.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/fields/SoSFRotation.h>
#include <Inventor/sensors/SoSensor.h>

class SoFieldSensor;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoDirectionalLightDragger
//
//  This changes the direction of directional lights.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoDirLtDrag
// C-api: public=rotation,translation
class SoDirectionalLightDragger : public SoDragger {

    SO_KIT_HEADER(SoDirectionalLightDragger);

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

  public:
    SoDirectionalLightDragger();

    SoSFRotation rotation;
    SoSFVec3f    translation;

  SoINTERNAL public:
    static void		initClass();	// initialize the class

  protected:

    SoFieldSensor *translFieldSensor;
    SoFieldSensor *rotFieldSensor;
    static void   fieldSensorCB( void *, SoSensor * );
    static void valueChangedCB( void *, SoDragger * );

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

    ~SoDirectionalLightDragger();

  private:
    static const char		geomBuffer[];
};

#endif /* _SO_DIRECTIONAL_LIGHT_DRAGGER_ */
