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
 |	This file defines the SoIndexedShape node class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_INDEXED_SHAPE_
#define  _SO_INDEXED_SHAPE_

#include <Inventor/fields/SoMFInt32.h>
#include <Inventor/nodes/SoVertexShape.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoIndexedShape
//
//  Abstract indexed shape node class. All nodes derived from this
//  (such as SoIndexedFaceSet and SoIndexedLineSet) are shapes
//  that are constructed from vertices defined by indexing into the
//  current coordinates. The coordinate indices are stored in the
//  coordIndex field, which is used by all subclasses.
//
//  One rule is used: all coordinate indices less than 0 may be used
//  for special purposes, such as to denote the end of a face or
//  polyline. This means that all indices < 0 can be ignored safely
//  when looking at vertices.
//
//  Depending on the current material, normal, and texture coordinate
//  binding values, materials, normals, and texture coordinates may be
//  accessed in order or may be indexed using the materialIndex,
//  normalIndex, and textureCoordIndex fields.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: abstract
// C-api: prefix=SoInd
class SoIndexedShape : public SoVertexShape {

    SO_NODE_ABSTRACT_HEADER(SoIndexedShape);

  public:
    // Fields inherited by all subclasses:
    SoMFInt32		coordIndex;		// Coordinate indices
    SoMFInt32		materialIndex;		// Material indices
    SoMFInt32		normalIndex;		// Surface normal indices
    SoMFInt32		textureCoordIndex;	// Texture Coordinate indices

  SoINTERNAL public:
    static void		initClass();

  protected:
    // Constructor - makes this abstract
    SoIndexedShape();

    // This handles bounding box computation for all shapes derived
    // from this class. It sets the bounding box to contain all
    // vertices of the shape, assuming that the shape uses the
    // coordinates indexed by all non-negative values in the
    // coordIndex field. It also sets the center to the average of the
    // vertices' coordinates.
    virtual void	computeBBox(SoAction *action, SbBox3f &box,
				    SbVec3f &center);

    // Returns TRUE if texture coordinates should be indexed
    static SbBool	areTexCoordsIndexed(SoAction *action);

    // Starting at a given index in the coordIndex array, figure out
    // how many vertices there are before either the end of the array
    // or the next 'border' index
    int			getNumVerts(int startCoord);

    // Setup for fast rendering.  This should be called by subclasses,
    // which can then use the textureI/colorI/normalI arrays (which
    // will either point to one of the coordIndex arrays, or to a
    // consective array of integers.  This must be called AFTER the
    // vpCache has been filled in.
    void		setupIndices(int numParts, int numFaces,
				SbBool needNormals, SbBool needTexCoords);

    // These must not be called unless setupIndices has been called first:
    const int32_t *	getNormalIndices()
	{ return (normalI ? normalI : consecutiveIndices); }
    const int32_t *	getColorIndices()
	{ return (colorI ? colorI : consecutiveIndices); }
    const int32_t *	getTexCoordIndices()
	{ return (texCoordI ? texCoordI : consecutiveIndices); }

    // Keep indices up to date if things change
    virtual void	notify(SoNotList *list);

    virtual ~SoIndexedShape();

  private:
    // Internal routines used to allocate sequential indices so the
    // same rendering loops can be used for indexed or non-indexed
    // cases:
    void	allocateSequential(int howMany);
    int32_t*	allocateSequentialWithHoles();

    // These are filled in by the setupIndices routine:
    const int32_t	*texCoordI;
    const int32_t	*colorI;
    const int32_t	*normalI;
    static int32_t	*consecutiveIndices;
    static int	numConsecutiveIndicesAllocated;

    unsigned char materialBinding;
    unsigned char normalBinding;
    unsigned char texCoordBinding;
};

#endif /* _SO_INDEXED_SHAPE_ */
