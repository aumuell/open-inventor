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
 |   $Revision: 1.1.1.1 $
 |
 |   Description:
 |      Defines these classes for creating links to be used in linkages.
 |
 |      Link             
 |      RivetHinge       
 |      Crank       
 |      Piston       
 |
 |   Author(s)          : Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
*/

#ifndef  _SO_LINK_CLASSES_
#define  _SO_LINK_CLASSES_

#include <Inventor/engines/SoCompose.h>
#include <Inventor/engines/SoCalculator.h>
#include <Inventor/nodekits/SoInteractionKit.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/draggers/SoDragger.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include <Inventor/actions/SoWriteAction.h>

#include "LinkEngines.h"

////////////////////////////////////////////////////////////////////
//    Class: LinkBase - base class for all links
//
//    New nodes in this subclass are:
//      appearance, 
//    New fields in this subclass are:
//         none
//
////////////////////////////////////////////////////////////////////
class LinkBase : public SoInteractionKit {

    SO_KIT_HEADER(LinkBase);

    // Define new parts in the catalog
    SO_KIT_CATALOG_ENTRY_HEADER(appearance);


  public:

    virtual SbBool undoConnections();
    virtual SbBool setUpConnections(SbBool onOff, SbBool doItAlways = FALSE);
    virtual void setDefaultOnNonWritingFields();

    // Fields
    SoSFBool draggersOn;
    SoSFBool isError;

    // constructor
    LinkBase();

    virtual void errorColor( SbBool useErrorColor );

    // Turns off draggers before writing, if they are on, and restores them
    // afterwards.
    virtual void write( SoWriteAction *action );

  SoINTERNAL public:
    static void initClass();

    virtual void setDraggers( SbBool on );

  protected:

    SoFieldSensor *draggersOnSensor;
    static void draggersOnSensorCB( void *, SoSensor *);

    SoFieldSensor *isErrorSensor;
    static void isErrorSensorCB( void *, SoSensor *);

    virtual void handleEvent(SoHandleEventAction *ha);

    SbBool isShowingErrorColor;
    SoNode *savedMaterial;

    // destructor
    virtual ~LinkBase();
};

////////////////////////////////////////////////////////////////////
//    Class: SimpleLink - base class for 1-bar links
//
//    New nodes in this subclass are:
//      originTranslator, originTranslateGeom,
//      angleRotator, angleRotateGeom,
//      endPointTranslateSeparator, endPointTranslator, endPointTranslateGeom,
//      oneDScaleSeparator, oneDScaler, oneDScaleGeom,
//      twoDScaleSeparator, twoDScaler, twoDScaleGeom,
//      threeDScaleSeparator, threeDScaler, threeDScaleGeom,
//
//    New fields in this subclass are:
//         origin         - location of first link point
//         angle          - angle of rotation from (1,0,0) about Z-axis
//         size           - scale of link
//         endPoint         - location of second link point
//
////////////////////////////////////////////////////////////////////
class SimpleLink : public LinkBase {

    SO_KIT_HEADER(SimpleLink);

    // Define new parts in the catalog
    SO_KIT_CATALOG_ENTRY_HEADER(originTranslator);
    SO_KIT_CATALOG_ENTRY_HEADER(originTranslateGeom);

    SO_KIT_CATALOG_ENTRY_HEADER(angleRotator);
    SO_KIT_CATALOG_ENTRY_HEADER(angleRotateGeom);

    SO_KIT_CATALOG_ENTRY_HEADER(endPointTranslateSeparator);
    SO_KIT_CATALOG_ENTRY_HEADER(endPointTranslator);
    SO_KIT_CATALOG_ENTRY_HEADER(endPointTranslateGeom);

    SO_KIT_CATALOG_ENTRY_HEADER(oneDScaleSeparator);
    SO_KIT_CATALOG_ENTRY_HEADER(oneDScaler);
    SO_KIT_CATALOG_ENTRY_HEADER(oneDScaleGeom);

