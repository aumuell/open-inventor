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
 * Copyright (C) 1990, 1991   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Description:
 |	Internal routines used for debugging
 | 
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <SoDebug.h>
#include <Inventor/SbDict.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/fields/SoField.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

static SbDict *ptrNameDict = NULL;
static SbDict *envDict = NULL;

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Fast version of getenv(), stores environment strings in an
//    SbDict for fast lookup (the standard getenv() does a linear
//    search through your whole environment).
//
// Use: public 

const char *
SoDebug::GetEnv(const char *envVar)
//
////////////////////////////////////////////////////////////////////////
{
    if (envDict == NULL) envDict = new SbDict;

    SbName name(envVar);
    unsigned long key = (unsigned long)name.getString();

    void *value;

    // Try looking in the dictionary first...
    if (!envDict->find(key, value)) {
	value = (void *)getenv(envVar);
	envDict->enter(key, value);
    }

    return (const char *)value;
}

#define MAXLEN 200
static char *debugStrings = NULL;
static int currentString = 0;
static int numBufferStrings = 0;

////////////////////////////////////////////////////////////////////////
//
// Description:
//    A printf() replacement that prints to an internal buffer that is
//    only printed once N lines have been collected.  This is good
//    for debugging real-time stuff whose timing changes if a
//    printf() is done in the middle of it.  The output goes to
//    stderr; a blank line is inserted every N lines, if N > 1.
//    Set the IV_DEBUG_BUFLEN variable to make N something other than 1.
//
// Use: public 

void
SoDebug::RTPrintf(const char *formatString ...)
//
////////////////////////////////////////////////////////////////////////
{
    va_list	ap;

    if (debugStrings == NULL) {
	const char *s = GetEnv("IV_DEBUG_BUFLEN");
	if (s != NULL)
	    numBufferStrings = atoi(s);

	if (numBufferStrings <= 0)
	    numBufferStrings = 1;

	debugStrings = new char [numBufferStrings * MAXLEN];
    }

    va_start(ap, formatString);
    vsprintf(&debugStrings[currentString * MAXLEN], formatString, ap);
    va_end(ap);

    if (++currentString == numBufferStrings) {
	// Dump strings to stderr
	for (int i = 0 ; i < currentString; i++)
	    fputs(&debugStrings[i * MAXLEN], stderr);
	// Blank line between dumps...
	if (numBufferStrings > 1)
	    fputc('\n', stderr);
	currentString = 0;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Give a pointer a name.  This can be extremely useful when trying
//    to track objects through queues, etc...  Uses an SbDict so this
//    is fast...
//
// Use: public 

void
SoDebug::NamePtr(const char *name, void *ptr)
//
////////////////////////////////////////////////////////////////////////
{
    if (ptrNameDict == NULL) ptrNameDict = new SbDict;

    // Ignore const-cast-away warning
    ptrNameDict->enter((unsigned long)ptr, (void *)name);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Return the name of a pointer named with NamePtr.
//
// Use: public 

const char *
SoDebug::PtrName(void *ptr)
//
////////////////////////////////////////////////////////////////////////
{
    if (ptrNameDict == NULL) ptrNameDict = new SbDict;

    void *value;
    if (!ptrNameDict->find((unsigned long)ptr, value))
	return "<noName>";
    return (const char *)value;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Applies an SoWriteAction to the graph rooted by the given node.
//    The results go to stdout
//
// Use: public 

void
SoDebug::write(SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
    node->ref();

    SoWriteAction	wa;
    wa.apply(node);

    node->unrefNoDelete();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Applies an SoWriteAction to the graph rooted by the given node.
//    The results go to given file (/tmp/debug.iv if NULL)
//
// Use: public 

void
SoDebug::writeFile(SoNode *node, const char *filename)
//
////////////////////////////////////////////////////////////////////////
{
    node->ref();

    if (filename == NULL) filename = "/tmp/debug.iv";

    SoWriteAction	wa;
    wa.getOutput()->openFile(filename);
    wa.apply(node);

    node->unrefNoDelete();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Applies an SoWriteAction to the given field.
//    The results go to stdout
//
// Use: public 

void
SoDebug::writeField(SoField *field)
//
////////////////////////////////////////////////////////////////////////
{
    SoFieldContainer *fc = field->getContainer();

    SbName fieldName;
    fc->getFieldName(field, fieldName);
    printf("Field name is: %s\n", fieldName.getString());
    if (fc->isOfType(SoNode::getClassTypeId())) {
	printf("Field is part of node:\n");

	SoNode *node = (SoNode *)fc;
	node->ref();

	SoWriteAction	wa;
	wa.apply(node);

	node->unrefNoDelete();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Prints out name of a node.
//
// Use: public 

void
SoDebug::printName(SoBase *base)
//
////////////////////////////////////////////////////////////////////////
{
    const char *name = base->getName().getString();

    if (name)
	puts(name);
    else
	puts(" not named ");
}
