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
 |	SoText2V2
 |
 |   Authors: Alan Norton
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/actions/SoWriteAction.h>
#include "SoText2V2.h"

char* convToAscii(const SbString& str);

SO_NODE_SOURCE(SoText2V2);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    init that class!
//
void
SoText2V2::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoText2V2, "Text2", SoText2);

    // Tell the type system whenever it wants to create something
    // of type SoText2 to create a SoText2V2
    SoType::overrideType(
		 SoText2::getClassTypeId(), &SoText2V2::createInstance);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
SoText2V2::SoText2V2()
//
////////////////////////////////////////////////////////////////////////
{    
    SO_NODE_CONSTRUCTOR(SoText2V2);

    // This is a built in node in all versions of Inventor
    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
SoText2V2::~SoText2V2()
//
////////////////////////////////////////////////////////////////////////
{
    
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    write the alternate rep for this node
//    We need to convert nonstandard (e.g. UTF-8) characters to legitimate
//    ascii values.
//
void
SoText2V2::write(SoWriteAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // We don't to trigger notification during writing. 
    enableNotify(FALSE);
    
    SoOutput	*out = action->getOutput();

    // Not in write-reference counting phase
    if (out->getStage() != SoOutput::COUNT_REFS) {
	if (! string.isDefault()) {
	    // go through every string in the field, make sure it's ascii	  
	    for (int i= 0; i< string.getNum(); i++)
		string.set1Value(i, convToAscii( string[i]));        	    
	}
    }
    
    SoText2::write(action);
}
