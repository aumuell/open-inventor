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
// IfFlattener class: takes all the shapes in the scene graph
// represented by a IfHolder and flattens it into a bunch of
// triangles. The coordinates, normals, texture coordinates, and
// material indices are stored in the SoIndexedTriangleStripSet in the
// IfHolder.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef  _IF_FLATTENER_
#define  _IF_FLATTENER_

#include <Inventor/SbLinear.h>
#include <Inventor/actions/SoCallbackAction.h>

class IfHolder;
class SoNode;
class SoShape;

class IfFlattener {

  public:
    IfFlattener();
    ~IfFlattener();

    void	flatten(IfHolder *_holder);

  private:
    IfHolder	*holder;	// Holds most important stuff
    int		numVerts;	// Number of vertices stored
    SbMatrix	pointMatrix;	// For transforming points
    SbMatrix	normalMatrix;	// For transforming normals
    SbMatrix	textureMatrix;	// For transforming texture coords
    int		numMtlIndices;	// Number of material indices stored
    int		fieldSize;	// Number of entries allocated for fields

    // These hold pointers to the field value arrays (returned by
    // startEditing()) to make additions much faster:
    SbVec3f	*coordVals;
    SbVec3f	*normalVals;
    SbVec2f	*texCoordVals;
    int32_t	*mtlIndexVals;

    void	expandFields();

    void	prepareForShape(SoCallbackAction *cba, const SoShape *shape);
    void	addTriangle(SoCallbackAction *cba,
			    const SoPrimitiveVertex *verts[3]);
    void	finishShape(SoCallbackAction *cba, const SoShape *shape);

    void	addCoordinate(const SbVec3f &point);
    void	addNormal(const SbVec3f &normal);
    void	addMaterialIndex(int materialIndex); 
    void	addTextureCoordinate(const SbVec4f &texCoord);

    void	createGraph();

    //////////////////////////////////////////////////////////////////
    //
    // Callbacks

    static SoCallbackAction::Response	preShapeCB(void *userData,
						   SoCallbackAction *cba,
						   const SoNode *shape)
	{
	    ((IfFlattener *) userData)->
		prepareForShape(cba, (const SoShape *) shape);
	    return SoCallbackAction::CONTINUE;
	}

    static SoCallbackAction::Response	postShapeCB(void *userData,
						    SoCallbackAction *cba,
						    const SoNode *shape)
	{
	    ((IfFlattener *) userData)->
		finishShape(cba, (const SoShape *) shape);
	    return SoCallbackAction::CONTINUE;
	}

    static void	triangleCB(void *userData, SoCallbackAction *cba,
			   const SoPrimitiveVertex *v1,
			   const SoPrimitiveVertex *v2,
			   const SoPrimitiveVertex *v3)
	{
	    const SoPrimitiveVertex *v[3];
	    v[0] = v1;
	    v[1] = v2;
	    v[2] = v3;
	    ((IfFlattener *) userData)->addTriangle(cba, v);
	}
};

#endif /* _IF_FLATTENER_ */
