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
 |	SoSFPath
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoDB.h>
#include <Inventor/SoPath.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/fields/SoSFPath.h>

//////////////////////////////////////////////////////////////////////////////
//
// SoSFPath class - contains a pointer to an SoPath. This has to
// implement some extra path bookkeeping.
//
//////////////////////////////////////////////////////////////////////////////

// Use standard definitions of required methods, but we have to
// redefine the rest to keep track of references and auditors.

SO_SFIELD_REQUIRED_SOURCE(SoSFPath);


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoSFPath::SoSFPath()
//
////////////////////////////////////////////////////////////////////////
{
    // This will prevent treating random memory as an SoPath
    value = NULL;
    head  = NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: public

SoSFPath::~SoSFPath()
//
////////////////////////////////////////////////////////////////////////
{
    if (value != NULL) {
	if (value->getHead() != NULL)
	    value->getHead()->removeAuditor(this, SoNotRec::FIELD);
	value->removeAuditor(this, SoNotRec::FIELD);
	value->unref();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets value to given path pointer.
//
// Use: public

void
SoSFPath::setValue(SoPath *newValue)
//
////////////////////////////////////////////////////////////////////////
{
    setVal(newValue);
    valueChanged();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Equality test. (Standard definition.)
//
// Use: public

int
SoSFPath::operator ==(const SoSFPath &f) const
//
////////////////////////////////////////////////////////////////////////
{
    return getValue() == f.getValue();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Propagates notification through the field. We have to override
//    this to track changes to the path we are watching. For example,
//    if the path is truncated or the head node changes, we need to
//    reset our head node auditor.
//
//    NOTE: This code is essentially the same as the code in
//    SoPathSensor::notify().
//
// Use: internal

void
SoSFPath::notify(SoNotList *list)
//
////////////////////////////////////////////////////////////////////////
{
    // If there is nothing on the list, which would be the case if the
    // field has been read from file, do the notify thing.
    if (list->getFirstRec() == NULL) {
        SoSField::notify(list);
        return;
    }

    SbBool doNotify;

    // If the path started notification:
    if (list->getFirstRec()->getBase() == value)
	doNotify = TRUE;

    // If the notification came through the head node
    else
	doNotify = value->isRelevantNotification(list);

    // If the head node of the path changed, detach from old head and
    // attach to new:
    if (value->getHead() != head) {
	// Detach from old:
	if (head != NULL)
	    head->removeAuditor(this, SoNotRec::FIELD);
	head = value->getHead();
	// Attach to new:
	if (head != NULL)
	    head->addAuditor(this, SoNotRec::FIELD);
    }

    // Notify if we're supposed to...
    if (doNotify)
	SoSField::notify(list);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads value from file. Returns FALSE on error.
//
// Use: private

SbBool
SoSFPath::readValue(SoInput *in)
//
////////////////////////////////////////////////////////////////////////
{
    SbName	name;
    SoBase	*base;

    // See if it's a null pointer
    if (in->read(name)) {
	if (name == "NULL") {
	    setVal(NULL);
	    return TRUE;
	}
	else
	    in->putBack(name.getString());
    }

    // Read path
    if (! SoBase::read(in, base, SoPath::getClassTypeId())) {
	setVal(NULL);
	return FALSE;
    }

    setVal((SoPath *) base);

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides SoField::countWriteRefs() to maintain write-references
//    in path.
//
// Use: private

void
SoSFPath::countWriteRefs(SoOutput *out) const
//
////////////////////////////////////////////////////////////////////////
{
    // Do standard counting for field
    SoField::countWriteRefs(out);

    // Count path
    if (value != NULL) {
	SoWriteAction	wa(out);
	wa.continueToApply(value);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes value of field to file.
//
// Use: private

void
SoSFPath::writeValue(SoOutput *out) const
//
////////////////////////////////////////////////////////////////////////
{
    if (value != NULL) {
	SoWriteAction	wa(out);
	wa.continueToApply(value);
    }
    else
	out->write("NULL");
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Changes value in field without doing other notification stuff.
//    Keeps track of references and auditors.
//
// Use: private

void
SoSFPath::setVal(SoPath *newValue)
//
////////////////////////////////////////////////////////////////////////
{
    // Play it safe if we are replacing one pointer with the same pointer
    if (newValue != NULL)
	newValue->ref();

    // Get rid of old path, if any
    if (value != NULL) {

	// Remove auditor on head node
	if (head != NULL)
	    head->removeAuditor(this, SoNotRec::FIELD);

	// Remove auditor on path itself
	value->removeAuditor(this, SoNotRec::FIELD);

	value->unref();
    }

    value = newValue;

    if (value != NULL) {

	value->ref();

	// Add auditor on path
	value->addAuditor(this, SoNotRec::FIELD);

	// Add auditor on head node. (For efficiency, a path does not
	// audit its head node directly. Things that rely on this
	// notification - e.g., this field and path sensors - have to
	// do it themselves.)
	head = value->getHead();
	if (head != NULL)
	    head->addAuditor(this, SoNotRec::FIELD);
    }

    if (newValue != NULL)
	newValue->unref();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Update a copied field to use node copies if the head node has
//    been copied.
//
// Use: internal

void
SoSFPath::fixCopy(SbBool copyConnections)
//
////////////////////////////////////////////////////////////////////////
{
    if (value != NULL) {

	// If the head of the path has been copied, then each of the
	// other nodes in the path must have been copied as
	// well. Change the copied path to start at the copy and go
	// through the copied nodes.

	SoNode *headCopy = (SoNode *)
	    SoFieldContainer::findCopy(value->getHead(), copyConnections);

	if (headCopy != NULL) {

	    // Create a new path through the copied nodes
	    SoPath *pathCopy = new SoPath(headCopy);
	    pathCopy->ref();
	    for (int i = 1; i < ((SoFullPath *) value)->getLength(); i++)
		pathCopy->append(value->getIndex(i));

	    setVal(pathCopy);
	    pathCopy->unref();
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Override this to also check the head of the stored path.
//
// Use: internal, virtual

SbBool
SoSFPath::referencesCopy() const
//
////////////////////////////////////////////////////////////////////////
{
    // Do the normal test, and also see if the head of the stored path
    // is a copy
    return (SoSField::referencesCopy() ||
	    (value != NULL &&
	     SoFieldContainer::checkCopy(value->getHead()) != NULL));
}
