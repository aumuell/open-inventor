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
 |   $Revision $
 |
 |   Classes:
 |      SoProfile - abstract base class for all profiles
 |
 |   Author(s)          : Thad Beier, Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoPickAction.h>
#include <Inventor/elements/SoProfileCoordinateElement.h>
#include <Inventor/elements/SoProfileElement.h>
#include <Inventor/nodes/SoProfile.h>

SO_NODE_ABSTRACT_SOURCE(SoProfile);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: protected

SoProfile::SoProfile()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoProfile);
    SO_NODE_ADD_FIELD(index,   (0));
    SO_NODE_ADD_FIELD(linkage, (START_FIRST));

    // Set up static info for enumerated type field
    SO_NODE_DEFINE_ENUM_VALUE(Profile, START_FIRST);
    SO_NODE_DEFINE_ENUM_VALUE(Profile, START_NEW);
    SO_NODE_DEFINE_ENUM_VALUE(Profile, ADD_TO_CURRENT);

    SO_NODE_SET_SF_ENUM_TYPE(linkage, Profile);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoProfile::~SoProfile()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements all actions.
//
// Use: extender

void
SoProfile::doAction(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoProfileElement::add(action->getState(), this);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does callback action thing.
//
// Use: extender

void
SoProfile::callback(SoCallbackAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoProfile::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does GL render action.
//
// Use: extender

void
SoProfile::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoProfile::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does getBoundingBox action.
//
// Use: extender

void
SoProfile::getBoundingBox(SoGetBoundingBoxAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoProfile::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does pick action.
//
// Use: extender

void
SoProfile::pick(SoPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoProfile::doAction(action);
}
