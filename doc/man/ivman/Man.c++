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
// Class: Item
//
/////////////////////////////////////////////////////////////////////////////

Language	Item::curLanguage = ALL;

Item::Item()
{
    language = curLanguage;

    next = NULL;
}

SbBool
Item::hasNextToWrite()
{
    Item	*item;

    for (item = next; item != NULL; item = item->next)
	if (item->language == ALL || item->language == Writer::getLanguage())
	    return TRUE;

    return FALSE;
}

int
Item::getNumPreTabChars() const
{
    return -1;		// No need to set up tab
}

/////////////////////////////////////////////////////////////////////////////
//
// Class: ItemList
//
/////////////////////////////////////////////////////////////////////////////

SbBool
ItemList::isEmpty() const
{
    SbBool empty = TRUE;

    for (Item *item = first; item != NULL; item = item->next) {
	if (item->language == ALL || item->language == Writer::getLanguage()) {
	    empty = FALSE;
	    break;
	}
    }

    return empty;
}

void
ItemList::add(Item *item, SbBool append)
{
    if (last == NULL)
	first = last = item;

    else if (append) {
	// Add to end of list
	last->next = item;
	last = item;
    }

    else {
	// Add to start of list
	item->next = first;
	first = item;
    }
}

#define MIN_TAB_CHARS	20
#define MAX_TAB_CHARS	30

void
ItemList::write(SbBool runIn)
{
    Item *item;

    // For on-line pages, tab stop depends on longest line before the
    // tab. For the book version, just use a fixed tab stop.
    if (Writer::isForBook())
	Writer::setTab(16);

    else {
	int	numTabChars = -1, maxTabChars = -1, n;
	
	// Compute pre-tab length
	for (item = first; item != NULL; item = item->next) {
	    if (item->language == ALL ||
		item->language == Writer::getLanguage()) {
		
		n = item->getNumPreTabChars();
		
		if (n >= 0) {
		    
		    // Add 2 characters (to get a couple of spaces)
		    n += 2;
		    
		    // Compute maximum number of tab chars
		    if (n > maxTabChars)
			maxTabChars = n;
		    
		    // Compute real tab length (ignoring tabs that will
		    // break to the next line anyway)
		    if (n < MAX_TAB_CHARS && n > numTabChars)
			numTabChars = n;
		}
	    }
	}
	
	// Make sure tab value is within range
	if (maxTabChars >= 0) {
	    if (numTabChars < 20)
		numTabChars = 20;
	    else if (numTabChars > 30)
		numTabChars = 30;
	}
	Writer::setTab(numTabChars);
    }

    for (item = first; item != NULL; item = item->next)
	if (item->language == ALL || item->language == Writer::getLanguage())
	    item->write(runIn);

    if (runIn)
	Writer::space(.75);
}

int
ItemList::getNum() const
{
    Item *item;
    int	 num = 0;

    for (item = first; item != NULL; item = item->next)
	num++;

    return num;    
}

/////////////////////////////////////////////////////////////////////////////
//
// Class: Inherit
//
/////////////////////////////////////////////////////////////////////////////

Inherit::Inherit(const SbName &name)
{
    className = name;
}

SbBool
Inherit::read()
{
    // This should never be called, since it is not read in.
    Reader::error("Inherit::read called - internal error");
    return FALSE;
}

void
Inherit::write(SbBool)
{
    Writer::write(className);
    if (hasNextToWrite())
	Writer::write(" > ");
    else
	Writer::write("\n");
}

/////////////////////////////////////////////////////////////////////////////
//
// Class: Include
//
/////////////////////////////////////////////////////////////////////////////

SbBool
Include::read()
{
    if (! Reader::readString(file)) {
	Reader::error("INCLUDE string");
	return FALSE;
    }
    return TRUE;
}

void
Include::write(SbBool)
{
    Writer::setFont(Writer::FIXED_ROMAN);
    if (Writer::getLanguage() == C)
	Writer::write("#include <Inventor_c/");
    else
	Writer::write("#include <Inventor/");
    Writer::write(file);
    Writer::write(">\n");

    if (hasNextToWrite())
	Writer::breakLine();
    else
	Writer::space();
}

