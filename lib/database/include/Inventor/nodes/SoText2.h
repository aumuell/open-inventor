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
 |	This file defines the SoText2 node class.
 |
 |   Author(s)		: Thad Beier
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_TEXT_2_
#define  _SO_TEXT_2_

#include <Inventor/fields/SoMFString.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/nodes/SoShape.h>

// C-api: end
class SoBitmapFontCache;  // Internal class
// C-api: begin

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoText2
//
//  2D, screen-aligned text.  The origin of the text is always 0,0,0
//  in object space, but the text is always the same size (based on
//  the fontSize from the SoFont node) and is always aligned with the
//  screen.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoTxt2
// C-api: public=string, interval, justification
class SoText2 : public SoShape {

    SO_NODE_HEADER(SoText2);

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

    // Constructor
    SoText2();

  SoEXTENDER public:
    virtual void	GLRender(SoGLRenderAction *action);
    virtual void	rayPick(SoRayPickAction *action);

  SoINTERNAL public:
    static void		initClass();

  protected:
    // This method is a no-op for SoText2, since there are no
    // primitives it can generate
    virtual void	generatePrimitives(SoAction *action);

    // Computes bounding box of text
    virtual void	computeBBox(SoAction *action, SbBox3f &box,
				    SbVec3f &center);

    virtual ~SoText2();

  private:
    // Based on justification and line spacing, this returns the
    // offset (in pixels) for one of the lines of text.
    SbVec3f		getPixelStringOffset(int whichLine);

    // Internal class that allows Text2 nodes to share font
    // information, GL display lists, etc.
    SoBitmapFontCache	*myFont;
};

#endif /* _SO_TEXT_2_ */
