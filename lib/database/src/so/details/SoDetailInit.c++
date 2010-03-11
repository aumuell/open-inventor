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
 |	call initClasses for all detail classes
 |
 |   Author(s)		: Ronen Barzel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/details/SoDetails.h>

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes ALL Inventor detail classes.
//
// Use: internal

void
SoDetail::initClasses()
//
////////////////////////////////////////////////////////////////////////
{
    SoDetail::initClass();

    SoConeDetail::initClass();
    SoCubeDetail::initClass();
    SoCylinderDetail::initClass();
    SoFaceDetail::initClass();
    SoLineDetail::initClass();
    SoPointDetail::initClass();
    SoTextDetail::initClass();
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes class.
//
// Use: internal
//

void
SoConeDetail::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_DETAIL_INIT_CLASS(SoConeDetail, SoDetail);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes class.
//
// Use: internal
//

void
SoCubeDetail::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_DETAIL_INIT_CLASS(SoCubeDetail, SoDetail);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes class.
//
// Use: internal
//

void
SoCylinderDetail::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_DETAIL_INIT_CLASS(SoCylinderDetail, SoDetail);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes class.
//
// Use: internal
//

void
SoDetail::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    // Allocate a new type id
    // No real parent id, so pass 'badType' as the parent type
    classTypeId = SoType::createType(SoType::badType(), "SoDetail", NULL);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes class.
//
// Use: internal
//

void
SoFaceDetail::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_DETAIL_INIT_CLASS(SoFaceDetail, SoDetail);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes class.
//
// Use: internal
//

void
SoLineDetail::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_DETAIL_INIT_CLASS(SoLineDetail, SoDetail);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes class.
//
// Use: internal
//

void
SoPointDetail::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_DETAIL_INIT_CLASS(SoPointDetail, SoDetail);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes class.
//
// Use: internal
//

void
SoTextDetail::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_DETAIL_INIT_CLASS(SoTextDetail, SoDetail);
}

