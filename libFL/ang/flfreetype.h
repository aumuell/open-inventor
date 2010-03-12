#ifndef __flfreetype_h_
#define __flfreetype_h_

#include <ft2build.h>
#include FT_FREETYPE_H

#ifndef __fl_h_
#include "fl.h"
#endif /* __fl_h_ */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Copyright (c) 2000 Silicon Graphics, Inc.  All Rights Reserved.
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it would be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * Further, this software is distributed without any warranty that it is
 * free of the rightful claim of any third person regarding infringement
 * or the like.  Any license provided herein, whether implied or
 * otherwise, applies only to this software file.  Patent licenses, if
 * any, provided herein do not apply to combinations of this program with
 * other software, or any other product whatsoever.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write the Free Software Foundation, Inc., 59
 * Temple Place - Suite 330, Boston MA 02111-1307, USA.
 * 
 * Contact information: Silicon Graphics, Inc., 1600 Amphitheatre Pkwy,
 * Mountain View, CA  94043, or:
 * 
 * http://www.sgi.com 
 * 
 * For further information regarding this notice, see: 
 * 
 * http://oss.sgi.com/projects/GenInfo/NoticeExplan/
 */

typedef GLushort FLchar;	/* 16 bits wide char */

typedef struct FLFreeTypeOutline {
  FLoutline	outline;	/* FL font outline struct */
  FLbitmap	bitmap;		/* FL bitmap struct */

  FLchar	ch;		/* char code point */
  FLpt2		size;		/* size of char */
  GLfloat	advance;	/* advancement of char */
  GLboolean *	polygons;	/* flags indicating the face starts */
				/* a polygon or not */
  short *	indexes;	/* face indexes into vertices */
  short *	tessIndexes;	/* tesselated indexes */
  short		numVertices;	/* number of vertices */
  FLpt2 *	vertices;	/* vertices of character glyph */
  FT_GlyphSlot	glyph;		/* the glyph slot */
} FLFreeTypeOutline;
  
typedef struct FLFreeTypeFontStruct {
  FLfontStruct	fs;		/* FL font struct */

  char *	name;		/* font name */
  FLpt2		bound;		/* max char size */
  FLFreeTypeOutline **char8;	/* 8 bit char array */
  FLFreeTypeOutline **char16;	/* 16 bit char array */
  FLchar *	index16;	/* code point index into char16 */
  int		num16;		/* number of char16 */
  FT_Face	face;		/* the font face */
  GLboolean	hint;		/* is glyph hinting active? */
  GLboolean	grayRender;	/* is anti-aliasing active? */
  GLboolean	lowPrec;	/* force low precision */
} FLFreeTypeFontStruct;

extern GLboolean _flFTInitialize(
  void
);

extern void _flFTShutdown(
  void
);

extern FLfontStruct *_flFTCreateFont(
  const GLubyte *		/* fontName */,
  GLfloat [2][2]		/* mat */, 
  GLint				/* charNameCount */,
  GLubyte **			/* charNameVector */
);

extern void _flFTDestroyFont(
  FLfontStruct *		/* fs */
);

extern FLbitmap *_flFTGetBitmap(
  FLfontStruct *		/* fs */,
  GLuint                        /* c */
);

extern FLscalableBitmap *_flFTGetScalableBitmap(
  FLfontStruct *		/* fs */,
  GLuint                        /* c */
);

extern FLoutline *_flFTGetOutline(
  FLfontStruct *		/* fs */,
  GLuint			/* c */
);

extern FLbitmap *_flFTUniGetBitmap(
  FLfontStruct **		/* fsList */,
  GLubyte *			/* UCS2 */
);

extern FLoutline *_flFTUniGetOutline(
  FLfontStruct **		/* fsList */,
  GLubyte *			/* UCS2 */
);

extern FLFreeTypeOutline *_flFTNewGlyphChar(
  FLFreeTypeFontStruct *	/* fs */, 
  FLchar			/* c */,
  FT_GlyphSlot			/* slot */
);

extern void _flFTDeleteGlyphChar(
  FLFreeTypeOutline *		/* ch */
);

extern short _flFTGetNumVertices(
  FLFreeTypeOutline *		/* ch */
);

extern short *_flFTGetIndices(
  FLFreeTypeOutline *		/* ch */
);

extern GLboolean *_flFTGetPolygonFlags(
  FLFreeTypeOutline *		/* ch */
);

extern FLpt2 *_flFTGetVertices(
  FLFreeTypeOutline *		/* ch */
);

extern void _flFTSetIndices(
  FLFreeTypeOutline *		/* ch */,
  short *			/* idx */
);

extern void _flFTSetVertices(
  FLFreeTypeOutline *		/* ch */,
  short				/* num */,
  FLpt2 *			/* vtx */
);

extern int _flFTFinalizeFaceTable(
  FLFreeTypeOutline *		/* ch */,
  FT_Raster			/* raster */
);

extern int _flFTConvertGlyph(
  FLFreeTypeOutline *		/* ch */,
  FT_Raster			/* raster */,
  FT_Outline *			/* outline */
);

extern int _flFTGenerateGlyph(
  FLFreeTypeOutline *		/* ch */
);

extern FLFreeTypeFontStruct *_flFTNewGlyphFont(
  const char *			/* fontName */,
  FT_Face			/* newFace */
);

extern void _flFTDeleteGlyphFont(
  FLFreeTypeFontStruct *	/* font */
);

extern FLFreeTypeOutline *_flFTLoadChar(
  FLFreeTypeFontStruct *	/* font */,
  FLchar			/* c */
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __flfreetype_h_ */
