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
// Convert from ASCII/binary Inventor file to a file suitable for 
// use as a compiled in variable.
//
// Format for these files is as follows:
//
// const char *variableName = { "0xXXXXX 0xXXXXX 0xXXXX" };
//
// where the stuff between the quotes is a hex version of the binary file.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <Inventor/SoDB.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoOutput.h>
#include <Inventor/SoLists.h>
#include <Inventor/actions/SoWriteAction.h>

static void
print_usage(const char *progname)
{
    (void)fprintf(stderr, "Usage: %s variableName < inputFile.iv\n",
		  progname);
    (void)fprintf(stderr, "variableName : The name of the variable to create.\n");
    exit(99);
}

static void
parse_args(int argc, char **argv, char **variableName)
{
    if (argc != 2)
    {
	print_usage(argv[0]);
    }
    (*variableName) = strdup(argv[1]);

    return; 
}

main(int argc, char **argv)
{
    SoXt::init(argv[0]);

    // Parse arguments
    char *variableName = NULL;

    parse_args(argc, argv, &variableName );

    // read stuff:
    SoInput in;
    SoSeparator *root;

    in.setFilePointer(stdin);
    root = SoDB::readAll( &in );
    if (root)
	root->ref();

    // write stuff into a buffer
    SoOutput out;
    out.setBinary(TRUE);
    out.setBuffer( malloc(1000), 1000, realloc );
    SoWriteAction writer(&out);
    writer.apply(root);

    if (root)
	root->unref();

    // Create the output file
    void *buf;
    size_t size;
    out.getBuffer(buf, size);
    char *outputBuffer = (char *) buf;
    fprintf( stdout, "const char %s[] = {\n", variableName );
    fprintf(stderr,"bufferSize = %d\n", size);
    // All but last number get commas afterwards
    for ( int j = 0; j < size-1; j++ ) {
	fprintf( stdout, "0x%p,", outputBuffer[j] );
    }
    // Last number gets no comma afterwards
    fprintf( stdout, "0x%p", outputBuffer[size-1] );
    fprintf( stdout, "\n};\n");

    return 0;
}
