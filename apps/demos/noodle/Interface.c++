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
/*
 |   Classes:
 |	Interface
 |
 |   Author(s): Paul Isaacs
 |
 */


#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SoDB.h>

#include <Xm/MessageB.h>

#include <Inventor/actions/SoBoxHighlightRenderAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/manips/SoHandleBoxManip.h>
#include <Inventor/manips/SoTrackballManip.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/SoXtClipboard.h>
#include <Inventor/Xt/viewers/SoXtViewer.h>

#include "GeneralizedCylinder.h"
#include "WorldInfo.h"
#include "PullDowns.h"
#include "Interface.h"
#include "NoodleTextureGizmo.h"
#include "NoodleSurfaceGizmo.h"

#define SCREEN(w) XScreenNumberOfScreen(XtScreen(w))

extern SoNode *createProfileGraph( Widget, GeneralizedCylinder *);
extern SoNode *createCrossSectionGraph( Widget, GeneralizedCylinder *);
extern SoNode *createSpineGraph( Widget, GeneralizedCylinder *);
extern SoNode *createTwistGraph( Widget, GeneralizedCylinder *);

Interface::Interface()
{
    // Initialize other variables.
	bgColor.setValue(0,0,0);
	worldInfo = NULL;
	mgrWidget = NULL;
	mainViewer = NULL;
	menuItems = NULL;
	myTextureGizmo = NULL;
	mySurfaceGizmo = NULL;

	profileViewer = NULL;
	sectionViewer = NULL;
	spineViewer = NULL;
	twistViewer = NULL;

	closeProfileButton = NULL;
	closeSectionButton = NULL;
	closeSpineButton = NULL;
	closeTwistButton = NULL;
}

Interface::~Interface()
{
    worldInfo = NULL;
    if (myTextureGizmo)
	delete myTextureGizmo;
    if (mySurfaceGizmo)
	delete mySurfaceGizmo;
}

void       
Interface::setWorldInfo( WorldInfo *newWorldInfo )
{
    if (newWorldInfo == NULL)
	return;

    worldInfo = newWorldInfo;
    SoSelection *s = worldInfo->getSelectorNode();
    s->addSelectionCallback( &Interface::selectionCB, this );
}

/////////////////////////////////////////////////////////////////////
// Reads the scene given by filename and puts the results into worldInfo.
// Returns pointer to top of scene graph.
// If filename is NULL, uses name already found in worldInfo.
// If filename and worldInfo are both NULL, it is an error.
SoSeparator * 
Interface::readScene( char *newFileName, SbBool okIfNoName )
{
    if (newFileName == NULL && 
	(worldInfo == NULL || worldInfo->getFileName() == NULL)) {
	if ( ! okIfNoName ) {
	    fprintf(stderr, "Interface::readScene programming error:"
			    "Can\'t read scene because no filename given and \n"
			    "Interface has worldInfo equal to NULL\n");
	}
	return (NULL);
    }

    SoInput in;
    SoSeparator *root;
    FILE *filePtr = NULL;
    
    if (newFileName != NULL && worldInfo != NULL)
	worldInfo->setFileName( newFileName );

    const char *fName 
	    = (worldInfo == NULL) ? newFileName : worldInfo->getFileName();

    fprintf(stderr, "Reading file %s...\n", fName );

    const char *slashPtr;
    char *searchPath = NULL;

    filePtr = fopen(fName, "r");
    if (filePtr == NULL) {
	fprintf(stderr, "Error opening file %s\n", fName);
	return (NULL);
    }

    //
    // If the filename includes a directory path, add the 
    // directory name to the list of directories where to look
    // for files referenced in the input file (e.g. SoFile nodes)
    //
    if ((slashPtr = strrchr(fName, '/')) != NULL) {
	searchPath = strdup(fName);
	searchPath[slashPtr - fName] = '\0';
	in.addDirectoryFirst(searchPath);
    }

    in.setFilePointer(filePtr);
    root = SoDB::readAll(&in);
    if (root == NULL) 
	fprintf(stderr, "Error reading file %s\n", fName);

    fclose(filePtr);
    if ( worldInfo != NULL)
	worldInfo->setScene( root );
    return root;
}

