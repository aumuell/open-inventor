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
// IfFixer class. This is the main entry point into the ivfix code. It
// can be used in other programs to "fix" scene graphs.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef  _IF_FIXER_
#define  _IF_FIXER_

#include "IfShapeList.h"

class IfShape;
class SoMaterial;

class IfFixer {

  public:
    // Possible reporting levels
    enum ReportLevel {
	NONE,			// No reporting
	LOW,			// Minimal reporting
	HIGH			// Lots of reporting
    };

    IfFixer();
    ~IfFixer();

    // Sets the status reporting level and file. By default, no status
    // report is made
    void		setReportLevel(ReportLevel level, FILE *fp);

    // Sets flag indicating whether to output triangle strips (the
    // default) or independent faces
    void		setStripFlag(SbBool flag)	{ doStrips = flag; }

    // Sets flag indicating whether to output shape properties as
    // SoVertexProperty nodes (the default) or regular property nodes
    void		setVertexPropertyFlag(SbBool flag) { doVP = flag; }

    // Sets flags indicating whether to output normals or texture
    // coordinates. The default is TRUE in both cases, meaning that
    // normals and texture coordinates will be output when necessary.
    void		setNormalFlag(SbBool flag)	 { doNormals  = flag; }
    void		setTextureCoordFlag(SbBool flag) { doTexCoords= flag; }

    // Fixes a scene graph, returning the root of the result, or NULL
    // on error. If the passed root is not ref'ed, its memory will be
    // freed up before this finishes.
    SoNode *		fix(SoNode *root);

  private:
    SbBool		doStrips;
    SbBool		doVP;
    SbBool		doNormals;
    SbBool		doTexCoords;
};

#endif /* _IF_FIXER_ */
