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
 |   $Revision: 1.1 $
 |
 |   Description:
 |	This file defines some macros that implement things common to
 |	many subclasses of SoElement. They may be used to make SoElement
 |	subclassing easier. In all of the macros, the "className"
 |	parameter refers to the name of the element subclass.
 |
 |   Defined macros:
 |
 |	Within class header:
 |
 |		SO_ELEMENT_ABSTRACT_HEADER(className)
 |		SO_ELEMENT_HEADER(className)
 |
 |	Within class source:
 |
 |	    At file scope:
 |
 |		SO_ELEMENT_ABSTRACT_SOURCE(className)
 |		SO_ELEMENT_SOURCE(className)
 |
 |	    Class initialization (in initClass):
 |
 |		SO_ELEMENT_INIT_ABSTRACT_CLASS()
 |		SO_ELEMENT_INIT_CLASS()
 |
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_SUB_ELEMENT_
#define  _SO_SUB_ELEMENT_

#include <Inventor/elements/SoElement.h>

// C-api: end

/////////////////////////////////////////////////////////////////////////////
//
// Macros to be called within the class definition header for a element
// subclass:
//

////////////////////////////////////////////
//
// These define type-identifier and stack index variables and methods
// that all subclasses must support.
//

//
// This one is for abstract element classes.
//
#define SO_ELEMENT_ABSTRACT_HEADER(className)				      \
  public:								      \
    static SoType	getClassTypeId()	{ return classTypeId; }	      \
    static int		getClassStackIndex()	{ return classStackIndex; }   \
  protected:								      \
    static int		classStackIndex;				      \
    className();							      \
  private:								      \
    static SoType	classTypeId

//
// This one is for non-abstract element classes.
//
#define SO_ELEMENT_HEADER(className)					      \
    SO_ELEMENT_ABSTRACT_HEADER(className);				      \
  private:								      \
    static void *createInstance()

/////////////////////////////////////////////////////////////////////////////
//
// Macros to be called within the source file for a element subclass:
//

////////////////////////////////////////////
//
// This declares the static variables defined in SO_ELEMENT_HEADER
// or SO_ELEMENT_ABSTRACT_HEADER.
//

#define SO__ELEMENT_ABSTRACT_VARS(className)				      \
SoType	className::classTypeId;						      \
int		className::classStackIndex

#define SO__ELEMENT_VARS(className)					      \
SO__ELEMENT_ABSTRACT_VARS(className)

//
// Methods on an abstract type
//
#define SO__ELEMENT_ABSTRACT_METHODS(className)				      \
									      \
className::className()							      \
{									      \
}

//
// Methods on a non-abstract type
//
#define SO__ELEMENT_METHODS(className)					      \
									      \
className::className()							      \
{									      \
    setTypeId(classTypeId);						      \
    setStackIndex(classStackIndex);					      \
}									      \
									      \
void *									      \
className::createInstance()						      \
{									      \
    return new className;						      \
}

/////////////////////////////////////////////
//
// These include all the definitions required
// at file scope
//

#define	SO_ELEMENT_ABSTRACT_SOURCE(className)				      \
    SO__ELEMENT_ABSTRACT_VARS(className);				      \
    SO__ELEMENT_ABSTRACT_METHODS(className)

#define	SO_ELEMENT_SOURCE(className)					      \
    SO__ELEMENT_VARS(className);					      \
    SO__ELEMENT_METHODS(className)

////////////////////////////////////////////
//
// This initializes the type identifer variables defined in
// SO_ELEMENT_HEADER or SO_ELEMENT_ABSTRACT_HEADER. This macro
// should be called from within initClass(). The parentClass argument
// should be the class that this subclass is derived from.
//

#define SO_ELEMENT_INIT_ABSTRACT_CLASS(className,parentClass)		      \
    do {								      \
	classTypeId = SoType::createType(parentClass::getClassTypeId(),	      \
					 SO__QUOTE(className),		      \
					 NULL);				      \
	if (parentClass::classStackIndex < 0)				      \
	    classStackIndex = createStackIndex(classTypeId);		      \
	else								      \
	    classStackIndex = parentClass::classStackIndex;		      \
    } while (0)

#define SO_ELEMENT_INIT_CLASS(className,parentClass)			      \
    do {								      \
	classTypeId = SoType::createType(parentClass::getClassTypeId(),	      \
					 SO__QUOTE(className),		      \
					 &className::createInstance);	      \
	if (parentClass::classStackIndex < 0)				      \
	    classStackIndex = createStackIndex(classTypeId);		      \
	else								      \
	    classStackIndex = parentClass::classStackIndex;		      \
    } while (0)

// C-api: begin

#endif /* _SO_SUB_ELEMENT_ */
