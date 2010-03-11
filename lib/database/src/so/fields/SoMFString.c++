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
 |	SoMFString
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/errors/SoDebugError.h>
#include <Inventor/fields/SoMFString.h>

//////////////////////////////////////////////////////////////////////////////
//
// SoMFString class
//
//////////////////////////////////////////////////////////////////////////////

// Use standard definitions of all basic methods
SO_MFIELD_SOURCE(SoMFString, SbString, const SbString &);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets values from array of character strings. (Convenience function)
//
// Use: public

void
SoMFString::setValues(int start, int num, const char *strings[])
{
    int	newNum = start + num;
    int	i;

    if (newNum > getNum())
	makeRoom(newNum);

    for (i = 0; i < num; i++)
	values[start + i] = strings[i];

    valueChanged();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets value from character string. (Convenience function)
//
// Use: public

void
SoMFString::setValue(const char *string)
//
////////////////////////////////////////////////////////////////////////
{
    setValue(SbString(string));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Convenience function to delete text from consecutive strings.
//    All text from the given character of one line (fromChar in
//    fromLine) to the given character of another line (toChar in
//    toLine), inclusive, is deleted. Any leftover text on fromLine
//    and toLine is merged into one line.
//
// Use: public

void
SoMFString::deleteText(int fromLine, int fromChar, int toLine, int toChar)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (fromLine > toLine) {
	SoDebugError::post("SoMFString::deleteText",
			   "fromLine (%d) > toLine (%d)", fromLine, toLine);
	return;
    }
    if (fromLine == toLine && fromChar > toChar) {
	SoDebugError::post("SoMFString::deleteText",
			   "fromChar (%d) > toChar (%d) on same line",
			   fromChar, toChar);
	return;
    }
#endif

    // If operation is all within one line
    if (fromLine == toLine) {
	values[fromLine].deleteSubString(fromChar, toChar);

	// Make sure notification occurs
	valueChanged();
    }

    else {
	// Remove end characters from first line and beginning chars
	// from last line
	values[fromLine].deleteSubString(fromChar);
	values[toLine].deleteSubString(0, toChar);

	// Merge the two lines into 1 string and store in field
	SbString mergeString = values[fromLine];
	mergeString += values[toLine];
	values[fromLine] = mergeString;

	// Remove intervening lines (this also causes notify to occur)
	deleteValues(fromLine + 1, toLine - fromLine);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads one (indexed) value from file. Returns FALSE on error.
//
// Use: private

SbBool
SoMFString::read1Value(SoInput *in, int index)
//
////////////////////////////////////////////////////////////////////////
{
    return in->read(values[index]);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes one (indexed) value to file.
//
// Use: private

void
SoMFString::write1Value(SoOutput *out, int index) const
//
////////////////////////////////////////////////////////////////////////
{
    out->write(values[index]);
}
