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
 |	SoFieldData
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <assert.h>
#include <Inventor/SbString.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoOutput.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/errors/SoReadError.h>
#include <Inventor/fields/SoField.h>
#include <Inventor/fields/SoFieldData.h>
#include <Inventor/nodes/SoNode.h>

// Syntax for reading/writing type information to files
#define OPEN_BRACE_CHAR		'['
#define CLOSE_BRACE_CHAR	']'
#define VALUE_SEPARATOR_CHAR	','

// In the binary file format, the number of fields integer is actually
// a bitfield containing the number of fields written and whether or
// not there is a description of those fields following.  This assumes
// there are less than 2^14 (16,384) different fields in any node.
// We also assume that unsigned shorts have at least 14 bits.
#define NOT_BUILTIN_BIT (1<<14)

//////////////////////////////////////////////////////////////////////////////
//
//  SoFieldEntry holds the name and offset of a field in an SoFieldData.
//  SoEnumEntry holds the type name and name/value pairs for an enum type.
//  These are internal to Inventor. A list of each of these is stored
//  in the SoFieldData.
//
//////////////////////////////////////////////////////////////////////////////

struct SoFieldEntry {
    SbName		name;		// Name of field
    int32_t		offset;		// Offset of field within object
};

struct SoEnumEntry {
    SbName		typeName;	// Name of enum type
    int			num;		// number of values
    int			arraySize;	// size of arrays
    int			*vals;		// array of values
    SbName		*names;		// array of names

    SoEnumEntry(const SbName &name);
    SoEnumEntry(const SoEnumEntry &o);
    ~SoEnumEntry();

    static int		growSize;		// amount to grow arrays
};

int SoEnumEntry::growSize = 6;

SoEnumEntry::SoEnumEntry(const SbName &name)
{
    typeName	= name;
    num		= 0;
    arraySize	= growSize;
    vals	= new int[arraySize];
    names	= new SbName[arraySize];
}

SoEnumEntry::SoEnumEntry(const SoEnumEntry &o)
{
    typeName	= o.typeName,
    num		= o.num;
    arraySize	= num;
    vals		= new int[arraySize];
    names		= new SbName[arraySize];
    for (int i=0; i<num; i++) {
	vals[i] = o.vals[i];
	names[i] = o.names[i];
    }
}

