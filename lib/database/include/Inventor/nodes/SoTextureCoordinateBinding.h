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
 |   $Revision $
 |
 |   Description:
 |	This file defines the SoTextureCoordinateBinding node class.
 |
 |   Author(s)		: John Rohlf
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_TEXTURE_COORDINATE_BINDING_
#define  _SO_TEXTURE_COORDINATE_BINDING_

#include <Inventor/elements/SoTextureCoordinateBindingElement.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/nodes/SoSubNode.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoTextureCoordinateBinding
//
//  Texture coordinate binding node. This indicates how multiple texture 
//  coordinates are to be distributed over an object.
//
//  If there is a texture function defined, then neither a texture coordinate
//  nor a texture coordinate binding node is needed
//
//  Unlike normal bindings, only a few of the typical bindings make sense:
//  PER_VERTEX, and PER_VERTTEX_INDEXED
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoTexCoordBind
// C-api: public=value
class SoTextureCoordinateBinding : public SoNode {

    SO_NODE_HEADER(SoTextureCoordinateBinding);

  public:
    enum Binding {				// Binding value
	PER_VERTEX	   = SoTextureCoordinateBindingElement::PER_VERTEX,
	PER_VERTEX_INDEXED =
	    SoTextureCoordinateBindingElement::PER_VERTEX_INDEXED,

// C-api: end
#ifndef IV_STRICT
// C-api: begin
	// Obsolete bindings:
	DEFAULT		  = PER_VERTEX_INDEXED
// C-api: end
#endif
// C-api: begin
    };

    // Fields
    SoSFEnum		value;		// Normal binding value

    // Constructor
    SoTextureCoordinateBinding();

  SoEXTENDER public:
    virtual void	doAction(SoAction *action);
    virtual void	callback(SoCallbackAction *action);
    virtual void	GLRender(SoGLRenderAction *action);
    virtual void	pick(SoPickAction *action);

  SoINTERNAL public:
    static void		initClass();

  protected:
    virtual ~SoTextureCoordinateBinding();

    // Reads stuff into instance. Upgrades old bindings.
    virtual SbBool	readInstance(SoInput *in, unsigned short flags);
};

#endif /* _SO_TEXTURE_COORDINATE_BINDING_ */
