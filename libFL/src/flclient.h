#ifndef __flclient_h_
#define __flclient_h_

#ifndef __gl_h_
#include <GL/gl.h>
#endif /* __gl_h_ */

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


/* This file should be included in those OpenGL programs which need Font  */
/* Library (FL) functions from the file /usr/lib/libFL.so.                */

/* The fontNamePreference parameter in a call to flCreateContext can have  */
/* one of the following values:                                            */
#define FL_FONTNAME     0
#define FL_FILENAME     1
#define FL_XFONTNAME    2

/* The parameter hint in a call to flSetHint can have one of the following */
/* values:                                                                 */
#define FL_HINT_AABITMAPFONTS   1      /* bound to font                    */
#define FL_HINT_CHARSPACING     2      /* bound to font                    */
#define FL_HINT_FONTTYPE        3      /* bound to font                    */
#define FL_HINT_MAXAASIZE       4      /* bound to font                    */
#define FL_HINT_MINOUTLINESIZE  5      /* bound to font                    */
#define FL_HINT_ROUNDADVANCE    6      /* bound to font                    */
#define FL_HINT_SCALETHRESH     7      /* bound to font                    */
#define FL_HINT_TOLERANCE       8      /* bound to font                    */

#define FL_FONTTYPE_ALL         0      /* use all types of fonts (default) */
#define FL_FONTTYPE_BITMAP      1      /* use only bitmap fonts            */
#define FL_FONTTYPE_OUTLINE     2      /* use only outline fonts           */

#define FL_ASCII           0
#define FL_ADOBE           1
#define FL_JISC6226        2
#define FL_CYRILLIC        3
#define FL_HANGUL          4
#define FL_DEVENAGERI      5
#define FL_ISO88591        6     /* ISO 8859-1 */
#define FL_DECTECH         7     /* DEC DECTECH */
#define FL_JISX020819760   8     /* JISX0208.1976-0 */
#define FL_JISX020119760   9     /* JISX0201.1976-0 */
#define FL_SUNOLCURSOR1   10     /* SUN OPEN LOOK CURSOR 1 */
#define FL_SUNOLGLYPH1    11     /* SUN OPEN LOOK GLYPH 1 */
#define FL_SPECIFIC       12     /* FONT SPECIFIC */
#define FL_JISX020819830  13     /* JISX0208.1983-0 */
#define FL_KSC560119870   14     /* KSC5601.1987-0 */
#define FL_GB231219800    15     /* GB2312.1980-0 */
#define FL_78EUCH         16     /* CID 78-EUC-H */
#define FL_78H            17     /* CID 78-H */
#define FL_78RKSJH        18     /* CID 78-RKSJ-H */
#define FL_83PVRKSJH      19     /* CID 83pv-RKSJ-H */
#define FL_90MSRKSJH      20     /* CID 90ms-RKSJ-H */
#define FL_90PVRKSJH      21     /* CID 90pv-RKSJ-H */
#define FL_ADDH           22     /* CID Add-H */
#define FL_ADDRKSJH       23     /* CID Add-RKSJ-H */
#define FL_ADOBEJAPAN10   24     /* CID Adobe-Japan1-0 */
#define FL_ADOBEJAPAN11   25     /* CID Adobe-Japan1-1 */
#define FL_ADOBEJAPAN12   26     /* CID Adobe-Japan1-2 */
#define FL_EUCH           27     /* CID EUC-H */
#define FL_EXTH           28     /* CID Ext-H */
#define FL_EXTRKSJH       29     /* CID Ext-RKSJ-H */
#define FL_HIRAGANA       30     /* CID Hiragana */
#define FL_KATAKANA       31     /* CID Katakana */
#define FL_NWPH           32     /* CID NWP-H */
#define FL_RKSJH          33     /* CID RKSJ-H */
#define FL_ROMAN          34     /* CID Roman */
#define FL_WPSYMBOL       35     /* CID WP-Symbol */
#define FL_ADOBEJAPAN20   36     /* CID Adobe-Japan2-0 */
#define FL_HOJOH          37     /* CID Hojo-H */
#define FL_ISO88592       38     /* ISO 8859-2 */
#define FL_ISO88593       39     /* ISO 8859-3 */
#define FL_ISO88594       40     /* ISO 8859-4 */
#define FL_ISO88595       41     /* ISO 8859-5 */
#define FL_ISO88596       42     /* ISO 8859-6 */
#define FL_ISO88597       43     /* ISO 8859-7 */
#define FL_ISO88598       44     /* ISO 8859-8 */
#define FL_ISO88599       45     /* ISO 8859-9 */
#define FL_ISO885910      46     /* ISO 8859-10 */
#define FL_BIG5           47
#define FL_CNS1164319861  48     /* CNS11643.1986-1 */
#define FL_CNS1164319862  49     /* CNS11643.1986-2 */
#define FL_EUCCN          50 
#define FL_EUCJP          51 
#define FL_EUCKR          52
#define FL_EUCTW          53
#define FL_SJIS           54

