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

#include <ctype.h>
#include "Man.h"

/////////////////////////////////////////////////////////////////////////////
//
// Class: Writer
//
/////////////////////////////////////////////////////////////////////////////

FILE		*Writer::fp = NULL;
char		*Writer::fullName = NULL;
char		*Writer::sourceFileName = NULL;
SbBool		 Writer::inSynopsis = FALSE;
Language	 Writer::language = C_PLUS_PLUS;
SbBool		 Writer::forBook = FALSE;
float		 Writer::baseIndent = 0.0;
int		 Writer::numTabChars = -1;
SbBool		 Writer::indentedForTab = FALSE;

SbBool
Writer::open(const char *dirName, const char *fileName)
{
    SbString	name = dirName;

    name += "/";
    name += fileName;

    fullName = strdup(name.getString());

    fp = fopen(fullName, "w");

    if (fp == NULL) {
	fprintf(stderr, "Couldn't open file \"%s\" for writing\n", fullName);
	return FALSE;
    }

    return TRUE;
}

void
Writer::writePreface(const SbName &className)
{
    // These must be the first 2 lines in man pages. Tbl is necessary
    // for the nodekit part tables.
    write("'\\\"! tbl | mmdoc\n"
	  "'\\\"macro stdmacro\n");

    //
    // Write strings/macros that are used within man page
    //

    // Define code fonts. "Cr" is Roman code font, "Cb" is bold code
    // font. Use different stuff for nroff/troff.
    write(".ie n \\{\\\n"
	  ".   ds Cr \\fB\n"
	  ".   ds Cb \\fB\n"
	  ".\\}\n"
	  ".el \\{\\\n"
	  ".   ds Cr \\f7\n"
	  ".   ds Cb \\f8\n"
	  ".\\}\n");

    // Write man page header
    write(".TH ");
    write(className);
    write("(3IV)");
    write("\n");
}

void
Writer::startSection(const char *name)
{
    write(".SH ");
    write(name);
    write("\n");

    baseIndent = 0.0;
}

void
Writer::setFont(Font newFont)
{
    switch (newFont) {

      case TEXT_ROMAN:
	write("\\f1");
	break;
      case TEXT_ITALIC:
	write("\\f2");
	break;
      case TEXT_BOLD:
	write("\\f3");
	break;

      case CODE_ROMAN:
	if (isForBook())
	    write("\\f1");
	else
	    write("\\*(Cr");
	break;
      case CODE_BOLD:
	if (isForBook())
	    write("\\f3");
	else
	    write("\\*(Cb");
	break;

      case FIXED_ROMAN:
	write("\\*(Cr");
	break;
      case FIXED_BOLD:
	write("\\*(Cb");
	break;

      case PREVIOUS:
	write("\\fP");
	break;
    }
}

void
Writer::changeSize(float diff)
{
    char buf[32];

    sprintf(buf, "%g", diff);
    if (diff > 0)
	write(".ps +");
    else
	write(".ps ");
    write(buf);
    write("\n");
}

void
Writer::setBaseIndent(float indInInches)
{
    char buf[32];

    baseIndent = indInInches;

    // Indentation value = .5i + given value
    sprintf(buf, ".in %gi\n", baseIndent + 0.5);
    write(buf);
}

void
Writer::setIndent(int numChars, int firstLineChars)
{
    char buf[32];

    // Indentation value = .5i + given value
    sprintf(buf, ".in %dn+.5i\n", numChars);
    write(buf);
    sprintf(buf, ".ti %dn+.5i\n", firstLineChars);
    write(buf);
}

void
Writer::setTab(int _numTabChars)
{
    numTabChars = _numTabChars;

    if (numTabChars >= 0) {
	char	buf[32];

	// Set tab to number of characters (tabs are relative to current
	// indentation, so we don't have to add it in)
	if (isForBook())
	    sprintf(buf, "%gm", (float) numTabChars * .6);
	else
	    sprintf(buf, "%dm", numTabChars);
	write(".ta ");
	write(buf);
	write("\n");
    }
}

