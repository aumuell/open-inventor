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
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Description:
 |	This file defines the SoGLTextureImageElement class.
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_GL_TEXTURE_IMAGE_ELEMENT
#define  _SO_GL_TEXTURE_IMAGE_ELEMENT

class SoGLDisplayList;

#include <Inventor/elements/SoTextureImageElement.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoGLTextureImageElement
//
//  Element that stores the current texture image in GL. Overrides the
//  virtual methods on SoTextureImageElement to send the image
//  to GL when necessary.
//
//  Note that this class relies on SoTextureImageElement to store the
//  image in the instance.
//
//////////////////////////////////////////////////////////////////////////////

SoEXTENDER class SoGLTextureImageElement : public SoTextureImageElement {

    SO_ELEMENT_HEADER(SoGLTextureImageElement);
    
  public:
    // Initializes element
    virtual void	init(SoState *state);

    // Sets the current image, wrap modes and model.
    // This will try to build or use a GL
    // display list, if it can; the display list ID is returned (-1 if
    // there is none), and should be saved and passed in the next time
    // the element is set.  The node calling this routine is
    // responsible for calling glDeleteLists to free up the display
    // list at the right time.
    static SoGLDisplayList *
	set(SoState *state, SoNode *node,
	    const SbVec2s &s, int nc,
	    const unsigned char *bytes,
	    float texQuality,
	    int wrapS, int wrapT, int model, 
	    const SbColor &blendColor, SoGLDisplayList *list);

    // Override pop() method so side effects can occur in GL
    virtual void	pop(SoState *state, const SoElement *prevTopElement);

  SoINTERNAL public:
    // Initializes the SoGLTextureImageElement class
    static void		initClass();

  protected:
    // Catch setElt; if this setElt is called, it is an error...
    virtual void	setElt(const SbVec2s &s, int nc,
			       const unsigned char *bytes,
			       int wrapS, int wrapT, int model,
			       const SbColor &blendColor);

    virtual ~SoGLTextureImageElement();

  private:
    // Send texture environment information
    void		sendTexEnv(SoState *state);

    // Sends image in element to GL. The state is needed to determine
    // whether or not OpenGL extensions are available.  This will
    // build or use a texture object or display list, if possible.
    void		sendTex(SoState *state);

    SoGLDisplayList	*list;
    float		quality;
};

#endif /* _SO_GL_TEXTURE_IMAGE_ELEMENT */
