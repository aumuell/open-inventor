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
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Description:
 |	Defines the SoFontStyle class
 |
 |   Author(s): Chris Marrin
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_FONTSTYLE_
#define  _SO_FONTSTYLE_

#include <Inventor/fields/SoSFBitMask.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/actions/SoCallbackAction.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoFontStyle
//
//////////////////////////////////////////////////////////////////////////////

// C-api: public=family,style
class SoFontStyle : public SoFont {

    SO_NODE_HEADER(SoFontStyle);

  public:
    // Constructor
    SoFontStyle();

    enum Family {
	SERIF,		    // Use Serif style (such as Times-Roman)
	SANS, 		    // Use Sans Serif style (such as Helvetica)
	TYPEWRITER	    // Use fixed pitch style (such as Courier)
    };

    enum Style {
	NONE = 0,	    // No modification to Family
	BOLD = 0x1, 	    // Embolden Family
	ITALIC = 0x2	    // Italicize or Slant Family
    };

    // Fields
    SoSFEnum	family;
    SoSFBitMask	style;
    
    // Return the font name used by this node based on the settings of family and style
    SbString		getFontName();

  SoEXTENDER public:
    virtual void        doAction(SoAction *action);
    virtual void        callback(SoCallbackAction *action);
    virtual void        GLRender(SoGLRenderAction *action);
    virtual void        getBoundingBox(SoGetBoundingBoxAction *action);
    virtual void        pick(SoPickAction *action);

  SoINTERNAL public:
    static void		initClass();

  protected:
    virtual ~SoFontStyle();
    
  private:
    static char *fontList[][4];
};

#endif /* _SO_FONTSTYLE_ */