    SO_KIT_CATALOG_ENTRY_HEADER(twoDScaleSeparator);
    SO_KIT_CATALOG_ENTRY_HEADER(twoDScaler);
    SO_KIT_CATALOG_ENTRY_HEADER(twoDScaleGeom);

    SO_KIT_CATALOG_ENTRY_HEADER(threeDScaleSeparator);
    SO_KIT_CATALOG_ENTRY_HEADER(threeDScaler);
    SO_KIT_CATALOG_ENTRY_HEADER(threeDScaleGeom);

  public:

    virtual SbBool undoConnections();
    virtual SbBool setUpConnections(SbBool onOff, SbBool doItAlways = FALSE);
    virtual void setDefaultOnNonWritingFields();

    // Fields
    SoSFVec3f    origin;     // location of first link point
    SoSFFloat    angle;      // angle of rotation from (1,0,0) about Z-axis
    SoSFFloat    size;       // amount of scale of link

    SoSFVec3f    endPoint;      // location of second link point.
				// This is a "read-only" field as far as
				// connections go. Although you can connect 
				// points on other links from this one,
				// you should not try to connect 'endpoint'
				// from other fields.

    // constructor
    SimpleLink();

  SoINTERNAL public:
    static void initClass();

    virtual void setDraggers( SbBool on );

  protected:

    SoComposeVec3f *sizeZeroZeroEngine;
    SoComposeVec3f *sizeOneOneEngine;
    SoComposeVec3f *sizeSizeOneEngine;
    SoComposeVec3f *sizeSizeSizeEngine;

    EndPointFromParamsEngine *endPointEngine;

    // destructor
    virtual ~SimpleLink();
};

////////////////////////////////////////////////////////////////////
//    Class: GroundedSimpleLink - subclass of LinkBase with a a dragger to 
//                              control the origin field.
//
//    New nodes in this subclass are:
//      originDragger
//
//    New fields in this subclass are:
//      none
//
////////////////////////////////////////////////////////////////////
class GroundedSimpleLink : public SimpleLink {

    SO_KIT_HEADER(GroundedSimpleLink);

    // Define new parts in the catalog
    SO_KIT_CATALOG_ENTRY_HEADER(originDragger);

  public:

    virtual SbBool undoConnections();
    virtual SbBool setUpConnections(SbBool onOff, SbBool doItAlways = FALSE);
    virtual void setDefaultOnNonWritingFields();

    // Fields - none that are new

    // constructor
    GroundedSimpleLink();

  SoINTERNAL public:
    static void initClass();

    virtual void setDraggers( SbBool on );
    void setOriginDragger( SbBool on );

  protected:

    // destructor
    virtual ~GroundedSimpleLink();
};

////////////////////////////////////////////////////////////////////
//    Class: Link - subclass of GroundedSimpleLink with endPoint and 
//                  dragger to control endPoint
//
//    New nodes in this subclass are:
//      endPointDragger
//
//    New fields in this subclass are:
//      none.
//
////////////////////////////////////////////////////////////////////
class Link : public GroundedSimpleLink {

    SO_KIT_HEADER(Link);

    // Define new parts in the catalog
    SO_KIT_CATALOG_ENTRY_HEADER(endPointDragger);

  public:

    virtual SbBool undoConnections();
    virtual SbBool setUpConnections(SbBool onOff, SbBool doItAlways = FALSE);
    virtual void setDefaultOnNonWritingFields();

    // Fields - no new ones here.

    // constructor
    Link();

  SoINTERNAL public:
    static void initClass();

    virtual void setDraggers( SbBool on );
    void setEndPointDragger( SbBool on );

  protected:

    LinkEngine *getAngleAndSizeEngine;

    // destructor
    virtual ~Link();
};

////////////////////////////////////////////////////////////////////
//    Class: SizedLink - subclass of GroundedSimpleLink
//                        Adds a dragger to control the 'size' field.
//
//    New nodes in this subclass are:
//      sizeDragger
//
//    New fields in this subclass are - none :
//
////////////////////////////////////////////////////////////////////
class SizedLink : public GroundedSimpleLink {

    SO_KIT_HEADER(SizedLink);

