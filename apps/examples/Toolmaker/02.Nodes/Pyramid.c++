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

/*--------------------------------------------------------------
 *  This is an example from the Inventor Toolmaker,
 *  chapter 2, example 4.
 *
 *  Source file for "Pyramid" shape node.
 *------------------------------------------------------------*/


#include <GL/gl.h>
#include <Inventor/SbBox.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/elements/SoGLLazyElement.h> 
#include <Inventor/elements/SoGLTextureCoordinateElement.h>
#include <Inventor/elements/SoGLTextureEnabledElement.h>
#include <Inventor/elements/SoMaterialBindingElement.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/misc/SoState.h>
#include "Pyramid.h"

// Shorthand macro for testing whether the current parts field
// value (parts) includes a given part (part)
#define HAS_PART(parts, part) (((parts) & (part)) != 0)

SO_NODE_SOURCE(Pyramid);

// Normals to four side faces and to base
SbVec3f Pyramid::frontNormal, Pyramid::rearNormal;
SbVec3f Pyramid::leftNormal,  Pyramid::rightNormal;
SbVec3f Pyramid::baseNormal;

//
// This initializes the Pyramid class.
//

void
Pyramid::initClass()
{
   // Initialize type id variables
   SO_NODE_INIT_CLASS(Pyramid, SoShape, "Shape");
}

//
// Constructor
//

Pyramid::Pyramid()
{
   SO_NODE_CONSTRUCTOR(Pyramid);
   SO_NODE_ADD_FIELD(parts,     (ALL));
   SO_NODE_ADD_FIELD(baseWidth, (2.0));
   SO_NODE_ADD_FIELD(baseDepth, (2.0));
   SO_NODE_ADD_FIELD(height,    (2.0));

   // Set up static values and strings for the "parts"
   // enumerated type field. This allows the SoSFEnum class to
   // read values for this field. For example, the first line
   // below says that the first value (index 0) has the value
   // SIDES (defined in the header file) and is represented in
   // the file format by the string "SIDES".
   SO_NODE_DEFINE_ENUM_VALUE(Part, SIDES);
   SO_NODE_DEFINE_ENUM_VALUE(Part, BASE);
   SO_NODE_DEFINE_ENUM_VALUE(Part, ALL);

   // Copy static information for "parts" enumerated type field
   // into this instance. 
   SO_NODE_SET_SF_ENUM_TYPE(parts, Part);

   // If this is the first time the constructor is called, set
   // up the static normals
   if (SO_NODE_IS_FIRST_INSTANCE()) {
      float invRoot5      = 1.0 / sqrt(5.0);
      float invRoot5Twice = 2.0 * invRoot5;

      frontNormal.setValue(0.0, invRoot5,  invRoot5Twice);
      rearNormal.setValue( 0.0, invRoot5, -invRoot5Twice);
      leftNormal.setValue( -invRoot5Twice, invRoot5, 0.0);
      rightNormal.setValue( invRoot5Twice, invRoot5, 0.0);
      baseNormal.setValue(0.0, -1.0, 0.0);
   }
}

//
// Destructor
//

Pyramid::~Pyramid()
{
}

//
// Turns on a part of the pyramid. (Convenience function.)
//

void
Pyramid::addPart(Part part)
{
   parts.setValue(parts.getValue() | part);
}

//
// Turns off a part of the pyramid. (Convenience function.)
//

void
Pyramid::removePart(Part part)
{
   parts.setValue(parts.getValue() & ~part);
}

//
// Returns whether a given part is on or off. (Convenience
// function.)
//

SbBool
Pyramid::hasPart(Part part) const
{
   return HAS_PART(parts.getValue(), part);
}

//
// Implements the SoGLRenderAction for the Pyramid node.
//

