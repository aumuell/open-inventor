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

//  -*- C++ -*-

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Description:
 |	This file defines some macros that implement things common to
 |	many subclasses of SoDetail. They may be used to make SoDetail
 |	subclassing easier. In all of the macros, the "className"
 |	parameter refers to the name of the detail subclass.
 |
 |   Defined macros:
 |
 |	Within class header:
 |		SO_DETAIL_HEADER
 |
 |	Within class source:
 |		SO_DETAIL_SOURCE
 |		SO_DETAIL_INIT_CLASS
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_SUB_DETAIL_
#define  _SO_SUB_DETAIL_

#include <Inventor/details/SoDetail.h>

// C-api: end
/////////////////////////////////////////////////////////////////////////////
//
// Macros to be called within the class definition header for a detail
// subclass:
//

////////////////////////////////////////////
//
// This defines type-identifier variables and methods that all
// subclasses must support. 
//

#define SO_DETAIL_HEADER(className)					      \
  public:								      \
    virtual SoType		getTypeId() const;			      \
    static SoType		getClassTypeId() { return classTypeId; }      \
  private:								      \
    static SoType		classTypeId

/////////////////////////////////////////////////////////////////////////////
//
// Macros to be called within the source file for a detail subclass:
//

////////////////////////////////////////////
//
// This declares the static variables defined in SO_DETAIL_HEADER.
//

#define SO__DETAIL_ID_VARS(className)					      \
    SoType			className::classTypeId

////////////////////////////////////////////
//
// This implements the methods defined in SO_DETAIL_HEADER.
//

#define SO__DETAIL_ID_METHODS(className)				      \
									      \
SoType									      \
className::getTypeId() const						      \
{									      \
    return classTypeId;							      \
}

////////////////////////////////////////////
//
// This contains the definitions necessary at file scope
//
#define SO_DETAIL_SOURCE(className)					      \
    SO__DETAIL_ID_VARS(className);					      \
    SO__DETAIL_ID_METHODS(className)

////////////////////////////////////////////
//
// This initializes the type identifer variables defined in
// SO_DETAIL_HEADER. This macro should be called from within
// initClass().
//

#define SO_DETAIL_INIT_CLASS(className, parentClass)			      \
    classTypeId = SoType::createType(parentClass::getClassTypeId(),	      \
				     SO__QUOTE(className), NULL)

// C-api: begin

#endif /* _SO_SUB_DETAIL_ */
