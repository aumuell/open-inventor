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
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Description:
 |	Displays 4 views of a scene simultaneously, using 4 viewers. 3
 |	of the viewers are plane viewers, showing top, front, and
 |	right side orthographic views, and the other is an examiner
 |	viewer showing an arbitrary perspective view. The window
 |	configuration is like this:
 |
 |
 | 			Top	Right
 |
 |
 |			Front	Perspective
 |
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Xm/Form.h>
#include <Inventor/SoDB.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/Xt/viewers/SoXtPlaneViewer.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoSeparator.h>

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Prints usage message.
//

static void
printUsage(const char *progName)
//
////////////////////////////////////////////////////////////////////////
{
    fprintf(stderr, "Usage: %s datafile\n", progName);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Creates and returns a plane viewer with the given name that
//    views along the given direction. The viewer will have an
//    orthographic camera and no decoration.
//

static SoXtPlaneViewer *
createPlaneViewer(Widget parent, const char *name, SoNode *sceneRoot,
		  const SbVec3f &viewDirection)
//
////////////////////////////////////////////////////////////////////////
{
    SoXtPlaneViewer	*viewer;

    viewer = new SoXtPlaneViewer(parent, name);

    viewer->setCameraType(SoOrthographicCamera::getClassTypeId());
    viewer->setDecoration(FALSE);
    viewer->setSceneGraph(sceneRoot);

    // Specify the camera's orientation to change the view from
    // looking along the negative z axis to look along the given
    // direction
    viewer->getCamera()->orientation =
		    SbRotation(SbVec3f(0.0, 0.0, -1.0), viewDirection);

    // Make sure all is in view in this direction
    viewer->viewAll();

    return viewer;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Mainline
//

main(int argc, char **argv)
//
////////////////////////////////////////////////////////////////////////
{
    SoInput		in;
    SoNode		*root;
    SoXtPlaneViewer	*topViewer, *frontViewer, *rightViewer;
    SoXtExaminerViewer	*perspViewer;
    Widget		form, topWidget, frontWidget, rightWidget, perspWidget;
    int			argN;
    Arg			args[20];

    if (argc != 2) {
	printUsage(argv[0]);
	return 1;
    }

    // Init Inventor
    Widget mainWindow = SoXt::init(argv[0]);

    // Open and read input scene graph
    if (! in.openFile(argv[1])) {
	fprintf(stderr, "%s: Cannot open %s\n", argv[0], argv[1]);
	return 1;
    }
    root = SoDB::readAll(&in);
    if (root == NULL) {
	fprintf(stderr, "%s: Problem reading data\n", argv[0]);
	return 1;
    }

    root->ref();

    // Create a form widget to hold the viewers
    form = XmCreateForm(mainWindow, "form", NULL, 0);

    // Build each of the four viewers
    topViewer   = createPlaneViewer(form, "Top View",   root,
				    SbVec3f(0., -1.,  0.));
    frontViewer = createPlaneViewer(form, "Front View", root,
				    SbVec3f(0.,  0., -1.));
    rightViewer = createPlaneViewer(form, "Right View", root,
				    SbVec3f(-1., 0.,  0.));
    perspViewer = new SoXtExaminerViewer(form);
    perspViewer->setSceneGraph(root);

    // Viewers have the scene, so we don't need it here any more
    root->unref();

    // Get the widgets for the viewers
    topWidget   = topViewer->getWidget();
    frontWidget = frontViewer->getWidget();
    rightWidget = rightViewer->getWidget();
    perspWidget = perspViewer->getWidget();


    // Arrange the viewer's widgets in the form
    argN = 0;
    XtSetArg(args[argN], XmNtopAttachment,	XmATTACH_FORM);		argN++;
    XtSetArg(args[argN], XmNtopOffset,		0);			argN++;
    XtSetArg(args[argN], XmNleftAttachment,	XmATTACH_FORM);		argN++;
    XtSetArg(args[argN], XmNleftOffset,		0);			argN++;
    XtSetArg(args[argN], XmNrightAttachment,	XmATTACH_POSITION);	argN++;
    XtSetArg(args[argN], XmNrightPosition,	50);			argN++;
    XtSetArg(args[argN], XmNbottomAttachment,	XmATTACH_POSITION);	argN++;
    XtSetArg(args[argN], XmNbottomPosition,	50);			argN++;
    XtSetValues(topWidget, args, argN);

    argN = 0;
    XtSetArg(args[argN], XmNbottomAttachment,	XmATTACH_FORM);		argN++;
    XtSetArg(args[argN], XmNbottomOffset,	0);			argN++;
    XtSetArg(args[argN], XmNleftAttachment,	XmATTACH_FORM);		argN++;
    XtSetArg(args[argN], XmNleftOffset,		0);			argN++;
    XtSetArg(args[argN], XmNrightAttachment,	XmATTACH_POSITION);	argN++;
    XtSetArg(args[argN], XmNrightPosition,	50);			argN++;
    XtSetArg(args[argN], XmNtopAttachment,	XmATTACH_WIDGET);	argN++;
    XtSetArg(args[argN], XmNtopWidget,		topWidget);		argN++;
    XtSetValues(frontWidget, args, argN);
    
    argN = 0;
    XtSetArg(args[argN], XmNtopAttachment,	XmATTACH_FORM);		argN++;
    XtSetArg(args[argN], XmNtopOffset,		0);			argN++;
    XtSetArg(args[argN], XmNrightAttachment,	XmATTACH_FORM);		argN++;
    XtSetArg(args[argN], XmNrightOffset,	0);			argN++;
    XtSetArg(args[argN], XmNleftAttachment,	XmATTACH_WIDGET);	argN++;
    XtSetArg(args[argN], XmNleftWidget,		topWidget);		argN++;
    XtSetArg(args[argN], XmNbottomAttachment,	XmATTACH_POSITION);	argN++;
    XtSetArg(args[argN], XmNbottomPosition,	50);			argN++;
    XtSetValues(rightWidget, args, argN);

    argN = 0;
    XtSetArg(args[argN], XmNbottomAttachment,	XmATTACH_FORM);		argN++;
    XtSetArg(args[argN], XmNbottomOffset,	0);			argN++;
    XtSetArg(args[argN], XmNrightAttachment,	XmATTACH_FORM);		argN++;
    XtSetArg(args[argN], XmNrightOffset,	0);			argN++;
    XtSetArg(args[argN], XmNleftAttachment,	XmATTACH_WIDGET);	argN++;
    XtSetArg(args[argN], XmNleftWidget,		frontWidget);		argN++;
    XtSetArg(args[argN], XmNtopAttachment,	XmATTACH_WIDGET);	argN++;
    XtSetArg(args[argN], XmNtopWidget,		rightWidget);		argN++;
    XtSetValues(perspWidget, args, argN);

    // Show the widgets
    XtManageChild(form);
    topViewer->show();
    frontViewer->show();
    rightViewer->show();
    perspViewer->show();
    SoXt::show(mainWindow);

    // Enter the main loop
    SoXt::mainLoop();
}
