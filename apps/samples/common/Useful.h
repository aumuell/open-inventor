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
// Some routines I found generally useful
//

class SoInput;
class SoNode;
class SoSeparator;
class SoPath;

class Useful
{
  public:
    //
    // Read in a file.  These will all print an error message to
    // stderr and return NULL if there is an error.
    //
    static SoSeparator *readFile(const char *filename);
    static SoSeparator *readFile(FILE *fp);
    static SoSeparator *readFile(SoInput &in);

    //
    // Searches for and returns the last node of the given type on the
    // given path.  I use this to search for which material or
    // materialbinding or whatever affects a particular object in the
    // scene graph.  If ignore flags are set this will not necessarily
    // always be correct.
    //
    static SoNode *searchLastType(SoPath *, SoType);
};


