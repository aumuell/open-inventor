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
 |   $Revision: 1.1.1.1 $
 |
 |   Description:
 |	This file contains the declaration of the engines used by 
 |      linkages
 |
 |   Classes:
 |	EndPointFromParamsEngine
 |	LinkEngine
 |	RivetHingeEngine
 |	DoubleLinkMoveOriginEngine
 |	DoubleLinkMoveSharedPtEngine
 |	PistonErrorEngine
 |
 |	ZAngleFromRotationEngine
 |
 |   Author(s)		: Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_LINK_ENGINES_
#define  _SO_LINK_ENGINES_

#include <Inventor/engines/SoSubEngine.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFRotation.h>

/////////////////////////////////////////////////////////////
//  EndPointFromParamsEngine
//
/////////////////////////////////////////////////////////////
class EndPointFromParamsEngine : public SoEngine {

    SO_ENGINE_HEADER(EndPointFromParamsEngine);
    
  public:

    // Inputs
    SoSFVec3f	inOrigin;
    SoSFFloat	inSize;
    SoSFFloat	inAngle;
    
    // Output
    // The endPoint of a link given origin, size, and angle.
    SoEngineOutput outEndPoint;	     // (SoSFVec3f) 
    
    // Constructor
    EndPointFromParamsEngine();
    
  SoINTERNAL public:

    static void initClass();

  private:

    // Destructor
    virtual ~EndPointFromParamsEngine();

    // Evaluation method
    virtual void evaluate();
};

/////////////////////////////////////////////////////////////
//  LinkEngine
//
/////////////////////////////////////////////////////////////
class LinkEngine : public SoEngine {

    SO_ENGINE_HEADER(LinkEngine);
    
  public:

    // Inputs
    SoSFVec3f	inOrigin;
    SoSFVec3f	inEndPoint;
    
    // Output
    // The endPoint of a link given origin, size, and angle.
    SoEngineOutput outSize;	     // (SoSFFloat) 
    SoEngineOutput outAngle;	     // (SoSFFloat) 
    
    // Constructor
    LinkEngine();
    
  SoINTERNAL public:

    static void initClass();

  private:

    // Destructor
    virtual ~LinkEngine();

    // Evaluation method
    virtual void evaluate();
};

/////////////////////////////////////////////////////////////
//  RivetHingeEngine
//
/////////////////////////////////////////////////////////////
class RivetHingeEngine : public SoEngine {

    SO_ENGINE_HEADER(RivetHingeEngine);
    
  public:

    // Inputs
    SoSFVec3f	inOrigin;
    SoSFFloat	inSize;
    SoSFVec3f	inHingePoint;
    
    // Outputs
    // outError is TRUE if size is too short to reach the hinge point.
    SoEngineOutput outAngle;	     // (SoSFFloat) 
    SoEngineOutput outError;	     // (SoSFBool) 
    
    // Constructor
    RivetHingeEngine();
    
  SoINTERNAL public:

    static void initClass();

  private:

    // Destructor
    virtual ~RivetHingeEngine();

    // Evaluation method
    virtual void evaluate();
};

/////////////////////////////////////////////////////////////
//  DoubleLinkMoveOriginEngine
//
/////////////////////////////////////////////////////////////
class DoubleLinkMoveOriginEngine : public SoEngine {

    SO_ENGINE_HEADER(DoubleLinkMoveOriginEngine);
    
  public:

    // Inputs
    SoSFVec3f	inOrigin1;
    SoSFVec3f	inOrigin2;
    SoSFFloat	inSize1;
    SoSFFloat	inSize2;
    SoSFVec3f	inSharedPoint;
    
    // Outputs
    // outError is TRUE if size is too short to reach the hinge point.
    SoEngineOutput outSharedPoint;   // (SoSFVec3f) 
    SoEngineOutput outError;	     // (SoSFBool) 
    
    // Constructor
    DoubleLinkMoveOriginEngine();
    
  SoINTERNAL public:

    static void initClass();

  private:

    // Destructor
    virtual ~DoubleLinkMoveOriginEngine();

    // Evaluation method
    virtual void evaluate();
};

/////////////////////////////////////////////////////////////
//  DoubleLinkMoveSharedPtEngine
//
/////////////////////////////////////////////////////////////
class DoubleLinkMoveSharedPtEngine : public SoEngine {

    SO_ENGINE_HEADER(DoubleLinkMoveSharedPtEngine);
    
  public:

    // Inputs
    SoSFVec3f	inOrigin1;
    SoSFVec3f	inOrigin2;
    SoSFVec3f	inSharedPoint;
    
    // Outputs
    // outError is TRUE if size is too short to reach the hinge point.
    SoEngineOutput outSize1;         // (SoSFFloat) 
    SoEngineOutput outSize2;         // (SoSFFloat) 
    SoEngineOutput outError;	     // (SoSFBool) 
    
    // Constructor
    DoubleLinkMoveSharedPtEngine();
    
  SoINTERNAL public:

    static void initClass();

  private:

    // Destructor
    virtual ~DoubleLinkMoveSharedPtEngine();

    // Evaluation method
    virtual void evaluate();
};

/////////////////////////////////////////////////////////////
//  PistonErrorEngine
//
/////////////////////////////////////////////////////////////
class PistonErrorEngine : public SoEngine {

    SO_ENGINE_HEADER(PistonErrorEngine);
    
  public:

    // Inputs
    SoSFVec3f	inOrigin1;
    SoSFVec3f	inOrigin2;
    SoSFFloat	inSize1;
    SoSFFloat	inSize2;
    
    // Outputs
    // outError is TRUE if origins are too far or too close to each other.
    SoEngineOutput outError;	     // (SoSFBool) 
    
    // Constructor
    PistonErrorEngine();
    
  SoINTERNAL public:

    static void initClass();

  private:

    // Destructor
    virtual ~PistonErrorEngine();

    // Evaluation method
    virtual void evaluate();
};

/////////////////////////////////////////////////////////////
//  ZAngleFromRotationEngine
//
/////////////////////////////////////////////////////////////
class ZAngleFromRotationEngine : public SoEngine {

    SO_ENGINE_HEADER(ZAngleFromRotationEngine);
    
  public:

    // Inputs
    SoSFRotation   inRotation;
    
    // Outputs
    SoEngineOutput outAngle;      // (SoSFFloat)
    
    // Constructor
    ZAngleFromRotationEngine();
    
  SoINTERNAL public:

    static void initClass();

  private:

    // Destructor
    virtual ~ZAngleFromRotationEngine();

    // Evaluation method
    virtual void evaluate();
};

#endif  /* _SO_LINK_ENGINES_ */
