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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifdef __sgi
#include <alloca.h>
#endif
#include <assert.h>
#include <locale.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/Xt/viewers/SoXtWalkViewer.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/nodes/SoCallback.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoShape.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/sensors/SoTimerSensor.h>

#include "../../samples/widgets/MyColorEditor.h"
#include "ivviewMenus.h"

#define SCREEN(w) XScreenNumberOfScreen(XtScreen(w))

typedef struct {
    int    count;	// Number of files read
    char **names;	// File names
    SoNode *original;	// Un-munged scene
    SoSeparator *optimized;	// Optimized scene
    SoSeparator *ivfixed;	// Ivfixed scene
} FileInfo;

// Function prototypes 
static void print_usage(const char *);
static void parse_args(int , char **);
static SbBool readScene(FileInfo &);	// Returns TRUE if error
static void initPerfMeter(SoXtViewer *);
static void viewStartCB(void *, SoXtViewer *);
static void setBusyCursor(SbBool);
static void createBusyCursor(Display *);
static void overlayViewportCB(void *, SoAction *);
static void countUpdatesCB(void *, SoAction *);
static void perfSensorCB(void *, SoSensor *);
static void showAboutDialog();
static void getNewScene();
static void optimizationChanged(FileInfo &, SbBool);
static void newSceneCB(Widget, void *, XmFileSelectionBoxCallbackStruct *);
static void editBackgroundColor();
static void backgroundColorCB(void *, const SbColor *);
static void processTopbarEvent(Widget, ivMenuItem *, XmAnyCallbackStruct *);
static Widget buildAndLayoutMenus(Widget );
static SoXtViewer *buildAndLayoutViewer(Widget , Widget, SbBool);
static SoSeparator *addUpdateCounter(SoNode *);
static int countTriangles(SoNode *);
static void countTriangleCB(void *, SoCallbackAction *, 
		const SoPrimitiveVertex *, const SoPrimitiveVertex *, 
		const SoPrimitiveVertex *);

// Global variables 
static SoXtViewer *viewer = NULL;
static Widget shell = NULL;
static SbBool useWalkViewer = FALSE;
static FileInfo files = { 0, NULL, NULL, NULL, NULL };
static SbBool showPerfMeter = FALSE;
static int numTris = 0;
static int numFramesRendered = 0;
static SbBool ivfix = FALSE;
static SbBool shapeHintBackface = FALSE;

// Definitions for busy cursor:
static Cursor busyCursor = 0;
#define hourglass_width 17
#define hourglass_height 21
#define hourglass_x_hot 8
#define hourglass_y_hot 11
static char hourglass_bits[] = {
   0x00, 0x00, 0x00, 0xfe, 0xff, 0x00, 0x04, 0x40, 0x00, 0x04, 0x40, 0x00,
   0x04, 0x40, 0x00, 0xe8, 0x2e, 0x00, 0xd0, 0x17, 0x00, 0xa0, 0x0b, 0x00,
   0x40, 0x05, 0x00, 0x40, 0x05, 0x00, 0x40, 0x04, 0x00, 0x40, 0x04, 0x00,
   0x40, 0x04, 0x00, 0x20, 0x09, 0x00, 0x10, 0x11, 0x00, 0x88, 0x23, 0x00,
   0xc4, 0x47, 0x00, 0xe4, 0x4f, 0x00, 0xf4, 0x5f, 0x00, 0xfe, 0xff, 0x00,
   0x00, 0x00, 0x00};
   
static char hourglass_mask_bits[] = {
   0xff, 0xff, 0x01, 0xff, 0xff, 0x01, 0xfe, 0xff, 0x00, 0xfe, 0xff, 0x00,
   0xfe, 0xff, 0x00, 0xfc, 0x7f, 0x00, 0xf8, 0x3f, 0x00, 0xf0, 0x1f, 0x00,
   0xe0, 0x0f, 0x00, 0xe0, 0x0f, 0x00, 0xe0, 0x0f, 0x00, 0xe0, 0x0f, 0x00,
   0xe0, 0x0f, 0x00, 0xf0, 0x1f, 0x00, 0xf8, 0x3f, 0x00, 0xfc, 0x7f, 0x00,
   0xfe, 0xff, 0x00, 0xfe, 0xff, 0x00, 0xfe, 0xff, 0x00, 0xff, 0xff, 0x01,
   0xff, 0xff, 0x01};

