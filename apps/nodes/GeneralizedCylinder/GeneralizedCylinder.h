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
 |   Description:
 |      Defines the GeneralizedCylinder class. A nodekit that
 |      makes extruded objects.
 |
 |   Author(s)          : Paul Isaacs
 |
*/

#ifndef _GENERALIZED_CYLINDER_
#define _GENERALIZED_CYLINDER_

#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/nodekits/SoSeparatorKit.h>
#include <Inventor/fields/SoSFShort.h>
#include <Inventor/fields/SoSFEnum.h>

class SoCoordinate3;
class SoIndexedFaceSet;
class SoSensor;
class SoNodeSensor;
class SoWriteAction;
class NurbMaker;

////////////////////////////////////////////////////////////////////
//    Class: GeneralizedCylinder 
//
// This class constructs a generalized cylinder based on 4 curves:
// The 4 curves are held in the parts:
//  profileCoords, crossSectionCoords, spineCoords, twistCoords.
//
// Users of this class should access only these four parts and the
// 10 fields in order to create different objects.
//
//    The structure of the catalog for this class is:
//
//                       this
//                         |
//                ----------------
//                |              | 
//           "callbackList" "topSeparator"
//                               |
//      -----------------------------------------------------------------
//      |           |            |             |                  |     |
//   "pickStyle" "appearance" "transform" "texture2Transform"     | "childList"
//                                                                |
//   --------------------------------------------------------------
//   |                                          |
//  "inputSwitch"                           "outputSep"
//   |                               ---------------------------------------
//   |                               |(the following appear all in a row)
//   |                          "shapeHints","textureBinding","textureCoords",
//   |                           "texture2", "coords", "faceSet", 
//   |                           "quadMesh", "nurbsSurfaceGroup",
//   |                           "triangleStripSet",
//   |                          "capTextureCoords", "capTextureBinding",
//   |                          "topCapCoords", "topCapFaces",
//   |                          "bottomCapCoords", "bottomCapFaces"
//   |
//   -----------------------------
//   |                |      |   | 
//  "profileCoords"   |      |   |
//      "crossSectionCoords" |   |
//                "spineCoords"  |
//                         "twistCoords"
//
////////////////////////////////////////////////////////////////////

// C-api: prefix=GenCylinder
class GeneralizedCylinder : public SoSeparatorKit {

    SO_KIT_HEADER(GeneralizedCylinder);

    SO_KIT_CATALOG_ENTRY_HEADER(inputSwitch);
    SO_KIT_CATALOG_ENTRY_HEADER(profileCoords);
    SO_KIT_CATALOG_ENTRY_HEADER(crossSectionCoords);
    SO_KIT_CATALOG_ENTRY_HEADER(spineCoords);
    SO_KIT_CATALOG_ENTRY_HEADER(twistCoords);
    SO_KIT_CATALOG_ENTRY_HEADER(outputGrp);
    SO_KIT_CATALOG_ENTRY_HEADER(shapeHints);
    SO_KIT_CATALOG_ENTRY_HEADER(textureBinding);
    SO_KIT_CATALOG_ENTRY_HEADER(textureCoords);
    SO_KIT_CATALOG_ENTRY_HEADER(texture2);
    SO_KIT_CATALOG_ENTRY_HEADER(coords);
    SO_KIT_CATALOG_ENTRY_HEADER(faceSet);
    SO_KIT_CATALOG_ENTRY_HEADER(quadMesh);
    SO_KIT_CATALOG_ENTRY_HEADER(nurbsSurfaceGroup);
    SO_KIT_CATALOG_ENTRY_HEADER(triangleStripSet);
    SO_KIT_CATALOG_ENTRY_HEADER(capTextureBinding);
    SO_KIT_CATALOG_ENTRY_HEADER(capTextureCoords);
    SO_KIT_CATALOG_ENTRY_HEADER(topCapCoords);
    SO_KIT_CATALOG_ENTRY_HEADER(topCapFaces);
    SO_KIT_CATALOG_ENTRY_HEADER(bottomCapCoords);
    SO_KIT_CATALOG_ENTRY_HEADER(bottomCapFaces);

