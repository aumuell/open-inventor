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
 * Copyright (C) 1990,91,92   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Classes:
 |	SoBase
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <dlfcn.h>
#include <ctype.h>

#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoOutput.h>
#include <Inventor/SoPath.h>
#include <Inventor/sensors/SoDataSensor.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/errors/SoReadError.h>
#include <Inventor/fields/SoField.h>
#include <Inventor/misc/SoBase.h>
#include <Inventor/misc/SoNotification.h>
#include <Inventor/misc/upgraders/SoUpgrader.h>
#include <SoUnknownNode.h>
#include <SoUnknownEngine.h>
#include "fields/SoGlobalField.h"

// The global name dictionaries
SbDict		*SoBase::nameObjDict;
SbDict		*SoBase::objNameDict;

// Syntax for writing instances to files
#define OPEN_BRACE		'{'
#define CLOSE_BRACE		'}'
#define DEFINITION_KEYWORD	"DEF"
#define REFERENCE_KEYWORD	"USE"
#define NULL_KEYWORD		"NULL"

SbString	SoBase::instancePrefix = "+";

SbBool		SoBase::traceRefs = FALSE;

SoType		SoBase::classTypeId;

uint32_t	SoBase::currentWriteCounter = 0;

// This speed up reading a little:
static SbName *globalFieldName;

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Setup SoBase's type info, and the global name dictionary.
//
// Use: public

void
SoBase::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    classTypeId = SoType::createType(SoType::badType(), "Base");

    // Set up global name dictionaries
    nameObjDict = new SbDict;
    objNameDict = new SbDict;

    globalFieldName = new SbName("GlobalField");
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor for SoBase.
//
// Use: protected

