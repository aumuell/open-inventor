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

// This may look like C code, but it is really -*- C++ -*-

/*
 * Copyright (C) 1990,91,92   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.3 $
 |
 |   Classes:
 |	SoFieldContainer
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SbDict.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoLists.h>
#include <Inventor/SoOutput.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/errors/SoReadError.h>
#include <Inventor/fields/SoField.h>
#include <Inventor/fields/SoFieldContainer.h>
#include <Inventor/fields/SoFieldData.h>
#include <Inventor/misc/SoNotification.h>

#include <stdlib.h>

// Syntax for reading/writing type information to files
#define OPEN_BRACE_CHAR		'['
#define CLOSE_BRACE_CHAR	']'
#define VALUE_SEPARATOR_CHAR	','

// This is a list of SbDict instances used duting copy operations to
// keep track of instances. It is a list to allow recursive copying.
SbPList	*SoFieldContainer::copyDictList = NULL;

// These are used by SoFieldContainer::get() to hold the returned
// field string
// We rely on the compiler to initialize them to zero for us...
char	*SoFieldContainer::fieldBuf;
int	 SoFieldContainer::fieldBufSize;

SoType	SoFieldContainer::classTypeId;

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Set up type info.
//
// Use: internal

void
SoFieldContainer::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    classTypeId = SoType::createType(SoBase::getClassTypeId(),
				     "FieldContainer");
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: protected

SoFieldContainer::SoFieldContainer()
//
////////////////////////////////////////////////////////////////////////
{
    notifyEnabled = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: protected

SoFieldContainer::~SoFieldContainer()
//
////////////////////////////////////////////////////////////////////////
{
    // Fields must be disconnected when things are deleted.  Backwards
    // connections (feeding into my fields) are handled by the SoField
    // destructor.  Forward connections are handled in SoBase's
    // destructor (it loops through the auditor list and calls
    // disconnect for any field to field connections it finds).
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets all fields to their default values and turns on their
//    default flags.
//
// Use: public

void
SoFieldContainer::setToDefaults()
//
////////////////////////////////////////////////////////////////////////
{
    // Create an instance of the same type - it will have default values
    SoFieldContainer *def = (SoFieldContainer *) getTypeId().createInstance();
    def->ref();

    const SoFieldData *fieldData = getFieldData();

    if ((fieldData != NULL) && (def != NULL))
	fieldData->overlay(this, def, FALSE);

    def->unref();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if all fields have their default values (even if
//    the isDefault() flags are not all set).
//
// Use: public

SbBool
SoFieldContainer::hasDefaultValues() const
//
////////////////////////////////////////////////////////////////////////
{
    // Create an instance of the same type - it will have default values
    SoFieldContainer *def = (SoFieldContainer *) getTypeId().createInstance();
    def->ref();

    // Compare with this instance
    SbBool	sameValues = fieldsAreEqual(def);

    // Get rid of default instance
    def->unref();

    return sameValues;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if the field values match those of the passed
//    instance, which is assumed to be of the same type as this.
//
// Use: public

SbBool
SoFieldContainer::fieldsAreEqual(const SoFieldContainer *fc) const
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (getTypeId() != fc->getTypeId()) {
	SoDebugError::post("SoFieldContainer::fieldsAreEqual",
			   "Instances are of different types");
	return FALSE;
    }
#endif /* DEBUG */

    // Compare fields in field data
    return getFieldData()->isSame(this, fc);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Copies field values from the passed instance, which is assumed
//    to be of the same type as this. If the copyConnections flag is
//    TRUE (it is FALSE by default), any connections to (but not from)
//    fields of the instance are copied, as well.
//
// Use: public

void
SoFieldContainer::copyFieldValues(const SoFieldContainer *fc,
				  SbBool copyConnections)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (getTypeId() != fc->getTypeId()) {
	SoDebugError::post("SoFieldContainer::copyFieldValues",
			   "Instances are of different types");
	return;
    }
#endif /* DEBUG */

    // Copy fields in field data instances
    const SoFieldData *fieldData = getFieldData();
    if (fieldData != NULL)
	fieldData->overlay(this, fc, copyConnections);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets fields in object from info in passed string. The format of
