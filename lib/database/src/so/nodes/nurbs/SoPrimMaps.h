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
 |   $Revision: 1.1.1.1 $
 |
 |   Description:
 |	This file defines the curve and surface map classes used for
 |      receiving tessellated primitives from the nurbs library to
 |      return to the user.
 |
 |   Author(s)		: Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_NURBS_PRIM_MAPS_
#define  _SO_NURBS_PRIM_MAPS_

#include <math.h>
#include <stdlib.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SoPrimitiveVertex.h>
#include "clients/softcurvemaps.h"
#include "clients/softsurfacemaps.h"
//#include "clients/gl4types.h"

class SoAction;

// These define the callback routines for returning the primitive
// details back to the shape nodes.
typedef void        SoNurbsPrimCB( SoAction *, SoPrimitiveVertex *, void * );

class _SoNurbsPrimV3CurveMap : public _SoNurbsVertex3CurveMap
{
public:
                        _SoNurbsPrimV3CurveMap();
    virtual             ~_SoNurbsPrimV3CurveMap();
    virtual void	point( float * );
    void                initialize( SoAction *, SoNurbsPrimCB *, void * );
    void                startNewCurve() {ptIndex = 0;}

protected:
    SoAction            *action;
    SoNurbsPrimCB       *primCB;
    void                *userData;
    SbVec3f     	*CP;
    SbVec4f		*TP;   // Texture Coordinates
    SbVec3f		normal;
    int                 ptIndex;
    
};

class _SoNurbsPrimV4CurveMap : public _SoNurbsVertex4CurveMap
{
public:
                        _SoNurbsPrimV4CurveMap();
    virtual             ~_SoNurbsPrimV4CurveMap();
    virtual void	point( float * );
    void                initialize( SoAction *, SoNurbsPrimCB *, void * );
    void                startNewCurve() {ptIndex = 0;}

protected:
    SoAction            *action;
    SoNurbsPrimCB       *primCB;
    void                *userData;
    SbVec3f     	*CP;
    SbVec4f		*TP;   // Texture Coordinates
    SbVec3f		normal;
    int                 ptIndex;
};

class _SoNurbsPrimTex2CurveMap : public _SoNurbsCurveMap
{
public:
                        _SoNurbsPrimTex2CurveMap();
                        ~_SoNurbsPrimTex2CurveMap();
    virtual void        point( float * );
    virtual void        uderiv( float * );
    virtual void        uuderiv( float * );
};

class _SoNurbsPrimV3SurfaceMap : public _SoNurbsVertex3SurfaceMap
{
public:
                        _SoNurbsPrimV3SurfaceMap(){}
    virtual             ~_SoNurbsPrimV3SurfaceMap();
    virtual void	point( float *p );
    void                initialize( SoAction *, SoNurbsPrimCB *, void * );
    void                startNewTriStrip();
    void                swapVertices();

protected:
    SoAction            *action;
    SoNurbsPrimCB       *primCB;
    void                *userData;
    _SoNurbsSurfPartial	SP[3];
    SbVec4f		TP[3];   // Texture Coordinates
    int                 cacheIndices[2], curCacheIndex, curPrimIndex;
    SbBool              cacheReady;

    void                copyPartials( _SoNurbsSurfPartial &from,
                                      _SoNurbsSurfPartial &to );
};

class _SoNurbsPrimV4SurfaceMap : public _SoNurbsVertex4SurfaceMap
{
public:
                        _SoNurbsPrimV4SurfaceMap(){}
    virtual             ~_SoNurbsPrimV4SurfaceMap();
    virtual void	point( float *p );
    void                initialize( SoAction *, SoNurbsPrimCB *, void * );
    void                startNewTriStrip();
    void                swapVertices();

protected:
    SoAction            *action;
    SoNurbsPrimCB       *primCB;
    void                *userData;
    _SoNurbsSurfPartial	SP[3];
    SbVec4f		TP[3];   // Texture Coordinates
    int                 cacheIndices[2], curCacheIndex, curPrimIndex;
    SbBool              cacheReady;

    void                copyPartials( _SoNurbsSurfPartial &from,
                                      _SoNurbsSurfPartial &to );
};

class _SoNurbsPrimTex2SurfaceMap : public _SoNurbsSurfaceMap
{
public:
                        _SoNurbsPrimTex2SurfaceMap() :
                                _SoNurbsSurfaceMap( N_T2D, 0, 2 ) {}
    virtual             ~_SoNurbsPrimTex2SurfaceMap();
    virtual void        point( float *p );
    virtual void        uderiv( float * );
    virtual void        vderiv( float * );
    virtual void        uuderiv( float * );
    virtual void        uvderiv( float * );
    virtual void        vvderiv( float * );
};

#endif  /*_SO_NURBS_PRIM_MAPS_ */


