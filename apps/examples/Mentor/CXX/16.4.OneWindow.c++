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
 *  This is an example from the Inventor Mentor,
 *  chapter 16, example 4.
 *
 *  This example builds a render area and Material Editor within 
 *  a window supplied by the application. It uses a Motif form 
 *  widget to lay both components inside the same window.  
 *  It attaches the editor to the material of an object.
 *-----------------------------------------------------------*/

#include <stdlib.h>
#include <Xm/Form.h>
#include <Inventor/SoDB.h>          
#include <Inventor/Xt/SoXt.h>          
#include <Inventor/Xt/SoXtMaterialEditor.h>
#include <Inventor/Xt/SoXtRenderArea.h>  
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoSeparator.h> 

int
main(int , char **argv)
{
   // Initialize Inventor and Xt
   Widget myWindow = SoXt::init(argv[0]);
   
   // Build the form to hold both components
   Widget myForm = XtCreateWidget("Form", 
            xmFormWidgetClass, myWindow, NULL, 0);
   
   // Build the render area and Material Editor
   SoXtRenderArea *myRenderArea = new SoXtRenderArea(myForm);
   myRenderArea->setSize(SbVec2s(200, 200));
   SoXtMaterialEditor *myEditor = 
            new SoXtMaterialEditor(myForm);
   
   // Layout the components within the form
   Arg  args[8];
   XtSetArg(args[0], XmNtopAttachment,    XmATTACH_FORM);
   XtSetArg(args[1], XmNbottomAttachment, XmATTACH_FORM);
   XtSetArg(args[2], XmNleftAttachment,   XmATTACH_FORM); 
   XtSetArg(args[3], XmNrightAttachment,  XmATTACH_POSITION);
   XtSetArg(args[4], XmNrightPosition,    40);
   XtSetValues(myRenderArea->getWidget(), args, 5);
   XtSetArg(args[2], XmNrightAttachment,  XmATTACH_FORM); 
   XtSetArg(args[3], XmNleftAttachment,   XmATTACH_POSITION);
   XtSetArg(args[4], XmNleftPosition,     41); 
   XtSetValues(myEditor->getWidget(), args, 5);
   
   // Create a scene graph
   SoSeparator *root = new SoSeparator;
   SoPerspectiveCamera *myCamera = new SoPerspectiveCamera;
   SoMaterial *myMaterial = new SoMaterial;
   
   root->ref();
   myCamera->position.setValue(0.212482, -0.881014, 2.5);
   myCamera->heightAngle = M_PI/4;
   root->addChild(myCamera);
   root->addChild(new SoDirectionalLight);
   root->addChild(myMaterial);

   // Read the geometry from a file and add to the scene
   SoInput myInput;
   if (!myInput.openFile("/usr/share/src/Inventor/examples/data/dogDish.iv"))
      exit (1);
   SoSeparator *geomObject = SoDB::readAll(&myInput);
   if (geomObject == NULL)
      exit (1);
   root->addChild(geomObject);
   
   // Make the scene graph visible
   myRenderArea->setSceneGraph(root);
   
   // Attach the material editor to the material in the scene
   myEditor->attach(myMaterial);
   
   // Show the main window
   myRenderArea->show();
   myEditor->show();
   SoXt::show(myForm);    // this calls XtManageChild
   SoXt::show(myWindow);  // this calls XtRealizeWidget
   
   // Loop forever
   SoXt::mainLoop();
}
