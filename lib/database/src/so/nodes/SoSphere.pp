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
 |      SoSphere
 |
@@ This next line is true for the output file only - ignore it here:
 |   Note: This file was preprocessed from another file. Do not edit it.
 |
 |   Author(s)          : Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <GL/gl.h>
#include <Inventor/SbBox.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/bundles/SoMaterialBundle.h>
#include <Inventor/elements/SoComplexityElement.h>
#include <Inventor/elements/SoComplexityTypeElement.h>
#include <Inventor/elements/SoGLTextureCoordinateElement.h>
#include <Inventor/elements/SoGLTextureEnabledElement.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/nodes/SoSphere.h>

#ifdef _CRAY
#define atan2f	atan2
#endif

SO_NODE_SOURCE(SoSphere);

// Computes S and T texture coordinates from point on surface
#define COMPUTE_S_T(point, s, t)					      \
    s = atan2f(point[0], point[2]) * .159 + .5;				      \
    t = atan2f(point[1],						      \
	       sqrtf(point[0]*point[0] + point[2]*point[2])) * .318 + .5

;
// Adjusts S texture coordinate in unstable areas
#define ADJUST_S(s, octant)						      \
    if (s < .001 && (octant == 1 || octant == 3))			      \
	s = 1.0;							      \
    else if (s > .999 && (octant == 5 || octant == 7))			      \
	s = 0.0

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoSphere::SoSphere()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoSphere);
    SO_NODE_ADD_FIELD(radius, (1.0));
    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoSphere::~SoSphere()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Performs GL rendering of a sphere.
//
// Use: extender

void
SoSphere::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // First see if the object is visible and should be rendered now
    if (! shouldGLRender(action))
	return;

    // Make sure the first current material is sent to GL
    SoMaterialBundle	mb(action);
    mb.sendFirst();

    // See if texturing is enabled
    SbBool doTextures = SoGLTextureEnabledElement::get(action->getState());

    // Render the sphere. The GLRenderGeneric() method handles any
    // case. The GLRenderNvertTnone() handles the case where we are
    // outputting normals but no texture coordinates. This case is
    // handled separately since it occurs often and warrants its own
    // method.

    if (! doTextures && ! mb.isColorOnly())
	GLRenderNvertTnone(action);
    else
	GLRenderGeneric(action, ! mb.isColorOnly(), doTextures);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements ray picking
//
// Use: extender

