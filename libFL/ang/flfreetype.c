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
#include <string.h>
#include <math.h>
#include "fl.h"
#include "flfreetype.h"

typedef unsigned short SbUCS2;

/* from ftoutln.h */
extern FT_Error FT_Outline_Get_Bitmap(FT_Library  library,
				      FT_Outline *outline,
				      FT_Bitmap*  map);

#ifndef PI
#define PI			3.141592654
#endif

#define RADIAN(deg)		((deg)/180.0*PI)

#define KLUDGE_FACTOR		80.0
#define PIXEL_ROW_ALIGNMENT	4

/* Bitmap precisions */
#define BM_PRECISION_BITS	6
#define BM_PRECISION		(1 << BM_PRECISION_BITS)
#define BM_PRECISION_MASK	(-1L << BM_PRECISION_BITS)

#define BM_FLOOR(x)		((x) & BM_PRECISION_MASK)
#define BM_CEILING(x)		(((x) + BM_PRECISION - 1) & BM_PRECISION_MASK)
#define BM_TRUNC(x)		((signed long)(x) >> BM_PRECISION_BITS)

static FT_Library		library;

GLboolean
_flFTInitialize(void)
{
  FT_Error error;

  error = FT_Init_FreeType(&library);
  if (error) {
    fprintf(stderr, "_flFTInitialize: initialise FreeType failed\n");
    return FALSE;
  }

  return TRUE;	/* OK */
}

void
_flFTShutdown(void)
{
  /* TODO */
}

FLfontStruct *
_flFTCreateFont(const GLubyte *fontName,
		GLfloat	       mat[2][2],
		GLint	       charNameCount,
		GLubyte **     charNameVector)
{
  FLfontStruct *fs;
  char *	path;
  FT_Face	face;
  FT_Error	error;

  if (! (path = _flSearchFont(fontName)))
    return NULL;

  error = FT_New_Face(library, path, 0, &face);

  free(path);

  if (! error) {
    int px = (mat[0][0] == 1.0) ? 32 : mat[0][0];
    int py = (mat[1][1] == 1.0) ? 32 : mat[1][1];

    error = FT_Set_Char_Size(face, px << 6, py << 6, mat[0][1], mat[1][0]);

    if (! error) {
      fs = (FLfontStruct *) _flFTNewGlyphFont(fontName, face);
      return fs;
    }
  }

  return NULL;
}

void
_flFTDestroyFont(FLfontStruct *fs)
{
  FLFreeTypeFontStruct *fontStruct = (FLFreeTypeFontStruct *) fs;
  FT_Face face = fontStruct->face;
  FT_Done_Face(face);
}

FLscalableBitmap *
_flFTGetScalableBitmap(FLfontStruct *_fs, GLuint c)
{
  FLscalableBitmap *bit3;

  /* TODO */
  bit3 = NULL;

  return bit3;
}

static void
bitmap_convert(int width, int height, int pitch, int pitch2, char *src, char *dst)
{
  int i, j, c;
  unsigned char bit;
  char *p;

  for (j = 0; j < height; j++) {
    bit = 0x80;
    p = src;
    c = *p;
    for (i = 0; i < width; i++) {
      if ((c & bit))
	dst[(height - 1 - j) * pitch2 + i / 8] |= (1 << (7 - (i & 7)));

      bit >>= 1;
      if (! bit) {
	bit = 0x80;
	p++;
	c = *p;
      }
    }

    src += pitch;
  }
}

FLbitmap *
_flFTGetBitmap(FLfontStruct *_fs, GLuint c)
{
  FLFreeTypeFontStruct *fs = (FLFreeTypeFontStruct *) _fs;
  FLFreeTypeOutline *outline = (FLFreeTypeOutline *) _flFTGetOutline(_fs, c);
  FT_Face face = fs->face;
  FT_GlyphSlot glyph = outline->glyph;
  FT_Bitmap  bit2;
  FLbitmap *bit3;
  FT_Error error;
  int width, height, pitch, size;
  int left, right, top, bottom;
  int bbox_width, bbox_height;
  int bearing_x, bearing_y;
  int pitch2, size2;

  TRACE(("_flFTGetBitmap: '%c'(0x%04x)\n", c, c));

  /* See if it is already created */
  bit3 = &outline->bitmap;
  if (bit3->bitmap)
    return bit3;

  /* No, proceed to create it */
  left        = BM_FLOOR(glyph->metrics.horiBearingX);
  right       = BM_CEILING(glyph->metrics.horiBearingX + glyph->metrics.width);
  width       = BM_TRUNC(right - left);
    
  top         = BM_CEILING(glyph->metrics.horiBearingY);
  bottom      = BM_FLOOR(glyph->metrics.horiBearingY - glyph->metrics.height);
  height      = BM_TRUNC(top - bottom);

  bearing_x   = BM_TRUNC(glyph->metrics.horiBearingX);
  bearing_y   = BM_TRUNC(glyph->metrics.horiBearingY);

  bbox_width  = BM_TRUNC(face->bbox.xMax - face->bbox.xMin);
  bbox_height = BM_TRUNC(face->bbox.yMax - face->bbox.yMin);

  if (glyph->format == ft_glyph_format_outline) {    
    pitch     = (width + 7) >> 3;
    size      = pitch * height; 
    pitch2    = ((width + (PIXEL_ROW_ALIGNMENT << 3) - 1) >> 5) << 2;
    size2     = pitch2 * height;

    bit2.width      = width;
    bit2.rows       = height;
    bit2.pitch      = pitch;
    bit2.pixel_mode = ft_pixel_mode_mono;
    bit2.buffer     = (GLubyte *) malloc(size * sizeof(GLubyte));

    bit3->width     = bit2.width;
    bit3->height    = bit2.rows;
    bit3->xorig     = bearing_x;
    bit3->yorig     = height - bearing_y;
    bit3->xmove     = width > 0 ? width : (bbox_width / 2.0);
    bit3->ymove     = 0.0;
    bit3->bitmap    = (GLubyte *) malloc(size2 * sizeof(GLubyte));

    FT_Outline_Translate(&glyph->outline, -left, -bottom);
    memset(bit2.buffer, 0, size);
    memset(bit3->bitmap, 0, size2);

    if (fs->lowPrec)
      glyph->outline.flags &= ~ft_outline_high_precision;
      
    error = FT_Outline_Get_Bitmap(library, &glyph->outline, &bit2);

    bitmap_convert(width, height, pitch, pitch2, bit2.buffer, bit3->bitmap);

    free(bit2.buffer);
  }
  else {
    bit3->width     = glyph->bitmap.width;
    bit3->height    = glyph->bitmap.rows;
    bit3->xorig     = bearing_x;
    bit3->yorig     = height - bearing_y;
    bit3->xmove     = bit3->width > 0 ? bit3->width : (bbox_width / 2.0);
    bit3->ymove     = 0.0;
    bit3->bitmap    = glyph->bitmap.buffer;
  }

  return bit3;
}

