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

#include <getopt.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInteraction.h>
#include <Inventor/nodekits/SoNodeKit.h>
#include <Inventor/Xt/SoXt.h>
#include "Man.h"

/////////////////////////////////////////////////////////////////////////////
//
// Prints usage message.
//
/////////////////////////////////////////////////////////////////////////////

static void
printUsage(const char *progname)
{
    fprintf(stderr,
	    "Usage: %s [-bc] dir1/class1 dir2/class2 ...\n"
	    "  where class1 is the most derived class\n"
	    "  -b creates output for the reference manual book, as opposed\n"
	    "     to on-line man pages (the default)\n"
	    "  -c indicates C, as opposed to C++ (the default)\n",
	    progname);

    exit(1);
}

/////////////////////////////////////////////////////////////////////////////
//
// Creates and returns 1 class definition structure. Returns NULL on
// error. This is passed the main ClassDef (the one the man page is
// for) to pass into the ClassDef::setUp method. If this ClassDef IS
// the main one, this pointer is NULL.
//
/////////////////////////////////////////////////////////////////////////////

static ClassDef *
createClassDef(const char *pathName, const ClassDef *mainCd)
{
    ClassDef	*cd = new ClassDef();

    if (Reader::open(pathName) && cd->read() && cd->setUp(mainCd))
	return cd;

    delete cd;

    return NULL;
}

/////////////////////////////////////////////////////////////////////////////
//
// Mainline
//
/////////////////////////////////////////////////////////////////////////////

main(int argc, char *argv[])
{
    SbBool	ok = TRUE;
    int		c, curArg;

    Writer::setLanguage(C_PLUS_PLUS);

    while ((c = getopt(argc, argv, "bc")) != -1) {
	switch (c) {
	  case 'b':
	    Writer::setForBook(TRUE);
	    break;
	  case 'c':
	    Writer::setLanguage(C);
	    break;
	  default:
	    ok = FALSE;
	    break;
	}
    }
    curArg = optind;

    // Check for at least 1 class at end
    if (! ok || curArg >= argc)
	printUsage(argv[0]);

    SoDB::init();
    SoNodeKit::init();
    SoInteraction::init();
    SoXt::init(argv[0]);

    // Process each class in turn
    ClassDef	*mainCd = NULL, *curCd = NULL, *cd;
    while (curArg < argc) {

	cd = createClassDef(argv[curArg++], mainCd);

	if (cd == NULL) {
	    ok = FALSE;
	    break;
	}

	if (mainCd == NULL)
	    mainCd = cd;
	mainCd->addInherit(cd);

	if (curCd != NULL)
	    curCd->setParent(cd);

	curCd = cd;
    }

    // Remove any inherited methods that have the same name or are
    // constructors
    mainCd->cullMethods();

    if (ok) {
	Writer::setPointer(stdout);
	
	// give the writer the name of the original source file
	// in case we have error messages to print (to let the user
	// know which file we barfed on). (alain)
	if (Writer::getLanguage() == C_PLUS_PLUS)
	    Writer::setSourceFileName(argv[1]);
	else
	    Writer::setSourceFileName(argv[2]);
	
	mainCd->write(FALSE);
    }

    return ! ok;
}
