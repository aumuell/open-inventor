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

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.3 $
 |
 |   Classes:
 |	EndPointFromParamsEngine
 |	LinkEngine
 |	RivetHingeEngine
 |	DoubleLinkMoveOriginEngine
 |	DoubleLinkMoveSharedPtEngine
 |	PistonErrorEngine
 |
 |      ZAngleFromRotationEngine
 |
 |   Author(s)          : Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include "LinkEngines.h"

/////////////////////////////////////////////////////////////
//  EndPointFromParamsEngine
//
/////////////////////////////////////////////////////////////

SO_ENGINE_SOURCE(EndPointFromParamsEngine);

//
// Description:
//    This initializes the EndPointFromParamsEngine class.
//
// Use: internal
//

void
EndPointFromParamsEngine::initClass()
{
    SO__ENGINE_INIT_CLASS(EndPointFromParamsEngine, 
			 "EndPointFromParamsEngine", SoEngine);
}

//
// Description:
//    Constructor
//
// Use: public
//

EndPointFromParamsEngine::EndPointFromParamsEngine()
{
    SO_ENGINE_CONSTRUCTOR(EndPointFromParamsEngine);
    SO_ENGINE_ADD_INPUT(inOrigin,     (SbVec3f(0,0,0)));
    SO_ENGINE_ADD_INPUT(inSize,	      (1.0));
    SO_ENGINE_ADD_INPUT(inAngle,      (0.0));

    SO_ENGINE_ADD_OUTPUT(outEndPoint,      SoSFVec3f);
    isBuiltIn = TRUE;
}

//
// Description:
//    Destructor
//
// Use: private
//

EndPointFromParamsEngine::~EndPointFromParamsEngine()
{
}

//
// Description:
//    Evaluation routine
//
// Use: private
//

void
EndPointFromParamsEngine::evaluate()
{
    // We're just gonna rotate about +z by the angle field value.
    // This here is faster than building rotations, etc.
    SbVec3f linkDir( cosf(inAngle.getValue()), sinf(inAngle.getValue()), 0.0);

    // Calculate the endPoint based on other params.
    SbVec3f newEndPoint = inOrigin.getValue() + (inSize.getValue() * linkDir);

    if ( outEndPoint.getNumConnections() ) {
	SO_ENGINE_OUTPUT(outEndPoint, SoSFVec3f, setValue( newEndPoint ) );
    }
}

/////////////////////////////////////////////////////////////
//  LinkEngine
//
/////////////////////////////////////////////////////////////

SO_ENGINE_SOURCE(LinkEngine);

//
// Description:
//    This initializes the LinkEngine class.
//
// Use: internal
//

void
LinkEngine::initClass()
{
    SO__ENGINE_INIT_CLASS(LinkEngine, "LinkEngine", SoEngine);
}

//
// Description:
//    Constructor
//
// Use: public
//

LinkEngine::LinkEngine()
{
    SO_ENGINE_CONSTRUCTOR(LinkEngine);
    SO_ENGINE_ADD_INPUT(inOrigin,     (SbVec3f(0,0,0)));
    SO_ENGINE_ADD_INPUT(inEndPoint,   (SbVec3f(1,0,0)));

    SO_ENGINE_ADD_OUTPUT(outSize,       SoSFFloat);
    SO_ENGINE_ADD_OUTPUT(outAngle,      SoSFFloat);
    isBuiltIn = TRUE;
}

//
// Description:
//    Destructor
//
// Use: private
//

LinkEngine::~LinkEngine()
{
}

//
// Description:
//    Evaluation routine
//
// Use: private
//

void
LinkEngine::evaluate()
{
    SbVec3f endPointDir = inEndPoint.getValue() - inOrigin.getValue();

    // We are dealing only in the xy plane here. So zero out the z component
    // of the direction.
    endPointDir.setValue(endPointDir[0], endPointDir[1], 0.0);
    float endPointDist = endPointDir.normalize();

    // angle is the angle between (inEndPoint - inOrigin) and (1,0,0)
    // It must be in the sense of rotation about (0,0,1), not (0,0,-1)
    // Find the rotation...
    SbRotation theRot( SbVec3f(1,0,0), endPointDir );

    // Decompose the rotation into angle and axis.
    SbVec3f theAxis;
    float   theAngle;
    theRot.getValue( theAxis, theAngle );

    // We want an angle about the positive Z axis.
    // If the axis is negative, we need to flip the angle.
    if ( theAxis[2] < 0 )
    theAngle *= -1.0;

    // Set angle based on our findings...
    if ( outAngle.getNumConnections() ) {
	SO_ENGINE_OUTPUT(outAngle, SoSFFloat, setValue( theAngle ) );
    }

    // Set size as the distance between inOrigin and inEndPoint
    if ( outSize.getNumConnections() ) {
	SO_ENGINE_OUTPUT(outSize,  SoSFFloat, setValue( endPointDist ) );
    }
}

