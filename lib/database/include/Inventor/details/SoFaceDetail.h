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
 |   $Revision: 1.1 $
 |
 |   Description:
 |	This file defines the SoFaceDetail class.
 |
 |   Author(s)		: Thaddeus Beier, Dave Immel, Howard Look
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef _SO_FACE_DETAIL_
#define _SO_FACE_DETAIL_

#include <Inventor/details/SoPointDetail.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoFaceDetail
//
//  Detail information about vertex-based shapes made of faces. It
//  adds indices of various items that vary among faces and vertices.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoFaceDtl
class SoFaceDetail : public SoDetail {

    SO_DETAIL_HEADER(SoFaceDetail);

  public:
    // Constructor and destructor.
    SoFaceDetail();
    virtual ~SoFaceDetail();

    // Returns the number of points in the face
    // C-api: name=getNumPts
    int32_t			getNumPoints() const	{ return numPoints; }

    // Returns the point detail for the indexed point of the face
    // C-api: name=getPt
    const SoPointDetail *	getPoint(int i) const	{ return &point[i]; }

    // Returns the index of the face within a shape
    // C-api: name=getFaceInd
    int32_t			getFaceIndex() const	{ return faceIndex; }

    // Returns the index of the part within a shape
    // C-api: name=getPartInd
    int32_t			getPartIndex() const	{ return partIndex; }

    // Returns an instance that is a copy of this instance. The caller
    // is responsible for deleting the copy when done.
    virtual SoDetail *		copy() const;

  SoEXTENDER public:
    // Sets the number of points in the face and allocates room for the points
    void			setNumPoints(int32_t num);

    // Copies a point detail from the given detail
    void			setPoint(int32_t index, const SoPointDetail *pd);

    // Sets the face index and part index
    void			setFaceIndex(int32_t i)	{ faceIndex = i; }
    void			setPartIndex(int32_t i)	{ partIndex = i; }

    // Return a pointer to the point details.
    SoPointDetail *		getPoints()		{ return &point[0]; }

#ifndef IV_STRICT
    void    setNumPoints(long num)				// System long
		{ setNumPoints ((int32_t) num); }
    void    setPoint(long index, const SoPointDetail *pd)	// System long
		{ setPoint ((int32_t) index, pd); }
    void    setFaceIndex(long i)				// System long
		{ setFaceIndex ((int32_t) i); }
    void    setPartIndex(long i)				// System long
		{ setPartIndex ((int32_t) i); }
#endif

  SoINTERNAL public:
    static void			initClass();

  private:
    SoPointDetail		*point;
    int32_t			numPoints;
    int32_t			faceIndex, partIndex;
};

#endif /* _SO_FACE_DETAIL_ */
