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
 *  chapter 6, example 10.
 *
 *  Source file for "Ticker" engine.
 *------------------------------------------------------------*/

#include <Inventor/SoDB.h>
#include "Ticker.h"

SO_ENGINE_SOURCE(Ticker);

//
// Initializes the Ticker class.
//

void
Ticker::initClass()
{
   SO_ENGINE_INIT_CLASS(Ticker, SoEngine, "Engine");
}

//
// Constructor
//

Ticker::Ticker()
{
   // Do standard constructor stuff
   SO_ENGINE_CONSTRUCTOR(Ticker);

   // Define input field and its default value
   SO_ENGINE_ADD_INPUT(timeIn,  (SbTime::zero()));

   // Define the output, specifying its type
   SO_ENGINE_ADD_OUTPUT(timeOut, SoSFTime);

   // Initialize the variable that stores the number of seconds
   // of the last time the input changed
   lastSeconds = -1.0;

   // Connect to the global "realTime" field by default. This
   // way, users do not have to make this connection explicitly,
   // and can change it if they want.
   timeIn.connectFrom(SoDB::getGlobalField("realTime"));
}

//
// Destructor. Does nothing.
//

Ticker::~Ticker()
{
}

//
// This is called when one of our input fields changes. We will
// use it to determine whether to produce any output. Since we
// have only 1 input field, we don't have to see which field
// changed.
//

void
Ticker::inputChanged(SoField *)
{

   // Get the current input time and get rid of any fractional
   // part, using the math library's floor() function
   SbTime   currentTime = timeIn.getValue();
   double   currentSeconds = floor(currentTime.getValue());

   // If the new number of seconds is different from the last
   // one we stored, enable the output. The next time this
   // engine is evaluated, the time will be output.
   if (currentSeconds != lastSeconds) {
      timeOut.enable(TRUE);
      lastSeconds = currentSeconds;
   }

   // Otherwise, make sure the output is disabled, since we
   // don't want to output any values until we cross the next
   // second barrier.
   else
      timeOut.enable(FALSE);
}

//
// This is the evaluation routine.
//

void
Ticker::evaluate()
{
   // Output the current number of seconds
   SbTime   currentTime(lastSeconds);
   SO_ENGINE_OUTPUT(timeOut, SoSFTime, setValue(currentTime));
}
