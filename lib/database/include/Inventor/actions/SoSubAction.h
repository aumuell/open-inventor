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
 |	many subclasses of SoAction. They may be used to make SoAction
 |	subclassing easier. In all of the macros, the "className"
 |	parameter refers to the name of the action subclass.
 |
 |   Defined macros:
 |
 |	Within class header:
 |		SO_ACTION_HEADER
 |
 |	Within class source:
 |		SO_ACTION_SOURCE
 |		SO_ACTION_INIT_CLASS
 |		SO_ACTION_CONSTRUCTOR
 |		SO_ACTION_ADD_METHOD
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_SUB_ACTION_
#define  _SO_SUB_ACTION_

#include <Inventor/actions/SoAction.h>
#include <Inventor/SbString.h>

// C-api: end
/////////////////////////////////////////////////////////////////////////////
//
// Macros to be called within the class definition header for an action
// subclass:
//

////////////////////////////////////////////
//
// This defines type-identifier variables and methods that all
// subclasses must support. 
//

#define SO_ACTION_HEADER(className)					      \
  public:								      \
    virtual SoType		getTypeId() const;			      \
    static SoType		getClassTypeId()			      \
				    { return classTypeId; }		      \
  SoEXTENDER public:							      \
    static void 		addMethod(SoType t, SoActionMethod method)    \
				    { methods->addMethod(t, method); }	      \
    static void 		enableElement(SoType t, int stkIndex)	      \
				    { enabledElements->enable(t, stkIndex);}  \
  protected:								      \
    virtual const SoEnabledElementsList & getEnabledElements() const;	      \
    static SoEnabledElementsList *enabledElements;			      \
    static SoActionMethodList	*methods;				      \
  private:								      \
    static SoType		classTypeId

/////////////////////////////////////////////////////////////////////////////
//
// Macros to be called within the source file for a action subclass:
//

////////////////////////////////////////////
//
// This declares the static variables defined in SO_ACTION_HEADER.
//

#define SO__ACTION_VARS(className)					      \
    SoEnabledElementsList	*className::enabledElements;		      \
    SoActionMethodList		*className::methods;			      \
    SoType			className::classTypeId

////////////////////////////////////////////
//
// This implements the methods defined in SO_ACTION_HEADER.
//

#define SO__ACTION_METHODS(className)					      \
									      \
SoType									      \
className::getTypeId() const						      \
{									      \
    return classTypeId;							      \
}									      \
									      \
const SoEnabledElementsList &						      \
className::getEnabledElements() const					      \
{									      \
    return *enabledElements;						      \
}

////////////////////////////////////////////
//
// This contains the definitions necessary at file scope
//
#define SO_ACTION_SOURCE(className)					      \
    SO__ACTION_VARS(className);						      \
    SO__ACTION_METHODS(className)

////////////////////////////////////////////
//
// This initializes the type identifer variables defined in
// SO_ACTION_HEADER. This macro should be called from within
// initClass().
//

#define SO_ACTION_INIT_CLASS(className, parentClass)			      \
    enabledElements = new SoEnabledElementsList(parentClass::enabledElements);\
    methods = new SoActionMethodList(parentClass::methods);		      \
    classTypeId    = SoType::createType(parentClass::getClassTypeId(),	      \
					SO__QUOTE(className), NULL)

///////////////////////////////////////////////
//
// This is included at the beginning of
// a constructor, to do required initializations
//

#define SO_ACTION_CONSTRUCTOR(className)				      \
    traversalMethods = methods

////////////////////////////////////////////
//
// This macro can be used by action subclasses within initClass() to
// register a method to call for a specific node class. It is passed
// the name of the node class and the method to call.
//

#define SO_ACTION_ADD_METHOD(nodeClass, method)				      \
    addMethod(nodeClass::getClassTypeId(), method)
// C-api: begin

#endif /* _SO_SUB_ACTION_ */
