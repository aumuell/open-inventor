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

/*--------------------------------------------------------------
 *  This is an example from the Inventor Toolmaker,
 *  chapter 4, example 3.
 *
 *  This program initializes the GetVolumeAction class from the
 *  previous examples, reads a scene graph from "volume.iv",
 *  and prints the resulting computed volume to stdout.
 *------------------------------------------------------------*/

#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoInteraction.h>
#include <Inventor/nodes/SoSeparator.h>

// Header file for new action class
#include "GetVolumeAction.h"

main()
{
   // Initialize Inventor
   SoInteraction::init();

   // Initialize the new action class
   GetVolumeAction::initClass();

   // Open the file and read the scene
   SoInput      myInput;
   SoSeparator  *root;
   if (! myInput.openFile("volume.iv")) {
      fprintf(stderr, "Can't open \"volume.iv\" for reading\n");
      return 1;
   }
   root = SoDB::readAll(&myInput);
   if (root == NULL) {
      printf("Couldn't read scene from \"volume.iv\"\n");
      return 2;
   }
   root->ref();

   // Compute the volume: apply a GetVolumeAction to the root
   GetVolumeAction va;
   va.apply(root);

   // Print the result
   printf("Total volume = %g\n", va.getVolume());

   return 0;
}