//////////////////////////////////////////////////////////////////////
//
// Print the help message
//
static void
print_usage(const char *progname)
{
    fprintf(stderr, "Usage: %s [-hpw] [infiles]\n", progname);
    fprintf(stderr,
	    "\t-h : Print this message (help) and exit\n"
	    "\t-p : Enable performance meter\n"
	    "\t-w : Use walk viewer (examiner viewer is default)\n"
	    );

    exit(99);
}

/////////////////////////////////////////////////////////////////////
//
// Parse the command line arguments
//
static void
parse_args(int argc, char **argv)
{
    int err = FALSE;	// Flag: error in options?
    int c;
    
    showPerfMeter = FALSE;

    while ((c = getopt(argc, argv, "hpqw")) != -1) {
	switch(c) {
	  case 'h':	// Help
	    err = TRUE;
	    break;
	  case 'p':
	    showPerfMeter = TRUE;
	    break;
	  case 'q':
	    // no-op for backward compatibility only
	    break;
	  case 'w':
	    useWalkViewer = TRUE;
	    break;
	  default:
	    err = TRUE;
	    break;
	}
    }

    // Can't handle -pw, at least right now:
    if (showPerfMeter && useWalkViewer) {
	fprintf(stderr, "Sorry, can't show performance when using "
		"walk viewer.  Ignoring -p option\n");
	showPerfMeter = FALSE;
    }

    /* Handle multiple filenames */
    if (optind == argc) {
	// Act like one argument "-" was given
	argc++;
	argv[optind] = "-";
    }
	
    files.count = argc - optind;
    files.names = (char **) malloc (files.count * sizeof(char *));
    for (int i=0; i<files.count; i++, optind++) 
	files.names[i] = argv[optind];

    if (err)
	print_usage(argv[0]);
}


/////////////////////////////////////////////////////////////////////
//
// Read the scene from the file(s).
// If there are multiple files, combine their contents under one separator.
// Returns TRUE if read was OK, FALSE if read error.
//
static SbBool
readScene(FileInfo &files)
{
    SoInput in;
    SoSeparator *root;
    
    if (files.count == 0) 
	return 0;

    // Watch out!  There's a hidden dependence here between this code
    // and the code in the optimizationChanged routine that runs files
    // through ivfix.  In particular, the optimizationChanged code
    // assumes that, if stdin is being read from, that no other
    // filenames are given.
    if (files.count==1 && strcmp(files.names[0], "-")==0) {
	// Read from stdin
	if (isatty(fileno(stdin))) {
	    fprintf(stderr, "Trying to read from standard input, ");
	    fprintf(stderr, "but standard input is a tty!\n");
	    return 0;
	} 
	else 
	    in.setFilePointer(stdin);

	root = SoDB::readAll(&in);
	if (root == NULL) 
	    fprintf(stderr, "Error reading file %s\n", files.names[0]);
	else
	    root->ref();
	
	// SoInput destructor automatically closes file.
    }
    else {  	// Not reading from stdin:
	root = new SoSeparator;
	root->ref();

	//
	// Combine all the files under one separator
	//
        for (int i=0; i<files.count; i++) {
	    if (!in.openFile(files.names[i])) {
		root->unref();
		return 0;
	    }
	    SoSeparator *inputRoot = SoDB::readAll(&in);
	    if (inputRoot == NULL) {
		root->unref();
		return 0;
	    }
	    root->addChild(inputRoot);
	    in.closeFile();
        } 
    }

    if (root && showPerfMeter) {
	SoSeparator *oldRoot = root;
	root = addUpdateCounter(oldRoot);
	root->ref();
	oldRoot->unref();
	numTris = countTriangles(root);
    }

    // Nuke old scene(s):
    if (files.original) {
	files.original->unref();
	files.original = NULL;
    }
    if (files.optimized) {
	files.optimized->unref();
	files.optimized = NULL;
    }
    if (files.ivfixed) {
	files.ivfixed->unref();
	files.ivfixed = NULL;
    }

    // And create new ones:
    if (root) {
	files.original = root;
	optimizationChanged(files, FALSE);
    }
    
    return (root != NULL);
}

