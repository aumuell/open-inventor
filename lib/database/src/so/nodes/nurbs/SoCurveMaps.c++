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

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision $
 |
 |   Classes:
 |      _SoNurbsCurveV3Map
 |      _SoNurbsCurveV4Map
 |
 |   Author(s)          : Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


/* Polynomial Evaluator Interface */

#include <string.h>
#include "SoCurveMaps.h"

_SoNurbsCurveV3Map::~_SoNurbsCurveV3Map() 
{ 
}

_SoNurbsCurveV4Map::~_SoNurbsCurveV4Map() 
{ 
}


void
_SoNurbsCurveV3Map::initialize( int32_t *nv, int32_t *nav, SbVec2f **v )
{
    numVertices = nv;
    numAllocVertices = nav;
    vertices = v;
}

void
_SoNurbsCurveV4Map::initialize( int32_t *nv, int32_t *nav, SbVec2f **v )
{
    numVertices = nv;
    numAllocVertices = nav;
    vertices = v;
}

void
_SoNurbsCurveV3Map::point( float *v )
{
    //
    // Add the vertex to the list of vertices.  The 3rd component of the
    // vertex is 0.0 and is ignored.  Allocate more vertices if there is
    // not enough space.
    //
    if (*numVertices == *numAllocVertices)
    {
        *numAllocVertices += CRV_ALLOC_BLOCK;
        SbVec2f *tmpBlock = new SbVec2f[*numAllocVertices];
	memcpy ((void *)tmpBlock, (void *)*vertices,
                (int) *numVertices*sizeof(SbVec2f));
        delete[] *vertices;
        *vertices = tmpBlock;
    }

    (*vertices)[*numVertices][0] = v[0];
    (*vertices)[(*numVertices)++][1] = v[1];
}


void
_SoNurbsCurveV4Map::point( float *v )
{
    //
    // Add the vertex to the list of vertices.  The 3rd component of the
    // vertex is 0.0 and is ignored.  Allocate more vertices if there is
    // not enough space.
    //
    if (*numVertices == *numAllocVertices)
    {
        *numAllocVertices += CRV_ALLOC_BLOCK;
        SbVec2f *tmpBlock = new SbVec2f[*numAllocVertices];
        memcpy ((void *)tmpBlock, (void *)*vertices,
                (int) *numVertices*sizeof(SbVec2f));
        delete[] *vertices;
        *vertices = tmpBlock;
    }

    (*vertices)[*numVertices][0] = v[0]/v[3];
    (*vertices)[*numVertices++][1] = v[1]/v[3];
}



