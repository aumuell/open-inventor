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

#include <Inventor/nodes/glVertArray.h>
#include <stdio.h>

#define C3_BIT 0x8
#define N3_BIT 0x4
#define T2_BIT 0x2
#define V3_BIT 0x1
#define ALL_BITS (C3_BIT | N3_BIT | T2_BIT | V3_BIT)

#define V3		(V3_BIT)
#define N3_V3		(N3_BIT | V3_BIT)
#define T2_V3		(T2_BIT | V3_BIT)
#define T2_C3_V3	(T2_BIT | C3_BIT | V3_BIT)
#define T2_N3_V3	(T2_BIT | N3_BIT | V3_BIT)
#define C3_V3		(C3_BIT | V3_BIT)
#define C3_N3_V3	(C3_BIT | N3_BIT | V3_BIT)

static struct {
    int fastCase;		/* Bits: C3_BIT/etc */
    int enabled;	/* Bits: C3_BIT/etc */

    int sizeV;
    GLsizei strideV, countV;
    GLenum typeV;
    const void *ptrV;

    GLsizei strideN, countN;
    GLenum typeN;
    const void *ptrN;

    int sizeC;
    GLsizei strideC, countC;
    GLenum typeC;
    const void *ptrC;

    int sizeTC;
    GLsizei strideTC, countTC;
    GLenum typeTC;
    const void *ptrTC;
} vStuff;

void (*v3fptr)(const GLfloat *) = glVertex3fv;
void (*c3fptr)(const GLfloat *) = glColor3fv;
void (*n3fptr)(const GLfloat *) = glNormal3fv;
void (*t2fptr)(const GLfloat *) = glTexCoord2fv;

void slow_ArrayElementEXT(int i);
void slow_DrawArraysEXT(GLenum mode, int first, GLsizei count);

void
VA_glArrayElementEXT(int i) {
    switch (vStuff.fastCase) {
      case V3:
	(*v3fptr)( (GLfloat *)(((char *)vStuff.ptrV)+vStuff.strideV*i));
	return;
      case N3_V3:
	(*n3fptr)( (GLfloat *)(((char *)vStuff.ptrN)+vStuff.strideN*i));
	(*v3fptr)( (GLfloat *)(((char *)vStuff.ptrV)+vStuff.strideV*i));
	return;
      case T2_V3:
	(*t2fptr)( (GLfloat *)(((char *)vStuff.ptrTC)+vStuff.strideTC*i));
	(*v3fptr)( (GLfloat *)(((char *)vStuff.ptrV)+vStuff.strideV*i));
	return;
      case T2_C3_V3:
	(*t2fptr)( (GLfloat *)(((char *)vStuff.ptrTC)+vStuff.strideTC*i));
	(*c3fptr)( (GLfloat *)(((char *)vStuff.ptrN)+vStuff.strideC*i));
	(*v3fptr)( (GLfloat *)(((char *)vStuff.ptrV)+vStuff.strideV*i));
	return;
      case T2_N3_V3:
	(*t2fptr)( (GLfloat *)(((char *)vStuff.ptrTC)+vStuff.strideTC*i));
	(*n3fptr)( (GLfloat *)(((char *)vStuff.ptrN)+vStuff.strideN*i));
	(*v3fptr)( (GLfloat *)(((char *)vStuff.ptrV)+vStuff.strideV*i));
	return;
      case C3_V3:
	(*c3fptr)( (GLfloat *)(((char *)vStuff.ptrC)+vStuff.strideC*i));
	(*v3fptr)( (GLfloat *)(((char *)vStuff.ptrV)+vStuff.strideV*i));
	return;
      case C3_N3_V3:
	(*c3fptr)( (GLfloat *)(((char *)vStuff.ptrC)+vStuff.strideC*i));
	(*n3fptr)( (GLfloat *)(((char *)vStuff.ptrN)+vStuff.strideN*i));
	(*v3fptr)( (GLfloat *)(((char *)vStuff.ptrV)+vStuff.strideV*i));
	return;
      case ALL_BITS:
	(*c3fptr)( (GLfloat *)(((char *)vStuff.ptrC)+vStuff.strideC*i));
	(*n3fptr)( (GLfloat *)(((char *)vStuff.ptrN)+vStuff.strideN*i));
	(*t2fptr)( (GLfloat *)(((char *)vStuff.ptrTC)+vStuff.strideTC*i));
	(*v3fptr)( (GLfloat *)(((char *)vStuff.ptrV)+vStuff.strideV*i));
	return;
      default:
	slow_ArrayElementEXT(i);
	return;
    }
}

