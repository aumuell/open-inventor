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
 |	This file defines the SoPointSet node class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_POINT_SET_
#define  _SO_POINT_SET_

#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/nodes/SoNonIndexedShape.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoPointSet
//
//  Set of points, using the current coordinates. The startIndex field
//  gives the starting coordinate index for the first point. The number of
//  points to draw is determined by the numPoints field.
//
//////////////////////////////////////////////////////////////////////////////

// This value, when used in the numPoints field, means use the rest of
// the coordinates as points
#define SO_POINT_SET_USE_REST_OF_POINTS	(-1)

// C-api: prefix=SoPtSet
// C-api: public=startIndex, numPoints, vertexProperty
class SoPointSet : public SoNonIndexedShape {

    SO_NODE_HEADER(SoPointSet);

  public:
    // Fields
    SoSFInt32		numPoints;	// Number of points to draw

    // Constructor
    SoPointSet();

  SoEXTENDER public:
    // Implements actions
    virtual void	GLRender(SoGLRenderAction *action);

    // Redefine this to tell open caches that they contain points
    virtual void	getBoundingBox(SoGetBoundingBoxAction *action);

  SoINTERNAL public:
    static void		initClass();

  protected:
    // Generates points representing point set
    virtual void	generatePrimitives(SoAction *action);

    // Computes bounding box of point set
    virtual void	computeBBox(SoAction *action, SbBox3f &box,
				    SbVec3f &center);

    // Overrides standard method to create an SoPointDetail instance
    virtual SoDetail *	createPointDetail(SoRayPickAction *action,
					  const SoPrimitiveVertex *v,
					  SoPickedPoint *pp);

  protected:
    virtual ~SoPointSet();

  private:
    // Returns TRUE if materials/normals are bound to individual points
    SbBool		areMaterialsPerPoint(SoAction *action) const;
    SbBool		areNormalsPerPoint(SoAction *action) const;
};

#endif /* _SO_POINT_SET_ */
