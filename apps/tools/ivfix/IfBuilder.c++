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

#define DEBUG_WRITE 0

#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/elements/SoLazyElement.h>
#include <Inventor/elements/SoTextureImageElement.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSeparator.h>

#include "IfAssert.h"
#include "IfBuilder.h"
#include "IfCondenser.h"
#include "IfFlattener.h"
#include "IfHolder.h"
#include "IfReporter.h"
#include "IfShape.h"
#include "IfStripper.h"

// All roots at Level 5 have this name so we can find them easily:
#define LEVEL_5_ROOT_NAME	"__level5Root"

#if DEBUG_WRITE
#include <Inventor/actions/SoWriteAction.h>
#endif

/////////////////////////////////////////////////////////////////////////////
//
// Constructor.
//
/////////////////////////////////////////////////////////////////////////////

IfBuilder::IfBuilder()
{
}

/////////////////////////////////////////////////////////////////////////////
//
// Destructor.
//
/////////////////////////////////////////////////////////////////////////////

IfBuilder::~IfBuilder()
{
}

/////////////////////////////////////////////////////////////////////////////
//
// This takes a sorted IfShapeList and builds a minimal scene graph that
// represents it.
//
/////////////////////////////////////////////////////////////////////////////

SoNode *
IfBuilder::build(const IfShapeList &shapeList, SbBool _doStrips, SbBool _doVP,
		 SbBool _doAnyNormals, SbBool _doAnyTexCoords)
{
    doStrips       = _doStrips;
    doVP           = _doVP;
    doAnyNormals   = _doAnyNormals;
    doAnyTexCoords = _doAnyTexCoords;

    //////////////////////////////////////////////////////////////////
    //
    // We are going to build a 5-level scene graph. The top level (0)
    // is just a root node. Levels 1-5 are separators under which
    // nodes of the corresponding levels are stored, as well as the
    // lower-level root separators. These nodes are stored only when
    // they differ between shapes.
    //
    // When we are done, we will prune out any unnecessary separators
    // at levels that do not contain differences.
    //

    // Build the top-level root
    roots[0] = new SoSeparator;
    roots[0]->ref();

    // Build the other 5 roots
    IfShape *shape = shapeList[0];
    buildRoots(1, shape);

    // Run through the sorted list of shapes. Every time we hit a
    // shape that differs from the previous shape at Level 1, 2, 3, or
    // 4, we create a new root at the appropriate level and add the
    // nodes at that level to it.

    for (int i = 1; i < shapeList.getLength(); i++) {

	IfShape *shape = shapeList[i];

	// Skip over a shape that is the same as the previous one (as
	// a result of merging)
	if (shape == shapeList[i - 1])
	    continue;

	ASSERT(shape->differenceLevel >= 0 && shape->differenceLevel <= 5);

	if (shape->differenceLevel > 0)
	    buildRoots(shape->differenceLevel, shape);

	// Always add the appropriate nodes for Level 5
	else
	    shape->addNodesForLevel(roots[5], 5);

	// We are done with the nodes in this shape, so unref them to
	// see if we can free up some memory
	shape->clearNodes();
    }

#if DEBUG_WRITE
    {
	SoWriteAction wa;
	wa.getOutput()->openFile("PREFLATTEN.iv");
	wa.apply(roots[0]);
    }
#endif

    // Now replace all level-5 roots with the result of flattening,
    // condensing, and so forth
    replaceLevel5();

#if DEBUG_WRITE
    {
	SoWriteAction wa;
	wa.getOutput()->openFile("PREREMOVE.iv");
	wa.apply(roots[0]);
    }
#endif

    // Now remove any separators that have only one child
    removeUnnecessarySeparators(roots[0], 0);

    // Get rid of the top separator if it has only one child
    while (roots[0]->getNumChildren() == 1) {
	SoSeparator *newRoot = (SoSeparator *) roots[0]->getChild(0);
	newRoot->ref();
	roots[0]->unref();
	roots[0] = newRoot;

	// If the new top root isn't really a separator, stop. (It
	// might be a derived class.)
	if (roots[0]->getTypeId() != SoSeparator::getClassTypeId())
	    break;
    }

#if DEBUG_WRITE
    {
	SoWriteAction wa;
	wa.getOutput()->openFile("POSTREMOVE.iv");
	wa.apply(roots[0]);
    }
#endif

    roots[0]->unrefNoDelete();
    return roots[0];
}