void
VA_glDrawArraysEXT(GLenum mode, int first, GLsizei count) {
    int i;

    switch (vStuff.fastCase) {
      case V3:
	glBegin(mode);
	for (i = first; i < first+count; i++) {
	    (*v3fptr)( (GLfloat *)(((char *)vStuff.ptrV)+vStuff.strideV*i));
	}
	glEnd();
	return;
      case N3_V3:
	glBegin(mode);
	for (i = first; i < first+count; i++) {
	    (*n3fptr)( (GLfloat *)(((char *)vStuff.ptrN)+vStuff.strideN*i));
	    (*v3fptr)( (GLfloat *)(((char *)vStuff.ptrV)+vStuff.strideV*i));
	}
	glEnd();
	return;
      case T2_V3:
	glBegin(mode);
	for (i = first; i < first+count; i++) {
	    (*t2fptr)( (GLfloat *)(((char *)vStuff.ptrTC)+vStuff.strideTC*i));
	    (*v3fptr)( (GLfloat *)(((char *)vStuff.ptrV)+vStuff.strideV*i));
	}
	glEnd();
	return;
      case T2_N3_V3:
	glBegin(mode);
	for (i = first; i < first+count; i++) {
	    (*t2fptr)( (GLfloat *)(((char *)vStuff.ptrTC)+vStuff.strideTC*i));
	    (*n3fptr)( (GLfloat *)(((char *)vStuff.ptrN)+vStuff.strideN*i));
	    (*v3fptr)( (GLfloat *)(((char *)vStuff.ptrV)+vStuff.strideV*i));
	}
	glEnd();
	return;
      case C3_V3:
	glBegin(mode);
	for (i = first; i < first+count; i++) {
	    (*c3fptr)( (GLfloat *)(((char *)vStuff.ptrC)+vStuff.strideC*i));
	    (*v3fptr)( (GLfloat *)(((char *)vStuff.ptrV)+vStuff.strideV*i));
	}
	glEnd();
	return;
      case T2_C3_V3:
	glBegin(mode);
	for (i = first; i < first+count; i++) {
	    (*t2fptr)( (GLfloat *)(((char *)vStuff.ptrTC)+vStuff.strideTC*i));
	    (*c3fptr)( (GLfloat *)(((char *)vStuff.ptrC)+vStuff.strideC*i));
	    (*v3fptr)( (GLfloat *)(((char *)vStuff.ptrV)+vStuff.strideV*i));
	}
	glEnd();
	return;
      case C3_N3_V3:
	glBegin(mode);
	for (i = first; i < first+count; i++) {
	    (*c3fptr)( (GLfloat *)(((char *)vStuff.ptrC)+vStuff.strideC*i));
	    (*n3fptr)( (GLfloat *)(((char *)vStuff.ptrN)+vStuff.strideN*i));
	    (*v3fptr)( (GLfloat *)(((char *)vStuff.ptrV)+vStuff.strideV*i));
	}
	glEnd();
	return;
      case ALL_BITS:
	glBegin(mode);
	for (i = first; i < first+count; i++) {
	    (*t2fptr)( (GLfloat *)(((char *)vStuff.ptrTC)+vStuff.strideTC*i));
	    (*c3fptr)( (GLfloat *)(((char *)vStuff.ptrC)+vStuff.strideC*i));
	    (*n3fptr)( (GLfloat *)(((char *)vStuff.ptrN)+vStuff.strideN*i));
	    (*v3fptr)( (GLfloat *)(((char *)vStuff.ptrV)+vStuff.strideV*i));
	}
	glEnd();
	return;
      default:
	slow_DrawArraysEXT(mode, first, count);
	return;
    }
}

