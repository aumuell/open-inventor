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
 |   $Revision: 1.1 $
 |
 |   Description:
 |	This file defines the SoVertexProperty node class, and the
 |       SoVertexPropertyCache class.
 |
 |   Author(s)		: Alan Norton, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_VERTEX_PROPERTY
#define  _SO_VERTEX_PROPERTY

#include <Inventor/fields/SoMFUInt32.h>
#include <Inventor/fields/SoMFVec3f.h>
#include <Inventor/fields/SoMFVec2f.h>
#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoTextureCoordinateBinding.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoVertexProperty
//
//  SoNode class that manages arrays of data for GLVertex Array Extension.
//  Data arrays include: vertices, normals, colors, texture coordinates.
//  Also has NormalBinding and MaterialBinding fields
//
//////////////////////////////////////////////////////////////////////////////

class SoVertexPropertyCache;
// C-api: prefix=SoVertProp
// C-api: public=vertex, texCoord, normal, normalBinding, orderedRGBA, materialBinding
class SoVertexProperty : public SoNode {

    SO_NODE_HEADER(SoVertexProperty);

  public:

    // Fields:
    SoMFVec3f		vertex;		// Coordinate point(s)
    SoMFVec2f    	texCoord;	// Texture coordinate(s)

    SoMFVec3f		normal;		// Normal vector(s)
    SoSFEnum		normalBinding;  // Ignored unless normal field
					// set

    SoMFUInt32		orderedRGBA;	// Diffuse+transparency
    SoSFEnum		materialBinding;// Ignored unless orderedRGBA
					// field set

    enum Binding {
	OVERALL =		SoMaterialBindingElement::OVERALL,
	PER_PART =		SoMaterialBindingElement::PER_PART,
	PER_PART_INDEXED =	SoMaterialBindingElement::PER_PART_INDEXED,
	PER_FACE =		SoMaterialBindingElement::PER_FACE,
	PER_FACE_INDEXED =	SoMaterialBindingElement::PER_FACE_INDEXED,
	PER_VERTEX =		SoMaterialBindingElement::PER_VERTEX,
	PER_VERTEX_INDEXED =	SoMaterialBindingElement::PER_VERTEX_INDEXED
    };

    // Constructor
    SoVertexProperty();

  SoEXTENDER public:
    virtual void	doAction(SoAction *action);
    virtual void	GLRender(SoGLRenderAction *action);
    virtual void	getBoundingBox(SoGetBoundingBoxAction *action);
    virtual void	callback(SoCallbackAction *action);
    virtual void	pick(SoPickAction *action);

  SoINTERNAL public:
    static void		initClass();

  protected:
    virtual ~SoVertexProperty();
    
};

//////////////////////////////////////////////////////////////////////////////
//
//   Description:
//	The SoVertexPropertyCache class is a convenient way for vertex-based
//      shapes to store information on how they should render
//      themselves.  It takes care of some messiness that they'd
//      otherwise have to figure out.  It is designed to be a very
//      lightweight class.
//
//////////////////////////////////////////////////////////////////////////////

#include <Inventor/elements/SoShapeStyleElement.h>

typedef void SoVPCacheFunc(const char *const);

