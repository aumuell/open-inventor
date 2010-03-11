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
//______________________________________________________________________
//_____________  S I L I C O N   G R A P H I C S   I N C .  ____________
//
//   $Source: /oss/CVS/cvs/inventor/apps/tools/ivnorm/ivnorm.c++,v $
//   $Revision: 1.3 $
//   $Date: 2001/09/25 00:45:30 $
//
//   ivnorm
//
//      This is the mainline for the IRIS Inventor `ivnorm' program.
//   This program reads an Inventor data file, finds all of the 
//   indexed face set nodes, generates normals for each, and writes the
//   result in tact (it does not remove any properties or hierarchy)
//   to a new file.
//
//   Author(s)          : Gavin Bell
//
//   Notes:
//
//_____________  S I L I C O N   G R A P H I C S   I N C .  ____________
//______________________________________________________________________

#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>

#include <Inventor/SoInteraction.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/actions/SoWriteAction.h>

#include "FindNormals.h"

Face::Orientation orientation = Face::UNKNOWN;
float 	    creaseAngle = M_PI/6.0;		// 30 degrees
int 	    findVNorms  = 0;
int	    verbose = 0;
char	*inFileName = "stdin";
char	*outFileName = "stdout";

//-----------------------------------------------------------------------------
//
// My standard command-line parser for tools that can take two
// filenames or act as a filter.
//
//-----------------------------------------------------------------------------
static void
ParseCommandLine(int argc, char **argv, FILE **in, FILE **out)
{
    int err;	/* Flag: error in options? */
    int c;
    *in = stdin;
    *out = stdout;
    err = 0;
    
    while ((c = getopt(argc, argv, "cCva:bVh")) != -1)
    {
	switch(c)
	{
	  case 'c':
	    orientation = Face::CCW;
	    break;
	  case 'C':
	    orientation = Face::CW;
	    break;
	  case 'v':
	    findVNorms = TRUE;
	    break;
	  case 'a':
	    findVNorms = TRUE;
	    // convert from given arg (in degrees) to radians
	    creaseAngle = atof(optarg) * M_PI / 180.0;
	    break;
	  case 'V':
	    verbose = TRUE;
	    break;
	  case 'h':	/* Help */
	  default:
	    err = 1;
	    break;
	}
    }
    /* Handle optional filenames */
    for (; optind < argc; optind++)
    {
	if (*in == stdin)
	{
	    inFileName = argv[optind];
	    *in = fopen(inFileName, "r");
	    if (*in == NULL)
	    {
		char buf[500];
		sprintf(buf, "%s: %s", argv[0], inFileName);
		perror(buf);
		exit(99);
	    }
	}
	else if (*out == stdout)
	{
	    outFileName = argv[optind];
	    *out = fopen(outFileName, "w");
	    if (*out == NULL)
	    {
		char buf[500];
		sprintf(buf, "%s: %s", argv[0], outFileName);
		perror(buf);
		exit(99);
	    }
	}
	else err = 1;	/* Too many arguments */
    }

    /* If stdin is a terminal, spit out usage */
    if (*in == stdin && isatty(fileno(stdin))) err = 1;

    if (err)
    {
	(void)fprintf(stderr, "Usage: %s [options]", argv[0]);
	(void)fprintf(stderr, " [infile] [outfile]\n");
	(void)fprintf(stderr, "-c        Assume counter-clockwise faces\n");
	(void)fprintf(stderr, "-C        Assume clockwise faces\n");
	(void)fprintf(stderr, "-v        Find vertex normals\n");
	(void)fprintf(stderr, "-a angle  Use angle (in degrees)"
		      " as crease angle\n");
	(void)fprintf(stderr, "-V        verbose trace\n");
	(void)fprintf(stderr, "-h        This message (help)\n");
	exit(99);
    }
}


int
main(int argc, char **argv)
{
    FILE *fin;
    FILE *fout;

    SoInteraction::init();
 
    ParseCommandLine(argc, argv, &fin, &fout);

    SoNode *node = NULL;
    SbBool ok;

    SoInput in;
    in.setFilePointer(fin);

    SoOutput out;
    out.setFilePointer(fout);
    SoWriteAction wa(&out);

    FindNormals normalFinder;
    normalFinder.AssumeOrientation(orientation);
    normalFinder.setCreaseAngle(creaseAngle);
    normalFinder.setVerbose(verbose);
    
    for (;;) {
	if (verbose) fprintf(stderr, "%s: reading graph from '%s'.\n", argv[0], inFileName);
	ok = SoDB::read(&in, node);
	if (verbose) fprintf(stderr, "%s: finished reading graph.\n", argv[0], inFileName);
	if (!ok || !node)
	    break;

	node->ref();
	// Generate normals
	normalFinder.apply(node, findVNorms);

	// Write out the (modified) scene graph
	if (verbose) fprintf(stderr, "%s: writing graph to '%s'.\n", argv[0], outFileName);
	out.setBinary(in.isBinary());
	wa.apply(node);
	if (verbose) fprintf(stderr, "%s: finished writing graph.\n", argv[0], outFileName);
    }

    if (!ok)
    {
	fprintf(stderr, "Bad data.\n");
	return 1;
    }

    return 0;
}
