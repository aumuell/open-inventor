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
 |	This file defines the SoShapeHints node class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_SHAPE_HINTS_
#define  _SO_SHAPE_HINTS_

#include <Inventor/elements/SoGLShapeHintsElement.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/nodes/SoSubNode.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoShapeHints
//
//  This node is used to give hints about subsequent shapes defined in
//  a scene graph. It allows Inventor to provide or optimize certain
//  features (such as back-face culling and two-sided lighting) based
//  on information about shapes. The vertexOrdering, faceType, and
//  shapeType fields hold this information. There is also a crease
//  angle field, which is used when default normals are generated for
//  a polyhedral shape. It defines the smallest edge angle that will
//  be shaded as a crease (faceted), rather than as smooth.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: public=vertexOrdering,shapeType,faceType, creaseAngle
class SoShapeHints : public SoNode {

    SO_NODE_HEADER(SoShapeHints);

  public:
    // Hints about ordering of face vertices: if ordering of all
    // vertices of all faces is known to be consistent when viewed
    // from "outside" shape or not.
    enum VertexOrdering {
	UNKNOWN_ORDERING = SoShapeHintsElement::UNKNOWN_ORDERING,
	CLOCKWISE	 = SoShapeHintsElement::CLOCKWISE,
	COUNTERCLOCKWISE = SoShapeHintsElement::COUNTERCLOCKWISE
    };

    // Hints about entire shape: if shape is known to be a solid
    // object, as opposed to a surface.
    enum ShapeType {
	UNKNOWN_SHAPE_TYPE = SoShapeHintsElement::UNKNOWN_SHAPE_TYPE,
	SOLID		   = SoShapeHintsElement::SOLID
    };

    // Hints about faces of shape: if all faces are known to be convex
    // or not.
    enum FaceType {
	UNKNOWN_FACE_TYPE = SoShapeHintsElement::UNKNOWN_FACE_TYPE,
	CONVEX		  = SoShapeHintsElement::CONVEX
    };

    // Fields
    SoSFEnum		vertexOrdering;	// Ordering of face vertices
    SoSFEnum		shapeType;	// Info about shape geometry
    SoSFEnum		faceType;	// Info about face geometry
    SoSFFloat		creaseAngle;	// Smallest angle for sharp edge

    // Constructor
    SoShapeHints();

  SoEXTENDER public:
    virtual void	doAction(SoAction *action);
    virtual void	GLRender(SoGLRenderAction *action);
    virtual void	callback(SoCallbackAction *action);
    virtual void	pick(SoPickAction *action);
    virtual void	getBoundingBox(SoGetBoundingBoxAction *action);

  SoINTERNAL public:
    static void		initClass();

  protected:
    virtual ~SoShapeHints();

  private:
    // Does regular doAction stuff, also sets shapehints element
    void		doAction2(SoAction *action);
};

#endif /* _SO_SHAPE_HINTS_ */
