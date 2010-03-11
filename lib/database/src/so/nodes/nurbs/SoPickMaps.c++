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
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |      _SoNurbsPickV3CurveMap
 |      _SoNurbsPickV4CurveMap
 |      _SoNurbsPickV3SurfaceMap
 |      _SoNurbsPickV4SurfaceMap
 |
 |   Author(s)          : Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <stdio.h>
#include "SoAddPrefix.h"
#include <Inventor/SoPickedPoint.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/details/SoDetail.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoNormal.h>
#include "SoPickMaps.h"

// This stores the texture coordinate until the vertex comes through.
static float tmpTexPt[2] = {0.0, 0.0};

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor for the SoPickV3CurveMap class.
//
// Use: public

_SoNurbsPickV3CurveMap::~_SoNurbsPickV3CurveMap()

//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor for the SoPickV4CurveMap class.
//
// Use: public

_SoNurbsPickV4CurveMap::~_SoNurbsPickV4CurveMap()

//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initialize the v3 curve map.
//
// Use: public

void
_SoNurbsPickV3CurveMap::initialize( SoRayPickAction *pa )

//
////////////////////////////////////////////////////////////////////////
{
    ptIndex = 0;
    pickAction = pa;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initialize the v4 curve map.
//
// Use: public

void
_SoNurbsPickV4CurveMap::initialize( SoRayPickAction *pa )

//
////////////////////////////////////////////////////////////////////////
{
    ptIndex = 0;
    pickAction = pa;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This callback routine is called when a v3f call is generated
//    by the NURBS library.
//
// Use: protected

void
_SoNurbsPickV3CurveMap::point( float *v )

//
////////////////////////////////////////////////////////////////////////
{
    //
    // Store the incoming point and the texture coordinate at that point
    //
    CP[ptIndex][0] = v[0];
    CP[ptIndex][1] = v[1];
    CP[ptIndex][2] = v[2];
    TP[ptIndex][0] = tmpTexPt[0];
    TP[ptIndex][1] = tmpTexPt[1];

    //
    // If the vertex has not completed a line yet, just increment
    // the point count and return.
    //
    if (ptIndex < 1)
    {
        ++ptIndex;
        return;    
    }

    // Intersect the current line
    intersectLine();

    // Overwrite the old vertex with the current vertex.
    CP[0][0] = CP[1][0];
    CP[0][1] = CP[1][1];
    CP[0][2] = CP[1][2];
    TP[0][0] = TP[1][0];
    TP[0][1] = TP[1][1];
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This callback routine is called when a v4f call is generated
//    by the NURBS library.
//
// Use: protected

void
_SoNurbsPickV4CurveMap::point( float *v )

//
////////////////////////////////////////////////////////////////////////
{
    //
    // Store the incoming point and the texture coordinate at that point
    //
    CP[ptIndex][0] = v[0]/v[3];
    CP[ptIndex][1] = v[1]/v[3];
    CP[ptIndex][2] = v[2]/v[3];
    TP[ptIndex][0] = tmpTexPt[0];
    TP[ptIndex][1] = tmpTexPt[1];

    //
    // If the vertex has not completed a line yet, just increment
    // the point count and return.
    //
    if (ptIndex < 1)
    {
        ++ptIndex;
        return;    
    }

    // Intersect the current line
    intersectLine();

    // Overwrite the old vertex with the current vertex.
    CP[0][0] = CP[1][0];
    CP[0][1] = CP[1][1];
    CP[0][2] = CP[1][2];
    TP[0][0] = TP[1][0];
    TP[0][1] = TP[1][1];
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This routine intersects the current line with the pickLine.
//    If an intersection exists, a detail is filled in.
//
// Use: protected

void
_SoNurbsPickV3CurveMap::intersectLine()

//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f             point;
    SoPickedPoint       *pp;

    if (!pickAction->intersect(CP[0], CP[1], point))
        return;

    pp = pickAction->addIntersection(point);
    if (pp != NULL) {

        SbVec3f         norm;
        SbVec4f         texCoord;
        float           ratioFromV1;

        // Compute normal as vector pointing back along the pick ray.
	norm = -pickAction->getLine().getDirection();
        norm.normalize();
        pp->setObjectNormal(norm);
        pp->setMaterialIndex(0);

        // Compute interpolated texture coordinate 
        ratioFromV1 = ((point - CP[0]).length() /
                       (CP[1] - CP[0]).length());
        texCoord[0] = (TP[0][0] * (1.0 - ratioFromV1) +
                    TP[1][0] * ratioFromV1);
        texCoord[1] = (TP[0][1] * (1.0 - ratioFromV1) +
                    TP[1][1] * ratioFromV1);
        texCoord[2] = 0.0;
        texCoord[3] = 1.0;
        pp->setObjectTextureCoords(texCoord);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This routine intersects the current line with the pickLine.
//    If an intersection exists, a detail is filled in.
//
// Use: protected

void
_SoNurbsPickV4CurveMap::intersectLine()

//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f             point;
    SoPickedPoint       *pp;

    if (!pickAction->intersect(CP[0], CP[1], point))
        return;

    pp = pickAction->addIntersection(point);
    if (pp != NULL) {

        SbVec3f         norm;
        SbVec4f         texCoord;
        float           ratioFromV1;

        // Compute normal as vector pointing back along the pick ray.
	norm = -pickAction->getLine().getDirection();
        norm.normalize();
        pp->setObjectNormal(norm);
        pp->setMaterialIndex(0);

        // Compute interpolated texture coordinate 
        ratioFromV1 = ((point - CP[0]).length() /
                       (CP[1] - CP[0]).length());
        texCoord[0] = (TP[0][0] * (1.0 - ratioFromV1) +
                    TP[1][0] * ratioFromV1);
        texCoord[1] = (TP[0][1] * (1.0 - ratioFromV1) +
                    TP[1][1] * ratioFromV1);
        texCoord[2] = 0.0;
        texCoord[3] = 1.0;
        pp->setObjectTextureCoords(texCoord);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor for the SoPickV3SurfaceMap class.
//
// Use: public

_SoNurbsPickV3SurfaceMap::~_SoNurbsPickV3SurfaceMap()

//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor for the SoPickV4SurfaceMap class.
//
// Use: public

_SoNurbsPickV4SurfaceMap::~_SoNurbsPickV4SurfaceMap()

//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initialize the v3 surface map.
//
// Use: public

void
_SoNurbsPickV3SurfaceMap::initialize( SoRayPickAction *pa )

//
////////////////////////////////////////////////////////////////////////
{
    cacheIndices[0] = 0;
    cacheIndices[1] = 1;
    curCacheIndex   = 0;
    curPrimIndex    = 2;
    cacheReady = FALSE;
    pickAction = pa;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initialize the v4 surface map.
//
// Use: public

void
_SoNurbsPickV4SurfaceMap::initialize( SoRayPickAction *pa )

//
////////////////////////////////////////////////////////////////////////
{
    cacheIndices[0] = 0;
    cacheIndices[1] = 1;
    curCacheIndex   = 0;
    curPrimIndex    = 2;
    cacheReady = FALSE;
    pickAction = pa;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This method begins a new strip of triangles.
//
// Use: protected

void
_SoNurbsPickV3SurfaceMap::startNewTriStrip()

//
////////////////////////////////////////////////////////////////////////
{
    cacheIndices[0] = 0;
    cacheIndices[1] = 1;
    curCacheIndex   = 0;
    curPrimIndex    = 2;
    cacheReady = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This method begins a new strip of triangles.
//
// Use: protected

void
_SoNurbsPickV4SurfaceMap::startNewTriStrip()

//
////////////////////////////////////////////////////////////////////////
{
    cacheIndices[0] = 0;
    cacheIndices[1] = 1;
    curCacheIndex   = 0;
    curPrimIndex    = 2;
    cacheReady = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This method swaps the least recent vertices.
//
// Use: public

void
_SoNurbsPickV3SurfaceMap::swapVertices()

//
////////////////////////////////////////////////////////////////////////
{
    curCacheIndex = 1 - curCacheIndex;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This method swaps the least recent vertices.
//
// Use: public

void
_SoNurbsPickV4SurfaceMap::swapVertices()

//
////////////////////////////////////////////////////////////////////////
{
    curCacheIndex = 1 - curCacheIndex;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This callback routine is called when a v3f call is generated
//    by the NURBS library.
//
// Use: protected

void
_SoNurbsPickV3SurfaceMap::point( float *v )

//
////////////////////////////////////////////////////////////////////////
{
    //
    // Store the incoming point and calculate the normal at that point
    //
    P.p[0] = v[0];
    P.p[1] = v[1];
    P.p[2] = v[2];
    computeNormal ();

    if (!cacheReady) {

        // The cache of mesh vertices is still filling, so a triangle
        // isn't ready to be intersected yet.  Just fill this point
        // into the cache and update the cache vertices.
        SP[cacheIndices[curCacheIndex]].p[0] = P.p[0];
        SP[cacheIndices[curCacheIndex]].p[1] = P.p[1];
        SP[cacheIndices[curCacheIndex]].p[2] = P.p[2];
        SP[cacheIndices[curCacheIndex]].norm[0] = P.norm[0];
        SP[cacheIndices[curCacheIndex]].norm[1] = P.norm[1];
        SP[cacheIndices[curCacheIndex]].norm[2] = P.norm[2];
        TP[cacheIndices[curCacheIndex]][0] = tmpTexPt[0];
        TP[cacheIndices[curCacheIndex]][1] = tmpTexPt[1];
        if (curCacheIndex == 1) {
            cacheReady = 1;
        }
        curCacheIndex = 1 - curCacheIndex;
        return;
    }

    // The cache is full now, so triangles are now forming with each
    // vertex.  Take the two cached vertices and the current vertex
    // to form a triangle and intersect it.
    SP[curPrimIndex].p[0] = P.p[0];
    SP[curPrimIndex].p[1] = P.p[1];
    SP[curPrimIndex].p[2] = P.p[2];
    SP[curPrimIndex].norm[0] = P.norm[0];
    SP[curPrimIndex].norm[1] = P.norm[1];
    SP[curPrimIndex].norm[2] = P.norm[2];
    TP[curPrimIndex][0] = tmpTexPt[0];
    TP[curPrimIndex][1] = tmpTexPt[1];
    intersectTriangle();

    // Update the cache indices
    int tmp = curPrimIndex;
    curPrimIndex = cacheIndices[curCacheIndex];
    cacheIndices[curCacheIndex] = tmp; 
    curCacheIndex = 1 - curCacheIndex;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This callback routine is called when a v4f call is generated
//    by the NURBS library.
//
// Use: protected

void
_SoNurbsPickV4SurfaceMap::point( float *v )

//
////////////////////////////////////////////////////////////////////////
{
    //
    // Store the incoming point and calculate the normal at that point
    //
    P.p[0] = v[0];
    P.p[1] = v[1];
    P.p[2] = v[2];
    P.p[3] = v[3];
    computeFirstPartials ();
    computeNormal ();

    if (!cacheReady) {

        // The cache of mesh vertices is still filling, so a triangle
        // isn't ready to be intersected yet.  Just fill this point
        // into the cache and update the cache vertices.
        SP[cacheIndices[curCacheIndex]].p[0] = P.p[0]/P.p[3];
        SP[cacheIndices[curCacheIndex]].p[1] = P.p[1]/P.p[3];
        SP[cacheIndices[curCacheIndex]].p[2] = P.p[2]/P.p[3];
        SP[cacheIndices[curCacheIndex]].norm[0] = P.norm[0];
        SP[cacheIndices[curCacheIndex]].norm[1] = P.norm[1];
        SP[cacheIndices[curCacheIndex]].norm[2] = P.norm[2];
        TP[cacheIndices[curCacheIndex]][0] = tmpTexPt[0];
        TP[cacheIndices[curCacheIndex]][1] = tmpTexPt[1];
        if (curCacheIndex == 1) {
            cacheReady = 1;
        }
        curCacheIndex = 1 - curCacheIndex;
        return;
    }

    // The cache is full now, so triangles are now forming with each
    // vertex.  Take the two cached vertices and the current vertex
    // to form a triangle and intersect it.
    SP[curPrimIndex].p[0] = P.p[0]/P.p[3];
    SP[curPrimIndex].p[1] = P.p[1]/P.p[3];
    SP[curPrimIndex].p[2] = P.p[2]/P.p[3];
    SP[curPrimIndex].norm[0] = P.norm[0];
    SP[curPrimIndex].norm[1] = P.norm[1];
    SP[curPrimIndex].norm[2] = P.norm[2];
    TP[curPrimIndex][0] = tmpTexPt[0];
    TP[curPrimIndex][1] = tmpTexPt[1];
    intersectTriangle();

    // Update the cache indices
    int tmp = curPrimIndex;
    curPrimIndex = cacheIndices[curCacheIndex];
    cacheIndices[curCacheIndex] = tmp; 
    curCacheIndex = 1 - curCacheIndex;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This routine intersects the current triangle with the pickLine.
//    If an intersection exists, a detail is filled in.
//
// Use: protected

void
_SoNurbsPickV3SurfaceMap::intersectTriangle()

//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f             point;
    SbVec3f             barycentric;
    SbBool              onFrontSide;
    SoPickedPoint       *pp;

    if (!pickAction->intersect(SP[cacheIndices[0]].p,
                              SP[cacheIndices[1]].p,
                              SP[curPrimIndex].p,
                              point, barycentric, onFrontSide))
        return;

    pp = pickAction->addIntersection(point);
    if (pp != NULL) {

        SbVec3f         norm, n0, n1, n2;
        SbVec4f         texCoord;

        // Compute normal by interpolating vertex normals using
        // barycentric coordinates
	n0 = SP[0].norm;
	n1 = SP[1].norm;
	n2 = SP[2].norm;
	norm.setValue(barycentric, n0, n1, n2);
        norm.normalize();
        pp->setObjectNormal(norm);
        pp->setMaterialIndex(0);

        // Compute texture coordinates the same way
        texCoord[0] = barycentric[0] * TP[cacheIndices[0]][0] +
                      barycentric[1] * TP[cacheIndices[1]][0] +
                      barycentric[2] * TP[curPrimIndex][0];
        texCoord[1] = barycentric[0] * TP[cacheIndices[0]][1] +
                      barycentric[1] * TP[cacheIndices[1]][1] +
                      barycentric[2] * TP[curPrimIndex][1];
        texCoord[2] = 0.0;
        texCoord[3] = 1.0;
        pp->setObjectTextureCoords(texCoord);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This routine intersects the current triangle with the pickLine.
//    If an intersection exists, a detail is filled in.
//
// Use: protected

void
_SoNurbsPickV4SurfaceMap::intersectTriangle()

//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f             point;
    SbVec3f             barycentric;
    SbBool              onFrontSide;
    SoPickedPoint       *pp;

    if (!pickAction->intersect(SP[cacheIndices[0]].p,
                              SP[cacheIndices[1]].p,
                              SP[curPrimIndex].p,
                              point, barycentric, onFrontSide))
        return;

    pp = pickAction->addIntersection(point);
    if (pp != NULL) {

        SbVec3f         norm, n0, n1, n2;
        SbVec4f         texCoord;

        // Compute normal by interpolating vertex normals using
        // barycentric coordinates
	n0 = SP[0].norm;
	n1 = SP[1].norm;
	n2 = SP[2].norm;
	norm.setValue(barycentric, n0, n1, n2);
        norm.normalize();
        pp->setObjectNormal(norm);
        pp->setMaterialIndex(0);

        // Compute texture coordinates the same way
        texCoord[0] = barycentric[0] * TP[cacheIndices[0]][0] +
                      barycentric[1] * TP[cacheIndices[1]][0] +
                      barycentric[2] * TP[curPrimIndex][0];
        texCoord[1] = barycentric[0] * TP[cacheIndices[0]][1] +
                      barycentric[1] * TP[cacheIndices[1]][1] +
                      barycentric[2] * TP[curPrimIndex][1];
        texCoord[2] = 0.0;
        texCoord[3] = 1.0;
        pp->setObjectTextureCoords(texCoord);
    }
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    This routine copies the values from one SurfacePartial structure
//    to another.
//
// Use: protected

void
_SoNurbsPickV3SurfaceMap::copyPartials(
         _SoNurbsSurfPartial &from,
        _SoNurbsSurfPartial &to )

//
////////////////////////////////////////////////////////////////////////
{
    to.p[0] = from.p[0];
    to.p[1] = from.p[1];
    to.p[2] = from.p[2];
    to.p[3] = from.p[3];
    to.du[0] = from.du[0];
    to.du[1] = from.du[1];
    to.du[2] = from.du[2];
    to.du[3] = from.du[3];
    to.dv[0] = from.dv[0];
    to.dv[1] = from.dv[1];
    to.dv[2] = from.dv[2];
    to.dv[3] = from.dv[3];
    to.norm[0] = from.norm[0];
    to.norm[1] = from.norm[1];
    to.norm[2] = from.norm[2];
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This routine copies the values from one SurfacePartial structure
//    to another.
//
// Use: protected

void
_SoNurbsPickV4SurfaceMap::copyPartials(
        _SoNurbsSurfPartial &from,
        _SoNurbsSurfPartial &to )

//
////////////////////////////////////////////////////////////////////////
{
    to.p[0] = from.p[0];
    to.p[1] = from.p[1];
    to.p[2] = from.p[2];
    to.p[3] = from.p[3];
    to.du[0] = from.du[0];
    to.du[1] = from.du[1];
    to.du[2] = from.du[2];
    to.du[3] = from.du[3];
    to.dv[0] = from.dv[0];
    to.dv[1] = from.dv[1];
    to.dv[2] = from.dv[2];
    to.dv[3] = from.dv[3];
    to.norm[0] = from.norm[0];
    to.norm[1] = from.norm[1];
    to.norm[2] = from.norm[2];
}

void _SoNurbsPickTex2CurveMap::point( float *p )
{
    // Save the point to be used when the next vertex
    // comes along.
    tmpTexPt[0] = p[0];
    tmpTexPt[1] = p[1];
}

void _SoNurbsPickTex2SurfaceMap::point( float *p )
{
    // Save the point to be used when the next vertex
    // comes along.
    tmpTexPt[0] = p[0];
    tmpTexPt[1] = p[1];
}

_SoNurbsPickTex2CurveMap::_SoNurbsPickTex2CurveMap() : _SoNurbsCurveMap( N_T2D, 0, 2 ) {}
_SoNurbsPickTex2CurveMap::~_SoNurbsPickTex2CurveMap() { }
void _SoNurbsPickTex2CurveMap::uderiv( float * ) { }
void _SoNurbsPickTex2CurveMap::uuderiv( float * ) { }

_SoNurbsPickTex2SurfaceMap::~_SoNurbsPickTex2SurfaceMap() { }
void _SoNurbsPickTex2SurfaceMap::uderiv( float * ) { }
void _SoNurbsPickTex2SurfaceMap::vderiv( float * ) { }
void _SoNurbsPickTex2SurfaceMap::uuderiv( float * ) { }
void _SoNurbsPickTex2SurfaceMap::uvderiv( float * ) { }
void _SoNurbsPickTex2SurfaceMap::vvderiv( float * ) { }
