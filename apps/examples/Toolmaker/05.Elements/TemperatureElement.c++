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
 *  chapter 5, example 2.
 *
 *  Source file for "TemperatureElement" element.
 *------------------------------------------------------------*/

#include "TemperatureElement.h"

SO_ELEMENT_SOURCE(TemperatureElement);

//
// Initializes the TemperatureElement class.
//

void
TemperatureElement::initClass()
{
   SO_ELEMENT_INIT_CLASS(TemperatureElement, SoFloatElement);
}

//
// Destructor
//

TemperatureElement::~TemperatureElement()
{
}

//
// Initializes the first instance used in an action's state.
//

void
TemperatureElement::init(SoState *)
{
   data = getDefault();
}

//
// Sets the current temperature in the state.
//
// Note: the 2.0/2.0.1 version of this method had one additional 
// argument, i.e.  set(SoState *state, SoNode *node, float temp)

void

TemperatureElement::set(SoState *state, float temp)
{
   // Use the corresponding method on SoFloatElement to set the
   // value in the top instance in the state
   SoFloatElement::set(classStackIndex, state, temp);
}

//
// Returns the current temperature from the state.
//

float
TemperatureElement::get(SoState *state)
{
   // Use the corresponding method on SoFloatElement to get the
   // value from the top instance in the state
   return SoFloatElement::get(classStackIndex, state);
}
