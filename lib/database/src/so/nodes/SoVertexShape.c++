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
 |      SoVertexShape
 |
 |   Author(s)          : Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/caches/SoNormalCache.h>
#include <Inventor/elements/SoCoordinateElement.h>
#include <Inventor/elements/SoCreaseAngleElement.h>
#include <Inventor/elements/SoShapeHintsElement.h>
#include <Inventor/nodes/SoVertexShape.h>
#include <Inventor/elements/SoDrawStyleElement.h>
#include <Inventor/elements/SoComplexityTypeElement.h>
#include <Inventor/actions/SoGLRenderAction.h>

SO_NODE_ABSTRACT_SOURCE(SoVertexShape);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: protected

SoVertexShape::SoVertexShape()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoVertexShape);
    SO_NODE_ADD_FIELD(vertexProperty, (NULL));

    normalCache = NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoVertexShape::~SoVertexShape()
//
////////////////////////////////////////////////////////////////////////
{
    if (normalCache != NULL)
	delete normalCache;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Subclasses may define this method to generate normals to use
//    when the normal binding is DEFAULT and there aren't enough
//    normals in the state. This should use the given SoNormalBundle
//    to generate the normals. Returns TRUE if normals were generated.
//
//    The default method returns TRUE.
//
// Use: extender, virtual

SbBool
SoVertexShape::generateDefaultNormals(SoState *, SoNormalBundle *)
//
////////////////////////////////////////////////////////////////////////
{
    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets up a cache to hold normals. This assumes the cache depends
//    on only the current coordinates, shape hints, and crease angle.
//
// Use: protected

void
SoVertexShape::setNormalCache(SoState *state,
			      int numNormals, const SbVec3f *normals)
//
////////////////////////////////////////////////////////////////////////
{
    if (normalCache != NULL)
	normalCache->unref();

    normalCache = new SoNormalCache(state);
    normalCache->ref();

    normalCache->set(numNormals, normals);

    // Set up the dependencies
#define ADD_DEPENDENCY(elt)						      \
    normalCache->addElement(state->getConstElement(elt::getClassStackIndex()))

    ADD_DEPENDENCY(SoCoordinateElement);
    ADD_DEPENDENCY(SoCreaseAngleElement);
    ADD_DEPENDENCY(SoShapeHintsElement);

#undef ADD_DEPENDENCY
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Passes on notification after invalidating any caches.
//
// Use: internal

void
SoVertexShape::notify(SoNotList *list)
//
////////////////////////////////////////////////////////////////////////
{
    // Destroy cache, if present
    if (normalCache != NULL) {
	normalCache->unref();
	normalCache = NULL;
    }
    vpCache.invalidate();

    // Then do the usual stuff
    SoShape::notify(list);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if the shape should be rendered now.  Does more then
//    the method in SoShape, because of possible transparency in vertex
//    property node.
//
// Use: protected
//
//////////////////////////////////////////////////////////////////////////
SbBool
SoVertexShape::shouldGLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{

    // Check if the shape is invisible
    if (SoDrawStyleElement::get(action->getState()) ==
	SoDrawStyleElement::INVISIBLE)
	return FALSE;

    // If the shape is transparent and transparent objects are being
    // delayed, don't render now
    // if there is transparency in the vertex property node, the object is
    // transparent.  If there are no colors in the vertexProperty node,
    // we have to check the state.
    if (!vpCache.colorIsInVtxProp()){
    	if (action->handleTransparency())
		return FALSE;
    }
    else if (vpCache.transpIsInVtxProp()){
	if (action->handleTransparency(TRUE))
		return FALSE;
    }
    else
	    SoLazyElement::setBlending(action->getState(), FALSE);        

    // If the current complexity is BOUNDING_BOX, just render the
    // cuboid surrounding the shape and tell the shape to stop
    if (SoComplexityTypeElement::get(action->getState()) ==
	SoComplexityTypeElement::BOUNDING_BOX) {
	GLRenderBoundingBox(action);
	return FALSE;
    }

    // Otherwise, go ahead and render the object
    return TRUE;
}