/////////////////////////////////////////////////////////////
//  RivetHingeEngine
//
/////////////////////////////////////////////////////////////

SO_ENGINE_SOURCE(RivetHingeEngine);

//
// Description:
//    This initializes the RivetHingeEngine class.
//
// Use: internal
//

void
RivetHingeEngine::initClass()
{
    SO__ENGINE_INIT_CLASS(RivetHingeEngine, "RivetHingeEngine", SoEngine);
}

//
// Description:
//    Constructor
//
// Use: public
//

RivetHingeEngine::RivetHingeEngine()
{
    SO_ENGINE_CONSTRUCTOR(RivetHingeEngine);
    SO_ENGINE_ADD_INPUT(inOrigin,     (SbVec3f(0,0,0)));
    SO_ENGINE_ADD_INPUT(inSize,	      (1.0));
    SO_ENGINE_ADD_INPUT(inHingePoint, (SbVec3f(1,0,0)));

    SO_ENGINE_ADD_OUTPUT(outAngle,         SoSFFloat);
    SO_ENGINE_ADD_OUTPUT(outError,         SoSFBool);
    isBuiltIn = TRUE;
}

//
// Description:
//    Destructor
//
// Use: private
//

RivetHingeEngine::~RivetHingeEngine()
{
}

//
// Description:
//    Evaluation routine
//
// Use: private
//

void
RivetHingeEngine::evaluate()
{
    SbBool isError = FALSE;
    SbVec3f hingePointDir = inHingePoint.getValue()    - inOrigin.getValue();

    if (hingePointDir == SbVec3f(0,0,0)) {
	// Don't change the angle, we don't know where to go!
	isError = TRUE;
    }
    else {
	// We only move in xy plane, so neaten it up...
        hingePointDir.setValue( hingePointDir[0], hingePointDir[1], 0.0);
	float hingePointDist = hingePointDir.normalize();

	// The angle is the given by the angle between (hingePoint - origin)
	// and (1,0,0). It must be in the sense of rotation
	// about (0,0,1), not (0,0,-1)
	// Find the rotation...
	SbRotation theRot( SbVec3f(1,0,0), hingePointDir );

	// Decompose the rotation into angle and axis.
	SbVec3f theAxis;
	float   theAngle;
	theRot.getValue( theAxis, theAngle );

	// We want an angle about the positive Z axis.
	// If the axis is negative, we need to flip the angle.
	if ( theAxis[2] < 0 )
	    theAngle *= -1.0;

	// Set angle based on our findings...
	if ( outAngle.getNumConnections() )
	    SO_ENGINE_OUTPUT(outAngle, SoSFFloat, setValue( theAngle ));

	// If the hinge point is  further away than the length of the
	// link, change to errorColor...
	if ( hingePointDist > inSize.getValue() )
	    isError = TRUE;
    }

    if ( outError.getNumConnections() ) {
	SO_ENGINE_OUTPUT(outError, SoSFBool, setValue( isError ) );
    }
}

/////////////////////////////////////////////////////////////
//  DoubleLinkMoveOriginEngine
//
/////////////////////////////////////////////////////////////

SO_ENGINE_SOURCE(DoubleLinkMoveOriginEngine);

//
// Description:
//    This initializes the DoubleLinkMoveOriginEngine class.
//
// Use: internal
//

void
DoubleLinkMoveOriginEngine::initClass()
{
    SO__ENGINE_INIT_CLASS(DoubleLinkMoveOriginEngine, 
			 "DoubleLinkMoveOriginEngine", SoEngine);
}

//
// Description:
//    Constructor
//
// Use: public
//

