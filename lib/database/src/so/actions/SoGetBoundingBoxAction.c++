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
 |   $Revision: 1.2 $
 |
 |   Classes:
 |	SoGetBoundingBoxAction
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/elements/SoBBoxModelMatrixElement.h>
#include <Inventor/elements/SoLocalBBoxMatrixElement.h>
#include <Inventor/elements/SoViewingMatrixElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/misc/SoState.h>

SO_ACTION_SOURCE(SoGetBoundingBoxAction);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoGetBoundingBoxAction::SoGetBoundingBoxAction(const SbViewportRegion &region)
//
////////////////////////////////////////////////////////////////////////
{
    SO_ACTION_CONSTRUCTOR(SoGetBoundingBoxAction);

    vpRegion = region;

    resetPath = NULL;
    resetBefore = FALSE;
    resetWhat = ALL;

    // empty the bounding box
    box.makeEmpty();

    // By default, we use world space even if a camera is present
    setInCameraSpace(FALSE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: public

SoGetBoundingBoxAction::~SoGetBoundingBoxAction()
//
////////////////////////////////////////////////////////////////////////
{
    if (resetPath != NULL)
	resetPath->unref();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the current viewport region.
//
// Use: public

void
SoGetBoundingBoxAction::setViewportRegion(const SbViewportRegion &newRegion)
//
////////////////////////////////////////////////////////////////////////
{
    vpRegion = newRegion;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Set the path to do a reset transform before or after.
//
// Use: public

void
SoGetBoundingBoxAction::setResetPath(const SoPath *path,
				     SbBool _resetBefore,
				     ResetType _what )
//
////////////////////////////////////////////////////////////////////////
{
    if (resetPath != NULL)
	resetPath->unref();

    resetPath = path;

    if (resetPath != NULL)
	resetPath->ref();

    resetBefore = _resetBefore;
    resetWhat = _what;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Do a reset transform if the conditions are met
//
// Use: extender

void
SoGetBoundingBoxAction::checkResetBefore()
//
////////////////////////////////////////////////////////////////////////
{
    checkReset(TRUE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Do a reset transform if the conditions are met
//
// Use: extender

void
SoGetBoundingBoxAction::checkResetAfter()
//
////////////////////////////////////////////////////////////////////////
{
    checkReset(FALSE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Do a reset transform if the conditions are met
//
// Use: public

void
SoGetBoundingBoxAction::checkReset(SbBool _resetBefore)
//
////////////////////////////////////////////////////////////////////////
{
    if (resetPath == NULL || _resetBefore != resetBefore)
	return;

    const SoPath *curPath = getCurPath();

    if (*curPath == *resetPath) {

	// Reset the transform if necessary. This has the side effect
	// of clearing out (to identity) all currently open local
	// matrices (instances of SoLocalBBoxMatrixElement). This is
	// necessary because separators above the current node should
	// NOT apply their local matrices to whatever we calculate
	// here, since we reset the transformation.
	if (resetWhat & TRANSFORM)
	    SoBBoxModelMatrixElement::reset(state, curPath->getTail());

	// empty the bounding box if necessary
	if (resetWhat & BBOX)
	    box.makeEmpty();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Gets the bounding box "center".  Different shapes may specify
//    that their center should not be considered the center of their
//    bounding box (e.g. text might be centered on its baseline, which
//    wouldn't be the center of its bounding box).
//
// Use: public

const SbVec3f &
SoGetBoundingBoxAction::getCenter() const
//
////////////////////////////////////////////////////////////////////////
{
    return center;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Used by shapes to set their preferred center.
//
// Use: extender

void
SoGetBoundingBoxAction::setCenter(const SbVec3f &c, SbBool transformCenter)
//
///////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (centerSet)
	SoDebugError::postWarning("SoGetBoundingBoxAction::setCenter",
				  "overriding a previously set center; some "
				  "group class is not doing its job!");
#endif

    if (transformCenter) {
	SbMatrix m = SoLocalBBoxMatrixElement::get(getState());

	if (isInCameraSpace())
	    m.multRight(SoViewingMatrixElement::get(getState()));

	m.multVecMatrix(c, center);
    }
    else
	center = c;

    centerSet = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if center has been set. Used by groups so they can
//    properly average the centers of their children.
//
// Use: extender

SbBool
SoGetBoundingBoxAction::isCenterSet() const
//
////////////////////////////////////////////////////////////////////////
{
    return centerSet;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Resets the center to the origin and unsets the centerSet flag.
//    This is used by groups so they can properly average the centers
//    of their children.
//
// Use: extender

void
SoGetBoundingBoxAction::resetCenter()
//
////////////////////////////////////////////////////////////////////////
{
    centerSet = FALSE;
    center.setValue(0.0, 0.0, 0.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Extends the current bounding box.
//
// Use: extender

void
SoGetBoundingBoxAction::extendBy(const SbBox3f &b)
//
///////////////////////////////////////////////////////////////////////
{
    SbXfBox3f	xfb = b;
    SbMatrix	m = SoLocalBBoxMatrixElement::get(getState());

    if (isInCameraSpace())
	m.multRight(SoViewingMatrixElement::get(getState()));

    xfb.transform(m);

    box.extendBy(xfb);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Extends the current bounding box.
//
// Use: extender

void
SoGetBoundingBoxAction::extendBy(const SbXfBox3f &b)
//
///////////////////////////////////////////////////////////////////////
{
    SbXfBox3f	xfb = b;
    SbMatrix	m = SoLocalBBoxMatrixElement::get(getState());

    if (isInCameraSpace())
	m.multRight(SoViewingMatrixElement::get(getState()));

    xfb.transform(m);

    box.extendBy(xfb);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initiates action on a graph.
//
// Use: protected

void
SoGetBoundingBoxAction::beginTraversal(SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
    SoViewportRegionElement::set(state, vpRegion);

    box.makeEmpty();
    resetCenter();

    traverse(node);
}
