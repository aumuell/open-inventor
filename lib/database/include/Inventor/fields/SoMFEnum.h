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
 |   Classes:
 |	SoMFEnum
 |
 |   Macros:
 |
 |	SO_NODE_SET_MF_ENUM_TYPE(fieldName, enumType)
 |	SO_ENGINE_SET_MF_ENUM_TYPE(fieldName, enumType)
 |		These assign a specific type of enum to an MFEnum field.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_MF_ENUM_
#define  _SO_MF_ENUM_

#include <Inventor/SbString.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/fields/SoSubField.h>

//////////////////////////////////////////////////////////////////////////////
//
//  SoMFEnum subclass of SoMField. This field holds enumerated types.
//  Each instance of the field must define enumeration mnemonics and
//  character strings. This stores the enumeration values as integers
//  (as allowed by C++).
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoMEnum
class SoMFEnum : public SoMField {

    // Use standard field stuff
    SO_MFIELD_HEADER(SoMFEnum, int, int);

  public:
    //
    // Some convenience functions:
    //

    // Set value from SbName or character string
    // C-api: name=setStr
    void		setValue(const SbName &name);

    // Set indexed value from SbName or character string
    // C-api: name=set1Str
    void		set1Value(int index, const SbName &name);

  SoEXTENDER public:
    // Sets up value/name correspondances
    void		setEnums(int num, int *vals, SbName *names);

  SoINTERNAL public:
    static void		initClass();

  protected:
    SbBool		legalValuesSet; // TRUE if setEnums called
    int			numEnums;	// Number of enumeration values
    int			*enumValues;	// Enumeration values
    SbName		*enumNames;	// Mnemonic names of values

    // Looks up enum name, returns value. Returns FALSE if not found.
    virtual SbBool	findEnumValue(const SbName &name, int &val);

    // Looks up enum value, returns ptr to name. Returns FALSE if not found.
    virtual SbBool	findEnumName(int val, const SbName *&name) const;
};

// C-api: end

////////////////////////////////////////////
//
//  Debugging macros (used by other macros in this file)
//  The container must have a field 'firstInstance'.
//
#ifdef DEBUG
#define SO__MF_ENUM_CHECK_DATA(vals, typeName, fieldName, containerMacroName) \
    do {								      \
	if (vals == NULL && firstInstance)				      \
	    SoDebugError::post("SO_SET_MF_ENUM_TYPE",			      \
			       "Field %s (%s, line %d): Did you forget to"    \
			       " use SO_%s_DEFINE_ENUM_VALUE(%s, ...)?",      \
			       fieldName, __FILE__, __LINE__,		      \
			       containerMacroName, typeName);		      \
    } while(0)
#else
#define SO__MF_ENUM_CHECK_DATA(vals, typeName, fieldName, containerMacroName)	while(0)
#endif
	

////////////////////////////////////////////
//
// This defines the specific type of enum expected by a particular
// SoMFEnum field.
//

#define	SO__MF_ENUM_SET_TYPE(fieldName, enumType, contMacroName, contData)    \
    do {								      \
	int _so_mf_enum_num;						      \
	int *_so_mf_enum_vals;						      \
	SbName *_so_mf_enum_names;					      \
	contData->getEnumData(SO__QUOTE(enumType),			      \
				_so_mf_enum_num,			      \
				_so_mf_enum_vals,			      \
				_so_mf_enum_names);			      \
	SO__MF_ENUM_CHECK_DATA(_so_mf_enum_vals,			      \
			       SO__QUOTE(enumType),			      \
			       SO__QUOTE(fieldName),			      \
			       contMacroName);				      \
	fieldName.setEnums(_so_mf_enum_num,				      \
			   _so_mf_enum_vals,				      \
			   _so_mf_enum_names);				      \
    } while (0)

#define SO_NODE_SET_MF_ENUM_TYPE(fieldName, enumType)			      \
    SO__MF_ENUM_SET_TYPE(fieldName,enumType,"NODE",fieldData)

#define SO_ENGINE_SET_MF_ENUM_TYPE(fieldName, enumType)			      \
    SO__MF_ENUM_SET_TYPE(fieldName,enumType,"ENGINE",inputData)

//
// End of macros
//
/////////////////////////////////////////////////////////////////////////////

// C-api: begin

#endif /* _SO_MF_ENUM_ */
