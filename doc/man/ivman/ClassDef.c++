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

#include <Inventor/SoLists.h>
#include <Inventor/SoPath.h>
#include <Inventor/engines/SoConcatenate.h>
#include <Inventor/engines/SoEngine.h>
#include <Inventor/engines/SoGate.h>
#include <Inventor/engines/SoSelectOne.h>
#include <Inventor/fields/SoField.h>
#include <Inventor/fields/SoMFTime.h>
#include <Inventor/fields/SoMFInt32.h>
#include <Inventor/fields/SoMFUInt32.h>
#include <Inventor/fields/SoSFTime.h>
#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/fields/SoSFUInt32.h>
#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/nodekits/SoNodekitCatalog.h>
#include "Man.h"

/////////////////////////////////////////////////////////////////////////////
//
// Class: ClassDef
//
/////////////////////////////////////////////////////////////////////////////

const ClassDef *	ClassDef::curClass = NULL;

ClassDef::ClassDef()
{
    parentClass = NULL;
}

ClassDef::~ClassDef()
{
    if (type.isDerivedFrom(SoBase::getClassTypeId()) && instance != NULL)
	((SoBase *) instance)->unref();
}

void
ClassDef::addInherit(ClassDef *def)
{
    // FALSE = add to start of list
    inherits.add(new Inherit(def->name), FALSE);
}

SbBool
ClassDef::read()
{
    SbName	nm;
    SbBool	ok = TRUE;

    curClass = this;

    // Read statements while they exist
    while (ok && Reader::readName(nm)) {

	// See what kind of statement we have
	switch (getStatementType(nm)) {

	  case NAME:
	    ok = (Reader::readName(name) &&
		  Reader::readName(cPrefix) &&
		  Reader::readName(cEnumPrefix) &&
		  Reader::readBracketed(shortDescription));
	    if (! ok)
		Reader::error("NAME spec");
	    break;

	  case DESCRIPTION:
	    ok = Reader::readBracketed(longDescription);
	    if (! ok)
		Reader::error("DESC");
	    break;

	  case INCLUDE:
	    ok = readItem(new Include, &includes);
	    break;

	  case TYPEDEF:
	    ok = readItem(new Typedef, &typedefs);
	    break;

	  case DEFINE:
	    ok = readItem(new Define, &defines);
	    break;

	  case ENUM:
	    ok = readItem(new Enum, &enums);
	    break;

	  case FIELD:
	    ok = readItem(new Field, &fields);
	    break;

	  case INPUT:
	    ok = readItem(new Input, &inputs);
	    break;

	  case OUTPUT:
	    ok = readItem(new Output, &outputs);
	    break;

	  case PART:
	    ok = readItem(new Part, &parts);
	    break;

	  case METHOD:
	    ok = readItem(new Method, &methods);
	    break;

	  case DRAGGER_RESOURCE:
	    ok = readItem(new DraggerResource, &draggerResources);
	    break;

	  case MACRO:
	    ok = readItem(new Macro, &macros);
	    break;

	  case ACTION:
	    ok = readItem(new Action, &actions);
	    break;

	  case MISC:
	    ok = readItem(new Misc, &miscs);
	    break;

	  case ALSO:
	    ok = readItem(new Also, &alsos);
	    break;

	  case BEGIN:
	    {
		SbName	langName;

		ok = Reader::readName(langName);

		if (ok) {
		    if (langName == "C")
			Item::setCurLanguage(C);
		    else if (langName == "C++")
			Item::setCurLanguage(C_PLUS_PLUS);
		    else
			ok = FALSE;
		}

		if (! ok)
		    Reader::error("BEGIN language name");
	    }
	    break;

	  case END:
	    Item::setCurLanguage(ALL);
	    break;

	  default:
	    Reader::error("statement keyword");
	    ok = FALSE;
	}
    }

    // If there's stuff left in the file, error
    if (Reader::get() != EOF)
	ok = FALSE;

    return ok;
}

SbBool
ClassDef::readItem(Item *item, ItemList *list)
{
    if (! item->read())
	return FALSE;

    list->add(item);

    return TRUE;
}

ClassDef::StatementType
ClassDef::getStatementType(const SbName &keyword)
{
    static SbName	*names = NULL;
    static const char	*keywords[] = {
	"NAME",
	"DESC",
	"INCLUDE",
	"TYPEDEF",
	"DEFINE",
	"ENUM",
	"FIELD",
	"INPUT",
	"OUTPUT",
	"PART",
	"METHOD",
	"MACRO",
	"ACTION",
	"MISC",
	"ALSO",
	"BEGIN",
	"END",
	"DRAGGER_RESOURCE",
    };	

#define NUM_KEY_WORDS (sizeof(keywords) / sizeof(keywords[0]))

    int	i;

    if (names == NULL) {
	names = new SbName[NUM_KEY_WORDS];
	for (i = 0; i < NUM_KEY_WORDS; i++)
	    names[i] = keywords[i];
    }

    for (i = 0; i < NUM_KEY_WORDS; i++)
	if (keyword == keywords[i])
	    return (StatementType) i;

    return UNKNOWN;
}

