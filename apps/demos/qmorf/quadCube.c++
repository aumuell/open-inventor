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

//
// Code to generate a cube made out of a single quad-mesh
//
// Basic shape is:
//     /\ 
//    /  \ 
//   /    \ 
//  /      \ 
//  --------
//  |      |
//  |      |
//  |  --  |
//  |      |
//  |      |
//  --------
//  \      /
//   \    /
//    \  /
//     \/
// This is rotated about the y axis to create 4 symmetric copies.
//

#include "QuadThing.h"
#include <Inventor/SoDB.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/actions/SoWriteAction.h>

//
// Can create either a seam on the back-diagonal (in which case the
// cube is rotated 45 degrees) or a seam along the back face.
//
int diagonalFlag = 1;

QuadThing *
createQuadCube(int sizex, int sizey)
{
    int nh = sizey/4;
    int nw = sizex/4;

    SbVec3f delta_row, delta_column, r_start;
    SbVec3f *points = new SbVec3f[sizex*sizey];
    int row, column, c;

    // This rotation matrix is used if the diagonalFlag is set.  If it
    // is, we rotate the cube so the corners face toward and away from
    // the viewer, and the seam of the quadmesh is along the far
    // corner.  This matches other primitives we create (including
    // CyberWare heads).
    // Otherwise, the indexing is mucked to make the seam the middle
    // of the far face (and the cube is not rotated).

    SbMatrix m;
    m.setRotate(SbRotation(SbVec3f(0,1,0), M_PI/4.0));

    // Form top triangular piece:
    delta_row.setValue(1.0/(nh-1), 0.0, -1.0/(nh-1));

    for (row = 0; row < nh; row++)
    {
	r_start = SbVec3f(0.0, 1., 0.0) + delta_row * row;

	// Width of entire row is (2*row)/(nh-1), so:
	delta_column.setValue( -(2.0*row/(nh-1))/(nw-1), 0, 0);

	for (column = 0; column < nw; column++)
	{
	    if (diagonalFlag) c = column;
	    else c = (column+sizex-nw/2)%sizex;

	    int i = row*sizex+c;
	    points[i] = r_start + delta_column * column;
	    points[i] *= .1;
	    if (diagonalFlag)
		m.multVecMatrix(points[i], points[i]);
	}
    }

    // Do big square piece:
    delta_row.setValue(0, -2.0/(2*nh-1), 0);
    delta_column.setValue(-2.0/(nw-1), 0, 0);
    for (row = 0; row < nh*2; row++)
    {
	r_start = SbVec3f(1.0, 1.0, -1.0) + delta_row * row;
	
	for (column = 0; column < nw; column++)
	{
	    if (diagonalFlag) c = column;
	    else c = (column+sizex-nw/2)%sizex;

	    int i = (nh+row)*sizex+c;
	    points[i] = r_start + delta_column * column;
	    points[i] *= .1;
	    if (diagonalFlag)
		m.multVecMatrix(points[i], points[i]);
	}
    }

    // Do bottom triangular piece:
    delta_row.setValue(-1.0/(nh-1), 0, 1.0/(nh-1));
    for (row = 0; row < nh; row++)
    {
	r_start = SbVec3f(1.0, -1.0, -1.0) + delta_row * row;
	
	delta_column.setValue( -(2.0*((nh-1)-row)/(nh-1))/(nw-1), 0, 0);
	
	for (column = 0; column < nw; column++)
	{
	    if (diagonalFlag) c = column;
	    else c = (column+sizex-nw/2)%sizex;

	    int i = (nh*3+row)*sizex+c;
	    points[i] = r_start + delta_column * column;
	    points[i] *= .1;
	    if (diagonalFlag)
		m.multVecMatrix(points[i], points[i]);
	}
    }

    // Now, rotate the piece to create other 3 pieces:
    for (row = 0; row < nh*4; row++)
    {
	for (column = 0; column < nw; column++)
	{
	    if (diagonalFlag) c = column;
	    else c = (column+sizex-nw/2)%sizex;

	    float x, y, z;
	    points[row*sizex + c].getValue(x, y, z);

	    points[row*sizex + (nw+c)%sizex].setValue(z, y, -x);
	    points[row*sizex + (nw*2+c)%sizex].setValue(-x, y, -z);
	    points[row*sizex + (nw*3+c)%sizex].setValue(-z, y, x);
	}
    }
    if (!diagonalFlag)	// HACK WARNING!
    {
	// Force first and last vertices to match
	for (row = 0; row < nh*4; row++)
	{
	    points[row*sizex+sizex-1] = points[row*sizex];
	}
    }

    QuadThing *result = new QuadThing(sizex, sizey, points);

    return result;
}

main(int argc, char **argv)
{
    if (argc > 1) diagonalFlag = 0;

    SoDB::init();

    QuadThing *result = createQuadCube(40, 40);

    // Now write it out to stdout
    SoWriteAction wa;

    wa.apply(result->getSceneGraph());

    return 0;
}
