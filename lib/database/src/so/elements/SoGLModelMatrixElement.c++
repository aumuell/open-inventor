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
 |   $Revision: 1.1 $
 |
 |   Classes:
 |	SoGLModelMatrixElement
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <GL/gl.h>
#include <Inventor/elements/SoGLModelMatrixElement.h>
#include <Inventor/elements/SoGLViewingMatrixElement.h>
#include <Inventor/errors/SoDebugError.h>
#include <math.h>

SO_ELEMENT_SOURCE(SoGLModelMatrixElement);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoGLModelMatrixElement::~SoGLModelMatrixElement()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes element
//
// Use: public

void
SoGLModelMatrixElement::init(SoState *_state)
//
////////////////////////////////////////////////////////////////////////
{
    // Do normal initialization stuff
    SoModelMatrixElement::init(_state);

    // Save state in instance in case we need it later
    state = _state;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pushes element, causing side effects in GL.
//
// Use: public

void
SoGLModelMatrixElement::push(SoState *_state)
//
////////////////////////////////////////////////////////////////////////
{
    // Save the old matrix first, just in case
    SoModelMatrixElement::push(_state);

    // Save state in instance in case we need it later
    state = _state;

    glPushMatrix();

    // And remember the viewMatrixElement nodeId from our parent:
    SoGLModelMatrixElement *mtxElt = 
	(SoGLModelMatrixElement *) getNextInStack();
    viewEltNodeId = mtxElt->viewEltNodeId;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pops element, causing side effects in GL.
//
// Use: public

void
SoGLModelMatrixElement::pop(SoState *, const SoElement *)
//
////////////////////////////////////////////////////////////////////////
{
    glPopMatrix();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets model matrix to identity matrix in element accessed from
//    state. Overrides method on SoModelMatrixElement, since we have
//    to keep the model-view matrix set correctly.
//
// Use: protected, virtual

void
SoGLModelMatrixElement::makeEltIdentity()
//
////////////////////////////////////////////////////////////////////////
{
    // Since the GL stores a model-view matrix, which is composed of
    // both the view matrix and the model matrix, we have to resend
    // the view matrix to effectively make the model matrix identity.
    // First we have to access the view matrix from the state and then
    // set the model matrix to that value. Accessing the view matrix
    // will "capture" that element, which is necessary for caching to
    // detect the affects of changes to the view matrix on this
    // operation.

    const SbMatrix &viewMat = SoGLViewingMatrixElement::get(state);

    // Set the matrix in the element to identity
    SoModelMatrixElement::makeEltIdentity();

    // Send the current viewing matrix to GL
    glLoadMatrixf((float *) viewMat.getValue());
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets model matrix in element. Assumes that the passed matrix
//    contains the correct combination of view and model matrices.
//
// Use: protected

void
SoGLModelMatrixElement::setElt(const SbMatrix &matrix)
//
////////////////////////////////////////////////////////////////////////
{
    // See the comments in makeEltIdentity() as to why we need to deal
    // with the viewing matrix

    const SbMatrix &viewMat = SoGLViewingMatrixElement::get(state);

    // Set the matrix in the element to the given one
    SoModelMatrixElement::setElt(matrix);

    // Send the product of the viewing matrix and the given matrix to GL
    glLoadMatrixf((float *) (matrix * viewMat).getValue());
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Multiplies into model matrix in element.
//
// Use: protected, virtual

void
SoGLModelMatrixElement::multElt(const SbMatrix &matrix)
//
////////////////////////////////////////////////////////////////////////
{
    SoModelMatrixElement::multElt(matrix);

    glMultMatrixf((float *) matrix.getValue());
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Translates model matrix in element by the given vector.
//
// Use: public, virtual

void
SoGLModelMatrixElement::translateEltBy(const SbVec3f &translation)
//
////////////////////////////////////////////////////////////////////////
{
    SoModelMatrixElement::translateEltBy(translation);

    glTranslatef(translation[0], translation[1], translation[2]);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Rotates model matrix in element by the given rotation.
//
// Use: public, virtual

void
SoGLModelMatrixElement::rotateEltBy(const SbRotation &rotation)
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f	axis;
    float	angle;

    SoModelMatrixElement::rotateEltBy(rotation);

    rotation.getValue(axis, angle);

    glRotatef(angle * (180.0 / M_PI), axis[0], axis[1], axis[2]);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Scales model matrix in element by the given factors.
//
// Use: public, virtual

void
SoGLModelMatrixElement::scaleEltBy(const SbVec3f &scaleFactor)
//
////////////////////////////////////////////////////////////////////////
{
    SoModelMatrixElement::scaleEltBy(scaleFactor);

    glScalef(scaleFactor[0], scaleFactor[1], scaleFactor[2]);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Virtual method that does matrix push.  GL version overrides to
//    make glPushMatrix call:
//
// Use: public, static

SbMatrix
SoGLModelMatrixElement::pushMatrixElt()
//
////////////////////////////////////////////////////////////////////////
{
    //
    // There's no cache dependency here, because we only care if the
    // nodeId changes when traversing our children, we don't really
    // care about the particular value of the ViewingMatrixElement.
    // Well, actually I'm lying a little here.  IF you happened to
    // instance a camera both above and below a TransformSeparator,
    // and THEN you replaced the instance above the TransformSeparator
    // with another camera, AND the TransformSeparator was in a cache,
    // you'll get incorrect behavior.  I'm not going to worry about
    // that case...
    //
    viewEltNodeId = SoGLViewingMatrixElement::getNodeId(state);

    glPushMatrix();
    return SoModelMatrixElement::pushMatrixElt();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Virtual method that does matrix pop.  GL version overrides to
//    make glPopMatrix call:
//
// Use: public, static

void
SoGLModelMatrixElement::popMatrixElt(const SbMatrix &matrix)
//
////////////////////////////////////////////////////////////////////////
{
    glPopMatrix();
    SoModelMatrixElement::popMatrixElt(matrix);

    uint32_t afterNodeId = SoGLViewingMatrixElement::getNodeId(state);

    if (afterNodeId != viewEltNodeId) {
	// Camera underneth us, must reset:
	setElt(matrix);
    }
}

