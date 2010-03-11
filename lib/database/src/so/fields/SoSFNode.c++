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
 |	SoSFNode
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoDB.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/fields/SoSFNode.h>

//////////////////////////////////////////////////////////////////////////////
//
// SoSFNode class - contains a pointer to an SoNode. This has to
// implement some extra node bookkeeping.
//
//////////////////////////////////////////////////////////////////////////////

// Use standard definitions of required methods, but we have to
// redefine the rest to keep track of references and auditors.

SO_SFIELD_REQUIRED_SOURCE(SoSFNode);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoSFNode::SoSFNode()
//
////////////////////////////////////////////////////////////////////////
{
    // This will prevent treating random memory as an SoNode
    value = NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: public

SoSFNode::~SoSFNode()
//
////////////////////////////////////////////////////////////////////////
{
    if (value != NULL) {
	value->removeAuditor(this, SoNotRec::FIELD);
	value->unref();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets value to given node pointer.
//
// Use: public

void
SoSFNode::setValue(SoNode *newValue)
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
SoSFNode::operator ==(const SoSFNode &f) const
//
////////////////////////////////////////////////////////////////////////
{
    return getValue() == f.getValue();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads value from file. Returns FALSE on error.
//
// Use: private

SbBool
SoSFNode::readValue(SoInput *in)
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
    
    // Read node
    if (! SoBase::read(in, base, SoNode::getClassTypeId())) {
	setVal(NULL);
	return FALSE;
    }

    setVal((SoNode *) base);

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides SoField::countWriteRefs() to maintain write-references
//    in node.
//
// Use: private

void
SoSFNode::countWriteRefs(SoOutput *out) const
//
////////////////////////////////////////////////////////////////////////
{
    // Do standard counting for field
    SoField::countWriteRefs(out);

    // Count node
    if (value != NULL)
	value->writeInstance(out);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes value of field to file.
//
// Use: private

void
SoSFNode::writeValue(SoOutput *out) const
//
////////////////////////////////////////////////////////////////////////
{
    if (value != NULL)
	value->writeInstance(out);
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
SoSFNode::setVal(SoNode *newValue)
//
////////////////////////////////////////////////////////////////////////
{
    // Play it safe if we are replacing one pointer with the same pointer
    if (newValue != NULL)
	newValue->ref();

    // Get rid of old node, if any
    if (value != NULL) {
	value->removeAuditor(this, SoNotRec::FIELD);
	value->unref();
    }

    value = newValue;

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
//    Update a copied field to use the copy of the node if there is
//    one.
//
// Use: internal

void
SoSFNode::fixCopy(SbBool copyConnections)
//
////////////////////////////////////////////////////////////////////////
{
    if (value != NULL) {
	SoNode *nodeCopy = (SoNode *)
	    SoFieldContainer::findCopy(value, copyConnections);
	if (nodeCopy != NULL)
	    setVal(nodeCopy);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Override this to also check the stored node.
//
// Use: internal, virtual

SbBool
SoSFNode::referencesCopy() const
//
////////////////////////////////////////////////////////////////////////
{
    // Do the normal test, and also see if the stored node is a copy
    return (SoSField::referencesCopy() ||
	    (value != NULL && SoFieldContainer::checkCopy(value) != NULL));
}
