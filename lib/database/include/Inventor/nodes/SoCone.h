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
 |	This file defines the SoCone node class.
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_CONE_
#define  _SO_CONE_

#include <Inventor/SbLinear.h>
#include <Inventor/fields/SoSFBitMask.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/nodes/SoShape.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoCone
//
//  Simple cone, aligned with y-axis. Default has apex point at y = 1,
//  bottom radius 1, and height 2, so that the size is -1 to +1 in all
//  3 dimensions. There are two parts to the cone: the sides and the
//  bottom face (in that order).
//
//////////////////////////////////////////////////////////////////////////////

// C-api: public=parts, bottomRadius, height
class SoCone : public SoShape {

    SO_NODE_HEADER(SoCone);

  public:

    enum Part {			// Cone parts:
	SIDES	= 0x01,			// The conical part
	BOTTOM	= 0x02,			// The bottom circular face
	ALL	= 0x03		 	// All parts
    };

    // Fields
    SoSFBitMask		parts;		// Visible parts of cone
    SoSFFloat		bottomRadius;	// Radius of bottom circular face
    SoSFFloat		height;		// Size in y dimension

    // Constructor
    SoCone();

    // Turns on/off a part of the cone. (Convenience functions)
    void		addPart(SoCone::Part part);
    void		removePart(SoCone::Part part);

    // Returns whether a given part is on or off. (Convenience function)
    SbBool		hasPart(SoCone::Part part) const;

  SoEXTENDER public:
    // Implements actions
    virtual void	GLRender(SoGLRenderAction *action);
    virtual void	rayPick(SoRayPickAction *action);

  SoINTERNAL public:
    static void		initClass();

  protected:
    // Generates triangles representing a cone
    virtual void	generatePrimitives(SoAction *action);

    // Computes bounding box of cone
    virtual void	computeBBox(SoAction *action, SbBox3f &box,
				    SbVec3f &center);
    virtual ~SoCone();

  private:
    static SbVec2f	*coordsArray;	// Storage for base coords
    static SbVec3f	*normalsArray;	// Storage for base normals
    static int		maxCoords;	// Current max num of coords/normals

    // Computes number of sides and sections to use to represent
    // cone (based on complexity), then computes ring of x,z
    // coordinates around base of cone and stores in baseCoords. It
    // computes and stores normals in sideNormals, too.
    void		computeBase(SoAction *action,
				    int &numSides, int &numSections,
				    SbVec2f *&baseCoords,
				    SbVec3f *&sideNormals) const;

    // Computes real radius and half-height
    void		getSize(float &rad, float &hHeight) const;

    // These render the cone
    void		GLRenderGeneric(SoGLRenderAction *action,
					SbBool sendNormals, SbBool doTextures);
    void		GLRenderNvertTnone(SoGLRenderAction *action);

    // Computes intersection of ray with infinite canonical cone
    SbBool		intersectInfiniteCone(const SbLine &ray,
					      SbVec3f &enterPoint,
					      SbVec3f &exitPoint) const;
};

// C-api: begin
#endif /* _SO_CONE_ */
