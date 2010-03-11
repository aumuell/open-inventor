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
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |	SoGlobalField
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoGlobalField
//
//	There is an instance of this class for every global field
//      created.  This class also maintains the dictionary associating
//      global field names with global field containers.
//
//      This entire class is private.
//
//////////////////////////////////////////////////////////////////////////////

#include <Inventor/errors/SoDebugError.h>
#include <Inventor/errors/SoReadError.h>
#include "SoGlobalField.h"

// SbName to SoGlobalField* dictionary.  Keyed by the name.
SbDict *SoGlobalField::nameDict;

// The GlobalFieldContainer type (the name is simply GlobalField)
SoType SoGlobalField::classTypeId;

//////////////////////////////////////////////////////////////////////////////
//
// Description:
//    Setup type information.  Called by SoDB::init()
//
// Use: internal

void
SoGlobalField::initClass()
//
//////////////////////////////////////////////////////////////////////////////
{
    classTypeId = SoType::createType(SoFieldContainer::getClassTypeId(),
				     "GlobalField", NULL);
    nameDict = new SbDict(20);  // Assume small number of global fields
}

//////////////////////////////////////////////////////////////////////////////
//
// Description:
//	This either creates a new globalField (if name hasn't been found
//	before) or returns the globalField with the given name and type.
//	It will return NULL if there is an error. "alreadyExists" will be
//	set to TRUE if a global field of the same name and type already
//	exists.
//
// Use: internal

SoGlobalField *
SoGlobalField::create(const SbName &name, SoType type, SbBool &alreadyExists)
//
//////////////////////////////////////////////////////////////////////////////
{
    // Look for an existing field of the same name/type:
    SoGlobalField *result = SoGlobalField::find(name);
    if (result != NULL) {
	if (! result->getType().isDerivedFrom(type)) {
	    const char *resName = result->getName().getString();
	    const char *resTypeName = result->getType().getName().getString();
	    SoDebugError::post("SoGlobalField::create",
			       "Global field %s has two types (%s, %s)", 
			       resName, resTypeName,
			       type.getName().getString());
	    return NULL;
	}
	else {
	    alreadyExists = TRUE;
	    return result;
	}
    }

    alreadyExists = FALSE;

    // Make sure field is a proper field type

    if (! type.isDerivedFrom(SoField::getClassTypeId())) {
	const char *resName = result->getName().getString();
	SoDebugError::post("SoGlobalField::create",
			   "(field %s) Type %s is not a field type",
			   resName, type.getName().getString());
	return NULL;
    }
    SoField *field = (SoField *)type.createInstance();

    // Make sure it isn't an abstract field type...

    if (field == NULL) {
	const char *typeName = type.getName().getString();
	SoDebugError::post("SoGlobalField::create",
			   "Cannot create field %s of type %s",
			   name.getString(), typeName);
	return NULL;
    }

    return new SoGlobalField(name, field);
}

//////////////////////////////////////////////////////////////////////////////
//
// Description:
//	GlobalField constructor.  It creates a globalField, given the name
//	of the field and a pointer to the field to use.  This is private;
//	the create() method is the public interface to creating new global
//	fields.
//
// Use: private

SoGlobalField::SoGlobalField(const SbName &name, SoField *field)
//
//////////////////////////////////////////////////////////////////////////////
{
    typeField.setValue(field->getTypeId().getName());
    value = field;
#ifdef DEBUG
    if (field->getContainer())
	SoDebugError::post("SoGlobalField::SoGlobalField",
			   "Field already has container!");

#endif /* DEBUG */
    field->setContainer(this);
    field->setDefault(FALSE);

    unsigned long key = (unsigned long)(name.getString());
#ifdef DEBUG
    void *junk;
    if (nameDict->find(key, junk) != FALSE)
	SoDebugError::post("SoGlobalField::SoGlobalField",
			   "There is already a global field named %s",
			   name.getString());
#endif		
    nameDict->enter(key, this);

    fieldData = new SoFieldData;
    fieldData->addField(this, name.getString(), field);

    ref();
}

//////////////////////////////////////////////////////////////////////////////
//
// Description:
//	Destructor.  It is responsible for deleting the globalField from
//	the dictionary.
//
// Use: private

SoGlobalField::~SoGlobalField()
//
//////////////////////////////////////////////////////////////////////////////

{
    // If fieldData is NULL, don't delete the entry. This happens only
    // for the "dummy" instance that is created to read in a real
    // instance of a global field.
    if (fieldData == NULL)
	return;

    unsigned long key = (unsigned long)(getName().getString());
    nameDict->remove(key);

    delete value;
}

//////////////////////////////////////////////////////////////////////////////
//
// Description:
//	Find a globalField with the given name in the dictionary.  Returns
//	NULL if it there is none.
//
// Use: internal

SoGlobalField *
SoGlobalField::find(const SbName &name)
//
//////////////////////////////////////////////////////////////////////////////
{
    unsigned long key = (unsigned long)(name.getString());
    void *result;
    if (nameDict->find(key, result) == FALSE) return NULL;
    return (SoGlobalField *)result;
}

//////////////////////////////////////////////////////////////////////////////
//
// Description:
//	Change the name of a global field.
//
// Use: internal

void
SoGlobalField::changeName(const SbName &newName)
//
//////////////////////////////////////////////////////////////////////////////

