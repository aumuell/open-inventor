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
 |   $Revision: 1.1.1.1 $
 |
 |   Description:
 |	Upgrade from 2.0 VertexProperty unknown node format to the 2.1 known format
 |
 |   Author(s)		: David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_V2VERTEX_PROPERTY_
#define  _SO_V2VERTEX_PROPERTY_

#include <Inventor/fields/SoMFUInt32.h>
#include <Inventor/fields/SoMFVec3f.h>
#include <Inventor/fields/SoMFVec2f.h>
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/misc/upgraders/SoUpgrader.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoV2VertexProperty
//
//  This is EXACTLY the same format as the 2.1 VertexProperty, but it looks for
//  the alternateRep field (which does not exist in 2.1 for VertexProperty).
//
//////////////////////////////////////////////////////////////////////////////

class SoV2VertexProperty : public SoUpgrader {

    SO_NODE_HEADER(SoV2VertexProperty);

  public:
    enum Binding {
	OVERALL =		SoMaterialBinding::OVERALL,
	PER_PART =		SoMaterialBinding::PER_PART,
	PER_PART_INDEXED =	SoMaterialBinding::PER_PART_INDEXED,
	PER_FACE =		SoMaterialBinding::PER_FACE,
	PER_FACE_INDEXED =	SoMaterialBinding::PER_FACE_INDEXED,
	PER_VERTEX =		SoMaterialBinding::PER_VERTEX,
	PER_VERTEX_INDEXED =	SoMaterialBinding::PER_VERTEX_INDEXED
    };
    
    // Fields:
    SoMFVec3f		vertex;		// Coordinate point(s)
    SoMFVec2f    	texCoord;	// Texture coordinate(s)

    SoMFVec3f		normal;		// Normal vector(s)
    SoSFEnum		normalBinding;  // Ignored unless normal field
					// set

    SoMFUInt32		orderedRGBA;	// Diffuse+transparency
    SoSFEnum		materialBinding;// Ignored unless orderedRGBA
					// field set

    SoSFNode		alternateRep;	// This is what's different from the 2.1 version!

    // Constructor
    SoV2VertexProperty();

  SoINTERNAL public:
    static void		initClass();

    virtual SoNode	*createNewNode();
    
    //virtual upgrade method used to discard extra characters in binary format:
    virtual SbBool upgrade(SoInput *in, const SbName &refName, SoBase *&result);

  protected:
    virtual ~SoV2VertexProperty();
};

#endif /* _SO_V2VERTEX_PROPERTY_ */
