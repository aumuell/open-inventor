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

/*-------------------------------------------------------------
 *  This is an example from The Inventor Toolmaker,
 *  chapter 9.
 *
 *  This example demonstrates creating a new highlight style
 *  by subclassing from SoGLRenderAction.
 *------------------------------------------------------------*/

#include <stdlib.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>

#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/nodes/SoSelection.h>

#include "ShowSelectionRA.h"

int
main(int , char *argv[])
{
   // Initialization
   Widget mainWindow = SoXt::init(argv[0]);
   ShowSelectionRenderAction::initClass();
    
   // Open the data file
   SoInput in;   
   char *datafile = "/usr/share/src/Inventor/examples/data/monitor.iv";
   if (! in.openFile(datafile)) {
      fprintf(stderr, "Cannot open %s for reading.\n", datafile);
      exit(1);
   }

   // Read the input file
   SoNode *n;
   SoSeparator *sep = new SoSeparator;
   while ((SoDB::read(&in, n) != FALSE) && (n != NULL))
      sep->addChild(n);
   
   // Create a selection root to show off our new highlight.
   SoSelection *sel = new SoSelection;
   sel->addChild(sep);

   // Create two viewers, one to show the scene, the other
   // to show the selected objects.
   SoXtExaminerViewer *viewer1 = new SoXtExaminerViewer(mainWindow);
   viewer1->setSceneGraph(sel);
   viewer1->setTitle("Scene");

   SoXtExaminerViewer *viewer2 = new SoXtExaminerViewer();
   viewer2->setSceneGraph(sel);
   viewer2->setGLRenderAction(new ShowSelectionRenderAction());    
   viewer2->redrawOnSelectionChange(sel);
   viewer2->setDecoration(FALSE);
   viewer2->setTitle("Selection");

   viewer1->show();
   viewer2->show();
   
   SoXt::show(mainWindow);
   SoXt::mainLoop();
}

