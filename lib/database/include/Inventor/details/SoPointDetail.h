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
 |	This file defines the SoPointDetail class.
 |
 |   Author(s)		: Thaddeus Beier, Dave Immel, Howard Look
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef _SO_POINT_DETAIL_
#define _SO_POINT_DETAIL_

#include <Inventor/details/SoSubDetail.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoPointDetail
//
//  Detail information about vertex-based shapes made of points. It
//  stores indices of various items that vary from point to point.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoPtDtl
class SoPointDetail : public SoDetail {

    SO_DETAIL_HEADER(SoPointDetail);

  public:
    // Constructor and destructor
    SoPointDetail();
    virtual ~SoPointDetail();

    // Returns the index of the point in the relevant coordinate node
    // C-api: name=getCoordInd
    int32_t	getCoordinateIndex() const	{ return coordIndex; }

    // These returns the index of the material, normal, and texture
    // coordinates for the point. Note that they may not always be
    // valid indices into nodes (e.g., if normals or texture
    // coordinates are generated automatically).
    // C-api: name=getMtlInd
    int32_t	getMaterialIndex() const	{ return materialIndex; }
    // C-api: name=getNormInd
    int32_t	getNormalIndex() const		{ return normalIndex; }
    // C-api: name=getTexCoordInd
    int32_t	getTextureCoordIndex() const	{ return texCoordIndex; }

    // Returns an instance that is a copy of this instance. The caller
    // is responsible for deleting the copy when done.
    virtual SoDetail *	copy() const;

  SoEXTENDER public:
    // Each of these sets one of the indices in the detail
    void	setCoordinateIndex(int32_t i)	{ coordIndex	= i; }
    void	setMaterialIndex(int32_t i)	{ materialIndex	= i; }
    void	setNormalIndex(int32_t i)		{ normalIndex	= i; }
    void	setTextureCoordIndex(int32_t i)	{ texCoordIndex	= i; }

#ifndef IV_STRICT
     void      setCoordinateIndex(long i)			// System long
			{ setCoordinateIndex ((int32_t) i); }
     void      setMaterialIndex(long i)				// System long
			{ setMaterialIndex ((int32_t) i); }
     void      setNormalIndex(long i)				// System long
			{ setNormalIndex ((int32_t) i); }
     void      setTextureCoordIndex(long i)			// System long
			{ setTextureCoordIndex ((int32_t) i); }
#endif

  SoINTERNAL public:
    static void	initClass();

  private:
    int32_t	coordIndex, materialIndex, normalIndex, texCoordIndex;
};

#endif /* _SO_POINT_DETAIL_ */
