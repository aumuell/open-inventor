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
//    This sample program reads in an inventor data file, and
//  uses the extender chapter viewer to view the data.
//
//  Usage:
//	simpleViewer some_model
//

#include <Inventor/SoDB.h>	// for file reading
#include <Inventor/SoInput.h>	// for file reading
#include <Inventor/Xt/SoXt.h>	// For SoXtAppInitialize()
#include "simpleViewer.h"


int main(int argc, char *argv[])
{
    if (argc < 2) {
	printf("Usage: %s file.iv\n", argv[0]);
	return 1;
    }
    
    // Initialize Inventor and Xt
    Widget myWindow = SoXt::init(argv[0]);
    
    // Read file in
    SoInput in;
    SoNode  *scene;
    if (! in.openFile(argv[1]))
	return 1;
    if(! SoDB::read(&in, scene) || scene == NULL)
	return 1;
    
    // create and show the viewer
    simpleViewer *myViewer = new simpleViewer(myWindow);
    myViewer->setSceneGraph(scene);
    myViewer->setTitle("Custom Viewer");
    myViewer->show();
    
    SoXt::show(myWindow);
    SoXt::mainLoop();
}
