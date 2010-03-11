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

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |      SoTransform
 |
 |   Author(s)          : Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoPickAction.h>
#include <Inventor/elements/SoBBoxModelMatrixElement.h>
#include <Inventor/elements/SoGLModelMatrixElement.h>
#include <Inventor/elements/SoLocalBBoxMatrixElement.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodes/SoTransform.h>

SO_NODE_SOURCE(SoTransform);

// Shorthand for accumulating a rotation into matrix and inverse using
// intermediate matrix m. Destructive to "rot".
#define ROTATE(rot, matrix, inverse, m)					      \
    rot.getValue(m);							      \
    matrix.multLeft(m);							      \
    rot.invert();							      \
    rot.getValue(m);							      \
    inverse.multRight(m)

// Shorthand for accumulating a scale vector into matrix and inverse
// using intermediate matrix m. Destructive to "vec".
#define SCALE(vec, matrix, inverse, m)					      \
    m.setScale(vec);							      \
    matrix.multLeft(m);							      \
    vec[0] = 1.0 / vec[0];						      \
    vec[1] = 1.0 / vec[1];						      \
    vec[2] = 1.0 / vec[2];						      \
    m.setScale(vec);							      \
    inverse.multRight(m)

// Shorthand for accumulating a translation vector into matrix and
// inverse using intermediate matrix m. Non-destructive to "vec".
#define TRANSLATE(vec, matrix, inverse, m)				      \
    m.setTranslate(vec);						      \
    matrix.multLeft(m);							      \
    m.setTranslate(-vec);						      \
    inverse.multRight(m)

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoTransform::SoTransform()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoTransform);
    SO_NODE_ADD_FIELD(translation,     (0.0, 0.0, 0.0));
    SO_NODE_ADD_FIELD(rotation,        (0.0, 0.0, 0.0, 1.0));
    SO_NODE_ADD_FIELD(scaleFactor,     (1.0, 1.0, 1.0));
    SO_NODE_ADD_FIELD(scaleOrientation,(0.0, 0.0, 0.0, 1.0));
    SO_NODE_ADD_FIELD(center,          (0.0, 0.0, 0.0));
    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoTransform::~SoTransform()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the transform to translate the origin to the fromPoint and
//    rotate the negative z-axis (0,0,-1) to lie on the vector from
//    fromPoint to toPoint. This always tries to keep the "up"
//    direction the positive y-axis, unless that is impossible.
//    All current info in the node is wiped out.
//
// Use: public

