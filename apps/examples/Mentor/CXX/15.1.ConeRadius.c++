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

/*----------------------------------------------------------------
 *  This is an example from the Inventor Mentor.
 *  chapter 15, example 1.
 *
 *  Uses an SoTranslate1Dragger to control the bottomRadius field 
 *  of an SoCone.  The 'translation' field of the dragger is the 
 *  input to an SoDecomposeVec3f engine. The engine extracts the
 *  x component from the translation. This extracted value is
 *  connected to the bottomRadius field of the cone.
 *----------------------------------------------------------------*/

#include <stdlib.h>
#include <Inventor/engines/SoCompose.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>

#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>

#include <Inventor/draggers/SoTranslate1Dragger.h>

int
main(int , char **argv)
{
   Widget myWindow = SoXt::init(argv[0]);
   if (myWindow == NULL) exit(1);

   SoSeparator *root = new SoSeparator;
   root->ref();

   // Create myDragger with an initial translation of (1,0,0)
   SoTranslate1Dragger *myDragger = new SoTranslate1Dragger;
   root->addChild(myDragger);
   myDragger->translation.setValue(1,0,0);

   // Place an SoCone above myDragger
   SoTransform *myTransform = new SoTransform;
   SoCone      *myCone = new SoCone;
   root->addChild(myTransform);
   root->addChild(myCone);
   myTransform->translation.setValue(0,3,0);

   // SoDecomposeVec3f engine extracts myDragger's x-component
   // The result is connected to myCone's bottomRadius.
   SoDecomposeVec3f *myEngine = new SoDecomposeVec3f;
   myEngine->vector.connectFrom(&myDragger->translation);
   myCone->bottomRadius.connectFrom(&myEngine->x);

   // Display them in a viewer
   SoXtExaminerViewer *myViewer 
      = new SoXtExaminerViewer(myWindow);
   myViewer->setSceneGraph(root);
   myViewer->setTitle("Dragger Edits Cone Radius");
   myViewer->viewAll();
   myViewer->show();

   SoXt::show(myWindow);
   SoXt::mainLoop();
}