SbBool
ClassDef::setUp(const ClassDef *mainDef)
{
    curClass = this;

    // Find out the type of the class, if any. Remove the "So" from
    // the name, since the names don't contain the prefix.
    // But first check to see if the prefix is Sb.  If so, don't setup
    // up the file format section.
    SbBool isSb = strncmp(name.getString(), "Sb", 2) ? FALSE : TRUE;
    type = SoType::fromName(name.getString() + 2);

    // Get an instance of the class, if possible. If this is not the
    // main ClassDef (mainDef is NULL), we will use an instance of the
    // main ClassDef for this class as well, rather than choosing one
    // randomly in getInstance().
    if (mainDef != NULL)
	instance = mainDef->instance;

    else if (type.isBad() || isSb)
	instance = NULL;

    else {
	instance = getInstance();

	// Ref instance if necessary to keep it around
	if (type.isDerivedFrom(SoBase::getClassTypeId()) && instance != NULL)
	    ((SoBase *) instance)->ref();
    }

    // Set the C prefix to the class name, unless it was specified
    if (Writer::getLanguage() == C && ! cPrefix)
	cPrefix = name;

    if (Writer::getLanguage() == C)
	setUpTypedefs();

    if (! fields.isEmpty() && ! setUpFields())
	return FALSE;

    if (! inputs.isEmpty() && ! setUpInputs())
	return FALSE;

    if (! outputs.isEmpty() && ! setUpOutputs())
	return FALSE;

    // Look up part types (if any)
    if (! parts.isEmpty() && ! setUpParts())
	return FALSE;

    // Produce nodekit catalog and file format if necessary
    if (! isSb) {
	setUpCatalog();
        setUpFileFormat();
    }

    return TRUE;
}

SbBool
ClassDef::setUpTypedefs()
{
    // The C language version has a typedef for the class if the
    // prefix is not the same as the class name
    if (cPrefix != name) {
	Typedef	*def = new Typedef;
	def->language = C;
	def->type = name.getString();
	def->name = cPrefix;

	typedefs.add(def);
    }

    return TRUE;
}

SbBool
ClassDef::setUpFields()
{
    if (instance == NULL ||
	! type.isDerivedFrom(SoFieldContainer::getClassTypeId())) {
	fprintf(stderr, "Can't create a (non-abstract) field container from "
		"class \"%s\"\n", name.getString());
	return FALSE;
    }

    SoFieldContainer *fc = (SoFieldContainer *) instance;

    for (Item *item = fields.first; item != NULL; item = item->next) {
	Field *f = (Field *) item;

	SoField *field = fc->getField(f->name);

	if (field == NULL) {
	    fprintf(stderr, "No field \"%s\" in class \"%s\"\n",
		    f->name.getString(), name.getString());
	    return FALSE;
	}

	SbString typeName = "So";
	SoType fieldType = field->getTypeId();
	if (fieldType == SoSFInt32::getClassTypeId()) {
	    typeName += "SFInt32";
	} else 	if (fieldType == SoMFInt32::getClassTypeId()) {
	    typeName += "MFInt32";
	} else 	if (fieldType == SoSFUInt32::getClassTypeId()) {
	    typeName += "SFUInt32";
	} else 	if (fieldType == SoMFUInt32::getClassTypeId()) {
	    typeName += "MFUInt32";
	} else
	    typeName += fieldType.getName().getString();
	
	f->type = typeName;
    }

    return TRUE;
}

SbBool
ClassDef::setUpInputs()
{
    if (instance == NULL ||
	! type.isDerivedFrom(SoEngine::getClassTypeId())) {
	fprintf(stderr, "Can't create a (non-abstract) engine from "
		"class \"%s\"\n", name.getString());
	return FALSE;
    }

    SoEngine *eng = (SoEngine *) instance;

    for (Item *item = inputs.first; item != NULL; item = item->next) {
	Input *in = (Input *) item;

	SoField *input = eng->getField(in->name);

	if (input == NULL) {
	    fprintf(stderr, 
		    "No input \"%s\" in class \"%s\"\n"
		    "Assuming <inputType>\n",	
		    in->name.getString(), name.getString());
	    SbString typeName = "<inputType>";	
	    in->type = typeName;
	} 
        else {
	    SbString typeName = "So";
	    SoType inputType = input->getTypeId();

	if (inputType == SoSFInt32::getClassTypeId()) {
	    typeName += "SFInt32";
	} else 	if (inputType == SoMFInt32::getClassTypeId()) {
	    typeName += "MFInt32";
	} else 	if (inputType == SoSFUInt32::getClassTypeId()) {
	    typeName += "SFUInt32";
	} else 	if (inputType == SoMFUInt32::getClassTypeId()) {
	    typeName += "MFUInt32";
	} else
	    typeName += inputType.getName().getString();

	    in->type = typeName;
 	}
    }

    return TRUE;
}

