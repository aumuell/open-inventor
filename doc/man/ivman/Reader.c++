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

#include <stdlib.h>
#include <ctype.h>
#include "Man.h"

/////////////////////////////////////////////////////////////////////////////
//
// Class: Reader
//
/////////////////////////////////////////////////////////////////////////////

FILE		*Reader::fp;
int		 Reader::lineNum;
char		*Reader::fullName = NULL;

#define BUFSIZE	1024

SbBool
Reader::open(const char *pathName)
{
    if (fullName != NULL)
	free(fullName);

    fullName = strdup(pathName);

    fp = fopen(fullName, "r");

    if (fp == NULL) {
	fprintf(stderr, "Couldn't open file \"%s\"\n", fullName);
	return FALSE;
    }

    lineNum = 1;

    return TRUE;
}

SbBool
Reader::open(const char *dirName, const char *fileName)
{
    SbString	name = dirName;

    name += "/";
    name += fileName;

    return open(name.getString());
}

void
Reader::error(const char *what)
{
    fprintf(stderr, "Error reading %s on line %d of \"%s\"\n",
	    what, lineNum, fullName);
}

SbBool
Reader::readName(SbName &name)
{
    char	buf[132];

    if (! readBuf(buf, TRUE))
	return FALSE;

    name = buf;
    return TRUE;
}

SbBool
Reader::readString(SbString &string)
{
    char	buf[2048];

    if (! readBuf(buf, FALSE))
	return FALSE;

    string = buf;
    return TRUE;
}

SbBool
Reader::readBracketed(SbString &string)
{
    char	buf[BUFSIZE];
    int		c, c2, i = 0;

    string.makeEmpty();

    skipWhiteSpace();

    // Read open brace character
    if (get() != OPEN_CHAR)
	return FALSE;

    skipWhiteSpace();

    // Read characters into buffer until close brace or EOF
    while ((c = get()) != EOF && c != CLOSE_CHAR) {

	// Check for backslash-open/close sequence
	if (c == '\\') {
	    if ((c2 = get()) == OPEN_CHAR || c2 == CLOSE_CHAR)
		c = c2;
	    else
		putBack(c2);
	}

	buf[i++] = c;

	// If the buffer is full, add it to the string and get ready
	// for more
	if (i == BUFSIZE - 1) {
	    buf[i] = '\0';
	    string += buf;
	    i = 0;
	}
    }
	   
    if (c != CLOSE_CHAR)
	return FALSE;

    // Add rest of buffer to string
    if (i > 0) {
	buf[i] = '\0';
	string += buf;
    }

    return TRUE;
}

SbBool
Reader::readUpTo(int endChar, SbString &string)
{
    char	buf[BUFSIZE];
    int		c, i = 0;

    string.makeEmpty();

    skipWhiteSpace();

    // Read characters into buffer until given char or EOF
    while ((c = get()) != EOF && c != endChar) {

	buf[i++] = c;

	// If the buffer is full, add it to the string and get ready
	// for more
	if (i == BUFSIZE - 1) {
	    buf[i] = '\0';
	    string += buf;
	    i = 0;
	}
    }
	   
    if (c != endChar)
	return FALSE;

    putBack(c);

    // Add rest of buffer to string
    if (i > 0) {
	buf[i] = '\0';
	string += buf;
    }

    return TRUE;
}

SbBool
Reader::readBuf(char *buf, SbBool isName)
{
    int	c;

    skipWhiteSpace();

    // See if stuff is quoted
    if ((c = get()) == QUOTE_CHAR) {
	char	*b = buf;

	c = get();

	while (c != EOF && c != QUOTE_CHAR) {
	    *b++ = c;
	    c = get();
	}
	*b = '\0';

	if (c != QUOTE_CHAR)
	    return FALSE;
    }

    else {
	char	*b = buf;

	putBack(c);

	// If reading a name, stop at a separator character
	while ((c = get()) != EOF && ! isspace(c)) {
	    if (isName && c == SEP_CHAR)
		break;
	    *b++ = c;
	}
	*b = '\0';

	if (b == buf)
	    return FALSE;

	if (c != EOF)
	    putBack(c);
    }

    return TRUE;
}

void
Reader::skipWhiteSpace()
{
    int	c;

    while (1) {

	// Skip over space characters
	while ((c = get()) != EOF && isspace(c))
	    ;

	// Check for comment just after the space
	if (c == COMMENT_CHAR) {

	    // Skip all characters up to next newline
	    while ((c = get()) != EOF && c != '\n')
		;
	}

	// No comment? Stop!
	else
	    break;
    }

    // Put the last character (if any) back
    if (c != EOF)
	putBack(c);
}

int
Reader::get()
{
    int c = getc(fp);

    if (c == '\n')
	lineNum++;

    return c;
}

void
Reader::putBack(int c)
{
    if (c == '\n')
	lineNum--;

    ungetc(c, fp);
}

void
Reader::cleanString(SbString &string, SbBool inCode, SbBool changeSbName)
{
    const char	*s = string.getString();
    int		len = string.getLength();
    char	realbuf[2048], *buf, *b;
    SbString	cleanString;


    if (len >= 2048)
	buf = new char[len + 1];
    else
	buf = realbuf;

    // Remove leading spaces
    while (isspace(*s))
	s++;

    b = buf;

    while (*s != '\0') {

	// Copy non-space stuff
	while (*s != '\0' && ! isspace(*s))
	    *b++ = *s++;

	// Condense spaces
	if (isspace(*s)) {
	    // Don't allow spaces after open parens in code
	    if (inCode && b > buf && *(b-1) == '(')
		;
	    else
		*b++ = ' ';
	    s++;
	    while (isspace(*s))
		s++;
	}
    }

    // Remove trailing space, if any
    if (b > buf && *(b - 1) == ' ')
	b--;

    // Terminate string
    *b = '\0';

    // Replace "const SbName &" with "const char *" in the C version
    // but only if the class name is not SbName.
    if ((Writer::getLanguage() == C) && changeSbName &&
        (ClassDef::getCurClass()->getName() != "SbName")) {
	for (b = buf; *b != '\0'; b++)
	    if ( !strncmp(b,"const SbName &", 14) ) {
		b[0] = 'c';
		b[1] = 'o';
		b[2] = 'n';
		b[3] = 's';
		b[4] = 't';
		b[5] = ' ';
		b[6] = 'c';
		b[7] = 'h';
		b[8] = 'a';
		b[9] = 'r';
		b[10]= ' ';
		b[11]= '*';
		b[12]= ' ';
		b[13]= ' ';
		// We just created two extra spaces at the end. So we
		// need to move everything over.
		char *q = b+12;
		while (*q != '\0') {
		    *q = q[2];
		    q++;
		}
	    }
    }

    // Replace references (&) with pointers (*) in the C version
    if (Writer::getLanguage() == C)
	for (b = buf; *b != '\0'; b++)
	    if (*b == '&')
		*b = '*';

    string = buf;

    if (len >= 2048)
	delete [] buf;
}
