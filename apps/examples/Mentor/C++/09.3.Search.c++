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

/*------------------------------------------------------------
 *  This is an example from the Inventor Mentor,
 *  chapter 9, example 3.
 *
 *  Search Action example.
 *  Read in a scene from a file.
 *  Search through the scene looking for a light.
 *  If none exists, add a directional light to the scene
 *  and print out the modified scene.
 *-----------------------------------------------------------*/

#include <Inventor/SoDB.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoSeparator.h>

main(int argc, char **argv)
{
   // Initialize Inventor
   SoDB::init();

   // Open and read input scene graph
   SoInput sceneInput;
   if (! sceneInput.openFile("/usr/share/src/Inventor/examples/data/bird.iv"))
      return (1);

   SoSeparator *root = SoDB::readAll(&sceneInput);
   if (root == NULL) 
      return (1);
   root->ref();

//////////////////////////////////////////////////////////////
// CODE FOR The Inventor Mentor STARTS HERE

   SoSearchAction mySearchAction;

   // Look for first existing light derived from class SoLight
   mySearchAction.setType(SoLight::getClassTypeId());
   mySearchAction.setInterest(SoSearchAction::FIRST);
    
   mySearchAction.apply(root);
   if (mySearchAction.getPath() == NULL) { // No lights found

      // Add a default directional light to the scene
      SoDirectionalLight *myLight = new SoDirectionalLight;
      root->insertChild(myLight, 0);
   }

// CODE FOR The Inventor Mentor ENDS HERE
//////////////////////////////////////////////////////////////

   SoWriteAction myWriteAction;
   myWriteAction.apply(root);

   root->unref();
   return 0;
}