FLoutline *
_flFTGetOutline(FLfontStruct *_fs, GLuint c)
{
  FLFreeTypeFontStruct *fs = (FLFreeTypeFontStruct *) _fs;
  FLFreeTypeOutline *outline;
  int start, end, num, code, i;

  TRACE(("_flFTGetOutline: '%c'(0x%04x)\n", c, c));

  // Search for the code point c in the 8 bit range
  if (c < 256) {
    outline = fs->char8[c];
    if (! outline)
      fs->char8[c] = outline = _flFTLoadChar(fs, c);
    return (FLoutline *) outline;
  }

  // Search for the code point c in the 16 bit range
  start = 0;
  end   = fs->num16 - 1;

  for (i = end >> 1; start < end; i = (start + end) >> 1) {
    code = fs->index16[i];
    if (c < code)
      end = i - 1;
    else if (c > code)
      start = i + 1;
    else
      return (FLoutline *) fs->char16[i];
  }

  // Cannot find code point c, load the glyph char
  outline = _flFTLoadChar(fs, c);
  if (outline) {
    FLchar *newIndex16;
    FLFreeTypeOutline **newChar16;

    fs->num16++;
    newIndex16 = (FLchar *) malloc(fs->num16 * sizeof(FLchar));
    newChar16  = (FLFreeTypeOutline **) malloc(fs->num16 * sizeof(FLFreeTypeOutline *));

    if (fs->index16) {
      if (start > 0) {
	memcpy(newIndex16, fs->index16, sizeof(FLchar) * start);
	memcpy(newChar16, fs->char16, sizeof(FLFreeTypeOutline *) * start);
      }

      if ((num = fs->num16 - start - 1) > 0) {
	memcpy(newIndex16 + start + 1, fs->index16 + start, sizeof(FLchar) * num);
	memcpy(newChar16 + start  + 1, fs->char16 + start, sizeof(FLFreeTypeOutline *) * num);
      }

      free(fs->index16);
      free(fs->char16);
    }

    fs->index16 = newIndex16;
    fs->char16  = newChar16;

    fs->index16[start] = c;
    fs->char16[start]  = outline;
  }
    
  return (FLoutline *) outline;
}

FLbitmap *
_flFTUniGetBitmap(FLfontStruct **fsList, GLubyte *UCS2)
{
  FLfontStruct *fs;
  FLbitmap *bitmap;
  GLuint c = (UCS2[0] << 8) | UCS2[1];

  TRACE(("_flFTUniGetBitmap: 0x%04x\n", c));
  while ((fs = *fsList++))
    if ((bitmap = _flFTGetBitmap(fs, c)))
      return bitmap;

  return NULL;
}

FLoutline *
_flFTUniGetOutline(FLfontStruct **fsList, GLubyte *UCS2)
{
  FLfontStruct *fs;
  FLoutline *outline;
  GLuint c = (UCS2[0] << 8) | UCS2[1];

  TRACE(("_flFTUniGetOutline: 0x%04x\n", c));
  while ((fs = *fsList++))
    if ((outline = _flFTGetOutline(fs, c)))
      return outline;

  return NULL;
}

/*------------------------------------------------------------------*/

FLFreeTypeOutline *
_flFTNewGlyphChar(FLFreeTypeFontStruct *fs, FLchar c, FT_GlyphSlot slot)
{
  FLFreeTypeOutline *outline;
  FLbitmap *bitmap;

  TRACE(("_flFTNewGlyphChar: '%c'(0x%x), slot=%p\n",
	 c < 128 ? c : '?', c, slot));

  if (! (outline = (FLFreeTypeOutline *) malloc(sizeof(FLFreeTypeOutline))))
    return NULL;

  outline->ch          = c;
  outline->size.x      = 0.0;
  outline->size.y      = 0.0;
  outline->advance     = 0.0;
  outline->indexes     = NULL;
  outline->tessIndexes = NULL;
  outline->numVertices = 0;
  outline->vertices    = NULL;
  outline->glyph       = slot;

  _flFTGenerateGlyph(outline);

  bitmap               = &outline->bitmap;
  bitmap->width        = 0;
  bitmap->height       = 0;
  bitmap->xorig        = 0.0;
  bitmap->yorig        = 0.0;
  bitmap->xmove        = 0.0;
  bitmap->ymove        = 0.0;
  bitmap->bitmap       = NULL;

  return outline;
}

void
_flFTDeleteGlyphChar(FLFreeTypeOutline *outline)
{
  TRACE(("_flFTDeleteGlyphChar: outline=%p\n", outline));

  if (outline) {
    FT_Done_GlyphSlot(outline->glyph);
  }
}

short
_flFTGetNumVertices(FLFreeTypeOutline *outline)
{
  if (outline) {
    if (! outline->indexes)
      ; /*  _flFTGenerateGlyph(); */
    return outline->numVertices;
  }

  return 0;
}

short *
_flFTGetIndices(FLFreeTypeOutline *outline)
{
  if (outline) {
    if (! outline->indexes)
      ; /*  _flFTGenerateGlyph(); */
    return outline->indexes;
  }

  return NULL;
}

FLpt2 *
_flFTGetVertices(FLFreeTypeOutline *outline)
{
  if (outline) {
    if (! outline->indexes)
      ; /* _flFTGenerateGlyph(); */
    return outline->vertices;
  }

  return NULL;
}

GLboolean *
_flFTGetPolygonFlags(FLFreeTypeOutline *outline)
{
  if (outline) {
    if (! outline->indexes)
      ; /* _flFTGenerateGlyph(); */
    return outline->polygons;
  }

  return NULL;
}

/*------------------------------------------------------------------*/

FLFreeTypeFontStruct *
_flFTNewGlyphFont(const char *fontName, FT_Face newFace)
{
  FLFreeTypeFontStruct *fs;

  if (! (fs = (FLFreeTypeFontStruct *) malloc(sizeof(FLFreeTypeFontStruct))))
    return NULL;

  fs->char8      = (FLFreeTypeOutline **) malloc(256 * sizeof(FLFreeTypeOutline *));
  fs->char16     = NULL;
  fs->index16    = NULL;
  fs->num16      = 0;
  memset(fs->char8, 0, 256 * sizeof(FLFreeTypeOutline *));

  fs->name       = strdup(fontName);
  fs->index16    = NULL;
  fs->face       = newFace;
  fs->hint       = TRUE;
  fs->grayRender = TRUE;
  fs->lowPrec    = FALSE;

  fs->bound.x = (newFace->bbox.xMax - newFace->bbox.xMin) >> 5;
  fs->bound.y = (newFace->bbox.yMax - newFace->bbox.yMin) >> 5;

  fs->bound.x /= KLUDGE_FACTOR;
  fs->bound.y /= KLUDGE_FACTOR;
  if (fs->bound.y < 0.6)
    fs->bound.y = 1.2;

#if 0
  {
    int i;

    printf(">>> num_charmaps=%d\n", newFace->num_charmaps);
    for (i = 0; i < newFace->num_charmaps; i++) {
      char *s = "???";
      switch (newFace->charmaps[i]->encoding) {
      case ft_encoding_symbol:	       s = "symbol"; break;
      case ft_encoding_unicode:	       s = "unicode"; break;
      case ft_encoding_latin_2:	       s = "latin_2"; break;
      case ft_encoding_sjis:	       s = "sjis"; break;
      case ft_encoding_big5:	       s = "big5"; break;
      case ft_encoding_adobe_standard: s = "adobe_standard"; break;
      case ft_encoding_adobe_expert:   s = "adobe_expert"; break;
      case ft_encoding_adobe_custom:   s = "adobe_custom"; break;
      case ft_encoding_apple_roman:    s = "apple_roman"; break;
      }
      printf(">>>   %d. encoding=%s, platform_id=%d, encoding_id=%d\n",
	     i, s, newFace->charmaps[i]->platform_id, newFace->charmaps[i]->encoding_id);
    }
  }
#endif

  return fs;
}