DoubleLinkMoveOriginEngine::DoubleLinkMoveOriginEngine()
{
    SO_ENGINE_CONSTRUCTOR(DoubleLinkMoveOriginEngine);
    SO_ENGINE_ADD_INPUT(inOrigin1,     (SbVec3f(0,0,0)));
    SO_ENGINE_ADD_INPUT(inOrigin2,     (SbVec3f(1,0,0)));
    SO_ENGINE_ADD_INPUT(inSize1,	      (1.0));
    SO_ENGINE_ADD_INPUT(inSize2,	      (1.0));
    SO_ENGINE_ADD_INPUT(inSharedPoint,	      (SbVec3f(0,.5,0)));

    SO_ENGINE_ADD_OUTPUT(outSharedPoint,   SoSFVec3f);
    SO_ENGINE_ADD_OUTPUT(outError,         SoSFBool);
    isBuiltIn = TRUE;
}

//
// Description:
//    Destructor
//
// Use: private
//

DoubleLinkMoveOriginEngine::~DoubleLinkMoveOriginEngine()
{
}

//
// Description:
//    Evaluation routine
//
// Use: private
//

void
DoubleLinkMoveOriginEngine::evaluate()
{

    // Find the outSharedPoint...

    //             sharedPoint
    //
    //         \./
    //         ...
    //     s1 . . . s2
    //       .  .  .
    //      .  h.   .
    //     .    .    .
    //   \............./
    //    o1    |    o2
    //    |     |      |
    //    |< a1>|< a2 >|
    //    |            |
    //    |<--  D ---->|
    // 
    // Derivation of h in terms of s1, s2, and D
    // (where s1 = size1, s2 = size2, and D = (o2-o1).normalize())
    //
    // s1*s1 = a1*a1 + h*h;   s2*s2 = a2*a2 + h*h;
    // D = a1 + a2;       a2 = D - a1;
    // s1*s1 = a1*a1 + h*h;   s2*s2 = (D-a1)*(D-a1) + h*h
    // a1 = (+/-) sqrt( s1*s1 - h*h);
    // s2*s2 = (D (+/-) sqrt( s1*s1 - h*h))*(D (+/-) sqrt( s1*s1 - h*h)) + h*h
    // s2*s2 = D*D (+/-) 2*D*sqrt(s1*s1-h*h) + (s1*s1-h*h) +h*h
    // s2*s2 = D*D (+/-) 2*D*sqrt(s1*s1-h*h) + s1*s1
    // 2*D*sqrt(s1*s1-h*h) = (+/-) (D*D - s2*s2 + s1*s1)
    //  sqrt( s1*s1 - h*h) = (+/-) ((D*D - s2*s2 + s1*s1) / (2*D))
    //  s1*s1 - h*h = (D*D - s2*s2 + s1*s1)* (D*D - s2*s2 + s1*s1) / (4*D*D)
    //  h*h = s1*s1 - ((D*D - s2*s2 + s1*s1)* (D*D - s2*s2 + s1*s1) / (4*D*D))
    // So, let:
    //    k1 = D*D - s2*s2 + s1*s1;
    //    k2 = 4*D*D;
    //    k3 = s1*s1;
    // Then,
    //    h*h = k3 - (k1*k1 / k2 );
    //    h = (+/-) sqrt( k3 - k1*k1/k2);

    float s1 = inSize1.getValue();    
    float s2 = inSize2.getValue();    

    SbVec3f DVec = inOrigin2.getValue() - inOrigin1.getValue();
    float D = DVec.normalize();

    float    k1 = D*D - s2*s2 + s1*s1;
    float    k2 = 4*D*D;
    float    k3 = s1*s1;

    SbBool gotError = FALSE;
    SbVec3f newSharedPoint;

    if (k2 == 0.0) {
	// Both origins are same location.  This is an error if (size1 != size2)
	if ( s1 != s2) 
	    gotError = TRUE;

	// Make newSharedPoint be a distance of size1 from origin.
	// For direction, use current direction to sharedPoint.
	SbVec3f myDir = inSharedPoint.getValue() - inOrigin1.getValue();
	if (myDir == SbVec3f(0,0,0))
	    newSharedPoint = inSharedPoint.getValue();
	else {
	    myDir.normalize();
	    newSharedPoint = inOrigin1.getValue() + s1 * myDir;
	}
    }
    else {
        float hSquared;
	hSquared = k3 - (k1 * k1 ) / k2;
	if (hSquared >= 0.0) {

	    gotError = FALSE;

	    float h = sqrt( hSquared );
	    float a1 = sqrt( s1*s1 - hSquared);

	    SbVec3f zVec(0,0,1);
	    SbVec3f perpVec = zVec.cross( DVec );

	    // Determine if we want to use plus a1 or minus a1
	    // For this test, the sign of h doesn't matter.
		// Get the sharedPoints we find using pos and neg a1
		// By leaving origin1 and travelling along DVec and perpVec
		SbVec3f posA1Pt =inOrigin1.getValue() + DVec * a1 + perpVec * h;
		SbVec3f negA1Pt =inOrigin1.getValue() - DVec * a1 + perpVec * h;
		// How far is each from origin2?  Use the square of the dist.
		SbVec3f posDiff = posA1Pt - inOrigin2.getValue();
		SbVec3f negDiff = negA1Pt - inOrigin2.getValue();
		float posSq = posDiff.dot(posDiff);
		float negSq = negDiff.dot(negDiff);

		// Which is closer to square of size2?
		// If value from negative test is closer, use -a1
		float s2sq = s2 * s2;
		if ( fabs(negSq - s2sq) < fabs(posSq - s2sq) )
		    a1 *= -1;

	    // Get answers for both +h and -h. Both are correct.
	    // Use the one closer to the old value of sharedPonit.
		SbVec3f choice1 =inOrigin1.getValue() + DVec * a1 + perpVec * h;
		SbVec3f choice2 =inOrigin1.getValue() + DVec * a1 - perpVec * h;

		// Which choice is closer to the old value?
		SbVec3f diff1 = choice1 - inSharedPoint.getValue();
		SbVec3f diff2 = choice2 - inSharedPoint.getValue();
		float val1 = diff1.dot( diff1 ); 
		float val2 = diff2.dot( diff2 ); 
		newSharedPoint = (val1 < val2 ) ? choice1 : choice2;
	}
	else {
	    gotError = TRUE;
	    // Error of some sort. Make sharedPoint lie on line connecting 
	    // the points, at a distance of size1 from origin1.
	    float a1 = s1;
	    SbVec3f choice1, choice2;
	    choice1 = inOrigin1.getValue() + DVec * a1;
	    choice2 = inOrigin1.getValue() - DVec * a1;

	    // Which choice is closer to the old value?
	    SbVec3f diff1 = choice1 - inSharedPoint.getValue();
	    SbVec3f diff2 = choice2 - inSharedPoint.getValue();
	    float val1 = diff1.dot( diff1 ); 
	    float val2 = diff2.dot( diff2 ); 
	    newSharedPoint = (val1 < val2 ) ? choice1 : choice2;
	}
    }

    if ( outSharedPoint.getNumConnections() ) {
	SO_ENGINE_OUTPUT(outSharedPoint, SoSFVec3f, setValue( newSharedPoint ));
    }
    if ( outError.getNumConnections() ) {
	SO_ENGINE_OUTPUT(outError, SoSFBool, setValue( gotError ) );
    }
}

