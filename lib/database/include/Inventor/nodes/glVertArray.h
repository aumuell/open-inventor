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

/*
 * Simple implementation of the vertex array extension to let us work
 * until we get versions of OpenGL that really support it.
 */
#include <GL/gl.h>

#ifndef GL_VERTEX_ARRAY_EXT

#ifdef __cplusplus
extern "C" {
#endif

void VA_glArrayElementEXT(int i);
void VA_glDrawArraysEXT(GLenum mode, int first, GLsizei count);
void VA_glVertexPointerEXT(int size, GLenum type, GLsizei stride,
			   GLsizei count, const void* pointer);
void VA_glNormalPointerEXT(GLenum type, GLsizei stride,
			   GLsizei count, const void* pointer);
void VA_glColorPointerEXT(int size, GLenum type, GLsizei stride,
			  GLsizei count, const void* pointer);
void VA_glTexCoordPointerEXT(int size, GLenum type, GLsizei stride,
			     GLsizei count, const void* pointer);

void VA_glEnable(GLenum what);
void VA_glDisable(GLenum what);

#define GL_VERTEX_ARRAY_EXT 0
#define GL_NORMAL_ARRAY_EXT 1
#define GL_COLOR_ARRAY_EXT 2
#define GL_TEXTURE_COORD_ARRAY_EXT 3

#define glArrayElementEXT VA_glArrayElementEXT 
#define glDrawArraysEXT VA_glDrawArraysEXT
#define glVertexPointerEXT VA_glVertexPointerEXT
#define glNormalPointerEXT VA_glNormalPointerEXT
#define glColorPointerEXT VA_glColorPointerEXT
#define glTexCoordPointerEXT VA_glTexCoordPointerEXT 


#ifdef __cplusplus
}
#endif

#else

#define VA_glEnable(foo) glEnable(foo)
#define VA_glDisable(foo) glDisable(foo)

#endif