SbBool
ClassDef::setUpOutputs()
{
    if (instance == NULL ||
	! type.isDerivedFrom(SoEngine::getClassTypeId())) {
	fprintf(stderr, "Can't create a (non-abstract) engine from "
		"class \"%s\"\n", name.getString());
	return FALSE;
    }

    SoEngine *eng = (SoEngine *) instance;

    for (Item *item = outputs.first; item != NULL; item = item->next) {
	Output *out = (Output *) item;

	SoEngineOutput *output = eng->getOutput(out->name);

	if (output == NULL) {
	    fprintf(stderr, "No output \"%s\" in class \"%s\"\n"
			"Assuming <outputType>\n",
		    out->name.getString(), name.getString());
	    SbString typeName = "<outputType>";
	    out->type = typeName;
	} 
	else {
	    SbString typeName = "So";

	    SoType outputType = output->getConnectionType();
	    if (outputType == SoSFInt32::getClassTypeId()) {
		typeName += "SFInt32";
	    } else 	if (outputType == SoMFInt32::getClassTypeId()) {
		typeName += "MFInt32";
	    } else 	if (outputType == SoSFUInt32::getClassTypeId()) {
		typeName += "SFUInt32";
	    } else 	if (outputType == SoMFUInt32::getClassTypeId()) {
		typeName += "MFUInt32";
	    } else
		typeName += outputType.getName().getString();
	
	    out->type = typeName;
        }
    }

    return TRUE;
}

SbBool
ClassDef::setUpParts()
{
    if (instance == NULL ||
	! type.isDerivedFrom(SoBaseKit::getClassTypeId())) {
	fprintf(stderr, "Can't create a (non-abstract) node kit from "
		"class \"%s\"\n", name.getString());
	return FALSE;
    }

    SoBaseKit *kit = (SoBaseKit *) instance;

    for (Item *item = parts.first; item != NULL; item = item->next) {
	Part *part = (Part *) item;

	const SoNodekitCatalog *cat = kit->getNodekitCatalog();
	SoType type = cat->getType(part->name);

	if (type.isBad()) {
	    fprintf(stderr,"No part \"%s\" in class \"%s\"\n",
		part->name.getString(), name.getString());
		return FALSE;
	}

	SbString typeName = "So";
	typeName += type.getName().getString();

	part->type = typeName;
    }

    return TRUE;
}

SbBool
ClassDef::setUpCatalog()
{
    int	i;

    if (instance == NULL ||
	! type.isDerivedFrom(SoBaseKit::getClassTypeId())) {
	return TRUE;
    }

    SoBaseKit *kit = (SoBaseKit *) instance;

    const SoNodekitCatalog *cat = kit->getNodekitCatalog();

    //
    // Main catalog part table
    //

    // The .ne (need) request makes sure the smallest table fits on
    // the page. For some reason, tbl gets the wrong value for its own
    // .ne, so this hack fixes up some simple cases.
    // First, figure out how many entries are gonna be in the table:
    int numInPublicTable = 0;
    for (i = 0; i < cat->getNumEntries(); i++ )
	if (cat->isPublic(i)) 
	    numInPublicTable++;

    // Next, print a .ne command to a string, adding our fudge number:
#define PUBLIC_TBL_FUDGE_NUM 9
    char nePublicString[20];
    sprintf(nePublicString, ".ne %d\n", PUBLIC_TBL_FUDGE_NUM+numInPublicTable);

    catalog =   nePublicString;
    catalog +=  ".TS\n"
		"box, tab(!);\n"
		"cb s  s  s\n"
		"lb lb lb lb\n"
		"lb lb lb lb\n"
		"l  l  l  c.\n"
		"All parts\n"
		"!!!NULL by\n"
		"Part Name!Part Type!Default Type!Default\n"
		"\n";

    for (i = 0; i < cat->getNumEntries(); i++ ) {

	if (cat->isPublic(i)) {

	    // Part name
	    catalog += cat->getName(i).getString();

	    // Part type
	    catalog += "!";
	    catalog += cat->getType(i).getName().getString();

	    // Default type
	    catalog += "!";
	    if (cat->getDefaultType(i) != cat->getType(i))
		catalog += cat->getDefaultType(i).getName().getString();
	    else
		catalog += "--";

	    // NULL by default
	    catalog += "!";
	    catalog += (cat->isNullByDefault(i) ? "yes" : "no");

	    catalog += "\n";
	}
    }

    catalog += "\n.TE\n";

    //
    // Catalog list part table
    //

    // Make sure there is at least one list first
    SbBool	anyLists = FALSE;
    for (i = 0; i < cat->getNumEntries(); i++ ) {
	if (cat->isList(i) && cat->isPublic(i)) {
	    anyLists = TRUE;
	    break;
	}
    }

    if (anyLists) {

	// The .ne (need) request makes sure the smallest table fits on
	// the page. For some reason, tbl gets the wrong value for its own
	// .ne, so this hack fixes up some simple cases.

	// First, figure out how many entries are gonna be in the table:
	int numInListTable = 0;
	for (i = 0; i < cat->getNumEntries(); i++ )
	    if (cat->isList(i) && cat->isPublic(i))
		numInListTable++;

	// Next, print a .ne command to a string, adding our fudge number:
#define LIST_TBL_FUDGE_NUM 9
	char neListSTring[20];
	sprintf(neListSTring, ".ne %d\n", LIST_TBL_FUDGE_NUM+numInListTable);

	catalog += neListSTring;
	catalog += ".TS\n"
		   "box, tab(!);\n"
		   "cb s  s\n"
		   "lb lb lbw(30n)\n"
		   "l  l  l.\n"
		   "Extra information for list parts from above table\n"
		   ".sp\n"
		   "Part Name!Container Type!Permissible Types\n"
		   "\n";

	for (i = 0; i < cat->getNumEntries(); i++ ) {

	    if (cat->isList(i) && cat->isPublic(i)) {

		// Part name
		catalog += cat->getName(i).getString();

		// Container type
		catalog += "!";
		catalog += cat->getListContainerType(i).getName().getString();

		// Permissible types
 		// Note: type names are entered within T{ and T}
		// blocks so that they can be formatted when necessary.
		catalog += "!T{\n";
		const SoTypeList &permTypes = cat->getListItemTypes(i);
		for (int j = 0; j < permTypes.getLength(); j++) {
		    catalog += permTypes[j].getName().getString();
		    if (j < permTypes.getLength() - 1)
			catalog += ", ";
		}
		catalog += "\nT}";

		catalog += "\n";
	    }
	}

	catalog += "\n.TE\n";
    }

    return TRUE;
}

