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
 *  chapter 7, example 3.
 *
 *  This program creates an instance of PyramidKit and
 *  puts it in an examinerViewer.
 *------------------------------------------------------------*/
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>

// Header files for new node classes
#include "../02.Nodes/Pyramid.h"
#include "PyramidKit.h"

main(int, char **argv)
{
   // Initialize Inventor and Xt
   Widget myWindow = SoXt::init(argv[0]);
   if (myWindow == NULL) exit(1);

   // Initialize the new node classes
   Pyramid::initClass();
   PyramidKit::initClass();

   PyramidKit *myKit = new PyramidKit;
   myKit->ref();
   myKit->set("material { diffuseColor 0.5 0.5 1.0 }");
   myKit->set("shape { baseWidth 2.0 baseDepth 1.0 height 4.0 }");

   SoXtExaminerViewer *viewer = 
      new SoXtExaminerViewer(myWindow);
   viewer->setSceneGraph(myKit);
   viewer->setTitle("PyramidKit");
   viewer->show();
   viewer->viewAll();

   SoXt::show(myWindow);
   SoXt::mainLoop();
}