/////////////////////////////////////////////////////////////////////////////
//
// Class: Typedef
//
/////////////////////////////////////////////////////////////////////////////

SbBool
Typedef::read()
{
    if (! Reader::readString(type) ||
	! Reader::readName(name) ||
	! Reader::readString(extra)) {
	Reader::error("TYPEDEF spec");
	return FALSE;
    }
    return TRUE;
}

void
Typedef::write(SbBool)
{
    Writer::indentForTab();
    Writer::setFont(Writer::CODE_ROMAN);
    Writer::write("typedef ");
    Writer::write(type);
    Writer::tab();
    Writer::setFont(Writer::CODE_BOLD);
    Writer::write(name);
    Writer::setFont(Writer::CODE_ROMAN);
    Writer::write(extra);
    Writer::write("\n");
    if (hasNextToWrite())
	Writer::breakLine();
    else
	Writer::space();
}

int
Typedef::getNumPreTabChars() const
{
    return strlen("typedef ") + type.getLength();
}

/////////////////////////////////////////////////////////////////////////////
//
// Class: Define
//
/////////////////////////////////////////////////////////////////////////////

SbBool
Define::read()
{
    if (! Reader::readBracketed(definition)) {
	Reader::error("DEFINE string");
	return FALSE;
    }
    return TRUE;
}

void
Define::write(SbBool)
{
    Writer::setFont(Writer::CODE_ROMAN);
    Writer::write("#define ");
    Writer::write(definition);
    Writer::write("\n");
    if (hasNextToWrite())
	Writer::breakLine();
    else
	Writer::space();
}

/////////////////////////////////////////////////////////////////////////////
//
// Class: EnumDef
//
/////////////////////////////////////////////////////////////////////////////

SbBool
EnumDef::read()
{
    if (! Reader::readName(name) ||
	! Reader::readString(description)) {
	Reader::error("ENUM definition pair");
	return FALSE;
    }
    return TRUE;
}

void
EnumDef::write(SbBool)
{
    Writer::indentForTab();
    Writer::setFont(Writer::CODE_BOLD);
    if (Writer::getLanguage() == C)
	Writer::write(ClassDef::getCurClass()->getCEnumPrefix());
    else {
	Writer::write(ClassDef::getCurClass()->getName());
	Writer::write("::");
    }
    Writer::write(name);
    Writer::setFont(Writer::TEXT_ROMAN);
    Writer::tab();
    Writer::writeBracketed(description);
    Writer::breakLine();
}

int
EnumDef::getNumPreTabChars() const
{
    int	classLength;

    if (Writer::getLanguage() == C)
	classLength = ClassDef::getCurClass()->getCEnumPrefix().getLength();
    else
	classLength = ClassDef::getCurClass()->getName().getLength() + 2;

    return classLength + name.getLength();
}

/////////////////////////////////////////////////////////////////////////////
//
// Class: Enum
//
/////////////////////////////////////////////////////////////////////////////

SbBool
Enum::read()
{
    EnumDef	*def;
    int		c;

    if (! Reader::readName(name)) {
	Reader::error("ENUM name");
	return FALSE;
    }

    // Read name, description pairs within braces
    Reader::skipWhiteSpace();
    if (Reader::get() != OPEN_CHAR) {
	Reader::error("ENUM definition list");
	return FALSE;
    }

    while (TRUE) {

	// Read the next definition pair. There has to be at least 1.
	def = new EnumDef;
	if (! def->read())
	    return FALSE;
	defs.add(def);

	// See if the list is ending
	Reader::skipWhiteSpace();
	if ((c = Reader::get()) == CLOSE_CHAR)
	    break;
	else if (c != SEP_CHAR) {
	    Reader::error("ENUM definition list");
	    return FALSE;
	}
    }

    return TRUE;
}

