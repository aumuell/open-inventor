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
 |   $Revision: 1.3 $
 |
 |   Description:
 |	Generates primitives for all shapes in input file, spins them
 |	around a little.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <unistd.h>
#include <stdlib.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoFaceSet.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoShape.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/sensors/SoTimerSensor.h>

struct Options {
    int		numGrowthFrames;
    int		numPauseFrames;
    float	rotationSpeed;
    float	translationSpeed;
    const char	*inputFileName;
};

static Options options;

// These are used to build scene graphs during an SoCallbackAction and
// to modify them during the timer callback
static SoCoordinate3		*coordNode;
static SoFaceSet		*faceSetNode;
static SoNormal			*normalNode;
static SoRotationXYZ		*rotNode;

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
    fprintf(stderr, "Usage: %s [-gprt] datafile\n", progName);
    fprintf(stderr,
	    "\t-g N  Grow N frames before shrinking (default 100)\n"
	    "\t-p N  Pause N frames before each cycle (default 20)\n"
	    "\t-r N  Rotation speed (default 10.0)\n"
	    "\t-t N  Translation speed (default 0.05)\n");
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Parses command line arguments, setting options.
//

static SbBool
parseArgs(int argc, char *argv[])
//
////////////////////////////////////////////////////////////////////////
{
    SbBool	ok = TRUE;
    int		c, curArg;

    // Initialize options
    options.numGrowthFrames	= 100;
    options.numPauseFrames	= 20;
    options.rotationSpeed	= 10.0;
    options.translationSpeed	= 0.05;
    options.inputFileName	= NULL;

    while ((c = getopt(argc, argv, "g:p:r:t:")) != -1) {
	switch (c) {
	  case 'g':
	    options.numGrowthFrames = atoi(optarg);
	    break;
	  case 'p':
	    options.numPauseFrames = atoi(optarg);
	    break;
	  case 'r':
	    options.rotationSpeed = atof(optarg);
	    break;
	  case 't':
	    options.translationSpeed = atof(optarg);
	    break;
	  default:
	    ok = FALSE;
	    break;
	}
    }

    curArg = optind;

    // Check for input filename at end
    if (curArg < argc)
	options.inputFileName = argv[curArg++];

    // Not enough or extra arguments? Error!
    if (options.inputFileName == NULL || curArg < argc)
	ok = FALSE;

    return ok;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Multiplies the given point by the given matrix, returning the
//    result.
//

static const SbVec3f &
transformPoint(const SbMatrix &m, const SbVec3f &p)
//
////////////////////////////////////////////////////////////////////////
{
    static SbVec3f	tp;

    m.multVecMatrix(p, tp);
    return tp;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Multiplies the given normal by the given matrix, returning the
//    result.
//

static const SbVec3f &
transformNormal(const SbMatrix &m, const SbVec3f &n)
//
////////////////////////////////////////////////////////////////////////
{
    static SbVec3f	tn;

    m.multDirMatrix(n, tn);
    tn.normalize();
    return tn;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback used to add triangle info to scene graph being built.
//

static void
addTriangleCB(void *, SoCallbackAction *cba,
	      const SoPrimitiveVertex *v1,
	      const SoPrimitiveVertex *v2,
	      const SoPrimitiveVertex *v3)
//
////////////////////////////////////////////////////////////////////////
{
    int		n;

    // Get current model matrix
    const SbMatrix	&mm = cba->getModelMatrix();
    SbMatrix		nm  = mm.inverse().transpose();

    // Add coordinates
    n = coordNode->point.getNum();
    coordNode->point.set1Value(n,   transformPoint(mm, v1->getPoint()));
    coordNode->point.set1Value(n+1, transformPoint(mm, v2->getPoint()));
    coordNode->point.set1Value(n+2, transformPoint(mm, v3->getPoint()));

    // Add normals
    n = normalNode->vector.getNum();
    normalNode->vector.set1Value(n,   transformNormal(nm, v1->getNormal()));
    normalNode->vector.set1Value(n+1, transformNormal(nm, v2->getNormal()));
    normalNode->vector.set1Value(n+2, transformNormal(nm, v3->getNormal()));

    // Add a face to face set
    n = faceSetNode->numVertices.getNum();
    faceSetNode->numVertices.set1Value(n, 3);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Creates a scene graph containing a face set that is made up of
//    primitives.
//

static SoSeparator *
buildGraph(SoNode *root)
//
////////////////////////////////////////////////////////////////////////
{
    SoSeparator			*newRoot;
    SoRotationXYZ		*rot;
    SoMaterialBinding		*mb;
    SoMaterial			*mat;
    SoNormalBinding		*nb;
    SoNormal			*norm;
    SoCoordinate3		*coord;
    SoFaceSet			*fset;

    // Construct scene graph
    newRoot = new SoSeparator;
    newRoot->ref();

    rot = new SoRotationXYZ;
    rot->axis = SoRotationXYZ::Y;

    mb = new SoMaterialBinding;
    mb->value = SoMaterialBinding::OVERALL;

    mat = new SoMaterial;
    mat->ambientColor.setValue(0.24725, 0.2245, 0.0645);
    mat->diffuseColor.setValue(0.34615, 0.3143, 0.0903);
    mat->specularColor.setValue(0.797357, 0.723991, 0.208006);
    mat->shininess = 0.65;

    nb = new SoNormalBinding;
    nb->value = SoNormalBinding::PER_VERTEX;

    norm = new SoNormal;
    norm->vector.deleteValues(0);

    coord = new SoCoordinate3;
    coord->point.deleteValues(0);

    fset = new SoFaceSet;
    fset->numVertices.deleteValues(0);

    newRoot->addChild(rot);
    newRoot->addChild(mb);
    newRoot->addChild(mat);
    newRoot->addChild(nb);
    newRoot->addChild(norm);
    newRoot->addChild(coord);
    newRoot->addChild(fset);

    rotNode	= rot;
    coordNode	= coord;
    normalNode	= norm;
    faceSetNode	= fset;

    // Set up callbacks to add to scene graph, then apply
    SoType		shapeType = SoShape::getClassTypeId();
    SoCallbackAction	ca;
    ca.addTriangleCallback(shapeType, addTriangleCB, NULL);
    ca.apply(root);

    newRoot->unrefNoDelete();
    return newRoot;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Creates a scene graph with a big invisible sphere so that the
//    viewer will view a large enough region. 
//

static void
addBigSphere(SoSeparator *root)
//
////////////////////////////////////////////////////////////////////////
{
    // See how big the scene starts out
    SbViewportRegion region;
    SoGetBoundingBoxAction	ba(region);
    ba.apply(root);
    SbBox3f	box = ba.getBoundingBox();
    SbVec3f	size;
    box.getSize(size[0], size[1], size[2]);

    SoSeparator	*sep = new SoSeparator;
    SoDrawStyle	*ds  = new SoDrawStyle;
    SoTransform	*xf  = new SoTransform;

    ds->style = SoDrawStyle::INVISIBLE;
    xf->translation = box.getCenter();
    xf->scaleFactor = 1.5 * size;		// 3x as big

    sep->addChild(ds);
    sep->addChild(xf);
    sep->addChild(new SoSphere);

    root->addChild(sep);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Timer sensor callback that changes coordinates.
//

static void
timerCB(void *, SoSensor *)
//
////////////////////////////////////////////////////////////////////////
{
    static SbBool	firstTime = TRUE;
    static SbVec3f	center;
    static int		counter = 0, osc = 0;
    static SbBool	increasing = TRUE;
    static SbVec3f	*origCoords;
    int			i, n;

    n = coordNode->point.getNum();

    if (firstTime) {
	// Save original coordinates and compute center of all of them
	origCoords = new SbVec3f[n];

	const SbVec3f	*coords = coordNode->point.getValues(0);

	center.setValue(0.0, 0.0, 0.0);

	for (i = 0; i < n; i++) {
	    origCoords[i] = coords[i];
	    center += coords[i];
	}

	center /= n;

	firstTime = FALSE;
    }

    // Pause for a little while
    if (counter < options.numPauseFrames) {
	counter++;
	return;
    }

    // Rotate object
    rotNode->angle = (options.rotationSpeed *
		      (float) counter /
		      (options.numGrowthFrames - options.numPauseFrames));

    // Translate coordinates outward from center, one triangle at a time
    SbVec3f	*coords = coordNode->point.startEditing();
    SbVec3f	avgCoord, offset;
    for (i = 0; i < n; i += 3) {
	avgCoord = (origCoords[i] + origCoords[i+1] + origCoords[i+2]) / 3.0;
	offset = options.translationSpeed * osc * (avgCoord - center);
	coords[i]   = origCoords[i]   + offset;
	coords[i+1] = origCoords[i+1] + offset;
	coords[i+2] = origCoords[i+2] + offset;
    }
    coordNode->point.finishEditing();

    counter++;
    if (increasing) {
	if (++osc == options.numGrowthFrames)
	    increasing = FALSE;
    }
    else {
	if (--osc == -1) {
	    counter = 0;	// Reset for another pause
	    osc = 0;
	    increasing = TRUE;
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback for Xt events, used to set up the timer sensor once the
//    main window has been mapped.
//

static void
eventHandler(Widget, XtPointer *data, XAnyEvent *xe, Boolean *)
//
////////////////////////////////////////////////////////////////////////
{
    SoTimerSensor *timer = (SoTimerSensor *) data;

    if (xe->type == MapNotify)
	timer->schedule();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Mainline
//

void
main(int argc, char **argv)

//
////////////////////////////////////////////////////////////////////////
{
    SoInput		in;
    SoNode		*root;
    SoXtExaminerViewer	*viewer;
    SoTimerSensor	*timer;
    SoSeparator		*newRoot;

    // Init Inventor
    Widget mainWindow = SoXt::init(argv[0]);

    // Parse arguments
    if (! parseArgs(argc, argv)) {
	printUsage(argv[0]);
	exit(1);
    }

    // Open and read input scene graph
    if (! in.openFile(options.inputFileName)) {
	fprintf(stderr, "%s: Cannot open %s\n",
		argv[0], options.inputFileName);
	exit(1);
    }
    root = SoDB::readAll(&in);
    if (root == NULL) {
	fprintf(stderr, "%s: Problem reading data\n", argv[0]);
	exit(1);
    }

    root->ref();
    viewer = new SoXtExaminerViewer(mainWindow);
    viewer->setBackgroundColor(SbColor(0.1, 0.1, 0.3));
    newRoot = buildGraph(root);
    newRoot->ref();
    root->unref();

    // Add a big sphere to get the scene viewed properly
    addBigSphere(newRoot);
    viewer->setSceneGraph(newRoot);
    // Remove the sphere
    newRoot->removeChild(newRoot->getNumChildren() - 1);
    newRoot->unref();

    viewer->show();
    SoXt::show(mainWindow);

    timer = new SoTimerSensor(timerCB, NULL);
    timer->setInterval(0.03333);

    // Wait for the window to be mapped before scheduling the timer
    XtAddEventHandler(mainWindow, StructureNotifyMask, False,
		      (XtEventHandler) eventHandler, (XtPointer) timer);

    SoXt::mainLoop();
}
