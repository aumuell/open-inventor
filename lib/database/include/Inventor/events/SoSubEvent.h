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
 * Copyright (C) 1990-94   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Description:
 |	This file defines some macros that implement things common to
 |	many subclasses of SoEvent. They may be used to make SoEvent
 |	subclassing easier. In all of the macros, the "className"
 |	parameter refers to the name of the event subclass.
 |
 |   Defined macros:
 |
 |	Within class header:
 |
 |		SO_EVENT_HEADER(className)
 |
 |	Within class source:
 |
 |	    At file scope:
 |
 |		SO_EVENT_SOURCE(className)
 |
 |	    Class initialization (initClass):
 |
 |		SO_EVENT_INIT_CLASS()
 |
 |
 |   Author(s)		: David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_SUB_EVENT_
#define  _SO_SUB_EVENT_

#include <Inventor/events/SoEvent.h>

// C-api: end

//  *** note: many of the macros use the "do { ... } while(0)"
//  ***       hack to define multiline blocks as a single statement
//  ***       of code that can be used anywhere and ended with a semicolon

/////////////////////////////////////////////////////////////////////////////
//
// Macros to be called within the class definition header for an event
// subclass:
//

////////////////////////////////////////////
//
// These defines type-identifier and naming variables and methods that
// all subclasses and abstract subclasses must support.
//

#define SO_EVENT_HEADER()						      \
  public:								      \
    static SoType	getClassTypeId()	/* Returns class type id */   \
				    { return classTypeId; }		      \
    virtual SoType	getTypeId() const;	/* Returns type id	*/    \
  private:								      \
    static SoType	classTypeId		/* Type id		*/


/////////////////////////////////////////////////////////////////////////////
//
// Macros to be called within the source file for an event subclass:
//

////////////////////////////////////////////
//
// This declares the static variables defined in SO_EVENT_HEADER
//

#define SO__EVENT_VARS(className)					      \
    SoType		className::classTypeId


////////////////////////////////////////////
//
// Methods on the type
//

#define SO__EVENT_METHODS(className)				              \
									      \
    SoType								      \
    className::getTypeId() const					      \
    {									      \
	return classTypeId;						      \
    }


/////////////////////////////////////////////
//
// These include all the definitions required
// at file scope
//

#define	SO_EVENT_SOURCE(className)					      \
    SO__EVENT_VARS(className);						      \
    SO__EVENT_METHODS(className)


////////////////////////////////////////////
//
// This initializes the type identifer variables defined in
// SO_EVENT_HEADER . This macro should be called from within initClass().
// The parentClass argument should be the class that this subclass is
// derived from.
//

#define SO_EVENT_INIT_CLASS(className,parentClass)			      \
    classTypeId =							      \
        SoType::createType(parentClass::getClassTypeId(),		      \
		   SO__QUOTE(className))



// C-api: begin

#endif /* _SO_SUB_EVENT_ */

