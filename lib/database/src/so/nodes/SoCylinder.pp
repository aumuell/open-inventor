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
 |      SoCylinder
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
#include <Inventor/details/SoCylinderDetail.h>
#include <Inventor/elements/SoComplexityElement.h>
#include <Inventor/elements/SoComplexityTypeElement.h>
#include <Inventor/elements/SoGLTextureCoordinateElement.h>
#include <Inventor/elements/SoGLTextureEnabledElement.h>
#include <Inventor/elements/SoLightModelElement.h>
#include <Inventor/elements/SoMaterialBindingElement.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/nodes/SoCylinder.h>

SO_NODE_SOURCE(SoCylinder);

// Shorthand for testing whether current parts includes PART
#define HAS_PART(PARTS, PART)	((PARTS & (PART)) != 0)

// Returns S or T texture coord for point on top or bottom of
// cylinder, given x or z coord
#define BOT_TEX_S(x)	((x) * .5 + .5)
#define BOT_TEX_T(z)	((z) * .5 + .5)
#define TOP_TEX_S(x)	BOT_TEX_S(x)
#define TOP_TEX_T(z)	(1.0 - BOT_TEX_T(z))

// Cylinder ring geometry (x,z coords of points around 1 cross-section ring)
SbVec2f		*SoCylinder::coordsArray;	// Ring x,z coordinates
int		 SoCylinder::maxCoords;		// Current size of coord array

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoCylinder::SoCylinder()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoCylinder);
    isBuiltIn = TRUE;

    SO_NODE_ADD_FIELD(parts,  (ALL));
    SO_NODE_ADD_FIELD(radius, (1.0));
    SO_NODE_ADD_FIELD(height, (2.0));

    // Set up static info for enumerated type field
    SO_NODE_DEFINE_ENUM_VALUE(Part, SIDES);
    SO_NODE_DEFINE_ENUM_VALUE(Part, TOP);
    SO_NODE_DEFINE_ENUM_VALUE(Part, BOTTOM);
    SO_NODE_DEFINE_ENUM_VALUE(Part, ALL);

    // Set size of array to 0 so it will be allocated first time
    if (SO_NODE_IS_FIRST_INSTANCE())
	maxCoords = 0;

    // Set up info in enumerated type field
    SO_NODE_SET_SF_ENUM_TYPE(parts, Part);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoCylinder::~SoCylinder()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Turns on a part of the cylinder. (Convenience function.)
//
// Use: public

