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
 |      _SoNurbsPrimV3CurveMap
 |      _SoNurbsPrimV4CurveMap
 |      _SoNurbsPrimV3SurfaceMap
 |      _SoNurbsPrimV4SurfaceMap
 |      _SoNurbsPrimTex2CurveMap
 |      _SoNurbsPrimTex2SurfaceMap
 |
 |   Author(s)          : Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <stdio.h>
#include <Inventor/actions/SoAction.h>
#include <Inventor/elements/SoNormalElement.h>
#include "SoAddPrefix.h"
#include "SoPrimMaps.h"

// This stores the texture coordinate until the vertex comes through.
static float tmpPrimTex[2] = {0.0, 0.0};

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor for the _SoNurbsPrimV3CurveMap class.
//
// Use: public

_SoNurbsPrimV3CurveMap::_SoNurbsPrimV3CurveMap()

//
////////////////////////////////////////////////////////////////////////
{
    CP = new SbVec3f[2];
    TP = new SbVec4f[2];
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor for the _SoNurbsPrimV4CurveMap class.
//
// Use: public

_SoNurbsPrimV4CurveMap::_SoNurbsPrimV4CurveMap()

//
////////////////////////////////////////////////////////////////////////
{
    CP = new SbVec3f[2];
    TP = new SbVec4f[2];
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor for the _SoNurbsPrimV3CurveMap class.
//
// Use: public

_SoNurbsPrimV3CurveMap::~_SoNurbsPrimV3CurveMap()

//
////////////////////////////////////////////////////////////////////////
{
    delete [] TP;
    delete [] CP;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor for the _SoNurbsPrimV4CurveMap class.
//
// Use: public

_SoNurbsPrimV4CurveMap::~_SoNurbsPrimV4CurveMap()

//
////////////////////////////////////////////////////////////////////////
{
    delete [] TP;
    delete [] CP;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initialize the v3 curve map.
//
// Use: public

void
_SoNurbsPrimV3CurveMap::initialize(
    SoAction *act,
    SoNurbsPrimCB *cb,
    void *data )

//
////////////////////////////////////////////////////////////////////////
{
    const SoNormalElement *normElem =
            SoNormalElement::getInstance(act->getState());
    if(normElem->getNum()>0){
	const SbVec3f &tmpNorm = normElem->get(0);   
	normal.setValue(tmpNorm.getValue());
    } 
    else normal.setValue(0., 0., 1.);
    
    ptIndex  = 0;
    action   = act;
    primCB   = cb;
    userData = data;
    TP[0].setValue(0.0, 0.0, 0.0, 1.0);
    TP[1] = TP[0];
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initialize the v4 curve map.
//
// Use: public

void
_SoNurbsPrimV4CurveMap::initialize(
    SoAction *act,
    SoNurbsPrimCB *cb,
    void *data )

//
////////////////////////////////////////////////////////////////////////
{
    const SoNormalElement *normElem =
            SoNormalElement::getInstance(act->getState());
	    
    if(normElem->getNum()>0){
	const SbVec3f &tmpNorm = normElem->get(0);   
	normal.setValue(tmpNorm.getValue());
    } 
    else normal.setValue(0., 0., 1.);    
  
    ptIndex = 0;
    action   = act;
    primCB   = cb;
    userData = data;
    TP[0].setValue(0.0, 0.0, 0.0, 1.0);
    TP[1] = TP[0];
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This callback routine is called when a v3f call is generated
//    by the NURBS library.
//
// Use: protected

void
_SoNurbsPrimV3CurveMap::point( float *v )

//
////////////////////////////////////////////////////////////////////////
{
    //
    // Store the incoming point and the current normal at that point
    //
    CP[ptIndex][0] = v[0];
    CP[ptIndex][1] = v[1];
    CP[ptIndex][2] = v[2];
    TP[ptIndex][0] = tmpPrimTex[0];
    TP[ptIndex][1] = tmpPrimTex[1];

    //
    // If the vertex has not completed a line yet, just increment
    // the point count and return.
    //
    if (ptIndex < 1)
    {
        ++ptIndex;
        return;    
    }

    //
    // This vertex completes a line.  Generate a primitive for the
    // line and invoke the callbacks for it.
    //
    SoPrimitiveVertex pv[2];

    pv[0].setPoint(SbVec3f(CP[0]));
    pv[1].setPoint(SbVec3f(CP[1]));
    pv[0].setNormal(normal);
    pv[1].setNormal(normal);
    pv[0].setTextureCoords(TP[0]);
    pv[1].setTextureCoords(TP[1]);

    (*primCB)(action, pv, userData);

    // Overwrite the old vertex with the current vertex.
    CP[0] = CP[1];
    TP[0] = TP[1];
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This callback routine is called when a v4f call is generated
//    by the NURBS library.
//
// Use: protected

void
_SoNurbsPrimV4CurveMap::point( float *v )

//
////////////////////////////////////////////////////////////////////////
{
    //
    // Store the incoming point and the current normal at that point
    //
    CP[ptIndex][0] = v[0]/v[3];
    CP[ptIndex][1] = v[1]/v[3];
    CP[ptIndex][2] = v[2]/v[3];
    TP[ptIndex][0] = tmpPrimTex[0];
    TP[ptIndex][1] = tmpPrimTex[1];

    //
    // If the vertex has not completed a line yet, just increment
    // the point count and return.
    //
    if (ptIndex < 1)
    {
        ++ptIndex;
        return;    
    }

    //
    // This vertex completes a line.  Generate a primitive for the
    // line and invoke the callbacks for it.
    //
    SoPrimitiveVertex pv[2];

    pv[0].setPoint(SbVec3f(CP[0]));
    pv[1].setPoint(SbVec3f(CP[1]));
    pv[0].setNormal(normal);
    pv[1].setNormal(normal);
    pv[0].setTextureCoords(TP[0]);
    pv[1].setTextureCoords(TP[1]);

    (*primCB)(action, pv, userData);

    // Overwrite the old vertex with the current vertex.
    CP[0] = CP[1];
    TP[0] = TP[1];
}



////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor for the SoPrimV3SurfaceMap class.
//
// Use: public

_SoNurbsPrimV3SurfaceMap::~_SoNurbsPrimV3SurfaceMap()

//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor for the SoPrimV4SurfaceMap class.
//
// Use: public

_SoNurbsPrimV4SurfaceMap::~_SoNurbsPrimV4SurfaceMap()

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
_SoNurbsPrimV3SurfaceMap::initialize(
    SoAction *act,
    SoNurbsPrimCB *cb,
    void *data )

//
////////////////////////////////////////////////////////////////////////
{
    cacheIndices[0] = 0;
    cacheIndices[1] = 1;
    curCacheIndex   = 0;
    curPrimIndex    = 2;
    cacheReady = FALSE;
    action   = act;
    primCB   = cb;
    userData = data;
    TP[0].setValue(0.0, 0.0, 0.0, 1.0);
    TP[1] = TP[0];
    TP[2] = TP[0];
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initialize the v4 surface map.
//
// Use: public

void
_SoNurbsPrimV4SurfaceMap::initialize(
    SoAction *act,
    SoNurbsPrimCB *cb,
    void *data )

//
////////////////////////////////////////////////////////////////////////
{
    cacheIndices[0] = 0;
    cacheIndices[1] = 1;
    curCacheIndex   = 0;
    curPrimIndex    = 2;
    cacheReady = FALSE;
    action   = act;
    primCB   = cb;
    userData = data;
    TP[0].setValue(0.0, 0.0, 0.0, 1.0);
    TP[1] = TP[0];
    TP[2] = TP[0];
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This method begins a new strip of triangles.
//
// Use: protected

void
_SoNurbsPrimV3SurfaceMap::startNewTriStrip()

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
_SoNurbsPrimV4SurfaceMap::startNewTriStrip()

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
_SoNurbsPrimV3SurfaceMap::swapVertices()

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
_SoNurbsPrimV4SurfaceMap::swapVertices()

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
_SoNurbsPrimV3SurfaceMap::point( float *v )

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
        TP[cacheIndices[curCacheIndex]][0] = tmpPrimTex[0];
        TP[cacheIndices[curCacheIndex]][1] = tmpPrimTex[1];
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
    TP[curPrimIndex][0] = tmpPrimTex[0];
    TP[curPrimIndex][1] = tmpPrimTex[1];

    // Create a detail for this triangle and invoke the callback.
    SoPrimitiveVertex pv[3];

    pv[0].setPoint(SbVec3f(SP[cacheIndices[0]].p));
    pv[1].setPoint(SbVec3f(SP[cacheIndices[1]].p));
    pv[2].setPoint(SbVec3f(SP[curPrimIndex].p));
    pv[0].setNormal(SbVec3f(SP[cacheIndices[0]].norm));
    pv[1].setNormal(SbVec3f(SP[cacheIndices[1]].norm));
    pv[2].setNormal(SbVec3f(SP[curPrimIndex].norm));
    pv[0].setTextureCoords(TP[cacheIndices[0]]);
    pv[1].setTextureCoords(TP[cacheIndices[1]]);
    pv[2].setTextureCoords(TP[curPrimIndex]);
    (*primCB)(action, pv, userData);

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
_SoNurbsPrimV4SurfaceMap::point( float *v )

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
        TP[cacheIndices[curCacheIndex]][0] = tmpPrimTex[0];
        TP[cacheIndices[curCacheIndex]][1] = tmpPrimTex[1];
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
    TP[curPrimIndex][0] = tmpPrimTex[0];
    TP[curPrimIndex][1] = tmpPrimTex[1];

    // Create a detail for this triangle and invoke the callback.
    SoPrimitiveVertex pv[3];

    pv[0].setPoint(SbVec3f(SP[cacheIndices[0]].p));
    pv[1].setPoint(SbVec3f(SP[cacheIndices[1]].p));
    pv[2].setPoint(SbVec3f(SP[curPrimIndex].p));
    pv[0].setNormal(SbVec3f(SP[cacheIndices[0]].norm));
    pv[1].setNormal(SbVec3f(SP[cacheIndices[1]].norm));
    pv[2].setNormal(SbVec3f(SP[curPrimIndex].norm));
    pv[0].setTextureCoords(TP[cacheIndices[0]]);
    pv[1].setTextureCoords(TP[cacheIndices[1]]);
    pv[2].setTextureCoords(TP[curPrimIndex]);
    (*primCB)(action, pv, userData);

    // Update the cache indices
    int tmp = curPrimIndex;
    curPrimIndex = cacheIndices[curCacheIndex];
    cacheIndices[curCacheIndex] = tmp; 
    curCacheIndex = 1 - curCacheIndex;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This routine copies the values from one SurfacePartial structure
//    to another.
//
// Use: protected

void
_SoNurbsPrimV3SurfaceMap::copyPartials(
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
_SoNurbsPrimV4SurfaceMap::copyPartials(
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

void _SoNurbsPrimTex2CurveMap::point( float *p )
{
    // Save the point to be used when the next vertex
    // comes along.
    tmpPrimTex[0] = p[0];
    tmpPrimTex[1] = p[1];
}

void _SoNurbsPrimTex2SurfaceMap::point( float *p )
{
    // Save the point to be used when the next vertex
    // comes along.
    tmpPrimTex[0] = p[0];
    tmpPrimTex[1] = p[1];
}

_SoNurbsPrimTex2CurveMap::_SoNurbsPrimTex2CurveMap() : _SoNurbsCurveMap( N_T2D, 0, 2 ) {}
_SoNurbsPrimTex2CurveMap::~_SoNurbsPrimTex2CurveMap() { }
void _SoNurbsPrimTex2CurveMap::uderiv( float * ) { }
void _SoNurbsPrimTex2CurveMap::uuderiv( float * ) { }

_SoNurbsPrimTex2SurfaceMap::~_SoNurbsPrimTex2SurfaceMap() { }
void _SoNurbsPrimTex2SurfaceMap::uderiv( float * ) { }
void _SoNurbsPrimTex2SurfaceMap::vderiv( float * ) { }
void _SoNurbsPrimTex2SurfaceMap::uuderiv( float * ) { }
void _SoNurbsPrimTex2SurfaceMap::uvderiv( float * ) { }
void _SoNurbsPrimTex2SurfaceMap::vvderiv( float * ) { }
