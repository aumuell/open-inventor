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
 |   $Revision: 1.2 $
 |
 |   Classes:
 |	SbString
 |
 |   Author(s)		: Nick Thompson, Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SbString.h>
#include <Inventor/errors/SoDebugError.h>

//
// Constructor that initializes to a substring.
//

SbString::SbString(const char *str, int start, int end)
{
    int size = end - start + 1;

    if (size < SB_STRING_STATIC_STORAGE_SIZE)
	string = staticStorage;
    else
	string = new char[size+1];

    strncpy(string, str+start, size);
    string[size] = '\0';
    storageSize = size;
}

//
// Constructor that initializes to string formed from given integer.
// For example, SbString(1234) gives the string "1234".
//

SbString::SbString(int digitString)
{
    char	buf[32];

    string = staticStorage;
    sprintf(buf, "%d", digitString);
    *this = buf;
}

//
// Destructor
//

SbString::~SbString()
{
    if (string != staticStorage)
	delete [] string;
}

//
// Makes more room in storage for string for n more bytes,
// allocating or reallocating if necessary.
//

void
SbString::expand(int bySize)
{
    int newSize = strlen(string) + bySize + 1;

    if (newSize >= SB_STRING_STATIC_STORAGE_SIZE &&
	(string == staticStorage || newSize > storageSize)) {

	char *newString = new char[newSize];

	strcpy(newString, string);

	if (string != staticStorage)
	    delete [] string;

	string      = newString;
	storageSize = newSize;
    }
}

// Simple hashing algorithm that will xor all the characters in a string
//   after shifting to the left 0, 5, 10, 15, 20, 1, 6, 11, 16, 21, 2, ... 
//   positions.

uint32_t
SbString::hash(const char *s)
{
    uint32_t	total, shift;

    total = shift = 0;
    while (*s) {
	total = total ^ ((*s) << shift);
	shift+=5;
	if (shift>24) shift -= 24;
	s++;
    }

    return( total );
}

//
// Sets string to be the empty string (""). If freeOld is TRUE
// (default), any old storage is freed up.
//

void
SbString::makeEmpty(SbBool freeOld)
{
    if (string != staticStorage) {
	if (freeOld)
	    delete [] string;
	string = staticStorage;
    }
    string[0] = '\0';
}

//
// Returns new string representing given sub-string. If endChar is
// -1 (the default), the sub-string from startChar until the end
// is returned.
//

SbString
SbString::getSubString(int startChar, int endChar) const
{
    int		len = getLength();

    // Get substring that starts at specified character
    SbString	tmp = &string[startChar];

    // Delete characters from end if necessary
    if (endChar >= 0 && endChar < len - 1)
	tmp.deleteSubString(endChar - startChar + 1);

    return tmp;
}

//
// Deletes the specified characters from the string. If endChar is
// -1 (the default), all characters from startChar until the end
// are deleted.
//

void
SbString::deleteSubString(int startChar, int endChar)
{
    int		len = getLength();

    // Modify string in place
    if (endChar < 0 || endChar >= len - 1)
	string[startChar] = '\0';
    else {

#ifdef DEBUG
	if (startChar > endChar) {
	    SoDebugError::post("SbString::deleteSubString",
			       "startChar > endChar");
	    return;
	}
#endif

	int	i, numToMove = len - endChar - 1;

	for (i = 0; i < numToMove; i++)
	    string[startChar + i] = string[endChar + i + 1];
	string[startChar + numToMove] = '\0';
    }

    // Use temporary string to allow us to free up old storage if necessary
    SbString	tmp = string;
    *this = tmp;
}

//
// Assignment operator for character string
//

SbString &
SbString::operator =(const char *str)
{
    int size = (str == NULL ? 0 : strlen(str)) + 1;

    // If the string we are assigning to this is a pointer into the
    // string already in this, we have to make sure we don't step on
    // the old string.
    if (str >= string &&
	str < string + (string != staticStorage ? storageSize :
			SB_STRING_STATIC_STORAGE_SIZE)) {

	SbString tmp = str;
	*this = tmp;
	return *this;
    }

    // If there's enough room in the static storage, use it. First,
    // free up string if it was allocated.
    if (size < SB_STRING_STATIC_STORAGE_SIZE) {
	if (string != staticStorage)
	    makeEmpty();
    }

    // If we were using the static storage, we have to allocate a new string
    else if (string == staticStorage)
	string = new char[size];

    // Otherwise, if there is not enough room in the currently
    // allocated string, allocate a new one. If there is, use it again.
    else if (size > storageSize) {
	delete [] string;
	string = new char[size];
    }

    // Copy away!
    strcpy(string, str);
    storageSize = size;
    return *this;
}

//
// Concatenation operator "+=" for string
//

SbString &
SbString::operator +=(const char *str)
{
    if (str != NULL) {
	expand(strlen(str));
	strcat(string, str);
    }
    return *this;
}

//
// Concatenation operator "+=" for SbString
//

SbString &
SbString::operator +=(const SbString &str)
{
    (*this) += str.getString();
    return *this;
}

//
// Equality operator for SbString/char* and SbString/SbString comparison
//

int
operator ==(const SbString &str, const char *s)
{
    return (str.string[0] == s[0] && ! strcmp(str.string, s));
}

//
// Inequality operator for SbString/char* and SbString/SbString comparison
//

int
operator !=(const SbString &str, const char *s)
{
    return (str.string[0] != s[0] || strcmp(str.string, s));
}
