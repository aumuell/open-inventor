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

//  -*- C++ -*-

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Description:
 |	This file defines some macros that implement things common to
 |	many subclasses of SoNode. They may be used to make SoNode
 |	subclassing easier. In all of the macros, the "className"
 |	parameter refers to the name of the node subclass.
 |
 |	Methods containing the word "ABSTRACT" are to be used for
 |	abstract subclasses in place of the corresponding regular
 |	macro.  Note that abstract classes may not have fields (you
 |	cannot call ADD_FIELD in their constructor).
 |
 |   Defined macros:
 |
 |	Within class header:
 |
 |		SO_NODE_HEADER(className)
 |		SO_NODE_ABSTRACT_HEADER(className)
 |
 |	Within class source:
 |
 |	    At file scope:
 |
 |		SO_NODE_SOURCE(className)
 |		SO_NODE_ABSTRACT_SOURCE(className)
 |
 |	    Class initialization (initClass):
 |
 |		SO_NODE_INIT_CLASS()
 |		SO_NODE_INIT_ABSTRACT_CLASS()
 |
 |	    Constructor initialization:
 |
 |		SO_NODE_CONSTRUCTOR(className)
 |		SO_NODE_ADD_FIELD(fieldMember,(defaultValue))
 |		SO_NODE_DEFINE_ENUM_VALUE(enumType,enumValue)
 |		SO_NODE_IS_FIRST_INSTANCE()	// a boolean value
 |
 |		   // the following are defined in <fields/So[SM]fEnum.h>:
 |		SO_NODE_SET_SF_ENUM_TYPE(fieldName,enumType)
 |		SO_NODE_SET_MF_ENUM_TYPE(fieldName,enumType)
 |
 |   Author(s)		: Paul S. Strauss, Gavin Bell, Ronen Barzel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_SUB_NODE_
#define  _SO_SUB_NODE_

#include <Inventor/errors/SoDebugError.h>
#include <Inventor/fields/SoFieldData.h>
#include <Inventor/nodes/SoNode.h>

// C-api: end

//  *** note: many of the macros use the "do { ... } while(0)"
//  ***       hack to define multiline blocks as a single statement
//  ***       of code that can be used anywhere and ended with a semicolon

///////////////////////////////////////////////////////////
//
//  Debugging macros (used by other macros in this file)
//
#ifdef DEBUG
#define SO__NODE_CHECK_INIT(className)					      \
do {									      \
    if (classTypeId == SoType::badType()) {				      \
	SoDebugError::post("SO_NODE_CONSTRUCTOR",			      \
			   "Can't construct a node of type "		      \
			   SO__QUOTE(className)				      \
			   " until initClass() has been called");	      \
	return;								      \
    }									      \
} while (0)

#define SO__NODE_CHECK_CONSTRUCT(where)					      \
do {									      \
    if (fieldData == NULL) {						      \
	SoDebugError::post(where,					      \
			   "Instance not properly constructed.\n"	      \
			   "Did you forget to put SO_NODE_CONSTRUCTOR()"      \
			   " in the constructor?");			      \
	fieldData = new							      \
	    SoFieldData(parentFieldData ? *parentFieldData : NULL);	      \
    }									      \
} while(0)

#else
#define SO__NODE_CHECK_INIT(className)	while(0)
#define SO__NODE_CHECK_CONSTRUCT(where)	while(0)
#endif

/////////////////////////////////////////////////////////////////////////////
//
// Macros to be called within the class definition header for a node
// subclass:
//

////////////////////////////////////////////
//
// These defines type-identifier and naming variables and methods that
// all subclasses and abstract subclasses must support.
//

#define SO_NODE_ABSTRACT_HEADER(className)				      \
  public:								      \
    static SoType	getClassTypeId()	/* Returns class type id */   \
				    { return classTypeId; }		      \
    virtual SoType	getTypeId() const;	/* Returns type id	*/    \
  protected:								      \
    virtual const SoFieldData	*getFieldData() const;			      \
    static const SoFieldData **getFieldDataPtr()			      \
	{ return (const SoFieldData **)&fieldData; }			      \
  private:								      \
    static SoType	classTypeId;		/* Type id		*/    \
    static SbBool	firstInstance; /* true until 2nd c'tor call */	      \
    static SoFieldData		*fieldData;				      \
    static const SoFieldData	**parentFieldData

//
// Non-abstract classes have everything abstract classes have, plus a way
// to create an instance
//
#define SO_NODE_HEADER(className)					      \
    SO_NODE_ABSTRACT_HEADER(className);					      \
    static void *createInstance()

/////////////////////////////////////////////////////////////////////////////
//
// Macros to be called within the source file for a node subclass:
//

////////////////////////////////////////////
//
// This declares the static variables defined in SO_NODE_HEADER
// or SO_NODE_ABSTRACT_HEADER.
//

#define SO__NODE_ABSTRACT_VARS(className)				      \
    SoType		className::classTypeId;				      \
    SoFieldData	       *className::fieldData;				      \
    const SoFieldData **className::parentFieldData;			      \
    SbBool		className::firstInstance = TRUE

//
// Non-abstract nodes have all the stuff abstract nodes do
//
#define SO__NODE_VARS(className)					      \
    SO__NODE_ABSTRACT_VARS(className)

////////////////////////////////////////////
//
// Methods on the abstract type
//

#define SO__NODE_ABSTRACT_METHODS(className)				      \
									      \
    SoType								      \
    className::getTypeId() const					      \
    {									      \
	return classTypeId;						      \
    }									      \
									      \
    const SoFieldData *							      \
    className::getFieldData() const					      \
    {									      \
	SO__NODE_CHECK_CONSTRUCT(SO__QUOTE(className));			      \
	return fieldData;						      \
    }