/* font direction */
#define FL_FONT_LEFTTORIGHT         0
#define FL_FONT_RIGHTTOLEFT         1
#define FL_FONT_BOTTOMTOTOP         2
#define FL_FONT_TOPTOBOTTOM         3

typedef GLint FLfontNumber;

/* character outline data structures */
typedef struct {
    GLfloat        x;
    GLfloat        y;
} FLpt2;

typedef struct FLoutline {
    GLshort        outlinecount;
    GLshort        *vertexcount;
    FLpt2          **vertex;
    GLfloat        xadvance;
    GLfloat        yadvance;
} FLoutline;

/* FLcontext is a pointer to private (opaque) data */
typedef struct __FLcontextRec *FLcontext;

typedef struct FLbitmap {
    GLsizei width;
    GLsizei height;
    GLfloat xorig;
    GLfloat yorig;
    GLfloat xmove;
    GLfloat ymove;
    GLubyte *bitmap;
} FLbitmap;

typedef struct FLscalableBitmap { /* font metrics in thousandths of an em */ 
    GLsizei ymove;  /* in thousandths of an em */
    GLsizei xmove;  /* in thousandths of an em */
    GLsizei llx;    /* in thousandths of an em */
    GLsizei lly;    /* in thousandths of an em */
    GLsizei urx;    /* in thousandths of an em */
    GLsizei ury;    /* in thousandths of an em */
    GLsizei width;  /* bitmap width in pixels */
    GLsizei height; /* bitmap height in pixels */
    GLfloat xorig;  /* x coordinate for the bitmap origin in pixels */ 
    GLfloat yorig;  /* y coordinate for the bitmap origin in pixels */ 
    GLubyte *bitmap;
} FLscalableBitmap;

typedef struct FLFontProp {
    unsigned long name;
    unsigned long value;
} FLFontProp;

typedef struct {
    short       lbearing;       /* origin to left edge of raster */
    short       rbearing;       /* origin to right edge of raster */
    short       width;          /* advance to next char's origin */
    short       ascent;         /* baseline to top edge of raster */
    short       descent;        /* baseline to bottom edge of raster */
    unsigned short attributes;  /* per char flags (not predefined) */
} FLCharStruct;

typedef struct FLfontStruct {
    FLfontNumber       fn;      /* font number (handle) */
    unsigned    direction;      /* hint about direction the font is painted */
    unsigned    min_char_or_byte2;/* first character */
    unsigned    max_char_or_byte2;/* last character */
    unsigned    min_byte1;      /* first row that exists */
    unsigned    max_byte1;      /* last row that exists */
    int         all_chars_exist;/* flag if all characters have non-zero size*/
    unsigned    default_char;   /* char to print for undefined character */
    int         n_properties;   /* how many properties there are */
    FLFontProp  *properties;    /* pointer to array of additional properties*/
    FLCharStruct min_bounds;    /* minimum bounds over all existing char*/
    FLCharStruct max_bounds;    /* maximum bounds over all existing char*/
    FLCharStruct *per_char;     /* first_char to last_char information */
    int         ascent;         /* log. extent above baseline for spacing */
    int         descent;        /* log. descent below baseline for spacing */
} FLfontStruct;


/* functions provided (exported) by the OpenGL Font Library (FL) */
extern void flAAColor(
    GLuint                         /* colorFull */, 
    GLuint                         /* colorHalf */
);

extern void flAACpack(
    GLuint                         /* colorFull */, 
    GLuint                         /* colorHalf */
);

extern void flCpack(
    GLuint                         /* color */
);

extern FLcontext flCreateContext(
    const GLubyte *                /* fontPath */,
    GLint                          /* fontNamePreference */,
    const GLubyte *                /* fontNameRestriction */,
    GLfloat                        /* pointsPerUMx */,
    GLfloat                        /* pointsPerUMy */
);

extern FLfontNumber flCreateFont(
    const GLubyte *                /* fontName */,
    GLfloat [2][2]                 /* mat */, 
    GLint                          /* charNameCount */,
    GLubyte **                     /* charNameVector */
);