/////////////////////////////////////////////////////////////////////
//
// Called when optimization options change, or when a new file is
// read.
//
static void
optimizationChanged(FileInfo &files, SbBool maintainCamera)
{
    // If no original file, have nothing to do:
    if (files.original == NULL) return;

    // We'll recreate the optimized graph:
    if (files.optimized) {
	files.optimized->unref();
	files.optimized = NULL;
    }
    if (ivfix) {
	SbBool err = FALSE;
	char *tempFile = NULL;
	setBusyCursor(TRUE);

	if (files.ivfixed == NULL) do {

	    // First, special case-- read from stdin:
	    if (files.count==1 && strcmp(files.names[0], "-")==0) {
		tempFile = tempnam(NULL, "ivfix");
		if (tempFile == NULL) {
		    err = 1;
		    break;
		}
		// 
		// Run ivfix on the original scene
		//

		// String long enough for temp filename plus "ivfix >"
		char ivfixCommand[L_tmpnam + 10];
		sprintf(ivfixCommand, "ivfix > %s", tempFile);
		FILE *ivfixPipe = popen(ivfixCommand, "w");
		if (ivfixPipe == NULL) {
		    err = TRUE;
		    break;
		}

		// Write to ivfix through pipe:
		SoOutput out;
		out.setBinary(TRUE);
		out.setFilePointer(ivfixPipe);
		SoWriteAction wa(&out);
		wa.apply(files.original);
		if (ferror(ivfixPipe)) {
		    err = TRUE;
		    break;
		}
		pclose(ivfixPipe);

		// Now read tempFile:
		SoInput in;
		if (!in.openFile(tempFile)) {
		    err = TRUE;
		    break;
		}
		files.ivfixed = SoDB::readAll(&in);
		if (files.ivfixed == NULL) {
		    err = TRUE;
		    break;
		} else {
		    files.ivfixed->ref();
		}
		in.closeFile();
	    }
	    else {  // Not reading from stdin:
		files.ivfixed = new SoSeparator;
		files.ivfixed->ref();
		for (int i = 0; i < files.count; i++) {
		    const char *filename = files.names[i];
		    char *ivfixCommand = (char *)
			alloca(strlen("ivfix ")+strlen(filename)+1);
		    sprintf(ivfixCommand, "ivfix %s", filename);

		    // Read from ivfix:
		    FILE *ivfixPipe = popen(ivfixCommand, "r");
		    if (ivfixPipe == NULL) {
			files.ivfixed->unref();
			files.ivfixed = NULL;
			err = TRUE;
			break;
		    }

		    // If the filename includes a directory path, add
		    // the directory name to the list of directories
		    // where to look for input files:
		    const char *slashPtr;
		    char *searchPath = NULL;
		    if ((slashPtr = strrchr(filename, '/')) != NULL) {
			searchPath = strdup(filename);
			searchPath[slashPtr - filename] = '\0';
			SoInput::addDirectoryFirst(searchPath);
		    }

		    SoInput in;
		    in.setFilePointer(ivfixPipe);
		    SoSeparator *root = SoDB::readAll(&in);

		    if (searchPath) {
			SoInput::removeDirectory(searchPath);
			free(searchPath);
		    }

		    if (root == NULL) {
			files.ivfixed->unref();
			files.ivfixed = NULL;
			err = TRUE;
			break;
		    }
		    files.ivfixed->addChild(root);
		    
		    pclose(ivfixPipe);
		}
	    }

	    if (showPerfMeter) {
		SoNode *oldRoot = files.ivfixed;
		files.ivfixed = addUpdateCounter(oldRoot);
		files.ivfixed->ref();
		oldRoot->unref();
		numTris = countTriangles(files.ivfixed);
	    }
	} while (0);
	
	if (files.ivfixed) {
	    files.optimized = files.ivfixed;
	    files.optimized->ref();
	}
	
	setBusyCursor(FALSE);

	// Clean up temporary file (whether or not there was an error):
	if (tempFile && unlink(tempFile) < 0) {
	    fprintf(stderr, "Warning: Error removing %s after ivfix\n",
		    tempFile);
	}
	if (tempFile) free(tempFile);

	// If there was an error:
	if (err) {
	    system("xmessage 'Error running ivfix' > /dev/null");
	}
    }

    if (shapeHintBackface) {
	SoSeparator *optRoot = new SoSeparator;
	optRoot->renderCaching = SoSeparator::OFF;
	optRoot->boundingBoxCaching = SoSeparator::OFF;
	optRoot->pickCulling = SoSeparator::OFF;

	if (files.optimized != NULL) {
	    // user also wants ivfix....
	    optRoot->addChild(files.ivfixed);
	    files.optimized->unref();
	    files.optimized = NULL;
	} else {
	    optRoot->addChild(files.original);
	}
	files.optimized = optRoot;
	files.optimized->ref();

	// Insert a ShapeHints node as the first child of files.optimized
	SoShapeHints *hints = new SoShapeHints;
	hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
	hints->shapeType = SoShapeHints::SOLID;
	hints->setOverride(TRUE);
	files.optimized->insertChild(hints, 0);
    }

    //
    // Set scene graph to either optimized or original scene graph:
    //
    SoNode *currentCamera = NULL;
    if (maintainCamera) {
	SoNode *viewerCamera = viewer->getCamera();
	if (viewerCamera) {
	    // Make a copy:
	    currentCamera = viewerCamera->copy(FALSE);
	    currentCamera->ref();
	}
    }

    if (files.optimized) {
	// optimized AND/OR ivfixed
	if (viewer->getSceneGraph() != files.optimized)
	    viewer->setSceneGraph(files.optimized);
    } else {
	if (viewer->getSceneGraph() != files.original) {
	    viewer->setSceneGraph(files.original);
	}
    }
    if (currentCamera) {
	SoNode *viewerCamera = viewer->getCamera();
	if (viewerCamera) {
	    // Copy back:
	    viewerCamera->copyFieldValues(currentCamera, FALSE);
	}
	currentCamera->unref();
    }
}

