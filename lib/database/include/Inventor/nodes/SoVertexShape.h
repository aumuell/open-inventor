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
 |	This file defines the SoVertexShape node class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_VERTEX_SHAPE_
#define  _SO_VERTEX_SHAPE_

#include <Inventor/nodes/SoShape.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/fields/SoSFNode.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoVertexShape
//
//  Abstract vertex-based shape node class. All nodes derived from
//  this class are shapes that are constructed from vertices at some
//  or all of the current coordinates.  They all have vertexProperty nodes,
//  and vpCaches.
//
//////////////////////////////////////////////////////////////////////////////

class SoNormalBundle;
class SoNormalCache;
class SoState;


// C-api: abstract
// C-api: prefix=SoVtx
class SoVertexShape : public SoShape {

    SO_NODE_ABSTRACT_HEADER(SoVertexShape);
    
  public:
      SoSFNode		vertexProperty;

  SoEXTENDER public:
    // Subclasses may define this method to generate normals to use
    // when the normal binding is DEFAULT and there aren't enough
    // normals in the state. This should use the given SoNormalBundle
    // to generate the normals. Returns TRUE if normals were generated.
    virtual SbBool	generateDefaultNormals(SoState *state,
					       SoNormalBundle *nb);

  SoINTERNAL public:
    static void		initClass();

    // Redefines this to invalidate normal cache
    virtual void	notify(SoNotList *list);

  protected:
    // Constructor - makes this abstract
    SoVertexShape();

    virtual ~SoVertexShape();
    
    // Returns TRUE if the shape should be rendered. 
    // Checks for transparency in vertexProperty node before invoking
    // render action.
    virtual SbBool	shouldGLRender(SoGLRenderAction *action);
    
    // vpCache stores information from one render to the next about
    // what information needs to be grabbed from the state, etc.
    SoVertexPropertyCache	vpCache;

    // Sets up a cache to hold normals. This assumes the cache depends
    // on only the current coordinates, shape hints, and crease angle.
    void		setNormalCache(SoState *state,
				       int numNormals, const SbVec3f *normals);

    // Returns the current normal cache, or NULL if there is none
    SoNormalCache	*getNormalCache() const { return normalCache; }

  private:
    // This allows instances to cache normals that have been generated
    SoNormalCache	*normalCache;
};

#endif /* _SO_VERTEX_SHAPE_ */