void
SoSphere::rayPick(SoRayPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f		enterPoint, exitPoint, normal;
    SbVec4f		texCoord(0.0, 0.0, 0.0, 1.0);
    SoPickedPoint	*pp;

    // First see if the object is pickable
    if (! shouldRayPick(action))
	return;

    // Compute the picking ray in our current object space
    computeObjectSpaceRay(action);

    // Create SbSphere with correct radius, centered at zero
    float	rad = (radius.isIgnored() ? 1.0 : radius.getValue());
    SbSphere	sph(SbVec3f(0., 0., 0.), rad);

    // Intersect with pick ray. If found, set up picked point(s)
    if (sph.intersect(action->getLine(), enterPoint, exitPoint)) {
	if (action->isBetweenPlanes(enterPoint) &&
	    (pp = action->addIntersection(enterPoint)) != NULL) {

	    normal = enterPoint;
	    normal.normalize();
	    pp->setObjectNormal(normal);
	    COMPUTE_S_T(enterPoint, texCoord[0], texCoord[1]);
	    pp->setObjectTextureCoords(texCoord);
	}

	if (action->isBetweenPlanes(exitPoint) &&
	    (pp = action->addIntersection(exitPoint)) != NULL) {

	    normal = exitPoint;
	    normal.normalize();
	    pp->setObjectNormal(normal);
	    COMPUTE_S_T(exitPoint, texCoord[0], texCoord[1]);
	    texCoord[2] = texCoord[3] = 0.0;
	    pp->setObjectTextureCoords(texCoord);
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Computes bounding box of sphere.
//
// Use: protected

void
SoSphere::computeBBox(SoAction *, SbBox3f &box, SbVec3f &center)
//
////////////////////////////////////////////////////////////////////////
{
    float	rad = (radius.isIgnored() ? 1.0 : radius.getValue());

    box.setBounds(-rad, -rad, -rad, rad, rad, rad);
    center.setValue(0.0, 0.0, 0.0);
}

@@===========================================================================
@@
@@ This is the main triangle-spewing method for the SoSphere class. It
@@ is used to generate primitives and to render.
@@
@@===========================================================================
@={ SpewTriangles
    int		i, j, k, s_x, s_y, s_z, order, octant;
    float	botWidth, topWidth, yTop, yBot, tmp;
    SbVec3f	vec;
    int		depth;
@?{ GeneratePrimitives
    float	rad, sAvg;
    SbVec4f	tex;
    SbBool	genTexCoords;
    SoPrimitiveVertex	pv;
    const SoTextureCoordinateElement	*tce;
@?}
@?{ RenderGeneric
    float	s, t, sAvg;
@?}

    // Compute depth based on complexity
    depth = computeDepth(action);

@?{ GeneratePrimitives
    rad = (radius.isIgnored() ? 1.0 : radius.getValue());

    // Determine whether we should generate our own texture coordinates
    switch (SoTextureCoordinateElement::getType(action->getState())) {
      case SoTextureCoordinateElement::EXPLICIT:
	genTexCoords = TRUE;
	break;
      case SoTextureCoordinateElement::FUNCTION:
	genTexCoords = FALSE;
	break;
    }

    // If we're not generating our own coordinates, we'll need the
    // texture coordinate element to get coords based on points/normals.
    if (! genTexCoords)
	tce = SoTextureCoordinateElement::getInstance(action->getState());
    else {
	tex[2] = 0.0;
	tex[3] = 1.0;
    }
@?}

    for (octant = 0; octant < 8; octant++) {
	s_x = -(((octant & 01) << 1) - 1);
	s_y = -( (octant & 02)       - 1);
	s_z = -(((octant & 04) >> 1) - 1);
	order = s_x * s_y * s_z;

	for (i = 0; i < depth - 1; i++) {
	    yBot = (float) i      / depth;
	    yTop = (float)(i + 1) / depth;

	    botWidth = 1.0 - yBot;
	    topWidth = 1.0 - yTop;

@?{ Render
	    glBegin(GL_TRIANGLE_STRIP);
@?}
@?{ GeneratePrimitives
	    beginShape(action, TRIANGLE_STRIP);
@?}

	    for (j = 0; j < depth - i; j++) {

		// First vertex
		k = order > 0 ? depth - i - j : j;
		tmp = (botWidth * k) / (depth - i);
		vec.setValue(s_x * tmp, s_y * yBot, s_z * (botWidth - tmp));
		vec.normalize();

@?{ GeneratePrimitives
		if (genTexCoords) {
		    COMPUTE_S_T(vec, tex[0], tex[1]);
		}
		else
		    tex = tce->get(vec * rad, vec);
		pv.setPoint(vec * rad);
		pv.setNormal(vec);
		pv.setTextureCoords(tex);
		shapeVertex(&pv);
@?}
@?{ RenderGeneric
		if (doTextures) {
		    COMPUTE_S_T(vec, s, t);
		    glTexCoord2f(s, t);
		}
		if (sendNormals)
		    glNormal3fv(vec.getValue());
@?}
@?{ RenderNormalsNoTexture
		glNormal3fv(vec.getValue());
@?}    
@?{ Render
		glVertex3fv((vec*rad).getValue());
@?}

		// Second vertex
		k = order > 0 ? (depth - i - 1) - j : j;
		tmp = (topWidth * k) / (depth - i - 1);
		vec.setValue(s_x * tmp, s_y * yTop, s_z * (topWidth - tmp));
		vec.normalize();

@?{ GeneratePrimitives
		if (genTexCoords) {
		    COMPUTE_S_T(vec, tex[0], tex[1]);
		    ADJUST_S(tex[0], octant);
		}
		else
		    tex = tce->get(vec * rad, vec);
		pv.setPoint(vec * rad);
		pv.setNormal(vec);
		pv.setTextureCoords(tex);
		shapeVertex(&pv);
@?}
@?{ RenderGeneric
		if (doTextures) {
		    COMPUTE_S_T(vec, s, t);
		    ADJUST_S(s, octant);
		    glTexCoord2f(s, t);
		}
		if (sendNormals)
		    glNormal3fv(vec.getValue());
@?}
@?{ RenderNormalsNoTexture
		glNormal3fv(vec.getValue());
@?}    
@?{ Render
		glVertex3fv((vec*rad).getValue());
@?}
	    }

	    // Last vertex
	    k = order > 0 ? depth - i - j : j;
	    tmp = (botWidth * k) / (depth - i);
	    vec.setValue(s_x * tmp, s_y * yBot, s_z * (botWidth - tmp));
	    vec.normalize();

@?{ GeneratePrimitives
	    if (genTexCoords) {
		COMPUTE_S_T(vec, tex[0], tex[1]);
	    }
	    else
		tex = tce->get(vec * rad, vec);
	    pv.setPoint(vec * rad);
	    pv.setNormal(vec);
	    pv.setTextureCoords(tex);
	    shapeVertex(&pv);

	    endShape();
@?}
@?{ RenderGeneric
	    if (doTextures) {
		COMPUTE_S_T(vec, s, t);
		ADJUST_S(s, octant);
		glTexCoord2f(s, t);
	    }
	    if (sendNormals)
		glNormal3fv(vec.getValue());
@?}
@?{ RenderNormalsNoTexture
	    glNormal3fv(vec.getValue());
@?}    
@?{ Render
	    glVertex3fv((vec*rad).getValue());

	    glEnd();
@?}
	}

	// Handle the top/bottom polygons specially, to avoid divide by zero
@?{ Render
	glBegin(GL_TRIANGLE_STRIP);
@?}
@?{ GeneratePrimitives
	beginShape(action, TRIANGLE_STRIP);
@?}

	yBot = (float) i / depth;
	yTop = 1.0;
	botWidth = 1 - yBot;

	// First cap vertex
	if (order > 0)
	    vec.setValue(0.0, s_y * yBot, s_z * botWidth);
	else
	    vec.setValue(s_x * botWidth, s_y * yBot, 0.0);
	vec.normalize();

@?{ GeneratePrimitives
	if (genTexCoords) {
	    COMPUTE_S_T(vec, tex[0], tex[1]);
	    ADJUST_S(tex[0], octant);
	    sAvg = tex[0];
	}
	else
	    tex = tce->get(vec * rad, vec);
	pv.setPoint(vec * rad);
	pv.setNormal(vec);
	pv.setTextureCoords(tex);
	shapeVertex(&pv);
@?}
@?{ RenderGeneric
	if (doTextures) {
	    COMPUTE_S_T(vec, s, t);
	    ADJUST_S(s, octant);
	    sAvg = s;
	    glTexCoord2f(s, t);
	}
	if (sendNormals)
	    glNormal3fv(vec.getValue());
@?}
@?{ RenderNormalsNoTexture
	glNormal3fv(vec.getValue());
@?}    
@?{ Render
	glVertex3fv((vec*rad).getValue());
@?}

	// Second cap vertex
	if (order > 0)
	    vec.setValue(s_x * botWidth, s_y * yBot, 0.0);
	else
	    vec.setValue(0.0, s_y * yBot, s_z * botWidth);
	vec.normalize();

@?{ GeneratePrimitives
	if (genTexCoords) {
	    COMPUTE_S_T(vec, tex[0], tex[1]);
	    ADJUST_S(tex[0], octant);
	    sAvg = (sAvg + tex[0]) / 2;
	}
	else
	    tex = tce->get(vec * rad, vec);
	pv.setPoint(vec * rad);
	pv.setNormal(vec);
	pv.setTextureCoords(tex);
	shapeVertex(&pv);
@?}
@?{ RenderGeneric
	if (doTextures) {
	    COMPUTE_S_T(vec, s, t);
	    ADJUST_S(s, octant);
	    sAvg = (sAvg + s) / 2;
	    glTexCoord2f(s, t);
	}
	if (sendNormals)
	    glNormal3fv(vec.getValue());
@?}
@?{ RenderNormalsNoTexture
	glNormal3fv(vec.getValue());
@?}    
@?{ Render
	glVertex3fv((vec*rad).getValue());
@?}

	// Third cap vertex
	vec.setValue(0.0, s_y, 0.0);

@?{ GeneratePrimitives
	if (genTexCoords) {
	    tex[0] = sAvg;
	    tex[1] = s_y * .5 + .5;
	}
	else
	    tex = tce->get(vec * rad, vec);
	pv.setPoint(vec * rad);
	pv.setNormal(vec);
	pv.setTextureCoords(tex);
	shapeVertex(&pv);

	endShape();
@?}
@?{ RenderGeneric
	if (doTextures) {
	    s = sAvg;
	    t = s_y * .5 + .5;
	    glTexCoord2f(s, t);
	}
	if (sendNormals)
	    glNormal3fv(vec.getValue());
@?}
@?{ RenderNormalsNoTexture
	glNormal3fv(vec.getValue());
@?}    
@?{ Render
	glVertex3fv((vec*rad).getValue());

	glEnd();
@?}
    }
@=}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Generates triangles representing a sphere.
//
// Use: protected

void
SoSphere::generatePrimitives(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    @= GeneratePrimitives	true
    @= Render
    @= RenderGeneric
    @= RenderNormalsNoTexture
    @SpewTriangles
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Generic rendering of sphere with or without normals, with or
//    without texture coordinates.
//
// Use: private

void
SoSphere::GLRenderGeneric(SoGLRenderAction *action,
			  SbBool sendNormals, SbBool doTextures)
//
////////////////////////////////////////////////////////////////////////
{
    float rad = (radius.isIgnored() ? 1.0 : radius.getValue());

    @= GeneratePrimitives
    @= Render			true
    @= RenderGeneric		true
    @= RenderNormalsNoTexture
    @SpewTriangles
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Renders sphere with normals and without texture coordinates.
//
// Use: private

void
SoSphere::GLRenderNvertTnone(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    float rad = (radius.isIgnored() ? 1.0 : radius.getValue());

    @= GeneratePrimitives
    @= Render			true
    @= RenderGeneric
    @= RenderNormalsNoTexture	true
    @SpewTriangles
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Computes tesselation depth based on current complexity. Assumes
//    SoComplexityTypeElement and SoComplexityElement are enabled for
//    this action.
//
// Use: private

int
SoSphere::computeDepth(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    float	complexity = SoComplexityElement::get(action->getState());
    int		depth;

    // In screen space, set the tesselation depth based on the
    // complexity and the size of the sphere when projected onto the screen.
    if (SoComplexityTypeElement::get(action->getState()) ==
	SoComplexityTypeElement::SCREEN_SPACE) {

	float	rad = (radius.isIgnored() ? 1.0 : radius.getValue());
	SbVec3f	p(rad, rad, rad);
	SbVec2s	rectSize;
	short	maxSize;

	getScreenSize(action->getState(), SbBox3f(-p, p), rectSize);

	maxSize = (rectSize[0] > rectSize[1] ? rectSize[0] : rectSize[1]);

	depth = 1 + (int) (.1 * complexity * maxSize);
    }

    //
    // In object space, compute tesselation depth based only on
    // complexity value. We want the following correspondences between
    // complexity and depth:
    //		0.0 -> 1
    //		0.5 -> 5
    //		1.0 -> 30
    // So we'll use linear ramps from 0 to .5 and from .5 to 1
    //
    else {
	if (complexity < 0.5)
	    depth = 1   + (int) ((complexity) * 9.0);
	else
	    depth = -20 + (int) ((complexity) * 50.0);
    }

    return depth;
}

#undef COMPUTE_S_T

#ifdef _CRAY
#undef atan2f
#endif
