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
 |   Classes:
 |      GeneralizedCylinder
 |
 |   Author(s)          : Paul Isaacs
 |
 */

#include <stdio.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SoDB.h>
#include <Inventor/nodekits/SoAppearanceKit.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoFaceSet.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoQuadMesh.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoTextureCoordinateBinding.h>
#include <Inventor/nodes/SoTextureCoordinate2.h>
#include <Inventor/nodes/SoTexture2Transform.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTriangleStripSet.h>

#include <Inventor/manips/SoTransformManip.h>
#include <Inventor/draggers/SoDragger.h>
#include <Inventor/nodes/SoSurroundScale.h>

#include <Inventor/actions/SoWriteAction.h>

#include <Inventor/sensors/SoNodeSensor.h>

#include "GeneralizedCylinder.h"
#include "Triangulator.h"
#include "NurbMaker.h"
#include <Inventor/nodes/SoVertexShape.h>


SO_KIT_SOURCE(GeneralizedCylinder);


////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the GeneralizedCylinder class.
//
// Use: internal

void
GeneralizedCylinder::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__KIT_INIT_CLASS(GeneralizedCylinder, "GeneralizedCylinder",
            SoSeparatorKit);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

GeneralizedCylinder::GeneralizedCylinder()
//
////////////////////////////////////////////////////////////////////////
{
    SO_KIT_CONSTRUCTOR(GeneralizedCylinder);

    SO_KIT_ADD_FIELD(renderShapeType,    (FACE_SET));
    SO_KIT_ADD_FIELD(normsFlipped,       (FALSE));
    SO_KIT_ADD_FIELD(profileClosed,      (FALSE));
    SO_KIT_ADD_FIELD(crossSectionClosed, (FALSE));
    SO_KIT_ADD_FIELD(spineClosed,        (FALSE));
    SO_KIT_ADD_FIELD(twistClosed,        (FALSE));
    SO_KIT_ADD_FIELD(minNumRows,         (1));
    SO_KIT_ADD_FIELD(minNumCols,         (1));
    SO_KIT_ADD_FIELD(withSides,          (TRUE));
    SO_KIT_ADD_FIELD(withTopCap,         (TRUE));
    SO_KIT_ADD_FIELD(withBottomCap,      (TRUE));
    SO_KIT_ADD_FIELD(withTextureCoords,  (FALSE));

    SO_KIT_DEFINE_ENUM_VALUE(RenderShapeType,	FACE_SET);
    SO_KIT_DEFINE_ENUM_VALUE(RenderShapeType,	TRIANGLE_STRIP_SET);
    SO_KIT_DEFINE_ENUM_VALUE(RenderShapeType,	QUAD_MESH);
    SO_KIT_DEFINE_ENUM_VALUE(RenderShapeType,	BEZIER_SURFACE);
    SO_KIT_DEFINE_ENUM_VALUE(RenderShapeType,	CUBIC_SPLINE_SURFACE);
    SO_KIT_DEFINE_ENUM_VALUE(RenderShapeType,	CUBIC_TO_EDGE_SURFACE);
    SO_KIT_SET_SF_ENUM_TYPE(renderShapeType,	RenderShapeType);

    // The input half of the scene graph
    SO_KIT_ADD_CATALOG_ENTRY(inputSwitch,SoSwitch,TRUE,topSeparator,childList,FALSE );
    SO_KIT_ADD_CATALOG_ENTRY(profileCoords,SoCoordinate3,TRUE,inputSwitch,,TRUE );
    SO_KIT_ADD_CATALOG_ENTRY(crossSectionCoords,SoCoordinate3,TRUE,inputSwitch,,TRUE );
    SO_KIT_ADD_CATALOG_ENTRY(spineCoords,SoCoordinate3,TRUE,inputSwitch,,TRUE );
    SO_KIT_ADD_CATALOG_ENTRY(twistCoords,SoCoordinate3,TRUE,inputSwitch,,TRUE );

    // The output half of the scene graph
    // ( the shape produced during 'updateSurface' based on the contents
    // of the input half of the scene graph
    SO_KIT_ADD_CATALOG_ENTRY(outputGrp,SoGroup,TRUE,topSeparator,childList,FALSE );
    SO_KIT_ADD_CATALOG_ENTRY(shapeHints,SoShapeHints,TRUE,outputGrp,,TRUE );
    SO_KIT_ADD_CATALOG_ENTRY(textureBinding,SoTextureCoordinateBinding,TRUE,outputGrp,,TRUE );
    SO_KIT_ADD_CATALOG_ENTRY(textureCoords,SoTextureCoordinate2,TRUE,outputGrp,,TRUE );
    SO_KIT_ADD_CATALOG_ENTRY(texture2,SoTexture2,TRUE,outputGrp,,TRUE );
    SO_KIT_ADD_CATALOG_ENTRY(coords,SoCoordinate3,TRUE,outputGrp,,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(faceSet,SoFaceSet,TRUE,outputGrp,,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(quadMesh,SoQuadMesh,TRUE,outputGrp,,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(nurbsSurfaceGroup,SoGroup,TRUE,outputGrp,,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(triangleStripSet,SoTriangleStripSet,TRUE,outputGrp,,TRUE);
    SO_KIT_ADD_CATALOG_ENTRY(capTextureBinding,SoTextureCoordinateBinding,TRUE,outputGrp,,TRUE );
    SO_KIT_ADD_CATALOG_ENTRY(capTextureCoords,SoTextureCoordinate2,TRUE,outputGrp,,TRUE );
    SO_KIT_ADD_CATALOG_ENTRY(topCapCoords,SoCoordinate3,TRUE,outputGrp,,TRUE );
    SO_KIT_ADD_CATALOG_ENTRY(topCapFaces,SoIndexedFaceSet,TRUE,outputGrp,,TRUE );
    SO_KIT_ADD_CATALOG_ENTRY(bottomCapCoords,SoCoordinate3,TRUE,outputGrp,,TRUE );
    SO_KIT_ADD_CATALOG_ENTRY(bottomCapFaces,SoIndexedFaceSet,TRUE,outputGrp,,TRUE );

    SO_KIT_INIT_INSTANCE();


    //////////////////////////////////////////
    // Set up the input half of the subgraph
    //////////////////////////////////////////
    SoSwitch *crvSwitch = new SoSwitch;
    crvSwitch->whichChild = SO_SWITCH_NONE;
    setAnyPart( "inputSwitch",crvSwitch,TRUE );

    setPart( "profileCoords",new SoCoordinate3 );
    setPart( "crossSectionCoords",new SoCoordinate3 );
    setPart( "spineCoords", new SoCoordinate3 );
    setPart( "twistCoords", new SoCoordinate3 );

    // Set the default values for these curves...
    set("profileCoords      { point [ 1 -1 0, 1 1  0 ] }" );
    set("crossSectionCoords { point [ 1  0 1, 1 0 -1, -1 0 -1, -1 0 1 ] }" );
    set("spineCoords        { point [ 0 -1 0, 0 1  0 ] }" );
    set("twistCoords        { point [ 0 -1 0, 0 1  0 ] }" );



    //////////////////////////////////////////
    // Set up the flags and parameters
    //////////////////////////////////////////

    fullProfile = new SoCoordinate3;
    fullCrossSection = new SoCoordinate3;
    fullProfile->ref();
    fullCrossSection->ref();

    profileAlreadyClosed = FALSE;
    crossSectionAlreadyClosed = FALSE;
    spineAlreadyClosed = FALSE;
    twistAlreadyClosed = FALSE;
    alreadyWithTextureCoords = FALSE;

    //////////////////////////////////////////
    // Set up the output half of the subgraph
    // that needs to be ready ahead of time.
    //////////////////////////////////////////

    //
    // This combination of shape hints will turn on two-sided lighting
    // for us (if it is available)
    //
    SoShapeHints *hints = new SoShapeHints;
    hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    hints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
    hints->faceType = SoShapeHints::CONVEX; 

    hints->creaseAngle = M_PI/6.0;
    setPart( "shapeHints", hints );

    topCapScratchFace = new SoIndexedFaceSet;
    topCapScratchFace->ref();

    bottomCapScratchFace = new SoIndexedFaceSet;
    bottomCapScratchFace->ref();

    numAllocedSpineKeyRots = 0;
    spineKeyPointRotations = NULL;
    spineParamDistances = NULL;

    // Initialize our nurbMaker
    myNurbMaker = NULL;

    selfSensor = new SoNodeSensor( GeneralizedCylinder::fieldsChangedCB, this);
    selfSensor->setPriority(0);

    inputSensor = new SoNodeSensor( GeneralizedCylinder::inputChangedCB, this);
    inputSensor->setPriority(0);

    setUpConnections(TRUE,TRUE);
}

//    detach/attach any sensors, callbacks, and/or field connections.
//    Called by:            start/end of SoBaseKit::readInstance
//    and on new copy by:   start/end of SoBaseKit::copy.
//    Classes that redefine must call setUpConnections(TRUE,TRUE) 
//    at end of constructor.
//    Returns the state of the node when this was called.
SbBool
GeneralizedCylinder::setUpConnections( SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( onOff ) {
	// We connect AFTER base class.
	SoSeparatorKit::setUpConnections( onOff, doItAlways );

	// Turn sensors on...
	SoNode *iSw = inputSwitch.getValue();
	if (iSw && inputSensor->getAttachedNode() != iSw)
	    inputSensor->attach(iSw);
	if (selfSensor->getAttachedNode() != this)
	    selfSensor->attach(this);
	updateSurface();
    }
    else {

	// We disconnect BEFORE base class.
	if (selfSensor->getAttachedNode())
	    selfSensor->detach();
	if (inputSensor->getAttachedNode())
	    inputSensor->detach();

	SoSeparatorKit::setUpConnections( onOff, doItAlways );
    }
    return !(connectionsSetUp = onOff);
}

void	
GeneralizedCylinder::setDefaultOnNonWritingFields()
{
    // Before writing, call setDefault(TRUE) on all parts 
    // under the output group.
    outputGrp.setDefault(TRUE);
    shapeHints.setDefault(TRUE);
    textureBinding.setDefault(TRUE);
    textureCoords.setDefault(TRUE);
    texture2Transform.setDefault(TRUE);
    coords.setDefault(TRUE);
    faceSet.setDefault(TRUE);
    quadMesh.setDefault(TRUE);
    nurbsSurfaceGroup.setDefault(TRUE);
    triangleStripSet.setDefault(TRUE);
    capTextureBinding.setDefault(TRUE);
    capTextureCoords.setDefault(TRUE);
    topCapCoords.setDefault(TRUE);
    topCapFaces.setDefault(TRUE);
    bottomCapCoords.setDefault(TRUE);
    bottomCapFaces.setDefault(TRUE);

    // Call base class...
    SoSeparatorKit::setDefaultOnNonWritingFields();
}

//
// Destructor.  Must unref everything reffed in the constructor.
//
GeneralizedCylinder::~GeneralizedCylinder()
{
    if ( topCapScratchFace != NULL) {
	topCapScratchFace->unref();
	topCapScratchFace = NULL;
    }
    if ( bottomCapScratchFace != NULL) {
	bottomCapScratchFace->unref();
	bottomCapScratchFace = NULL;
    }

    if ( fullProfile != NULL ) {
	fullProfile->unref();
	fullProfile = NULL;
    }
    if ( fullCrossSection != NULL ) {
	fullCrossSection->unref();
	fullProfile = NULL;
    }

     if ( selfSensor != NULL ) {
	delete selfSensor;
	selfSensor = NULL;
     }

     if ( inputSensor != NULL ) {
	delete inputSensor;
	inputSensor = NULL;
     }
     if (myNurbMaker != NULL)
	delete myNurbMaker;
}


void 
GeneralizedCylinder::loadRow( int rowNum, SbVec3f *newCoords )
{
    float   scale, paramDist;
    SbVec3f center;
    SbRotation rot;
    SbRotation twistRot;

    scale = fullProfile->point[rowNum][0];
    paramDist = (fullProfile->point[rowNum][1] - profileMinY) 
		* profileOverHeight;

    getSpineInfo( paramDist, center, rot );

    getTwistInfo( paramDist, twistRot );

    SbMatrix changer, temp;
    changer.setRotate( twistRot );
    temp.setScale( scale );
    changer.multRight( temp );

    temp.setRotate( rot );

    changer.multRight( temp );
    temp.setTranslate( center );
    changer.multRight( temp );

    int numCrossPts = fullCrossSection->point.getNum();
    for ( int i = 0; i < numCrossPts; i++ ) 
	changer.multVecMatrix( fullCrossSection->point[i], newCoords[i] );
}

void 
GeneralizedCylinder::loadTextureRow( int rowNum, SbVec2f *newCoords )
{
    float     uVal, vVal;
    SoMFVec3f *cSC = &fullCrossSection->point;

    if ( rowNum == -1 ) {
	// special case.  This is for the caps of the object.
	for ( int i = 0; i < cSC->getNum(); i++ )  {
	    uVal = ( (*cSC)[i][0] - crossSectionMinX ) / crossSectionWidth;
	    vVal = 1 - (( (*cSC)[i][2] - crossSectionMinZ) / crossSectionDepth);
	    newCoords[i].setValue( uVal, vVal );
	}
	return;
    }

    // vVal is distance parametrically along the spine...
    vVal = (fullProfile->point[rowNum][1] - profileMinY) 
		* profileOverHeight;

    SbVec3f   diff;
    int       numCrossPts = cSC->getNum();
    float     thisLength = 0.0;
    for ( int i = 0; i < numCrossPts; i++ )  {
	if ( i > 0 ) {
	    diff = (*cSC)[i] - (*cSC)[i-1];
	    thisLength += diff.length();
	}

	uVal = thisLength / crossSectionLength;

	newCoords[i].setValue( uVal, vVal );
    }
}

void 
GeneralizedCylinder::getSpineInfo( float paramDist, SbVec3f &center, 
				   SbRotation &rot )
{
    if ( spineLength == 0.0 ) {
	center = SbVec3f( 0,paramDist * profileHeight,0);
	rot = SbRotation::identity();
	return;
    }
	
    float desiredDist = paramDist * spineLength;

    float lengthSoFar = 0.0;
    float nextLength = 0.0;
    SbVec3f p1, p2, diff, norm1, norm2, norm;
    SbVec3f zAxis( 0, 0, 1 );

    SoCoordinate3 *spinC = SO_GET_PART( this, "spineCoords", SoCoordinate3 );

    if ( paramDist > 1.0 ) {
	int last = spinC->point.getNum() - 1;
	center = spinC->point[ last ];
	rot = getSpinePointRotation( last, last, 0.0 );
	return;
    }
    else if ( paramDist < 0.0 ) {
	center = spinC->point[ 0 ];
	rot = getSpinePointRotation( 0, 0, 0.0 );
	return;
    }

    for ( int i = 0; i < spinC->point.getNum() - 1; i++ ) {
	
	p1 = spinC->point[i];
	p2 = spinC->point[i+1];
	diff = p2 - p1;
	nextLength = lengthSoFar + diff.length();

	if ( desiredDist >= lengthSoFar && desiredDist <= nextLength ) {
	    float fraction = (desiredDist - lengthSoFar) 
			     / (nextLength - lengthSoFar);
	    center = p1 + fraction * ( p2 - p1 );
	    rot = getSpinePointRotation( i, i+1, fraction );
	    return;
	}

	lengthSoFar = nextLength;
    }
}

SbRotation
GeneralizedCylinder::getSpinePointRotation(int ind0, int ind1, float paramDist )
{
    SbRotation rot0, rot1, interpedRot;

    if ( ind0 == ind1 )
	return( spineKeyPointRotations[ind0]);
    else {
	rot0 = spineKeyPointRotations[ind0];
	rot1 = spineKeyPointRotations[ind1];

	interpedRot = SbRotation::slerp( rot0, rot1, paramDist );

	return interpedRot;
    }
}

void 
GeneralizedCylinder::initSpineKeyPointRotations()
{
    SoCoordinate3 *spinC = SO_GET_PART( this, "spineCoords", SoCoordinate3 );

    if ( spinC->point.getNum() > numAllocedSpineKeyRots ) {
	if ( spineKeyPointRotations != NULL )
	    delete [] spineKeyPointRotations;
	spineKeyPointRotations = new SbRotation[spinC->point.getNum()];
        numAllocedSpineKeyRots = spinC->point.getNum();
    }

    if (spinC->point.getNum() <= 0)
	return;

    if (spinC->point.getNum() == 1) {
	spineKeyPointRotations[0] = SbRotation::identity();
	return;
    }

    SbVec3f xAxis( 1, 0, 0 );
    SbVec3f yAxis( 0, 1, 0 );
    SbVec3f zAxis( 0, 0, 1 );
    SbVec3f zeroVec( 0, 0, 0 );
    SbVec3f diff1, diff2;

    if (spinC->point.getNum() == 2) {
	diff1 = spinC->point[1] - spinC->point[0];
	diff1.normalize();
	spineKeyPointRotations[0].setValue( yAxis, diff1 );
	spineKeyPointRotations[1] = spineKeyPointRotations[0];
	return;
    }

    // More than two points...
    SbVec3f rotatedX, rotatedY, rotatedZ, prevRotatedZ;
    SbVec3f p0, p1, p2;
    int     lastInd = spinC->point.getNum() - 1;

    // Find rotation for First point...

	// First, treat case where spine is a closed polygon
	if ( spineClosed.getValue() ) {
	    // Z axis is perp to plane formed by three points around p[0]
	    diff1 = spinC->point[lastInd-1] - spinC->point[0];
	    diff2 = spinC->point[1] - spinC->point[0];
	    rotatedZ = diff2.cross( diff1 );
	    // If 3 pts colinear, don't do this, but go on to other algorithm...
	    if ( rotatedZ != zeroVec ) {
		rotatedZ.normalize();
		// But make it lean towards the Z axis of the previous point.
		if ( rotatedZ.dot( zAxis ) < 0 )
		    rotatedZ *= -1.0;

		// X axis is cross product of (nextPt-prevPt) with Z axis.
		diff1 = spinC->point[1] - spinC->point[lastInd-1];
		rotatedX = diff1.cross( rotatedZ );
		rotatedX.normalize();

		rotatedY = rotatedZ.cross( rotatedX );
	    }
	}

	// Spine not closed, or it was closed but couldn't get rotation...
	if ( rotatedZ == zeroVec || !spineClosed.getValue() ) {
	    // Y axis just points from point 0 to first point that differs.
	    int i = 0;
	    p0 = p1 = spinC->point[i++];
	    while (p0 == p1 && i <= lastInd )
		p1 = spinC->point[i++];
	    if ( p0 == p1 ) {
		// all points were the same. Make everyone have no rotation.
		for ( int j = 0; j <= lastInd; j++ ) {
		    spineKeyPointRotations[j] = SbRotation::identity();
		}
		return;
	    }
	    rotatedY = p1 - p0;
	    rotatedY.normalize();

	    // Z axis is perpendicular to plane formed by 1st three points.
	    // Repeat as long as the two vectors are colinear.
	    diff1 = p0 - p1;
	    diff1.normalize();
	    rotatedZ = zeroVec;
	    while (rotatedZ == zeroVec && i <= lastInd ) {
		p2 = spinC->point[i++];
		diff2 = p2 - p1;
		diff2.normalize();
		rotatedZ = diff2.cross( diff1 );
	    }
	    if ( rotatedZ == zeroVec ) {
		// all points coplanar. Make everyone have the same rotation.
		for ( int j = 0; j <= lastInd; j++ ) {
		    spineKeyPointRotations[j].setValue( yAxis, rotatedY );
		}
		return;
	    }
	    rotatedZ.normalize();
	    // But make it lean towards positive Z each time.
	    if ( rotatedZ.dot( zAxis ) < 0 )
		rotatedZ *= -1.0;

	    // X axis is calculated based on Y and Z
	    rotatedX = rotatedY.cross( rotatedZ );
	}

	SbMatrix firstAnswerMat(rotatedX[0],rotatedX[1],rotatedX[2],0,
			        rotatedY[0],rotatedY[1],rotatedY[2],0,
			        rotatedZ[0],rotatedZ[1],rotatedZ[2],0,
	                                 0 ,         0 ,         0 ,1);
	spineKeyPointRotations[0].setValue(firstAnswerMat);


    prevRotatedZ = rotatedZ;

    // Find rotation for all but first and last points...
    for ( int ind = 1; ind < lastInd; ind++ ) {
	// Z axis is perpendicular to plane formed by three points surrounding
	// This one.
	diff1 = spinC->point[ind-1] - spinC->point[ind];
	diff2 = spinC->point[ind+1] - spinC->point[ind];
	diff1.normalize();
	diff2.normalize();
	rotatedZ = diff2.cross( diff1 );
	if (rotatedZ == zeroVec)      // if colinear, use previous rotatedZ
	    rotatedZ = prevRotatedZ;
	rotatedZ.normalize();
	// But make it lean towards the Z axis of the previous point.
	if ( rotatedZ.dot( prevRotatedZ ) < 0.0 )
	    rotatedZ *= -1.0;

	// X axis is cross product of (nextPt-prevPt) with Z axis.
	diff1 = spinC->point[ind+1] - spinC->point[ind-1];
	if ( diff1 != zeroVec ) {
	    diff1.normalize();
	    rotatedX = diff1.cross( rotatedZ );
	    rotatedX.normalize();
	}
	// if diff1 = 0, just use previous value for rotatedX

	// Y axis is calculated based on X and Z
	rotatedY = rotatedZ.cross( rotatedX );

	SbMatrix answerMat(rotatedX[0],rotatedX[1],rotatedX[2],0,
	                   rotatedY[0],rotatedY[1],rotatedY[2],0,
	                   rotatedZ[0],rotatedZ[1],rotatedZ[2],0,
	                            0 ,         0 ,         0 ,1);
	spineKeyPointRotations[ind].setValue(answerMat);

	prevRotatedZ = rotatedZ;
    }

    // Find rotation for Last point...
	if ( spineClosed.getValue() ) {
	    spineKeyPointRotations[lastInd] = spineKeyPointRotations[0];
	}
	else {
	    // Y axis just points from next-to-last point to last point
	    diff1 = spinC->point[lastInd] - spinC->point[lastInd - 1];
	    if (diff1 != zeroVec )
		rotatedY = diff1;
	    // otherwise use previous value...
	    rotatedY.normalize();

	    // Z axis is same as previous value.
	    rotatedZ = prevRotatedZ;

	    // X axis is calculated based on Y and Z
	    rotatedX = rotatedY.cross( rotatedZ );

	    SbMatrix lastAnswerMat(rotatedX[0],rotatedX[1],rotatedX[2],0,
				   rotatedY[0],rotatedY[1],rotatedY[2],0,
				   rotatedZ[0],rotatedZ[1],rotatedZ[2],0,
					    0 ,         0 ,         0 ,1);
	    spineKeyPointRotations[lastInd].setValue(lastAnswerMat);
	}
}

void 
GeneralizedCylinder::getTwistInfo( float paramDist, SbRotation &twistRot )
{
    // X coordinate determines amount of twist

    float desiredHeight;

    desiredHeight = paramDist * (twistMaxY - twistMinY) + twistMinY;

    if (desiredHeight < twistMinY)
	desiredHeight = twistMinY;
    else if (desiredHeight > twistMaxY)
	desiredHeight = twistMaxY;

    float thisHeight, nextHeight, thisAngle, nextAngle;
    float angle, fraction;
    SbVec3f yAxis( 0, 1, 0 );

    SoCoordinate3 *twisC = SO_GET_PART( this, "twistCoords", SoCoordinate3 );

    for ( int i = 0; i < twisC->point.getNum() - 1; i++ ) {
	
	thisHeight = twisC->point[i][1];
	nextHeight = twisC->point[i+1][1];

	if ( desiredHeight >= thisHeight && desiredHeight <= nextHeight ) {
	    fraction = (desiredHeight - thisHeight) / (nextHeight - thisHeight);
	    thisAngle = twisC->point[i][0];
	    nextAngle = twisC->point[i+1][0];
	    angle = thisAngle + fraction * ( nextAngle - thisAngle );

	    twistRot = SbRotation( yAxis, angle );
	    return;
	}
	if ( desiredHeight >= nextHeight && desiredHeight <= thisHeight ) {
	    fraction = (desiredHeight - nextHeight) / (thisHeight - nextHeight);
	    thisAngle = twisC->point[i][0];
	    nextAngle = twisC->point[i+1][0];
	    angle = nextAngle + fraction * ( thisAngle - nextAngle );

	    twistRot = SbRotation( yAxis, angle );
	    return;
	}
    }

    twistRot = SbRotation::identity();
    return;
}

void 
GeneralizedCylinder::initUpdateInfo()
{
    profileMinY = 100000000;
    float maxY = -100000000;

    float val;

    SoCoordinate3 *profC = SO_GET_PART( this, "profileCoords", SoCoordinate3 );
    SoCoordinate3 *crosC = SO_GET_PART( this, "crossSectionCoords", SoCoordinate3 );
    SoCoordinate3 *spinC = SO_GET_PART( this, "spineCoords", SoCoordinate3 );
    SoCoordinate3 *twisC = SO_GET_PART( this, "twistCoords", SoCoordinate3 );

    int i;
    for ( i = 0; i < profC->point.getNum(); i++ ) {
	val = profC->point[i][1];
	if ( profileMinY > val )
	    profileMinY = val;
	if ( maxY < val )
	    maxY = val;
    }

    if ( maxY == profileMinY )
	maxY += 1.0;
    profileHeight = maxY - profileMinY; 
    profileOverHeight = 1.0 / profileHeight;

    //Spine length, and parametric spine distance of each point on spine.
    if ( spineParamDistances != NULL )
	delete [] spineParamDistances;
    spineParamDistances = new float[spinC->point.getNum()];
    spineLength = 0.0;
    spineParamDistances[0] = 0;
    SbVec3f diff;
    for ( i = 0; i < spinC->point.getNum() - 1; i++ ) {
	diff = spinC->point[i+1] - spinC->point[i];
	spineLength += diff.length();
	spineParamDistances[i+1] = spineLength; // absolute distance
    }
    if ( spineLength != 0.0 ) {
	// convert absolute distance into parametric distance.
	for ( i = 0; i < spinC->point.getNum(); i++ )
	    spineParamDistances[i] = spineParamDistances[i] / spineLength;
    }

    // Rotations at key points of the spine...
    initSpineKeyPointRotations();

    //Profile length
    profileLength = 0.0;
    for ( i = 0; i < profC->point.getNum() - 1; i++ ) {
	diff = profC->point[i+1] - profC->point[i];
	profileLength += diff.length();
    }

    //CrossSection length
    crossSectionLength = 0.0;
    for ( i = 0; i < crosC->point.getNum() - 1; i++ ) {
	diff = crosC->point[i+1] - crosC->point[i];
	crossSectionLength += diff.length();
    }

    float xVal, zVal;
    float maxX = -100000000;
    float maxZ = -100000000;
    crossSectionMinX = crossSectionMinZ = 100000000;

    for ( i = 0; i < crosC->point.getNum(); i++ ) {
	xVal = crosC->point[i][0];
	if ( crossSectionMinX > xVal )
	    crossSectionMinX = xVal;
	if ( maxX < xVal )
	    maxX = xVal;

	zVal = crosC->point[i][2];
	if ( maxZ < zVal )
	    maxZ = zVal;
	if ( crossSectionMinZ > zVal )
	    crossSectionMinZ = zVal;
    }

    if ( maxX == crossSectionMinX )
	maxX += 1.0;
    if ( maxZ == crossSectionMinZ )
	maxZ += 1.0;
    crossSectionWidth = maxX - crossSectionMinX; 
    crossSectionDepth = maxZ - crossSectionMinZ; 



    twistMinY = 100000000;
    twistMaxY = -100000000;
    for ( i = 0; i < twisC->point.getNum(); i++ ) {
	val = twisC->point[i][1];
	if ( twistMinY > val )
	    twistMinY = val;
	if ( twistMaxY < val )
	    twistMaxY = val;
	val = twisC->point[i][0];
    }

    calculateFullProfile();
    calculateFullCrossSection();
}

//
// Given a set of 2D profile coordinates (the z-coordinate of this
// Vec3f field is ignored), this creates a generalized cylinder
// If passed NULL, this routine will use the last coordinates passed
// (this is used when the number of sides in the generalized cylinder is
// changed).
//

void
GeneralizedCylinder::updateSurface()
{
    initUpdateInfo();

    const SoMFVec3f *prof =  &fullProfile->point;
    const SoMFVec3f *cross = &fullCrossSection->point;

    int numRows = prof->getNum();
    int numCols = cross->getNum();

    SoFaceSet          *myFaceSet = NULL;
    SoTriangleStripSet *myTStripSet = NULL;
    SoQuadMesh         *myQMesh = NULL;
    SoGroup            *myNurbsSurfs = NULL;

    GeneralizedCylinder::RenderShapeType sType
        = (GeneralizedCylinder::RenderShapeType)renderShapeType.getValue();

    // DEAL WITH SHAPE HINTS
    SoShapeHints *hints = (SoShapeHints *) getPart("shapeHints", FALSE);
    if (hints) {
	switch(sType) {
	    case BEZIER_SURFACE:
	    case CUBIC_SPLINE_SURFACE:
	    case CUBIC_TO_EDGE_SURFACE:
		hints->vertexOrdering = SoShapeHints::UNKNOWN_ORDERING;
		break;
	    case FACE_SET:
	    case TRIANGLE_STRIP_SET:
	    case QUAD_MESH:
	        hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
		break;
        }
    }

    SoCoordinate3 *coords = NULL;

    // DELETE AND GET THE CORRECT NODES.
    if (withSides.getValue() == FALSE ) {
	setPart( "faceSet", NULL );
	setPart( "triangleStripSet", NULL );
	setPart( "quadMesh", NULL );
	setPart( "nurbsSurfaceGroup", NULL );
	setPart( "coords", NULL );
    }
    else {
	switch(sType) {
	    case FACE_SET:
		myFaceSet = SO_GET_PART( this, "faceSet", SoFaceSet );
		setPart( "triangleStripSet", NULL );
		setPart( "quadMesh", NULL );
		setPart( "nurbsSurfaceGroup", NULL );
		break;
	    case TRIANGLE_STRIP_SET:
		myTStripSet 
		    = SO_GET_PART(this,"triangleStripSet", SoTriangleStripSet );
		setPart( "faceSet", NULL );
		setPart( "quadMesh", NULL );
		setPart( "nurbsSurfaceGroup", NULL );
		break;
	    case QUAD_MESH:
		myQMesh = SO_GET_PART( this, "quadMesh", SoQuadMesh );
		setPart( "faceSet", NULL );
		setPart( "triangleStripSet", NULL );
		setPart( "nurbsSurfaceGroup", NULL );
		break;
	    case BEZIER_SURFACE:
	    case CUBIC_SPLINE_SURFACE:
	    case CUBIC_TO_EDGE_SURFACE:
		myNurbsSurfs = SO_GET_PART(this, "nurbsSurfaceGroup", SoGroup );
		setPart( "faceSet", NULL );
		setPart( "triangleStripSet", NULL );
		setPart( "quadMesh", NULL );
		break;
	}
        coords  = SO_GET_PART( this, "coords", SoCoordinate3 );
    }


    // FORGET IT IF TOO FEW POINTS
    if ( numRows < 2 || numCols < 2 )  {
	setPart("faceSet", NULL);
	setPart("triangleStripSet", NULL );
	setPart("quadMesh", NULL );
	setPart("nurbsSurfaceGroup", NULL );

	setPart("topCapFaces", NULL);
	setPart("bottomCapFaces", NULL);

	if (withSides.getValue())
	    coords->point.deleteValues( 0, -1 );
	updateSurroundingManip();
	return;
    }

    // SET UP TOPOLOGICAL INFORMATION
    int      numCoordsTotal;
    SoMFInt32 *nV;
    if ( withSides.getValue() ) {
	switch (sType ) {
	    case QUAD_MESH:
		// There will be (numRows * numCols) coordinates
		numCoordsTotal = numRows * numCols;
		myQMesh->verticesPerColumn = numRows;
		myQMesh->verticesPerRow = numCols;
		if (myQMesh->startIndex.getValue() != 0)
		    myQMesh->startIndex = 0;
		break;
	    case TRIANGLE_STRIP_SET:
		// Each strip (there are numRows-1 in all) has 2*numCols coordinates
		numCoordsTotal = (numRows - 1) * (numCols * 2);

		nV = &myTStripSet->numVertices;

		if (nV->getNum() != (numRows-1)) {
		    nV->setNum(numRows-1);
		    for (int k = 0; k < (numRows-1); k++ )
			nV->set1Value( k, (numCols*2) );
		}
		if (myTStripSet->startIndex.getValue() != 0)
		    myTStripSet->startIndex = 0;
		break;
	    case FACE_SET:
		// Each square is made of 2 tris, drawn with 3 coords. 
		{
		    int numTris = 2 * (numRows - 1) * (numCols - 1);
		    numCoordsTotal = numTris * 3;

		    nV = &myFaceSet->numVertices;
		    if (nV->getNum() != numTris) {
			nV->setNum(numTris);
			for (int k = 0; k < numTris; k++ )
			    nV->set1Value( k, 3 );
		    }
		    if (myFaceSet->startIndex.getValue() != 0)
			myFaceSet->startIndex = 0;
		}
		break;
	    case BEZIER_SURFACE:
	    case CUBIC_SPLINE_SURFACE:
	    case CUBIC_TO_EDGE_SURFACE:
		{
		// There will be (numRows * numCols) coordinates
		numCoordsTotal = numRows * numCols;

		// Enlist a NurbMaker to create the surface
		// patch nodes.
		if (myNurbMaker == NULL)
		    myNurbMaker = new NurbMaker;
		if (sType == BEZIER_SURFACE)
		    myNurbMaker->setPatchType( NurbMaker::BEZIER );
		else if (sType == CUBIC_SPLINE_SURFACE) 
		    myNurbMaker->setPatchType( NurbMaker::CUBIC );
		else 
		    myNurbMaker->setPatchType( NurbMaker::CUBIC_TO_EDGE );
		myNurbMaker->setFlipNormals( normsFlipped.getValue() );

		SbVec2s closure( spineClosed.getValue(),
				    crossSectionClosed.getValue());
		myNurbMaker->setWraparound(closure);

		SbVec2s numMeshDivs(numCols, numRows);

		SoGroup *surfGroup;
		surfGroup = myNurbMaker->createNurbsGroup( numMeshDivs, closure );
		setPart("nurbsSurfaceGroup", surfGroup );
		}
		break;
	}

	if (coords->point.getNum() < numCoordsTotal)
	    coords->point.insertSpace(0, numCoordsTotal - coords->point.getNum());
	else if ( coords->point.getNum() > numCoordsTotal )
	    coords->point.deleteValues(0, coords->point.getNum() - numCoordsTotal );
    }

    int topRow, botRow;

    static SbVec3f *topPoints = NULL;
    static SbVec3f *botPoints = NULL;
    static SbVec2f *topTex = NULL;
    static SbVec2f *botTex = NULL;

    static int     p_alloc = 0;
    if ( p_alloc != numCols ) {
	if ( topPoints != NULL ) {
	    delete [] topPoints;
	    topPoints = NULL;
	}
	if ( botPoints != NULL ) {
	    delete [] botPoints;
	    botPoints = NULL;
	}
	if ( topTex != NULL ) {
	    delete [] topTex;
	    topTex = NULL;
	}
	if ( botTex != NULL ) {
	    delete [] botTex;
	    botTex = NULL;
	}
	p_alloc = 0;
    }
    if ( p_alloc == 0 ) {
	p_alloc = numCols;
	topPoints = new SbVec3f[p_alloc];
	botPoints = new SbVec3f[p_alloc];
	topTex = new SbVec2f[p_alloc];
	botTex = new SbVec2f[p_alloc];
    }


    // If either cap is being drawn, and texture is on...
    if (    withTextureCoords.getValue() 
	 && ( withTopCap.getValue()    || withBottomCap.getValue() ) 
	 && crossSectionClosed.getValue() && numCols >= 4 ) {
	 loadTextureRow( -1, topTex );
         SoTextureCoordinate2 *cTC  
		    = SO_GET_PART( this, "capTextureCoords", SoTextureCoordinate2 );
	 cTC->point.setValues( 0, numCols, topTex );
    }
    else {
	setPart("capTextureCoords", NULL);
    }

    SoCoordinate3 *topCapC = SO_GET_PART(this,"topCapCoords",SoCoordinate3);
    SoIndexedFaceSet *topCapF = SO_GET_PART(this,"topCapFaces", SoIndexedFaceSet);
    SoCoordinate3 *botCapC = SO_GET_PART(this,"bottomCapCoords",SoCoordinate3);
    SoIndexedFaceSet *botCapF =SO_GET_PART(this,"bottomCapFaces", SoIndexedFaceSet);

    if ( withTopCap.getValue() && crossSectionClosed.getValue() 
	 && numCols >= 4 ) {


	SoMFVec3f *topC = &topCapC->point;
	SoMFInt32  *topF = &topCapF->coordIndex;
	SoMFInt32  *topS = &topCapScratchFace->coordIndex;

	// Load the values of the section points into topCapC.
	// Leave out the last point, since it is a repeat of the first
	// (remember, crossSectionClosed == TRUE !)
	if ( numCols - 1 < topC->getNum() )
	    topC->deleteValues( 0, topC->getNum() - (numCols - 1) );
	topC->setValues( 0, numCols - 1, cross->getValues(0) );

	// The triangulator likes the y values to be 0.0. So flatten the 
	// sucker out.
	SbVec3f *bunchaTopVecs = topC->startEditing();
	SbBool topVecsChanged = FALSE;
	for (int topCount = 0; topCount < topC->getNum(); topCount++ ) {
	    SbVec3f theVec = (*topC)[topCount];
	    if (bunchaTopVecs[topCount][1] != 0.0) {
	        bunchaTopVecs[topCount][1] = 0.0;
		topVecsChanged = TRUE;
	    }
	}
	bunchaTopVecs = NULL;
	if (topVecsChanged)
	    topC->finishEditing();

	// Load the indices to draw the section as a single polygon into
	// topCapScratchFace
	if ( (topC->getNum() + 1) < topS->getNum() )
	    topS->deleteValues( 0, topS->getNum() - (topC->getNum() + 1) );
	else if ( (topC->getNum() + 1) > topS->getNum() )
	    topS->insertSpace( 0, (topC->getNum() + 1) - topS->getNum() );
	int32_t *scratchVals = topS->startEditing();
	for ( int i = 0; i < topC->getNum(); i++ ) {
	    scratchVals[i] = i;
	}
	scratchVals[ topC->getNum() ] = -1;  // to close the polygon
	topS->finishEditing();

	// Before calling loadRow (i.e., while the points still lay in the 
	// y=0 plane), and before triangulating (i.e., while we still have
	// a record of a polygon which follows the entire cross section)
	// determine if the cross section is clockwise ordered. This will
	// affect the normal vector we select.
	SbBool isSectionClockWise;
	isSectionClockWise = Triangulator::clockWiseTest(
					(*topC), (*topS), 0, topC->getNum());

	// Triangulate, putting result into topCapF
	Triangulator::triangulate( *topC, *topS, *topF );

	// Triangulation makes everything clockwise ordered.
	// Assure that normals are in the right direction...
	if ( normsFlipped.getValue() != isSectionClockWise ) {
	    // reverse the order of the points in topF.  
	    // Each polygon in topF should be a triangle at this point,
	    // so reversing the order is simple.
	    int32_t *vals = topF->startEditing();
	    int32_t swapTemp;
	    for ( int ind = 0; ind < topF->getNum(); ind += 4 ) {
		// Switch ind and ind+2 (ind+3 is a -1 to signal end of poly)
		swapTemp = vals[ind];
		vals[ind] = vals[ind+2];
		vals[ind+2] = swapTemp;
	    }
	    topF->finishEditing();
	}

	// Check that, after triangulation, there are still polygons left...
	if ( topF->getNum() > 0 ) {

	    // Transform the values for the points in the section as they
	    // appear in the final row 
	    topC->insertSpace( 0, 1 ); // Add room for an extra point at the 
				       // end since loadRow will put it there...
	    SbVec3f *topCapPoints = topC->startEditing();
	    loadRow( 0, topCapPoints );
	    topC->finishEditing();
	}
    }
    else {
	// Clear out the coord and face set for top cap
	topCapC->point.deleteValues( 0, -1 );
	topCapF->coordIndex.deleteValues( 0, -1 );
    }

    if ( withBottomCap.getValue() && crossSectionClosed.getValue() 
	 && numCols >= 4 ) {

	SoMFVec3f *botC = &botCapC->point;
	SoMFInt32  *botF = &botCapF->coordIndex;
	SoMFInt32  *botS = &bottomCapScratchFace->coordIndex;

	// Load the values of the section points into botCapC.
	// Leave out the last point, since it is a repeat of the first
	// (remember, crossSectionClosed == TRUE !)
	if ( numCols - 1 < botC->getNum() )
	    botC->deleteValues( 0, botC->getNum() - (numCols - 1) );
	botC->setValues( 0, numCols - 1, cross->getValues(0) );

	// The triangulator likes the y values to be 0.0. So flatten the 
	// sucker out.
	SbVec3f *bunchaBotVecs = botC->startEditing();
	SbBool botVecsChanged = FALSE;
	for (int botCount = 0; botCount < botC->getNum(); botCount++ ) {
	    SbVec3f theVec = (*botC)[botCount];
	    if (bunchaBotVecs[botCount][1] != 0.0) {
	        bunchaBotVecs[botCount][1] = 0.0;
		botVecsChanged = TRUE;
	    }
	}
	bunchaBotVecs = NULL;
	if (botVecsChanged)
	    botC->finishEditing();

	// Load the indices to draw the section as a single polygon into
	// bottomCapScratchFace
	if ( (botC->getNum() + 1) < botS->getNum() )
	    botS->deleteValues( 0, botS->getNum() - (botC->getNum() + 1) );
	else if ( (botC->getNum() + 1) > botS->getNum() )
	    botS->insertSpace( 0, (botC->getNum() + 1) - botS->getNum() );
	int32_t *scratchVals = botS->startEditing();
	for ( int i = 0; i < botC->getNum(); i++ ) {
	    scratchVals[i] = i;
	}
	scratchVals[ botC->getNum() ] = -1;  // to close the polygon
	botS->finishEditing();

	// Before calling loadRow (i.e., while the points still lay in the 
	// y=0 plane), and before triangulating (i.e., while we still have
	// a record of a polygon which follows the entire cross section)
	// determine if the cross section is clockwise ordered. This will
	// affect the normal vector we select.
	SbBool isSectionClockWise;
	isSectionClockWise = Triangulator::clockWiseTest(
					(*botC), (*botS), 0, botC->getNum());

	// Triangulate, putting result into bottomCapFaces
	Triangulator::triangulate( *botC, *botS, *botF );

	// Triangulation makes everything clockwise ordered.
	// Assure that normals are in the right direction...
	if ( normsFlipped.getValue() == isSectionClockWise ) {
	    // reverse the order of the points in botF.  
	    // Each polygon in topF should be a triangle at this point,
	    // so reversing the order is simple.
	    int32_t *vals = botF->startEditing();
	    int32_t swapTemp;
	    for ( int ind = 0; ind < botF->getNum(); ind += 4 ) {
		// Switch ind and ind+2 (ind+3 is a -1 to signal end of poly)
		swapTemp = vals[ind];
		vals[ind] = vals[ind+2];
		vals[ind+2] = swapTemp;
	    }
	    botF->finishEditing();
	}

	// Check that, after triangulation, there are still polygons left...
	if ( botF->getNum() > 0 ) {

	    // Transform the values for the points in the section as they
	    // appear in the final row 
	    botC->insertSpace( 0, 1 ); // Add room for an extra point at the 
				       // end, since loadRow will put it there.
	    SbVec3f *bottomCapPoints = botC->startEditing();
	    loadRow( numRows - 1, bottomCapPoints );
	    botC->finishEditing();
	}
    }

    else {
	// Clear out the coord and face set for bottom cap
	botCapC->point.deleteValues( 0, -1 );
	botCapF->coordIndex.deleteValues( 0, -1 );
    }



    if ( withSides.getValue() ) {

	SbVec2f *tCEdit;
	SoTextureCoordinate2 *tCN; 
	if ( withTextureCoords.getValue() ) {
	    tCN = SO_GET_PART(this, "textureCoords", SoTextureCoordinate2 );
	    if (tCN->point.getNum() < numCoordsTotal)
		tCN->point.insertSpace(0, numCoordsTotal - tCN->point.getNum());
	    else if ( tCN->point.getNum() > numCoordsTotal )
		tCN->point.deleteValues(0, tCN->point.getNum() - numCoordsTotal );
	    tCEdit = tCN->point.startEditing();
	}
	else {
	    setPart("textureCoords", NULL);
	}

	SbVec3f *coordsEdit = coords->point.startEditing();
	int curCoord = 0;
	int curTexCoord = 0;

	for (topRow = 0, botRow = 1; topRow < numRows - 1; topRow++ , botRow++ ){
	    if ( topRow == 0 ) {
		// calculate topRow
		loadRow( topRow, topPoints );
		if ( withTextureCoords.getValue() )
		    loadTextureRow( topRow, topTex );
	    }
	    else {
		// switch rows, so old botRow becomes new topRow
		SbVec3f *temp = topPoints;
		topPoints = botPoints;
		botPoints = temp;
		if ( withTextureCoords.getValue() ) {
		    SbVec2f *temp2 = topTex;
		    topTex = botTex;
		    botTex = temp2;
		}
	    }

	    // calculate botRow 
	    loadRow( botRow, botPoints );
	    if ( withTextureCoords.getValue() )
		loadTextureRow( botRow, botTex );


	    // put the coordinates for this row triangles into the node...
	    SbBool flipped = normsFlipped.getValue();

	    switch(sType) {
		case QUAD_MESH:
		case BEZIER_SURFACE:
		case CUBIC_SPLINE_SURFACE:
		case CUBIC_TO_EDGE_SURFACE:
		    // For these two, load only coords for the bottom row.
		    // However load the top row also the first time through
		    {
		    int curCol;
		    if (topRow == 0) {
			if ( flipped ) 
			    for (curCol = 0; curCol < numCols; curCol++ )
				coordsEdit[curCoord++] = topPoints[curCol];
			else
			    for (curCol = numCols-1; curCol >= 0; curCol-- )
				coordsEdit[curCoord++] = topPoints[curCol];
		    }

		    // load bottom row
		    if ( flipped ) 
			for (curCol = 0; curCol < numCols; curCol++ )
			    coordsEdit[curCoord++] = botPoints[curCol];
		    else
			for (curCol = numCols-1; curCol >= 0; curCol-- )
			    coordsEdit[curCoord++] = botPoints[curCol];
		    }
		    break;
		case TRIANGLE_STRIP_SET:
		    {
			for( int curCol = 0; curCol < numCols; curCol++ ) {
			    if ( ! flipped ) {
				coordsEdit[curCoord++] = topPoints[curCol];
				coordsEdit[curCoord++] = botPoints[curCol];
			    }
			    else {
				coordsEdit[curCoord++] = botPoints[curCol];
				coordsEdit[curCoord++] = topPoints[curCol];
			    }
			}
		    }
		    break;
		case FACE_SET:
		    int leftCol, rightCol;
		    for ( leftCol = 0,  rightCol = 1; leftCol < numCols - 1;
			 leftCol++, rightCol++ ) {

			if (  ! flipped ) {
			    // first draw the top left triangle
			    coordsEdit[curCoord++] = topPoints[leftCol];
			    coordsEdit[curCoord++] = topPoints[rightCol];
			    coordsEdit[curCoord++] = botPoints[leftCol];

			    // then draw the bottom right triangle
			    coordsEdit[curCoord++] = botPoints[leftCol];
			    coordsEdit[curCoord++] = topPoints[rightCol];
			    coordsEdit[curCoord++] = botPoints[rightCol];
			}
			else {
			    // first draw the top left triangle
			    coordsEdit[curCoord++] = botPoints[leftCol];
			    coordsEdit[curCoord++] = topPoints[rightCol];
			    coordsEdit[curCoord++] = topPoints[leftCol];

			    // then draw the bottom right triangle
			    coordsEdit[curCoord++] = botPoints[rightCol];
			    coordsEdit[curCoord++] = topPoints[rightCol];
			    coordsEdit[curCoord++] = botPoints[leftCol];
			}
		    }
		    break;
	    }

	    if ( withTextureCoords.getValue() ) {
		switch(sType) {
		    case QUAD_MESH:
		    case BEZIER_SURFACE:
		    case CUBIC_SPLINE_SURFACE:
		    case CUBIC_TO_EDGE_SURFACE:
			// For these two, load only coords for the bottom row.
			// However load the top row also the first time through
			{
			    int curCol;
			    if (topRow == 0) {
				if ( flipped ) 
				    for (curCol = 0; curCol < numCols; curCol++ )
					tCEdit[curTexCoord++] = topTex[curCol];
				else
				    for (curCol = numCols-1; curCol >= 0; curCol-- )
					tCEdit[curTexCoord++] = topTex[curCol];
			    }

			    // load bottom row
			    if ( flipped ) 
				for (curCol = 0; curCol < numCols; curCol++ )
				    tCEdit[curTexCoord++] = botTex[curCol];
			    else
				for (curCol = numCols-1; curCol >= 0; curCol-- )
				    tCEdit[curTexCoord++] = botTex[curCol];
			}
			break;
		    case TRIANGLE_STRIP_SET:
			{
			    for (int curCol=0; curCol < numCols;curCol++){
				if ( ! flipped ) {
				    tCEdit[ curTexCoord++] = topTex[curCol];
				    tCEdit[ curTexCoord++] = botTex[curCol];
				}
				else {
				    tCEdit[ curTexCoord++] = botTex[curCol];
				    tCEdit[ curTexCoord++] = topTex[curCol];
				}
			    }
			}
			break;
		    case FACE_SET:
			int leftCol, rightCol;

			// set the texture coordinates for these triangles...
			for ( leftCol = 0,  rightCol = 1; leftCol < numCols - 1;
			     leftCol++, rightCol++ ) {

			    if (  ! flipped ) {
				// first draw the top left triangle
				tCEdit[ curTexCoord++] = topTex[leftCol];
				tCEdit[ curTexCoord++] = topTex[rightCol];
				tCEdit[ curTexCoord++] = botTex[leftCol];

				// then draw the bottom right triangle
				tCEdit[ curTexCoord++] = botTex[leftCol];
				tCEdit[ curTexCoord++] = topTex[rightCol];
				tCEdit[ curTexCoord++] = botTex[rightCol];
			    }
			    else {
				// first draw the top left triangle
				tCEdit[ curTexCoord++] = botTex[leftCol];
				tCEdit[ curTexCoord++] = topTex[rightCol];
				tCEdit[ curTexCoord++] = topTex[leftCol];

				// then draw the bottom right triangle
				tCEdit[ curTexCoord++] = botTex[rightCol];
				tCEdit[ curTexCoord++] = topTex[rightCol];
				tCEdit[ curTexCoord++] = botTex[leftCol];
			    }
			}
			break;
		}
	    }
	}
	coords->point.finishEditing();
	if ( withTextureCoords.getValue() ) {
	    tCN->point.finishEditing();
	}
    }

    // If there's a manip as our transform, tell it to change size.
    updateSurroundingManip();
}

SoSeparator *
GeneralizedCylinder::makeVanillaVersion()
{
    // Start with a nice update!
    updateSurface();

    SoSeparator *answer = new SoSeparator;
    answer->ref();

    SoNode *newKid;

    int numCols = fullCrossSection->point.getNum();

    // Go through all the relevant parts. If you find them, add them as 
    // a child to the answer.

    if ( newKid = getPart( "callbackList", FALSE ))
	answer->addChild( newKid );
    if ( newKid = getPart( "pickStyle", FALSE ))
	answer->addChild( newKid );

    if ( newKid = getPart( "appearance.lightModel", FALSE ))
	answer->addChild( newKid );
    if ( newKid = getPart( "appearance.environment", FALSE ))
	answer->addChild( newKid );
    if ( newKid = getPart( "appearance.drawStyle", FALSE ))
	answer->addChild( newKid );
    if ( newKid = getPart( "appearance.material", FALSE ))
	answer->addChild( newKid );
    if ( newKid = getPart( "appearance.complexity", FALSE ))
	answer->addChild( newKid );
    if ( withTextureCoords.getValue() ) {
	if ( newKid = getPart( "appearance.texture2", FALSE ))
	    answer->addChild( newKid );
    }
    if ( newKid = getPart( "appearance.font", FALSE ))
	answer->addChild( newKid );

    if ( newKid = getPart( "units", FALSE ))
	answer->addChild( newKid );
    if ( newKid = getPart( "transform", FALSE ))
	answer->addChild( newKid );
    if ( withTextureCoords.getValue() ) {
	if ( newKid = getPart( "texture2Transform", FALSE ))
	    answer->addChild( newKid );
    }

    if ( newKid = getPart( "shapeHints", FALSE ))
	answer->addChild( newKid );
    if ( withTextureCoords.getValue() ) {
	if ( newKid = getPart( "textureBinding", FALSE ))
	    answer->addChild( newKid );
	if ( newKid = getPart( "textureCoords", FALSE ))
	    answer->addChild( newKid );
	if ( newKid = getPart( "texture2", FALSE ))
	    answer->addChild( newKid );
    }
    if ( newKid = getPart( "coords", FALSE ))
	answer->addChild( newKid );
    if ( newKid = getPart( "faceSet", FALSE ))
	answer->addChild( newKid );
    if ( newKid = getPart( "quadMesh", FALSE ))
	answer->addChild( newKid );
    if ( newKid = getPart( "nurbsSurfaceGroup", FALSE ))
	answer->addChild( newKid );
    if ( newKid = getPart( "triangleStripSet", FALSE ))
	answer->addChild( newKid );

    if (    withTextureCoords.getValue() 
	 && ( withTopCap.getValue()    || withBottomCap.getValue() ) 
	 && crossSectionClosed.getValue() && numCols >= 4 ) {
	if ( newKid = getPart( "capTextureBinding", FALSE ))
	    answer->addChild( newKid );
	if ( newKid = getPart( "capTextureCoords", FALSE ))
	    answer->addChild( newKid );
    }

    if ( withTopCap.getValue()
	 && crossSectionClosed.getValue() && numCols >= 4 ) {
	if ( newKid = getPart( "topCapCoords", FALSE ))
	    answer->addChild( newKid );
	if ( newKid = getPart( "topCapFaces", FALSE ))
	    answer->addChild( newKid );
    }

    if ( withBottomCap.getValue()
	 && crossSectionClosed.getValue() && numCols >= 4 ) {
	if ( newKid = getPart( "bottomCapCoords", FALSE ))
	    answer->addChild( newKid );
	if ( newKid = getPart( "bottomCapFaces", FALSE ))
	    answer->addChild( newKid );
    }

    if ( newKid = getPart( "childList", FALSE ))
	answer->addChild( newKid );

    answer->unrefNoDelete();
    return answer;
}

void
GeneralizedCylinder::changeCurveClosure( char *curveName, SbBool newClosed )
{
    SoCoordinate3 *coords = SO_GET_PART( this, curveName, SoCoordinate3 );

    SoMFVec3f *cField = &coords->point;
    int numC = cField->getNum();
    if ( newClosed == TRUE ) {
	if ( numC > 0 ) {
	    // make a new final point that is a copy of the zero'th
	    if ( numC > 1 && ((*cField)[0] == (*cField)[numC-1] ))
		// First make sure that the first and last points aren't already
		// already the same.
		return;
	    else
		cField->set1Value( numC, (*cField)[0] );
	}
    }
    else if ( numC > 1 ) {
	// remove the last point
	if ( (*cField)[0] != (*cField)[numC-1] )
	    // First make sure that the first and last points are the same.
	    return;
	else
	    cField->deleteValues( numC - 1, 1 );
    }
}

void
GeneralizedCylinder::changeWithTextureCoords( SbBool newWith )
{
    if ( withTextureCoords.getValue() != newWith )
         withTextureCoords = newWith;

    if ( newWith ) {
	set( "textureBinding { value PER_VERTEX }");
	set( "capTextureBinding { value PER_VERTEX_INDEXED }");
	set( "texture2 { filename \"defaultTexture.rgb\" }" );
    }
    else {

	// The texture2 and texture2Transform parts may have some useful
	// information, so we check them before deleting. 
	// The others can just go bye-bye

	SoTexture2 *tex2 = (SoTexture2 *) getPart("texture2",FALSE);
	if ( tex2 != NULL ) {
	    tex2->filename = "";
	    if (tex2->hasDefaultValues())
	        setPart("texture2", NULL );
	}
	SoTexture2Transform *txXf 
	    = (SoTexture2Transform *) getPart("texture2Transform",FALSE);
	if ( txXf != NULL ) {
	    if (txXf->hasDefaultValues())
	        setPart("texture2Transform", NULL );
	}

	setPart("textureBinding", NULL );
	setPart("capTextureBinding", NULL );

	setPart("textureCoords", NULL );
	setPart("capTextureCoords", NULL );
    }
}

void
GeneralizedCylinder::calculateFullProfile()
{
    SoCoordinate3 *profC = SO_GET_PART( this, "profileCoords", SoCoordinate3 );
    SoCoordinate3 *spinC = SO_GET_PART( this, "spineCoords", SoCoordinate3 );
    SoCoordinate3 *twisC = SO_GET_PART( this, "twistCoords", SoCoordinate3 );

    int numInProf = profC->point.getNum();

    // Start by just copying profC into fullProfile...
    int i = numInProf - fullProfile->point.getNum();
    if ( i > 0 )
	fullProfile->point.insertSpace(0, i);
    else if ( i < 0 )
	fullProfile->point.deleteValues(0, -i);
    fullProfile->point.setValues(0,numInProf,profC->point.getValues(0));

    // Next, add in any points in the spine curve that are not demarcated
    // in the profile curve.
    for ( i = 0; i < spinC->point.getNum(); i++ ) {
	// Get parametric distance of point along length of spine.
	float pDist  = spineParamDistances[i];
	float pHeight = profileMinY + pDist * profileHeight;

	// Translate this parametric distance into a point or points in 
	// the profile curve.
	SbVec3f p0, p1, profPoint;
	float   amt;
	for ( int j = 0; j < fullProfile->point.getNum() - 1; j++ ) {
	    p0 = fullProfile->point[j];
	    p1 = fullProfile->point[j+1];
	    // If the spine point falls at a point along the profile that
	    // lies between p0 and p1, then interpolate and add it in.
	    if ( pHeight > p0[1] && pHeight < p1[1] ) {
		amt = (pHeight - p0[1]) / (p1[1] - p0[1]);
		profPoint = p0 + amt * ( p1 - p0);
		// Add the point
		fullProfile->point.insertSpace(j+1,1);
		fullProfile->point.set1Value(j+1,profPoint);
		j++;
	    }
	    else if ( pHeight < p0[1] && pHeight > p1[1] ) {
		amt = (pHeight - p1[1]) / (p0[1] - p1[1]);
		profPoint = p1 + amt * ( p0 - p1);
		// Add the point
		fullProfile->point.insertSpace(j+1,1);
		fullProfile->point.set1Value(j+1,profPoint);
		j++;
	    }
	}
    }

    // Next, add in any points in the twist curve that are not demarcated
    // in the profile curve.
    for ( i = 0; i < twisC->point.getNum(); i++ ) {
    }


    if ( numInProf < 2 )
	return;

    if ( minNumRows.getValue() > numInProf ) {
	
	float overDist = profileLength / minNumRows.getValue();
	SbVec3f p0, p1, newPt, diff;
	float length;
	for ( int i = 0; i < fullProfile->point.getNum() - 1; i++ ) {
	    p0 = fullProfile->point[i];
	    p1 = fullProfile->point[i+1];
	    diff = p1 - p0;
	    length = diff.length();
	    diff.normalize();
	    if ( length > overDist ) {
		newPt = p0 + diff * overDist;
		fullProfile->point.insertSpace(i+1,1);
		fullProfile->point.set1Value(i+1,newPt);
	    }
	}
    }
}
void
GeneralizedCylinder::calculateFullCrossSection()
{
    SoCoordinate3 *crosC = SO_GET_PART( this, "crossSectionCoords", SoCoordinate3 );

    int numInCross = crosC->point.getNum();

    // Start by just copying crosC into fullCrossSection...
    int i = numInCross - fullCrossSection->point.getNum();
    if ( i > 0 )
	fullCrossSection->point.insertSpace(0, i);
    else if ( i < 0 )
	fullCrossSection->point.deleteValues(0, -i);
    fullCrossSection->point.setValues( 0, numInCross, crosC->point.getValues(0));

    if ( numInCross < 2 )
	return;

    if ( minNumCols.getValue() > numInCross ) {
	
	float overDist = crossSectionLength / minNumCols.getValue();
	SbVec3f p0, p1, newPt, diff;
	float length;
	for ( int i = 0; i < fullCrossSection->point.getNum() - 1; i++ ) {
	    p0 = fullCrossSection->point[i];
	    p1 = fullCrossSection->point[i+1];
	    diff = p1 - p0;
	    length = diff.length();
	    diff.normalize();
	    if ( length > overDist ) {
		newPt = p0 + diff * overDist;
		fullCrossSection->point.insertSpace(i+1,1);
		fullCrossSection->point.set1Value(i+1,newPt);
	    }
	}
    }
}

void
GeneralizedCylinder::fieldsChangedCB(void *data, SoSensor *sens )
{
    if ( ((SoDataSensor *)sens)->getTriggerNode() != data )
	return;

    GeneralizedCylinder *s = (GeneralizedCylinder *)data;

    SoField *triggerField = ((SoDataSensor *)sens)->getTriggerField();
    SbBool flagFieldChanged = FALSE;

    if (triggerField == &s->renderShapeType)         
	 flagFieldChanged = TRUE;
    else if (triggerField == &s->normsFlipped)       
	flagFieldChanged = TRUE;
    else if (triggerField == &s->profileClosed)      
	flagFieldChanged = TRUE;
    else if (triggerField == &s->crossSectionClosed) 
	flagFieldChanged = TRUE;
    else if (triggerField == &s->spineClosed)        
	flagFieldChanged = TRUE;
    else if (triggerField == &s->twistClosed)        
	flagFieldChanged = TRUE;
    else if (triggerField == &s->minNumRows)         
	flagFieldChanged = TRUE;
    else if (triggerField == &s->minNumCols)         
	flagFieldChanged = TRUE;
    else if (triggerField == &s->withSides)         
	flagFieldChanged = TRUE;
    else if (triggerField == &s->withTopCap)         
	flagFieldChanged = TRUE;
    else if (triggerField == &s->withBottomCap)      
	flagFieldChanged = TRUE;
    else if (triggerField == &s->withTextureCoords)  
	flagFieldChanged = TRUE;

    if (flagFieldChanged == FALSE )
	return;

    // Special stuff to do if a curve's closurehas been changed
    if ( s->profileClosed.getValue() != s->profileAlreadyClosed ) {
	s->changeCurveClosure( "profileCoords", s->profileClosed.getValue() );
	s->profileAlreadyClosed = s->profileClosed.getValue();
    }
    if ( s->crossSectionClosed.getValue() != s->crossSectionAlreadyClosed ) {
	s->changeCurveClosure( "crossSectionCoords", 
				s->crossSectionClosed.getValue() );
	s->crossSectionAlreadyClosed = s->crossSectionClosed.getValue();
    }
    if ( s->spineClosed.getValue() != s->spineAlreadyClosed ) {
	s->changeCurveClosure( "spineCoords", s->spineClosed.getValue() );
	s->spineAlreadyClosed = s->spineClosed.getValue();
    }
    if ( s->twistClosed.getValue() != s->twistAlreadyClosed ) {
	s->changeCurveClosure( "twistCoords", s->twistClosed.getValue() );
	s->twistAlreadyClosed = s->twistClosed.getValue();
    }

    // Special stuff to do if a curve's texture status has been changed
    if ( s->withTextureCoords.getValue() != s->alreadyWithTextureCoords) {
	s->changeWithTextureCoords( s->withTextureCoords.getValue() );
	s->alreadyWithTextureCoords = s->withTextureCoords.getValue();
    }

    s->updateSurface();
}

void
GeneralizedCylinder::inputChangedCB(void *data, SoSensor *sens )
{
    GeneralizedCylinder *s = (GeneralizedCylinder *)data;
    SoNode *notifier = ((SoNodeSensor *)sens)->getTriggerNode();

    SoCoordinate3 *coordPart = NULL;
    SbBool        isClosed;

    coordPart = SO_CHECK_PART( s, "profileCoords", SoCoordinate3 );
    if ( notifier == coordPart )
	isClosed = s->profileClosed.getValue();
    else {
	coordPart = SO_CHECK_PART( s, "crossSectionCoords", SoCoordinate3 );
        if ( notifier == coordPart )
	    isClosed = s->crossSectionClosed.getValue();
	else {
	    coordPart = SO_CHECK_PART( s, "spineCoords", SoCoordinate3 );
	    if ( notifier == coordPart )
		isClosed = s->spineClosed.getValue();
	    else {
		coordPart = SO_CHECK_PART( s, "twistCoords", SoCoordinate3 );
		if ( notifier == coordPart )
		    isClosed = s->twistClosed.getValue();
		else
		    coordPart = NULL;
	    }
	}
    }

    if (coordPart == NULL)
	return;

    if (isClosed) {
	SoMFVec3f *cField = &(coordPart->point);
	int numC = cField->getNum();
	// make the last point follow the first point.
	if ( numC == 3 )
	    cField->set1Value( numC, (*cField)[0] );
	else if ( numC > 3 && (*cField)[0] != (*cField)[numC - 1] )
	    cField->set1Value( numC - 1, (*cField)[0] );
    }

    // Clamp the values of the profile coordinates so that 
    // no values are below zero:
    if (coordPart == s->profileCoords.getValue()) {
	SbBool changed = FALSE;
	coordPart->point.enableNotify(FALSE);
	SbVec3f *pCoords = coordPart->point.startEditing();
	for ( int i = 0; i < coordPart->point.getNum(); i++ ) {
	    if ( pCoords[i][0] < 0.0 ) {
		pCoords[i].setValue( 0.0, pCoords[i][1], pCoords[i][2] );
		changed = TRUE;
	    }
	}
	coordPart->point.finishEditing();
	coordPart->point.enableNotify(TRUE);
	if (changed) {
	    // return now -- changing the coords will trigger notification
	    // and bring us back here.
	    coordPart->point.touch();
	}
    }

    s->updateSurface();
}


void 
GeneralizedCylinder::updateSurroundingManip()
{
    SoNode *n;

    // Is there a transform?
    n = getPart( "transform", FALSE);
    if (n==NULL) return;

    // If it's not a transform manip, then return...
    if ( !n->isOfType( SoTransformManip::getClassTypeId() ) ) 
	return;

    SoTransformManip *xfm = (SoTransformManip *) n;

    // Try to get the dragger from the manip.
    SoDragger *d = xfm->getDragger();
    if (d==NULL)
	return;

    SoSurroundScale *ss 
	= (SoSurroundScale *) d->getPart( "surroundScale", FALSE );

    if ( ss != NULL)
	ss->invalidate();
}

void 
GeneralizedCylinder::changeTransformType( SoType newType )
{
    // Return if manip is already of correct type.
    SoTransform *curXf = (SoTransform *) getPart("transform",FALSE);
    if ( curXf && (curXf->getTypeId() == newType ))
	return;


    if (   newType.isBad() || 
	 ! newType.isDerivedFrom(SoTransformManip::getClassTypeId() ) ||
	 ! newType.canCreateInstance() ) {

	// If no transform part, just return:
        SoFullPath *xfPath = (SoFullPath *) createPathToPart("transform",FALSE);
	if ( ! xfPath )
	    return;

	// If tail is not an SoTransformManip, just return.
	SoNode *tail = xfPath->getTail();
	if ( ! tail->isOfType( SoTransformManip::getClassTypeId() ) )
	    return;

	// Tail is an SoTransformManip.  Tell it to replace itself with 
	// a regular transform
	xfPath->ref();
	tail->ref();
	SoTransformManip *m = (SoTransformManip *) tail;
	m->replaceManip(xfPath, new SoTransform );
	tail->unref();
	xfPath->unref();
    }
    else {

	// Make a manip of given type, tell it to insert itself in the path
        SoPath *xfPath = createPathToPart("transform",TRUE);
	SoTransformManip *m = (SoTransformManip *) newType.createInstance();
	xfPath->ref();
	m->replaceNode(xfPath );
        xfPath->unref();
    }
}
