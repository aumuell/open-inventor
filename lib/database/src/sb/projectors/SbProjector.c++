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
 |	SbProjector
 |
 |   Author(s)		: Howard Look
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/projectors/SbProjector.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////
//
// Class:
//    SbProjector
//
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructors
//
// Use: protected
//
////////////////////////////////////////////////////////////////////////

SbProjector::SbProjector()
{
    setWorkingSpace(SbMatrix::identity());
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Set the view volume.
//
// Use: virtual public
void
SbProjector::setViewVolume(const SbViewVolume &vol)
//
////////////////////////////////////////////////////////////////////////
{
    viewVol = vol;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Set the working space.
//
// Use: virtual public
void
SbProjector::setWorkingSpace(const SbMatrix &space)
//
////////////////////////////////////////////////////////////////////////
{
    workingToWorld = space;
    worldToWorking = workingToWorld.inverse();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//  Given this mouse point, return the line it projects to
//  in working space.
//
// Use: virtual public
SbLine
SbProjector::getWorkingLine(const SbVec2f &point) const
//
////////////////////////////////////////////////////////////////////////
{
    // First project the line to the world space line, then tranform
    // that line into working space.

    SbLine worldLine;
    viewVol.projectPointToLine(point, worldLine);

    SbLine workingLine;
    worldToWorking.multLineMatrix(worldLine, workingLine);

    return workingLine;
}


