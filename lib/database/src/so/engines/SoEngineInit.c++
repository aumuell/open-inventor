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
 |   Class:
 |	initClass code for all engine classes
 |
 |   Author(s)		: Ronen Barzel, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/engines/SoEngines.h>
#include <SoUnknownEngine.h>
#include "SoFieldConverters.h"

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes ALL Inventor engine classes.
//
// Use: internal

void
SoEngine::initClasses()
//
////////////////////////////////////////////////////////////////////////
{
    //
    // base class
    //
    SoEngine::initClass();

    //
    // collections of engines init'd by their own base class method
    //
    SoInterpolate::initClasses();

    // Field converters
    SoFieldConverter::initClass();  // Must come first
    SoBuiltinFieldConverter::initClass();
    SoConvToTrigger::initClass();

    //
    // composers/decomposers
    //
    SoComposeMatrix::initClass();
    SoComposeRotation::initClass();
    SoComposeRotationFromTo::initClass();
    SoComposeVec2f::initClass();
    SoComposeVec3f::initClass();
    SoComposeVec4f::initClass();
    SoDecomposeMatrix::initClass();
    SoDecomposeRotation::initClass();
    SoDecomposeVec2f::initClass();
    SoDecomposeVec3f::initClass();
    SoDecomposeVec4f::initClass();

    // miscellaneous engines
    SoBoolOperation::initClass();
    SoCalculator::initClass();
    SoComputeBoundingBox::initClass();
    SoConcatenate::initClass();
    SoCounter::initClass();
    SoElapsedTime::initClass();
    SoGate::initClass();
    SoOnOff::initClass();
    SoOneShot::initClass();
    SoSelectOne::initClass();
    SoTimeCounter::initClass();
    SoTransformVec3f::initClass();
    SoTriggerAny::initClass();
    SoUnknownEngine::initClass();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoBoolOperation class.
//
// Use: internal

void
SoBoolOperation::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__ENGINE_INIT_CLASS(SoBoolOperation, "BoolOperation", SoEngine);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoCalculator class.
//
// Use: internal

void
SoCalculator::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__ENGINE_INIT_CLASS(SoCalculator, "Calculator", SoEngine);
}

//////////////////
//
// Utility macro defines basic source for composition/decomposition
// engines
//
#define SO_COMPOSE__INIT(Name,String)			\
    void Name::initClass() {				\
	SO__ENGINE_INIT_CLASS(Name,String,SoEngine);	\
    }
SO_COMPOSE__INIT(SoComposeVec2f,"ComposeVec2f");
SO_COMPOSE__INIT(SoComposeVec3f,"ComposeVec3f");
SO_COMPOSE__INIT(SoComposeVec4f,"ComposeVec4f");
SO_COMPOSE__INIT(SoDecomposeVec2f,"DecomposeVec2f");
SO_COMPOSE__INIT(SoDecomposeVec3f,"DecomposeVec3f");
SO_COMPOSE__INIT(SoDecomposeVec4f,"DecomposeVec4f");
SO_COMPOSE__INIT(SoComposeRotation,"ComposeRotation");
SO_COMPOSE__INIT(SoComposeRotationFromTo,"ComposeRotationFromTo");
SO_COMPOSE__INIT(SoDecomposeRotation,"DecomposeRotation");
SO_COMPOSE__INIT(SoComposeMatrix,"ComposeMatrix");
SO_COMPOSE__INIT(SoDecomposeMatrix,"DecomposeMatrix");
#undef SO_COMPOSE__INIT

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoComputeBoundingBox class.
//
// Use: internal

void
SoComputeBoundingBox::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__ENGINE_INIT_CLASS(SoComputeBoundingBox,
			  "ComputeBoundingBox", SoEngine);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoConcatenate class.
//
// Use: internal

void
SoConcatenate::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__ENGINE_INIT_CLASS(SoConcatenate, "Concatenate",
				       SoEngine);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoCounter class.
//
// Use: internal

void
SoCounter::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__ENGINE_INIT_CLASS(SoCounter, "Counter", SoEngine);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoElapsedTime class.
//
// Use: internal

void
SoElapsedTime::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__ENGINE_INIT_CLASS(SoElapsedTime, "ElapsedTime", SoEngine);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the base SoEngine class.  Called by
//    SoDB::init().
//
// Use: internal

void
SoEngine::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    // Allocate a new engine type id
    // No real parent id
    classTypeId = SoType::createType(SoFieldContainer::getClassTypeId(),
	SbName("Engine"), 0, 0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoFieldConverter class.
//
// Use: internal

void
SoFieldConverter::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__ENGINE_INIT_ABSTRACT_CLASS(SoFieldConverter, "FieldConverter",
				       SoEngine);
}

