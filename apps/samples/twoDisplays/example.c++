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
 *  This sample shows displaying Inventor viewers on different
 *  X displays.
 *------------------------------------------------------------*/

#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <Xm/Label.h>
#include <stdio.h>

#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoCube.h>

void
main(int argc, char *argv[])
{
   Widget mainWindow = SoXt::init(argv[0]);
   XtAppContext appContext = SoXt::getAppContext();
   
   // Open another display
   Display *altDisplay = NULL;
   if (argc == 2) {
       printf("Opening display %s\n", argv[1]);
       altDisplay = XtOpenDisplay(
	  appContext, 
	  argv[1],     // display name
	  argv[0],     // application name
	  "Inventor",  // class name
	  NULL, 0, 
	  &argc, argv);
   }

   SoXtExaminerViewer *v1 = new SoXtExaminerViewer(mainWindow);
   v1->setSceneGraph(new SoCone);
   v1->show();	
   XtRealizeWidget(mainWindow);

   if (altDisplay != NULL) {
      Widget altWindow = XtAppCreateShell(
         argv[0],     // application name
	 "Inventor",  // class name
	 applicationShellWidgetClass, // widget class
	 altDisplay, 
	 NULL, 0);
      SoXtExaminerViewer *v2 = new SoXtExaminerViewer(altWindow);
      v2->setSceneGraph(new SoCube);
      v2->show();		    
      XtRealizeWidget(altWindow);
   }
         
   SoXt::mainLoop();
}

