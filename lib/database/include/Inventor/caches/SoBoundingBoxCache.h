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
 |	This file defines the SoBoundingBoxCache class, which is used for
 |	storing caches during GL rendering.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

// C-api: end

#ifndef  _SO_BOUNDING_BOX_CACHE
#define  _SO_BOUNDING_BOX_CACHE

#include <Inventor/SbBox.h>
#include <Inventor/caches/SoCache.h>

////////////////////////////////////////////////////////////////////////
//
//  Class SoBoundingBoxCache:
//
//  A bounding box cache stores a bounding box and center point. It
//  also stores a flag indicating whether the box contains any line or
//  point objects, meaning that intersection tests with the box should
//  use a little extra leeway.
//
////////////////////////////////////////////////////////////////////////

SoEXTENDER class SoBoundingBoxCache : public SoCache {

  public:
    // Constructor and destructor
    SoBoundingBoxCache(SoState *state);
    ~SoBoundingBoxCache();

    // Stores a bounding box and center point in the cache
    void		set(const SbXfBox3f &boundingBox,
			    SbBool centerSet,
			    const SbVec3f &centerPoint);

    const SbXfBox3f &	getBox() const		{ return box; }

    const SbBox3f &	getProjectedBox() const	{ return projectedBox; }

    // Returns TRUE if the center is valid.
    SbBool		isCenterSet() const	{ return centerSet; }
	

    // Returns the center (in object space)
    const SbVec3f &	getCenter() const	{ return center; }

    // Sets the hasLinesOrPoints flag to TRUE in all open bounding box
    // caches in the given state. (The flag is FALSE by default.)
    static void		setHasLinesOrPoints(SoState *state);

    // Returns the hasLinesOrPoints flag
    SbBool		hasLinesOrPoints() const	 { return hasLOrP; }

  private:
    SbXfBox3f		box;		// Bounding box
    SbBox3f		projectedBox;	// Box in local space
    SbBool		centerSet;	// If the center was set
    SbVec3f		center;		// Center point
    SbBool		hasLOrP;	// TRUE if contains lines or points
};

#endif /* _SO_BOUNDING_BOX_CACHE */
