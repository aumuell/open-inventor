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
 *  This is an example from the Inventor Toolmaker,
 *  chapter 8.
 *
 *  This program initializes the node classes from this chapter,
 *  reads a scene graph from "newManips.iv", writes the graph to 
 *  stdout, and renders the graph in an examiner viewer.
 *------------------------------------------------------------*/

#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/nodes/SoSeparator.h>

// Header files for new node classes
#include "TranslateRadialDragger.h"
#include "RotTransDragger.h"
#include "TranslateRadialManip.h"
#include "RotTransManip.h"
#include "Coordinate3Manip.h"

main(int, char **argv)
{
   SoInput      myInput;
   SoSeparator  *root;

   // Initialize Inventor and Xt
   Widget myWindow = SoXt::init(argv[0]);
   if (myWindow == NULL) exit(1);

   // Initialize the new node classes
   TranslateRadialDragger::initClass();
   RotTransDragger::initClass();
   TranslateRadialManip::initClass();
   RotTransManip::initClass();
   Coordinate3Manip::initClass();

   if (! myInput.openFile("newManips.iv")) {
      fprintf(stderr, "Can't open \"newManips.iv\"\n");
      return 1;
   }

   root = SoDB::readAll(&myInput);
   if (root == NULL) {
      printf("File \"newManips.iv\" contains bad data\n");
      return 2;
   }

   root->ref();

   // Write the graph to stdout
   SoWriteAction wa;
   wa.apply(root);

   // Render it
   SoXtExaminerViewer *myViewer =
      new SoXtExaminerViewer(myWindow);
   myViewer->setSceneGraph(root);
   myViewer->setTitle("New Manips");
   myViewer->show();
   myViewer->viewAll();

   SoXt::show(myWindow);
   SoXt::mainLoop();

   return 0;
}
