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

#include <stdio.h>
#include <stdlib.h>
struct __FLcontextRec;
#include "flclient.h"
#include "fl.h"

struct __FLcontextRec {
  const GLubyte *	fontPath;
  GLint			fontNamePreference;
  const GLubyte *	fontNameRestriction;
  GLfloat		pointsPerUMx;
  GLfloat		pointsPerUMy;

  FLfontNumber		current_font;	/* current font */
  GLint			numFont;	/* number of fonts in fontTable */
  FLfontStruct **	fontTable;	/* table of created font struct */
  GLfloat		hintValue[9];	/* Hint 1 to 8 (see flclient.h) */
};

static FLcontext current_context = NULL;

static FLfontStruct **
_flGetFontInfoList(GLubyte *fontList /* "fn1,fn2,fn3, ..." */)
{
  FLfontStruct **list;
  GLubyte *p, *p2;
  int numFonts, i, c;

  /* Count the number of fonts specified */
  numFonts = 0;
  for (p2 = p = fontList; (c = *p); p++)
    if (c == ',') {
      p2 = p;
      numFonts++;
    }

  if (p != p2)
    numFonts++;

  /* Create a list of pointers to font info struct */
  list = (FLfontStruct **) malloc((numFonts + 1) * sizeof(FLfontStruct *));
  if (list) {
    for (i = 0, p = fontList; *p; p++, i++) {
      for (p2 = p++; (c = *p) && c != ','; p++)
	;
      *p = '\0';
      list[i] = flGetFontInfo((FLfontNumber)atoi(p2));
      *p = c;
    }
    list[i] = NULL;
  }

  return list;
}

void
flAAColor(GLuint colorFull, GLuint colorHalf)
{
  FLcontext ctx = current_context;

  if (! ctx)
    return;

  /* TODO - not used in Inventor */
}

void
flAACpack(GLuint colorFull, GLuint colorHalf)
{
  FLcontext ctx = current_context;

  if (! ctx)
    return;

  /* TODO - not used in Inventor */
}

void
flCpack(GLuint color)
{
  FLcontext ctx = current_context;

  if (! ctx)
    return;

  /* TODO - not used in Inventor */
}

FLcontext
flCreateContext(const GLubyte *fontPath,
		GLint	       fontNamePreference,
		const GLubyte *fontNameRestriction,
		GLfloat	       pointsPerUMx,
		GLfloat	       pointsPerUMy)
{
  FLcontext ctx = (FLcontext) malloc(sizeof(struct __FLcontextRec));

  if (ctx) {
    ctx->fontPath			   = fontPath;
    ctx->fontNamePreference		   = fontNamePreference;
    ctx->fontNameRestriction		   = fontNameRestriction;
    ctx->pointsPerUMx			   = pointsPerUMx;
    ctx->pointsPerUMy			   = pointsPerUMy;

    ctx->current_font			   = BAD_FONT_NUMBER;
    ctx->numFont			   = 0;
    ctx->fontTable			   = NULL;

    /* FIXME: The hint values are mostly guess work! */
    ctx->hintValue[FL_HINT_AABITMAPFONTS]  = 0.0;
    ctx->hintValue[FL_HINT_CHARSPACING]	   = 1.0;
    ctx->hintValue[FL_HINT_FONTTYPE]	   = FL_FONTTYPE_OUTLINE;
    ctx->hintValue[FL_HINT_MAXAASIZE]	   = 2.0;
    ctx->hintValue[FL_HINT_MINOUTLINESIZE] = 12.0;
    ctx->hintValue[FL_HINT_ROUNDADVANCE]   = 1.0;
    ctx->hintValue[FL_HINT_SCALETHRESH]	   = 1.0;
    ctx->hintValue[FL_HINT_TOLERANCE]	   = 0.1;
  }

  TRACE(("flCreateContext: fontPath=[%s], ctx=%p\n",
	 SAFE_STR(fontPath), ctx));
  return ctx;
}

FLfontNumber
flCreateFont(const GLubyte *fontName,
	     GLfloat	    mat[2][2], 
	     GLint	    charNameCount,
	     GLubyte **	    charNameVector)
{
  FLcontext ctx = current_context;
  FLfontStruct *fs;
  FLfontNumber fn;

  TRACE(("flCreateFont: fontName=[%s], count=%d\n",
	 SAFE_STR(fontName), charNameCount));

  if (! ctx)
    return BAD_FONT_NUMBER;

  if (! (fs = _flCreateFont(fontName, mat, charNameCount, charNameVector)))
    return BAD_FONT_NUMBER;

  /* Search for an not used font handle */
  fn = 1;
  if (ctx->fontTable) {
    while (fn <= ctx->numFont)
      if (ctx->fontTable[fn])
	fn++;
      else
	break;
  }

  /* See if need to expand the font handle table */
  if (fn > ctx->numFont) {
    FLfontStruct **table;

    ctx->numFont++;

    if (! (table = (FLfontStruct **) realloc(ctx->fontTable, (ctx->numFont + 1) * sizeof(FLfontStruct *)))) {
      ctx->numFont--;
      _flDestroyFont(fs);
      return BAD_FONT_NUMBER;
    }

    ctx->fontTable = table;
  }

  fs->fn = fn;
  ctx->fontTable[fn] = fs;

  return fn;
}

