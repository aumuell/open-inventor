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

#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoIndexedTriangleStripSet.h>

#include "IfHolder.h"
#include "IfReporter.h"
#include "IfShape.h"
#include "IfShapeList.h"

FILE   *IfReporter::fp = stderr;
SbBool	IfReporter::verbose = FALSE;
SbBool	IfReporter::details = FALSE;

/////////////////////////////////////////////////////////////////////////////
//
// Reports an operation that takes time. "Done" is printed when
// done.
//
/////////////////////////////////////////////////////////////////////////////

void
IfReporter::startReport(const char *msg, SbBool isDetail)
{
    if (! verbose || (isDetail && ! details))
	return;

    fprintf(fp, "%s ... ", msg);
    fflush(fp);
}

void
IfReporter::finishReport(SbBool isDetail)
{
    if (! verbose || (isDetail && ! details))
	return;

    fprintf(fp, "Done\n");
}

/////////////////////////////////////////////////////////////////////////////
//
// Reports an index.
//
/////////////////////////////////////////////////////////////////////////////

void
IfReporter::reportIndex(const char *msg, int index, int total)
{
    if (! verbose)
	return;

    // Report only sporadically unless all details are on
    if (details ||
	index == total ||
	total < 20 ||
	(index % (total / 20)) == 0)

	fprintf(fp, "%s %5d (of %d) \n", msg, index + 1, total);
}

/////////////////////////////////////////////////////////////////////////////
//
// Reports a IfHolder
//
/////////////////////////////////////////////////////////////////////////////

void
IfReporter::reportHolder(const char *msg, IfHolder *holder)
{
    if (! details)
	return;

    int nc = holder->coords->point.getNum();
    int ntc = holder->triSet->coordIndex.getNum();
    const int32_t *ind = holder->triSet->coordIndex.getValues(0);

    // Count strips, triangles, and vertices

    int ns = 0, nt = 0, nv = 0;
    int vertsInStrip = 0;

    for (int i = 0; i < ntc; i++) {

	if (ind[i] < 0) {
	    if (vertsInStrip > 2)
		ns++;
	    vertsInStrip = 0;
	}

	else {
	    nv++;
	    if (++vertsInStrip > 2)
		nt++;
	}
    }

    fprintf(fp,
	    "   %s: %5d strips, %5d tris, %5d verts, %5d coords\n",
	    msg, ns, nt, nv, nc);
}

/////////////////////////////////////////////////////////////////////////////
//
// Reports a IfShapeList.
//
/////////////////////////////////////////////////////////////////////////////

void
IfReporter::reportShapeList(const char *msg, IfShapeList *shapeList,
			    SbBool checkDiff)
{
    if (! verbose)
	return;

    fprintf(fp, "%s: %d shapes", msg, shapeList->getLength());

    if (checkDiff) {
	int numDiff = 1;
	for (int i = 1; i < shapeList->getLength(); i++)
	    if ((*shapeList)[i]->differenceLevel > 0)
		numDiff++;
	fprintf(fp, " (%d different property sets)\n", numDiff);
    }
    else
	fprintf(fp, "\n");
}

/////////////////////////////////////////////////////////////////////////////
//
// Reports on the number of nodes in the given graph
//
/////////////////////////////////////////////////////////////////////////////

void
IfReporter::reportNodeCount(const char *msg, SoNode *root)
{
    if (! verbose)
	return;

    int nodeCount = 0;

    SoCallbackAction cba;
    cba.addPreCallback(SoNode::getClassTypeId(), countCB, &nodeCount);
    cba.apply(root);

    fprintf(fp, "%s: %d nodes\n", msg, nodeCount);
}