SbBool
ClassDef::setUpFileFormat()
{
    hasFileFormat = FALSE;
    fileFormat.makeEmpty();
    fileFormatTabLength = -1;

    // We need file format if the class is derived from SoNode or SoEngine
    if (type.isDerivedFrom(SoNode::getClassTypeId()) || 
	type.isDerivedFrom(SoEngine::getClassTypeId())) {

	// Abstract class?
	if (isAbstract) {
	    fileFormat = "This is an abstract class. See the reference page "
		"of a derived class for the format and default values.\n";
	    hasFileFormat = TRUE;
	}

	else if (type.isDerivedFrom(SoBaseKit::getClassTypeId())) {
	    ////////////////////////////////////////////////////////
	    //
	    // Special case for nodekits. They print their fields in 
	    // a different order than they are defined.
	    //
	    if (instance == NULL)
		return TRUE;

	    SoBaseKit *kit = (SoBaseKit *) instance;
	    const SoNodekitCatalog *cat = kit->getNodekitCatalog();

	    SoFieldList		list;
	    kit->getFields(list);

	    SoField  *field;
	    int      partNum, i;
	    SbString fieldVal;
	    SbName   fieldName;
	    int      len;

	    // First, write out fields that are NOT parts...
	    // Whatever they are, just write out the field value.
	    for (i = 0; i < list.getLength(); i++) {

		field = list[i];
		kit->getFieldName(field, fieldName);
		partNum = cat->getPartNumber(fieldName);

		if (partNum == SO_CATALOG_NAME_NOT_FOUND) {
		    field->get(fieldVal);
		    fileFormat += fieldName.getString();
		    fileFormat += "\t";
		    fileFormat += fieldVal;
		    fileFormat += "\n";
		    len = fieldName.getLength();
		    if (len > fileFormatTabLength)
			fileFormatTabLength = len;
		}
	    }

	    // Note: we skip all of this for the BOOK version...
	    if (! Writer::isForBook()) {

		// Second, write out fields that are PUBLIC PARTS.
		// Private parts do not get written in the man page.
		// When writing parts, we write <"resourcename"
		// resource> if the node is named in the global
		// database. Otherwise, we write out the scene graph.
		SoNode *partNode;
		for (i = 0; i < list.getLength(); i++) {

		    field = list[i];
		    kit->getFieldName(field, fieldName);
		    partNum = cat->getPartNumber(fieldName);

		    if (partNum != SO_CATALOG_NAME_NOT_FOUND &&
			cat->isPublic(partNum)) {

			partNode = ((SoSFNode *)field)->getValue();
			if (partNode == NULL  || !partNode->getName()) {
			    // If the node has no name in the global dictionary
			    // or is NULL, print out the value of the node.
			    // This might turn out to be an entire scene graph!
			    field->get(fieldVal);
			}
			else {
			    // Just print out the name of the node.
			    fieldVal = "<";
			    fieldVal += partNode->getName().getString();
			    fieldVal += " resource>";
			}
			fileFormat += fieldName.getString();
			fileFormat += "\t";
			fileFormat += fieldVal;
			fileFormat += "\n";
			len = fieldName.getLength();
			if (len > fileFormatTabLength)
			    fileFormatTabLength = len;
		    }
		}

		// Finally, dragger resources may exist for parts that are in
		// nested draggers. Write out these nested partNames
		// and <"resourcename" resource>
		if (! draggerResources.isEmpty()) {
		    Item *item;
		    for (item = draggerResources.first; 
			 item != NULL; item = item->next ) {

			DraggerResource *res = (DraggerResource *) item;

			// Check if any of the parts that use this resource
			// do not simply match the partNames in the catalog.
			// If not, we write out the partName and resource.
			for (int j = 0; j < res->getNumPartDefs(); j++ ) {
			    const SbName pDefName = res->getPartDefName(j);
			    if ( cat->getPartNumber(pDefName) 
				== SO_CATALOG_NAME_NOT_FOUND ) {
				// Write it all out...
				fieldVal = "<";
				fieldVal += res->getResourceName().getString();
				fieldVal += " resource>";

				fileFormat += pDefName.getString();
				fileFormat += "\t";
				fileFormat += fieldVal;
				fileFormat += "\n";
				len = pDefName.getLength();
				if (len > fileFormatTabLength)
				    fileFormatTabLength = len;
			    }
			}
		    }
		}
	    }

	    hasFileFormat = TRUE;

	    //
	    ////////////////////////////////////////////////////////
	}
	else {
	    SoFieldContainer *fc = (SoFieldContainer *) instance;

	    ////////////////////////////////////////////////////////
	    //
	    // Special case for engines whose constructor takes the
	    // type of field to operate on
	    //
	    if (fc->isOfType(SoConcatenate::getClassTypeId()))
		fileFormat +=	"type\t<inputType>\n"
				"input0\t[]\n"
				"input1\t[]\n"
				"input2\t[]\n"
				"input3\t[]\n"
				"input4\t[]\n"
				"input5\t[]\n"
				"input6\t[]\n"
				"input7\t[]\n"
				"input8\t[]\n"
				"input9\t[]\n";

	    else if (fc->isOfType(SoGate::getClassTypeId()) ||
		     fc->isOfType(SoSelectOne::getClassTypeId()))
		fileFormat += "type\t<inputType>\ninput\t[]\n";
	    //
	    ////////////////////////////////////////////////////////

	    // Make sure each field does not have a default value
	    SoFieldList		list;

	    fc->getFields(list);

	    for (int i = 0; i < list.getLength(); i++) {
		SoField		*field = list[i];
		SbString	fieldVal;
		SbName		fieldName;
		int		len;

		fc->getFieldName(field, fieldName);

		// Special case for time fields connected to the clock
		if (field->isConnected() &&
		    (field->isOfType(SoSFTime::getClassTypeId()) ||
		     field->isOfType(SoMFTime::getClassTypeId())))
		    fieldVal = "<current time>";
		else
		    field->get(fieldVal);

		fileFormat += fieldName.getString();
		fileFormat += "\t";
		fileFormat += fieldVal;
		fileFormat += "\n";

		len = fieldName.getLength();
		if (len > fileFormatTabLength)
		    fileFormatTabLength = len;
	    }
	    hasFileFormat = TRUE;
	}
    }

    return TRUE;
}