void
flDestroyContext(FLcontext ctx)
{
  FLfontNumber fn;

  TRACE(("flDestroyContext: ctx=%p\n", ctx));

  if (ctx) {
    if (ctx == current_context)
      current_context = NULL;

    if (ctx->fontTable) {
      for (fn = 1; fn <= ctx->numFont; fn++)
	_flDestroyFont(ctx->fontTable[fn]);
      free(ctx->fontTable);
    }

    free(ctx);
  }
}

void
flDestroyFont(FLfontNumber fn)
{
  TRACE(("flDestroyFont: fn=%d\n", fn));
  _flDestroyFont(flGetFontInfo(fn));
}

void
flDrawCharacters(GLubyte *str)
{
  TRACE(("flDrawCharacters\n"));
  /* TODO - not used in Inventor */
}

void
flDrawNCharacters(void *str, 
		  GLint charCount,
		  GLint bytesPerCharacter)
{
  TRACE(("flDrawNCharacters: count=%d, bytesPerChar=%d\n",
	 charCount, bytesPerCharacter));
  /* TODO - not used in Inventor */
}

void
flEnumerateFonts(void (*callback)(GLubyte *))
{
  TRACE(("flEnumerateFonts\n"));
  /* TODO - not used in Inventor */
}

void
flEnumerateSizes(GLubyte *typeface, void (*callback)(GLfloat))
{
  TRACE(("flEnumerateSizes\n"));
  /* TODO - not used in Inventor */
}

void
flFreeBitmap(FLbitmap *bitmapPtr)
{
  TRACE(("flFreeBitmap: bitmapPtr=%p, bitmapPtr->bitmap=%p\n",
	 bitmapPtr, bitmapPtr ? bitmapPtr->bitmap : NULL));

  if (bitmapPtr && bitmapPtr->bitmap) {
    free(bitmapPtr->bitmap);
    bitmapPtr->bitmap = NULL;
  }
}

void
flFreeScalableBitmap(FLscalableBitmap *bitmapPtr)
{
  TRACE(("flFreeScalableBitmap\n"));

  if (bitmapPtr) {
    if (bitmapPtr->bitmap) free(bitmapPtr->bitmap);

    free(bitmapPtr);
  }
}

void
flFreeFontInfo(FLfontStruct *fontStruct)
{
  TRACE(("flFreeFontInfo\n"));

  if (fontStruct) {
    if (fontStruct->properties) free(fontStruct->properties);
    if (fontStruct->per_char)   free(fontStruct->per_char);

    free(fontStruct);
  }
}

void
flFreeFontNames(GLubyte **list)
{
  TRACE(("flFreeFontNames\n"));

  if (list) {
    free(list);
  }
}

void
flFreeFontSizes(GLfloat *list)
{
  TRACE(("flFreeFontSizes\n"));

  if (list) {
    free(list);
  }
}

void
flFreeOutline(FLoutline *outline)
{
  TRACE(("flFreeOutline\n"));

  if (outline) {
    int j;

    if (outline->vertexcount) free(outline->vertexcount);

    for (j = 0; j < outline->outlinecount; j++)
      if (outline->vertex[j])
	free(outline->vertex[j]);

    free(outline);
  }
}

GLboolean
flGetOutlineBBox(FLfontNumber fn, 
		 GLuint	      c, 
		 GLfloat *    llx,
		 GLfloat *    lly,
		 GLfloat *    urx,
		 GLfloat *    ury)
{
  TRACE(("flGetOutlintBBox\n"));

  /* TODO - not used in Inventor */

  return FALSE;
}

FLbitmap *
flGetBitmap(FLfontNumber fn, GLuint c)
{
  TRACE(("flGetBitmap: fn=%d, c='%c'(%u)\n", fn, c, c));
  return _flGetBitmap(flGetFontInfo(fn), c);
}

FLscalableBitmap *
flGetScalableBitmap(FLfontNumber fn, GLuint c)
{
  TRACE(("flGetScalableBitmap\n"));
  return _flGetScalableBitmap(flGetFontInfo(fn), c);
}

FLcontext
flGetCurrentContext(void)
{
  TRACE(("flGetCurrentContext: ctx=%p\n", current_context));
  return current_context;
}