////////////////////////////////////////////////////////////////////
//
// Initialize the performance meter in the overlay planes
//
static void
initPerfMeter(SoXtViewer *vwr)
{
    //
    // Create the overlay scene    
    //
    SoSeparator *perfMeter = new SoSeparator;

    // Add a callback node which will set the viewport 
    SoCallback *overlayCB = new SoCallback;
    overlayCB->setCallback(overlayViewportCB);
    perfMeter->addChild(overlayCB);

    // Add an orthographic camera
    perfMeter->addChild(new SoOrthographicCamera());

    // Add the text string to display the tris per second
    char str[150];
    SoFont *perfMeterFont = new SoFont;
    perfMeterFont->size = 18;
    perfMeterFont->name = "Helvetica";
    perfMeter->addChild(perfMeterFont);
    SoText2 *perfMeterText = new SoText2;
    perfMeterText->justification = SoText2::LEFT;
    sprintf (str, "%d tris/frame", numTris);
    perfMeterText->string.setValue(str);
    perfMeter->addChild(perfMeterText);

    SbColor col(1, 1, 1);
    vwr->setOverlayColorMap(1, 1, &col);
    vwr->setOverlaySceneGraph(perfMeter);

    // Update the performance meter every two seconds
    SoTimerSensor *perfSensor = 
		new SoTimerSensor(perfSensorCB, perfMeterText);
    perfSensor->setInterval(2.0);
    perfSensor->schedule();

    // Reset the meter to only show the number of tris/frame -
    // otherwise the numbers might look misleading
    vwr->addStartCallback(viewStartCB, perfMeterText);
}


static void
overlayViewportCB(void *, SoAction *action)
{
    if (action->isOfType(SoGLRenderAction::getClassTypeId())) 
        glViewport(0, 0, 40, 40);
}


