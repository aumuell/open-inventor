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
 |	This file defines the SoIndexedTriangleStripSet node class.
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_INDEXED_TRIANGLE_SET_
#define  _SO_INDEXED_TRIANGLE_SET_

#include <Inventor/nodes/SoIndexedShape.h>
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/nodes/SoVertexProperty.h>
class SoCoordinateElement;


//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoIndexedTriangleStripSet
//
//  Indexed set of triangles strips.  Strips are separated by the
//  special index SO_END_STRIP_INDEX (-1).  The N indices in the strip
//  define N-2 triangles, which are formed by indexing into the
//  current coordinates. Depending on the current material and normal
//  binding values, the materials and normals for the triangles or
//  vertices may be accessed in order or indexed. If they are indexed,
//  the materialIndex and normalIndex fields are used.
//
//////////////////////////////////////////////////////////////////////////////

// This coordinate index indicates that the current triangle ends and the
// next triangle begins
#define SO_END_STRIP_INDEX	(-1)

class SoNormalBundle;
class SoCoordinateElement;

// C-api: prefix=SoIndTriStripSet
// C-api: public=coordIndex, materialIndex, normalIndex, textureCoordIndex, vertexProperty
class SoIndexedTriangleStripSet : public SoIndexedShape {

    SO_NODE_HEADER(SoIndexedTriangleStripSet);

  public:
    // Constructor
    SoIndexedTriangleStripSet();

  SoEXTENDER public:
    // Implements actions
    virtual void	GLRender(SoGLRenderAction *action);

    // Generates default normals using the given normal bundle. 
    // Returns TRUE.
    virtual SbBool	generateDefaultNormals(SoState *state,
					       SoNormalBundle *nb);
					       
    //Typedef of pointer to method on IndexedTriangleStripSet;
    //This will be used to simplify declaration and initialization.
    typedef void (SoIndexedTriangleStripSet::*PMTSS)(SoGLRenderAction *);
					          
  SoINTERNAL public:
    static void		initClass();

    // This enum is used to indicate the current material or normal binding
    enum Binding {
	OVERALL,
	PER_STRIP,	PER_STRIP_INDEXED,
	PER_TRIANGLE,	PER_TRIANGLE_INDEXED,
	PER_VERTEX,	PER_VERTEX_INDEXED
    };
    
  protected:
    // Generates triangles representing strips
    virtual void	generatePrimitives(SoAction *action);
    

    // Overrides standard method to create an SoFaceDetail instance
    virtual SoDetail *	createTriangleDetail(SoRayPickAction *action,
					     const SoPrimitiveVertex *v1,
					     const SoPrimitiveVertex *v2,
					     const SoPrimitiveVertex *v3,
					     SoPickedPoint *pp);

    ~SoIndexedTriangleStripSet();

  private:
    // Returns current material or normal binding from action's state
    Binding		getMaterialBinding(SoAction *action);
    Binding		getNormalBinding(SoAction *action,
					 SoNormalBundle &nb);

    // Figures out normals, if necessary. Returns TRUE if normals were
    // generated
    SbBool		figureNormals(SoState *state, SoNormalBundle *nb);
    
    // Reset numVertices/numStrips when notified
    virtual void	notify(SoNotList *list);
    
    // Fill in numStrips, numTriangles and numVertices[]
    void countStripsAndTris();
    
    // Number of strips, total number of triangles, and number of
    // vertices per strip:
    int		numStrips;
    int		numTriangles;
    int *	numVertices;

    // 32 different render functions; names are formed like this:
    // Om -- overall material  Pm -- per part material
    // Fm -- per face material Vm -- per vertex material
    // On -- overall normal    Pm -- per part normal
    // Fn -- per face normal   Vm -- per vertex normal
    // T  -- per vertex texture coordinates
    void OmOn(SoGLRenderAction *); void OmOnT(SoGLRenderAction *);
    void OmPn(SoGLRenderAction *); void OmPnT(SoGLRenderAction *);
    void OmFn(SoGLRenderAction *); void OmFnT(SoGLRenderAction *);
    void OmVn(SoGLRenderAction *); void OmVnT(SoGLRenderAction *);
    void PmOn(SoGLRenderAction *); void PmOnT(SoGLRenderAction *);
    void PmPn(SoGLRenderAction *); void PmPnT(SoGLRenderAction *);
    void PmFn(SoGLRenderAction *); void PmFnT(SoGLRenderAction *);
    void PmVn(SoGLRenderAction *); void PmVnT(SoGLRenderAction *);
    void FmOn(SoGLRenderAction *); void FmOnT(SoGLRenderAction *);
    void FmPn(SoGLRenderAction *); void FmPnT(SoGLRenderAction *);
    void FmFn(SoGLRenderAction *); void FmFnT(SoGLRenderAction *);
    void FmVn(SoGLRenderAction *); void FmVnT(SoGLRenderAction *);
    void VmOn(SoGLRenderAction *); void VmOnT(SoGLRenderAction *);
    void VmPn(SoGLRenderAction *); void VmPnT(SoGLRenderAction *);
    void VmFn(SoGLRenderAction *); void VmFnT(SoGLRenderAction *);
    void VmVn(SoGLRenderAction *); void VmVnT(SoGLRenderAction *);

    // Array of function pointers to render functions:
    static PMTSS renderFunc[32];
    
    
};

#endif /* _SO_INDEXED_TRIANGLE_SET_ */