void
_flFTDeleteGlyphFont(FLFreeTypeFontStruct *fs)
{
  if (fs) {
    if (fs->name) free(fs->name);
    if (fs->face) FT_Done_Face(fs->face);

    free(fs);
  }
}

FLFreeTypeOutline *
_flFTLoadChar(FLFreeTypeFontStruct *fs, FLchar c)
{
  FLFreeTypeOutline *ch;
  FLoutline *outline;
  FT_UInt i;
  FT_Error error;
  GLshort *vertexcount;
  FLpt2 **vertex, *v;
  short *index, *start, *end;
  int flags, j;

  flags = FT_LOAD_DEFAULT | FT_LOAD_NO_BITMAP;
  if (! fs->hint)
    flags |= FT_LOAD_NO_HINTING;

  i = FT_Get_Char_Index(fs->face, c);
  error = FT_Load_Glyph(fs->face, i, flags);

  ch = error ? NULL : _flFTNewGlyphChar(fs, c, fs->face->glyph);
  if (! ch) {
    TRACE(("_flFTLoadChar: c='%c'(%d) failed, error=%d\n", c, c, error));
    return NULL;
  }

  /* Fix character advancement */
  if (ch->size.x == 0.0)
    ch->advance = (fs->face->max_advance_width >> 6) / KLUDGE_FACTOR;

  TRACE(("_flFTLoadChar: c='%c'(%d), numVertices=%d\n",
	 c, c, ch->numVertices));

  /* Fill up FL outline struct */
  outline = &ch->outline;
  outline->outlinecount = 0;
  outline->xadvance = ch->advance;
  outline->yadvance = 0.0;

  if (ch->numVertices == 0) {
    outline->vertexcount = NULL;
    outline->vertex = NULL;
    return ch;
  }

  /* Count the number of outline contours */
  for (index = ch->indexes ; *index != -1; index++, outline->outlinecount++)
    for (index++; *index != -1; index++)
      ;

  /* Create the vertex count and vertex arrays */
  vertexcount = (GLshort *) malloc(outline->outlinecount * sizeof(GLshort));
  vertex = (FLpt2 **) malloc(outline->outlinecount * sizeof(FLpt2 *));
  outline->vertexcount = vertexcount;
  outline->vertex = vertex;

  for (index = ch->indexes ; *(start = index) != -1; index++) {
    for (index++; *index != -1; index++)
      ;
    *vertexcount = index - start;
    *vertex = (FLpt2 *) malloc(*vertexcount * sizeof(FLpt2));
    v = *vertex;

    end = index--;
    while (index >= start) {
      j = *index--;
      v->x = ch->vertices[j].x;
      v->y = ch->vertices[j].y;
      v++;
    }
    index = end;

    vertexcount++;
    vertex++;
  }

  return ch;
}

/*------------------------------------------------------------------*/

/*

  The following code was adapted from FreeType2's scan convertion
  code in rasterization routines (ftraster.c).

  The function FT_Outline_Decompose() is called to decompose the
  outlines into series of Move_To, Line_To, Conic_To, and Cubic_To
  functions. In Conic_To and Cubic_To functions, the given curve is
  repeatedly split into halves with Split_Conic and Splic_Cubic,
  respcetively, until they get segments straight enough to be
  approximated with lines, ie. until the angles of a segment's
  bounding triangle or quadrangle corresponding to its endpoints are
  small enough (plus there should be some additional criterion for 
  cubic curves with an inflexion (ie. S-shaped curves)).

*/

#define FT_RASTER_CONSTANT_PRECISION
/* #define FT_DYNAMIC_BEZIER_STEPS */
#define FT_PRECISION_BITS    8

#undef  FAILURE
#define FAILURE  TRUE

#undef  SUCCESS
#define SUCCESS  FALSE

/* Please don't touch the following macros. Their importance is
 * historical to FreeType, but they have some nice effects, like getting
 * rid of all `->' symbols when accessing the raster object (replacing
 * them with a simple `.').
 */

/* used in function signatures to define the _first_ argument */
#define RAS_ARG_  FT_Raster  raster,
#define RAS_ARG   FT_Raster  raster

/* used to call a function within this component, first parameter */
#define RAS_VAR_  raster,
#define RAS_VAR   raster

/* used to access the current raster object, with a `.' instead of a `->' */
#define ras       (*raster)

/* Error codes returned by the scan-line converter/raster. */
#define ErrRaster_Ok                     0
#define ErrRaster_Uninitialized_Object   1
#define ErrRaster_Overflow               2
#define ErrRaster_Negative_Height        3
#define ErrRaster_Invalid_Outline        4
#define ErrRaster_Invalid_Map            5
#define ErrRaster_AntiAlias_Unsupported  6
#define ErrRaster_Invalid_Pool           7
#define ErrRaster_Unimplemented          8
#define ErrRaster_Bad_Palette_Count      9

#define Flow_Up     1
#define Flow_Down  -1

#define SET_High_Precision(p)	Set_High_Precision (RAS_VAR_  p)

/* Fast MulDiv, as `b' is always < 64. Don't use intermediate precision. */
#define FMulDiv(a, b, c)	((a) * (b) / (c))

/* The maximum number of stacked Bezier curves.  Setting this constant
 * to more than 32 is a pure waste of space.
 */
#define MaxBezier		32

/* The number fractional bits of *input* coordinates.  We always use the
 * 26.6 format (i.e, 6 bits for the fractional part), but hackers are
 * free to experiment with different values.
 */
#define INPUT_BITS		6

/* The lower and upper thresholds for determining whether to continue
 * splitting line segments and whether to reject a line segment as being
 * too straight.
 */
#define LOWER_THRESHOLD		RADIAN(160.0)
#define UPPER_THRESHOLD		RADIAN(170.0)

/* Some common types */
typedef unsigned char Byte, *PByte;
typedef int           TResult;

/* The type of the pixel coordinates used within the render pool during
 * scan-line conversion.  We use longs to store either 26.6 or 22.10
 * fixed float values, depending on the `precision' we want to use
 * (i.e., low resp. high precision).  These are ideals in order to
 * subdivise Bezier arcs in halves by simple additions and shifts.
 *
 * Note that this is an 8-bytes integer on 64 bits systems.
 */
typedef long TPos, *PPos;

/* This structure defines a point in a plane. */
typedef struct TPoint {
  TPos  x, y; 
} TPoint;

/* The type of a scanline position/coordinate within a map. */
typedef int  TScan, *PScan;

/* States and directions of each line, arc, and face. */
typedef enum TDirection {
  Unknown,
  Ascending,
  Descending,
  Flat
} TDirection;

/* The `master' structure used for decomposing outlines. */
typedef struct TFace {
  struct TFace * link;	/* link to next face - various purpose */
  TPoint *offset;	/* start of face's data in render pool */
  int	  n_points;	/* number of points */
  int     flow;		/* Face orientation: Asc/Descending */
  struct TFace * next;	/* next face in same contour, used */
			/* during drop-out control */
} TFace;

/* The size in _TPos_ of a face record in the render pool. */
#define AlignFaceSize  \
  ((sizeof(TFace) + sizeof(TPos) - 1) / sizeof(TPos))

#ifdef FT_RASTER_CONSTANT_PRECISION

#define PRECISION_BITS    FT_PRECISION_BITS
#define PRECISION         (1 << PRECISION_BITS)
#define PRECISION_MASK    (-1L << PRECISION_BITS)
#define PRECISION_HALF    (PRECISION >> 1)
#define PRECISION_JITTER  (PRECISION >> 5)
#define PRECISION_STEP    PRECISION_HALF

#else

