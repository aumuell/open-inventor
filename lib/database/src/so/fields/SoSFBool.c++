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
 |	SoSFBool
 |
 |   Author(s)		: Ronen Barzel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/errors/SoReadError.h>
#include <Inventor/fields/SoSFBool.h>

//////////////////////////////////////////////////////////////////////////////
//
// SoSFBool class
//
//////////////////////////////////////////////////////////////////////////////

// Use standard definitions of all basic methods
SO_SFIELD_SOURCE(SoSFBool, SbBool, SbBool);


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads value from file. Returns FALSE on error.
//
// Use: private

SbBool
SoSFBool::readValue(SoInput *in)
//
////////////////////////////////////////////////////////////////////////
{
    // accept 0 or 1 for both binary and ascii
    // with integer fields)
    if (in->read(value)) {
	if (value != 0 && value != 1) {
	    SoReadError::post(in, "Illegal value for SoSFBool: %d "
			      "(must be 0 or 1)", value);
	    return FALSE;
	}
	return TRUE;
    }

    // binary doesn't use TRUE/FALSE strings
    if (in->isBinary())
	return FALSE;

    // read TRUE/FALSE keyword
    SbName n;
    if (!in->read(n, TRUE))
	return FALSE;
    
    if (n == "TRUE") {
	value = TRUE;
	return TRUE;
    }

    if (n == "FALSE") {
	value = FALSE;
	return TRUE;
    }

    SoReadError::post(in, "Unknown value (\"%s\") for SoSFBool ",
		      "(must be TRUE or FALSE)", n.getString());
    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes value of field to file.
//
// Use: private

void
SoSFBool::writeValue(SoOutput *out) const
//
////////////////////////////////////////////////////////////////////////
{
    if (out->isBinary())
	out->write(value?1:0);
    else
	out->write(value?"TRUE":"FALSE");
}
