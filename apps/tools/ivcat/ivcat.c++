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
// Convert from/to ASCII/binary Inventor
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInteraction.h>
#include <Inventor/nodes/SoFile.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoOutput.h>
#include <Inventor/SoLists.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/nodekits/SoBaseKit.h>

static void
print_usage(const char *progname)
{
    (void)fprintf(stderr, "Usage: %s [-bfth] [-o filename] file ...\n",
		  progname);
    (void)fprintf(stderr, "-b : Output binary format (default ASCII)\n");
    (void)fprintf(stderr, "-o : Write output to [filename]\n");
    (void)fprintf(stderr, "-f : Expand File nodes\n");
    (void)fprintf(stderr, "-t : Expand Texture2 nodes\n");
    (void)fprintf(stderr, "-h : This message (help)\n");
    (void)fprintf(stderr, "If given a filename of '-' or if not given\n");
    (void)fprintf(stderr, "any filenames, standard input will be read\n");
    exit(99);
}

static int
parse_args(int argc, char **argv, char **outfilename,
	   int &expandFileNodes, int &expandTextureNodes)
{
    int err = 0;	// Flag: error in options?
    int c;
    int result=0;	// 0 = ASCII, 1 = BINARY
    
    while ((c = getopt(argc, argv, "bfto:h")) != -1) {
	switch(c) {
	  case 'b':
	    result = 1;
	    break;
	  case 'f':
	    expandFileNodes = 1;
	    break;
	  case 't':
	    expandTextureNodes = 1;
	    break;
	  case 'o':
	    *outfilename = optarg;
	    break;
	  case 'h':	// Help
	  default:
	    err = 1;
	    break;
	}
    }

    if (err) {
	print_usage(argv[0]);
    }

    return result;
}

//
// This routine searches for and expands all SoFile nodes in the
// given scene graph.  It does this by making all the children of a
// SoFile node the children of its parent.
//
static void
nukeFileNodes(SoNode *&root)
{
    //
    // Special case: if root is a file node, replace it with a group.
    //
    if (root->isOfType(SoFile::getClassTypeId())) {
	SoFile *f = (SoFile *)root;
	SoGroup *g = f->copyChildren();
	root->unref();
	root = g;
	root->ref();
    }

    // Search for all file nodes
    SoSearchAction sa;
    sa.setType(SoFile::getClassTypeId());
    sa.setInterest(SoSearchAction::FIRST);
    sa.setSearchingAll(TRUE);
    
    sa.apply(root);
    
    // We'll keep on searching until there are no more file nodes
    // left.  We don't search for all file nodes at once, because we
    // need to modify the scene graph, and so the paths returned may
    // be truncated (if there are several file nodes under a group, if
    // there are files within files, etc).  Dealing properly with that
    // is complicated-- it is easier (but slower) to just reapply the
    // search until it fails.
    // We need an SoFullPath here because we're searching node kit
    // contents.
    SoFullPath *p = (SoFullPath *) sa.getPath();
    while (p != NULL) {
	SoGroup *parent = (SoGroup *)p->getNodeFromTail(1);
	assert(parent != NULL);

	SoFile *file = (SoFile *)p->getTail();

	// If the filename includes a directory path, add the directory name 
	// to the list of directories where to look for input files 
	const char* filename = file->name.getValue().getString();
	const char *slashPtr;
	char *searchPath = NULL;
        if ((slashPtr = strrchr(filename, '/')) != NULL) {
            searchPath = strdup(filename);
            searchPath[slashPtr - filename] = '\0';
            SoInput::addDirectoryFirst(searchPath);
	}

	int fileIndex = parent->findChild(file);
	assert(fileIndex != -1);
	
	// Now, add group of all children to file's parent's list of children,
	// right after the file node:
        SoGroup *fileGroup = file->copyChildren();
	fileGroup->ref();
	if (fileGroup != NULL)
	    parent->insertChild(fileGroup, fileIndex+1);
	else
	    // So we can at least see where the file node contents were
	    // supposed to go.
	    parent->insertChild(new SoGroup, fileIndex+1);

	// And expand the child node from the group.
	// Note that if the File node is multiply instanced,
	// the groups will not be instanced, but the children of the
	// groups will be.
	parent->removeChild(fileIndex);

	sa.apply(root);
	p = (SoFullPath *) sa.getPath();
    }
}