void *
ClassDef::getInstance()
{
    void	*inst;

    isAbstract = ! type.canCreateInstance();

    // If the given class is not abstract, return an instance of it
    if (! isAbstract)
	inst = type.createInstance();

    else {
	// Otherwise, find all classes derived from the class and
	// return the first one that is not abstract
	SoTypeList list;
	SoType::getAllDerivedFrom(type, list);

	inst = NULL;

	for (int i = 0; i < list.getLength(); i++) {
	    if (list[i].canCreateInstance()) {
		inst = list[i].createInstance();
		break;
	    }
	}
    }

    // No non-abstract classes
    return inst;
}

void
ClassDef::cullMethods()
{
    ////////////////////////////////////////////
    //
    // This method is called on the main ClassDef - the one the man
    // page is for. It removes any method from an inherited class that:
    //	   (1) is a constructor,
    //  or (2) has the same name as a method on a more derived class,
    //  or (3) is a destructor and the language is C++.
    //
    // It also removes hidden methods from the main ClassDef and all
    // inherited ClassDefs.
    //
    ////////////////////////////////////////////

    ClassDef	*cd, *inheritedCd;
    Method	*meth, *inhMeth, *prevMeth;

    //
    // Pass 1: Mark all constructors and destructors
    //
    for (cd = parentClass; cd != NULL; cd = cd->parentClass) {
	for (meth = (Method *) cd->methods.first;
	     meth != NULL;
	     meth = (Method *) meth->next) {

	    // Remove constructors
	    if (meth->type == Method::CONSTRUCTOR)
		meth->skipThis = TRUE;

	    // Remove destructors from C++
	    else if (meth->type == Method::DESTRUCTOR &&
		     Writer::getLanguage() == C_PLUS_PLUS)
		meth->skipThis = TRUE;
	}
    }

    //
    // Pass 2: Mark any method with the same name as in a more
    // derived class. This is a slow way to do this, but it shouldn't
    // be too bad in most cases.
    //
    for (cd = this; cd != NULL; cd = cd->parentClass) {
	for (meth = (Method *) cd->methods.first;
	     meth != NULL;
	     meth = (Method *) meth->next) {

	    if (! meth->skipThis) {

		// Look through inherited classes for a method with
		// the same name

		for (inheritedCd = cd->parentClass; inheritedCd != NULL;
		     inheritedCd = inheritedCd->parentClass) {

		    if (Writer::getLanguage() == C) {
			for (inhMeth = (Method *) inheritedCd->methods.first;
			     inhMeth != NULL;
			     inhMeth = (Method *) inhMeth->next) {

			    if (inhMeth->cName == meth->cName)
				inhMeth->skipThis = TRUE;
			}
		    }
		    else {
			for (inhMeth = (Method *) inheritedCd->methods.first;
			     inhMeth != NULL;
			     inhMeth = (Method *) inhMeth->next) {

			    if (inhMeth->name == meth->name)
				inhMeth->skipThis = TRUE;
			}
		    }
		}
	    }
	}
    }

    //
    // Pass 3: Mark hidden methods
    //
    for (cd = this; cd != NULL; cd = cd->parentClass) {
	for (meth = (Method *) cd->methods.first;
	     meth != NULL;
	     meth = (Method *) meth->next) {

	    if (meth->isHidden)
		meth->skipThis = TRUE;
	}
    }

    //
    // Pass 4: Actually remove marked methods from lists
    //
    for (cd = this; cd != NULL; cd = cd->parentClass) {

	prevMeth = NULL;

	for (meth = (Method *) cd->methods.first;
	     meth != NULL;
	     meth = (Method *) meth->next) {

	    if (meth->skipThis) {
		if (prevMeth == NULL)
		    cd->methods.first = meth->next;
		else
		    prevMeth->next = meth->next;
	    }
	    else
		prevMeth = meth;
	}
    }
}

