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

#include "IfBuilder.h"
#include "IfCollector.h"
#include "IfFixer.h"
#include "IfMerger.h"
#include "IfReplacer.h"
#include "IfReporter.h"
#include "IfShape.h"
#include "IfShapeList.h"
#include "IfSorter.h"
#include "IfWeeder.h"

/////////////////////////////////////////////////////////////////////////////
//
// Constructor.
//
/////////////////////////////////////////////////////////////////////////////

IfFixer::IfFixer()
{
    doStrips	= TRUE;
    doVP	= TRUE;
    doNormals	= TRUE;
    doTexCoords	= TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Destructor.
//
/////////////////////////////////////////////////////////////////////////////

IfFixer::~IfFixer()
{
}

/////////////////////////////////////////////////////////////////////////////
//
// Sets the status reporting level and file. By default, no status
// report is made.
//
/////////////////////////////////////////////////////////////////////////////

void
IfFixer::setReportLevel(ReportLevel level, FILE *fp)
{
    IfReporter::setFile(fp);

    SbBool verbose, details;

    switch (level) {
      case NONE:
	verbose = details = FALSE;
	break;
      case LOW:
	verbose = TRUE;
	details = FALSE;
	break;
      case HIGH:
	verbose = details = TRUE;
	break;
    }

    IfReporter::setVerbose(verbose);
    IfReporter::setDetails(details);
}

/////////////////////////////////////////////////////////////////////////////
//
// Fixes a scene graph, returning the root of the result, or NULL
// on error. If the passed root is not ref'ed, its memory will be
// freed up before this finishes.
//
/////////////////////////////////////////////////////////////////////////////

SoNode *
IfFixer::fix(SoNode *root)
{
    root->ref();

    IfReporter::reportNodeCount("Original graph", root);

    // Replace hard-to-deal-with nodes with friendlier nodes
    IfReporter::startReport("Replacing problem nodes");
    IfReplacer *replacer = new IfReplacer;
    replacer->replace(root);
    delete replacer;
    IfReporter::finishReport();

    // Collect all shapes
    IfReporter::startReport("Collecting shapes");
    IfShape *shapes;
    IfCollector *collector = new IfCollector;
    int numShapes = collector->collect(root, shapes, doTexCoords);
    delete collector;
    IfReporter::finishReport();

    // The IfShape instances ref() all the nodes they keep, so we don't
    // have to keep around the original scene graph
    root->unref();

    // Make sure we have at least 1
    if (numShapes == 0)
	return NULL;

    // Create a list of the shapes
    IfShapeList shapeList(numShapes, shapes);
    IfReporter::reportShapeList("After collecting", &shapeList);

    // Sort the shapes
    IfReporter::startReport("Sorting shapes");
    IfSorter *sorter = new IfSorter;
    sorter->sort(shapeList);
    delete sorter;
    IfReporter::finishReport();
    IfReporter::reportShapeList("After sorting", &shapeList, TRUE);

    // Merge adjacent shapes to get a minimal list
    IfReporter::startReport("Merging shapes");
    IfMerger *merger = new IfMerger;
    merger->merge(shapeList);
    delete merger;
    IfReporter::finishReport();
    IfReporter::reportShapeList("After merging", &shapeList, TRUE);

    // Build a scene graph from the sorted list of shapes
    IfBuilder *builder = new IfBuilder;
    SoNode *resultRoot = builder->build(shapeList, doStrips, doVP,
					doNormals, doTexCoords);
    resultRoot->ref();
    delete builder;
    IfReporter::finishReport();

    // Weed out any unnecessary stuff. This works in place.
    IfReporter::startReport("Weeding unnecessary values");
    IfWeeder *weeder = new IfWeeder;
    weeder->weed(resultRoot);
    delete weeder;
    IfReporter::finishReport();

    IfReporter::reportNodeCount("Final graph", resultRoot);

    delete [] shapes;

    resultRoot->unrefNoDelete();
    return resultRoot;
}
