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

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |      SoOnOff
 |
 |   Author(s)          : Ronen Barzel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/fields/SoSFBool.h>
#include <Inventor/engines/SoOnOff.h>

SO_ENGINE_SOURCE(SoOnOff);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoOnOff::SoOnOff()
//
////////////////////////////////////////////////////////////////////////
{
    SO_ENGINE_CONSTRUCTOR(SoOnOff);
    SO_ENGINE_ADD_INPUT(on,	  ());
    SO_ENGINE_ADD_INPUT(off,	  ());
    SO_ENGINE_ADD_INPUT(toggle,	  ());
    SO_ENGINE_ADD_OUTPUT(isOn, SoSFBool);
    SO_ENGINE_ADD_OUTPUT(isOff, SoSFBool);
    isBuiltIn = TRUE;

    state = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoOnOff::~SoOnOff()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Detects input triggers
//
// Use: private

void
SoOnOff::inputChanged(SoField *which)
//
////////////////////////////////////////////////////////////////////////
{
    if (which == &on)
	state = TRUE;
    
    else if (which == &off)
	state = FALSE;

    else if (which == &toggle)
	state = !state;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Evaluation routine
//
// Use: private

void
SoOnOff::evaluate()
//
////////////////////////////////////////////////////////////////////////
{
    // make sure to pull trigger connections
    on.getValue();
    off.getValue();
    toggle.getValue();

    SO_ENGINE_OUTPUT(isOn, SoSFBool, setValue(state));
    SO_ENGINE_OUTPUT(isOff, SoSFBool, setValue(!state));
}
