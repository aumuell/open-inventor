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
 |      Defines the NurbMaker class. Given a quadMesh,
 |      a bunch of parameters, it creates a group node with a bunch 
 |      of SoIndexedNurbsSurface nodes. These nodes will create a nurbs 
 |      surface if placed after an SoCoordinate3 node with the number of
 |      points specified by the QuadMesh node.
 |   
 |      Note that you only need to look at the SoCoordinate3 node if 
 |      you need to figure out whether edges match for wraparound. 
 |      Otherwise this is all strictly 'topological'
 |
 |   Author(s)          : Paul Isaacs
 |
*/

#ifndef _NURB_MAKER
#define _NURB_MAKER

#include <Inventor/SbLinear.h>

class SoGroup;
class SoCoordinate3;
class SoQuadMesh;
class SoIndexedNurbsSurface;

////////////////////////////////////////////////////////////////////
//    Class: NurbMaker 
//
////////////////////////////////////////////////////////////////////

// C-api: prefix=NrbMkr
class NurbMaker {

  public:
    // Constructor, destructor
    NurbMaker();
    ~NurbMaker();

    // You can save me some work if you tell me whether the edges line up.
    // If you're not wrapping it won't matter, but if you wrap it will.
    SoGroup *createNurbsGroup(SbVec2s numQuadMeshDivisions, 
			      SbVec2s doEdgesMatch );
    SoGroup *createNurbsGroup(SoQuadMesh    *quadNode, 
			      SoCoordinate3 *coordNode = NULL);

    // Default is FALSE
    void   setFlipNormals(SbBool newFlip) {flipNormals = newFlip; };
    SbBool isFlipNormals() { return flipNormals; };

    enum PatchType {
	BEZIER,
	CUBIC,
	CUBIC_TO_EDGE,
	USER_KNOTS
    };
    // Default is CUBIC_TO_EDGE
    void  setPatchType(PatchType newPatchType);
    PatchType getPatchType() { return patchType; }

    // Default is FALSE.
    void setWraparound( SbVec2s newWrap )
		{ myWrap = newWrap; }
    const SbVec2s &getWraparound() { return myWrap; }

    // Default is 1, for CUBIC
    // This tells how many rows/columns to move over when we go to make
    // the next sub-patch. Setting patchType to CUBIC changes it to 1, 
    // and BEZIER to 3.
    void setPatchShift( SbVec2s newShift )
		{ myShift = newShift; }
    const SbVec2s &getPatchShift()
		{ return myShift; }


    // Default is not to use the user knots, but standard ones for 
    // BEZIER, CUBIC, or CUBIC_TO_EDGE 
    void  setUserKnots( SbVec2s newNumKnots, 
			float *newUKnots = NULL, float *newVKnots = NULL);
    void  getUserKnots( SbVec2s &numKnots, float *UKnots, float *VKnots);
    void setUserCurveOrder( SbVec2s newOrder ) { userOrder = newOrder; }
    const SbVec2s &getUserCurveOrder() { return userOrder; }

  protected:

    void establishMyKnotParams();
    void applyCubicToEdgeKnotVectors(int row, int col, 
	    SoIndexedNurbsSurface *myNurb, int lastRowToDo, int lastColToDo);
  private:
    SoGroup *nurbsGroup;

    PatchType patchType;
    SbBool    flipNormals;

    SbVec2s  userNumKnots;
    float   *userUKnots,  *userVKnots;
    SbVec2s  userOrder;

    SbVec2s  myNumKnots;
    SbVec2s  myOrder;
    SbVec2s  myShift;
    SbVec2s  myWrap;
    float   *myUKnots,  *myVKnots;
};

#endif /* _NURB_MAKER */