const char *
type_enum(GLenum type) {
    switch(type) {
      case GL_BYTE:
	return "BYTE";
      case GL_UNSIGNED_BYTE:
	return "UNSIGNED_BYTE";
      case GL_SHORT:
	return "SHORT";
      case GL_UNSIGNED_SHORT:
	return "UNSIGNED_SHORT";
      case GL_INT:
	return "INT";
      case GL_UNSIGNED_INT:
	return "UNSIGNED_INT";
      case GL_FLOAT:
	return "FLOAT";
/*      case GL_DOUBLE_EXT:
	return "DOUBLE_EXT";
*/
      default:
	break;
    }
    return "BAD ENUM TYPE";
}

/* General, slow case */
void
slow_ArrayElementEXT(int i) {
    fprintf(stderr, "Slow vertex array case unsupported:\n");
    if (vStuff.enabled & C3_BIT)
	fprintf(stderr, "Color: size(%d) type(%s)\n", vStuff.sizeC,
		type_enum(vStuff.typeC));
    if (vStuff.enabled & N3_BIT)
	fprintf(stderr, "Normal: type(%s)\n",
		type_enum(vStuff.typeN));
    if (vStuff.enabled & T2_BIT)
	fprintf(stderr, "TexCoord: size(%d) type(%s)\n", vStuff.sizeTC,
		type_enum(vStuff.typeTC));
    if (vStuff.enabled & V3_BIT)
	fprintf(stderr, "Vertex: size(%d) type(%s)\n", vStuff.sizeV,
		type_enum(vStuff.typeV));
}
void
slow_DrawArraysEXT(GLenum mode, int first, GLsizei count) {

    fprintf(stderr, "Slow vertex array case unsupported:\n");
    if (vStuff.enabled & C3_BIT)
	fprintf(stderr, "Color: size(%d) type(%s)\n", vStuff.sizeC,
		type_enum(vStuff.typeC));
    if (vStuff.enabled & N3_BIT)
	fprintf(stderr, "Normal: type(%s)\n",
		type_enum(vStuff.typeN));
    if (vStuff.enabled & T2_BIT)
	fprintf(stderr, "TexCoord: size(%d) type(%s)\n", vStuff.sizeTC,
		type_enum(vStuff.typeTC));
    if (vStuff.enabled & V3_BIT)
	fprintf(stderr, "Vertex: size(%d) type(%s)\n", vStuff.sizeV,
		type_enum(vStuff.typeV));
}

void
VA_glVertexPointerEXT(int size, GLenum type, GLsizei stride, GLsizei count,
		 const void* pointer) {

    if (stride == 0) stride = sizeof(float)*size;

    vStuff.sizeV = size;
    vStuff.typeV = type;
    vStuff.strideV = stride;
    vStuff.countV = count;
    vStuff.ptrV = pointer;

    if (size == 3 && type == GL_FLOAT && (vStuff.enabled & V3_BIT)) {
	vStuff.fastCase = vStuff.fastCase | V3_BIT;
    } else {
	vStuff.fastCase = vStuff.fastCase & ((~V3_BIT) & ALL_BITS);
    }
}

void
VA_glNormalPointerEXT(GLenum type, GLsizei stride, GLsizei count,
		 const void* pointer){

    if (stride == 0) stride = sizeof(float)*3;

    vStuff.typeN = type;
    vStuff.strideN = stride;
    vStuff.countN = count;
    vStuff.ptrN = pointer;

    if (type == GL_FLOAT && (vStuff.enabled & N3_BIT)) {
	vStuff.fastCase = vStuff.fastCase | N3_BIT;
    } else {
	vStuff.fastCase = vStuff.fastCase & ((~N3_BIT) & ALL_BITS);
    }
}

