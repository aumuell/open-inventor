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
 * Copyright (C) 1995-96   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.2 $
 |
 |   Description:
 |	This file defines the SoWindowElement class.
 |
 |   Author(s)		: Alain Dumesny
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_WINDOW_ELEMENT
#define  _SO_WINDOW_ELEMENT

#include <Inventor/elements/SoSubElement.h>
#include <GL/glx.h>

class SoGLRenderAction;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoWindowElement
//
//  Element that stores the current window attributes. The current window
//  is defined as the OpenGL window underneath the cursor - or NULL if the
//  cursor is not over an OpenGL window.
//
//////////////////////////////////////////////////////////////////////////////

SoEXTENDER class SoWindowElement : public SoElement {

    SO_ELEMENT_HEADER(SoWindowElement);

  public:
    // Initializes element
    virtual void	init(SoState *state);

    // Always returns TRUE, since this element should never be a
    // criterion for cache invalidation.
    virtual SbBool	matches(const SoElement *elt) const;

    // Create and return a copy of this element
    virtual SoElement	*copyMatchInfo() const;

    // Sets the window, context and glRenderAction info
    static void		set(SoState *state, Window window, GLXContext context, 
			    Display *display, SoGLRenderAction *glAction);

    // Returns the current window, context and glRenderAction
    static void	get(SoState *state, Window &window, GLXContext &context, 
			    Display *&display, SoGLRenderAction *&glAction);
    
    virtual void    push(SoState *state);
    
  SoINTERNAL public:
    // Initializes the SoWindowElement class
    static void		initClass();

  protected:
    Window		window;
    GLXContext		context;
    Display		*display;
    SoGLRenderAction	*glRenderAction;
    
    virtual	~SoWindowElement();
};

#endif /* _SO_WINDOW_ELEMENT */
