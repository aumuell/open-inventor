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

//
//  Sample SceneViewer program.
//   This program reads in a Inventor datafile, and creates a SceneViewer
//   to allow interaction with it.
//

#include <stdlib.h>
#include <X11/Intrinsic.h>
#include <X11/keysym.h>
#include <locale.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInteraction.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/nodekits/SoNodeKit.h>
#include <Inventor/nodes/SoSelection.h>

#include "SoSceneViewer.h"
#include "../../samples/widgets/MyFileRead.h"

// #define TEST_PICK_CB
// #define DISABLE_SEL_PICK_MATCH

#ifdef TEST_PICK_CB
// Test the selection pick callback mechanism
static SoPath *
selectionPickCB(void *userData, const SoPickedPoint *pick)
{
    // Pick the topmost group beneath the selection node
    SoPath *p = pick->getPath();
    SoSelection *sel = (SoSelection *) userData;
    
    // See which child of selection got picked
    int selIndex = -1;
    if (p->getHead() == sel)
	selIndex = 0;
    else {  
	for (int i = 1; (selIndex == -1) && (i < p->getLength() - 1); i++)
	    if (p->getNode(i) == sel)
		selIndex = i;
    }
    
    // Return a path truncated to this point
    if (selIndex == -1) {
	fprintf(stderr, "SceneViewer selectionPickCB - did not find sel node in pick path!\n");
	return NULL;
    }
    
    return p->copy(selIndex, 2); // 2 nodes, starting at selection node
}
#endif

static SbBool
getArgs(int argc, char **argv, char *&envFile, char *&filename)
{
    SbBool ok = TRUE;
    
    envFile = NULL;
    filename = NULL;

    if (argc == 2) {
	filename = argv[1];
    }
    else if (argc > 2) {
    	// see if there is a -e before the file name
    	if (strcmp(argv[1], "-e") == 0) {
	    envFile = argv[2];
	    if (argc > 3)
	    	filename = argv[3];
	}
	else if (argc == 4) {
	    // maybe the -e is after the file name
	    if (strcmp(argv[2], "-e") == 0) {
		envFile = argv[3];
		filename = argv[1];
	    }
	}
	else ok = FALSE;
    }
    
    return ok;
}

// Stolen from Annotator.
//
static String _myXtDefaultLanguageProc(Display *, String xnl, XtPointer)
{
    if (! setlocale(LC_ALL, xnl))
        XtWarning("locale not supported by C library, locale unchanged");
    
    if (! XSupportsLocale()) {
        XtWarning("locale not supported by Xlib, locale set to C");
        setlocale(LC_ALL, "C");
    }
    if (! XSetLocaleModifiers(""))
        XtWarning("X locale modifiers not supported, using default");

    setlocale(LC_NUMERIC, "C");
    
    return setlocale(LC_CTYPE, NULL);
}

int
main(int argc, char **argv)
{
    Widget	 	mainWindow;
    SoSceneViewer	*sv;
    SoInput		in;
    char		*envFile, *filename;

    XtSetLanguageProc( NULL, _myXtDefaultLanguageProc, NULL );    

    // check usage
    if (! getArgs(argc, argv, envFile, filename)) {
	fprintf(stderr, "usage:  SceneViewer [-e environ.iv] [file.iv]\n");
	exit( 1 );
    }
    
    // workaround for bug 200909 - this will force OpenGL to create
    // a connection with the X server to receive delete window events
    // (so that OpenGL can delete Accumulation and other software
    // buffers when the window is destroyed).
    if (putenv("GL_CHECK_WINDOW_DESTROY=y"))
	fprintf(stderr, "Could not set the GL_CHECK_WINDOW_DESTROY env.\n");
    
    SbBool useStdin = (filename && strcmp(filename,"-")==0);

    // init Inventor
    mainWindow = SoXt::init(argv[0], "SceneViewer");

    // read the file in, creating a selection node as the root
    // of the scene graph.
    SoSelection *selRoot = new SoSelection;
    selRoot->ref();
#ifdef TEST_PICK_CB
    selRoot->setPickFilterCallback(selectionPickCB, selRoot);
#endif

#ifdef DISABLE_SEL_PICK_MATCH
    selRoot->setPickMatching(FALSE);
#endif
    
    SbBool doRead = FALSE;
    SbBool readFromFile = FALSE;
    if (useStdin) {
	doRead = TRUE;
	in.setFilePointer(stdin);
    }
    else if (filename) {
	doRead = TRUE;
	readFromFile = TRUE;
    }
	
    if (doRead) {
	fprintf( stderr, "Reading input file...");
	SoSeparator *readRoot;
	
	// If reading from a file, call MyFileRead in libInventorWidgets sample code.
	// It will do an SoDB::readAll on Inventor files.
	// On non-Inventor files, it will use FTR rules to attempt file conversion
	// to the Inventor format before reading.
	SbString errmsg;
	if (readFromFile)
	     readRoot = MyFileRead(filename, errmsg);
	else readRoot = SoDB::readAll(&in);
	
	if (readRoot != NULL) {
	    selRoot->addChild(readRoot);
	}
	
	if ( selRoot->getNumChildren() == 0 ) {
	    if (readFromFile)
		fprintf(stderr, "%s\n", errmsg.getString());
	    fprintf(stderr, "No data read; creating empty scene.\n" );
	}
	else
	    fprintf(stderr, "done.\n");
    }

    //
    // Create the SceneViewer
    //
    sv = new SoSceneViewer(mainWindow, NULL, TRUE, selRoot, envFile);
    selRoot->unref();

    //
    // Build and show the SceneViewer
    //
    sv->show();
    XtRealizeWidget(mainWindow);

    //
    // Loop forever
    //
    SoXt::mainLoop();
}