void
Pyramid::GLRender(SoGLRenderAction *action)
{
    
   // Access the state from the action
   SoState  *state = action->getState();

   // See which parts are enabled
   int curParts = (parts.isIgnored() ? ALL : parts.getValue());

   // First see if the object is visible and should be rendered
   // now. This is a method on SoShape that checks for INVISIBLE
   // draw style, BOUNDING_BOX complexity, and delayed
   // transparency.
   if (! shouldGLRender(action))
      return;

   // Declare a pointer to a GLLazyElement.  This will be used if we
   // send multiple colors.
   SoGLLazyElement* lazyElt;
   
   // Note that in inventor 2.1 we do not use beginSolidShape and
   // endSolidShape.  Instead, this information should be provided in
   // shape hints.
     
   // Change the current GL matrix to draw the pyramid with the
   // correct size. This is easier than modifying all of the
   // coordinates and normals of the pyramid. (For extra
   // efficiency, you can check if the field values are all set
   // to default values - if so, then you can skip this step.)
   // Scale world if necessary
   float         halfWidth, halfHeight, halfDepth;
   getSize(halfWidth, halfHeight, halfDepth);
   glPushMatrix();
   glScalef(halfWidth, halfHeight, halfDepth);

   // See if texturing is enabled. If so, we will have to
   // send explicit texture coordinates. The "doTextures" flag
   // will indicate if we care about textures at all.
   
   // Note this has changed slightly in Inventor version 2.1.
   // The texture coordinate type now is either FUNCTION or DEFAULT.
   // Texture coordinates are needed only for DEFAULT textures.

   SbBool doTextures =
      (SoGLTextureEnabledElement::get(state) &&
       SoTextureCoordinateElement::getType(state) !=
       SoTextureCoordinateElement::FUNCTION);

   // Determine if we need to send normals. Normals are
   // necessary if we are not doing BASE_COLOR lighting.
         
   // we use the Lazy element to get the light model.   
   SbBool sendNormals = (SoLazyElement::getLightModel(state) 
	    != SoLazyElement::BASE_COLOR);      

   // Determine if there's a material bound per part
   SoMaterialBindingElement::Binding binding = 
      SoMaterialBindingElement::get(state);
   SbBool materialPerPart =
      (binding == SoMaterialBindingElement::PER_PART ||
       binding == SoMaterialBindingElement::PER_PART_INDEXED);
       
   // issue a lazy element send.
   // This send will ensure that all material state in GL is current.
 
   SoGLLazyElement::sendAllMaterial(state);

   // Render the parts of the pyramid. We don't have to worry
   // about whether to render filled regions, lines, or points,
   // since that is already taken care of. We are also ignoring
   // complexity, which we could use to render a more
   // finely-tesselated version of the pyramid.

   // We'll use this macro to make the code easier. It uses the
   // "point" variable to store the vertex point to send.
   SbVec3f  point;

#define SEND_VERTEX(x, y, z, s, t)   \
      point.setValue(x, y, z);       \
      if (doTextures)                \
         glTexCoord2f(s, t);         \
      glVertex3fv(point.getValue())

   if (HAS_PART(curParts, SIDES)) {

      // Draw each side separately, so that normals are correct.
      // If sendNormals is TRUE, send face normals with the
      // polygons. Make sure the vertex order obeys the
      // right-hand rule.

      glBegin(GL_TRIANGLES);

      // Front face: left front, right front, apex
      if (sendNormals)
         glNormal3fv(frontNormal.getValue());
      SEND_VERTEX(-1.0, -1.0,  1.0, .25,  0.0);
      SEND_VERTEX( 1.0, -1.0,  1.0, .50,  0.0);
      SEND_VERTEX( 0.0,  1.0,  0.0, .325, 1.0);

      // Right face: right front, right rear, apex
      if (sendNormals)
         glNormal3fv(rightNormal.getValue());
      SEND_VERTEX( 1.0, -1.0,  1.0, .50,  0.0);
      SEND_VERTEX( 1.0, -1.0, -1.0, .75,  0.0);
      SEND_VERTEX( 0.0,  1.0,  0.0, .625, 1.0);

      // Rear face: right rear, left rear, apex
      if (sendNormals)
         glNormal3fv(rearNormal.getValue());
      SEND_VERTEX( 1.0, -1.0, -1.0, .75,  0.0);
      SEND_VERTEX(-1.0, -1.0, -1.0, 1.0,  0.0);
      SEND_VERTEX( 0.0,  1.0,  0.0, .875, 1.0);

      // Left face: left rear, left front, apex
      if (sendNormals)
         glNormal3fv(leftNormal.getValue());
      SEND_VERTEX(-1.0, -1.0, -1.0, 0.0,  0.0);
      SEND_VERTEX(-1.0, -1.0,  1.0, .25,  0.0);
      SEND_VERTEX( 0.0,  1.0,  0.0, .125, 1.0);

      glEnd();
   }

   if (HAS_PART(curParts, BASE)) {

      // Send the next material if it varies per part. 
      // use SoGLLazyElement::sendDiffuseByIndex().
      // This will also send transparency, so that if transparency type
      // is not SCREEN_DOOR, there can be a change of transparency across
      // the shape:
      
      if (materialPerPart){
	  //obtain a current copy of the SoGLLazyElement, use this
	  //for the send:
	  lazyElt = (SoGLLazyElement*)SoLazyElement::getInstance(state);
	  lazyElt->sendDiffuseByIndex(1);
      }

      if (sendNormals)
         glNormal3fv(baseNormal.getValue());

      // Base: left rear, right rear, right front, left front
      glBegin(GL_QUADS);
      SEND_VERTEX(-1.0, -1.0, -1.0, 0.0,  0.0);
      SEND_VERTEX( 1.0, -1.0, -1.0, 1.0,  0.0);
      SEND_VERTEX( 1.0, -1.0,  1.0, 1.0,  1.0);
      SEND_VERTEX(-1.0, -1.0,  1.0, 0.0,  1.0);
      glEnd();
   }

   // Restore the GL matrix
   glPopMatrix();

   // reset the diffuse color, if we sent it twice:
   if (materialPerPart) lazyElt->reset(state, SoLazyElement::DIFFUSE_MASK);

}