/////////////////////////////////////////////////////////////////////////////
//
// Builds the roots from the given level down.
//
/////////////////////////////////////////////////////////////////////////////

void
IfBuilder::buildRoots(int startLevel, IfShape *shape)
{
    ASSERT(startLevel > 0 && startLevel <= 5);

    for (int level = startLevel; level <= 5; level++) {

	roots[level] = new SoSeparator;
	roots[level-1]->addChild(roots[level]);

	// Add the appropriate nodes for the level
	shape->addNodesForLevel(roots[level], level);
    }

    // Every time this is called, a new Level 5 root is built. Name it
    // so we can find all of them easily later on, unless we really
    // don't want to flatten it because the IfShape says so.
    if (! shape->dontFlatten)
	roots[5]->setName(LEVEL_5_ROOT_NAME);
}

/////////////////////////////////////////////////////////////////////////////
//
// Replaces all level-5 roots with the result of flattening.
//
/////////////////////////////////////////////////////////////////////////////

void
IfBuilder::replaceLevel5()
{
    // Find all paths to the roots with the Level 5 name
    SoSearchAction sa;
    sa.setName(LEVEL_5_ROOT_NAME);
    sa.setInterest(SoSearchAction::ALL);
    sa.apply(roots[0]);

    // There may not be any, depending on the shape types we found
    int numSubGraphs = sa.getPaths().getLength();

    for (int i = 0; i < numSubGraphs; i++) {

	IfReporter::reportIndex("Doing subgraph", i, numSubGraphs);

	const SoPath *path = sa.getPaths()[i];
	SoNode *level5Root = path->getTail();
	ASSERT(path->getLength() > 1);
	ASSERT(level5Root->getTypeId() == SoSeparator::getClassTypeId());

	// Traverse the path with a callback action to determine if
	// normals and texture coordinates are required
	SbBool doNormals, doTexCoords;
	getFlags((SoPath *) path, doNormals, doTexCoords);

	// Replace the level 5 root with the result of flattening its
	// subgraph
	SoNode *flatRoot = flatten(path,
				   doAnyNormals && doNormals,
				   doAnyTexCoords && doTexCoords);
	SoSeparator *parent = (SoSeparator *) path->getNodeFromTail(1);
	ASSERT(parent->getTypeId() == SoSeparator::getClassTypeId());
	parent->replaceChild(level5Root, flatRoot);
    }
}

/////////////////////////////////////////////////////////////////////////////
//
// Traverses the given path with a callback action to determine if
// normals and texture coordinates are required for shapes in it.
//
/////////////////////////////////////////////////////////////////////////////

void
IfBuilder::getFlags(SoPath *path, SbBool &doNormals, SbBool &doTexCoords)
{
    // The callback will store the flag values in this array, which we
    // pass in as user data
    SbBool flags[2];

    SoCallbackAction ca;
    ca.addPreTailCallback(flagCB, flags);
    ca.apply(path);

    // Access the stored flags
    doNormals   = flags[0];
    doTexCoords = flags[1];
}

/////////////////////////////////////////////////////////////////////////////
//
// This is called through the callback.
//
/////////////////////////////////////////////////////////////////////////////

void
IfBuilder::setFlags(SoCallbackAction *cba, SbBool *flags)
{
    // Do normals only if lighting is on
    flags[0] = (SoLazyElement::getLightModel(cba->getState()) ==
		SoLazyElement::PHONG);

    // Do texture coordinates only if there is a texture
    SbVec2s s;
    int nc, wrapS, wrapT, model;
    SbColor blendColor;
    flags[1] = (SoTextureImageElement::get(cba->getState(), s, nc,
					   wrapS, wrapT,
					   model, blendColor) != NULL);
}

/////////////////////////////////////////////////////////////////////////////
//
// Flattens the subgraph at the tail of the given path, returning
// the resulting graph.
//
/////////////////////////////////////////////////////////////////////////////

