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

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |	SoPickedPoint
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoPath.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/details/SoDetail.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoPickStyleElement.h>
#include <Inventor/elements/SoTextureMatrixElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/errors/SoDebugError.h>

SoGetMatrixAction *SoPickedPoint::matrixAction = NULL;

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor - internal since instances are created only by the
//    SoRayPickAction.
//
// Use: internal

SoPickedPoint::SoPickedPoint(const SoPath *_path, SoState *_state,
			     const SbVec3f &objSpacePoint)
//
////////////////////////////////////////////////////////////////////////
{
    int	i, n;

    // Make a copy of the path since it most likely comes from the
    // current traversal path in an action, which will be changed soon.
    path =  _path->copy();
    path->ref();

    // Save state so we can get matrices when we need them later
    state = _state;

    // Transform the object space point by the current modeling matrix
    // to get the world space point
    SoModelMatrixElement::get(state).multVecMatrix(objSpacePoint, worldPoint);

    // Make room in the detail list for one detail per node in the
    // path. Set all the detail pointers to NULL.
    n = ((const SoFullPath *) path)->getLength();
    details.set(n - 1, NULL);		// Allocates space
    for (i = n - 2; i >= 0; --i)
	details.set(i, NULL);

    // Initialize material index to 0, the most common value
    materialIndex = 0;

    // Set on-geometry flag based on current pick style
    onGeometry = (SoPickStyleElement::get(state) !=
		  SoPickStyleElement::BOUNDING_BOX);

    // Save the viewportRegion, we'll need it later:
    vpRegion = SoViewportRegionElement::get(state);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Copy constructor.
//
// Use: public

SoPickedPoint::SoPickedPoint(const SoPickedPoint &pp)
//
////////////////////////////////////////////////////////////////////////
{
    worldPoint	   = pp.worldPoint;
    worldNormal    = pp.worldNormal;
    imageTexCoords = pp.imageTexCoords;
    materialIndex  = pp.materialIndex;
    path	   = pp.path;
    onGeometry	   = pp.onGeometry;
    vpRegion	   = pp.vpRegion;

    // Copy the details - note that the copy() method for SoDetailList
    // makes copies of the detail instances. This has to be done
    // because the instances are deleted when the list is deleted.
    details = pp.details;

    // Ref the path, since we unref it when we are deleted
    path->ref();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: public

SoPickedPoint::~SoPickedPoint()
//
////////////////////////////////////////////////////////////////////////
{
    // Free up path
    path->unref();

    // The SoDetailList destructor deletes all details in the list, so
    // we don't have to do it here
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns an instance that is a copy of this instance. The caller
//    is responsible for deleting the copy when done.
//
// Use: public

SoPickedPoint *
SoPickedPoint::copy() const
//
////////////////////////////////////////////////////////////////////////
{
    SoPickedPoint *newCopy = new SoPickedPoint(*this);
    return newCopy;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the detail that corresponds to the given node in the path.
//
// Use: public

const SoDetail *
SoPickedPoint::getDetail(const SoNode *node) const
//
////////////////////////////////////////////////////////////////////////
{
    int	index;

    // Test for default case, corresponding to tail of path
    if (node == NULL)
	index = ((const SoFullPath *) path)->getLength() - 1;

    else
	index = getNodeIndex(node);

    return details[index];
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the transformation matrix to go from the object space
//    corresponding to the given node in the path to world space.
//
// Use: public

SbMatrix
SoPickedPoint::getObjectToWorld(const SoNode *node) const
//
////////////////////////////////////////////////////////////////////////
{
    getMatrix(node);
    return matrixAction->getMatrix();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the transformation matrix to go from world space to the
//    object space corresponding to the given node in the path.
//
// Use: public

SbMatrix
SoPickedPoint::getWorldToObject(const SoNode *node) const
//
////////////////////////////////////////////////////////////////////////
{
    getMatrix(node);
    return matrixAction->getInverse();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the transformation matrix to go from the object space
//    corresponding to the given node in the path to image space.
//
// Use: public

SbMatrix
SoPickedPoint::getObjectToImage(const SoNode *node) const
//
////////////////////////////////////////////////////////////////////////
{
    getMatrix(node);
    return matrixAction->getTextureMatrix();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the transformation matrix to go from image space to the
//    object space corresponding to the given node in the path.
//
// Use: public

SbMatrix
SoPickedPoint::getImageToObject(const SoNode *node) const
//
////////////////////////////////////////////////////////////////////////
{
    getMatrix(node);
    return matrixAction->getTextureInverse();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the intersection point in object space that corresponds
//    to the given node in the path.
//
// Use: public

SbVec3f
SoPickedPoint::getObjectPoint(const SoNode *node) const
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f	v;

    getWorldToObject(node).multVecMatrix(worldPoint, v);

    return v;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the surface normal in object space that corresponds to
//    the given node in the path.
//
// Use: public

SbVec3f
SoPickedPoint::getObjectNormal(const SoNode *node) const
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f	v;

    getWorldToObject(node).multDirMatrix(worldNormal, v);
    v.normalize();

    return v;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the texture coordinates in object space that corresponds
//    to the given node in the path.
//
// Use: public

SbVec4f
SoPickedPoint::getObjectTextureCoords(const SoNode *node) const
//
////////////////////////////////////////////////////////////////////////
{
    return multVecMatrix4(getImageToObject(node), imageTexCoords);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the object-space normal.
//
// Use: extender

void
SoPickedPoint::setObjectNormal(const SbVec3f &normal)
//
////////////////////////////////////////////////////////////////////////
{
    // Transform the object space normal by the current modeling
    // matrix o get the world space normal. Use the inverse transpose
    // of the odel matrix so that normals are not scaled incorrectly.
    SbMatrix normalMatrix =
	SoModelMatrixElement::get(state).inverse().transpose();

    normalMatrix.multDirMatrix(normal, worldNormal);
    worldNormal.normalize();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the object-space texture coordinates.
//
// Use: extender

void
SoPickedPoint::setObjectTextureCoords(const SbVec4f &texCoords)
//
////////////////////////////////////////////////////////////////////////
{
    // Transform the object space coords by the current texture matrix
    // to get the image space texture coords
    imageTexCoords = multVecMatrix4(SoTextureMatrixElement::get(state),
				    texCoords);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the detail corresponding to the given node in the path.
//    NULL may be passed to remove a detail.
//
// Use: extender

void
SoPickedPoint::setDetail(SoDetail *detail, SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
    int	i;

    // Find node in path
    i = getNodeIndex(node);

#ifdef DEBUG
    if (i < 0)
	SoDebugError::post("SoPickedPoint::setDetail",
			   "Node %#x is not found in path", node);
#endif /* DEBUG */

    details.set(i, detail);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns index in path of given node, or -1 if not found.
//
// Use: private

int
SoPickedPoint::getNodeIndex(const SoNode *node) const
//
////////////////////////////////////////////////////////////////////////
{
    int	i;

    // Search from bottom up for node in path, since details are
    // usually examined near the bottom
    for (i = ((const SoFullPath *) path)->getLength() - 1; i >= 0; i--)
	if (path->getNode(i) == node)
	    return i;

#ifdef DEBUG
    SoDebugError::post("SoPickedPoint::getNodeIndex",
		       "Node %#x is not found in path", node);
#endif /* DEBUG */

    return -1;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Applies given instance of SoGetMatrixAction to path to node.
//
// Use: private

void
SoPickedPoint::getMatrix(const SoNode *node) const
//
////////////////////////////////////////////////////////////////////////
{
    SoPath *xfPath;

    // Construct a path from the root down to this node. Use the given
    // path if it's the same
    if (node == NULL || node == ((SoFullPath *)path)->getTail())
	xfPath = path;

    else {
	int	index = getNodeIndex(node);
	xfPath = path->copy(0, index + 1);
	xfPath->ref();
    }

    // Create an action instance if necessary, then apply it to the path
    if (matrixAction == NULL)
	matrixAction = new SoGetMatrixAction(vpRegion);
    matrixAction->apply(xfPath);

    if (xfPath != path)
	xfPath->unref();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Multiplies SbVec4f by matrix - for transforming texture coords.
//
// Use: private, static

SbVec4f
SoPickedPoint::multVecMatrix4(const SbMatrix &m, const SbVec4f v)
//
////////////////////////////////////////////////////////////////////////
{
    int		i;
    SbVec4f	v2;

    for (i = 0; i < 4; i++)
	v2[i] = (v[0] * m[0][i] +
		 v[1] * m[1][i] +
		 v[2] * m[2][i] +
		 v[3] * m[3][i]);

    return v2;
}
