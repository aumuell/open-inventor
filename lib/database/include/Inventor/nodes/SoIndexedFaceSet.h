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
 |	This file defines the SoIndexedFaceSet node class.
 |
 |   Author(s)		: Paul S. Strauss, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_INDEXED_FACE_SET_
#define  _SO_INDEXED_FACE_SET_

#include <Inventor/nodes/SoIndexedShape.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoIndexedFaceSet
//
//  Indexed set of faces. Each face consists of 3 or more vertices,
//  each of which is denoted by an integer index (from the coordIndex
//  field) into the current coordinates. Depending on the current
//  material and normal binding values, the materials and normals for
//  the faces or vertices may be accessed in order or indexed. If they
//  are indexed, the materialIndex and normalIndex fields are used.
//
//////////////////////////////////////////////////////////////////////////////

// This coordinate index indicates that the current face ends and the
// next face begins
#define SO_END_FACE_INDEX	(-1)

class SoNormalBundle;
class SoCoordinateElement;
class SoShapeStyleElement;

// C-api: prefix=SoIndFSet
// C-api: public=coordIndex, materialIndex, normalIndex, textureCoordIndex, vertexProperty
class SoIndexedFaceSet : public SoIndexedShape {

    SO_NODE_HEADER(SoIndexedFaceSet);

  public:
    // Inherits fields

    // Constructor
    SoIndexedFaceSet();

  SoEXTENDER public:
    // Implements actions
    virtual void	GLRender(SoGLRenderAction *action);

    // Generates default normals using the given normal bundle. 
    // Returns TRUE.
    virtual SbBool	generateDefaultNormals(SoState *state,
					       SoNormalBundle *nb);

    // Typedef of pointer to method on IndexedFaceSet;
    // This will be used to simplify declaration and initialization.
    typedef void (SoIndexedFaceSet::*PMFS)(SoGLRenderAction *);
					          
  SoINTERNAL public:
    static void		initClass();

    // This enum is used to indicate the current material or normal binding
    enum Binding {
	OVERALL, PER_FACE, PER_FACE_INDEXED, PER_VERTEX,
	    PER_VERTEX_INDEXED
    };

  protected:
    // Generates triangles representing faces
    virtual void	generatePrimitives(SoAction *action);

    // Overrides standard method to create an SoFaceDetail instance
    virtual SoDetail *	createTriangleDetail(SoRayPickAction *action,
					     const SoPrimitiveVertex *v1,
					     const SoPrimitiveVertex *v2,
					     const SoPrimitiveVertex *v3,
					     SoPickedPoint *pp);
    ~SoIndexedFaceSet();

  private:
    // Saves normal binding when generating primitives for picking
    Binding		savedNormalBinding;

    // Returns current material or normal binding from action's state
    Binding		getMaterialBinding(SoAction *action);
    Binding		getNormalBinding(SoAction *action,
					 SoNormalBundle *nb);

    // Figures out normals, if necessary. Returns TRUE if normals were
    // generated
    SbBool		figureNormals(SoState *state, SoNormalBundle *nb);

    // set numTris/Quads/Faces to -1 when notified
    virtual void	notify(SoNotList *list);
    
    // Set number of triangles/quads/faces, based on coordIndex array.
    void setupNumTrisQuadsFaces();
    
    // Number of triangles, quads, (n>4)-vertex faces
    int	    numTris, numQuads, numFaces;

    // 18*3ferent render functions; names are formed like this:
    // Om -- overall material
    // Fm -- per face material Vm -- per vertex material
    // On -- overall normal
    // Fn -- per face normal   Vm -- per vertex normal
    // T  -- per vertex texture coordinates
    void TriOmOn(SoGLRenderAction *); void TriOmOnT(SoGLRenderAction *);
    void TriOmFn(SoGLRenderAction *); void TriOmFnT(SoGLRenderAction *);
    void TriOmVn(SoGLRenderAction *); void TriOmVnT(SoGLRenderAction *);
    void TriFmOn(SoGLRenderAction *); void TriFmOnT(SoGLRenderAction *);
    void TriFmFn(SoGLRenderAction *); void TriFmFnT(SoGLRenderAction *);
    void TriFmVn(SoGLRenderAction *); void TriFmVnT(SoGLRenderAction *);
    void TriVmOn(SoGLRenderAction *); void TriVmOnT(SoGLRenderAction *);
    void TriVmFn(SoGLRenderAction *); void TriVmFnT(SoGLRenderAction *);
    void TriVmVn(SoGLRenderAction *); void TriVmVnT(SoGLRenderAction *);
    void QuadOmOn(SoGLRenderAction *); void QuadOmOnT(SoGLRenderAction *);
    void QuadOmFn(SoGLRenderAction *); void QuadOmFnT(SoGLRenderAction *);
    void QuadOmVn(SoGLRenderAction *); void QuadOmVnT(SoGLRenderAction *);
    void QuadFmOn(SoGLRenderAction *); void QuadFmOnT(SoGLRenderAction *);
    void QuadFmFn(SoGLRenderAction *); void QuadFmFnT(SoGLRenderAction *);
    void QuadFmVn(SoGLRenderAction *); void QuadFmVnT(SoGLRenderAction *);
    void QuadVmOn(SoGLRenderAction *); void QuadVmOnT(SoGLRenderAction *);
    void QuadVmFn(SoGLRenderAction *); void QuadVmFnT(SoGLRenderAction *);
    void QuadVmVn(SoGLRenderAction *); void QuadVmVnT(SoGLRenderAction *);
    void GenOmOn(SoGLRenderAction *); void GenOmOnT(SoGLRenderAction *);
    void GenOmFn(SoGLRenderAction *); void GenOmFnT(SoGLRenderAction *);
    void GenOmVn(SoGLRenderAction *); void GenOmVnT(SoGLRenderAction *);
    void GenFmOn(SoGLRenderAction *); void GenFmOnT(SoGLRenderAction *);
    void GenFmFn(SoGLRenderAction *); void GenFmFnT(SoGLRenderAction *);
    void GenFmVn(SoGLRenderAction *); void GenFmVnT(SoGLRenderAction *);
    void GenVmOn(SoGLRenderAction *); void GenVmOnT(SoGLRenderAction *);
    void GenVmFn(SoGLRenderAction *); void GenVmFnT(SoGLRenderAction *);
    void GenVmVn(SoGLRenderAction *); void GenVmVnT(SoGLRenderAction *);

    // Array of function pointers to render functions:
    static PMFS TriRenderFunc[32];
    static PMFS QuadRenderFunc[32];
    static PMFS GenRenderFunc[32];
};

#endif /* _SO_INDEXED_FACE_SET_ */