/////////////////////////////////////////////////////////////
//  DoubleLinkMoveSharedPtEngine
//
/////////////////////////////////////////////////////////////

SO_ENGINE_SOURCE(DoubleLinkMoveSharedPtEngine);

//
// Description:
//    This initializes the DoubleLinkMoveSharedPtEngine class.
//
// Use: internal
//

void
DoubleLinkMoveSharedPtEngine::initClass()
{
    SO__ENGINE_INIT_CLASS(DoubleLinkMoveSharedPtEngine, 
			 "DoubleLinkMoveSharedPtEngine", SoEngine);
}

//
// Description:
//    Constructor
//
// Use: public
//

DoubleLinkMoveSharedPtEngine::DoubleLinkMoveSharedPtEngine()
{
    SO_ENGINE_CONSTRUCTOR(DoubleLinkMoveSharedPtEngine);
    SO_ENGINE_ADD_INPUT(inOrigin1,     (SbVec3f(0,0,0)));
    SO_ENGINE_ADD_INPUT(inOrigin2,     (SbVec3f(1,0,0)));
    SO_ENGINE_ADD_INPUT(inSharedPoint, (SbVec3f(0,.5,0)));

    SO_ENGINE_ADD_OUTPUT(outSize1,         SoSFFloat);
    SO_ENGINE_ADD_OUTPUT(outSize2,         SoSFFloat);
    SO_ENGINE_ADD_OUTPUT(outError,         SoSFBool);
    isBuiltIn = TRUE;
}

//
// Description:
//    Destructor
//
// Use: private
//

DoubleLinkMoveSharedPtEngine::~DoubleLinkMoveSharedPtEngine()
{
}

//
// Description:
//    Evaluation routine
//
// Use: private
//

