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
 |      Field converter classes.
 |
 |   Author(s)          : Ronen Barzel, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/engines/SoFieldConverter.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/fields/SoFields.h>
#include "SoFieldConverters.h"

//
// Maximum number of multi-valued field types, and maximum total
// number of field types:
//
#define MAXMFIELDS	1024
#define MAXFIELDS	MAXMFIELDS*2

// Constants for all the field types; used so we can quickly switch()
// on the field type at evaluate() time.
enum TypeConst 
{
    MFBitMask = 1,
    MFBool,
    MFColor,
    MFEnum,
    MFFloat,
    MFInt32,
    MFMatrix,
    MFName,
    MFNode,
    MFPath,
    MFPlane,
    MFRotation,
    MFShort,
    MFString,
    MFTime,
    MFUInt32,
    MFUShort,
    MFVec2f,
    MFVec3f,
    MFVec4f,
    SFBitMask = MAXMFIELDS+1,
    SFBool,
    SFColor,
    SFEnum,
    SFFloat,
    SFInt32,
    SFMatrix,
    SFName,
    SFNode,
    SFPath,
    SFPlane,
    SFRotation,
    SFShort,
    SFString,
    SFTime,
    SFUInt32,
    SFUShort,
    SFVec2f,
    SFVec3f,
    SFVec4f,
    BAD_TYPE
};

//
// Can't use regular SO_ENGINE_SOURCE macro because of the funky
// handling of getFieldData/getEngineOutputData:
//
SO__ENGINE_VARS(SoConvToTrigger);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor; takes type of input
//
// Use: public

