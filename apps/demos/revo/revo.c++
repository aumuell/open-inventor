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
// Surface of revolution program
//

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>

#include <Inventor/SoDB.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/SoXtRenderArea.h>
#include <Inventor/Xt/SoXtClipboard.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoCallback.h>
#include <Inventor/nodes/SoSeparator.h>

#include <Xm/Xm.h>
#include <Xm/Text.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/ScrollBar.h>
#include <Xm/PushBG.h>
#include <Xm/LabelG.h>
#include <Xm/ToggleBG.h>
#include <Xm/RowColumn.h>

#include "../../samples/common/InventorLogo.h"
#include "LineManip.h"
#include "RevClass.h"

//
// These are defined in profile.c++
//
extern SoNode *createProfileGraph(Widget, RevolutionSurface *);
extern void clearPoints();

//
// Callback for 'About...' button
//
void
showAboutDialog(Widget, XtPointer, XtPointer)
{
    if (access(IVPREFIX "/demos/Inventor/revo.about", R_OK) != 0)
    {
	system("xmessage 'Sorry, could not find "
	       IVPREFIX "/demos/Inventor/revo.about' > /dev/null");
	return;
    }

    char command[100];
    sprintf(command, "which acroread >& /dev/null");

    int err = system(command);
    if (err)
    {
	system("xmessage 'You must install acroread"
	       " for this function to work' > /dev/null");
	return;
    }

    sprintf(command, "acroread " IVPREFIX "/demos/Inventor/revo.about &");
    system(command);
}	

//
// Called by the quit button
//
static void
quitCallback(Widget, XtPointer, XtPointer)
{
    exit(0);
}

//
// Callback for the text widget (called when the user hits return).
//
void
changeNumSides(Widget textWidget, XtPointer data, XtPointer)
{
    static char t[4];
    char *str = XmTextGetString((Widget)textWidget);
    int NumSides = atoi(str) + 1;
    if (NumSides < 4) NumSides = 4;

    sprintf(t, "%d", (NumSides-1) % 1000);
    XmTextSetString((Widget)textWidget, t);

    XtFree(str);

    RevolutionSurface *s = (RevolutionSurface *)data;
    s->changeNumSides(NumSides);
}

//
// Callback for the copy button.  Copies the surface of revolution to
// the clipboard.
//
static void
copyCallback(Widget, XtPointer vwr, XtPointer cbstruct)
{
    static SoXtClipboard *theClipboard = NULL;

    SoXtExaminerViewer *v = (SoXtExaminerViewer *)vwr;

    if (theClipboard == NULL)
    {
	theClipboard = new SoXtClipboard(v->getWidget());
    }
    SoNode *root = v->getSceneGraph();
    theClipboard->copy(root,
	((XmAnyCallbackStruct *)cbstruct)->event->xbutton.time);
}

static void
clearCallback(Widget, XtPointer, XtPointer)
{
    clearPoints();
}
////////////////////////////////////////////////////////////////////////
//
//  Draw the Inventor logo in the overlays.
//
////////////////////////////////////////////////////////////////////////

static void
logoCB(void *, SoAction *action)
{
    if (action->isOfType(SoGLRenderAction::getClassTypeId()))
        glViewport(0, 0, 80, 80);  // See Dave Mott for details!
}
static void
setOverlayLogo(SoXtRenderArea *ra)
{
    static SoSeparator *logo = NULL;
    if (logo == NULL) {
        SoInput in;
        in.setBuffer((void *)ivLogo, ivLogoSize);    // common directory
        logo = SoDB::readAll(&in);
        logo->ref();

        SoCallback *cb = new SoCallback; // set the GL viewport
        cb->setCallback(logoCB);
        logo->insertChild(cb, 0);
    }
    SbColor col(1, 1, 1);
    ra->setOverlayColorMap(1, 1, &col);
    ra->setOverlaySceneGraph(logo);
}


////////////////////////////////////////////////////////////////////////
//
//  main
//
////////////////////////////////////////////////////////////////////////