void
VA_glColorPointerEXT(int size, GLenum type, GLsizei stride, GLsizei count,
		const void* pointer) {

    if (stride == 0) stride = sizeof(float)*size;

    vStuff.sizeC = size;
    vStuff.typeC = type;
    vStuff.strideC = stride;
    vStuff.countC = count;
    vStuff.ptrC = pointer;

    if (size == 3 && type == GL_FLOAT && (vStuff.enabled & C3_BIT)) {
	vStuff.fastCase = vStuff.fastCase | C3_BIT;
    } else {
	vStuff.fastCase = vStuff.fastCase & ((~C3_BIT) & ALL_BITS);
    }
}

void
VA_glTexCoordPointerEXT(int size, GLenum type, GLsizei stride, GLsizei count,
		   const void* pointer) {

    if (stride == 0) stride = sizeof(float)*size;

    vStuff.sizeTC = size;
    vStuff.typeTC = type;
    vStuff.strideTC = stride;
    vStuff.countTC = count;
    vStuff.ptrTC = pointer;

    if (size == 2 && type == GL_FLOAT && (vStuff.enabled & T2_BIT)) {
	vStuff.fastCase = vStuff.fastCase | T2_BIT;
    } else {
	vStuff.fastCase = vStuff.fastCase & ((~T2_BIT) & ALL_BITS);
    }
}

void
VA_glEnable(GLenum what) {
    /* Set bit, then call Pointer routine with current values so it */
    /* will figure out if we're on a fast-path mode */
    switch (what) {
      case GL_VERTEX_ARRAY_EXT:
	vStuff.enabled |= V3_BIT;
	glVertexPointerEXT(vStuff.sizeV, vStuff.typeV, vStuff.strideV,
			 vStuff.countV, vStuff.ptrV);
	break;
      case GL_NORMAL_ARRAY_EXT:
	vStuff.enabled |= N3_BIT;
	glNormalPointerEXT(vStuff.typeN, vStuff.strideN,
			 vStuff.countN, vStuff.ptrN);
	break;
      case GL_COLOR_ARRAY_EXT:
	vStuff.enabled |= C3_BIT;
	glColorPointerEXT(vStuff.sizeC, vStuff.typeC, vStuff.strideC,
			  vStuff.countC, vStuff.ptrC);
	break;
      case GL_TEXTURE_COORD_ARRAY_EXT:
	vStuff.enabled |= T2_BIT;
	glTexCoordPointerEXT(vStuff.sizeTC, vStuff.typeTC, vStuff.strideTC,
			     vStuff.countTC, vStuff.ptrTC);
	break;
      default:
	fprintf(stderr, "Ack, bad enum (%d) to VA_glEnable\n", what);
	break;
    }
}

void
VA_glDisable(GLenum what) {
    /* Unset bit, then call Pointer routine with current values so it */
    /* will figure out if we're on a fast-path mode */
    switch (what) {
      case GL_VERTEX_ARRAY_EXT:
	vStuff.enabled &= ((~V3_BIT) & ALL_BITS);
	glVertexPointerEXT(vStuff.sizeV, vStuff.typeV, vStuff.strideV,
			 vStuff.countV, vStuff.ptrV);
	break;
      case GL_NORMAL_ARRAY_EXT:
	vStuff.enabled &= ((~N3_BIT) & ALL_BITS);
	glNormalPointerEXT(vStuff.typeN, vStuff.strideN,
			 vStuff.countN, vStuff.ptrN);
	break;
      case GL_COLOR_ARRAY_EXT:
	vStuff.enabled &= ((~C3_BIT) & ALL_BITS);
	glColorPointerEXT(vStuff.sizeC, vStuff.typeC, vStuff.strideC,
			  vStuff.countC, vStuff.ptrC);
	break;
      case GL_TEXTURE_COORD_ARRAY_EXT:
	vStuff.enabled &= ((~T2_BIT) & ALL_BITS);
	glTexCoordPointerEXT(vStuff.sizeTC, vStuff.typeTC, vStuff.strideTC,
			     vStuff.countTC, vStuff.ptrTC);
	break;
      default:
	fprintf(stderr, "Ack, bad enum (%d) to VA_glEnable\n", what);
	break;
    }
}

#endif
