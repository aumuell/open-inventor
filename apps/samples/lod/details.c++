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

#include <stdlib.h>
#include <Inventor/SoDB.h>	
#include <Inventor/SoInput.h>
#include <Inventor/Xt/SoXt.h>	
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
// #include <Inventor/nodes/SoLevelOfDetail.h>
#include <Inventor/nodes/SoLOD.h>
#include <Inventor/nodes/SoSeparator.h>


void
main(int argc, char *argv[])
{
    char *filename = "lod.iv";

    if (argc < 2) {
	fprintf(stderr, "NOTE: You can specify your own geometry file.\n");
	fprintf(stderr, "Run: %s inputFile.\n", argv[0]);
	fprintf(stderr, "Running with the default geometry, %s.\n", filename);
    } else {
	filename = argv[1];
    }
    
    // Initialize Inventor and Xt
    Widget mainWindow = SoXt::init(argv[0]);
    
    // Read the geometry from the file
    SoInput in;
    SoNode *fileGeom;
    if (! in.openFile(filename))
	exit(1);
    if(! SoDB::read(&in, fileGeom) || fileGeom == NULL)
	exit(1);

    // Create the root node and the empty LevelOfDetail node
    SoSeparator *root = new SoSeparator;
    root->ref();

    // Using the SoLOD node is the recommended way of doing
    // level-of-detail switching in Inventor 2.1 - it's faster
    // than the old SoLevelOfDetail
    SoLOD *lod = new SoLOD; 
    root->addChild(lod);
    lod->range.set1Value(0, 25);
    lod->range.set1Value(1, 50);
    lod->range.set1Value(2, 100);
    lod->center.setValue(0,0,0);

    // Pre-2.1 programs must use the SoLevelOfDetail node
    // SoLevelOfDetail *lod = new SoLevelOfDetail; 
    // root->addChild(lod);
    // lod->screenArea.set1Value(0, 110000.);
    // lod->screenArea.set1Value(1,  20000.);
    // lod->screenArea.set1Value(2,   3000.);
    // lod->screenArea.set1Value(3,   1000.);

    // Search for the different levels by name, and insert
    // into the level-of-detail group
    SoSeparator *level1 = (SoSeparator *)fileGeom->getByName("Level1");
    if (level1 != NULL && 
	level1->isOfType(SoSeparator::getClassTypeId())) {
	lod->addChild(level1);
    }

    SoSeparator *level2 = (SoSeparator *)fileGeom->getByName("Level2");
    if (level2 != NULL && 
	level2->isOfType(SoSeparator::getClassTypeId())) {
	lod->addChild(level2);
    }

    SoSeparator *level3 = (SoSeparator *)fileGeom->getByName("Level3");
    if (level3 != NULL && 
	level3->isOfType(SoSeparator::getClassTypeId())) {
	lod->addChild(level3);
    }

    SoSeparator *level4 = (SoSeparator *)fileGeom->getByName("Level4");
    if (level4 != NULL && 
	level4->isOfType(SoSeparator::getClassTypeId())) {
	lod->addChild(level4);
    }

    fprintf(stderr, "\nUse the left&middle mouse buttons to zoom\n");
    
    // Create the viewer
    SoXtExaminerViewer *viewer = new SoXtExaminerViewer(mainWindow);
    viewer->setDecoration(FALSE);
    viewer->setTitle("Details, details");
    viewer->setSceneGraph(root);
    viewer->show();
    
    // Loop forever
    SoXt::show(mainWindow);
    SoXt::mainLoop();
}
