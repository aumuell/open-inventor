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
 |	SoGLClipPlaneElement
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <GL/gl.h>
#include <Inventor/elements/SoGLClipPlaneElement.h>
#include <Inventor/misc/SoState.h>

// This holds the maximum number of concurrent GL clip planes
int	SoGLClipPlaneElement::maxGLPlanes = -1;

SO_ELEMENT_SOURCE(SoGLClipPlaneElement);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoGLClipPlaneElement::~SoGLClipPlaneElement()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pops element, causing side effects in GL.
//
// Use: public

void
SoGLClipPlaneElement::pop(SoState *state, const SoElement *prevTopElement)
//
////////////////////////////////////////////////////////////////////////
{
    // Since popping this element has GL side effects, make sure any
    // open caches capture it
    capture(state);

    // Disable clip planes created in previous elements. Note that the
    // index into the planes array is identical to the GL clip plane
    // id, for convenience.

    const SoGLClipPlaneElement *prevElt =
	(const SoGLClipPlaneElement *) prevTopElement;

    int maxId = prevElt->getNum();
    if (maxId > getMaxGLPlanes())
	maxId = getMaxGLPlanes();

    for (int i = prevElt->startIndex; i < maxId; i++)
	if (i < getMaxGLPlanes())
	    glDisable((GLenum)(GL_CLIP_PLANE0 + i));

    // Do parent's pop stuff
    SoClipPlaneElement::pop(state, prevTopElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the maximum number of concurrent clipping planes supported
//    by GL implementation.
//
// Use: protected, static

int
SoGLClipPlaneElement::getMaxGLPlanes()
//
////////////////////////////////////////////////////////////////////////
{
    // Inquire GL if not already done
    if (maxGLPlanes < 0) {
	GLint		max;
	glGetIntegerv(GL_MAX_CLIP_PLANES, &max);
	maxGLPlanes = (int) max;
    }

    return maxGLPlanes;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds clip plane to element and sends it to GL.
//
// Use: protected, virtual

void
SoGLClipPlaneElement::addToElt(const SbPlane &plane,
			       const SbMatrix &modelMatrix)
//
////////////////////////////////////////////////////////////////////////
{
    // Do normal stuff
    SoClipPlaneElement::addToElt(plane, modelMatrix);

    // If we haven't run out of clipping planes, send this one to GL.
    // Note that we send the plane in object space, since GL already
    // will transform it by the current model matrix.
    int	planeId = getNum() - 1;
    if (planeId < getMaxGLPlanes()) {

	const SbPlane	&objPlane = get(planeId, FALSE);
	const SbVec3f	&norm     = objPlane.getNormal();
	GLdouble	planeEquation[4];

	planeEquation[0] = norm[0];
	planeEquation[1] = norm[1];
	planeEquation[2] = norm[2];
	planeEquation[3] = -objPlane.getDistanceFromOrigin();

	glClipPlane((GLenum)(GL_CLIP_PLANE0 + planeId), planeEquation);

	glEnable((GLenum)(GL_CLIP_PLANE0 + planeId));
    }
}
