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
 *  chapter 3.
 *
 *  Create a little scene graph and then name some nodes and
 *  get back nodes by name.
 *------------------------------------------------------------*/

#include <Inventor/SoDB.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/sensors/SoNodeSensor.h>

// Function protoype
void RemoveCube();

int
main(int , char **)
{
   SoDB::init();

   // Create some objects and give them names:
   SoSeparator *root = new SoSeparator;
   root->ref();
   root->setName("Root");

   SoCube *myCube = new SoCube;
   root->addChild(myCube);
   myCube->setName("MyCube");

   SoSphere *mySphere = new SoSphere;
   root->addChild(mySphere);
   mySphere->setName("MySphere");

   RemoveCube();
}

void
RemoveCube()
{
   // Remove the cube named 'MyCube' from the separator named
   // 'Root'.  In a real application, isOfType() would probably
   // be used to make sure the nodes are of the correct type
   // before doing the cast.

   SoSeparator *myRoot;
   myRoot = (SoSeparator *)SoNode::getByName("Root");

   SoCube *myCube;
   myCube = (SoCube *)SoNode::getByName("MyCube");

   myRoot->removeChild(myCube);
}

