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
 * Copyright (C) 1990-93   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Description:
 |	Downgrade from 2.0 IndexedTriangleMesh format to the 1.0 format
 |      (converts from an IndexedTriangleStripSet)
 |
 |   Author(s): Gavin Bell,  David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_V1INDEXEDTRIANGLEMESH_
#define  _SO_V1INDEXEDTRIANGLEMESH_

#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/fields/SoMFInt32.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoIndexedTriangleMeshV1
//
//////////////////////////////////////////////////////////////////////////////

#define SO_END_MESH_INDEX	(-1)	/* End of mesh; start of next one */
#define SO_SWAP_MESH_INDEX	(-2)	/* Swaps vertices */

class SoIndexedTriangleStripSet;

class SoIndexedTriangleMeshV1 : public SoNode {

    SO_NODE_HEADER(SoIndexedTriangleMeshV1);

  public:
    // Fields
    SoMFInt32		coordIndex;
    SoMFInt32		materialIndex;
    SoMFInt32		normalIndex;
    SoMFInt32		textureCoordIndex;

    // Constructor
    SoIndexedTriangleMeshV1();

    // Convert the 2.0 tri strip set to a 1.0 tmesh
    static SoNode *	downgrade(SoIndexedTriangleStripSet *tstrip);
    
  SoINTERNAL public:
    static void		initClass();
};

#endif /* _SO_V1INDEXEDTRIANGLEMESH_ */
