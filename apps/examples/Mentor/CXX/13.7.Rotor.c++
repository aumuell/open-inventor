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
 *  chapter 13, example 8.
 *
 *  Rotor node example.  
 *  Read in the tower and vanes of a windmill from a file.
 *  Use a rotor node to rotate the vanes.
 *------------------------------------------------------------*/

#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoRotor.h>

SoSeparator *
readFile(const char *filename)
{
   // Open the input file
   SoInput mySceneInput;
   if (!mySceneInput.openFile(filename)) {
      fprintf(stderr, "Cannot open file %s\n", filename);
      return NULL;
   }

   // Read the whole file into the database
   SoSeparator *myGraph = SoDB::readAll(&mySceneInput);
   if (myGraph == NULL) {
      fprintf(stderr, "Problem reading file\n");
      return NULL;
   } 

   mySceneInput.closeFile();
   return myGraph;
}

int
main(int argc, char **argv)
{
   // Initialize Inventor and Xt
   Widget myWindow = SoXt::init(argv[0]);

   SoSeparator *root = new SoSeparator;
   root->ref();

   // Read in the data for the windmill tower
   SoSeparator *windmillTower = 
            readFile("/usr/share/src/Inventor/examples/data/windmillTower.iv");
   root->addChild(windmillTower);

   // Add a rotor node to spin the vanes
   SoRotor *myRotor = new SoRotor;
   myRotor->rotation.setValue(SbVec3f(0, 0, 1), 0); // z axis
   myRotor->speed = 0.2;
   root->addChild(myRotor);

   // Read in the data for the windmill vanes
   SoSeparator *windmillVanes = 
            readFile("/usr/share/src/Inventor/examples/data/windmillVanes.iv");
   root->addChild(windmillVanes);

   // Create a viewer
   SoXtExaminerViewer *myViewer = 
            new SoXtExaminerViewer(myWindow);

   // attach and show viewer
   myViewer->setSceneGraph(root);
   myViewer->setTitle("Windmill");
   myViewer->show();
    
   // Loop forever
   SoXt::show(myWindow);
   SoXt::mainLoop();
}

