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
*/



#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoQuadMesh.h>
#include <Inventor/nodes/SoIndexedNurbsSurface.h>

#include "NurbMaker.h"

// These knot vectors are used to make CUBIC_TO_EDGE splines
// meet up with the mesh edge.

static float cubic2FromLateEdgeKnots[8] = { 0, 1, 2, 3, 4, 5, 6, 6 };
static float cubic1FromLateEdgeKnots[8] = { 0, 1, 2, 3, 4, 5, 5, 5 };
static float cubic0FromLateEdgeKnots[8] = { 0, 1, 2, 3, 4, 4, 4, 4 };

static float cubic2FromEarlyEdgeKnots[8] = { 0, 0, 1, 2, 3, 4, 5, 6 };
static float cubic1FromEarlyEdgeKnots[8] = { 0, 0, 0, 1, 2, 3, 4, 5 };
static float cubic0FromEarlyEdgeKnots[8] = { 0, 0, 0, 0, 1, 2, 3, 4 };

// Creates group with new SoIndexedNurbsSurface nodes underneath.
SoGroup *
NurbMaker::createNurbsGroup( SoQuadMesh *quadNode, SoCoordinate3 *coordNode )
{
    SbBool rowsMatch = FALSE;
    SbBool colsMatch = FALSE;

    int vPerCol = (int) quadNode->verticesPerColumn.getValue();
    int vPerRow = (int) quadNode->verticesPerRow.getValue();

    SbVec2s quadMeshDivs( vPerRow, vPerCol );

    // Only bother checking if we're wrapping.
    if (coordNode != NULL && myWrap[0] == TRUE) {
	rowsMatch = TRUE;
        // Test if first and last row are the same.
	for (int topInd = 0, botInd = vPerRow * (vPerCol - 1);
	     topInd < vPerRow; topInd++, botInd++ ) {
	     if (  coordNode->point[topInd] != coordNode->point[botInd] ) {
		rowsMatch = FALSE;
	     }
	}
    }
    if (coordNode != NULL && myWrap[1] == TRUE) {
	SbBool colsMatch = TRUE;
        // Test if first and last col are the same.
	for (int leftInd = 0, rightInd = vPerRow - 1;
	     leftInd < vPerRow * (vPerCol - 1); 
	     leftInd += vPerRow, rightInd += vPerRow) {
	     if (  coordNode->point[leftInd] != coordNode->point[rightInd] ) {
		colsMatch = FALSE;
	     }
	}
    }

    return ( createNurbsGroup( quadMeshDivs, SbVec2s(rowsMatch,colsMatch )));
}

