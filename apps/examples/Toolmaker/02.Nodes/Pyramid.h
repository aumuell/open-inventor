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
 *  chapter 2, example 3.
 *
 *  Header file for "Pyramid" shape node.
 *------------------------------------------------------------*/

#include <Inventor/SbLinear.h>
#include <Inventor/fields/SoSFBitMask.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/nodes/SoShape.h>
// SoShape.h includes SoSubNode.h; no need to include it again

// Pyramid texture coordinates are defined on the sides so that
// the seam is along the left rear edge, wrapping
// counterclockwise around the sides. The texture coordinates on
// the base are set up so the texture is right side up when the
// pyramid is tilted back.

class Pyramid : public SoShape {

   SO_NODE_HEADER(Pyramid);

 public:

   enum Part {                  // Pyramid parts:
      SIDES = 0x01,             // The 4 side faces
      BASE  = 0x02,             // The bottom square face
      ALL   = 0x03              // All parts
   };

   // Fields
   SoSFBitMask   parts;         // Visible parts
   SoSFFloat     baseWidth;     // Width of base
   SoSFFloat     baseDepth;     // Depth of base
   SoSFFloat     height;        // Height, base to apex 

   // Initializes this class
   static void   initClass();

   // Constructor
   Pyramid();

   // Turns on/off a part of the pyramid. (Convenience)
   void          addPart(Part part);
   void          removePart(Part part);

   // Returns whether a given part is on or off. (Convenience)
   SbBool        hasPart(Part part) const;

 protected:
   // This implements the GL rendering action. We will inherit
   // all other action behavior, including rayPick(), which is
   // defined by SoShape to pick against all of the triangles
   // created by generatePrimitives.
   virtual void  GLRender(SoGLRenderAction *action);

   // Generates triangles representing a pyramid
   virtual void  generatePrimitives(SoAction *action);

   // This computes the bounding box and center of a pyramid. It
   // is used by SoShape for the SoGetBoundingBoxAction and also
   // to compute the correct box to render or pick when
   // complexity is BOUNDING_BOX. Note that we do not have to
   // define a getBoundingBox() method, since SoShape already
   // takes care of that (using this method).
   virtual void   computeBBox(SoAction *action,
                              SbBox3f &box, SbVec3f &center);
 private:
   // Face normals. These are static because they are computed
   // once and are shared by all instances
   static SbVec3f frontNormal, rearNormal;
   static SbVec3f leftNormal,  rightNormal; 
   static SbVec3f baseNormal;

   // Destructor
   virtual ~Pyramid();

   // Computes and returns half-width, half-height, and
   // half-depth based on current field values
   void           getSize(float &halfWidth,
                          float &halfHeight,
                          float &halfDepth) const;
};