SoNode *
IfBuilder::flatten(const SoPath *path, SbBool doNormals, SbBool doTexCoords)
{
    // Copy all properties from higher in the path to this root, just
    // in case the subgraph at the tail needs them. Specifically,
    // SoShapeHints nodes that are higher up will affect the triangle
    // generation, so we need to make sure they are included.
    SoSeparator *root = collectObject(path);
    root->ref();

    // Create a IfHolder to hold everything
    IfHolder *holder = new IfHolder(root, doStrips, doNormals, doTexCoords);
    root->unref();

    // Flatten to produce triangles

    IfReporter::startReport("  Flattening", TRUE);
    IfFlattener *flattener = new IfFlattener;
    flattener->flatten(holder);
    delete flattener;
    IfReporter::finishReport(TRUE);

    IfReporter::reportHolder("    After flattening", holder);

    // Condense the result
    IfReporter::startReport("  Condensing", TRUE);
    IfCondenser *condenser = new IfCondenser;
    condenser->condense(holder);
    delete condenser;
    IfReporter::finishReport(TRUE);

    IfReporter::reportHolder("    After condensing", holder);

    if (doStrips) {
	// Produce better triangle strips
	IfReporter::startReport("  Stripping ", TRUE);
	IfStripper *stripper = new IfStripper;
	stripper->strip(holder);
	delete stripper;
	IfReporter::finishReport(TRUE);

	IfReporter::reportHolder("    After stripping ", holder);
    }

    if (doVP) {
	// Find the last material in the object
	int i;
	for (i = root->getNumChildren() - 1; i >= 0; i--) {
	    if (root->getChild(i)->isOfType(SoMaterial::getClassTypeId()))
		break;
	}
	SoMaterial *mtl = (i >= 0 ? (SoMaterial *) root->getChild(i) : NULL);
	holder->convertToVertexProperty(mtl);
    }

    SoNode *result = holder->root;
    result->ref();
    delete holder;
    result->unrefNoDelete();

    return result;
}

/////////////////////////////////////////////////////////////////////////////
//
// Collects all properties along the given path (above the tail)
// and returns a separator-rooted graph that contains all of them
// and the tail of the path.
//
/////////////////////////////////////////////////////////////////////////////

SoSeparator *
IfBuilder::collectObject(const SoPath *path)
{
    // Create a separator to hold everything
    SoSeparator *root = new SoSeparator;
    root->ref();

    // Traverse all nodes along the path. They should all be separators.
    int numAboveTail = path->getLength() - 1;
    for (int i = 0; i < numAboveTail; i++) {
	ASSERT(path->getNode(i)->isOfType(SoSeparator::getClassTypeId()));

	SoSeparator *sep = (SoSeparator *) path->getNode(i);

	int nextIndexInPath = path->getIndex(i+1);

	for (int j = 0; j < nextIndexInPath; j++) {
	    SoNode *node = sep->getChild(j);
	    if (node->affectsState())
		root->addChild(node);
	}
    }

    root->addChild(path->getTail());

    root->unrefNoDelete();
    return root;
}

/////////////////////////////////////////////////////////////////////////////
//
// Recursive procedure that removes any separators that have only one
// child.
//
/////////////////////////////////////////////////////////////////////////////

void
IfBuilder::removeUnnecessarySeparators(SoSeparator *root, int level)
{
    // If the last child is a separator, move its children here and
    // remove it.

    int curLevel = level;
    while (TRUE) {

	SoNode *lastKid = root->getChild(root->getNumChildren() - 1);

	if (lastKid->getTypeId() == SoSeparator::getClassTypeId()) {

	    // A Level 2 separator should NOT be removed unless there
	    // is no camera under its Level 1 separator. This keeps
	    // caching ok under the camera.
	    if (curLevel == 1 && root->getNumChildren() > 1)
		break;

	    SoSeparator *kidSep = (SoSeparator *) lastKid;

	    // Remove the kid from the root
	    kidSep->ref();
	    root->removeChild(root->getNumChildren() - 1);

	    // Move the children
	    for (int i = 0; i < kidSep->getNumChildren(); i++)
		root->addChild(kidSep->getChild(i));

	    kidSep->unref();
	}
	else
	    break;

	curLevel++;
    }

    // Recurse on the children
    for (int i = 0; i < root->getNumChildren(); i++) {
	SoNode *kid = root->getChild(i);
	if (kid->getTypeId() == SoSeparator::getClassTypeId())
	    removeUnnecessarySeparators((SoSeparator *) kid, level + 1);
    }
}
