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
// A little class that encapsulates the code that actually does the
// surface of revolution.
//

class SoSeparator;
class SoCoordinate3;
class SoMFVec3f;
class SoNormal;
class SoQuadMesh;

class RevolutionSurface
{
  public:
    RevolutionSurface();
    ~RevolutionSurface();

    //
    // Create a surface of revolution given the x/y coordinates of its
    // profile.
    //
    void createSurface(const SoMFVec3f *);

    //
    // Get the scene graph that represents the surface of revolution
    //
    SoSeparator *getSceneGraph();

    //
    // Change the number of sides in the surface of revolution
    //
    void changeNumSides(int);

  private:
    SoSeparator *root;
    SoCoordinate3 *coords;
    SoNormal *norms;
    SoQuadMesh *qmesh;
    int NumSides;
};

const double creaseAngle = M_PI/6.0;	// 30 degrees
