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

//  -*- C++ -*-

/*
 * Copyright (C) 1990,91,92,93   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Description:
 |	This file contains the definition of the SoOffscreenRenderer class.
 |
 |   Classes:
 |	SoOffscreenRenderer
 |
 |   Author(s)		: Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_OFFSCREEN_RENDERER_
#define  _SO_OFFSCREEN_RENDERER_

#include <stdio.h>
#include <GL/glx.h>
#include <Inventor/SbColor.h>
#include <Inventor/SbViewportRegion.h>
#include <Inventor/SbLinear.h>
#include <X11/Xlib.h>

class SoNode;
class SoPath;
class SoGLRenderAction;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoOffscreenRenderer
//
//  This file contains the definition of the SoOffscreenRenderer class.
//  This class is used for rendering a scene graph to an offscreen memory
//  buffer which can be used for printing or generating textures.
//
//  The implementation of this class uses the X Pixmap for rendering.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoOffRnd
class SoOffscreenRenderer {
 public:

    // Constructor
    SoOffscreenRenderer( const SbViewportRegion &viewportRegion );
    // C-api: name=CreateAct
    SoOffscreenRenderer( SoGLRenderAction *ra );

    // Destructor
    ~SoOffscreenRenderer();

    enum Components {
        LUMINANCE = 1,
        LUMINANCE_TRANSPARENCY = 2,
        RGB = 3,                      // The default
        RGB_TRANSPARENCY = 4
    };

    // C-api: name=getScrPixPerInch
    static float	getScreenPixelsPerInch();

    // Set/get the components to be rendered
    // C-api: name=setComp
    void		setComponents( Components components )
				{comps = components;}
    // C-api: name=getComp
    Components		getComponents() const
				{return comps;}

    // Set/get the viewport region
    // C-api: name=setVP
    void		setViewportRegion( const SbViewportRegion &region );
    // C-api: name=getVP
    const SbViewportRegion  &getViewportRegion() const;

    // Get the maximum supported resolution of the viewport.
    // C-api: name=getMaxRes
    static SbVec2s	getMaximumResolution();

    // Set/get the background color
    // C-api: name=setBkgCol
    void		setBackgroundColor( const SbColor &c )
				{backgroundColor = c;}
    // C-api: name=getBkgCol
    const SbColor & getBackgroundColor() const
				{return backgroundColor;}

    // Set and get the render action to use
    // C-api: name=setAct
    void                      setGLRenderAction(SoGLRenderAction *ra);
    // C-api: name=getAct
    SoGLRenderAction *        getGLRenderAction() const;

    // Render the given scene into a buffer
    // C-api: name=rnd
    SbBool		render( SoNode *scene );
    // C-api: name=rndPath
    SbBool		render( SoPath *scene );

    // Return the buffer containing the rendering
    // C-api: name=getBuff
    unsigned char *     getBuffer() const;

    // Write the buffer as a .rgb file into the given FILE
    // C-api: name=toRGB
    SbBool		writeToRGB( FILE *fp ) const;

    // Write the buffer into encapsulated PostScript.  If a print size is
    // not given, adjust the size of the print so it is WYSIWYG with respect
    // to the viewport region on the current device.
    // C-api: name=toPS
    SbBool		writeToPostScript( FILE *fp ) const;
    // C-api: name=toPSSize
    SbBool		writeToPostScript( FILE *fp,
                                const SbVec2f &printSize ) const;

  private:
    unsigned char *	pixelBuffer;
    Components          comps;
    SbColor		backgroundColor;
    SoGLRenderAction	*userAction, *offAction;
    SbViewportRegion    renderedViewport;

    // These are used for rendering to the offscreen pixmap
    Display 		*display;
    XVisualInfo  	*visual;
    GLXContext 		context;
    GLXPixmap 		pixmap;
    Pixmap 		pmap;

    // Setup the offscreen pixmap
    SbBool		setupPixmap();

    // Initialize an offscreen pixmap
    static SbBool 	initPixmap( Display * &dpy, XVisualInfo * &vi,
                                GLXContext &cx, const SbVec2s &sz,
                                GLXPixmap &glxPmap, Pixmap &xpmap );

    // Read pixels back from the Pixmap
    void		readPixels();

    // Set the graphics context
    SbBool		setContext() const;

    // Return the format used in the rendering
    void		getFormat( GLenum &format ) const;

    static void		putHex( FILE *fp, char val, int &hexPos );
};

#endif /* _SO_OFFSCREEN_RENDERER_ */

