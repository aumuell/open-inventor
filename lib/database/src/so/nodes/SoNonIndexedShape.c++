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
 |      SoNonIndexedShape
 |
 |   Author(s)          : Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/elements/SoCoordinateElement.h>
#include <Inventor/nodes/SoNonIndexedShape.h>

SO_NODE_ABSTRACT_SOURCE(SoNonIndexedShape);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: protected

SoNonIndexedShape::SoNonIndexedShape()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoNonIndexedShape);

    SO_NODE_ADD_FIELD(startIndex, (0));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoNonIndexedShape::~SoNonIndexedShape()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This is a convenience method to simplify the job of computing
//    bounding boxes for subclasses; it can be called from a
//    subclass's computeBBox() method. It sets the given bounding box
//    to contain all vertices of the shape, assuming that the shape
//    uses the numVertices coordinates beginning at the value in
//    startIndex. (If numVertices is negative, it uses all coordinates
//    from startIndex on.) It also sets the center to the average of
//    the vertices' coordinates.
//
// Use: protected

void
SoNonIndexedShape::computeCoordBBox(SoAction *action, int numVertices,
				    SbBox3f &box, SbVec3f &center)
//
////////////////////////////////////////////////////////////////////////
{
    int32_t			i, lastIndex;
    const SoCoordinateElement	*ce = NULL;
    const SbVec3f 		*vpCoords = NULL;

    SoVertexProperty *vp = (SoVertexProperty *)vertexProperty.getValue();
    if (vp && vp->vertex.getNum() > 0) {
	vpCoords = vp->vertex.getValues(0);
    } else {
	ce = SoCoordinateElement::getInstance(action->getState());
    }

    // Start with an empty box and zero sum
    center.setValue(0.0, 0.0, 0.0);
    box.makeEmpty();

    // Loop through coordinates, keeping max bounding box and sum of coords
    i = startIndex.getValue();
    if (numVertices < 0) {
	lastIndex = (ce ? ce->getNum() - 1 : vp->vertex.getNum() - 1);
	numVertices = (int) (lastIndex - i + 1);
    }
    else
	lastIndex = i + numVertices - 1;

    while (i <= lastIndex) {

	const SbVec3f &v = (ce ? ce->get3((int) i) : vpCoords[i]);

	box.extendBy(v);
	center += v;

	i++;
    }

    // The center point is the average of the vertices
    center /= (float) numVertices;
}