SoConvToTrigger::SoConvToTrigger()
//
////////////////////////////////////////////////////////////////////////
{
    SO_ENGINE_CONSTRUCTOR(SoConvToTrigger);
    SO_ENGINE_ADD_OUTPUT(output, SoSFTrigger);
    myInputData = new SoFieldData(inputData);
    input = NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor; cleans up
//
// Use: public

SoConvToTrigger::~SoConvToTrigger()
//
////////////////////////////////////////////////////////////////////////
{
    delete myInputData;
    if (input != NULL) {
	delete input;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Database calls to get correct input field.  Creates field and
//    fieldData on the fly.
//
// Use: public

SoField *
SoConvToTrigger::getInput(SoType type)
//
////////////////////////////////////////////////////////////////////////
{
    input = (SoField *) type.createInstance();
    input->setContainer(this);
    myInputData->addField(this, "input", input);

    return input;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Database calls to get correct output.  Easy, because this always
//    outputs an SoSFTrigger.
//
// Use: public

SoEngineOutput *
SoConvToTrigger::getOutput(SoType)
//
////////////////////////////////////////////////////////////////////////
{
    return &output;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    inputChanged; needs to pull input value
//
// Use: public

void
SoConvToTrigger::inputChanged(SoField *whichInput)
//
////////////////////////////////////////////////////////////////////////
{
    whichInput->evaluate();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Evaluate; just needs to evaluate input field.
//
// Use: public

void
SoConvToTrigger::evaluate()
//
////////////////////////////////////////////////////////////////////////
{
    input->evaluate();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns type id.
//
// Use: public
//
SoType
SoConvToTrigger::getTypeId() const
//
////////////////////////////////////////////////////////////////////////
{
    return classTypeId;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns field data
//
// Use: internal
//
const SoFieldData *
SoConvToTrigger::getFieldData() const 
//
////////////////////////////////////////////////////////////////////////
{
    return myInputData;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns output data
//
// Use: internal
//
const SoEngineOutputData *
SoConvToTrigger::getOutputData() const
//
////////////////////////////////////////////////////////////////////////
{
    return outputData;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Create an instance of the class, using private constructor.
//    Used only by database when reading in from file.
//
// Use: internal
//
void *
SoConvToTrigger::createInstance()
//
////////////////////////////////////////////////////////////////////////
{
    return new SoConvToTrigger;
}

//
// Can't use regular SO_ENGINE_SOURCE macro because of the funky
// handling of getFieldData/getEngineOutputData:
//
SO__ENGINE_VARS(SoBuiltinFieldConverter);

////////////////////////////////////////////////////////////////////////
//
// Description:
//   
//
// Use:

SoBuiltinFieldConverter::SoBuiltinFieldConverter()
//
////////////////////////////////////////////////////////////////////////
{
    SO_ENGINE_CONSTRUCTOR(SoBuiltinFieldConverter);
    myInputData = new SoFieldData(inputData);
    myOutputData = new SoEngineOutputData(outputData);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   
//
// Use:

SoBuiltinFieldConverter::~SoBuiltinFieldConverter()
//
////////////////////////////////////////////////////////////////////////
{
    delete myInputData;
    delete myOutputData;

    if (input != NULL) {
	delete input;
	delete output;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Called by database with the type we're converting to.  The engine
//   output is built on the fly, based on the type.  Also sets the
//   'outType' integer so we can quickly decide what conversion to do
//   at evaluate() time.
//
// Use: internal

SoEngineOutput *
SoBuiltinFieldConverter::getOutput(SoType type)
//
////////////////////////////////////////////////////////////////////////
{
    output = new SoEngineOutput;
    output->setContainer(this);
    myOutputData->addOutput(this, "output", output, type);
    
#define DECIDEOUT(class) \
    (type == SO__CONCAT(So,class)::getClassTypeId()) { \
	outType = class; \
    }
    
    if DECIDEOUT(MFBitMask)
    else if DECIDEOUT(MFBool)
    else if DECIDEOUT(MFColor)
    else if DECIDEOUT(MFEnum)
    else if DECIDEOUT(MFFloat)
    else if DECIDEOUT(MFInt32)
    else if DECIDEOUT(MFMatrix)
    else if DECIDEOUT(MFName)
    else if DECIDEOUT(MFNode)
    else if DECIDEOUT(MFPath)
    else if DECIDEOUT(MFPlane)
    else if DECIDEOUT(MFRotation)
    else if DECIDEOUT(MFShort)
    else if DECIDEOUT(MFString)
    else if DECIDEOUT(MFTime)
    else if DECIDEOUT(MFUInt32)
    else if DECIDEOUT(MFUShort)
    else if DECIDEOUT(MFVec2f)
    else if DECIDEOUT(MFVec3f)
    else if DECIDEOUT(MFVec4f)
    else if DECIDEOUT(SFBitMask)
    else if DECIDEOUT(SFBool)
    else if DECIDEOUT(SFColor)
    else if DECIDEOUT(SFEnum)
    else if DECIDEOUT(SFFloat)
    else if DECIDEOUT(SFInt32)
    else if DECIDEOUT(SFMatrix)
    else if DECIDEOUT(SFName)
    else if DECIDEOUT(SFNode)
    else if DECIDEOUT(SFPath)
    else if DECIDEOUT(SFPlane)
    else if DECIDEOUT(SFRotation)
    else if DECIDEOUT(SFShort)
    else if DECIDEOUT(SFString)
    else if DECIDEOUT(SFTime)
    else if DECIDEOUT(SFUInt32)
    else if DECIDEOUT(SFUShort)
    else if DECIDEOUT(SFVec2f)
    else if DECIDEOUT(SFVec3f)
    else if DECIDEOUT(SFVec4f)
#undef DECIDEOUT
#ifdef DEBUG	
    else {
	SoDebugError::post("(internal) SoBuiltinFieldConverter::getOutput"
		       "no output for type '%s'", type.getName().getString());
    }
#endif    
    return output;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//   Called by database with the type we're converting from.  The input
//   field is built on the fly, based on the type.  Also sets the
//   'inType' integer so we can quickly decide what conversion to do
//   at evaluate() time.
//
// Use: internal

SoField *
SoBuiltinFieldConverter::getInput(SoType type)
//
////////////////////////////////////////////////////////////////////////
{
    input = (SoField *)type.createInstance();
    
#define DECIDEIN(class,defaultValue) \
    (type == SO__CONCAT(So,class)::getClassTypeId()) { \
	inType = class; \
	((SO__CONCAT(So,class) *)input)->setValue defaultValue ; \
    }
    
    if DECIDEIN(MFBitMask,(0))
    else if DECIDEIN(MFBool,(FALSE))
    else if DECIDEIN(MFColor,(0,0,0))
    else if DECIDEIN(MFEnum,(0))
    else if DECIDEIN(MFFloat,(0))
    else if DECIDEIN(MFInt32,(0))
    else if DECIDEIN(MFMatrix,(SbMatrix::identity()))
    else if DECIDEIN(MFName,(""))
    else if DECIDEIN(MFNode,(NULL))
    else if DECIDEIN(MFPath,(NULL))
    else if DECIDEIN(MFPlane,(SbPlane(SbVec3f(0,0,0),0)))
    else if DECIDEIN(MFRotation,(SbRotation()))
    else if DECIDEIN(MFShort,(0))
    else if DECIDEIN(MFString,(""))
    else if DECIDEIN(MFTime,(SbTime::zero()))
    else if DECIDEIN(MFUInt32,(0))
    else if DECIDEIN(MFUShort,(0))
    else if DECIDEIN(MFVec2f,(0,0))
    else if DECIDEIN(MFVec3f,(0,0,0))
    else if DECIDEIN(MFVec4f,(0,0,0,0))
    else if DECIDEIN(SFBitMask,(0))
    else if DECIDEIN(SFBool,(FALSE))
    else if DECIDEIN(SFColor,(0,0,0))
    else if DECIDEIN(SFEnum,(0))
    else if DECIDEIN(SFFloat,(0))
    else if DECIDEIN(SFInt32,(0))
    else if DECIDEIN(SFMatrix,(SbMatrix::identity()))
    else if DECIDEIN(SFName,(""))
    else if DECIDEIN(SFNode,(NULL))
    else if DECIDEIN(SFPath,(NULL))
    else if DECIDEIN(SFPlane,(SbPlane(SbVec3f(0,0,0),0)))
    else if DECIDEIN(SFRotation,(SbRotation()))
    else if DECIDEIN(SFShort,(0))
    else if DECIDEIN(SFString,(""))
    else if DECIDEIN(SFTime,(SbTime::zero()))
    else if DECIDEIN(SFUInt32,(0))
    else if DECIDEIN(SFUShort,(0))
    else if DECIDEIN(SFVec2f,(0,0))
    else if DECIDEIN(SFVec3f,(0,0,0))
    else if DECIDEIN(SFVec4f,(0,0,0,0))
#undef DECIDEIN
#ifdef DEBUG	
    else {
	SoDebugError::post("(internal) SoBuiltinFieldConverter::getInput",
		       "no input for type '%s'", type.getName().getString());
    }
#endif    

    input->setContainer(this);
    myInputData->addField(this, "input", input);

    return input;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Write input to output, with a conversion in between.
//
// Use: private
//

void
SoBuiltinFieldConverter::evaluate()
//
////////////////////////////////////////////////////////////////////////
{
    // For efficiency and to reduce bloat, we don't use the standard
    // SO_ENGINE_OUTPUT macro:

#ifdef DEBUG
    if (input == NULL) {
	SoDebugError::post("(internal) SoBuiltinFieldConverter::evaluate",
			   "NULL input field");
	return;
    }
#endif    

    // We know that our engineOutput cannot be disabled, since nobody
    // but us has access to it. If we are connected from an
    // engineOutput and that engineOutput is disabled, we still want
    // to convert that value - this will happen the first time this
    // engine is evaluated after the output is disabled. After the
    // first time, notification will be blocked, so we don't have to
    // check for a disabled output here.

    for (int i = 0; i < output->getNumConnections(); i++) {
	SoField *outField = (*output)[i];
	if (!outField->isReadOnly())
	    doConversion(outField);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Actually do the conversion, writing to the given field.
//    A massive switch, which is the opposite of object-oriented but
//    which makes the library a lot smaller.
//
// Use: private

void
SoBuiltinFieldConverter::doConversion(SoField *outField)
//
////////////////////////////////////////////////////////////////////////
{
    // Various variables needed by the conversion cases:
    int i;
    SbMatrix matrix;
    SbString string;

    // Combine inType/outType into one integer.
    switch (inType*MAXFIELDS + outType) {

#define CASE(typeIn,typeOut) case typeIn*MAXFIELDS+typeOut

// This macro is for converting the single/multi fields into their
// corresponding multi/single value fields.
// In normal code, it looks like:
// Single to Multi:
//   SoMField->setValue(SoSField->getValue())
// Multi so Single:
//   if (MField->getNum() > 0) SoSField->setValue(SoMField[0])

#define CONV1(type)							    \
      CASE(SO__CONCAT(SF,type),SO__CONCAT(MF,type)):		    \
	((SO__CONCAT(SoMF,type) *)outField)->setValue(			    \
	    ((SO__CONCAT(SoSF,type) *)input)->getValue());		    \
	break;								    \
      CASE(SO__CONCAT(MF,type),SO__CONCAT(SF,type)):		    \
	if (((SoMField *)input)->getNum() > 0)				    \
	    ((SO__CONCAT(SoSF,type) *)outField)->setValue(		    \
		(*(SO__CONCAT(SoMF,type) *)input)[0]);			    \
	break
	
// Cases for all the field types:
	CONV1(BitMask);
	CONV1(Bool);
	CONV1(Color);
	CONV1(Enum);
	CONV1(Float);
	CONV1(Int32);
	CONV1(Matrix);
	CONV1(Name);
	CONV1(Node);
	CONV1(Path);
	CONV1(Plane);
	CONV1(Rotation);
	CONV1(Short);
	CONV1(String);
	CONV1(Time);
	CONV1(UInt32);
	CONV1(UShort);
	CONV1(Vec2f);
	CONV1(Vec3f);
	CONV1(Vec4f);
#undef CONV1
	
//
// Conversions to/from a string for all field types.  The eight cases
// are:
// Single to/from Single string:
// Single to/from Multiple string:
// Multi to/from Single string:
//    input->get(string); outField->set(string);
// Multi to/from Multi string:
//    for (i = 0; i < ((SoMField *)input)->getNum(); i++) {
//       (SoMField *)input->get1(i, string);
//       (SoMField *)outField->set1(i, string);
//
// Note: we must use the SF/MFString->setValue() routines and not just
// plain set() in case there is whitespace in the string, since set()
// takes file format, and in the file format strings with whitespace
// must be quoted.
//
#define CONVSTR(type)							    \
      CASE(SO__CONCAT(SF,type),SFString):				    \
      CASE(SO__CONCAT(MF,type),SFString):				    \
	input->get(string);						    \
	((SoSFString *)outField)->setValue(string);			    \
	break;								    \
      CASE(SO__CONCAT(SF,type),MFString):				    \
	input->get(string);						    \
	((SoMFString *)outField)->set1Value(0,string);			    \
	break;								    \
      CASE(SO__CONCAT(MF,type),MFString):				    \
	for (i = 0; i < ((SoMField *)input)->getNum(); i++) {		    \
	    ((SoMField *)input)->get1(i, string);			    \
	    ((SoMFString *)outField)->set1Value(i, string);		    \
	}								    \
	break;								    \
      CASE(SFString,SO__CONCAT(SF,type)):				    \
      CASE(MFString,SO__CONCAT(SF,type)):				    \
      CASE(SFString,SO__CONCAT(MF,type)):				    \
	input->get(string);						    \
	outField->set(string.getString());				    \
	break;								    \
      CASE(MFString,SO__CONCAT(MF,type)):				    \
	for (i = 0; i < ((SoMField *)input)->getNum(); i++) {		    \
	    ((SoMField *)input)->get1(i, string);			    \
	    ((SoMField *)outField)->set1(i, string.getString());	    \
	}								    \
	break
	
// All types except string:
	CONVSTR(BitMask);
	CONVSTR(Bool);
	CONVSTR(Color);
	CONVSTR(Enum);
	CONVSTR(Float);
	CONVSTR(Int32);
	CONVSTR(Matrix);
	CONVSTR(Name);
	CONVSTR(Node);
	CONVSTR(Path);
	CONVSTR(Plane);
	CONVSTR(Rotation);
	CONVSTR(Short);
	CONVSTR(UInt32);
	CONVSTR(UShort);
	CONVSTR(Vec2f);
	CONVSTR(Vec3f);
	CONVSTR(Vec4f);
#undef CONVSTR

// Special case for time to string; if the time is great enough,
// format as a date:
      CASE(SFTime,SFString):
      {
	  SbTime t = ((SoSFTime *)input)->getValue();
	  if (t.getValue() > 3.15e7) string = t.formatDate();
	  else string = t.format();
	  ((SoSFString *)outField)->setValue(string);
      }
      break;
      CASE(SFTime,MFString):
      {
	  SbTime t = ((SoSFTime *)input)->getValue();
	  if (t.getValue() > 3.15e7) string = t.formatDate();
	  else string = t.format();
	  ((SoMFString *)outField)->set1Value(0, string);
      }
      break;
      CASE(MFTime,SFString):
      {
	  SbTime t = (*((SoMFTime *)input))[0];
	  if (t.getValue() > 3.15e7) string = t.formatDate();
	  else string = t.format();
	  ((SoSFString *)outField)->setValue(string);
      }
      break;
      CASE(MFTime,MFString):
      {
	  for (i = 0; i < ((SoMField *)input)->getNum(); i++) {
	      SbTime t = (*((SoMFTime *)input))[i];
	      if (t.getValue() > 3.15e7) string = t.formatDate();
	      else string = t.format();
	      ((SoMFString *)outField)->set1Value(i, string);
	  }
      }
      break;
      CASE(SFString,SFTime):
      CASE(MFString,SFTime):
      CASE(SFString,MFTime):
	input->get(string);
	outField->set(string.getString());
	break;
      CASE(MFString,MFTime):
	for (i = 0; i < ((SoMField *)input)->getNum(); i++) {
	    ((SoMField *)input)->get1(i, string);
	    ((SoMField *)outField)->set1(i, string.getString());
	}
      break;
      

// This macro will do most of the conversions, relying on the C++
// built-in type conversions.  It does all eight combinations of
// single/multi to single/multi conversions for two types that are
// different.  HALF_CONV does the conversions one-way, CONV does them
// both ways:
// Single to single: 
//   SoSField->setValue(SoSField->getValue());
// Multi to single:
//   if (SoMField->getNum() > 0) SoSField->setValue(SoMField[0])
// Single to multi:
//   SoMField->setValue(SoSField->getValue())
// Multi to multi:
//   for (i = 0; i < SoMField->getNum(); i++) {
//      SoMField->set1Value(i, SoMfield[i]);
//   }
//
#define HALF_CONV(typeIn,typeOut,valTypeOut)				      \
      CASE(SO__CONCAT(SF,typeIn),SO__CONCAT(SF,typeOut)):		      \
	((SO__CONCAT(SoSF,typeOut) *)outField)->setValue((valTypeOut)	      \
	    ((SO__CONCAT(SoSF,typeIn) *)input)->getValue());		      \
	break;								      \
      CASE(SO__CONCAT(MF,typeIn),SO__CONCAT(SF,typeOut)):		      \
	if (((SoMField *)input)->getNum() > 0)				      \
	    ((SO__CONCAT(SoSF,typeOut) *)outField)->setValue((valTypeOut)     \
		(*(SO__CONCAT(SoMF,typeIn) *)input)[0]);		      \
	break;								      \
      CASE(SO__CONCAT(SF,typeIn),SO__CONCAT(MF,typeOut)):		      \
	((SO__CONCAT(SoMF,typeOut) *)outField)->setValue((valTypeOut)	      \
	    ((SO__CONCAT(SoSF,typeIn) *)input)->getValue());		      \
	break;								      \
      CASE(SO__CONCAT(MF,typeIn),SO__CONCAT(MF,typeOut)):		      \
	for (i = 0; i < ((SoMField *)input)->getNum(); i++) {		      \
	    ((SO__CONCAT(SoMF,typeOut) *)outField)->set1Value(i,	      \
		(valTypeOut) (*(SO__CONCAT(SoMF,typeIn) *)input)[i]);	      \
	}								      \
	break

#define CONV(type1,valType1,type2,valType2)				      \
    HALF_CONV(type1,type2,valType2);					      \
    HALF_CONV(type2,type1,valType1)
	

// Simple conversions for most fields:

	CONV(Bool,SbBool,Float,float);
	CONV(Bool,SbBool,Int32,int32_t);
	CONV(Bool,SbBool,Short,short);
	CONV(Bool,SbBool,UInt32,uint32_t);
	CONV(Bool,SbBool,UShort,unsigned short);

	CONV(Color,const SbColor &,Vec3f,const SbVec3f &);

	CONV(Float,float,Int32,int32_t);
	CONV(Float,float,Short,short);
	CONV(Float,float,UInt32,uint32_t);
	CONV(Float,float,UShort,unsigned short);

	CONV(Int32,int32_t,Short,short);
	CONV(Int32,int32_t,UInt32,uint32_t);
	CONV(Int32,int32_t,UShort,unsigned short);

	CONV(Short,short,UInt32,uint32_t);
	CONV(Short,short,UShort,unsigned short);

	CONV(UInt32,uint32_t,UShort,unsigned short);

// Some wacky oddball conversions that we have to special-case:

// Float to time can be handled by regular code because SbTime has a
// constructor that takes a float, but time to float needs to be
// special-cased:
      HALF_CONV(Float, Time, float);

      CASE(SFTime, SFFloat):
	((SoSFFloat *)outField)->setValue(
	    ((SoSFTime *)input)->getValue().getValue());
	break;
      CASE(SFTime, MFFloat):
	((SoMFFloat *)outField)->setValue(
	    ((SoSFTime *)input)->getValue().getValue());
	break;
      CASE(MFTime, SFFloat):
	((SoSFFloat *)outField)->setValue(
	    (*(SoMFTime *)input)[0].getValue());
	break;
      CASE(MFTime, MFFloat):
	for (i = 0; i < ((SoMFTime *)input)->getNum(); i++) {
	    ((SoMFFloat *)outField)->set1Value(i,
		(*(SoMFTime *)input)[i].getValue());
	}
	break;
	    
      CASE(SFMatrix, SFRotation):
        ((SoSFRotation *) outField)->setValue(
	    SbRotation(((SoSFMatrix *) input)->getValue()));
	break;
      CASE(SFMatrix, MFRotation):
        ((SoMFRotation *) outField)->setValue(
	    SbRotation(((SoSFMatrix *) input)->getValue()));
	break;
      CASE(MFMatrix, SFRotation):
        ((SoSFRotation *) outField)->setValue(
	    SbRotation((* (SoMFMatrix *) input)[0]));
        break;
      CASE(MFMatrix, MFRotation):
	for (i = 0; i < ((SoMFMatrix *)input)->getNum(); i++) {
	    ((SoMFRotation *)outField)->set1Value(i,
		SbRotation((* (SoMFMatrix *) input)[i]));
	}
        break;

      CASE(SFRotation, SFMatrix):
	matrix.setRotate(((SoSFRotation *)input)->getValue());
	((SoSFMatrix *)outField)->setValue(matrix);
	break;
      CASE(SFRotation, MFMatrix):
	matrix.setRotate(((SoSFRotation *)input)->getValue());
	((SoSFMatrix *)outField)->setValue(matrix);
	break;
      CASE(MFRotation, SFMatrix):
	matrix.setRotate((*(SoMFRotation *)input)[0]);
	((SoSFMatrix *)outField)->setValue(matrix);
	break;
      CASE(MFRotation, MFMatrix):
	for (i = 0; i < ((SoMFRotation *)input)->getNum(); i++) {
	    matrix.setRotate((*(SoMFRotation *)input)[i]);
	    ((SoMFMatrix *)outField)->set1Value(i, matrix);
	}
	break;

      default:
#ifdef DEBUG
        SoDebugError::post("SoBuiltinFieldConverter::doConversion",
			   "Can't convert type %d to type %d\n",
			   inType, outType);
#endif
        break;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns type id.
//
// Use: public
//
SoType
SoBuiltinFieldConverter::getTypeId() const
//
////////////////////////////////////////////////////////////////////////
{
    return classTypeId;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns field data
//
// Use: internal
//
const SoFieldData *
SoBuiltinFieldConverter::getFieldData() const 
//
////////////////////////////////////////////////////////////////////////
{
    return myInputData;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns output data
//
// Use: internal
//
const SoEngineOutputData *
SoBuiltinFieldConverter::getOutputData() const
//
////////////////////////////////////////////////////////////////////////
{
    return myOutputData;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Create an instance of the class, using private constructor.
//    Used only by database when reading in from file.
//
// Use: internal
//
void *
SoBuiltinFieldConverter::createInstance()
//
////////////////////////////////////////////////////////////////////////
{
    return new SoBuiltinFieldConverter;
}
