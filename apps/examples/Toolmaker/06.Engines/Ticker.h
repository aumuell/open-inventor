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
 *  chapter 6, example 9.
 *
 *  Header file for "Ticker" engine.
 *------------------------------------------------------------*/

#include <Inventor/engines/SoSubEngine.h>
#include <Inventor/fields/SoSFTime.h>

class Ticker : public SoEngine {

 public:

   SO_ENGINE_HEADER(Ticker);

   // Input:
   SoSFTime        timeIn;

   // Output:
   SoEngineOutput  timeOut;  // (SoSFTime)

   // Initialization
   static void initClass();

   // Constructor
   Ticker();

 private:
   // This saves the number of seconds of the last time this
   // engine was notified of a change
   double          lastSeconds;

   // Destructor
   virtual ~Ticker();

   // This is called when our input value changes - we will use
   // it to see if we need to output a new time value.
   virtual void inputChanged(SoField *whichField);

   // Evaluation method
   virtual void evaluate();
};
