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
 |	SoWWWInlineV2
 |
 |   Authors: David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/actions/SoWriteAction.h>
#include "SoWWWInlineV2.h"

SO_NODE_SOURCE(SoWWWInlineV2);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    init that class!
//
void
SoWWWInlineV2::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoWWWInlineV2, "WWWInline", SoWWWInline);

    // Tell the type system whenever it wants to create something
    // of type SoWWWInline to create a SoWWWInlineV2
    SoType::overrideType(
		 SoWWWInline::getClassTypeId(), &SoWWWInlineV2::createInstance);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
SoWWWInlineV2::SoWWWInlineV2()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoWWWInlineV2);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
SoWWWInlineV2::~SoWWWInlineV2()
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
SoWWWInlineV2::write(SoWriteAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoOutput	*out = action->getOutput();

    // In write-reference counting phase
    if (out->getStage() == SoOutput::COUNT_REFS) {
	// add the children to the alternateRep so they can be written
	// there and USE'd in the real node.

	// NOTE: may be referenced more than once.  ONLY BUILD
	// alternateRep ONCE, THOUGH!
	
	// SoWWWInline already has an alternateRep field - make sure it is set
	if (alternateRep.getValue() == NULL && getChildData() != NULL) {
	    alternateRep.getValue()->enableNotify(FALSE);
	    alternateRep = getChildData();
	}
    }
    
    SoWWWInline::write(action);
}