{
    // Remove old entry from dictionary
    unsigned long key = (unsigned long)(getName().getString());
    nameDict->remove(key);

    // Create a new fieldData with the correct name for the field:
    SoField *field = value;
    delete fieldData;
    fieldData = new SoFieldData;
    fieldData->addField(this, newName.getString(), field);
    key = (unsigned long)(getName().getString());

    // If there is already an entry with the new name...
    void *oldGlobalField;
    if (nameDict->find(key, oldGlobalField) == TRUE) {
	// Delete the old one
	((SoGlobalField *)oldGlobalField)->unref();
	nameDict->remove(key);
    }

    // Enter this node under the new name
    nameDict->enter(key, this);
}

//////////////////////////////////////////////////////////////////////////////
//
// Description:
//	This is a virtual function used by the database so notification and
//	field to field connections work.  The fieldData for globalFields
//	contains only the value field; the other are hidden from the
//	database and are used only when reading or writing.
//

const SoFieldData *
SoGlobalField::getFieldData() const
//
//////////////////////////////////////////////////////////////////////////////
{
    return fieldData;
}

//////////////////////////////////////////////////////////////////////////////
//
// Description:
//	Returns typeid.
//
// Use: internal
//
SoType
SoGlobalField::getTypeId() const
//
//////////////////////////////////////////////////////////////////////////////
{
    return classTypeId;
}

//////////////////////////////////////////////////////////////////////////////
//
// Description:
//	Write out an instance of a GlobalField.
//	Called by SoBase::write() during a writeAction.
//
// Use: internal

void
SoGlobalField::writeInstance(SoOutput *out)
//
//////////////////////////////////////////////////////////////////////////////
{
    if (! writeHeader(out, FALSE, FALSE)) {
	typeField.write(out, "type");
	value->write(out, getName());
	writeFooter(out);
    }
}

//////////////////////////////////////////////////////////////////////////////
//
// Description:
//	Special method used by SoBase::read to read in a global field.
//	The regular reading method can't be used because there can be
//	only one global field with a given name, so SoBase can't just
//	immediately create a new instance when it reads the
//	"GlobalField {" string from the file.  So, it calls this
//	routine instead.  This returns NULL if there was a read error.
//
// Use: internal, static

SoGlobalField *
SoGlobalField::read(SoInput *in)
//
//////////////////////////////////////////////////////////////////////////////
{
    // Read in "type SFFloat":
    SbName typeName;
    if (!in->read(typeName, TRUE) || typeName != SbName("type"))
	return NULL;

    // Read the value of this private "type" field, which is the name
    // of this globalField. It has to be read as a field, since that's
    // how it's written. (The binary version depends on this!)
    SoSFName typeField;
    if (! typeField.read(in, "type"))
	return FALSE;
    typeName = typeField.getValue();

    SoType type = SoType::fromName(typeName);
    if (! type.isDerivedFrom(SoField::getClassTypeId())) {
	SoReadError::post(in, "\"%s\" is not a type of field",
			  typeName.getString());
	return FALSE;
    }

    // Now, get the name of the next field, which is the name of this
    // globalField:
    SbName myName;
    if (! in->read(myName))
	return FALSE;
    
    // Try to create/get an appropriate global field:
    SbBool	alreadyExists;
    SoGlobalField *result = create(myName, type, alreadyExists);

    // If there already was one with the same name and a different type
    if (result == NULL)
	return NULL;

    if (alreadyExists) {
	// We need to read and throw away the value...
	SoField *t = (SoField *)type.createInstance();
	// If the field has connections, it needs to have a non-NULL
	// container...
	t->setContainer(result);
	t->enableNotify(FALSE);
	if (!t->read(in, myName)) {
	    delete result;
	    delete t;
	    return NULL;
	}
	// Get rid of any connections that may have been established--
	// this gets rid of any extraneous auditors.
	t->disconnect();
	delete t;
    }
    else {
	// Read into newly-created global field.
	if (!result->value->read(in, myName)) {
	    // Problem reading value field
	    delete result;
	    return NULL;
	}
    }
    return result;
}

//////////////////////////////////////////////////////////////////////////////
//
// Description:
//	Return the field this is a container for:
//
// Use: internal

SoField *
SoGlobalField::getMyField() const
//
//////////////////////////////////////////////////////////////////////////////
{
    return value;
}

//////////////////////////////////////////////////////////////////////////////
//
// Description:
//	Return the name of this global field.
//
// Use: internal

SbName
SoGlobalField::getName() const
//
//////////////////////////////////////////////////////////////////////////////
{
    return fieldData->getFieldName(0);
}

//////////////////////////////////////////////////////////////////////////////
//
// Description:
//	A handy utility routine that saved some typing
//
// Use: private

SoType
SoGlobalField::getType() const
//
//////////////////////////////////////////////////////////////////////////////
{
    return SoType::fromName(typeField.getValue());
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds a reference to the instance when writing. Always lie about
//    isFromField, since it will always be from a field but we want to
//    write out the global field anyway.
//
// Use: internal, virtual

void
SoGlobalField::addWriteReference(SoOutput *out, SbBool)
//
////////////////////////////////////////////////////////////////////////
{
    SoFieldContainer::addWriteReference(out, FALSE);
}
