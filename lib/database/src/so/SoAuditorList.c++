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
 * Copyright (C) 1991,92   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |	SoAuditorList (internal)
 |
 |   Author(s): Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/errors/SoDebugError.h>
#include <Inventor/misc/SoAuditorList.h>
#include <Inventor/misc/SoBase.h>
#include <Inventor/fields/SoField.h>
#include <Inventor/sensors/SoDataSensor.h>

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor.
//
// Use: public

SoAuditorList::SoAuditorList() : SbPList()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: public

SoAuditorList::~SoAuditorList()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds an auditor of the given type to the list
//
// Use: public

void
SoAuditorList::append(void *auditor, SoNotRec::Type type)
//
////////////////////////////////////////////////////////////////////////
{
    SbPList::append(auditor);
    SbPList::append((void *) type);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets auditor and type for given index
//
// Use: public

void
SoAuditorList::set(int index, void *auditor, SoNotRec::Type type)
//
////////////////////////////////////////////////////////////////////////
{
    // This is often called to set the first auditor in the list
    // (which is the container of fields and engine outputs, so handle
    // that case specially)
    if (SbPList::getLength() == 0)
	append(auditor, type);

    else {
	SbPList *list = this;
	(*list)[index * 2]     = auditor;
	(*list)[index * 2 + 1] = (void *) type;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Finds an auditor in the list, returning the index or -1 if not found.
//
// Use: public

int
SoAuditorList::find(void *auditor, SoNotRec::Type type) const
//
////////////////////////////////////////////////////////////////////////
{
    int		i;

    for (i = 0; i < SbPList::getLength(); i += 2)
	if ((*this)[i] == auditor && (*this)[i+1] == (void *) type)
	    return i / 2;

    return -1;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns auditor object for given index.
//
// Use: public

void *
SoAuditorList::getObject(int index) const
//
////////////////////////////////////////////////////////////////////////
{
    return (*this)[index * 2];
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns type for given index.
//
// Use: public

SoNotRec::Type
SoAuditorList::getType(int index) const
//
////////////////////////////////////////////////////////////////////////
{
    return (SoNotRec::Type) ((unsigned long) (*this)[index * 2 + 1]);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Removes an auditor from the list.
//
// Use: public

void
SoAuditorList::remove(int index)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (index < 0 || index >= getLength()) {
	SoDebugError::post("SoAuditorList::remove",
			   "Index %d is out of range 0 - %d",
			   index, getLength() - 1);
	return;
    }
#endif /* DEBUG */

    SbPList::remove(index * 2 + 1);
    SbPList::remove(index * 2);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns number of auditors in list.
//    
//
// Use: public

int
SoAuditorList::getLength() const
//
////////////////////////////////////////////////////////////////////////
{
    return SbPList::getLength() / 2;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Propagates notification to all auditors in list.
//
// Use: public

void
SoAuditorList::notify(SoNotList *list)
//
////////////////////////////////////////////////////////////////////////
{
    int	numAuditors = getLength();

    // No auditors? Do nothing.
    if (numAuditors == 0)
	;

    // If there's only 1 auditor just propagate to it. (This is the
    // typical case, we assume.)
    else if (numAuditors == 1)
	notify1(list, 0);

    // Otherwise, we have to do some extra work. We have to make a
    // copy of the record list so we can propagate identical
    // information to each auditor.
    else {
	SoNotList	workingList(list);
	int		i;

	for (i = 0; i < numAuditors; i++) {

	    // Copy given list to working list after first time -
	    // each notification may change stuff in the list
	    if (i > 0)
		workingList = *list;

	    notify1(&workingList, i);
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Propagates notification to indexed auditor.
//
// Use: private

void
SoAuditorList::notify1(SoNotList *list, int index)
//
////////////////////////////////////////////////////////////////////////
{
    SoNotRec::Type	audType = getType(index);

    list->setLastType(audType);

    switch (audType) {

	// These 2 cases are notifying a base of some sort
      case SoNotRec::CONTAINER:
      case SoNotRec::PARENT:
	((SoBase *) getObject(index))->notify(list);
	break;
	
	// This one is notifying a data sensor
      case SoNotRec::SENSOR:
	((SoDataSensor *) getObject(index))->notify(list);
	break;
	
	// And these two are notifying a connected field
      case SoNotRec::FIELD:
      case SoNotRec::ENGINE:
	((SoField *) getObject(index))->notify(list);
	break;
    }
}