    // Define new parts in the catalog
    SO_KIT_CATALOG_ENTRY_HEADER(sizeDragger);

  public:

    virtual SbBool undoConnections();
    virtual SbBool setUpConnections(SbBool onOff, SbBool doItAlways = FALSE);
    virtual void setDefaultOnNonWritingFields();

    // Fields - none new.

    // constructor
    SizedLink();

  SoINTERNAL public:
    static void initClass();

    virtual void setDraggers( SbBool on );
    void setSizeDragger( SbBool on );

  protected:

    SoDecomposeVec3f *sizeFromTranslateEngine;

    // destructor
    virtual ~SizedLink();
};
////////////////////////////////////////////////////////////////////
//    Class: RivetHinge - subclass of SizedLink
//                        Determines angle from 'origin' and 'hingePoint' 
//                        field.  Adds dragger to control 'hingePoint'
//
//    New nodes in this subclass are:
//      hingePointDragger, 
//
//    New fields in this subclass are:
//         hingePoint   - location of the rivet hinge
//
////////////////////////////////////////////////////////////////////
class RivetHinge : public SizedLink {

    SO_KIT_HEADER(RivetHinge);

    // Define new parts in the catalog
    SO_KIT_CATALOG_ENTRY_HEADER(hingePointDragger);

  public:

    virtual SbBool undoConnections();
    virtual SbBool setUpConnections(SbBool onOff, SbBool doItAlways = FALSE);
    virtual void setDefaultOnNonWritingFields();

    // Fields
    SoSFVec3f    hingePoint;   // location of the rivet hinge.

    // constructor
    RivetHinge();

  SoINTERNAL public:
    static void initClass();

    virtual void setDraggers( SbBool on );
    void setHingePointDragger( SbBool on );

  protected:

    RivetHingeEngine *myRivetEngine;

    // destructor
    virtual ~RivetHinge();
};
////////////////////////////////////////////////////////////////////
//
//    Class: Crank - subclass of SizedLink
//                   Adds dragger to control size and inAngle.
//
//    New nodes in this subclass are:
//      angleDragger
//
//    New fields in this subclass are:
//      none
//
////////////////////////////////////////////////////////////////////
class Crank : public SizedLink {

    SO_KIT_HEADER(Crank);

    // Define new parts in the catalog
    SO_KIT_CATALOG_ENTRY_HEADER(angleDragger);

  public:

    virtual SbBool undoConnections();
    virtual SbBool setUpConnections(SbBool onOff, SbBool doItAlways = FALSE);
    virtual void setDefaultOnNonWritingFields();

    // Fields - no new ones.

    // constructor
    Crank();

  SoINTERNAL public:
    static void initClass();

    virtual void setDraggers( SbBool on );
    void setAngleDragger( SbBool on );

  protected:

    ZAngleFromRotationEngine *angleFromRotationEngine;

    // destructor
    virtual ~Crank();
};

////////////////////////////////////////////////////////////////////
//    Class: DoubleLink - a class that contains a two-bar linkage.
//
//    New nodes in this subclass are:
//      origin1Dragger, origin2Dragger,
//      sharedPointDragger
//      Link1, Link2
//
//    New fields in this subclass are:
//         origin1       - origin of first link
//         origin2       - origin of second link
//         size1         - size of first link
//         size2         - size of second link
//         sharedPoint   - endPoint of both links
//
////////////////////////////////////////////////////////////////////
class DoubleLink : public LinkBase {

    SO_KIT_HEADER(DoubleLink);

    // Define new parts in the catalog
    SO_KIT_CATALOG_ENTRY_HEADER(origin1Dragger);
    SO_KIT_CATALOG_ENTRY_HEADER(origin2Dragger);
    SO_KIT_CATALOG_ENTRY_HEADER(sharedPointDragger);
    SO_KIT_CATALOG_ENTRY_HEADER(link1);
    SO_KIT_CATALOG_ENTRY_HEADER(link2);

  public:

    virtual SbBool undoConnections();
    virtual SbBool setUpConnections(SbBool onOff, SbBool doItAlways = FALSE);
    virtual void setDefaultOnNonWritingFields();

