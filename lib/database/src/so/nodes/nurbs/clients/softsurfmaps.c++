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

#include <stdio.h>
#include "softsurfacemaps.h"

_SoNurbsVertex3SurfaceMap::~_SoNurbsVertex3SurfaceMap(){}
void _SoNurbsVertex3SurfaceMap::domain( float * ) { }
void _SoNurbsVertex3SurfaceMap::uuderiv( float * ) { }
void _SoNurbsVertex3SurfaceMap::uvderiv( float * ) { }
void _SoNurbsVertex3SurfaceMap::vvderiv( float * ) { }

static int numVerts = 0;
static float texCachePt[2][2] = {{0.0, 0.0},{0.0, 0.0}};
static float curTexPt[2] = {0.0, 0.0};
static int validTexCoords = 0;

void 
_SoNurbsVertex3SurfaceMap::point( float *v )
{
    computeNormal();

    // Render the vertices as independent triangles.
    // If less than three vertices have been sent,
    // just put them in the cache.  Otherwise, send
    // the cache vertices and the current vertex to
    // the GL.
    if (numVerts > 1) {
       
        // Send down first cache vertex
        ::glNormal3fv( cachePt[0].norm );
        if (validTexCoords)
            ::glTexCoord2fv( texCachePt[0] );
        ::glVertex3fv( cachePt[0].p );

        // Send down second cache vertex
        ::glNormal3fv( cachePt[1].norm );
        if (validTexCoords)
            ::glTexCoord2fv( texCachePt[1] );
        ::glVertex3fv( cachePt[1].p );

        // Send down current vertex
        ::glNormal3fv( P.norm );
        if (validTexCoords)
            ::glTexCoord2fv( curTexPt );
        ::glVertex3fv( v );
        isUsed = 1;
    }
    
    // Save the point away in the correct cache location.
    cachePt[ptIndex].p[0] = v[0];
    cachePt[ptIndex].p[1] = v[1];
    cachePt[ptIndex].p[2] = v[2];
    cachePt[ptIndex].p[3] = 1.0;
    cachePt[ptIndex].norm[0] = P.norm[0];
    cachePt[ptIndex].norm[1] = P.norm[1];
    cachePt[ptIndex].norm[2] = P.norm[2];
    texCachePt[ptIndex][0] = curTexPt[0];
    texCachePt[ptIndex][1] = curTexPt[1];
    ptIndex = 1 - ptIndex;
    numVerts++;
}

void 
_SoNurbsVertex3SurfaceMap::startNewTriStrip()
{
    // A new tri strip has begun from scratch.
    // Just reset the ptIndex to start saving points.
    ptIndex = 0;
    numVerts = 0;
    validTexCoords = 0;
}

void
_SoNurbsVertex3SurfaceMap::swapTriStripVertices()
{
    if (!isUsed)
        return;

    swapCalled = 1;
    ptIndex = 1 - ptIndex;
}


void 
_SoNurbsVertex3SurfaceMap::uderiv( float *v )
{
    P.du[0] = v[0];
    P.du[1] = v[1];
    P.du[2] = v[2];
}

void 
_SoNurbsVertex3SurfaceMap::vderiv( float *v )
{
    P.dv[0] = v[0];
    P.dv[1] = v[1];
    P.dv[2] = v[2];
}

void
_SoNurbsVertex3SurfaceMap::computeNormal( void )
{
    /* compute normal, modulo factor of 1/P.p[3]^4 */
    P.norm[0] = P.du[1]*P.dv[2] - P.du[2]*P.dv[1];
    P.norm[1] = P.du[2]*P.dv[0] - P.du[0]*P.dv[2];
    P.norm[2] = P.du[0]*P.dv[1] - P.du[1]*P.dv[0];

    //
    // The normals must be normalized because on PIs, the nmode()
    // gl call doesn't work.
    //
    float *p = P.norm;
    float len = sqrtf(p[0]*p[0] + p[1]*p[1] + p[2]*p[2]);
    if( len == 0.0 ) return;
    len = 1.0/len;
    p[0] *= len;
    p[1] *= len;
    p[2] *= len;
}

_SoNurbsVertex4SurfaceMap::~_SoNurbsVertex4SurfaceMap(){}
void _SoNurbsVertex4SurfaceMap::domain( float * ) { }
void _SoNurbsVertex4SurfaceMap::uuderiv( float * ) { }
void _SoNurbsVertex4SurfaceMap::uvderiv( float * ) { }
void _SoNurbsVertex4SurfaceMap::vvderiv( float * ) { }

void 
_SoNurbsVertex4SurfaceMap::point( float *v )
{
    P.p[0] = v[0];
    P.p[1] = v[1];
    P.p[2] = v[2];
    P.p[3] = v[3];

    computeFirstPartials( );
    computeNormal( );

    // Render the vertices as independent triangles.
    // If less than three vertices have been sent,
    // just put them in the cache.  Otherwise, send
    // the cache vertices and the current vertex to
    // the GL.
    if (numVerts > 1) {

        // Send down first cache vertex
        ::glNormal3fv( cachePt[0].norm );
        if (validTexCoords)
            ::glTexCoord2fv( texCachePt[0] );
        ::glVertex4fv( cachePt[0].p );

        // Send down second cache vertex
        ::glNormal3fv( cachePt[1].norm );
        if (validTexCoords)
            ::glTexCoord2fv( texCachePt[1] );
        ::glVertex4fv( cachePt[1].p );

        // Send down current vertex
        ::glNormal3fv( P.norm );
        if (validTexCoords)
            ::glTexCoord2fv( curTexPt );
        ::glVertex4fv( v );
        isUsed = 1;
    }
    
    // Save the point away in the correct cache location.
    cachePt[ptIndex].p[0] = v[0];
    cachePt[ptIndex].p[1] = v[1];
    cachePt[ptIndex].p[2] = v[2];
    cachePt[ptIndex].p[3] = v[3];
    cachePt[ptIndex].norm[0] = P.norm[0];
    cachePt[ptIndex].norm[1] = P.norm[1];
    cachePt[ptIndex].norm[2] = P.norm[2];
    texCachePt[ptIndex][0] = curTexPt[0];
    texCachePt[ptIndex][1] = curTexPt[1];
    ptIndex = 1 - ptIndex;
    numVerts++;
}

