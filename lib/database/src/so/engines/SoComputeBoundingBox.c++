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
 |      SoComputeBoundingBox
 |
 |   Author(s)          : Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/engines/SoComputeBoundingBox.h>

SO_ENGINE_SOURCE(SoComputeBoundingBox);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoComputeBoundingBox::SoComputeBoundingBox()
//
////////////////////////////////////////////////////////////////////////
{
    SO_ENGINE_CONSTRUCTOR(SoComputeBoundingBox);

    SO_ENGINE_ADD_INPUT(node, (NULL));
    SO_ENGINE_ADD_INPUT(path, (NULL));

    SO_ENGINE_ADD_OUTPUT(min,		SoSFVec3f);
    SO_ENGINE_ADD_OUTPUT(max,		SoSFVec3f);
    SO_ENGINE_ADD_OUTPUT(boxCenter,	SoSFVec3f);
    SO_ENGINE_ADD_OUTPUT(objectCenter,	SoSFVec3f);

    isBuiltIn = TRUE;

    // Create the action only when necessary
    bba = NULL;

    // Assume default NULL values
    gotNode = gotPath = FALSE;

    // Disable all of our outputs
    min.enable(FALSE);
    max.enable(FALSE);
    boxCenter.enable(FALSE);
    objectCenter.enable(FALSE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoComputeBoundingBox::~SoComputeBoundingBox()
//
////////////////////////////////////////////////////////////////////////
{
    if (bba != NULL)
	delete bba;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets viewport region to use for bounding box computation.
//
// Use: public

void
SoComputeBoundingBox::setViewportRegion(const SbViewportRegion &vpReg)
//
////////////////////////////////////////////////////////////////////////
{
    // Set the viewport region in the SoGetBoundingBoxAction. If we
    // don't have an action instance yet, create one.
    if (bba == NULL)
	bba = new SoGetBoundingBoxAction(vpReg);
    else
	bba->setViewportRegion(vpReg);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns viewport region used for bounding box computation.
//
// Use: public

const SbViewportRegion &
SoComputeBoundingBox::getViewportRegion() const
//
////////////////////////////////////////////////////////////////////////
{
    // Get the viewport region from the SoGetBoundingBoxAction. If we
    // don't have an action instance yet, create one. Cast away const
    // to do this.
    if (bba == NULL) {
	SoComputeBoundingBox	*cbb = (SoComputeBoundingBox *) this;
	cbb->bba = new SoGetBoundingBoxAction(SbViewportRegion());
    }

    return bba->getViewportRegion();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Detects when the inputs changed. Outputs are enabled if either
//    of the inputs is non-NULL.
//
// Use: protected

void
SoComputeBoundingBox::inputChanged(SoField *whichInput)
//
////////////////////////////////////////////////////////////////////////
{
    // See if the changed pointer is NULL
    if (whichInput == &node)
	gotNode = (node.getValue() != NULL);
    else
	gotPath = (path.getValue() != NULL);

    // If either input is now non-NULL, enable the outputs
    SbBool	enable = (gotNode || gotPath);
    min.enable(enable);
    max.enable(enable);
    boxCenter.enable(enable);
    objectCenter.enable(enable);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Evaluation routine
//
// Use: private

void
SoComputeBoundingBox::evaluate()
//
////////////////////////////////////////////////////////////////////////
{
    // Create an action instance if necessary
    if (bba == NULL)
	bba = new SoGetBoundingBoxAction(SbViewportRegion());

    // This should be called only when the outputs are enabled, so we
    // know that we have a non-NULL node or path
    if (gotPath)
	bba->apply(path.getValue());
    else
	bba->apply(node.getValue());

    SbBox3f	box = bba->getBoundingBox();
    SbVec3f	boxCent = box.getCenter();
    SbVec3f	objCent = bba->getCenter();

    SO_ENGINE_OUTPUT(min,    	   SoSFVec3f, setValue(box.getMin()));
    SO_ENGINE_OUTPUT(max,    	   SoSFVec3f, setValue(box.getMax()));
    SO_ENGINE_OUTPUT(boxCenter,    SoSFVec3f, setValue(boxCent));
    SO_ENGINE_OUTPUT(objectCenter, SoSFVec3f, setValue(objCent));
}
