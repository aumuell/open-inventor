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
 |   $Revision: 1.1.1.1 $
 |
 |   Classes	: SoXtImportInterestList
 |
 |   Author(s)	: David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include "SoXtImportInterest.h"

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Append to the paste interest list. cb will be called when dataType
// is pasted.
//
void
SoXtImportInterestList::append(
    Atom dataType, SoXtClipboardImportCB *cb, void *userData)
//
////////////////////////////////////////////////////////////////////////
{
    // Remove this dataType if already registered
    int which = find(dataType);
    remove(which);
    
    // If cb is set, add it to the list
    if (cb != NULL) {
	SoXtImportInterest *data = new SoXtImportInterest(dataType, cb, userData);
	list.append(data);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Find an entry for the passed dataType in the paste list.
// Returns -1 if no entry found.
//
int
SoXtImportInterestList::find(Atom dataType)
//
////////////////////////////////////////////////////////////////////////
{
    for (int i = 0; i < list.getLength(); i++) {
	SoXtImportInterest *data = (SoXtImportInterest *) list[i];
	if (data->getDataType() == dataType)
	    return i;
    }
    
    return -1;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Remove the passed entry from the paste interest list.
//
void
SoXtImportInterestList::remove(int which)
//
////////////////////////////////////////////////////////////////////////
{
    if (which >= 0 && which < list.getLength()) {
	SoXtImportInterest *data = (SoXtImportInterest *) list[which];
	delete data;
	list.remove(which);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Return the interest data for 'which' item in the list.
//
const SoXtImportInterest *
SoXtImportInterestList::get(int which)
//
////////////////////////////////////////////////////////////////////////
{
    if (which >= 0 && which < list.getLength())
	 return (SoXtImportInterest *) list[which];
    else return NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Return the data type for 'which' item in the list.
//
Atom
SoXtImportInterestList::getDataType(int which)
//
////////////////////////////////////////////////////////////////////////
{
    const SoXtImportInterest *data = get(which);
    if (data != NULL)
	 return data->getDataType();
    else return 0;
}