#define PRECISION_BITS    ras.precision_bits
#define PRECISION         ras.precision
#define PRECISION_MASK    ras.precision_mask
#define PRECISION_HALF    ras.precision_half
#define PRECISION_JITTER  ras.precision_jitter
#define PRECISION_STEP    ras.precision_step

#endif

/* Compute lowest integer coordinate below a given value. */
#define FLOOR(x)	((x) & PRECISION_MASK)

/* Compute highest integer coordinate above a given value. */
#define CEILING(x)	(((x) + PRECISION - 1) & PRECISION_MASK)

/* Get integer coordinate of a given 26.6 or 22.10 `x' coordinate -- no
 * rounding.
 */
#define TRUNC(x)	((signed long)(x) >> PRECISION_BITS)

/* Get the fractional part of a given coordinate. */
#define FRAC(x)		((x) & (PRECISION-1))

/* Scale an `input coordinate' (as found in FT_Outline structures) into
 * a `work coordinate' which depends on current resolution and render mode.
 */
#define SCALED(x)	(((x) << ras.scale_shift) - ras.scale_delta)

/* Show command for debugging. */
#if 0
#define SHOW_COMMAND(cmd, px, py) printf("%s %ld,%ld\n", cmd, px, py)
#else
#define SHOW_COMMAND(cmd, px, py)
#endif

/* Records a generated point */
#define ENTER_POINT(top, px, py, cmd) do {			\
    SHOW_COMMAND(cmd, px, py);					\
    top->x = px;						\
    top->y = py;						\
    top++;							\
  } while (0)

#define ENTER_POINT_CHECK(top, px, py, cmd) do {		\
    if (px != (top - 1)->x && py != (top - 1)->y) {		\
      ENTER_POINT(top, px, py, cmd);				\
    }								\
  } while (0)

/* The most used variables are at the beginning of the structure.  Thus,
 * their offset can be coded with less opcodes which results in a
 * smaller executable.
 */
typedef struct FT_RasterRec_ {
  TPoint *    cursor;		/* Current cursor in render pool */
  TPoint *    pool;		/* The render pool base address */
  TPoint *    pool_size;	/* The render pool's size */
  TPoint *    pool_limit;	/* Limit of faces zone in pool */

  TPoint      last;
  long        minY, maxY;
  long        minX, maxX;

  int         error;

#ifndef FT_RASTER_CONSTANT_PRECISION
  int         precision_bits;	/* precision related variables */
  int         precision;
  int         precision_half;
  long        precision_mask;
  int         precision_shift;
  int         precision_step;
  int         precision_jitter;
#endif

  FT_Outline *outline;

  int         n_points;		/* number of points in current glyph */
  int         n_contours;	/* number of contours in current glyph */
  int         n_extrema;	/* number of `extrema' scanlines */

  TPoint *    arc;		/* current Bezier arc pointer */

  int         num_faces;	/* current number of faces */

  char        fresh;		/* signals a fresh new face which */
				/* `start' field must be completed */
  char        joint;		/* signals that the last arc ended */
				/* exactly on a scanline.  Allows */
				/* removal of doublets */
  TFace *     cur_face;		/* current face */
  TFace *     start_face;	/* head of linked list of faces */
  TFace *     first_face;	/* contour's first face in case */
				/* of impact */
  TDirection  state;	/* rendering state */

  int         scale_shift;	/* == 0  for bitmaps */
				/* == 1  for 5-levels pixmaps */
				/* == 2  for 17-levels pixmaps */

  int         scale_delta;	/* ras.precision_half for bitmaps */
				/* 0 for pixmaps */

  TPoint      arcs[2 * MaxBezier + 1]; /* The Bezier stack */
} FT_RasterRec_;


/* A function type describing the functions used to split bezier arcs */
typedef void (*TSplitter) (TPoint *base);

/*
 * Sets precision variables according to the parameter flag.
 */
static void
Set_High_Precision (RAS_ARG_ char  High)
{
#ifdef FT_RASTER_CONSTANT_PRECISION
  (void)High;
  (void)&ras;
#else
  if (High) {
    ras.precision_bits   = 10;
    ras.precision_step   = 128;
    ras.precision_jitter = 24;
  }
  else {
    ras.precision_bits   = 6;
    ras.precision_step   = 32;
    ras.precision_jitter = 2;
  }

  ras.precision       = 1L << ras.precision_bits;
  ras.precision_half  = ras.precision / 2;
  ras.precision_shift = ras.precision_bits - INPUT_BITS;
  ras.precision_mask  = -ras.precision;
#endif
}

/*
 * Creates a new Face in the render pool.
 */
static TResult
New_Face (RAS_ARG_ TDirection direction)
{
  if (ras.start_face == NULL) {
    ras.cur_face   = (TFace *)ras.cursor;	/* current face */
    ras.start_face = ras.cur_face;		/* first face in pool */
    ras.cursor    += AlignFaceSize;		/* record face in buffer */
  }

  /* check for overflow */
  if (ras.cursor >= ras.pool_limit) {
    ras.error = ErrRaster_Overflow;
    return FAILURE;
  }

  /* record face direction */
  switch (direction) {
  case Ascending:
    ras.cur_face->flow = Flow_Up;
    break;

  case Descending:
    ras.cur_face->flow = Flow_Down;
    break;

  default:
    ras.error = ErrRaster_Invalid_Map;
    return FAILURE;
  }

  /* initialize a few fields */
  {
    TFace *  cur = ras.cur_face;

    cur->offset = ras.cursor;	/* address of first coordinate */
    cur->link   = (TFace *)0;	/* link to next face in pool */
    cur->next   = (TFace *)0;	/* link to next face in contour */
  }

  /* record the first face in a contour */
  if (ras.first_face == NULL)
    ras.first_face = ras.cur_face;

  ras.state = direction;
  ras.fresh = TRUE;		/* this face has no coordinates yet */
  ras.joint = FALSE;

  return SUCCESS;
}

/*
 * Finalizes the current Face and computes its height.  If it is
 * not 0, the face's fields are updated and a new face is
 * pushed on top of its coordinates.  Otherwise the current face
 * is kept and the recording of intersections is restarted.
 */
static TResult
End_Face (RAS_ARG)
{
  int  n;

  n = ras.cursor - ras.cur_face->offset;

  if (n < 0) {
    /* This error should _never_ occur unless the raster is buggy */
    ras.error = ErrRaster_Negative_Height;
    return FAILURE;
  }

  if (n > 0) {
    TFace *oldp, *newp;

    /* record scanline height in current face, create a new one
       and set a link from the old one to it */
    oldp           = ras.cur_face;
    oldp->n_points = n;
    ras.cur_face   = newp = (TFace *)ras.cursor;

    ras.cursor    += AlignFaceSize;

    newp->n_points = 0;
    newp->offset   = ras.cursor;
    oldp->next     = newp;

    ras.num_faces++;
  }

  /* check for overflow */
  if (ras.cursor >= ras.pool_limit) {
    ras.error = ErrRaster_Overflow;
    return FAILURE;
  }

  ras.joint = FALSE;

  return SUCCESS;
}

#if 0
/*
 * Records that a given scanline contains at least one local
 * extremum.  The table of extrema is placed at the end of the render
 * pool and grows downwards.  It is used during the sweep phase.
 */
