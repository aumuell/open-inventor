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

//  -*- C++ -*-

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Description:
 |	This file defines the SoText3 node class.
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_TEXT_3_
#define  _SO_TEXT_3_

#include <Inventor/SbBox.h>
#include <Inventor/fields/SoMFString.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/fields/SoSFBitMask.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/nodes/SoShape.h>

#include <GL/gl.h>   // For GLenum declaration

// C-api: end

//  GLU Library version dependency.
#ifndef gluTESSELATOR
#define gluTESSELATOR  GLUtriangulatorObj
#if defined(__glu_h__) && defined(GLU_VERSION_1_2)
#define gluTESSELATOR  GLUtesselator
#else
extern "C" struct GLUtriangulatorObj;
#endif
#endif

// C-api: begin

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoText3
//
//  Specify 3D text.
//
//////////////////////////////////////////////////////////////////////////////

class SoOutlineFontCache;
class SoPrimitiveVertex;
class SoTextureCoordinateElement;

// C-api: prefix=SoTxt3
// C-api: public=string, spacing, parts, justification
class SoText3 : public SoShape {

    SO_NODE_HEADER(SoText3);

  public:
    enum Justification {		// Justification types
	LEFT	= 0x01,
	RIGHT	= 0x02,
	CENTER	= 0x03
    };

    enum Part {			// Justification types
	FRONT	= 0x01,
	SIDES	= 0x02,
	BACK  	= 0x04,
	ALL     = 0x07
    };

    // Fields
    SoMFString		string;		// the strings to display
    SoSFFloat		spacing;	// interval between strings
    SoSFBitMask		parts;		// Visible parts of text
    SoSFEnum		justification;

    // Constructor
    SoText3();

  SoEXTENDER public:

    // Method to get the bounds of the given character in the given
    // string.  This must be called during the application of an
    // action; use a callbackAction applied to a Path to the text node
    // if you have to (this is really designed to be used in a Text3
    // manipulator, which will have direct access to the state).
    // The bounds are based on the font metric information, not the
    // geometric bounding box of the character.  So, for example,
    // while a space character has an empty bounding box,
    // getCharacterBounds() will return a box as high as the font
    // height, as wide as a space, and as deep as the profile.
    SbBox3f		getCharacterBounds(SoState *state,
				int stringIndex, int charIndex);

    virtual void	GLRender(SoGLRenderAction *action);
    virtual void	rayPick(SoRayPickAction *action);

  SoINTERNAL public:
    static void		initClass();

  protected:
    // Computes bounding box of text
    virtual void	computeBBox(SoAction *action, SbBox3f &box,
				    SbVec3f &center);

    // Generates primitives
    virtual void	generatePrimitives(SoAction *);
    
    virtual ~SoText3();

  private:
    // Setup the internal font cache
    SbBool setupFontCache(SoState *state, SbBool forRender = FALSE);

    // Return bounding box of the fronts of all characters.
    void getFrontBBox(SbBox2f &result);

    // Figure out how much each line of text is offset (based on width
    // of characters and justification)
    SbVec2f getStringOffset(int line);
    
    // Figure out where a particular character in a particular line
    // starts:
    SbVec2f getCharacterOffset(int line, int whichChar);

    // Render the fronts of the characters
    void renderFront(SoGLRenderAction *action, int line,
		     GLUtriangulatorObj *tobj);
    
    // Render the sides of the characters
    void renderSide(SoGLRenderAction *action, int line);

    // Creates a text detail when picking:
    SoDetail * createTriangleDetail(SoRayPickAction *,
				    const SoPrimitiveVertex *,
				    const SoPrimitiveVertex *,
				    const SoPrimitiveVertex *,
				    SoPickedPoint *);

    // Generates the fronts of the characters, by getting the outlines
    // and calling the glu tesselation code:
    void generateFront(int line);
    
    // Generates the sides (bevel) of the characters
    void generateSide(int line);

    // Callback functions, so the same routines (inside the internal
    // SoOutlineFontCache class) can be used to generate primitives
    // and render.
    static void generateSideTris(int nPoints, const SbVec3f *p1,
		const SbVec3f *n1, const SbVec3f *p2,
		const SbVec3f *n2, const float *sTexCoords,
		const float *tTexCoords);
    static void renderSideTris(int nPoints, const SbVec3f *p1,
		const SbVec3f *n1, const SbVec3f *p2,
		const SbVec3f *n2, const float *sTexCoords,
		const float *tTexCoords);

    // Static callbacks invoked by the glu tesselation code:
    static void beginCB(GLenum primType);
    static void endCB();
    static void vtxCB(void *vertex);
    
    // Private data:
    // SoOutlineFontCache is an internal, opaque class used to
    // maintain gl display lists and other information for each
    // character in a font.
    SoOutlineFontCache *myFont;

    // All this stuff is used while generating primitives:
    static SoText3 *currentGeneratingNode;
    static SoPrimitiveVertex *genPrimVerts[3];
    static SbVec3f genTranslate;
    static int genWhichVertex;
    static uint32_t genPrimType;
    static SoAction *genAction;
    static SbBool genBack;
    static SbBool genTexCoord;
    static const SoTextureCoordinateElement *tce;
};

#endif /* _SO_TEXT_3_ */