void
Writer::indentForTab()
{
    char	buf[32];

    // Set indentation to base indent + number of characters + .5
    // inches (for indent offset) + .5 inches (for extra indentation)
    sprintf(buf, "%gi+%dn", baseIndent + 1.0, numTabChars);
    write(".in ");
    write(buf);
    write("\n");

    // Set indentation on first line to be base indent
    sprintf(buf, "%gi", baseIndent + 0.5);
    write(".ti ");
    write(buf);
    write("\n");

    // Reset tab stop, so it gets current indentation
    setTab(numTabChars);

    // Save the pre-tab stuff in a string so we can test its
    // length when we output the tab
    write(".ds Pt ");

    indentedForTab = TRUE;
}

void
Writer::tab()
{
    if (indentedForTab) {
	char	buf[132];

	//
	// NOTE: Do not try to clean any of this up. Troff is very
	// unpredictable, and this is the only permutation I could
	// find that didn't insert extra spaces or misalign tabs
	// anywhere.
	//

	// End the string containing the pre-tab stuff
	write("\n");

	// If the pre-tab stuff is wider than the tab space, set the
	// indentation for the next line to the base indentation.
	// Added .ne's to prevent bad page breaks, added \\ after
	// .ie and .el to eliminate extra linebreaks -- AE
	sprintf(buf,
		".ie \\w'\\*(Pt'>=%dn \\{\\\n"
		".ne 3\n"
		"\\*(Pt\n"
		".ti %gi\n"
		"\t\\c\\\n"
		"\\}\n"
		".el\\{\\\n"
		".ne 2\n"
		"\\*(Pt\t\\c\\\n"
		"\\}\n",
		numTabChars, baseIndent + .5);
	write(buf);

	indentedForTab = FALSE;
    }

    else
	write("\t");
}

void
Writer::setFill(SbBool flag)
{
    if (flag)
	write(".fi\n");
    else
	write(".nf\n");
}

void
Writer::breakLine()
{
    write(".br\n");
}

void
Writer::space(float amt)
{
    if (amt == 1.0)
	write(".sp\n");
    else {
	char buf[32];
	sprintf(buf, ".sp %g\n", amt);
	write(buf);
    }
}

void
Writer::write(const char *string)
{
    fprintf(fp, "%s", string);
}

void
Writer::write(const SbName &name)
{
    fprintf(fp, "%s", name.getString());
}

void
Writer::write(const SbString &string)
{
    fprintf(fp, "%s", string.getString());
}

void
Writer::writeBracketed(const SbString &string)
{
    const char	*s = string.getString();

    // Remove leading spaces
    while (isspace(*s))
	s++;

    writeStuff(s, TRUE, '\0');

    // Write out a newline
    putc('\n', fp);
}

void
Writer::writeDescription(const SbString &string)
{
    breakLine();
    setBaseIndent(0.5);
    setFont(Writer::TEXT_ROMAN);
    writeBracketed(string);
    space();
    setBaseIndent(0.0);
}

const char *
Writer::writeStuff(const char *string, SbBool compressSpace, int blockChar)
{
    const char *s = string;
    SbBool	inSpace = FALSE;
    int		curChar = 0;

    // Have to check for special character sequences
    while (*s != '\0') {

	// Check for special characters
	while (*s == '\\') {

	    // Check for end of block
	    if (blockChar != '\0' && *(s+1) == '.')
		return endBlock(s, blockChar);

	    // Check for beginning of new block
	    else
		s = startBlock(s, compressSpace);

	    inSpace = FALSE;
	}

	// Treat spaces specially
	if (compressSpace) {
	    if (isspace(*s)) {
		if (! inSpace) {
		    // Insert a newline every once in a while just to
		    // keep troff happy - it has some problems with
		    // really long lines
		    if (curChar > 200 && *(s+1) != '\0' && *(s+1) != '\\') {
			putc('\n', fp);
		    //  XXX This is ugly, but keeps us from getting bit
	 	    //  by an ugly bug, whereby we wrap at a significant
		    //  character like ' or . thereby causing troff to
		    //  eat text. Should just test for all significant
		    //  characters above, but I can't remember them all offhand.
		    //  The non-printing character (\&) wards off these evil 
		    //  control characters.
		    //  AE
			write("\\&");
			curChar = 0;
		    }
		    else
			putc(' ', fp);
		    inSpace = TRUE;
		}
	    }

	    else {
		inSpace = FALSE;
		if (*s != '\0')
		    putc(*s, fp);
	    }
	    curChar++;
	}
	else
	    if (*s != '\0')
		putc(*s, fp);

	if (*s != '\0')
	    s++;
    }

    return s;
}