static TResult
Insert_Extrema (RAS_ARG_ TScan y)
{
  TPoint *extrema;
  TScan  y2;
  int    n;

  n       = ras.n_extrema - 1;
  extrema = ras.pool_size - ras.n_extrema;

  /* look for first y extremum that is <= */
  while (n >= 0 && y < extrema[n])
    n--;

  /* if it is <, simply insert it, ignore if == */
  if (n >= 0 && y > extrema[n])
    while (n >= 0) {
      y2 = extrema[n];
      extrema[n] = y;
      y = y2;
      n--;
    }

  if (n < 0) {
    ras.pool_limit--;
    ras.n_extrema++;
    ras.pool_size[-ras.n_extrema] = y;

    if (ras.pool_limit <= ras.cursor) {
      ras.error = ErrRaster_Overflow;
      return FAILURE;
    }
  }
  return SUCCESS;
}
#endif /* 0 */

static int
Compute_Orientation (int n_points, TPoint *pt, FT_BBox *bbox)
{
  FLpt2 v1, v2;
  TPoint *p[3];
  int n;
  
  for (n = 0; n_points--; pt++)
    if (pt->x == bbox->xMin || pt->x == bbox->xMax ||
        pt->y == bbox->yMin || pt->y == bbox->yMax) {
      p[n++] = pt;
      
      if (n == 3)
        break;
    }

  v1.x = p[1]->x - p[0]->x;
  v1.y = p[1]->y - p[0]->y;
  v2.x = p[2]->x - p[1]->x;
  v2.y = p[2]->y - p[1]->y;
  return (v1.x * v2.y - v1.y * v2.x) > 0.0 ? 1 : -1;
}

/*
 * Adjusts all links in the faces list.  Called when the outline
 * parsing is done.
 */
int
_flFTFinalizeFaceTable(  FLFreeTypeOutline *ch, RAS_ARG)
{
  TFace *	p, *start;
  short *	index;
  GLboolean *	flag;
  FLpt2 *	vertex;
  FLpt2		vmin, vmax, v1;
  FT_BBox	bbox, lastBBox;
  GLboolean	isPolygon;
  int		direction, lastDirection;
  int		n, i, j;

  n = ras.num_faces;
  start = ras.start_face;

  if (n > 1) {
    p = ras.start_face;
    while (n > 0) {
      if (n > 1)
	p->link = (TFace *) (p->offset + p->n_points);
      else
	p->link = NULL;

#if 0
      switch (p->flow) {
      case Flow_Down:
	bottom     = p->start - p->height+1;
	top        = p->start;
	p->start   = bottom;
	p->offset += p->height-1;
	break;

      case Flow_Up:
      default:
	bottom = p->start;
	top    = p->start + p->height-1;
      }

      if (Insert_Extrema (RAS_VAR_ bottom) ||
	  Insert_Extrema (RAS_VAR_ top+1))
	return FAILURE;
#endif

      p = p->link;
      n--;
    }
  }
  else
    ras.start_face = NULL;

  /* Compute the size require for indexes array. */
  ch->numVertices = 0;
  for (p = start, n = ras.num_faces; n--; p = p->link)
    ch->numVertices += p->n_points;

  index  = (short *) malloc ((ch->numVertices + ras.num_faces + 1) * sizeof(short));
  flag   = (GLboolean *) malloc (ras.num_faces * sizeof(GLboolean));
  vertex = (FLpt2 *) malloc (ch->numVertices * sizeof(FLpt2));

  /* Build up indexes and vertices from points, compute the size and
     character advance as well */
  ch->indexes = index;
  ch->polygons = flag;
  ch->vertices = vertex;
  lastDirection = 0;
  j = 0;

  vmin.x = vmin.y = 0.0;
  vmax.x = vmax.y = 0.0;

  for (p = start, n = ras.num_faces; n--; p = p->link) {
    TPoint *p0 = &p->offset[0];
    bbox.xMin = bbox.xMax = p0->x;
    bbox.yMin = bbox.yMax = p0->y;

    /* Store scaled points and assign indexs to each point */
    for (i = 0; i < p->n_points; i++) {
      TPoint *pt = &p->offset[i];
      
      /* Compute the bounding box of the contour */
      if (pt->x < bbox.xMin)
	bbox.xMin = pt->x;
      else if (pt->x > bbox.xMax)
	bbox.xMax = pt->x;

      if (pt->y < bbox.yMin)
	bbox.yMin = pt->y;
      else if (pt->y > bbox.yMax)
	bbox.yMax = pt->y;

      /* Scale the point */
      v1.x = pt->x >> 6;
      v1.y = pt->y >> 6;
      v1.x /= KLUDGE_FACTOR;
      v1.y /= KLUDGE_FACTOR;

      /* Compute the overall bounding box of the glyph */
      if (v1.x < vmin.x)
	vmin.x = v1.x;
      else if (v1.x > vmax.x)
	vmax.x = v1.x;

      if (v1.y < vmin.y)
	vmin.y = v1.y;
      else if (v1.y > vmax.y)
	vmax.y = v1.y;

      *vertex++ = v1;
      *index++ = j++;
    }
    *index++ = -1;

    /* Compute the orientation of the contour by looking at its normal */
    direction = Compute_Orientation(p->n_points, p->offset, &bbox);

    if (lastDirection)
      isPolygon = (direction == lastDirection || 
		   !(lastBBox.xMin < bbox.xMin && lastBBox.xMax > bbox.xMax &&
		     lastBBox.yMin < bbox.yMin && lastBBox.yMax > bbox.yMax));
    else
      isPolygon = TRUE;

    *flag++ = isPolygon;

    if (isPolygon) {
      lastDirection = direction;
      lastBBox = bbox;
    }
  }
  *index = -1;

  ch->size.x  = vmax.x - vmin.x;
  ch->size.y  = vmax.y - vmin.y;
  ch->advance = ch->size.x + 0.1;

  return SUCCESS;
}

#ifdef FT_DYNAMIC_BEZIER_STEPS
static TPos
Dynamic_Bezier_Threshold (RAS_ARG_ int degree, TPoint *arc)
{
  TPos    min_x,  max_x,  min_y, max_y, A, B;
  TPos    wide_x, wide_y, threshold;
  TPoint* cur   = arc;
  TPoint* limit = cur + degree;

  /* first of all, set the threshold to the maximum x or y extent */
  min_x = max_x = arc[0].x;
  min_y = max_y = arc[0].y;
  cur++;
  for ( ; cur < limit; cur++) {
    TPos  x = cur->x;
    TPos  y = cur->y;

    if (x < min_x) min_x = x;
    if (x > max_x) max_x = x;

    if (y < min_y) min_y = y;
    if (y > max_y) max_y = y;
  }
  wide_x = (max_x - min_x) << 4;
  wide_y = (max_y - min_y) << 4;

  threshold = wide_x;
  if (threshold < wide_y) threshold = wide_y;

  /* now compute the second and third order error values */

  wide_x = arc[0].x + arc[1].x - arc[2].x*2;
  wide_y = arc[0].y + arc[1].y - arc[2].y*2;

  if (wide_x < 0) wide_x = -wide_x;
  if (wide_y < 0) wide_y = -wide_y;

  A = wide_x; if (A < wide_y) A = wide_y;

  if (degree >= 3) {
    wide_x = arc[3].x - arc[0].x + 3*(arc[2].x - arc[3].x);
    wide_y = arc[3].y - arc[0].y + 3*(arc[2].y - arc[3].y);

    if (wide_x < 0) wide_x = -wide_x;
    if (wide_y < 0) wide_y = -wide_y;

    B = wide_x; if ( B < wide_y ) B = wide_y;
  }
  else
    B = 0;

  while (A > 0 || B > 0) {
    threshold >>= 1;
    A         >>= 2;
    B         >>= 3;
  }

  if (threshold < PRECISION_STEP)
    threshold = PRECISION_STEP;

  return threshold;
}
#endif