////////////////////////////////////////////////////////////////////
//
// A sensor callback in the overlay scene updates the
// performance meter while the object is animating.
// 
static void
perfSensorCB(void *data, SoSensor *)
{
    SoText2 *perfText = (SoText2 *) data;
    static SbBool wasAnimating = FALSE;
    static SbTime startTime; 
    static double lastFrames = 0;
    char str[100];
    
    assert(!useWalkViewer);

    if (((SoXtExaminerViewer *)viewer)->isAnimating()) {
	if (wasAnimating) {
    	    SbTime timeDiff = SbTime::getTimeOfDay() - startTime;
    	    double frames = 
		numFramesRendered * 1000.0 / timeDiff.getMsecValue();
	    frames = floor(frames * 100.0 + 0.5) / 100.0;

	    // Update the perf meter display if the numbers have changed
	    if (frames != lastFrames) {
                sprintf (str, 
			"%d tris/frame,   %.2lf frames/sec,   %.0lf tris/sec", 
			numTris, frames, frames * numTris);
    	        perfText->string.setValue(str);
	        lastFrames = frames;
	    }
	        
	    // Don't reset the timer here.  By using the total
	    // time from when the object started animating
	    // we will get more accurate numbers - especially 
	    // for slow objects.
	}
	else {
	    // Reset the timer when we detect the object has
	    // started animating.
            numFramesRendered = 0;
            startTime = SbTime::getTimeOfDay();
	    wasAnimating = TRUE;
	}
    }
    else
	wasAnimating = FALSE;
}

//
// Reset the meter to only show the tris/sec number.
// Otherwise the numbers might look misleading
//
static void
viewStartCB(void *data, SoXtViewer *)
{
    char str[150];
    SoText2 *perfText = (SoText2 *) data;

    sprintf (str, "%d tris/frame", numTris);
    perfText->string.setValue(str);
}

////////////////////////////////////////////////////////////////////
//
// Turn busy cursor on/off
//
static void
setBusyCursor(SbBool showBusy)
{
    Display *display = shell ? XtDisplay(shell) : NULL;
    Window window = shell ? XtWindow(shell) : (Window) NULL;

    if (!shell || !display || !window) return;

    // make sure we have defined the cursor
    if (! busyCursor)
	createBusyCursor(display);

    if (showBusy)
	XDefineCursor(display, window, busyCursor);
    else
	XUndefineCursor(display, window);
    
    if (viewer)
	viewer->setCursorEnabled(!showBusy);
    
    // force the busy cursor to show up right now
    if (showBusy)
	XSync(display, False);
}

////////////////////////////////////////////////////////////////////
//
// Define busy cursor
//
static void
createBusyCursor(Display *display)
{
    Drawable d = DefaultRootWindow(display);
    XColor foreground, background;
    foreground.red = 65535;
    foreground.green = foreground.blue = 0;
    background.red = background.green = background.blue = 65535;
    
    Pixmap source = XCreateBitmapFromData(display, d, 
	hourglass_bits, hourglass_width, hourglass_height);
    Pixmap mask = XCreateBitmapFromData(display, d, 
	hourglass_mask_bits, hourglass_width, hourglass_height);
    
    busyCursor = XCreatePixmapCursor(display, source, mask, 
	&foreground, &background, hourglass_x_hot, hourglass_y_hot);
    
    XFreePixmap(display, source);
    XFreePixmap(display, mask);
}

////////////////////////////////////////////////////////////////////
//
// Add a callback node to the main scene which will count the number 
// of frames that have been rendered (to compute the tris/sec).
// Make sure the callback doesn't get cached or ruin the caching for
// the rest of the scene
//
static SoSeparator *
addUpdateCounter(SoNode *root)
{
    SoSeparator *newRoot = new SoSeparator;
    newRoot->renderCaching = SoSeparator::OFF;
    newRoot->boundingBoxCaching = SoSeparator::OFF;
    newRoot->pickCulling = SoSeparator::OFF;

    SoCallback *countUpdates = new SoCallback;
    countUpdates->setCallback(countUpdatesCB);

    newRoot->addChild(countUpdates);
    newRoot->addChild(root);

    return (newRoot);
}
	
static void
countUpdatesCB(void *, SoAction *action)
{
    if (action->isOfType(SoGLRenderAction::getClassTypeId())) {
	numFramesRendered++;
    }
}