/////////////////////////////////////////////////////////////////////
// Writes the scene contained in worldInfo.
// If asVanilla==FALSE, uses special GeneralizedCylinder class in the file. 
// If asVanilla==TRUE, uses only standard Inventor nodes.
// If filename is NULL, uses name already found in worldInfo.
// If filename and worldInfo->getFileName() are both NULL, it is an error.
void 
Interface::writeToFile( SbBool asVanilla, char *newFileName )
{
    if ( worldInfo == NULL ||
	(newFileName == NULL && worldInfo->getFileName() == NULL)) {
	fprintf(stderr, "Interface::writeToFile programming error:"
			"Can\'t write scene because no filename given or \n"
			"Interface has worldInfo equal to NULL\n");
	return;
    }

    SoWriteAction   wa;

    if (newFileName != NULL )
	worldInfo->setFileName( newFileName );

    const char *fName = worldInfo->getFileName();

    if (! wa.getOutput()->openFile(fName)) {
	char str[100];
	strcpy(str, "Error creating file: ");
	strcat(str, fName);
	SoXt::createSimpleErrorDialog(mgrWidget, 
				      "File Write Error Dialog", str);
    }
    else {
	// Temporarily remove manips from scene
	SoType savedManipType = worldInfo->getManipType();
	worldInfo->setManipType( SoTransform::getClassTypeId() );

	// Get the scene from worldInfo and write it out.
	SoSeparator *scene;
	if ( asVanilla )
	    scene = worldInfo->getVanillaSceneCopy();
	else
	    scene = worldInfo->getScene();


	scene->ref();
	wa.apply(scene);
	wa.getOutput()->closeFile();
	scene->unref();

	// Restore manips to scene.
	worldInfo->setManipType( savedManipType );
    }
}

///////////////////////////////////////////////////////////////////
//
// If scene isn't empty, check if user wants to save scene first.
// If not, then create an empty scene so user can start over.
//
void 
Interface::fileNewEvent()
{
    if (worldInfo->isSceneEmpty() == FALSE ) {
	createOkayCancelDialog(mgrWidget, 
	    Interface::newSceneCB,
	    "New Scene Error Dialog",
	    "Your current scene is not empty.",
	    "Hit OK to continue, CANCEL if you want to Save first.");

    }
    else
	Interface::newSceneCB(NULL, (XtPointer) this, NULL);
}

///////////////////////////////////////////////////////////////////
//
// If scene not empty, check if user wants to save scene first.
// If not, bring up the file dialog box and read a new scene from file.
//
void 
Interface::fileOpenEvent()
{
    if (worldInfo->isSceneEmpty() == FALSE ) {
	createOkayCancelDialog(mgrWidget, 
	    Interface::openSceneCB,
	    "Open Scene Error Dialog",
	    "Your current scene is not empty.",
	    "Hit OK to continue, CANCEL if you want to Save first.");

    }
    else
	Interface::openSceneCB(NULL, (XtPointer) this, NULL);
}

///////////////////////////////////////////////////////////////////
//
// If scene is empty, show a dialog and return.
// If there is more than one file in the list, direct them to Save As...
// If everything's okay, ask the world for the scene and write it out.
//
void 
Interface::fileSaveEvent()
{
    if (worldInfo->isSceneEmpty() == TRUE ) {
	SoXt::createSimpleErrorDialog(mgrWidget, 
	    "File Save Empty Error Dialog",
	    "The current scene is empty. Create a scene first,",
	    "or fix my code to grey out this menu item.");
    }
    else
	writeToFile( FALSE );
}

///////////////////////////////////////////////////////////////////
//
// If the scene is empty, show a dialog and return.
// If everything's okay, ask the world for the scene and write it out.
//
void 
Interface::fileSaveAsEvent()
{
    if (worldInfo->isSceneEmpty() == TRUE ) {
	SoXt::createSimpleErrorDialog(mgrWidget, 
	    "File Save As Empty Error Dialog",
	    "The current scene is empty. Create a scene first,",
	    "or fix my code to grey out this menu item.");
    }
    else
	showFileSelectionDialog( (XtCallbackProc) Interface::writeToFileCB );
}

