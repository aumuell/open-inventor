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
// Takes a scene graph and generates normals for it
//

#include <assert.h>
#include <math.h>

#include <Inventor/SoDB.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/actions/SoSearchAction.h>

#include "FindNormals.h"
#include "Edges.h"

FindNormals::FindNormals()
{
    defaultOrientation = Face::UNKNOWN;
    creaseAngle = M_PI/6;
    verbose = FALSE;
}

FindNormals::~FindNormals()
{
}

void
FindNormals::AssumeOrientation(FaceOrientation o)
{
    defaultOrientation = o;
}    

void
FindNormals::apply(SoNode *root, int doVertexNormals)
{
    //
    // IndexedFaceSets:
    // First, search for them:
    //
    if (verbose)
	fprintf(stderr, "FindNormals: searching for IndexedFaceSets.\n");
    SoSearchAction sa;
    sa.setType(SoIndexedFaceSet::getClassTypeId());
    sa.setInterest(SoSearchAction::ALL);
    sa.apply(root);
    SoPathList &paths = sa.getPaths();
    if (verbose) fprintf(stderr, "FindNormals: searching finished.\n");
    for (int i=0; i < paths.getLength(); i++) {
	if (verbose && !(i%50))
	    fprintf(stderr, "FindNormals: faceSet %d of %d\n",
			    i+1, paths.getLength());

	SoGroup *parent = (SoGroup*)paths[i]->getNodeFromTail(1);

	// disable notification, or we'll have n^2 time spent notifying
	// the entire path list for every change.
#ifndef INVENTOR1
	SbBool pNot = parent->enableNotify(FALSE);
#else
	parent->setNotification(FALSE);
#endif

	doIndexedFaceSet(paths[i], doVertexNormals);

	// restore notification
#ifndef INVENTOR1
	parent->enableNotify(pNot);
#else
	parent->setNotification(TRUE);
#endif
    }
}

SoNode *
FindNormals::searchLastType(SoPath *p, SoType t)
{
    SoSearchAction sa;
    sa.setType(t);
    sa.setInterest(SoSearchAction::LAST);
    sa.apply(p);
    SoPath *foundPath = sa.getPath();

    if (foundPath != NULL)
	return foundPath->getTail();

    return NULL;
}

void
FindNormals::doIndexedFaceSet(SoPath *p, int doVertexNormals)
{
    // If there is already a normal node, bail

    if (searchLastType(p, SoNormal::getClassTypeId()) != NULL)
	return;

    //
    // First, find Coordinates in path:
    //
    SoCoordinate3 *coords = (SoCoordinate3 *)
	searchLastType(p, SoCoordinate3::getClassTypeId());

    SoIndexedFaceSet *ifs = (SoIndexedFaceSet *)p->getTail();

    assert(coords != NULL);

    FaceList faces(coords->point.getValues(0), ifs);

    if (defaultOrientation == Face::UNKNOWN)
    {
	faces.findOrientation();
    }
    else
    {
	for (int i = 0; i < faces.getLength(); i++)
	{
	    faces[i]->orientation = defaultOrientation;
	}
	faces.correctOrientation();
    }

    SoNormal *n = new SoNormal;
    SoNormalBinding *normb = new SoNormalBinding;

    SoNode *tn = p->getNode(p->getLength()-2);
    assert(tn->isOfType(SoGroup::getClassTypeId()));

    SoGroup *g = (SoGroup *)tn;
    
    int index = g->findChild(ifs);

    assert(index != -1);

    g->insertChild(normb, index);
    g->insertChild(n, index);

    faces.findShapeInfo();
    if (faces.isSolid()) {
	SoShapeHints *hints = new SoShapeHints;
	hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
	hints->shapeType = SoShapeHints::SOLID;
	g->insertChild(hints, index);
    }

    if (doVertexNormals)
    {
	faces.findVertexNormals(n, ifs, creaseAngle);
	normb->value.setValue(SoNormalBinding::PER_VERTEX_INDEXED);
    }
    else
    {
	faces.findFacetNormals(n);
	normb->value.setValue(SoNormalBinding::PER_FACE);
    }
}
