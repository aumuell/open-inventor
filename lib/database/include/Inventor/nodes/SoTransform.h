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
 |	This file defines the SoTransform node class.
 |
 |   Author(s)		: Paul S. Strauss, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_TRANSFORM_
#define  _SO_TRANSFORM_

#include <Inventor/fields/SoSFRotation.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/nodes/SoTransformation.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoTransform
//
//  Geometric transformation node.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoXf
// C-api: public= translation, rotation, scaleFactor, scaleOrientation, center
class SoTransform : public SoTransformation {

    SO_NODE_HEADER(SoTransform);

  public:
    // Fields
    SoSFVec3f		translation;	 // Translation vector
    SoSFRotation	rotation;	 // Rotation
    SoSFVec3f		scaleFactor;	 // Scale factors
    SoSFRotation	scaleOrientation;// Defines rotational space for scale
    SoSFVec3f		center;	         // Center point for scale and rotate

    // Constructor
    SoTransform();

    // Sets the transform to translate the origin to the fromPoint and
    // rotate the negative z-axis (0,0,-1) to lie on the vector from
    // fromPoint to toPoint. This always tries to keep the "up"
    // direction the positive y-axis, unless that is impossible.
    // All current info in the node is wiped out.
    // C-api: begin
    // C-api: name=ptAt
    void		pointAt(const SbVec3f &fromPoint,
				const SbVec3f &toPoint);

    // returns composite matrices that take you from object space to each
    // of scale/rotation/translation spaces
    // C-api: name=getScaleSpaceMx
    void		getScaleSpaceMatrix(SbMatrix &mat, SbMatrix &inv) const;
    // C-api: name=getRotSpaceMx
    void		getRotationSpaceMatrix(SbMatrix &mat,
                                               SbMatrix &inv) const;
    // C-api: name=getXlateSpaceMx
    void		getTranslationSpaceMatrix(SbMatrix &mat,
                                                  SbMatrix &inv) const;

    // Convenience functions that combine the effects of a matrix
    // transformation into the transformation stored in this node. The
    // first method premultiplies the transformation and the second
    // postmultiplies it.
    void		multLeft(const SbMatrix &mat);
    void		multRight(const SbMatrix &mat);

    // These are the same as multLeft() and multRight(), except that
    // the transformation to multiply into this node comes from
    // another transformation node.
    void		combineLeft(SoTransformation *nodeOnLeft);
    void		combineRight(SoTransformation *nodeOnRight);

    // Sets the fields in the node to implement the transformation
    // represented by the given matrix
    // C-api: name=setMx
    void		setMatrix(const SbMatrix &mat);

    // Changes the center of the transformation to the given point
    // without affecting the overall effect of the transformation
    void		recenter(const SbVec3f &newCenter);

  SoEXTENDER public:
    virtual void	doAction(SoAction *action);
    virtual void	callback(SoCallbackAction *action);
    virtual void	GLRender(SoGLRenderAction *action);
    virtual void	getBoundingBox(SoGetBoundingBoxAction *action);
    virtual void	getMatrix(SoGetMatrixAction *action);
    virtual void	pick(SoPickAction *action);

  SoINTERNAL public:
    static void		initClass();

  protected:
    virtual ~SoTransform();
};

#endif /* _SO_TRANSFORM_ */
