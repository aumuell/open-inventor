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
 |	SoAnnotation
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <GL/gl.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/nodes/SoAnnotation.h>

SO_NODE_SOURCE(SoAnnotation);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoAnnotation::SoAnnotation()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoAnnotation);

    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoAnnotation::~SoAnnotation()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Traversal for GL rendering
//
// Use: extender

void
SoAnnotation::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // If the action is currently rendering the delayed paths, turn
    // off depth buffer comparisons and render like a separator
    if (action->isRenderingDelayedPaths()) {
	SbBool zbuffEnabled = glIsEnabled(GL_DEPTH_TEST);
	if (zbuffEnabled)
	    glDisable(GL_DEPTH_TEST);
	
	SoSeparator::GLRender(action);
	
	// turn the depth comparisons if it was originally turned on
	if (zbuffEnabled)
	    glEnable(GL_DEPTH_TEST); 
    }

    // Otherwise, tell the render action that we want to render this
    // node last
    else {
	// Since we need to get a chance to add the path to the
	// action, make sure we're not in any caches
	SoCacheElement::invalidate(action->getState());
	action->addDelayedPath(action->getCurPath()->copy());
    }
}