static double
Bounding_Triangle_Angle(TPos x1, TPos y1, 
			TPos x,  TPos y,
			TPos x2, TPos y2)
{
  FLpt2 a, b;
  GLfloat a_len, b_len, len2;

  a.x = x1 - x;
  a.y = y1 - y;
  b.x = x2 - x;
  b.y = y2 - y;
  a_len = (GLfloat) sqrt(a.x * a.x + a.y * a.y);
  b_len = (GLfloat) sqrt(b.x * b.x + b.y * b.y);
  len2 = a_len * b_len;
  return len2 == 0.0 ? 2.0 * PI : acos((a.x * b.x + a.y * b.y) / len2);
}

static TResult
Bezier_Up_Sign(RAS_ARG_ int degree,
	       TSplitter  splitter,
	       TPos       miny,
	       TPos       maxy,
	       TPos	  sign)
{
  TPos  x1, y1, x2, y2, x, y, e0, e2;
  double angle, lower_threshold, upper_threshold;

  TPoint*  arc;
  TPoint*  start_arc;

  TPoint * top;

  arc = ras.arc;
  y1  = arc[degree].y;
  y2  = arc[0].y;
  top = ras.cursor;

  if (y2 < miny || y1 > maxy)
    goto Fin;

#ifdef FT_DYNAMIC_BEZIER_STEPS
  /* compute dynamic bezier step threshold */
  /*threshold = Dynamic_Bezier_Threshold (RAS_VAR_ degree, arc);*/
  lower_threshold = LOWER_THRESHOLD;
  upper_threshold = UPPER_THRESHOLD;
#else
  lower_threshold = LOWER_THRESHOLD;
  upper_threshold = UPPER_THRESHOLD;
#endif

  start_arc = arc;

  /* loop while there is still an arc on the bezier stack */
  while (arc >= start_arc) {
    ras.joint = FALSE;

    x1 = arc[degree].x;	/* start of top-most arc */
    y1 = arc[degree].y;
    x2 = arc[0].x;	/* final of the top-most arc */
    y2 = arc[0].y;

    e2 = FLOOR (y2);	/* integer end y */
    if (e2 > maxy)
      e2 = FLOOR (maxy);
      
    e0 = CEILING (y1);
    if (e0 < miny)
      e0 = CEILING (miny);

    y  = (e0 + e2) / 2;

    if (e2 == e0) {
      ENTER_POINT(top, x2, sign * y2, "approx");
	
      arc -= degree;	/* pop the arc */
    }
    else {
      if (x2 < x1) {
	if (sign > 0)
	  x  = (x1 - FMulDiv (x2 - x1, y - e0, e2 - e0));
	else
	  x  = (x1 + FMulDiv (x1 - x2, y - e0, e2 - e0));
      }
      else {
	if (sign > 0)
	  x  = (x2 + FMulDiv (x2 - x1, y - e0, e2 - e0));
	else
	  x  = (x2 - FMulDiv (x1 - x2, y - e0, e2 - e0));
      }

      angle = Bounding_Triangle_Angle(x1, e0, x, y, x2, e2);

      if (abs(x2 - x) >= 64 && e2 - y >= 64 && angle < lower_threshold) {
	/* if the arc's bounding triangle's angle is too great, split it */
	splitter(arc);
	arc += degree;
      }
      
      else {
	/* otherwise, approximate it as a segment and compute
	   its intersection with the current scanline */
	if (((x2 < x && x < x1) || (x1 < x && x < x2)) &&
	    angle < upper_threshold) 
	  ENTER_POINT(top, x, sign * y, "approx");
	
	arc -= degree;     /* pop the arc */
      }
    }
  }

Fin:
  ras.cursor = top;
  ras.arc   -= degree;
  return SUCCESS;
}

static TResult
Bezier_Up(RAS_ARG_ int degree,
	  TSplitter  splitter,
	  TPos       miny,
	  TPos       maxy)
{
  return Bezier_Up_Sign(RAS_VAR_ degree, splitter, miny, maxy, 1);
}

/*
 * Computes the scan-line intersections of a descending second-order
 * Bezier arc and stores them in the render pool.  The arc is taken
 * from the top of the stack.
 */
static TResult
Bezier_Down(RAS_ARG_ int degree,
	    TSplitter  splitter,
	    TPos       miny,
	    TPos       maxy)
{
  TPoint*  arc = ras.arc;
  TResult  result, fresh;

  arc[0].y = -arc[0].y;
  arc[1].y = -arc[1].y;
  arc[2].y = -arc[2].y;
  if (degree > 2)
    arc[3].y = -arc[3].y;

  fresh = ras.fresh;

  result = Bezier_Up_Sign(RAS_VAR_ degree, splitter, -maxy, -miny, -1);

  arc[0].y = -arc[0].y;
  return result;
}

static TResult
Bezier_Right_Sign(RAS_ARG_ int degree,
		  TSplitter  splitter,
		  TPos       minx,
		  TPos       maxx,
		  TPos	     sign)
{
  TPos  x1, y1, x2, y2, x, y, e0, e2;
  double angle, lower_threshold, upper_threshold;

  TPoint*  arc;
  TPoint*  start_arc;

  TPoint * top;

  arc = ras.arc;
  x1  = arc[degree].x;
  x2  = arc[0].x;
  top = ras.cursor;

  if (x1 < minx || x2 > maxx)
    goto Fin;

#ifdef FT_DYNAMIC_BEZIER_STEPS
  /* compute dynamic bezier step threshold */
  /*threshold = Dynamic_Bezier_Threshold (RAS_VAR_ degree, arc);*/
  lower_threshold = LOWER_THRESHOLD;
  upper_threshold = UPPER_THRESHOLD;
#else
  lower_threshold = LOWER_THRESHOLD;
  upper_threshold = UPPER_THRESHOLD;
#endif

  start_arc = arc;

  /* loop while there is still an arc on the bezier stack */
  while (arc >= start_arc) {
    ras.joint = FALSE;

    x1 = arc[degree].x;	/* start of right-most arc */
    y1 = arc[degree].y;
    x2 = arc[0].x;	/* final of the right-most arc */
    y2 = arc[0].y;

    e2 = FLOOR (x2);	/* integer end x */
    if (e2 > maxx)
      e2 = FLOOR (maxx);
    
    e0 = CEILING (x1);
    if (e0 < minx)
      e0 = CEILING (minx);
      
    x  = (e0 + e2) / 2;

    if (e2 == e0) {
      ENTER_POINT(top, sign * x2, y2, "approx");
      
      arc -= degree;	/* pop the arc */
    }
    else {
      if (y2 < y1) {
	if (sign > 0)
	  y  = (y1 - FMulDiv (y2 - y1, x - e0, e2 - e0));
	else
	  y  = (y1 + FMulDiv (y1 - y2, x - e0, e2 - e0));
      }
      else {
	if (sign > 0)
	  y  = (y2 + FMulDiv (y2 - y1, x - e0, e2 - e0));
	else
	  y  = (y2 - FMulDiv (y1 - y2, x - e0, e2 - e0));
      }

      angle = Bounding_Triangle_Angle(e0, y1, x, y, e2, y2);
      
      if (e2 - x >= 64 && abs(y2 - y) >= 64 && angle < lower_threshold) {
	/* if the arc's bounding triangle's angle is too great, split it */
	splitter(arc);
	arc += degree;
      }
      
      else {
	/* otherwise, approximate it as a segment and compute
	   its intersection with the current scanline */
	if (((y2 < y && y < y1) || (y1 < y && y < y2)) &&
	    angle < upper_threshold) 
	  ENTER_POINT(top, sign * x, y, "approx");
	
	arc -= degree;     /* pop the arc */
      }
    }
  }

Fin:
  ras.cursor = top;
  ras.arc   -= degree;
  return SUCCESS;
}

