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
 |	This file defines the SoCube node class.
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_CUBE_
#define  _SO_CUBE_

#include <Inventor/SbLinear.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/nodes/SoShape.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoCube
//
//  Simple cube. Default size is -1 to +1 in all 3 dimensions, but the
//  width, height, and depth fields can be used to change these.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: public=width, height, depth
class SoCube : public SoShape {

    SO_NODE_HEADER(SoCube);

  public:
    // Fields
    SoSFFloat		width;		// Size in x dimension
    SoSFFloat		height;		// Size in y dimension
    SoSFFloat		depth;		// Size in z dimension

    // Constructor
    SoCube();

  SoEXTENDER public:
    // Implements actions
    virtual void	GLRender(SoGLRenderAction *action);
    virtual void	rayPick(SoRayPickAction *action);

  SoINTERNAL public:
    static void		initClass();

  protected:
    // Generates triangles representing a cube
    virtual void	generatePrimitives(SoAction *action);

    // Computes bounding box of cube
    virtual void	computeBBox(SoAction *action, SbBox3f &box,
				    SbVec3f &center);

    // Overrides standard method to create an SoCubeDetail instance
    virtual SoDetail *	createTriangleDetail(SoRayPickAction *action,
					     const SoPrimitiveVertex *v1,
					     const SoPrimitiveVertex *v2,
					     const SoPrimitiveVertex *v3,
					     SoPickedPoint *pp);

    virtual ~SoCube();

  private:
    static SbVec3f	coords[8];	// Corner coordinates
    static SbVec2f	texCoords[4];	// Face corner texture coordinates
    static SbVec3f	normals[6];	// Face normals
    static SbVec3f	edgeNormals[12];// Edge normals (for wire-frame)
    static const SbVec3f *verts[6][4];	// Vertex references to coords

    // This flag indicates whether picking is done on a real cube or a
    // cube that is just a bounding box representing another shape. If
    // this flag is TRUE, a pick on the cube should not generate a
    // detail, since the bounding box is not really in the picked path.
    SbBool		pickingBoundingBox;

    // Returns TRUE if per face materials are specified
    SbBool		isMaterialPerFace(SoAction *action) const;

    // Computes number of divisions per side based on complexity
    int			computeNumDivisions(SoAction *action) const;

    // Computes real half-width, -height, -depth
    void		getSize(float &hWidth,
				float &hHeight,
				float &hDepth) const;

    // These render the cube
    void		GLRenderGeneric(SoGLRenderAction *action,
					SbBool sendNormals, SbBool doTextures);
    void		GLRenderNvertTnone(SoGLRenderAction *action);

    // Renders or picks cube representing given bounding box. These
    // are used by SoShape to implement BOUNDING_BOX complexity.
    void		GLRenderBoundingBox(SoGLRenderAction *action,
					    const SbBox3f &bbox);
    void		rayPickBoundingBox(SoRayPickAction *action,
					   const SbBox3f &bbox);

    // SoShape needs to get at the above methods
friend class SoShape; 
};

#endif /* _SO_CUBE_ */