////////////////////////////////////////////
//
// These implement the methods defined in SO_NODE_HEADER or
// SO_NODE_ABSTRACT_HEADER.
//

#define SO__NODE_METHODS(className)					      \
									      \
    SO__NODE_ABSTRACT_METHODS(className)				      \
									      \
    void *								      \
    className::createInstance()						      \
    {									      \
	return (void *)(new className);					      \
    }


/////////////////////////////////////////////
//
// These include all the definitions required
// at file scope
//

#define	SO_NODE_SOURCE(className)					      \
    SO__NODE_VARS(className);						      \
    SO__NODE_METHODS(className)

#define	SO_NODE_ABSTRACT_SOURCE(className)				      \
    SO__NODE_ABSTRACT_VARS(className);					      \
    SO__NODE_ABSTRACT_METHODS(className)


///////////////////////////////////////////////////////////
//
//  Internal initialization macros
//

#define SO__NODE_INIT_CLASS(className,classPrintName,parentClass)	      \
do {									      \
    classTypeId =							      \
        SoType::createType(parentClass::getClassTypeId(),		      \
		   classPrintName,					      \
		   &className::createInstance,				      \
		   SoNode::nextActionMethodIndex++);			      \
    parentFieldData = parentClass::getFieldDataPtr();			      \
} while (0)

#define SO__NODE_INIT_ABSTRACT_CLASS(className,classPrintName,parentClass)    \
do {									      \
    classTypeId =							      \
        SoType::createType(parentClass::getClassTypeId(),		      \
		   classPrintName,					      \
		   NULL,						      \
		   SoNode::nextActionMethodIndex++);			      \
    parentFieldData = parentClass::getFieldDataPtr();			      \
} while (0)

////////////////////////////////////////////
//
// This initializes the type identifer variables defined in
// SO_NODE_HEADER or SO_NODE_ABSTRACT_HEADER. This macro
// should be called from within initClass(). The parentClass argument
// should be the class that this subclass is derived from.
//

#define SO_NODE_INIT_CLASS(className,parentClass,parentPrintClass)			      \
do {									      \
    classTypeId =							      \
        SoType::createType(SoType::fromName(parentPrintClass),		      \
		   SO__QUOTE(className),				      \
		   &className::createInstance,				      \
		   SoNode::nextActionMethodIndex++);			      \
    parentFieldData = parentClass::getFieldDataPtr();			      \
} while (0)

#define SO_NODE_INIT_ABSTRACT_CLASS(className, parentClass, parentPrintClass) \
do {									      \
    classTypeId =							      \
        SoType::createType(SoType::fromName(parentPrintClass),		      \
		   SO__QUOTE(className),				      \
		   NULL,						      \
		   SoNode::nextActionMethodIndex++);			      \
    parentFieldData = parentClass::getFieldDataPtr();			      \
} while (0)

///////////////////////////////////////////////
//
// This is included at the beginning of
// a constructor, to do required initializations
//

#define SO_NODE_CONSTRUCTOR(className)					      \
do {									      \
    SO__NODE_CHECK_INIT(className);					      \
    if (fieldData == NULL)						      \
	fieldData = new SoFieldData(					      \
	    parentFieldData ? *parentFieldData : NULL);			      \
    else								      \
	firstInstance = FALSE;						      \
    isBuiltIn = FALSE;							      \
} while(0)

////////////////////////////////////////////////
//
// This is a boolean value that can be tested
// in constructors.
//

#define SO_NODE_IS_FIRST_INSTANCE()					      \
    (firstInstance == TRUE)


////////////////////////////////////////////
//
// This adds the info for a field to the SoFieldData and sets the
// default value for it. The parameters are as follows:
//	fieldName:	the name of the field (as a member)
//	defValue:	the default value enclosed in parentheses
//
// For example,
//
//	SO_NODE_ADD_FIELD(ambientColor, (0.2, 0.2, 0.2));
//	SO_NODE_ADD_FIELD(shininess,    (0.0));
//
// adds info about fields named ambientColor and shininess with the
// given default values.
//

#define SO_NODE_ADD_FIELD(fieldName,defValue)				      \
    do {								      \
	SO__NODE_CHECK_CONSTRUCT(__FILE__);				      \
	if (firstInstance)						      \
	    fieldData->addField(this, SO__QUOTE(fieldName),		      \
				&this->fieldName);			      \
	this->fieldName.setValue defValue;				      \
	this->fieldName.setContainer(this);				      \
    } while (0)

////////////////////////////////////////////
//
// This registers a value of an enum type.
//	enumType:	the name of the enum type
//	enumValue:	the name of a value of that enum type
//
// If a node defines an enum, each of the enum's values
// should be registered using this macro.  For example:
//
//	[ in MyNode.h file: ]
//	class MyNode {
//		...
//		enum Chipmunk { ALVIN, SIMON, THEODORE };
//              ...
//      }
//	
//	[ in constructor MyNode::MyNode(): ]
//	SO_NODE_DEFINE_ENUM_VALUE(Chipmunk, ALVIN);
//	SO_NODE_DEFINE_ENUM_VALUE(Chipmunk, SIMON);
//	SO_NODE_DEFINE_ENUM_VALUE(Chipmunk, THEODORE);
//

#define SO_NODE_DEFINE_ENUM_VALUE(enumType,enumValue)			      \
    do {								      \
	SO__NODE_CHECK_CONSTRUCT(__FILE__);				      \
	if (firstInstance)						      \
	    fieldData->addEnumValue(SO__QUOTE(enumType),		      \
				SO__QUOTE(enumValue),			      \
				enumValue);				      \
    } while (0)

// C-api: begin

#endif /* _SO_SUB_NODE_ */