extern void flDestroyContext(
    FLcontext                      /* ctx */
);

extern void flDestroyFont(
    FLfontNumber                   /* fn */
);

extern void flDrawCharacters(
    GLubyte *                      /* str */
);

extern void flDrawNCharacters(
    void *                         /* str */, 
    GLint                          /* charCount */,
    GLint                          /* bytesPerCharacter */
);

extern void flEnumerateFonts(
    void (*fn)(GLubyte *)          /* name of callback function */
);

extern void flEnumerateSizes(
    GLubyte *                      /* typeface */,
    void (*fn)(GLfloat)            /* name of callback function */
);

extern void flFreeBitmap(
    FLbitmap *                     /* bitmapPtr */
);

extern void flFreeScalableBitmap(
    FLscalableBitmap *             /* bitmapPtr */
);

extern void flFreeFontInfo(
    FLfontStruct *                 /* fontStruct */
);

extern void flFreeFontNames(
    GLubyte **                     /* list */
);

extern void flFreeFontSizes(
    GLfloat *                      /* list */
);

extern void flFreeOutline(
    FLoutline *                     /* outline */
);

extern GLboolean flGetOutlineBBox(
    FLfontNumber                   /* fn */,
    GLuint                         /* c */, 
    GLfloat *                      /* llx */, 
    GLfloat *                      /* lly */,
    GLfloat *                      /* urx */, 
    GLfloat *                      /* ury */
);

extern FLbitmap *flGetBitmap(
    FLfontNumber                   /* fn */, 
    GLuint                         /* c */
);

extern FLscalableBitmap *flGetScalableBitmap(
    FLfontNumber                   /* fn */,
    GLuint                         /* c */
);

extern FLcontext flGetCurrentContext(
    void
);

extern GLboolean flMakeCurrentContext(
    FLcontext                      /* ctx */
);

extern FLfontNumber flGetCurrentFont(
    void
);

extern FLfontStruct *flGetFontInfo(
    FLfontNumber                   /* fn */
);

extern FLoutline *flGetOutline(
    FLfontNumber                   /* fn */, 
    GLuint                         /* c */
);

extern GLboolean  flGetStringWidth(
    FLfontNumber                   /* fn */,
    GLubyte *                      /* str */, 
    GLfloat *                      /* dx  */, 
    GLfloat *                      /* dy */
);

extern GLboolean flGetStringWidthN(
    FLfontNumber                   /* fn */,
    void *                         /* str */, 
    GLint                          /* charCount */,
    GLint                          /* bytesPerCharacter */, 
    GLfloat *                      /* dx */, 
    GLfloat *                      /* dy */
);

extern GLubyte **flListFonts(
    GLint                          /* maxNames */, 
    GLint *                        /* countReturn */
);

extern GLfloat *flListSizes(
    const GLubyte *                /* typeface */, 
    GLint *countReturn
);

extern GLboolean flMakeCurrentContext(
    FLcontext                      /* ctx */
);

extern GLboolean flMakeCurrentFont(
    FLfontNumber                   /* fn */
);

extern FLfontNumber flScaleRotateFont(
    const GLubyte *                /* fontName */,
    GLfloat                        /* fontScale */, 
    GLfloat                        /* angle */
);

extern void flSetHint(
    GLuint                         /* hint */, 
    GLfloat                        /* hintValue */
);

extern void flGetFontBBox(
    FLfontNumber                   /* fn */,
    GLfloat *                      /* llx */,
    GLfloat *                      /* lly */,
    GLfloat *                      /* urx */,
    GLfloat *                      /* ury */,
    GLint *                        /* isfixed */
);

extern FLbitmap *flUniGetBitmap(
    GLubyte *                      /* fontList */, /* "fn1,fn2,fn3, ..." */
    GLubyte *                      /* UCS2 */
);

extern FLoutline *flUniGetOutline(
    GLubyte *                      /* fontList */, /* "fn1,fn2,fn3, ..." */
    GLubyte *                      /* UCS2 */
);

extern GLboolean flUniGetOutlineBBox(
    GLubyte *                      /* fontList */, /* "fn1,fn2,fn3, ..." */
    GLubyte *                      /* UCS2 */,
    GLfloat *                      /* llx */,
    GLfloat *                      /* lly */,
    GLfloat *                      /* urx */,
    GLfloat *                      /* ury */
);

extern FLscalableBitmap *flUniGetScalableBitmap(
    GLubyte *                      /* fontList */, /* "fn1,fn2,fn3, ..." */
    GLubyte *                      /* UCS2 */
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __flclient_h_ */