    // Fields
    SoSFVec3f    origin1;     // origin of link1
    SoSFVec3f    origin2;     // origin of link2
    SoSFFloat    size1;       // size of link1
    SoSFFloat    size2;       // size of link2
    SoSFVec3f    sharedPoint; // endPoint of both links.  This is a read-only
			      // field as far as connections go, since it
			      // is calculated based on the other four fields.
			      // You may connect other points from this one,
			      // but do not connect it from other fields.
			      // Note, however, that you can drag this point
			      // with the sharedPointDragger. This results 
			      // in changing the sizes of the two links while
			      // keeping the origins stationary.

    // constructor
    DoubleLink();

    virtual void errorColor( SbBool useErrorColor );

  SoINTERNAL public:
    static void initClass();

    virtual void setDraggers( SbBool on );
    void setOrigin1Dragger( SbBool on );
    void setOrigin2Dragger( SbBool on );
    void setSharedPointDragger( SbBool on );

  protected:

    DoubleLinkMoveOriginEngine   *myOriginEngine;
    DoubleLinkMoveSharedPtEngine *mySharedPtEngine;

    static void sharedPtDragStartCB( void *, SoDragger * );
    static void sharedPtDragFinishCB( void *, SoDragger * );

    // destructor
    virtual ~DoubleLink();
};

////////////////////////////////////////////////////////////////////
//    Class: Piston
//
//    contains two RivetHinge nodes, and hooks up its own fields to the
//    fields of the RivetHinges
//
//    New nodes in this subclass are:
//      origin1Dragger, origin2Dragger
//      link1, link2
//
//    New fields in this subclass are:
//         origin1         - location of first link's fixed point
//         origin2         - location of second link's fixed point
//         size1      - size of the first link
//         size2      - size of the second link
//
////////////////////////////////////////////////////////////////////
class Piston : public LinkBase {

    SO_KIT_HEADER(Piston);

    // Define new parts in the catalog
    SO_KIT_CATALOG_ENTRY_HEADER(origin1Dragger);
    SO_KIT_CATALOG_ENTRY_HEADER(origin2Dragger);
    SO_KIT_CATALOG_ENTRY_HEADER(link1);
    SO_KIT_CATALOG_ENTRY_HEADER(link2);

  public:

    virtual SbBool undoConnections();
    virtual SbBool setUpConnections(SbBool onOff, SbBool doItAlways = FALSE);
    virtual void setDefaultOnNonWritingFields();

    // Fields
    SoSFVec3f    origin1;
    SoSFVec3f    origin2;
    SoSFFloat    size1;
    SoSFFloat    size2;

    // constructor
    Piston();

    virtual void errorColor( SbBool useErrorColor );

  SoINTERNAL public:
    static void initClass();

    virtual void setDraggers( SbBool on );
    void setOrigin1Dragger( SbBool on );
    void setOrigin2Dragger( SbBool on );

  protected:

    PistonErrorEngine *myPistonErrorEngine;
    
    // destructor
    virtual ~Piston();
};

class Button : public SoDragger {

    SO_KIT_HEADER(Button);

    // Define new parts in the catalog
    SO_KIT_CATALOG_ENTRY_HEADER(buttonSwitch);

    SO_KIT_CATALOG_ENTRY_HEADER(buttonSep);
    SO_KIT_CATALOG_ENTRY_HEADER(buttonGeom);
    SO_KIT_CATALOG_ENTRY_HEADER(buttonText);
    SO_KIT_CATALOG_ENTRY_HEADER(buttonActiveSep);
    SO_KIT_CATALOG_ENTRY_HEADER(buttonActiveGeom);
    SO_KIT_CATALOG_ENTRY_HEADER(buttonActiveText);

  public:

    // constructor
    Button();

  SoINTERNAL public:
    static void initClass();

  protected:

    static void startCB( void *, SoDragger *);
    static void finishCB( void *, SoDragger *);

    void dragStart();
    void dragFinish();

    // destructor
    virtual ~Button();
};
#endif /* _SO_LINK_CLASSES_ */
