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

/////////////////////////////////////////////////////////////////////////////
//
// IfHolder class: this holds the nodes and other info used in the
// process of fixing a scene graph.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef  _IF_HOLDER_
#define  _IF_HOLDER_

#include <Inventor/SbBasic.h>

class SoCoordinate3;
class SoIndexedFaceSet;
class SoIndexedShape;
class SoIndexedTriangleStripSet;
class SoMaterial;
class SoMaterialBinding;
class SoNode;
class SoNormal;
class SoNormalBinding;
class SoSeparator;
class SoTextureCoordinate2;
class SoTextureCoordinateBinding;

class IfHolder {

  public:
    IfHolder(SoNode *graphRoot, SbBool doStrips,
	     SbBool doNormals, SbBool doTexCoords);
    ~IfHolder();

    // This is the root of the graph being fixed
    SoNode			*origRoot;

    // This flag specifies whether to create triangle strips or
    // independent triangular faces
    SbBool			doStrips;

    // These flags specify whether to create normals and texture
    // coordinates
    SbBool			doNormals;
    SbBool			doTexCoords;

    // These nodes form the graph resulting from fixing a scene graph:
    SoSeparator			*root;
    SoCoordinate3		*coords;
    SoNormal			*normals;
    SoNormalBinding		*normalBinding;
    SoMaterialBinding		*materialBinding;
    SoTextureCoordinate2	*texCoords;
    SoTextureCoordinateBinding	*texCoordBinding;

    // One of these is used to hold the resulting triangles:
    SoIndexedFaceSet		*faceSet;
    SoIndexedTriangleStripSet	*stripSet;

    // This holds whichever of the above shapes is actually used:
    SoIndexedShape		*triSet;

    // Converts the scene graph to use an SoVertexProperty node for
    // the properties. The given material is used in case the
    // materials need to be copied into the SoVertexProperty node.
    void			convertToVertexProperty(SoMaterial *mtl);
};

#endif /* _IF_HOLDER_ */
