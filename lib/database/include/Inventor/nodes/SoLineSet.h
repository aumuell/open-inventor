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
 |	This file defines the SoLineSet node class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_LINE_SET_
#define  _SO_LINE_SET_

#include <Inventor/fields/SoMFInt32.h>
#include <Inventor/nodes/SoNonIndexedShape.h>

class SoState;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoLineSet
//
//  Set of (poly)lines. Each line consists of 2 or more vertices,
//  taken in order from the current coordinates, which are joined to
//  form segments. The startIndex field gives the starting coordinate
//  index for the first line. If materials or normals are bound to
//  vertices, they will begin at that index, as well; otherwise, they
//  will start at index 0.  The number of vertices in each polyline is
//  determined by successive entries in the numVertices field.
//
//////////////////////////////////////////////////////////////////////////////


#ifndef IV_STRICT
// Deprecated feature; specifying USE_REST_OF_VERTICES as the last
// value in the numVertices field will use all the points in the
// coordinate array.
#define SO_LINE_SET_USE_REST_OF_VERTICES	(-1)
#endif


// C-api: prefix=SoLSet
// C-api: public=startIndex, numVertices, vertexProperty
class SoLineSet : public SoNonIndexedShape {

    SO_NODE_HEADER(SoLineSet);

  public:
    // Fields
    SoMFInt32		numVertices;	// Number of vertices per line

    // Constructor
    SoLineSet();

  SoEXTENDER public:
    // Implements actions
    virtual void	GLRender(SoGLRenderAction *action);

    // Redefine this to tell open caches that they contain lines
    virtual void	getBoundingBox(SoGetBoundingBoxAction *action);

  SoINTERNAL public:
    static void		initClass();

    // This enum is used to indicate the current material or normal binding
    enum Binding {
	OVERALL, PER_LINE, PER_SEGMENT, PER_VERTEX
    };
    
    //Typedef of pointer to method on IndexedLineSet;
    //This will be used to simplify declaration and initialization.
    typedef void (SoLineSet::*PMLS)(SoGLRenderAction *);
    
  protected:
    // Generates line segments representing line set
    virtual void	generatePrimitives(SoAction *action);

    // Computes bounding box of line set
    virtual void	computeBBox(SoAction *action, SbBox3f &box,
				    SbVec3f &center);

    // Overrides standard method to create an SoLineDetail instance
    virtual SoDetail *	createLineSegmentDetail(SoRayPickAction *action,
						const SoPrimitiveVertex *v1,
						const SoPrimitiveVertex *v2,
						SoPickedPoint *pp);

    virtual ~SoLineSet();

  private:
    // Returns current material or normal binding from action's state
    static Binding	getMaterialBinding(SoAction *action);
    static Binding	getNormalBinding(SoAction *action);

    // Returns TRUE if normal binding is AUTO and there aren't enough
    // PER_VERTEX normals, in which case we turn off lighting and draw
    // BASE_COLOR.
    SbBool	wouldGenerateNormals(SoState *state);

    // Blow vpCache when vertexProperty field changes:
    virtual void	notify(SoNotList *list);

    // This stores the total number of vertices; we use this
    // information to influence Separator's auto-caching algorithm
    // (shapes with very few triangles should be cached because
    // traversing them can be expensive, shapes with lots of triangles
    // shouldn't be cached because they'll take up too much memory).
    int				totalNumVertices;

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
    static PMLS renderFunc[32];

};

#endif /* _SO_LINE_SET_ */