void
SoConvToTrigger::initClass()
{
    SO__ENGINE_INIT_CLASS(SoConvToTrigger,"SoConvToTrigger",SoFieldConverter);
    classTypeId.makeInternal();
    SoType trigType = SoSFTrigger::getClassTypeId();
    SoType conv = SoConvToTrigger::getClassTypeId();

    // Any field type can be connected to a trigger, so just register
    // the converter on all field classes
    SoTypeList	fieldTypes;
    int		numFieldTypes;
    numFieldTypes = SoType::getAllDerivedFrom(SoField::getClassTypeId(),
					      fieldTypes);
    for (int i = 0; i < numFieldTypes; i++)
	if (fieldTypes[i] != trigType)
	    SoDB::addConverter(fieldTypes[i], trigType, conv);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoGate class.
//
// Use: internal

void
SoGate::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__ENGINE_INIT_CLASS(SoGate, "Gate", SoEngine);
}

////////////////////////////////////////////////////////////////////////
//
//
// Description:
//    This initializes the SoInterpolate class.
//
// Use: internal

void
SoInterpolate::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__ENGINE_INIT_ABSTRACT_CLASS(SoInterpolate, "Interpolate",
				       SoEngine);
}

////////////////////////////////////////////////////////////////////////
//
//
// Description:
//    This initializes the builtin SoInterpolate classes.
//
// Use: internal

void
SoInterpolate::initClasses()
//
////////////////////////////////////////////////////////////////////////
{
    // ... first the base class
    SoInterpolate::initClass();
    // ... then the derived classes
    SoInterpolateFloat::initClass();
    SoInterpolateRotation::initClass();
    SoInterpolateVec2f::initClass();
    SoInterpolateVec3f::initClass();
    SoInterpolateVec4f::initClass();
}

SO_INTERPOLATE_INITCLASS(SoInterpolateFloat, "InterpolateFloat");
SO_INTERPOLATE_INITCLASS(SoInterpolateRotation, "InterpolateRotation");
SO_INTERPOLATE_INITCLASS(SoInterpolateVec2f, "InterpolateVec2f");
SO_INTERPOLATE_INITCLASS(SoInterpolateVec3f, "InterpolateVec3f");
SO_INTERPOLATE_INITCLASS(SoInterpolateVec4f, "InterpolateVec4f"); 

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoOnOff class.
//
// Use: internal

void
SoOnOff::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__ENGINE_INIT_CLASS(SoOnOff, "OnOff", SoEngine);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoOneShot class.
//
// Use: internal

void
SoOneShot::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__ENGINE_INIT_CLASS(SoOneShot, "OneShot", SoEngine);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoSelectOne class.
//
// Use: internal
//
void
SoSelectOne::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__ENGINE_INIT_CLASS(SoSelectOne, "SelectOne", SoEngine);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoTimeCounter class.
//
// Use: internal

void
SoTimeCounter::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__ENGINE_INIT_CLASS(SoTimeCounter, "TimeCounter", SoEngine);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoTransformVec3f class.
//
// Use: internal

void
SoTransformVec3f::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__ENGINE_INIT_CLASS(SoTransformVec3f, "TransformVec3f", SoEngine);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoTriggerAny class.
//
// Use: internal

void
SoTriggerAny::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__ENGINE_INIT_CLASS(SoTriggerAny, "TriggerAny", SoEngine);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoUnknownEngine class.
//
// Use: internal

void
SoUnknownEngine::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__ENGINE_INIT_CLASS(SoUnknownEngine, "UnknownEngine", SoEngine);
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the built-in field converter class.
//
// Use: internal

void
SoBuiltinFieldConverter::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__ENGINE_INIT_CLASS(SoBuiltinFieldConverter, 
            "BuiltinFieldConverter", SoFieldConverter);
    classTypeId.makeInternal();

// Macro for registering the single/multi to multi/single field
// conversions:
#define REG1(type) \
    SoDB::addConverter(SO__CONCAT(SoSF,type)::getClassTypeId(),		    \
		       SO__CONCAT(SoMF,type)::getClassTypeId(),		    \
		       getClassTypeId());				    \
    SoDB::addConverter(SO__CONCAT(SoMF,type)::getClassTypeId(),		    \
		       SO__CONCAT(SoSF,type)::getClassTypeId(),		    \
		       getClassTypeId())
    