void
SoTransform::pointAt(const SbVec3f &fromPoint, const SbVec3f &toPoint)
//
////////////////////////////////////////////////////////////////////////
{
    // Wipe out all transformation info
    setToDefaults();

    // Translate origin to fromPoint
    translation = fromPoint;

    // Rotate negative z-axis to correct vector
    // ??? This does not preserve "up" direction - fix it later!!!! ???
    rotation.setValue(SbRotation(SbVec3f(0.0, 0.0, -1.0), toPoint - fromPoint));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns composite matrix that takes you from object space to
//    scale space.
//
// Use: public

void
SoTransform::getScaleSpaceMatrix(SbMatrix &mat, SbMatrix &inv) const
//
////////////////////////////////////////////////////////////////////////
{
    SbMatrix	m;

    mat.makeIdentity();
    inv.makeIdentity();

    if (! scaleFactor.isIgnored() && ! scaleFactor.isDefault()) {
	SbVec3f	s = scaleFactor.getValue();
	SCALE(s, mat, inv, m);

        if (! scaleOrientation.isIgnored() && ! scaleOrientation.isDefault()) {
	    SbRotation	r = scaleOrientation.getValue();
	    r.invert();
	    ROTATE(r, mat, inv, m);
	}

    }
    if (! center.isIgnored() && ! center.isDefault()) {
	SbVec3f c = center.getValue();
	c.negate();
	TRANSLATE(c, mat, inv, m);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns composite matrix that takes you from object space to
//    rotation space.
//
// Use: public

void
SoTransform::getRotationSpaceMatrix(SbMatrix &mat, SbMatrix &inv) const
//
////////////////////////////////////////////////////////////////////////
{
    SbMatrix	m, minv;

    mat.makeIdentity();
    inv.makeIdentity();

    if (! rotation.isIgnored() && ! rotation.isDefault()) {
	SbRotation	r = rotation.getValue();
	ROTATE(r, mat, inv, m);
    }

    if ((! scaleFactor.isIgnored() && ! scaleFactor.isDefault()) ||
	(!      center.isIgnored() && !      center.isDefault())) {

        if (! scaleOrientation.isIgnored() && ! scaleOrientation.isDefault()) {
	    SbRotation	r = scaleOrientation.getValue();
	    ROTATE(r, mat, inv, m);
	}

	getScaleSpaceMatrix(m, minv);
        mat.multLeft(m);
        inv.multRight(minv);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns composite matrix that takes you from object space to
//    translation space.
//
// Use: public

void
SoTransform::getTranslationSpaceMatrix(SbMatrix &mat, SbMatrix &inv) const
//
////////////////////////////////////////////////////////////////////////
{
    SbMatrix	m, minv;

    mat.makeIdentity();
    inv.makeIdentity();

    if (! translation.isIgnored() && ! translation.isDefault()) {
	const SbVec3f &v = translation.getValue();
	TRANSLATE(v, mat, inv, m);
    }

    if (! center.isIgnored() && ! center.isDefault()) {
	const SbVec3f &c = center.getValue();
	TRANSLATE(c, mat, inv, m);
    }

    getRotationSpaceMatrix(m, minv);
    mat.multLeft(m);
    inv.multRight(minv);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Convenience function that premultiplies the effects of a matrix
//    transformation into the transformation stored in this node.
//
// Use: public

void
SoTransform::multLeft(const SbMatrix &mat)
//
////////////////////////////////////////////////////////////////////////
{
    // Get the matrix corresponding to this transform
    SoGetMatrixAction ma(SbVec2s(1,1));
    ref();
    ma.apply(this);
    unrefNoDelete();

    // Multiply the result and set this node from it
    setMatrix(mat * ma.getMatrix());
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Convenience function that postmultiplies the effects of a matrix
//    transformation into the transformation stored in this node.
//
// Use: public

void
SoTransform::multRight(const SbMatrix &mat)
//
////////////////////////////////////////////////////////////////////////
{
    // Get the matrix corresponding to this transform
    SoGetMatrixAction ma(SbVec2s(1,1));
    ref();
    ma.apply(this);
    unrefNoDelete();

    // Multiply the result and set this node from it
    setMatrix(ma.getMatrix() * mat);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This is the same as multLeft(), except that the transformation
//    to multiply into this node comes from another transformation
//    node.
//
// Use: public

void
SoTransform::combineLeft(SoTransformation *nodeOnLeft)
//
////////////////////////////////////////////////////////////////////////
{
    // Get the matrix corresponding to the given node and multiply
    SoGetMatrixAction ma(SbVec2s(1,1));
    nodeOnLeft->ref();
    ma.apply(nodeOnLeft);
    nodeOnLeft->unrefNoDelete();
    multLeft(ma.getMatrix());
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This is the same as multRight(), except that the transformation
//    to multiply into this node comes from another transformation
//    node.
//
// Use: public

void
SoTransform::combineRight(SoTransformation *nodeOnRight)
//
////////////////////////////////////////////////////////////////////////
{
    // Get the matrix corresponding to the given node and multiply
    SoGetMatrixAction ma(SbVec2s(1,1));
    nodeOnRight->ref();
    ma.apply(nodeOnRight);
    nodeOnRight->unrefNoDelete();
    multRight(ma.getMatrix());
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the fields in the node to implement the transformation
//    represented by the given matrix.
//
// Use: public

void
SoTransform::setMatrix(const SbMatrix &mat)
//
////////////////////////////////////////////////////////////////////////
{
    // Factor mat into the necessary fields, while keeping the center
    // field unchanged. Therefore, we need to factor a new matrix,
    // [mNoCenter], given by:
    //		[centerInverse][mNoCenter][center] = [mat]
    //
    // So, [mNoCenter] = [center][mat][centerInverse]

    SbMatrix mNoCenter, subMat;
    mNoCenter.setTranslate(-center.getValue());
    mNoCenter.multLeft(mat);
    subMat.setTranslate(center.getValue());
    mNoCenter.multLeft(subMat);

    // Now, factor mNoCenter and plug it back into the fields
    SbMatrix	shearRotMat, rotMat, projMat;
    SbVec3f	sVec, tVec;

    if (mNoCenter.factor(shearRotMat, sVec, rotMat, tVec, projMat)) {
	SbRotation	rot = rotMat;

	if (translation.getValue() != tVec)
	    translation = tVec;

	if (rotation.getValue() != rot)
	    rotation = rot;

	if (scaleFactor.getValue() != sVec)
	    scaleFactor = sVec;
	
	// Don't change the scale orientation if the scale is unity
	if (sVec != SbVec3f(1.0, 1.0, 1.0)) {
	    rot = shearRotMat.transpose();

	    if (scaleOrientation.getValue() != rot)
		scaleOrientation = rot;
	}
    }
    else
	SoDebugError::post("SoTransform::setMatrix",
			   "Can't factor given matrix");
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Changes the center of the transformation to the given point
//    without affecting the overall effect of the transformation.
//
// Use: public

void
SoTransform::recenter(const SbVec3f &newCenter)
//
////////////////////////////////////////////////////////////////////////
{
    // We want to change the center field of the transform node to newCenter,
    // while keeping the resulting size, orientation, and location the
    // same.
    // If we just change the center field, and the scale is not (1,1,1),
    // the result can be that the object shifts in space.
    // So, we need to do the following:
    // [1] find [xfMat], the SbMatrix we get by applying a getMatrix action
    SoGetMatrixAction ma(SbVec2s(1,1));
    ref();
    ma.apply(this);
    unrefNoDelete();
    const SbMatrix	&xfMat = ma.getMatrix();

    // Factor the resulting matrix into the necessary fields, while
    // making the center field be equal to newCenter. Therefore, we
    // need to factor a new matrix, [mNoCenter], given by:
    // [newCenterInverse][mNoCenter][newCenter] = [xfMat]
    //
    // So, [mNoCenter] = [newCenter][xfMat][newCenterInverse]

    SbMatrix	mNoCenter, subMat;
    mNoCenter.setTranslate(-newCenter);
    mNoCenter.multLeft(xfMat);
    subMat.setTranslate(newCenter);
    mNoCenter.multLeft(subMat);

    // Now, factor mNoCenter to get remaining fields of node...
    SbMatrix	shearRotMat, rotMat, projMat;
    SbVec3f	sVec, tVec;
    if (mNoCenter.factor(shearRotMat, sVec, rotMat, tVec, projMat)) {
	SbRotation rot;

	if (center.getValue() != newCenter)
	    center = newCenter;
	if (translation.getValue() != tVec)
	    translation = tVec;
	rot = rotMat;
	if (rotation.getValue() != rot)
	     rotation = rot;
	if (scaleFactor.getValue() != sVec)
	     scaleFactor = sVec;
	// don't change the scale orientation if the scale is unity
	if (sVec != SbVec3f(1.0, 1.0, 1.0)) {
	    rot = shearRotMat.transpose();
	    if (scaleOrientation.getValue() != rot)
		scaleOrientation = rot;
	}
    }
    else
	SoDebugError::post("SoTransform::recenter",
			   "Can't factor centering matrix");
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does all the work for most actions.
//
// Use: extender

void
SoTransform::doAction(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool	doCenter, doScaleOrient;
    SoState	*state = action->getState();

    doCenter      = (! center.isIgnored() && ! center.isDefault());
    doScaleOrient = (! scaleOrientation.isIgnored() &&
		     ! scaleOrientation.isDefault());

    // Do these in GL (right-to-left) order
    if (! translation.isIgnored() && ! translation.isDefault())
	SoModelMatrixElement::translateBy(state, this, translation.getValue());

    if (doCenter)
	SoModelMatrixElement::translateBy(state, this, center.getValue());

    if (! rotation.isIgnored() && ! rotation.isDefault())
	SoModelMatrixElement::rotateBy(state, this, rotation.getValue());

    if (! scaleFactor.isIgnored() && ! scaleFactor.isDefault()) {

	if (doScaleOrient)
	    SoModelMatrixElement::rotateBy(state, this,
					   scaleOrientation.getValue());

	SoModelMatrixElement::scaleBy(state, this, scaleFactor.getValue());

	if (doScaleOrient) {
	    SbRotation r = scaleOrientation.getValue();
	    r.invert();
	    SoModelMatrixElement::rotateBy(state, this, r);
	}
    }

    if (doCenter)
	SoModelMatrixElement::translateBy(state, this, -center.getValue());
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Handles callback action
//
// Use: extender

void
SoTransform::callback(SoCallbackAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoTransform::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Handles GL render action
//
// Use: extender

void
SoTransform::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoTransform::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Handles get bounding box action
//
// Use: extender

void
SoTransform::getBoundingBox(SoGetBoundingBoxAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoTransform::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns transformation matrix.
//
// Use: extender

void
SoTransform::getMatrix(SoGetMatrixAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SbMatrix	&ctm = action->getMatrix();
    SbMatrix	&inv = action->getInverse();
    SbMatrix	m;
    SbVec3f	v, centerVec;
    SbBool	doCenter, doScaleOrient;

    doCenter      = (! center.isIgnored() && ! center.isDefault());
    doScaleOrient = (! scaleOrientation.isIgnored() &&
		     ! scaleOrientation.isDefault());

    if (! translation.isIgnored() && ! translation.isDefault()) {
	v = translation.getValue();
	TRANSLATE(v, ctm, inv, m);
    }

    if (doCenter) {
	centerVec = center.getValue();
	TRANSLATE(centerVec, ctm, inv, m);
    }

    if (! rotation.isIgnored() && ! rotation.isDefault()) {
	SbRotation	r = rotation.getValue();
	ROTATE(r, ctm, inv, m);
    }

    if (! scaleFactor.isIgnored() && ! scaleFactor.isDefault()) {

	if (doScaleOrient) {
	    SbRotation	r = scaleOrientation.getValue();
	    ROTATE(r, ctm, inv, m);
	}

	SbVec3f	s = scaleFactor.getValue();
	SCALE(s, ctm, inv, m);

	if (doScaleOrient) {
	    SbRotation	r = scaleOrientation.getValue();
	    r.invert();
	    ROTATE(r, ctm, inv, m);
	}

    }

    if (doCenter) {
	centerVec.negate();
	TRANSLATE(centerVec, ctm, inv, m);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Handles pick action.
//
// Use: extender

void
SoTransform::pick(SoPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoTransform::doAction(action);
}

#undef ROTATE
#undef SCALE
#undef TRANSLATE
