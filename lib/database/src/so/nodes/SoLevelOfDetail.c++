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
 |	SoLevelOfDetail
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/misc/SoState.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/elements/SoComplexityElement.h>
#include <Inventor/elements/SoComplexityTypeElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/nodes/SoLevelOfDetail.h>
#include <Inventor/nodes/SoShape.h>

SoGetBoundingBoxAction *SoLevelOfDetail::bboxAction = NULL;

SO_NODE_SOURCE(SoLevelOfDetail);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoLevelOfDetail::SoLevelOfDetail()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoLevelOfDetail);
    SO_NODE_ADD_FIELD(screenArea, (0));
    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor that takes approximate number of children.
//
// Use: public

SoLevelOfDetail::SoLevelOfDetail(int nChildren) : SoGroup(nChildren)
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoLevelOfDetail);
    SO_NODE_ADD_FIELD(screenArea, (0));
    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoLevelOfDetail::~SoLevelOfDetail()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements typical traversal, determining child to traverse
//    based on screen area.
//
// Use: extender

void
SoLevelOfDetail::doAction(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    int		childToTraverse;
    int		numKids  = getNumChildren();
    int		numAreas = screenArea.getNum();
    SoState	*state   = action->getState();

    // If no children or 1 child, decision is easy
    if (numKids == 0)
	return;

    if (numKids == 1 || numAreas == 0)
	childToTraverse = 0;

    // BOUNDING_BOX complexity means use the cheapest version
    else if (SoComplexityTypeElement::get(state) ==
	     SoComplexityTypeElement::BOUNDING_BOX)
	childToTraverse = numKids - 1;

    // Need to decide on a child based on screen area and complexity value
    else {
	SbVec2s	rectSize;
	float	complexity, projectedArea;
	int	i, numAreas = screenArea.getNum();

	// Compute the 3D bounding box of the children
	const SbViewportRegion &vpreg = SoViewportRegionElement::get(state);
	if (bboxAction == NULL)
	    bboxAction = new SoGetBoundingBoxAction(vpreg);
	else
	    bboxAction->setViewportRegion(vpreg);
	bboxAction->apply(this);

	// Compute the screen size of the bbox and its area
	SoShape::getScreenSize(state, bboxAction->getBoundingBox(), rectSize);
	projectedArea = rectSize[0] * rectSize[1];

	// Adjust the area based on the complexity
	complexity = SoComplexityElement::get(state);

	// For complexity less than or equal to the default (0.5), use
	// a linear ramp so that complexity 0 always gives the last
	// child (area = 0) and complexity 0.5 uses the area as is.
	if (complexity <= 0.5)
	    projectedArea *= 2.0 * complexity;

	// For complexity greater than the default, scale the area so
	// that at complexity = 1, the first (most detailed) child is
	// always used. Don't bother scaling the area if it won't make
	// it bigger, though
	else if (projectedArea < screenArea[0] && projectedArea > 0.0) {
	    float	ratio = screenArea[0] / projectedArea;
	    projectedArea *= (2.0 * (ratio - 1.0) * complexity) - ratio + 2.0;
	}

	for (i = 0; i < numAreas; i++)
	    if (projectedArea >= screenArea[i])
		break;

	// Make sure we didn't go off the deep end
	if (i >= numKids)
	    i = numKids - 1;

	childToTraverse = i;
    }

    // Traverse just the one kid
    children->traverse(action, childToTraverse, childToTraverse);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements callback action for levelOfDetail nodes.
//
// Use: extender

void
SoLevelOfDetail::callback(SoCallbackAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoLevelOfDetail::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Traversal for rendering. This uses the screen area comparison.
//
// Use: extender

void
SoLevelOfDetail::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoLevelOfDetail::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements ray picking.
//
// Use: extender

void
SoLevelOfDetail::rayPick(SoRayPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoLevelOfDetail::doAction(action);
}
