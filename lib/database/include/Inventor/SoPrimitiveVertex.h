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
 |   $Revision: 1.1.1.1 $
 |
 |   Description:
 |	This file defines the base SoPrimitiveVertex class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef _SO_PRIMITIVE_VERTEX_
#define _SO_PRIMITIVE_VERTEX_

#include <Inventor/SbLinear.h>

class SoDetail;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoPrimitiveVertex
//
//  An SoPrimitiveVertex represents a vertex of a primitive (triangle,
//  line segment, point) that is being generated via the
//  SoCallbackAction.
//
//  An SoPrimitiveVertex contains an object-space point, normal,
//  texture coordinates, material index, and a pointer to an instance
//  of some SoDetail subclass. This detail may contain more
//  information about the vertex, or may be a NULL pointer if there is
//  no such info.
//
//  Instances of SoPrimitiveVertex are typically created on the stack
//  by shape classes while they are generating primitives. Anyone who
//  wants to save them as return values from SoCallbackAction should
//  probably make copies of them.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoPrimVtx
class SoPrimitiveVertex {

  public:
    // Constructors
    SoPrimitiveVertex();
    // C-api: name=CreateCopy
    SoPrimitiveVertex(const SoPrimitiveVertex &pv);

    // Destructor
    ~SoPrimitiveVertex();

    // These return the surface point, normal, and texture coordinates
    // in object space:
    // C-api: name=getPt
    const SbVec3f &	getPoint() const		{ return point; }
    // C-api: name=getNorm
    const SbVec3f &	getNormal() const		{ return normal; }
    // C-api: name=getTexCoords
    const SbVec4f &	getTextureCoords() const	{ return texCoords; }

    // Returns the index into the current set of materials of the
    // material active at the vertex
    // C-api: name=getMtlInd
    int			getMaterialIndex() const    { return materialIndex; }

    // Returns the detail giving more info about the vertex. Note that
    // this pointer may be NULL.
    // C-api: name=getDtl
    const SoDetail *	getDetail() const		{ return detail; }

    // Copy assignment operator. Note that just the pointer to the
    // detail is copied, and not the detail itself.
    SoPrimitiveVertex &	operator =(const SoPrimitiveVertex &pv);

  SoEXTENDER public:

    // These methods are typically called by shape classes during
    // primtiive generation

    // These set the object space point, normal, and texture coordinates:
    void		setPoint(const SbVec3f &pt)	  { point     = pt; }
    void		setNormal(const SbVec3f &norm)	  { normal    = norm; }
    void		setTextureCoords(const SbVec4f &t){ texCoords = t; }

    // Sets the material index. The index is set to 0 during construction.
    void		setMaterialIndex(int index)  { materialIndex = index; }

    // Sets the detail corresponding to the vertex. The pointer may be
    // NULL, although it is set to NULL during construction.
    void		setDetail(SoDetail *d)		  { detail = d; }

  private:
    SbVec3f		point;		// Object-space point
    SbVec3f		normal;		// Object-space normal
    SbVec4f		texCoords;	// Object-space texture coordinates
    int			materialIndex;	// Material index
    SoDetail		*detail;	// Extra detail info
};

#endif /* _SO_PRIMITIVE_VERTEX_ */
