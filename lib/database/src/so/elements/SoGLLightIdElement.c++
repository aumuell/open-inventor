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
 |	SoGLLightIdElement
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <GL/gl.h>
#include <Inventor/elements/SoGLLightIdElement.h>

SO_ELEMENT_SOURCE(SoGLLightIdElement);

// This holds the maximum number of concurrent GL sources
int	SoGLLightIdElement::maxGLSources = -1;

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoGLLightIdElement::~SoGLLightIdElement()
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
SoGLLightIdElement::init(SoState *)
//
////////////////////////////////////////////////////////////////////////
{
    data = getDefault();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Increments the current light id. This should be called when a
//    light source becomes active. This returns the new id, or -1 if
//    the element is being overridden or if the maximum number of GL
//    light sources has been exceeded. Otherwise, enables light source.
//
// Use: public, static

int
SoGLLightIdElement::increment(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    SoGLLightIdElement	*elt;

    // Get an instance we can change (pushing if necessary)
    elt = (SoGLLightIdElement *) getElement(state, classStackIndex);

    // Being overridden?
    if (elt == NULL)
	return -1;

    // Increment the current data in the element. Since we initialize
    // this to -1 in init() and copy it in push(), the data field is
    // always up to date. This makes it possible to increment() twice
    // without a push() in between.
    elt->data++;

    // Too many sources?
    if (elt->data >= getMaxGLSources())
	return -1;

    // It's a valid source, so enable it
    glEnable((GLenum)(GL_LIGHT0 + elt->data));

    return elt->data;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the maximum number of concurrent light sources supported
//    by GL implementation.
//
// Use: protected, static

int
SoGLLightIdElement::getMaxGLSources()
//
////////////////////////////////////////////////////////////////////////
{
    // Inquire GL if not already done
    if (maxGLSources < 0) {
	GLint	max;
	glGetIntegerv(GL_MAX_LIGHTS, &max);
	maxGLSources = max;
    }

    return maxGLSources;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pushes element, copying current id from previous top instance.
//
// Use: public

void
SoGLLightIdElement::push(SoState *)
//
////////////////////////////////////////////////////////////////////////
{
    data = ((SoGLLightIdElement *) getNextInStack())->data;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pops element, disabling light in GL
//
// Use: public

void
SoGLLightIdElement::pop(SoState *state, const SoElement *prevTopElement)
//
////////////////////////////////////////////////////////////////////////
{
    // Since popping this element has GL side effects, make sure any
    // open caches capture it
    capture(state);

    const SoGLLightIdElement *prevElt =
	(const SoGLLightIdElement *) prevTopElement;
    int	i, max;

    // Disable previous light(s), if valid. All lights between the
    // previous element and this one should be turned off.
    max = getMaxGLSources();
    for (i = (int) prevElt->data; i > data; i--)
	if (i < max)
	    glDisable((GLenum)(GL_LIGHT0 + i));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Prints element for debugging.
//
// Use: public
//
////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
void
SoGLLightIdElement::print(FILE *fp) const
{
    SoElement::print(fp);

    fprintf(fp, "\tLight id = %ld\n", data);
}
#else  /* DEBUG */
void
SoGLLightIdElement::print(FILE *) const
{
}
#endif /* DEBUG */
