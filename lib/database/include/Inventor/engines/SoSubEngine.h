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
 |   $Revision: 1.1.1.1 $
 |
 |   Description:
 |	This file defines some macros that implement things common to
 |	subclasses of SoEngine. They may be used to make SoEngine
 |	subclassing easier. In all of the macros, the "className"
 |	parameter refers to the name of the engine subclass.
 |
 |   Defined macros:
 |
 |	Within class header:
 |
 |		SO_ENGINE_HEADER(className)
 |		SO_ENGINE_ABSTRACT_HEADER(className)
 |
 |	Within class source:
 |
 |	    At file scope:
 |
 |		SO_ENGINE_SOURCE(className)
 |		SO_ENGINE_ABSTRACT_SOURCE(className)
 |
 |	    Inside the initClass method:
 |
 |		SO_ENGINE_INIT_CLASS()
 |		SO_ENGINE_INIT_ABSTRACT_CLASS()
 |
 |	    Inside the constructor:
 |
 |		SO_ENGINE_CONSTRUCTOR(className)
 |		SO_ENGINE_ADD_INPUT(inputName, (defaultValue))
 |		SO_ENGINE_ADD_OUTPUT(outputName, outputType)
 |		SO_ENGINE_DEFINE_ENUM_VALUE(enumType, enumValue)
 |		SO_ENGINE_IS_FIRST_INSTANCE()	// a boolean value
 |
 |		   // the following are defined in <fields/So[SM]fEnum.h>:
 |		SO_ENGINE_SET_SF_ENUM_TYPE(fieldName,enumType)
 |		SO_ENGINE_SET_MF_ENUM_TYPE(fieldName,enumType)
 |
 |	    Inside the evaluate method:
 |
 |		SO_ENGINE_OUTPUT(outputName, outputType, method)
 |
 |   Author(s)		: Paul S. Strauss, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_SUB_ENGINE_
#define  _SO_SUB_ENGINE_

#include <Inventor/engines/SoEngine.h>
#include <Inventor/engines/SoOutputData.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/fields/SoFieldData.h>

// C-api: end

///////////////////////
//
// Debugging macros called by other macros in this file
//
#ifdef DEBUG
#define SO__ENGINE_CHECK_INIT(className)				      \
    if (classTypeId == SoType::badType()) {				      \
	SoDebugError::post("SO_ENGINE_CONSTRUCTOR",			      \
			   "Can't construct an engine of type "		      \
			   SO__QUOTE(className),			      \
			   "until initClass() has been called");	      \
	return;								      \
    }

#define SO__ENGINE_CHECK_CONSTRUCT(where)				      \
    do {								      \
	if (inputData == NULL) {					      \
	    SoDebugError::post(where,					      \
			       "Instance not properly constructed.\n"	      \
			       "Did you forget to put "			      \
			       "SO_ENGINE_CONSTRUCTOR()"		      \
			       " in the constructor?");			      \
	    inputData = new SoFieldData(parentInputData ?		      \
					*parentInputData : NULL);	      \
	    outputData = new SoEngineOutputData(parentOutputData ?	      \
						*parentOutputData : NULL);    \
	}								      \
    } while(0)
#else
#define SO__ENGINE_CHECK_INIT(className)	while(0)
#define SO__ENGINE_CHECK_CONSTRUCT(where)	while(0)
#endif

/////////////////////////////////////////////////////////////////////////////
//
// Macros to be called within the class definition header:
//

////////////////////////////////////////////
//
// These defines type-identifier and naming variables and methods that
// all engines and abstract engines must support
//

#define SO_ENGINE_ABSTRACT_HEADER(className)				      \
  public:								      \
  static SoType	getClassTypeId() { return classTypeId; }		      \
    virtual SoType	getTypeId() const; /* Returns type id	*/	      \
  public:								      \
    virtual const SoFieldData *		getFieldData()  const;		      \
    virtual const SoEngineOutputData *	getOutputData() const;		      \
  protected:								      \
    static const SoFieldData **		getInputDataPtr() 		      \
	{ return (const SoFieldData **)&inputData; }			      \
    static const SoEngineOutputData **	getOutputDataPtr()		      \
	{ return (const SoEngineOutputData **)&outputData; }		      \
  private:								      \
    static SoType	classTypeId;	/* Type id		*/	      \
    static SbBool	firstInstance;	/* True for first ctor call */	      \
    static SoFieldData	*inputData;	/* Info on input fields */	      \
    static SoEngineOutputData	*outputData;		/* Info on outputs */ \
    static const SoFieldData	**parentInputData;	/* parent's fields */ \
    static const SoEngineOutputData **parentOutputData

