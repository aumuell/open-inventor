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
 |   $Revision $
 |
 |   Classes:
 |	SoTextureCoordinateBundle class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <GL/gl.h>
#include <Inventor/SbBox.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/bundles/SoTextureCoordinateBundle.h>
#include <Inventor/elements/SoGLTextureEnabledElement.h>
#include <Inventor/elements/SoTextureCoordinateBindingElement.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodes/SoShape.h>

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoTextureCoordinateBundle::SoTextureCoordinateBundle(SoAction *action,
						     SbBool forRendering,
						     SbBool setUpDefault) :
	SoBundle(action)
//
////////////////////////////////////////////////////////////////////////
{
    isRendering = forRendering;
    //??? When Gavin gets back, he can explain if tCoords is really needed:
    tCoords = NULL;

    setFunction = FALSE;

    if (isRendering)
	setUpForGLRender(action);
    else
	setUpForPrimGen(action, setUpDefault);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: public

SoTextureCoordinateBundle::~SoTextureCoordinateBundle()
//
////////////////////////////////////////////////////////////////////////
{
    if (setFunction) state->pop();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets up bundle for primitive generation.
//
// Use: private

void
SoTextureCoordinateBundle::setUpForPrimGen(SoAction *action,
					   SbBool setUpDefault)
//
////////////////////////////////////////////////////////////////////////
{
    // Primitive generation always requires texture coordinates
    needCoords = TRUE;

    texCoordElt   = SoTextureCoordinateElement::getInstance(state);
    GLTexCoordElt = NULL;

    switch (texCoordElt->getType()) {

      case SoTextureCoordinateElement::EXPLICIT:
	if (texCoordElt->getNum() < 1) {
	    // Don't bother computing the function if not requested
	    if (setUpDefault) {
		// Set up a texture coordinate function that compute the
		// texture coordinates. We need to do this through the
		// element so the state can be restored correctly.
		state->push();
		SoNode *currentNode = action->getCurPathTail();	
		SoTextureCoordinateElement::setFunction(state, currentNode,
							generateCoord, this);
		// Get the new instance in here
		texCoordElt = SoTextureCoordinateElement::getInstance(state);

		setUpDefaultCoordSpace(action);

		setFunction = TRUE;
	    }
	    isFunc = TRUE;
	} else {
	    isFunc = FALSE;
	}
	break;

      case SoTextureCoordinateElement::FUNCTION:
	isFunc = TRUE;
	break;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets up bundle for GL rendering.
//
// Use: private

void
SoTextureCoordinateBundle::setUpForGLRender(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    if (! SoGLTextureEnabledElement::get(action->getState()))
	needCoords = isFunc = FALSE;

    else {
	// Access a GL version of the element for sending texture
	// coordinates to GL
	texCoordElt   = SoTextureCoordinateElement::getInstance(state);
	GLTexCoordElt = (const SoGLTextureCoordinateElement *) texCoordElt;

	switch (GLTexCoordElt->getType()) {

	  case SoGLTextureCoordinateElement::EXPLICIT:
	    if (GLTexCoordElt->getNum() > 0) {
		needCoords = TRUE;
		isFunc     = FALSE;
	    } else {
		// Set up the default coordinate function before it is
		// used by the SoGLTextureCoordinateElement
		setUpDefaultCoordSpace(action);

		// Set up a texture coordinate function that will do the
		// correct tex gen stuff. We need to do this through the
		// element so the state can be restored correctly.
		state->push();
		setFunction = TRUE;
		SoNode *currentNode = action->getCurPathTail();
		SoGLTextureCoordinateElement::
		    setTexGen(state, currentNode, setUpTexGen, this,
			      generateCoord, this);

		// Get the new instance in here
		texCoordElt   = SoTextureCoordinateElement::getInstance(state);
		GLTexCoordElt = (const SoGLTextureCoordinateElement *)
		    texCoordElt;


		// No longer need coordinates, since texgen does the job
		needCoords = FALSE;

		// Set this flag to TRUE so that if we are using primitive
		// generation to do the rendering, we can tell that we
		// have a function to use
		isFunc = TRUE;
	    }
	    break;

	  case SoGLTextureCoordinateElement::FUNCTION:
#ifdef DEBUG
	    SoDebugError::post(
		"SoTextureCoordinateBundle::setUpForGLRender",
		"GLTextureCoordinateElement is FUNCTION!");
#endif
	    needCoords = isFunc = FALSE;
	    break;
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets up parameters for computing default texture coordinates.
//    The default coordinates are defined based on the object-space
//    bounding box of the shape. The S coordinate ranges from 0 to 1
//    over the side of the box with the largest dimension, and the T
//    coordinate ranges over the side of the box with the next largest
//    dimension. The repeat size is the same in both dimensions so
//    that a square texture remains square.
//
// Use: private

void
SoTextureCoordinateBundle::setUpDefaultCoordSpace(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoNode *tail = action->getCurPathTail();
#ifdef DEBUG
    if (!tail->isOfType(SoShape::getClassTypeId())) {
	SoDebugError::post(
	    "SoTextureCoordinateBundle::setUpDefaultCoordSpace",
	    "Tail of path is not a shape node!");
    }
#endif
    SoShape *shape = (SoShape *)tail;

    SbBox3f box;
    SbVec3f center;
    shape->computeBBox(action, box, center);

    const SbVec3f	&min    = box.getMin();
    SbVec3f		boxSize = box.getMax() - min;

    // Look for the largest two dimensions of the box
    if (boxSize[0] > boxSize[1] && boxSize[0] > boxSize[2]) {
	coordS = 0;
	coordT = boxSize[1] > boxSize[2] ? 1 : 2;
    }
    else if (boxSize[1] > boxSize[2]) {
	coordS = 1;
	coordT = boxSize[0] > boxSize[2] ? 0 : 2;
    }
    else {
	coordS = 2;
	coordT = boxSize[0] > boxSize[1] ? 0 : 1;
    }

    // Set up vectors for S and T coordinates. The length of the
    // vectors should be the size of the box in that dimension. Since
    // we want the texture to remain square, we use the same length
    // for both dimensions.
    float tmp = 1.0 / boxSize[coordS];
    sVector.setValue(0.0, 0.0, 0.0, -min[coordS] * tmp);
    tVector.setValue(0.0, 0.0, 0.0, -min[coordT] * tmp);
    sVector[coordS] = tVector[coordT] = tmp;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback registered with SoTextureCoordinateElement for
//    computing texture coordinate from point and normal - used for
//    default coordinate generation. The userData arg will be "this".
//
// Use: private

const SbVec4f &
SoTextureCoordinateBundle::generateCoord(void *userData,
					 const SbVec3f &point,
					 const SbVec3f & /* normal */)
//
////////////////////////////////////////////////////////////////////////
{
    SoTextureCoordinateBundle *tcb = (SoTextureCoordinateBundle *) userData;

    static SbVec4f	result;

    // The S and T coordinates of the result are the dot products of
    // the point with sVector and tVector. Since this computation is
    // done very frequently (during primitive generation for picking
    // on vertex-based shapes), we can avoid some operations that
    // result in 0 by doing the dot products explicitly.
    int	sDim = tcb->coordS, tDim = tcb->coordT;


    result.setValue(point[sDim] * tcb->sVector[sDim] + tcb->sVector[3],
		    point[tDim] * tcb->tVector[tDim] + tcb->tVector[3],
		    0.0,
		    1.0);

    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback registered with SoGLTextureCoordinateElement for
//    setting up GL texture generation for default coordinates. The
//    userData arg will be "this".
//
// Use: private

void
SoTextureCoordinateBundle::setUpTexGen(void *userData)
//
////////////////////////////////////////////////////////////////////////
{
    SoTextureCoordinateBundle *tcb = (SoTextureCoordinateBundle *) userData;

    glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenfv(GL_S, GL_OBJECT_PLANE, tcb->sVector.getValue());

    glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenfv(GL_T, GL_OBJECT_PLANE, tcb->tVector.getValue());
}