void
ClassDef::write(SbBool)
{
    curClass = this;

    Writer::writePreface(name);

    if (Writer::isForBook())
	writeForBook();
    else
	writeForNonBook();
}

void
ClassDef::writeForBook()
{
    ClassDef	*cd;
    SbBool	anyItem;

    ////////////////////////////////////////////////////////
    //
    // NAME section
    //

    writeName();

    ////////////////////////////////////////////////////////
    //
    // INHERITS FROM section
    //

    Writer::startSection("INHERITS FROM");
    inherits.write();

    ////////////////////////////////////////////////////////
    //
    // DESCRIPTION section
    //

    Writer::startSection("DESCRIPTION");
    Writer::writeBracketed(longDescription);

    ////////////////////////////////////////////////////////
    //
    // FIELDS section
    //

    anyItem = FALSE;
    for (cd = this; cd != NULL; cd = cd->parentClass) {
	if (! cd->fields.isEmpty()) {
	    anyItem = TRUE;
	    break;
	}
    }
    if (anyItem) {
	Writer::startSection("FIELDS");
	if (! fields.isEmpty())
	    fields.write();
	for (cd = parentClass; cd != NULL; cd = cd->parentClass)
	    if (! cd->fields.isEmpty())
		cd->writeFromClass(&cd->fields, "Fields", TRUE);
    }

    ////////////////////////////////////////////////////////
    //
    // INPUTS section
    //

    anyItem = FALSE;
    for (cd = this; cd != NULL; cd = cd->parentClass) {
	if (! cd->inputs.isEmpty()) {
	    anyItem = TRUE;
	    break;
	}
    }
    if (anyItem) {
	Writer::startSection("INPUTS");
	if (! inputs.isEmpty())
	    inputs.write();
	for (cd = parentClass; cd != NULL; cd = cd->parentClass)
	    if (! cd->inputs.isEmpty())
		cd->writeFromClass(&cd->inputs, "Inputs", TRUE);
    }

    ////////////////////////////////////////////////////////
    //
    // OUTPUTS section
    //

    anyItem = FALSE;
    for (cd = this; cd != NULL; cd = cd->parentClass) {
	if (! cd->outputs.isEmpty()) {
	    anyItem = TRUE;
	    break;
	}
    }
    if (anyItem) {
	Writer::startSection("OUTPUTS");
	if (! outputs.isEmpty())
	    outputs.write();
	for (cd = parentClass; cd != NULL; cd = cd->parentClass)
	    if (! cd->outputs.isEmpty())
		cd->writeFromClass(&cd->outputs, "Outputs", TRUE);
    }

    ////////////////////////////////////////////////////////
    //
    // PARTS section
    //

    anyItem = FALSE;
    for (cd = this; cd != NULL; cd = cd->parentClass) {
	if (! cd->parts.isEmpty()) {
	    anyItem = TRUE;
	    break;
	}
    }
    if (anyItem) {
	Writer::startSection("PARTS");
	if (! parts.isEmpty())
	    parts.write();
	for (cd = parentClass; cd != NULL; cd = cd->parentClass)
	    if (! cd->parts.isEmpty())
		cd->writeFromClass(&cd->parts, "Parts", TRUE);
    }

    ////////////////////////////////////////////////////////
    //
    // METHODS section
    //

    anyItem = FALSE;
    for (cd = this; cd != NULL; cd = cd->parentClass) {
	if (! cd->methods.isEmpty()) {
	    anyItem = TRUE;
	    break;
	}
    }
    if (anyItem) {
	Writer::startSection(Writer::getLanguage() == C ?
			     "FUNCTIONS" : "METHODS");
	if (! methods.isEmpty())
	    methods.write();
	for (cd = parentClass; cd != NULL; cd = cd->parentClass)
	    if (! cd->methods.isEmpty())
		cd->writeFromClass(&cd->methods,
				   (Writer::getLanguage() == C ?
				    "Functions" : "Methods"),
				   TRUE);
    }

    ////////////////////////////////////////////////////////
    //
    // MACROS section
    //

    anyItem = FALSE;
    for (cd = this; cd != NULL; cd = cd->parentClass) {
	if (! cd->macros.isEmpty()) {
	    anyItem = TRUE;
	    break;
	}
    }
    if (anyItem) {
	Writer::startSection("MACROS");
	if (! macros.isEmpty())
	    macros.write();
	for (cd = parentClass; cd != NULL; cd = cd->parentClass)
	    if (! cd->macros.isEmpty())
		cd->writeFromClass(&cd->macros, "Macros", TRUE);
    }

    ////////////////////////////////////////////////////////
    //
    // ACTION BEHAVIOR section
    //

    if (! actions.isEmpty()) {
	Writer::startSection("ACTION BEHAVIOR");
	actions.write();
    }

    ////////////////////////////////////////////////////////
    //
    // CATALOG PARTS section
    //

    if (! ! catalog) {
	Writer::startSection("CATALOG PARTS");
	Writer::write(catalog);
    }

    ////////////////////////////////////////////////////////
    //
    // FILE FORMAT section
    //

    writeFileFormat();

    ////////////////////////////////////////////////////////
    //
    // Include files
    //

    if (! includes.isEmpty()) {
	Writer::startSection("INCLUDE FILE");

	Writer::changeSize(-1);

	// Include files for this class
	includes.write();

	// XXX we needed to list these things somewhere,
	// so here they are --AE
    	// Typedefs for this class
    	typedefs.write();
	
    	// Definitions for this class
    	defines.write();

    	// Enums for this class
    	enums.write();
    }

    ////////////////////////////////////////////////////////
    //
    // Miscellaneous sections
    //

    if (! miscs.isEmpty())
	miscs.write();

    ////////////////////////////////////////////////////////
    //
    // SEE ALSO section
    //

    if (! alsos.isEmpty()) {
	Writer::startSection("SEE ALSO");
	alsos.write();
    }

    ////////////////////////////////////////////////////////

    Writer::finish();
}

