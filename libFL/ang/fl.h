#ifndef __fl_h_
#define __fl_h_

#ifndef __flclient_h_
#include "flclient.h"
#endif /* __flclient_h_ */

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

#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif

#define DEBUG 1

#define SAFE_STR(str) ((char*)(str)?(char*)(str):"nil")
#if DEBUG
#define TRACE(args) if (fl_debug) printf args
#else
#define TRACE(args)
#endif

#define BAD_FONT_NUMBER ((FLfontNumber)NULL)

typedef GLboolean FLInitializeFunc(
  void
);

typedef void FLShutdownFunc(
  void
);

typedef FLfontStruct *FLCreateFontFunc(
  const GLubyte *		/* fontName */,
  GLfloat [2][2]		/* mat */, 
  GLint				/* charNameCount */,
  GLubyte **			/* charNameVector */
);

typedef void FLDestroyFontFunc(
  FLfontStruct *		/* fs */
);

typedef FLbitmap *FLGetBitmapFunc(
  FLfontStruct *		/* fs */,
  GLuint			/* c */
);

typedef FLscalableBitmap *FLGetScalableBitmapFunc(
  FLfontStruct *		/* fs */, 
  GLuint			/* c */
);

typedef FLoutline *FLGetOutlineFunc(
  FLfontStruct *		/* fs */,
  GLuint			/* c */
);

typedef FLbitmap *FLUniGetBitmapFunc(
  FLfontStruct **		/* fsList */,
  GLubyte *			/* UCS2 */
);

typedef FLoutline *FLUniGetOutlineFunc(
  FLfontStruct **		/* fsList */,
  GLubyte *			/* UCS2 */
);



/* Function prototypes */
extern FLfontStruct *_flCreateFont(
  const GLubyte *		/* fontName */,
  GLfloat [2][2]		/* mat */, 
  GLint				/* charNameCount */,
  GLubyte **			/* charNameVector */
);

extern char *_flSearchFont(
  const GLubyte *		/* fontName */
);

extern void _flDestroyFont(
  FLfontStruct *		/* fs */
);

extern FLbitmap *_flGetBitmap(
  FLfontStruct *		/* fs */,
  GLuint                        /* c */
);

extern FLscalableBitmap *_flGetScalableBitmap(
  FLfontStruct *		/* fs */, 
  GLuint			/* c */
);

extern FLoutline *_flGetOutline(
  FLfontStruct *		/* fs */,
  GLuint			/* c */
);

extern FLbitmap *_flUniGetBitmap(
  FLfontStruct **		/* fsList */,
  GLubyte *			/* UCS2 */
);

extern FLoutline *_flUniGetOutline(
  FLfontStruct **		/* fsList */,
  GLubyte *			/* UCS2 */
);

#if DEBUG
extern int fl_debug;
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __fl_h_ */