  public:
    GeneralizedCylinder();

  public:

    //
    // Create a generalized cylinder given the coordinates of its
    // profile, crossSection, spine, and twist.
    //
    void updateSurface();

    // Makes a copy of this node as a separator with only the children needed
    // to display it.
    SoSeparator *makeVanillaVersion();

    // How should the shape be rendered?
    enum RenderShapeType {
	FACE_SET,
	TRIANGLE_STRIP_SET,
	QUAD_MESH,
	BEZIER_SURFACE,
	CUBIC_SPLINE_SURFACE,
	CUBIC_TO_EDGE_SURFACE
    };
    SoSFEnum  renderShapeType;	

    SoSFShort profileClosed;
    SoSFShort crossSectionClosed;
    SoSFShort spineClosed;
    SoSFShort twistClosed;

    SoSFShort minNumRows;
    SoSFShort minNumCols;
    SoSFShort withSides;
    SoSFShort withTopCap;
    SoSFShort withBottomCap;

    SoSFShort normsFlipped;
    SoSFShort withTextureCoords;

    // Used for changing between transform and different types of manip
    void changeTransformType( SoType newType );

  SoINTERNAL public:
    static void initClass();

  protected:

    // detach/attach any sensors, callbacks, and/or field connections.
    // Called by:            start/end of SoBaseKit::readInstance
    // and on new copy by:   start/end of SoBaseKit::copy.
    // Classes that redefine must call setUpConnections(TRUE,TRUE) 
    // at end of constructor.
    // Returns the state of the node when this was called.
    virtual SbBool setUpConnections( SbBool onOff, SbBool doItAlways = FALSE ); 

    // Makes it so that the outputGrp and all nodes beneath it do not
    // write out. Since the outputGrp can be re-created from the other 
    // fields, we don't bother writing out all the information, which would 
    // create a larger file.
    virtual void	setDefaultOnNonWritingFields();

    NurbMaker *myNurbMaker;

    void updateSurroundingManip();

  private:
    // destructor
    virtual ~GeneralizedCylinder();

    // stuff used by updateSurface
    void loadRow( int rowNum, SbVec3f *newCoords );
    void loadTextureRow( int rowNum, SbVec2f *newCoords );
    void initUpdateInfo();
    void calculateFullProfile();
    void calculateFullCrossSection();
    void getSpineInfo( float paramDist, SbVec3f &center, SbRotation &rot );
    SbRotation getSpinePointRotation( int ind0, int ind1, float paramDist );
    void initSpineKeyPointRotations();
    void getTwistInfo( float paramDist, SbRotation &twistRot );

    SoIndexedFaceSet *topCapScratchFace;
    SoIndexedFaceSet *bottomCapScratchFace;

    SoCoordinate3 *fullProfile;
    SoCoordinate3 *fullCrossSection;

    float profileMinY, profileHeight, profileOverHeight;
    float crossSectionMinX, crossSectionWidth;
    float crossSectionMinZ, crossSectionDepth;
    float twistMinY, twistMaxY;

    float spineLength;
    float *spineParamDistances; // parametric distance along spine of each
			        // point on the spine.
    float profileLength;
    float crossSectionLength;
    SbRotation *spineKeyPointRotations;
    int        numAllocedSpineKeyRots;

    void changeCurveClosure( char *curveName, SbBool newClosed );
    SbBool profileAlreadyClosed;
    SbBool crossSectionAlreadyClosed;
    SbBool spineAlreadyClosed;
    SbBool twistAlreadyClosed;

    void changeWithTextureCoords( SbBool newWith );
    SbBool alreadyWithTextureCoords;

    static void fieldsChangedCB(void *data, SoSensor *sens );
    SoNodeSensor *selfSensor;

    static void inputChangedCB(void *data, SoSensor *sens );
    SoNodeSensor *inputSensor;
};

const double creaseAngle = M_PI/6.0;	// 30 degrees

#endif /* _GENERALIZED_CYLINDER_ */