void
ClassDef::writeForNonBook()
{
    ////////////////////////////////////////////////////////
    //
    // NAME section
    //

    writeName();

    ////////////////////////////////////////////////////////
    //
    // INHERITS FROM section
    //

    Writer::startSection("INHERITS FROM");
    inherits.write();

    ////////////////////////////////////////////////////////
    //
    // SYNOPSIS section
    //

    writeSynopsis();

    ////////////////////////////////////////////////////////
    //
    // DESCRIPTION section
    //

    Writer::startSection("DESCRIPTION");
    Writer::writeBracketed(longDescription);

    ////////////////////////////////////////////////////////
    //
    // FIELDS section
    //

    if (! fields.isEmpty()) {
	Writer::startSection("FIELDS");
	fields.write();
    }

    ////////////////////////////////////////////////////////
    //
    // INPUTS section
    //

    if (! inputs.isEmpty()) {
	Writer::startSection("INPUTS");
	inputs.write();
    }

    ////////////////////////////////////////////////////////
    //
    // OUTPUTS section
    //

    if (! outputs.isEmpty()) {
	Writer::startSection("OUTPUTS");
	outputs.write();
    }

    ////////////////////////////////////////////////////////
    //
    // PARTS section
    //

    if (! parts.isEmpty()) {
	Writer::startSection("PARTS");
	parts.write();
    }

    ////////////////////////////////////////////////////////
    //
    // METHODS section
    //

    if (! methods.isEmpty()) {
	Writer::startSection(Writer::getLanguage() == C ?
			     "FUNCTIONS" : "METHODS");
	methods.write();
    }

    ////////////////////////////////////////////////////////
    //
    // MACROS section
    //

    if (! macros.isEmpty()) {
	Writer::startSection("MACROS");
	macros.write();
    }

    ////////////////////////////////////////////////////////
    //
    // ACTION BEHAVIOR section
    //

    if (! actions.isEmpty()) {
	Writer::startSection("ACTION BEHAVIOR");
	actions.write();
    }

    ////////////////////////////////////////////////////////
    //
    // CATALOG PARTS section
    //

    if (! ! catalog) {
	Writer::startSection("CATALOG PARTS");
	Writer::write(catalog);
    }

    ////////////////////////////////////////////////////////
    //
    // DRAGGER RESOURCES section (for draggers)
    //

    if (! draggerResources.isEmpty()) {
	Writer::startSection("DRAGGER RESOURCES");
	draggerResources.write();
    }

    ////////////////////////////////////////////////////////
    //
    // FILE FORMAT section
    //

    writeFileFormat();

    ////////////////////////////////////////////////////////
    //
    // Miscellaneous sections
    //

    if (! miscs.isEmpty())
	miscs.write();

    ////////////////////////////////////////////////////////
    //
    // SEE ALSO section
    //

    if (! alsos.isEmpty()) {
	Writer::startSection("SEE ALSO");
	alsos.write();
    }

    ////////////////////////////////////////////////////////

    Writer::finish();
}