SoEXTENDER class SoVertexPropertyCache {
  public:

    // API used by shapes:

    SbBool mightNeedSomethingFromState(const SoShapeStyleElement *sse) const
	{ return needFromState & sse->getRenderCaseMask(); }

    void	fillInCache(const SoVertexProperty *,   SoState *);

    SbBool	shouldGenerateNormals(const SoShapeStyleElement *sse) const
	{ return (generateNormals && sse->needNormals()); }

    SbBool	shouldGenerateTexCoords(const SoShapeStyleElement *sse) const
	{ return (generateTexCoords && sse->needTexCoords()); }

    int		getRenderCase(const SoShapeStyleElement *sse) const
	{ return renderCase & sse->getRenderCaseMask(); }
	
    SbBool	haveTexCoordsInVP()
	{ return needFromState & TEXTURE_FUNCTION_BIT; }

    void	sendVertex(const char *vp) const
	{ (*vertexFunc)(vp); }
    void	sendNormal(const char *np) const
	{ (*normalFunc)(np); }
    void	sendColor(const char *cp) const
	{ (*colorFunc)(cp); }
    void	sendTexCoord(const char *tcp) const
	{ (*texCoordFunc)(tcp); }

    int		getNumVertices() const { return numVerts; }
    int		getNumNormals() const { return numNorms; }
    int		getNumColors() const { return numColors; }
    int		getNumTexCoords() const { return numTexCoords; }

    const char *getVertices(int i) const
	{ return vertexPtr + vertexStride*i; }
    const char *getNormals(int i) const 
	{ return normalPtr + normalStride*i; }
    const char *getColors(int i) const
	{ return colorPtr + colorStride*i; }
    const char *getTexCoords(int i) const
	{ return texCoordPtr + texCoordStride*i; }
	
    SbBool colorIsInVtxProp() const {return colorIsInVP;}
    
    //Indicates if non-opaque transparency is in vertex property.
    SbBool transpIsInVtxProp() const {return transpIsInVP;}

    int		getVertexStride() const { return vertexStride; }
    int		getNormalStride() const { return normalStride; }
    int		getColorStride() const { return colorStride; }
    int		getTexCoordStride() const { return texCoordStride; }
    
    SoNormalBindingElement::Binding	getNormalBinding()
	{ return (SoNormalBindingElement::Binding)normalBinding; }
	
    SoMaterialBindingElement::Binding	getMaterialBinding()
	{ return (SoMaterialBindingElement::Binding)materialBinding; }

    SoTextureCoordinateBindingElement::Binding	getTexCoordBinding()
	{ return (SoTextureCoordinateBindingElement::Binding)texCoordBinding; }

    void	invalidate() { needFromState = ALL_FROM_STATE_BITS; }

    // Constructor.   Initializes everything.
    SoVertexPropertyCache();
    
  public:

    // Tables of functions, data, and strides to increment through data.
    SoVPCacheFunc *vertexFunc;
    const char *vertexPtr;
    int vertexStride, numVerts;
    SoVPCacheFunc *normalFunc;
    const char *normalPtr;
    int normalStride, numNorms;
    SoVPCacheFunc *colorFunc;
    const char *colorPtr;
    int colorStride, numColors;
    SoVPCacheFunc *texCoordFunc;
    const char *texCoordPtr;
    int texCoordStride, numTexCoords;

    SbBool generateNormals;
    SbBool generateTexCoords;
    
    unsigned char needFromState;

    // 32 rendering cases.  Non-indexed shapes can get away with just
    // looking at the low 5 bits.  The bits are arranged like this:
    // 43210  BITS
    // -----  ----
    // 00...  Overall color
    // 01...  Part color
    // 10...  Face color
    // 11...  Vtx color
    // ..00.  Overall/No norm
    // ..01.  Part norm
    // ..10.  Face norm
    // ..11.  Vtx norm
    // ....?  Explicit exture coordinates needed
  SoINTERNAL public:
    enum { 
	COLOR_BITS = 0x18,
	PART_COLOR = 0x8,
	FACE_COLOR = 0x10,
	VERTEX_COLOR = 0x18,

	NORMAL_BITS = 0x6,
	PART_NORMAL = 0x2,
	FACE_NORMAL = 0x4,
	VERTEX_NORMAL = 0x6,

	TEXCOORD_BIT = 0x1
	};
    // Bit-mask, set by ::fillInCache method
    // Bits use same as above, plus one for coords and one for 
    // overrides (diffuse color, material binding, transparency)
    // plus one for texture function (if tcoords in VP)
  
    enum {
	TEXTURE_FUNCTION_BIT = 0x80,
	OVERRIDE_FROM_STATE_BIT = 0x40, 
	COORD_FROM_STATE_BIT = 0x20,
	COLOR_FROM_STATE_BITS = COLOR_BITS,
	NORMAL_FROM_STATE_BITS = NORMAL_BITS,
	TEXCOORD_FROM_STATE_BIT = TEXCOORD_BIT,
	ALL_FROM_STATE_BITS = 0xFF
    };
  private:

    //
    // VertexPropertyCache also stores whether or not colors, normals and
    // texture coordinates should be indexed.  Suggestion for writing
    // rendering loops:
    //   
    // To minimize loop overhead, loop unrolling is a good idea.
    // Doing two iterations per loop makes it about 20% faster.  Doing
    // three iterations per loop makes it about 28% faster (only 8%
    // more than two iterations).
    //  
    // On 200MHZ machines, an unrolled-by-3 loop can do close to 5
    // million iterations per second, assuming one call per iteration
    // (e.g. one-color, unlit polygons).
    //
    unsigned char normalBinding;
    unsigned char materialBinding;
    unsigned char texCoordBinding;
    unsigned char renderCase;
    unsigned char colorIsInVP;
    unsigned char transpIsInVP;
};

#endif /* _SO_VERTEX_PROPERTY */

