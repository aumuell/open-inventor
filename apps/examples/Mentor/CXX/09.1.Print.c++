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
 *  chapter 9, example 1.
 *
 *  Printing example.
 *  Read in an Inventor file and display it in ExaminerViewer.  Press
 *  the "p" key and the scene renders into a PostScript
 *  file for printing.
 *----------------------------------------------------------*/

#include <stdlib.h>
#include <Inventor/SbViewportRegion.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoOffscreenRenderer.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>

typedef struct cbdata {
    SoXtExaminerViewer *vwr;
    char               *filename;
    SoNode             *scene;
} callbackData;

//////////////////////////////////////////////////////////////
// CODE FOR The Inventor Mentor STARTS HERE

SbBool 
printToPostScript (SoNode *root, FILE *file,
   SoXtExaminerViewer *viewer, int printerDPI)
{
   // Calculate size of the images in inches which is equal to
   // the size of the viewport in pixels divided by the number
   // of pixels per inch of the screen device.  This size in
   // inches will be the size of the Postscript image that will
   // be generated.
   const SbViewportRegion &vp  = viewer->getViewportRegion();
   const SbVec2s &imagePixSize = vp.getViewportSizePixels();
   SbVec2f imageInches;
   float pixPerInch;

   pixPerInch = SoOffscreenRenderer::getScreenPixelsPerInch();
   imageInches.setValue((float)imagePixSize[0] / pixPerInch,
                        (float)imagePixSize[1] / pixPerInch);

   // The resolution to render the scene for the printer
   // is equal to the size of the image in inches times
   // the printer DPI;
   SbVec2s postScriptRes;
   postScriptRes.setValue((short)(imageInches[0]*printerDPI),
                          (short)(imageInches[1]*printerDPI));

   // Create a viewport to render the scene into.
   SbViewportRegion myViewport;
   myViewport.setWindowSize(postScriptRes);
   myViewport.setPixelsPerInch((float)printerDPI);

   // Render the scene
   SoOffscreenRenderer *myRenderer = 
            new SoOffscreenRenderer(myViewport);
   if (!myRenderer->render(root)) {
      delete myRenderer;
      return FALSE;
   }

   // Generate PostScript and write it to the given file
   myRenderer->writeToPostScript(file);

   delete myRenderer;
   return TRUE;
}

// CODE FOR The Inventor Mentor ENDS HERE
//////////////////////////////////////////////////////////////

static void
processKeyEvents( void *data, SoEventCallback *cb )
{
   if (SO_KEY_PRESS_EVENT(cb->getEvent(), P)) {
    
      callbackData *cbData = (callbackData *)data;
      FILE *myFile = fopen(cbData->filename, "w");

      if (myFile == NULL) {
         fprintf(stderr, "Cannot open output file\n");
         exit(1);
      }

      printf("Printing scene... ");
      fflush(stdout);
      if (!printToPostScript(cbData->scene, myFile,
            cbData->vwr, 75)) {
         fprintf(stderr, "Cannot print image\n");
         fclose(myFile);
         exit(1);
      }

      fclose(myFile);
      printf("  ...done printing.\n");
      fflush(stdout);
      cb->setHandled(); 
   } 
}

int
main(int argc, char **argv)
{
   // Initialize Inventor and Xt
   Widget appWindow = SoXt::init(argv[0]);
   if (appWindow == NULL)
      exit(1);

   // Verify the command line arguments
   if (argc != 3) {
      fprintf(stderr, "Usage: %s infile.iv outfile.ps\n", argv[0]);
      exit(1); 
   }
  
   printf("To print the scene: press the 'p' key while in picking mode\n");

   // Make a scene containing an event callback node
   SoSeparator         *root    = new SoSeparator;
   SoEventCallback     *eventCB = new SoEventCallback;
   root->ref();
   root->addChild(eventCB);

   // Read the geometry from a file and add to the scene
   SoInput myInput;
   if (!myInput.openFile(argv[1]))
      exit (1);
   SoSeparator *geomObject = SoDB::readAll(&myInput);
   if (geomObject == NULL)
      exit (1);
   root->addChild(geomObject);

   SoXtExaminerViewer *viewer =
         new SoXtExaminerViewer(appWindow, NULL, TRUE, 
            SoXtExaminerViewer::BUILD_ALL, SoXtExaminerViewer::EDITOR);
   viewer->setSceneGraph(root);
   viewer->setTitle("Print to PostScript");

   // Setup the event callback data and routine for performing the print
   callbackData *data = new callbackData;
   data->vwr = viewer;
   data->filename = argv[2];
   data->scene = viewer->getSceneGraph();
   eventCB->addEventCallback(SoKeyboardEvent::getClassTypeId(),
         processKeyEvents, data);
   viewer->show();

   SoXt::show(appWindow);
   SoXt::mainLoop();
}

