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
 |	call initClasses for all error classes
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/errors/SoErrors.h>

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes ALL Inventor error classes.
//
// Use: internal

void
SoError::initClasses()
//
////////////////////////////////////////////////////////////////////////
{
    SoError::initClass();

    SoDebugError::initClass();
    SoMemoryError::initClass();
    SoReadError::initClass();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoDebugError class.
//
// Use: internal

void
SoDebugError::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    // Initialize type id
    classTypeId = SoType::createType(SoError::getClassTypeId(), "DebugError");

    // Set handler to default handler
    handlerCB = defaultHandlerCB;
    cbData    = NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoError class.
//
// Use: internal

void
SoError::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    // Initialize type id
    classTypeId = SoType::createType(SoType::badType(), "Error");

    // Set handler to default handler
    handlerCB = defaultHandlerCB;
    cbData    = NULL;

    wasInitted = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoMemoryError class.
//
// Use: internal

void
SoMemoryError::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    // Initialize type id
    classTypeId = SoType::createType(SoError::getClassTypeId(), "MemoryError");

    // Set handler to default handler
    handlerCB = defaultHandlerCB;
    cbData    = NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoReadError class.
//
// Use: internal

void
SoReadError::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    // Initialize type id
    classTypeId = SoType::createType(SoError::getClassTypeId(), "ReadError");

    // Set handler to default handler
    handlerCB = defaultHandlerCB;
    cbData    = NULL;
}