// Cases for all the field types:
    REG1(BitMask);
    REG1(Bool);
    REG1(Color);
    REG1(Enum);
    REG1(Float);
    REG1(Int32);
    REG1(Matrix);
    REG1(Name);
    REG1(Node);
    REG1(Path);
    REG1(Plane);
    REG1(Rotation);
    REG1(Short);
    REG1(String);
    REG1(Time);
    REG1(UInt32);
    REG1(UShort);
    REG1(Vec2f);
    REG1(Vec3f);
    REG1(Vec4f);
#undef REG1

// Macro for registering all of the to/from single/multi-valued
// string conversions (eight for each type):
#define REGSTRING(type) \
    SoDB::addConverter(SO__CONCAT(SoSF,type)::getClassTypeId(),		    \
		       SoSFString::getClassTypeId(),			    \
		       getClassTypeId());				    \
    SoDB::addConverter(SO__CONCAT(SoMF,type)::getClassTypeId(),		    \
		       SoSFString::getClassTypeId(),			    \
		       getClassTypeId());				    \
    SoDB::addConverter(SO__CONCAT(SoSF,type)::getClassTypeId(),		    \
		       SoMFString::getClassTypeId(),			    \
		       getClassTypeId());				    \
    SoDB::addConverter(SO__CONCAT(SoMF,type)::getClassTypeId(),		    \
		       SoMFString::getClassTypeId(),			    \
		       getClassTypeId());				    \
    SoDB::addConverter(SoSFString::getClassTypeId(),			    \
		       SO__CONCAT(SoSF,type)::getClassTypeId(),		    \
		       getClassTypeId());				    \
    SoDB::addConverter(SoSFString::getClassTypeId(),			    \
		       SO__CONCAT(SoMF,type)::getClassTypeId(),		    \
		       getClassTypeId());				    \
    SoDB::addConverter(SoMFString::getClassTypeId(),			    \
		       SO__CONCAT(SoSF,type)::getClassTypeId(),		    \
		       getClassTypeId());				    \
    SoDB::addConverter(SoMFString::getClassTypeId(),			    \
		       SO__CONCAT(SoMF,type)::getClassTypeId(),		    \
		       getClassTypeId())

// All types except string:
    REGSTRING(BitMask);
    REGSTRING(Bool);
    REGSTRING(Color);
    REGSTRING(Enum);
    REGSTRING(Float);
    REGSTRING(Int32);
    REGSTRING(Matrix);
    REGSTRING(Name);
    REGSTRING(Node);
    REGSTRING(Path);
    REGSTRING(Plane);
    REGSTRING(Rotation);
    REGSTRING(Short);
    REGSTRING(Time);
    REGSTRING(UInt32);
    REGSTRING(UShort);
    REGSTRING(Vec2f);
    REGSTRING(Vec3f);
    REGSTRING(Vec4f);
#undef REGSTRING

// Macro for other conversions (float to int32_t, etc):

#define REGHALF(type1,type2) \
    SoDB::addConverter(SO__CONCAT(SoSF,type1)::getClassTypeId(),	    \
		       SO__CONCAT(SoSF,type2)::getClassTypeId(),	    \
		       getClassTypeId());				    \
    SoDB::addConverter(SO__CONCAT(SoMF,type1)::getClassTypeId(),	    \
		       SO__CONCAT(SoSF,type2)::getClassTypeId(),	    \
		       getClassTypeId());				    \
    SoDB::addConverter(SO__CONCAT(SoSF,type1)::getClassTypeId(),	    \
		       SO__CONCAT(SoMF,type2)::getClassTypeId(),	    \
		       getClassTypeId());				    \
    SoDB::addConverter(SO__CONCAT(SoMF,type1)::getClassTypeId(),	    \
		       SO__CONCAT(SoMF,type2)::getClassTypeId(),	    \
		       getClassTypeId())

#define REGCONV(type1,type2)						    \
	REGHALF(type1,type2);						    \
	REGHALF(type2,type1)

    REGCONV(Bool,Float);
    REGCONV(Bool,Int32);
    REGCONV(Bool,Short);
    REGCONV(Bool,UInt32);
    REGCONV(Bool,UShort);

    REGCONV(Color, Vec3f);

    REGCONV(Float,Int32);
    REGCONV(Float,Short);
    REGCONV(Float,UInt32);
    REGCONV(Float,UShort);

    REGCONV(Int32,Short);
    REGCONV(Int32,UInt32);
    REGCONV(Int32,UShort);

    REGCONV(Short,UInt32);
    REGCONV(Short,UShort);

    REGCONV(UInt32,UShort);

    REGCONV(Float, Time);

    REGCONV(Matrix, Rotation);
}
