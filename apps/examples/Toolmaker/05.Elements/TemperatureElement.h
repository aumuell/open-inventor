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
 *  chapter 5, example 1.
 *
 *  Header file for "TemperatureElement" element.
 *------------------------------------------------------------*/

#include <Inventor/elements/SoFloatElement.h>

class TemperatureElement : public SoFloatElement {

   SO_ELEMENT_HEADER(TemperatureElement);

 public:
   // Initializes the TemperatureElement class
   static void    initClass();

   // Initializes element
   virtual void   init(SoState *state);

   // Sets the current temperature in the state to the given
   // temperature (in degrees Fahrenheit)
   // Note: the 2.0/2.0.1 version of this method had one additional 
   // argument, i.e. set(SoState *state, SoNode *node, float temp);
   
   static void    set(SoState *state, float temp);

   // Returns the current temperature from the state
   static float   get(SoState *state);

   // Returns the default temperature
   static float   getDefault()         { return 98.6; }

 private:
   virtual ~TemperatureElement();
};