///////////////////////////////////////////////////////////////////
//
// Brings up the "About..." dialog
//
static void
showAboutDialog()
{
   if (access(IVPREFIX "/share/help/ivview/ivview.about", R_OK) != 0) {
        system("xmessage 'Sorry, could not find "
               IVPREFIX "/share/help/ivview/ivview.about' > /dev/null");
        return;
    }

    char command[100];
    sprintf(command, "which acroread > /dev/null");

    int err = system(command);
    if (err) {
        system("xmessage 'You must install acroread"
               " for this function to work' > /dev/null");
        return;
    }

    sprintf(command, "acroread " IVPREFIX "/share/help/ivview/ivview.about &");
    system(command);
}

///////////////////////////////////////////////////////////////////
//
// Use a motif file selection dialog to get the new filename.  
// Set up a callback to open the file and create the new scenegraph.
//
static void
getNewScene()
{
    static Widget fileDialog = NULL;


    if (fileDialog == NULL) {
        Arg args[5];
        int n = 0;

        // Unmanage when ok/cancel are pressed
        XtSetArg(args[n], XmNautoUnmanage, TRUE); n++;
        fileDialog = XmCreateFileSelectionDialog(
            shell, "File Dialog", args, n);

        XtAddCallback(fileDialog, XmNokCallback,
                      (XtCallbackProc)newSceneCB, NULL);
    }

    // Manage the dialog
    XtManageChild(fileDialog);
}

//
// Callback routine that gets called when the new filename
// has been entered
//
static void
newSceneCB(Widget, void *, XmFileSelectionBoxCallbackStruct *data)
{

    // Get the file name
    char *filename;
    XmStringGetLtoR(data->value,
        (XmStringCharSet) XmSTRING_DEFAULT_CHARSET, &filename);
   
    files.count = 1;
    files.names[0] = strdup(filename);
    XtFree(filename);

    setBusyCursor(TRUE);
    SbBool readOk = readScene(files);
    setBusyCursor(FALSE);

    if (!readOk) {
        char str[100];
        strcpy(str, "Error reading file: ");
        strcat(str, files.names[0]);
        SoXt::createSimpleErrorDialog(shell, "File Error Dialog", str);
        return;
    } 
}


///////////////////////////////////////////////////////////////////
//
// Modify the viewer's background color using the color editor
// from the sample directory
//
static void
editBackgroundColor()
{
    static MyColorEditor *backgroundColorEditor = NULL;

    if (backgroundColorEditor == NULL) {
	backgroundColorEditor = new MyColorEditor;
	backgroundColorEditor->setColor(SbColor(0, 0, 0));
        backgroundColorEditor->setTitle( "Background Color" );
        backgroundColorEditor->addColorChangedCallback(
            			backgroundColorCB);
    }

    backgroundColorEditor->show();
}

//
// Callback routine that gets called when the background
// color editor values change
//
static void
backgroundColorCB(void *, const SbColor *color)
{
    viewer->setBackgroundColor(*color);
}


///////////////////////////////////////////////////////////////////
//
// Process the topbar menu events.
static void
processTopbarEvent(Widget, ivMenuItem *data, XmAnyCallbackStruct *)
{
    switch (data->id) {

    //
    // File
    //
 
    case IV_FILE_ABOUT:
 	showAboutDialog();
	break;

    case IV_FILE_OPEN:
	getNewScene();
	break;

    case IV_FILE_QUIT:
	exit (0);
	break;	

    //
    // Edit
    //
  
    case IV_EDIT_TRANSPARENCY:
	// Toggle the transparency mode
	if (viewer->getTransparencyType() == SoGLRenderAction::SCREEN_DOOR)
	    viewer->setTransparencyType(SoGLRenderAction::DELAYED_BLEND);
	else
            viewer->setTransparencyType(SoGLRenderAction::SCREEN_DOOR);
	break;

    case IV_EDIT_BACKGROUND_CLR:
	editBackgroundColor();
	break;

    //
    // Optimize
    //
    case IV_OPTIMIZE_IVFIX:
	ivfix = !ivfix;
	optimizationChanged(files, TRUE);
	break;

    case IV_OPTIMIZE_SHAPEHINTS:
	shapeHintBackface = !shapeHintBackface;
	optimizationChanged(files, TRUE);
	break;

    } 
}

