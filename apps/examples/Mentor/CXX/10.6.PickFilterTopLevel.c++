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
 *  This is an example from The Inventor Mentor,
 *  chapter 10, example 6.
 *
 *  This example demonstrates the use of the pick filter
 *  callback to implement a top level selection policy.
 *  That is, always select the top most group beneath the
 *  selection node,  rather than selecting the actual
 *  shape that was picked.
 *------------------------------------------------------------*/

#include <stdlib.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>

#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoPath.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/actions/SoBoxHighlightRenderAction.h>
#include <Inventor/nodes/SoSelection.h>


// Pick the topmost node beneath the selection node
SoPath *
pickFilterCB(void *, const SoPickedPoint *pick)
{    
   // See which child of selection got picked
   SoPath *p = pick->getPath();
   int i;
   for (i = 0; i < p->getLength() - 1; i++) {
      SoNode *n = p->getNode(i);
      if (n->isOfType(SoSelection::getClassTypeId()))
         break;
   }
   
   // Copy 2 nodes from the path:
   // selection and the picked child
   return p->copy(i, 2);
}


int
main(int argc, char *argv[])
{
   // Initialization
   Widget mainWindow = SoXt::init(argv[0]);
    
   // Open the data file
   SoInput in;   
   char *datafile = "/usr/share/src/Inventor/examples/data/parkbench.iv";
   if (! in.openFile(datafile)) {
      fprintf(stderr, "Cannot open %s for reading.\n", datafile);
      exit(1);
   }

   // Read the input file
   SoNode *n;
   SoSeparator *sep = new SoSeparator;
   while ((SoDB::read(&in, n) != FALSE) && (n != NULL))
      sep->addChild(n);
   
   // Create two selection roots - one will use the pick filter.
   SoSelection *topLevelSel = new SoSelection;
   topLevelSel->addChild(sep);
   topLevelSel->setPickFilterCallback(pickFilterCB);

   SoSelection *defaultSel = new SoSelection;
   defaultSel->addChild(sep);

   // Create two viewers, one to show the pick filter for top level
   // selection, the other to show default selection.
   SoXtExaminerViewer *viewer1 = new SoXtExaminerViewer(mainWindow);
   viewer1->setSceneGraph(topLevelSel);
   viewer1->setGLRenderAction(new SoBoxHighlightRenderAction());
   viewer1->redrawOnSelectionChange(topLevelSel);
   viewer1->setTitle("Top Level Selection");

   SoXtExaminerViewer *viewer2 = new SoXtExaminerViewer();
   viewer2->setSceneGraph(defaultSel);
   viewer2->setGLRenderAction(new SoBoxHighlightRenderAction());    
   viewer2->redrawOnSelectionChange(defaultSel);
   viewer2->setTitle("Default Selection");

   viewer1->show();
   viewer2->show();
   
   SoXt::show(mainWindow);
   SoXt::mainLoop();
}

