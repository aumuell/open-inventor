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
 |	This file defines the SoCylinder node class.
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_CYLINDER_
#define  _SO_CYLINDER_

#include <Inventor/SbLinear.h>
#include <Inventor/fields/SoSFBitMask.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/nodes/SoShape.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoCylinder
//
//  Simple cylinder, aligned with y-axis. Default size is -1 to +1 in
//  all 3 dimensions, but the radius and height fields can be used to
//  change that. There are essentially three parts to the cylinder:
//  the sides, the top face, and the bottom face (in that order).
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoCyl
// C-api: public=parts, radius, height
class SoCylinder : public SoShape {

    SO_NODE_HEADER(SoCylinder);

  public:

    enum Part {			// Cylinder parts
	SIDES	= 0x01,			// The tubular part
	TOP	= 0x02,			// The top circular face
	BOTTOM	= 0x04,			// The bottom circular face
	ALL	= 0x07			// All parts
    };

    // Fields
    SoSFBitMask		parts;		// Visible parts of cylinder
    SoSFFloat		radius;		// Radius in x and z dimensions
    SoSFFloat		height;		// Size in y dimension

    // Constructor
    SoCylinder();

    // Turns on/off a part of the cylinder. (Convenience functions)
    // C-api: begin
    void		addPart(SoCylinder::Part part);
    void		removePart(SoCylinder::Part part);

    // Returns whether a given part is on or off. (Convenience function)
    SbBool		hasPart(SoCylinder::Part part) const;

  SoEXTENDER public:
    // Implements actions
    virtual void	GLRender(SoGLRenderAction *action);
    virtual void	rayPick(SoRayPickAction *action);

  SoINTERNAL public:
    static void		initClass();

  protected:
    // Generates triangles representing a cylinder
    virtual void	generatePrimitives(SoAction *action);

    // Computes bounding box of cylinder
    virtual void	computeBBox(SoAction *action, SbBox3f &box,
				    SbVec3f &center);

    virtual ~SoCylinder();

  private:
    static SbVec2f	*coordsArray;	// Storage for ring coords
    static int		maxCoords;	// Current max num of coords

    // Computes number of sides and sections to use to represent
    // cylinder (based on complexity), then computes ring of x,z
    // coordinates around cylinder and stores in ringCoords.
    void		computeRing(SoAction *action,
				    int &numSides, int &numSections,
				    SbVec2f *&ringCoords) const;

    // Computes real radius and half-height
    void		getSize(float &rad, float &hHeight) const;

    // These render the cylinder
    void		GLRenderGeneric(SoGLRenderAction *action,
					SbBool sendNormals, SbBool doTextures);
    void		GLRenderNvertTnone(SoGLRenderAction *action);
};

#endif /* _SO_CYLINDER_ */
