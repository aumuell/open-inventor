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
 * Copyright (C) 1990,91,92   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |	SbCylinderProjector
 |
 |   Author(s)		: Howard Look
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/projectors/SbCylinderProjector.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructors
//
// Use: protected
//
////////////////////////////////////////////////////////////////////////

SbCylinderProjector::SbCylinderProjector(SbBool orient)
: SbProjector()
{
    orientToEye = FALSE;
    setCylinder(SbCylinder(SbLine(SbVec3f(0,0,0), SbVec3f(0,1,0)), 1.0));
    setOrientToEye(orient);
    setFront(TRUE);
}

SbCylinderProjector::SbCylinderProjector(
    const SbCylinder &c,
    SbBool orient)
: SbProjector()
{
    orientToEye = FALSE;
    setCylinder(c);
    setOrientToEye(orient);
    setFront(TRUE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
// Misc set routines.
//
// Use: public
//
////////////////////////////////////////////////////////////////////////

void
SbCylinderProjector::setCylinder(const SbCylinder &c)
{
    cylinder = c;
    needSetup = TRUE;
}

void
SbCylinderProjector::setOrientToEye(SbBool b)
{
    if (orientToEye != b) {
        orientToEye = b;
        needSetup = TRUE;
    }	
}

void
SbCylinderProjector::setFront(SbBool b)
{
    intersectFront = b;
    needSetup = TRUE;
}

SbBool
SbCylinderProjector::isPointInFront(const SbVec3f &point ) const
{
    SbViewVolume viewVol = getViewVolume();
    SbBool       ptInFront = TRUE;

    SbVec3f closestPtOnAxis, axisPtToInput;

    closestPtOnAxis = cylinder.getAxis().getClosestPoint( point );
    axisPtToInput = point - closestPtOnAxis;

    if ( viewVol.getProjectionType() == SbViewVolume::PERSPECTIVE ) {
	SbVec3f lclProjPt;
	worldToWorking.multVecMatrix( viewVol.getProjectionPoint(), lclProjPt);
	SbVec3f cylPtToProj = lclProjPt - closestPtOnAxis;
	if ( axisPtToInput.dot( cylPtToProj ) < 0.0 ) 
	    ptInFront = FALSE;
    }
    else {
	SbVec3f lclZDir;
	worldToWorking.multDirMatrix( viewVol.zVector(), lclZDir );
	if ( axisPtToInput.dot( lclZDir ) < 0.0 ) 
	    ptInFront = FALSE;
    }
    return ptInFront;	
}

void
SbCylinderProjector::setWorkingSpace(const SbMatrix &space)
{
    SbProjector::setWorkingSpace(space);
    needSetup = TRUE;
}

SbVec3f
SbCylinderProjector::projectAndGetRotation(
    const SbVec2f &point, SbRotation &rot)
{
    SbVec3f oldPoint = lastPoint;
    SbVec3f newPoint = project(point);
    rot = getRotation(oldPoint, newPoint);
    return newPoint;
}

