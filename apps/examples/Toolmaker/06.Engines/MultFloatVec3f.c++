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
 *  chapter 6, example 1.
 *
 *  Header file for "MultFloatVec3f" engine.
 *------------------------------------------------------------*/

#include "MultFloatVec3f.h"

SO_ENGINE_SOURCE(MultFloatVec3f);

//
// Initializes the MultFloatVec3f class. This is a one-time
// thing that is done after database initialization and before
// any instance of this class is constructed.
//

void
MultFloatVec3f::initClass()
{
   // Initialize type id variables. The arguments to the macro
   // are: the name of the engine class and the class this is
   // derived from.
   SO_ENGINE_INIT_CLASS(MultFloatVec3f, SoEngine, "Engine");
}

//
// Constructor
//

MultFloatVec3f::MultFloatVec3f()
{
   // Do standard constructor stuff
   SO_ENGINE_CONSTRUCTOR(MultFloatVec3f);

   // Define input fields and their default values
   SO_ENGINE_ADD_INPUT(scalar,  (0.0));
   SO_ENGINE_ADD_INPUT(vector,  (0.0, 0.0, 0.0));

   // Define the output, specifying its type
   SO_ENGINE_ADD_OUTPUT(product, SoSFVec3f);
}

//
// Destructor. Does nothing.
//

MultFloatVec3f::~MultFloatVec3f()
{
}

//
// This is the evaluation routine.
//

void
MultFloatVec3f::evaluate()
{
   // Compute the product of the input fields
   SbVec3f  prod = scalar.getValue() * vector.getValue();

   // "Send" the value to the output. In effect, we are setting
   // the value in all fields to which this output is connected.
   SO_ENGINE_OUTPUT(product, SoSFVec3f, setValue(prod));
}
