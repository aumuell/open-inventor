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
 *  chapter 6, example 8.
 *
 *  Source file for "FanIn" engine.
 *------------------------------------------------------------*/

#include "FanIn.h"

SO_ENGINE_SOURCE(FanIn);

//
// Initializes the FanIn class.
//

void
FanIn::initClass()
{
   SO_ENGINE_INIT_CLASS(FanIn, SoEngine, "Engine");
}

//
// Constructor
//

FanIn::FanIn()
{
   // Do standard constructor stuff
   SO_ENGINE_CONSTRUCTOR(FanIn);

   // Define input fields and their default values
   SO_ENGINE_ADD_INPUT(input0,  (0.0));
   SO_ENGINE_ADD_INPUT(input1,  (0.0));
   SO_ENGINE_ADD_INPUT(input2,  (0.0));
   SO_ENGINE_ADD_INPUT(input3,  (0.0));

   // Define the output, specifying its type
   SO_ENGINE_ADD_OUTPUT(output, SoSFFloat);

   // Initialize the pointer that indicates which field changed
   // most recently
   lastChangedField = NULL;
}

//
// Destructor. Does nothing.
//

FanIn::~FanIn()
{
}

//
// This is called when one of our input fields changes. We will
// use it to determine which input value to output.
//

void
FanIn::inputChanged(SoField *whichField)
{
   // Save a pointer to the field that changed; cast it to an
   // SoSFFloat, since that's the only input field type we have
   lastChangedField = (SoSFFloat *) whichField;
}

//
// This is the evaluation routine.
//

void
FanIn::evaluate()
{
   // If evaluate() is called, we must have been notified at
   // some point of a change (including a new connection), so
   // our lastChangedField should never be NULL here. Check it
   // anyway, just for completeness
   if (lastChangedField == NULL) {
      fprintf(stderr, "Uh-oh, there's a NULL lastChangedField"
              "in FanIn::evaluate()!\n");
      return;
   }

   // Output the value from the last changed field
   float   value = lastChangedField->getValue();
   SO_ENGINE_OUTPUT(output, SoSFFloat, setValue(value));
}