SoEnumEntry::~SoEnumEntry()
{
    delete [] vals;
    delete [] names;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Copy constructor
//
// Use: public

SoFieldData::SoFieldData(const SoFieldData &src) :
	fields(src.fields.getLength())
//
////////////////////////////////////////////////////////////////////////
{
    copy(&src);
}
	

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Like a copy constructor, but takes a pointer rather than reference.
//    The pointer may be NULL.
//
// Use: public

SoFieldData::SoFieldData(const SoFieldData *src) :
	fields(src ? src->fields.getLength() : 0)
//
////////////////////////////////////////////////////////////////////////
{
    if (src) copy(src);
}
	
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: public

SoFieldData::~SoFieldData()

//
////////////////////////////////////////////////////////////////////////
{
    struct SoFieldEntry *tmpField;
    struct SoEnumEntry  *tmpEnum;

    // Delete the list of fields and enums;
    for (int i=0; i<fields.getLength(); i++) {
        tmpField = (struct SoFieldEntry *)fields[i];
        delete tmpField;
    }

    for (int j=0; j<enums.getLength(); j++) {
        tmpEnum = (struct SoEnumEntry *)enums[j];
        delete tmpEnum;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds a field to current fieldData.
//
// Use: public

void
SoFieldData::addField(SoFieldContainer *defobj,	// Object with default values
		      const char *fieldName,	// Name of field
		      const SoField *field)	// Pointer 2 field (in defNode)
//
////////////////////////////////////////////////////////////////////////
{
    struct SoFieldEntry *newField = new struct SoFieldEntry;
    newField->name   = fieldName;
    newField->offset = (const char *) field - (const char *) defobj;

    fields.append((void *) newField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Copies values and flags from field data in object to another
//    object (of the same type, presumably). If copyConnections is
//    TRUE, any connections to the fields are copied as well.
//
// Use: public

void
SoFieldData::overlay(SoFieldContainer *to, const SoFieldContainer *from,
		     SbBool copyConnections) const
//
////////////////////////////////////////////////////////////////////////
{
    // Access both field datas instead of just one, in case they
    // differ per instance.
    const SoFieldData *fromFD = from->getFieldData();
    const SoFieldData   *toFD =   to->getFieldData();

    SoField	*fromField, *toField;
    int		i;

    for (i = 0; i < fromFD->fields.getLength(); i++) {

	// Access the fields using the appropriate field data instances
	toField   =   toFD->getField(to,   i);
	fromField = fromFD->getField(from, i);

	// If both fields have default values, we don't bother copying
	// the value:
	if (! fromField->isDefault() || ! toField->isDefault())
	    // This method copies just the value...
	    toField->copyFrom(*fromField);

	// ... so we still have to copy the rest
	toField->setIgnored(fromField->isIgnored());
	toField->setDefault(fromField->isDefault());
	toField->enableNotify(fromField->isNotifyEnabled());

	// Allow the field to fix itself up after the copy. Most
	// fields do nothing here, but node, path, and engine fields
	// need to make sure instances are handled properly.
	toField->fixCopy(copyConnections);

	// Copy the connection if necessary
	if (fromField->isConnected() && copyConnections)
	    toField->copyConnection(fromField);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns name of field with given index in given object.
//
// Use: public

const SbName &
SoFieldData::getFieldName(int index) const
//
////////////////////////////////////////////////////////////////////////
{
    return ((SoFieldEntry *) fields[index])->name;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns pointer to field with given index within given node instance.
//
// Use: public

SoField *
SoFieldData::getField(const SoFieldContainer *object, int index) const
//
////////////////////////////////////////////////////////////////////////
{
    // This generates a CC warning; there's not much we can do about it...
    return (SoField *) ((char *) object +
			((SoFieldEntry *) fields[index])->offset);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns index of field, given the field and the instance it is in.

int
SoFieldData::getIndex(const SoFieldContainer *fc, const SoField *field) const
//
////////////////////////////////////////////////////////////////////////
{
    for (int i = 0; i < getNumFields(); i++)
	if (getField(fc, i) == field)
	    return i;

#ifdef DEBUG
    SoDebugError::post("SoFieldData::getIndex", "Can't find index for field");
#endif /* DEBUG */

    return -1;
}

/////////////////////////////////////////////////////////////////
//
// Description:
//
//     Strips leading and trailing white space off a string, returning
// an SbName.
// 
// Use: private in this file.  Should really be a method of SbName
//
#include <ctype.h>
static SbName
stripWhite(const char *name)
//
////////////////////////////////////////////////////////////////////////
{
    int firstchar = -1;
    int lastchar = -1;
    int lastwhite = -1;

    // scan string, marking first & last non-white char, and last white
    int i;
    for (i=0; name[i]; i++) {
	if (isspace(name[i]))
	    lastwhite = i;
	else {
	    if (firstchar == -1) firstchar = i;
	    lastchar = i;
	}
    }

    // No trailing whitespace?  -- just start at first non-white char
    if (lastchar > lastwhite)
	return SbName(&name[firstchar]);

    // trailing whitespace -- need to copy into buffer (to be able
    // to place trailing NULL-termination, since the the string is const).
    char buf[500];
    int b;
    for (b=0, i=firstchar; i<=lastchar; i++, b++)
	buf[b] = name[i];
    buf[b] = 0;
    return SbName(buf);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds an enum value to current fieldData.  If the enum type
//    hasn't been added yet, a new entry is created for it.
//
// Use: public

void
SoFieldData::addEnumValue(const char *typeNameArg, const char *valNameArg,
			  int val)
//
////////////////////////////////////////////////////////////////////////
{
    struct SoEnumEntry	*e = NULL;
    SbName typeName = stripWhite(typeNameArg);
    SbName valName = stripWhite(valNameArg);

    // look for an entry for this type name
    for (int i=0; i<enums.getLength(); i++) {
	e = (struct SoEnumEntry *) enums[i];
	if (e->typeName == typeName)
	    break;
	else
	    e = NULL;
    }
    // make an entry if there wasn't one already
    if (e == NULL) {
	e = new SoEnumEntry(typeName);
	enums.append((void*) e);
    }
    // grow arrays if needed
    if (e->num == e->arraySize) {
	e->arraySize += SoEnumEntry::growSize;
	int *ovals = e->vals;
	SbName *onames = e->names;
	e->vals = new int[e->arraySize];
	e->names = new SbName[e->arraySize];
	for (int i=0; i<e->num; i++) {
	    e->vals[i] = ovals[i];
	    e->names[i] = onames[i];
	}
	delete [] ovals;
	delete [] onames;
    }
    e->vals[e->num] = val;
    e->names[e->num] = valName;
    e->num++;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns name/value arrays for a given enum type.
//
// Use: public

void
SoFieldData::getEnumData(char *typeNameArg, int &num,
			 int *&vals, SbName *&names)
//
////////////////////////////////////////////////////////////////////////
{
    SbName typeName = stripWhite(typeNameArg);

    // look for an entry for this type name
    for (int i=0; i<enums.getLength(); i++) {
	struct SoEnumEntry *e = (struct SoEnumEntry *) enums[i];
	if (e->typeName == typeName) {
	    num		= e->num;
	    vals	= e->vals;
	    names	= e->names;
	    return;
	}
    }
    // no entry found.
    num = 0;
    vals = NULL;
    names = NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads field type information for an unrecognized node.  The
//    ASCII syntax is a set of field type - field name pairs,
//    separated by commas, enclosed withing square brackets.  Returns
//    TRUE on success, FALSE if there is a syntax error or
//    unrecognized field type.
//
//    numDescriptions is a very large number when reading ASCII (where
//    we just read until the closing square brace).
//
// Use: private

SbBool
SoFieldData::readFieldDescriptions(
    SoInput *in, SoFieldContainer *object, int numDescriptions) const

//
////////////////////////////////////////////////////////////////////////
{
    SbBool gotChar;
    SbName fieldType, fieldName;
    char   c;

    SbBool isBinary = in->isBinary();

    SbBool hadFieldsDefined = fields.getLength() > 0;

    if (!isBinary) 
	if (! ((gotChar = in->read(c)) || c != OPEN_BRACE_CHAR))
	    return FALSE;

    for (int i = 0; i < numDescriptions; i++) {

	// Check for closing brace:
	if (!isBinary) {
	    // Check for closing brace:
	    if (in->read(c) && c == CLOSE_BRACE_CHAR)
		return TRUE;
	    else in->putBack(c);
	}

	SbName type, fieldName;
	if (!in->read(type, TRUE)) return FALSE;
	if (!in->read(fieldName, TRUE)) return FALSE;

	SoType fldType = SoType::fromName(type);

	if (!hadFieldsDefined) {
	    // Only create fields if fields haven't already been
	    // defined.  This isn't 100% correct-- we'll create
	    // field for nodes/engines that have not fields if the
	    // user specifies fields/inputs for them.  But that's
	    // a case I'm not going to worry about (there are VERY
	    // few nodes that have no fields).

	    if (fldType.isBad())
		return FALSE;

	    // Create and initialize an instance of the field.
	    // Add it to the field data.
	    SoField *fld = (SoField *)(fldType.createInstance());
	    fld->setContainer(object);

	    // Cast const away:
	    SoFieldData *This = (SoFieldData *)this;
	    This->addField(object, fieldName.getString(), fld);
	}
#ifdef DEBUG
	else {
	    // Check to make sure specification matches reality:
	    SoField *f = object->getField(fieldName);
	    if (f == NULL) {
		SoDebugError::post("SoFieldData::readFieldDescriptions",
				   "%s does not have a field named %s",
				   object->getTypeId().getName().getString(),
				   fieldName.getString());
	    }
	    else if (!f->isOfType(fldType)) {
		SoDebugError::postWarning("SoFieldData::readFieldDescriptions",
				   "%s.%s is type %s, NOT type %s",
				   object->getTypeId().getName().getString(),
				   fieldName.getString(),
				   f->getTypeId().getName().getString(),
				   type.getString());
	    }
	}
#endif
	if (!isBinary) {
	    // Better get a ',' or a ']' at this point:
            if (! in->read(c))
                return FALSE;
            if (c != VALUE_SEPARATOR_CHAR) {
		if (c == CLOSE_BRACE_CHAR)
		    return TRUE;
		else return FALSE;
	    }
	    // Got a ',', continue reading
	}
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads into fields of object according to SoInput. The third
//    parameter indicates whether an unknown field should be reported
//    as an error (default is TRUE); this can be FALSE for nodes that
//    have children.
//
// Use: internal

SbBool
SoFieldData::read(SoInput *in, SoFieldContainer *object,
		  SbBool errorOnUnknownField, SbBool &notBuiltIn) const 
//
////////////////////////////////////////////////////////////////////////
{
    // TRUE if reading an Inventor file pre-Inventor 2.1:
    SbBool	oldFileFormat = (in->getIVVersion() < 2.1f);

    // TRUE if reading binary file format:
    SbBool	isBinary = in->isBinary();

    // Assume it is a built-in node, until we figure out otherwise:
    notBuiltIn = FALSE;

    // COMPATIBILITY case:

    if (oldFileFormat && isBinary) {

	// If 2.0 file format, and the thing we're reading
	// isn't (and, we assume, wasn't) built-in:
	if (in->getIVVersion() > 1.0f && !object->getIsBuiltIn()) {
	    // The "fields" or "inputs" string is read by SoBase,
	    // because it may need them to decide whether or not to
	    // create an UnknownNode or an UnknownEngine.
	    int numDescriptions;
	    if (!in->read(numDescriptions))
		return FALSE;

	    notBuiltIn = TRUE;
	    // The rest of it is just like 2.1 format, IF the object
	    // has any fields:
	    if (!readFieldDescriptions(in, object, numDescriptions))
		return FALSE;
	}
	// In the old file format, objects with no fields
	// didn't write out anything for the field values:
	if (fields.getLength() == 0) return TRUE;

	// This is mostly like Inventor 2.1 file format, except that
	// there is no NOT_BUILTIN_BIT:
	int numFieldsWritten;
	if (!in->read(numFieldsWritten))
	    return FALSE;
	return readFields(in, object, numFieldsWritten);
    }

    // BINARY case:

    else if (isBinary) {
	unsigned short numWritten;

	// First read number of fields written
	if (! in->read(numWritten))
	    return FALSE;

	// Figure out if field descriptions were written:
	if (numWritten & NOT_BUILTIN_BIT) {
	    notBuiltIn = TRUE;
	    numWritten &= (~NOT_BUILTIN_BIT); // Clear bit
	}

	if (notBuiltIn) {
	    if (!readFieldDescriptions(in, object, numWritten))
		return FALSE;
	}

	return readFields(in, object, numWritten);
    }

    // ASCII case:
    else {

	// Only check for "fields" or "inputs" the first time:
	SbBool firstTime = TRUE;

	SbName fieldName;

	// Keep reading fields until done
	while (TRUE) {

	    // If no field name, just return.
	    if (!in->read(fieldName, TRUE) || ! fieldName)
		return TRUE;

	    // Read field descriptions.  Field descriptions may be
	    // given for built-in nodes, and do NOT have to be given
	    // for non-built-in nodes as long as their code can be
	    // DSO-loaded.
	    if (firstTime) {
		firstTime = FALSE;
		if (fieldName == "fields" || fieldName == "inputs") {
		    notBuiltIn = TRUE;
		    if (!readFieldDescriptions(in, object, NOT_BUILTIN_BIT))
			return TRUE;
		    continue;
		}
	    }

	    SbBool foundName;
	    if (! read(in, object, fieldName, foundName))
		return FALSE;

	    // No match with any valid field name
	    if (!foundName) {
		if (errorOnUnknownField) {
		    SoReadError::post(in, "Unknown field \"%s\"",
				      fieldName.getString());
		    return FALSE;
		}
		else {
		    // Put the field name back in the stream
		    in->putBack(fieldName.getString());
		    return TRUE;
		}
	    }
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Used when reading binary file format:
//
// Use: internal

SbBool
SoFieldData::readFields(SoInput *in, SoFieldContainer *object,
		       int numWritten) const
//
////////////////////////////////////////////////////////////////////////
{
    SbName fieldName;

    // Read the fields - no tolerance for bad data
    for (int f = 0; f < numWritten; f++) {
	if (! in->read(fieldName, TRUE) || ! fieldName)
	    return FALSE;

	SbBool foundName;
	if (! read(in, object, fieldName, foundName))
	    return FALSE;

	// No match with any valid field name
	if (! foundName) {
	    SoReadError::post(in, "Unknown field \"%s\"",
			      fieldName.getString());
	    return FALSE;
	}
    }
    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// This function is used when the field name has already been
// read, and just the value needs to be read in.  It is used by
// the above read() method and to read in GlobalFields.
//
// Use: internal

SbBool
SoFieldData::read(SoInput *in, SoFieldContainer *object,
		  const SbName &fieldName, SbBool &foundName) const
//
////////////////////////////////////////////////////////////////////////
{
    int i;
    for (i = 0; i < fields.getLength(); i++) {
	if (fieldName == getFieldName(i)) {
	    if (! getField(object, i)->read(in, fieldName))
		return FALSE;
	    break;
	}
    }
    if (i == fields.getLength())
	foundName = FALSE;
    else foundName = TRUE;

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes all fields. The info in the SoOutput provides more
//    specifics (file pointer, etc.). This may be called for either of
//    the 2 stages of writing.
//
// Use: internal

void
SoFieldData::write(SoOutput *out, const SoFieldContainer *container) const
//
////////////////////////////////////////////////////////////////////////
{
    // If in writing stage and writing binary, count number of fields
    // to write and write it
    if (out->getStage() == SoOutput::WRITE) {

	SbBool isBuiltIn = container->getIsBuiltIn();

	// In binary, we combine the isBuiltIn flag with the number of
	// fields we're going to write (which is also the number of
	// fields we'll describe):
	if (out->isBinary()) {
	    unsigned short numToWrite = 0;
	    for (int i = 0; i < fields.getLength(); i++) {
		SoField *field = getField(container, i);
		if (field->shouldWrite())
		    numToWrite++;
	    }
	    if (!isBuiltIn)
		out->write(numToWrite | NOT_BUILTIN_BIT);
	    else
		out->write(numToWrite);
	}
	if (!isBuiltIn)
	    writeFieldDescriptions(out, container);
    }

    // Let each field do its thing
    for (int i = 0; i < fields.getLength(); i++) {

	SoField *field = getField(container, i);

	if (field->shouldWrite())
	    field->write(out, getFieldName(i));
    }
}

////////////////////////////////////////////////////////////////////////
//
//    Write out field descriptions
//
// Use: internal

void
SoFieldData::writeFieldDescriptions(SoOutput *out,
				    const SoFieldContainer *container) const
//
////////////////////////////////////////////////////////////////////////
{
    SbBool isBinary = out->isBinary();

    if (!isBinary) {
        out->indent();
	if (container->isOfType(SoNode::getClassTypeId()))
	    out->write("fields");
	else
	    out->write("inputs");
        out->write(' ');
        out->write(OPEN_BRACE_CHAR);
        out->write(' ');
        out->incrementIndent(2);
    }

    // For each field, write out the field type name and field name.
    int i = 0;
    int numWritten = 0;
    for (i = 0; i < getNumFields(); i++) {

	SoField *field = getField(container, i);

	// Don't write descriptions of fields that won't be written:
	if (!field->shouldWrite())
	    continue;

        SoType  type = field->getTypeId();

        out->write(type.getName().getString());
        if (! isBinary)
            out->write(' ');
        out->write(getFieldName(i).getString());

        // Format nice and pretty for ascii
        if (! isBinary) {
            if (i != getNumFields() - 1) {
                out->write(VALUE_SEPARATOR_CHAR);
		++numWritten;
		if ((numWritten%4) == 0) { // 4 pairs per line
		    out->write('\n');
		    out->indent();
		} else out->write(' ');
            }
        }
    }

    // Write out the closing brace of the field type information.
    if (! out->isBinary()) {
        out->write(' ');
        out->write(CLOSE_BRACE_CHAR);
        out->write('\n');
        out->decrementIndent(2);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Copies all fields from given field data into this one, creating
//    the new fields. This should be used only for copying fields from
//    a base class into a derived class, since that's the only
//    situation in which we are guaranteed that the field offsets are
//    the same.
//
// Use: internal

void
SoFieldData::copy(const SoFieldData *from)
//
////////////////////////////////////////////////////////////////////////
{
    int			i;
    if (from == NULL)
	return;

    // Copy field entries
    for (i = 0; i < from->fields.getLength(); i++) {

	struct SoFieldEntry *fromField =
	    (struct SoFieldEntry *) from->fields[i];

	fields.append((void *) new struct SoFieldEntry(*fromField));
    }

    // Copy enum entries
    for (i = 0; i < from->enums.getLength(); i++) {

	struct SoEnumEntry *fromEnum = (struct SoEnumEntry *) from->enums[i];

	enums.append((void *) new struct SoEnumEntry(*fromEnum));
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if the values in the fields of one container are
//    the same as those in another (of the same type, presumably).
//
// Use: internal

SbBool
SoFieldData::isSame(const SoFieldContainer *c1, 
		    const SoFieldContainer *c2) const
//
////////////////////////////////////////////////////////////////////////
{
    int	i;

    for (i = 0; i < fields.getLength(); i++)
	if (! getField(c1, i)->isSame(*getField(c2, i)))
	    return FALSE;

    return TRUE;
}