FLfontNumber
flGetCurrentFont(void)
{
  FLcontext ctx = current_context;
  FLfontNumber fn = ctx ? ctx->current_font : BAD_FONT_NUMBER;

  TRACE(("flGetCurrentFont: fn=%d\n", fn));

  return fn;
}

FLfontStruct *
flGetFontInfo(FLfontNumber fn)
{
  FLcontext ctx = current_context;
  return (ctx && fn > 0 && fn <= ctx->numFont) ? ctx->fontTable[fn] : NULL;
}

FLoutline *
flGetOutline(FLfontNumber fn, GLuint c)
{
  TRACE(("flGetOutline: fn=%d, c='%c'(%u)\n", fn, c, c));
  return _flGetOutline(flGetFontInfo(fn), c);
}

GLboolean
flGetStringWidth(FLfontNumber fn,
		 GLubyte *    str, 
		 GLfloat *    dx,
		 GLfloat *    dy)
{
  TRACE(("flGetStringWidth: fn=%d\n", fn));
  /* TODO - not used in Inventor */
  return FALSE;
}

GLboolean
flGetStringWidthN(FLfontNumber fn,
		  void *       str,
		  GLint	       charCount,
		  GLint	       bytesPerCharacter, 
		  GLfloat *    dx,
		  GLfloat *    dy)
{
  TRACE(("flGetStringWidthN\n"));
  /* TODO - not used in Inventor */
  return FALSE;
}

GLubyte **
flListFonts(GLint maxNames, GLint *countReturn)
{
  TRACE(("flListFonts\n"));
  /* TODO - not used in Inventor */
  return NULL;
}

GLfloat *
flListSizes(const GLubyte *typeface, GLint *countReturn)
{
  TRACE(("flListSizes\n"));
  /* TODO - not used in Inventor */
  return NULL;
}

GLboolean
flMakeCurrentContext(FLcontext ctx)
{
  TRACE(("flMakeCurrentContext: ctx=%p\n", ctx));

  if (! ctx)
    return FALSE;

  current_context = ctx;
  return TRUE;
}

GLboolean
flMakeCurrentFont(FLfontNumber fn)
{
  FLcontext ctx = current_context;

  TRACE(("flMakeCurrentFont: fn=%d\n", fn));

  if (! ctx)
    return FALSE;

  ctx->current_font = fn;
  return TRUE;
}

FLfontNumber
flScaleRotateFont(const GLubyte *fontName,
		  GLfloat	 fontScale,
		  GLfloat	 angle)
{
  TRACE(("flScaleRotateFont: fontName=%s\n", fontName));
  /* TODO - not used in Inventor */
  return BAD_FONT_NUMBER;
}

void
flSetHint(GLuint hint, GLfloat hintValue)
{
  FLcontext ctx = current_context;
  if (hint > 0 && hint <= 8)
    ctx->hintValue[hint] = hintValue;
}

void
flGetFontBBox(FLfontNumber fn,
	      GLfloat *	   llx,
	      GLfloat *    lly,
	      GLfloat *	   urx,
	      GLfloat *	   ury,
	      GLint *	   isfixed)
{
  TRACE(("flGetFontBBox: fn=%d\n", fn));
  /* TODO - not used in Inventor */
}

FLbitmap *
flUniGetBitmap(GLubyte *fontList, /* "fn1,fn2,fn3, ..." */
	       GLubyte *UCS2)
{
  FLfontStruct **fsList;
  FLbitmap *bitmap;

  TRACE(("flUniGetBitmap\n"));

  fsList = _flGetFontInfoList(fontList);
  bitmap = _flUniGetBitmap(fsList, UCS2);
  free(fsList);
  return bitmap;
}

FLoutline *
flUniGetOutline(GLubyte *fontList, /* "fn1,fn2,fn3, ..." */
		GLubyte *UCS2)
{
  FLfontStruct **fsList;
  FLoutline *outline;

  TRACE(("flUniGetOutline\n"));

  fsList  = _flGetFontInfoList(fontList);
  outline = _flUniGetOutline(fsList, UCS2);
  free(fsList);
  return outline;
}

GLboolean
flUniGetOutlineBBox(GLubyte *fontList, /* "fn1,fn2,fn3, ..." */
		    GLubyte *UCS2,
		    GLfloat *llx, 
		    GLfloat *lly,
		    GLfloat *urx, 
		    GLfloat *ury)
{
  TRACE(("flUniGetOutlineBBox\n"));
  /* TODO - not used in Inventor */
  return FALSE;
}

FLscalableBitmap *
flUniGetScalableBitmap(GLubyte *fontList, /* "fn1,fn2,fn3, ..." */
		       GLubyte *UCS2)
{
  FLscalableBitmap *sb = NULL;

  TRACE(("flUniGetScalableBitmap\n"));
  /* TODO - not used in Inventor */
  return sb;
}
