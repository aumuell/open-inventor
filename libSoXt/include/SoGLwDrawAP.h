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

#ifndef _SoGLwDrawAP_h
#define _SoGLwDrawAP_h

#ifdef __GLX_MOTIF
#include "SoGLwMDrawA.h"
#else
#include "SoGLwDrawA.h"
#endif

typedef struct _SoGLwDrawingAreaClassPart
{
    XtPointer extension;
} SoGLwDrawingAreaClassPart;

#ifdef __GLX_MOTIF
typedef struct _SoGLwMDrawingAreaClassRec {
    CoreClassPart		core_class;
    XmPrimitiveClassPart	primitive_class;
    SoGLwDrawingAreaClassPart	SoglwDrawingArea_class;
} SoGLwMDrawingAreaClassRec;

extern SoGLwMDrawingAreaClassRec SoglwMDrawingAreaClassRec;

#else /* not __GLX_MOTIF */

typedef struct _SoGLwDrawingAreaClassRec {
    CoreClassPart		core_class;
    SoGLwDrawingAreaClassPart	SoglwDrawingArea_class;
} SoGLwDrawingAreaClassRec;

extern SoGLwDrawingAreaClassRec SoglwDrawingAreaClassRec;
#endif /* __GLX_MOTIF */

typedef struct {
    /* resources */
    XVisualInfo *	visualInfo;
    Boolean		installColormap;
    Boolean		allocateBackground;
    Boolean		allocateOtherColors;
    Boolean		installBackground;
    XtCallbackList	ginitCallback;
    XtCallbackList	resizeCallback;
    XtCallbackList	exposeCallback;
    XtCallbackList	inputCallback;
} SoGLwDrawingAreaPart;

#ifdef __GLX_MOTIF
typedef struct _SoGLwMDrawingAreaRec {
    CorePart		core;
    XmPrimitivePart	primitive;
    SoGLwDrawingAreaPart	SoglwDrawingArea;
} SoGLwMDrawingAreaRec;
#else /* not __GLX_MOTIF */
typedef struct _SoGLwDrawingAreaRec {
    CorePart		core;
    SoGLwDrawingAreaPart	SoglwDrawingArea;
} SoGLwDrawingAreaRec;
#endif /* __GLX_MOTIF */

#endif /* _SoGLwDrawP_h */