void
DoubleLinkMoveSharedPtEngine::evaluate()
{
    // Figure out sizes based on distances between points.
    float newSize1 
	= (inSharedPoint.getValue() - inOrigin1.getValue()).length();
    float newSize2 
	= (inSharedPoint.getValue() - inOrigin2.getValue()).length();

    if ( outSize1.getNumConnections() ) {
	SO_ENGINE_OUTPUT(outSize1, SoSFFloat, setValue( newSize1 ) );
    }
    if ( outSize2.getNumConnections() ) {
	SO_ENGINE_OUTPUT(outSize2, SoSFFloat, setValue( newSize2 ) );
    }

    // This always wipes out the error!
    if ( outError.getNumConnections() ) {
	SO_ENGINE_OUTPUT(outError, SoSFBool, setValue( FALSE ) );
    }
}
/////////////////////////////////////////////////////////////
//  PistonErrorEngine
//
/////////////////////////////////////////////////////////////

SO_ENGINE_SOURCE(PistonErrorEngine);

//
// Description:
//    This initializes the PistonErrorEngine class.
//
// Use: internal
//

void
PistonErrorEngine::initClass()
{
    SO__ENGINE_INIT_CLASS(PistonErrorEngine, 
			 "PistonErrorEngine", SoEngine);
}

//
// Description:
//    Constructor
//
// Use: public
//

PistonErrorEngine::PistonErrorEngine()
{
    SO_ENGINE_CONSTRUCTOR(PistonErrorEngine);
    SO_ENGINE_ADD_INPUT(inOrigin1,     (SbVec3f(0,0,0)));
    SO_ENGINE_ADD_INPUT(inOrigin2,     (SbVec3f(1,0,0)));
    SO_ENGINE_ADD_INPUT(inSize1,	      (1.0));
    SO_ENGINE_ADD_INPUT(inSize2,	      (1.0));

    SO_ENGINE_ADD_OUTPUT(outError,         SoSFBool);
    isBuiltIn = TRUE;
}

//
// Description:
//    Destructor
//
// Use: private
//

PistonErrorEngine::~PistonErrorEngine()
{
}

//
// Description:
//    Evaluation routine
//
// Use: private
//

void
PistonErrorEngine::evaluate()
{
    // If the hinge point is  further away than the length of the
    // link, change to errorColor...
    SbBool gotError = FALSE;
    float dist = (inOrigin1.getValue() - inOrigin2.getValue()).length();
    float s1 = inSize1.getValue();
    float s2 = inSize2.getValue();
    if ( dist > ( s1 + s2 ) )
	gotError = TRUE;
    if ( (dist < s1) || (dist < s2) )
	gotError = TRUE;

    // This always wipes out the error!
    if ( outError.getNumConnections() ) {
	SO_ENGINE_OUTPUT(outError, SoSFBool, setValue( gotError ) );
    }
}

/////////////////////////////////////////////////////////////
//  ZAngleFromRotationEngine
//
/////////////////////////////////////////////////////////////

SO_ENGINE_SOURCE(ZAngleFromRotationEngine);

//
// Description:
//    This initializes the ZAngleFromRotationEngine class.
//
// Use: internal
//

void
ZAngleFromRotationEngine::initClass()
{
    SO__ENGINE_INIT_CLASS(ZAngleFromRotationEngine,
            "ZAngleFromRotationEngine", SoEngine);
}

//
// Description:
//    Constructor
//
// Use: public
//

ZAngleFromRotationEngine::ZAngleFromRotationEngine()
{
    SO_ENGINE_CONSTRUCTOR(ZAngleFromRotationEngine);
    SO_ENGINE_ADD_INPUT(inRotation,     (SbRotation::identity()));

    SO_ENGINE_ADD_OUTPUT(outAngle,      SoSFFloat);
    isBuiltIn = TRUE;
}

//
// Description:
//    Destructor
//
// Use: private
//

ZAngleFromRotationEngine::~ZAngleFromRotationEngine()
{
}

//
// Description:
//    Evaluation routine
//
// Use: private
//

void
ZAngleFromRotationEngine::evaluate()
{
    if ( outAngle.getNumConnections() ) {
	// Decompose the rotation into angle and axis.
	SbVec3f theAxis;
	float   theAngle;
	SbRotation theRot = inRotation.getValue();
	theRot.getValue( theAxis, theAngle );

	// We want an angle about the positive Z axis.
	// If the axis is negative, we need to flip the angle. 
	if ( theAxis[2] < 0 )
	    theAngle *= -1.0;

	SO_ENGINE_OUTPUT(outAngle, SoSFFloat, setValue( theAngle ) );
    }
}

