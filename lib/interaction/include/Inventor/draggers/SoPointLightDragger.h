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
 |	This file defines the point light dragger class.
 |	This is nothing more than a drag point dragger that whose
 |	default geometry is most appropriate when dragging a
 |	point light source.
 |
 | NOTE TO DEVELOPERS:
 |     For info about the structure of SoPointLightDragger:
 |     [1] compile: /usr/share/src/Inventor/samples/ivNodeKitStructure
 |     [2] type:    ivNodeKitStructure SoPointLightDragger.
 |     [3] The program prints a diagram of the scene graph and a table with 
 |         information about each part.
 |
 |  The following parts in this dragger are created at construction time.
 |  'ResourceName' corresponds to the name of the default geometry for the
 |  part. The dragger's constructor gets the scene graph for 'ResourceName'
 |  by querying the global dictionary ( SoDB::getByName("ResourceName"); ).  
 |
 |  Resource Name:                        Part Name:
 |
 |  pointLightOverallMaterial           - material
 |
 |  pointLightTranslatorLineTranslator  - 
 |                                    - translator.xTranslator.translator
 |                                    - translator.yTranslator.translator
 |                                    - translator.zTranslator.translator
 |  pointLightTranslatorLineTranslatorActive  - 
 |                                    - translator.xTranslator.translatorActive
 |                                    - translator.yTranslator.translatorActive
 |                                    - translator.zTranslator.translatorActive
 |
 |  pointLightTranslatorPlaneTranslator - 
 |                                    - translator.yzTranslator.translator
 |                                    - translator.xzTranslator.translator
 |                                    - translator.xyTranslator.translator
 |  pointLightTranslatorPlaneTranslatorActive  - 
 |                                    - translator.yzTranslator.translatorActive
 |                                    - translator.xzTranslator.translatorActive
 |                                    - translator.xyTranslator.translatorActive
 |
 |
 |   Author(s): Paul Isaacs, Howard Look
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_POINT_LIGHT_DRAGGER_
#define  _SO_POINT_LIGHT_DRAGGER_

#include <Inventor/SbLinear.h>
#include <Inventor/draggers/SoDragger.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/sensors/SoSensor.h>

class SoFieldSensor;

// C-api: prefix=SoPtLtDrag
// C-api: public=translation
class SoPointLightDragger : public SoDragger
{
    SO_KIT_HEADER(SoPointLightDragger);

    // This gives the dragger an overall material.  It is edited by lightManips
    // to make its dragger match the color of the light.  Any materials within 
    // other parts will override this one. 
    SO_KIT_CATALOG_ENTRY_HEADER(material);

    SO_KIT_CATALOG_ENTRY_HEADER(translator);

  public:
    SoPointLightDragger();

    SoSFVec3f    translation;

  SoINTERNAL public:
    static void initClass();  // initialize the class

  protected:

    SoFieldSensor *fieldSensor;
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

    virtual ~SoPointLightDragger();

  private:

    static const char geomBuffer[];
};    

#endif  /* _SO_POINT_LIGHT_DRAGGER_ */
