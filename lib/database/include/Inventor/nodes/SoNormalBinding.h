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
 |	This file defines the SoNormalBinding node class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_NORMAL_BINDING_
#define  _SO_NORMAL_BINDING_

#include <Inventor/elements/SoNormalBindingElement.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/nodes/SoSubNode.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoNormalBinding
//
//  Normal binding node. This indicates how multiple surface normals are
//  to be distributed over an object.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoNormBind
// C-api: public=value
class SoNormalBinding : public SoNode {

    SO_NODE_HEADER(SoNormalBinding);

  public:
    enum Binding {
	// Whole object has same normal
	OVERALL		  = SoNormalBindingElement::OVERALL,
	// One normal for each part of object
	PER_PART	  = SoNormalBindingElement::PER_PART,
	// One normal for each part of object, indexed
	PER_PART_INDEXED  = SoNormalBindingElement::PER_PART_INDEXED,
	// One normal for each face of object
	PER_FACE	  = SoNormalBindingElement::PER_FACE,
	// One normal for each face, indexed
	PER_FACE_INDEXED  = SoNormalBindingElement::PER_FACE_INDEXED,
	// One normal for each vertex of object
	PER_VERTEX	  = SoNormalBindingElement::PER_VERTEX,
	// One normal for each vertex, indexed
	PER_VERTEX_INDEXED= SoNormalBindingElement::PER_VERTEX_INDEXED,

// C-api: end
#ifndef IV_STRICT
// C-api: begin
	// Obsolete bindings:
	DEFAULT		  = PER_VERTEX_INDEXED,
	NONE		  = PER_VERTEX_INDEXED
// C-api: end
#endif
// C-api: begin
    };

    // Fields
    SoSFEnum		value;		// Normal binding value

    // Constructor
    SoNormalBinding();

  SoEXTENDER public:
    virtual void	doAction(SoAction *action);
    virtual void	GLRender(SoGLRenderAction *action);
    virtual void	callback(SoCallbackAction *action);
    virtual void	pick(SoPickAction *action);

  SoINTERNAL public:
    static void		initClass();

  protected:
    virtual ~SoNormalBinding();

    // Reads stuff into instance. Upgrades old bindings.
    virtual SbBool	readInstance(SoInput *in, unsigned short flags);
};

#endif /* _SO_NORMAL_BINDING_ */
