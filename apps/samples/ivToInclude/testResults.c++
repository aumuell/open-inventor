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

// Reads a binary-style variable from a buffer in another file
// and displays the result in a viewer.

//
// The file 'foo.h' contains a single variable called foo.
// This variable has been written out using the program
// ivToIncludeFile.
//
// For example, issuing the command:
// ivToIncludeFile foo < myFile.iv
//
// Will read in the file myFile.iv and write out a file called
// foo.h
// Inside foo.h will be the following:
// const char foo[] = { blah blah blah };
//
// Where 'blah blah blah' is a hex representation of the binary version of 
// the contents of myFile.iv
//
#include "foo.h"


#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/Xt/SoXt.h>

SoNode *
readFromIncludeFile()
{
   SoInput in;

   SoSeparator *root = new SoSeparator;

   if (foo == NULL )
    return root;

    in.setBuffer((void *) foo, (size_t) sizeof(foo));

    SoNode *n;
    while ((SoDB::read( &in, n ) != FALSE) && (n != NULL))
	root->addChild(n);

    return root;
}

main( int argc, char **argv )
{
   // Initialize Inventor and Xt
   Widget appWindow = SoXt::init(argv[0]);
   if ( appWindow == NULL ) exit( 1 );

   SoNode *root = readFromIncludeFile();
   root->ref();

   SoXtExaminerViewer *viewer = new SoXtExaminerViewer(appWindow);
   viewer->setSceneGraph( root );
   viewer->show();
   viewer->viewAll();

   SoXt::show( appWindow );
   SoXt::mainLoop();
}
