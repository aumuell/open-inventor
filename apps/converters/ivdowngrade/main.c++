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
 * Copyright (C) 1993-95   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.4 $
 |
 |   Author(s) : David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <Inventor/SbDict.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInteraction.h>
#include <Inventor/SoInput.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/errors/SoReadError.h>
#include <Inventor/nodes/SoFile.h>
#include <Inventor/nodes/SoSeparator.h>
#include "SoEnvironmentV1.h"
#include "SoPickStyleV1.h"
#include "SoShapeHintsV1.h"
#include "SoTextureCoordinatePlaneV1.h"
#include "SoIndexedTriangleMeshV1.h"
#include "SoWWWAnchorV2.h"
#include "SoWWWInlineV2.h"
#include "SoAsciiTextV2.h"
#include "SoFontStyleV2.h"
#include "SoPackedColorV2.h"
#include "SoVertexPropertyV2.h"
#include "SoText2V2.h"
#include "SoText3V2.h"
#include "SoLODV2.h"

static char *infile = NULL;
static char *outfile = NULL;
static float targetVersion = 2.0;

extern void expandFileNodes(SoNode *&root);
extern SoNode *downgradeToV1(SoNode *n);
extern SoNode *downgradeToV2(SoNode *n);


void
getargs(int argc, char *argv[])
{
    int c, err=0, showUsage = 0;
    while ((c = getopt(argc, argv, "hbv:")) != -1) switch (c) {
	case 'h':
	    // help
	    showUsage = 1;
	    break;
	    
	case 'b': 
	    // We cannot do a binary write, because the binary format
	    // for groups changed from 1.0 to 2.0. We check for the 
	    // option only because it is expected that Inventor file
	    // translators support -b.
	    break;
	    
	case 'v': 
	    // Version we are downgrading to: 1.0 or 2.0
	    targetVersion = atof(optarg);
	    break;
	default: err = 1;
    }
    if (optind == argc-2) {
	infile = argv[argc-2];
	outfile = argv[argc-1];
    }
    else if (optind == argc-1) {
	infile = argv[argc-1];
	// outfile is stdout
    }
    // ??? else read from stdin ? ... else err = 1; //??? can't read from stdin right now.
    
    if (targetVersion != 1.0 && targetVersion != 2.0)
	err = 1;

    if (showUsage || err) {
	fprintf(stderr, "Usage: %s [-v targetVersion] inputFile outputFile\n", argv[0]);
	
	if (err)
	    exit(-1);
    }
}

// Fills in either the path list or the root node, depending what is in the file.
// If reading a node, we always create an extra group on top. This will not be written
// out later, it just lets us carry all the nodes around.
void
readAll(SoInput *in, SoPathList &list, SoNode *&root)
{
    SoBase *base;
    SoGroup *g = new SoGroup;
    g->ref();
    
    while (1) {
	SoDB::read(in, base);
	if (base != NULL) {
	    if (base->isOfType(SoPath::getClassTypeId()))
		 list.append((SoPath *) base);
	    else g->addChild((SoNode *) base);
	}
	else break;
    }
    
    // Did we read paths?
    root = NULL;
    if (list.getLength() > 0) {
	g->unref();
    }
    else {
	// We read nodes.
	if (g->getNumChildren() == 0) {
	    g->unref();
	}
	else {
	    root = g;
	}
    }
}

//
//  Try to recreate path through the passed scene graph.
//  path was valid in the version 2 scene graph, but we may not
//  be able to copy it fully in the version 1 scene graph (i.e.
//  if some nodes could not be converted.)
//
SoPath *
recreatePath(SbPList &pathIndices, SoNode *sceneGraph)
{
    SoPath *newPath = new SoPath(sceneGraph);
    
    if (! sceneGraph->isOfType(SoGroup::getClassTypeId()))
	return newPath;

    SbBool haltHere = FALSE;
    SoGroup *group = (SoGroup *) sceneGraph;
    for (int i = 1; (i < pathIndices.getLength()) && (! haltHere); i++) {
	unsigned long index = (unsigned long) pathIndices[i];

	// Is this a valid index?
	if (index < group->getNumChildren()) {
	    newPath->append(index);
	    group = (SoGroup *) group->getChild(index);
	    if (! group->isOfType(SoGroup::getClassTypeId()))
		haltHere = TRUE;
	}
	else haltHere = TRUE;
    }
    
    return newPath;
}

void
readErrorCB(const SoError *, void *)
{
    // We do not want inventor to output read errors,
    // so we pass them through this dummy routine.
}

