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
 |	This file defines the SoMaterialBinding node class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_MATERIAL_BINDING_
#define  _SO_MATERIAL_BINDING_

#include <Inventor/elements/SoMaterialBindingElement.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/nodes/SoSubNode.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoMaterialBinding
//
//  Material binding node. This indicates how multiple materials are
//  to be distributed over an object.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoMtlBind
// C-api: public=value
class SoMaterialBinding : public SoNode {

    SO_NODE_HEADER(SoMaterialBinding);

  public:
    enum Binding {
	// Whole object has same material
	OVERALL		  = SoMaterialBindingElement::OVERALL,
	// One material for each part of object
	PER_PART	  = SoMaterialBindingElement::PER_PART,
	// One material for each part of object, indexed
	PER_PART_INDEXED  = SoMaterialBindingElement::PER_PART_INDEXED,
	// One material for each face of object
	PER_FACE	  = SoMaterialBindingElement::PER_FACE,
	// One material for each face, indexed
	PER_FACE_INDEXED  = SoMaterialBindingElement::PER_FACE_INDEXED,
	// One material for each vertex of object
	PER_VERTEX	  = SoMaterialBindingElement::PER_VERTEX,
	// One material for each vertex, indexed
	PER_VERTEX_INDEXED= SoMaterialBindingElement::PER_VERTEX_INDEXED,

// C-api: end
#ifndef IV_STRICT
// C-api: begin
	// Obsolete bindings:
	DEFAULT		  = OVERALL,
	NONE		  = OVERALL
// C-api: end
#endif
// C-api: begin
    };

    // Fields
    SoSFEnum		value;		// Material binding value

    // Constructor
    SoMaterialBinding();

  SoEXTENDER public:
    virtual void	doAction(SoAction *action);
    virtual void	GLRender(SoGLRenderAction *action);
    virtual void	callback(SoCallbackAction *action);
    virtual void	pick(SoPickAction *action);

  SoINTERNAL public:
    static void		initClass();

  protected:
    virtual ~SoMaterialBinding();

    // Reads stuff into instance. Upgrades old bindings.
    virtual SbBool	readInstance(SoInput *in, unsigned short flags);
};

#endif /* _SO_MATERIAL_BINDING_ */
