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
// Simple test program which will run a set of custom components.
//

#include <stdlib.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/SoXtMaterialEditor.h>
#include "MyTextureEditor.h"
#include "MyMaterialPalette.h"
#include "MySimpleMaterialEditor.h"
#include "MyColorEditor.h"
#include "MyThumbWheel.h"

void usage(char *name)
{
    printf("Usage:  %s whichComp\n", name);
    printf(" whichComp : m = simple material Editor\n");
    printf("             M = material Editor\n");
    printf("             p = material palette\n");
    printf("             c = color editor\n");
    printf("             t = Thumb Wheel\n");
    printf("             T = Texture Editor\n");
    exit(0);
}

int
main(unsigned int argc, char *argv[])
{
    if (argc != 2)
	usage(argv[0]);
    
    // Initialize Inventor and Xt
    Widget mainWindow = SoXt::init(argv[0]);
    
    //
    // Create the right component
    //
    SoXtComponent *comp;
    switch(*argv[1]) {
	case 'm': comp = new MySimpleMaterialEditor(mainWindow); break;
	case 'M': comp = new SoXtMaterialEditor(mainWindow); break;
	case 'c': comp = new MyColorEditor(mainWindow); break;
	case 't': comp = new MyThumbWheel(mainWindow); break;
	case 'p': comp = new MyMaterialPalette(mainWindow); break;
	case 'T': comp = new MyTextureEditor(mainWindow); break;
	default: usage(argv[0]); break;
    }
    comp->show();
    XtRealizeWidget(mainWindow);
    
    SoXt::mainLoop();
}