static TResult
Bezier_Right(RAS_ARG_ int degree,
	     TSplitter  splitter,
	     TPos       minx,
	     TPos       maxx)
{
  return Bezier_Right_Sign(RAS_VAR_ degree, splitter, minx, maxx, 1);
}

/*
 * Computes the scan-line intersections of a descending second-order
 * Bezier arc and stores them in the render pool.  The arc is taken
 * from the top of the stack.
 */
static TResult
Bezier_Left(RAS_ARG_ int degree,
	    TSplitter  splitter,
	    TPos       minx,
	    TPos       maxx)
{
  TPoint*  arc = ras.arc;
  TResult  result, fresh;

  arc[0].x = -arc[0].x;
  arc[1].x = -arc[1].x;
  arc[2].x = -arc[2].x;
  if (degree > 2)
    arc[3].x = -arc[3].x;

  fresh = ras.fresh;

  result = Bezier_Right_Sign(RAS_VAR_ degree, splitter, -maxx, -minx, -1);

  arc[0].x = -arc[0].x;
  return result;
}

/*
 * Subdivides one second-order Bezier arc into two joint sub-arcs in
 * the Bezier stack.
 */
static void
Split_Conic (TPoint *base)
{
  TPos  a, b;

  base[4].x = base[2].x;
  b = base[1].x;
  a = base[3].x = (base[2].x + b + 1) >> 1;
  b = base[1].x = (base[0].x + b + 1) >> 1;
  base[2].x = (a + b + 1) >> 1;

  base[4].y = base[2].y;
  b = base[1].y;
  a = base[3].y = (base[2].y + b + 1) >> 1;
  b = base[1].y = (base[0].y + b + 1) >> 1;
  base[2].y = (a + b) / 2;
}

/*
 * Clears the Bezier stack and pushes a new arc on top of it.
 */
static void
Push_Conic (RAS_ARG_ FT_Vector *p2, FT_Vector *p3)
{
#undef  STORE
#define STORE(_arc, point) {					\
    _arc.x = SCALED (point->x);					\
    _arc.y = SCALED (point->y);					\
  }

  TPoint*  arc;

  ras.arc = arc = ras.arcs;

  arc[2] = ras.last;
  STORE (arc[1], p2);
  STORE (arc[0], p3);
#undef  STORE
}

/*
 * Subdivides a third-order Bezier arc into two joint sub-arcs in
 * the Bezier stack.
 */
static void
Split_Cubic (TPoint *base)
{
  TPos   a, b, c, d;

  base[6].x = base[3].x;
  c = base[1].x;
  d = base[2].x;
  base[1].x = a = (base[0].x + c + 1) >> 1;
  base[5].x = b = (base[3].x + d + 1) >> 1;
  c = (c + d + 1) >> 1;
  base[2].x = a = (a + c + 1) >> 1;
  base[4].x = b = (b + c + 1) >> 1;
  base[3].x = (a + b + 1) >> 1;

  base[6].y = base[3].y;
  c = base[1].y;
  d = base[2].y;
  base[1].y = a = (base[0].y + c + 1) >> 1;
  base[5].y = b = (base[3].y + d + 1) >> 1;
  c = (c + d + 1) >> 1;
  base[2].y = a = (a + c + 1) >> 1;
  base[4].y = b = (b + c + 1) >> 1;
  base[3].y = (a + b + 1) >> 1;
}

/*
 * Clears the Bezier stack and pushes a new third-order Bezier arc on
 * top of it.
 */
static void
Push_Cubic (RAS_ARG_ FT_Vector *p2, FT_Vector *p3, FT_Vector *p4)
{
#undef  STORE
#define STORE(_arc, point) {					\
    _arc.x = SCALED (point->x);					\
    _arc.y = SCALED (point->y);					\
  }

  TPoint*  arc;
  ras.arc = arc = ras.arcs;

  arc[3] = ras.last;
  STORE (arc[2], p2);
  STORE (arc[1], p3);
  STORE (arc[0], p4);

#undef STORE
}

static TResult
Check_Contour (RAS_ARG)
{
  TFace *  lastFace;

  /* Sometimes, the first and last face in a contour join on
     an integer scan-line; we must then remove the last intersection
     from the last face to get rid of doublets */
  if ((FRAC (ras.last.y) == 0 &&
       ras.last.y >= ras.minY &&
       ras.last.y <= ras.maxY))
  {
    if (ras.first_face && ras.first_face->flow == ras.cur_face->flow)
      ras.cursor--;
  }

  lastFace = ras.cur_face;
  if (End_Face (RAS_VAR))
    return FAILURE;

  /* close the `next face in contour' linked list */
  lastFace->next = ras.first_face;

  return SUCCESS;
}

/*
 * This function injects a new contour in the render pool.
 */
static int
Move_To (FT_Vector *to, FT_Raster raster)
{
  TPoint *top;

  /* if there was already a contour being built, perform some checks */
  if (ras.start_face)
    if (Check_Contour (RAS_VAR ))
      goto Fail;

  if (New_Face (RAS_VAR_ Ascending))
    goto Fail;

  /* set the `current last point' */
  ras.last.x = SCALED (to->x);
  ras.last.y = SCALED (to->y);

  ras.state      = Unknown;
  ras.first_face = NULL;

  top = ras.cursor;

  ENTER_POINT(top, ras.last.x, ras.last.y, "move");

  ras.cursor = top;

  return SUCCESS;

Fail:
  return FAILURE;
}

/*
 * This function injects a new line segment in the render pool and
 * adjusts the faces list accordingly.
 */
static int
Line_To (FT_Vector *to, FT_Raster raster)
{
  TPoint *top;
  TPos  x, y;

  x = SCALED(to->x);
  y = SCALED(to->y);

  ras.last.x = x;
  ras.last.y = y;

  top = ras.cursor;

  ENTER_POINT(top, ras.last.x, ras.last.y, "line");

  ras.cursor = top;

  return SUCCESS;
}