// Creates group with new SoIndexedNurbsSurface nodes underneath.
SoGroup *
NurbMaker::createNurbsGroup( SbVec2s numQuadMeshDivisions, 
			     SbVec2s doEdgesMatch )
{
    establishMyKnotParams();
    // The patch size is the number of control points on the side
    // of each patch. It's determined by the number of knots and order
    SbVec2s patchSize = myNumKnots - myOrder;

    // Make a group to put the new NURBS under
    if (nurbsGroup != NULL)
	nurbsGroup->unref();

    nurbsGroup = new SoGroup;
    nurbsGroup->ref();

    // What are the mesh dimensions?
    int vPerRow = (int) numQuadMeshDivisions[0];
    int vPerCol = (int) numQuadMeshDivisions[1];

    // Determine last row and last column that we can use for our 
    // upper left corner of our little patch.
    // Initially, figure that the last patch's right/bottom edge
    // need to line up with the mesh's right/bottom edge.
    int lastRowToDo = vPerCol - patchSize[1];
    int lastColToDo = vPerRow - patchSize[0];

    // If we don't wrap, all rows and columns are used.
    // If we are wrapping and the first&last row/column are the same,
    // then we will never use the last row/column.
    int numUseableRows = vPerCol;
    int numUseableCols = vPerRow;

    // If we're wrapping rows, the last patch left edge is the same as the
    // mesh's right edge.
    // But if the left and right edges of the mesh are identical, we don't
    // need to repeat the set of patches that uses the mesh's right edge as 
    // their own (the patches' own) left edge.
    if (myWrap[0] == TRUE) {
	if (doEdgesMatch[0]) {
	    lastRowToDo = vPerCol - 2;
	    numUseableRows = vPerCol - 1;
	}
	else {
	    lastRowToDo = vPerCol - 1;
	    numUseableRows = vPerCol;
	}
    }

    // If we're wrapping cols, the last patch top edge is the same as the
    // mesh's bottom edge.
    // But if the top and bottom edges of the mesh are identical, we don't
    // need to repeat the set of patches that uses the mesh's bottom edge as 
    // their own (the patches' own) top edge.
    if (myWrap[1] == TRUE) {
	if (doEdgesMatch[1]) {
	    lastColToDo = vPerRow - 2;
            numUseableCols = vPerRow - 1;
	}
	else {
	    lastColToDo = vPerRow - 1;
	    numUseableCols = vPerRow;
	}
    }

    // In each patch, we will lay down the coordinate indices as either 
    // increasing from left to right , as in:
    //   0  1  2  3
    //   4  5  6  7
    //   8  9 10 11 
    //  12 13 14 15
    //
    // or increasing from right to left, depending on 'flipNormals'
    //   3  2  1  0
    //   7  6  5  4
    //  11 10  9  8
    //  15 14 13 12
    int numPatchInds = patchSize[0] * patchSize[1];
    int *patchInds = new int [ numPatchInds ];
    if ( flipNormals == FALSE ) {
	for (int i = 0, count=0; i < patchSize[1]; i++ ){
	    for(int j = 0; j < patchSize[0]; j++ ) {
		patchInds[count] = (i * patchSize[0]) + j;
		count++;
	    }
	}
    }
    else for (int i = 0, count=0; i < patchSize[1]; i++ ){
	    for(int j = patchSize[0]-1; j >=0; j--) {
		patchInds[count] = (i * patchSize[0]) + j;
		count++;
	    }
    }

    // Now, create the patches. Each is a nurb with 
    // a patchSize[0]by patchSize[1] control point patch.
    // The knots are as already established.
    for ( int row = 0; row <= lastRowToDo; row+= myShift[0] ) {
	for ( int col = 0; col <= lastColToDo; col+= myShift[1] ) {

	    // Create the NURB and set the easy fields...
	    SoIndexedNurbsSurface *myNurb = new SoIndexedNurbsSurface;
	    nurbsGroup->addChild(myNurb);
	    myNurb->numUControlPoints = patchSize[0];
	    myNurb->numVControlPoints = patchSize[1];
	    if ( patchType != CUBIC_TO_EDGE) {
		myNurb->uKnotVector.setValues(0,myNumKnots[0],myUKnots);
		myNurb->vKnotVector.setValues(0,myNumKnots[1],myVKnots);
	    }
	    else {
		applyCubicToEdgeKnotVectors(row, col, myNurb, 
					    lastRowToDo, lastColToDo);
	    }

	    for (int pRow = 0,count = 0; pRow < patchSize[1]; pRow++ ) {

		// Now we've got to set up the indices. Determine the row 
		// number in the quadmesh (0<=value<numUseableRows)
		// to use as the row number in the patch( 0<=pRow<patchSize[1])
		int rowInQuadMesh = row + pRow;

		// Now check to see if we need to wraparound this row.
		if (rowInQuadMesh >= numUseableRows)
		    rowInQuadMesh -= numUseableRows;

		for (int pCol = 0; pCol < patchSize[0]; pCol++ ) {

		    // Determine the col number in the mesh 
		    // (0<=value<numUseableCols) to use as the col number 
		    // in the patch( 0<=pRow<patchSize[0])
		    int colInQuadMesh = col + pCol;

		    // Now check to see if we need to wraparound this column.
		    if (colInQuadMesh >= numUseableCols)
			colInQuadMesh -= numUseableCols;


		    myNurb->coordIndex.set1Value( patchInds[count], 
				rowInQuadMesh * vPerRow + colInQuadMesh);

		    count++;
		}
	    }
	}
    }

    return nurbsGroup;
}

NurbMaker::NurbMaker()
{
    nurbsGroup = NULL;

    patchType  = CUBIC_TO_EDGE;

    myNumKnots.setValue(0,0);
    myUKnots    = myVKnots    = NULL;
    myOrder.setValue(4,4);
    myShift.setValue(1,1);
    myWrap.setValue(0,0);

    userNumKnots.setValue(0,0);
    userUKnots    = userVKnots    = NULL;
    userOrder.setValue(4,4);

    flipNormals = FALSE;
}

NurbMaker::~NurbMaker()
{
    if (nurbsGroup) {
	nurbsGroup->unref();
        nurbsGroup = NULL;
    }	
}

