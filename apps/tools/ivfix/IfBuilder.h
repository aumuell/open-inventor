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
// IfBuilder class: takes a sorted IfShapeList and builds a minimal scene
// graph that represents it.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef  _IF_BUILDER_
#define  _IF_BUILDER_

#include <Inventor/actions/SoCallbackAction.h>
#include "IfShapeList.h"

class IfHolder;
class IfShape;
class SoNode;
class SoSeparator;

class IfBuilder {

  public:
    IfBuilder();
    ~IfBuilder();

    SoNode *	build(const IfShapeList &shapeList, SbBool doStrips,
		      SbBool doVP, SbBool doAnyNormals, SbBool doAnyTexCoords);

  private:
    SbBool	doStrips;	
    SbBool	doVP;	
    SbBool	doAnyNormals;	
    SbBool	doAnyTexCoords;	
    SoSeparator	*roots[6];		// Roots at 6 levels of graph

    // Builds the roots from the given level down
    void	buildRoots(int startLevel, IfShape *shape);

    // Replaces all level-5 roots with the result of flattening
    void	replaceLevel5();

    // Traverses the given path with a callback action to determine if
    // normals and texture coordinates are required for shapes in it
    void	getFlags(SoPath *path, SbBool &doNormals, SbBool &doTexCoords);

    // This is called through the callback
    static void	setFlags(SoCallbackAction *cba, SbBool *flags);

    // Flattens the subgraph at the tail of the given path, returning
    // the resulting graph
    SoNode *	flatten(const SoPath *path,
			SbBool doNormals, SbBool doTexCoords);

    // Collects all properties along the given path (above the tail)
    // and returns a separator-rooted graph that contains all of them
    // and the tail of the path.
    static SoSeparator * collectObject(const SoPath *path);

    // Recursive procedure that removes any separators that have only
    // one child
    void	removeUnnecessarySeparators(SoSeparator *root, int level);

    // Converts a flattened scene graph to use an SoVertexProperty
    // node for its properties
    void	convertToVertexProperty(IfHolder *holder);

    // Callback for getFlags
    static SoCallbackAction::Response flagCB(void *userData,
					     SoCallbackAction *cba,
					     const SoNode *)
	{ setFlags(cba, (SbBool *) userData);
	  return SoCallbackAction::ABORT; }
};

#endif /* _IF_BUILDER_ */
