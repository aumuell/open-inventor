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
 |   Description:
 |	This file defines the Interface class. It creates and manages
 |      the pulldown menus for noodle.
 |
 |   Author(s): Paul Isaacs
 |
 */

#ifndef  _INTERFACE_
#define  _INTERFACE_

#include <Xm/Xm.h>

#include <Inventor/SbLinear.h>
#include <Inventor/SoLists.h>
#include <Inventor/nodes/SoSelection.h>

class GeneralizedCylinder;
class WorldInfo;
class SoXtViewer;
class NoodleTextureGizmo;
class NoodleSurfaceGizmo;

class Interface;

struct NoodleButtonInfo {
    char *name;
    int   id;
    int   buttonType;  // PUSH, TOGGLE, RADIO
    SbBool isOn;       // relevant for toggles.
    char *accelerator; // e.g. "Alt <Key> p"
    char *accelText;   // text that appears in the menu item
};

struct NoodleMenuItem {
    int    id;
    Widget widget;
    Interface *ownerInterface;
};

struct NoodleMenu {
    char  *name;
    int    id;
    struct NoodleButtonInfo *subMenu;
    int    subItemCount;
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: Interface
//
//////////////////////////////////////////////////////////////////////////////

class Interface {
    
  public:

    Interface();
    ~Interface();

    // Builds widgets for the top bar menus
    Widget build( Widget parentWidget );

    void       setWorldInfo( WorldInfo *newWorldInfo );
    WorldInfo *getWorldInfo() { return worldInfo; }

    void       setMainViewer( SoXtViewer *newMainViewer ) 
			      { mainViewer = newMainViewer; }
    SoXtViewer *getMainViewer() { return mainViewer; }

    void setProfileViewer( SoXtViewer *newV ) { profileViewer = newV; }
    void setSectionViewer( SoXtViewer *newV ) { sectionViewer = newV; }
    void setSpineViewer(   SoXtViewer *newV ) { spineViewer = newV; }
    void setTwistViewer(   SoXtViewer *newV ) { twistViewer = newV; }

    void setProfileCloseButton( Widget newW ) { closeProfileButton = newW; }
    void setSectionCloseButton( Widget newW ) { closeSectionButton = newW; }
    void setSpineCloseButton(   Widget newW ) { closeSpineButton = newW; }
    void setTwistCloseButton(   Widget newW ) { closeTwistButton = newW; }

    SbColor getBackgroundColor() { return bgColor; }
    void    setBackgroundColor( SbColor newColor ) { bgColor = newColor; }

    // Reads the scene given by filename and puts the results into worldInfo.
    // Returns pointer to top of scene graph.
    // If filename is NULL, uses name already found in worldInfo.
    // If filename and worldInfo are both NULL, it is an error.
    SoSeparator *readScene( char *newFileName = NULL, SbBool okIfNoName =FALSE);

    // Writes the scene contained in worldInfo.
    // If asVanilla==FALSE, uses special GeneralizedCylinder class in the file. 
    // If asVanilla==TRUE, uses only standard Inventor nodes.
    // If filename is NULL, uses name already found in worldInfo.
    // If filename and worldInfo are both NULL, it is an error.
    void writeToFile( SbBool asVanilla, char *newFileName = NULL );

    // Sets info in the interface to reflect contents.
    void setPrimarySelection( GeneralizedCylinder *g );

    // This is public, since the main window calls this 
    // function when the user selects quit from the upper left button menu.
    void fileQuitEvent();

  protected:

    // Callback added to WorldInfo's selection node to invoke
    // setPrimarySelection() on the new selection.
    static void selectionCB( void *, SoPath *);

    WorldInfo  *worldInfo;

    // Processes events for the top bar menus
    static void processTopbarEvent(Widget, NoodleMenuItem *, 
				   XmAnyCallbackStruct *);

    // These get done in response to events in the pull down menu.
    void fileNewEvent();
    void fileOpenEvent();
    void fileSaveEvent();
    void fileSaveAsEvent();
    void fileSaveVanillaEvent();

    // These build dialogs and/or call up showcase to give info.
    void createOkayCancelDialog(Widget, XtCallbackProc,char *,char *,char *);
    void showAboutDialog();
    void showFileSelectionDialog( XtCallbackProc okCB );

    // These callbacks from the dialog boxes cause other actions to be taken.
    static void destroyDialogCB(Widget dialog, void *, void *);

    // Callback functions relating to the FILE menu.
    static void newSceneCB(Widget, void *, void *);
    static void openSceneCB(Widget, void *, void *);
    static void readFromFileCB(Widget, void *,
			       XmFileSelectionBoxCallbackStruct *);
    static void writeToFileCB(Widget, void *,
			       XmFileSelectionBoxCallbackStruct *);
    static void writeToVanillaFileCB(Widget, void *,
			       XmFileSelectionBoxCallbackStruct *);
    static void quitProgramCB(Widget, void *, void *);

  private:

    void setRenderStyleRadioButtons( 
			    GeneralizedCylinder::RenderShapeType shapeType );
    void setManipTypeRadioButtons( const SoType &manipType );

    SbColor            bgColor;
    SoXtViewer    *mainViewer; // The viewer used to display the scene.

    Widget mgrWidget;
    NoodleMenuItem *menuItems;

    NoodleTextureGizmo *myTextureGizmo;
    NoodleSurfaceGizmo *mySurfaceGizmo;

    SoXtViewer *profileViewer;
    SoXtViewer *sectionViewer;
    SoXtViewer *spineViewer;
    SoXtViewer *twistViewer;

    Widget closeProfileButton;
    Widget closeSectionButton;
    Widget closeSpineButton;
    Widget closeTwistButton;
};

#endif /* _INTERFACE_ */
