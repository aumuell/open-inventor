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
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |      Decal
 |
 |   Author(s)          : Ajay Sreekanth, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */
	       
#include <GL/gl.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/elements/SoGLCacheContextElement.h>
#include <Inventor/misc/SoChildList.h>

#include "Decal.h"


SO_NODE_SOURCE(Decal);

//////////////////////////////////////////////////////////////////////
//
//      Decal class: Draws coplanar polygons correctly ordered
//
//////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the Decal class.
//
// Use: internal

void
Decal::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_INIT_CLASS(Decal, SoSeparator, "Separator");
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

Decal::Decal()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(Decal);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

Decal::~Decal()
//
////////////////////////////////////////////////////////////////////////
{
}

//////////////////////////////////////////////////////////////////
//
// Description:
//    Traversal for GL rendering.
//
//    The polygon offset extension makes this really simple; we just
//    draw the on-top layers with a bigger and bigger offset.
//
//    If you don't have the polygon offset extension, this can be made
//    to work for planar decal with the following two-pass algorithm:
//    First the depth buffer update is turned off and the base is
//    rendered followed by the layers in order.  Depth testing is
//    left on as is the color buffer update, thus producing the
//    correct display.  In the second pass, depth buffer update
//    is turned back on, while color buffer update is turned off.
//    The base is now rendered correctly updating the depth buffer
//
// Use: extender

void
Decal::GLRenderBelowPath(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoChildList *childList = getChildren();
    if (! getNumChildren()) return;

    SoState *state = action->getState();
    state->push();

#ifdef GL_EXT_polygon_offset
    // See the comments in the
    // /usr/include/Inventor/elements/SoGLCacheContextElement for more
    // details on this-- it is the correct way to determine whether or
    // not an OpenGL extension is available:
    static int offsetExtInt = -1;
    if (offsetExtInt == -1) {
	offsetExtInt = 
	    SoGLCacheContextElement::getExtID("GL_EXT_polygon_offset");
    }
    if (SoGLCacheContextElement::extSupported(state, offsetExtInt)) {

	glEnable(GL_POLYGON_OFFSET_EXT);

	float offset = 0;

	for (int i=0; i < childList->getLength(); i++) {
	    glPolygonOffsetEXT(offset, 0);
	    childList->traverse(action, i);

	    offset -= 0.1;
	}

	glDisable(GL_POLYGON_OFFSET_EXT);

    } else {

#endif
	// Extension not supported at compile time or at run-time:

	// first pass
	// turn off depth update, leaving color update on
	GLboolean oldDepthMask;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &oldDepthMask);
	if (oldDepthMask) 
	    glDepthMask(GL_FALSE);
	// render base followed by layers in order
	childList->traverse(action);
	if (oldDepthMask)
	    glDepthMask(oldDepthMask);

	state->pop();  // Pop and push state between passes to avoid
	state->push(); // problems with non-Separator children

	// second pass
	// turn off color update leaving depth update on
	GLboolean colorMode;
	glGetBooleanv(GL_RGBA_MODE, &colorMode);
	if (colorMode) { 	// RGBA mode
	    GLboolean oldColorMask[4];
	    glGetBooleanv(GL_COLOR_WRITEMASK, oldColorMask);
	    GLboolean anyMask = (oldColorMask[0] || oldColorMask[1] ||
				 oldColorMask[2] || oldColorMask[3]);
	    if (anyMask) 
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	    // only render base
	    childList->traverse(action, 0);
	    if (anyMask)
		glColorMask(oldColorMask[0], oldColorMask[1],
			    oldColorMask[2], oldColorMask[3]);
	}
	else {	// color index mode
	    GLint	oldMask;
	    glGetIntegerv(GL_INDEX_WRITEMASK, &oldMask);
	    if (oldMask)
		glIndexMask(0x0);
	    // only render base
	    childList->traverse(action, 0);
	    if (oldMask)
		glIndexMask(oldMask);
	}
#ifdef GL_EXT_polygon_offset
    }
#endif
    state->pop();
}