void
Enum::write(SbBool)
{
    Writer::setBaseIndent(0.0);
    Writer::setFont(Writer::CODE_ROMAN);
    Writer::write("enum ");
    Writer::setFont(Writer::CODE_BOLD);
    if (Writer::getLanguage() == C)
	Writer::write(ClassDef::getCurClass()->getName());
    Writer::write(name);
    Writer::setFont(Writer::CODE_ROMAN);
    Writer::write(" {\n");

    Writer::setBaseIndent(0.5);
    defs.write();
    Writer::setBaseIndent(0.0);

    Writer::setFont(Writer::CODE_ROMAN);
    Writer::write("}\n");
    Writer::space();
}

/////////////////////////////////////////////////////////////////////////////
//
// Class: Field
//
/////////////////////////////////////////////////////////////////////////////

SbBool
Field::read()
{
    if (! readStuff()) {
	Reader::error("FIELD spec");
	return FALSE;
    }
    return TRUE;
}

SbBool
Field::readStuff()
{
    return (Reader::readName(name) &&
	    Reader::readBracketed(description));
}

void
Field::write(SbBool runIn)
{
    if (runIn) {
	Writer::setFont(Writer::CODE_ROMAN);
	Writer::write(name);
	if (hasNextToWrite())
	    Writer::write(", ");
	else
	    Writer::write("\n");
    }

    else {
	Writer::indentForTab();
	Writer::setFont(Writer::CODE_ROMAN);
	Writer::write(type);
	Writer::tab();
	Writer::setFont(Writer::CODE_BOLD);
	Writer::write(name);
	Writer::write("\n");

	// Write the description only if we're not writing the synopsis
	// and there is a non-empty description:
	if (! Writer::getInSynopsis() && ! ! description)
	    Writer::writeDescription(description);

	else {
	    if (hasNextToWrite())
		Writer::breakLine();
	    else
		Writer::space();
	}
    }
}

int
Field::getNumPreTabChars() const
{
    return type.getLength();
}

/////////////////////////////////////////////////////////////////////////////
//
// Class: Input
//
/////////////////////////////////////////////////////////////////////////////

Input::Input()
{
}

