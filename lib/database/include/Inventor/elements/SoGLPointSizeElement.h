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
 |	This file defines the SoGLPointSizeElement class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_GL_POINT_SIZE_ELEMENT
#define  _SO_GL_POINT_SIZE_ELEMENT

#include <Inventor/elements/SoPointSizeElement.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoGLPointSizeElement
//
//  Element that stores the current point size in GL. Overrides the
//  virtual methods on SoPointSizeElement to send the point size to GL
//  when necessary.
//
//  Note that this class relies on SoPointSizeElement to store the
//  size in the instance.
//
//////////////////////////////////////////////////////////////////////////////

SoEXTENDER class SoGLPointSizeElement : public SoPointSizeElement {

    SO_ELEMENT_HEADER(SoGLPointSizeElement);

  public:
    // Initializes element
    virtual void	init(SoState *state);

    // Override push() method to copy state pointer and value of
    // previous element, to avoid sending GL commands if the elements
    // are the same.
    virtual void	push(SoState *state);

    // Override pop() method so side effects can occur in GL
    virtual void	pop(SoState *state, const SoElement *prevTopElement);

  SoINTERNAL public:
    // Initializes the SoGLPointSizeElement class
    static void		initClass();

  protected:
    // Sets the point size in an instance. Has GL side effects.
    virtual void	setElt(float size);

    virtual ~SoGLPointSizeElement();

  private:
    // This variable is TRUE if the value in this element was copied
    // from our parent.  If set is called with the same value, no GL
    // commands have to be done-- it is as if this element doesn't
    // exist, and the parent's value is used.  Of course, a cache
    // dependency will have to be added in this case (if the parent
    // changes, the cache is not valid).  Set sets this flag to false.
    SbBool		copiedFromParent;

    // We need to store the state so we can get the viewport region element
    SoState		*state;

    // Sends point size in element to GL
    void		send();
};

#endif /* _SO_GL_POINT_SIZE_ELEMENT */