///////////////////////////////////////////////////////////////////////
//
// Create the top menu bar and the associated menus
//
static Widget
buildAndLayoutMenus(Widget parent)
{
    ivMenuItem *menuItems = new ivMenuItem[IV_MENU_NUM];
    int i;
    for (i=0; i<IV_MENU_NUM; i++) {
	menuItems[i].id = i;
	menuItems[i].widget = NULL;
    }

    // 
    // Create the topbar menu
    //
    Widget menuWidget = XmCreateMenuBar(parent, "menuBar", NULL, 0);

    Arg popupargs[4];
    int popupn = 0;
#ifdef MENUS_IN_POPUP
    Widget shell = SoXt::getShellWidget(menuWidget);
    SoXt::getPopupArgs(XtDisplay(menuWidget), SCREEN(menuWidget),
		       popupargs, &popupn);
#endif

    int itemCount = XtNumber(pulldownData);
    WidgetList buttons = (WidgetList) XtMalloc(itemCount * sizeof(Widget));

    
    Arg args[12];
    int n;
    for (i=0; i<itemCount; i++) {
	//
        // Make Topbar menu button
	//
        Widget subMenu = 
		XmCreatePulldownMenu(menuWidget, "subMenu", popupargs, popupn);

#ifdef MENUS_IN_POPUP
	// register callbacks to load/unload the pulldown colormap when the
	// pulldown menu is posted.
	SoXt::registerColormapLoad(subMenu, shell);
#endif
	
        int id = pulldownData[i].id;
        menuItems[id].widget = subMenu;

        XtSetArg(args[0], XmNsubMenuId, subMenu);
        buttons[i] = XtCreateWidget(pulldownData[i].name,
            		xmCascadeButtonGadgetClass, menuWidget, args, 1);

	//
        // Make subMenu buttons
	//
        int subItemCount = pulldownData[i].subItemCount;
        WidgetList subButtons = 
		(WidgetList) XtMalloc(subItemCount * sizeof(Widget));

        for (int j=0; j<subItemCount; j++) {
            if (pulldownData[i].subMenu[j].buttonType == IV_SEPARATOR) {
                subButtons[j] = XtCreateWidget(
			NULL, xmSeparatorGadgetClass, subMenu, NULL, 0);
  	    }
            else {
		String callbackReason;
		WidgetClass widgetClass;

                switch (pulldownData[i].subMenu[j].buttonType) {
                    case IV_PUSH_BUTTON:
                        widgetClass = xmPushButtonGadgetClass;
                        callbackReason = XmNactivateCallback;
                        n = 0;
                        break;
                    case IV_TOGGLE_BUTTON:
                        widgetClass = xmToggleButtonGadgetClass;
                        callbackReason = XmNvalueChangedCallback;
                        n = 0;
                        break;
                    case IV_RADIO_BUTTON:
                        widgetClass = xmToggleButtonGadgetClass;
                        callbackReason = XmNvalueChangedCallback;
                        XtSetArg(args[0], XmNindicatorType, XmONE_OF_MANY);
                        n = 1;
                        break;
                    default:
                        fprintf(stderr, 
				"ivview INTERNAL ERROR: bad buttonType\n");
                        break;
                }

		//
                // Check for keyboard accelerator
		//
                char *accel = pulldownData[i].subMenu[j].accelerator;
                char *accelText = pulldownData[i].subMenu[j].accelText;
                XmString xmstr = NULL;
                if (accel != NULL) {
                    XtSetArg(args[n], XmNaccelerator, accel); n++;

                    if (accelText != NULL) {
                        xmstr = XmStringCreate(accelText,
                                         XmSTRING_DEFAULT_CHARSET);
                        XtSetArg(args[n], XmNacceleratorText, xmstr); n++;
                    }
                }

                subButtons[j] = XtCreateWidget(
					pulldownData[i].subMenu[j].name,
                    			widgetClass, subMenu, args, n);
                if (xmstr != NULL)
                    XmStringFree(xmstr);
                id = pulldownData[i].subMenu[j].id;
                menuItems[id].widget = subButtons[j];
                XtAddCallback(subButtons[j], callbackReason,
                    (XtCallbackProc)processTopbarEvent,
                    (XtPointer) &menuItems[id]);
            }
        }
        XtManageChildren(subButtons, subItemCount);
        XtFree((char *)subButtons);
    }
    XtManageChildren(buttons, itemCount);
    XtFree((char *)buttons);

    //
    // Layout the menu bar
    //
    n = 0;
    XtSetArg(args[n], XmNtopAttachment,   XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment,  XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetValues(menuWidget, args, n);
    XtManageChild(menuWidget);

    return (menuWidget);
}


///////////////////////////////////////////////////////////////////
// Create the viewer and lay it out so that it is attached
// to the bottom of the menu bar
//
static SoXtViewer *
buildAndLayoutViewer(Widget parent, Widget menuWidget, SbBool walk)
{
    //
    //
    Arg args[12];
    int n = 0;
    SoXtViewer *viewer = NULL;
    if (!walk)
	viewer = new SoXtExaminerViewer(parent);
    else
	viewer = new SoXtWalkViewer(parent);

    viewer->setViewing(TRUE);	// default to VIEW
    
    XtSetArg(args[n], XmNtopAttachment,    XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget,        menuWidget); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment,   XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment,  XmATTACH_FORM); n++;
    XtSetValues(viewer->getWidget(), args, n);
    
    return (viewer);
}



