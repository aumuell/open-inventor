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
 |	This file defines the abstract SoInterpolate class and
 |	macros used to derive classes from it, as well as the
 |	derived classes themselves.
 |
 |   Macros:
 |	SO_INTERPOLATE_HEADER
 |	SO_INTERPOLATE_SOURCE
 |	SO_INTERPOLATE_INITCLASS
 |
 |   Classes:
 |	
 |      SoInterpolate	(abstract base class)
 |      SoInterpolateFloat
 |      SoInterpolateRotation
 |      SoInterpolateVec2f
 |      SoInterpolateVec3f
 |      SoInterpolateVec4f
 |
 |   Author(s)		: Ronen Barzel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_INTERPOLATE_
#define  _SO_INTERPOLATE_

#include <Inventor/SoDB.h>
#include <Inventor/engines/SoSubEngine.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoMFFloat.h>
#include <Inventor/fields/SoMFRotation.h>
#include <Inventor/fields/SoMFVec2f.h>
#include <Inventor/fields/SoMFVec3f.h>
#include <Inventor/fields/SoMFVec4f.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoInterpolate
//
//  Abstract base class for all interpolater engines. An interpolater
//  engine interpolates linearly between two values, based on "alpha"
//  between 0 and 1.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: abstract
// C-api: prefix=SoInterp
// C-api: public=alpha,output
class SoInterpolate : public SoEngine {

    SO_ENGINE_ABSTRACT_HEADER(SoInterpolate);

  public:
    // derived classes will define "input0" and "input1" fields
    SoSFFloat		alpha;
    SoEngineOutput	output;

  SoINTERNAL public:
    static void		initClass();
    static void		initClasses();	// init all builtin derived classes

  protected:
    SoInterpolate();
    static int max(int a, int b) { return (a > b) ? a : b; }
    static int clamp(int i, int n) { return (i < n) ? i : n-1; }

    ~SoInterpolate();
};

// C-api: end
/////////////////////////////////////////////////////////////////////////////
//
// These macros can be used to easily define interpolater engine
// headers and source.
//
/////////////////////////////////////////////////////////////////////////////

//
// This macro is to be used within the class definition header. It
// declares variables and methods that all interpolaters support
//

#define SO_INTERPOLATE_HEADER(className)		      		\
									\
    SO_ENGINE_HEADER(className);					\
									\
  private:								\
    ~className();							\
    virtual void	evaluate();					\
									\
  SoINTERNAL public:							\
    static void		initClass();					\
									\
  public:								\
    className()

//
// This macro is to be used within the class source. It defines the
// variables and methods declared in SO_INTERPOLATE_HEADER().
// The "interpVal" argument should be an expression that returns
// the interpolation between local variables "v0" and "v1" of type
// "valType", based on local variable "a" which is a float.
//

#define SO_INTERPOLATE_SOURCE(className, type,		 		\
			valType, defaultVal0, defaultVal1, interpVal)	\
									\
SO_ENGINE_SOURCE(className);						\
									\
className::className()							\
{									\
    SO_ENGINE_CONSTRUCTOR(className);					\
    SO_ENGINE_ADD_INPUT(alpha, (0.0));			      		\
    SO_ENGINE_ADD_INPUT(input0, defaultVal0);				\
    SO_ENGINE_ADD_INPUT(input1, defaultVal1);				\
    SO_ENGINE_ADD_OUTPUT(output, type);			      		\
    isBuiltIn = TRUE;							\
}									\
									\
className::~className()							\
{									\
}									\
									\
void									\
className::evaluate()							\
{									\
    int n0 = input0.getNum();						\
    int n1 = input1.getNum();						\
    float a = alpha.getValue();						\
    for (int i=max(n0,n1)-1; i>=0; i--) {				\
	valType v0 = input0[clamp(i,n0)];				\
	valType v1 = input1[clamp(i,n1)];				\
	SO_ENGINE_OUTPUT(output, type, set1Value(i, interpVal));	\
    }									\
}

/////////////////////////////////////////////////////////////////////////////
//
// This macro defines the initClass method for Interpolators.  It is
// separate from the _SOURCE macro so that we can put all of the
// engine's initClass methods near each other to reduce the number of
// pages touched on startup (decreasing startup time and the amount of
// memory used).
//

#define SO_INTERPOLATE_INITCLASS(className, classPrintName)		\
									\
void									\
className::initClass()							\
{									\
    SO__ENGINE_INIT_CLASS(className, classPrintName, SoInterpolate);	\
}
// C-api: begin
/////////////////////////////////////////////////////////////////////////////
//
// Interpolaters for the builtin field types.
//
/////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoInterpFloat
// C-api: public=alpha,output
// C-api: public=input0, input1
class SoInterpolateFloat : public SoInterpolate {
    SO_INTERPOLATE_HEADER(SoInterpolateFloat);
    SoMFFloat		input0;
    SoMFFloat		input1;
};

// C-api: prefix=SoInterpRot
// C-api: public=alpha,output
// C-api: public=input0, input1
class SoInterpolateRotation : public SoInterpolate {
    SO_INTERPOLATE_HEADER(SoInterpolateRotation);
    SoMFRotation	input0;
    SoMFRotation	input1;
};

// C-api: prefix=SoInterpV2f
// C-api: public=alpha,output
// C-api: public=input0, input1
class SoInterpolateVec2f : public SoInterpolate {
    SO_INTERPOLATE_HEADER(SoInterpolateVec2f);
    SoMFVec2f		input0;
    SoMFVec2f		input1;
};

// C-api: prefix=SoInterpV3f
// C-api: public=alpha,output
// C-api: public=input0, input1
class SoInterpolateVec3f : public SoInterpolate {
    SO_INTERPOLATE_HEADER(SoInterpolateVec3f);
    SoMFVec3f		input0;
    SoMFVec3f		input1;
};

// C-api: prefix=SoInterpV4f
// C-api: public=alpha,output
// C-api: public=input0, input1
class SoInterpolateVec4f : public SoInterpolate {
    SO_INTERPOLATE_HEADER(SoInterpolateVec4f);
    SoMFVec4f		input0;
    SoMFVec4f		input1;
};

#endif /* _SO_INTERPOLATE_ */