const char *
Writer::startBlock(const char *string, SbBool compressSpace)
{
    // We know that *s is a backslash, and that *(s+1) is not a .
    const char *s = string + 1;

    switch (*s) {

      case 'a':			// Argument
	setFont(isForBook() ? TEXT_ITALIC : CODE_ROMAN);
	s = writeStuff(s + 1, compressSpace, 'a');
	break;

      case 'b':			// Indented, unfilled block
	write("\n");
	setFill(FALSE);
	setBaseIndent(baseIndent + .5);
	s = writeStuff(s + 1, FALSE, 'b');
	break;

      case 'c':			// Class name
	setFont(CODE_BOLD);
	s = writeStuff(s + 1, compressSpace, 'c');
	break;

      case 'e':			// Emphasis
	setFont(TEXT_ITALIC);
	s = writeStuff(s + 1, compressSpace, 'e');
	break;

      case 'k':			// Other kind of kode
	setFont(CODE_ROMAN);
	s = writeStuff(s + 1, compressSpace, 'k');
	break;

      case 'm':			// Method name
	setFont(CODE_BOLD);
	s = writeStuff(s + 1, compressSpace, 'm');
	break;

      case 'p':			// Begin new paragraph (Not ended by \.)
	write("\n");
	space();
	s++;
	// Skip over white space at the start of the next paragraph
	while (isspace(*s))
	    s++;
	break;

      case 'v':			// Variable name
	setFont(CODE_BOLD);
	s = writeStuff(s + 1, compressSpace, 'v');
	break;

      case 'x':			// External man page or method reference
	setFont(TEXT_BOLD);
	s = writeStuff(s + 1, compressSpace, 'x');
	break;

      case '+':			// C++ only
	if (language != C_PLUS_PLUS)
	    s = skipBlock(s + 1);
	else
	    s = writeStuff(s + 1, compressSpace, '+');
	break;

      case '-':			// C only
	if (language != C)
	    s = skipBlock(s + 1);
	else
	    s = writeStuff(s + 1, compressSpace, '-');
	break;

      default:
	// Write out the special characters
	putc(*string, fp);
	putc(*(string + 1), fp);
	s++;
	break;
    }

    return s;
}

const char *
Writer::endBlock(const char *string, int blockChar)
{
    // We know that *s is a backslash, and that *(s+1) is a .
    const char *s = string + 2;

    switch (blockChar) {
      case 'b':			// Indented, unfilled block
	write("\n");
	setFill(TRUE);
	setBaseIndent(baseIndent - .5);
	// Skip over extra spaces
	while (isspace(*s))
	    s++;
	break;

      case 'a':			// Any other font
      case 'c':
      case 'e':
      case 'k':
      case 'm':
      case 'v':
      case 'x':
	setFont(TEXT_ROMAN);
	break;

      case '+':			// C++ only
      case '-':			// C++ only
	// Nothing to do
	break;

      default:
	fprintf(stderr, "Internal error - ending block of type '%c'\n",
		blockChar);
	break;
    }

    return s;
}

const char *
Writer::skipBlock(const char *string)
{
    // Skip to end of block, including blocks nested in this one
    const char *s = string;

    while (TRUE) {
	while (*s != '\0' && *s != '\\')
	    s++;

	if (*s == '\\') {
	    s++;
	    if (*s == '.')
		return s + 1;
	    // Skip character after backslash since there's no ending delimiter
	    else if (*s == 'p')
		s++;
	    else 
		// if (*s == 'b' || *s == 'c' || *s == '+' || *s == '-')
		s = skipBlock(s + 1);
	    //	    else
	    //		s++;	// Skip character after backslash
	}
	else {
	    fprintf(stderr, "\nERROR - %s reach end of file ! (block not terminated)\n", 
		sourceFileName);
	    fprintf(stderr, "problem at: %s\n", string);
	    return s;
	}
    }
}

void
Writer::finish()
{
    fflush(fp);
}
