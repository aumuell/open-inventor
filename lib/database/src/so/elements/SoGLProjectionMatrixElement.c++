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
 |	SoGLProjectionMatrixElement
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <GL/gl.h>
#include <Inventor/elements/SoGLProjectionMatrixElement.h>

SO_ELEMENT_SOURCE(SoGLProjectionMatrixElement);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoGLProjectionMatrixElement::~SoGLProjectionMatrixElement()
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
SoGLProjectionMatrixElement::pop(SoState *state, const SoElement *prevElt)
//
////////////////////////////////////////////////////////////////////////
{
    //
    // If the previous element is at depth zero, don't bother
    // restoring the matrices-- they will just be setup again the next
    // time a renderAction is applied.  Essentially, the first camera
    // in a scene graph will leak outside of its separator.
    //
    if (prevElt->getDepth() != 0) {
	// Since popping this element has GL side effects, make sure any
	// open caches capture it
	capture(state);

	// Restore previous projection matrix
	send();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets GLProjection matrix in element.
//
// Use: protected, virtual

void
SoGLProjectionMatrixElement::setElt(const SbMatrix &matrix)
//
////////////////////////////////////////////////////////////////////////
{
    // Set matrix in element
    projectionMatrix = matrix;
    send();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sends current projection matrix to GL.
//
// Use: private

void
SoGLProjectionMatrixElement::send()
//
////////////////////////////////////////////////////////////////////////
{
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf((float *) projectionMatrix.getValue());
    glMatrixMode(GL_MODELVIEW);
}
