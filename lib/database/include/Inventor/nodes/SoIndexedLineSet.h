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
 |	This file defines the SoIndexedLineSet node class.
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_INDEXED_LINE_SET_
#define  _SO_INDEXED_LINE_SET_

#include <Inventor/nodes/SoIndexedShape.h>

class SoState;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoIndexedLineSet
//
//  Indexed set of (poly)lines. Each line consists of 2 or more
//  vertices, each of which is denoted by an integer index (from the
//  coordIndex field) into the current coordinates. Depending on the
//  current material and normal binding values, the materials and
//  normals for the lines or vertices may be accessed in order or
//  indexed. If they are indexed, the materialIndex and normalIndex
//  fields are used. Texture coordinates may also be indexed, using
//  the textureCoordIndex field.
//
//////////////////////////////////////////////////////////////////////////////

// This coordinate index indicates that the current line ends and the
// next line begins
#define SO_END_LINE_INDEX	(-1)

// C-api: prefix=SoIndLSet
// C-api: public=coordIndex, materialIndex, normalIndex, textureCoordIndex, vertexProperty
class SoIndexedLineSet : public SoIndexedShape {

    SO_NODE_HEADER(SoIndexedLineSet);

  public:
    // Inherits all fields

    // Constructor
    SoIndexedLineSet();

  SoEXTENDER public:
    // Implements actions
    virtual void	GLRender(SoGLRenderAction *action);

    // Redefine this to tell open caches that they contain lines
    virtual void	getBoundingBox(SoGetBoundingBoxAction *action);
					       
    //Typedef of pointer to method on IndexedLineSet;
    //This will be used to simplify declaration and initialization.
    typedef void (SoIndexedLineSet::*PMILS)(SoGLRenderAction *);
					          
  SoINTERNAL public:
    static void		initClass();

    // This enum is used to indicate the current material or normal binding
    enum Binding {
	OVERALL,
	PER_SEGMENT,	PER_SEGMENT_INDEXED,
	PER_LINE,	PER_LINE_INDEXED,
	PER_VERTEX,	PER_VERTEX_INDEXED
    };

  protected:
    // Generates line segments representing line set
    virtual void	generatePrimitives(SoAction *action);

    // Overrides standard method to create an SoLineDetail instance
    virtual SoDetail *	createLineSegmentDetail(SoRayPickAction *action,
						const SoPrimitiveVertex *v1,
						const SoPrimitiveVertex *v2,
						SoPickedPoint *pp);
    virtual ~SoIndexedLineSet();

  private:
    // Returns current material or normal binding from action's state
    static Binding	getMaterialBinding(SoAction *action);
    static Binding	getNormalBinding(SoAction *action);

    // Reset numVertices/numSegments/numPolylines when notified
    virtual void	notify(SoNotList *list);
    
    // Fill in numPolylines, numSegments 
    void countPolylinesAndSegments();
    
    // Number of polylines, number of line segments, and vertices per polyline

    int		numSegments;
    int		numPolylines;
    int *	numVertices;
        
    // Returns TRUE if normal binding is AUTO and there aren't enough
    // PER_VERTEX normals, in which case we turn off lighting and draw
    // BASE_COLOR.
    SbBool	wouldGenerateNormals(SoState *state);

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
    static PMILS renderFunc[32];
    
};

#endif /* _SO_INDEXED_LINE_SET_ */
    
