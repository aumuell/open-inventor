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

/*--------------------------------------------------------------
 *  This is an example from the Inventor Mentor,
 *  chapter 11, example 2.
 *
 *  Example of creatinge a scene graph by reading from a string.
 *  Create a dodecahedron, made of an IndexedFaceSet.  
 *------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>

#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/nodes/SoSeparator.h>

/////////////////////////////////////////////////////////////
// CODE FOR The Inventor Mentor STARTS HERE

// Reads a dodecahedron from the following string: 
// (Note: ANSI compilers automatically concatenate 
// adjacent string literals together, so the compiler sees 
// this as one big string)

static char *dodecahedron =
   "Separator {"
   "   Normal {"
   "      vector ["
   "         0.553341 0 0.832955, 0.832955 0.553341 0,"
   "         0.832955 -0.553341 0, 0 -0.832955 0.553341,"
   "         -0.553341 0 0.832955, 0 0.832955 0.553341,"
   "         0 0.832955 -0.553341, -0.832955 0.553341 0,"
   "         -0.832955 -0.553341 0, 0 -0.832955 -0.553341,"
   "         0.553341 0 -0.832955, -0.553341 0 -0.832955,"
   "      ]"
   "   }"
   "   NormalBinding { value PER_FACE }"
   "   Material {"
   "      diffuseColor ["
   "         1  0  0,   0 1  0,   0  0 1,   0  1  1,"
   "         1  0  1,  .5 1  0,  .5  0 1,  .5  1  1,"
   "         1 .3 .7,  .3 1 .7,  .3 .7 1,  .5 .5 .8"
   "      ]"
   "   }"
   "   MaterialBinding { value PER_FACE }"
   "   Coordinate3 {"
   "      point ["
   "         1.7265 0 0.618,    1 1 1,"
   "         0 0.618 1.7265,    0 -0.618 1.7265,"
   "         1 -1 1,    -1 -1 1,"
   "         -0.618 -1.7265 0,    0.618 -1.7265 0,"
   "         1 -1 -1,    1.7265 0 -0.618,"
   "         1 1 -1,    0.618 1.7265 0,"
   "         -0.618 1.7265 0,    -1 1 1,"
   "         -1.7265 0 0.618,    -1.7265 0 -0.618,"
   "         -1 -1 -1,    0 -0.618 -1.7265,"
   "         0 0.618 -1.7265,    -1 1 -1"
   "      ]"
   "   }"
   "   IndexedFaceSet {"
   "      coordIndex ["
   "         1, 2, 3, 4, 0, -1,  0, 9, 10, 11, 1, -1,"
   "         4, 7, 8, 9, 0, -1,  3, 5, 6, 7, 4, -1,"
   "         2, 13, 14, 5, 3, -1,  1, 11, 12, 13, 2, -1,"
   "         10, 18, 19, 12, 11, -1,  19, 15, 14, 13, 12, -1,"
   "         15, 16, 6, 5, 14, -1,  8, 7, 6, 16, 17, -1,"
   "         9, 8, 17, 18, 10, -1,  18, 17, 16, 15, 19, -1,"
   "      ]"
   "   }"
   "}";

// Routine to create a scene graph representing a dodecahedron
SoNode *
makeDodecahedron()
{
   // Read from the string.
   SoInput in;
   in.setBuffer(dodecahedron, strlen(dodecahedron));

   SoNode *result;
   SoDB::read(&in, result);

   return result;
}

// CODE FOR The Inventor Mentor ENDS HERE
/////////////////////////////////////////////////////////////

int
main(int argc, char **argv)
{
   // Initialize Inventor and Xt
   Widget myWindow = SoXt::init(argv[0]);
   if (myWindow == NULL) exit(1);

   SoNode *root = makeDodecahedron();
   root->ref();

   SoXtExaminerViewer *myViewer = 
            new SoXtExaminerViewer(myWindow);
   myViewer->setSceneGraph(root);
   myViewer->setTitle("String Reader");
   myViewer->show();

   SoXt::show(myWindow);
   SoXt::mainLoop();
}