///////////////////////////////////////////////////////////////////
//

static void
countTriangleCB(void *userData, SoCallbackAction *,
     		const SoPrimitiveVertex *, 
		const SoPrimitiveVertex *,
                const SoPrimitiveVertex *)
{
    int32_t *curCount = (int32_t *) userData;
    (*curCount)++;
}


int
countTriangles(SoNode *root)
{
    SoCallbackAction ca;
    int32_t numTris = 0;

    root->ref();

    ca.addTriangleCallback(SoShape::getClassTypeId(), countTriangleCB,
                           (void *) &numTris);
    ca.apply(root);

    root->unrefNoDelete();

    return (numTris);
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


///////////////////////////////////////////////////////////////////
//
int
main(int argc, char **argv)
{
    XtAppContext appContext;

    XtSetLanguageProc( NULL,  _myXtDefaultLanguageProc, NULL );

    shell = XtAppInitialize(
	    &appContext,		// context will be returned
	    "Ivview",			// application class
	    NULL, 0,			// options
	    (int *) &argc, argv, 	// command line args in/out
	    NULL,			// fallback_resources
	    NULL, 0);			// args, num_args

    XtGetSubresources(  shell, &fileData[0], "intl", "Intl",
                        intl_file_resources, intl_num_file_resources,
			NULL, 0 );
    XtGetSubresources(  shell, &editData[0], "intl", "Intl",
                        intl_edit_resources, intl_num_edit_resources,
			NULL, 0 );
    XtGetSubresources(  shell, &optimizeData[0], "intl", "Intl",
                        intl_optimize_resources, intl_num_optimize_resources,
			NULL, 0 );
    XtGetSubresources(  shell, &pulldownData[0], "intl", "Intl",
                        intl_pulldown_resources, intl_num_pulldown_resources,
			NULL, 0 );

    // Parse command line arguments
    parse_args(argc, argv);
    
    // build the main window - this is done FIRST (before initialzing
    // Inventor and reading) to make the application startup feel
    // faster.
    
    Widget form = XtCreateWidget("Form", xmFormWidgetClass, shell, NULL, 0);
    XtVaSetValues(shell, XmNwidth, 520, XmNheight, 510, NULL);
    
    Widget menuWidget = buildAndLayoutMenus(form);  
    XtManageChild(form); 
    XtRealizeWidget(shell);
    XmUpdateDisplay(shell);  // better than XSync() because it redraws too
    setBusyCursor(TRUE);
    
    // Initialize Inventor 
    SoXt::init(shell);

    // Create and lay out the viewer
    viewer = buildAndLayoutViewer(form, menuWidget, useWalkViewer);
    viewer->show();
    XSync(XtDisplay(shell), False);
    
    // Read the input scene from the file(s)
    SbBool readOk = readScene(files);

    // The lower left corner of the window is used to draw
    // the tris/second meter (if enabled)
    if (showPerfMeter) 
	initPerfMeter(viewer);

    setBusyCursor(FALSE);
    
    SoXt::mainLoop();
}