SbBool
Input::read()
{
    if (! readStuff()) {
	Reader::error("INPUT spec");
	return FALSE;
    }
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Class: Output
//
/////////////////////////////////////////////////////////////////////////////

Output::Output()
{
}

SbBool
Output::read()
{
    if (! readStuff()) {
	Reader::error("OUTPUT spec");
	return FALSE;
    }
    return TRUE;
}

void
Output::write(SbBool runIn)
{
    if (runIn) {
	Writer::setFont(Writer::CODE_ROMAN);
	Writer::write(name);
	if (hasNextToWrite())
	    Writer::write(", ");
	else
	    Writer::write("\n");
    }
    else {
	Writer::indentForTab();
	Writer::setFont(Writer::CODE_ROMAN);
	Writer::write("(");
	Writer::write(type);
	Writer::write(")");
	Writer::tab();
	Writer::setFont(Writer::CODE_BOLD);
	Writer::write(name);
	Writer::setFont(Writer::CODE_ROMAN);
	Writer::write("\n");

	// Write the description only if we're not writing the synopsis
	// and there is a non-empty description:
	if (! Writer::getInSynopsis() && ! ! description)
	    Writer::writeDescription(description);

	else {
	    if (hasNextToWrite())
		Writer::breakLine();
	    else
		Writer::space();
	}
    }
}

int
Output::getNumPreTabChars() const
{
    return type.getLength() + 2;	// 2 parentheses
}

/////////////////////////////////////////////////////////////////////////////
//
// Class: Part
//
/////////////////////////////////////////////////////////////////////////////

SbBool
Part::read()
{
    if (! Reader::readName(name) ||
	! Reader::readBracketed(description)) {
	Reader::error("PART spec");
	return FALSE;
    }
    return TRUE;
}

void
Part::write(SbBool runIn)
{
    if (runIn) {
	Writer::setFont(Writer::CODE_ROMAN);
	Writer::write(name);
	if (hasNextToWrite())
	    Writer::write(", ");
	else
	    Writer::write("\n");
    }
    else {
	Writer::indentForTab();
	Writer::setFont(Writer::CODE_ROMAN);
	Writer::write("(");
	Writer::write(type);
	Writer::write(")");
	Writer::tab();
	Writer::setFont(Writer::CODE_BOLD);
	Writer::write(name);
	Writer::setFont(Writer::CODE_ROMAN);
	Writer::write("\n");

	// Write the description only if we're not writing the synopsis
	// and there is a non-empty description:
	if (! Writer::getInSynopsis() && ! ! description)
	    Writer::writeDescription(description);

	else {
	    if (hasNextToWrite())
		Writer::breakLine();
	    else
		Writer::space();
	}
    }
}

int
Part::getNumPreTabChars() const
{
    return type.getLength() + 2;	// 2 parentheses
}

/////////////////////////////////////////////////////////////////////////////
//
// Class: Method
//
/////////////////////////////////////////////////////////////////////////////

Method::Method()
{
    skipThis = FALSE;
}

Method::~Method()
{
}

SbBool
Method::read()
{
    // Method definitions can get very complicated, so we just read
    // the C method name and then read the rest as a big string until
    // the description (starting with an OPEN_CHAR) is hit.
    if (! Reader::readName(cName) ||
	! Reader::readUpTo(OPEN_CHAR, full) ||
	! Reader::readBracketed(description) ||
	! parse()) {
	Reader::error("METHOD spec");
	return FALSE;
    }
    return TRUE;
}

SbBool
Method::parse()
{
    int		i, nest, endOfName;
    const char	*str = full.getString();

    type = REGULAR;
    isHidden = isStatic = isVirtual = isFriend = isConst = FALSE;

    // Find the last close parenthesis
    for (i = full.getLength() - 1; i >= 0; --i) {
	if (str[i] == ')')
	    break;
    }

    // If none, error
    if (i < 0)
	return FALSE;

    // Back up to the matching open paren, skipping nested parens
    nest = 1;
    for (--i ; i >= 0; --i) {
	if (str[i] == ')')
	    nest++;
	else if (str[i] == '(' && --nest == 0)
	    break;
    }

    // If none, error
    if (i < 0)
	return FALSE;

    // The word before this paren is the name of the method. Skip over
    // spaces between the name and the paren.
    for (--i ; i >= 0; --i)
	if (! isspace(str[i]))
	    break;

    if (i < 0)
	return FALSE;

    endOfName = i;

    // Get the name. Assume for now it begins at the previous white
    // space, unless it contains alphanumeric characters, in which
    // case, stop if there's a non-alphanumeric (or ~) character
    // before it. 
    // (There may be a '*' or a '&' without a space.)
    SbBool	endsInAlNum = isalnum(str[i]);

    if (endsInAlNum) {
	for (--i; i >= 0; --i) {
	    if (! (isalnum(str[i]) || str[i] == '~'))
		break;
	}
    }
    else {
	for (--i; i >= 0; --i) {
	    if (isspace(str[i]))
		break;
	}
    }

    name = full.getSubString(i + 1, endOfName).getString();

    // See if it's a constructor or destructor
    if (name == ClassDef::getCurClass()->getName())
	type = CONSTRUCTOR;
    else if (name.getString()[0] == '~' &&
	     name.getString() + 1 == ClassDef::getCurClass()->getName())
	type = DESTRUCTOR;

    // See if it's an operator
    else {
	int	j, prevEnd;

	// Skip over space before name
	for (j = i; j >= 0; --j)
	    if (isalnum(str[j]))
		break;
	prevEnd = j;

	// Get word previous to that
	for (; j >= 0; --j)
	    if (! isalnum(str[j]))
		break;

	if (j > 0 && j < prevEnd &&
	    full.getSubString(j + 1, prevEnd) == "operator") {
	    type = OPERATOR;

	    // In the C version, operators get translated into method
	    // names, using correspondences stolen from "ncbind".
	    if (Writer::getLanguage() == C) {
		static const char *opNames[][2] = {
		    "+",    "Add",
		    "-",    "Sub",
		    "+=",   "AddTo",
		    "-=",   "SubFrom",
		    "*",    "Mult",
		    "*=",   "MultBy",
		    "/",    "Div",
		    "/=",   "DivBy",
		    "%",    "Modulo",
		    "==",   "IsEq",
		    "!=",   "IsNEq",
		    "<",    "IsLT",
		    ">",    "IsGT",
		    "<=",   "IsLEq",
		    ">=",   "IsGEq",
		    "new",  "New",
		    "delete",	"Del",
		    "^",    "Exp",
		    "&",    "And",
		    "|",    "Or",
		    "~",    "Xor",
		    "!",    "Not",
		    "=",    "Copy",
		    "%=",   "ModBy",
		    "^=",   "ExpBy",
		    "&=",   "AndBy",
		    "|=",   "OrBy",
		    "<<",   "ShiftLeft",
		    ">>",   "ShiftRight",
		    ">>=",  "ShiftRightBy",
		    "<<=",  "ShiftLeftBy",
		    "&&",   "AndIs",
		    "||",   "OrIs",
		    "++",   "Inc",
		    "--",   "Dec",
		    ",",    "FuckIfIKnow",
		    "->*",  "FuckIfIKnow",
		    "->",   "Indirect",
		    "()",   "Invoke",
		    "[]",   "GetEl",
		};

#define NUM_OP_NAMES (sizeof(opNames) / sizeof(opNames[0]))

		int op;
		for (op = 0; op < NUM_OP_NAMES; op++) {
		    if (name == opNames[op][0]) {
			name = opNames[op][1];
			break;
		    }
		}
		if (op == NUM_OP_NAMES)
		    name = "UnknownOperator";
	    }

	    else {
		SbString n = "operator ";
		n += name.getString();
		name = n;
	    }
	    i = j;
	}	    
    }

    // Save the stuff before and after the name
    if (i > 0)
	pre  = full.getSubString(0, i).getString();
    post = full.getSubString(endOfName + 1).getString();

    // Figure out whether it is static, virtual, friend
    const char *s = pre.getString();

    while (TRUE) {
	while (isspace(*s))
	    s++;

	if      (strlen(s) > 6 && ! strncmp(s, "static", 6) && isspace(s[6])) {
	    isStatic = TRUE;
	    s += 7;
	}
	else if (strlen(s) > 6 && ! strncmp(s, "friend", 6) && isspace(s[6])) {
	    isFriend = TRUE;
	    s += 7;
	}
	else if (strlen(s) > 6 && ! strncmp(s, "hidden", 6) && isspace(s[6])) {
	    isHidden = TRUE;
	    s += 7;
	}
	else if (strlen(s) > 7 && ! strncmp(s, "virtual",7) && isspace(s[7])) {
	    isVirtual = TRUE;
	    s += 8;
	}
	else
	    break;
    }
    // Remove stuff we just found
    pre = s;

    // Figure out whether it is const
    s = post.getString() + post.getLength() - 1;

    while (s >= post.getString() && isspace(*s))
	s--;

    if (s - post.getString() >= 4 && ! strncmp(s - 4, "const", 5)) {
	isConst = TRUE;
	post.deleteSubString((s - post.getString()) - 4);
    }

    // Clean up pre and post
    Reader::cleanString(pre, TRUE, FALSE);
    Reader::cleanString(post, TRUE);

    // Deal with the C method name. If it is "", it is the same as the
    // regular name, with the first character capitalized.
    if (Writer::getLanguage() == C) {
	if (! cName ) {
	    // Leave cName empty if this is a constructor/destructor:
	    if (type != CONSTRUCTOR && type != DESTRUCTOR) {
		SbString	n;
		char	startChar[2];
		startChar[0] = toupper(name.getString()[0]);
		startChar[1] = '\0';
		n = startChar;
		n += name.getString() + 1;
		cName = n;
	    }
	}

	// Otherwise, convert first character to upper case if necessary
	else if (islower(cName.getString()[0])) {
	    SbString	n;
	    char	startChar[2];
	    startChar[0] = toupper(cName.getString()[0]);
	    startChar[1] = '\0';
	    n = startChar;
	    n += cName.getString() + 1;
	    cName = n;
	}

	// If it is just white space, this method should be ignored in C
	else {
	    for (s = cName.getString(); *s!= '\0'; *s++)
		if (! isspace(*s))
		    break;
	    if (*s == '\0')
		language = C_PLUS_PLUS;
	}
    }

    return TRUE;
}

void
Method::write(SbBool runIn)
{
    if (runIn) {
	Writer::setFont(Writer::CODE_ROMAN);
	writeName();
	if (hasNextToWrite())
	    Writer::write(", ");
	else
	    Writer::write("\n");
    }
    else {
	Writer::indentForTab();
	Writer::setFont(Writer::CODE_ROMAN);

	//
	// Stuff before name
	//

	if (Writer::getLanguage() == C) {
	    if (type == CONSTRUCTOR) {
		Writer::write(ClassDef::getCurClass()->getName());
		Writer::write(" *");
	    }
	    else if (type == DESTRUCTOR)
		Writer::write("void");
	}
	else {
	    if (isStatic)
		Writer::write("static ");
	    if (isVirtual)
		Writer::write("virtual ");
	    if (isFriend)
		Writer::write("friend ");
	}

	Writer::write(pre);
	Writer::tab();
	Writer::setFont(Writer::CODE_BOLD);

	//
	// Name
	//

	writeName();

	//
	// Stuff after name
	//

	Writer::setFont(Writer::CODE_ROMAN);

	if (Writer::getLanguage() == C) {
	    // Assume first character of "post" is the open paren. If this
	    // is not a static method, add a "this" argument as the first
	    // one. Make it a const pointer if the method is const.
	    if (isStatic || type == CONSTRUCTOR)
		Writer::write(post);
	    else {
		Writer::write("(");
		if (isConst)
		    Writer::write("const ");
		Writer::write(ClassDef::getCurClass()->getCPrefix());
		if (post.getString()[1] == ')')
		    Writer::write(" *this");
		else
		    Writer::write(" *this, ");
		Writer::write(post.getString() + 1);
	    }
	}

	else
	    Writer::write(post);


	if (Writer::getLanguage() == C_PLUS_PLUS && isConst)
	    Writer::write(" const");
	Writer::write("\n");

	// Write the description only if we're not writing the synopsis
	// and there is a non-empty description:
	if (! Writer::getInSynopsis() && ! ! description)
	    Writer::writeDescription(description);

	else {
	    if (hasNextToWrite())
		Writer::breakLine();
	    else
		Writer::space();
	}
    }
}

void
Method::writeName()
{
    if (Writer::getLanguage() == C) {
	Writer::write(ClassDef::getCurClass()->getCPrefix());
	if (type == CONSTRUCTOR && !cName)
	    Writer::write("Create");
	else if (type == DESTRUCTOR && !cName)
	    Writer::write("Delete");
	else
	    Writer::write(cName);
    }

    else
	Writer::write(name);
}

int
Method::getNumPreTabChars() const
{
    int num = 0;

    if (Writer::getLanguage() == C) {
	if (type == CONSTRUCTOR)
	    num += ClassDef::getCurClass()->getName().getLength() + 2; // "* "
	else if (type == DESTRUCTOR)
	    num += 4;	// "void"
    }
    else {
	if (isStatic)
	    num += strlen("static ");
	if (isVirtual)
	    num += strlen("virtual ");
	if (isFriend)
	    num += strlen("friend ");
    }

    return num + pre.getLength();
}

/////////////////////////////////////////////////////////////////////////////
//
// Class: Macro
//
/////////////////////////////////////////////////////////////////////////////

SbBool
Macro::read()
{
    if (! Reader::readName(name) ||
	! Reader::readUpTo(OPEN_CHAR, arguments) ||
	! Reader::readBracketed(description)) {
	Reader::error("MACRO spec");
	return FALSE;
    }

    return TRUE;
}

void
Macro::write(SbBool runIn)
{
    if (runIn) {
	Writer::setFont(Writer::CODE_ROMAN);
	Writer::write(name);
	if (hasNextToWrite())
	    Writer::write(", ");
	else
	    Writer::write("\n");
    }
    else {
	Writer::setFont(Writer::CODE_BOLD);

	// The name may contain an openParen and part of the first argument.
	// Strip the name off the front and write the rest with the arguments.
	const char *n = name.getString();
	char *c = (char *)n;
	int len = strlen(n);
	int i;
	for (i=0; i<len; c++, i++) {
	    if (*c == '(') {

		// Found a paren.  Change the paren to an end of string
		// character, write out the name, change the paren back
		// and write the rest of the name out.
		*c = '\0';
		Writer::write(n);
		*c = '(';
		Writer::write(c);
		Writer::write(" ");
		break;
	    }
	}
	if (i == len)
	    Writer::write(name);

	Writer::setFont(Writer::CODE_ROMAN);
	Writer::write(arguments);
	Writer::write("\n");

	// Write the description only if we're not writing the synopsis
	// and there is a non-empty description:
	if (! Writer::getInSynopsis() && ! ! description)
	    Writer::writeDescription(description);

	else {
	    if (hasNextToWrite())
		Writer::breakLine();
	    else
		Writer::space();
	}
    }
}

/////////////////////////////////////////////////////////////////////////////
//
// Class: ActionName
//
/////////////////////////////////////////////////////////////////////////////

SbBool
ActionName::read()
{
    if (! Reader::readName(name)) {
	Reader::error("ACTION name");
	return FALSE;
    }
    return TRUE;
}

void
ActionName::write(SbBool)
{
    Writer::write(name);
    if (hasNextToWrite())
	Writer::write(", ");
    else
	Writer::write("\n");
}

/////////////////////////////////////////////////////////////////////////////
//
// Class: Action
//
/////////////////////////////////////////////////////////////////////////////

SbBool
Action::read()
{
    ActionName	*name;
    int		c;

    while (TRUE) {

	// Read action names. There has to be at least 1.
	name = new ActionName;
	if (! name->read())
	    return FALSE;
	names.add(name);

	// See if the list is ending
	Reader::skipWhiteSpace();
	if ((c = Reader::get()) != SEP_CHAR) {
	    Reader::putBack(c);
	    break;
	}
    }

    if (! Reader::readBracketed(description)) {
	Reader::error("ACTION description");
	return FALSE;
    }

    return TRUE;
}

void
Action::write(SbBool)
{
    Writer::setFont(Writer::CODE_BOLD);
    names.write();
    Writer::writeDescription(description);
}

/////////////////////////////////////////////////////////////////////////////
//
// Class: Misc
//
/////////////////////////////////////////////////////////////////////////////

SbBool
Misc::read()
{
    return (Reader::readName(name) &&
	    Reader::readBracketed(description));
}

void
Misc::write(SbBool)
{
    Writer::startSection(name.getString());
    Writer::writeBracketed(description);
}

/////////////////////////////////////////////////////////////////////////////
//
// Class: Also
//
/////////////////////////////////////////////////////////////////////////////

SbBool
Also::read()
{
    if (! Reader::readBracketed(list)) {
	Reader::error("ALSO list");
	return FALSE;
    }
    return TRUE;
}

void
Also::write(SbBool)
{
    if (! Writer::isForBook())
	Writer::setFont(Writer::CODE_BOLD);
    Writer::writeBracketed(list);
    Writer::write("\n");
}

/////////////////////////////////////////////////////////////////////////////
//
// Class: DraggerResource
//
/////////////////////////////////////////////////////////////////////////////

const SbName &
DraggerResource::getPartDefName(int which) const
{
    DraggerResourcePartDef  *p = (DraggerResourcePartDef *) parts.first;
    int curNum = 0;
    while( p != NULL && curNum < which) {
	p = (DraggerResourcePartDef *) p->next;
	curNum++;
    }
    if (p)
	return p->name;
    else {
	fprintf(stderr,"Error getting the %dth part that uses the dragger\n",
		which);
	fprintf(stderr,"resource name %s\n", name.getString());
    }
    static SbName empty("");
    return empty;
}

SbBool
DraggerResource::read()
{
    // Read name, description pairs within braces
    Reader::skipWhiteSpace();
    if (Reader::get() != OPEN_CHAR) {
	Reader::error("DRAGGER RESOURCE");
	return FALSE;
    }

    // Read one resource name, followed by the sep char
    if (! Reader::readName(name)) {
	Reader::error("DRAGGER RESOURCE DraggerResource::name");
	return FALSE;
    }
    if (Reader::get() != SEP_CHAR) {
	Reader::error("DRAGGER RESOURCE name needs comma after it");
	return FALSE;
    }

    // Read one or more parts, followed by sep char
    while (TRUE) {
	int c;

	// Read the next definition pair. There has to be at least 1.
	DraggerResourcePartDef *part = new DraggerResourcePartDef;
	if (! part->read())
	    return FALSE;
	parts.add(part);

	// List ends with SEP_CHAR
	Reader::skipWhiteSpace();
	if ((c = Reader::get()) == SEP_CHAR)
	    break;
	else 
	    Reader::putBack(c);
    }

    // Read one default geometry (appearance), followed by sep char
    if (! Reader::readString(defaultGeom)) {
	Reader::error("DRAGGER RESOURCE DraggerResource::defaultGeom");
	return FALSE;
    }
    if (Reader::get() != SEP_CHAR) {
	Reader::error("DRAGGER RESOURCE defaultGeom needs comma after it");
	return FALSE;
    }

    // Read one description.
    if (! Reader::readString(description)) {
	Reader::error("DRAGGER RESOURCE DraggerResource::description");
	return FALSE;
    }

    // That should be it.
    Reader::skipWhiteSpace();
    if (Reader::get() != CLOSE_CHAR) {
	Reader::error("DRAGGER RESOURCE missing close character");
	return FALSE;
    }

    return TRUE;
}

void
DraggerResource::write(SbBool)
{
    Writer::setFont(Writer::TEXT_ROMAN);

    Writer::setIndent(0, 0);
    Writer::setTab(18);
    Writer::write("Resource:");
    Writer::tab();
    Writer::setFont(Writer::CODE_BOLD);
    Writer::write(name);
    Writer::write("\n");
    Writer::breakLine();

    Writer::setFont(Writer::TEXT_ROMAN);
    Writer::setIndent(0, 0);
    Writer::setTab(18);
    if (parts.getNum() == 1)
	 Writer::write("Part:");
    else Writer::write("Parts:");
    parts.write();

    Writer::setIndent(0, 0);
    Writer::setTab(18);
    Writer::write("Appearance:");
    Writer::tab();
    Writer::writeBracketed(defaultGeom);
    Writer::breakLine();

    Writer::setIndent(20, 0);
    Writer::setTab(18);
    Writer::write("Description:");
    Writer::tab();
    Writer::writeBracketed(description);
    Writer::space();
}

int
DraggerResource::getNumPreTabChars() const
{
    // Return the length of the longest word between
    // Resource, Part, Description, and Appearance.
    return strlen("Description:");
}

DraggerResource::DraggerResource() {} // quiet the compiler

/////////////////////////////////////////////////////////////////////////////
//
// Class: DraggerResourcePartDef
//
/////////////////////////////////////////////////////////////////////////////

SbBool
DraggerResourcePartDef::read()
{
    if (! Reader::readName(name)) {
	Reader::error("DRAGGER RESOURCE DraggerResourcePartDef::name");
	return FALSE;
    }
    return TRUE;
}

void
DraggerResourcePartDef::write(SbBool)
{
    Writer::tab();
    Writer::write(name);
    Writer::write("\n");
    Writer::breakLine();
}
