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

#ifndef _SoGLwDrawA_h
#define _SoGLwDrawA_h

#include <GL/glx.h>
#include <GL/gl.h>

/****************************************************************
 *
 * SoGLwDrawingArea widgets
 *
 ****************************************************************/

/* Resources:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 attribList	     AttribList		int *		NULL
 visualInfo	     VisualInfo		VisualInfo	NULL
 installColormap     InstallColormap	Boolean		TRUE
 allocateBackground  AllocateColors	Boolean		FALSE
 allocateOtherColors AllocateColors	Boolean		FALSE
 installBackground   InstallBackground	Boolean		TRUE
 exposeCallback      Callback		Pointer		NULL
 ginitCallback       Callback		Pointer		NULL
 inputCallback       Callback		Pointer		NULL
 resizeCallback      Callback		Pointer		NULL

*** The following resources all correspond to the GLX configuration
*** attributes and are used to create the attribList if it is NULL
 bufferSize	     BufferSize		int		0
 level		     Level		int		0
 rgba		     Rgba		Boolean		FALSE
 doublebuffer	     Doublebuffer	Boolean		FALSE
 stereo		     Stereo		Boolean		FALSE
 auxBuffers	     AuxBuffers		int		0
 redSize	     ColorSize		int		0
 greenSize	     ColorSize		int		0
 blueSize	     ColorSize		int		0
 alphaSize	     AlphaSize		int		0
 depthSize	     DepthSize		int		0
 stencilSize	     StencilSize	int		0
 accumRedSize	     AccumColorSize	int		0
 accumGreenSize	     AccumColorSize	int		0
 accumBlueSize	     AccumColorSize	int		0
 accumAlphaSize	     AccumAlphaSize	int		0
*/

#define SoGLwNattribList		"attribList"
#define SoGLwCAttribList		"AttribList"
#define SoGLwNvisualInfo		"visualInfo"
#define SoGLwCVisualInfo		"VisualInfo"
#define SoGLwRVisualInfo		"VisualInfo"

#define SoGLwNinstallColormap	"installColormap"
#define SoGLwCInstallColormap	"InstallColormap"
#define SoGLwNallocateBackground	"allocateBackground"
#define SoGLwNallocateOtherColors	"allocateOtherColors"
#define SoGLwCAllocateColors	"AllocateColors"
#define SoGLwNinstallBackground	"installBackground"
#define SoGLwCInstallBackground	"InstallBackground"

#define SoGLwCCallback		"Callback"
#define SoGLwNexposeCallback	"exposeCallback"
#define SoGLwNginitCallback	"ginitCallback"
#define SoGLwNresizeCallback	"resizeCallback"
#define SoGLwNinputCallback	"inputCallback"

#define SoGLwNbufferSize		"bufferSize"
#define SoGLwCBufferSize		"BufferSize"
#define SoGLwNlevel		"level"
#define SoGLwCLevel		"Level"
#define SoGLwNrgba		"rgba"
#define SoGLwCRgba		"Rgba"
#define SoGLwNdoublebuffer	"doublebuffer"
#define SoGLwCDoublebuffer	"Doublebuffer"
#define SoGLwNstereo		"stereo"
#define SoGLwCStereo		"Stereo"
#define SoGLwNauxBuffers		"auxBuffers"
#define SoGLwCAuxBuffers		"AuxBuffers"
#define SoGLwNredSize		"redSize"
#define SoGLwNgreenSize		"greenSize"
#define SoGLwNblueSize		"blueSize"
#define SoGLwCColorSize		"ColorSize"
#define SoGLwNalphaSize		"alphaSize"
#define SoGLwCAlphaSize		"AlphaSize"
#define SoGLwNdepthSize		"depthSize"
#define SoGLwCDepthSize		"DepthSize"
#define SoGLwNstencilSize		"stencilSize"
#define SoGLwCStencilSize		"StencilSize"
#define SoGLwNaccumRedSize	"accumRedSize"
#define SoGLwNaccumGreenSize	"accumGreenSize"
#define SoGLwNaccumBlueSize	"accumBlueSize"
#define SoGLwCAccumColorSize	"AccumColorSize"
#define SoGLwNaccumAlphaSize	"accumAlphaSize"
#define SoGLwCAccumAlphaSize	"AccumAlphaSize"

#ifdef __GLX_MOTIF
typedef struct _SoGLwMDrawingAreaClassRec	*SoGLwMDrawingAreaWidgetClass;
typedef struct _SoGLwMDrawingAreaRec	*SoGLwMDrawingAreaWidget;

extern WidgetClass SoglwMDrawingAreaWidgetClass;
#else /* not __GLX_MOTIF */
typedef struct _SoGLwDrawingAreaClassRec	*SoGLwDrawingAreaWidgetClass;
typedef struct _SoGLwDrawingAreaRec	*SoGLwDrawingAreaWidget;

extern WidgetClass SoglwDrawingAreaWidgetClass;
#endif

/* Callback reasons */
#ifdef __GLX_MOTIF
#define SoGLwCR_EXPOSE	XmCR_EXPOSE
#define SoGLwCR_RESIZE	XmCR_RESIZE
#define SoGLwCR_INPUT	XmCR_INPUT
#else /* not __GLX_MOTIF */
/* The same values as Motif, but don't use Motif constants */
#define SoGLwCR_EXPOSE	38
#define SoGLwCR_RESIZE	39
#define SoGLwCR_INPUT	40
#endif /* __GLX_MOTIF */

#define SoGLwCR_GINIT	32136	/* Arbitrary number that should neverr clash*/

typedef struct
{
    int     reason;
    XEvent  *event;
    Dimension width, height;		/* for resize callback */
} SoGLwDrawingAreaCallbackStruct;

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/* front ends to glXMakeCurrent and glXSwapBuffers */
extern void SoGLwDrawingAreaMakeCurrent (Widget w, GLXContext ctx);
extern void SoGLwDrawingAreaSwapBuffers (Widget w);

#ifdef __GLX_MOTIF
#ifdef _NO_PROTO
extern Widget SoGLwCreateMDrawingArea ();
#else /* _NO_PROTO */

extern Widget SoGLwCreateMDrawingArea (Widget parent, char *name,
				     ArgList arglist, Cardinal argcount);
#endif /* _NO_PROTO */

#endif /* __GLX_MOTIF */

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif /* _SoGLwDrawA_h */
