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

/**************************************************************************
 * 		 Copyright (C) 1991-94, Silicon Graphics, Inc.		  *
 *  These coded instructions, statements, and computer programs  contain  *
 *  unpublished  proprietary  information of Silicon Graphics, Inc., and  *
 *  are protected by Federal copyright law.  They  may  not be disclosed  *
 *  to  third  parties  or copied or duplicated in any form, in whole or  *
 *  in part, without the prior written consent of Silicon Graphics, Inc.  *
 **************************************************************************/

//  -*- C++ -*-

/*
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Description:
 |	This file defines the SoAsciiText node class.
 |
 |   Author(s)		: Chris Marrin,  Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_ASCIITEXT_
#define  _SO_ASCIITEXT_

#include <Inventor/SbBox.h>
#include <Inventor/fields/SoMFString.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/fields/SoSFBitMask.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoMFFloat.h>
#include <Inventor/nodes/SoShape.h>

#include <GL/gl.h>   // For GLenum declaration

//  GLU Library version dependency.   Introduced with Iris 6.2.
#ifndef gluTESSELATOR
#if defined(__glu_h__) && (GLU_VERSION_1_2)
#define gluTESSELATOR  GLUtesselator
#else
#define gluTESSELATOR  GLUtriangulatorObj
extern "C" struct GLUtriangulatorObj;
#endif
#endif


//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoAsciiText
//
//  Specify 3D text for Vrml.
//
//////////////////////////////////////////////////////////////////////////////

class MyOutlineFontCache;
class SoPrimitiveVertex;
class SoTextureCoordinateElement;

// C-api: public=string,spacing,justification,width
class SoAsciiText : public SoShape {

    SO_NODE_HEADER(SoAsciiText);

  public:
    enum Justification {		// Justification types
	LEFT	= 0x01,
	RIGHT	= 0x02,
	CENTER	= 0x03
    };

    // Fields
    SoMFString		string;		// the strings to display
    SoSFFloat		spacing;	// interval between strings
    SoSFEnum		justification;
    SoMFFloat		width;		// width of each string

    // Constructor
    SoAsciiText();

  SoEXTENDER public:

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
    
    virtual ~SoAsciiText();

  private:
    // Setup the internal font cache
    SbBool setupFontCache(SoState *state, SbBool forRender = FALSE);

    // Return bounding box of the fronts of all characters.
    void getFrontBBox(SbBox2f &result);

    // Extend a 

    // Figure out how much each line of text is offset (based on width
    // of characters and justification)
    SbVec2f getStringOffset(int line, float width);
    
    // Render the fronts of the characters
    void renderFront(SoGLRenderAction *action, const SbString &string,
		     float width, GLUtriangulatorObj *tobj);
    
    // Creates a text detail when picking:
    SoDetail * createTriangleDetail(SoRayPickAction *,
				    const SoPrimitiveVertex *,
				    const SoPrimitiveVertex *,
				    const SoPrimitiveVertex *,
				    SoPickedPoint *);

    // Generates the fronts of the characters, by getting the outlines
    // and calling the glu tesselation code:
    void generateFront(const SbString &string, float width);
    
    // Static callbacks invoked by the glu tesselation code:
    static void beginCB(GLenum primType);
    static void endCB();
    static void vtxCB(void *vertex);
    
    // Private data:
    // MyOutlineFontCache is an internal, opaque class used to
    // maintain gl display lists and other information for each
    // character in a font.
    MyOutlineFontCache *myFont;

    // All this stuff is used while generating primitives:
    static SoAsciiText *currentGeneratingNode;
    static SoPrimitiveVertex *genPrimVerts[3];
    static SbVec3f genTranslate;
    static int genWhichVertex;
    static uint32_t genPrimType;
    static SoAction *genAction;
    static SbBool genBack;
    static SbBool genTexCoord;
    static const SoTextureCoordinateElement *tce;
};

#endif /* _SO_ASCIITEXT_ */