//
// Injects a new conic Bezier arc and adjusts the face list
// accordingly.
//
static int
Conic_To (FT_Vector *control,
	  FT_Vector *to,
	  FT_Raster  raster)
{
  TPoint *    top;
  TPos        y1, y2, y3, ymin, ymax;
  TPos	      x1, x2, x3, xmin, xmax;
  TDirection  state_bez;

  Push_Conic (RAS_VAR_  control, to);

  do {
    y1 = ras.arc[2].y;
    y2 = ras.arc[1].y;
    y3 = ras.arc[0].y;
    x1 = ras.arc[2].x;
    x2 = ras.arc[1].x;
    x3 = ras.arc[0].x;

    /* first, categorize the Bezier arc */

    if ( y1 <= y3 ) {
      ymin = y1;
      ymax = y3;
    }
    else {
      ymin = y3;
      ymax = y1;
    }

    if ( x1 <= x3 ) {
      xmin = x1;
      xmax = x3;
    }
    else {
      xmin = x3;
      xmax = x1;
    }

    if ((ymax - ymin) >= (xmax - xmin)) {
      if (y2 < ymin || y2 > ymax) {
	/* this arc has no given direction, split it !! */
	Split_Conic (ras.arc);
	ras.arc += 2;
      }
      else if (y1 == y3) {
	/* this arc is flat, ignore it and pop it from the bezier stack */
	ras.arc -= 2;
      }
      else {
	/* the arc is y-monotonous, either ascending or descending
	   detect a change of direction */
	state_bez = y1 < y3 ? Ascending : Descending;
	
	/* now call the appropriate routine */
	if (state_bez == Ascending) {
	  if (Bezier_Up (RAS_VAR_  2, Split_Conic, ras.minY, ras.maxY))
	    goto Fail;
	}
	else
	  if (Bezier_Down (RAS_VAR_  2, Split_Conic, ras.minY, ras.maxY))
	    goto Fail;
      }
    }

    else {
      if (x2 < xmin || x2 > xmax) {
	/* this arc has no given direction, split it !! */
	Split_Conic (ras.arc);
	ras.arc += 2;
      }
      else if (x1 == x3) {
	/* this arc is flat, ignore it and pop it from the bezier stack */
	ras.arc -= 2;
      }
      else {
	/* the arc is y-monotonous, either ascending or descending
	   detect a change of direction */
	state_bez = x1 < x3 ? Ascending : Descending;

	/* now call the appropriate routine */
	if (state_bez == Ascending) {
	  if (Bezier_Right (RAS_VAR_  2, Split_Conic, ras.minX, ras.maxX))
	    goto Fail;
	}
	else
	  if (Bezier_Left (RAS_VAR_  2, Split_Conic, ras.minX, ras.maxX))
	    goto Fail;
      }
    }

  } while (ras.arc >= ras.arcs);

  ras.last.x = x3;
  ras.last.y = y3;

  top = ras.cursor;

  ENTER_POINT_CHECK(top, ras.last.x, ras.last.y, "conic");

  ras.cursor = top;

  return SUCCESS;

Fail:
  return FAILURE;
}

/*
 * Injects a new cubic Bezier arc and adjusts the face list
 * accordingly.
 */
static int
Cubic_To (FT_Vector *control1,
	  FT_Vector *control2,
	  FT_Vector *to,
	  FT_Raster  raster)
{
  TPoint *    top;
  TPos        y1, y2, y3, y4, x4, ymin1, ymax1, ymin2, ymax2;
  TDirection  state_bez;

  Push_Cubic (RAS_VAR_  control1, control2, to);

  do {
    y1 = ras.arc[3].y;
    y2 = ras.arc[2].y;
    y3 = ras.arc[1].y;
    y4 = ras.arc[0].y;
    x4 = ras.arc[0].x;

    /* first, categorize the Bezier arc */

    if ( y1 <= y4 ) {
      ymin1 = y1;
      ymax1 = y4;
    }
    else {
      ymin1 = y4;
      ymax1 = y1;
    }

    if ( y2 <= y3 ) {
      ymin2 = y2;
      ymax2 = y3;
    }
    else {
      ymin2 = y3;
      ymax2 = y2;
    }

    if (ymin2 < ymin1 || ymax2 > ymax1) {
      // this arc has no given direction, split it!
      Split_Cubic (ras.arc);
      ras.arc += 3;
    }
    else if (y1 == y4) {
      // this arc is flat, ignore it and pop it from the bezier stack
      ras.arc -= 3;
    }
    else {
      state_bez = (y1 <= y4) ? Ascending : Descending;

      /* compute intersections */
      if (state_bez == Ascending) {
	if (Bezier_Up (RAS_VAR_ 3, Split_Cubic, ras.minY, ras.maxY))
	  goto Fail;
      }
      else
	if (Bezier_Down (RAS_VAR_ 3, Split_Cubic, ras.minY, ras.maxY))
	  goto Fail;
    }

  } while (ras.arc >= ras.arcs);

  ras.last.x = x4;
  ras.last.y = y4;

  top = ras.cursor;

  ENTER_POINT_CHECK(top, ras.last.x, ras.last.y, "cubic");

  ras.cursor = top;

  return SUCCESS;

Fail:
  return FAILURE;
}

/*
 * Converts a glyph into a series of segments and arcs and makes a
 * faces list with them.
 */
int
_flFTConvertGlyph(FLFreeTypeOutline *ch, RAS_ARG_ FT_Outline *outline)
{
  static FT_Outline_Funcs interface_table = {
    (FT_Outline_MoveTo_Func)Move_To,
    (FT_Outline_LineTo_Func)Line_To,
    (FT_Outline_ConicTo_Func)Conic_To,
    (FT_Outline_CubicTo_Func)Cubic_To
  };
  int error;

  /* Set up state in the raster object */
  ras.start_face = NULL;
  ras.joint      = FALSE;
  ras.fresh      = FALSE;

  ras.pool_limit = ras.pool_size - AlignFaceSize;

  ras.n_extrema = 0;

  ras.cur_face         = (TFace *)ras.cursor;
  ras.cur_face->offset = ras.cursor;
  ras.num_faces        = 0;

  /* Now decompose curve */
  error = FT_Outline_Decompose(outline, &interface_table, &ras);
  if (error)
    return FAILURE;

  /* XXX: the error condition is in ras.error */

  /* Check the last contour if needed */
  if (Check_Contour(RAS_VAR))
    return FAILURE;

  /* Finalize faces list */
  return _flFTFinalizeFaceTable(ch, RAS_VAR);
}

int
_flFTGenerateGlyph(FLFreeTypeOutline *ch)
{
  Byte pool[4096];
  FT_RasterRec_ rec;
  FT_Raster	raster = &rec;
  FT_Outline *	outline = &ch->glyph->outline;
  int		top, bottom, y_min, y_max;
  int		left, right, x_min, x_max;
  int		error;

  ras.cursor = ras.pool = (TPoint *) pool;
  ras.pool_size = (TPoint *) pool + sizeof(pool);

  /* return immediately if the outline is empty */
  if (outline->n_points == 0 || outline->n_contours <= 0)
    return ErrRaster_Ok;

  if (!outline || !outline->contours || !outline->points)
    return ErrRaster_Invalid_Outline;

  if (outline->n_points != outline->contours[outline->n_contours - 1] + 1)
    return ErrRaster_Invalid_Outline;

  ras.outline  = outline;

  SET_High_Precision((char)((outline->flags & ft_outline_high_precision)!= 0));

  ras.scale_shift  = PRECISION_BITS - INPUT_BITS;
  ras.scale_delta  = PRECISION_HALF;

  top      = BM_CEILING(ch->glyph->metrics.horiBearingY);
  bottom   = BM_FLOOR(ch->glyph->metrics.horiBearingY - ch->glyph->metrics.height);
  left     = BM_CEILING(ch->glyph->metrics.horiBearingX);
  right    = BM_FLOOR(ch->glyph->metrics.horiBearingX + ch->glyph->metrics.width);
  y_min    = 0;
  y_max    = BM_TRUNC(top - bottom);
  x_min    = 0;
  x_max    = BM_TRUNC(right - left);

  ras.maxY = ((long)y_max << PRECISION_BITS) - 1;
  ras.minY =  (long)y_min << PRECISION_BITS;
  ras.maxX = ((long)x_max << PRECISION_BITS) - 1;
  ras.minX =  (long)x_min << PRECISION_BITS;

  error = _flFTConvertGlyph(ch, RAS_VAR_  ras.outline);
  if (error)
    return error;

  return 0;
}