#define SO_ENGINE_HEADER(className)					      \
									      \
    SO_ENGINE_ABSTRACT_HEADER(className);				      \
									      \
  private:								      \
    static void *createInstance()	/* Creates and returns instance */

////////////////////////////////////////////
//
// This declares the static variables defined in SO_ENGINE_HEADER
//

#define SO__ENGINE_ABSTRACT_VARS(className)				      \
    SoType			  className::classTypeId;		      \
    SbBool			  className::firstInstance=TRUE;	      \
    SoEngineOutputData		 *className::outputData;		      \
    SoFieldData *		  className::inputData;			      \
    const SoEngineOutputData	**className::parentOutputData;		      \
    const SoFieldData **	  className::parentInputData

#define SO__ENGINE_VARS(className)					      \
    SO__ENGINE_ABSTRACT_VARS(className)

////////////////////////////////////////////
//
// These implement the methods defined in SO_ENGINE_HEADER
//

#define SO__ENGINE_ABSTRACT_METHODS(className)				      \
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
	return inputData;						      \
    }									      \
									      \
    const SoEngineOutputData *						      \
    className::getOutputData() const					      \
    {									      \
	return outputData;						      \
    }

#define SO__ENGINE_METHODS(className)					      \
									      \
    SO__ENGINE_ABSTRACT_METHODS(className)				      \
									      \
    void *								      \
    className::createInstance()						      \
    {									      \
	return (void *)(new className);					      \
    }

/////////////////////////////////////////////////////////////////////////////
//
// Macros to be called within the source file for a engine subclass:
//

#define SO_ENGINE_SOURCE(className)					      \
    SO__ENGINE_VARS(className);						      \
    SO__ENGINE_METHODS(className)

#define SO_ENGINE_ABSTRACT_SOURCE(className)				      \
    SO__ENGINE_ABSTRACT_VARS(className);				      \
    SO__ENGINE_ABSTRACT_METHODS(className)

///////////////////////////////////////////////////////////
//
//  Internal initialization macros
//

#define SO__ENGINE_INIT_CLASS(className, classPrintName, parentClass)	      \
    do {								      \
	classTypeId =							      \
	    SoType::createType(parentClass::getClassTypeId(),		      \
			       classPrintName,				      \
			       &className::createInstance);		      \
	parentInputData = parentClass::getInputDataPtr();		      \
	parentOutputData = parentClass::getOutputDataPtr();		      \
    } while (0)

#define SO__ENGINE_INIT_ABSTRACT_CLASS(className,classPrintName,parent)	      \
    do {								      \
	classTypeId = SoType::createType(parent::getClassTypeId(),	      \
					 classPrintName);		      \
	parentInputData = parent::getInputDataPtr();			      \
	parentOutputData = parent::getOutputDataPtr();			      \
    } while (0)

////////////////////////////////////////////
//
// This initializes the type identifer variables
// This macro should be called from within initClass(). The parentClass
// argument should be the class that this subclass is derived from.
//

#define SO_ENGINE_INIT_CLASS(className,parentClass,parentPrintClass)	      \
    do {								      \
	classTypeId =							      \
	    SoType::createType(SoType::fromName(parentPrintClass),	      \
			       SO__QUOTE(className),			      \
			       &className::createInstance);		      \
	parentInputData = parentClass::getInputDataPtr();		      \
	parentOutputData = parentClass::getOutputDataPtr();		      \
    } while (0)

#define SO_ENGINE_INIT_ABSTRACT_CLASS(className,parent,parentPrintClass)      \
    do {								      \
	classTypeId = SoType::createType(SoType::fromName(parentPrintClass),  \
					 SO__QUOTE(className));		      \
	parentInputData = parent::getInputDataPtr();			      \
	parentOutputData = parent::getOutputDataPtr();			      \
    } while (0)

/////////////////////////////////////////////////////////////////////////////
//
// Macro to be called within each constructor
//

