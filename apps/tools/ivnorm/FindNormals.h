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
// Definition for a class that trys to orient a surface and find
// normals for it.
//

#include "Faces.h"

class SoNode;
class SoPath;
class SoMFVec3f;
class SoNormal;

class FindNormals
{
  public:
    FindNormals();
    ~FindNormals();

    //
    // If the application knows that the faces are already
    // consistently oriented (default is UNKNOWN):
    //
    void AssumeOrientation(FaceOrientation);

    //
    // Pass TRUE if this should find vertex normals
    // Pass FALSE if not.
    //
    void findVertexNormals(int);

    // If finding vertex normals, pass in the angle (in radians)
    // beyond which a sharp edge should be formed.
    void setCreaseAngle(float c) { creaseAngle = c; }

    //
    // This finds normals in the given scene graph, and inserts
    // Normal and NormalBinding nodes into the scene graph.  It may
    // also modify the fields in IndexedFaceSet nodes to correct
    // mis-oriented faces and to set the normalIndex field.
    //
    void apply(SoNode *, int);

    //
    // turn on trace output
    //
    void setVerbose(SbBool v) { verbose = v; }
    SbBool isVerbose() const { return verbose; }

  private:
    FaceOrientation defaultOrientation;
    float creaseAngle;

    SoNode *searchLastType(SoPath *, SoType);
    void doIndexedFaceSet(SoPath *, int);

    SbBool verbose;
};
