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
 |	SoMFNode
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoDB.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/fields/SoMFNode.h>

//////////////////////////////////////////////////////////////////////////////
//
// SoMFNode class - contains pointers to SoNodes. This has to
// implement some extra node bookkeeping.
//
//////////////////////////////////////////////////////////////////////////////

// Use standard definitions of required methods, but we have to
// redefine the rest to keep track of references and auditors.

SO_MFIELD_REQUIRED_SOURCE(SoMFNode);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor (standard stuff)
//
// Use: public

SoMFNode::SoMFNode()
//
////////////////////////////////////////////////////////////////////////
{
    values = NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: public

SoMFNode::~SoMFNode()
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
SoMFNode::find(SoNode *targetValue, SbBool addIfNotFound)
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
SoMFNode::setValues(int start, int num, const SoNode **newValues)
{
    int	newNum = start + num, i;

    if (newNum > getNum())
	makeRoom(newNum);

    for (i = 0; i < num; i++)
	setVal(start + i, (SoNode *) newValues[i]);

    valueChanged();
}

void
SoMFNode::set1Value(int index, SoNode *newValue)
{
    if (index >= getNum())
	makeRoom(index + 1);
    setVal(index, newValue);
    valueChanged();
}

void
SoMFNode::setValue(SoNode *newValue)
{
    makeRoom(1);
    setVal(0, newValue);
    valueChanged();
}

int
SoMFNode::operator ==(const SoMFNode &f) const
{
    int			i, num = getNum();
    const SoNode	**myVals, **itsVals;

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
SoMFNode::deleteAllValues()
{
    allocValues(0);
}

void
SoMFNode::copyValue(int to, int from)
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
SoMFNode::allocValues(int newNum)
{
    int	i;

    if (values == NULL)	{
	if (newNum > 0) {
	    values = new SoNode * [newNum];

	    // Make sure all pointers are initialized to NULL
	    for (i = 0; i < newNum; i++)
		values[i] = NULL;
	}
    }
    else {
	SoNode	**oldValues = values;

	if (newNum > 0) {
	    values = new SoNode * [newNum];
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
		if (oldValues[i] != NULL) {
		    oldValues[i]->removeAuditor(this, SoNotRec::FIELD);
		    oldValues[i]->unref();
		}
	    }

	    delete [] oldValues;
	}
    }

    num = maxNum = newNum;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads one (indexed) value from file. Returns FALSE on error.
//
// Use: private

SbBool
SoMFNode::read1Value(SoInput *in, int index)
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

    // Read node
    if (! SoBase::read(in, base, SoNode::getClassTypeId())) {
	setVal(index, NULL);
	return FALSE;
    }

    // Set value (adds a reference to node)
    setVal(index, (SoNode *) base);

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides SoField::countWriteRefs() to maintain write-references
//    in nodes.
//
// Use: private

void
SoMFNode::countWriteRefs(SoOutput *out) const
//
////////////////////////////////////////////////////////////////////////
{
    // Do standard counting for field
    SoField::countWriteRefs(out);

    // Count nodes
    for (int i = 0; i < num; i++) {
	SoNode *node = values[i];

	if (node != NULL)
	    node->writeInstance(out);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes value of field to file.
//
// Use: private

void
SoMFNode::write1Value(SoOutput *out, int index) const
//
////////////////////////////////////////////////////////////////////////
{
    if (values[index] != NULL)
	values[index]->writeInstance(out);
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
SoMFNode::setVal(int index, SoNode *newValue)
//
////////////////////////////////////////////////////////////////////////
{
    SoNode	*value = values[index];

    // Play it safe if we are replacing one pointer with the same pointer
    if (newValue != NULL)
	newValue->ref();

    // Get rid of old node, if any
    if (value != NULL) {
	value->removeAuditor(this, SoNotRec::FIELD);
	value->unref();
    }

    value = values[index] = newValue;

    if (value != NULL) {
	value->ref();
	value->addAuditor(this, SoNotRec::FIELD);
    }

    if (newValue != NULL)
	newValue->unref();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Update a copied field to use the copy of each node if there is
//    one.
//
// Use: internal

void
SoMFNode::fixCopy(SbBool copyConnections)
//
////////////////////////////////////////////////////////////////////////
{
    for (int i = 0; i < num; i++) {
	if (values[i] != NULL) {
	    SoNode *nodeCopy = (SoNode *)
		SoFieldContainer::findCopy(values[i], copyConnections);
	    if (nodeCopy != NULL)
		setVal(i, nodeCopy);
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Override this to also check the stored nodes.
//
// Use: internal, virtual

SbBool
SoMFNode::referencesCopy() const
//
////////////////////////////////////////////////////////////////////////
{
    // Do the normal test, and also see if the stored nodes are copies
    if (SoMField::referencesCopy())
	return TRUE;

    for (int i = 0; i < num; i++)
	if (values[i] != NULL &&
	    SoFieldContainer::checkCopy(values[i]) != NULL)
	    return TRUE;

    return FALSE;
}
