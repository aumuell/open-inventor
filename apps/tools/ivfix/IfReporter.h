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

/////////////////////////////////////////////////////////////////////////////
//
// IfReporter class: this reports various status messages and other
// information during the processing of a scene
//
/////////////////////////////////////////////////////////////////////////////

#ifndef  _IF_REPORTER_
#define  _IF_REPORTER_

#include <Inventor/actions/SoCallbackAction.h>

class IfHolder;
class IfShapeList;
class SoNode;

class IfReporter {

  public:
    // Sets file to report to. Default is stderr.
    static void		setFile(FILE *reportFile) { fp = reportFile; }

    // Sets whether to report verbosely or not
    static void		setVerbose(SbBool flag)	{ verbose = flag; }

    // Sets whether to report details
    static void		setDetails(SbBool flag)	{ details = flag; }

    // Reports an operation that takes time. "Done" is printed when
    // done.
    static void		startReport(const char *msg, SbBool isDetail = FALSE);
    static void		finishReport(SbBool isDetail = FALSE);

    // Reports an index
    static void		reportIndex(const char *msg, int index, int total);

    // Reports a IfHolder
    static void		reportHolder(const char *msg, IfHolder *holder);

    // Reports a IfShapeList
    static void		reportShapeList(const char *msg,
					IfShapeList *shapeList,
					SbBool checkDiff = FALSE);

    // Reports on the number of nodes in the given graph
    static void		reportNodeCount(const char *msg, SoNode *root);

  private:
    static FILE		*fp;
    static SbBool	verbose;
    static SbBool	details;

    // Callback for reportNodeCount
    static SoCallbackAction::Response countCB(void *userData,
					      SoCallbackAction *,
					      const SoNode *)
	{ (* (int *) userData)++; return SoCallbackAction::CONTINUE; }
};

#endif /* _IF_REPORTER_ */
