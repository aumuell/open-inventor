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
 *  chapter 6, example 4.
 *
 *  Source file for "SoComposeVec2f" engine.
 *------------------------------------------------------------*/

#include "SoComposeVec2f.h"

SO_ENGINE_SOURCE(SoComposeVec2f);

//
// Initializes the SoComposeVec2f class.
//

void
SoComposeVec2f::initClass()
{
   SO_ENGINE_INIT_CLASS(SoComposeVec2f, SoEngine, "Engine");
}

//
// Constructor
//

SoComposeVec2f::SoComposeVec2f()
{
   // Do standard constructor stuff
   SO_ENGINE_CONSTRUCTOR(SoComposeVec2f);

   // Define input fields and their default values
   SO_ENGINE_ADD_INPUT(x,  (0.0));
   SO_ENGINE_ADD_INPUT(y,  (0.0));

   // Define the output, specifying its type
   SO_ENGINE_ADD_OUTPUT(vector, SoMFVec2f);
}

//
// Destructor. Does nothing.
//

SoComposeVec2f::~SoComposeVec2f()
{
}

//
// This is the evaluation routine.
//

void
SoComposeVec2f::evaluate()
{
   // Figure out how many input values we have
   int numX = x.getNum();
   int numY = y.getNum();

   // We will output as many values as there are in the input
   // with the greater number of values
   int numToOutput = (numX > numY ? numX : numY);

   // Make sure that all of the fields connected from the output
   // have enough room for the results. The SoMField::setNum()
   // method does this.
   SO_ENGINE_OUTPUT(vector, SoMFVec2f, setNum(numToOutput));

   // Now output the vectors composed from the input values
   float xValue, yValue;
   int   i;
   for (i = 0; i < numToOutput; i++) {

      // If there are different numbers of values in the input
      // fields, repeat the last value as necessary.
      xValue = (i < numX ? x[i] : x[numX - 1]);
      yValue = (i < numY ? y[i] : y[numY - 1]);

      // Set the vector value in the indexed slot in all
      // connected fields
      SO_ENGINE_OUTPUT(vector, SoMFVec2f,
                       set1Value(i, xValue, yValue));
   }
}
