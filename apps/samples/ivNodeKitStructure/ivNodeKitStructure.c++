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

//
// Print information about the Structure of a Nodekit class.
//

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <Inventor/SoDB.h>
#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/Xt/SoXt.h>

static void
print_usage(const char *progname)
{
    (void)fprintf(stderr, "\nThis program prints a diagram and a table of the structure of a nodekit class.\n");
    (void)fprintf(stderr, "See the README for details about the diagrams and tables.\n\n");
    (void)fprintf(stderr, "Usage: %s [-h] className\n\n", progname);
    (void)fprintf(stderr, "       -h : This message (help)\n");
    (void)fprintf(stderr, "className : The name of any node class");
    (void)fprintf(stderr, "derived from SoBaseKit.\n");
    (void)fprintf(stderr, "            This includes all dragger classes as well.\n");
    exit(99);
}

static void
parse_args(int argc, char **argv)
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

    if (err) {
	print_usage(argv[0]);
    }
}

main(int argc, char **argv)
{
    SoXt::init(argv[0]);

    // Parse arguments

    parse_args(argc, argv);
    char *classString;

    if (optind != (argc - 1))
	print_usage(argv[0]);
    else
	classString = strdup(argv[optind]);

    // 'fromName' takes the name of the Class as it appears in file.
    // For the built-in Inventor types, this differs from the class name
    // because it has no 'So' prefix.
    // Therefore, if the class name begins with 'So', our first try will be
    // with the prefix removed.
    SbBool startsWithSo = FALSE;
    if ( !strncmp( classString, "So", 2 ) ) {
	startsWithSo = TRUE;
    }

    char *lookupString;
    if (startsWithSo == TRUE)
	lookupString = classString + 2;
    else
	lookupString = classString;

    SoType classType = SoType::fromName(lookupString);

    // If we got a bad type and the name began with "So" we will type
    // a message and try again with the full name.
    if (classType == SoType::badType() && startsWithSo) {
	fprintf(stderr,"The className you gave has a prefix of \"So\"\n");
	fprintf(stderr,"I tried to remove the prefix and look it up but\n");
	fprintf(stderr,"found no such class. Now I'll try again with the\n");
	fprintf(stderr,"full string you gave me\n");
	classType = SoType::fromName(classString);
    }

    if (classType == SoType::badType()) {
	fprintf(stderr, "ERROR: The given className is not a valid\n");
	fprintf(stderr, "       node type. Message retrieved \n");
	fprintf(stderr, "       from dlerror() follows:\n%s\n", dlerror());
	exit(0);
    }
    else if ( ! classType.isDerivedFrom(SoBaseKit::getClassTypeId())) {
	fprintf(stderr, "ERROR: The given className is not a subClass\n");
	fprintf(stderr, "       of SoBaseKit.\n");
	exit(0);
    }

    SoBaseKit *myKit = (SoBaseKit *) classType.createInstance();

    // Print the diagram and the table.
    myKit->printDiagram();
    fprintf(stdout, "\n\n");
    myKit->printTable();

    return 0;
}
