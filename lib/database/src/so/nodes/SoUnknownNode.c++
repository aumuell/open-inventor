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
 |	SoUnknownNode
 |
 |   Author(s)		: Paul S. Strauss, Dave Immel, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <stdlib.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/fields/SoMFString.h>
#include <Inventor/fields/SoSFNode.h>
#include <SoUnknownNode.h>

// Because of the special treatment of fieldData, we can't use the
// regular SO_NODE_SOURCE macro.
SO__NODE_VARS(SoUnknownNode);

SoType
SoUnknownNode::getTypeId() const
{
    return classTypeId;
}

const SoFieldData *
SoUnknownNode::getFieldData() const
{
    return instanceFieldData;
}

void *
SoUnknownNode::createInstance()
{
#ifdef DEBUG
    SoDebugError::post("SoUnknownNode::createInstance",
		       "Unknown nodes should be created only by "
		       "SoBase::read methods when reading");
#endif
    return (void *)(new SoUnknownNode());
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoUnknownNode::SoUnknownNode() : hiddenChildren(this)
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoUnknownNode);

    instanceFieldData = new SoFieldData(fieldData);
    className = NULL;

    hasChildren = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Set the class name of the unknown node
//
// Use: public

void
SoUnknownNode::setClassName( const char *name )

//
////////////////////////////////////////////////////////////////////////
{
    className = strdup(name);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoUnknownNode::~SoUnknownNode()
//
////////////////////////////////////////////////////////////////////////
{
    //
    // The fields of the unknown node have been allocated by the SoFieldData
    // class.  The unknown node will delete them here, because the field
    // data does not have enough information to know where its fields are
    // stored.  This could be redesigned.
    //
    SoFieldList fieldList;
    int         numFields = getFields(fieldList);

    for (int i=0; i<numFields; i++)
        delete fieldList[i];

    delete instanceFieldData;

    if (className) free((void *)className);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads field type data, fields, and children into instance of
//    SoUnknownNode. Returns FALSE on error.
//
// Use: virtual, protected

SbBool
SoUnknownNode::readInstance(SoInput *in, unsigned short flags)

//
////////////////////////////////////////////////////////////////////////
{
    int i;

    // This is mostly the normal SoGroup reading code, but we look for
    // an alternateRep field after reading and move our public
    // children onto the hidden children list:

    hasChildren = (!in->isBinary() || (flags & IS_GROUP));
    SbBool result;
    if (hasChildren) {
	result = SoGroup::readInstance(in, flags);

	// If read ASCII AND didn't read any children, set hasChildren
	// to FALSE:
	if (!in->isBinary() && getNumChildren() == 0) hasChildren = FALSE;

	// Add all kids to hiddenChildren, then remove them all from the
	// regular (SoGroup) list
	for (i = 0; i < getNumChildren(); i++) {
	    hiddenChildren.append(getChild(i));
	}
	removeAllChildren();
    }
    else {
	result = SoNode::readInstance(in, flags);
    }

    // Check to see if an alternate representation was read and
    // store a pointer to it if one is found.
    int num = instanceFieldData->getNumFields();
    SbBool haveAlternateRep = FALSE;
    for (i=0; i<num; i++) {
        if (instanceFieldData->getFieldName(i) == SbName("alternateRep")) {
	    SoField *f = instanceFieldData->getField(this, i);
	    if (f->isOfType(SoSFNode::getClassTypeId())) {
		haveAlternateRep = TRUE;
		SoSFNode *alternateRepField = (SoSFNode *)f;
		SoNode *n = alternateRepField->getValue();
		if (n != NULL)
		    addChild(n);
	    }
            break;
        }
    }

    // If no alternateRep was specified, look for a field named "isA"
    // of type MFString and try to automatically create an
    // alternateRep:
    if (!haveAlternateRep) for (i=0; i<num; i++) {
        if (instanceFieldData->getFieldName(i) == SbName("isA")) {
	    SoField *f = instanceFieldData->getField(this, i);
	    if (f->isOfType(SoMFString::getClassTypeId())) {
		createFromIsA((SoMFString *)f);
	    }
	}
    }

    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Given an MFString "isA", create an appropriate alternateRep and
//    adds it as a child.
//
// Use: private

void
SoUnknownNode::createFromIsA(SoMFString *isA)
//
////////////////////////////////////////////////////////////////////////
{
    for (int i = 0; i < isA->getNum(); i++) {
	SoType t = SoType::fromName((*isA)[i]);

	if (t.canCreateInstance() &&
	    t.isDerivedFrom(SoNode::getClassTypeId())) {

	    SoNode *alternateRep = (SoNode *)t.createInstance();
	    alternateRep->ref();
#ifdef DEBUG
	    if (alternateRep == NULL) {
		SoDebugError::post("SoUnknownNode::createFromIsA",
				   "SoType.createInstance returned "
				   "NULL (type %s)",
				   t.getName().getString());
		return;
	    }
#endif
	    // Copy over all fields that are shared:
	    int num = instanceFieldData->getNumFields();
	    for (int j=0; j<num; j++) {
		const SbName &fieldName = instanceFieldData->getFieldName(j);
		SoField *f = instanceFieldData->getField(this, j);
		// Don't copy over fields with default values:
		if (f->isDefault()) continue;
		
		SoField *nf = alternateRep->getField(fieldName);
		if (nf != NULL && nf->getTypeId() == f->getTypeId()) {
		    nf->copyFrom(*f);
		    if (f->isConnectedFromField()) {
			SoField *cf;
			f->getConnectedField(cf);
			nf->connectFrom(cf);
		    } else if (f->isConnectedFromEngine()) {
			SoEngineOutput *eo;
			f->getConnectedEngine(eo);
			nf->connectFrom(eo);
		    }
		}
	    }
	    // And if alternateRep is a group, copy over hidden
	    // children:
	    if (alternateRep->isOfType(SoGroup::getClassTypeId())) {
		SoGroup *g = (SoGroup *)alternateRep;
		for (int kid = 0; kid < hiddenChildren.getLength();
		     kid++) {
		    g->addChild(hiddenChildren[kid]);
		}
	    }
	    addChild(alternateRep);
	    return;
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements write action.  We use SoGroup's write method, after
//    fooling around with some stuff so the correct class name and the
//    correct children get written.
//
// Use: private

void
SoUnknownNode::write(SoWriteAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    int i;

    SbBool saveNotify = enableNotify(FALSE);

    // Remember alternateRep, if set:
    SoNode *alternateRep = NULL;

    if (hasChildren) {
	if (getNumChildren() != 0) {
	    alternateRep = getChild(0);
	    alternateRep->ref();
	}

	// Add hiddenChildren to regular child list temporarily:
	removeAllChildren();
	for (i = 0; i < hiddenChildren.getLength(); i++) {
	    addChild(hiddenChildren[i]);
	}
	// Now write:
	SoGroup::write(action);

	removeAllChildren();
    }
    else {
	SoNode::write(action);
    }

    if (alternateRep != NULL) {
	addChild(alternateRep);
	alternateRep->unref();
    }

    enableNotify(saveNotify);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Override searchAction so children (alternateRep) aren't
//    searched.
//
// Use: public

void
SoUnknownNode::search(SoSearchAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoNode::search(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Recursively adds this node and all nodes under it to the copy
//    dictionary. Returns the copy of this node.
//
// Use: protected, virtual

SoNode *
SoUnknownNode::addToCopyDict() const
//
////////////////////////////////////////////////////////////////////////
{
    // If this node is already in the dictionary, nothing else to do
    SoNode *copy = (SoNode *) checkCopy(this);
    if (copy == NULL) {

	// Create and add a new instance to the dictionary
	copy = new SoUnknownNode;
	copy->ref();
	addCopy(this, copy);		// Adds a ref()
	copy->unrefNoDelete();

	// Recurse on children, if any
	for (int i = 0; i < hiddenChildren.getLength(); i++)
	    hiddenChildren[i]->addToCopyDict();
    }

    return copy;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Copies the contents of the given node into this instance.
//
// Use: protected, virtual

void
SoUnknownNode::copyContents(const SoFieldContainer *fromFC,
			    SbBool copyConnections)
//
////////////////////////////////////////////////////////////////////////
{
    // Make sure the copy has the correct class name
    const SoUnknownNode *fromUnk = (const SoUnknownNode *) fromFC;
    setClassName(fromUnk->className);

    // For each field in the original node, create a new field and add
    // it to the new node

    // NOTE: We can't use SoNode::copyContents() to copy the field
    // data, since that uses SoFieldData::overlay(), which assumes the
    // fields have the same offsets in both nodes. Instead, we just
    // copy the field values ourselves.

    const SoFieldData *fromData = fromUnk->getFieldData();
    SoFieldData  *toData	= (SoFieldData *) getFieldData();

    int i;
    for (i = 0; i < fromData->getNumFields(); i++) {

	SoField      *fromField	= fromData->getField(fromUnk, i);
        const SbName fieldName	= fromData->getFieldName(i);
        SoType       fieldType	= fromField->getTypeId();
        SoField      *toField	= (SoField *) (fieldType.createInstance());

        toField->enableNotify(FALSE);
        toField->setContainer(this);
        toField->setDefault(TRUE);
        toField->enableNotify(TRUE);

        toData->addField(this, fieldName.getString(), toField);

	toField->setContainer(this);
	toField->copyFrom(*fromField);
	toField->setIgnored(fromField->isIgnored());
	toField->setDefault(fromField->isDefault());
	toField->fixCopy(copyConnections);
	if (fromField->isConnected() && copyConnections)
	    toField->copyConnection(fromField);
    }

    // Copy the kids
    for (i = 0; i < fromUnk->hiddenChildren.getLength(); i++) {

	// If this node is being copied, it must be "inside" (see
	// SoNode::copy() for details.) Therefore, all of its children
	// must be inside, as well, since our addToCopyDict() takes
	// care of that.
	SoNode *fromKid = fromUnk->getChild(i);
	SoNode *kidCopy = (SoNode *) findCopy(fromKid, copyConnections);

#ifdef DEBUG
	if (kidCopy == NULL)
	    SoDebugError::post("(internal) SoUnknownNode::copyContents",
			       "Child %d has not been copied yet", i);
#endif /* DEBUG */

	hiddenChildren.append(kidCopy);
    }

    // No need to copy the override flag, since this flag will have no
    // effect on an unknown node, and it is not read from or written
    // to files.
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns file format name.
//
// Use: protected

const char *
SoUnknownNode::getFileFormatName() const
//
////////////////////////////////////////////////////////////////////////
{
    return className;
}