//
// Generates triangles representing a pyramid.
//

void
Pyramid::generatePrimitives(SoAction *action)
{
   // The pyramid will generate 6 triangles: 1 for each side
   // and 2 for the base. (Again, we are ignoring complexity.)
   // This variable is used to store each vertex.
   SoPrimitiveVertex   pv;

   // Access the state from the action
   SoState  *state = action->getState();

   // See which parts are enabled
   int curParts = (parts.isIgnored() ? ALL : parts.getValue());

   // We need the size to adjust the coordinates
   float halfWidth, halfHeight, halfDepth;
   getSize(halfWidth, halfHeight, halfDepth);

   // See if we have to use a texture coordinate function,
   // rather than generating explicit texture coordinates.
   SbBool useTexFunc = 
      (SoTextureCoordinateElement::getType(state) ==
       SoTextureCoordinateElement::FUNCTION);

   // If we need to generate texture coordinates with a
   // function, we'll need an SoGLTextureCoordinateElement.
   // Otherwise, we'll set up the coordinates directly.
   const SoTextureCoordinateElement *tce;
   SbVec4f texCoord;
   if (useTexFunc)
      tce = SoTextureCoordinateElement::getInstance(state);
   else {
      texCoord[2] = 0.0;
      texCoord[3] = 1.0;
   }

   // Determine if there's a material bound per part
   SoMaterialBindingElement::Binding binding = 
      SoMaterialBindingElement::get(state);
   SbBool materialPerPart =
      (binding == SoMaterialBindingElement::PER_PART ||
       binding == SoMaterialBindingElement::PER_PART_INDEXED);

   // We'll use this macro to make the code easier. It uses the
   // "point" variable to store the primitive vertex's point.
   SbVec3f  point;

#define GEN_VERTEX(pv, x, y, z, s, t, normal)   \
      point.setValue(halfWidth  * x,            \
                     halfHeight * y,            \
                     halfDepth  * z);           \
      if (useTexFunc)                           \
         texCoord = tce->get(point, normal);    \
      else {                                    \
         texCoord[0] = s;                       \
         texCoord[1] = t;                       \
      }                                         \
      pv.setPoint(point);                       \
      pv.setNormal(normal);                     \
      pv.setTextureCoords(texCoord);            \
      shapeVertex(&pv)

   if (HAS_PART(curParts, SIDES)) {

      // We will generate 4 triangles for the sides of the
      // pyramid. We can use the beginShape() / shapeVertex() /
      // endShape() convenience functions on SoShape to make the
      // triangle generation easier and clearer. (The
      // shapeVertex() call is built into the macro.)

      // Note that there is no detail information for the
      // Pyramid. If there was, we would create an instance of
      // the correct subclass of SoDetail (such as
      // PyramidDetail) and call pv.setDetail(&detail); once.

      beginShape(action, TRIANGLES);

      // Front face: left front, right front, apex
      GEN_VERTEX(pv, -1.0, -1.0,  1.0, .25,  0.0, frontNormal);
      GEN_VERTEX(pv,  1.0, -1.0,  1.0, .50,  0.0, frontNormal);
      GEN_VERTEX(pv,  0.0,  1.0,  0.0, .325, 1.0, frontNormal);

      // Right face: right front, right rear, apex
      GEN_VERTEX(pv,  1.0, -1.0,  1.0, .50,  0.0, rightNormal);
      GEN_VERTEX(pv,  1.0, -1.0, -1.0, .75,  0.0, rightNormal);
      GEN_VERTEX(pv,  0.0,  1.0,  0.0, .625, 1.0, rightNormal);

      // Rear face: right rear, left rear, apex
      GEN_VERTEX(pv,  1.0, -1.0, -1.0, .75,  0.0, rearNormal);
      GEN_VERTEX(pv, -1.0, -1.0, -1.0, 1.0,  0.0, rearNormal);
      GEN_VERTEX(pv,  0.0,  1.0,  0.0, .875, 1.0, rearNormal);

      // Left face: left rear, left front, apex
      GEN_VERTEX(pv, -1.0, -1.0, -1.0, 0.0,  0.0, leftNormal);
      GEN_VERTEX(pv, -1.0, -1.0,  1.0, .25,  0.0, leftNormal);
      GEN_VERTEX(pv,  0.0,  1.0,  0.0, .125, 1.0, leftNormal);

      endShape();
   }

   if (HAS_PART(curParts, BASE)) {

      // Increment the material index in the vertex if
      // necessary. (The index is set to 0 by default.)
      if (materialPerPart)
         pv.setMaterialIndex(1);

      // We will generate two triangles for the base, as a
      // triangle strip.
      beginShape(action, TRIANGLE_STRIP);

      // Base: left front, left rear, right front, right rear
      GEN_VERTEX(pv, -1.0, -1.0,  1.0, 0.0,  1.0, baseNormal);
      GEN_VERTEX(pv, -1.0, -1.0, -1.0, 0.0,  0.0, baseNormal);
      GEN_VERTEX(pv,  1.0, -1.0,  1.0, 1.0,  1.0, baseNormal);
      GEN_VERTEX(pv,  1.0, -1.0, -1.0, 1.0,  0.0, baseNormal);

      endShape();
   }
}

