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
 |      SoCone
 |
@@ This next line is true for the output file only - ignore it here:
 |   Note: This file was preprocessed from another file. Do not edit it.
 |
 |   Texture coordinates on the cone are defined as:
 |
 |	Sides:	S ranges from 0 to 1, ccw, with the seam at the -z axis
 |		T ranges from 1 at the top to 0 at the bottom
 |
 |	Bottom: a circle is cut from the center of the texture square
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
#include <Inventor/details/SoConeDetail.h>
#include <Inventor/elements/SoComplexityElement.h>
#include <Inventor/elements/SoComplexityTypeElement.h>
#include <Inventor/elements/SoGLTextureCoordinateElement.h>
#include <Inventor/elements/SoGLTextureEnabledElement.h>
#include <Inventor/elements/SoLightModelElement.h>
#include <Inventor/elements/SoMaterialBindingElement.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/nodes/SoCone.h>

SO_NODE_SOURCE(SoCone);

// Shorthand for testing whether current parts includes PART
#define HAS_PART(PARTS, PART)	((PARTS & (PART)) != 0)

// Returns S or T texture coord for point on bottom of cone, given x
// or z coord
#define BOT_TEX_S(x)	((x) * .5 + .5)
#define BOT_TEX_T(z)	((z) * .5 + .5)

// Cone ring geometry (x,z coords of points around 1 cross-section ring)
SbVec2f		*SoCone::coordsArray;		// Ring x,z coordinates
SbVec3f		*SoCone::normalsArray;		// Ring normals
int		 SoCone::maxCoords;		// Current size of arrays

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoCone::SoCone()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoCone);
    isBuiltIn = TRUE;

    SO_NODE_ADD_FIELD(parts,		(ALL));
    SO_NODE_ADD_FIELD(bottomRadius,	(1.0));
    SO_NODE_ADD_FIELD(height,	(2.0));

    // Set up static info for enumerated type field
    SO_NODE_DEFINE_ENUM_VALUE(Part, SIDES);
    SO_NODE_DEFINE_ENUM_VALUE(Part, BOTTOM);
    SO_NODE_DEFINE_ENUM_VALUE(Part, ALL);

    // Set up info in enumerated type field
    SO_NODE_SET_SF_ENUM_TYPE(parts, Part);

    // Set size of arrays to 0 so they will be allocated first time
    if (SO_NODE_IS_FIRST_INSTANCE())
	maxCoords = 0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoCone::~SoCone()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Turns on a part of the cone. (Convenience function.)
//
// Use: public

