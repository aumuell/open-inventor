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
// IfCollector class: finds all the shapes in a scene graph and collects
// the properties assigned to each one. The results are stored in a
// list of IfShape instances.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef  _IF_COLLECTOR_
#define  _IF_COLLECTOR_

#include <Inventor/SoLists.h>
#include <Inventor/actions/SoCallbackAction.h>

class IfShape;
class SoNode;
class SoPath;
class SoVertexProperty;

class IfCollector {

  public:
    IfCollector();
    ~IfCollector();

    // Collects the shapes in the given scene. Returns the number of shapes.
    int		collect(SoNode *sceneRoot, IfShape *&shapeList,
			SbBool doTexCoords);

  private:
    IfShape	*currentShape;		// IfShape being collected
    SoPathList	pathsToShapes;		// Paths to shapes in scene
    SbBool	doingTexCoords;		// TRUE if tex coords will be produced

    // Stores a path to a shape found in the scene
    void	storePath(SoCallbackAction *cba);

    // Collects properties for a single shape
    void	collectShape(const SoPath *pathToShape, IfShape *shape);

    // Handles properties stored in a vertexProperty node
    void	handleVertexProperty(IfShape *shape, SoVertexProperty *vp);

    // Stores a node encountered during traversal of a path to a
    // shape. Returns a callback action response code.
    SoCallbackAction::Response	storeNode(SoCallbackAction *cba, SoNode *node);

    // Callbacks
    static SoCallbackAction::Response	storePathCB(void *userData,
						    SoCallbackAction *cba,
						    const SoNode *)
	{ ((IfCollector *) userData)->storePath(cba);
	  return SoCallbackAction::PRUNE; }

    static SoCallbackAction::Response	storeNodeCB(void *userData,
						    SoCallbackAction *cba,
						    const SoNode *node)
	{ return ((IfCollector *) userData)->storeNode(cba, (SoNode *) node); }
};

#endif /* _IF_COLLECTOR_ */