void
ClassDef::writeName()
{
    Writer::startSection("NAME");
    Writer::write(name);
    if (Writer::getLanguage() == C) {
	Writer::write(" (");
	Writer::write(cPrefix);
	Writer::write(") \\(em ");
    }
    else
	Writer::write(" \\(em ");

    Writer::writeBracketed(shortDescription);
}

void
ClassDef::writeSynopsis()
{
    ClassDef *cd;

    Writer::setInSynopsis(TRUE);

    Writer::startSection("SYNOPSIS");
    Writer::changeSize(-1);

    // Include files for this class
    includes.write();

    // Typedefs for this class
    typedefs.write();

    // Definitions for this class
    defines.write();

    // Enums for this class
    enums.write();

    // Fields for this class, then all parent classes
    for (cd = this; cd != NULL; cd = cd->parentClass)
	if (! cd->fields.isEmpty())
	    cd->writeFromClass(&cd->fields, "Fields");

    // Inputs for this class, then all parent classes
    for (cd = this; cd != NULL; cd = cd->parentClass)
	if (! cd->inputs.isEmpty())
	    cd->writeFromClass(&cd->inputs, "Inputs");

    // Outputs for this class, then all parent classes
    for (cd = this; cd != NULL; cd = cd->parentClass)
	if (! cd->outputs.isEmpty())
	    cd->writeFromClass(&cd->outputs, "Outputs");

    // Parts for this class, then all parent classes
    for (cd = this; cd != NULL; cd = cd->parentClass)
	if (! cd->parts.isEmpty())
	    cd->writeFromClass(&cd->parts, "Parts");

    // Methods for this class, then all parent classes
    for (cd = this; cd != NULL; cd = cd->parentClass)
	if (! cd->methods.isEmpty())
	    cd->writeFromClass(&cd->methods,
			       (Writer::getLanguage() == C ?
				"Functions" : "Methods"));

    // Macros for this class, then all parent classes
    for (cd = this; cd != NULL; cd = cd->parentClass)
	if (! cd->macros.isEmpty())
	    cd->writeFromClass(&cd->macros, "Macros");

    Writer::setInSynopsis(FALSE);
}

void
ClassDef::writeFileFormat()
{
    // Paths have a special file format:
    if (type == SoPath::getClassTypeId()) {
	Writer::startSection("FILE FORMAT/DEFAULTS");

	Writer::setFill(FALSE);
	Writer::setFont(Writer::FIXED_ROMAN);
	Writer::write("SoPath {\n");
	Writer::setBaseIndent(0.5);
	Writer::write("[head node]\n"
		      "[number of remaining indices]\n"
		      "[index]\n"
		      "\\&...\n"
		      "[index]\n");
	Writer::setBaseIndent(0.0);
	Writer::write("}\n\n");

	Writer::setFill(TRUE);
	Writer::setFont(Writer::TEXT_ROMAN);
	Writer::write("Note that the indices in a written path are "
		      "adjusted based on the nodes that are actually "
		      "written to a file. Since nodes in the graph "
		      "that have no effect on the path (such as some "
		      "separator nodes) are not written, the siblings "
		      "of such nodes must undergo index adjustment "
		      "when written. The actual nodes in the graph "
		      "remain unchanged.\n");
    }

    else if (hasFileFormat) {
	Writer::startSection("FILE FORMAT/DEFAULTS");

	if (isAbstract)
	    Writer::write(fileFormat);

	else {
	    Writer::setFill(FALSE);
	    Writer::setFont(Writer::FIXED_ROMAN);
	    // Remove "So" prefix from name
	    Writer::write(name.getString() + 2);
	    Writer::write(" {\n");

	    // Classes with no fields have an empty fileFormat string
	    if (! ! fileFormat) {
		Writer::setBaseIndent(0.5);
		Writer::setTab(fileFormatTabLength +
			       (fileFormatTabLength < 0 ? 0 : 2));
		Writer::write(fileFormat);
		Writer::setBaseIndent(0.0);
	    }

	    Writer::write("}\n");
	}
    }
}

void
ClassDef::writeFromClass(ItemList *list, const char *whats, SbBool runIn)
{
    if (Writer::isForBook()) {
	Writer::setBaseIndent(0.0);
	Writer::setFont(Writer::TEXT_BOLD);
    }
    else {
	Writer::setBaseIndent(0.5);
	Writer::setFont(Writer::TEXT_ROMAN);
    }
    Writer::write(whats);
    Writer::write(" from class ");
    Writer::write(name);
    Writer::write(":\n");
    if (Writer::isForBook()) {
	Writer::setBaseIndent(0.5);
	Writer::space(.25);
    }
    else {
	Writer::setBaseIndent(0.0);
	Writer::space();
    }

    list->write(runIn);
}