void
SoCone::addPart(SoCone::Part part)
//
////////////////////////////////////////////////////////////////////////
{
    parts.setValue(parts.getValue() | part);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Turns off a part of the cone. (Convenience function.)
//
// Use: public

void
SoCone::removePart(SoCone::Part part)
//
////////////////////////////////////////////////////////////////////////
{
    parts.setValue(parts.getValue() & ~part);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns whether a given part is on or off. (Convenience function.)
//
// Use: public

SbBool
SoCone::hasPart(SoCone::Part part) const
//
////////////////////////////////////////////////////////////////////////
{
    return HAS_PART(parts.getValue(), part);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Performs GL rendering of a cone.
//
// Use: protected

void
SoCone::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // First see if the object is visible and should be rendered now
    if (! shouldGLRender(action))
	return;

    // See if texturing is enabled
    SbBool doTextures = SoGLTextureEnabledElement::get(action->getState());

    // Render the cone. The GLRenderGeneric() method handles any
    // case. The GLRenderNvertTnone() handles the case where we are
    // outputting normals but no texture coordinates. This case is
    // handled separately since it occurs often and warrants its own
    // method.
    SbBool sendNormals = (SoLightModelElement::get(action->getState()) !=
			  SoLightModelElement::BASE_COLOR);
    if (! doTextures && sendNormals)
	GLRenderNvertTnone(action);
    else
	GLRenderGeneric(action, sendNormals, doTextures);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements ray picking
//
// Use: protected

void
SoCone::rayPick(SoRayPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // First see if the object is pickable
    if (! shouldRayPick(action))
	return;

    int			curParts =(parts.isIgnored() ? ALL : parts.getValue());
    SbMatrix		matrix, matrix2;
    float		radius, halfHeight;
    SbVec3f		enterPoint, exitPoint, objectPoint, normal;
    SbVec4f		texCoord;
    SoPickedPoint	*pp;
    SoConeDetail	*detail;
    SbBool		materialPerPart;
    int			numHits = 0;

    // Get size of this cone
    getSize(radius, halfHeight);

    // Compute a matrix that will transform a canonical cone (apex at
    // the origin, bottom radius 1) to this cone
    matrix.setTranslate(SbVec3f(0.0, halfHeight, 0.0));
    matrix2.setScale(SbVec3f(radius, 2.0 * halfHeight, radius));
    matrix.multLeft(matrix2);

    // Compute the object-space picking ray, using the matrix we
    // computed in addition to the one stored in the action
    computeObjectSpaceRay(action, matrix);

    SoMaterialBindingElement::Binding mbe =
	SoMaterialBindingElement::get(action->getState());
    materialPerPart =
	(mbe == SoMaterialBindingElement::PER_PART_INDEXED ||
	 mbe == SoMaterialBindingElement::PER_PART);

    // See if the line intersects an infinite cone
    if (HAS_PART(curParts, SIDES) &&
	intersectInfiniteCone(action->getLine(), enterPoint, exitPoint)) {

        // Convert the point into object space from "canonical" space
	matrix.multVecMatrix(enterPoint, objectPoint);

	// See if the enter point is within the part of the cone we
	// are interested in: between the apex (at y = 0) and the base
	// (at y = -1). Also check if the intersection is between the
	// near and far clipping planes.

	if (enterPoint[1] <= 0.0 && enterPoint[1] >= -1.0) {

	    numHits++;

	    if (action->isBetweenPlanes(objectPoint) &&
		(pp = action->addIntersection(objectPoint)) != NULL) {

		// The normal is perpendicular to the vector V from the
		// apex to the intersection point in the plane containing
		// both  V and the y-axis. Using a couple of cross
		// products, we arrive at the following result.
		// (First, translate the cone point back down as if the
		// center were at the origin, making the math easier.)
		objectPoint[1] -= halfHeight;
		normal.setValue(-objectPoint[0] * objectPoint[1],
				 objectPoint[0] * objectPoint[0] + 
				 objectPoint[2] * objectPoint[2],
				-objectPoint[1] * objectPoint[2]);
		normal.normalize();
		pp->setObjectNormal(normal);

		texCoord.setValue(atan2f(enterPoint[0], enterPoint[2])
				  * (1.0 / (2.0 * M_PI)) + 0.5,
				  enterPoint[1] + 1.0,
				  0.0, 1.0);
		pp->setObjectTextureCoords(texCoord);

		detail = new SoConeDetail();
		detail->setPart(SIDES);
		pp->setDetail(detail, this);
	    }
	}

	// Do same for exit point

	// Convert the point into object space from "canonical" space
	matrix.multVecMatrix(exitPoint, objectPoint);

	if (exitPoint[1] <= 0.0 && exitPoint[1] >= -1.0) {

	    numHits++;

	    if (action->isBetweenPlanes(objectPoint) &&
		(pp = action->addIntersection(objectPoint)) != NULL) {

		// The normal is perpendicular to the vector V from the
		// apex to the intersection point in the plane containing
		// both  V and the y-axis. Using a couple of cross
		// products, we arrive at the following result.
		// (First, translate the cone point back down as if the
		// apex were at the origin, making the math easier.)
		objectPoint[1] -= halfHeight;
		normal.setValue(-objectPoint[0] * objectPoint[1],
				 objectPoint[0] * objectPoint[0] + 
				 objectPoint[2] * objectPoint[2],
				-objectPoint[1] * objectPoint[2]);
		normal.normalize();
		pp->setObjectNormal(normal);

		texCoord.setValue(atan2f(exitPoint[0], exitPoint[2])
				  * (1.0 / (2.0 * M_PI)) + 0.5,
				  exitPoint[1] + 1.0,
				  0.0, 1.0);
		pp->setObjectTextureCoords(texCoord);

		detail = new SoConeDetail();
		detail->setPart(SIDES);
		pp->setDetail(detail, this);
	    }
	}
    }

    // If we haven't hit the cone twice already, check for an
    // intersection with the bottom face
    if (numHits < 2 && HAS_PART(curParts, BOTTOM)) {
	SbVec3f	norm(0.0, -1.0, 0.0);

	// Construct a plane containing the bottom face (in canonical space)
	SbPlane		bottomFacePlane(norm, 1.0);

	// See if the ray hits this plane
	if (bottomFacePlane.intersect(action->getLine(), enterPoint)) {

	    // Convert the point into object space from "canonical" space
	    matrix.multVecMatrix(enterPoint, objectPoint);

	    // See if the intersection is within the correct radius
	    // and is within the clipping planes
	    float distFromYAxisSquared = (enterPoint[0] * enterPoint[0] +
					  enterPoint[2] * enterPoint[2]);

	    if (distFromYAxisSquared <= 1.0 &&
		action->isBetweenPlanes(objectPoint) &&
		(pp = action->addIntersection(objectPoint)) != NULL) {

		pp->setObjectNormal(norm);

		texCoord.setValue(0.5 + enterPoint[0] / 2.0,
				  0.5 + enterPoint[2] / 2.0,
				  0.0, 1.0);
		pp->setObjectTextureCoords(texCoord);

		if (materialPerPart)
		    pp->setMaterialIndex(1);

		detail = new SoConeDetail();
		detail->setPart(BOTTOM);
		pp->setDetail(detail, this);
	    }
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Computes bounding box of cone.
//
// Use: protected

void
SoCone::computeBBox(SoAction *, SbBox3f &box, SbVec3f &center)
//
////////////////////////////////////////////////////////////////////////
{
    int		curParts = (parts.isIgnored() ? ALL : parts.getValue());

    if (curParts == 0)		// No parts at all!
	box.setBounds(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

    else {
	float	br, h;
	SbVec3f	min, max;

	getSize(br, h);

	min.setValue(-br, -h, -br);

	if (HAS_PART(curParts, SIDES))
	    max.setValue(br,  h,  br);
	else
	    max.setValue(br, -h,  br);

	box.setBounds(min, max);
    }

    center.setValue(0.0, 0.0, 0.0);
}

@@===========================================================================
@@
@@ Local definitions used with SpewTriangles to set up and send vertices.
@@
@@===========================================================================
@@
@={ SendSideVertices
    // Deal with normal
@?{ GeneratePrimitives
    pv.setNormal(sideNormals[side]);
@?}
@?{ RenderGeneric
    if (sendNormals)
	glNormal3fv(sideNormals[side].getValue());
@?}
@?{ RenderNormalsNoTexture
    glNormal3fv(sideNormals[side].getValue());
@?}

    // Point at bottom of section
    pt.setValue(outerRadius * baseCoords[side][0], yBot,
		outerRadius * baseCoords[side][1]);
@?{ GeneratePrimitives
    pt[0] *= radius;
    pt[1] *= halfHeight;
    pt[2] *= radius;
    if (genTexCoords) {
	tex[0] = s;
	tex[1] = tBot;
    }
    else
	tex = tce->get(pt, sideNormals[side]);
    pv.setPoint(pt);
    pv.setTextureCoords(tex);
    shapeVertex(&pv);
@?}
@?{ RenderGeneric
    if (doTextures)
	glTexCoord2f(s, tBot);
    glVertex3fv(SCALE(pt).getValue());
@?}
@?{ RenderNormalsNoTexture
    glVertex3fv(SCALE(pt).getValue());
@?}

    // Point at top of section
    pt.setValue(innerRadius * baseCoords[side][0], yTop,
		innerRadius * baseCoords[side][1]);
@?{ GeneratePrimitives
    pt[0] *= radius;
    pt[1] *= halfHeight;
    pt[2] *= radius;
    if (genTexCoords) {
	tex[0] = s;
	tex[1] = tTop;
    }
    else
	tex = tce->get(pt, sideNormals[side]);
    pv.setPoint(pt);
    pv.setTextureCoords(tex);
    shapeVertex(&pv);
@?}
@?{ RenderGeneric
    if (doTextures)
	glTexCoord2f(s, tTop);
    glVertex3fv(SCALE(pt).getValue());
@?}
@?{ RenderNormalsNoTexture
    glVertex3fv(SCALE(pt).getValue());
@?}
@?{ DoTextures
    s += ds;
@?}
@=}
@@
@@===========================================================================
@@
@={ SendBotVertex
@?{ GeneratePrimitives
    pt[0] *= radius;
    pt[2] *= radius;
    if (genTexCoords) {
	tex[0] = BOT_TEX_S(pt[0]);
	tex[1] = BOT_TEX_T(pt[2]);
    }
    else
	tex = tce->get(pt, norm);
    pv.setPoint(pt);
    pv.setTextureCoords(tex);
    shapeVertex(&pv);
@?}
@?{ RenderGeneric
    if (doTextures)
	glTexCoord2f(BOT_TEX_S(pt[0]), BOT_TEX_T(pt[2]));
    glVertex3fv(SCALE(pt).getValue());
@?}
@?{ RenderNormalsNoTexture
    glVertex3fv(SCALE(pt).getValue());
@?}
@=}
@@
@@===========================================================================
@@
@@ This is the main triangle-spewing method for the SoCone class. It
@@ is used to generate primitives and to render.
@@
@@===========================================================================
@={ SpewTriangles
    SbBool		materialPerPart;
    int			curParts, numSides, numSections, side, section;
    float		yTop, yBot, dy;
@?{ DoTextures
    float		s, ds, tTop, tBot, dt;
@?}
    float		outerRadius, innerRadius, dRadius;
    SbVec2f		*baseCoords;
    SbVec3f		*sideNormals, pt, norm;
@?{ Render
    SoMaterialBundle	mb(action);
@?}
@?{ GeneratePrimitives
    float		radius, halfHeight;
    SbVec4f		tex;
    SbBool		genTexCoords;
    SoPrimitiveVertex	pv;
    SoConeDetail	detail;
    const SoTextureCoordinateElement	*tce;
@?}

    SoMaterialBindingElement::Binding mbe =
	SoMaterialBindingElement::get(action->getState());
    materialPerPart =
	(mbe == SoMaterialBindingElement::PER_PART_INDEXED ||
	 mbe == SoMaterialBindingElement::PER_PART);

    curParts = (parts.isIgnored() ? ALL : parts.getValue());

    // Compute number of sides and sections to use to represent
    // cone, then compute ring of x,z coordinates around cone
    // and store in baseCoords.
    computeBase(action, numSides, numSections, baseCoords, sideNormals);

@?{ Render
    // Make sure first material is sent if necessary
    mb.sendFirst();
@?}
@?{ GeneratePrimitives
    pv.setDetail(&detail);

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

    getSize(radius, halfHeight);
@?}

    dRadius = 1.0 / numSections;

    if (HAS_PART(curParts, SIDES)) {

	// Draw each section of sides as a triangle mesh, from top to bottom
	yTop = 1.0;
	dy   = -2.0 / numSections;
@?{ DoTextures
	tTop = 1.0;
	dt   = -1.0 / numSections;
	ds   =  1.0 / numSides;
@?}

	innerRadius = 0.0;

	for (section = 0; section < numSections; section++) {

	    outerRadius = innerRadius + dRadius;
	    yBot = yTop + dy;

@?{ DoTextures
	    tBot = tTop + dt;
	    s    = 0.0;
@?}

@?{ Render
	    glBegin(GL_TRIANGLE_STRIP);
@?}
@?{ GeneratePrimitives
	    detail.setPart(SIDES);

	    beginShape(action, TRIANGLE_STRIP);
@?}

	    for (side = 0; side < numSides; side++) {
		@ SendSideVertices
	    }

	    // Join end of strip back to beginning
	    side = 0;
@?{ DoTextures
	    s = 1.0;
@?}
	    @ SendSideVertices

@?{ Render
	    glEnd();
@?}
@?{ GeneratePrimitives
	    endShape();
@?}

	    // Prepare for next section down
	    innerRadius = outerRadius;
	    yTop = yBot;
@?{ DoTextures
	    tTop = tBot;
@?}
	}
    }

    // Draw bottom face as a series of concentric rings. The number of
    // rings is the same as the number of sections of the sides of the
    // cone.
    if (HAS_PART(curParts, BOTTOM)) {
	norm.setValue(0.0, -1.0, 0.0);
@?{ GeneratePrimitives
	pt[1] = -halfHeight;
@?}
@?{ Render
	pt[1] = -1.0;
@?}

@?{ Render
	if (materialPerPart)
	    mb.send(1, FALSE);
@?}
@?{ RenderGeneric
	if (sendNormals)
	    glNormal3fv(norm.getValue());
@?}
@?{ RenderNormalsNoTexture
	glNormal3fv(norm.getValue());
@?}
@?{ GeneratePrimitives
	if (materialPerPart)
	    pv.setMaterialIndex(1);
	pv.setNormal(norm);
	detail.setPart(BOTTOM);
@?}

	// Start at the outside and work in
	outerRadius = 1.0;
	for (section = numSections - 1; section >= 0; --section) {

	    innerRadius = outerRadius - dRadius;

	    // Innermost ring is drawn as a triangle fan. This not
	    // only gets better shading (because the center vertex is
	    // sent), but also avoids the problem of having a polygon
	    // with too many vertices.
	    if (section == 0) {
@?{ Render
		glBegin(GL_TRIANGLE_FAN);
@?}
@?{ GeneratePrimitives
		beginShape(action, TRIANGLE_FAN);
@?}

		// Center point comes first
		pt[0] = pt[2] = 0.0;
@?{ GeneratePrimitives
		if (genTexCoords)
		    tex[0] = tex[1] = 0.5;
		else
		    tex = tce->get(norm, norm);
		pv.setPoint(pt);
		pv.setTextureCoords(tex);
		shapeVertex(&pv);
@?}
@?{ RenderGeneric
		if (doTextures)
		    glTexCoord2f(0.5, 0.5);
@?}
@?{ Render
		glVertex3fv(SCALE(pt).getValue());
@?}

		// Send all vertices around ring
		for (side = 0; side < numSides; side++) {
		    pt[0] = outerRadius * baseCoords[side][0];
		    pt[2] = outerRadius * baseCoords[side][1];
		    @ SendBotVertex
		}
		// Send first vertex again
		pt[0] = outerRadius * baseCoords[0][0];
		pt[2] = outerRadius * baseCoords[0][1];
		@ SendBotVertex

@?{ Render
		glEnd();
@?}
@?{ GeneratePrimitives
		endShape();
@?}
	    }

	    // Other rings are triangle strips
	    else {
@?{ Render
		glBegin(GL_TRIANGLE_STRIP);
@?}
@?{ GeneratePrimitives
		beginShape(action, TRIANGLE_STRIP);
@?}

		// Go in reverse order so that vertex ordering is correct
		for (side = numSides - 1; side >= 0; side--) {
		    // Send points on outer and inner rings
		    pt[0] = outerRadius * baseCoords[side][0];
		    pt[2] = outerRadius * baseCoords[side][1];
		    @ SendBotVertex
		    pt[0] = innerRadius * baseCoords[side][0];
		    pt[2] = innerRadius * baseCoords[side][1];
		    @ SendBotVertex
		}

		// Join end of strip back to beginning
		side = numSides - 1;
		pt[0] = outerRadius * baseCoords[side][0];
		pt[2] = outerRadius * baseCoords[side][1];
		@ SendBotVertex
		pt[0] = innerRadius * baseCoords[side][0];
		pt[2] = innerRadius * baseCoords[side][1];
		@ SendBotVertex

@?{ Render
		glEnd();
@?}
@?{ GeneratePrimitives
		endShape();
@?}

		// Prepare for next ring
		outerRadius = innerRadius;
	    }
	}
    }
@=}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Generates triangles representing a cone.
//
// Use: protected

void
SoCone::generatePrimitives(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    @= GeneratePrimitives	true
    @= DoTextures		true
    @= Render
    @= RenderGeneric
    @= RenderNormalsNoTexture
    @SpewTriangles
}

//
// Macro to multiply out coordinates to avoid extra GL calls:
//
#define SCALE(pt) (tmp[0] = (pt)[0]*scale[0], tmp[1] = (pt)[1]*scale[1], \
		   tmp[2] = (pt)[2]*scale[2], tmp)

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Generic rendering of cone with or without normals, with or
//    without texture coordinates.
//
// Use: private

void
SoCone::GLRenderGeneric(SoGLRenderAction *action,
			SbBool sendNormals, SbBool doTextures)
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f scale, tmp;
    getSize(scale[0], scale[1]);
    scale[2] = scale[0];

    @= GeneratePrimitives
    @= DoTextures		true
    @= Render			true
    @= RenderGeneric		true
    @= RenderNormalsNoTexture
    @SpewTriangles
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Renders cone with normals and without texture coordinates.
//
// Use: private

void
SoCone::GLRenderNvertTnone(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f scale, tmp;
    getSize(scale[0], scale[1]);
    scale[2] = scale[0];

    @= GeneratePrimitives
    @= DoTextures
    @= Render			true
    @= RenderGeneric
    @= RenderNormalsNoTexture	true
    @SpewTriangles
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Computes number of sides per circular cross-section and number
//    of sections, based on complexity, then computes ring of x,z
//    coordinates around base of cone and stores in baseCoords. It
//    computes and stores normals in sideNormals, too.
//
// Use: private

void
SoCone::computeBase(SoAction *action, int &numSides, int &numSections,
		    SbVec2f *&baseCoords, SbVec3f *&sideNormals) const
//
////////////////////////////////////////////////////////////////////////
{
    float	complexity = SoComplexityElement::get(action->getState());
    float	theta, dTheta, cosTheta, sinTheta, t1, t2;
    int		side;

    float	radius, halfHeight;
    getSize(radius, halfHeight);
    float	height = 2*halfHeight;

    // In object space, just base number of divisions on complexity
    if (SoComplexityTypeElement::get(action->getState()) ==
	SoComplexityTypeElement::OBJECT_SPACE) {

	// If complexity is between 0 and .5 (inclusive), use 1 section
	// and between 3 and 16 sides:
	if (complexity <= 0.5) {
	    numSections = 1;
	    numSides    = (int) (complexity * 26.0 + 3.0);
	}

	// If complexity is between .5 and 1, use between 1 and 8 sections
	// and between 16 and 64 sides:
	else {
	    numSections = (int) (14.0 * complexity - 6.0);
	    numSides    = (int) (complexity * 96.0 - 32.0);
	}
    }

    // In screen space, set the number of sides/sections based on the
    // complexity and the size of the cone when projected onto the screen.
    else {
	SbVec2s		rectSize;
	short		maxSize;

	SbVec3f		p(radius, halfHeight, radius);

	getScreenSize(action->getState(), SbBox3f(-p, p), rectSize);

	maxSize = (rectSize[0] > rectSize[1] ? rectSize[0] : rectSize[1]);

	numSections = 1 + (int) (0.2  * complexity * maxSize);
	numSides    = 3 + (int) (0.25 * complexity * maxSize);
    }

    // Make sure the current storage for base coordinates is big enough
    if (numSides > maxCoords) {

	if (maxCoords > 0) {
	    delete [] coordsArray;
	    delete [] normalsArray;
	}

	maxCoords = numSides;

	coordsArray  = new SbVec2f[maxCoords];
	normalsArray = new SbVec3f[maxCoords];
    }

    baseCoords  = coordsArray;
    sideNormals = normalsArray;

    // Compute x and z coordinates around base
    theta  = 0.0;
    dTheta = 2.0 * M_PI / numSides;

    // Looking at the XY silhouette of the cone, (t1,t2) is the normal
    // in the XY plane.
    t1 = radius / sqrt(radius*radius + height*height);
    t2 = height / sqrt(radius*radius + height*height);

    for (side = 0; side < numSides; side++) {
	cosTheta = cos(theta);
	sinTheta = sin(theta);
	
	// Theta == 0 generates a point down the -Z axis, which
	// explains the weird (sinTheta, -cosTheta)...
	baseCoords[side].setValue(sinTheta, -cosTheta);
	sideNormals[side].setValue(t2 * sinTheta, t1, -t2 * cosTheta);
	theta += dTheta;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Computes real bottom radius, half-height.
//
// Use: private

void
SoCone::getSize(float &botRad, float &hHeight) const
//
////////////////////////////////////////////////////////////////////////
{
    botRad  = (bottomRadius.isIgnored() ? 1.0 : bottomRadius.getValue());
    hHeight = (      height.isIgnored() ? 1.0 :       height.getValue() / 2.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Computes intersection of given ray with an infinite cone with
//    its apex at (0,0,0) and a cross-section of radius 1 at y = -1.
//    The equation of this cone is   x*x - y*y + z*z = 0. This returns
//    FALSE if no intersection was found. Otherwise, it fills in
//    enterPoint and exitPoint with the two intersections, ordered by
//    distance from the start of the ray.
//
// Use: private

SbBool
SoCone::intersectInfiniteCone(const SbLine &ray,
			      SbVec3f &enterPoint, SbVec3f &exitPoint) const
//
////////////////////////////////////////////////////////////////////////
{
    const SbVec3f	&pos = ray.getPosition();
    const SbVec3f	&dir = ray.getDirection();
    float		a, b, c, discriminant, sqroot, t0, t1;

    // The equation of the ray is I = pos + t * dir, where "pos" is the
    // starting position and "dir" is the direction.

    // Substituting the intersection point "I" into the equation of
    // the cone gives us a quadratic, whose a, b, and c coefficients
    // are as follows
    a =  dir[0] * dir[0] - dir[1] * dir[1] + dir[2] * dir[2];
    b = (pos[0] * dir[0] - pos[1] * dir[1] + pos[2] * dir[2]) * 2.0;
    c =  pos[0] * pos[0] - pos[1] * pos[1] + pos[2] * pos[2];

    // If the discriminant of the quadratic is negative, there's no
    // intersection
    discriminant = b * b - 4.0 * a * c;
    if (discriminant < 0.0)
	return FALSE;

    sqroot = sqrtf(discriminant);

    // Some Magic to stabilize the answer
    if (b > 0.0) {
	t0 = -(2.0 * c) / (sqroot + b);
	t1 = -(sqroot + b) / (2.0 * a);
    }
    else {
	t0 = (2.0 * c) / (sqroot - b);
	t1 = (sqroot - b) / (2.0 * a);
    }	    

    enterPoint = pos + t0 * dir;
    exitPoint  = pos + t1 * dir;

    return TRUE;
}