//
// This routine searches for and expands all SoTexture2 nodes in the
// given scene graph.  It does this by doing a
// startEditing/finishEditing operation on the image field of the
// texture nodes.
//
static void
nukeTextureNodes(SoNode *&root)
{
    // Search for all texture nodes
    SoSearchAction sa;
    sa.setType(SoTexture2::getClassTypeId());
    sa.setInterest(SoSearchAction::ALL);
    
    sa.apply(root);
    
    SoPathList &pl = sa.getPaths();

    for (int i = 0; i < pl.getLength(); i++) {
	SoTexture2 *tex = (SoTexture2 *)((SoFullPath *) pl[i])->getTail();
	
	// Stuff returned by startEditing:
	SbVec2s size; int nc;
	(void)tex->image.startEditing(size, nc);
	tex->image.finishEditing();
    }
}

main(int argc, char **argv)
{
    int expandFileNodes = 0;
    int expandTextureNodes = 0;

    SoInteraction::init();

    // Search children of node kits.  This is needed to be able to
    // find and expand nodes in the "contents" of SoWrapperKits.
    SoBaseKit::setSearchingChildren(TRUE);

    // Parse arguments
    char *outputfile = NULL;
    int binary = 0;

    binary = parse_args(argc, argv, &outputfile, expandFileNodes,
			expandTextureNodes);

    // read stuff:
    SoInput in;
    SoNode *root;
    SoNodeList nodeList;

    if (optind == argc) {
	++argc;	            // Act like one argument "-" was given
	argv[optind] = "-";
    }
    for (; optind < argc; optind++) {
	char *filename = argv[optind];

	if (strcmp(filename, "-") == 0) {
	    if (isatty(fileno(stdin))) {
		fprintf(stderr, "Trying to read from standard input, ");
		fprintf(stderr, "but standard input is a tty!\n");
		print_usage(argv[0]);
	    }
	    in.setFilePointer(stdin);
	    filename = NULL;	// Tested later...
	}
	else if (in.openFile(filename) == FALSE) {
	    fprintf(stderr, "Could not open file %s\n", filename);
	}

	// If the filename includes a directory path, add the directory name 
	// to the list of directories where to look for input files 
	if (filename != NULL) {
	    const char *slashPtr;
	    char *searchPath = NULL;
	    if ((slashPtr = strrchr(filename, '/')) != NULL) {
		searchPath = strdup(filename);
		searchPath[slashPtr - filename] = '\0';
		in.addDirectoryFirst(searchPath);
	    }
	}

	do {
	    int read_ok = SoDB::read(&in, root);

	    if (!read_ok) {
		fprintf(stderr, "Error reading %s\n",
			filename == NULL ? "stdin" : filename);
		print_usage(argv[0]);
	    }
	    else if (root != NULL) {
		root->ref();
		if (expandFileNodes) {
		    nukeFileNodes(root);
		}
		if (expandTextureNodes) {
		    nukeTextureNodes(root);
		}
		nodeList.append(root);
		root->unref();
	    }
	} while (root != NULL);

	if (filename != NULL) in.closeFile();
    }

    // write stuff
    SoOutput out;
    out.setBinary(binary);
    if (outputfile == NULL) {
	out.setFilePointer(stdout);
    }
    else {
	if (out.openFile(outputfile) == FALSE) {
	    fprintf(stderr, "Couldn't open %s for writing\n",
		    outputfile);
	    print_usage(argv[0]);
	}
    }
    SoWriteAction writer(&out);

    for (int i = 0; i < nodeList.getLength(); i++) {
	writer.apply(nodeList[i]);
    }
    
    if (outputfile != NULL) out.closeFile();

    return 0;
}