int
main(int argc, char *argv[])
{
//??? for (int k = 0; k < argc; k++) printf("%s ", argv[k]);printf("\n");

    // Get arguments
    getargs(argc, argv);
    
    // Init Inventor
    SoInteraction::init();
    SoNode *root = NULL;
    SoPathList pathList;
    
    // Squelch error postings
    SoReadError::setHandlerCallback(readErrorCB, NULL);
    
    // For 2.1 to 2.0 downgrade, we init our helper classes before reading
    if (targetVersion == 2.0) {
	SoWWWAnchorV2::initClass();
	SoWWWInlineV2::initClass();
	SoAsciiTextV2::initClass();
	SoFontStyleV2::initClass();
	SoVertexPropertyV2::initClass();
	SoLODV2::initClass();
    }

    // PackedColor downgrader is used for downgrades to both 1.0 and 2.0
    // and so are text2 and text3:
    SoPackedColorV2::initClass();
    SoText3V2::initClass();
    SoText2V2::initClass();
    	
    // See if this is an Inventor 2.0 file
    SoInput in;
    if (infile != NULL) {
	if (in.openFile(infile)) {
	    char tmpbuf[81];
	    char *buf = fgets(tmpbuf, 80, in.getCurFile());
	    in.closeFile();
    
	    if (SoDB::isValidHeader(buf)) {
		// If the filename includes a directory path, add the directory name 
		// to the list of directories where to look for input files 
		const char *slashPtr;
		char *searchPath = NULL;
		if ((slashPtr = strrchr(infile, '/')) != NULL) {
		    searchPath = strdup(infile);
		    searchPath[slashPtr - infile] = '\0';
		    in.addDirectoryFirst(searchPath);
		}

		// NOTE: file could contain nodes, or it could contain paths,
		// i.e. for copy/paste. If paths, we need to write paths back out!
		// Call our own routine to read...
		in.openFile(infile);
		readAll(&in, pathList, root);
		in.closeFile();
	    }
	}
    }
    else {
	// Reading from stdin.
	readAll(&in, pathList, root);
    }
    
    // Bail if we hit an error
    if ((root == NULL) && (pathList.getLength() == 0))
	return -1;

    if (root != NULL)
	root->ref();
    
    // We need to replace all SoFile nodes with SoGroup.
    // This is because the files referred to are probably 2.0 files.
    // Better to just bring the data in right here and convert the
    // entire scene graph once.
    // We have to do this before we init our downgrader classes,
    // or SoDB gets confused and tries to get our 1.0 classes to read
    // 2.0 data during the SoFile::copyChildren() call.
    if (root != NULL)
	expandFileNodes(root);
    else {
	for (int i = 0; i < pathList.getLength(); i++) {
	    SoPath *path = pathList[i];
	    root = path->getHead();
	    expandFileNodes(root);
	}
	root = NULL;
    }
    
    // Prepare to write out down-graded scene
    SoWriteAction wa;
    SoOutput *out = wa.getOutput();
    
    if (outfile != NULL)
	out->openFile(outfile);
    
    // Inventor V1.0 cannot handle "+" in node names, so use "_" instead.
    if (targetVersion == 1.0)
	SoBase::setInstancePrefix("_");
    
    // NOTE: we cannot binary write, because the binary format
    // for groups changed between 1.0 and 2.0, and for everything
    // changed from 2.0 to 2.1. But writing/reading ascii works fine.

    // This will point to the downgraded scene graph
    SoNode *sceneGraph = NULL;
    
    // Based on the target, intitialize any helper classes needed to do the downgrade
    if (targetVersion == 1.0) {
	// Init our 1.0 classes after reading is complete, since
	// we'll be re-using class names when we go to write, and
	// we don't want to confuse the reading mechanism which looks
	// up classes by name.
	SoEnvironmentV1::initClass();
	SoPickStyleV1::initClass();
	SoShapeHintsV1::initClass();
	SoTextureCoordinatePlaneV1::initClass();
	SoIndexedTriangleMeshV1::initClass();
    
	// NOTES: we can do nothing for the following nodes:
	//  SoLightModel - v2.0 is a subset of v1.0
	//  SoMaterial - v2.0 shininess should be fine for v1.0
	//  SoTextureCoordinateEnvironment - v2.0 has no fields since it
	//     affects ALL coords (S and T), and v1.0 has only one field
	//     which defaults to ALL. So we don't have to do anything.
	
	// The header string
	out->setHeaderString("#Inventor V1.0 ascii");
    }
    else if (targetVersion == 2.0) {
	// The header string
	out->setHeaderString("#Inventor V2.0 ascii");
    }
    
    // Write the scene graph to our output file
    if (root != NULL) {
	// Downgrade to version 1.0 happens as a separate process to the SoWriteAction.
	// This is because there are so many changes since 1.0 - we really have to go
	// in and make changes to the scene.
	if (targetVersion == 1.0)
	    sceneGraph = downgradeToV1(root);
	    
	// Downgrade to 2.0 will occur during the SoWriteAction traversal
	// thanks to the saviour SoUnknownNode!
	else if (targetVersion == 2.0)
	    sceneGraph = downgradeToV2(root);
	    
	sceneGraph->ref();
	
	// Only write out the nodes we read in, i.e. the children of the root
	if (sceneGraph->isOfType(SoGroup::getClassTypeId())) {
	    SoGroup *g = (SoGroup *) sceneGraph;
	    for (int i = 0; i < g->getNumChildren(); i++)
		wa.apply(g->getChild(i));
	}
	else wa.apply(sceneGraph);

#if 0
SoWriteAction wa2;
wa2.getOutput()->setHeaderString(targetVersion == 2.0 ? "#Inventor V2.0 ascii" : "#Inventor V1.0 ascii");
wa2.apply(sceneGraph);
#endif

	sceneGraph->unref();
    }
    
    // Or write the paths to our output file
    else {
	SoPathList newPathList;
	for (int i = 0; i < pathList.getLength(); i++) {
	    SoPath *path = pathList[i];
	    path->ref();
	    SbPList pathIndices;
	    
	    // Squirrel off the pathIndices so the path doesn't get ruined
	    // as we monkey with the scene graph
	    for (int j = 0; j < path->getLength(); j++)
		pathIndices.append((void *) (unsigned long) path->getIndex(j));
	    
	    if (targetVersion == 1.0)
		sceneGraph = downgradeToV1(path->getHead());
	    else if (targetVersion == 2.0)
		sceneGraph = downgradeToV2(path->getHead());
		
	    SoPath *newPath = recreatePath(pathIndices, sceneGraph);
	    newPathList.append(newPath);
	    path->unref();
	}
        
	wa.apply(newPathList);
	
#if 0
SoWriteAction wa2;
wa2.getOutput()->setHeaderString(targetVersion == 2.0 ? "#Inventor V2.0 ascii" : "#Inventor V1.0 ascii");
wa2.apply(newPathList);
#endif

    }
    
    out->closeFile();
    
    return 0; // 0 means success
}

