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
 * Copyright (C) 1995-96   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   SoFontStyle implementation.
 |
 |   Author(s)	: Chris Marrin
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <assert.h>

#include <Inventor/actions/SoAction.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoPickAction.h>
#include <Inventor/elements/SoFontNameElement.h>
#include <Inventor/elements/SoFontSizeElement.h>
#include <Inventor/elements/SoOverrideElement.h>
#include <Inventor/nodes/SoFontStyle.h>

SO_NODE_SOURCE(SoFontStyle);

char *SoFontStyle::fontList[][4] = 
		    {
			{ "Utopia-Regular", "Utopia-Bold", 
			  "Utopia-Italic", "Utopia-BoldItalic" }, 
			{ "Helvetica", "Helvetica-Bold", 
			  "Helvetica-Oblique", "Helvetica-BoldOblique" }, 
			{ "Courier", "Courier-Bold", 
			  "Courier-Oblique", "Courier-BoldOblique" }, 
		    };

////////////////////////////////////////////////////////////////////////
//
//  Constructor
//  
SoFontStyle::SoFontStyle()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoFontStyle);
    SO_NODE_ADD_FIELD(family, (SERIF));
    SO_NODE_ADD_FIELD(style, (NONE));

    // Set up static info for enumerated type field
    SO_NODE_DEFINE_ENUM_VALUE(Family, SERIF);
    SO_NODE_DEFINE_ENUM_VALUE(Family, SANS);
    SO_NODE_DEFINE_ENUM_VALUE(Family, TYPEWRITER);

    SO_NODE_DEFINE_ENUM_VALUE(Style, NONE);
    SO_NODE_DEFINE_ENUM_VALUE(Style, BOLD);
    SO_NODE_DEFINE_ENUM_VALUE(Style, ITALIC);

    // Set up info in enumerated type field
    SO_NODE_SET_SF_ENUM_TYPE(family, Family);
    SO_NODE_SET_SF_ENUM_TYPE(style, Style);

    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
//  Destructor
//  
SoFontStyle::~SoFontStyle()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Gets the font name based on the family and style
//
// Use: protected

SbString
SoFontStyle::getFontName()
//
////////////////////////////////////////////////////////////////////////
{   
    int f = family.getValue();
    int s = style.getValue();
    
    if (family.isIgnored()) f = SERIF;
    if (style.isIgnored()) s = NONE;
    
    if (f < 0 || f > 2) f = 0;
    if (s < 0 || s > 3) s = 0;
    
    return fontList[f][s];
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Handles actions
//
// Use: extender

void
SoFontStyle::doAction(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{   
    SoState *state = action->getState();

    // set font to the given font style
    if ((!family.isIgnored() || !style.isIgnored()) 
	&& !SoOverrideElement::getFontNameOverride(state)
							) {
	
	SbString font = getFontName();
	
	if (isOverride()) {
	    SoOverrideElement::setFontNameOverride(state, this, TRUE);
	}
	SoFontNameElement::set(state, this, font.getString());
    }
    
    if (! size.isIgnored()
	&& ! SoOverrideElement::getFontSizeOverride(state)
							) {
	if (isOverride()) {
	    SoOverrideElement::setFontSizeOverride(state, this, TRUE);
	}
	SoFontSizeElement::set(state, this, size.getValue());
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does callback action thing.
//
// Use: extender

void
SoFontStyle::callback(SoCallbackAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoFontStyle::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does GL render action.
//
// Use: extender

void
SoFontStyle::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoFontStyle::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does pick action...
//
// Use: extender

void
SoFontStyle::pick(SoPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoFontStyle::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does get bounding box action...
//
// Use: extender

void
SoFontStyle::getBoundingBox(SoGetBoundingBoxAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoFontStyle::doAction(action);
}
