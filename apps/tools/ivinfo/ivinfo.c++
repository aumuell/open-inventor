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
 |   $Revision: 1.3 $
 |
 |   Description:
 |	ivinfo - prints out information about an Inventor data file,
 |	including:
 |
 |		- whether data is ASCII or binary
 |		- number of root nodes
 |		- number of nodes under each root
 |		- contents of any SoInfo nodes found in the data
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <stdlib.h>
#include <unistd.h>

#include <Inventor/SoDB.h>
#include <Inventor/SoInteraction.h>
#include <Inventor/SoInput.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoInfo.h>

static const char *progName;

//
// Counts and returns the number of nodes under the given root node.
// This is recursive.
//

static int
countNodes(const SoNode *root)
{
    int numNodes = 1;

    if (root->isOfType(SoGroup::getClassTypeId())) {

	const SoGroup	*group = (const SoGroup *) root;
	int		i;

	for (i = 0; i < group->getNumChildren(); i++)
	    numNodes += countNodes(group->getChild(i));
    }

    return numNodes;
}

//
// Searches for all SoInfo nodes in the input graph, printing out the
// string field for any that are found.
//

static void
printInfo(SoNode *root)
{
    SoSearchAction	sa;
    const SoInfo	*info;
    int			i;

    // Set up action to search for SoInfo nodes
    sa.setType(SoInfo::getClassTypeId());
    sa.setInterest(SoSearchAction::ALL);
    sa.setSearchingAll(TRUE);

    sa.apply(root);

    for (i = 0; i < sa.getPaths().getLength(); i++) {
	info = (const SoInfo *) sa.getPaths()[i]->getTail();
	printf("%s\n", info->string.getValue().getString());
    }
}

static void
printUsage(const char *progname)
{
    fprintf(stderr, "Usage: %s [-h] [file]\n", progname);
    fprintf(stderr,
	    "-h : Print this message (help)\n"
	    "If no filename is given, standard input will be read\n");
}

static void
parseArgs(int argc, char **argv, SoInput &in)
{
    int err = 0;	// Flag: error in options?
    int c;
    
    while ((c = getopt(argc, argv, "h")) != -1) {
	switch(c) {
	  case 'h':	// Help
	  default:
	    err = 1;
	    break;
	}
    }
    if (optind+1 == argc) {	// One optional filename
	if (! in.openFile(argv[optind])) {
	    fprintf(stderr, "Could not open file %s\n", argv[optind]);
	    err = 1;
	}
    }
    else if (optind == argc) {	// No filename
	if (isatty(fileno(stdin))) {
	    fprintf(stderr, "Trying to read from standard input, "
	            "but standard input is a tty!\n");
	    err = 1;
	}
    }
    else err = 1;
    if (err) {
	printUsage(argv[0]);
	exit(1);
    }
}

main(int argc, char *argv[])
{
    SoInput	in;
    SoNode	*root;
    SbBool	gotAny = FALSE;

    progName = argv[0];

    SoInteraction::init();

    parseArgs(argc, argv, in);

    while (TRUE) {

	if (! SoDB::read(&in, root)) {
	    fprintf(stderr, "%s: bad data in input\n", progName);
	    return 1;
	}

	if (root == NULL) {
	    if (! gotAny)
		printf("%s: no data in input\n", progName);
	    break;
	}

	root->ref();

	if (! gotAny) {
	    printf("%s: data format is %s\n", progName,
		   in.isBinary() ? "binary" : "ASCII");
	    gotAny = TRUE;
	}

	else
	    printf("-----------------------------------------------------\n");

	printf("%s: number of nodes under root is %d\n", progName,
	       countNodes(root));

	printInfo(root);

	root->unref();
    }

    return(0);
}
