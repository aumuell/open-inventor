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
 |      SoTriggerAny
 |
 |   Author(s)          : Ronen Barzel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoDB.h>
#include <Inventor/engines/SoTriggerAny.h>

SO_ENGINE_SOURCE(SoTriggerAny);


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoTriggerAny::SoTriggerAny()
//
////////////////////////////////////////////////////////////////////////
{
    SO_ENGINE_CONSTRUCTOR(SoTriggerAny);
    SO_ENGINE_ADD_INPUT(input0, ());
    SO_ENGINE_ADD_INPUT(input1, ());
    SO_ENGINE_ADD_INPUT(input2, ());
    SO_ENGINE_ADD_INPUT(input3, ());
    SO_ENGINE_ADD_INPUT(input4, ());
    SO_ENGINE_ADD_INPUT(input5, ());
    SO_ENGINE_ADD_INPUT(input6, ());
    SO_ENGINE_ADD_INPUT(input7, ());
    SO_ENGINE_ADD_INPUT(input8, ());
    SO_ENGINE_ADD_INPUT(input9, ());
    SO_ENGINE_ADD_OUTPUT(output, SoSFTrigger);
    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoTriggerAny::~SoTriggerAny()
//
////////////////////////////////////////////////////////////////////////
{
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Evaluation routine accesses the input triggers, to ensure
//    evaluation of whatever's driving them.
//
// Use: private

void
SoTriggerAny::evaluate()
//
////////////////////////////////////////////////////////////////////////
{
    input0.getValue();
    input1.getValue();
    input2.getValue();
    input3.getValue();
    input4.getValue();
    input5.getValue();
    input6.getValue();
    input7.getValue();
    input8.getValue();
    input9.getValue();
}
