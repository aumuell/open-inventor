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
 |	SoSFEnum
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <Inventor/errors/SoDebugError.h>
#include <Inventor/errors/SoReadError.h>
#include <Inventor/fields/SoSFEnum.h>

//////////////////////////////////////////////////////////////////////////////
//
// SoSFEnum class
//
//////////////////////////////////////////////////////////////////////////////

// Use standard definitions except for constructor/destructor
SO_SFIELD_REQUIRED_SOURCE(SoSFEnum);
SO_SFIELD_VALUE_SOURCE(SoSFEnum, int, int);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor.
//
// Use: extender

SoSFEnum::SoSFEnum()
//
////////////////////////////////////////////////////////////////////////
{
    enumValues = NULL;
    enumNames  = NULL;
    numEnums = 0;
    legalValuesSet = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: extender

SoSFEnum::~SoSFEnum()
//
////////////////////////////////////////////////////////////////////////
{
    if (!legalValuesSet && numEnums > 0) {
	delete[] enumValues;
	delete[] enumNames;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets value from given SbName, which must contain the name of a
//    valid enumerator.
//
// Use: public

void
SoSFEnum::setValue(const SbName &name)
//
////////////////////////////////////////////////////////////////////////
{
    int	enumVal;

    if (findEnumValue(name, enumVal))
	setValue(enumVal);

#ifdef DEBUG
    else
	SoDebugError::post("SoSFEnum::setValue",
			   "No enum for name \"%s\"", name.getString());
#endif /* DEBUG */
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Called by nodes to set legal enum names and values
//
// Use: extender public

void
SoSFEnum::setEnums(int num, int *vals, SbName *names)

//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (legalValuesSet == TRUE) {
	SoDebugError::post("SoSFEnum::setEnums",
			   "setEnums called twice");
    }
#endif

    legalValuesSet = TRUE;
    numEnums = num;
    enumValues = vals;
    enumNames = names;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Looks up enum name, returns value. Returns FALSE if not found.
//
// Use: protected

SbBool
SoSFEnum::findEnumValue(const SbName &name, int &val)
//
////////////////////////////////////////////////////////////////////////
{
    int	i;

    // Look through names table for one that matches
    for (i = 0; i < numEnums; i++) {
	if (name == enumNames[i]) {
	    val = enumValues[i];
	    return TRUE;
	}
    }

    if (!legalValuesSet) {
	// Must be part of an unknown node, add name, value:
	int *oldValues = enumValues;
	SbName *oldNames = enumNames;
	enumValues = new int[numEnums+1];
	enumNames = new SbName[numEnums+1];
	if (numEnums != 0) {
	    for (i = 0; i < numEnums; i++) {
		enumValues[i] = oldValues[i];
		enumNames[i] = oldNames[i];
	    }
	    delete[] oldValues;
	    delete[] oldNames;
	}
	val = numEnums;
	enumValues[numEnums] = numEnums;
	enumNames[numEnums] = name;
	++numEnums;
	return TRUE;
    }

    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Looks up enum value, returns pointer to name. Returns FALSE if
//    not found.
//
// Use: protected

SbBool
SoSFEnum::findEnumName(int val, const SbName *&name) const
//
////////////////////////////////////////////////////////////////////////
{
    int	i;

    // Find string corresponding to given value
    for (i = 0; i < numEnums; i++) {
	if (val == enumValues[i]) {
	    name = &enumNames[i];
	    return TRUE;
	}
    }

    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads value from file. Returns FALSE on error.
//
// Use: private

SbBool
SoSFEnum::readValue(SoInput *in)
//
////////////////////////////////////////////////////////////////////////
{
    SbName	n;

    // Read mnemonic value as a character string identifier
    if (! in->read(n, TRUE))
	return FALSE;

    if (findEnumValue(n, value))
	return TRUE;

    // Not found? Too bad
    SoReadError::post(in, "Unknown SoSFEnum enumeration value \"%s\"",
		      n.getString());
    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes value of field to file.
//
// Use: private

void
SoSFEnum::writeValue(SoOutput *out) const
//
////////////////////////////////////////////////////////////////////////
{
    const SbName	*n;

#ifdef DEBUG
    if (enumValues == NULL) {
	SoDebugError::post("SoSFEnum::writeValue",
			   "Enum values were never initialized");
	return;
    }
#endif /* DEBUG */

    // Find and write string corresponding to given value
    if (findEnumName(value, n))
	out->write(n->getString());

    // Whoops! Something bad was in the field value
    else {
	SoDebugError::post("SoSFEnum::writeValue",
			   "Illegal value (%d) in field", value);
    }
}
