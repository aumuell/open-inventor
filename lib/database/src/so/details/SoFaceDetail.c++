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
 |	SoFaceDetail
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/details/SoFaceDetail.h>

SO_DETAIL_SOURCE(SoFaceDetail)

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor.
//
// Use: public
//

SoFaceDetail::SoFaceDetail()
//
////////////////////////////////////////////////////////////////////////
{
    point = NULL;
    numPoints = 0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: public
//

SoFaceDetail::~SoFaceDetail()
//
////////////////////////////////////////////////////////////////////////
{
    if (point != NULL)
	delete [] point;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns an instance that is a copy of this instance. The caller
//    is responsible for deleting the copy when done.
//
// Use: public, virtual
//

SoDetail *
SoFaceDetail::copy() const
//
////////////////////////////////////////////////////////////////////////
{
    SoFaceDetail *newDetail = new SoFaceDetail;

    newDetail->faceIndex = faceIndex;
    newDetail->partIndex = partIndex;

    newDetail->setNumPoints(numPoints);
    for (int i = 0; i < numPoints; i++)
	newDetail->setPoint(i, &point[i]);

    return newDetail;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the number of points in the face and allocates room for the
//    points.
//
// Use: extender
//

void
SoFaceDetail::setNumPoints(int32_t num)
//
////////////////////////////////////////////////////////////////////////
{
    // Free up old ones if necessary
    if (point != NULL && numPoints < num) {
	delete [] point;
	point = NULL;	// So they get allocated below
    }

    // Allocate space if necessary
    if (point == NULL && num > 0)
	point = new SoPointDetail[num];

    numPoints = num;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Copies a point detail from the given detail.
//
// Use: extender
//

void
SoFaceDetail::setPoint(int32_t index, const SoPointDetail *pd)
//
////////////////////////////////////////////////////////////////////////
{
    point[index] = *pd;
}
