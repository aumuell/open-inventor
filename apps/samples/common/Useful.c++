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
// A class containing some utility routines that I find useful.
//

#include <Inventor/SoDB.h>
#include <Inventor/SoLists.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/nodes/SoSeparator.h>

#include "Useful.h"

//
// Read a file given a filename and return a separator containing all
// of the stuff in the file.
//
SoSeparator *
Useful::readFile(const char *filename)
{
    SoInput in;
    if (filename != NULL) {
	if (in.openFile(filename) == FALSE) {
	    fprintf(stderr, "Could not open file %s\n", filename);
	    return NULL;
	}
    }
    return readFile(in);
}
//
// Read a file given a file pointer...
//
SoSeparator *
Useful::readFile(FILE *fp)
{
    SoInput in;
    in.setFilePointer(fp);
    return readFile(in);
}
//
// And read a file given an SoInput.  This is used by the other
// readFile routines.
//
SoSeparator *
Useful::readFile(SoInput &in)
{
    SoSeparator *graph = new SoSeparator;
    graph->ref();

    //
    // Keep on reading until there are no more nodes to read
    //
    SoNode *root;
    do {
	int read_ok = SoDB::read(&in, root);

	if (!read_ok) {
	    fprintf(stderr, "Error reading file\n");
	    graph->unref();
	    return NULL;
	}
	else if (root != NULL) graph->addChild(root);

    } while (root != NULL);
    in.closeFile();

    //
    // Try to avoid creating extra separators; if this scene graph
    // already has exactly one separator at the top, use it.  This
    // will avoid an explosion of separators at the top of scenes that
    // would otherwise occur if we automatically created a new
    // separator every time a scene graph was read.
    //
    if (graph->getNumChildren() == 1 && 
		graph->getChild(0)->isOfType(
		SoSeparator::getClassTypeId())) {
	SoSeparator *result = (SoSeparator *)graph->getChild(0);
	result->ref();	// Note the order here!
	graph->unref();

	result->unrefNoDelete();
	return result;
    }

    graph->unrefNoDelete();
    return graph;
}

//
// I use this routine to make a good guess at which material or
// texture or material binding or other property affects a given
// shape.  For example, the last material on the path to an object
// will be that object's material(s), unless ignore flags are set on
// that material's fields.
//
SoNode *
Useful::searchLastType(SoPath *p, SoType t)
{
    SoSearchAction sa;
    sa.setType(t);
    sa.setInterest(SoSearchAction::LAST);
    sa.apply(p);
    SoPath *outPath = sa.getPath();

    SoNode *result = NULL;
    if (outPath != NULL && (outPath->getLength() > 0) )
        result = outPath->getTail();

    return result; 
}