///////////////////////////////////////////////////////////////////
//
// If the scene is empty, show a dialog and return.
// If everything's okay, ask the world for the scene and write it out.
//
void 
Interface::fileSaveVanillaEvent()
{
    if (worldInfo->isSceneEmpty() == TRUE ) {
	SoXt::createSimpleErrorDialog(mgrWidget, 
	    "File Save As Empty Error Dialog",
	    "The current scene is empty. Create a scene first,",
	    "or fix my code to grey out this menu item.");
    }
    else
	showFileSelectionDialog( (XtCallbackProc) Interface::writeToVanillaFileCB );
}

///////////////////////////////////////////////////////////////////
//
// If the scene is not empty, see if the user would like to save.
// Otherwise, quit.
//
void 
Interface::fileQuitEvent()
{
    if (worldInfo->isSceneEmpty() == FALSE ) {
	createOkayCancelDialog(mgrWidget, 
	    Interface::quitProgramCB,
	    "Quit Error Dialog",
	    "Your current scene is not empty.",
	    "Hit OK to Quit anyway, CANCEL if you want to Save first.");

    }
    else
	Interface::quitProgramCB(NULL, (XtPointer) this, NULL);
}

////////////////////////////////////////////////////////////////////////
//
// creates a dialog box.
// Prints a message and has two buttons:
// CANCEL and OK.
//
// When calling this, you must pass a callback for when the 
// OK button is pressed.
//
void
Interface::createOkayCancelDialog(Widget widget, XtCallbackProc okCB,
    char *dialogTitle, char *str1, char *str2)
{
    Widget shell = SoXt::getShellWidget(widget);
    if (shell == NULL)
	return;
    
    Arg args[5];
    XmString xmstr = XmStringCreateSimple(str1);
    xmstr = XmStringConcat(xmstr, XmStringSeparatorCreate());
    xmstr = XmStringConcat(xmstr, XmStringCreateSimple(str2));
    
    int n = 0;
    XtSetArg(args[n], XmNautoUnmanage, FALSE); n++;
    XtSetArg(args[n], XtNtitle, dialogTitle); n++;
    XtSetArg(args[n], XmNmessageString, xmstr); n++;
    Widget dialog = XmCreateErrorDialog(shell, "Error Dialog", args, n);
    XmStringFree(xmstr);
    
    XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));
    
    // First, destroy the dialog box.
    // Then, call the passed callback.
    XtAddCallback(dialog, XmNokCallback, 
	(XtCallbackProc) Interface::destroyDialogCB, (XtPointer)this);
    XtAddCallback(dialog, XmNokCallback, 
	(XtCallbackProc) okCB, (XtPointer)this);

    // If the user cancels, we'll always want to destroy the dialog.
    // register callback to destroy (and not just unmap) the dialog
    XtAddCallback(dialog, XmNcancelCallback, 
	(XtCallbackProc) Interface::destroyDialogCB, (XtPointer)this);
    
    XtManageChild(dialog);
}

