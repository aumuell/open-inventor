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
 |	This file defines the SoGLViewportRegionElement class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_GL_VIEWPORT_REGION_ELEMENT
#define  _SO_GL_VIEWPORT_REGION_ELEMENT

#include <Inventor/elements/SoViewportRegionElement.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoGLViewportRegionElement
//
//  Element that stores the current viewport region in GL. Overrides
//  the virtual methods on SoViewportRegionElement to send the region
//  to GL when necessary.
//
//  Note that this class relies on SoViewportRegionElement to store the
//  region in the instance.
//
//////////////////////////////////////////////////////////////////////////////

SoEXTENDER class SoGLViewportRegionElement : public SoViewportRegionElement {

    SO_ELEMENT_HEADER(SoGLViewportRegionElement);

  public:
    // Initializes element
    virtual void	init(SoState *state);

    // Override push() and pop() methods to keep GL up to date
    virtual void	push(SoState *state);
    virtual void	pop(SoState *state, const SoElement *prevTopElement);

  SoINTERNAL public:
    // Initializes the SoGLViewportRegionElement class
    static void		initClass();

  protected:
    // Sets the region in an instance. Has GL side effects.
    virtual void	setElt(const SbViewportRegion &vpReg);

    virtual ~SoGLViewportRegionElement();

  private:
    // This flag lets us know if the element is set to its initial
    // (usually bogus) value, which is not sent to GL. This is so we
    // can tell whether the GL value is up to date.
    SbBool		isDefault;

    // Sends region in element to GL
    void		send();
};

#endif /* _SO_GL_VIEWPORT_REGION_ELEMENT */