#define SO_ENGINE_CONSTRUCTOR(className)				      \
    do {								      \
        SO__ENGINE_CHECK_INIT(className);				      \
        if (inputData == NULL) {					      \
            inputData = new SoFieldData(parentInputData ?		      \
					*parentInputData : NULL);	      \
	    outputData = new SoEngineOutputData(parentOutputData ?	      \
						*parentOutputData : NULL);    \
	}								      \
        else								      \
            firstInstance = FALSE;					      \
        isBuiltIn = FALSE;						      \
    } while(0)

////////////////////////////////////////////////
//
// This is a boolean value that can be tested
// in constructors.
//

#define SO_ENGINE_IS_FIRST_INSTANCE()	(firstInstance == TRUE)

////////////////////////////////////////////
//
// This adds the info for an input to the SoFieldData and sets the
// default value for it. The parameters are as follows:
//	inputName:	the name of the input (as a member)
//	defValue:	the default value enclosed in parentheses
//
// For example,
//
//	SO_ENGINE_ADD_INPUT(vector1, (0, 0, 0));
//	SO_ENGINE_ADD_INPUT(triggerTime, (0.0));
//
// adds info about inputs named vector1 and triggerTime with the
// given default values.  The inputs must be public member variables 
// of a type derived from SoField.
//

#define SO_ENGINE_ADD_INPUT(inputName, defValue)			      \
    do {								      \
	SO__ENGINE_CHECK_CONSTRUCT(__FILE__);				      \
	if (firstInstance)						      \
	    inputData->addField(this, SO__QUOTE(inputName),		      \
				&this->inputName);			      \
	this->inputName.setValue defValue;				      \
	this->inputName.setContainer(this);				      \
    } while(0)

////////////////////////////////////////////
//
// This adds the info for an output to the SoEngineOutputData.
// The parameters are as follows:
//	fieldName:	the name of the output (as a member)
//	type:		the type of the output (name of SoField subclass)
//
// For example,
//
//	SO_ENGINE_ADD_OUTPUT(result, SoSFVec3f);
//	SO_ENGINE_ADD_OUTPUT(hour, SoSFInt32);
//
// adds info about outputs named result and int32_t that can be hooked up
// to fields of the given type.
// The outputs must be public member variables of type SoEngineOutput.
//

#define SO_ENGINE_ADD_OUTPUT(outputName, type)				      \
    do {								      \
	SO__ENGINE_CHECK_CONSTRUCT(__FILE__);				      \
	if (firstInstance)						      \
	    outputData->addOutput(this, SO__QUOTE(outputName),		      \
				  &this->outputName,			      \
				  type::getClassTypeId());		      \
	this->outputName.setContainer(this);				      \
    } while(0)

////////////////////////////////////////////
//
// This registers a value of an enum type.
//	enumType:	the name of the enum type
//	enumValue:	the name of a value of that enum type
//
// If a engine defines an enum, each of the enum's values
// should be registered using this macro.  For example:
//
//	[ in MyFunc.h file: ]
//	class MyFunc {
//		...
//		enum Chipmunk { ALVIN, SIMON, THEODORE };
//              ...
//      }
//	
//	[ in constructor MyFunc::MyFunc(): ]
//	SO_ENGINE_DEFINE_ENUM_VALUE(Chipmunk, ALVIN);
//	SO_ENGINE_DEFINE_ENUM_VALUE(Chipmunk, SIMON);
//	SO_ENGINE_DEFINE_ENUM_VALUE(Chipmunk, THEODORE);
//

#define SO_ENGINE_DEFINE_ENUM_VALUE(enumType,enumValue)			      \
    do {								      \
	SO__ENGINE_CHECK_CONSTRUCT(__FILE__);				      \
	if (firstInstance)						      \
	    inputData->addEnumValue(SO__QUOTE(enumType),		      \
				    SO__QUOTE(enumValue), enumValue);	      \
    } while (0)

////////////////////////////////////////////
//
// This takes care of writing the value to all connected outputs.
// This should be called in the evaluate() routine.
//

#define SO_ENGINE_OUTPUT(outputName,type,code)				      \
    do {								      \
	if (outputName.isEnabled()) {					      \
	    for (int _eng_out_i = 0;					      \
		 _eng_out_i < outputName.getNumConnections();		      \
		 _eng_out_i++) {					      \
		type *_eng_out_temp = (type *) outputName[_eng_out_i];	      \
		if (!_eng_out_temp->isReadOnly()) {			      \
		    _eng_out_temp->code;				      \
		}							      \
	    }								      \
	}								      \
    } while(0)

// C-api: begin

#endif /* _SO_SUB_ENGINE_ */
