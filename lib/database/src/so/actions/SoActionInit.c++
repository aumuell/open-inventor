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
 * Copyright (C) 1990,91,92   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Class:
 |	call initClasses for all action classes
 |
 |   Author(s)		: Ronen Barzel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/actions/SoActions.h>
#include <Inventor/elements/SoElements.h>
#include <Inventor/misc/SoBasic.h>

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes ALL Inventor action classes.
//
// Use: internal

void
SoAction::initClasses()
//
////////////////////////////////////////////////////////////////////////
{
    // base class must be initialized first
    SoAction::initClass();

    SoCallbackAction::initClass();
    SoGLRenderAction::initClass();
    SoGetBoundingBoxAction::initClass();
    SoGetMatrixAction::initClass();
    SoHandleEventAction::initClass();
    SoPickAction::initClass();
    SoRayPickAction::initClass();
    SoSearchAction::initClass();
    SoWriteAction::initClass();
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoAction class.
//
// Use: internal

void
SoAction::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    enabledElements = new SoEnabledElementsList(NULL);
    methods = new SoActionMethodList(NULL);

    // Allocate a new action type id. There's no real parent id, so we
    // can't use the regular macro.
    classTypeId = SoType::createType(SoType::badType(), "SoAction", NULL);

    // Enable override element for all actions.
    enabledElements->enable(SoOverrideElement::getClassTypeId(),
			    SoOverrideElement::getClassStackIndex());
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoCallbackAction class.
//
// Use: internal

void
SoCallbackAction::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_ACTION_INIT_CLASS(SoCallbackAction, SoAction);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoGLRenderAction class.
//
// Use: internal

void
SoGLRenderAction::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_ACTION_INIT_CLASS(SoGLRenderAction, SoAction);

    SO_ENABLE(SoGLRenderAction, SoGLLazyElement);
    SO_ENABLE(SoGLRenderAction, SoGLRenderPassElement);
    SO_ENABLE(SoGLRenderAction, SoViewportRegionElement);
    SO_ENABLE(SoGLRenderAction, SoWindowElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoGetBoundingBoxAction class.
//
// Use: internal

void
SoGetBoundingBoxAction::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_ACTION_INIT_CLASS(SoGetBoundingBoxAction, SoAction);

    SO_ENABLE(SoGetBoundingBoxAction, SoViewportRegionElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoGetMatrixAction class.
//
// Use: internal

void
SoGetMatrixAction::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_ACTION_INIT_CLASS(SoGetMatrixAction, SoAction);

    SO_ENABLE(SoGetMatrixAction, SoViewportRegionElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoHandleEventAction class.
//
// Use: internal

void
SoHandleEventAction::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_ACTION_INIT_CLASS(SoHandleEventAction, SoAction);

    SO_ENABLE(SoHandleEventAction, SoViewportRegionElement);
    SO_ENABLE(SoHandleEventAction, SoWindowElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoPickAction class.
//
// Use: internal

void
SoPickAction::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_ACTION_INIT_CLASS(SoPickAction, SoAction);

    SO_ENABLE(SoPickAction, SoViewportRegionElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoRayPickAction class.
//
// Use: internal

void
SoRayPickAction::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_ACTION_INIT_CLASS(SoRayPickAction, SoPickAction);

    SO_ENABLE(SoRayPickAction, SoPickRayElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoSearchAction class.
//
// Use: internal

void
SoSearchAction::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_ACTION_INIT_CLASS(SoSearchAction, SoAction);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoWriteAction class.
//
// Use: internal

void
SoWriteAction::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_ACTION_INIT_CLASS(SoWriteAction, SoAction);
}