int
main(int, char **argv)
{
    Widget w = SoXt::init(argv[0]);
    if (w == NULL) exit(1);
    
    LineManip2::initClass();
    
    RevolutionSurface *revSurf = new RevolutionSurface();

    Arg resources[20];	
    int n = 0;
    XtSetArg(resources[n], "width", 800); n++;
    XtSetArg(resources[n], "height", 400); n++;
    Widget form = XmCreateForm(w, "form", resources, n); n = 0;
    
    //
    // This RowColumn widget holds all the stuff at the bottom
    // of the screen
    //
    XtSetArg(resources[n], XmNorientation, XmHORIZONTAL); ++n;
    XtSetArg(resources[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNadjustLast, FALSE); ++n;
    XtSetArg(resources[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    Widget rc = XmCreateRowColumn(form, "bottomStuff", resources, n); 
    n = 0;
    
    //
    // Add the viewer to view the shaded geometry
    // 
    XtSetArg(resources[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(resources[n], XmNleftPosition, 50); n++;
    XtSetArg(resources[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(resources[n], XmNbottomWidget, rc); n++;
    Widget frame2 = XmCreateFrame(form, "renderAreaFrame", resources, n); 
    n = 0;

    SoXtExaminerViewer *examViewer = new SoXtExaminerViewer(frame2);
    examViewer->setSceneGraph(revSurf->getSceneGraph());
    examViewer->setBorder(FALSE);
    examViewer->setDecoration(FALSE);
    setOverlayLogo( examViewer );    // Add Inventor logo to overlays
    examViewer->show();
    XtManageChild(frame2);
    
    //
    // Add viewer to edit the revolution profile
    //
    XtSetArg(resources[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(resources[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(resources[n], XmNrightPosition, 50); n++;
    XtSetArg(resources[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(resources[n], XmNbottomWidget, rc); n++;
    Widget frame1 = XmCreateFrame(form, "renderAreaFrame", resources, n); 
    n = 0;

    SoXtRenderArea *ra = new SoXtRenderArea(frame1);
    ra->setSceneGraph(createProfileGraph(ra->getWidget(), revSurf));
    ra->setBorder(FALSE);
    ra->show();
    XtManageChild(frame1);

#define STRING(a) XmStringCreate(a,XmSTRING_DEFAULT_CHARSET)
    
    XtSetArg(resources[n], XmNlabelString, STRING("# sides:")); ++n;
    Widget nsidesLabel = XmCreateLabelGadget(
		rc, "nsidesLabel", resources, n); 
    n = 0;
    XtManageChild(nsidesLabel);
    
    XtSetArg(resources[n], XmNvalue, "20"); ++n;
    XtSetArg(resources[n], XmNcolumns, 3); ++n;
    XtSetArg(resources[n], XmNeditMode, XmSINGLE_LINE_EDIT); ++n;
    Widget nsidesEdit = XmCreateText(
		rc, "nsidesEdit", resources, n); 
    n = 0;
    XtAddCallback(nsidesEdit, XmNactivateCallback, 
		changeNumSides, (XtPointer)revSurf);
    XtManageChild(nsidesEdit);
    
    XtSetArg(resources[n], XmNlabelString, STRING("Copy")); ++n;
    Widget copyButton = XmCreatePushButtonGadget(
		rc, "copy", resources, n); 
    n = 0;
    XtAddCallback(copyButton, XmNactivateCallback,
		  copyCallback, (XtPointer)examViewer);
    XtManageChild(copyButton);
    
    XtSetArg(resources[n], XmNlabelString, STRING("Clear")); ++n;
    Widget clearButton = XmCreatePushButtonGadget(
		rc, "clear", resources, n); 
    n = 0;
    XtAddCallback(clearButton, XmNactivateCallback,
		  clearCallback, (XtPointer)examViewer);
    XtManageChild(clearButton);

    XtSetArg(resources[n], XmNlabelString, STRING("About...")); ++n;
    Widget aboutButton = XmCreatePushButtonGadget(
		rc, "about", resources, n); 
    n = 0;
    XtAddCallback(aboutButton, XmNactivateCallback,
		  showAboutDialog, NULL);
    XtManageChild(aboutButton);

    XtSetArg(resources[n], XmNlabelString, STRING("Quit")); ++n;
    Widget quitButton = XmCreatePushButtonGadget(
		rc, "quit", resources, n); 
    n = 0;
    XtAddCallback(quitButton, XmNactivateCallback,
		  quitCallback, NULL);
    XtManageChild(quitButton);
    
    XtManageChild(rc);
    XtManageChild(form);

    examViewer->viewAll();
    SoXt::show(w);
    SoXt::mainLoop();

    return 0;  // Keep C++ from complaining...
}