//    the string is the same as the ASCII file format, i.e., the stuff
//    that could appear between the '{' and '}' for the object in a file.
//
//    The SoInput parameter is NULL in the public method and may be
//    non-NULL in the internal one. If it is non-NULL, it specifies
//    the SoInput from which to get the reference dictionary to use
//    when reading from the string.
//
// Use: public/internal

SbBool
SoFieldContainer::set(const char *fieldDataString, SoInput *dictIn)
//
////////////////////////////////////////////////////////////////////////
{
    const SoFieldData	*fieldData = getFieldData();

    if (fieldData != NULL) {
	SoInput	in(dictIn);
	in.setBuffer((void *) fieldDataString, strlen(fieldDataString));
	SbBool isBuiltIn; // Not used
	return fieldData->read(&in, this, FALSE, isBuiltIn);
    }
    else
	return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does opposite of "set()" - stores data from fields in passed
//    SbString. The format of the string is the same as the ASCII file
//    format, i.e., the stuff that could appear between the '{' and
//    '}' for the object in a file.
//
//    The SoOutput parameter is NULL in the public method and may be
//    non-NULL in the internal one. If it is non-NULL, it specifies
//    the SoOutput from which to get the reference dictionary to use
//    when writing into the string.
//
// Use: public/internal

void
SoFieldContainer::get(SbString &fieldDataString, SoOutput *dictOut)
//
////////////////////////////////////////////////////////////////////////
{
    const SoFieldData	*fieldData = getFieldData();

#ifdef DEBUG
    if (fieldData == NULL) {
	SoDebugError::post("SoFieldContainer::get",
			   "Field data is NULL!");
	return;
    }
#endif

    SoOutput	out(dictOut);

    // Prepare a character buffer and SoOutput for writing field strings
    if (fieldBufSize == 0) {
	fieldBufSize = 1028;
	fieldBuf = (char *) malloc((unsigned) fieldBufSize);
    }

    // Set up output into a string buffer
    out.setBuffer((void *) fieldBuf, fieldBufSize,
		  &SoFieldContainer::reallocFieldBuf);

    // Make sure that the file header and lots of white space will NOT
    // be written into the string
    out.setCompact(TRUE);

    // Prepare to begin writing by incrementing the 
    // current write counter in SoBase
    SoBase::incrementCurrentWriteCounter();

    // Count references first
    out.setStage(SoOutput::COUNT_REFS);
    getFieldData()->write(&out, this);

    // Next, do actual writing
    out.setStage(SoOutput::WRITE);
    getFieldData()->write(&out, this);

    // Make sure the string is terminated
    out.reset();

    // Store the result in the passed SbString
    fieldDataString = fieldBuf;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads stuff into instance of subclass of SoFieldContainer.
//    Returns FALSE on error.
//
// Use: protected

SbBool
SoFieldContainer::readInstance(SoInput *in, 
			       unsigned short /* flags not used */)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool notBuiltIn; // Not used
    return getFieldData()->read(in, this, TRUE, notBuiltIn);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds a reference to the instance when writing. isFromField
//    indicates whether the reference is from a field-to-field
//    connection.
//
// Use: internal, virtual

void
SoFieldContainer::addWriteReference(SoOutput *out, SbBool isFromField)
//
////////////////////////////////////////////////////////////////////////
{
    // Do standard stuff
    SoBase::addWriteReference(out, isFromField);

    // If this is the first reference to this instance and we are not
    // being referenced from a field-to-field connection, give fields
    // a chance to do their thing
    if (! isFromField && ! hasMultipleWriteRefs()) {
	SoFieldData	*fieldData = (SoFieldData *) getFieldData();

	// Since the SoOutput is in the COUNT_REFS stage, this call to
	// write() will just take care of counting references
	if (fieldData != NULL)
	    fieldData->write(out, this);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes instance to SoOutput.
//
// Use: internal

void
SoFieldContainer::writeInstance(SoOutput *out)
//
////////////////////////////////////////////////////////////////////////
{
    if (! writeHeader(out, FALSE, FALSE)) {

	getFieldData()->write(out, this);

	writeFooter(out);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Default method returns a NULL SoFieldData structure for the object.
//
// Use: internal

const SoFieldData *
SoFieldContainer::getFieldData() const
//
////////////////////////////////////////////////////////////////////////
{
    return NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This is a callback function passed to SoOutput::setBuffer() by
//    SoFieldContainer::get(). It is used to reallocate the buffer in which the
//    returned field info string is stored. Since we need to keep
//    track of the pointer to the buffer and its size, we can't just
//    call realloc() instead.
//
// Use: private, static

void *
SoFieldContainer::reallocFieldBuf(void *ptr, size_t newSize)
//
////////////////////////////////////////////////////////////////////////
{
    fieldBuf = (char *) realloc(ptr, newSize);
    fieldBufSize = newSize;

    return fieldBuf;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Return a list of my fields.
//
// Use: public

int
SoFieldContainer::getFields(SoFieldList &list) const
//
////////////////////////////////////////////////////////////////////////
{
    int			i;
    const SoFieldData	*fd = getFieldData();

    if (fd == NULL)
	return 0;

    for (i = 0; i < fd->getNumFields(); i++) {
	list.append(fd->getField(this, i));
    }

    return fd->getNumFields();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns a pointer to the field with the given name. If no such
//    field exists, NULL is returned.
//
// Use: public

SoField *
SoFieldContainer::getField(const SbName &fieldName) const
//
////////////////////////////////////////////////////////////////////////
{
    int			i;
    const SoFieldData	*fd = getFieldData();

    if (fd == NULL)
	return NULL;

    // Search fields for one with given name
    for (i = 0; i < fd->getNumFields(); i++)
	if (fd->getFieldName(i) == fieldName)
	    return fd->getField(this, i);

    // Not found...
    return NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns (in fieldName) the name of the field pointed to. Returns
//    FALSE if the field is not contained within the field container
//    instance.
//
// Use: public

SbBool
SoFieldContainer::getFieldName(const SoField *field, SbName &fieldName) const
//
////////////////////////////////////////////////////////////////////////
{
    int			i;
    const SoFieldData	*fd = getFieldData();

    if (fd == NULL)
	return FALSE;

    // Search fields for one with given pointer
    for (i = 0; i < fd->getNumFields(); i++) {
	if (fd->getField(this, i) == field) {
	    fieldName = fd->getFieldName(i);
	    return TRUE;
	}
    }

    // Not found...
    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Propagates notification.
//
// Use: internal

void
SoFieldContainer::notify(SoNotList *list)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (list == NULL) {
	SoDebugError::post("SoFieldContainer::notify",
			   "Notification list pointer is NULL");
	return;
    }
#endif /* DEBUG */

    // Don't notify if application has disabled notification for some reason
    if (! isNotifyEnabled())
	return;

    // There are only 2 types of notification possible here: CONTAINER
    // (a field notifying its container), or PARENT (a node notifying
    // its parent). The other types are handled by the other classes.

    switch (list->getLastRec()->getType()) {
      case SoNotRec::CONTAINER:
	// We don't need to create a new record, since the last
	// record's base is already set to this. Just notify all
	// auditors.
	SoBase::notify(list);
	break;

      case SoNotRec::PARENT:
	// If we're notifying from a child node to a parent node,
	// create a new record that contains the parent node as a
	// base, then notify.
	{
	    SoNotRec	rec(this);
	    list->append(&rec);
	    SoBase::notify(list);
	}
	break;

      default:
#ifdef DEBUG
	SoDebugError::post("SoFieldContainer::notify (internal)",
			   "Got notification of type %d",
			   list->getLastRec()->getType());
#endif /* DEBUG */
	break;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the copy dictionary. Returns FALSE if the dictionary
//    already exists (probably indicating a recursive copy is taking
//    place).
//
// Use: internal, static

void
SoFieldContainer::initCopyDict()
//
////////////////////////////////////////////////////////////////////////
{
    if (copyDictList == NULL)
	copyDictList = new SbPList;

    SbDict *copyDict = new SbDict;

    // Insert the new dictionary at the beginning. Since most copies
    // are non-recursive, having to make room in the list won't happen
    // too frequently. Accessing the list happens a lot, so using slot
    // 0 will speed that up some.
    copyDictList->insert(copyDict, 0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds an entry to the copy dictionary.
//
// Use: internal, static

void
SoFieldContainer::addCopy(const SoFieldContainer *orig,
			  const SoFieldContainer *copy)
//
////////////////////////////////////////////////////////////////////////
{
    SbDict *copyDict = (SbDict *) (*copyDictList)[0];

    // Add a reference when entering an instance into the
    // dictionary. The references are removed before the dictionary is
    // deleted.
    copy->ref();

    // ??? HACK ALERT: We use the notifyEnabled flag in the copy to
    // indicate whether its contents have been copied yet. If the flag
    // is FALSE, the copy has not yet been done. It's pretty safe to do
    // this because the notifyEnabled flag is not copied between
    // instances. When copyContents() is called for the first time on
    // a copied instance, we reset the flag to TRUE.
    ((SoFieldContainer *) copy)->notifyEnabled = FALSE;

    copyDict->enter((unsigned long) orig, (void *) copy);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    If a copy of the given instance is in the dictionary, this
//    returns it. Otherwise, it returns NULL. The copy is not changed
//    in any way.
//
// Use: internal, static

SoFieldContainer *
SoFieldContainer::checkCopy(const SoFieldContainer *orig)
//
////////////////////////////////////////////////////////////////////////
{
    SbDict *copyDict = (SbDict *) (*copyDictList)[0];

    void *copyPtr;
    if (! copyDict->find((unsigned long) orig, copyPtr))
	return NULL;

    return (SoFieldContainer *) copyPtr;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    If a copy of the given instance is not in the dictionary, this
//    returns NULL. Otherwise, this copies the contents of the
//    original into the copy (if not already done) and returns a
//    pointer to the copy.
//
// Use: internal, static

SoFieldContainer *
SoFieldContainer::findCopy(const SoFieldContainer *orig,
			   SbBool copyConnections)
//
////////////////////////////////////////////////////////////////////////
{
    if (! copyDictList || ! (*copyDictList)[0])
	return NULL;

    SbDict *copyDict = (SbDict *) (*copyDictList)[0];

    void *copyPtr;
    if (! copyDict->find((unsigned long) orig, copyPtr))
	return NULL;

    SoFieldContainer *copyFC = (SoFieldContainer *) copyPtr;

    // ??? Copy the contents only if the notifyEnabled flag is FALSE,
    // indicating that the copy has not yet been done. See the HACK
    // ALERT above.
    if (! copyFC->notifyEnabled) {
	copyFC->notifyEnabled = TRUE;
	copyFC->copyContents(orig, copyConnections);
    }

    return copyFC;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Cleans up the copy dictionary.
//
// Use: internal, static

void
SoFieldContainer::copyDone()
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (copyDictList->getLength() <= 0) {
	SoDebugError::post("SoFieldContainer::copyDone",
			   "No dictionary left to get rid of");
	return;
    }
#endif /* DEBUG */

    SbDict *copyDict = (SbDict *) (*copyDictList)[0];

    // Unref every instance in the copy dictionary
    copyDict->applyToAll(unrefCopy);

    delete copyDict;

    copyDictList->remove(0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This callback is used to unref() all instances in the
//    copyDict when copyDone() is called.
//
// Use: private, static

void
SoFieldContainer::unrefCopy(unsigned long, void *instPtr)
//
////////////////////////////////////////////////////////////////////////
{
    SoFieldContainer *inst = (SoFieldContainer *) instPtr;

    // Set the notifyEnabled bit to TRUE if it wasn't already done
    if (! inst->notifyEnabled)
	inst->notifyEnabled = TRUE;

    inst->unref();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Copies the contents of the given node into this instance. The
//    default implementation copies just field values and the name.
//
// Use: internal, virtual

void
SoFieldContainer::copyContents(const SoFieldContainer *fromFC,
			       SbBool copyConnections)
//
////////////////////////////////////////////////////////////////////////
{
    // Access the field data and overlay it
    const SoFieldData *fieldData = getFieldData();
    if (fieldData != NULL)
	fieldData->overlay(this, fromFC, copyConnections);

    // Copy the name, if it has one
    const SbName &name = fromFC->getName();
    if (! ! name)
	setName(name);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    During a copy operation, this copies an instance that is
//    encountered through a field connection. The default
//    implementation just returns the original pointer - no copy is
//    done. Subclasses such as nodes and engines handle this
//    differently.
//
// Use: internal, virtual

SoFieldContainer *
SoFieldContainer::copyThroughConnection() const
//
////////////////////////////////////////////////////////////////////////
{
    return (SoFieldContainer *) this;
}