void
NurbMaker::establishMyKnotParams()
{
    int i;

    // Take care of knot array allocation.
    if (    patchType == BEZIER || patchType == CUBIC 
	 || patchType == CUBIC_TO_EDGE ) {
	if ( myNumKnots[0] != 8 ) {
	    if (myUKnots != NULL) delete []myUKnots;
	    myNumKnots[0] = 8;
	    myUKnots    = new float[8];
	}
	if ( myNumKnots[1] != 8 ) {
	    if (myVKnots != NULL) delete []myVKnots;
	    myNumKnots[1] = 8;
	    myVKnots    = new float[8];
	}
    }
    else {
	if (myUKnots != NULL) delete []myUKnots;
	if (myVKnots != NULL) delete []myVKnots;

	myNumKnots = userNumKnots;

	myUKnots    = new float[myNumKnots[0]];
	myVKnots    = new float[myNumKnots[1]];
    }

    // Take care of knot values and order values.
    switch (patchType) {
	case BEZIER:
	    myOrder.setValue(4,4);
	    for (i = 0; i < 4; i++)
		myUKnots[i] = myVKnots[i] = 0;
	    for (i = 4; i < 8; i++)
		myUKnots[i] = myVKnots[i] = 1;
	    break;
	case CUBIC:
	case CUBIC_TO_EDGE:
	    myOrder.setValue(4,4);
	    for (i = 0; i < 8; i++)
		myUKnots[i] = myVKnots[i] = i;
	    break;
	case USER_KNOTS:
	    myOrder    = userOrder;
	    for (i = 0; i < myNumKnots[0]; i++)
		myUKnots[i] = userUKnots[i];
	    for (i = 0; i < myNumKnots[1]; i++) {
		myVKnots[i] = userVKnots[i];
	    }
	    break;
    }
}

void
NurbMaker::setUserKnots( SbVec2s newNumKnots,
			 float *newUKnots, float *newVKnots)
{
    userNumKnots = newNumKnots;
    if (userUKnots == NULL)
	delete [] userUKnots;
    if (userVKnots == NULL)
	delete [] userVKnots;
    userUKnots = new float [userNumKnots[0]];
    userVKnots = new float [userNumKnots[1]];
    for (int i = 0; i < userNumKnots[0]; i++ )
	userUKnots[i] = newUKnots[i];
    for (int j = 0; j < userNumKnots[1]; j++ )
	userVKnots[j] = newVKnots[j];
}

void
NurbMaker::getUserKnots( SbVec2s &numKnots,
			 float * UKnots, float * VKnots) 
{
    numKnots = userNumKnots;
    if (UKnots == NULL)
	delete [] UKnots;
    if (VKnots == NULL)
	delete [] VKnots;
    UKnots = new float [userNumKnots[0]];
    VKnots = new float [userNumKnots[1]];
    for (int i = 0; i < userNumKnots[0]; i++ )
	UKnots[i] = userUKnots[i];
    for (int j = 0; j < userNumKnots[1]; j++ )
	VKnots[j] = userVKnots[j];
}

void 
NurbMaker::setPatchType(PatchType newPatchType)
{
    patchType = newPatchType;
    if (patchType == BEZIER) {
	myShift.setValue(3,3);
    }
    else if (patchType == CUBIC || patchType == CUBIC_TO_EDGE) {
	myShift.setValue(1,1);
    }
}

void
NurbMaker::applyCubicToEdgeKnotVectors(int row, int col, 
	    SoIndexedNurbsSurface *myNurb, int lastRowToDo, int lastColToDo)
{
    float *uKs, *vKs;

    if ( myWrap[1] )               uKs = myUKnots;
    else if (col == 0 )            uKs = cubic0FromEarlyEdgeKnots;
    else if (col == lastColToDo)   uKs = cubic0FromLateEdgeKnots;
    else if (col == 1 )            uKs = cubic1FromEarlyEdgeKnots;
    else if (col == lastColToDo-1) uKs = cubic1FromLateEdgeKnots;
    else if (col == 2 )            uKs = cubic2FromEarlyEdgeKnots;
    else if (col == lastColToDo-2) uKs = cubic2FromLateEdgeKnots;
    else                           uKs = myUKnots;

    if ( myWrap[0] )               vKs = myVKnots;
    else if (row == 0 )            vKs = cubic0FromEarlyEdgeKnots;
    else if (row == lastRowToDo)   vKs = cubic0FromLateEdgeKnots;
    else if (row == 1 )            vKs = cubic1FromEarlyEdgeKnots;
    else if (row == lastRowToDo-1) vKs = cubic1FromLateEdgeKnots;
    else if (row == 2 )            vKs = cubic2FromEarlyEdgeKnots;
    else if (row == lastRowToDo-2) vKs = cubic2FromLateEdgeKnots;
    else                           vKs = myVKnots;

    myNurb->uKnotVector.setValues(0,8,uKs);
    myNurb->vKnotVector.setValues(0,8,vKs);

};
