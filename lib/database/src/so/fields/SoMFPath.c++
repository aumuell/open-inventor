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
 |   $Revision: 1.2 $
 |
 |   Classes:
 |	SoMFPath
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoDB.h>
#include <Inventor/SoPath.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/fields/SoMFPath.h>

//////////////////////////////////////////////////////////////////////////////
//
// SoMFPath class - contains pointers to SoPaths. This has to
// implement some extra path bookkeeping.
//
//////////////////////////////////////////////////////////////////////////////

// Use standard definitions of required methods, but we have to
// redefine the rest to keep track of references and auditors.

SO_MFIELD_REQUIRED_SOURCE(SoMFPath);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor (standard stuff)
//
// Use: public

SoMFPath::SoMFPath()
//
////////////////////////////////////////////////////////////////////////
{
    values = NULL;
    heads = NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: public

SoMFPath::~SoMFPath()
//
////////////////////////////////////////////////////////////////////////
{
    deleteAllValues();
}

////////////////////////////////////////////////////////////////////////
//
// These all override the definitions in SO_MFIELD_VALUE_SOURCE() to
// keep track of references and auditors.
//
////////////////////////////////////////////////////////////////////////

int
SoMFPath::find(SoPath *targetValue, SbBool addIfNotFound)
{
    int	i, num = getNum();

    for (i = 0; i < num; i++)
	if (values[i] == targetValue)
	    return i;

    if (addIfNotFound)
	set1Value(num, targetValue);

    return -1;
}

void
SoMFPath::setValues(int start, int num, const SoPath **newValues)
{
    int	newNum = start + num, i;

    if (newNum > getNum())
	makeRoom(newNum);

    for (i = 0; i < num; i++)
	setVal(start + i, (SoPath *) newValues[i]);

    valueChanged();
}

void
SoMFPath::set1Value(int index, SoPath *newValue)
{
    if (index >= getNum())
	makeRoom(index + 1);
    setVal(index, newValue);
    valueChanged();
}

void
SoMFPath::setValue(SoPath *newValue)
{
    makeRoom(1);
    setVal(0, newValue);
    valueChanged();
}

int
SoMFPath::operator ==(const SoMFPath &f) const
{
    int			i, num = getNum();
    const SoPath	**myVals, **itsVals;

    if (num != f.getNum())
	return FALSE;

    myVals  = getValues(0);
    itsVals = f.getValues(0);

    for (i = 0; i < num; i++)
	if (! (myVals[i] == itsVals[i]))
	    return FALSE;

    return TRUE;
}

void
SoMFPath::deleteAllValues()
{
    allocValues(0);
}

void
SoMFPath::copyValue(int to, int from)
{
    if (to != from)
	setVal(to, values[from]);
}

////////////////////////////////////////////////////////////////////////
//
// This overrides the definition in SO_MFIELD_ALLOC_SOURCE() to
// keep track of references and auditors.
//
////////////////////////////////////////////////////////////////////////

void
SoMFPath::allocValues(int newNum)
{
    int	i;

    if (values == NULL)	{
	if (newNum > 0) {
	    values = new SoPath * [newNum];

	    // Make sure all pointers are initialized to NULL
	    for (i = 0; i < newNum; i++)
		values[i] = NULL;
	}
    }
    else {
	SoPath	**oldValues = values;

	if (newNum > 0) {
	    values = new SoPath * [newNum];
	    for (i = 0; i < num && i < newNum; i++)
		values[i] = oldValues[i];

	    // Initialize unused pointers to NULL
	    for (i = num; i < newNum; i++)
		values[i] = NULL;
	}
	else
	    values = NULL;

	// Free up any old stuff
	if (oldValues != NULL) {

	    // Remove auditors and references on unused values
	    for (i = newNum; i < num; i++) {
		SoPath	*path = oldValues[i];
		if (path != NULL) {
		    if (path->getHead() != NULL)
			path->getHead()->removeAuditor(this, SoNotRec::FIELD);
		    path->removeAuditor(this, SoNotRec::FIELD);
		    path->unref();
		}
	    }

	    delete [] oldValues;
	}
    }

    // Reallocate heads array and fill it in
    if (heads != NULL)
	delete [] heads;

    if (newNum > 0) {
	heads = new SoNode * [newNum];
	for (i = 0; i < newNum; i++)
	    heads[i] = (values[i] != NULL ? values[i]->getHead() : NULL);
    }

    num = maxNum = newNum;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Propagates notification through the field. We have to override
//    this to track changes to the paths we are watching. For example,
//    if a path is truncated or its head node changes, we need to
//    reset our head node auditor.
//
//    NOTE: This code is essentially the same as the code in
//    SoPathSensor::notify().
//
// Use: internal

void
SoMFPath::notify(SoNotList *list)
//
////////////////////////////////////////////////////////////////////////
{
    // If there is nothing on the list, which would be the case if the
    // field has been read from file, do the notify thing.
    if (list->getFirstRec() == NULL) {
	SoMField::notify(list);
        return;
    }

    SbBool		doNotify;
    int			i;
    const SoBase 	*firstBase = list->getFirstRec()->getBase();


    // Find the path or path head node that started notification
    for (i = 0; i < num; i++) {
	if (firstBase == values[i] || firstBase == heads[i])
	    break;
    }

    // This should never happen...
    if (i == num)
	return;

    // If the path started notification:
    if (firstBase == values[i])
	doNotify = TRUE;

    // If the notification came through the head node
    else if (firstBase == heads[i])
	doNotify = values[i]->isRelevantNotification(list);

    // If the head node of the path changed, detach from old head and
    // attach to new:
    if (values[i]->getHead() != heads[i]) {
	// Detach from old:
	if (heads[i] != NULL)
	    heads[i]->removeAuditor(this, SoNotRec::FIELD);
	heads[i] = values[i]->getHead();
	// Attach to new:
	if (heads[i] != NULL)
	    heads[i]->addAuditor(this, SoNotRec::FIELD);
    }

    // Notify if we're supposed to...
    if (doNotify)
	SoMField::notify(list);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads one (indexed) value from file. Returns FALSE on error.
//
// Use: private

SbBool
SoMFPath::read1Value(SoInput *in, int index)
//
////////////////////////////////////////////////////////////////////////
{
    SbName	name;
    SoBase	*base;

    // See if it's a null pointer
    if (in->read(name)) {
	if (name == "NULL") {
	    setVal(index, NULL);
	    return TRUE;
	}
	else
	    in->putBack(name.getString());
    }

    // Read path
    if (! SoBase::read(in, base, SoPath::getClassTypeId())) {
	setVal(index, NULL);
	return FALSE;
    }

    // Set value (adds a reference to path)
    setVal(index, (SoPath *) base);

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides SoField::countWriteRefs() to maintain write-references
//    in paths.
//
// Use: private

void
SoMFPath::countWriteRefs(SoOutput *out) const
//
////////////////////////////////////////////////////////////////////////
{
    // Do standard counting for field
    SoField::countWriteRefs(out);

    // Count paths
    for (int i = 0; i < num; i++) {
	SoPath *path = values[i];

	if (path != NULL) {
	    SoWriteAction	wa(out);
	    wa.continueToApply(path);
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes value of field to file.
//
// Use: private

void
SoMFPath::write1Value(SoOutput *out, int index) const
//
////////////////////////////////////////////////////////////////////////
{
    if (values[index] != NULL) {
	SoWriteAction	wa(out);
	wa.continueToApply(values[index]);
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
SoMFPath::setVal(int index, SoPath *newValue)
//
////////////////////////////////////////////////////////////////////////
{
    SoPath	*value = values[index];

    // Play it safe if we are replacing one pointer with the same pointer
    if (newValue != NULL)
	newValue->ref();

    // Get rid of old path, if any
    if (value != NULL) {

	// Remove auditor on head node
	if (heads[index] != NULL)
	    heads[index]->removeAuditor(this, SoNotRec::FIELD);

	// Remove auditor on path itself
	value->removeAuditor(this, SoNotRec::FIELD);

	value->unref();
    }

    value = values[index] = newValue;

    if (value != NULL) {

	value->ref();

	// Add auditor on path
	value->addAuditor(this, SoNotRec::FIELD);

	// Add auditor on head node. (For efficiency, a path does not
	// audit its head node directly. Things that rely on this
	// notification - e.g., this field and path sensors - have to
	// do it themselves.)
	heads[index] = value->getHead();
	if (heads[index] != NULL)
	    heads[index]->addAuditor(this, SoNotRec::FIELD);
    }

    if (newValue != NULL)
	newValue->unref();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Update a copied field to use node copies if the head nodes have
//    been copied.
//
// Use: internal

void
SoMFPath::fixCopy(SbBool copyConnections)
//
////////////////////////////////////////////////////////////////////////
{
    for (int i = 0; i < num; i++) {
	if (values[i] != NULL) {

	    // If the head of the path has been copied, then each of
	    // the other nodes in the path must have been copied as
	    // well. Change the copied path to start at the copy and
	    // go through the copied nodes.

	    SoNode *headCopy = (SoNode *)
		SoFieldContainer::findCopy(values[i]->getHead(),
					   copyConnections);

	    if (headCopy != NULL) {

		// Create a new path through the copied nodes
		SoPath *pathCopy = new SoPath(headCopy);
		pathCopy->ref();
		int len = ((SoFullPath *) values[i])->getLength();
		for (int j = 1; j < len; j++)
		    pathCopy->append(values[j]->getIndex(j));

		setVal(i, pathCopy);
		pathCopy->unref();
	    }
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
SoMFPath::referencesCopy() const
//
////////////////////////////////////////////////////////////////////////
{
    // Do the normal test, and also see if the heads of the stored paths
    // are copies
    if (SoMField::referencesCopy())
	return TRUE;

    for (int i = 0; i < num; i++)
	if (values[i] != NULL &&
	    SoFieldContainer::checkCopy(values[i]->getHead()) != NULL)
	    return TRUE;

    return FALSE;
}
