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

/////////////////////////////////////////////////////////////////////////////
//  This file exists solely to deal with build issues.  
//
//  The  ivman  executable links against the Inventor library but does not 
//  use graphics.  This executable is then used on the  ** build machine **  
//  to create man pages.  Thus, cross-platform builds use target-platform 
//  Inventor libraries and build machine system libraries.  Inevitably, 
//  there are mismatches in the two sets of libraries (and include files).
// 
//  It is sufficient to stub out graphics routines since graphics are not 
//  used within ivman.
// 
/////////////////////////////////////////////////////////////////////////////


#include <GL/glu.h>


/////////////////////////////////////////////////////////////////////////////
//
// problem 1:  OpenGl Texture extensions.
//      If doing cross compiles (e.g. 6.2 builds on a 5.3 Indigo2),
//      then we'll need to stub out some routines.
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(GL_EXT_texture_object) && !defined(GL_VERSION_1_1)
extern "C" {
void
glGenTexturesEXT() {}

void
glBindTextureEXT() {}

void
glDeleteTexturesEXT() {}
}
#endif


/////////////////////////////////////////////////////////////////////////////
//
// problem 2:  Bug 309217, GLU versions 1.1 and 1.2 have API differences.
//      Need to support (for the Inventor lib) the 1.2 GLU API that is not
//      supported on the (1.1 GLU API) build machine.  
//
/////////////////////////////////////////////////////////////////////////////

#if defined(__cplusplus)
#if (defined(__glu_h__) || defined(GLU_H)) && defined(GLU_VERSION_1_1)

class GLUtesselator;
void gluTessCallback (GLUtesselator *, GLenum, void (*fn)(void)) {}

#endif
#endif


/////////////////////////////////////////////////////////////////////////////
//
// problem 3:  Bug 318776.  Added support for i18n in banyan, but build 
//      machine does not have the libraries.  Need to stub out the calls 
//      for i18n support.
//
/////////////////////////////////////////////////////////////////////////////

extern "C" {
void iconv_open() {}

void iconv() {}

void flUniGetBitmap() {}

void flUniGetOutline() {}

}
