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
 |   $Revision: 1.1 $
 |
 |   ppp (Paul's preprocessor): a tool for preprocessing source files.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

/////////////////////////////////////////////////////////////////////////////
//
//  usage: ppp < input > output
//
//  This program preprocesses source files, performing text
//  substitution of variables. It is designed to be very simple. All
//  input processing is line-based.
//
//  Lines beginning with "@" (as the first non-space character) are
//  special to the preprocessor, according to these rules:
//
//		@@ any text
// 
// This denotes a comment - it is ignored and does not get sent to the
// output.
// 
//		@= variableName text
// 
//  This defines a single-line variable. If "text" is missing, the
//  variable definition is removed.
// 
//		@={ variableName
//		    lines of text
//		@=}
// 
//  This defines a multiple-line variable.
// 
//		@ variableName
// 
//  This substitutes the definition of the named variable. If this is
//  done within another definition, the substitution occurs when the
//  definition is used.
// 
//		@! variableName
// 
//  This also substitutes the definition, but it occurs immediately,
//  even if it's within a definition.
//
//		@?{ variableName | variableName ...
//		    lines of text
//		@?}
//
//  This inserts text conditionally. If any of "variableNames" are
//  defined, the text is inserted. Otherwise, it is ignored. This may
//  be used inside or outside of definitions. Conditions may be
//  nested.
//
//
//  Note that indentation is preserved during substitution;
//  indentation before the variable use will be added to whatever
//  indentation is in the definition. Leading whitespace in the first
//  line within a definition is removed; the equivalent amount is
//  removed from all other lines in the definition, if possible.
//
/////////////////////////////////////////////////////////////////////////////

#include <inttypes.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// There is a nasty cyclic make dependency if we try to use libdatabase.a,
// so instead just include the following code directly:
#undef DEBUG
#include "../../lib/database/src/sb/Sb.c++"

// Special characters
#define MAGIC_CHAR		'@'
#define DEFINITION_CHAR		'='
#define IMMEDIATE_CHAR		'!'
#define CONDITIONAL_CHAR	'?'
#define BEGIN_CHAR		'{'
#define END_CHAR		'}'

// Possible types of input lines
enum LineType {
    COMMENT,
    BEGIN_DEFINITION,
    END_DEFINITION,
    ONE_LINE_DEFINITION,
    USE_VARIABLE,
    USE_VARIABLE_IMMEDIATE,
    BEGIN_CONDITIONAL,
    END_CONDITIONAL,
    OTHER,
};

// A line of input (or of a definition)
struct Line {
    SbString	string;			// String holding line (inc. newline)
    LineType	type;			// Type of line
    SbName	varName;		// Variable named on line (if approp.)
    int		indentation;		// Indentation of line
    int		firstChar;		// Index of 1st non-whitespace char
};

// A variable
struct Variable {
    SbName	name;			// Name of variable
    SbPList	definition;		// One Line per line of
					// definition
    Variable	*prev;			// The previous definition for
					// nested definitions.
};

static const char	*progName;
static int		lineNo;

// Variable whose definition we are currently adding to
static Variable		*curVariable = NULL;

// This is set to TRUE when ignoring lines that are in a conditional
// text block that failed the test. It is a stack so we can restore it
// properly. The top of the stack is in curIgnore.
static SbBool		ignoreText[1000] = { FALSE };
static int		curIgnore = 0;

// Holds dictionary relating variables to names
static SbDict		*varDict;

//////////////////////////////////////////////////////////////////////////////
//
//  Prints error message and exits.
//
//////////////////////////////////////////////////////////////////////////////

static void
error(const char *message)
{
    fprintf(stderr, "%s: %s - at line %d\n", progName, message, lineNo);

    exit(1);
}

//////////////////////////////////////////////////////////////////////////////
//
//  Stores in the "varName" field of the given Line instance the
//  variable name from a line that defines or uses a variable. For
//  example, given "@{ foobar \n" or "@foobar \n, this will store
//  "foobar".
//
//////////////////////////////////////////////////////////////////////////////

static void
storeVariableName(Line &line)
{
    char	buf[256], *b;
    const char	*c;

    // Move past special characters and space after them
    for (c = line.string.getString(); *c != MAGIC_CHAR; c++) ;
    c++;
    if (*c == DEFINITION_CHAR) {
	c++;
	if (*c == BEGIN_CHAR || *c == END_CHAR)
	    c++;
    }
    else if (*c == IMMEDIATE_CHAR)
	c++;
    for ( ; isspace(*c); c++) ;

    // Copy characters until next space into buffer
    b = buf;
    while (! isspace(*c))
	*b++ = *c++;
    *b++ = '\0';

    line.varName = buf;
}

//////////////////////////////////////////////////////////////////////////////
//
//  Figures out the type of the given Line, storing it in the "type"
//  field. This also stores the indentation of the line (the number of
//  spaces up to the first non-whitespace character) in "indentation".
//  If appropriate, it also stores the variable name in the "varName"
//  field.
//
//////////////////////////////////////////////////////////////////////////////

static void
storeLineType(Line &line)
{
    const char	*c;

    // Find first non-space character, saving indentation
    line.indentation = 0;
    for (c = line.string.getString(); isspace(*c); c++)
	line.indentation += (*c == '\t' ? 8 : 1);
    line.firstChar = c - line.string.getString();

    // If it's not a special character, we're done
    if (*c != MAGIC_CHAR)
	line.type = OTHER;

    // Check the next character
    else {
	switch (*(c + 1)) {

	  case MAGIC_CHAR:
	    line.type = COMMENT;
	    break;

	  case DEFINITION_CHAR:
	    if (*(c + 2) == BEGIN_CHAR)
		line.type = BEGIN_DEFINITION;
	    else if (*(c + 2) == END_CHAR)
		line.type = END_DEFINITION;
	    else
		line.type = ONE_LINE_DEFINITION;
	    if (line.type != END_DEFINITION)
		storeVariableName(line);
	    break;

	  case IMMEDIATE_CHAR:
	    line.type = USE_VARIABLE_IMMEDIATE;
	    storeVariableName(line);
	    break;

	  case CONDITIONAL_CHAR:
	    if (*(c + 2) == BEGIN_CHAR)
		line.type = BEGIN_CONDITIONAL;
	    else if (*(c + 2) == END_CHAR)
		line.type = END_CONDITIONAL;
	    else
		error("Invalid conditional operator");
	    break;

	  default:
	    line.type = USE_VARIABLE;
	    storeVariableName(line);
	    break;
	}
    }
}

//////////////////////////////////////////////////////////////////////////////
//
// Returns the variable with the given name, or NULL if there's none.
//
//////////////////////////////////////////////////////////////////////////////

static Variable *
findVariable(const SbName &name)
{
    void	*varPtr;

    varDict->find((uint32_t) name.getString(), varPtr);

    return (Variable *) varPtr;
}

//////////////////////////////////////////////////////////////////////////////
//
// Removes variable with given name, if any.
//
//////////////////////////////////////////////////////////////////////////////

static void
removeVariable(const SbName &name)
{
    Variable *oldVar = findVariable(name);

    if (oldVar != NULL)
	delete oldVar;

    varDict->remove((uint32_t) name.getString());
}

//////////////////////////////////////////////////////////////////////////////
//
// Adds the given variable. If there already was one with that name,
// it is deleted first.
//
//////////////////////////////////////////////////////////////////////////////

static void
addVariable(Variable *var)
{
    removeVariable(var->name);
    varDict->enter((uint32_t) var->name.getString(), var);
}

//////////////////////////////////////////////////////////////////////////////
//
// Returns TRUE if we are in the middle of defining a variable.
//
//////////////////////////////////////////////////////////////////////////////

static SbBool
inDefinition()
{
    return (curVariable != NULL);
}

//////////////////////////////////////////////////////////////////////////////
//
// Begins the definition of the variable named on the given line.
//
//////////////////////////////////////////////////////////////////////////////

static void
beginDefinition(const Line &line)
{
    // Create a new variable
    Variable	*var = new Variable;

    var->name = line.varName;

    // Enter into dictionary
    addVariable(var);

    // Set curVariable to indicate we are defining a variable
    var->prev = curVariable;
    curVariable = var;
}

//////////////////////////////////////////////////////////////////////////////
//
// Adds to the definition of the current variable.
//
//////////////////////////////////////////////////////////////////////////////

static void
addToDefinition(const Line &line, int indentOffset)
{
    Line	*defLine = new Line;

    *defLine = line;
    defLine->indentation += indentOffset;

    curVariable->definition.append(defLine);
}

//////////////////////////////////////////////////////////////////////////////
//
// Ends the definition of the current variable.
//
//////////////////////////////////////////////////////////////////////////////

static void
endDefinition()
{
    Variable *save = curVariable;
    curVariable = save->prev;
    save->prev = NULL;
}

//////////////////////////////////////////////////////////////////////////////
//
// Processes a one-line definition.
//
//////////////////////////////////////////////////////////////////////////////

static void
processOneLineDefinition(const Line &line)
{
    // Find the definition part on the line, after the name of the variable
    const char	*c;

    // Move past special characters and space after them
    for (c = line.string.getString(); *c != MAGIC_CHAR; c++) ;
    c += 2;
    for ( ; isspace(*c); c++) ;

    // Move past characters of variable name and space after (except
    // ending newline)
    for ( ; ! isspace(*c); c++) ;
    for ( ; isspace(*c) && *c != '\n'; c++) ;

    // Create a new line that contains the rest of the definition, if
    // it's not empty
    if (*c != '\n') {
	beginDefinition(line);

	Line	defLine;
	defLine.string      = c;
	defLine.type        = OTHER;
	defLine.indentation = 0;
	defLine.firstChar   = 0;

	addToDefinition(defLine, 0);

	endDefinition();
    }

    // Otherwise, there's no definition, so remove it
    else
	removeVariable(line.varName);
}

//////////////////////////////////////////////////////////////////////////////
//
// Prints given string, with given amount of indentation first.
//
//////////////////////////////////////////////////////////////////////////////

static void
printWithIndentation(const char *string, int indentation)
{
    // Special case for empty strings
    if (string[0] == '\0')
	putchar('\n');

    else {
	// Add extra indentation, using tabs when possible
	while (indentation >= 8) {
	    putchar('\t');
	    indentation -= 8;
	}

	// Use spaces for rest
	while (indentation > 0) {
	    putchar(' ');
	    indentation--;
	}

	// Print things this way to avoid problems with formatting
	// characters (like '%') in the string itself
	printf("%s", string);
    }
}

//////////////////////////////////////////////////////////////////////////////
//
// Substitutes the definition of the variable named on the given line,
// printing out the result of the substitution. Recursive, if
// necessary.
//
//////////////////////////////////////////////////////////////////////////////

// Forward reference
static void processNonDefLine(const Line &line, int indentOffset,
			      SbBool immediate);

static void
substitute(const Line &line, int indentOffset, SbBool immediate)
{
    // Look up definition
    Variable	*variable = findVariable(line.varName);

    if (variable == NULL)
	error("No such variable");

    // Indentation of first line that is not a conditional text
    // delimiter is removed from indentation of all lines within
    // definition
    int		firstIndentation, i;
    SbBool	gotFirstLine = FALSE;

    for (i = 0; i < variable->definition.getLength(); i++) {
	const Line &line = * (const Line *) variable->definition[i];

	if (! gotFirstLine && (line.type != BEGIN_CONDITIONAL &&
			       line.type != END_CONDITIONAL)) {
	    firstIndentation = line.indentation;
	    gotFirstLine = TRUE;
	}

	processNonDefLine(line, indentOffset - firstIndentation, immediate);
    }
}

//////////////////////////////////////////////////////////////////////////////
//
// Returns TRUE if the given conditional line will reject text
//
//////////////////////////////////////////////////////////////////////////////

static SbBool
shouldIgnore(const Line &line)
{
    SbBool result = TRUE;
    char *tmp = strdup(line.string.getString()+3);
    char *t;

    t = strtok(tmp, "| \t\n");
    while (t && result) {
	if (findVariable(SbName(t))) result = FALSE;
	t = strtok(NULL, "| \t\n");
    }
    free(tmp);

    return result;
}

//////////////////////////////////////////////////////////////////////////////
//
// Processes a line of input or a line of an expanded variable
// definition. Prints out results or adds to current definition.
// The indentation offset for the line is passed in.
//
//////////////////////////////////////////////////////////////////////////////

static void
processNonDefLine(const Line &line, int indentOffset, SbBool immediate)
{
    // If we are in rejected conditional text, skip this line
    if (ignoreText[curIgnore]) {
	if (line.type == BEGIN_CONDITIONAL)
	    ignoreText[++curIgnore] = TRUE;
	else if (line.type == END_CONDITIONAL)
	    --curIgnore;
    }

    // If line contains the use of a variable, substitute for it,
    // unless we are in a definition
    else if (line.type == USE_VARIABLE_IMMEDIATE ||
	(line.type == USE_VARIABLE && (immediate || ! inDefinition())))
	substitute(line, line.indentation + indentOffset,
		   immediate || line.type == USE_VARIABLE_IMMEDIATE);

    // Add to current definition if one is open
    else if (inDefinition())
	addToDefinition(line, indentOffset);

    // If we're supposed to start ignoring text, do so
    else if (line.type == BEGIN_CONDITIONAL)
	ignoreText[++curIgnore] = shouldIgnore(line);

    // Skip over the end line for non-ignored text
    else if (line.type == END_CONDITIONAL)
	curIgnore--;

    // Otherwise, print the line as is
    else
	printWithIndentation(line.string.getString() + line.firstChar,
			     line.indentation + indentOffset);
}

//////////////////////////////////////////////////////////////////////////////
//
// Checks for definitions of variables on a line of input. If there is
// a definition, it begins the definition. Otherwise it adds to the
// current definition or to the output.
//
//////////////////////////////////////////////////////////////////////////////

static void
processLine(const Line &line)
{
    switch (line.type) {

      case COMMENT:
	// Do nothing
	break;

      case BEGIN_DEFINITION:
	beginDefinition(line);
	break;

      case END_DEFINITION:
	if (! inDefinition())
	    error("Not defining variable");
	endDefinition();
	break;

      case ONE_LINE_DEFINITION:
	processOneLineDefinition(line);
	break;

      case USE_VARIABLE:
      case USE_VARIABLE_IMMEDIATE:
      case BEGIN_CONDITIONAL:
      case END_CONDITIONAL:
      case OTHER:
	processNonDefLine(line, 0, FALSE);
    }
}

//////////////////////////////////////////////////////////////////////////////
//
// Mainline
//
//////////////////////////////////////////////////////////////////////////////

main(int, char *argv[])
{
    char	buf[1024];
    Line	line;

    progName = argv[0];
    lineNo   = 1;

    varDict = new SbDict;

    while(fgets(buf, 1024, stdin) != NULL) {
	line.string = buf;
	storeLineType(line);
	processLine(line);
	lineNo++;
    }

    return 0;
}