//
// Computes the bounding box and center of a pyramid.
//

void
Pyramid::computeBBox(SoAction *, SbBox3f &box, SbVec3f &center)
{
   // Figure out what parts are active
   int curParts = (parts.isIgnored() ? ALL : parts.getValue());

   // If no parts are active, set the bounding box to be tiny
   if (curParts == 0)
      box.setBounds(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

   else {
      // These points define the min and max extents of the box
      SbVec3f min, max;

      // Compute the half-width, half-height, and half-depth of
      // the pyramid. We'll use this info to set the min and max
      // points.
      float   halfWidth, halfHeight, halfDepth;
      getSize(halfWidth, halfHeight, halfDepth);

      min.setValue(-halfWidth, -halfHeight, -halfDepth);

      // The maximum point depends on whether the SIDES are
      // active. If not, only the base is present.
      if (HAS_PART(curParts, SIDES))
         max.setValue(halfWidth, halfHeight, halfDepth);
      else
         max.setValue(halfWidth, -halfHeight, halfDepth);

      // Set the box to bound the two extreme points
      box.setBounds(min, max);
   }

   // This defines the "natural center" of the pyramid. We could
   // define it to be the center of the base, if we want, but
   // let's just make it the center of the bounding box.
   center.setValue(0.0, 0.0, 0.0);
}

//
// Computes and returns half-width, half-height, and half-depth
// based on current field values.
//

void
Pyramid::getSize(float &halfWidth,
                 float &halfHeight,
                 float &halfDepth) const
{
   halfWidth  = (baseWidth.isIgnored() ? 1.0 :
                 baseWidth.getValue()  / 2.0);
   halfHeight = (height.isIgnored()    ? 1.0 :
                 height.getValue()     / 2.0);
   halfDepth  = (baseDepth.isIgnored() ? 1.0 :
                 baseDepth.getValue()  / 2.0);
}
