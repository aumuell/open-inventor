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
 |	This file defines the SoLineDetail class.
 |
 |   Author(s)		: Thaddeus Beier, Dave Immel, Howard Look
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef _SO_LINE_DETAIL_
#define _SO_LINE_DETAIL_

#include <Inventor/details/SoPointDetail.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoLineDetail
//
//  Detail information about vertex-based shapes made of line
//  segments. It adds indices of various items that vary among lines,
//  segments, and vertices.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoLineDtl
class SoLineDetail : public SoDetail {

    SO_DETAIL_HEADER(SoLineDetail);

  public:
    // Constructor and destructor.
    SoLineDetail();
    virtual ~SoLineDetail();

    // These return the point details for the two ends of the line segment
    // C-api: name=getPt0
    const SoPointDetail *	getPoint0() const	{ return &point[0]; }
    // C-api: name=getPt1
    const SoPointDetail *	getPoint1() const	{ return &point[1]; }

    // Returns the index of the line the segment is part of within a
    // shape (e.g., the third line within an SoLineSet)
    // C-api: name=getLineInd
    int32_t			getLineIndex() const	{ return lineIndex; }

    // Returns the index of the part (usually part = segment) within a
    // shape (e.g., the fifth segment overall within an SoLineSet)
    // C-api: name=getPartInd
    int32_t			getPartIndex() const	{ return partIndex; }

    // Returns an instance that is a copy of this instance. The caller
    // is responsible for deleting the copy when done.
    virtual SoDetail *		copy() const;

  SoEXTENDER public:
    // Copies a point detail from the given detail
    void			setPoint0(const SoPointDetail *pd);
    void			setPoint1(const SoPointDetail *pd);

    // Sets the line index and part index
    void			setLineIndex(int32_t i)	{ lineIndex = i; }
    void			setPartIndex(int32_t i)	{ partIndex = i; }

#ifndef IV_STRICT
    void    setLineIndex(long i)				// System long
		{ setLineIndex ((int32_t) i); }
    void    setPartIndex(long i)				// System long
		{ setPartIndex ((int32_t) i); }
#endif

  SoINTERNAL public:
    static void			initClass();

  private:
    SoPointDetail		point[2];
    int32_t			lineIndex, partIndex;
};

#endif /* _SO_LINE_DETAIL_ */
