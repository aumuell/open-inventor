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
 |	This file defines the SoGLClipPlaneElement class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_GL_CLIP_PLANE_ELEMENT
#define  _SO_GL_CLIP_PLANE_ELEMENT

#include <Inventor/elements/SoClipPlaneElement.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoGLClipPlaneElement
//
//  Element that adds a clipping plane to the set currently in GL.
//  Overrides the virtual methods on SoClipPlaneElement to send the
//  clipping plane to GL.
//
//  Note that this class relies on SoClipPlaneElement to store the
//  plane in the instance.
//
//////////////////////////////////////////////////////////////////////////////

SoEXTENDER class SoGLClipPlaneElement : public SoClipPlaneElement {

    SO_ELEMENT_HEADER(SoGLClipPlaneElement);

  public:
    // Override pop() method so side effects can occur in GL
    virtual void	pop(SoState *state, const SoElement *prevTopElement);

    // Returns the maximum number of concurrent clipping planes
    // supported by GL implementation
    static int		getMaxGLPlanes();

  SoINTERNAL public:
    // Initializes the SoGLClipPlaneElement class
    static void		initClass();

  protected:
    // Adds the clipping plane to an instance. Has GL side effects.
    virtual void	addToElt(const SbPlane &plane,
				 const SbMatrix &modelMatrix);

    virtual ~SoGLClipPlaneElement();

  private:
    static int		maxGLPlanes;	// Holds max number of GL clip planes

    // Sends clipping plane in element to GL
    void		send();
};

#endif /* _SO_GL_CLIP_PLANE_ELEMENT */
