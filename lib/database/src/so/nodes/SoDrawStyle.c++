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
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |      SoDrawStyle
 |
 |   Author(s)          : Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/elements/SoDrawStyleElement.h>
#include <Inventor/elements/SoLinePatternElement.h>
#include <Inventor/elements/SoLineWidthElement.h>
#include <Inventor/elements/SoPointSizeElement.h>
#include <Inventor/elements/SoOverrideElement.h>
#include <Inventor/nodes/SoDrawStyle.h>

SO_NODE_SOURCE(SoDrawStyle);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoDrawStyle::SoDrawStyle()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoDrawStyle);

    SO_NODE_ADD_FIELD(style,		(SoDrawStyleElement::getDefault()));
    SO_NODE_ADD_FIELD(pointSize,	(SoPointSizeElement::getDefault()));
    SO_NODE_ADD_FIELD(lineWidth,	(SoLineWidthElement::getDefault()));
    SO_NODE_ADD_FIELD(linePattern,	(SoLinePatternElement::getDefault()));

    // Set up static info for enumerated type field
    SO_NODE_DEFINE_ENUM_VALUE(Style, FILLED);
    SO_NODE_DEFINE_ENUM_VALUE(Style, LINES);
    SO_NODE_DEFINE_ENUM_VALUE(Style, POINTS);
    SO_NODE_DEFINE_ENUM_VALUE(Style, INVISIBLE);

    // Set up info in enumerated type field
    SO_NODE_SET_SF_ENUM_TYPE(style, Style);

    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoDrawStyle::~SoDrawStyle()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Handles actions.
//
// Use: extender

void
SoDrawStyle::doAction(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoState	*state = action->getState();
    SbBool	isFilled;

    if (! style.isIgnored()
	&& ! SoOverrideElement::getDrawStyleOverride(state)) {
	if (isOverride()) {
	    SoOverrideElement::setDrawStyleOverride(state, this, TRUE);
	}
	SoDrawStyleElement::set(state,
				(SoDrawStyleElement::Style) style.getValue());
	isFilled = style.getValue() == FILLED;
    }
    else
	isFilled = TRUE;

    if (! pointSize.isIgnored()
	&& ! SoOverrideElement::getPointSizeOverride(state)) {
	if (isOverride()) {
	    SoOverrideElement::setPointSizeOverride(state, this, TRUE);
	}
	SoPointSizeElement::set(state, pointSize.getValue());
    }

    if (! lineWidth.isIgnored()
	&& ! SoOverrideElement::getLineWidthOverride(state)) {
	if (isOverride()) {
	    SoOverrideElement::setLineWidthOverride(state, this, TRUE);
	}
	SoLineWidthElement::set(state, lineWidth.getValue());
    }

    if (! linePattern.isIgnored()
	&& ! SoOverrideElement::getLinePatternOverride(state)) {
	if (isOverride()) {
	    SoOverrideElement::setLinePatternOverride(state, this, TRUE);
	}
	SoLinePatternElement::set(state, linePattern.getValue());
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does callback action thing.
//
// Use: extender

void
SoDrawStyle::callback(SoCallbackAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoDrawStyle::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does GL render action.
//
// Use: extender

void
SoDrawStyle::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoDrawStyle::doAction(action);
}
