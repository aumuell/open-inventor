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
 |   Classes:
 |	SoFontStyleV2
 |
 |   Authors: David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/nodes/SoFont.h>
#include "SoFontStyleV2.h"
#include "Util.h"

SO_NODE_SOURCE(SoFontStyleV2);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    init that class!
//
void
SoFontStyleV2::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoFontStyleV2, "FontStyle", SoFontStyle);

    // Tell the type system whenever it wants to create something
    // of type SoFontStyle to create a SoFontStyleV2
    SoType::overrideType(
		 SoFontStyle::getClassTypeId(), &SoFontStyleV2::createInstance);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
SoFontStyleV2::SoFontStyleV2()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoFontStyleV2);
    SO_NODE_ADD_FIELD(alternateRep, (NULL));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
SoFontStyleV2::~SoFontStyleV2()
//
////////////////////////////////////////////////////////////////////////
{
    
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    write the alternate rep for this node
//
void
SoFontStyleV2::write(SoWriteAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoOutput	*out = action->getOutput();

    // In write-reference counting phase
    if (out->getStage() == SoOutput::COUNT_REFS) {
	// Only do this once!
	if (alternateRep.isDefault()) {
	    SoFont *font = new SoFont;
	    alternateRep = font;
	    
	    // Set the Font name based on the FontStyle family and style
	    font->name = getFontName();
	    
	    COPY_FIELD(font, this, size);
    
	    // Inventor V2.0 cannot read unknown nodes with SoSFEnum or SoSFBitMask fields.
	    // Don't let any enums get written out.
	    // NOTE: a better solution might be to write the enum as a string,
	    // and have the upgrader for this node convert that string back
	    // to an enum value. But this bug was found too close to the 2.1 MR date
	    // to do that more complicated fix. Instead, just don't write the enum.
	    family.setDefault(TRUE);
	    style.setDefault(TRUE);
	}
    }
    
    SoFontStyle::write(action);
}

