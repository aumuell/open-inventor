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

//  -*- C++ -*-

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Description:
 |	This file defines the base SoPickedPoint class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef _SO_PICKED_POINT_
#define _SO_PICKED_POINT_

#include <Inventor/SbLinear.h>
#include <Inventor/SbViewportRegion.h>
#include <Inventor/SoLists.h>

class SoDetail;
class SoGetMatrixAction;
class SoPath;
class SoState;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoPickedPoint
//
//  An SoPickedPoint represents a point on the surface of an object
//  that was picked by applying an SoRayPickAction to a scene. It
//  contains the point of intersection, the surface normal and texture
//  coordinates at that point, and other items.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoPickPt
class SoPickedPoint {

  public:
    // Copy constructor
    SoPickedPoint(const SoPickedPoint &pp);

    // Destructor
    ~SoPickedPoint();

    // Returns an instance that is a copy of this instance. The caller
    // is responsible for deleting the copy when done.
    SoPickedPoint *	copy() const;

    // These return the intersection point and surface normal in world
    // space, and the texture coordinates in image space:
    // C-api: name=getPt
    const SbVec3f &	getPoint() const	 { return worldPoint; }
    // C-api: name=getNorm
    const SbVec3f &	getNormal() const	 { return worldNormal; }
    // C-api: name=getTexCoords
    const SbVec4f &	getTextureCoords() const { return imageTexCoords; }

    // Returns the index into the current set of materials of the
    // material active at the intersection point. Note that if the
    // materials are interpolated between vertices, the index will
    // correspond to the material at one of the vertices.
    // C-api: name=getMtlInd
    int			getMaterialIndex() const { return materialIndex; }

    // Returns the path to the object that was intersected
    SoPath *		getPath() const		 { return path; }

    // Returns whether the intersection is actually on the geometry of
    // the character that was hit, as opposed to being on the bounding
    // box. The pick style affects this decision.
    // C-api: name=isOnGeom
    SbBool		isOnGeometry() const	 { return onGeometry; }

    //////////////////////
    //
    // The following methods all take a pointer to a node in the
    // returned path. If the node pointer is NULL (the default), the
    // information corresponding to the tail of the path is returned.
    //

    // Returns the detail that corresponds to the given node in the path
    // C-api: name=getDtl
    const SoDetail *	getDetail(const SoNode *node = NULL) const;

    // These return the transformation matrices between world space
    // and the object space corresponding to the given node in the path
    // C-api: name=getObjToWorld
    SbMatrix		getObjectToWorld(const SoNode *node = NULL) const;
    // C-api: name=getWorldToObj
    SbMatrix		getWorldToObject(const SoNode *node = NULL) const;

    // These return the texture transformation matrices between image
    // space and the object space corresponding to the given node in
    // the path
    // C-api: name=getObjToImage
    SbMatrix		getObjectToImage(const SoNode *node = NULL) const;
    // C-api: name=getImageToObj
    SbMatrix		getImageToObject(const SoNode *node = NULL) const;

    // These return the intersection point, surface normal, and
    // texture coordinates in the object space corresponding to the
    // given node in the path
    // C-api: name=getObjPt
    SbVec3f		getObjectPoint(const SoNode *node = NULL) const;
    // C-api: name=getObjNorm
    SbVec3f		getObjectNormal(const SoNode *node = NULL) const;
    // C-api: name=getObjTexCoords
    SbVec4f		getObjectTextureCoords(const SoNode *node=NULL) const;

    //
    //////////////////////

  SoEXTENDER public:

    // NOTE: these methods should be called ONLY by the node that
    // causes the SoPickedPoint instance to be created, and ONLY at
    // the time it is created. Setting or changing the normal or
    // texture coordinates at a later time may cause undefined results.

    // Sets the normal in object space
    void		setObjectNormal(const SbVec3f &normal);

    // Sets the texture coordinates in object space
    void		setObjectTextureCoords(const SbVec4f &texCoords);

    // Sets the material index
    void		setMaterialIndex(int index)  { materialIndex = index; }

    // Sets the detail corresponding to the given node in the path.
    // NULL may be passed to remove a detail. All detail pointers are
    // NULL by default.
    void		setDetail(SoDetail *detail, SoNode *node);

  SoINTERNAL public:

    // The constructor is internal since instances are created only by
    // the SoRayPickAction
    SoPickedPoint(const SoPath *path, SoState *state,
		  const SbVec3f &objSpacePoint);

  private:
    // This action is used to get the world-to-object matrix
    static SoGetMatrixAction	*matrixAction;

    // Intersection point and normal in world space, and texture
    // coordinates in image space
    SbVec3f		worldPoint;
    SbVec3f		worldNormal;
    SbVec4f		imageTexCoords;

    // ViewportRegion, which is needed when figuring out matrices
    SbViewportRegion	vpRegion;

    // Material index
    int			materialIndex;

    // TRUE if pick was on geometry of shape as opposed to bbox
    SbBool		onGeometry;

    // The path to the shape that was picked
    SoPath		*path;

    // Details corresponding to nodes in path, one per node in path.
    // Many may be NULL.
    SoDetailList	details;

    // The pointer to the state allows us to access matrices to
    // convert from object space to world and image space. This
    // pointer is valid only during construction and setting of the
    // info inside the instance.
    SoState		*state;

    // Returns index in path of given node, or -1 if not found
    int			getNodeIndex(const SoNode *node) const;

    // Applies SoGetMatrixAction to path to node
    void		getMatrix(const SoNode *node) const;

    // Multiplies SbVec4f by matrix - for transforming texture coords
    static SbVec4f multVecMatrix4(const SbMatrix &m, const SbVec4f v);
};

#endif /* _SO_PICKED_POINT_ */
