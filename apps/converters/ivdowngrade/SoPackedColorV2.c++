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
 |	SoPackedColorV2
 |
 |   Authors: David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/elements/SoLazyElement.h>
#include <Inventor/actions/SoWriteAction.h>
#include "SoPackedColorV2.h"

SO_NODE_SOURCE(SoPackedColorV2);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    init that class!
//
void
SoPackedColorV2::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoPackedColorV2, "PackedColor", SoPackedColor);

    // Tell the type system whenever it wants to create something
    // of type SoPackedColor to create a SoPackedColorV2
    SoType::overrideType(
		 SoPackedColor::getClassTypeId(), &SoPackedColorV2::createInstance);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
SoPackedColorV2::SoPackedColorV2()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoPackedColorV2);
    SO_NODE_ADD_FIELD(rgba, (SoLazyElement::getDefaultPacked()));	 // 2.0 field

    // This is a built in node in all versions of Inventor
    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
SoPackedColorV2::~SoPackedColorV2()
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
SoPackedColorV2::write(SoWriteAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // We don't to trigger notification during writing. 
    enableNotify(FALSE);
    
    SoOutput	*out = action->getOutput();

    // In write-reference counting phase
    if (out->getStage() == SoOutput::COUNT_REFS) {
	if (! orderedRGBA.isDefault()) {
	    // Put the RGBA values into the rgba field, reversing the byte order
	    // and write out rgba, not orderedRGBA
	    for (int i= 0; i< orderedRGBA.getNum(); i++){
		uint32_t val = orderedRGBA[i];
		uint32_t newval = ((val&0xff) << 24)|((val&0xff00) << 8)|
		    ((val&0xff0000) >> 8)|((val & 0xff000000)>>24);	
		rgba.set1Value(i, newval);    
	    }
	    
	    orderedRGBA.setDefault(TRUE);
	}
    }
    
    SoPackedColor::write(action);
}

