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

/*--------------------------------------------------------------
 *  This is an example from the Inventor Toolmaker,
 *  chapter 6, example 12.
 *
 *  Source file for "ConvertSFShortToSFFloat" field converter
 *  engine.
 *------------------------------------------------------------*/

#include <Inventor/SoDB.h>
#include "ConvertSFShortToSFFloat.h"

SO_ENGINE_SOURCE(ConvertSFShortToSFFloat);

//
// Initializes the ConvertSFShortToSFFloat class.
//

void
ConvertSFShortToSFFloat::initClass()
{
   SO_ENGINE_INIT_CLASS(ConvertSFShortToSFFloat,
                        SoFieldConverter, "FieldConverter");

   // Register this converter's type with the Inventor database
   // to convert from a field (or engine output) of type
   // SoSFShort to a field of type SoSFFloat
   SoDB::addConverter(SoSFShort::getClassTypeId(),
                      SoSFFloat::getClassTypeId(),
                      getClassTypeId());
}

//
// Constructor
//

ConvertSFShortToSFFloat::ConvertSFShortToSFFloat()
{
   // Do standard constructor stuff
   SO_ENGINE_CONSTRUCTOR(ConvertSFShortToSFFloat);

   // Define input field and its default value
   SO_ENGINE_ADD_INPUT(input,  (0));

   // Define the output, specifying its type
   SO_ENGINE_ADD_OUTPUT(output, SoSFFloat);
}

//
// Destructor. Does nothing.
//

ConvertSFShortToSFFloat::~ConvertSFShortToSFFloat()
{
}

//
// This is the evaluation routine.
//

void
ConvertSFShortToSFFloat::evaluate()
{
   // Get the input value as a short, convert it to a float, and
   // output it
   float    value = (float) input.getValue();
   SO_ENGINE_OUTPUT(output, SoSFFloat, setValue(value));
}

//
// This returns the input field for the given type. Since we
// have only one input field, we don't have to check the type.
//

SoField *
ConvertSFShortToSFFloat::getInput(SoType)
{
   return &input;
}

//
// This does the same for the output.
//

SoEngineOutput *
ConvertSFShortToSFFloat::getOutput(SoType)
{
   return &output;
}
