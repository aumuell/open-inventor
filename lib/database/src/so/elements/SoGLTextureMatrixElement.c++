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
 |	SoGLTextureMatrixElement
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <GL/gl.h>
#include <Inventor/elements/SoGLTextureMatrixElement.h>

SO_ELEMENT_SOURCE(SoGLTextureMatrixElement);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoGLTextureMatrixElement::~SoGLTextureMatrixElement()
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
SoGLTextureMatrixElement::pop(SoState *, const SoElement *)
//
////////////////////////////////////////////////////////////////////////
{
    glMatrixMode(GL_TEXTURE);
    glLoadMatrixf((float *)(getElt().getValue()));
    glMatrixMode(GL_MODELVIEW);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Make the texture matrix the identity matrix.
//
// Use: protected, virtual

void
SoGLTextureMatrixElement::makeEltIdentity()
//
////////////////////////////////////////////////////////////////////////
{
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);

    SoTextureMatrixElement::makeEltIdentity();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Multiplies into texture matrix in element.
//
// Use: protected, virtual

void
SoGLTextureMatrixElement::multElt(const SbMatrix &matrix)
//
////////////////////////////////////////////////////////////////////////
{
    glMatrixMode(GL_TEXTURE);
    glMultMatrixf((float *) matrix.getValue());
    glMatrixMode(GL_MODELVIEW);

    SoTextureMatrixElement::multElt(matrix);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Translates texture matrix in element by the given vector.
//
// Use: public, virtual

void
SoGLTextureMatrixElement::translateEltBy(const SbVec3f &translation)
//
////////////////////////////////////////////////////////////////////////
{
    glMatrixMode(GL_TEXTURE);
    glTranslatef(translation[0], translation[1], translation[2]);
    glMatrixMode(GL_MODELVIEW);

    SoTextureMatrixElement::translateEltBy(translation);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Rotates texture matrix in element by the given rotation.
//
// Use: public, virtual

void
SoGLTextureMatrixElement::rotateEltBy(const SbRotation &rotation)
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f	axis;
    float	angle;

    rotation.getValue(axis, angle);

    glMatrixMode(GL_TEXTURE);
    glRotatef(angle * (180.0 / M_PI), axis[0], axis[1], axis[2]);
    glMatrixMode(GL_MODELVIEW);

    SoTextureMatrixElement::rotateEltBy(rotation);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Scales texture matrix in element by the given factors.
//
// Use: public, virtual

void
SoGLTextureMatrixElement::scaleEltBy(const SbVec3f &scaleFactor)
//
////////////////////////////////////////////////////////////////////////
{
    glMatrixMode(GL_TEXTURE);
    glScalef(scaleFactor[0], scaleFactor[1], scaleFactor[2]);
    glMatrixMode(GL_MODELVIEW);

    SoTextureMatrixElement::scaleEltBy(scaleFactor);
}
