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
 |	SoSFBitMask
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <Inventor/errors/SoDebugError.h>
#include <Inventor/errors/SoReadError.h>
#include <Inventor/fields/SoSFBitMask.h>

//////////////////////////////////////////////////////////////////////////////
//
// SoSFBitMask class
//
//////////////////////////////////////////////////////////////////////////////

// Can't use _DERIVED_SOURCE macro because need to implement a destructor:
SO_SFIELD_REQUIRED_SOURCE(SoSFBitMask);

SoSFBitMask::SoSFBitMask()
{
}

SoSFBitMask::~SoSFBitMask()
{
    if (!legalValuesSet && numEnums > 0) {
	delete[] enumValues;
	delete[] enumNames;
    }
}

// Special characters when reading or writing value in ASCII
#define OPEN_PAREN	'('
#define CLOSE_PAREN	')'
#define BITWISE_OR	'|'

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads value from file. Returns FALSE on error. See the comments
//    in writeValue for file format details.
//
// Use: private

SbBool
SoSFBitMask::readValue(SoInput *in)
//
////////////////////////////////////////////////////////////////////////
{
    char	c;
    SbName	n;
    int		v;

    value = 0;

    if (in->isBinary()) {

	// Read all non-empty strings
	while (in->read(n, TRUE) && ! (! n) ) {
	    if (findEnumValue(n, v))
		value |= v;

	    else {
		SoReadError::post(in,
				  "Unknown SoSFBitMask bit mask value \"%s\"",
				  n.getString());
		return FALSE;
	    }
	}
    }

    else {
	// Read first character
	if (! in->read(c))
	    return FALSE;

	// Check for parenthesized list of bitwise-or'ed flags
	if (c == OPEN_PAREN) {

	    // Read names separated by BITWISE_OR
	    while (TRUE) {
		if (in->read(n, TRUE) && ! (! n) ) {

		    if (findEnumValue(n, v))
			value |= v;

		    else {
			SoReadError::post(in, "Unknown SoSFBitMask bit "
					  "mask value \"%s\"", n.getString());
			return FALSE;
		    }
		}

		if (! in->read(c)) {
		    SoReadError::post(in, "EOF reached before '%c' "
				      "in SoSFBitMask value", CLOSE_PAREN);
		    return FALSE;
		}

		if (c == CLOSE_PAREN)
		    break;

		else if (c != BITWISE_OR) {
		    SoReadError::post(in, "Expected '%c' or '%c', got '%c' "
				      "in SoSFBitMask value",
				      BITWISE_OR, CLOSE_PAREN, c);
		    return FALSE;
		}
	    }
	}

	else {
	    in->putBack(c);

	    // Read mnemonic value as a character string identifier
	    if (! in->read(n, TRUE))
		return FALSE;

	    if (! findEnumValue(n, value)) {
		SoReadError::post(in, "Unknown SoSFBitMask bit "
				  "mask value \"%s\"", n.getString());
		return FALSE;
	    }
	}
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes value of field to file. If the value matches one mask
//    exactly, the name of that mask is written. Otherwise, this
//    matches one mask at at time against the value. Each time a mask
//    is found to be contained in the value, its name is written and
//    the remaining bits are processed. In the ASCII version, multiple
//    mask names are written as "(MASK1 | MASK2 | etc )". In the
//    binary version, the mask names are written as consecutive
//    strings, ending with an empty string; this is true even if one
//    mask matches exactly.
//
// Use: private

void
SoSFBitMask::writeValue(SoOutput *out) const
//
////////////////////////////////////////////////////////////////////////
{
    const SbName	*n;
    int			i;

#ifdef DEBUG
    if (enumValues == NULL) {
	SoDebugError::post("SoSFBitMask::writeValue",
			   "Enum values were never initialized");
	return;
    }
#endif /* DEBUG */

    // Look for one flag that matches entire value (if any)
    if (findEnumName(value, n))
	out->write(n->getString());

    // Otherwise, pull out 1 bit mask at a time until the value is complete
    else {
	int	v = value;

	if (! out->isBinary())
	    out->write(OPEN_PAREN);

	for (i = 0; i < numEnums; i++) {

	    // Mask must match exactly
	    if ( (enumValues[i] & v) == enumValues[i]) {

		out->write(enumNames[i].getString());

		// Turn off those bits
		v &= ~enumValues[i];

		if (v == 0)
		    break;

		if (! out->isBinary()) {
		    out->write(' ');
		    out->write(BITWISE_OR);
		    out->write(' ');
		}
	    }
	}

	// All bits must have been written out
	if (v != 0)
	    SoDebugError::post("SoSFBitMask::writeValue",
			       "unable to write some bits (%#x)", v);

	if (! out->isBinary())
	    out->write(CLOSE_PAREN);
    }

    if (out->isBinary())
	out->write("");
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Looks up enum name, returns value. Returns FALSE if not found.
//    SFEnum method not used because need to allocate unknown enum
//    bits uniquely.
//
// Use: protected virtual

SbBool
SoSFBitMask::findEnumValue(const SbName &name, int &val)
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
	val = 1<<numEnums;
	enumValues[numEnums] = 1<<numEnums;
	enumNames[numEnums] = name;
	++numEnums;
	return TRUE;
    }

    return FALSE;
}