///////////////////////////////////////////////////////////////////
//
// Brings up the "About..." dialog
//
void
Interface::showAboutDialog()
{
   if (access(IVPREFIX "/demos/Inventor/noodle.about", R_OK) != 0) {
        system("xmessage 'Sorry, could not find "
               IVPREFIX "/demos/Inventor/noodle.about' > /dev/null");
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

    sprintf(command, "acroread " IVPREFIX "/demos/Inventor/noodle.about &");
    system(command);
}

///////////////////////////////////////////////////////////////////
//
// Use a motif file selection dialog to get the new filename.  
// When calling this, you must pass a callback for when the 
// OK button is pressed.
//
void
Interface::showFileSelectionDialog( XtCallbackProc okCB )
{
    static Widget fileDialog = NULL;

    if (fileDialog == NULL) {
        Arg args[5];
        int n = 0;

        // Unmanage when ok/cancel are pressed
        XtSetArg(args[n], XmNautoUnmanage, TRUE); n++;
        fileDialog = XmCreateFileSelectionDialog(
            XtParent(mgrWidget), "File Dialog", args, n);

        XtAddCallback(fileDialog, XmNokCallback,
                      (XtCallbackProc)okCB, (XtPointer) this);
    }
    else {
	// We need to remove the old callback, 
	// then install the one we were passed, since this file selector
	// is used for both reading and writing files.
        XtRemoveAllCallbacks(fileDialog, XmNokCallback );
        XtAddCallback(fileDialog, XmNokCallback,
                      (XtCallbackProc)okCB, this);
    }

    // Manage the dialog
    XtManageChild(fileDialog);
}


//////////////////////////////////////////////////////////////
// Gets rid of dialog box.
void 
Interface::destroyDialogCB(Widget dialog, void *, void *)
{ 
    XtDestroyWidget(dialog); 
}

//////////////////////////////////////////////////////////////
// Makes a brand new scene.
//
void 
Interface::newSceneCB(Widget, void *userData, void *)
{
    Interface *myself = (Interface *) userData;
    // Create a new scene with one Noodle under a separator.
    SoSeparator *newRoot = new SoSeparator;
    newRoot->ref();
    newRoot->addChild( new GeneralizedCylinder );

    myself->getWorldInfo()->setScene( newRoot );
    newRoot->unref();
}

//////////////////////////////////////////////////////////////
// Brings up file selection widget and reads scene from selected file.
//
void 
Interface::openSceneCB(Widget, void *userData, void *)
{
    ((Interface *)userData)->showFileSelectionDialog( 
				(XtCallbackProc) Interface::readFromFileCB );
}

//////////////////////////////////////////////////////////////
// Brings up file selection widget and reads scene from selected file.
// Callback routine that gets called when the new filename
// has been entered. 
//
// Reads the scene and gives it to 'worldInfo'
//
void
Interface::readFromFileCB(Widget, void *userData, 
			  XmFileSelectionBoxCallbackStruct *data)
{
    // Get the file name
    char *filename;
    XmStringGetLtoR(data->value,
        (XmStringCharSet) XmSTRING_DEFAULT_CHARSET, &filename);
   
    ((Interface *)userData)->readScene( filename );

    XtFree(filename);
}

//////////////////////////////////////////////////////////////
// Callback routine that gets called when the new filename
// has been entered. 
//
// Writes the scene held by 'worldInfo'
//
void
Interface::writeToFileCB(Widget, void *userData, 
		    XmFileSelectionBoxCallbackStruct *data)
{
    // Get the file name
    char *filename;
    XmStringGetLtoR(data->value,
        (XmStringCharSet) XmSTRING_DEFAULT_CHARSET, &filename);

    ((Interface *)userData)->writeToFile( FALSE, filename );

    XtFree(filename);
}

//////////////////////////////////////////////////////////////
// Callback routine that gets called when the new filename
// has been entered. 
//
// Writes the scene AS A STANDARD INVENTOR FILE WITHOUT GENERALIZED CYLINDERS
// to file held by 'worldInfo'
//
void
Interface::writeToVanillaFileCB(Widget, void *userData, 
				XmFileSelectionBoxCallbackStruct *data)
{
    // Get the file name
    char *filename;
    XmStringGetLtoR(data->value,
        (XmStringCharSet) XmSTRING_DEFAULT_CHARSET, &filename);

    // TRUE means to write it 'vanilla style'
    ((Interface *) userData)->writeToFile( TRUE, filename );
   
    XtFree(filename);
}

void 
Interface::quitProgramCB(Widget, void *, void *)
{
    exit(0);
}

///////////////////////////////////////////////////////////////////
//
// Process the topbar menu events.
void
Interface::processTopbarEvent(Widget, 
		      NoodleMenuItem *data, XmAnyCallbackStruct *cbstruct )
{
    Interface *myself = data->ownerInterface;
    WorldInfo *myWorld = myself->worldInfo;
    GeneralizedCylinder *myNoodle = myWorld->getCurrentNoodle();

    static SoXtClipboard *theClipboard = NULL;

    switch (data->id) {

    //
    // File
    //
 
    case MM_FILE_NEW:
	myself->fileNewEvent();
	break;

    case MM_FILE_OPEN:
	myself->fileOpenEvent();
	break;

    case MM_FILE_SAVE:
	myself->fileSaveEvent();
	break;

    case MM_FILE_SAVE_AS:
	myself->fileSaveAsEvent();
	break;

    case MM_FILE_SAVE_VANILLA:
	myself->fileSaveVanillaEvent();
	break;

    case MM_FILE_QUIT:
	myself->fileQuitEvent();
	break;	

    //
    // Edit
    //
  
    case MM_EDIT_NEW:
	// Add the current selection to the list of deleted objects.
	myWorld->addNewNoodle(); 
	break;

    case MM_EDIT_DELETE:
	// Add the current selection to the list of deleted objects.
	myWorld->deleteCurrentNoodle();
        // Set the primary selection to NULL...
	myself->setPrimarySelection(NULL);
	break;

    case MM_EDIT_UNDELETE:
	// Undelete the most recently deleted object.
	myWorld->undeleteNoodle();
	break;

    case MM_EDIT_COPY:
    case MM_EDIT_COPY_ALL:
	// Establish what we are copying
	    SoNode *copyStuff;
	    if (data->id == MM_EDIT_COPY)
		copyStuff = myNoodle;
	    else
		copyStuff = myWorld->getScene();

	// Make a clipboard
	    if (theClipboard == NULL)
		theClipboard = new SoXtClipboard(myself->mainViewer->getWidget());

	// Copy into clipboard
	    if ( copyStuff != NULL )
		theClipboard->copy( copyStuff, cbstruct->event->xbutton.time);
	break;

    //
    // Options
    //
  
    case MM_SHAPE_FACE_SET:
	if ( myNoodle && XmToggleButtonGetState( data->widget ) ) {
	    myNoodle->renderShapeType = GeneralizedCylinder::FACE_SET;
	    myself->setRenderStyleRadioButtons( GeneralizedCylinder::FACE_SET );
	}
	break;
    case MM_SHAPE_TRI_STRIP:
	if ( myNoodle && XmToggleButtonGetState( data->widget ) ) {
	    myNoodle->renderShapeType = GeneralizedCylinder::TRIANGLE_STRIP_SET;
	    myself->setRenderStyleRadioButtons(GeneralizedCylinder::TRIANGLE_STRIP_SET);
	}
	break;
    case MM_SHAPE_QUAD_MESH:
	if ( myNoodle && XmToggleButtonGetState( data->widget ) ) {
	    myNoodle->renderShapeType = GeneralizedCylinder::QUAD_MESH;
	    myself->setRenderStyleRadioButtons(GeneralizedCylinder::QUAD_MESH);
	}
	break;
    case MM_SHAPE_CUBIC_SPLINE:
	if ( myNoodle && XmToggleButtonGetState( data->widget ) ) {
	    myNoodle->renderShapeType = GeneralizedCylinder::CUBIC_SPLINE_SURFACE;
	    myself->setRenderStyleRadioButtons(GeneralizedCylinder::CUBIC_SPLINE_SURFACE);
	}
	break;
    case MM_SHAPE_CUBIC_TO_EDGE:
	if ( myNoodle && XmToggleButtonGetState( data->widget ) ) {
	    myNoodle->renderShapeType = GeneralizedCylinder::CUBIC_TO_EDGE_SURFACE;
	    myself->setRenderStyleRadioButtons(GeneralizedCylinder::CUBIC_TO_EDGE_SURFACE);
	}
	break;
    case MM_SHAPE_BEZIER:
	if ( myNoodle && XmToggleButtonGetState( data->widget ) ) {
	    myNoodle->renderShapeType = GeneralizedCylinder::BEZIER_SURFACE;
	    myself->setRenderStyleRadioButtons(GeneralizedCylinder::BEZIER_SURFACE);
	}
	break;

    case MM_PARTS_SIDES:
	if (myNoodle  != NULL ) {
	    GeneralizedCylinder *c = myNoodle;
	    if (c->withSides.getValue())
		c->withSides = FALSE;
	    else
		c->withSides = TRUE;
	}
	break;
    case MM_PARTS_TOP_CAP:
	if (myNoodle  != NULL ) {
	    GeneralizedCylinder *c = myNoodle;
	    if (c->withTopCap.getValue())
		c->withTopCap = FALSE;
	    else
		c->withTopCap = TRUE;
	}
	break;
    case MM_PARTS_BOT_CAP:
	if (myNoodle  != NULL ) {
	    GeneralizedCylinder *c = myNoodle;
	    if (c->withBottomCap.getValue())
		c->withBottomCap = FALSE;
	    else
		c->withBottomCap = TRUE;
	}
	break;

    //
    // Manips
    //
    case MM_MANIPS_HBOX:
	if ( XmToggleButtonGetState( data->widget ) ) {
	    if (myWorld->getManipType() != SoHandleBoxManip::getClassTypeId()) {

		// Don't show bounding box (check action type to be safe...)
		SoGLRenderAction *ra = myself->mainViewer->getGLRenderAction();
		if (ra->isOfType( SoBoxHighlightRenderAction::getClassTypeId()))
			((SoBoxHighlightRenderAction *) ra)->setVisible(FALSE);

		myWorld->setManipType( SoHandleBoxManip::getClassTypeId() );
	        myself->setManipTypeRadioButtons( SoHandleBoxManip::getClassTypeId() );
	    }
	}
	break;
    case MM_MANIPS_TRACKBALL:
	if ( XmToggleButtonGetState( data->widget ) ) {
	    if (myWorld->getManipType() != SoTrackballManip::getClassTypeId()) {

		// Don't show bounding box (check action type to be safe...)
		SoGLRenderAction *ra = myself->mainViewer->getGLRenderAction();
		if (ra->isOfType( SoBoxHighlightRenderAction::getClassTypeId()))
			((SoBoxHighlightRenderAction *) ra)->setVisible(FALSE);

		myWorld->setManipType( SoTrackballManip::getClassTypeId() );
	        myself->setManipTypeRadioButtons( SoTrackballManip::getClassTypeId() );
	    }
	}
	break;
    case MM_MANIPS_NONE:
	if ( XmToggleButtonGetState( data->widget ) ) {
	    if (myWorld->getManipType() != SoTransform::getClassTypeId() ) {

		// Do show bounding box (check renderaction type to be safe...)
		SoGLRenderAction *ra = myself->mainViewer->getGLRenderAction();
		if (ra->isOfType( SoBoxHighlightRenderAction::getClassTypeId()))
			((SoBoxHighlightRenderAction *) ra)->setVisible(TRUE);

		myWorld->setManipType( SoTransform::getClassTypeId() );
	        myself->setManipTypeRadioButtons( SoTransform::getClassTypeId() );
	    }
	}
	break;

    case MM_GIZMOS_TEXTURE:
	{
	    // Create a texture gizmo, if it does not exist.
	    if (myself->myTextureGizmo == NULL ) {
	        myself->myTextureGizmo = new NoodleTextureGizmo();
		myself->myTextureGizmo->setTitle( "Texture Gizmo" );
	        // Set the noodle for the gizmo.
		myself->myTextureGizmo->setNoodle( myNoodle ); 
	    }
	    // Show the texture gizmo. 
	    myself->myTextureGizmo->show();
	}
	break;

    case MM_GIZMOS_SURFACE:
	{
	    if (myself->mySurfaceGizmo == NULL ) {
	        // Create a surface gizmo, if it does not exist.
	        myself->mySurfaceGizmo = new NoodleSurfaceGizmo();
		myself->mySurfaceGizmo->setTitle( "Surface Gizmo" );
	        // Set the noodle for the gizmo.
		myself->mySurfaceGizmo->setNoodle( myNoodle ); 
	    }
	    // Show the surface gizmo. 
	    myself->mySurfaceGizmo->show();
	}
	break;

    // 
    // About
    // 
    case MM_ABOUT_ABOUT:
 	myself->showAboutDialog();
	break;

    } 
}

///////////////////////////////////////////////////////////////////
//
// Makes all settings of the interface reflect the given object
void
Interface::setPrimarySelection( GeneralizedCylinder *g )
{
    if (menuItems == NULL)
	return;

    SbBool bv;

    // Render Style:
	if (g == NULL)
	    setRenderStyleRadioButtons( GeneralizedCylinder::FACE_SET );
	else
	    setRenderStyleRadioButtons( (GeneralizedCylinder::RenderShapeType) 
					 g->renderShapeType.getValue());

    // Parts on or off:
	bv = (g) ? g->withSides.getValue() : FALSE; 
	XmToggleButtonSetState( menuItems[MM_PARTS_SIDES].widget,bv,FALSE);
	bv = (g) ? g->withTopCap.getValue() : FALSE; 
	XmToggleButtonSetState( menuItems[MM_PARTS_TOP_CAP].widget,bv,FALSE);
	bv = (g) ? g->withBottomCap.getValue() : FALSE; 
	XmToggleButtonSetState( menuItems[MM_PARTS_BOT_CAP].widget,bv,FALSE);

    // Manip Type:
	SoNode *xf = NULL;
	if ( g )
	    xf = g->getPart( "transform", FALSE );
	if ( ! xf )
	    setManipTypeRadioButtons( SoTransform::getClassTypeId() );
	else
	    setManipTypeRadioButtons( xf->getTypeId() );

    // Texture
	if (myTextureGizmo)
	    myTextureGizmo->setNoodle( g );

    // Surface
	if (mySurfaceGizmo)
	    mySurfaceGizmo->setNoodle( g );

    // Tell the curve viewers to build new scenes...
	if (profileViewer)
	    profileViewer->setSceneGraph(
		createProfileGraph(profileViewer->getWidget(), g));
	if (sectionViewer)
	    sectionViewer->setSceneGraph(
		createCrossSectionGraph(sectionViewer->getWidget(),g));
	if (spineViewer)
	    spineViewer->setSceneGraph(
		createSpineGraph(spineViewer->getWidget(), g));
	if (twistViewer)
	    twistViewer->setSceneGraph(
		createTwistGraph(twistViewer->getWidget(), g));

    // Curve information for each of the four editors...
	bv = (g) ? g->profileClosed.getValue() : FALSE;
	XmToggleButtonSetState(closeProfileButton, bv, FALSE );
	bv = (g) ? g->crossSectionClosed.getValue() : FALSE;
	XmToggleButtonSetState(closeSectionButton, bv, FALSE );
	bv = (g) ? g->spineClosed.getValue() : FALSE;
	XmToggleButtonSetState(closeSpineButton,  bv, FALSE );
	bv = (g) ? g->twistClosed.getValue() : FALSE;
	XmToggleButtonSetState(closeTwistButton, bv, FALSE );

}

void
Interface::setRenderStyleRadioButtons( GeneralizedCylinder::RenderShapeType shapeType )
{
    // First turn all radio buttons to FALSE...
    XmToggleButtonSetState(menuItems[MM_SHAPE_FACE_SET].widget,FALSE,FALSE);
    XmToggleButtonSetState(menuItems[MM_SHAPE_TRI_STRIP].widget,FALSE,FALSE);
    XmToggleButtonSetState(menuItems[MM_SHAPE_QUAD_MESH].widget,FALSE,FALSE);
    XmToggleButtonSetState(menuItems[MM_SHAPE_CUBIC_SPLINE].widget,FALSE,FALSE);
    XmToggleButtonSetState(menuItems[MM_SHAPE_CUBIC_TO_EDGE].widget,FALSE,FALSE);
    XmToggleButtonSetState(menuItems[MM_SHAPE_BEZIER].widget,FALSE,FALSE);

    // Set the appropriate radio button for shape rendering type.
    Widget onWidget = NULL;
    switch(shapeType) {
	case GeneralizedCylinder::FACE_SET:
	    onWidget = menuItems[MM_SHAPE_FACE_SET].widget;
	    break;
	case GeneralizedCylinder::TRIANGLE_STRIP_SET:
	    onWidget = menuItems[MM_SHAPE_TRI_STRIP].widget;
	    break;
	case GeneralizedCylinder::QUAD_MESH:
	    onWidget = menuItems[MM_SHAPE_QUAD_MESH].widget;
	    break;
	case GeneralizedCylinder::CUBIC_SPLINE_SURFACE:
	    onWidget = menuItems[MM_SHAPE_CUBIC_SPLINE].widget;
	    break;
	case GeneralizedCylinder::CUBIC_TO_EDGE_SURFACE:
	    onWidget = menuItems[MM_SHAPE_CUBIC_TO_EDGE].widget;
	    break;
	case GeneralizedCylinder::BEZIER_SURFACE:
	    onWidget = menuItems[MM_SHAPE_BEZIER].widget;
	    break;
    }
    if (onWidget)
	XmToggleButtonSetState( onWidget, TRUE, FALSE );
}

void
Interface::setManipTypeRadioButtons( const SoType &manipType )
{
    // First turn all radio buttons to FALSE...
    XmToggleButtonSetState(menuItems[MM_MANIPS_HBOX].widget,FALSE,FALSE);
    XmToggleButtonSetState(menuItems[MM_MANIPS_TRACKBALL].widget,FALSE,FALSE);
    XmToggleButtonSetState(menuItems[MM_MANIPS_NONE].widget,FALSE,FALSE);

    // Set the appropriate radio button for manip type.
    Widget onWidget = NULL;
    if ( manipType.isDerivedFrom( SoHandleBoxManip::getClassTypeId() ) )
	onWidget = menuItems[MM_MANIPS_HBOX].widget;
    else if ( manipType.isDerivedFrom( SoTrackballManip::getClassTypeId()) )
	onWidget = menuItems[MM_MANIPS_TRACKBALL].widget;
    else
	onWidget = menuItems[MM_MANIPS_NONE].widget;

    if (onWidget)
	XmToggleButtonSetState( onWidget, TRUE, FALSE );
}

///////////////////////////////////////////////////////////////////////
//
// Callback registered on WorldInfo's selection node to invoke
// setPrimarySelection() on the new selection.
//
void
Interface::selectionCB( void *userData, SoPath *selectPath )
{
    // Return if pickFilter truncated path down to nothing.
    if (selectPath->getLength() == 0)
	return;

    Interface *myself = (Interface *) userData;

    SoNodeKitPath *nkPath = (SoNodeKitPath *) selectPath;
    GeneralizedCylinder *g = (GeneralizedCylinder *) nkPath->getTail();

    myself->setPrimarySelection( g );
}

///////////////////////////////////////////////////////////////////////
//
// Create the top menu bar and the associated menus
//
Widget
Interface::build( Widget parentWidget )
{
    mgrWidget = parentWidget;
    if (mgrWidget == NULL)
	fprintf(stderr, "Error. NULL passed to Interface::build\n");

    menuItems = new NoodleMenuItem[MM_MENU_NUM];
    int i;
    for (i=0; i<MM_MENU_NUM; i++) {
	menuItems[i].id = i;
	menuItems[i].widget = NULL;
	menuItems[i].ownerInterface = this;
    }

    // 
    // Create the topbar menu
    //
    Widget menuWidget = XmCreateMenuBar(mgrWidget, "menuBar", NULL, 0);

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
        XtSetArg(args[0], XmNtearOffModel, XmTEAR_OFF_ENABLED);
        XtSetValues(subMenu, args, 1);

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
            if (pulldownData[i].subMenu[j].buttonType == MM_SEPARATOR) {
                subButtons[j] = XtCreateWidget(
			NULL, xmSeparatorGadgetClass, subMenu, NULL, 0);
  	    }
            else {
		String callbackReason;

                switch (pulldownData[i].subMenu[j].buttonType) {
                    case MM_PUSH_BUTTON:
                        widgetClass = xmPushButtonGadgetClass;
                        callbackReason = XmNactivateCallback;
                        n = 0;
                        break;
                    case MM_TOGGLE_BUTTON:
                        widgetClass = xmToggleButtonGadgetClass;
                        callbackReason = XmNvalueChangedCallback;
                        n = 0;
                        break;
                    case MM_RADIO_BUTTON:
                        widgetClass = xmToggleButtonGadgetClass;
                        callbackReason = XmNvalueChangedCallback;
                        XtSetArg(args[0], XmNindicatorType, XmONE_OF_MANY);
                        n = 1;
                        break;
                    case MM_SEPARATOR:
			n = 0;
			fprintf(stderr, 
				"Programming Error in Interface::build\n");
			break;
                    default:
                        fprintf(stderr, 
				"noodle INTERNAL ERROR: bad buttonType\n");
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
                    (XtCallbackProc)Interface::processTopbarEvent,
                    (XtPointer) &menuItems[id]);

		// If a toggle, set the starting state:
                if (pulldownData[i].subMenu[j].buttonType == MM_TOGGLE_BUTTON ||
                    pulldownData[i].subMenu[j].buttonType == MM_RADIO_BUTTON){
		    SbBool isOn;
		    isOn = pulldownData[i].subMenu[j].isOn;

		    XmToggleButtonGadgetSetState(subButtons[j], isOn, FALSE);
		}
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
