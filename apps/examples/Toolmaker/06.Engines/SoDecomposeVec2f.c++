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
 *  chapter 6, example 6.
 *
 *  Source file for "SoDecomposeVec2f" engine.
 *------------------------------------------------------------*/

#include "SoDecomposeVec2f.h"

SO_ENGINE_SOURCE(SoDecomposeVec2f);

//
// Initializes the SoDecomposeVec2f class.
//

void
SoDecomposeVec2f::initClass()
{
   SO_ENGINE_INIT_CLASS(SoDecomposeVec2f, SoEngine, "Engine");
}

//
// Constructor
//

SoDecomposeVec2f::SoDecomposeVec2f()
{
   // Do standard constructor stuff
   SO_ENGINE_CONSTRUCTOR(SoDecomposeVec2f);

   // Define input field and its default value
   SO_ENGINE_ADD_INPUT(vector,  (0.0, 0.0));

   // Define the outputs, specifying their types
   SO_ENGINE_ADD_OUTPUT(x, SoMFFloat);
   SO_ENGINE_ADD_OUTPUT(y, SoMFFloat);
}

//
// Destructor. Does nothing.
//

SoDecomposeVec2f::~SoDecomposeVec2f()
{
}

//
// This is the evaluation routine.
//

void
SoDecomposeVec2f::evaluate()
{
   // Figure out how many input values we have
   int numToOutput = vector.getNum();

   // Make sure that all of the fields connected from the
   // outputs have enough room for the results. The
   // SoMField::setNum() method does this.
   SO_ENGINE_OUTPUT(x, SoMFFloat, setNum(numToOutput));
   SO_ENGINE_OUTPUT(y, SoMFFloat, setNum(numToOutput));

   // Now output the values extracted from the input vectors
   for (int i = 0; i < numToOutput; i++) {
      SO_ENGINE_OUTPUT(x, SoMFFloat,
                       set1Value(i, vector[i][0]));
      SO_ENGINE_OUTPUT(y, SoMFFloat,
                       set1Value(i, vector[i][1]));
   }
}