void
SoCylinder::addPart(SoCylinder::Part part)
//
////////////////////////////////////////////////////////////////////////
{
    parts.setValue(parts.getValue() | part);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Turns off a part of the cylinder. (Convenience function.)
//
// Use: public

void
SoCylinder::removePart(SoCylinder::Part part)
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
SoCylinder::hasPart(SoCylinder::Part part) const
//
////////////////////////////////////////////////////////////////////////
{
    return (parts.getValue() & part);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Performs GL rendering of a cylinder.
//
// Use: private

void
SoCylinder::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // First see if the object is visible and should be rendered now
    if (! shouldGLRender(action))
	return;

    // See if texturing is enabled
    SbBool doTextures = SoGLTextureEnabledElement::get(action->getState());

    // Render the cylinder. The GLRenderGeneric() method handles any
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
SoCylinder::rayPick(SoRayPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // First see if the object is pickable
    if (! shouldRayPick(action))
	return;

    int			curParts =(parts.isIgnored() ? ALL : parts.getValue());
    SbLine		pickLine;
    float		radius, halfHeight;
    SbVec3f		enterPoint, exitPoint, normal;
    SbVec4f		texCoord;
    SoPickedPoint	*pp;
    SoCylinderDetail	*detail;
    SbBool		materialPerPart;
    int			numHits = 0;

    // Compute the picking ray in our current object space
    computeObjectSpaceRay(action);

    // Get size of this cylinder
    getSize(radius, halfHeight);

    // Construct an infinite cylinder to test sides for intersection
    SbCylinder	infiniteCyl;
    infiniteCyl.setRadius(radius);

    SoMaterialBindingElement::Binding mbe =
	SoMaterialBindingElement::get(action->getState());
    materialPerPart =
	(mbe == SoMaterialBindingElement::PER_PART_INDEXED ||
	 mbe == SoMaterialBindingElement::PER_PART);

    // See if the line intersects the cylinder
    if (HAS_PART(curParts, SIDES) &&
	infiniteCyl.intersect(action->getLine(), enterPoint, exitPoint)) {

	// See if the enter point is within the real cylinder and is
	// between the near and far clipping planes.
	if (enterPoint[1] <= halfHeight && enterPoint[1] >= -halfHeight) {

	    numHits++;

	    if (action->isBetweenPlanes(enterPoint) &&
		(pp = action->addIntersection(enterPoint)) != NULL) {

		// The normal at the point is the same as the point but
		// with a 0 y coordinate
		normal.setValue(enterPoint[0], 0.0, enterPoint[2]);
		normal.normalize();
		pp->setObjectNormal(normal);

		texCoord.setValue(atan2f(enterPoint[0], enterPoint[2])
				  * (1.0 / (2.0 * M_PI)) + 0.5,
				  (enterPoint[1] + halfHeight) /
				  (2.0 * halfHeight),
				  0.0, 1.0);
		pp->setObjectTextureCoords(texCoord);

		detail = new SoCylinderDetail();
		detail->setPart(SIDES);
		pp->setDetail(detail, this);
	    }
	}

	// Do same for exit point
	if (exitPoint[1] <= halfHeight && exitPoint[1] >= -halfHeight) {
	    numHits++;

	    if (action->isBetweenPlanes(exitPoint) &&
		(pp = action->addIntersection(exitPoint)) != NULL) {
		normal.setValue(exitPoint[0], 0.0, exitPoint[2]);
		normal.normalize();
		pp->setObjectNormal(normal);
		texCoord.setValue(atan2f(exitPoint[0], exitPoint[2])
				  * (1.0 / (2.0 * M_PI)) + 0.5,
				  (exitPoint[1] + halfHeight) /
				  (2.0 * halfHeight),
				  0.0, 1.0);
		pp->setObjectTextureCoords(texCoord);
		detail = new SoCylinderDetail();
		detail->setPart(SIDES);
		pp->setDetail(detail, this);
	    }
	}
    }

    // If we haven't hit the cylinder twice already, check for an
    // intersection with the top face
    if (numHits < 2 && HAS_PART(curParts, TOP)) {
	SbVec3f	norm(0.0, 1.0, 0.0);

	// Construct a plane containing the top face
	SbPlane	topFacePlane(norm, halfHeight);

	// See if the ray hits this plane
	if (topFacePlane.intersect(action->getLine(), enterPoint)) {

	    // See if the intersection is within the correct radius
	    // and is within the clipping planes
	    float distFromYAxisSquared = (enterPoint[0] * enterPoint[0] +
					  enterPoint[2] * enterPoint[2]);

	    if (distFromYAxisSquared <= radius * radius) {

		numHits++;

		if (action->isBetweenPlanes(enterPoint) &&
		    (pp = action->addIntersection(enterPoint)) != NULL) {
		    pp->setObjectNormal(norm);
		    texCoord.setValue(0.5 + enterPoint[0] / (2.0 * radius),
				      0.5 - enterPoint[2] / (2.0 * radius),
				      0.0, 1.0);
		    pp->setObjectTextureCoords(texCoord);
		    if (materialPerPart)
			pp->setMaterialIndex(1);
		    detail = new SoCylinderDetail();
		    detail->setPart(TOP);
		    pp->setDetail(detail, this);
		}
	    }
	}
    }

    // If we haven't hit the cylinder twice already, check for an
    // intersection with the bottom face
    if (numHits < 2 && HAS_PART(curParts, BOTTOM)) {
	SbVec3f	norm(0.0, -1.0, 0.0);

	// Construct a plane containing the bottom face
	SbPlane		bottomFacePlane(norm, halfHeight);

	// See if the ray hits this plane
	if (bottomFacePlane.intersect(action->getLine(), enterPoint)) {

	    // See if the intersection is within the correct radius
	    // and is within the clipping planes
	    float distFromYAxisSquared = (enterPoint[0] * enterPoint[0] +
					  enterPoint[2] * enterPoint[2]);

	    if (distFromYAxisSquared <= radius * radius &&
		action->isBetweenPlanes(enterPoint) &&
		(pp = action->addIntersection(enterPoint)) != NULL) {
		pp->setObjectNormal(norm);
		texCoord.setValue(0.5 + enterPoint[0] / (2.0 * radius),
				  0.5 + enterPoint[2] / (2.0 * radius),
				  0.0, 1.0);
		pp->setObjectTextureCoords(texCoord);
		if (materialPerPart)
		    pp->setMaterialIndex(2);
		detail = new SoCylinderDetail();
		detail->setPart(BOTTOM);
		pp->setDetail(detail, this);
	    }
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Computes bounding box of cylinder.
//
// Use: protected

void
SoCylinder::computeBBox(SoAction *, SbBox3f &box, SbVec3f &center)
//
////////////////////////////////////////////////////////////////////////
{
    int		curParts = (parts.isIgnored() ? ALL : parts.getValue());

    if (curParts == 0)		// No parts at all!
	box.setBounds(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

    else {
	float	r, h;
	SbVec3f	min, max;

	getSize(r, h);

	if (HAS_PART(curParts, SIDES | TOP))
	    max.setValue( r,  h,  r);
	else
	    max.setValue( r, -h,  r);

	if (HAS_PART(curParts, SIDES | BOTTOM))
	    min.setValue(-r, -h, -r);
	else
	    min.setValue(-r,  h, -r);

	box.setBounds(min, max);
    }

    center.setValue(0.0, 0.0, 0.0);
}

@@===========================================================================
@@
@@ Local definitions used with SpewTriangles to set up and send vertices.
@@
@@===========================================================================
@={ SendSideVertices
    pt[0] = ringCoords[side][0];
    pt[2] = ringCoords[side][1];

    // Deal with normal
    norm.setValue(pt[0], 0.0, pt[2]);
@?{ GeneratePrimitives
    pv.setNormal(norm);
@?}
@?{ RenderGeneric
    if (sendNormals)
	glNormal3fv(norm.getValue());
@?}
@?{ RenderNormalsNoTexture
    glNormal3fv(norm.getValue());
@?}

    // Point at bottom of section
    pt[1] = yBot;
@?{ GeneratePrimitives
    pt[0] *= radius;
    pt[1] *= halfHeight;
    pt[2] *= radius;

    if (genTexCoords) {
	tex[0] = s;
	tex[1] = tBot;
    }
    else
	tex = tce->get(pt, norm);
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
    pt[1] = yTop;
@?{ GeneratePrimitives
    pt[1] *= halfHeight;
    if (genTexCoords) {
	tex[0] = s;
	tex[1] = tTop;
    }
    else
	tex = tce->get(pt, norm);
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
@={ SendTopVertex
@?{ GeneratePrimitives
    pt[0] *= radius;
    pt[2] *= radius;
    if (genTexCoords) {
	tex[0] = TOP_TEX_S(pt[0]);
	tex[1] = TOP_TEX_T(pt[2]);
    }
    else
	tex = tce->get(pt, norm);
    pv.setPoint(pt);
    pv.setTextureCoords(tex);
    shapeVertex(&pv);
@?}
@?{ RenderGeneric
    if (doTextures)
	glTexCoord2f(TOP_TEX_S(pt[0]), TOP_TEX_T(pt[2]));
    glVertex3fv(SCALE(pt).getValue());
@?}
@?{ RenderNormalsNoTexture
    glVertex3fv(SCALE(pt).getValue());
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
@@ This is the main triangle-spewing method for the SoCylinder class. It
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
    SbVec2f		*ringCoords;
    SbVec3f		pt, norm;
@?{ Render
    SoMaterialBundle	mb(action);
@?}
@?{ GeneratePrimitives
    float		radius, halfHeight;
    SbVec4f		tex;
    SbBool		genTexCoords;
    SoPrimitiveVertex	pv;
    SoCylinderDetail	detail;
    const SoTextureCoordinateElement	*tce;
@?}

    SoMaterialBindingElement::Binding mbe =
	SoMaterialBindingElement::get(action->getState());
    materialPerPart =
	(mbe == SoMaterialBindingElement::PER_PART_INDEXED ||
	 mbe == SoMaterialBindingElement::PER_PART);

    curParts = (parts.isIgnored() ? ALL : parts.getValue());

    // Compute number of sides and sections to use to represent
    // cylinder, then compute ring of x,z coordinates around cylinder
    // and store in ringCoords.
    computeRing(action, numSides, numSections, ringCoords);

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

    if (HAS_PART(curParts, SIDES)) {

	// Draw each section of sides as a triangle mesh, from top to bottom
	yTop = 1.0;
	dy   = -2.0 / numSections;
@?{ DoTextures
	tTop = 1.0;
	dt   = -1.0 / numSections;
	ds   = -1.0 / numSides;
@?}

	for (section = 0; section < numSections; section++) {

	    yBot = yTop + dy;

@?{ DoTextures
	    tBot = tTop + dt;
	    s    = 1.0;
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
	    s = 0.0;
@?}
	    @ SendSideVertices

@?{ Render
	    glEnd();
@?}
@?{ GeneratePrimitives
	    endShape();
@?}

	    // Prepare for next section down
	    yTop = yBot;
@?{ DoTextures
	    tTop = tBot;
@?}
	}
    }

    // Draw top face as a series of concentric rings. The number of
    // rings is the same as the number of sections of the sides of the
    // cylinder.
    if (HAS_PART(curParts, TOP)) {
	norm.setValue(0.0, 1.0, 0.0);
@?{ GeneratePrimitives
	pt[1] = halfHeight;
@?}
@?{ Render
	pt[1] = 1.0;
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
	detail.setPart(TOP);
@?}

	// Start at the outside and work in
	outerRadius = 1.0;
	dRadius     = -1.0 / numSections;
	for (section = numSections - 1; section >= 0; --section) {

	    innerRadius = outerRadius + dRadius;

	    // Innermost ring is treated as a triangle fan. This not
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

		// Send all vertices around ring. Go in reverse order
		// so that vertex ordering is correct
		for (side = numSides - 1; side >= 0; side--) {
		    pt[0] = outerRadius * ringCoords[side][0];
		    pt[2] = outerRadius * ringCoords[side][1];
		    @ SendTopVertex
		}
		// Send first vertex again
		pt[0] = outerRadius * ringCoords[numSides - 1][0];
		pt[2] = outerRadius * ringCoords[numSides - 1][1];
		@ SendTopVertex

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

		for (side = 0; side < numSides; side++) {
		    // Send points on outer and inner rings
		    pt[0] = outerRadius * ringCoords[side][0];
		    pt[2] = outerRadius * ringCoords[side][1];
		    @ SendTopVertex
		    pt[0] = innerRadius * ringCoords[side][0];
		    pt[2] = innerRadius * ringCoords[side][1];
		    @ SendTopVertex
		}

		// Join end of strip back to beginning
		pt[0] = outerRadius * ringCoords[0][0];
		pt[2] = outerRadius * ringCoords[0][1];
		@ SendTopVertex
		pt[0] = innerRadius * ringCoords[0][0];
		pt[2] = innerRadius * ringCoords[0][1];
		@ SendTopVertex

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

    // Draw bottom face the same way as the top
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
	    mb.send(2, FALSE);
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
	    pv.setMaterialIndex(2);
	pv.setNormal(norm);
	detail.setPart(BOTTOM);
@?}

	// Start at the outside and work in
	outerRadius = 1.0;
	dRadius     = -1.0 / numSections;
	for (section = numSections - 1; section >= 0; --section) {

	    innerRadius = outerRadius + dRadius;

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
		    pt[0] = outerRadius * ringCoords[side][0];
		    pt[2] = outerRadius * ringCoords[side][1];
		    @ SendBotVertex
		}
		// Send first vertex again
		pt[0] = outerRadius * ringCoords[0][0];
		pt[2] = outerRadius * ringCoords[0][1];
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
		    pt[0] = outerRadius * ringCoords[side][0];
		    pt[2] = outerRadius * ringCoords[side][1];
		    @ SendBotVertex
		    pt[0] = innerRadius * ringCoords[side][0];
		    pt[2] = innerRadius * ringCoords[side][1];
		    @ SendBotVertex
		}

		// Join end of strip back to beginning
		side = numSides - 1;
		pt[0] = outerRadius * ringCoords[side][0];
		pt[2] = outerRadius * ringCoords[side][1];
		@ SendBotVertex
		pt[0] = innerRadius * ringCoords[side][0];
		pt[2] = innerRadius * ringCoords[side][1];
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
//    Generates triangles representing a cylinder.
//
// Use: protected

void
SoCylinder::generatePrimitives(SoAction *action)
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
//    Generic rendering of cylinder with or without normals, with or
//    without texture coordinates.
//
// Use: private

void
SoCylinder::GLRenderGeneric(SoGLRenderAction *action,
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
//    Renders cylinder with normals and without texture coordinates.
//
// Use: private

void
SoCylinder::GLRenderNvertTnone(SoGLRenderAction *action)
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
//    coordinates around cylinder and stores in ringCoords.
//
// Use: private

void
SoCylinder::computeRing(SoAction *action, int &numSides, int &numSections,
			SbVec2f *&ringCoords) const
//
////////////////////////////////////////////////////////////////////////
{
    float	complexity = SoComplexityElement::get(action->getState());
    float	theta, dTheta;
    int		side;

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
    // complexity and the size of the cylinder when projected onto the screen.
    else {
	SbVec2s		rectSize;
	short		maxSize;
	float		radius, halfHeight;

	getSize(radius, halfHeight);
	SbVec3f		p(radius, halfHeight, radius);

	getScreenSize(action->getState(), SbBox3f(-p, p), rectSize);

	maxSize = (rectSize[0] > rectSize[1] ? rectSize[0] : rectSize[1]);

	numSections = 1 + (int) (0.2  * complexity * maxSize);
	numSides    = 3 + (int) (0.25 * complexity * maxSize);
    }

    // Make sure the current storage for ring coordinates is big enough
    if (numSides > maxCoords) {

	if (maxCoords > 0)
	    delete [] coordsArray;

	maxCoords = numSides;

	coordsArray = new SbVec2f[maxCoords];
    }

    ringCoords = coordsArray;

    // Compute x and z coordinates around ring
    theta  = 0.0;
    dTheta = 2.0 * M_PI / numSides;
    for (side = 0; side < numSides; side++) {
	ringCoords[side].setValue(sin(theta), -cos(theta));
	theta += dTheta;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Computes real radius and half-height.
//
// Use: private

void
SoCylinder::getSize(float &rad, float &hHeight) const
//
////////////////////////////////////////////////////////////////////////
{
    rad     = (radius.isIgnored() ? 1.0 : radius.getValue());
    hHeight = (height.isIgnored() ? 1.0 : height.getValue() / 2.0);
}