SoBase::SoBase()
//
////////////////////////////////////////////////////////////////////////
{
    refCount = 0;

    writeStuff.hasName = 0;
    writeStuff.writeCounter = 0;
    writeStuff.writeRefFromField = FALSE;

#ifdef DEBUG
    if (traceRefs)
	SoDebugError::postInfo("SoBase::SoBase",
			       "for %#x", (const void *) this);
#endif /* DEBUG */
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor for SoBase.
//
// Use: protected

SoBase::~SoBase()
//
////////////////////////////////////////////////////////////////////////
{
    const SbName &myName = getName();
    if (myName != "")
	removeName(this, myName.getString());

#ifdef DEBUG
    if (traceRefs)
	SoDebugError::postInfo("SoBase::~SoBase",
			       "for %#x", (const void *) this);
#endif /* DEBUG */
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds a reference to an instance. This is defined to be const so
//    users can call it on any instance, even const ones.
//
// Use: public

void
SoBase::ref() const
//
////////////////////////////////////////////////////////////////////////
{
    // This generates a C++ warning.
    ((SoBase *) this)->refCount++;

#ifdef DEBUG
    if (traceRefs)
	SoDebugError::postInfo("SoBase::ref",
			       "refCount for %#x + => %2d",
			       (const void *) this, refCount);
#endif /* DEBUG */
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Removes a reference to an instance, deleting it if count becomes 0.
//    This is defined to be const so users can call it on any
//    instance, even const ones.
//
// Use: public

void
SoBase::unref() const
//
////////////////////////////////////////////////////////////////////////
{
    // This generates a C++ warning.
    SoBase 	*base = (SoBase *) this;

#ifdef DEBUG
    if (base->refCount <= 0)
	SoDebugError::postWarning("SoBase::unref",
				  "instance has reference count <= 0 already");

    if (traceRefs)
	SoDebugError::postInfo("SoBase::unref",
			       "refCount for %#x - => %2d",
			       (const void *) this, refCount - 1);
#endif /* DEBUG */

    if (--base->refCount == 0)
	// This generates a C++ warning
	((SoBase *) this)->destroy();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Removes a reference to an instance, NOT deleting it if count becomes 0.
//    Used with ref to temporarily make something safe for some other operation,
//    then restore the object to exactly the reference-count-state that it was.
//
// Use: public

void
SoBase::unrefNoDelete() const
//
////////////////////////////////////////////////////////////////////////
{
    // This generates a C++ warning.
    SoBase 	*base = (SoBase *) this;

#ifdef DEBUG
    if (base->refCount <= 0)
	SoDebugError::postWarning("SoBase::unrefNoDelete",
				  "instance has reference count <= 0 already");

    if (traceRefs)
	SoDebugError::postInfo("SoBase::unrefNoDelete",
			       "refCount for %#x - => %2d",
			       (const void *) this, refCount - 1);
#endif /* DEBUG */
    base->refCount--;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if base is an instance of a node of the given type
//    or an instance of a subclass of it.
//
// Use: public

SbBool					// Returns TRUE or FALSE
SoBase::isOfType(SoType type) const	// Type inquiring about
//
////////////////////////////////////////////////////////////////////////
{
    return getTypeId().isDerivedFrom(type);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the name of an instance.  If the instance has no name,
//    SbName("") will be returned.
//
// Use: public

SbName
SoBase::getName() const
//
////////////////////////////////////////////////////////////////////////
{
    void *n;

    if (!writeStuff.hasName) return SbName("");
    if (!objNameDict->find((unsigned long)this, n)) {
#ifdef DEBUG
	SoDebugError::post("SoBase::getName",
			   "hasName is TRUE, but couldn't find name!\n");
#endif
	return SbName("");
    }
    return SbName((char *)n);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets an instance's name.  Setting the name to "" un-names the
//    object.  The global dictionary stored in SoDB is updated based
//    on whatever is passed.
//
// Use: public

void
SoBase::setName(const SbName &newName)
//
////////////////////////////////////////////////////////////////////////
{
    //Following 4 lines just do what getName() would do, repeating that code
    //here fixes a thread-safe bug
    void *n;
    SbName oldName("");
    if ((writeStuff.hasName) && (objNameDict->find((unsigned long)this, n)))
	oldName = SbName((char *)n); 
    
    if (oldName.getLength() != 0)
	removeName(this, oldName.getString());

    // Empty name: leave unnamed.
    if (newName.getLength() == 0) return;

    // Make sure name is legal
    const char *str = newName.getString();
    SbBool isBad = 0;

    // Check for beginning-with-a-number:
    if (!SbName::isBaseNameStartChar(str[0])) isBad = TRUE;

    int i;
    for (i = 1; i < newName.getLength() && !isBad; i++) {
	isBad = !SbName::isBaseNameChar(str[i]);
    }

    if (!isBad) {
	addName(this, str);
    }
    else {
	// Replace bad characters with underscores
	SbString goodString;

	// Prepend underscore if name begins with number:
	if (!SbName::isBaseNameStartChar(str[0])) {
	    goodString += "_";
	}
	for (i = 0; i < newName.getLength(); i++) {
	    // Ugly little hack so we can use SbString's += operator,
	    // which doesn't do char's (only char *'s):
	    char temp[2];
	    temp[0] = str[i]; temp[1] = '\0';
	    if (!SbName::isBaseNameChar(str[i]))
		goodString += "_";
	    else
		goodString += temp;
	}
#ifdef DEBUG
	SoDebugError::post("SoBase::setName", "Bad characters in"
			   " name '%s'.  Replacing with name '%s'",
			   str, goodString.getString());
#endif       
	// MUST create an SbName here to create persistent storage for
	// the name.
	SbName goodName(goodString.getString());
	addName(this, goodName.getString());
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Actually deletes an instance. Allows subclasses to do other
//    stuff before the deletion if necessary.
//
// Use: protected

void
SoBase::destroy()
//
////////////////////////////////////////////////////////////////////////
{
    // If there are any auditors left, give them a chance to clean up.
    // The only type of auditors that can be attached to a base with a
    // zero ref count (which is why we are here) are sensors, which
    // are given a chance to detach themselves.
    //
    // NOTE: The implementation of dyingReference() for sensors may
    // require this base instance to be valid. Therefore, we cannot do
    // this stuff in the destructor, or it would be too late.

    for (int i = auditors.getLength() - 1; i >= 0; i--) {

	switch (auditors.getType(i)) {
	  case SoNotRec::SENSOR:
	    // Tell sensor that we are going away
	    ((SoDataSensor *) auditors.getObject(i))->dyingReference();

	    // The call to dyingReference() might remove auditors,
	    // shortening the auditors list; make sure we're not
	    // trying to access past the end.
	    if (i > auditors.getLength())
		i = auditors.getLength();
	    break;

	  default:
	    SoDebugError::post("(internal) SoBase::destroy",
			       "Got an auditor of type %d",
			       (int) auditors.getType(i));
	    break;
	}
    }

    delete this;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes a header (name, open brace) to file defined by SoOutput.
//    If the instance is multiply-referenced for writing, this will
//    either write a definition or reference to the instance. If
//    it writes out a reference, this returns TRUE, meaning that no
//    further writing is necessary for the instance.
//
// Use: protected

SbBool
SoBase::writeHeader(SoOutput *out, SbBool isGroup, SbBool isEngine) const
//
////////////////////////////////////////////////////////////////////////
{
    SbBool	isBinary = out->isBinary();

    // Cast const away
    if (! ((SoBase *) this)->shouldWrite())
	return TRUE;

    if (! isBinary)
	out->indent();

    // If this instance is named, but not multiply referenced, write
    // out the object's name
    if (getName().getLength() != 0 && ! hasMultipleWriteRefs()) {
	// We don't need to bother adding this name to the output
	// dictionary, since this isn't instanced.
	writeDef(out, -1);
    }
    // If multiply-referenced, check the dictionary for a previous definition
    else if (hasMultipleWriteRefs()) {
	int referenceId = out->findReference(this);

	// If already defined, write reference
	if (referenceId != (-1)) {
	    writeRef(out, referenceId);
	    return TRUE;
	}

	// Otherwise, add to dictionary and write definition
	else
	    writeDef(out, out->addReference(this));
    }

    // Write class name
    out->write(getFileFormatName());

    if (! isBinary) {
	out->write(' ');
	out->write(OPEN_BRACE);

	writeAnnotation(out);

	// Prepare for writing insides
	out->incrementIndent();
    } else {
	unsigned short ioFlags = 0x0;
	if (isEngine)
	    ioFlags |= IS_ENGINE;
	if (isGroup)
	    ioFlags |= IS_GROUP;
	out->write(ioFlags);
    }

    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes a footer (close brace) to file defined by SoOutput.
//
// Use: protected

void
SoBase::writeFooter(SoOutput *out) const
//
////////////////////////////////////////////////////////////////////////
{
    if (! out->isBinary()) {

	// Done writing insides
	out->decrementIndent();

	// Write footer
	out->indent();
	out->write(CLOSE_BRACE);
	out->write('\n');
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns file format name.  Overriden by unknown node/engine
//
// Use: protected

const char *
SoBase::getFileFormatName() const
//
////////////////////////////////////////////////////////////////////////
{
    return getTypeId().getName().getString();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initiates notification from an instance. The default method sets
//    up a new notification list and propagates it to all auditors.
//
// Use: internal

void
SoBase::startNotify()
//
////////////////////////////////////////////////////////////////////////
{
    SoNotRec	rec(this);
    SoNotList	list;

    // Indicate to the database that a notification is in progress
    SoDB::startNotify();

    // Assume the notification type is CONTAINER. This is a safe bet
    // for now. If it is another type, it will be changed
    // appropriately later on. We need to set it to something so that
    // SoFieldContainer will have some deterministic way to act.
    rec.setType(SoNotRec::CONTAINER);

    list.append(&rec);
    notify(&list);

    // Indicate to the database that the notification has completed
    SoDB::endNotify();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Propagates notification of modification through an instance of
//    some subclass of SoBase. The default method here does not create
//    and add a new record. It merely propagates the current record
//    list to all auditors. This method may be used by subclasses to
//    do the propagation after modifying the list appropriately.
//
// Use: internal

void
SoBase::notify(SoNotList *list)
//
////////////////////////////////////////////////////////////////////////
{
    auditors.notify(list);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds an auditor to list of auditors.
//
// Use: private

void
SoBase::addAuditor(void *auditor, SoNotRec::Type type)
//
////////////////////////////////////////////////////////////////////////
{
    auditors.append(auditor, type);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Removes an auditor from list of auditors.
//
// Use: private

void
SoBase::removeAuditor(void *auditor, SoNotRec::Type type)
//
////////////////////////////////////////////////////////////////////////
{
    int	audIndex = auditors.find(auditor, type);

#ifdef DEBUG
    if (audIndex < 0) {
	SoDebugError::post("SoBase::removeAuditor",
			   "can't find auditor %#x\n", auditor);
	return;
    }
#endif /* DEBUG */

    auditors.remove(audIndex);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Add a name/instance pair to the global dictionaries.  The
//    nameObjDict is keyed by the SbName's string's address.  The
//    value in each dictionary slot is a pointer to a SbPList
//    containing all SoBase's with the given name (the lists are
//    created dynamically when needed).  Even though the list stores
//    only SoBases, an SoBaseList isn't used because we don't want the
//    items on the list to be reference counted (if they are, they
//    will never be deleted if they are named).
//    The objNameDict is keyed by the SoBase's name.
//
// Use: internal

void
SoBase::addName(SoBase *b, const char *name)
//
////////////////////////////////////////////////////////////////////////
{
    SbPList *list;
    void *t;

    b->writeStuff.hasName = 1;

    // Look for name:
    if (!nameObjDict->find((unsigned long)name, t)) {
	// If not found, create a BaseList and enter it in the
	// dictionary
	list = new SbPList;
	nameObjDict->enter((unsigned long)name, list);
    } else {
	list = (SbPList *)t;
    }

#ifdef DEBUG
    // Make sure it isn't already on the list
    if (list->find(b) != -1)
	SoDebugError::post("SoBase::addName",
			   "Base %x with name \"%s\" is already in dictionary",
			   b, name);
#endif

    // Add name to the list:
    list->append(b);

    // And append to the objName dictionary:
    objNameDict->enter((unsigned long)b, (void *)name);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Removes a name/instance pair from the global dictionary.
//
// Use: internal

void
SoBase::removeName(SoBase *b, const char *name)
//
////////////////////////////////////////////////////////////////////////
{
    SbPList	*list;
    SbBool	found;
    void	*t;
    int		i;

    b->writeStuff.hasName = 0;

    // Look for name list
    found = nameObjDict->find((unsigned long) name, t);

    // Look for name within list
    if (found) {
	list = (SbPList *) t;
	i    = list->find(b);

	if (i < 0)
	    found = FALSE;

	else
	    list->remove(i);
    }

    // And remove from objName dict:
    found |= objNameDict->remove((unsigned long)b);

#ifdef DEBUG
    if (! found)
	SoDebugError::post("SoBase::removeName",
			   "Name \"%s\" (base %x) is not in dictionary",
			   name, b);
#endif

    return;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Internal routines used by node/path/function getByName routines.
//
// Use: internal, static

SoBase *
SoBase::getNamedBase(const SbName &name, SoType type)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (nameObjDict == NULL) {
	SoDebugError::post("SoBase::getName",
			   "SoDBinit() has not yet been called");
	return NULL;
    }
#endif

    SbPList *list;
    void *t;
    // Lookup the name in the dictionary
    if (! nameObjDict->find((unsigned long) name.getString(), t))
	return NULL;
    list = (SbPList *)t;

    // Search backwards through the list.  Return the last item of the
    // appropriate type.
    for (int i = list->getLength()-1; i >= 0; i--) {
	SoBase *b = (SoBase *)(*list)[i];
	if (b->isOfType(type)) return b;
    }
    return NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Internal routine used by the get() routines above.  Returns the
//    number of items added to the result list.
//
// Use: internal, static

int
SoBase::getNamedBases(const SbName &name, SoBaseList &result, SoType type)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (nameObjDict == NULL) {
	SoDebugError::post("SoBase::getName",
			   "SoDBinit() has not yet been called");
	return 0;
    }
#endif

    int numAdded = 0;
    SbPList *list;

    void *t;
    // Lookup the name in the dictionary
    if (! nameObjDict->find((unsigned long) name.getString(), t))
	return 0;

    list = (SbPList *)t;
    // Search backwards through the list.  Add all items of the
    // appropriate type to the result list.
    for (int i = list->getLength()-1; i >= 0; i--) {
	SoBase *b = (SoBase *)(*list)[i];
	if (b->isOfType(type)) {
	    result.append(b);
	    numAdded++;
	}
    }
    return numAdded;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads one instance of some subclass of SoBase. Returns pointer
//    to read-in instance in base, or NULL on EOF. Returns FALSE on
//    error. The last parameter is a subclass type to match. If the
//    returned base is not of this type, it is an error. A type of
//    ANY_TYPE (the default) will match any base.
//
// Use: internal

SbBool
SoBase::read(SoInput *in, SoBase *&base, SoType expectedType)
//
////////////////////////////////////////////////////////////////////////
{
    SbName	name;
    SbBool	ret;

    // Read header: name and opening brace. If not found, not an error -
    // just nothing to return.
    if (! in->read(name, TRUE)) {
	base = NULL;
	ret = in->curFile->headerOk;
    }

    // If name is empty, treat this as EOF. This happens, for example,
    // when the last child of a group has been read and the '}' is next.
    else if (! name) {
	base = NULL;
	ret = TRUE;
    }

    // Check for special case of name "NULL", indicating a NULL node
    // or path pointer. (This is used for node and path fields.)
    else if (name == NULL_KEYWORD) {
	base = NULL;
	ret = TRUE;
    }

    // Check for reference to existing node/path/function
    else if (name == REFERENCE_KEYWORD)
	ret = readReference(in, base);
    else
	ret = readBase(in, name, base);

    // Check for type match
    if (base != NULL) {
	if (! base->isOfType(expectedType)) {
	    const char *baseName = base->getTypeId().getName().getString();
	    SoReadError::post(in, "Expected a %s but got a %s",
			      expectedType.getName().getString(), baseName);
	    ret = FALSE;
	}
    }

    return ret;
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
SoBase::addWriteReference(SoOutput *, SbBool isFromField)
//
////////////////////////////////////////////////////////////////////////
{
    // If adding a reference from a field-to-field connection, just
    // set that flag to indicate it
    if (isFromField)
	writeStuff.writeRefFromField = TRUE;

    // Otherwise, update the counter if this is the first reference
    else if (writeStuff.writeCounter != getCurrentWriteCounter()) {
	writeStuff.writeCounter = getCurrentWriteCounter();
	writeStuff.multWriteRef = FALSE;
    }

    // Otherwise, indicate that we have multiple write references
    else
	writeStuff.multWriteRef = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if the instance should be written, based on the
//    write-reference info already accumulated.
//
// Use: internal

SbBool
SoBase::shouldWrite()
//
////////////////////////////////////////////////////////////////////////
{
    // If there is at least one current write reference
    if (writeStuff.writeCounter == getCurrentWriteCounter()) {

	// If there's also a reference from a field, make sure we know
	// that we have multiple references and reset the field flag
	if (writeStuff.writeRefFromField) {
	    writeStuff.multWriteRef = TRUE;
	    writeStuff.writeRefFromField = FALSE;
	}

	return TRUE;
    }

    // Ignore cases referenced only from field connections, and reset
    // the flag for next time
    if (writeStuff.writeRefFromField)
	writeStuff.writeRefFromField = FALSE;

    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes DEF identifier.  The identifier has two parts-- the
//    objects name, and the objects reference ID.  If the object is
//    not named, just '+referenceId' will be written.  If the object
//    is not instanced, then just the name will be written.  This
//    routine must not be called for objects that are neither named
//    nor instanced.
//
// Use: private

void
SoBase::writeDef(SoOutput *out, int referenceId) const
//
////////////////////////////////////////////////////////////////////////
{
    out->write(DEFINITION_KEYWORD);
    if (! out->isBinary())
	out->write(' ');

    // Assemble the whole identifier at once, so binary writing works
    // properly (it writes the identifier as count followed by the
    // characters).
    SbString t;
    const SbName &myName = getName();
    if (myName.getLength() != 0)
	t += myName.getString();
    if (referenceId != -1) {
	t += instancePrefix.getString();
	t += SbString(referenceId);
    }
    out->write(t.getString());
    if (! out->isBinary())
	out->write(' ');
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes USE identifier.  See the description of referenceId in
//    the writeDef function above, and note that since this is writing
//    a reference, it must always have a referenceId.
//
// Use: private

void
SoBase::writeRef(SoOutput *out, int referenceId) const
//
////////////////////////////////////////////////////////////////////////
{
    out->write(REFERENCE_KEYWORD);
    if (! out->isBinary())
	out->write(' ');

    SbString t;
    const SbName &myName = getName();
    if (myName.getLength() != 0) {
	t += myName.getString();
    }
    t += instancePrefix.getString();
    t += SbString(referenceId);
    out->write(t.getString());

    writeAnnotation(out);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    If writing in ASCII, this will write out the address and
//    reference count of the Base for debugging purposes (if the
//    correct bits are set on the SoOutput, of course).
//
// Use: private

void
SoBase::writeAnnotation(SoOutput *out) const
//
////////////////////////////////////////////////////////////////////////
{
    if (out->isBinary()) return;
    if (out->getAnnotation()
#ifdef DEBUG
	|| traceRefs
#endif /* DEBUG */
	) {
	out->write(" #");
	if (out->getAnnotation() & SoOutput::ADDRESSES) {
	    char buf[100];
	    sprintf(buf, " %#x", this);
	    out->write(buf);
	}
	if (out->getAnnotation() & SoOutput::REF_COUNTS
#ifdef DEBUG
	    || traceRefs
#endif /* DEBUG */
	    ) {
	    out->write(" RefCount=");
	    out->write(refCount);
	}
    }
    out->write('\n');
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads name of referenced base instance (assumes reference
//    keyword was read) and sets base to point to it. Returns FALSE on
//    error.
//
// Use: private

SbBool
SoBase::readReference(SoInput *in, SoBase *&base)
//
////////////////////////////////////////////////////////////////////////
{
    SbName	refName;
    SbBool	ret = TRUE;

    // Get name of referenced thing
    if (! in->read(refName, FALSE)) {
	SoReadError::post(in, "Premature end of file after "
			  REFERENCE_KEYWORD);
	ret = FALSE;
    }

    // Look name up in the dictionary
    else {
	// Ok, in ASCII we might have read too much-- check for a '.'
	// in the name, meaning we read the field identifier, too, and
	// will have to re-figure the name and put back the extra
	// characters:
	if ( !in->isBinary()) {
	    const char *chars = refName.getString();
	    for (int i = 0; i < refName.getLength(); i++) {
		if (chars[i] == '.') {
		    in->putBack(chars+i);
		    refName = SbString(chars, 0, i-1);
		}
	    }
	}

	if ((base = in->findReference(refName)) == NULL) {
	    SoReadError::post(in, "Unknown reference \"%s\"",
			      refName.getString());
	    ret = FALSE;
	}
    }

    return ret;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads base. Sets base to point to read-in instance. Returns
//    FALSE on error.
//
// Use: private

SbBool
SoBase::readBase(SoInput *in, SbName &className, SoBase *&base)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool		gotChar;
    SbName		refName;
    char		c;
    SbBool		ret = TRUE, flush = FALSE;

    // Assume NULL for now
    base = NULL;

    // Check for definition of new node/path
    if (className == DEFINITION_KEYWORD) {

	if (! in->read(refName, FALSE) || ! in->read(className, TRUE)) {
	    SoReadError::post(in, "Premature end of file after "
			      DEFINITION_KEYWORD);
	    ret = FALSE;
	}

	if (! refName) {
	    SoReadError::post(in, "No name given after ", DEFINITION_KEYWORD);
	    ret = FALSE;
	}

	if (! className) {
	    SoReadError::post(in, "Invalid definition of %s",
			      refName.getString());
	    ret = FALSE;
	}
    }

    if (ret) {

	// Save whether the file is binary in
	// case we open another file before we get to the close brace.
	SbBool isBinary = in->isBinary();

	// Look for open brace.
	if (!isBinary &&
	    (! (gotChar = in->read(c)) || c != OPEN_BRACE)) {
	    if (gotChar)
		SoReadError::post(in, "Expected '%c'; got '%c'",
				  OPEN_BRACE, c);
	    else
		SoReadError::post(in, "Expected '%c'; got EOF", OPEN_BRACE);
	    ret = FALSE;
	}

	else {
	    ret = readBaseInstance(in, className, refName, base);

	    if (! ret)
		flush = TRUE;

	    // Read closing brace.
	    else if (! isBinary &&
		     (! (gotChar = in->read(c)) || c != CLOSE_BRACE)) {
		if (gotChar)
		    SoReadError::post(in, "Expected '%c'; got '%c'",
				      CLOSE_BRACE, c);
		else
		    SoReadError::post(in, "Expected '%c'; got EOF",
				      CLOSE_BRACE);
		ret = FALSE;
	    }
	}
    }

    if (! ret && flush && ! in->isBinary())
	flushInput(in);

    return ret;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads instance of class with given name. Sets base to point to
//    read-in instance. Returns FALSE on error.
//
// Use: private

SbBool
SoBase::readBaseInstance(SoInput *in, const SbName &className,
			 const SbName &refName, SoBase *&base)
//
////////////////////////////////////////////////////////////////////////
{
    // This will be re-written with the correct values if binary file
    // format.  If ASCII, it shouldn't matter:
    unsigned short ioFlags = IS_GROUP | IS_ENGINE;

    SbBool isBinary = in->isBinary();
    SbBool oldFileFormat = (in->getIVVersion() < 2.1f);

    if (isBinary && !oldFileFormat) {
	in->read(ioFlags);
    }

    // Special case for global fields. Even though the word
    // "GlobalField" appears in the input file, the field itself
    // depends on the name about to be read. If the same name is used
    // twice, the same instance has to be used, so we don't want to
    // create a new instance each time.
    if (className == *globalFieldName) {
	base = SoGlobalField::read(in);

	if (base == NULL) return FALSE;
	
	// Store instance in input dictionary if a name was given for
	// it (do NOT want to add it to the global dictionary; it's
	// DEF name is not used for that).
	if (! (!refName))
	    in->addReference(refName, base, FALSE);

	return TRUE;
    }
    
    // And special case for nodes that need conversion from an old
    // version of the file format:
    SoUpgrader *upgrader;
    if ((upgrader = SoUpgrader::getUpgrader(className, in->getIVVersion()))
	!= NULL) {
	    
	upgrader->ref();
	SbBool result = upgrader->upgrade(in, refName, base);
	upgrader->unref();

	return result;
    }

    // The common case:

    // Create an instance of named type
    base = createInstance(in, className, ioFlags);
    if (base == NULL)
	return FALSE;

    // Store instance in dictionary if a name was given for it
    // This may also name the base, depending on the form of refName.
    if (! (!refName))
	in->addReference(refName, base);

    // Read stuff into instance.  Note that if the node has sensors on
    // its fields,  they might get triggered.  Make sure that the node
    // is referenced during the read.
    base->ref();
    SbBool result = base->readInstance(in, ioFlags);
    base->unrefNoDelete();

    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Create instance for given className, returning a
//    pointer to it. Returns NULL on error.
//
// Use: private

SoBase *
SoBase::createInstance(SoInput *in, SbName className, unsigned short ioFlags)
//
////////////////////////////////////////////////////////////////////////
{
    SoBase		*instance = NULL;

    SbBool isBinary = in->isBinary();
    SbBool oldFileFormat = (in->getIVVersion() < 2.1f);

    // Find named type in class dictionary.
    SoType type = SoType::fromName(className);

    // Need to create an unknown node or engine:
    if (type.isBad()) {

        SbBool createEngine = FALSE;

	// ASCII, old or new file format (they're the same):
	if (!isBinary) {
	    SbString unknownString;
            SbBool  readOK = in->read(unknownString);
	    if (!readOK || ((unknownString != "fields" &&
			     unknownString != "inputs"))) {
		SoReadError::post(in, "Unknown class \"%s\"",
				  className.getString());
		return NULL;
	    }
	    in->putBack(unknownString.getString());
	    if (unknownString == "inputs") {
		createEngine = TRUE;
	    }
	}

	// Binary, new file format
	else if (!oldFileFormat && isBinary) {
	    createEngine = ioFlags & IS_ENGINE;
	}

	// Binary, old file format:
	else {
	    SbString unknownString;
            SbBool  readOK = in->read(unknownString);
	    if (!readOK || ((unknownString != "fields" &&
			     unknownString != "inputs"))) {
		SoReadError::post(in, "Unknown class \"%s\"",
				  className.getString());
		return NULL;
	    }
	    // Cannot put back the string (which is OK)
	    if (unknownString == "inputs") {
		createEngine = TRUE;
	    }
	}	    

        if (!createEngine) {
#ifdef DEBUG
	    SoDebugError::postWarning("SoBase::createInstance",
		"Creating unknown node for object of type %s "
		"(could not open DSO)", className.getString());
#endif
            SoUnknownNode *tmpNode = new SoUnknownNode();
            tmpNode->setClassName(className.getString());
	    instance = tmpNode;
	} else {
#ifdef DEBUG
	    SoDebugError::postWarning("SoBase::createInstance",
		"Creating unknown engine for object of type %s "
		"(could not open DSO)", className.getString());
#endif
            SoUnknownEngine *tmpEngine = new SoUnknownEngine;
            tmpEngine->setClassName(className.getString());
	    instance = tmpEngine;
	}
    }

    else if (!type.isDerivedFrom(SoBase::getClassTypeId())) {
	SoReadError::post(in, "\"%s\" is not an SoBase",
			  className.getString());
	instance = NULL;
    }

    else {
	instance = (SoBase *)type.createInstance();

	// We may have the name of an abstract derived class.
	if (instance == NULL) {
	    SoReadError::post(in, "class \"%s\" is an abstract class",
			      className.getString());
	}
	// Binary, old file format, not built in: read "fields" field
	else if (oldFileFormat && isBinary) {
	    if (instance->isOfType(SoFieldContainer::getClassTypeId())
	        && !((SoFieldContainer *)instance)->getIsBuiltIn()) {
		SbString unknownString;
		SbBool  readOK = in->read(unknownString);
		if (!readOK || ((unknownString != "fields" &&
				 unknownString != "inputs"))) {
		    SoReadError::post(in, "Unknown class \"%s\"",
				      className.getString());
		    return NULL;
		}
	    }
	}
    }

    return instance;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Flushes input up to EOF or the next closing brace (watching for
//    nested closing braces). This should be called when an error is
//    found in input after the open brace for some SoBase subclass
//    instance.
//
// Use: private

void
SoBase::flushInput(SoInput *in)
//
////////////////////////////////////////////////////////////////////////
{
    int		nestLevel = 1;
    char	c;

    while (nestLevel > 0 && in->get(c)) {

	if (c == CLOSE_BRACE)
	    nestLevel--;

	else if (c == OPEN_BRACE)
	    nestLevel++;
    }
}

#undef OPEN_BRACE
#undef CLOSE_BRACE
#undef DEFINITION_KEYWORD
#undef REFERENCE_KEYWORD
#undef NULL_KEYWORD
