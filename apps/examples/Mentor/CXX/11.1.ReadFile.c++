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

/*-----------------------------------------------------------
 *  This is an example from the Inventor Mentor,
 *  chapter 11, example 1.
 *
 *  Example of reading from a file.
 *  Read a file given a filename and return a separator
 *  containing all of the file.  Return NULL if there is 
 *  an error reading the file.
 *----------------------------------------------------------*/

#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/nodes/SoSeparator.h>

/////////////////////////////////////////////////////////////
// CODE FOR The Inventor Mentor STARTS HERE

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

// CODE FOR The Inventor Mentor ENDS HERE 
/////////////////////////////////////////////////////////////

int
main(int argc, char **argv)
{
   // Initialize Inventor and Xt
   Widget myWindow = SoXt::init(argv[0]);

   // Read the file
   SoSeparator *scene = readFile("/usr/share/src/Inventor/examples/data/bookshelf.iv");

   // Create a viewer
   SoXtExaminerViewer *myViewer = 
            new SoXtExaminerViewer(myWindow);

   // attach and show viewer
   myViewer->setSceneGraph(scene);
   myViewer->setTitle("File Reader");
   myViewer->show();
    
   // Loop forever
   SoXt::show(myWindow);
   SoXt::mainLoop();
}