void 
_SoNurbsVertex4SurfaceMap::startNewTriStrip()
{
    // A new tri strip has begun from scratch.
    // Just reset the ptIndex to start saving points.
    ptIndex = 0;
    numVerts = 0;
    validTexCoords = 0;
}

void
_SoNurbsVertex4SurfaceMap::swapTriStripVertices()
{
    if (!isUsed)
        return;

    swapCalled = 1;
    ptIndex = 1 - ptIndex;
}

void 
_SoNurbsVertex4SurfaceMap::uderiv( float *v )
{
    P.du[0] = v[0];
    P.du[1] = v[1];
    P.du[2] = v[2];
    P.du[3] = v[3];
}

void 
_SoNurbsVertex4SurfaceMap::vderiv( float *v )
{
    P.dv[0] = v[0];
    P.dv[1] = v[1];
    P.dv[2] = v[2];
    P.dv[3] = v[3];
}

void
_SoNurbsVertex4SurfaceMap::computeFirstPartials( void )
{

    /* compute u partial, modulo factor of 1/P.p[3]^2 */
    P.du[0] = P.du[0]*P.p[3] - P.du[3]*P.p[0];
    P.du[1] = P.du[1]*P.p[3] - P.du[3]*P.p[1];
    P.du[2] = P.du[2]*P.p[3] - P.du[3]*P.p[2];

    /* compute v partial, modulo factor of 1/P.p[3]^2 */
    P.dv[0] = P.dv[0]*P.p[3] - P.dv[3]*P.p[0];
    P.dv[1] = P.dv[1]*P.p[3] - P.dv[3]*P.p[1];
    P.dv[2] = P.dv[2]*P.p[3] - P.dv[3]*P.p[2];
}

void
_SoNurbsVertex4SurfaceMap::computeNormal( void )
{
    /* compute normal, modulo factor of 1/P.p[3]^4 */
    P.norm[0] = P.du[1]*P.dv[2] - P.du[2]*P.dv[1];
    P.norm[1] = P.du[2]*P.dv[0] - P.du[0]*P.dv[2];
    P.norm[2] = P.du[0]*P.dv[1] - P.du[1]*P.dv[0];

    //
    // The normals must be normalized because on PIs, the nmode()
    // gl call doesn't work.
    //
    float *p = P.norm;
    float len = sqrtf(p[0]*p[0] + p[1]*p[1] + p[2]*p[2]);
    if (len <=0.0) return;
    len = 1.0/len;
    p[0] *= len;
    p[1] *= len;
    p[2] *= len;
}

void _SoNurbsColor4SurfaceMap::domain( float * ) { }
void _SoNurbsColor4SurfaceMap::point( float *p )
{
    ::glColor4fv( p );

    // Save the color away in case a swaptmesh comes
    // along in which case the point would be reused
    // at the start of the new tmesh.
    cachePt[ptIndex][0] = p[0];
    cachePt[ptIndex][1] = p[1];
    cachePt[ptIndex][2] = p[2];
    cachePt[ptIndex][3] = p[3];
    ptIndex = 1 - ptIndex;
}

void 
_SoNurbsColor4SurfaceMap::startNewTriStrip()
{
    // A new tri strip has begun from scratch.
    // Just reset the ptIndex to start saving points.
    ptIndex = 0;
}

void
_SoNurbsColor4SurfaceMap::swapTriStripVertices()
{
    ptIndex = 1 - ptIndex;
}


_SoNurbsColor4SurfaceMap::~_SoNurbsColor4SurfaceMap(){}
void _SoNurbsColor4SurfaceMap::uderiv( float * ) { }
void _SoNurbsColor4SurfaceMap::vderiv( float * ) { }
void _SoNurbsColor4SurfaceMap::uuderiv( float * ) { }
void _SoNurbsColor4SurfaceMap::uvderiv( float * ) { }
void _SoNurbsColor4SurfaceMap::vvderiv( float * ) { }

void _SoNurbsTexture2SurfaceMap::domain( float * ) { }
void _SoNurbsTexture2SurfaceMap::point( float *p )
{
    // Save the point as it will be used when a vertex comes along.
    curTexPt[0] = p[0];
    curTexPt[1] = p[1];
    validTexCoords = 1;
}

_SoNurbsTexture2SurfaceMap::~_SoNurbsTexture2SurfaceMap() { }
void _SoNurbsTexture2SurfaceMap::uderiv( float * ) { }
void _SoNurbsTexture2SurfaceMap::vderiv( float * ) { }
void _SoNurbsTexture2SurfaceMap::uuderiv( float * ) { }
void _SoNurbsTexture2SurfaceMap::uvderiv( float * ) { }
void _SoNurbsTexture2SurfaceMap::vvderiv( float * ) { }
