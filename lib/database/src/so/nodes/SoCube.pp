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
 |      SoCube
 |
@@ This next line is true for the output file only - ignore it here:
 |   Note: This file was preprocessed from another file. Do not edit it.
 |
 |   Author(s)          : Paul S. Strauss, Thad Beier
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
#include <Inventor/details/SoCubeDetail.h>
#include <Inventor/elements/SoComplexityElement.h>
#include <Inventor/elements/SoComplexityTypeElement.h>
#include <Inventor/elements/SoGLTextureCoordinateElement.h>
#include <Inventor/elements/SoGLTextureEnabledElement.h>
#include <Inventor/elements/SoLightModelElement.h>
#include <Inventor/elements/SoMaterialBindingElement.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/nodes/SoCube.h>

SO_NODE_SOURCE(SoCube);

// Cube geometry
SbVec3f		SoCube::coords[8];	// Corner coordinates
SbVec2f		SoCube::texCoords[4];	// Face corner texture coordinates
SbVec3f		SoCube::normals[6];	// Face normals
SbVec3f		SoCube::edgeNormals[12];// Edge normals (for wire-frame)
const SbVec3f  *SoCube::verts[6][4];	// Vertex references to coords

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoCube::SoCube()
//
////////////////////////////////////////////////////////////////////////
{
    float	sq2;

    SO_NODE_CONSTRUCTOR(SoCube);

    SO_NODE_ADD_FIELD(width,  (2.0));
    SO_NODE_ADD_FIELD(height, (2.0));
    SO_NODE_ADD_FIELD(depth,	 (2.0));

    isBuiltIn = TRUE;

    if (SO_NODE_IS_FIRST_INSTANCE()) {
	// Initialize corner coordinate values
	coords[0].setValue(-1.0,  1.0, -1.0);	// Left  Top    Back
	coords[1].setValue( 1.0,  1.0, -1.0);	// Right Top    Back
	coords[2].setValue(-1.0, -1.0, -1.0);	// Left  Bottom Back
	coords[3].setValue( 1.0, -1.0, -1.0);	// Right Bottom Back
	coords[4].setValue(-1.0,  1.0,  1.0);	// Left  Top    Front
	coords[5].setValue( 1.0,  1.0,  1.0);	// Right Top    Front
	coords[6].setValue(-1.0, -1.0,  1.0);	// Left  Bottom Front
	coords[7].setValue( 1.0, -1.0,  1.0);	// Right Bottom Front

	// Initialize face vertices to point into coords. The order of
	// vertices around the faces is chosen so that the texture
	// coordinates match up: texture coord (0,0) is at the first
	// vertex and (1,1) is at the third. The vertices obey the
	// right-hand rule for each face.
	verts[1][2] = verts[2][3] = verts[4][3] = &coords[0];
	verts[1][3] = verts[3][2] = verts[4][2] = &coords[1];
	verts[1][1] = verts[2][0] = verts[5][0] = &coords[2];
	verts[1][0] = verts[3][1] = verts[5][1] = &coords[3];
	verts[0][3] = verts[2][2] = verts[4][0] = &coords[4];
	verts[0][2] = verts[3][3] = verts[4][1] = &coords[5];
	verts[0][0] = verts[2][1] = verts[5][3] = &coords[6];
	verts[0][1] = verts[3][0] = verts[5][2] = &coords[7];

	// Initialize texture coordinates. These are for the 4 corners of
	// each face, starting at the lower left corner
	texCoords[0].setValue(0.0, 0.0);
	texCoords[1].setValue(1.0, 0.0);
	texCoords[2].setValue(1.0, 1.0);
	texCoords[3].setValue(0.0, 1.0);

	// Initialize face normals
	normals[0].setValue( 0.0,  0.0,  1.0);	// Front
	normals[1].setValue( 0.0,  0.0, -1.0);	// Back
	normals[2].setValue(-1.0,  0.0,  0.0);	// Left
	normals[3].setValue( 1.0,  0.0,  0.0);	// Right
	normals[4].setValue( 0.0,  1.0,  0.0);	// Top
	normals[5].setValue( 0.0, -1.0,  0.0);	// Bottom

	// Initialize edge normals. These are used when drawing simple
	// wire-frame versions of the cube. The order of these matters,
	// since the rendering routine relies on it. Each normal is the
	// average of the face normals of the two adjoining faces, so the
	// edge is fairly-well lit in any forward-facing orientation.

	sq2 = sqrt(2.0) / 2.0;

	edgeNormals[ 0].setValue( 0.0, -sq2,  sq2);		// Bottom front
	edgeNormals[ 1].setValue( sq2,  0.0,  sq2);		// Right  front
	edgeNormals[ 2].setValue( 0.0,  sq2,  sq2);		// Top    front
	edgeNormals[ 3].setValue(-sq2,  0.0,  sq2);		// Left   front
	edgeNormals[ 4].setValue( 0.0, -sq2, -sq2);		// Bottom rear
	edgeNormals[ 5].setValue(-sq2,  0.0, -sq2);		// Left   rear
	edgeNormals[ 6].setValue( 0.0,  sq2, -sq2);		// Top    rear
	edgeNormals[ 7].setValue( sq2,  0.0, -sq2);		// Right  rear
	edgeNormals[ 8].setValue(-sq2, -sq2,  0.0);		// Bottom left
	edgeNormals[ 9].setValue( sq2, -sq2,  0.0);		// Bottom right
	edgeNormals[10].setValue( sq2,  sq2,  0.0);		// Top    right
	edgeNormals[11].setValue(-sq2,  sq2,  0.0);		// Top    left
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoCube::~SoCube()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Performs GL rendering of a cube.
//
// Use: extender

void
SoCube::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // First see if the object is visible and should be rendered now
    if (! shouldGLRender(action))
	return;

    // See if texturing is enabled
    SbBool doTextures = SoGLTextureEnabledElement::get(action->getState());

    // Render the cube. The GLRenderGeneric() method handles any
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
//    Implements ray picking. We could just use the default mechanism,
//    generating primitives, but this would be inefficient if the
//    complexity is above 0.5. Therefore, we make sure that the
//    complexity is low and then use the primitive generation.
//
// Use: extender

void
SoCube::rayPick(SoRayPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // First see if the object is pickable
    if (! shouldRayPick(action))
	return;

    // Save the state so we don't affect the real complexity
    action->getState()->push();

    // Change the complexity
    SoComplexityElement::set(action->getState(), 0.0);
    SoComplexityTypeElement::set(action->getState(),
				 SoComplexityTypeElement::OBJECT_SPACE);

    // Pick using primitive generation. Make sure we know that we are
    // really picking on a real cube, not just a bounding box of
    // another shape.
    pickingBoundingBox = FALSE;
    SoShape::rayPick(action);

    // Restore the state
    action->getState()->pop();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Computes bounding box of cube.
//
// Use: protected

void
SoCube::computeBBox(SoAction *, SbBox3f &box, SbVec3f &center)
//
////////////////////////////////////////////////////////////////////////
{
    float	w, h, d;

    getSize(w, h, d);
    box.setBounds(-w, -h, -d, w, h, d);
    center.setValue(0.0, 0.0, 0.0);
}

@@===========================================================================
@@
@@ Local definitions used with SpewTriangles to set up and send vertices.
@@
@@===========================================================================
@@
@={ GenerateVertex
    pt.setValue((*verts[face][vert])[0] * w,
		(*verts[face][vert])[1] * h,
		(*verts[face][vert])[2] * d);
    if (genTexCoords) {
	tex[0] = texCoords[vert][0];
	tex[1] = texCoords[vert][1];
    }
    else
	tex = tce->get(pt, normals[face]);
    pv.setPoint(pt);
    pv.setTextureCoords(tex);
    shapeVertex(&pv);
@=}
@@
@@===========================================================================
@@
@={ SendStripVertices
@?{ GeneratePrimitives
    pt = topPoint;
    pt[0] *= w;
    pt[1] *= h;
    pt[2] *= d;
    if (genTexCoords) {
	tex[0] = s;
	tex[1] = (strip + 1) * di;
    }
    else
	tex = tce->get(pt, normals[face]);
    pv.setPoint(pt);
    pv.setTextureCoords(tex);
    shapeVertex(&pv);
    pt = botPoint;
    pt[0] *= w;
    pt[1] *= h;
    pt[2] *= d;
    if (genTexCoords) {
	tex[0] = s;
	tex[1] = strip * di;
    }
    else
	tex = tce->get(pt, normals[face]);
    pv.setPoint(pt);
    pv.setTextureCoords(tex);
    shapeVertex(&pv);
@?}
@?{ RenderGeneric
    if (doTextures) {
	glTexCoord2f(s, (strip + 1) * di);
	glVertex3fv(SCALE(topPoint).getValue());
	glTexCoord2f(s, strip * di);
	glVertex3fv(SCALE(botPoint).getValue());
    }
    else {
	glVertex3fv(SCALE(topPoint).getValue());
	glVertex3fv(SCALE(botPoint).getValue());
    }
@?}
@?{ RenderNormalsNoTexture
    glVertex3fv(SCALE(topPoint).getValue());
    glVertex3fv(SCALE(botPoint).getValue());
@?}
@=}
@@
@@===========================================================================
@@
@@ This is the main triangle-spewing method for the SoCube class. It
@@ is used to generate primitives and to render.
@@
@@===========================================================================
@={ SpewTriangles
    SbBool		materialPerFace;
    int			numDivisions, face, vert;
@?{ DoTextures
    float		s;
@?}
    SbVec3f		pt, norm;
@?{ Render
    SoMaterialBundle	mb(action);
@?}
@?{ GeneratePrimitives
    float		w, h, d;
    SbVec4f		tex;
    SbBool		genTexCoords;
    SoPrimitiveVertex	pv;
    SoCubeDetail	detail;
    const SoTextureCoordinateElement	*tce;
@?}

    materialPerFace = isMaterialPerFace(action);
    numDivisions    = computeNumDivisions(action);

@?{ Render
    // Make sure first material is sent if necessary
    if (materialPerFace)
	mb.setUpMultiple();
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

    getSize(w, h, d);
@?}

@?{ Render
    if (numDivisions == 1)
	glBegin(GL_QUADS);
@?}

    for (face = 0; face < 6; face++) {

@?{ Render
	if (materialPerFace && face > 0)
	    mb.send(face, numDivisions == 1);
@?}
@?{ RenderGeneric
	if (sendNormals)
	    glNormal3fv(normals[face].getValue());
@?}
@?{ RenderNormalsNoTexture
	glNormal3fv(normals[face].getValue());
@?}
@?{ GeneratePrimitives
	if (face == 0 || materialPerFace)
	    pv.setMaterialIndex(face);
	pv.setNormal(normals[face]);
	detail.setPart(face);
@?}

	// Simple case of one polygon per face 
	if (numDivisions == 1) {
@?{ Render
	    for (vert = 0; vert < 4; vert++) {
@?{ RenderGeneric
		if (doTextures)
		    glTexCoord2fv(texCoords[vert].getValue());
@?}
		glVertex3fv(SCALE(*verts[face][vert]).getValue());
	    }
@?}
@?{ GeneratePrimitives
	    beginShape(action, TRIANGLE_STRIP);
	    vert = 3;
	    @ GenerateVertex
	    vert = 0;
	    @ GenerateVertex
	    vert = 2;
	    @ GenerateVertex
	    vert = 1;
	    @ GenerateVertex
	    endShape();
@?}
	}

	// More than one polygon per face
	else {
	    float	di = 1.0 / numDivisions;
	    SbVec3f	topPoint,    botPoint,    nextBotPoint;
	    SbVec3f	horizSpace, vertSpace;
	    int		strip, rect;

	    botPoint = *verts[face][0];

	    // Compute spacing between adjacent points in both directions
	    horizSpace = di * (*verts[face][1] - botPoint);
	    vertSpace  = di * (*verts[face][3] - botPoint);

	    // For each horizontal strip
	    for (strip = 0; strip < numDivisions; strip++) {

		// Compute current top point. Save it to use as bottom
		// of next strip
		nextBotPoint = topPoint = botPoint + vertSpace;

@?{ Render
		glBegin(GL_TRIANGLE_STRIP);
@?}
@?{ GeneratePrimitives
		beginShape(action, TRIANGLE_STRIP);
@?}

		// Send points at left end of strip
@?{ DoTextures
		s = 0.0;
@?}
		@ SendStripVertices

		// For each rectangular piece of strip
		for (rect = 0; rect < numDivisions; rect++) {

		    // Go to next rect
		    topPoint += horizSpace;
		    botPoint += horizSpace;
@?{ DoTextures
		    s += di;
@?}

		    // Send points at right side of rect
		    @ SendStripVertices
		}

@?{ Render
		glEnd();
@?}
@?{ GeneratePrimitives
		endShape();
@?}

		// Get ready for next strip
		botPoint = nextBotPoint;
	    }
	}
    }
@?{ Render

    if (numDivisions == 1)
	glEnd();
@?}
@=}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Generates triangles representing a cube.
//
// Use: protected

void
SoCube::generatePrimitives(SoAction *action)
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
//    Generic rendering of cube with or without normals, with or
//    without texture coordinates.
//
// Use: private

void
SoCube::GLRenderGeneric(SoGLRenderAction *action,
			SbBool sendNormals, SbBool doTextures)
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f scale, tmp;
    getSize(scale[0], scale[1], scale[2]);

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
//    Renders cube with normals and without texture coordinates.
//
// Use: private

void
SoCube::GLRenderNvertTnone(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f scale, tmp;
    getSize(scale[0], scale[1], scale[2]);

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
//    Overrides standard method to create an SoCubeDetail instance
//    representing a picked intersection with a triangle that is half
//    of the face of a cube.
//
// Use: protected, virtual

SoDetail *
SoCube::createTriangleDetail(SoRayPickAction *,
			     const SoPrimitiveVertex *v1,
			     const SoPrimitiveVertex *,
			     const SoPrimitiveVertex *,
			     SoPickedPoint *)
//
////////////////////////////////////////////////////////////////////////
{
    SoCubeDetail	*detail;

    // Don't create a detail if the pick operation was performed on a
    // bounding box cube, not a real cube
    if (pickingBoundingBox)
	return NULL;

    detail = new SoCubeDetail;

    // The part code should be the same in all three details, so just use one
    detail->setPart(((const SoCubeDetail *) v1->getDetail())->getPart());

    return detail;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if per face materials are specified.
//
// Use: private

SbBool
SoCube::isMaterialPerFace(SoAction *action) const
//
////////////////////////////////////////////////////////////////////////
{
    SoMaterialBindingElement::Binding binding;

    binding = SoMaterialBindingElement::get(action->getState());

    return (binding == SoMaterialBindingElement::PER_PART ||
	    binding == SoMaterialBindingElement::PER_PART_INDEXED ||
	    binding == SoMaterialBindingElement::PER_FACE ||
	    binding == SoMaterialBindingElement::PER_FACE_INDEXED);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Computes number of divisions per side based on complexity.
//
// Use: private

int
SoCube::computeNumDivisions(SoAction *action) const
//
////////////////////////////////////////////////////////////////////////
{
    int		numDivisions;
    float	complexity;

    switch (SoComplexityTypeElement::get(action->getState())) {

      case SoComplexityTypeElement::OBJECT_SPACE:

	// In object space, the number of divisions is greater than 1
	// only for complexity values > 0.5. The maximum value is 16,
	// when complexity = 1.
	complexity = SoComplexityElement::get(action->getState());
	numDivisions = (complexity <= 0.5 ? 1 :
			-14 + (int) (complexity * 30.0));
	break;

      case SoComplexityTypeElement::SCREEN_SPACE:

	// In screen space, the number of divisions is based on the
	// complexity and the size of the cube when projected onto the
	// screen.
	short	maxSize;
	{
	    SbVec3f	p;
	    SbVec2s	rectSize;

	    getSize(p[0], p[1], p[2]);
	    getScreenSize(action->getState(), SbBox3f(-p, p), rectSize);
	    maxSize = (rectSize[0] > rectSize[1] ? rectSize[0] : rectSize[1]);
	}

	// Square complexity to get a more even increase in the number
	// of tesselation squares. Maximum bound is 1/4 the number of
	// pixels per side.
	complexity = SoComplexityElement::get(action->getState());
	numDivisions = 1 + (int) (0.25 * maxSize * complexity * complexity);
	break;

      case SoComplexityTypeElement::BOUNDING_BOX:

	// Most shapes do not have to handle this case, since it is
	// handled for them. However, since it is handled by drawing
	// the shape as a cube, the SoCube class has to handle it.
	numDivisions = 1;
	break;
    }

    return numDivisions;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Computes real half-width, -height, -depth.
//
// Use: private

void
SoCube::getSize(float &hWidth, float &hHeight, float &hDepth) const
//
////////////////////////////////////////////////////////////////////////
{
    hWidth  = ( width.isIgnored() ? 1.0 :  width.getValue() / 2.0);
    hHeight = (height.isIgnored() ? 1.0 : height.getValue() / 2.0);
    hDepth  = ( depth.isIgnored() ? 1.0 :  depth.getValue() / 2.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does GL rendering of a cube representing the given bounding box.
//    This is used for BOUNDING_BOX complexity. It does the minimum
//    work necessary.
//
// Use: private

void
SoCube::GLRenderBoundingBox(SoGLRenderAction *action, const SbBox3f &bbox)
//
////////////////////////////////////////////////////////////////////////
{
    int			face, vert;
    SoMaterialBundle	mb(action);
    SbVec3f		scale, tmp;

    // Make sure textures are disabled, just to speed things up
    action->getState()->push();
    SoGLTextureEnabledElement::set(action->getState(), FALSE);

    // Make sure first material is sent if necessary
    mb.sendFirst();

    // Scale and translate the cube to the correct spot
    const SbVec3f	&translate = bbox.getCenter();
    SbVec3f size;
    bbox.getSize(size[0], size[1], size[2]);
    scale = 0.5 * size;

    for (face = 0; face < 6; face++) {

	if (! mb.isColorOnly())
	    glNormal3fv(normals[face].getValue());

	glBegin(GL_POLYGON);

	for (vert = 0; vert < 4; vert++)
	    glVertex3fv((SCALE(*verts[face][vert]) + translate).getValue());

	glEnd();
    }

    // Restore state
    action->getState()->pop();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does picking of a cube representing the given bounding box. This
//    is used for BOUNDING_BOX complexity. It uses the same code as
//    for rayPick(), except that it makes sure the cube is transformed
//    first to where the bounding box is.
//
// Use: private

void
SoCube::rayPickBoundingBox(SoRayPickAction *action, const SbBox3f &bbox)
//
////////////////////////////////////////////////////////////////////////
{
    // Save the state so we don't affect the real complexity
    action->getState()->push();

    // Change the complexity
    SoComplexityElement::set(action->getState(), 0.0);
    SoComplexityTypeElement::set(action->getState(),
				 SoComplexityTypeElement::OBJECT_SPACE);

    // Change the current matrix to scale and translate the cube to the
    // correct spot. (We can't just use an extra matrix passed to
    // computeObjectSpaceRay(), since the points generated by
    // generatePrimitives() have to be transformed, not just the ray.)
    SbVec3f		size;
    bbox.getSize(size[0], size[1], size[2]);

    // If any of the dimensions is 0, beef it up a little bit to avoid
    // scaling by 0
    if (size[0] == 0.0)
	size[0] = 0.00001;
    if (size[1] == 0.0)
	size[1] = 0.00001;
    if (size[2] == 0.0)
	size[2] = 0.00001;

    SoModelMatrixElement::translateBy(action->getState(), this,
				      bbox.getCenter());
    SoModelMatrixElement::scaleBy(action->getState(), this,
				  0.5 * size);

    // Compute the picking ray in the space of the shape
    computeObjectSpaceRay(action);

    // Pick using primitive generation. Make sure we know that we are
    // picking on just a bounding box of another shape, so details
    // won't be created.
    pickingBoundingBox = TRUE;
    generatePrimitives(action);

    // Restore the state
    action->getState()->pop();
}
